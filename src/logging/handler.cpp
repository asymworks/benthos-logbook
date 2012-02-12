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

#include "logbook/logging/filter.hpp"
#include "logbook/logging/formatter.hpp"
#include "logbook/logging/handler.hpp"

using namespace logbook::logging;

log_handler::log_handler(int level)
	: m_level(level), m_formatter(0), m_mutex()
{
}

log_handler::~log_handler()
{
	if (m_formatter != 0)
	{
		delete m_formatter;
		m_formatter = 0;
	}
}

void log_handler::acquire()
{
	m_mutex.lock();
}

void log_handler::addFilter(log_filter * filter)
{
	if (! hasFilter(filter))
		m_filters.push_back(filter);
}

void log_handler::close()
{
	flush();
}

bool log_handler::filter(const log_record & rec) const
{
	std::list<log_filter *>::const_iterator it = m_filters.begin();
	for ( ; it != m_filters.end(); it++)
	{
		if (! (* it)->apply(rec))
			return false;
	}

	return true;
}

void log_handler::flush()
{
}

std::string default_format(const log_record & rec)
{
	std::string s;
	s = rec.asctime() + " ["
		+ rec.channel() + "] "
		+ rec.level_name() + ": "
		+ rec.message() + "\n";
	return s;
}

std::string log_handler::format(const log_record & rec) const
{
	if (m_formatter != 0)
		return m_formatter->format(rec);

	// Default Format String
	return default_format(rec);
}

void log_handler::handle(const log_record & rec)
{
	if ((rec.level() < m_level) || (! filter(rec)))
		return;

	acquire();

	try
	{
		emit_(rec);
	}
	catch (std::exception & e)
	{
		handleError(rec);
	}

	release();
}

void log_handler::handleError(const log_record & rec)
{
}

bool log_handler::hasFilter(log_filter * filt) const
{
	std::list<log_filter *>::const_iterator it = m_filters.begin();
	for ( ; it != m_filters.end(); it++)
	{
		if ((* it) == filt)
			return true;
	}
	return false;
}

int log_handler::level() const
{
	return m_level;
}

void log_handler::release()
{
	m_mutex.unlock();
}

void log_handler::removeFilter(log_filter * filter)
{
	if (hasFilter(filter))
		m_filters.remove(filter);
}

void log_handler::setFormatter(formatter * form)
{
	if (m_formatter != 0)
		delete m_formatter;

	m_formatter = form;
}

void log_handler::setLevel(int lvl)
{
	m_level = lvl;
}
