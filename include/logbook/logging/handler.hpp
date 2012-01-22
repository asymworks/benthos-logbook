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

#ifndef LOGGING_LOG_HANDLER_HPP_
#define LOGGING_LOG_HANDLER_HPP_

/**
 * @file include/logbook/logging/log_handler.hpp
 * @brief Log Record Handler Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <ctime>
#include <list>
#include <string>

#include <logbook/logging/mutex.hpp>
#include <logbook/logging/record.hpp>

namespace logbook { namespace logging {

// Forward Definition of log_filter and log_formatter
struct log_filter;
struct formatter;

/**
 * @brief Log Handler Base Class
 *
 * Log Handlers are the classes which send log records to the underlying logging
 * facility (e.g. a stream, syslog, etc.).  This abstract base class may not be
 * instantiated but it includes basic functionality useful for concrete child
 * classes.
 *
 * The default class includes a mutex which is used to control access to the
 * underlying logging facility.  Descendent classes which do not need mutex
 * may override acquire() and release() to no-ops to avoid using the built-in
 * mutex object.
 */
class log_handler
{
public:

	/**
	 * @brief Class Constructor
	 * @param[in] Initial Handler Level
	 *
	 * Creates the Log Handler with the initial logging level.
	 */
	log_handler(int level = level::NOTSET);

	//! @brief Class Destructor
	virtual ~log_handler();

	//! @brief Acquire the Handler Mutex
	virtual void acquire();

	/**
	 * @brief Add a Filter to the Log Handler
	 * @param[in] Filter Pointer
	 *
	 * Adds a Log Record filter to the Handler.  When a log record is passed
	 * to the Log Handler, all registered Log Filters will be applied to the
	 * record.  If any filter returns false, the message will be rejected.
	 *
	 * The caller is responsible for ensuring that the filter pointer is
	 * valid for the entire lifetime of the log handler.  Typically this is
	 * done by statically allocating the filter in the global scope so that
	 * it remains valid for the entire program lifetime.
	 *
	 * Filters which may not remain valid for the entire lifetime of the logger
	 * must be removed by calling removeFilter() prior to becoming invalid.
	 */
	void addFilter(log_filter * filter);

	//! @brief Close the Logging Handler
	virtual void close();

	/**
	 * @brief Emit a Log Record
	 * @param[in] Log Record
	 *
	 * Abstract method which must be overridden by subclasses to format and
	 * send the log message to the underlying logging facility.  Errors in
	 * the emit() function should be raised as exceptions which subclass
	 * std::exception to ensure handleError is called.
	 */
	virtual void emit(const log_record & rec) = 0;

	/**
	 * @brief Apply Filters to the Log Record
	 * @param[in] Log Record
	 * @return Filter Result
	 *
	 * Applies all registered filters sequentially to the given log record.  If
	 * any filter returns false, the return value is false; otherwise, the
	 * return value is true.
	 */
	bool filter(const log_record & rec) const;

	//! @brief Ensure all logging output has been flushed
	virtual void flush();

	/**
	 * @brief Format a Log Record
	 * @param[in] Log Record
	 * @return Formatted Log Message
	 *
	 * Formats the log record according to the internal formatter (if set) or
	 * the default formatting for the module.
	 */
	std::string format(const log_record & rec) const;

	/**
	 * @brief Handle a Log Record
	 * @param[in] Log Record
	 *
	 * Conditionally emits the given log record, depending on the current
	 * logging level.  Calls acquire(), emit(), and release().
	 */
	virtual void handle(const log_record & rec);

	/**
	 * @brief Handle an Error in the Logging Subsystem
	 * @param[in] Log Record
	 *
	 * This method should be called from handlers when an exception is
	 * encountered in the handle() method.  The default implementation does
	 * nothing (errors are silently ignored).  This is a good approach - users
	 * probably don't care about errors in the logger itself.  The specified
	 * log record is the one that was being processed when the error occurred.
	 *
	 * Note that the mutex is still locked while handleError() is called.
	 */
	virtual void handleError(const log_record & rec);

	//! @return Logging Level for this Handler
	int level() const;

	//! @brief Release the Handler Mutex
	virtual void release();

	/**
	 * @brief Remove a Filter from the Log Handler
	 * @param[in] Filter Pointer
	 *
	 * Removes a filter from the logger.  Following this call the filter may
	 * be destroyed if it is no longer referenced by any logger.
	 */
	void removeFilter(log_filter * filter);

	/**
	 * @brief Set the Logging Formatter for this Handler
	 * @param[in] Pointer to a Log Formatter
	 *
	 * Sets the formatter object for this log handler.  After passing the
	 * pointer, the log handler owns the formatter and will delete the pointer
	 * when it is no longer needed.
	 *
	 * Passing NULL to this method will cause the handler to use the default
	 * formatting for the module.
	 */
	void setFormatter(formatter * form);

	/**
	 * @brief Set the Logging Level for this Handler
	 * @param[in] Logging Level
	 *
	 * Sets the logging threshold for this handler to lvl.  Logging messages
	 * which are less severe than lvl will be ignored.  When a new handler is
	 * created, the level is set to level.NOTSET (which causes all messages to
	 * be processed).
	 */
	void setLevel(int lvl);

protected:

	/**
	 * @brief Check if a Filter is registered with this Handler
	 * @param[in] Filter Pointer
	 * @return True if the Filter is registered
	 */
	bool hasFilter(log_filter * filter) const;

private:
	int						m_level;
	formatter *				m_formatter;
	mutex					m_mutex;
	std::list<log_filter *>	m_filters;

};

} } /* logbook::logging */

#endif /* LOGGING_LOG_HANDLER_HPP_ */
