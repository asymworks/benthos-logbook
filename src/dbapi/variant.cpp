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

#include "logbook/dbapi/variant.hpp"

using namespace logbook::dbapi;

variant::variant()
	: m_value()
{
}

variant::variant(const variant & v)
	: m_value(v.m_value)
{
}

variant::~variant()
{
}

variant & variant::operator= (const variant & v)
{
	m_value = v.m_value;
	return * this;
}

void variant::clear()
{
	m_value = boost::none;
}

bool variant::is_null() const
{
	return ! m_value.is_initialized();
}

const sqlite_value & variant::value() const
{
	if (! m_value.is_initialized())
		throw std::runtime_error("Value is null");
	return m_value.get();
}

struct printer: boost::static_visitor<std::ostream &>
{
	std::ostream & os;

	printer(std::ostream & s)
		: os(s)
	{
	}

	std::ostream & operator() (int v) const
	{
		os << v;
		return os;
	}

	std::ostream & operator() (int64_t v) const
	{
		os << v;
		return os;
	}

	std::ostream & operator() (double v) const
	{
		os << v;
		return os;
	}

	std::ostream & operator() (const std::string & s) const
	{
		os << s;
		return os;
	}

	std::ostream & operator() (const std::vector<unsigned char> & v) const
	{
		os << "BLOB(len: " << v.size() << ")";
		return os;
	}
};

std::ostream & logbook::dbapi::operator<< (std::ostream & out, const variant & v)
{
	if (v.is_null())
	{
		out << "variant(null)";
		return out;
	}
	else
	{
		sqlite_value val(v.value());
		out << "variant(";
		boost::apply_visitor(printer(out), val);
		return (out << ")");
	}
}
