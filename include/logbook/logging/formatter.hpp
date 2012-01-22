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

#ifndef LOGGING_FORMATTER_HPP_
#define LOGGING_FORMATTER_HPP_

/**
 * @file include/logbook/logging/formatter.hpp
 * @brief Log Record Formatter Interface
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <ctime>
#include <string>

namespace logbook { namespace logging {

// Forward Definition of log_record
class log_record;

/**
 * @brief Log Formatter Interface
 *
 * Log Formatters create a string representation of Log Records prior to being
 * passed to a logging facility.
 */
struct formatter
{
	virtual ~formatter() { }

	/**
	 * @brief Format a Log Record
	 * @param[in] Log Record
	 *
	 * Format the log record according to the internal formatting rules and
	 * return the resulting string.
	 *
	 * @internal Implemented in log_handler.cpp
	 */
	virtual std::string format(const log_record & record) const = 0;

};

/**
 * @brief Default Log Formatter
 *
 * Default Log Formatter which can be used for subclassing.
 */
struct default_formatter: public formatter
{
	virtual ~default_formatter() { }

	/**
	 * @brief Format a Log Record
	 * @param[in] Log Record
	 *
	 * Format the log record according to the internal formatting rules and
	 * return the resulting string.
	 */
	virtual std::string format(const log_record & record) const;

};

} } /* logbook::logging */

#endif /* LOGGING_FORMATTER_HPP_ */
