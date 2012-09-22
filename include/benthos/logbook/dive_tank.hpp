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

#ifndef LOGBOOK_DIVE_TANK_HPP_
#define LOGBOOK_DIVE_TANK_HPP_

/**
 * @file include/benthos/logbook/tank.hpp
 * @brief Dive-Tank Mapping Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/mix.hpp>
#include <benthos/logbook/persistent.hpp>
#include <benthos/logbook/tank.hpp>

namespace benthos { namespace logbook {

/**
 * @brief Tank Used on a Dive
 *
 * Holds information linking a dive to a tank, including the mix in the tank
 * and the start/end pressure of the tank.
 */
class DiveTank: public TypedPersistent<DiveTank>
{
public:
	typedef boost::shared_ptr<DiveTank>		Ptr;
	typedef boost::weak_ptr<DiveTank>		WeakPtr;

public:

	//! Class Constructor
	DiveTank(Dive::Ptr dive);

	//! Class Destructor
	virtual ~DiveTank();

public:

	//! @return Dive Instance
	Dive::Ptr dive() const;

	//! @return Tank Ending Pressure
	const boost::optional<double> & end_pressure() const;

	//! @return Tank Index
	int index() const;

	//! @return Breathing Mix
	Mix::Ptr mix() const;

	//! @return Tank Starting Pressure
	const boost::optional<double> & start_pressure() const;

	//! @return Tank Instance
	Tank::Ptr tank() const;

	//! @brief Set the Tank Ending Pressure to NULL
	void setEndPressure(const boost::none_t &);

	//! @param[in] Tank Ending Pressure
	void setEndPressure(double value);

	//! @param[in] Tank Index
	void setIndex(int value);

	//! @brief Set the Mix to NULL
	void setMix(const boost::none_t &);

	//! @param[in] Breathing Mix
	void setMix(Mix::Ptr value);

	//! @brief Set the Tank Starting Pressure to NULL
	void setStartPressure(const boost::none_t &);

	//! @param[in] Tank Starting Pressure
	void setStartPressure(double value);

	//! @brief Set the Tank to NULL
	void setTank(const boost::none_t &);

	//! @param[in] Tank
	void setTank(Tank::Ptr value);

public:

	//! @internal Called when a Mix is deleted
	void evtMixDeleted(AbstractMapper::Ptr, Persistent::Ptr);

	//! @internal Called when a Tank is deleted
	void evtTankDeleted(AbstractMapper::Ptr, Persistent::Ptr);

protected:

	//! Called when the Persistent is attached to a Session
	virtual void attached(SessionPtr);

	//! Called when the Persistent is detached from a Session
	virtual void detached(SessionPtr);

private:
	Dive::Ptr					m_dive;
	int							m_index;
	Tank::Ptr					m_tank;
	Mix::Ptr					m_mix;
	boost::optional<double>		m_pxstart;
	boost::optional<double>		m_pxend;

	boost::signals2::connection		m_evtTankDel;		///< Event Connection for Tank Deletion
	boost::signals2::connection		m_evtMixDel;		///< Event Connection for Mix Deletion

};

/**
 * @brief Dive Tank Finder Interface
 *
 * Extends IFinder<DiveTank> to add lookup methods for Dives.
 */
struct IDiveTankFinder: public IFinder<DiveTank>
{
	typedef boost::shared_ptr<IDiveTankFinder>	Ptr;
	virtual ~IDiveTankFinder() { }


	/**
	 * @brief Return a list of all Tanks for a Dive
	 * @param[in] Dive Id
	 * @return List of Tanks
	 */
	virtual std::vector<DiveTank::Ptr> findByDive(int64_t dive_id) = 0;

	/**
	 * @brief Return a list of all Dives on a Tank
	 * @param[in] Tank Id
	 * @return List of Tanks
	 */
	virtual std::vector<DiveTank::Ptr> findByTank(int64_t tank_id) = 0;

};

} } /* benthos::logbook */

#endif /* LOGBOOK_DIVE_TANK_HPP_ */
