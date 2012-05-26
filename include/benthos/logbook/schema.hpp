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

#ifndef LOGBOOK_SCHEMA_HPP_
#define LOGBOOK_SCHEMA_HPP_

/**
 * @file include/benthos/logbook/schema.hpp
 * @brief Logbook Schema Management Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <benthos/logbook/dbapi.hpp>

namespace logbook {

/**
 * @brief Logbook Database Schema Class
 */
class Schema
{
public:

	//! Class Constructor
	Schema();

	//! Class Destructor
	~Schema();

public:

	/**
	 * @brief Create Database Tables
	 * @param[in] Database Connection
	 *
	 * If the database tables are already created, this method is a no-op.  The
	 * class only checks that the tables exist, not that they have the correct
	 * schema.
	 */
	void create(dbapi::connection::ptr conn) const;

	/**
	 * @brief Drop Database Tables
	 * @param[in] Database Connection
	 */
	void drop(dbapi::connection::ptr conn) const;

	/**
	 * @brief Upgrade the Database Schema
	 * @param[in] Database Connection
	 * @param[in] Target Schema Version
	 *
	 * Upgrades the database schema to the given version.  If the version is
	 * -1, the database will be upgraded to the latest schema version.
	 */
	void upgrade(dbapi::connection::ptr conn, int version = -1) const;

	/**
	 * @brief Downgrade the Database Schema
	 * @param[in] Database Connection
	 * @param[in] Target Schema Version
	 *
	 * Downgrades the database schema to the given version.  The version must
	 * be greater than 1 and less than the current database schema.
	 */
	void downgrade(dbapi::connection::ptr conn, int version) const;

private:
	void create_computers_tbl(dbapi::connection::ptr conn) const;
	void create_dives_tbl(dbapi::connection::ptr conn) const;
	void create_divetags_tbl(dbapi::connection::ptr conn) const;
	void create_mixes_tbl(dbapi::connection::ptr conn) const;
	void create_profiles_tbl(dbapi::connection::ptr conn) const;
	void create_sites_tbl(dbapi::connection::ptr conn) const;
	void create_version_tbl(dbapi::connection::ptr conn) const;

};

} /* logbook */

#endif /* LOGBOOK_SCHEMA_HPP_ */
