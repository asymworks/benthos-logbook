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

#ifndef DBAPI_VARIANT_HPP_
#define DBAPI_VARIANT_HPP_

/**
 * @file include/benthos/logbook/dbapi/variant.hpp
 * @brief Variant Value Type
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 *
 * This class implements a mini-variant type which wraps a SQLite value type.
 * SQLite supports integer, float, text, and blob types natively; this class
 * provides support for all four along with conversion convenience functions.
 *
 * This class employs boost::optional and boost::variant as the underlying
 * value holder.  Note that strings may be either ASCII or UTF-8 formatted;
 * UTF-16 is not supported by sqlitekit.
 */

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace benthos { namespace logbook { namespace dbapi {

/**
 * @brief SQLite Value Variant
 *
 * Value holder type which can hold either an integer (or int64), floating
 * point, string, or binary blob type.  Due to the "never empty" guarantee of
 * boost::variant, this class will always contain a value (cannot be null).
 */
typedef boost::variant<int, int64_t, double, std::string, std::vector<unsigned char> > sqlite_value;

/**
 * @brief SQLite Optional Value Variant
 *
 * Wrapper class around sqlite_value which allows the value to be NULL.
 */
typedef boost::optional<sqlite_value> sqlite_variant;

/**
 * @brief SQLiteKit Variant Class
 *
 * Wrapper class around sqlite_variant to provide convenience conversion
 * methods and assignment/get operators.
 */
class variant
{
public:

	//! Default Constructor
	variant();

	//! Copy Constructor
	variant(const variant & v);

	//! Initializer Constructor
	template <typename T>
	explicit variant(const T & v);

	//! Initializer Constructor
	template <typename T>
	explicit variant(const boost::optional<T> & v);

	//! Class Destructor
	~variant();

	//! Assignment Operator
	variant & operator= (const variant & v);

	//! Templated Assignment Operator
	template <typename T>
	variant & operator= (const T & v);

public:

	/**
	 * @brief Cast the Variant to the specified type
	 * @return Typecast Result
	 *
	 * Throws a std::runtime_error if the typecast is not possible.  Note that
	 * conversions to string and blob from floating point may result in long
	 * strings such as '12.30000000000000001'.
	 */
	template <typename T>
	T as() const;

	//! Set the Variant to a NULL (empty) value
	void clear();

	/**
	 * @brief Get the Variant Value
	 * @return Variant Value
	 *
	 * Throws a std::runtime_error if the value is null.  Throws boost::bad_get
	 * if the type is not correct.
	 */
	template <typename T>
	T get() const;

	/**
	 * @brief Get the Variant Value
	 * @return Optional Variant Value
	 *
	 * Throws boost::bad_get if the type is not correct.
	 */
	template <typename T>
	boost::optional<T> get_optional() const;

	//! @return Check whether the Type is the given type
	template <typename T>
	bool is() const;

	//! @return Check whether the Type is null
	bool is_null() const;

	//! @return Underlying Value
	const sqlite_value & value() const;

public:
	friend std::ostream & operator<< (std::ostream & out, const variant & v);

private:
	sqlite_variant		m_value;

};

/**
 * @brief Print a Variant to an Output Stream
 * @param[in] Output Stream
 * @param[in] Variant
 * @return Output Stream
 */
std::ostream & operator<< (std::ostream & out, const variant & v);

} } } /* benthos::logbook::dbapi */

#include "variant_conv.hpp"
#include "variant_impl.hpp"

#endif /* DBAPI_VARIANT_HPP_ */
