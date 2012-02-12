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

#include "logbook/dive_computer.hpp"
#include "logbook/dive.hpp"
#include "logbook/profile.hpp"
#include "logbook/session.hpp"

using namespace logbook;

class DiveComputerDives: public ObjectCollection<Dive, DiveComputer>
{
public:

	//! Class Constructor
	DiveComputerDives(DiveComputer::Ptr obj)
		: ObjectCollection(obj)
	{
	}

	//! Class Destructor
	virtual ~DiveComputerDives()
	{
	}

protected:

	//! @return List of Associated Items from the Database
	virtual std::vector<Dive::Ptr> load(DiveComputer::Ptr obj) const
	{
		IDiveFinder::Ptr df = boost::shared_dynamic_cast<IDiveFinder>(obj->session()->finder<Dive>());
		return df->findByComputer(obj->id());
	}

	//! @return Loaded Object from Database
	virtual Dive::Ptr load(boost::shared_ptr<Session> s, int64_t id) const
	{
		return s->finder<Dive>()->find(id);
	}

	//! @brief Link this Object to the Owning Object
	virtual void link(Dive::Ptr d, DiveComputer::Ptr t) const
	{
		d->setComputer(t);
	}

	//! @brief Check if the Owning Object is linked to this Object
	virtual bool linked(Dive::Ptr d, DiveComputer::Ptr t) const
	{
		return (d->computer() == t);
	}

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(Dive::Ptr d, DiveComputer::Ptr t) const
	{
		d->setComputer(boost::none);
	}

};

class DiveComputerProfiles: public ObjectCollection<Profile, DiveComputer>
{
public:

	//! Class Constructor
	DiveComputerProfiles(DiveComputer::Ptr obj)
		: ObjectCollection(obj)
	{
	}

	//! Class Destructor
	virtual ~DiveComputerProfiles()
	{
	}

protected:

	//! @return List of Associated Items from the Database
	virtual std::vector<Profile::Ptr> load(DiveComputer::Ptr obj) const
	{
		IProfileFinder::Ptr df = boost::shared_dynamic_cast<IProfileFinder>(obj->session()->finder<Profile>());
		return df->findByComputer(obj->id());
	}

	//! @return Loaded Object from Database
	virtual Profile::Ptr load(boost::shared_ptr<Session> s, int64_t id) const
	{
		return s->finder<Profile>()->find(id);
	}

	//! @brief Link this Object to the Owning Object
	virtual void link(Profile::Ptr d, DiveComputer::Ptr t) const
	{
		d->setComputer(t);
	}

	//! @brief Check if the Owning Object is linked to this Object
	virtual bool linked(Profile::Ptr d, DiveComputer::Ptr t) const
	{
		return (d->computer() == t);
	}

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(Profile::Ptr d, DiveComputer::Ptr t) const
	{
		d->setComputer(boost::none);
	}

};

DiveComputer::DiveComputer()
	: TypedPersistent<DiveComputer>()
{
}

DiveComputer::~DiveComputer()
{
}

IObjectCollection<Dive>::Ptr DiveComputer::dives()
{
	if (! m_dives)
		m_dives = IObjectCollection<Dive>::Ptr(new DiveComputerDives(boost::dynamic_pointer_cast<DiveComputer>(shared_from_this())));
	return m_dives;
}

IObjectCollection<Dive>::ConstPtr DiveComputer::dives() const
{
	if (! m_dives)
		m_dives = IObjectCollection<Dive>::Ptr(
			new DiveComputerDives(boost::dynamic_pointer_cast<DiveComputer>(
				boost::const_pointer_cast<Persistent>(shared_from_this())
			))
		);
	return m_dives;
}

IObjectCollection<Profile>::Ptr DiveComputer::profiles()
{
	if (! m_profiles)
		m_profiles = IObjectCollection<Profile>::Ptr(new DiveComputerProfiles(boost::dynamic_pointer_cast<DiveComputer>(shared_from_this())));
	return m_profiles;
}

