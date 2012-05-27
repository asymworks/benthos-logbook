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

#ifndef LOGBOOKMAPPERS_DIVE_MAPPER_HPP_
#define LOGBOOKMAPPERS_DIVE_MAPPER_HPP_

/**
 * @file src/logbook/mappers/dive_mapper.hpp
 * @brief Dive Data Mapper Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <benthos/logbook/dbapi.hpp>
#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/mapper.hpp>
#include <benthos/logbook/persistent.hpp>

namespace benthos { namespace logbook { namespace mappers {

/**
 * @brief Dive Data Mapper
 *
 * Data Mapper which implements CRUD operations for Dive domain objects
 * within the Logbook database.
 */
class DiveMapper: public Mapper<Dive>, public IDiveFinder
{
public:
	typedef Mapper<Dive>::Ptr	Ptr;
	typedef Mapper<Dive>::WPtr	WPtr;

protected:
	static std::string columns;			///< List of Columns Returned by SELECT

	static std::string sql_insert;		///< INSERT Statement SQL String
	static std::string sql_update;		///< UPDATE Statement SQL String
	static std::string sql_delete;		///< DELETE Statement SQL String

	static std::string sql_find_all;	///< Find All Statement SQL String
	static std::string sql_find_id;		///< Find By Id Statement SQL String
	static std::string sql_find_site;	///< Find By Site Statement SQL String
	static std::string sql_find_cpu;	///< Find By Computer Statement SQL String
	static std::string sql_find_ctry;	///< Find By Country Statement SQL String
	static std::string sql_find_dates;	///< Find By Dates Statement SQL String

	static std::string sql_find_recent;	///< Find Recently-Imported Statement SQL String

	static std::string sql_count_site;	///< Count By Site Statement SQL String
	static std::string sql_count_cpu;	///< Count By Computer Statement SQL String
	static std::string sql_avgrating;	///< Average Rating by Site Statement SQL String
	static std::string sql_maxdepth;	///< Maximum Depth by Site Statement SQL String
	static std::string sql_avgdepth;	///< Average Depth by Site Statement SQL String
	static std::string sql_avgtemp;		///< Average Temperature by Site Statement SQL String

	static std::string sql_find_tags;	///< Find Tags by Dive SQL String
	static std::string sql_drop_tags;	///< Drop Tags by Dive SQL String
	static std::string sql_add_tags;	///< Insert Tags SQL String
	static std::string sql_all_tags;	///< Find All Tags SQL String

public:

	//! Class Constructor
	DiveMapper(boost::shared_ptr<Session> session);

	//! Class Destructor
	virtual ~DiveMapper();

public:

	/**
	 * @brief Return the list of Persistent Objects to be Cascade-Updated
	 * @param[in] Domain Object
	 * @return List of Cascaded Objects
	 */
	virtual std::list<Persistent::Ptr> cascade_add(Persistent::Ptr o);

public:

	/**
	 * @brief Find all Tags
	 * @return List of Tags sorted alphabetically
	 */
	virtual std::vector<std::string> allTags() const;

	/**
	 * @brief Count Dives from a give Dive Computer
	 * @param[in] Dive Computer Id
	 * @return Number of Dives
	 */
	virtual unsigned int countByComputer(int64_t computer_id) const;

	/**
	 * @brief Count Dives at a give Dive Site
	 * @param[in] Dive Site Id
	 * @return Number of Dives
	 */
	virtual unsigned int countBySite(int64_t computer_id) const;

	/**
	 * @brief Return a list of all Dives
	 * @return List of Dives
	 */
	virtual std::vector<Dive::Ptr> find();

	/**
	 * @brief Return a single Dive by its identifier
	 * @param[in] Identifier
	 * @return Dive
	 */
	virtual Dive::Ptr find(int64_t id);

	/**
	 * @brief Find Dives imported within the given number of days
	 * @param[in] Number of Days
	 * @param[in] Maximum Number to Return
	 * @return List of Dives
	 */
	virtual std::vector<Dive::Ptr> findRecentlyImported(unsigned int days, int max);

	/**
	 * @brief Find Dives from a given Dive Computer
	 * @param[in] Dive Computer Id
	 * @return List of Dives
	 */
	virtual std::vector<Dive::Ptr> findByComputer(int64_t computer_id);

