/*
 * Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
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

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/profile.hpp>

#include "dive_computer_mapper.hpp"

using namespace logbook;
using namespace logbook::mappers;

std::string DiveComputerMapper::columns = "id, driver, serial, parser, token, "
		"last_transfer, driver_args, parser_args, name, manufacturer, model, "
		"hw_version, sw_version";

std::string DiveComputerMapper::sql_insert = "insert into computers values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13)";
std::string DiveComputerMapper::sql_update = "update computers set driver=?2, serial=?3, parser=?4, token=?5, last_transfer=?6, driver_args=?7, "
		"parser_args=?8, name=?9, manufacturer=?10, model=?11, hw_version=?12, sw_version=?13 where id=?1";
std::string DiveComputerMapper::sql_delete = "delete from computers where id=?1";

std::string DiveComputerMapper::sql_find_all = "select " + columns + " from computers";
std::string DiveComputerMapper::sql_find_id = "select " + columns + " from computers where id=?1";
std::string DiveComputerMapper::sql_find_serno = "select " + columns + " from computers where driver=?1 and serial=?2";

DiveComputerMapper::DiveComputerMapper(boost::shared_ptr<Session> session)
	: Mapper<DiveComputer>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
	m_find_serno_stmt = statement::ptr(new statement(m_conn, sql_find_serno));
}

DiveComputerMapper::~DiveComputerMapper()
{
}

void DiveComputerMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	bindUpdate(s, p);
}

void DiveComputerMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	DiveComputer::Ptr o = downcast(p);

	s->bind(2, o->driver());
	s->bind(3, o->serial());
	s->bind(4, o->parser());
	s->bind(5, o->token());
	s->bind(6, o->last_transfer());
	s->bind(7, o->driver_args());
	s->bind(8, o->parser_args());
	s->bind(9, o->name());
	s->bind(10, o->manufacturer());
	s->bind(11, o->model());
	s->bind(12, o->hw_version());
	s->bind(13, o->sw_version());
}

std::list<Persistent::Ptr> DiveComputerMapper::cascade_add(Persistent::Ptr p)
{
	std::list<Persistent::Ptr> result;
	DiveComputer::Ptr o = downcast(p);

	if (! o)
		return result;

	std::list<Profile::Ptr> profiles = o->profiles()->all();
	result.insert(result.end(), profiles.begin(), profiles.end());

	return result;
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

DiveComputer::Ptr DiveComputerMapper::doLoad(int64_t id, cursor::row_t r) const
{
	DiveComputer::Ptr o(new logbook::DiveComputer);

	mark_persistent_loading(o);

	set_persistent_id(o, id);
	o->setDriver(r[1].as<std::string>());
	o->setSerial(r[2].as<std::string>());

	SET_VARIANT(o, setParser, r[3], std::string);
	SET_VARIANT(o, setToken, r[4], std::string);
	SET_VARIANT(o, setLastTransfer, r[5], time_t);
	SET_VARIANT(o, setDriverArgs, r[6], std::string);
	SET_VARIANT(o, setParserArgs, r[7], std::string);
	SET_VARIANT(o, setName, r[8], std::string);
	SET_VARIANT(o, setManufacturer, r[9], std::string);
	SET_VARIANT(o, setModel, r[10], std::string);
	SET_VARIANT(o, setHWVersion, r[11], std::string);
	SET_VARIANT(o, setSWVersion, r[12], std::string);

	return o;
}

std::vector<DiveComputer::Ptr> DiveComputerMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

DiveComputer::Ptr DiveComputerMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::DiveComputer::Ptr();

	return load(r);
}

DiveComputer::Ptr DiveComputerMapper::findBySerial(const std::string & driver, const std::string & serial)
{
	m_find_serno_stmt->reset();
	m_find_serno_stmt->bind(1, driver);
	m_find_serno_stmt->bind(2, serial);

	dbapi::cursor::ptr c = m_find_serno_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::DiveComputer::Ptr();

	return load(r);
}
