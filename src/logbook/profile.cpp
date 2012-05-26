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

using namespace logbook;

Profile::Profile()
	: TypedPersistent<Profile>()
{
}

Profile::~Profile()
{
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
	if (m_computer && ! m_computer->profiles()->in_cascade())
	{
		m_computer->profiles()->remove(boost::dynamic_pointer_cast<Profile>(shared_from_this()), false);
	}

	m_computer.reset();
	mark_dirty();
}

void Profile::setComputer(DiveComputer::Ptr value)
{
	if (m_computer && ! m_computer->profiles()->in_cascade())
	{
		m_computer->profiles()->remove(boost::dynamic_pointer_cast<Profile>(shared_from_this()), false);
	}

	if (value && ! value->profiles()->in_cascade())
	{
		value->profiles()->add(boost::dynamic_pointer_cast<Profile>(shared_from_this()), false);
	}

	m_computer.swap(value);
	mark_dirty();
}

void Profile::setDive(const boost::none_t &)
{
	if (m_dive && ! m_dive->profiles()->in_cascade())
	{
		m_dive->profiles()->remove(boost::dynamic_pointer_cast<Profile>(shared_from_this()), false);
	}

	m_dive.reset();
	mark_dirty();
}

void Profile::setDive(Dive::Ptr value)
{
	if (m_dive && ! m_dive->profiles()->in_cascade())
	{
		m_dive->profiles()->remove(boost::dynamic_pointer_cast<Profile>(shared_from_this()), false);
	}

	if (value && ! value->profiles()->in_cascade())
	{
		value->profiles()->add(boost::dynamic_pointer_cast<Profile>(shared_from_this()), false);
	}

	m_dive.swap(value);
	mark_dirty();
}

void Profile::setImported(const boost::none_t &)
{
	m_imported.reset();
	mark_dirty();
}

void Profile::setImported(const time_t & value)
{
	m_imported = value;
	mark_dirty();
}

void Profile::setName(const boost::none_t &)
{
	m_name.reset();
	mark_dirty();
}

void Profile::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
}

void Profile::setProfile(const boost::none_t &)
{
	m_profile.clear();
	m_keys.clear();
	mark_dirty();
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
}

void Profile::setRawProfile(const boost::none_t &)
{
	m_raw.clear();
	mark_dirty();
}

void Profile::setRawProfile(const std::vector<unsigned char> & value)
{
	m_raw.assign(value.begin(), value.end());
	mark_dirty();
}

void Profile::setVendor(const boost::none_t &)
{
	m_vendor.reset();
	mark_dirty();
}

void Profile::setVendor(const std::string & value)
{
	m_vendor = value;
	mark_dirty();
}
