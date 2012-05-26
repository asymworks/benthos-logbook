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

#include <cstdlib>

#include "benthos/logbook/logging/handler.hpp"
#include "benthos/logbook/logging/logger.hpp"
#include "benthos/logbook/logging/record.hpp"

using namespace logbook::logging;

logger::logger(logger * parent, const std::string & name)
	: m_parent(parent), m_name(name), m_level(level::NOTSET),
	  m_propagate(true), m_handlers()
{
}

logger::~logger()
{
}

void logger::addHandler(log_handler * handler)
{
	if (! hasHandler(handler))
		m_handlers.push_back(handler);
}

void logger::critical(const char * fmt, ...) const
{
	va_list ap;
	va_start(ap, fmt);
	log(level::CRITICAL, fmt, ap);
	va_end(ap);
}

void logger::critical(const std::string & msg) const
{
	log(level::CRITICAL, msg);
}

void logger::debug(const char * fmt, ...) const
{
	va_list ap;
	va_start(ap, fmt);
	log(level::DEBUG, fmt, ap);
	va_end(ap);
}

void logger::debug(const std::string & msg) const
{
	log(level::DEBUG, msg);
}

int logger::effective_level() const
{
	const logger * l = this;
	while ((l->m_level == level::NOTSET) && (l != NULL))
		l = l->m_parent;

	if (l == NULL)
		return level::NOTSET;
	return l->m_level;
}

void logger::error(const char * fmt, ...) const
{
	va_list ap;
	va_start(ap, fmt);
	log(level::ERROR, fmt, ap);
	va_end(ap);
}

void logger::error(const std::string & msg) const
{
	log(level::ERROR, msg);
}

logger * logger::getChild(const std::string & suffix) const
{
	//FIXME: Implement
	return NULL;
}

void logger::handle(const log_record & rec) const
{
	std::list<log_handler *>::const_iterator it = m_handlers.begin();
	for ( ; it != m_handlers.end(); it++)
		if (isEnabledFor(rec.level()))
			(* it)->handle(rec);

	if (m_propagate && (m_parent != NULL))
		m_parent->handle(rec);
}

bool logger::hasHandler(log_handler * handler) const
{
	std::list<log_handler *>::const_iterator it = m_handlers.begin();
	for ( ; it != m_handlers.end(); it++)
	{
		if ((* it) == handler)
			return true;
	}
	return false;
}

void logger::info(const char * fmt, ...) const
{
	va_list ap;
	va_start(ap, fmt);
	log(level::INFO, fmt, ap);
	va_end(ap);
}

void logger::info(const std::string & msg) const
{
	log(level::INFO, msg);
}

bool logger::isEnabledFor(int level) const
{
	return (effective_level() <= level);
}

int logger::level() const
{
	return m_level;
}

void logger::log(int lvl, const char * fmt, ...) const
{
	va_list ap;
	va_start(ap, fmt);
	log(lvl, fmt, ap);
	va_end(ap);
}

#define FMT_BUFFER_LENGTH 102400
void logger::log(int lvl, const char * fmt, va_list args) const
{
	std::string msg;
	char * buf = (char *)malloc(FMT_BUFFER_LENGTH);

	vsprintf(buf, fmt, args);
	msg.assign(buf);
	free(buf);

	log(lvl, msg);
}

void logger::log(int lvl, const std::string & msg) const
{
	handle(log_record(m_name, lvl, msg));
}

const std::string & logger::name() const
{
	return m_name;
}

bool logger::propagate() const
{
	return m_propagate;
}

void logger::removeHandler(log_handler * handler)
{
	if (hasHandler(handler))
		m_handlers.remove(handler);
}

void logger::setLevel(int lvl)
{
	m_level = lvl;
}

void logger::setPropagate(bool value)
{
	m_propagate = value;
}

void logger::warning(const char * fmt, ...) const
{
	va_list ap;
	va_start(ap, fmt);
	log(level::WARNING, fmt, ap);
	va_end(ap);
}

void logger::warning(const std::string & msg) const
{
	log(level::WARNING, msg);
}