	/**
	 * @brief Find Dives in a given Country
	 * @param[in] Country
	 * @return List of Dives
	 */
	virtual std::vector<Dive::Ptr> findByCountry(const country & country_);

	/**
	 * @brief Find Dives in a given Time Span
	 * @param[in] Start Time
	 * @param[in] End Time
	 * @return List of Dives
	 */
	virtual std::vector<Dive::Ptr> findByDates(time_t start, time_t end);

	/**
	 * @brief Find Dives at a given Dive Site
	 * @param[in] Dive Site Id
	 * @return List of Dives
	 */
	virtual std::vector<Dive::Ptr> findBySite(int64_t site_id);

	/**
	 * @brief Get the Average Maximum Depth of Dives at a given Site
	 * @param[in] Dive Site Id
	 * @return Average Depth
	 */
	virtual boost::optional<double> avgDepthForSite(int64_t site_id) const;

	/**
	 * @brief Get the Average Minimum Temperature of Dives at a given Site
	 * @param[in] Dive Site Id
	 * @return Average Temperature
	 */
	virtual boost::optional<double> avgTempForSite(int64_t site_id) const;

	/**
	 * @brief Get the Maximum Depth of Dives at a given Site
	 * @param[in] Dive Site Id
	 * @return Maximum Depth
	 */
	virtual boost::optional<double> maxDepthForSite(int64_t site_id) const;

	/**
	 * @brief Get the Average Rating of Dives at a given Site
	 * @param[in] Dive Site Id
	 * @return Average Rating or NULL
	 */
	virtual boost::optional<double> ratingForSite(int64_t site_id) const;

protected:

	//! Perform Operations after Deleting a Persistent
	virtual void afterDelete(Persistent::Ptr o, int64_t oldId);

	//! Perform Operations after Inserting a Persistent
	virtual void afterInsert(Persistent::Ptr o);

	//! Perform Operations after Updating a Persistent
	virtual void afterUpdate(Persistent::Ptr o);

	//! Perform Operations after Loading a Persistent
	virtual void afterLoaded(Persistent::Ptr o);

protected:

	//! Bind an Object to the Insert Statement
	virtual void bindInsert(statement::ptr s, Persistent::Ptr o) const;

	//! Bind an Object to the Update Statement
	virtual void bindUpdate(statement::ptr s, Persistent::Ptr o) const;

	//! Load an Object from a Result Set
	virtual Dive::Ptr doLoad(int64_t id, dbapi::cursor::row_t r) const;

protected:
	dbapi::statement::ptr		m_find_all_stmt;		///< Find All Prepared Statement
	dbapi::statement::ptr		m_find_id_stmt;			///< Find By Id Prepared Statement
	dbapi::statement::ptr		m_find_site_stmt;		///< Find By Site Prepared Statement
	dbapi::statement::ptr		m_find_cpu_stmt;		///< Find By Computer Prepared Statement
	dbapi::statement::ptr		m_find_ctry_stmt;		///< Find By Country Prepared Statement
	dbapi::statement::ptr		m_find_dates_stmt;		///< Find By Dates Prepared Statement
	dbapi::statement::ptr		m_find_recent_stmt;		///< Find Recently-Imported Prepared Statement
	dbapi::statement::ptr		m_count_site_stmt;		///< Count By Site Prepared Statement
	dbapi::statement::ptr		m_count_cpu_stmt;		///< Count By Computer Prepared Statement
	dbapi::statement::ptr		m_avgrating_stmt;		///< Average Rating for Site Prepared Statement
	dbapi::statement::ptr		m_maxdepth_stmt;		///< Maximum Depth for Site Prepared Statement
	dbapi::statement::ptr		m_avgdepth_stmt;		///< Average Depth for Site Prepared Statement
	dbapi::statement::ptr		m_avgtemp_stmt;			///< Average Temperature for Site Prepared Statement

	dbapi::statement::ptr 		m_find_tags_stmt;		///< Find Tags by Dive Prepared Statement
	dbapi::statement::ptr 		m_drop_tags_stmt;		///< Drop Tags by Dive Prepared Statement
	dbapi::statement::ptr 		m_add_tags_stmt;		///< Insert Tags Prepared Statement
	dbapi::statement::ptr 		m_all_tags_stmt;		///< Find All Tags Prepared Statement

};

} } } /* benthos::logbook::mappers */

#endif /* LOGBOOKMAPPERS_DIVE_MAPPER_HPP_ */
