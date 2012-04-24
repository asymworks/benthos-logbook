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
#include "logbook/dive.hpp"
#include "logbook/profile.hpp"
#include "logbook/session.hpp"

using namespace logbook;

class DiveProfiles: public ObjectCollection<Profile, Dive>
{
public:

	//! Class Constructor
	DiveProfiles(Dive::Ptr obj)
		: ObjectCollection(obj)
	{
	}

	//! Class Destructor
	virtual ~DiveProfiles()
	{
	}

protected:

	//! @return List of Associated Items from the Database
	virtual std::vector<Profile::Ptr> load(Dive::Ptr obj) const
	{
		IProfileFinder::Ptr df = boost::shared_dynamic_cast<IProfileFinder>(obj->session()->finder<Profile>());
		return df->findByDive(obj->id());
	}

	//! @return Loaded Object from Database
	virtual Profile::Ptr load(boost::shared_ptr<Session> s, int64_t id) const
	{
		return s->finder<Profile>()->find(id);
	}

	//! @brief Link this Object to the Owning Object
	virtual void link(Profile::Ptr d, Dive::Ptr t) const
	{
		d->setDive(t);
	}

	//! @brief Check if the Owning Object is linked to this Object
	virtual bool linked(Profile::Ptr d, Dive::Ptr t) const
	{
		return (d->dive() == t);
	}

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(Profile::Ptr d, Dive::Ptr t) const
	{
		d->setDive(boost::none);
	}

};

Dive::Tags::Tags()
	: m_items()
{
}

Dive::Tags::~Tags()
{
}

std::list<std::string> Dive::Tags::all() const
{
	return std::list<std::string>(m_items.begin(), m_items.end());
}

void Dive::Tags::add(const std::string & tag)
{
	std::string ltag(tag);
	std::transform(ltag.begin(), ltag.end(), ltag.begin(), tolower);

	m_items.insert(tag);
}

void Dive::Tags::clear()
{
	m_items.clear();
}

void Dive::Tags::remove(const std::string & tag)
{
	m_items.erase(tag);
}

Dive::Dive()
	: TypedPersistent<Dive>(), m_repetition(1), m_interval(0), m_duration(0),
	  m_maxdepth(0), m_stop(false), m_tags(new Tags)
{
}

Dive::~Dive()
{
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

const boost::optional<double> Dive::end_pressure() const
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
		m_profiles = IObjectCollection<Profile>::Ptr(new DiveProfiles(boost::dynamic_pointer_cast<Dive>(shared_from_this())));
	return m_profiles;
}

IObjectCollection<Profile>::ConstPtr Dive::profiles() const
{
	if (! m_profiles)
		m_profiles = IObjectCollection<Profile>::Ptr(
			new DiveProfiles(boost::dynamic_pointer_cast<Dive>(
				boost::const_pointer_cast<Persistent>(shared_from_this())
			))
		);
	return m_profiles;
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
	return m_tags;
}

Dive::Tags::ConstPtr Dive::tags() const
{
	return m_tags;
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
}

void Dive::setAirTemp(double value)
{
	m_airtemp = value;
	mark_dirty();
}

void Dive::setAlgorithm(const boost::none_t &)
{
	m_algorithm.reset();
	mark_dirty();
}

void Dive::setAlgorithm(const std::string & value)
{
	m_algorithm = value;
	mark_dirty();
}

void Dive::setAvgDepth(const boost::none_t &)
{
	m_avgdepth.reset();
	mark_dirty();
}

void Dive::setAvgDepth(double value)
{
	m_avgdepth = value;
	mark_dirty();
}

void Dive::setComments(const boost::none_t &)
{
	m_comments.reset();
	mark_dirty();
}

void Dive::setComments(const std::string & value)
{
	m_comments = value;
	mark_dirty();
}

void Dive::setComputer(const boost::none_t &)
{
	m_computer.reset();
	mark_dirty();
}

void Dive::setComputer(DiveComputer::Ptr value)
{
	if (value && value->session() && session() && (value->session() != session()))
		throw std::runtime_error("DiveComputer belongs to a different session as the Dive");

	if (m_computer && ! m_computer->dives()->in_cascade())
	{
		m_computer->dives()->remove(boost::dynamic_pointer_cast<Dive>(shared_from_this()), false);
	}

	if (value && ! value->dives()->in_cascade())
	{
		value->dives()->add(boost::dynamic_pointer_cast<Dive>(shared_from_this()), false);
	}

	m_computer.swap(value);
	mark_dirty();
}

void Dive::setDateTime(const boost::none_t &)
{
	if (m_computer && ! m_computer->dives()->in_cascade())
	{
		m_computer->dives()->remove(boost::dynamic_pointer_cast<Dive>(shared_from_this()), false);
	}

	m_datetime.reset();
	mark_dirty();
}

void Dive::setDateTime(time_t value)
{
	m_datetime = value;
	mark_dirty();
}

void Dive::setDesatTime(const boost::none_t &)
{
	m_desat.reset();
	mark_dirty();
}

void Dive::setDesatTime(int value)
{
	if (value < 0)
		throw std::invalid_argument("Desaturation Time must be positive");

	m_desat = value;
	mark_dirty();
}

void Dive::setDuration(int value)
{
	if (value < 0)
		throw std::invalid_argument("Dive Duration must be positive");

	m_duration = value;
	mark_dirty();
}

void Dive::setEndPressure(const boost::none_t &)
{
	m_endpx.reset();
	mark_dirty();
}

void Dive::setEndPressure(double value)
{
	m_endpx = value;
	mark_dirty();
}

void Dive::setEndPressureGroup(const boost::none_t &)
{
	m_pg_end.reset();
	mark_dirty();
}

