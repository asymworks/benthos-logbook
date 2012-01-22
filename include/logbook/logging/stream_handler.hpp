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

#ifndef LOGGING_STREAM_HANDLER_HPP_
#define LOGGING_STREAM_HANDLER_HPP_

/**
 * @file include/logbook/logging/stream_handler.hpp
 * @brief Log Stream Handler Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <ctime>
#include <iostream>
#include <string>

#include <logbook/logging/handler.hpp>

namespace logbook { namespace logging {

/**
 * @brief Stream Log Handler Class
 *
 * Log Handler class which wraps std::ostream to send log messages to any
 * standard output stream object (e.g. std::cerr, std::filestream, etc).
 */
class stream_handler: public log_handler
{
public:

	/**
	 * @brief Class Constructor
	 * @param[in] Output Stream
	 * @param[in] Initial Handler Level
	 *
	 * Creates the Log Handler with the initial logging level.
	 */
	stream_handler(std::ostream & stream, int level = level::NOTSET);

	//! @brief Class Destructor
	virtual ~stream_handler();

	//! @brief Ensure all logging output has been flushed
	virtual void flush();

	/**
	 * @brief Emit a Log Record
	 * @param[in] Log Record
	 *
	 * Emits the log record to the output stream using the format() method.
	 */
	virtual void emit(const log_record & rec);

private:
	std::ostream &		m_stream;

};

} } /* logbook::logging */

#endif /* LOGGING_STREAM_HANDLER_HPP_ */
