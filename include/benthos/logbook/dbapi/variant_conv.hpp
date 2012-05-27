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

#ifndef DBAPI_VARIANT_CONV_HPP_
#define DBAPI_VARIANT_CONV_HPP_

#include <boost/lexical_cast.hpp>
#include <climits>

using namespace benthos::logbook::dbapi;

// Catch-all converter: throws runtime_error
template <typename T, typename U>
struct convert
{
	T operator() (const U & u) const
	{
		throw std::runtime_error(std::string("Cannot convert from ") + typeid(U).name() + " to " + typeid(T).name());
	}
};

// Identity converter
template <typename T>
struct convert<T, T>
{
	T operator() (const T & t) const
	{
		return t;
	}
};

// INTEGER -> INT64 converter
template <>
struct convert<int64_t, int>
{
	int64_t operator() (const int & v) const
	{
		return v;
	}
};

// INTEGER -> FLOAT converter
template <>
struct convert<double, int>
{
	double operator() (const int & v) const
	{
		return v;
	}
};

// INTEGER -> STRING converter
template <>
struct convert<std::string, int>
{
	std::string operator() (const int & v) const
	{
		return boost::lexical_cast<std::string>(v);
	}
};

// INTEGER -> BLOB converter
template <>
struct convert<std::vector<unsigned char>, int>
{
	std::vector<unsigned char> operator() (const int & v) const
	{
		std::string s = boost::lexical_cast<std::string>(v);
		return std::vector<unsigned char>(s.begin(), s.end());
	}
};

// INT64 -> INTEGER converter
template <>
struct convert<int, int64_t>
{
	int operator() (const int64_t & v) const
	{
		if ((v < INT_MIN) || (v > INT_MAX))
			throw std::out_of_range("Value %ll is out of range for int" + boost::lexical_cast<std::string>(v));
		return v;
	}
};

// INT64 -> FLOAT converter
template <>
struct convert<double, int64_t>
{
	double operator() (const int64_t & v) const
	{
		return v;
	}
};

// INT64 -> STRING converter
template <>
struct convert<std::string, int64_t>
{
	std::string operator() (const int64_t & v) const
	{
		return boost::lexical_cast<std::string>(v);
	}
};

// INT64 -> BLOB converter
template <>
struct convert<std::vector<unsigned char>, int64_t>
{
	std::vector<unsigned char> operator() (const int64_t & v) const
	{
		std::string s = boost::lexical_cast<std::string>(v);
		return std::vector<unsigned char>(s.begin(), s.end());
	}
};

// FLOAT -> INTEGER converter
template <>
struct convert<int, double>
{
	int operator() (const double & v) const
	{
		return v;
	}
};

// FLOAT -> INT64 converter
template <>
struct convert<int64_t, double>
{
	int64_t operator() (const double & v) const
	{
		return v;
	}
};

// FLOAT -> STRING converter
template <>
struct convert<std::string, double>
{
	std::string operator() (const double & v) const
	{
		return boost::lexical_cast<std::string>(v);
	}
};

// FLOAT -> BLOB converter
template <>
struct convert<std::vector<unsigned char>, double>
{
	std::vector<unsigned char> operator() (const double & v) const
	{
		std::string s = boost::lexical_cast<std::string>(v);
		return std::vector<unsigned char>(s.begin(), s.end());
	}
};

// STRING -> INTEGER converter
template <>
struct convert<int, std::string>
{
	int operator() (const std::string & v) const
	{
		int64_t i = boost::lexical_cast<int64_t>(v);
		if ((i < INT_MIN) || (i > INT_MAX))
			throw std::out_of_range("Value %ll is out of range for int" + boost::lexical_cast<int64_t>(i));
		return i;
	}
};

// STRING -> INT64 converter
template <>
struct convert<int64_t, std::string>
{
	int64_t operator() (const std::string & v) const
	{
		return boost::lexical_cast<int64_t>(v);
	}
};

// STRING -> FLOAT converter
template <>
struct convert<double, std::string>
{
	double operator() (const std::string & v) const
	{
		return boost::lexical_cast<double>(v);
	}
};

// STRING -> BLOB converter
template <>
struct convert<std::vector<unsigned char>, std::string>
{
	std::vector<unsigned char> operator() (const std::string & v) const
	{
		return std::vector<unsigned char>(v.begin(), v.end());
	}
};

// BLOB -> STRING converter
template <>
struct convert<std::string, std::vector<unsigned char> >
{
	std::string operator() (const std::vector<unsigned char> & v) const
	{
		return std::string(v.begin(), v.end());
	}
};

#endif /* DBAPI_VARIANT_CONV_HPP_ */
