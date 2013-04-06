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

#ifndef LOGBOOK_SESSION_HPP_
#define LOGBOOK_SESSION_HPP_

/**
 * @file include/benthos/logbook/session.hpp
 * @brief Database Session Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <list>
#include <map>
#include <set>
#include <typeinfo>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/utility.hpp>

#include <benthos/logbook/dbapi.hpp>
#include <benthos/logbook/logging.hpp>
#include <benthos/logbook/mapper.hpp>
#include <benthos/logbook/persistent.hpp>

using namespace benthos::logbook::dbapi;

namespace benthos { namespace logbook {

//! @brief Const Type Info Pointer (for use as std::map key)
typedef const std::type_info * const_typeinfo_ptr;

//! @brief Comparator for const_typeinfo_ptr values
struct typecmp
{
	bool operator()(const const_typeinfo_ptr & lhs, const const_typeinfo_ptr & rhs) const
	{
		return lhs->before(* rhs) != 0;
	}
};

//! @brief Mapper Registry Type
typedef std::map<const_typeinfo_ptr, AbstractMapper::Ptr, typecmp> mapper_registry;

//! @brief Unit of Work Entry List
typedef std::set<Persistent::Ptr> uow_registry;

//! @brief Identity Map Key Type
typedef std::pair<const_typeinfo_ptr, int64_t>	id_key_type;

//! @brief Type Comparator for the Identity Map Key
struct id_key_cmp
{
	bool operator()(const id_key_type & lhs, const id_key_type & rhs) const
	{
		return (lhs.first->before(* rhs.first) || (lhs.second < rhs.second));
	}
};

//! @brief Identity Map Type
typedef std::map<id_key_type, Persistent::WeakPtr, id_key_cmp> identity_map;

/**
 * @brief Database Session Class
 *
 * The Session class acts as both a mapper registry and the unit of work for the
 * Logbook database.
 */
