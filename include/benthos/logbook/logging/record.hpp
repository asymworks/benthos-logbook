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

#ifndef LOGGING_LOG_RECORD_HPP_
#define LOGGING_LOG_RECORD_HPP_

/**
 * @file include/benthos/logbook/logging/record.hpp
 * @brief Log Record Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <ctime>
#include <string>

namespace logbook { namespace logging {

/**@{
 * @name Log Level Constants
 *
 * Defines constants for the basic five Log Levels:
 * - DEBUG: Used for low-level debug outputs; very verbose
 * - INFO: Used for information related to normal operation; somewhat verbose
 * - WARNING: Used to communicate non-fatal conditions; default level
 * - ERROR: Used for recoverable errors which impact program operations
 * - CRITICAL: Used for non-recoverable errors which will terminate the program
 * - NOTSET: Default level for log messages which do not have a level assigned
 */
struct level
{
	static const int DEBUG;		///< Debug Level
	static const int INFO;		///< Information Level
	static const int WARNING;	///< Warning Level
	static const int ERROR;		///< Error Level
	static const int CRITICAL;	///< Critical Level
	static const int NOTSET;	///< Level Not Set
};
/*@}*/

/**
 * @brief Log Record Class
 *
 * Encapsulates all information about a log message including the logger
 * channel, the log message itself, the level, the timestamp, and the thread
 * id.  This class represents a read-only value holder object and cannot be
 * modified after construction.
 */
class log_record
{
public:

	/**
	 * @brief Class Constructor
	 * @param[in] Logger Channel
	 * @param[in] Log Level
	 * @param[in] Log Message
	 * @param[in] Thread Id
	 * @param[in] Timestamp
	 *
	 * The Thread Id and Timestamp are populated automatically by the
	 * constructor.
	 */
	log_record(const std::string & channel, int level, const std::string & message);

	//! @brief Class Destructor
	~log_record();

	//! @return Formatted Time Stamp
	std::string asctime() const;

	//! @return Logger Channel
	const std::string & channel() const;

	//! @return Log Level
	int level() const;

	//! @return Log Level Name
	std::string level_name() const;

	//! @return Log Message
	const std::string & message() const;

	//! @return Process Id
	unsigned long process_id() const;

	//! @return Thread Id
	unsigned long thread_id() const;

	//! @return Timestamp
	time_t timestamp() const;

private:
	std::string			m_channel;
	int					m_level;
	std::string			m_message;
	unsigned long		m_threadId;
	unsigned long		m_processId;
	time_t				m_timestamp;

};

} } /* logbook::logging */

#endif /* LOGGING_LOG_RECORD_HPP_ */
