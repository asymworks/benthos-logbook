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

#include "dive_site_mapper.hpp"

using namespace logbook;
using namespace logbook::mappers;

std::string DiveSiteMapper::columns = "id, name, place, country, latitude, "
		"longitude, platform, waterbody, bottom, salinity, timezone, comments";

std::string DiveSiteMapper::sql_insert = "insert into sites values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12)";
std::string DiveSiteMapper::sql_update = "update sites set name=?2, place=?3, country=?4, latitude=?5, longitude=?6, "
		"platform=?7, waterbody=?8, bottom=?9, salinity=?10, timezone=?11, comments=?12 where id=?1";
std::string DiveSiteMapper::sql_delete = "delete from sites where id=?1";

std::string DiveSiteMapper::sql_find_all = "select " + columns + " from sites";
std::string DiveSiteMapper::sql_find_id = "select " + columns + " from sites where id=?1";

DiveSiteMapper::DiveSiteMapper(boost::shared_ptr<Session> session)
	: Mapper<DiveSite>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
}

DiveSiteMapper::~DiveSiteMapper()
{
}

void DiveSiteMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	DiveSite::Ptr o = downcast(p);

	s->bind(2, o->name());
	s->bind(3, o->place());

	// Country type is special case
	if (! o->country_().is_initialized())
		s->bind(4, boost::none);
	else
		s->bind(4, o->country_()->code());

	s->bind(5, o->latitude());
	s->bind(6, o->longitude());
	s->bind(7, o->platform());
	s->bind(8, o->water_body());
	s->bind(9, o->bottom());
	s->bind(10, o->salinity());
	s->bind(11, o->timezone());
	s->bind(12, o->comments());
}

void DiveSiteMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	DiveSite::Ptr o = downcast(p);

	s->bind(2, o->name());
	s->bind(3, o->place());

	// Country type is special case
	if (! o->country_().is_initialized())
		s->bind(4, boost::none);
	else
		s->bind(4, o->country_()->code());

	s->bind(5, o->latitude());
	s->bind(6, o->longitude());
	s->bind(7, o->platform());
	s->bind(8, o->water_body());
	s->bind(9, o->bottom());
	s->bind(10, o->salinity());
	s->bind(11, o->timezone());
	s->bind(12, o->comments());
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

DiveSite::Ptr DiveSiteMapper::doLoad(int64_t id, cursor::row_t r) const
{
	DiveSite::Ptr o(new logbook::DiveSite);

	set_persistent_id(o, id);
	o->setName(r[1].as<std::string>());

	SET_VARIANT(o, setPlace, r[2], std::string);

	// Country type is special case
	if (r[3].is_null())
		o->setCountry(boost::none);
	else
		o->setCountry(country(r[3].as<std::string>()));

	SET_VARIANT(o, setLatitude, r[4], double);
	SET_VARIANT(o, setLongitude, r[5], double);
	SET_VARIANT(o, setPlatform, r[6], std::string);
	SET_VARIANT(o, setWaterBody, r[7], std::string);
	SET_VARIANT(o, setBottom, r[8], std::string);
	SET_VARIANT(o, setSalinity, r[9], std::string);
	SET_VARIANT(o, setTimezone, r[10], std::string);
	SET_VARIANT(o, setComments, r[11], std::string);

	return o;
}

std::vector<DiveSite::Ptr> DiveSiteMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

DiveSite::Ptr DiveSiteMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::DiveSite::Ptr();

	return load(r);
}
