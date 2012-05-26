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

#ifndef LOGBOOK_PROFILE_HPP_
#define LOGBOOK_PROFILE_HPP_

/**
 * @file include/benthos/logbook/profile.hpp
 * @brief Dive Profile Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>
#include <list>
#include <map>
#include <set>
#include <vector>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/collection.hpp>
#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/mix.hpp>
#include <benthos/logbook/persistent.hpp>

namespace logbook {

/**
 * @brief Waypoint Structure
 *
 * Holds information about a single profile waypoint including the time offset
 * from the beginning of the profile, any alarms, the current breathing mix and
 * a list of measured data (e.g. depth, temperature or tank pressure).
 */
struct waypoint
{
	unsigned int							time;		///< Time [seconds]
	Mix::Ptr								mix;		///< Breathing Mix
	std::set<std::string, cicmp>			alarms;		///< List of Alarms
	std::map<std::string, double, cicmp>	data;		///< Waypoint Data
};

/**
 * @brief Dive Profile
 *
 * Holds information about a dive profile, typically loaded from a dive
 * computer.  The profile object also includes vendor-specified parameters
 * which are stored as a JSON string.
 */
class Profile: public TypedPersistent<Profile>
{
public:
	typedef boost::shared_ptr<Profile>	Ptr;
	typedef boost::weak_ptr<Profile>	WeakPtr;

public:

	//! Class Constructor
	Profile();

	//! Class Destructor
	virtual ~Profile();

public:

	//! @return Dive Computer
	DiveComputer::Ptr computer() const;

	//! @return Dive
	Dive::Ptr dive() const;

	//! @return Import Date/Time
	const boost::optional<time_t> & imported() const;

	//! @return Set of Data Keys
	const std::set<std::string> & keys() const;

	//! @return Profile Name
	const boost::optional<std::string> & name() const;

	//! @return Profile Waypoints
	const std::list<waypoint> & profile() const;

	//! @return Raw Profile Data
	const std::vector<unsigned char> & raw_profile() const;

	//! @return Vendor-Specified Data
	const boost::optional<std::string> & vendor() const;

	//! @brief Set the Dive Computer to NULL
	void setComputer(const boost::none_t &);

	//! @param[in] Dive Computer
	void setComputer(DiveComputer::Ptr value);

	//! @brief Set the Dive to NULL
	void setDive(const boost::none_t &);

	//! @param[in] Dive
	void setDive(Dive::Ptr value);

	//! @brief Set the Import Date/Time to NULL
	void setImported(const boost::none_t &);

	//! @param[in] Import Date/Time
	void setImported(const time_t & value);

	//! @brief Set the Profile Name to NULL
	void setName(const boost::none_t &);

	//! @param[in] Profile Name
	void setName(const std::string & value);

	//! @brief Set the Profile Waypoints to NULL
	void setProfile(const boost::none_t &);

	//! @param[in] Profile Waypoints
	void setProfile(const std::list<waypoint> & value);

	//! @brief Set the Raw Profile to NULL
	void setRawProfile(const boost::none_t &);

	//! @param[in] Raw Profile Data
	void setRawProfile(const std::vector<unsigned char> & value);

	//! @brief Set the Vendor-Specified Data to NULL
	void setVendor(const boost::none_t &);

	//! @param[in] Vendor-Specified Data
	void setVendor(const std::string & value);

private:
	Dive::Ptr						m_dive;		///< Dive
	DiveComputer::Ptr				m_computer;	///< Dive Computer

	boost::optional<std::string>	m_name;		///< Profile Name
	std::set<std::string>			m_keys;		///< Profile Data Keys
	std::list<waypoint>				m_profile;	///< Profile Waypoints
	boost::optional<std::string>	m_vendor;	///< Vendor-Specified Data
	boost::optional<time_t>			m_imported;	///< Import Date/Time

	std::vector<unsigned char>		m_raw;		///< Raw Profile Data Blob

};

/**
 * @brief Dive Profile Finder Interface
 *
 * Extends IFinder<Profile> to add find-by-dive and find-by-computer methods.
 */
struct IProfileFinder: public IFinder<Profile>
{
	typedef boost::shared_ptr<IProfileFinder>	Ptr;
	virtual ~IProfileFinder() { }

	/**
	 * @brief Find Profiles that belong to a given Dive
	 * @param[in] Dive Identifier
	 * @return List of Profiles
	 */
	virtual std::vector<Profile::Ptr> findByDive(int64_t dive_id) = 0;

	/**
	 * @brief Find Profiles that belong to a given Dive Computer
	 * @param[in] Dive Computer Identifier
	 * @return List of Profiles
	 */
	virtual std::vector<Profile::Ptr> findByComputer(int64_t computer_id) = 0;

};

} /* logbook */

#endif /* LOGBOOK_PROFILE_HPP_ */
