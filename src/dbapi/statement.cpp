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

#include <boost/algorithm/string.hpp>
#include <boost/variant.hpp>

#include "benthos/logbook/dbapi/cursor.hpp"
#include "benthos/logbook/dbapi/statement.hpp"
#include "benthos/logbook/dbapi/dbapi_error.hpp"

using namespace benthos::logbook::dbapi;

enum
{
	TYPE_SELECT		= 0,
	TYPE_INSERT		= 1,
	TYPE_UPDATE		= 2,
	TYPE_DELETE		= 3,
	TYPE_REPLACE	= 4,
	TYPE_OTHER		= 5,
};

statement::statement(connection::ptr conn, const std::string & sql)
	: m_conn(conn), m_stmt(0), m_sql(), m_tail(), m_type(TYPE_OTHER)
{
	const char * _tail = 0;
	if (sqlite3_prepare_v2(m_conn->handle(), sql.c_str(), sql.size(), & m_stmt, & _tail) != SQLITE_OK)
		throw sql_error(conn);

	if (_tail)
		m_tail.assign(_tail);

	m_sql.assign(sql.begin(), sql.end() - m_tail.size());

	// Cache useful parameters so the getters can be const
	m_ncolumns = sqlite3_column_count(m_stmt);
	m_nparams = sqlite3_bind_parameter_count(m_stmt);
	m_readonly = sqlite3_stmt_readonly(m_stmt);

	// Derive the Statement Type
	std::string s = boost::trim_copy(sql);
	size_t idx = s.find_first_of(" \t\n\r");
	if (idx != std::string::npos)
		s.erase(idx, s.size() - idx);

	if (strcasecmp(s.c_str(), "select") == 0)
		m_type = TYPE_SELECT;
	else if (strcasecmp(s.c_str(), "insert") == 0)
		m_type = TYPE_INSERT;
	else if (strcasecmp(s.c_str(), "update") == 0)
		m_type = TYPE_UPDATE;
	else if (strcasecmp(s.c_str(), "delete") == 0)
		m_type = TYPE_DELETE;
	else if (strcasecmp(s.c_str(), "replace") == 0)
		m_type = TYPE_REPLACE;
}

statement::~statement()
{
	if (m_stmt)
		sqlite3_finalize(m_stmt);
}

void statement::bind(int idx, int value)
{
	bind<int>(idx, value);
}

void statement::bind(int idx, int64_t value)
{
	bind<int64_t>(idx, value);
}

void statement::bind(int idx, double value)
{
	bind<double>(idx, value);
}

void statement::bind(int idx, const std::string & value)
{
	bind<std::string>(idx, value);
}

void statement::bind(int idx, const char * value)
{
	bind<std::string>(idx, std::string(value));
}

void statement::bind(int idx, const std::vector<unsigned char> & value)
{
	bind<std::vector<unsigned char> >(idx, value);
}

void statement::bind(int idx, const variant & value)
{
	if (value.is_null())
	{
		bind(idx);
	}
	else
	{
		int rc = boost::apply_visitor(binder(m_stmt, idx), value.value());
		if (rc != SQLITE_OK)
			throw bind_error(m_conn);
	}
}

void statement::bind(int idx, const boost::none_t &)
{
	binder(m_stmt, idx)();
}

void statement::bind(int idx)
{
	binder(m_stmt, idx)();
}

void statement::bind(const std::string & name, int value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, int64_t value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, double value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, const std::string & value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, const char * value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, const std::vector<unsigned char> & value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, const variant & value)
{
	bind(find_index(name), value);
}

void statement::bind(const std::string & name, const boost::none_t &)
{
	bind(find_index(name));
}

void statement::bind(const std::string & name)
{
	bind(find_index(name));
}

void statement::check_index(int index) const
{
	if ((index <= 0) || (index > m_nparams))
		throw bind_error(std::string("Invalid Parameter Index: ") + boost::lexical_cast<std::string>(index));
}

cursor::ptr statement::exec()
{
	return cursor::ptr(new cursor(this->shared_from_this(), ! step()));
}

variant statement::exec_scalar()
{
	cursor::ptr c = exec();
	variant r = c->load_column(0);
	reset();
	return r;
}

int statement::find_index(const std::string & name)
{
	int idx = param_index(name);
	if (! idx)
		throw bind_error("Invalid Parameter Name: " + name);
	return idx;
}

sqlite3_stmt * statement::handle() const
{
	return m_stmt;
}

bool statement::is_delete() const
{
	return m_type == TYPE_DELETE;
}

bool statement::is_insert() const
{
	return m_type == TYPE_INSERT;
}

bool statement::is_readonly() const
{
	return m_readonly;
}

bool statement::is_replace() const
{
	return m_type == TYPE_REPLACE;
}

bool statement::is_select() const
{
	return m_type == TYPE_SELECT;
}

bool statement::is_update() const
{
	return m_type == TYPE_UPDATE;
}

int statement::num_columns() const
{
	return m_ncolumns;
}

int statement::num_params() const
{
	return m_nparams;
}

int statement::param_index(const std::string & name)
{
	return sqlite3_bind_parameter_index(m_stmt, name.c_str());
}

std::string statement::param_name(int index)
{
	const char * c = sqlite3_bind_parameter_name(m_stmt, index);
	return c == NULL ? std::string() : std::string(c);
}

void statement::reset()
{
	sqlite3_reset(m_stmt);
}

const std::string & statement::sql() const
{
	return m_sql;
}

bool statement::step()
{
	int rc = sqlite3_step(m_stmt);
	if (rc == SQLITE_ROW)
		return true;
	if (rc == SQLITE_DONE)
		return false;

	throw sql_error(m_conn);
}

const std::string & statement::tail() const
{
	return m_tail;
}
