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

#ifndef LOGBOOK_DIVE_HPP_
#define LOGBOOK_DIVE_HPP_

/**
 * @file include/logbook/dive.hpp
 * @brief Dive Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>
#include <ctime>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <logbook/dive_computer.hpp>
#include <logbook/dive_site.hpp>
#include <logbook/mix.hpp>
#include <logbook/persistent.hpp>

namespace logbook {

/**
 * @brief Dive Model Class
 */
class Dive: public TypedPersistent<Dive>
{
public:
	typedef boost::shared_ptr<Dive>	Ptr;
	typedef boost::weak_ptr<Dive>	WeakPtr;

public:

	//! Class Constructor
	Dive();

	//! Class Destructor
	virtual ~Dive();

private:
	/**
	 * Managed Collection Mappings:
	 * - Flags
	 * - Profiles
	 * - Equipment
	 */

public:

	//! @return Air Temperature in degrees Celsius
	const boost::optional<double> & air_temp() const;

	//! @return Average Depth in msw
	const boost::optional<double> & avg_depth() const;

	//! @return Dive Computer
	DiveComputer::Ptr computer() const;

	//! @return Dive Local Date/Time
	const boost::optional<time_t> & datetime() const;

	//! @return Dive Duration in Minutes
	int duration() const;

	//! @return Ending Tank Pressure in bar
	const boost::optional<double> end_pressure() const;

	//! @return Surface Interval in Minutes
	int interval() const;

	//! @return Maximum Depth in msw
	double max_depth() const;

	//! @return Maximum Water Temperature in degrees Celsius
	const boost::optional<double> & max_temp() const;

	//! @return Minimum Water Temperature in degrees Celsius
	const boost::optional<double> & min_temp() const;

	//! @return Breathing Mix
	Mix::Ptr mix() const;

	//! @return Dive Number
	const boost::optional<int> & number() const;

	//! @return Repetition Number
	int repetition() const;

	//! @return Dive Site
	DiveSite::Ptr site() const;

	//! @return Starting Tank Pressure in bar
	const boost::optional<double> & start_pressure() const;

	//! @return UTC Offset of the Dive Locale in Minutes
	const boost::optional<int> & utc_offset() const;

	//! @brief Set the Air Temperature to NULL
	void setAirTemp(const boost::none_t &);

	//! @param[in] Air Temperature in degrees Celsius
	void setAirTemp(double value);

	//! @brief Set the Average Depth to NULL
	void setAvgDepth(const boost::none_t &);

	//! @param[in] Average Depth
	void setAvgDepth(double value);

	//! @brief Set the Dive Computer to NULL
	void setComputer(const boost::none_t &);

	//! @param[in] Dive Computer
	void setComputer(DiveComputer::Ptr value);

	//! @brief Set the Dive Local Date/Time to NULL
	void setDateTime(const boost::none_t &);

	//! @param[in] Dive Local Date/Time
	void setDateTime(time_t value);

	//! @param[in] Dive Duration in minutes
	void setDuration(int value);

	//! @brief Set the Ending Pressure to NULL
	void setEndPressure(const boost::none_t &);

	//! @param[in] Ending Pressure in bar
	void setEndPressure(double value);

	//! @param[in] Surface Interval in minutes
	void setInterval(int value);

	//! @param[in] Maximum Depth in msw
	void setMaxDepth(double value);

	//! @brief Set the Maximum Water Temperature to NULL
	void setMaxTemp(const boost::none_t &);

	//! @param[in] Maximum Water Temperature in degrees Celsius
	void setMaxTemp(double value);

	//! @brief Set the Minimum Water Temperature to NULL
	void setMinTemp(const boost::none_t &);

	//! @param[in] Minimum Water Temperature in degrees Celsius
	void setMinTemp(double value);

	//! @brief Set the Breathing Mix to NULL
	void setMix(const boost::none_t &);

	//! @param[in] Breathing Mix
	void setMix(Mix::Ptr value);

	//! @brief Set the Dive Number to NULL
	void setNumber(const boost::none_t &);

	//! @param[in] Dive Number
	void setNumber(int value);

	//! @param[in] Repetition Number
	void setRepetition(int value);

	//! @brief Set the Dive Site to NULL
	void setSite(const boost::none_t &);

	//! @param[in] Dive Site
	void setSite(DiveSite::Ptr value);

	//! @brief Set the Starting Pressure to NULL
	void setStartPressure(const boost::none_t &);

	//! @brief Set the Starting Pressure in bar
	void setStartPressure(double value);

	//! @brief Set the UTC Offset of the Dive Locale to NULL
	void setUTCOffset(const boost::none_t &);

	//! @param[in] UTC Offset of the Dive Locale in Minutes
	void setUTCOffset(int value);

private:
	boost::optional<time_t>			m_datetime;		///< Dive Date/Time
	boost::optional<int>			m_utc_offset;	///< Dive UTC Offset
	boost::optional<int>			m_number;		///< Dive Number

	DiveSite::Ptr					m_site;			///< Dive Site
	DiveComputer::Ptr				m_computer;		///< Dive Computer

	int								m_repetition;	///< Repetitive Dive Number
	int								m_interval;		///< Surface Interval [minutes]
	int								m_duration;		///< Dive Bottom Time [seconds]
	double							m_maxdepth;		///< Dive Maximum Depth [msw]
	boost::optional<double>			m_avgdepth;		///< Dive Average Depth [msw]
	boost::optional<double>			m_airtemp;		///< Air Temperature [deg C]
	boost::optional<double>			m_maxtemp;		///< Dive Maximum Temperature [deg C]
	boost::optional<double>			m_mintemp;		///< Dive Minimum Temperature [deg C]
	boost::optional<double>			m_startpx;		///< Starting Tank Pressure [bar]
	boost::optional<double>			m_endpx;		///< Ending Tank Pressure [bar]
	Mix::Ptr						m_mix;			///< Breathing Mix

	boost::optional<std::string>	m_salinity;		///< Salinity ('fresh' or 'salt')
	boost::optional<std::string>	m_comments;		///< Comments
	boost::optional<int>			m_rating;		///< Rating [0-5]

	boost::optional<bool>			m_stop;			///< Safety Stop Performed
	boost::optional<double>			m_stopdepth;	///< Safety Stop Depth [msw]
	boost::optional<int>			m_stoptime;		///< Safety Stop Time [sec]

	boost::optional<double>			m_weight;		///< Weight Used [kg]
	boost::optional<std::string>	m_viz_cat;		///< Visibility Category ('excellent', 'good', 'fair' or 'poor')
	boost::optional<int>			m_viz_dist;		///< Visibility Distance [m]

	boost::optional<std::string>	m_pg_start;		///< Starting Pressure Group
	boost::optional<std::string>	m_pg_end;		///< Ending Pressure Group
	boost::optional<int>			m_rnt;			///< Residual Nitrogen Time [minutes]
	boost::optional<int>			m_desat;		///< Desaturation Time [minutes]
	boost::optional<int>			m_nofly;		///< No-Fly Time [minutes]
	boost::optional<std::string>	m_algorithm;	///< Decompression Algorithm/Table Name

};

} /* logbook */

#endif /* LOGBOOK_DIVE_HPP_ */
