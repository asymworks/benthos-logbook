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

#ifndef DBAPI_STATEMENT_IMPL_HPP_
#define DBAPI_STATEMENT_IMPL_HPP_

#include "dbapi_error.hpp"

using namespace logbook::dbapi;

/**
 * @brief Parameter Bind Helper Class
 */
struct binder: public boost::static_visitor<int>
{
	mutable sqlite3_stmt * 	stmt;
	int						colidx;

	binder(sqlite3_stmt * s, int idx)
		: stmt(s), colidx(idx)
	{
	}

	// BLOB binder
	int operator() (const std::vector<unsigned char> & v) const
	{
		return sqlite3_bind_blob(stmt, colidx, & v[0], v.size(), SQLITE_TRANSIENT);
	}

	// FLOAT binder
	int operator() (const double & v) const
	{
		return sqlite3_bind_double(stmt, colidx, v);
	}

	// INTEGER binder
	int operator() (const int & v) const
	{
		return sqlite3_bind_int(stmt, colidx, v);
	}

	// INT64 binder
	int operator() (const int64_t & v) const
	{
		return sqlite3_bind_int64(stmt, colidx, v);
	}

	// NULL binder
	int operator() () const
	{
		return sqlite3_bind_null(stmt, colidx);
	}

	// TEXT binder
	int operator() (const std::string & v) const
	{
		return sqlite3_bind_text(stmt, colidx, v.c_str(), v.size(), SQLITE_TRANSIENT);
	}
};

template <typename T>
void statement::bind(int idx, const T & value)
{
	check_index(idx);
	sqlite_value v(value);
	int rc = boost::apply_visitor(binder(m_stmt, idx), v);
	if (rc != SQLITE_OK)
		throw bind_error(m_conn);
}

template <typename T>
void statement::bind(int idx, const boost::optional<T> & value)
{
	check_index(idx);
	if (! value.is_initialized())
		bind(idx);
	else
		bind<T>(idx, value.get());
}

template <typename T>
void statement::bind(const std::string & name, const T & value)
{
	bind<T>(find_index(name), value);
}

template <typename T>
void statement::bind(const std::string & name, const boost::optional<T> & value)
{
	bind<boost::optional<T> >(find_index(name), value);
}

#endif /* DBAPI_STATEMENT_IMPL_HPP_ */
