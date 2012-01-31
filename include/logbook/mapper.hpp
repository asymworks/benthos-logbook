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

#ifndef LOGBOOK_MAPPER_HPP_
#define LOGBOOK_MAPPER_HPP_

/**
 * @file include/logbook/mapper.hpp
 * @brief Persistent Object Mapper Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <map>

#include <logbook/dbapi.hpp>
#include <logbook/persistent.hpp>

using namespace logbook::dbapi;

namespace logbook {

// Forward Definition of Session class
class Session;

typedef boost::shared_ptr<Session>	SessionPtr;
typedef boost::weak_ptr<Session>	SessionWPtr;

/**
 * @brief Abstract Mapper Class
 *
 * Base class for all Object Mappers which access Persistent objects.
 */
class AbstractMapper: public Persistent::Access
{
public:
	typedef boost::shared_ptr<AbstractMapper>	Ptr;
	typedef boost::weak_ptr<AbstractMapper>		WPtr;

public:

	//! Class Constructor
	AbstractMapper(boost::shared_ptr<Session> session);

	//! Class Destructor
	virtual ~AbstractMapper();

public:

	/**
	 * @brief Insert an Object into the Database
	 * @param[in] Domain Object
	 * @return Inserted RowId
	 * @throws dbapi_error on Database Error
	 */
	int64_t insert(Persistent::Ptr o);

	/**
	 * @brief Remove an Object from the Database
	 * @param[in] Domain Object
	 * @throws dbapi_error on Database Error
	 */
	void remove(Persistent::Ptr o);

	/**
	 * @brief Update an Object in the Database
	 * @param[in] Domain Object
	 * @throws dbapi_error on Database Error
	 */
	void update(Persistent::Ptr o);

protected:

	//! Perform Operations after Deleting a Persistent
	virtual void afterDelete(Persistent::Ptr o, int64_t oldId);

	//! Perform Operations before Deleting a Persistent
	virtual void beforeDelete(Persistent::Ptr o);

	//! Perform Operations after Inserting a Persistent
	virtual void afterInsert(Persistent::Ptr o);

	//! Perform Operations before Inserting a Persistent
	virtual void beforeInsert(Persistent::Ptr o);

	//! Perform Operations after Updating a Persistent
	virtual void afterUpdate(Persistent::Ptr o);

	//! Perform Operations before Updating a Persistent
	virtual void beforeUpdate(Persistent::Ptr o);

	//! Perform Operations after Loading a Persistent
	virtual void afterLoaded(Persistent::Ptr o);

protected:

	//! Bind an Object to the Insert Statement
	virtual void bindInsert(statement::ptr s, Persistent::Ptr o) const = 0;

	//! Bind an Object to the Update Statement
	virtual void bindUpdate(statement::ptr s, Persistent::Ptr o) const = 0;

	//! @return Insert Prepared Statement
	virtual statement::ptr insertStatement() const = 0;

	//! @return Remove Prepared Statement
	virtual statement::ptr removeStatement() const = 0;

	//! @return Update Prepared Statement
	virtual statement::ptr updateStatement() const = 0;

protected:
	boost::weak_ptr<Session>			m_session;	///< Database Session
	connection::ptr						m_conn;		///< Database Connection
	std::map<int64_t, Persistent::Ptr>	m_loaded;	///< Identity Map of Loaded Objects

};

/**
 * @brief Typed Mapper Class
 *
 * Templated Object Mapper class which operates on a particular class.  This
 * class should be subclassed for each domain model class which must be mapped.
 */
template <class D>
class Mapper: public AbstractMapper
{
public:
	typedef boost::shared_ptr<Mapper<D> >	Ptr;
	typedef boost::weak_ptr<Mapper<D> >		WPtr;

public:

	//! Class Constructor
	Mapper(boost::shared_ptr<Session> session)
		: AbstractMapper(session)
	{
	}

	//! Class Destructor
	virtual ~Mapper()
	{
	}

public:

	/**
	 * @brief Insert an Object into the Database
	 * @param[in] Domain Object
	 * @return Inserted RowId
	 */
	int64_t insert(typename D::Ptr o)
	{
		return AbstractMapper::insert(upcast(o));
	}

	/**
	 * @brief Remove an Object from the Database
	 * @param[in] Domain Object
	 * @throws dbapi_error on Database Error
	 */
	void remove(typename D::Ptr o)
	{
		return AbstractMapper::remove(upcast(o));
	}

	/**
	 * @brief Update an Object in the Database
	 * @param[in] Domain Object
	 * @throws dbapi_error on Database Error
	 */
	void update(typename D::Ptr o)
	{
		return AbstractMapper::update(upcast(o));
	}

protected:

	/**
	 * @brief Load a single Object from a Result Set
	 * @param[in] Result Set
	 * @return New Domain Object
	 */
	typename D::Ptr load(cursor::row_t r)
	{
		int64_t id = r[0].as<int64_t>();
		if (m_loaded.find(id) != m_loaded.end())
			return downcast(m_loaded[id]);

		typename D::Ptr result = doLoad(id, r);
		set_persistent_session(result, m_session.lock());
		m_loaded[id] = upcast(result);
		afterLoaded(m_loaded[id]);
		return result;
	}

	/**
	 * @brief Load multiple Objects from a Result Set
	 * @param[in] Cursor Pointer
	 * @return New Domain Object
	 */
	std::vector<typename D::Ptr> loadAll(cursor::ptr c)
	{
		std::vector<typename D::Ptr> result;

		std::vector<cursor::row_t> rs = c->fetchall();
		std::vector<cursor::row_t>::const_iterator it;
		for (it = rs.begin(); it != rs.end(); it++)
			result.push_back(load(* it));

		return result;
	}

	//! Load an Object from a Result Set
	virtual typename D::Ptr doLoad(int64_t id, cursor::row_t r) const = 0;

protected:

	//! Upcast to Persistent from Domain Model
	Persistent::Ptr upcast(typename D::Ptr o) const
	{
		return boost::shared_polymorphic_cast<Persistent>(o);
	}

	//! Downcast to Domain Model from Persistent
	typename D::Ptr downcast(Persistent::Ptr o) const
	{
		return boost::shared_polymorphic_downcast<D>(o);
	}

protected:
	virtual dbapi::statement::ptr insertStatement() const { return m_insert_stmt; }
	virtual dbapi::statement::ptr updateStatement() const { return m_update_stmt; }
	virtual dbapi::statement::ptr removeStatement() const { return m_delete_stmt; }

protected:
	dbapi::statement::ptr		m_insert_stmt;	///< INSERT Prepared Statement
	dbapi::statement::ptr		m_update_stmt;	///< UPDATE Prepared Statement
	dbapi::statement::ptr		m_delete_stmt;	///< DELETE Prepared Statement

};

} /* logbook */

#endif /* MAPPER_HPP_ */
