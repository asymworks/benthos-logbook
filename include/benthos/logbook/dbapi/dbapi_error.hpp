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

#ifndef DBAPI_ERROR_HPP_
#define DBAPI_ERROR_HPP_

/**
 * @file include/benthos/logbook/dbapi/dbapi_error.hpp
 * @brief DBAPI Error Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <exception>
#include <stdexcept>
#include <string>

#include <boost/shared_ptr.hpp>

namespace logbook { namespace dbapi {

// Forward Definition of dbapi::connection
class connection;

// Forward Definition of dbapi::connection::ptr
typedef boost::shared_ptr<connection> connection_ptr;

/**
 * @brief DBAPI Base Exception Class
 */
class dbapi_error: public std::runtime_error
{
public:

	/**
	 * @brief Class Constructor
	 * @param[in] Error Message
	 */
	dbapi_error(const std::string & msg);

	/**
	 * @brief Class Constructor
	 * @param[in] Database Connection
	 */
	dbapi_error(dbapi::connection * db);

	/**
	 * @brief Class Constructor
	 * @param[in] Database Connection Pointer
	 */
	dbapi_error(connection_ptr db);

	//! @brief Class Destructor
	virtual ~dbapi_error() throw ();

};

/**
 * @brief SQL Error Class
 *
 * Represents errors related to SQL statement preparation (syntax errors, etc).
 */
class sql_error: public dbapi_error
{
public:
	sql_error(const std::string & msg) : dbapi_error(msg) { }
	sql_error(dbapi::connection * db) : dbapi_error(db) { }
	sql_error(connection_ptr db) : dbapi_error(db) { }
	virtual ~sql_error() throw () { }
};

/**
 * @brief Bind Error Class
 *
 * Represents errors related to SQL statement binding.
 */
class bind_error: public dbapi_error
{
public:
	bind_error(const std::string & msg) : dbapi_error(msg) { }
	bind_error(dbapi::connection * db) : dbapi_error(db) { }
	bind_error(connection_ptr db) : dbapi_error(db) { }
	virtual ~bind_error() throw () { }
};

} } /* logbook::dbapi */

#endif /* DBAPI_ERROR_HPP_ */
