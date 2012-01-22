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

#ifndef LOGBOOKMAPPERS_MIX_MAPPER_HPP_
#define LOGBOOKMAPPERS_MIX_MAPPER_HPP_

/**
 * @file src/logbook/mappers/mix_mapper.hpp
 * @brief Gas Mixture Data Mapper Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <logbook/dbapi.hpp>
#include <logbook/mapper.hpp>
#include <logbook/mix.hpp>
#include <logbook/persistent.hpp>

namespace logbook { namespace mappers {

/**
 * @brief Gas Mixture Data Mapper
 *
 * Data Mapper which implements CRUD operations for Gas Mixture domain objects
 * within the Logbook database.
 */
class MixMapper: public Mapper<Mix>, public IFinder<Mix>
{
public:
	typedef Mapper<Mix>::Ptr	Ptr;
	typedef Mapper<Mix>::WPtr	WPtr;

protected:
	static std::string columns;			///< List of Columns Returned by SELECT

	static std::string sql_insert;		///< INSERT Statement SQL String
	static std::string sql_update;		///< UPDATE Statement SQL String
	static std::string sql_delete;		///< DELETE Statement SQL String

	static std::string sql_find_all;	///< Find All Statement SQL String
	static std::string sql_find_id;		///< Find By Id Statement SQL String

public:

	//! Class Constructor
	MixMapper(boost::shared_ptr<Session> session);

	//! Class Destructor
	virtual ~MixMapper();

public:

	/**
	 * @brief Return a list of all Dive Sites
	 * @return List of Dive Sites
	 */
	virtual std::vector<Mix::Ptr> find();

	/**
	 * @brief Return a single Dive Site by its identifier
	 * @param[in] Identifier
	 * @return Dive Site
	 */
	virtual Mix::Ptr find(int64_t id);

protected:

	//! Bind an Object to the Insert Statement
	virtual void bindInsert(statement::ptr s, Persistent::Ptr o) const;

	//! Bind an Object to the Update Statement
	virtual void bindUpdate(statement::ptr s, Persistent::Ptr o) const;

	//! Load an Object from a Result Set
	virtual Mix::Ptr doLoad(int64_t id, dbapi::cursor::row_t r) const;

protected:
	dbapi::statement::ptr		m_find_all_stmt;		///< Find All Prepared Statement
	dbapi::statement::ptr		m_find_id_stmt;			///< Find By Id Prepared Statement

};

} } /* logbook::mappers */

#endif /* LOGBOOKMAPPERS_MIX_MAPPER_HPP_ */
