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

#include "tank_mapper.hpp"

using namespace benthos::logbook;
using namespace benthos::logbook::mappers;

std::string TankMapper::columns = "id, name, type, pressure, volume";

std::string TankMapper::sql_insert = "insert into tanks values (?1, ?2, ?3, ?4, ?5)";
std::string TankMapper::sql_update = "update tanks set name=?2, type=?3, pressure=?4, volume=?5 where id=?1";
std::string TankMapper::sql_delete = "delete from tanks where id=?1";

std::string TankMapper::sql_find_all = "select " + columns + " from tanks";
std::string TankMapper::sql_find_id = "select " + columns + " from tanks where id=?1";
std::string TankMapper::sql_find_name = "select " + columns + " from tanks where upper(name) = upper(?1)";

TankMapper::TankMapper(boost::shared_ptr<Session> session)
	: Mapper<Tank>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
	m_find_name_stmt = statement::ptr(new statement(m_conn, sql_find_name));
}

TankMapper::~TankMapper()
{
}

void TankMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	bindUpdate(s, p);
}

void TankMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	Tank::Ptr o = downcast(p);

	s->bind(2, o->name());
	s->bind(3, o->type());
	s->bind(4, o->pressure());
	s->bind(5, o->volume());
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

Tank::Ptr TankMapper::doLoad(int64_t id, cursor::row_t r) const
{
	Tank::Ptr o(new logbook::Tank);

	mark_persistent_loading(o);

	set_persistent_id(o, id);
	SET_VARIANT(o, setName, r[1], std::string);
	SET_VARIANT(o, setType, r[2], std::string);

	o->setPressure(r[3].as<double>());
	o->setVolume(r[4].as<double>());

	return o;
}

std::vector<Tank::Ptr> TankMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

Tank::Ptr TankMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Tank::Ptr();

	return load(r);
}

Tank::Ptr TankMapper::findByName(const std::string & name)
{
	m_find_name_stmt->reset();
	m_find_name_stmt->bind(1, name);

	dbapi::cursor::ptr c = m_find_name_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Tank::Ptr();

	return load(r);
}
