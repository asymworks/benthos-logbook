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

#include "mix_mapper.hpp"

using namespace logbook;
using namespace logbook::mappers;

std::string MixMapper::columns = "id, name, o2, he, h2, ar";

std::string MixMapper::sql_insert = "insert into mixes values (?1, ?2, ?3, ?4, ?5, ?6)";
std::string MixMapper::sql_update = "update mixes set name=?2, o2=?3, he=?4, h2=?5, ar=?6 where id=?1";
std::string MixMapper::sql_delete = "delete from mixes where id=?1";

std::string MixMapper::sql_find_all = "select " + columns + " from mixes";
std::string MixMapper::sql_find_id = "select " + columns + " from mixes where id=?1";
std::string MixMapper::sql_find_name = "select " + columns + " from mixes where upper(name) = upper(?1)";
std::string MixMapper::sql_find_mix = "select " + columns + " from mixes where abs(o2 - ?1) < 5 and abs(he - ?2) < 5 order by o2 desc limit 1";

MixMapper::MixMapper(boost::shared_ptr<Session> session)
	: Mapper<Mix>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
	m_find_name_stmt = statement::ptr(new statement(m_conn, sql_find_name));
	m_find_mix_stmt = statement::ptr(new statement(m_conn, sql_find_mix));
}

MixMapper::~MixMapper()
{
}

void MixMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	Mix::Ptr o = downcast(p);

	s->bind(2, o->name());
	s->bind(3, (int)o->o2_permil());
	s->bind(4, (int)o->he_permil());
	s->bind(5, (int)o->h2_permil());
	s->bind(6, (int)o->ar_permil());
}

void MixMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	Mix::Ptr o = downcast(p);

	s->bind(2, o->name());
	s->bind(3, (int)o->o2_permil());
	s->bind(4, (int)o->he_permil());
	s->bind(5, (int)o->h2_permil());
	s->bind(6, (int)o->ar_permil());
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

Mix::Ptr MixMapper::doLoad(int64_t id, cursor::row_t r) const
{
	Mix::Ptr o(new logbook::Mix);

	mark_persistent_loading(o);

	set_persistent_id(o, id);
	SET_VARIANT(o, setName, r[1], std::string);

	o->setO2PerMil(r[2].as<int>());
	o->setHePerMil(r[3].as<int>());
	o->setH2PerMil(r[4].as<int>());
	o->setArPerMil(r[5].as<int>());

	return o;
}

std::vector<Mix::Ptr> MixMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

Mix::Ptr MixMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Mix::Ptr();

	return load(r);
}

Mix::Ptr MixMapper::findByName(const std::string & name)
{
	m_find_name_stmt->reset();
	m_find_name_stmt->bind(1, name);

	dbapi::cursor::ptr c = m_find_name_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Mix::Ptr();

	return load(r);
}

Mix::Ptr MixMapper::findByMix(unsigned int pmO2, unsigned int pmHe)
{
	m_find_mix_stmt->reset();
	m_find_mix_stmt->bind(1, (int)pmO2);
	m_find_mix_stmt->bind(2, (int)pmHe);

	dbapi::cursor::ptr c = m_find_mix_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Mix::Ptr();

	return load(r);
}
