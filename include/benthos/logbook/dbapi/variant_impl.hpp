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

#ifndef DBAPI_VARIANT_IMPL_HPP_
#define DBAPI_VARIANT_IMPL_HPP_

using namespace benthos::logbook::dbapi;

template <typename T>
variant::variant(const T & v)
	: m_value(v)
{
}

template <typename T>
variant::variant(const boost::optional<T> & v)
	: m_value(v)
{
}

template <typename T>
variant & variant::operator= (const T & v)
{
	if (! m_value.is_initialized())
		m_value = sqlite_value(v);
	else
		m_value.get() = v;

	return * this;
}

template <typename T>
struct cast_visitor: public boost::static_visitor<T>
{
	template <typename U>
	T operator() (const U & v) const
	{
		return convert<T, U>()(v);
	}
};

template <typename T>
T variant::as() const
{
	if (! m_value.is_initialized())
		return T();
	return boost::apply_visitor(cast_visitor<T>(), m_value.get());
}

template <typename T>
T variant::get() const
{
	if (! m_value)
		throw std::runtime_error("Value is null");
	return boost::get<T>(m_value.get());
}

template <typename T>
boost::optional<T> variant::get_optional() const
{
	if (! m_value)
		return boost::optional<T>();
	return boost::optional<T>(boost::get<T>(m_value.get()));
}

template <typename T, typename U>
struct is_class
{
	enum { value = false };
};

template <typename T>
struct is_class<T, T>
{
	enum { value = true };
};

template <typename T>
struct check_is_visitor : public boost::static_visitor<bool>
{
	template <typename U>
	bool operator() (const U & v) const
	{
		return is_class<T, U>::value;
	}
};

template <typename T>
bool variant::is() const
{
	return m_value.is_initialized() && boost::apply_visitor(check_is_visitor<T>(), m_value.get());
}

#endif /* DBAPI_VARIANT_IMPL_HPP_ */
