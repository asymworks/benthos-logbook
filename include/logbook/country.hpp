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

#ifndef LOGBOOK_COUNTRY_HPP_
#define LOGBOOK_COUNTRY_HPP_

/**
 * @file logbook/include/logbook/dive_site.hpp
 * @brief Dive Site Domain Model Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <iostream>
#include <string>

namespace logbook {

/**
 * @brief Country Class
 * @see countries.hpp
 *
 * Encapsulates an ISO3166 Country indexed by its code, with getters for the
 * various naming types supported by liblogbook.  Note that Country Names and
 * Official Names are encoded in UTF-8 internally which should work for most
 * applications.
 */
class country
{
public:

	//! Class Constructor
	country();

	//! Class Constructor
	country(const std::string & code);

	//! Copy Constructor
	country(const country & c);

	//! Class Destructor
	~country();

	//! Country Assignment Operator
	country & operator= (const country & c);

	//! STL Code Assignment Operator
	country & operator= (const std::string & code);

	//! C Code Assignment Operator
	country & operator= (const char * code);

public:

	//! @return ISO3166 Country Code
	const std::string & code() const;

	//! @return Formatted Country Name (UTF-8)
	std::string name() const;

	//! @return Capitalized Official Country Name (UTF-8)
	std::string official_name() const;

private:
	std::string 		m_code;

};

//! @brief Country Equality Test
inline bool operator== (country & lhs, country & rhs) { return lhs.code() == rhs.code(); }

//! @brief Country Less-Than Test (required for membership in ordered lists)
inline bool operator< (country & lhs, country & rhs) { return lhs.code() < rhs.code(); }

} /* logbook */

#endif /* LOGBOOK_COUNTRY_HPP_ */
