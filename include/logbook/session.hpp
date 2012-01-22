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
 * @file include/logbook/session.hpp
 * @brief Database Session Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <map>
#include <set>
#include <typeinfo>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <logbook/dbapi.hpp>
#include <logbook/mapper.hpp>
#include <logbook/persistent.hpp>

using namespace logbook::dbapi;

namespace logbook {

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

//! @brief Pair of <typeinfo, Persistent::Ptr>
typedef std::pair<const_typeinfo_ptr, Persistent::Ptr> uow_entry;

//! @brief Comparator for Unit of Work Entries
struct uow_entry_cmp
{
	bool operator()(const uow_entry & lhs, const uow_entry & rhs) const
	{
		if (lhs.first == rhs.first)
		{
			// Sort on Persistent::Ptr
			return lhs.second < rhs.second;
		}

		// Sort on Type Info
		return lhs.first->before(* rhs.first) != 0;
	}
};

//! @brief Unit of Work Entry List
typedef std::set<uow_entry, uow_entry_cmp> uow_registry;

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

	//! Class Constructor
	Session(connection::ptr conn);

	//! Class Destructor
	virtual ~Session();

public:

	/**
	 * @brief Commit Changes to the Database
	 *
	 * Commits all current changes to the database, including inserting new
	 * objects, removing deleted objects and updating changed objects.  The
	 * operation runs within a single transaction and so all operations will
	 * either succeed or fail together.
	 */
	void commit();

	//! @return Database Connection Pointer
	inline connection::ptr conn() const { return m_conn; }

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

	/**
	 * @brief Register a Deleted Object
	 * @param[in] Deleted Object Pointer
	 *
	 * Registers the Domain Object as deleted with the unit of work, so that
	 * it will be deleted the next time Session::commit() is called.
	 */
	void registerDeleted(Persistent::Ptr p);

	/**
	 * @brief Register a Dirty Object
	 * @param[in] Dirty Object Pointer
	 *
	 * Registers the Domain Object as dirty with the unit of work, so that
	 * it will be updated the next time Session::commit() is called.
	 */
	void registerDirty(Persistent::Ptr p);

	/**
	 * @brief Register a New Object
	 * @param[in] New Object Pointer
	 *
	 * Registers the Domain Object as a new object with the unit of work, so
	 * that it will be inserted the next time Session::commit() is called.
	 */
	void registerNew(Persistent::Ptr p);

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
		return boost::shared_dynamic_cast<IFinder<D> >(p);
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
		return boost::shared_dynamic_cast<Mapper<D> >(p);
	}

protected:

	//! Delete Removed Objects from the Database
	void deleteRemoved();

	//! Insert New Objects into the Database
	void insertNew();

	//! Update Dirty Objects in the Database
	void updateDirty();

private:
	connection::ptr		m_conn;		///< Database Connection
	mapper_registry		m_mappers;	///< Registry of Data Mappers

	uow_registry		m_new;		///< Registry of New Objects
	uow_registry		m_dirty;	///< Registry of Dirty Objects
	uow_registry		m_deleted;	///< Registry of Deleted Objects

};

} /* logbook */

#endif /* LOGBOOK_SESSION_HPP_ */
