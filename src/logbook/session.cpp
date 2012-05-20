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

#include "logbook/logging.hpp"

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

	// Prepare Savepoint Statements
	char hash[100];
	sprintf(hash, "__session_flush_%p", this);
	std::string spname(hash);
	std::string _begin("SAVEPOINT ");
	std::string _release("RELEASE SAVEPOINT ");
	std::string _rollback("ROLLBACK TO SAVEPOINT ");

	m_beginsp = statement::ptr(new statement(m_conn, _begin + spname));
	m_releasesp = statement::ptr(new statement(m_conn, _release + spname));
	m_rollbacksp = statement::ptr(new statement(m_conn, _rollback + spname));
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
	if (m_conn->transaction_active()) try
	{
		m_conn->rollback();
	}
	catch (std::exception & e)
	{
	}
}

void Session::add(Persistent::Ptr p)
{
	if (m_mappers.find(p->type_info()) == m_mappers.end())
		throw std::runtime_error(std::string("No mapper found for class ") + p->type_name());

	register_(p);

	std::list<Persistent::Ptr> cascade = cascade_add(p);
	std::list<Persistent::Ptr>::iterator it;
	for (it = cascade.begin(); it != cascade.end(); it++)
		register_(* it);
}

void Session::attach(Persistent::Ptr p)
{
	if (p->session() && (p->session().get() != this))
		throw std::runtime_error("Object is already registered with a different Session");

	id_key_type key(p->type_info(), p->id());
	if ((p->id() != -1) && (m_idmap.find(key) != m_idmap.end()) &&
		! m_idmap[key].expired() && (m_idmap[key].lock() != p))
	{
		throw std::runtime_error("Cannot register instance; another instance with "
				"the same key is already registered");
	}

	set_persistent_session(p, shared_from_this());
	m_events.after_attach(shared_from_this(), p);
}

void Session::begin()
{
	if (! m_conn->transaction_active())
		m_conn->begin();
	else
		throw std::runtime_error("A transaction is already active");
}

typedef std::list<Persistent::Ptr> (AbstractMapper::*pmfCascade)(Persistent::Ptr);

void walk_cascade_tree(Persistent::Ptr p, std::set<Persistent::Ptr> & set_, pmfCascade fn, Session::Ptr s, logging::logger * l)
{
	AbstractMapper::Ptr m = s->mapper(p->type_info());
	if (! m)
		throw std::runtime_error(std::string("No mapper found for class ") + p->type_name().c_str());

	if (set_.find(p) != set_.end())
		return;

	l->debug("Walking cascade tree for %s[%d][%p]", p->type_name().c_str(), p->id(), p.get());

	std::list<Persistent::Ptr> items = (m.get()->*fn)(p);
	std::list<Persistent::Ptr>::iterator it;
	for (it = items.begin(); it != items.end(); it++)
	{
		Persistent::Ptr item = * it;
		if (item && (set_.find(item) == set_.end()))
		{
			l->debug("Adding cascaded object of type %s[%d][%p]", (* it)->type_name().c_str(), (* it)->id(), (* it).get());

			set_.insert(* it);
			walk_cascade_tree(* it, set_, fn, s, l);
		}
	}

	l->debug("Done walking cascade tree");
}

std::list<Persistent::Ptr> Session::cascade_add(Persistent::Ptr p)
{
	std::set<Persistent::Ptr> result;
	walk_cascade_tree(p, result, & AbstractMapper::cascade_add, shared_from_this(), m_logger);
	return std::list<Persistent::Ptr>(result.begin(), result.end());
}

std::list<Persistent::Ptr> Session::cascade_delete(Persistent::Ptr p)
{
	std::set<Persistent::Ptr> result;
	walk_cascade_tree(p, result, & AbstractMapper::cascade_delete, shared_from_this(), m_logger);
	return std::list<Persistent::Ptr>(result.begin(), result.end());
}

std::list<Persistent::Ptr> Session::cascade_detach(Persistent::Ptr p)
{
	std::set<Persistent::Ptr> result;
	walk_cascade_tree(p, result, & AbstractMapper::cascade_detach, shared_from_this(), m_logger);
	return std::list<Persistent::Ptr>(result.begin(), result.end());
}

void Session::commit()
{
	if (! m_conn->transaction_active())
		begin();

	flush();
	m_conn->commit();
}

