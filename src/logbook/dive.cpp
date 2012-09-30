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
#include "benthos/logbook/dive.hpp"
#include "benthos/logbook/dive_tank.hpp"
#include "benthos/logbook/profile.hpp"
#include "benthos/logbook/object_collection.hpp"
#include "benthos/logbook/session.hpp"

using namespace benthos::logbook;

class DiveProfiles: public ObjectCollection<Profile, Dive>
{
public:

	//! Class Constructor
	DiveProfiles(Dive::Ptr obj)
		: ObjectCollection(boost::shared_polymorphic_cast<Persistent>(obj), "profiles", "dive")
	{
	}

	//! Class Destructor
	virtual ~DiveProfiles()
	{
	}

protected:

	//! @return List of Associated Items from the Database
	virtual std::vector<Profile::Ptr> doLoad(Persistent::Ptr obj)
	{
		IProfileFinder::Ptr df = boost::shared_dynamic_cast<IProfileFinder>(obj->session()->finder<Profile>());
		return df->findByDive(obj->id());
	}

	//! @brief Link this Object to the Owning Object
	virtual void link(Persistent::Ptr d, Persistent::Ptr obj)
	{
		Dive::Ptr dive = boost::shared_polymorphic_downcast<Dive>(obj);
		Profile::Ptr profile = boost::shared_polymorphic_downcast<Profile>(d);
		profile->setDive(dive);
	}

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(Persistent::Ptr d, Persistent::Ptr obj)
	{
		Dive::Ptr dive = boost::shared_polymorphic_downcast<Dive>(obj);
		Profile::Ptr profile = boost::shared_polymorphic_downcast<Profile>(d);
		profile->setDive(boost::none);
	}

};

class DiveTanks: public ObjectCollection<DiveTank, Dive>
{
public:

	//! Class Constructor
	DiveTanks(Dive::Ptr obj)
		: ObjectCollection(boost::shared_polymorphic_cast<Persistent>(obj), "tanks", "dive")
	{
	}

	//! Class Destructor
	virtual ~DiveTanks()
	{
	}

protected:

	//! @return List of Associated Items from the Database
	virtual std::vector<DiveTank::Ptr> doLoad(Persistent::Ptr obj)
	{
		IDiveTankFinder::Ptr df = boost::shared_dynamic_cast<IDiveTankFinder>(obj->session()->finder<DiveTank>());
		return df->findByDive(obj->id());
	}

	//! @brief Link this Object to the Owning Object
	virtual void link(Persistent::Ptr d, Persistent::Ptr obj)
	{
		/*
		 * The DiveTank object is statically linked to its dive, so just
		 * call the event handler here.
		 */
		on_link_update(d, "dive", boost::any(boost::dynamic_pointer_cast<Dive>(obj)));
	}

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(Persistent::Ptr d, Persistent::Ptr obj)
	{
		/*
		 * The DiveTank object is statically linked to its dive, so just
		 * call the event handler here.
		 */
		on_link_update(d, "dive", boost::any());
	}

};

Dive::Tags::Tags(Dive::Ptr dive)
	: m_items(), m_dive(dive)
{
}

Dive::Tags::~Tags()
{
}

std::list<std::string> Dive::Tags::all() const
{
	return std::list<std::string>(m_items.begin(), m_items.end());
}

void Dive::Tags::assign(Dive::Tags::ConstPtr tags)
{
	if (tags)
		m_items = (tags.get())->m_items;
	else
		m_items.clear();

	if (m_dive) m_dive->mark_dirty();
}

void Dive::Tags::add(const std::string & tag)
{
	std::string ltag(tag);
	std::transform(ltag.begin(), ltag.end(), ltag.begin(), tolower);

	m_items.insert(tag);
	if (m_dive) m_dive->mark_dirty();
}

void Dive::Tags::clear()
{
	m_items.clear();
	if (m_dive) m_dive->mark_dirty();
}

void Dive::Tags::remove(const std::string & tag)
{
	m_items.erase(tag);
	if (m_dive) m_dive->mark_dirty();
}

Dive::Dive()
	: TypedPersistent<Dive>(), m_repetition(1), m_interval(0), m_duration(0),
	  m_maxdepth(0), m_stop(false), m_tags(), m_profiles()
{
}

Dive::~Dive()
{
}

