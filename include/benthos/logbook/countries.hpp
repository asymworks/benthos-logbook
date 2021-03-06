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

#ifndef LOGBOOK_COUNTRIES_HPP_
#define LOGBOOK_COUNTRIES_HPP_

/**
 * @file include/benthos/logbook/country.hpp
 * @brief ISO 3166 Country Support
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <map>
#include <string>

//! Country Code Mapping Type
typedef std::map<std::string, std::string>		country_map_t;

//! Nicely titled (and translatable) country names.
extern const country_map_t & ISO3166_COUNTRIES;

/**
 * Nicely titled country names with duplicates for those which contain a comma
 * (containing the non-comma'd version).
 */
extern const country_map_t & ISO3166_COUNTRIES_PLUS;

//! Official capitalized country names.
extern const country_map_t & ISO3166_OFFICIAL_COUNTRIES;

#endif /* LOGBOOK_COUNTRIES_HPP_ */
