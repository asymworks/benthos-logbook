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

#include <cmath>
#include <stdexcept>

#include "benthos/logbook/tank.hpp"
using namespace benthos::logbook;

Tank::Tank()
	: TypedPersistent<Tank>(), m_name(), m_type(), m_pressure(1), m_volume(0)
{
}

Tank::~Tank()
{
}

double Tank::capacity() const
{
	return m_volume * m_pressure;
}

const boost::optional<std::string> & Tank::name() const
{
	return m_name;
}

double Tank::pressure() const
{
	return m_pressure;
}

const boost::optional<std::string> & Tank::type() const
{
	return m_type;
}

double Tank::volume() const
{
	return m_volume;
}

void Tank::setCapacity(double value)
{
	if (value < 0)
		throw std::invalid_argument("Tank Capacity cannot be negative");

	m_volume = value / m_pressure;
	mark_dirty();
	events().attr_set(ptr(), "capacity", boost::any(value));
	events().attr_set(ptr(), "volume", boost::any(m_volume));
}

void Tank::setName(const boost::none_t &)
{
	m_name.reset();
	mark_dirty();
	events().attr_set(ptr(), "name", boost::any());
}

void Tank::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
	events().attr_set(ptr(), "name", boost::any(value));
}

void Tank::setPressure(double value)
{
	if (value < 1)
		throw std::invalid_argument("Working Pressure cannot be less than 1 bar");

	m_pressure = value;
	mark_dirty();
	events().attr_set(ptr(), "pressure", boost::any(value));
}

void Tank::setType(const boost::none_t &)
{
	m_type.reset();
	mark_dirty();
	events().attr_set(ptr(), "type", boost::any());
}

void Tank::setType(const std::string & value)
{
	std::string lvalue(value);
	std::transform(value.begin(), value.end(), lvalue.begin(), ::tolower);

	if ((lvalue != "aluminum") && (lvalue != "steel"))
		throw std::invalid_argument("Type must be either 'aluminum' or 'steel'");

	m_type = lvalue;
	mark_dirty();
	events().attr_set(ptr(), "type", boost::any(lvalue));
}

void Tank::setVolume(double value)
{
	if (value < 0)
		throw std::invalid_argument("Tank Volume cannot be negative");

	m_volume = value;
	events().attr_set(ptr(), "capacity", boost::any(capacity()));
	events().attr_set(ptr(), "volume", boost::any(value));
}
