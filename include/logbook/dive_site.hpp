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

#ifndef LOGBOOK_DIVE_SITE_HPP_
#define LOGBOOK_DIVE_SITE_HPP_

/**
 * @file include/logbook/dive_site.hpp
 * @brief Dive Site Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <logbook/collection.hpp>
#include <logbook/country.hpp>
#include <logbook/persistent.hpp>

namespace logbook {

// Forward Definition of Dive
class Dive;

/**
 * @brief Dive Site Model Class
 */
class DiveSite: public TypedPersistent<DiveSite>
{
public:
	typedef boost::shared_ptr<DiveSite>	Ptr;
	typedef boost::weak_ptr<DiveSite>	WeakPtr;

public:

	//! Class Constructor
	DiveSite();

	//! Class Destructor
	virtual ~DiveSite();

public:

	//! @return Dive Collection
	IObjectCollection<Dive>::Ptr dives();

	//! @return Dive Collection
	IObjectCollection<Dive>::ConstPtr dives() const;

public:

	//! @return Average Depth of Dives at the Site
	const boost::optional<double> & avg_depth() const;

	//! @return Average Temperature of Dives at the Site
	const boost::optional<double> & avg_temp() const;

	//! @return Maximum Depth of Dives at the Site
	const boost::optional<double> & max_depth() const;

	//! @return Number of Dives at the Site
	unsigned int num_dives() const;

	//! @return Average Dive Rating
	const boost::optional<double> & rating() const;

public:

	//! @return Altitude
	const boost::optional<double> & altitude() const;

	//! @return Bottom Composition
	const boost::optional<std::string> & bottom() const;

	//! @return Comments
	const boost::optional<std::string> & comments() const;

	//! @return Country
	const boost::optional<country> & country_() const;

	//! @return Latitude
	const boost::optional<double> & latitude() const;

	//! @return Longitude
	const boost::optional<double> & longitude() const;

	//! @return Site Name
	const std::string & name() const;

	//! @return Place Name
	const boost::optional<std::string> & place() const;

	//! @return Dive Platform
	const boost::optional<std::string> & platform() const;

	//! @return Salinity
	const boost::optional<std::string> & salinity() const;

	//! @return Time Zone
	const boost::optional<std::string> & timezone() const;

	//! @return Water Body
	const boost::optional<std::string> & water_body() const;

	//! @brief Set the Altitude to NULL
	void setAltitude(const boost::none_t &);

	//! @param[in] Altitude
	void setAltitude(double value);

	//! @brief Set the Bottom Composition to NULL
	void setBottom(const boost::none_t &);

	//! @param[in] Bottom Composition
	void setBottom(const std::string & value);

	//! @brief Set the Comments to NULL
	void setComments(const boost::none_t &);

	//! @param[in] Comments
	void setComments(const std::string & value);

	//! @brief Set the Country to NULL
	void setCountry(const boost::none_t &);

	//! @param[in] Country
	void setCountry(const country & value);

	//! @brief Set Latitude to NULL
	void setLatitude(const boost::none_t &);

	//! @param[in] Latitude
	void setLatitude(double value);

	//! @brief Set Longitude to NULL
	void setLongitude(const boost::none_t &);

	//! @param[in] Longitude
	void setLongitude(double value);

	//! @param[in] Site Name
	void setName(const std::string & value);

	//! @brief Set the Place Name to NULL
	void setPlace(const boost::none_t &);

	//! @param[in] Place Name
	void setPlace(const std::string & value);

	//! @brief Set the Dive Platform to NULL
	void setPlatform(const boost::none_t &);

	//! @param[in] Dive Platform
	void setPlatform(const std::string & value);

	//! @brief Set the Salinity to NULL
	void setSalinity(const boost::none_t &);

	//! @param[in] Salinity
	void setSalinity(const std::string & value);

	//! @brief Set the Time Zone to NULL
	void setTimezone(const boost::none_t &);

	//! @param[in] Time Zone
	void setTimezone(const std::string & value);

	//! @brief Set the Water Body to NULL
	void setWaterBody(const boost::none_t &);

	//! @param[in] Water Body
	void setWaterBody(const std::string & value);

private:
	std::string								m_name;			///< Site Name
	boost::optional<std::string>			m_place;		///< Place Name
	boost::optional<country>				m_country;		///< Country

	boost::optional<double>					m_lat;			///< Latitude
	boost::optional<double>					m_lng;			///< Longitude

	boost::optional<std::string>			m_platform;		///< Dive Platform
	boost::optional<std::string>			m_waterbody;	///< Body of Water
	boost::optional<std::string>			m_bottom;		///< Bottom Composition
	boost::optional<std::string>			m_salinity;		///< Salinity ('fresh' or 'salt')
	boost::optional<double>					m_altitude;		///< Altitude [m]

	boost::optional<std::string>			m_timezone;		///< Time Zone
	boost::optional<std::string>			m_comments;		///< Comments

	mutable IObjectCollection<Dive>::Ptr	m_dives;		///< Dive Collection

};

/**
 * @brief Dive Finder Interface
 *
 * Extends IFinder<DiveSite> to add various lookup methods for field values.
 */
struct IDiveSiteFinder: public IFinder<DiveSite>
{
	typedef boost::shared_ptr<IDiveSiteFinder>	Ptr;
	virtual ~IDiveSiteFinder() { }

	//! @return Unique Field Values for DiveSite::bottom
	virtual std::vector<std::string> bottomValues() const = 0;

	//! @return Unique Field Values for DiveSite::platform
	virtual std::vector<std::string> platformValues() const = 0;

	//! @return Unique Field Values for DiveSite::water_body
	virtual std::vector<std::string> waterBodyValues() const = 0;

};

} /* logbook */

#endif /* LOGBOOK_DIVE_SITE_HPP_ */
