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

#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "benthos/logbook/country.hpp"
#include "benthos/logbook/countries.hpp"

using namespace logbook;

country::country()
	: m_code()
{
}

country::country(const std::string & code)
	: m_code(boost::algorithm::to_upper_copy(code))
{
	// Ensure the Code is Valid
	if (m_code.size() != 2)
		throw std::invalid_argument("Country Code must be a two-character string");
}

country::country(const country & c)
	: m_code(c.m_code)
{
}

country::~country()
{
}

country & country::operator= (const country & c)
{
	m_code = c.m_code;
	return * this;
}

country & country::operator= (const std::string & code)
{
	if (code.size() != 2)
		throw std::invalid_argument("Country Code must be a two-character string");
	m_code = boost::algorithm::to_upper_copy(code);
	return * this;
}

country & country::operator= (const char * code)
{
	return operator=(std::string(code));
}

const std::string & country::code() const
{
	return m_code;
}

std::string country::name() const
{
	country_map_t::const_iterator it = ISO3166_COUNTRIES.find(m_code);
	if (it == ISO3166_COUNTRIES.end())
		return std::string();
	return it->second;
}

std::string country::official_name() const
{
	country_map_t::const_iterator it = ISO3166_OFFICIAL_COUNTRIES.find(m_code);
	if (it == ISO3166_COUNTRIES.end())
		return std::string();
	return it->second;
}