void Dive::setEndPressureGroup(const std::string & value)
{
	m_pg_end = value;
	mark_dirty();
}

void Dive::setInterval(int value)
{
	if (value < 0)
		throw std::invalid_argument("Surface Interval must be positive");

	m_interval = value;
	mark_dirty();
}

void Dive::setMaxDepth(double value)
{
	m_maxdepth = value;
	mark_dirty();
}

void Dive::setMaxTemp(const boost::none_t &)
{
	m_maxtemp.reset();
	mark_dirty();
}

void Dive::setMaxTemp(double value)
{
	m_maxtemp = value;
	mark_dirty();
}

void Dive::setMinTemp(const boost::none_t &)
{
	m_mintemp.reset();
	mark_dirty();
}

void Dive::setMinTemp(double value)
{
	m_mintemp = value;
	mark_dirty();
}

void Dive::setMix(const boost::none_t &)
{
	m_mix.reset();
	mark_dirty();
}

void Dive::setMix(Mix::Ptr value)
{
	m_mix.swap(value);
	mark_dirty();
}

void Dive::setNoFlyTime(const boost::none_t &)
{
	m_nofly.reset();
	mark_dirty();
}

void Dive::setNoFlyTime(int value)
{
	if (value < 0)
		throw std::invalid_argument("No-Fly Time must be positive");

	m_nofly = value;
	mark_dirty();
}

void Dive::setNumber(const boost::none_t &)
{
	m_number.reset();
	mark_dirty();
}

void Dive::setNumber(int value)
{
	if (value < 1)
		throw std::invalid_argument("Dive number must be greater than 0");

	m_number = value;
	mark_dirty();
}

void Dive::setRating(const boost::none_t &)
{
	m_rating.reset();
	mark_dirty();
}

void Dive::setRating(int value)
{
	if ((value < 0) || (value > 5))
		throw std::invalid_argument("Rating must be between 0 and 5");

	m_rating = value;
	mark_dirty();
}

void Dive::setRepetition(int value)
{
	if (value < 1)
		throw std::invalid_argument("Repetition number must be greater than 0");

	if (value == 1)
		// Surface Interval must be 0 ("infinite") for first dive in
		// repetition group
		m_interval = 0;

	m_repetition = value;
	mark_dirty();
}

void Dive::setRNT(const boost::none_t &)
{
	m_rnt.reset();
	mark_dirty();
}

void Dive::setRNT(int value)
{
	if (value < 0)
		throw std::invalid_argument("Residual Nitrogen Time must be positive");

	m_rnt = value;
	mark_dirty();
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
}

void Dive::setSalinity(const boost::none_t &)
{
	m_salinity.reset();
	mark_dirty();
}

void Dive::setSalinity(const std::string & value)
{
	std::string lvalue(value);
	std::transform(value.begin(), value.end(), lvalue.begin(), tolower);

	if ((lvalue != "fresh") && (lvalue != "salt"))
		throw std::invalid_argument("Salinity must be 'fresh' or 'salt'");

	m_salinity = lvalue;
	mark_dirty();
}

void Dive::setSite(const boost::none_t &)
{
	if (m_site && ! m_site->dives()->in_cascade())
	{
		m_site->dives()->remove(boost::dynamic_pointer_cast<Dive>(shared_from_this()), false);
	}

	m_site.reset();
	mark_dirty();
}

void Dive::setSite(DiveSite::Ptr value)
{
	if (value && value->session() && session() && (value->session() != session()))
		throw std::runtime_error("DiveSite belongs to a different session as the Dive");

	if (m_site && ! m_site->dives()->in_cascade())
	{
		m_site->dives()->remove(boost::dynamic_pointer_cast<Dive>(shared_from_this()), false);
	}

	if (value && ! value->dives()->in_cascade())
	{
		value->dives()->add(boost::dynamic_pointer_cast<Dive>(shared_from_this()), false);
	}

	m_site.swap(value);
	mark_dirty();
}

void Dive::setStartPressure(const boost::none_t &)
{
	m_startpx.reset();
	mark_dirty();
}

void Dive::setStartPressure(double value)
{
	m_startpx = value;
	mark_dirty();
}

void Dive::setStartPressureGroup(const boost::none_t &)
{
	m_pg_start.reset();
	mark_dirty();
}

void Dive::setStartPressureGroup(const std::string & value)
{
	m_pg_start = value;
	mark_dirty();
}

void Dive::setStopDepth(const boost::none_t &)
{
	m_stopdepth.reset();
	mark_dirty();
}

void Dive::setStopDepth(double value)
{
	m_stopdepth = value;
	mark_dirty();
}

void Dive::setStopTime(const boost::none_t &)
{
	m_stoptime.reset();
	mark_dirty();
}

void Dive::setStopTime(int value)
{
	m_stoptime = value;
	mark_dirty();
}

void Dive::setUTCOffset(const boost::none_t &)
{
	m_utc_offset.reset();
	mark_dirty();
}

void Dive::setUTCOffset(int value)
{
	m_utc_offset = value;
	mark_dirty();
}

void Dive::setVisibilityCategory(const boost::none_t &)
{
	m_viz_cat.reset();
	mark_dirty();
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
}

void Dive::setVisibilityDistance(const boost::none_t &)
{
	m_viz_dist.reset();
	mark_dirty();
}

void Dive::setVisibilityDistance(double value)
{
	if (value < 0)
		throw std::invalid_argument("Visibility Distance must be positive");

	m_viz_dist = value;
	mark_dirty();
}

void Dive::setWeight(const boost::none_t &)
{
	m_weight.reset();
	mark_dirty();
}

void Dive::setWeight(double value)
{
	if (value < 0)
		throw std::invalid_argument("Weight used must be positive");

	m_weight = value;
	mark_dirty();
}
