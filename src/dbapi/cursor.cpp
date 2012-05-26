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

#include "benthos/logbook/dbapi/cursor.hpp"
#include "benthos/logbook/dbapi/dbapi_error.hpp"
#include "benthos/logbook/dbapi/statement.hpp"

#include <stdexcept>

using namespace logbook::dbapi;

cursor::cursor(statement::ptr stmt, bool empty)
	: m_stmt(stmt), m_done(empty), m_names(), m_tblnames(), m_orgnames()
{
	m_ncols = stmt->num_columns();

	// Load the Column Names
	for (int i = 0; i < m_ncols; i++)
	{
		const char * cn = sqlite3_column_name(stmt->handle(), i);
		const char * tn = sqlite3_column_table_name(stmt->handle(), i);
		const char * on = sqlite3_column_origin_name(stmt->handle(), i);

		m_names.push_back(std::string(cn == NULL ? "" : cn));
		m_tblnames.push_back(std::string(tn == NULL ? "" : tn));
		m_orgnames.push_back(std::string(on == NULL ? "" : on));
	}
}

cursor::~cursor()
{
}

int cursor::column_count() const
{
	return m_ncols;
}

const std::vector<std::string> & cursor::column_names() const
{
	return m_names;
}

const std::vector<std::string> & cursor::column_table_names() const
{
	return m_tblnames;
}

const std::vector<std::string> & cursor::column_origin_names() const
{
	return m_orgnames;
}

cursor::row_t cursor::fetchone()
{
	// Fetch the Current Row
	row_t result;
	if (m_done)
		return result;

	load_row(result);

	// Advance to the Next Row
	m_done = ! m_stmt.lock()->step();
	return result;
}

std::vector<cursor::row_t> cursor::fetchmany(size_t num)
{
	size_t n = 0;
	std::vector<cursor::row_t> result;
	if (m_done)
		return result;

	while (! m_done && (n++ < num))
	{
		row_t row;
		load_row(row);
		result.push_back(row);

		m_done = ! m_stmt.lock()->step();
	}

	return result;
}

std::vector<cursor::row_t> cursor::fetchall()
{
	std::vector<cursor::row_t> result;
	if (m_done)
		return result;

	while (! m_done)
	{
		row_t row;
		load_row(row);
		result.push_back(row);

		m_done = ! m_stmt.lock()->step();
	}

	return result;
}

int64_t cursor::last_rowid()
{
	statement::ptr st = m_stmt.lock();
	return sqlite3_last_insert_rowid(st->m_conn->handle());
}

variant cursor::load_column(int idx)
{
	statement::ptr st = m_stmt.lock();
	if ((idx < 0) || (idx >= st->num_columns()))
		throw std::out_of_range(std::string("Column ") + boost::lexical_cast<std::string>(idx) + " is out of range");
	int type = sqlite3_column_type(st->handle(), idx);

	switch (type)
	{
	case SQLITE_INTEGER:
	{
		int64_t i = sqlite3_column_int64(st->handle(), idx);
		if ((i < INT_MIN) || (i > INT_MAX))
			return variant(i);
		return variant((int)i);
	}

	case SQLITE_FLOAT:
		return variant(sqlite3_column_double(st->handle(), idx));

	case SQLITE_TEXT:
	{
		std::string s((const char *)sqlite3_column_text(st->handle(), idx));
		return variant(s);
	}

	case SQLITE_BLOB:
	{
		int len = sqlite3_column_bytes(st->handle(), idx);
		const void * data = sqlite3_column_blob(st->handle(), idx);
		std::vector<unsigned char> blob(len, * (const unsigned char *)(data));
		return variant(blob);
	}

	case SQLITE_NULL:
		return variant();

	default:
		throw dbapi_error(st->m_conn);

	}
}

void cursor::load_row(row_t & row)
{
	row.clear();
	for (int i = 0; i < column_count(); i++)
		row.push_back(load_column(i));
}

size_t cursor::rowcount()
{
	statement::ptr st = m_stmt.lock();
	return sqlite3_changes(st->m_conn->handle());
}
