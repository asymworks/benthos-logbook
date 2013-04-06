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

#ifndef LOGBOOK_PROXY_OBJECT_HPP_
#define LOGBOOK_PROXY_OBJECT_HPP_

/**
 * @file include/benthos/logbook/proxy_object.hpp
 * @brief Persistent Proxy Object Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/utility.hpp>

#include <benthos/logbook/persistent.hpp>
#include <benthos/logbook/mapper.hpp>
#include <benthos/logbook/session.hpp>

namespace benthos { namespace logbook {

/**
 * @brief Collection Proxy Object
 *
 * Proxy object which holds a shared pointer to a Persistent, as well as the
 * necessary information (session, class, id) required to reload the object.
 * If the object does not belong to a session, a strong pointer is held;
 * otherwise a weak pointer is held and the reload information is populated.
 *
 * The proxy object tracks changes to the object's state via the events sytem,
 * tracking session attach/detach and mapper insert/delete events to ensure the
 * object can still be retrieved.  Note that the proxy object's objective is to
 * retrieve the object at all costs, so when the object is detached, it is
 * transferred to a strong pointer in the proxy object.  It is the collection
 * object's responsibility to destroy the proxy object if the strong pointer
 * should not be held.
 */
class ProxyObject: public boost::noncopyable
{
public:
	typedef boost::shared_ptr<ProxyObject>		Ptr;
	typedef boost::weak_ptr<ProxyObject>		WPtr;

public:

	//! Class Constructor
	ProxyObject(Persistent::Ptr obj);

	//! Class Constructor
	ProxyObject(AbstractMapper::Ptr m, int64_t id, Persistent::Events & e);

	//! Class Destructor
	virtual ~ProxyObject();

	//! Compare with Persistent Pointer
	bool operator== (Persistent::Ptr obj) const;

public:

	/**
	 * @brief Return the Proxied Object
	 * @return Pointer to Object
	 *
	 * Returns a pointer to the proxied object, either directly, if the object
	 * is held with a strong pointer, by resolving the weak pointer, or by
	 * reloading the object from the database.  If the object cannot be loaded,
	 * an empty pointer is returned.
	 */
	Persistent::Ptr obj() const;

	//! Called when an object is attached to a session
	void on_attached(Persistent::Ptr, Session::Ptr);

	//! Called when an object is detached from a session
	void on_detached(Persistent::Ptr, Session::Ptr);

	//! Called when an object is inserted into the database
	void on_inserted(AbstractMapper::Ptr, Persistent::Ptr);

	//! Called when an object is deleted from the database
	void on_deleted(AbstractMapper::Ptr, Persistent::Ptr);

protected:

	//! Load the Object from the Database
	virtual Persistent::Ptr loadObj() const = 0;

protected:
	Persistent::Ptr					m_strongPtr;	///< Strong Pointer (objects which are not in a session)
	Persistent::WeakPtr				m_weakPtr;		///< Weak Pointer (objects which are in a session)
	mutable AbstractMapper::Ptr		m_mapper;		///< Mapper for the Object
	int64_t							m_objId;		///< Identifier for the Object

	boost::signals2::connection		m_cAttached;
	boost::signals2::connection		m_cDetached;
	boost::signals2::connection		m_cInserted;
	boost::signals2::connection		m_cDeleted;

};

/**
 * @brief Typed Proxy Object
 *
 * Implements ProxyObject::loadObj() to delegate to the correct mapper class.
 * This is required since the AbstractMapper and Mapper<> classes do not
 * implement a generic find() function.
 */
template <typename D>
class TypedProxyObject: public ProxyObject
{
public:

	//! Class Constructor
	TypedProxyObject(Persistent::Ptr obj)
		: ProxyObject(obj)
	{
	}

	//! Class Constructor
	TypedProxyObject(AbstractMapper::Ptr m, int64_t id)
		: ProxyObject(m, id, TypedPersistent<D>::ClassEvents())
	{
	}

	//! Class Destructor
	virtual ~TypedProxyObject()
	{
	}

protected:

	//! Load the Object from the Database
	virtual Persistent::Ptr loadObj() const
	{
		typename IFinder<D>::Ptr f = boost::dynamic_pointer_cast<IFinder<D> >(m_mapper);
		if (! f)
			throw std::runtime_error("Mapper for class " + TypedPersistent<D>().type_name() + " does not implement IFinder");

		typename D::Ptr obj = f->find(m_objId);
		return boost::dynamic_pointer_cast<Persistent>(obj);
	}

};

} } /* benthos::logbook */

#endif /* LOGBOOK_PROXY_OBJECT_HPP_ */
