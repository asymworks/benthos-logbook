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

#include <stdexcept>
#include "benthos/logbook/dive_tank.hpp"
#include "benthos/logbook/profile.hpp"
#include "benthos/logbook/session.hpp"

using namespace benthos::logbook;

DiveTank::DiveTank(Dive::Ptr dive)
	: m_dive(dive), m_index(1)
{
}

DiveTank::~DiveTank()
{
}

void DiveTank::attached(Session::Ptr s)
{
	Persistent::attached(s);

	m_evtMixDel = s->mapper<Mix>()->events().before_delete.connect(boost::bind(& DiveTank::evtMixDeleted, this, _1, _2));
	m_evtTankDel = s->mapper<Tank>()->events().before_delete.connect(boost::bind(& DiveTank::evtTankDeleted, this, _1, _2));
}

void DiveTank::detached(Session::Ptr s)
{
	m_evtMixDel.disconnect();
	m_evtTankDel.disconnect();

	Persistent::detached(s);
}

void DiveTank::evtMixDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	Mix::Ptr o = boost::dynamic_pointer_cast<Mix>(obj);
	if (o && (o == m_mix))
		setMix(boost::none);
}

void DiveTank::evtTankDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	Tank::Ptr o = boost::dynamic_pointer_cast<Tank>(obj);
	if (o && (o == m_tank))
		setTank(boost::none);
}

Dive::Ptr DiveTank::dive() const
{
	return m_dive;
}

const boost::optional<double> & DiveTank::end_pressure() const
{
	return m_pxend;
}

int DiveTank::index() const
{
	return m_index;
}

Mix::Ptr DiveTank::mix() const
{
	return m_mix;
}

const boost::optional<double> & DiveTank::start_pressure() const
{
	return m_pxstart;
}

Tank::Ptr DiveTank::tank() const
{
	return m_tank;
}

void DiveTank::setEndPressure(const boost::none_t &)
{
	m_pxend.reset();
	mark_dirty();
	events().attr_set(ptr(), "end_pressure", boost::any());
}

void DiveTank::setEndPressure(double value)
{
	m_pxend = value;
	mark_dirty();
	events().attr_set(ptr(), "end_pressure", boost::any(value));
}

void DiveTank::setIndex(int value)
{
	if (value < 1)
		throw std::invalid_argument("Index must be greater than 0");

	m_index = value;
	mark_dirty();
	events().attr_set(ptr(), "index", boost::any(value));
}

void DiveTank::setMix(const boost::none_t &)
{
	m_mix.reset();
	mark_dirty();
	events().attr_set(ptr(), "mix", boost::any());
}

void DiveTank::setMix(Mix::Ptr value)
{
	m_mix = value;
	mark_dirty();
	events().attr_set(ptr(), "mix", boost::any(value));
}

void DiveTank::setStartPressure(const boost::none_t &)
{
	m_pxstart.reset();
	mark_dirty();
	events().attr_set(ptr(), "start_pressure", boost::any());
}

void DiveTank::setStartPressure(double value)
{
	m_pxstart = value;
	mark_dirty();
	events().attr_set(ptr(), "start_pressure", boost::any(value));
}

void DiveTank::setTank(const boost::none_t &)
{
	m_tank.reset();
	mark_dirty();
	events().attr_set(ptr(), "tank", boost::any());
}

void DiveTank::setTank(Tank::Ptr value)
{
	m_tank = value;
	mark_dirty();
	events().attr_set(ptr(), "tank", boost::any(value));
}