void Dive::attached(Session::Ptr s)
{
	Persistent::attached(s);

	m_evtComputerDel = s->mapper<DiveComputer>()->events().before_delete.connect(boost::bind(& Dive::evtDiveComputerDeleted, this, _1, _2));
	m_evtMixDel = s->mapper<Mix>()->events().before_delete.connect(boost::bind(& Dive::evtMixDeleted, this, _1, _2));
	m_evtSiteDel = s->mapper<DiveSite>()->events().before_delete.connect(boost::bind(& Dive::evtDiveSiteDeleted, this, _1, _2));
}

void Dive::detached(Session::Ptr s)
{
	m_evtComputerDel.disconnect();
	m_evtMixDel.disconnect();
	m_evtSiteDel.disconnect();

	Persistent::detached(s);
}

void Dive::evtDiveComputerDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	DiveComputer::Ptr o = boost::dynamic_pointer_cast<DiveComputer>(obj);
	if (o && (o == m_computer))
		setComputer(boost::none);
}

void Dive::evtMixDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	Mix::Ptr o = boost::dynamic_pointer_cast<Mix>(obj);
	if (o && (o == m_mix))
		setMix(boost::none);
}

void Dive::evtDiveSiteDeleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	DiveSite::Ptr o = boost::dynamic_pointer_cast<DiveSite>(obj);
	if (o && (o == m_site))
		setSite(boost::none);
}

const boost::optional<double> & Dive::air_temp() const
{
	return m_airtemp;
}

const boost::optional<std::string> & Dive::algorithm() const
{
	return m_algorithm;
}

const boost::optional<double> & Dive::avg_depth() const
{
	return m_avgdepth;
}

const boost::optional<std::string> & Dive::comments() const
{
	return m_comments;
}

DiveComputer::Ptr Dive::computer() const
{
	return m_computer;
}

const boost::optional<time_t> & Dive::datetime() const
{
	return m_datetime;
}

const boost::optional<int> & Dive::desat_time() const
{
	return m_desat;
}

int Dive::duration() const
{
	return m_duration;
}

const boost::optional<double> & Dive::end_pressure() const
{
	return m_endpx;
}

const boost::optional<std::string> & Dive::end_pressure_group() const
{
	return m_pg_end;
}

int Dive::interval() const
{
	return m_interval;
}

double Dive::max_depth() const
{
	return m_maxdepth;
}

const boost::optional<double> & Dive::max_temp() const
{
	return m_maxtemp;
}

const boost::optional<double> & Dive::min_temp() const
{
	return m_mintemp;
}

Mix::Ptr Dive::mix() const
{
	return m_mix;
}

const boost::optional<int> & Dive::nofly_time() const
{
	return m_nofly;
}

const boost::optional<int> & Dive::number() const
{
	return m_number;
}

IObjectCollection<Profile>::Ptr Dive::profiles()
{
	if (! m_profiles)
	{
		m_profiles = IObjectCollection<Profile>::Ptr(new DiveProfiles(boost::dynamic_pointer_cast<Dive>(shared_from_this())));
		m_profiles->load();
	}
	return m_profiles;
}

IObjectCollection<Profile>::ConstPtr Dive::profiles() const
{
	return profiles();
}

const boost::optional<int> & Dive::rating() const
{
	return m_rating;
}

int Dive::repetition() const
{
	return m_repetition;
}

const boost::optional<int> & Dive::rnt() const
{
	return m_rnt;
}

const boost::optional<std::string> & Dive::salinity() const
{
	return m_salinity;
}

bool Dive::safety_stop() const
{
	return m_stop;
}

DiveSite::Ptr Dive::site() const
{
	return m_site;
}

const boost::optional<double> & Dive::start_pressure() const
{
	return m_startpx;
}

const boost::optional<std::string> & Dive::start_pressure_group() const
{
	return m_pg_start;
}

const boost::optional<double> & Dive::stop_depth() const
{
	return m_stopdepth;
}

const boost::optional<int> & Dive::stop_time() const
{
	return m_stoptime;
}

Dive::Tags::Ptr Dive::tags()
{
	if (! m_tags)
		m_tags = Dive::Tags::Ptr(new Dive::Tags(boost::dynamic_pointer_cast<Dive>(shared_from_this())));
	return m_tags;
}

Dive::Tags::ConstPtr Dive::tags() const
{
	if (! m_tags)
		m_tags = Dive::Tags::Ptr(
			new Dive::Tags(boost::dynamic_pointer_cast<Dive>(
				boost::const_pointer_cast<Persistent>(shared_from_this())
			))
		);
	return m_tags;
}

Tank::Ptr Dive::tank() const
{
	return m_tank;
}

