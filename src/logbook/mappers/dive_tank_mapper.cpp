/*
 * Copyright (C) 2012 Asymworks, LLC.  All Rights Reserved.
 *
 * Developed by: Asymworks, LLC <info@asymworks.com>
 * 				 http://www.asymworks.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimers in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the names of Asymworks, LLC, nor the names of its contributors
 *      may be used to endorse or promote products derived from this Software
 *      without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */

#include "dive_tank_mapper.hpp"
#include <benthos/logbook/session.hpp>

using namespace benthos::logbook;
using namespace benthos::logbook::mappers;

std::string DiveTankMapper::columns = "id, dive_id, tank_idx, tank_id, mix_id, px_start, px_end";

std::string DiveTankMapper::sql_insert = "insert into divetanks values (?1, ?2, ?3, ?4, ?5, ?6, ?7)";
std::string DiveTankMapper::sql_update = "update divetanks set dive_id=?2, tank_idx=?3, tank_id=?4, mix_id=?5, px_start=?6, px_end=?7 where id=?1";
std::string DiveTankMapper::sql_delete = "delete from divetanks where id=?1";

std::string DiveTankMapper::sql_find_all = "select " + columns + " from divetanks";
std::string DiveTankMapper::sql_find_id = "select " + columns + " from divetanks where id=?1";
std::string DiveTankMapper::sql_find_dive = "select " + columns + " from divetanks where dive_id=?1";
std::string DiveTankMapper::sql_find_tank = "select " + columns + " from divetanks where tank_id=?1";

DiveTankMapper::DiveTankMapper(boost::shared_ptr<Session> session)
	: Mapper<DiveTank>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
	m_find_dive_stmt = statement::ptr(new statement(m_conn, sql_find_dive));
	m_find_tank_stmt = statement::ptr(new statement(m_conn, sql_find_tank));
}

DiveTankMapper::~DiveTankMapper()
{
}

void DiveTankMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	bindUpdate(s, p);
}

void DiveTankMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	DiveTank::Ptr o = downcast(p);

	s->bind(2, o->dive()->id());
	s->bind(3, o->index());

	if (o->tank())
		s->bind(4, o->tank()->id());
	else
		s->bind(4, boost::none);

	if (o->mix())
		s->bind(5, o->mix()->id());
	else
		s->bind(5, boost::none);

	s->bind(6, o->start_pressure());
	s->bind(7, o->end_pressure());
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

DiveTank::Ptr DiveTankMapper::doLoad(int64_t id, cursor::row_t r) const
{
	IFinder<Dive>::Ptr dive_finder(m_session.lock()->finder<Dive>());
	IFinder<Mix>::Ptr mix_finder(m_session.lock()->finder<Mix>());
	IFinder<Tank>::Ptr tank_finder(m_session.lock()->finder<Tank>());

	DiveTank::Ptr o(new logbook::DiveTank(dive_finder->find(r[1].as<int64_t>())));

	mark_persistent_loading(o);
	set_persistent_id(o, id);
	o->setIndex(r[2].as<int>());

	if (r[3].is_null())
		o->setTank(boost::none);
	else
		o->setTank(tank_finder->find(r[3].as<int64_t>()));

	if (r[4].is_null())
		o->setMix(boost::none);
	else
		o->setMix(mix_finder->find(r[4].as<int64_t>()));

	SET_VARIANT(o, setStartPressure, r[5], double);
	SET_VARIANT(o, setEndPressure, r[6], double);

	return o;
}

std::vector<DiveTank::Ptr> DiveTankMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

DiveTank::Ptr DiveTankMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::DiveTank::Ptr();

	return load(r);
}

std::vector<DiveTank::Ptr> DiveTankMapper::findByDive(int64_t dive_id)
{
	m_find_dive_stmt->reset();
	m_find_dive_stmt->bind(1, dive_id);
	dbapi::cursor::ptr c = m_find_dive_stmt->exec();

	return loadAll(c);
}

std::vector<DiveTank::Ptr> DiveTankMapper::findByTank(int64_t tank_id)
{
	m_find_tank_stmt->reset();
	m_find_tank_stmt->bind(1, tank_id);
	dbapi::cursor::ptr c = m_find_tank_stmt->exec();

	return loadAll(c);
}

