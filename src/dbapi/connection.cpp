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

#include "logbook/dbapi/connection.hpp"
#include "logbook/dbapi/dbapi_error.hpp"
#include "logbook/dbapi/statement.hpp"

using namespace logbook::dbapi;

int _authorize_handler(void * p, int evcode, const char * p1, const char * p2, const char * dbname, const char * tvname)
{
	connection::authorize_handler * h = static_cast<connection::authorize_handler *>(p);
	return (* h)(evcode, p1, p2, dbname, tvname);
}

int _busy_handler(void * p, int cnt)
{
	connection::busy_handler * h = static_cast<connection::busy_handler *>(p);
	return (* h)(cnt);
}

int _commit_handler(void * p)
{
	connection::commit_handler * h = static_cast<connection::commit_handler *>(p);
	return (* h)();
}

void _rollback_handler(void * p)
{
	connection::rollback_handler * h = static_cast<connection::rollback_handler *>(p);
	(* h)();
}

void _update_handler(void * p, int opcode, const char * dbname, const char * tvname, sqlite3_int64 rowid)
{
	connection::update_handler * h = static_cast<connection::update_handler *>(p);
	(* h)(opcode, dbname, tvname, rowid);
}

connection::connection(const char * dbname)
	: m_db(0), m_transaction(false), s_begin(0), s_commit(0), s_rollback(0)
{
	// Default to in-memory database
	if (dbname == 0)
		dbname = ":memory:";

	// Open the Database Connection
	if (sqlite3_open(dbname, & m_db) != SQLITE_OK)
		throw dbapi_error(this);

	// Prepare the Transaction Statements
	const char * _begin = "BEGIN TRANSACTION";
	const char * _commit = "COMMIT TRANSACTION";
	const char * _rollback = "ROLLBACK TRANSACTION";

	if (sqlite3_prepare_v2(m_db, _begin, strlen(_begin), & s_begin, 0) != SQLITE_OK)
		throw sql_error(this);
	if (sqlite3_prepare_v2(m_db, _commit, strlen(_commit), & s_commit, 0) != SQLITE_OK)
		throw sql_error(this);
	if (sqlite3_prepare_v2(m_db, _rollback, strlen(_rollback), & s_rollback, 0) != SQLITE_OK)
		throw sql_error(this);
}

connection::~connection()
{
	// Roll Back any current Transaction
	if (m_transaction && (s_rollback != 0))
		sqlite3_step(s_rollback);

	// Release Transaction Statements
	if (s_begin != 0)
		sqlite3_finalize(s_begin);
	if (s_commit != 0)
		sqlite3_finalize(s_commit);
	if (s_rollback != 0)
		sqlite3_finalize(s_rollback);

	s_begin = 0;
	s_commit = 0;
	s_rollback = 0;

	// Close the Database Connection
	if (m_db != 0)
	{
		sqlite3_close(m_db);
		m_db = 0;
	}
}

void connection::begin()
{
	if (sqlite3_step(s_begin) != SQLITE_DONE)
		throw sql_error(this);
	m_transaction = true;
	sqlite3_reset(s_begin);
}

void connection::commit()
{
	if (sqlite3_step(s_commit) != SQLITE_DONE)
		throw sql_error(this);
	m_transaction = false;
	sqlite3_reset(s_commit);
}

int connection::error_code()
{
	return sqlite3_errcode(m_db);
}

const char * connection::error_msg()
{
	return sqlite3_errmsg(m_db);
}

boost::shared_ptr<cursor> connection::exec_sql(const std::string & sql)
{
	statement::ptr s(new statement(shared_from_this(), sql));
	return s->exec();
}

sqlite3 * connection::handle()
{
	return m_db;
}

void connection::rollback()
{
	sqlite3_step(s_rollback);
	m_transaction = false;
	sqlite3_reset(s_rollback);
}

void connection::set_authorize_handler(authorize_handler h)
{
	m_ah = h;
	sqlite3_set_authorizer(m_db, h ? _authorize_handler : 0, & m_ah);
}

void connection::set_busy_handler(busy_handler h)
{
	m_bh = h;
	sqlite3_busy_handler(m_db, h ? _busy_handler : 0, & m_bh);
}

void connection::set_busy_timeout(int ms)
{
	m_bh = busy_handler();
	sqlite3_busy_timeout(m_db, ms);
}

void connection::set_commit_handler(commit_handler h)
{
	m_ch = h;
	sqlite3_commit_hook(m_db, h ? _commit_handler : 0, & m_ch);
}

void connection::set_rollback_handler(rollback_handler h)
{
	m_rh = h;
	sqlite3_rollback_hook(m_db, h ? _rollback_handler : 0, & m_rh);
}

void connection::set_update_handler(update_handler h)
{
	m_uh = h;
	sqlite3_update_hook(m_db, h ? _update_handler : 0, & m_uh);
}

bool connection::transaction_active() const
{
	return m_transaction;
}
