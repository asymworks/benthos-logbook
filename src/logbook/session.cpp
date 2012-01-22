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

#include "logbook/session.hpp"

using namespace logbook;

Session::Session(connection::ptr conn)
	: m_conn(conn), m_mappers()
{
	// Ensure Foreign Key Checks are enabled
	m_conn->exec_sql("pragma foreign_keys=1");
}

Session::~Session()
{
}

void Session::commit()
{
	m_conn->begin();
	insertNew();
	updateDirty();
	deleteRemoved();
	m_conn->commit();

	// Register New objects with this Session
	uow_registry::iterator it;
	for (it = m_new.begin(); it != m_new.end(); it++)
		set_persistent_session(it->second, shared_from_this());

	// Unregister Deleted objects from this Session
	for (it = m_deleted.begin(); it != m_deleted.end(); it++)
		set_persistent_session(it->second, Session::Ptr());

	// Clear the UOW Lists
	m_new.clear();
	m_dirty.clear();
	m_deleted.clear();
}

void Session::deleteRemoved()
{
	uow_registry::iterator it;
	for (it = m_deleted.begin(); it != m_deleted.end(); it++)
	{
		AbstractMapper::Ptr p(m_mappers[it->first]);
		if (! p)
			throw std::runtime_error(std::string("No mapper found for class ") + it->first->name());

		p->remove(it->second);
	}
}

void Session::insertNew()
{
	uow_registry::iterator it;
	for (it = m_new.begin(); it != m_new.end(); it++)
	{
		AbstractMapper::Ptr p(m_mappers[it->first]);
		if (! p)
			throw std::runtime_error(std::string("No mapper found for class ") + it->first->name());

		p->insert(it->second);
	}
}

void Session::registerDeleted(Persistent::Ptr p)
{
	if (! p)
		throw std::invalid_argument("Deleted object cannot be None");

	if (! p->session() || (p->session().get() != this))
		throw std::runtime_error("Object does not belong to this Session");

	uow_entry e(p->type_info(), p);

	if (m_new.find(e) != m_new.end())
	{
		m_new.erase(e);
		return;
	}

	m_dirty.erase(e);
	if (m_deleted.find(e) == m_deleted.end())
		m_deleted.insert(e);
}

void Session::registerDirty(Persistent::Ptr p)
{
	if (! p)
		throw std::invalid_argument("Dirty object cannot be None");

	if (! p->session() || (p->session().get() != this))
		throw std::runtime_error("Object does not belong to this Session");

	uow_entry e(p->type_info(), p);

	if (m_deleted.find(e) != m_deleted.end())
		throw std::runtime_error("Object is already registered as deleted");

	if ((m_dirty.find(e) == m_dirty.end()) && (m_new.find(e) == m_new.end()))
		m_dirty.insert(e);
}

void Session::registerNew(Persistent::Ptr p)
{
	if (! p)
		throw std::invalid_argument("New object cannot be None");

	if (p->session() && (p->session().get() != this))
		throw std::runtime_error("Object is already owned by a Session");

	uow_entry e(p->type_info(), p);

	if (m_new.find(e) != m_new.end())
		throw std::runtime_error("Object is already registered as new");
	if (m_dirty.find(e) != m_dirty.end())
		throw std::runtime_error("Object is already registered as dirty");
	if (m_deleted.find(e) != m_deleted.end())
		throw std::runtime_error("Object is already registered as deleted");

	m_new.insert(e);
}

void Session::updateDirty()
{
	uow_registry::iterator it;
	for (it = m_dirty.begin(); it != m_dirty.end(); it++)
	{
		AbstractMapper::Ptr p(m_mappers[it->first]);
		if (! p)
			throw std::runtime_error(std::string("No mapper found for class ") + it->first->name());

		p->update(it->second);
	}
}
