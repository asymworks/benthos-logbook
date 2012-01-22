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

#ifndef LOGBOOKMAPPERS_DIVE_COMPUTER_MAPPER_HPP_
#define LOGBOOKMAPPERS_DIVE_COMPUTER_MAPPER_HPP_

/**
 * @file src/logbook/mappers/dive_computer_mapper.hpp
 * @brief Dive Computer Data Mapper Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <logbook/dbapi.hpp>
#include <logbook/dive_computer.hpp>
#include <logbook/mapper.hpp>
#include <logbook/persistent.hpp>

namespace logbook { namespace mappers {

/**
 * @brief Dive Computer Data Mapper
 *
 * Data Mapper which implements CRUD operations for Dive Computer domain objects
 * within the Logbook database.
 */
class DiveComputerMapper: public Mapper<DiveComputer>, public IDiveComputerFinder
{
public:
	typedef Mapper<DiveComputer>::Ptr	Ptr;
	typedef Mapper<DiveComputer>::WPtr	WPtr;

protected:
	static std::string columns;			///< List of Columns Returned by SELECT

	static std::string sql_insert;		///< INSERT Statement SQL String
	static std::string sql_update;		///< UPDATE Statement SQL String
	static std::string sql_delete;		///< DELETE Statement SQL String

	static std::string sql_find_all;	///< Find All Statement SQL String
	static std::string sql_find_id;		///< Find By Id Statement SQL String
	static std::string sql_find_serno;	///< Find By Serial Statement SQL String

public:

	//! Class Constructor
	DiveComputerMapper(boost::shared_ptr<Session> session);

	//! Class Destructor
	virtual ~DiveComputerMapper();

public:

	/**
	 * @brief Return a list of all Dive Computers
	 * @return List of Dive Computers
	 */
	virtual std::vector<DiveComputer::Ptr> find();

	/**
	 * @brief Return a single Dive Computer by its identifier
	 * @param[in] Identifier
	 * @return Dive Computer
	 */
	virtual DiveComputer::Ptr find(int64_t id);

	/**
	 * @brief Find a Dive Computer by Driver/Serial Number
	 * @param[in] Driver Name
	 * @param[in] Serial Number
	 * @return Dive Computer
	 */
	virtual DiveComputer::Ptr findBySerial(const std::string & driver, const std::string & serial);

protected:

	//! Bind an Object to the Insert Statement
	virtual void bindInsert(statement::ptr s, Persistent::Ptr o) const;

	//! Bind an Object to the Update Statement
	virtual void bindUpdate(statement::ptr s, Persistent::Ptr o) const;

	//! Load an Object from a Result Set
	virtual DiveComputer::Ptr doLoad(int64_t id, dbapi::cursor::row_t r) const;

protected:
	dbapi::statement::ptr		m_find_all_stmt;		///< Find All Prepared Statement
	dbapi::statement::ptr		m_find_id_stmt;			///< Find By Id Prepared Statement
	dbapi::statement::ptr		m_find_serno_stmt;		///< Find By Serial Number Prepared Statement

};

} } /* logbook::mappers */

#endif /* LOGBOOKMAPPERS_DIVE_COMPUTER_MAPPER_HPP_ */
