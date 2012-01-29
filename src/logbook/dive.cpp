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
#include "logbook/dive.hpp"

using namespace logbook;

Dive::Dive()
	: TypedPersistent<Dive>(), m_repetition(1)
{
}

Dive::~Dive()
{
}

const boost::optional<double> & Dive::air_temp() const
{
	return m_airtemp;
}

const boost::optional<double> & Dive::avg_depth() const
{
	return m_avgdepth;
}

DiveComputer::Ptr Dive::computer() const
{
	return m_computer;
}

const boost::optional<time_t> & Dive::datetime() const
{
	return m_datetime;
}

int Dive::duration() const
{
	return m_duration;
}

const boost::optional<double> Dive::end_pressure() const
{
	return m_endpx;
}

int Dive::interval() const
{
	return m_interval;
}

double Dive::max_depth() const
{
	return m_maxdepth;
}

const boost::optional<double> & Dive::max_temp() const
{
	return m_maxtemp;
}

const boost::optional<double> & Dive::min_temp() const
{
	return m_mintemp;
}

Mix::Ptr Dive::mix() const
{
	return m_mix;
}

const boost::optional<int> & Dive::number() const
{
	return m_number;
}

int Dive::repetition() const
{
	return m_repetition;
}

DiveSite::Ptr Dive::site() const
{
	return m_site;
}

const boost::optional<double> & Dive::start_pressure() const
{
	return m_startpx;
}

const boost::optional<int> & Dive::utc_offset() const
{
	return m_utc_offset;
}

void Dive::setAirTemp(const boost::none_t &)
{
	m_airtemp.reset();
	mark_dirty();
}

void Dive::setAirTemp(double value)
{
	m_airtemp = value;
	mark_dirty();
}

void Dive::setAvgDepth(const boost::none_t &)
{
	m_avgdepth.reset();
	mark_dirty();
}

void Dive::setAvgDepth(double value)
{
	m_avgdepth = value;
	mark_dirty();
}

void Dive::setComputer(const boost::none_t &)
{
	m_computer.reset();
	mark_dirty();
}

void Dive::setComputer(DiveComputer::Ptr value)
{
	m_computer.swap(value);
	mark_dirty();
}

void Dive::setDateTime(const boost::none_t &)
{
	m_datetime.reset();
	mark_dirty();
}

void Dive::setDateTime(time_t value)
{
	m_datetime = value;
	mark_dirty();
}

void Dive::setDuration(int value)
{
	m_duration = value;
	mark_dirty();
}

void Dive::setEndPressure(const boost::none_t &)
{
	m_endpx.reset();
	mark_dirty();
}

void Dive::setEndPressure(double value)
{
	m_endpx.reset();
	mark_dirty();
}

void Dive::setInterval(int value)
{
	m_interval = value;
	mark_dirty();
}

void Dive::setMaxDepth(double value)
{
	m_maxdepth = value;
	mark_dirty();
}

void Dive::setMaxTemp(const boost::none_t &)
{
	m_maxtemp.reset();
	mark_dirty();
}

void Dive::setMaxTemp(double value)
{
	m_maxtemp = value;
	mark_dirty();
}

void Dive::setMinTemp(const boost::none_t &)
{
	m_mintemp.reset();
	mark_dirty();
}

void Dive::setMinTemp(double value)
{
	m_mintemp = value;
	mark_dirty();
}

void Dive::setMix(const boost::none_t &)
{
	m_mix.reset();
	mark_dirty();
}

void Dive::setMix(Mix::Ptr value)
{
	m_mix.swap(value);
	mark_dirty();
}

void Dive::setNumber(const boost::none_t &)
{
	m_number.reset();
	mark_dirty();
}

void Dive::setNumber(int value)
{
	m_number = value;
	mark_dirty();
}

void Dive::setRepetition(int value)
{
	m_repetition = value;
	mark_dirty();
}

void Dive::setSite(const boost::none_t &)
{
	m_site.reset();
	mark_dirty();
}

void Dive::setSite(DiveSite::Ptr value)
{
	m_site.swap(value);
	mark_dirty();
}

void Dive::setStartPressure(const boost::none_t &)
{
	m_startpx.reset();
	mark_dirty();
}

void Dive::setStartPressure(double value)
{
	m_startpx = value;
	mark_dirty();
}

void Dive::setUTCOffset(const boost::none_t &)
{
	m_utc_offset.reset();
	mark_dirty();
}

void Dive::setUTCOffset(int value)
{
	m_utc_offset = value;
	mark_dirty();
}