void Session::delete_(Persistent::Ptr p)
{
	if (m_mappers.find(p->type_info()) == m_mappers.end())
		throw std::runtime_error(std::string("No mapper found for class ") + p->type_name());

	if (p->id() == -1)
		throw std::runtime_error("Object is not persisted");

	if (m_deleted.find(p) != m_deleted.end())
		return;

	attach(p);

	id_key_type key(p->type_info(), p->id());
	std::list<Persistent::Ptr> cascade = cascade_delete(p);
	std::list<Persistent::Ptr>::iterator it;

	m_deleted.insert(p);
	m_idmap.insert(std::pair<id_key_type, Persistent::WeakPtr>(key, Persistent::WeakPtr(p)));

	for (it = cascade.begin(); it != cascade.end(); it++)
	{
		id_key_type itkey((* it)->type_info(), (* it)->id());
		if (((* it)->id() == -1) || (m_deleted.find(* it) != m_deleted.end()))
			continue;

		attach(* it);
		m_deleted.insert(* it);
		m_idmap.insert(std::pair<id_key_type, Persistent::WeakPtr>(key, Persistent::WeakPtr(* it)));
	}
}

uow_registry Session::deleted() const
{
	return m_deleted;
}

void Session::detach(Persistent::Ptr p)
{
	if ((p->id() == -1) && (m_new.find(p) != m_new.end()))
	{
		m_new.erase(p);
		set_persistent_session(p, Ptr());
		return;
	}

	id_key_type key(p->type_info(), p->id());
	m_idmap.erase(key);
	m_deleted.erase(p);

	set_persistent_session(p, Ptr());
}

uow_registry Session::dirty() const
{
	uow_registry dirty;
	identity_map::const_iterator it;
	for (it = m_idmap.begin(); it != m_idmap.end(); it++)
	{
		if (it->first.second == -1)
			m_logger->warning("Found identity map entry with key of -1");

		Persistent::Ptr p(it->second.lock());
		if (p && (m_deleted.find(p) == m_deleted.end()) && p->is_dirty())
			dirty.insert(p);
	}

	return dirty;
}

Session::Events & Session::events()
{
	return m_events;
}

void Session::expunge(Persistent::Ptr p)
{
	if (m_mappers.find(p->type_info()) == m_mappers.end())
		throw std::runtime_error(std::string("No mapper found for class ") + p->type_name());

	if (! p->session() || (p->session().get() != this))
		throw std::runtime_error("Object is not present within this session");

	id_key_type key(p->type_info(), p->id());
	std::list<Persistent::Ptr> cascade = cascade_detach(p);
	std::list<Persistent::Ptr>::iterator it;

	detach(p);
	for (it = cascade.begin(); it != cascade.end(); it++)
		detach(* it);
}

void Session::finalize_flush(std::list<Persistent::Ptr> & objects)
{
	std::list<Persistent::Ptr>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		id_key_type key((* it)->type_info(), (* it)->id());

		if (m_deleted.find(* it) != m_deleted.end())
		{
			m_logger->debug("Removing deleted item %s[%d] from session", (* it)->type_name().c_str(), (* it)->id());

			set_persistent_session(* it, Ptr());
			mark_persistent_deleted(* it);

			m_idmap.erase(key);
			m_deleted.erase(* it);
		}
		else if (m_new.find(* it) != m_new.end())
		{
			m_logger->debug("Adding inserted item %s[%d] to session", (* it)->type_name().c_str(), (* it)->id());

			if ((* it)->id() == -1)
				throw std::runtime_error("The new instance has an id of -1");

			mark_persistent_clean(* it);

			m_idmap.insert(std::pair<id_key_type, Persistent::WeakPtr>(key, Persistent::WeakPtr(* it)));
			m_new.erase(* it);
		}
		else
		{
			mark_persistent_clean(* it);
		}
	}
}

