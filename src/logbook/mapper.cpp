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

#include "benthos/logbook/mapper.hpp"
#include "benthos/logbook/session.hpp"

using namespace benthos::logbook;

AbstractMapper::AbstractMapper(boost::shared_ptr<Session> session)
	: m_session(session), m_conn(session->conn()), m_loaded()
{
}

AbstractMapper::~AbstractMapper()
{
}

void AbstractMapper::afterDelete(Persistent::Ptr o, int64_t oldId)
{
}

void AbstractMapper::afterInsert(Persistent::Ptr o)
{
}

void AbstractMapper::afterLoaded(Persistent::Ptr o)
{
}

void AbstractMapper::afterUpdate(Persistent::Ptr o)
{
}

void AbstractMapper::attachToSession(Persistent::Ptr o)
{
	Session::Ptr s = m_session.lock();
	if (! s)
		throw std::runtime_error("Session Pointer has Expired");
	s->register_loaded(o);
	set_persistent_session(o, s);
}

void AbstractMapper::beforeDelete(Persistent::Ptr o)
{
}

void AbstractMapper::beforeInsert(Persistent::Ptr o)
{
}

void AbstractMapper::beforeUpdate(Persistent::Ptr o)
{
}

std::list<Persistent::Ptr> AbstractMapper::cascade_add(Persistent::Ptr o)
{
	return std::list<Persistent::Ptr>();
}

std::list<Persistent::Ptr> AbstractMapper::cascade_delete(Persistent::Ptr o)
{
	return std::list<Persistent::Ptr>();
}

std::list<Persistent::Ptr> AbstractMapper::cascade_detach(Persistent::Ptr o)
{
	return std::list<Persistent::Ptr>();
}

AbstractMapper::Events & AbstractMapper::events()
{
	return m_events;
}

int64_t AbstractMapper::insert(Persistent::Ptr o)
{
	m_events.before_insert(shared_from_this(), o);
	beforeInsert(o);

	dbapi::statement::ptr s(insertStatement());
	s->reset();
	s->bind(1, boost::none);
	bindInsert(s, o);

	dbapi::cursor::ptr c = s->exec();

	set_persistent_id(o, c->last_rowid());
	m_loaded[o->id()] = Persistent::WeakPtr(o);

	afterInsert(o);
	m_events.after_insert(shared_from_this(), o);

	return o->id();
}

void AbstractMapper::remove(Persistent::Ptr o)
{
	m_events.before_delete(shared_from_this(), o);
	beforeDelete(o);

	dbapi::statement::ptr s(removeStatement());
	s->reset();
	s->bind(1, o->id());
	s->exec();

	afterDelete(o, o->id());
	m_events.after_delete(shared_from_this(), o);
}

void AbstractMapper::update(Persistent::Ptr o)
{
	m_events.before_update(shared_from_this(), o);
	beforeUpdate(o);

	dbapi::statement::ptr s(updateStatement());
	s->reset();
	s->bind(1, o->id());
	bindUpdate(s, o);
	s->exec();

	afterUpdate(o);
	m_events.after_update(shared_from_this(), o);
}
