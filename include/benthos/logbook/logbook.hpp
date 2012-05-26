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

#ifndef LOGBOOK_LOGBOOK_HPP_
#define LOGBOOK_LOGBOOK_HPP_

/**
 * @file include/logbook/logbook.hpp
 * @brief Logbook Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <benthos/logbook/config.hpp>
#include <benthos/logbook/dbapi.hpp>
#include <benthos/logbook/logging.hpp>

#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/dive_site.hpp>
#include <benthos/logbook/mix.hpp>
#include <benthos/logbook/persistent.hpp>
#include <benthos/logbook/schema.hpp>
#include <benthos/logbook/session.hpp>

namespace logbook {

/**
 * @brief Logbook Class
 *
 * Main class for the Scuba Logbook library.  The Logbook class encapsuates
 * access to the Logbook database and includes helper methods for most common
 * operations such as querying for, adding and removing items.
 */
class Logbook: public boost::noncopyable
{
public:
	typedef boost::shared_ptr<Logbook>	Ptr;

protected:

	/**
	 * @brief Class Constructor
	 * @param[in] Logbook File Name
	 * @param[in] Database Connection
	 */
	Logbook(const std::string & filename, dbapi::connection::ptr conn);

public:

	//! Class Destructor
	virtual ~Logbook();

	/**
	 * @brief Open an existing Logbook file
	 * @param[in] File Name
	 * @return Logbook Instance
	 *
	 * Opens the Logbook with the given file name.  If the file name does not
	 * exist, or is not a valid Logbook, an exception will be raised.  If the
	 * Logbook file uses an out-of-date schema, a schema_error will be raised.
	 *
	 * To create a new Logbook file, use the static Logbook::Create() method.
	 *
	 * To upgrade a Logbook file's schema, use the static Logbook::Upgrade()
	 * method.
	 */
	static Logbook::Ptr Open(const std::string & filename);

	/**
	 * @brief Create a new Logbook file
	 * @param[in] File Name
	 * @param[in] Creator Name
	 * @param[in] Creator Version
	 * @return Logbook Instance
	 *
	 * Create a new, empty Logbook file and open it for use.  If the file exists
	 * an exception will be raised.  The newly-created Logbook will have the
	 * latest schema defined by the Schema class.
	 *
	 * The database creator name and version will be set if the creator and
	 * version parameters are non-default.
	 */
	static Logbook::Ptr Create(const std::string & filename,
			const std::string & creator = std::string(), int version = 0);

	/**
	 * @brief Upgrade a Logbook file
	 * @param[in] File Name
	 *
	 * Upgrade the schema of the Logbook file to the latest version.  If the
	 * file does not exist, or is not a valid Logbook, an exception will be
	 * raised.  If the Logbook already has the latest schema, this is a no-op.
	 *
	 * The Logbook library version and creator name will be updated with the
	 * values defined in this library.
	 */
	static void Upgrade(const std::string & filename);

public:

	//! @return Database Connection
	inline dbapi::connection::ptr connection() const { return m_conn; }

	//! @return Logbook File Name
	inline const std::string & filename() const { return m_filename; }

	//! @return Database Session
	inline Session::Ptr session() const { return m_session; }

private:
	std::string					m_filename;	///< Logbook File Name
	dbapi::connection::ptr		m_conn;		///< Database Connection
	Session::Ptr				m_session;	///< Database Session

};

} /* logbook */

#endif /* LOGBOOK_LOGBOOK_HPP_ */