IObjectCollection<DiveTank>::Ptr Dive::tanks()
{
	if (! m_tanks)
	{
		m_tanks = IObjectCollection<DiveTank>::Ptr(new DiveTanks(boost::dynamic_pointer_cast<Dive>(shared_from_this())));
		m_tanks->load();
	}
	return m_tanks;
}

IObjectCollection<DiveTank>::ConstPtr Dive::tanks() const
{
	return tanks();
}

const boost::optional<int> & Dive::utc_offset() const
{
	return m_utc_offset;
}

const boost::optional<std::string> & Dive::visibility_category() const
{
	return m_viz_cat;
}

const boost::optional<double> & Dive::visibility_distance() const
{
	return m_viz_dist;
}

const boost::optional<double> & Dive::weight() const
{
	return m_weight;
}

void Dive::setAirTemp(const boost::none_t &)
{
	m_airtemp.reset();
	mark_dirty();
	events().attr_set(ptr(), "air_temp", boost::any());
}

void Dive::setAirTemp(double value)
{
	m_airtemp = value;
	mark_dirty();
	events().attr_set(ptr(), "air_temp", boost::any(value));
}

void Dive::setAlgorithm(const boost::none_t &)
{
	m_algorithm.reset();
	mark_dirty();
	events().attr_set(ptr(), "algorithm", boost::any());
}

void Dive::setAlgorithm(const std::string & value)
{
	m_algorithm = value;
	mark_dirty();
	events().attr_set(ptr(), "algorithm", boost::any(value));
}

void Dive::setAvgDepth(const boost::none_t &)
{
	m_avgdepth.reset();
	mark_dirty();
	events().attr_set(ptr(), "avg_depth", boost::any());
}

void Dive::setAvgDepth(double value)
{
	m_avgdepth = value;
	mark_dirty();
	events().attr_set(ptr(), "avg_depth", boost::any(value));
}

void Dive::setComments(const boost::none_t &)
{
	m_comments.reset();
	mark_dirty();
	events().attr_set(ptr(), "comments", boost::any());
}

void Dive::setComments(const std::string & value)
{
	m_comments = value;
	mark_dirty();
	events().attr_set(ptr(), "comments", boost::any(value));
}

void Dive::setComputer(const boost::none_t &)
{
	m_computer.reset();
	mark_dirty();
	events().attr_set(ptr(), "computer", boost::any());
}

void Dive::setComputer(DiveComputer::Ptr value)
{
	if (value && value->session() && session() && (value->session() != session()))
		throw std::runtime_error("DiveComputer belongs to a different session as the Dive");

	m_computer = value;
	mark_dirty();
	events().attr_set(ptr(), "computer", boost::any(value));
}

void Dive::setDateTime(const boost::none_t &)
{
	m_datetime.reset();
	mark_dirty();
	events().attr_set(ptr(), "datetime", boost::any());
}

void Dive::setDateTime(time_t value)
{
	m_datetime = value;
	mark_dirty();
	events().attr_set(ptr(), "datetime", boost::any(value));
}

void Dive::setDesatTime(const boost::none_t &)
{
	m_desat.reset();
	mark_dirty();
	events().attr_set(ptr(), "desat_time", boost::any());
}

void Dive::setDesatTime(int value)
{
	if (value < 0)
		throw std::invalid_argument("Desaturation Time must be positive");

	m_desat = value;
	mark_dirty();
	events().attr_set(ptr(), "desat_time", boost::any(value));
}

void Dive::setDuration(int value)
{
	if (value < 0)
		throw std::invalid_argument("Dive Duration must be positive");

	m_duration = value;
	mark_dirty();
	events().attr_set(ptr(), "duration", boost::any(value));
}

void Dive::setEndPressure(const boost::none_t &)
{
	m_endpx.reset();
	mark_dirty();
	events().attr_set(ptr(), "end_pressure", boost::any());
}

void Dive::setEndPressure(double value)
{
	m_endpx = value;
	mark_dirty();
	events().attr_set(ptr(), "end_pressure", boost::any(value));
}

void Dive::setEndPressureGroup(const boost::none_t &)
{
	m_pg_end.reset();
	mark_dirty();
	events().attr_set(ptr(), "end_pressure_group", boost::any());
}

void Dive::setEndPressureGroup(const std::string & value)
{
	m_pg_end = value;
	mark_dirty();
	events().attr_set(ptr(), "end_pressure_group", boost::any(value));
}

