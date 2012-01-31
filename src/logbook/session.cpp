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

#include "mappers/dive_computer_mapper.hpp"
#include "mappers/dive_site_mapper.hpp"
#include "mappers/dive_mapper.hpp"
#include "mappers/mix_mapper.hpp"
#include "mappers/profile_mapper.hpp"

using namespace logbook;

Session::Session(connection::ptr conn)
	: m_conn(conn), m_mappers(), m_logger(logging::getLogger("orm.session"))
{
	// Ensure Foreign Key Checks are enabled
	m_conn->exec_sql("pragma foreign_keys=1");
}

Session::Ptr Session::Create(connection::ptr conn)
{
	Session::Ptr ptr(new Session(conn));

	// Register Mappers
	ptr->registerMapper<Dive>(new mappers::DiveMapper(ptr));
	ptr->registerMapper<DiveComputer>(new mappers::DiveComputerMapper(ptr));
	ptr->registerMapper<DiveSite>(new mappers::DiveSiteMapper(ptr));
	ptr->registerMapper<Mix>(new mappers::MixMapper(ptr));
	ptr->registerMapper<Profile>(new mappers::ProfileMapper(ptr));

	// Return new Session
	return ptr;
}

Session::~Session()
{
}

void Session::commit()
{
	m_conn->begin();
	insertNew();
	deleteRemoved();
	updateDirty();
	m_conn->commit();

	// Unregister Deleted objects from this Session
	uow_registry::iterator it;
	for (it = m_deleted.begin(); it != m_deleted.end(); it++)
		set_persistent_session(* it, Session::Ptr());

	// Clear the UOW Lists
	m_new.clear();
	m_dirty.clear();
	m_deleted.clear();
}

void Session::deleteRemoved()
{
	std::list<Persistent::Ptr>::iterator it;
	std::list<Persistent::Ptr> items = sort(m_deleted);
	for (it = items.begin(); it != items.end(); it++)
	{
		AbstractMapper::Ptr p(m_mappers[(* it)->type_info()]);
		if (! p)
			throw std::runtime_error(std::string("No mapper found for class ") + (* it)->type_info()->name());

		p->remove(* it);
	}
}

void Session::insertNew()
{
	std::list<Persistent::Ptr>::iterator it;
	std::list<Persistent::Ptr> items = sort(m_new);
	for (it = items.begin(); it != items.end(); it++)
	{
		AbstractMapper::Ptr p(m_mappers[(* it)->type_info()]);
		if (! p)
			throw std::runtime_error(std::string("No mapper found for class ") + (* it)->type_info()->name());

		p->insert(* it);
	}
}

bool Session::isDeleted(Persistent::Ptr p) const
{
	return (m_deleted.find(p) != m_deleted.end());
}

bool Session::isDirty(Persistent::Ptr p) const
{
	return (m_dirty.find(p) != m_dirty.end());
}

bool Session::isNew(Persistent::Ptr p) const
{
	return (m_new.find(p) != m_new.end());
}

void Session::registerDeleted(Persistent::Ptr p)
{
	if (! p)
		throw std::invalid_argument("Deleted object cannot be None");

	if (! p->session() || (p->session().get() != this))
		throw std::runtime_error("Object does not belong to this Session");

	if (m_new.find(p) != m_new.end())
	{
		m_new.erase(p);
		return;
	}

	m_dirty.erase(p);
	if (m_deleted.find(p) == m_deleted.end())
	{
		m_logger->debug("Registering deleted object %s[%d]", p->type_info()->name(), p->id());
		m_deleted.insert(p);
	}
}

void Session::registerDirty(Persistent::Ptr p)
{
	if (! p)
		throw std::invalid_argument("Dirty object cannot be None");

	if (! p->session() || (p->session().get() != this))
		throw std::runtime_error("Object does not belong to this Session");

	if (m_deleted.find(p) != m_deleted.end())
		throw std::runtime_error("Object is already registered as deleted");

	if ((m_dirty.find(p) == m_dirty.end()) && (m_new.find(p) == m_new.end()))
	{
		m_logger->debug("Registering dirty object %s[%d]", p->type_info()->name(), p->id());
		m_dirty.insert(p);
	}
}

void Session::registerNew(Persistent::Ptr p)
{
	if (! p)
		throw std::invalid_argument("New object cannot be None");

	if (p->session() && (p->session().get() != this))
		throw std::runtime_error("Object is already owned by a Session");

	if (m_new.find(p) != m_new.end())
		throw std::runtime_error("Object is already registered as new");
	if (m_dirty.find(p) != m_dirty.end())
		throw std::runtime_error("Object is already registered as dirty");
	if (m_deleted.find(p) != m_deleted.end())
		throw std::runtime_error("Object is already registered as deleted");

	set_persistent_session(p, shared_from_this());
	m_logger->debug("Registering new object of type %s", p->type_info()->name());

	m_new.insert(p);
}

std::list<Persistent::Ptr> Session::sort(const uow_registry & registry) const
{
	/*
	 * Hard-coded, inefficient topological sort.  This should be adequate for
	 * most use cases but can certainly be improved.
	 */

	std::list<Persistent::Ptr>::iterator it;
	std::list<Persistent::Ptr> items(registry.begin(), registry.end());
	std::list<Persistent::Ptr> result;

	// Scan for Mix/DiveSite/DiveComputer
	for (it = items.begin(); it != items.end(); )
	{
		if (((* it)->type_info() == (& typeid(Mix))) ||
			((* it)->type_info() == (& typeid(DiveSite))) ||
			((* it)->type_info() == (& typeid(DiveComputer))))
		{
			result.push_back(* it);
			it = items.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Scan for Dive
	for (it = items.begin(); it != items.end(); )
	{
		if ((* it)->type_info() == (& typeid(Dive)))
		{
			result.push_back(* it);
			it = items.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Append remainder of the Items (Profiles, others)
	for (it = items.begin(); it != items.end(); it++)
		result.push_back(* it);

	// Return sorted list
	items.clear();
	return result;
}

void Session::updateDirty()
{
	std::list<Persistent::Ptr>::iterator it;
	std::list<Persistent::Ptr> items = sort(m_dirty);
	for (it = items.begin(); it != items.end(); it++)
	{
		AbstractMapper::Ptr p(m_mappers[(* it)->type_info()]);
		if (! p)
			throw std::runtime_error(std::string("No mapper found for class ") + (* it)->type_info()->name());

		p->update(* it);
	}
}
