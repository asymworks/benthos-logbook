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

#include "benthos/logbook/config.hpp"
#include "benthos/logbook/persistent.hpp"
#include "benthos/logbook/session.hpp"

using namespace benthos::logbook;

Persistent::Persistent()
	: m_deleted(false), m_dirty(false), m_loading(false), m_id(-1), m_session()
{
}

Persistent::~Persistent()
{
}

void Persistent::attached(Session::Ptr s)
{
	class_events().attached(shared_from_this(), s);
}

void Persistent::detached(Session::Ptr s)
{
	class_events().detached(shared_from_this(), s);
}

Persistent::Events & Persistent::events()
{
	return class_events();
}

int64_t Persistent::id() const
{
	return m_id;
}

void Persistent::mark_clean()
{
	m_deleted = false;
	m_dirty = false;
	m_loading = false;
}

void Persistent::mark_deleted()
{
	m_deleted = true;
}

void Persistent::mark_dirty()
{
	m_dirty = true;
}

void Persistent::mark_loading()
{
	m_loading = true;
}

Persistent::Ptr Persistent::ptr()
{
	return shared_from_this();
}

Persistent::CPtr Persistent::ptr() const
{
	return shared_from_this();
}

Session::Ptr Persistent::session() const
{
	return m_session.lock();
}

void Persistent::set_id(int64_t id)
{
	m_id = id;
}

void Persistent::set_session(Session::Ptr p)
{
	if (! m_session.expired())
		detached(m_session.lock());

	m_session = p;

	if (p)
		attached(p);
}

#ifdef HAVE_CXXABI_H
#include <cxxabi.h>
#endif

std::string Persistent::type_name() const
{
#ifdef HAVE_CXA_DEMANGLE
	int status = -4;
	const char * tn = type_info()->name();
	char * res = abi::__cxa_demangle(tn, NULL, NULL, & status);
	std::string ret(status == 0 ? res : tn);
	free(res);
	return ret;
#else
	return std::string(type_info()->name());
#endif
}