void Dive::setInterval(int value)
{
	if (value < 0)
		throw std::invalid_argument("Surface Interval must be positive");

	m_interval = value;
	mark_dirty();
	events().attr_set(ptr(), "interval", boost::any(value));
}

void Dive::setMaxDepth(double value)
{
	m_maxdepth = value;
	mark_dirty();
	events().attr_set(ptr(), "max_depth", boost::any(value));
}

void Dive::setMaxTemp(const boost::none_t &)
{
	m_maxtemp.reset();
	mark_dirty();

	events().attr_set(ptr(), "max_temp", boost::any());
}

void Dive::setMaxTemp(double value)
{
	m_maxtemp = value;
	mark_dirty();
	events().attr_set(ptr(), "max_temp", boost::any(value));
}

void Dive::setMinTemp(const boost::none_t &)
{
	m_mintemp.reset();
	mark_dirty();
	events().attr_set(ptr(), "min_temp", boost::any());
}

void Dive::setMinTemp(double value)
{
	m_mintemp = value;
	mark_dirty();
	events().attr_set(ptr(), "min_temp", boost::any(value));
}

void Dive::setMix(const boost::none_t &)
{
	m_mix.reset();
	mark_dirty();
	events().attr_set(ptr(), "mix", boost::any());
}

void Dive::setMix(Mix::Ptr value)
{
	m_mix = value;
	mark_dirty();
	events().attr_set(ptr(), "mix", boost::any(value));
}

void Dive::setNoFlyTime(const boost::none_t &)
{
	m_nofly.reset();
	mark_dirty();
	events().attr_set(ptr(), "nofly_time", boost::any());
}

void Dive::setNoFlyTime(int value)
{
	if (value < 0)
		throw std::invalid_argument("No-Fly Time must be positive");

	m_nofly = value;
	mark_dirty();
	events().attr_set(ptr(), "nofly_time", boost::any(value));
}

void Dive::setNumber(const boost::none_t &)
{
	m_number.reset();
	mark_dirty();
	events().attr_set(ptr(), "number", boost::any());
}

void Dive::setNumber(int value)
{
	if (value < 0)
		throw std::invalid_argument("Dive number must be greater than 0");

	if (! value)
		m_number.reset();
	else
		m_number = value;

	mark_dirty();
	events().attr_set(ptr(), "number", boost::any(value));
}

void Dive::setRating(const boost::none_t &)
{
	m_rating.reset();
	mark_dirty();
	events().attr_set(ptr(), "rating", boost::any());
}

void Dive::setRating(int value)
{
	if ((value < 0) || (value > 5))
		throw std::invalid_argument("Rating must be between 0 and 5");

	m_rating = value;
	mark_dirty();
	events().attr_set(ptr(), "rating", boost::any(value));
}

void Dive::setRepetition(int value)
{
	bool setInterval = false;
	if (value < 1)
		throw std::invalid_argument("Repetition number must be greater than 0");

	if (value == 1)
	{
		// Surface Interval must be 0 ("infinite") for first dive in
		// repetition group
		m_interval = 0;
		setInterval = true;
	}

	m_repetition = value;
	mark_dirty();
	events().attr_set(ptr(), "repetition", boost::any(value));
	if (setInterval)
		events().attr_set(ptr(), "interval", boost::any(m_interval));
}

void Dive::setRNT(const boost::none_t &)
{
	m_rnt.reset();
	mark_dirty();
	events().attr_set(ptr(), "rnt", boost::any());
}

void Dive::setRNT(int value)
{
	if (value < 0)
		throw std::invalid_argument("Residual Nitrogen Time must be positive");

	if (value == 0)
		m_rnt.reset();
	else
		m_rnt = value;

	mark_dirty();
	events().attr_set(ptr(), "rnt", boost::any(value));
}

void Dive::setSafetyStop(bool value)
{
	if (! value)
	{
		m_stopdepth.reset();
		m_stoptime.reset();
	}

	m_stop = value;
	mark_dirty();
	events().attr_set(ptr(), "safety_stop", boost::any(value));
	if (! value)
	{
		events().attr_set(ptr(), "stop_depth", boost::any());
		events().attr_set(ptr(), "stop_time", boost::any());
	}
}

void Dive::setSalinity(const boost::none_t &)
{
	m_salinity.reset();
	mark_dirty();
	events().attr_set(ptr(), "salinity", boost::any());
}

