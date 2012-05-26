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

#ifndef DBAPI_CONNECTION_HPP_
#define DBAPI_CONNECTION_HPP_

/**
 * @file include/benthos/logbook/dbapi/connection.hpp
 * @brief DBAPI Connection Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <sqlite3.h>

namespace logbook { namespace dbapi {

class cursor;

/**
 * @brief Database Connection Class
 *
 * Represents a connection to an SQLite database, either in-memory or a file
 * in the filesystem.  Uses the RAII pattern so there is no specific connect()
 * or disconnect() methods; instead the constructor and destructor act as the
 * connect and disconnect operations and object lifetime corresponds exactly
 * to connection lifetime.
 */
class connection: public boost::noncopyable,
	public boost::enable_shared_from_this<connection>
{
public:
	typedef boost::shared_ptr<connection>	ptr;

public:

	/**
	 * @brief Database Busy Handler Function
	 *
	 * The Busy Handler, if registered with the connection, may be called
	 * whenever an attempt is made to open a database table that another thread
	 * or process has locked.  The argument to the function is the number of
	 * times the busy handler has been invoked for this locking event.
	 *
	 * If the busy callback returns 0, then no additional attempts are made to
	 * access the database and SQLITE_BUSY or SQLITE_IOERR_BLOCKED is returned.
	 * If the callback returns non-zero, then another attempt is made to open
	 * the database for reading and the cycle repeats.
	 */
	typedef boost::function<int (int)> busy_handler;

	/**
	 * @brief Database Commit Hook Handler Function
	 *
	 * The Commit Handler, if registered with the connection, is invoked
	 * whenever a transaction is committed.  The callback implementation must
	 * not do anything that will modify the database connection that invoked
	 * the callback. Any actions to modify the database connection must be
	 * deferred until after the completion of the sqlite3_step() call that
	 * triggered the commit or rollback hook in the first place. Note that
	 * sqlite3_prepare_v2() and sqlite3_step() both modify their database
	 * connections for the meaning of "modify" in this paragraph.
	 *
	 * When the commit hook callback routine returns zero, the COMMIT operation
	 * is allowed to continue normally. If the commit hook returns non-zero,
	 * then the COMMIT is converted into a ROLLBACK. The rollback hook is
	 * invoked on a rollback that results from a commit hook returning non-
	 * zero, just as it would be with any other rollback.
	 */
	typedef boost::function<int ()> commit_handler;

	/**
	 * @brief Database Rollback Hook Handler Function
	 *
	 * The Rollback Handler, if registered with the connection, is invoked
	 * whenever a transaction is rolled back.
	 */
	typedef boost::function<void ()> rollback_handler;

	/**
	 * @brief Database Update Hook Handler Function
	 *
	 * The Update Handler, if registered with the connection, is invoked
	 * whenever a row is updated, inserted or deleted.  The first callback
	 * argument is one of SQLITE_INSERT, SQLITE_DELETE, or SQLITE_UPDATE,
	 * depending on the operation that caused the callback to be invoked.
	 * The second and third arguments to the callback contain pointers to
	 * the database and table name containing the affected row. The final
	 * callback parameter is the rowid of the row. In the case of an update,
	 * this is the rowid after the update takes place.
	 */
	typedef boost::function<void (int, char const*, char const*, sqlite3_int64)> update_handler;

	/**
	 * @brief Database Authorization Hook Handler Function
	 *
	 * The Authorization Handler, if registered with the connection, is invoked
	 * as SQL statements are being compiled by dbapi::statement. At various
	 * points during the compilation process, as logic is being created to
	 * perform various actions, the authorizer callback is invoked to see if
	 * those actions are allowed. The authorizer callback should return
	 * SQLITE_OK to allow the action, SQLITE_IGNORE to disallow the specific
	 * action but allow the SQL statement to continue to be compiled, or
	 * SQLITE_DENY to cause the entire SQL statement to be rejected with an
	 * error. If the authorizer callback returns any value other than
	 * SQLITE_IGNORE, SQLITE_OK, or SQLITE_DENY then the call that triggered
	 * the authorizer will fail with an error message.
	 *
	 * The second parameter to the callback is an integer action code that
	 * specifies the particular action to be authorized. The third through
	 * sixth parameters to the callback are zero-terminated strings that
	 * contain additional details about the action to be authorized.  See
	 * the SQLite3 documentation for sqlite3_set_authorizer() for more details.
	 */
	typedef boost::function<int (int, char const*, char const*, char const*, char const*)> authorize_handler;

public:

	/**
	 * @brief Class Constructor
	 * @param[in] Database Name or NULL for in-memory (temporary) database
	 * @throws sqlitekit::db_error
	 *
	 * Opens a new SQLite3 connection.  The optional dbname parameter can be
	 * either a path to a SQLite3 database file or NULL.  If the dbname is NULL
	 * or ":memory:", a temporary SQLite3 database is constructed in local
	 * memory.  The in-memory database will be freed when the connection object
	 * is destroyed.
	 *
	 * If the database cannot be opened, a db_error exception will be thrown.
	 */
	connection(const char * dbname = 0);

	/**
	 * @brief Class Destructor
	 *
	 * Closes the SQLite3 connection and frees memory allocated for an in-
	 * memory database (if any).
	 */
	~connection();

	//! @brief Begin a Transaction
	void begin();

	//! @brief Commit the current Transaction
	void commit();

	//! @return SQLite3 Error Code
	int error_code();

	//! @return SQLite3 Error Message
	const char * error_msg();

	/**
	 * @brief Execute an SQL string and return result
	 * @param[in] SQL String
	 * @return Cursor
	 *
	 * Creates a prepared statement for the given SQL string and executes it,
	 * returing the cursor result.  This is shorthand for creating the statement
	 * and calling statement::exec().
	 */
	boost::shared_ptr<cursor> exec_sql(const std::string & sql);

	//! @return SQLite3 Database Handle
	sqlite3 * handle();

	//! @brief Roll Back the current Transaction
	void rollback();

	/**
	 * @brief Set the Authorization Handler
	 * @param[in] Handler Function
	 * @see authorize_handler
	 *
	 * Sets a new Authorization handler for the connection.
	 */
	void set_authorize_handler(authorize_handler h);

	/**
	 * @brief Set the Database Busy Handler
	 * @param[in] Handler Function
	 * @see busy_handler
	 *
	 * Sets a new Database Busy handler for the connection.
	 */
	void set_busy_handler(busy_handler h);

	/**
	 * @brief Set the Database Busy Timeout
	 * @param[in] Timeout Value in milliseconds
	 *
	 * Sets a timeout-based Database Busy handler, which will replace any busy
	 * handler registered with set_busy_handler().  If the database is locked
	 * and the specified timeout expires, the statement::step() operation will
	 * raise a timeout error.
	 */
	void set_busy_timeout(int ms);

	/**
	 * @brief Set the Commit Handler
	 * @param[in] Handler Function
	 * @see commit_handler
	 *
	 * Sets a new Commit handler for the connection.
	 */
	void set_commit_handler(commit_handler h);

	/**
	 * @brief Set the Rollback Handler
	 * @param[in] Handler Function
	 * @see rollback_handler
	 *
	 * Sets a new Rollback handler for the connection.
	 */
	void set_rollback_handler(rollback_handler h);

	/**
	 * @brief Set the Update Handler
	 * @param[in] Handler Function
	 * @see update_handler
	 *
	 * Sets a new Update handler for the connection.
	 */
	void set_update_handler(update_handler h);

	//! @return Check if a Transaction is Active
	bool transaction_active() const;

private:
	sqlite3 *			m_db;			///< SQLite3 Database Handle

	busy_handler		m_bh;			///< Database Busy Handler
	commit_handler		m_ch;			///< Commit Handler
	rollback_handler	m_rh;			///< Rollback Handler
	update_handler		m_uh;			///< Update Handler
	authorize_handler	m_ah;			///< Authorize Handler

	bool				m_transaction;	///< Transaction Active
	sqlite3_stmt *		s_begin;		///< Begin Transaction Statement
	sqlite3_stmt *		s_commit;		///< Commit Transaction Statement
	sqlite3_stmt *		s_rollback;		///< Rollback Transaction Statement

};

} } /* logbook::dbapi */

#endif /* DBAPI_CONNECTION_HPP_ */
