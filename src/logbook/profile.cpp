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

#include "benthos/logbook/profile.hpp"
#include "benthos/logbook/session.hpp"

using namespace benthos::logbook;

Profile::Profile()
	: TypedPersistent<Profile>()
{
}

Profile::~Profile()
{
}

void Profile::attached(Session::Ptr s)
{
	Persistent::attached(s);

	m_evtComputerDel = s->mapper<DiveComputer>()->events().before_delete.connect(boost::bind(& Profile::evtDiveComputerDeleted, this, _1, _2));
	m_evtDiveDel = s->mapper<Dive>()->events().before_delete.connect(boost::bind(& Profile::evtDiveDeleted, this, _1, _2));
	m_evtMixDel = s->mapper<Mix>()->events().before_delete.connect(boost::bind(& Profile::evtMixDeleted, this, _1, _2));
}

void Profile::detached(Session::Ptr s)
{
	m_evtComputerDel.disconnect();
	m_evtDiveDel.disconnect();
	m_evtMixDel.disconnect();

	Persistent::detached(s);
}

void Profile::evtDiveComputerDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	DiveComputer::Ptr o = boost::dynamic_pointer_cast<DiveComputer>(obj);
	if (o && (o == m_computer))
		setComputer(boost::none);
}

void Profile::evtDiveDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	Dive::Ptr o = boost::dynamic_pointer_cast<Dive>(obj);
	if (o && (o == m_dive))
		setDive(boost::none);
}

void Profile::evtMixDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	Mix::Ptr o = boost::dynamic_pointer_cast<Mix>(obj);
	if (! o || m_profile.empty())
		return;

	std::list<waypoint>::iterator it;
	for (it = m_profile.begin(); it != m_profile.end(); it++)
		if (it->mix == o)
			it->mix.reset();

	events().attr_set(ptr(), "keys", boost::any());
	events().attr_set(ptr(), "profile", boost::any());
}

DiveComputer::Ptr Profile::computer() const
{
	return m_computer;
}

Dive::Ptr Profile::dive() const
{
	return m_dive;
}

const boost::optional<time_t> & Profile::imported() const
{
	return m_imported;
}

const std::set<std::string> & Profile::keys() const
{
	return m_keys;
}

const boost::optional<std::string> & Profile::name() const
{
	return m_name;
}

const std::list<waypoint> & Profile::profile() const
{
	return m_profile;
}

const std::vector<unsigned char> & Profile::raw_profile() const
{
	return m_raw;
}

const boost::optional<std::string> & Profile::vendor() const
{
	return m_vendor;
}

void Profile::setComputer(const boost::none_t &)
{
	m_computer.reset();
	mark_dirty();
	events().attr_set(ptr(), "computer", boost::any());
}

void Profile::setComputer(DiveComputer::Ptr value)
{
	m_computer = value;
	mark_dirty();
	events().attr_set(ptr(), "computer", boost::any(value));
}

void Profile::setDive(const boost::none_t &)
{
	m_dive.reset();
	mark_dirty();
	events().attr_set(ptr(), "dive", boost::any());
}

void Profile::setDive(Dive::Ptr value)
{
	m_dive = value;
	mark_dirty();
	events().attr_set(ptr(), "dive", boost::any(value));
}

void Profile::setImported(const boost::none_t &)
{
	m_imported.reset();
	mark_dirty();
	events().attr_set(ptr(), "imported", boost::any());
}

void Profile::setImported(const time_t & value)
{
	m_imported = value;
	mark_dirty();
	events().attr_set(ptr(), "imported", boost::any(value));
}

void Profile::setName(const boost::none_t &)
{
	m_name.reset();
	mark_dirty();
	events().attr_set(ptr(), "name", boost::any());
}

void Profile::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
	events().attr_set(ptr(), "name", boost::any(value));
}

void Profile::setProfile(const boost::none_t &)
{
	m_profile.clear();
	m_keys.clear();
	mark_dirty();
	events().attr_set(ptr(), "keys", boost::any());
	events().attr_set(ptr(), "profile", boost::any());
}

void Profile::setProfile(const std::list<waypoint> & value)
{
	m_profile.assign(value.begin(), value.end());
	m_keys.clear();

	// Update the list of Keys
	std::list<waypoint>::const_iterator it;
	for (it = m_profile.begin(); it != m_profile.end(); it++)
	{
		std::map<std::string, double, cicmp>::const_iterator mit;
		for (mit = it->data.begin(); mit != it->data.end(); mit++)
		{
			std::string lkey(mit->first);
			std::transform(lkey.begin(), lkey.end(), lkey.begin(), tolower);
			m_keys.insert(lkey);
		}
	}

	mark_dirty();
	events().attr_set(ptr(), "keys", boost::any());
	events().attr_set(ptr(), "profile", boost::any());
}

void Profile::setRawProfile(const boost::none_t &)
{
	m_raw.clear();
	mark_dirty();
	events().attr_set(ptr(), "raw_profile", boost::any());
}

void Profile::setRawProfile(const std::vector<unsigned char> & value)
{
	m_raw.assign(value.begin(), value.end());
	mark_dirty();
	events().attr_set(ptr(), "raw_profile", boost::any(value));
}

void Profile::setVendor(const boost::none_t &)
{
	m_vendor.reset();
	mark_dirty();
	events().attr_set(ptr(), "vendor", boost::any());
}

void Profile::setVendor(const std::string & value)
{
	m_vendor = value;
	mark_dirty();
	events().attr_set(ptr(), "vendor", boost::any(value));
}
