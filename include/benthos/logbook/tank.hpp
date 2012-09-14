/*
 * Copyright (C) 2012 Asymworks, LLC.  All Rights Reserved.
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

#ifndef LOGBOOK_TANK_HPP_
#define LOGBOOK_TANK_HPP_

/**
 * @file include/benthos/logbook/tank.hpp
 * @brief Tank Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/persistent.hpp>

namespace benthos { namespace logbook {

/**
 * @brief Scuba Tank Model Class
 *
 * Represents a scuba tank used during a dive to hold breathing gas.  The tank
 * properties including composition (steel, aluminum), working pressure, water
 * volume and gas capacity.
 */
class Tank: public TypedPersistent<Tank>
{
public:
	typedef boost::shared_ptr<Tank>	Ptr;
	typedef boost::weak_ptr<Tank>	WeakPtr;

public:

	//! Class Constructor
	Tank();

	//! Class Destructor
	virtual ~Tank();

public:

	//! @return Tank Capacity [liters]
	double capacity() const;

	//! @return Tank Name
	const boost::optional<std::string> & name() const;

	//! @return Tank Working Pressure [bar]
	double pressure() const;

	//! @return Tank Type ("aluminum" or "steel")
	const boost::optional<std::string> & type() const;

	//! @return Tank Water Volume [liters]
	double volume() const;

	//! @param[in] Capacity in liters
	void setCapacity(double value);

	//! @brief Set the Tank Name to NULL
	void setName(const boost::none_t &);

	//! @param[in] Tank Name
	void setName(const std::string & value);

	//! @param[in] Working Pressure in bars
	void setPressure(double value);

	//! @brief Set the Tank Type to NULL
	void setType(const boost::none_t &);

	//! @param[in] Tank Type ("aluminum" or "steel")
	void setType(const std::string & value);

	//! @param[in] Water Volume in liters
	void setVolume(double value);

private:
	boost::optional<std::string>	m_name;		///< Tank Name
	boost::optional<std::string>	m_type;		///< Tank Type ("aluminum" or "steel")
	double							m_pressure;	///< Working Pressure [bar]
	double							m_volume;	///< Water Volume [liters]

};

/**
 * @brief Scuba Tank Finder Interface
 *
 * Extends IFinder<Tank> to add a find-by-name method.
 */
struct ITankFinder: public IFinder<Tank>
{
	typedef boost::shared_ptr<ITankFinder>	Ptr;
	virtual ~ITankFinder() { }

	/**
	 * @brief Find a Breathing Mix by Name (case insensitive)
	 * @param[in] Mix Name
	 * @return Breathing Mix
	 */
	virtual Tank::Ptr findByName(const std::string & name) = 0;

};

} } /* benthos::logbook */

#endif /* LOGBOOK_TANK_HPP_ */
