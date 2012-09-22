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

#include "benthos/logbook/proxy_object.hpp"
using namespace benthos::logbook;

ProxyObject::ProxyObject(Persistent::Ptr obj)
	: m_strongPtr(), m_weakPtr(), m_mapper(), m_objId(-1),
	  m_cAttached(), m_cDetached(), m_cInserted(), m_cDeleted()
{
	if (! obj)
		throw std::runtime_error("ProxyObject cannot proxy an empty pointer");

	if (obj->session())
	{
		// Store the Mapper and Id
		m_weakPtr = obj;
		m_mapper = obj->session()->mapper(obj->type_info());
		m_objId = obj->id();

		if (! m_mapper)
			throw std::runtime_error("Mapper for type " + obj->type_name() + " not registered");

		m_cInserted = m_mapper->events().after_insert.connect(boost::bind(& ProxyObject::on_inserted, this, _1, _2));
		m_cDeleted = m_mapper->events().before_delete.connect(boost::bind(& ProxyObject::on_deleted, this, _1, _2));
	}
	else
	{
		// Store the Strong Pointer
		m_strongPtr = obj;
	}

	// Register Events
	m_cAttached = obj->class_events().attached.connect(boost::bind(& ProxyObject::on_attached, this, _1, _2));
	m_cDetached = obj->class_events().detached.connect(boost::bind(& ProxyObject::on_detached, this, _1, _2));
}

ProxyObject::ProxyObject(AbstractMapper::Ptr m, int64_t id, Persistent::Events & e)
	: m_strongPtr(), m_weakPtr(), m_mapper(m), m_objId(id),
	  m_cAttached(), m_cDetached(), m_cInserted(), m_cDeleted()
{
	if (! m)
		throw std::runtime_error("ProxyObject cannot proxy an object without a mapper");

	// Register Events
	m_cAttached = e.attached.connect(boost::bind(& ProxyObject::on_attached, this, _1, _2));
	m_cDetached = e.detached.connect(boost::bind(& ProxyObject::on_detached, this, _1, _2));

	m_cInserted = m_mapper->events().after_insert.connect(boost::bind(& ProxyObject::on_inserted, this, _1, _2));
	m_cDeleted = m_mapper->events().before_delete.connect(boost::bind(& ProxyObject::on_deleted, this, _1, _2));
}

ProxyObject::~ProxyObject()
{
	m_cAttached.disconnect();
	m_cDetached.disconnect();

	if (m_cInserted.connected())
		m_cInserted.disconnect();
	if (m_cDeleted.connected())
		m_cDeleted.disconnect();
}

bool ProxyObject::operator== (Persistent::Ptr obj) const
{
	if (! obj)
		return false;

	/*
	 * If we are holding a strong pointer, it's an easy comparison
	 */
	if (m_strongPtr)
		return (m_strongPtr == obj);

	/*
	 * If we are not holding a strong pointer, the object must be in a session
	 * to be equal to the object we are proxying.
	 */
	if (! obj->session())
		return false;

	/*
	 * If the weak pointer is still resolvable, it's again an easy comparison
	 */
	Persistent::Ptr p = m_weakPtr.lock();
	if (p)
		return (p == obj);

	/*
	 * If the object id is -1 (i.e. the object is in the session but not
	 * committed to the database, and for some reason we lost the weak pointer),
	 * return -1 since it's not guaranteed to be the same object.
	 */
	if (m_objId == -1)
		return false;

	/*
	 * Last resort is to compare the mapper and object id values.
	 */
	if ((m_mapper == obj->session()->mapper(obj->type_info())) && (m_objId == obj->id()))
		return true;

	// No match
	return false;
}

Persistent::Ptr ProxyObject::obj() const
{
	// Return a Strong Pointer
	if (m_strongPtr)
		return m_strongPtr;

	// Dereference the Weak Pointer
	Persistent::Ptr p = m_weakPtr.lock();
	if (p)
		return p;

	// If the object is not in the database, return empty
	if (m_objId == -1)
		return Persistent::Ptr();

	// Load from Database
	return loadObj();
}

void ProxyObject::on_attached(Persistent::Ptr o, Session::Ptr s)
{
	if (m_strongPtr && (o == m_strongPtr))
	{
		m_strongPtr.reset();
		m_weakPtr = o;
		m_mapper = o->session()->mapper(o->type_info());
		m_objId = o->id();

		if (! m_mapper)
			throw std::runtime_error("Mapper for type " + o->type_name() + " not registered");

		m_cInserted = m_mapper->events().after_insert.connect(boost::bind(& ProxyObject::on_inserted, this, _1, _2));
		m_cDeleted = m_mapper->events().before_delete.connect(boost::bind(& ProxyObject::on_deleted, this, _1, _2));
	}
}

void ProxyObject::on_detached(Persistent::Ptr o, Session::Ptr s)
{
	Persistent::Ptr p = m_weakPtr.lock();
	AbstractMapper::Ptr m = s->mapper(o->type_info());

	if ((p && (p == o)) || ((m_objId != -1) && (m == m_mapper) && (o->id() == m_objId)))
	{
		m_strongPtr = o;
		m_weakPtr.reset();
		m_mapper.reset();
		m_objId = -1;

		m_cInserted.disconnect();
		m_cDeleted.disconnect();
	}
}

void ProxyObject::on_inserted(AbstractMapper::Ptr m, Persistent::Ptr o)
{
	Persistent::Ptr p = m_weakPtr.lock();
	if (p && (p == o))
		m_objId = o->id();
}

void ProxyObject::on_deleted(AbstractMapper::Ptr m, Persistent::Ptr o)
{
	Persistent::Ptr p = m_weakPtr.lock();
	if ((p && (p == o)) || ((m_objId != -1) && (m == m_mapper) && (o->id() == m_objId)))
		m_objId = -1;
}