IObjectCollection<Profile>::ConstPtr DiveComputer::profiles() const
{
	if (! m_profiles)
		m_profiles = IObjectCollection<Profile>::Ptr(
			new DiveComputerProfiles(boost::dynamic_pointer_cast<DiveComputer>(
				boost::const_pointer_cast<Persistent>(shared_from_this())
			))
		);
	return m_profiles;
}

const std::string & DiveComputer::driver() const
{
	return m_driver;
}

const boost::optional<std::string> & DiveComputer::driver_args() const
{
	return m_driver_args;
}

const boost::optional<std::string> & DiveComputer::hw_version() const
{
	return m_hw_version;
}

const boost::optional<time_t> & DiveComputer::last_transfer() const
{
	return m_last_transfer;
}

const boost::optional<std::string> & DiveComputer::manufacturer() const
{
	return m_manufacturer;
}

const boost::optional<std::string> & DiveComputer::model() const
{
	return m_model;
}

const boost::optional<std::string> & DiveComputer::name() const
{
	return m_name;
}

const boost::optional<std::string> & DiveComputer::parser() const
{
	return m_parser;
}

const boost::optional<std::string> & DiveComputer::parser_args() const
{
	return m_parser_args;
}

const std::string & DiveComputer::serial() const
{
	return m_serial;
}

const boost::optional<std::string> & DiveComputer::sw_version() const
{
	return m_sw_version;
}

const boost::optional<std::string> & DiveComputer::token() const
{
	return m_token;
}

void DiveComputer::setDriver(const std::string & value)
{
	m_driver = value;
	mark_dirty();
}

void DiveComputer::setDriverArgs(const boost::none_t &)
{
	m_driver_args.reset();
	mark_dirty();
}

void DiveComputer::setDriverArgs(const std::string & value)
{
	m_driver_args = value;
	mark_dirty();
}

void DiveComputer::setHWVersion(const boost::none_t &)
{
	m_hw_version.reset();
	mark_dirty();
}

void DiveComputer::setHWVersion(const std::string & value)
{
	m_hw_version = value;
	mark_dirty();
}

void DiveComputer::setLastTransfer(const boost::none_t &)
{
	m_last_transfer.reset();
	mark_dirty();
}

void DiveComputer::setLastTransfer(const time_t & value)
{
	m_last_transfer = value;
	mark_dirty();
}

void DiveComputer::setManufacturer(const boost::none_t &)
{
	m_manufacturer.reset();
	mark_dirty();
}

void DiveComputer::setManufacturer(const std::string & value)
{
	m_manufacturer = value;
	mark_dirty();
}

void DiveComputer::setModel(const boost::none_t &)
{
	m_model.reset();
	mark_dirty();
}

void DiveComputer::setModel(const std::string & value)
{
	m_model = value;
	mark_dirty();
}

void DiveComputer::setName(const boost::none_t &)
{
	m_name.reset();
	mark_dirty();
}

void DiveComputer::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
}

void DiveComputer::setParser(const boost::none_t &)
{
	m_parser.reset();
	mark_dirty();
}

void DiveComputer::setParser(const std::string & value)
{
	m_parser = value;
	mark_dirty();
}

void DiveComputer::setParserArgs(const boost::none_t &)
{
	m_parser_args.reset();
	mark_dirty();
}

void DiveComputer::setParserArgs(const std::string & value)
{
	m_parser_args = value;
	mark_dirty();
}

void DiveComputer::setSerial(const std::string & value)
{
	m_serial = value;
	mark_dirty();
}

void DiveComputer::setSWVersion(const boost::none_t &)
{
	m_sw_version.reset();
	mark_dirty();
}

void DiveComputer::setSWVersion(const std::string & value)
{
	m_sw_version = value;
	mark_dirty();
}

void DiveComputer::setToken(const boost::none_t &)
{
	m_token.reset();
	mark_dirty();
}

void DiveComputer::setToken(const std::string & value)
{
	m_token = value;
	mark_dirty();
}
