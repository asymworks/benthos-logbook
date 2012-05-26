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

#ifndef DBAPI_CURSOR_HPP_
#define DBAPI_CURSOR_HPP_

/**
 * @file include/benthos/logbook/dbapi/cursor.hpp
 * @brief DBAPI Cursor Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <vector>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/dbapi/statement.hpp>
#include <benthos/logbook/dbapi/variant.hpp>

namespace logbook { namespace dbapi {

/**
 * @brief Database Cursor Class
 *
 * Presets the results of a query as an iterable set of rows.  Cursors may
 * not be created directly; instead they must be accessed by executing a
 * prepared statement via statement::exec().  Data may be accessed by using the
 * cursor::fetchone(), cursor::fetchmany() and cursor::fetchall() methods.
 *
 * The cursor implements boost::noncopyable and is accessed only as a shared
 * pointer returned by the statement::exec() function.
 */
class cursor: public boost::noncopyable
{
public:
	typedef boost::shared_ptr<cursor>	ptr;
	typedef std::vector<variant>		row_t;

protected:

	/**
	 * @brief Class Constructor
	 * @param[in] Statement
	 * @param[in] Empty Result
	 */
	cursor(boost::shared_ptr<statement> stmt, bool empty);

public:

	//! Class Destructor
	~cursor();

	//! @return Number of Columns in the Result Set
	int column_count() const;

	//! @return Names of the Columns
	const std::vector<std::string> & column_names() const;

	//! @return Names of the Columns
	const std::vector<std::string> & column_table_names() const;

	//! @return Unaliased Names of the Columns
	const std::vector<std::string> & column_origin_names() const;

	/**
	 * @brief Fetch the next Row in the Cursor
	 * @return Row
	 *
	 * Fetches the next row in the cursor and increments the cursor pointer.
	 * If the cursor is already at the end the returned vector will be empty.
	 *
	 * BLOBs returned by this method are cached in memory and so will be valid
	 * as long as the row_t object's lifetime.  This can use quite a bit of
	 * memory so use care when using this method on databases with a lot of
	 * BLOB data.  Note that the BLOB will be returned within the boost::any as
	 * boost::shared_ptr<blob>.
	 */
	row_t fetchone();

	/**
	 * @brief Fetch a number of Rows from the Cursor
	 * @param[in] Number of Rows to Fetch
	 * @return Vector of Rows
	 *
	 * Fetches up to the requested number of rows in the cursor, equivalent
	 * to calling fetchone() repeatedly.  If the requested number of rows is
	 * greater than the number of rows remaining in the cursor, the returned
	 * vector will be shorter than requested and the cursor will be set to the
	 * end of the results.
	 *
	 * BLOBs returned by this method are cached in memory and so will be valid
	 * as long as the row_t object's lifetime.  This can use quite a bit of
	 * memory so use care when using this method on databases with a lot of
	 * BLOB data.  Note that the BLOB will be returned within the boost::any as
	 * boost::shared_ptr<blob>.
	 */
	std::vector<row_t> fetchmany(size_t num);

	/**
	 * @brief Fetch all Rows in the Cursor
	 * @return Vector of Rows
	 *
	 * Fetches all rows in the cursor, equivalent to calling fetchone()
	 * repeatedly.  After executing this function the cursor will be at the
	 * end and subsequent calls to fetchone() will return empty vectors.
	 *
	 * BLOBs returned by this method are cached in memory and so will be valid
	 * as long as the row_t object's lifetime.  This can use quite a bit of
	 * memory so use care when using this method on databases with a lot of
	 * BLOB data.  Note that the BLOB will be returned within the boost::any as
	 * boost::shared_ptr<blob>.
	 */
	std::vector<row_t> fetchall();

	/**
	 * @brief Return the last inserted rowid
	 * @return Row Id
	 *
	 * Returns the rowid of the most recently inserted row.  This has no
	 * effect on update, delete, or select queries.
	 */
	int64_t last_rowid();

	/**
	 * @brief Return the number of Updated/Inserted/Deleted Rows
	 * @return Number of Changed Rows
	 *
	 * Returns the number of rows affected by the most recent insert, update,
	 * or delete operation.  This has no effect for select queries.
	 */
	size_t rowcount();

private:

	//! Load a Column of the Result
	variant load_column(int idx);

	//! Load a Row of the Result
	void load_row(row_t & row);

private:
	friend class statement;

	boost::weak_ptr<statement>		m_stmt;
	bool							m_done;

	int								m_ncols;
	std::vector<std::string>		m_names;
	std::vector<std::string>		m_tblnames;
	std::vector<std::string>		m_orgnames;

};

} } /* logbook::dbapi */

#endif /* DBAPI_CURSOR_HPP_ */
