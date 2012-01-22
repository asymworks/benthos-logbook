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

#ifndef DBAPI_STATEMENT_HPP_
#define DBAPI_STATEMENT_HPP_

/**
 * @file include/logbook/dbapi/connection.hpp
 * @brief DBAPI Prepared Statement Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>
#include <string>

#include <boost/enable_shared_from_this.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <logbook/dbapi/connection.hpp>
#include <logbook/dbapi/variant.hpp>

#include <sqlite3.h>

namespace logbook { namespace dbapi {

// Forward Definition of cursor
class cursor;

/**
 * @brief Prepared Statement Class
 *
 * Encapsulates an SQLite3 prepared statement.  This is a lightweight wrapper
 * around the underlying SQLite3 API.  The class uses the RAII pattern, so the
 * SQL statement is prepared in the constructor and is released in the class
 * destructor.  The statement::bind() functions are used to bind parameters
 * either by name or by column index.  Any parameter convention supported by
 * SQLite3 is supported by this class:
 * - ?
 * - ?NNN
 * - :VVV
 * - @VVV
 * - $VVV
 * where NNN is an integer and VVV is a text name.  No checking is done by the
 * class directly; however, errors in statement::bind() will be propagated as
 * sqlitekit exceptions.
 *
 * Prepared statements are executed by calling the statement::exec() function,
 * which returns a cursor object linked to the statement which is used to
 * retrieve results of the SQL statement.  To retrieve a single scalar value,
 * the statement::exec_scalar() function may be used as a shortcut.  It will
 * return the value in the first column of the first row of the result set.
 */
class statement: public boost::noncopyable,
	public boost::enable_shared_from_this<statement>
{
public:
	typedef boost::shared_ptr<statement>	ptr;

public:

	/**
	 * @brief Class Constructor
	 * @param[in] Database Connection
	 * @param[in] SQL Statement
	 * @throws sql_error
	 *
	 * Prepares a statement for the given SQL statement.  If an error is found
	 * while preparing the statement, an sql_error exception will be thrown.
	 */
	statement(connection::ptr conn, const std::string & sql);

	//! @brief Class Destructor
	virtual ~statement();

	/**@{
	 * @brief Bind a Parameter by Parameter Name
	 * @param[in] Parameter Name
	 * @param[in] Bind Value
	 * @throws bind_error
	 *
	 * Binds a value to the named parameter.  If the statement was not prepared
	 * using named parameters, or if the named parameter does not exist, this
	 * method will raise a bind_error exception.
	 */
	template <typename T>
	void bind(const std::string & name, const T & value);

	template <typename T>
	void bind(const std::string & name, const boost::optional<T> & value);

	void bind(const std::string & name, int value);
	void bind(const std::string & name, int64_t value);
	void bind(const std::string & name, double value);
	void bind(const std::string & name, const std::string & value);
	void bind(const std::string & name, const char * value);
	void bind(const std::string & name, const std::vector<unsigned char> & value);
	void bind(const std::string & name, const variant & value);
	void bind(const std::string & name, const boost::none_t &);
	void bind(const std::string & name);
	/*@}*/

	/**@{
	 * @brief Bind a Parameter by Parameter Index
	 * @param[in] Parameter Index
	 * @param[in] Bind Value
	 * @throws bind_error
	 *
	 * Binds a value to the indexed parameter.  If the parameter index is out
	 * of range, does not exist, this method will raise a bind_error exception.
	 */
	template <typename T>
	void bind(int idx, const T & value);

	template <typename T>
	void bind(int idx, const boost::optional<T> & value);

	void bind(int idx, int value);
	void bind(int idx, int64_t value);
	void bind(int idx, double value);
	void bind(int idx, const std::string & value);
	void bind(int idx, const char * value);
	void bind(int idx, const std::vector<unsigned char> & value);
	void bind(int idx, const variant & value);
	void bind(int idx, const boost::none_t &);
	void bind(int idx);
	/*@}*/

	/**
	 * @brief Execute the Prepared Statement
	 * @return Database Cursor
	 *
	 * Executes the prepared statement and returns a cursor object which is
	 * used to iterate through the returned rows, if any.
	 */
	boost::shared_ptr<cursor> exec();

	/**
	 * @brief Execute the Prepared Statement and return a Scalar
	 * @return Scalar Value
	 *
	 * Executes the prepared statement and returns the first column of the
	 * first row in the result set.  The return value will be empty if the
	 * statement returned no rows or if the result is NULL.
	 *
	 * Note that if a blob is returned, it will be returned as a cached_blob
	 * and will remain in memory until the boost::any value is destroyed.
	 */
	variant exec_scalar();

	/**
	 * @brief Get the Parameter Index for a Parameter Name
	 * @param[in] Parameter Name
	 * @return Parameter Index
	 * @throws bind_error
	 *
	 * Finds the index for a named parameter.  Throws bind_error if there is no
	 * parameter with the specified name.
	 */
	int find_index(const std::string & name);

	//! @return Statement Handle
	sqlite3_stmt * handle() const;

	//! @return True if the Statement is a delete statement
	bool is_delete() const;

	//! @return True if the Statement is an insert statement
	bool is_insert() const;

	//! @return True if the Statement makes no direct change to the database
	bool is_readonly() const;

	//! @return True if the Statement is a replace statement
	bool is_replace() const;

	//! @return True if the Statement is a select statement
	bool is_select() const;

	//! @return True if the Statement is an update statement
	bool is_update() const;

	//! @return Number of Columns in the Result Set
	int num_columns() const;

	//! @return Number of Parameters
	int num_params() const;

	/**
	 * @brief Get the Parameter Index for a Parameter Name
	 * @param[in] Parameter Name
	 * @return Parameter Index
	 *
	 * Finds the index for a named parameter.  Returns 0 if the named parameter
	 * is not contained in the query.
	 */
	int param_index(const std::string & name);

	/**
	 * @brief Get the Parameter Name for a Parameter Index
	 * @param[in] Parameter Index
	 * @return Parameter Name
	 *
	 * Finds the index for a named parameter.  Returns an empty string if the
	 * index is out of range or if the parameter is nameless.
	 */
	std::string param_name(int index);

	//! @brief Reset the Bind Parameters
	void reset();

	//! @return Statement SQL
	const std::string & sql() const;

	/**
	 * @brief Advance the Result Set to the next Result
	 * @return True if the Result has another Row
	 * @throws sql_error
	 *
	 * Calls sqlite3_step on the statement.  If SQLITE_ROW is returned, the
	 * return value is true; if SQLITE_DONE is returned, the return value is
	 * false.  If an error occurs, a sql_error is raised.
	 */
	bool step();

	//! @return Statement Tail
	const std::string & tail() const;

protected:

	/**
	 * @brief Check a Parameter Index for Validity
	 * @param[in] Parameter Index
	 * @throws bind_error
	 *
	 * Checks that the given parameter is in range for this statement and throws
	 * an error if not.
	 */
	void check_index(int index) const;

private:
	friend class cursor;

	connection::ptr				m_conn;
	sqlite3_stmt * 				m_stmt;

	std::string					m_sql;
	std::string					m_tail;

	int							m_ncolumns;
	int 						m_nparams;
	bool						m_readonly;
	int							m_type;

};

} } /* logbook::dbapi */

#include "statement_impl.hpp"

#endif /* DBAPI_STATEMENT_HPP_ */
