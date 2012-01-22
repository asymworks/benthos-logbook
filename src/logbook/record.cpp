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

#include <cstdio>
#include <pthread.h>
#include <unistd.h>

#include "logbook/logging/record.hpp"

using namespace logbook::logging;

const int level::NOTSET = -1;
const int level::DEBUG = 0;
const int level::INFO = 1;
const int level::WARNING = 2;
const int level::ERROR = 3;
const int level::CRITICAL = 4;

log_record::log_record(const std::string & channel, int level, const std::string & message)
	: m_channel(channel), m_level(level), m_message(message),
	  m_threadId(pthread_self()), m_processId(getpid()), m_timestamp(time(NULL))
{
}

log_record::~log_record()
{
}

std::string log_record::asctime() const
{
	char buf[100];
	strftime(buf, 100, "%F %T", localtime(& m_timestamp));
	return std::string(buf);
}

const std::string & log_record::channel() const
{
	return m_channel;
}

int log_record::level() const
{
	return m_level;
}

std::string log_record::level_name() const
{
	// Level Names
	static const char * names[] =
	{
		"NOTSET",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"CRITICAL",
	};

	if ((m_level < level::NOTSET) || (m_level > level::CRITICAL))
	{
		char buf[100];
		sprintf(buf, "UNKNOWN(%d)", m_level);
		return std::string(buf);
	}

	return std::string(names[m_level + 1]);
}

const std::string & log_record::message() const
{
	return m_message;
}

unsigned long log_record::process_id() const
{
	return m_processId;
}

unsigned long log_record::thread_id() const
{
	return m_threadId;
}

time_t log_record::timestamp() const
{
	return m_timestamp;
}