void Dive::setSalinity(const std::string & value)
{
	std::string lvalue(value);
	std::transform(value.begin(), value.end(), lvalue.begin(), tolower);

	if ((lvalue != "fresh") && (lvalue != "salt"))
		throw std::invalid_argument("Salinity must be 'fresh' or 'salt'");

	m_salinity = lvalue;
	mark_dirty();
	events().attr_set(ptr(), "salinity", boost::any(lvalue));
}

void Dive::setSite(const boost::none_t &)
{
	m_site.reset();
	mark_dirty();
	events().attr_set(ptr(), "site", boost::any());
}

void Dive::setSite(DiveSite::Ptr value)
{
	if (value && value->session() && session() && (value->session() != session()))
		throw std::runtime_error("DiveSite belongs to a different session as the Dive");

	m_site = value;
	mark_dirty();
	events().attr_set(ptr(), "site", boost::any(value));
}

void Dive::setStartPressure(const boost::none_t &)
{
	m_startpx.reset();
	mark_dirty();
	events().attr_set(ptr(), "start_pressure", boost::any());
}

void Dive::setStartPressure(double value)
{
	m_startpx = value;
	mark_dirty();
	events().attr_set(ptr(), "start_pressure", boost::any(value));
}

void Dive::setStartPressureGroup(const boost::none_t &)
{
	m_pg_start.reset();
	mark_dirty();
	events().attr_set(ptr(), "start_pressure_group", boost::any());
}

void Dive::setStartPressureGroup(const std::string & value)
{
	m_pg_start = value;
	mark_dirty();
	events().attr_set(ptr(), "start_pressure_group", boost::any(value));
}

void Dive::setStopDepth(const boost::none_t &)
{
	m_stopdepth.reset();
	mark_dirty();
	events().attr_set(ptr(), "stop_depth", boost::any());
}

void Dive::setStopDepth(double value)
{
	m_stopdepth = value;
	mark_dirty();
	events().attr_set(ptr(), "stop_depth", boost::any(value));
}

void Dive::setStopTime(const boost::none_t &)
{
	m_stoptime.reset();
	mark_dirty();
	events().attr_set(ptr(), "stop_time", boost::any());
}

void Dive::setStopTime(int value)
{
	m_stoptime = value;
	mark_dirty();
	events().attr_set(ptr(), "stop_time", boost::any(value));
}

void Dive::setTank(const boost::none_t &)
{
	m_tank.reset();
	mark_dirty();
	events().attr_set(ptr(), "tank", boost::any());
}

void Dive::setTank(Tank::Ptr value)
{
	m_tank = value;
	mark_dirty();
	events().attr_set(ptr(), "tank", boost::any(value));
}

void Dive::setUTCOffset(const boost::none_t &)
{
	m_utc_offset.reset();
	mark_dirty();
	events().attr_set(ptr(), "utc_offset", boost::any());
}

void Dive::setUTCOffset(int value)
{
	m_utc_offset = value;
	mark_dirty();
	events().attr_set(ptr(), "utc_offset", boost::any(value));
}

void Dive::setVisibilityCategory(const boost::none_t &)
{
	m_viz_cat.reset();
	mark_dirty();
	events().attr_set(ptr(), "visibility_category", boost::any());
}

void Dive::setVisibilityCategory(const std::string & value)
{
	std::string lvalue(value);
	std::transform(value.begin(), value.end(), lvalue.begin(), tolower);

	if ((lvalue != "excellent") && (lvalue != "good") &&
		(lvalue != "fair") && (lvalue != "poor"))
		throw std::invalid_argument("Visibility Category must be one of 'excellent', 'good', 'fair' or 'poor'");

	m_viz_cat = lvalue;
	mark_dirty();
	events().attr_set(ptr(), "visibility_category", boost::any(lvalue));
}

void Dive::setVisibilityDistance(const boost::none_t &)
{
	m_viz_dist.reset();
	mark_dirty();
	events().attr_set(ptr(), "visibility_distance", boost::any());
}

void Dive::setVisibilityDistance(double value)
{
	if (value < 0)
		throw std::invalid_argument("Visibility Distance must be positive");

	m_viz_dist = value;
	mark_dirty();
	events().attr_set(ptr(), "visibility_distance", boost::any(value));
}

void Dive::setWeight(const boost::none_t &)
{
	m_weight.reset();
	mark_dirty();
	events().attr_set(ptr(), "weight", boost::any());
}

void Dive::setWeight(double value)
{
	if (value < 0)
		throw std::invalid_argument("Weight used must be positive");

	m_weight = value;
	mark_dirty();
	events().attr_set(ptr(), "weight", boost::any(value));
}
