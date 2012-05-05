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
#include "logbook/dive_site.hpp"
#include "logbook/dive.hpp"
#include "logbook/session.hpp"

using namespace logbook;

class DiveSiteDives: public ObjectCollection<Dive, DiveSite>
{
public:

	//! Class Constructor
	DiveSiteDives(DiveSite::Ptr obj)
		: ObjectCollection(obj)
	{
	}

	//! Class Destructor
	virtual ~DiveSiteDives()
	{
	}

protected:

	//! @return List of Associated Items from the Database
	virtual std::vector<Dive::Ptr> load(DiveSite::Ptr obj) const
	{
		IDiveFinder::Ptr df = boost::shared_dynamic_cast<IDiveFinder>(obj->session()->finder<Dive>());
		return df->findBySite(obj->id());
	}

	//! @return Loaded Object from Database
	virtual Dive::Ptr load(boost::shared_ptr<Session> s, int64_t id) const
	{
		return s->finder<Dive>()->find(id);
	}

	//! @brief Link this Object to the Owning Object
	virtual void link(Dive::Ptr d, DiveSite::Ptr t) const
	{
		d->setSite(t);
	}

	//! @brief Check if the Owning Object is linked to this Object
	virtual bool linked(Dive::Ptr d, DiveSite::Ptr t) const
	{
		return (d->site() == t);
	}

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(Dive::Ptr d, DiveSite::Ptr t) const
	{
		d->setSite(boost::none);
	}

};

DiveSite::DiveSite()
	: TypedPersistent<DiveSite>()
{
}

DiveSite::~DiveSite()
{
}

const boost::optional<std::string> & DiveSite::bottom() const
{
	return m_bottom;
}

const boost::optional<std::string> & DiveSite::comments() const
{
	return m_comments;
}

const boost::optional<country> & DiveSite::country_() const
{
	return m_country;
}

IObjectCollection<Dive>::Ptr DiveSite::dives()
{
	if (! m_dives)
		m_dives = IObjectCollection<Dive>::Ptr(new DiveSiteDives(boost::dynamic_pointer_cast<DiveSite>(shared_from_this())));
	return m_dives;
}

IObjectCollection<Dive>::ConstPtr DiveSite::dives() const
{
	if (! m_dives)
		m_dives = IObjectCollection<Dive>::Ptr(
			new DiveSiteDives(boost::dynamic_pointer_cast<DiveSite>(
				boost::const_pointer_cast<Persistent>(shared_from_this())
			))
		);
	return m_dives;
}

const boost::optional<double> & DiveSite::latitude() const
{
	return m_lat;
}

const boost::optional<double> & DiveSite::longitude() const
{
	return m_lng;
}

const std::string & DiveSite::name() const
{
	return m_name;
}

unsigned int DiveSite::num_dives() const
{
	if (id() == -1)
		return 0;
	IDiveFinder::Ptr df = boost::shared_dynamic_cast<IDiveFinder>(session()->finder<Dive>());
	return df->countBySite(id());
}

const boost::optional<std::string> & DiveSite::place() const
{
	return m_place;
}

const boost::optional<std::string> & DiveSite::platform() const
{
	return m_platform;
}

const boost::optional<double> & DiveSite::rating() const
{
	static boost::optional<double> o;
	if (id() == -1)
	{
		o.reset();
		return o;
	}

	IDiveFinder::Ptr df = boost::shared_dynamic_cast<IDiveFinder>(session()->finder<Dive>());
	dbapi::variant v = df->ratingForSite(id());

	if (v.is_null())
		o.reset();
	else
		o = v.get<double>();

	return o;
}

const boost::optional<std::string> & DiveSite::salinity() const
{
	return m_salinity;
}

const boost::optional<std::string> & DiveSite::timezone() const
{
	return m_timezone;
}

const boost::optional<std::string> & DiveSite::water_body() const
{
	return m_waterbody;
}

void DiveSite::setBottom(const boost::none_t &)
{
	m_bottom.reset();
	mark_dirty();
}

void DiveSite::setBottom(const std::string & value)
{
	m_bottom = value;
	mark_dirty();
}

void DiveSite::setComments(const boost::none_t &)
{
	m_comments.reset();
	mark_dirty();
}

void DiveSite::setComments(const std::string & value)
{
	m_comments = value;
	mark_dirty();
}

void DiveSite::setCountry(const boost::none_t &)
{
	m_country.reset();
	mark_dirty();
}

void DiveSite::setCountry(const country & value)
{
	m_country = value;
	mark_dirty();
}

void DiveSite::setLatitude(const boost::none_t &)
{
	m_lat.reset();
	mark_dirty();
}

void DiveSite::setLatitude(double value)
{
	if ((value > 90) || (value < -90))
		throw std::invalid_argument("Latitude must be between -90 and 90");
	m_lat = value;
	mark_dirty();
}

void DiveSite::setLongitude(const boost::none_t &)
{
	m_lng.reset();
	mark_dirty();
}

void DiveSite::setLongitude(double value)
{
	if ((value > 180) || (value < -180))
		throw std::invalid_argument("Longitude must be between -180 and 180");
	m_lng = value;
	mark_dirty();
}

void DiveSite::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
}

void DiveSite::setPlace(const boost::none_t &)
{
	m_place.reset();
	mark_dirty();
}

void DiveSite::setPlace(const std::string & value)
{
	m_place = value;
	mark_dirty();
}

void DiveSite::setPlatform(const boost::none_t &)
{
	m_platform.reset();
	mark_dirty();
}

void DiveSite::setPlatform(const std::string & value)
{
	m_platform = value;
	mark_dirty();
}

void DiveSite::setSalinity(const boost::none_t &)
{
	m_salinity.reset();
	mark_dirty();
}

void DiveSite::setSalinity(const std::string & value)
{
	std::string lvalue(value);
	std::transform(value.begin(), value.end(), lvalue.begin(), ::tolower);

	if ((lvalue != "fresh") && (lvalue != "salt"))
		throw std::invalid_argument("Salinity must be either 'fresh' or 'salt'");

	m_salinity = lvalue;
	mark_dirty();
}

void DiveSite::setTimezone(const boost::none_t &)
{
	m_timezone.reset();
	mark_dirty();
}

void DiveSite::setTimezone(const std::string & value)
{
	m_timezone = value;
	mark_dirty();
}

void DiveSite::setWaterBody(const boost::none_t &)
{
	m_waterbody.reset();
	mark_dirty();
}

void DiveSite::setWaterBody(const std::string & value)
{
	m_waterbody = value;
	mark_dirty();
}
