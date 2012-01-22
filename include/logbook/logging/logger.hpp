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

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

/**
 * @file include/logbook/logging/logger.hpp
 * @brief Logger Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdarg>
#include <ctime>
#include <list>
#include <string>

namespace logbook { namespace logging {

// Forward Definition of log_handler and log_record
class log_handler;
class log_record;

/**
 * @brief Logger Class
 *
 * The Logger class exposes the interface directly used by applications to
 * access logging functionality.  Loggers are never created directly; but are
 * accessed by calling logging::getLogger() and passing the requested logger
 * name.
 *
 * Loggers use a hierarchical dotted-path naming convention, allowing nested
 * loggers with separate handlers and logging priority at each level.
 */
class logger
{
public:

	logger()=delete;
	logger & operator=(const logger &)=delete;

protected:

	/**
	 * @brief Class Constructor
	 * @param[in] Parent Logger
	 * @param[in] Dotted-Path Name of the Logger
	 *
	 * Loggers should not be constructed directly by the client; instead they
	 * should be accessed with logging::getLogger().
	 */
	logger(logger * parent, const std::string & name);

public:

	//! Class Destructor
	virtual ~logger();

	/**
	 * @brief Add a Handler to the Logger
	 * @param[in] Handler Pointer
	 *
	 * Adds a handler to the logger.  The caller is responsible for ensuring
	 * that the handler pointer is valid for the entire lifetime of the logger.
	 * Typically this is done by statically allocating the handler in the global
	 * scope so that it remains valid for the entire program lifetime.
	 *
	 * Handlers which may not remain valid for the entire lifetime of the logger
	 * must be removed by calling removeHandler() prior to becoming invalid.
	 */
	void addHandler(log_handler * handler);

	/**
	 * @brief Log a Critical Message
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void critical(const char * fmt, ...) const;

	/**
	 * @brief Log a Critical Message
	 * @param[in] Log Message
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void critical(const std::string & msg) const;

	/**
	 * @brief Log a Debug Message
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void debug(const char * fmt, ...) const;

	/**
	 * @brief Log a Debug Message
	 * @param[in] Log Message
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void debug(const std::string & msg) const;

	//! @return Effective Logger Level
	int effective_level() const;

	/**
	 * @brief Log an Error Message
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void error(const char * fmt, ...) const;

	/**
	 * @brief Log an Error Message
	 * @param[in] Log Message
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void error(const std::string & msg) const;

	/**
	 * @brief Get a Child Logger
	 * @param[in] Child Logger Suffix
	 * @return Logger Pointer
	 *
	 * Returns a pointer to a child logger.  The given logger name is only
	 * the suffix, so calling
	 * @code logging::getLogger('abc')->getChild('def.ghi') @endcode
	 * is equivalent to calling
	 * @code logging::getLogger('abc.def.ghi') @endcode
	 */
	logger * getChild(const std::string & suffix) const;

	/**
	 * @brief Handle a Log Record
	 * @param[in] Log Record
	 *
	 * Handle a log record by passing it to all handlers of this logger and its
	 * ancestors until propagate() is false.
	 */
	void handle(const log_record & rec) const;

	/**
	 * @brief Log an Info Message
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void info(const char * fmt, ...) const;

	/**
	 * @brief Log an Info Message
	 * @param[in] Log Message
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void info(const std::string & msg) const;

	/**
	 * @brief Check if the Logger is enabled for the given Level
	 * @param[in] Log Level
	 * @return True if the Level will be processed
	 *
	 * Checks whether the given level is at least as severe as the effective
	 * log level threshold.
	 */
	bool isEnabledFor(int level) const;

	//! @return Local Logger Level
	int level() const;

	/**
	 * @brief Log a Message
	 * @param[in] Log Level
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void log(int lvl, const char * fmt, ...) const;

	/**
	 * @brief Log a Message
	 * @param[in] Log Level
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void log(int lvl, const char * fmt, va_list args) const;

	/**
	 * @brief Log a Message
	 * @param[in] Log Level
	 * @param[in] Log Message
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void log(int lvl, const std::string & msg) const;

	//! @return Logger Name
	const std::string & name() const;

	//! @return True if the Logger propagates messages to its parent
	bool propagate() const;

	/**
	 * @brief Remove a Handler from the Logger
	 * @param[in] Handler Pointer
	 *
	 * Removes a handler from the logger.  Following this call the logger may
	 * be destroyed if it is no longer referenced by any logger.
	 */
	void removeHandler(log_handler * handler);

	/**
	 * @brief Set the Level of this Logger
	 * @param[in] New Log Level
	 *
	 * Sets the threshold for this logger to lvl.  Logging messages less severe
	 * than lvl are ignored.  When a logger is created, the level is set to
	 * NOTSET, which causes all messages to be processed when the logger is the
	 * root logger, or delegated to the parent if the logger is a non-root
	 * logger.  The root logger is created with the default level WARNING.
	 *
	 * The term 'delegation' means that if a logger has a priority of NOTSET,
	 * its chain of ancestors is traversed until an ancestor with a level other
	 * than NOTSET, or the root logger is found.
	 */
	void setLevel(int lvl);

	/**
	 * @brief Set whether the Logger propagates messages to its parent
	 * @param[in] Set to False to disable propagation
	 *
	 * If the propagate() property evaluates to true, log messages will be
	 * passed to all parent loggers through the root logger.  The default is
	 * to propagate all log messages.
	 */
	void setPropagate(bool value);

	/**
	 * @brief Log a Warning Message
	 * @param[in] Log Message
	 * @param[in] Message Formatting Arguments
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void warning(const char * fmt, ...) const;

	/**
	 * @brief Log a Warning Message
	 * @param[in] Log Message
	 *
	 * Constructs a Log Record from the given arguments and passes it to the
	 * handle() method.
	 */
	void warning(const std::string & msg) const;

protected:

	/**
	 * @brief Check if a Handler is registered with this Logger
	 * @param[in] Handler Pointer
	 * @return True if the Handler is registered
	 */
	bool hasHandler(log_handler * handler) const;

protected:
	logger *					m_parent;
	std::string					m_name;
	int							m_level;
	bool						m_propagate;
	std::list<log_handler *>	m_handlers;


};

} } /* logbook::logging */

#endif /* LOGGER_HPP_ */
