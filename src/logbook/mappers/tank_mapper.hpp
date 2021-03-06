/*
 * Copyright (C) 2012 Asymworks, LLC.  All Rights Reserved.
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

#ifndef LOGBOOKMAPPERS_TANK_MAPPER_HPP_
#define LOGBOOKMAPPERS_TANK_MAPPER_HPP_

/**
 * @file src/logbook/mappers/tank_mapper.hpp
 * @brief Scuba Tank Data Mapper Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <benthos/logbook/dbapi.hpp>
#include <benthos/logbook/mapper.hpp>
#include <benthos/logbook/persistent.hpp>
#include <benthos/logbook/tank.hpp>

namespace benthos { namespace logbook { namespace mappers {

/**
 * @brief Scuba Tank Data Mapper
 *
 * Data Mapper which implements CRUD operations for Scuba Tank domain objects
 * within the Logbook database.
 */
class TankMapper: public Mapper<Tank>, public ITankFinder
{
public:
	typedef Mapper<Tank>::Ptr	Ptr;
	typedef Mapper<Tank>::WPtr	WPtr;

protected:
	static std::string columns;			///< List of Columns Returned by SELECT

	static std::string sql_insert;		///< INSERT Statement SQL String
	static std::string sql_update;		///< UPDATE Statement SQL String
	static std::string sql_delete;		///< DELETE Statement SQL String

	static std::string sql_find_all;	///< Find All Statement SQL String
	static std::string sql_find_id;		///< Find By Id Statement SQL String
	static std::string sql_find_name;	///< Find By Name Statement SQL String

public:

	//! Class Constructor
	TankMapper(boost::shared_ptr<Session> session);

	//! Class Destructor
	virtual ~TankMapper();

public:

	/**
	 * @brief Return a list of all Tanks
	 * @return List of Tanks
	 */
	virtual std::vector<Tank::Ptr> find();

	/**
	 * @brief Return a single Tank by its identifier
	 * @param[in] Identifier
	 * @return Tank
	 */
	virtual Tank::Ptr find(int64_t id);

	/**
	 * @brief Find a Tank by Name (case insensitive)
	 * @param[in] Tank Name
	 * @return Tank
	 */
	virtual Tank::Ptr findByName(const std::string & name);

protected:

	//! Bind an Object to the Insert Statement
	virtual void bindInsert(statement::ptr s, Persistent::Ptr o) const;

	//! Bind an Object to the Update Statement
	virtual void bindUpdate(statement::ptr s, Persistent::Ptr o) const;

	//! Load an Object from a Result Set
	virtual Tank::Ptr doLoad(int64_t id, dbapi::cursor::row_t r) const;

protected:
	dbapi::statement::ptr		m_find_all_stmt;		///< Find All Prepared Statement
	dbapi::statement::ptr		m_find_id_stmt;			///< Find By Id Prepared Statement
	dbapi::statement::ptr		m_find_name_stmt;		///< Find By Name Prepared Statement

};

} } } /* benthos::logbook::mappers */

#endif /* LOGBOOKMAPPERS_TANK_MAPPER_HPP_ */