class Session: public boost::noncopyable,
	public boost::enable_shared_from_this<Session>,
	public Persistent::Access
{
public:
	typedef boost::shared_ptr<Session>	Ptr;
	typedef boost::weak_ptr<Session>	WPtr;

public:

	//! Session Event Structure
	typedef struct
	{
		boost::signals2::signal<void (Ptr, Persistent::Ptr)>	after_attach;
		boost::signals2::signal<void (Ptr, Persistent::Ptr)>	before_detach;

	} Events;

protected:

	//! Class Constructor
	Session(connection::ptr conn);

public:

	//! Class Factory Method
	static Session::Ptr Create(connection::ptr conn);

	//! Class Destructor
	virtual ~Session();

public:

	/**
	 * @brief Add an Instance to the Session
	 * @param[in] Domain Object Pointer
	 *
	 * Registers the domain object with the session so that it will be added
	 * or updated the next time flush() or commit() is called.  The opposite of
	 * add() is expunge().
	 */
	void add(Persistent::Ptr p);

	/**
	 * @brief Begin a new Transaction
	 *
	 * Begins a new transaction.  If a transaction is already pending, this call
	 * will throw an exception.
	 */
	void begin();

	/**
	 * @brief Commit the current Transaction
	 *
	 * Commits the current transaction.  If no transaction is pending this call
	 * is a no-op.
	 */
	void commit();

	//! @return Database Connection Pointer
	inline connection::ptr conn() const { return m_conn; }

	/**
	 * @brief Mark and Instance as Deleted
	 * @param[in] Domain Object Pointer
	 *
	 * Registers the domain object as deleted.  The delete operation will be
	 * executed upon flush() or commit().
	 */
	void delete_(Persistent::Ptr p);

	//! @return Session Event Signals
	Events & events();

	/**
	 * @brief Expunge an Object from the Session
	 * @param[in] Domain Object Pointer
	 *
	 * Unregisters an object with this Session.  All internal references to the
	 * object will be removed and the object will no longer be associated with
	 * the Session.  Related objects will also be expunged according to cascade
	 * rules.
	 */
	void expunge(Persistent::Ptr p);

	/**
	 * @brief Flush Changes to the Database
	 *
	 * Flushes unsaved changes to the database, including inserting new objects,
	 * removing deleted objects and updating changed objects.  This method will
	 * start a new transaction if one is not active but will not commit.
	 */
	void flush();

	/**
	 * @brief Roll Back the current Transaction
	 *
	 * Rolls back the current transaction.  If no transaction is pending, this
	 * call is a no-op.
	 */
	void rollback();

public:

	//! @return List of Deleted Instances
	uow_registry deleted() const;

	//! @return List of Dirty Instances
	uow_registry dirty() const;

	//! @return List of New Instances
	uow_registry new_() const;

public:

	/**
	 * @brief Register a Data Mapper
	 * @param[in] Mapper Object Pointer
	 *
	 * Registers a Mapper Object for the templated domain model class.  If there
	 * is already a mapper registered, the new mapper will replace the old one.
	 *
	 * Note that this function will take ownership of the pointer and will free
	 * the memory when it is no longer needed.  The recommended invocation of
	 * this method is
	 *
	 * @code
	 * session_ptr->registerMapper<D>(new DMapper(session_ptr));
	 * @endcode
	 */
	template <typename D>
	void registerMapper(Mapper<D> * m)
	{
		typename Mapper<D>::Ptr mptr(m);
		const_typeinfo_ptr ti = & typeid(D);
		m_mappers[ti] = mptr;
	}

public:

	/**
	 * @brief Get a Finder for a Domain Model Class
	 * @return Finder Pointer
	 */
	template <typename D>
	typename IFinder<D>::Ptr finder()
	{
		const_typeinfo_ptr ti = & typeid(D);
		AbstractMapper::Ptr p(m_mappers[ti]);
		return boost::dynamic_pointer_cast<IFinder<D> >(p);
	}

	/**
	 * @brief Get a Data Mapper for a Domain Model Class
	 * @return Data Mapper Pointer
	 */
	template <typename D>
	typename Mapper<D>::Ptr mapper()
	{
		const_typeinfo_ptr ti = & typeid(D);
		AbstractMapper::Ptr p(m_mappers[ti]);
		return boost::dynamic_pointer_cast<Mapper<D> >(p);
	}

	//! @return Data Mapper
	AbstractMapper::Ptr mapper(const_typeinfo_ptr ti)
	{
		return m_mappers[ti];
	}

protected:

	//! Attach an Object to this Session
	void attach(Persistent::Ptr p);

	//! Get a list of Cascaded Objects for an Add
	std::list<Persistent::Ptr> cascade_add(Persistent::Ptr p);

	//! Get a list of Cascaded Objects for an Delete
	std::list<Persistent::Ptr> cascade_delete(Persistent::Ptr p);

	//! Get a list of Cascaded Objects for an Detach
	std::list<Persistent::Ptr> cascade_detach(Persistent::Ptr p);

	//! Detach an Object from this Session
	void detach(Persistent::Ptr p);

	//! Finalize Flush Changes
	void finalize_flush(std::list<Persistent::Ptr> & objects);

	//! Prune the Identity Map
	void prune();

	//! Register an Object as Persistent with this Session (New or for Updates)
	void register_(Persistent::Ptr p);

	//! Register a newly loaded Object with the Session (Called by AbstractMapper)
	void register_loaded(Persistent::Ptr p);

	//! Register a new Object with the Session
	void register_new(Persistent::Ptr p);

	//! Register a Persisted Object with the Session
	void register_update(Persistent::Ptr p);

	//! Run the Flush Operation
	void run_flush(std::list<Persistent::Ptr> & objects);

protected:

	//! Delete Removed Objects from the Database
	void deleteRemoved();

	//! Insert New Objects into the Database
	void insertNew();

	//! Update Dirty Objects in the Database
	void updateDirty();

	/**
	 * @brief Sort a Unit of Work Registry
	 * @param[in] Unit of Work Registry
	 * @return Ordered list of Persistents
	 *
	 * Performs a topological sort of the unit of work registry to ensure that
	 * database operations are carried out in the proper order such that object
	 * relations are consistent.
	 *
	 * In particular, this method moves objects of type Mix, DiveSite and
	 * DiveComputer to the beginning so that their id's are assigned first in
	 * an insert operation and are correctly picked up when owning objects
	 * such as Dive and Profile are inserted.
	 */
	std::list<Persistent::Ptr> sort(const uow_registry & registry) const;

private:
	connection::ptr		m_conn;			///< Database Connection
	mapper_registry		m_mappers;		///< Registry of Data Mappers
	logging::logger *	m_logger;		///< Logger Instance

	uow_registry		m_new;			///< Registry of New Objects
	uow_registry		m_deleted;		///< Registry of Deleted Objects
	identity_map		m_idmap;		///< Registry of Persistent Objects

	statement::ptr		m_beginsp;		///< Begin Savepoint Statement
	statement::ptr		m_releasesp;	///< Release Savepoint Statement
	statement::ptr		m_rollbacksp;	///< Rollback Savepoint Statement

	Events				m_events;		///< Session Event Signals

private:
	friend class AbstractMapper;

};

} } /* benthos::logbook */

#endif /* LOGBOOK_SESSION_HPP_ */
