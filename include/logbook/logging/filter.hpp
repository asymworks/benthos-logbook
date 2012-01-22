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

#ifndef LOGGING_LOG_FILTER_HPP_
#define LOGGING_LOG_FILTER_HPP_

/**
 * @file include/logbook/logging/filter.hpp
 * @brief Log Record Filter Interface
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <ctime>
#include <string>

namespace logbook { namespace logging {

// Forward Definition of log_record
class log_record;

/**
 * @brief Log Filter Interface
 *
 * Log Filters enable fine-grained control over which Log Records are passed to
 * a logging facility.
 */
struct log_filter
{
	virtual ~log_filter() { }

	/**
	 * @brief Filter Log Records
	 * @param[in] Log Record
	 * @return True to pass the Log Record; False to reject it
	 *
	 * This method allows the filter to apply arbitrary criteria to a log
	 * message to decide whether to pass it along in the logging chain.  If
	 * the result is False, the log message will not be logged; if it is
	 * True, then processing will continue.
	 */
	virtual bool apply(const log_record & record) const = 0;

};

} } /* logbook::logging */

#endif /* LOGGING_LOG_FILTER_HPP_ */