void Session::flush()
{
	logging::logger * l = logging::getLogger("session");

	uow_registry dirty_ = dirty();
	uow_registry objs;

	if (m_new.empty() && m_deleted.empty() && dirty_.empty())
		return;

	m_logger->debug("Calling Session::flush with %u insertions, %u deletions and %u updates",
		m_new.size(), m_deleted.size(), dirty_.size());

	objs.insert(m_deleted.begin(), m_deleted.end());
	objs.insert(m_new.begin(), m_new.end());
	objs.insert(dirty_.begin(), dirty_.end());

	/*
	 * The idea here is to do a topological sort on all updates but maintaining
	 * the delete-insert-update order so that updates to foreign keys persist
	 * correctly in one shot.
	 */
	std::list<Persistent::Ptr> updates = sort(objs);

	/*
	 * Ensure the flush() call executes in a savepoint so that if there is an
	 * error, only the current flush() will get rolled back.
	 */
	try
	{
		m_beginsp->exec();
		run_flush(updates);
		finalize_flush(updates);
		prune();
		m_releasesp->exec();
	}
	catch (std::exception & e)
	{
		m_rollbacksp->exec();
		throw;
	}
}

uow_registry Session::new_() const
{
	return m_new;
}

void Session::prune()
{
	ssize_t sl = m_idmap.size();

	identity_map::iterator it;
	for (it = m_idmap.begin(); it != m_idmap.end(); )
	{
		if (it->second.expired())
			it = m_idmap.erase(it);
		else
			++it;
	}

	m_logger->debug("Pruned identity map (currently %u items, removed %u expired pointers)", m_idmap.size(), sl - m_idmap.size());
}

void Session::rollback()
{
	if (m_conn->transaction_active())
		m_conn->rollback();
}

void Session::register_(Persistent::Ptr p)
{
	if (p->id() == -1)
		register_new(p);
	else
		register_update(p);
}

void Session::register_loaded(Persistent::Ptr p)
{
	id_key_type key(p->type_info(), p->id());
	if ((m_idmap.find(key) != m_idmap.end()) && (! m_idmap[key].expired()) && (m_idmap[key].lock() != p))
	{
		char msg[255];
		sprintf(msg, "Stale data detected in Identity Map: %s[%ld]", p->type_name().c_str(), p->id());
		throw std::runtime_error(std::string(msg));
	}

	m_idmap.insert(std::pair<id_key_type, Persistent::WeakPtr>(key, Persistent::WeakPtr(p)));
}

void Session::register_new(Persistent::Ptr p)
{
	if (p->id() != -1)
		throw std::runtime_error("Object is already persisted; it cannot be registered as new");

	attach(p);

	m_logger->debug("Registering new object of type %s", p->type_name().c_str());
	m_new.insert(p);
}

void Session::register_update(Persistent::Ptr p)
{
	if (p->id() == -1)
		throw std::runtime_error("Object is not persisted");

	id_key_type key(p->type_info(), p->id());
	if ((m_idmap.find(key) != m_idmap.end()) && (m_deleted.find(p) == m_deleted.end()))
		return;

	if (p->is_deleted())
		throw std::runtime_error("Object has been deleted; use make_transient() to return the "
				"object to transient status");

	attach(p);
	m_deleted.erase(p);
	m_idmap.insert(std::pair<id_key_type, Persistent::WeakPtr>(key, Persistent::WeakPtr(p)));
}

void Session::run_flush(std::list<Persistent::Ptr> & objects)
{
	std::list<Persistent::Ptr>::iterator it;
	for (it = objects.begin(); it != objects.end(); it++)
	{
		AbstractMapper::Ptr mapper(m_mappers[(* it)->type_info()]);
		if (! mapper)
			throw std::runtime_error(std::string("No mapper found for class ") + (* it)->type_name());

		if (m_deleted.find(* it) != m_deleted.end())
		{
			m_logger->debug("Calling remove() on %s[%d]", (* it)->type_name().c_str(), (* it)->id());
			mapper->remove(* it);
		}
		else if (m_new.find(* it) != m_new.end())
		{
			m_logger->debug("Calling insert() on %s[%p]", (* it)->type_name().c_str(), (* it).get());
			mapper->insert(* it);
		}
		else
		{
			m_logger->debug("Calling update() on %s[%d]", (* it)->type_name().c_str(), (* it)->id());
			mapper->update(* it);
		}
	}
}

std::list<Persistent::Ptr> Session::sort(const uow_registry & registry) const
{
	m_logger->debug("Sorting %u items in Session::sort()", registry.size());

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

	// Print how many items we ended up with
	m_logger->debug("Sorted %u items in Session::sort()", result.size());

	// Return sorted list
	items.clear();
	return result;
}
