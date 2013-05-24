/*
 * util.hpp
 *
 *  Created on: Sep 14, 2012
 *      Author: lowsnr
 */

#ifndef LOGBOOK_UTIL_HPP_
#define LOGBOOK_UTIL_HPP_

/**
 * @file include/benthos/logbook/util.hpp
 * @brief Logbook Utility and Helper Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstring>
#include <string>

namespace benthos { namespace logbook {

#if defined(_WIN32) || defined(WIN32)
#define strcasecmp _stricmp
#endif

//! @brief Case-Insensitive String Compare
struct cicmp
{
	bool operator() (const std::string & lhs, const std::string & rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

} } /* benthos::logbook */

#endif /* LOGBOOK_UTIL_HPP_ */
