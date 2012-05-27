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

#ifndef LOGBOOK_DIVE_COMPUTER_HPP_
#define LOGBOOK_DIVE_COMPUTER_HPP_

/**
 * @file include/benthos/logbook/dive_computer.hpp
 * @brief Dive Computer Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>
#include <ctime>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/collection.hpp>
#include <benthos/logbook/persistent.hpp>

namespace benthos { namespace logbook {

// Forward Declaration of Dive and Profile
class Dive;
class Profile;

/**
 * @brief Dive Computer Model Class
 */
class DiveComputer: public TypedPersistent<DiveComputer>
{
public:
	typedef boost::shared_ptr<DiveComputer>	Ptr;
	typedef boost::weak_ptr<DiveComputer>	WeakPtr;

public:

	//! Class Constructor
	DiveComputer();

	//! Class Destructor
	virtual ~DiveComputer();

public:

	//! @return Dive Collection
	IObjectCollection<Dive>::Ptr dives();

	//! @return Dive Collection
	IObjectCollection<Dive>::ConstPtr dives() const;

	//! @return Dive Collection
	IObjectCollection<Profile>::Ptr profiles();

	//! @return Dive Collection
	IObjectCollection<Profile>::ConstPtr profiles() const;

public:

	//! @return Driver Name
	const std::string & driver() const;

	//! @return Driver Arguments
	const boost::optional<std::string> & driver_args() const;

	//! @return Hardware Version
	const boost::optional<std::string> & hw_version() const;

	//! @return Last Transfer Time
	const boost::optional<time_t> & last_transfer() const;

	//! @return Manufacturer Name
	const boost::optional<std::string> & manufacturer() const;

	//! @return Model Name
	const boost::optional<std::string> & model() const;

	//! @return User-Defined Name
	const boost::optional<std::string> & name() const;

	//! @return Parser Name
	const boost::optional<std::string> & parser() const;

	//! @return Parser Arguments
	const boost::optional<std::string> & parser_args() const;

	//! @return Serial Number
	const std::string & serial() const;

	//! @return Software Version
	const boost::optional<std::string> & sw_version() const;

	//! @return Transfer Token
	const boost::optional<std::string> & token() const;

	//! @param[in] Driver Name
	void setDriver(const std::string & value);

	//! @brief Set the Driver Arguments to NULL
	void setDriverArgs(const boost::none_t &);

	//! @param[in] Driver Arguments
	void setDriverArgs(const std::string & value);

	//! @brief Set the Hardware Version to NULL
	void setHWVersion(const boost::none_t &);

	//! @param[in] Hardware Version
	void setHWVersion(const std::string & value);

	//! @brief Set the Last Transfer Time to NULL
	void setLastTransfer(const boost::none_t &);

	//! @param[in] Last Transfer Time
	void setLastTransfer(const time_t & value);

	//! @brief Set the Manufacturer Name to NULL
	void setManufacturer(const boost::none_t &);

	//! @param[in] Manufacturer Name
	void setManufacturer(const std::string & value);

	//! @brief Set the Model Name to NULL
	void setModel(const boost::none_t &);

	//! @param[in] Model Name
	void setModel(const std::string & value);

	//! @brief Set the User-Defined Name to NULL
	void setName(const boost::none_t &);

	//! @param[in] User-Defined Name
	void setName(const std::string & value);

	//! @brief Set Parser Name to NULL
	void setParser(const boost::none_t &);

	//! @param[in] Parser Name
	void setParser(const std::string & value);

	//! @brief Set the Parser Arguments to NULL
	void setParserArgs(const boost::none_t &);

	//! @param[in] Parser Arguments
	void setParserArgs(const std::string & value);

	//! @param[in] Serial Number
	void setSerial(const std::string & value);

	//! @brief Set the Software Version to NULL
	void setSWVersion(const boost::none_t &);

	//! @param[in] Software Version
	void setSWVersion(const std::string & value);

	//! @brief Set the Transfer Token to NULL
	void setToken(const boost::none_t &);

	//! @param[in] Transfer Token
	void setToken(const std::string & value);

private:
	std::string						m_serial;			///< Serial Number
	std::string						m_driver;			///< Driver Name
	boost::optional<std::string>	m_parser;			///< Parser Name
	boost::optional<std::string>	m_token;			///< Token
	boost::optional<time_t>			m_last_transfer;	///< Last Transfer
	boost::optional<std::string>	m_driver_args;		///< Driver Arguments
	boost::optional<std::string>	m_parser_args;		///< Parser Arguments

	boost::optional<std::string>	m_name;				///< User-Defined Name
	boost::optional<std::string>	m_manufacturer;		///< Manufacturer Name
	boost::optional<std::string>	m_model;			///< Model Name
	boost::optional<std::string>	m_hw_version;		///< Hardware Version String
	boost::optional<std::string>	m_sw_version;		///< Software Version String

	mutable IObjectCollection<Dive>::Ptr	m_dives;	///< Dive Collection
	mutable IObjectCollection<Profile>::Ptr	m_profiles;	///< Profile Collection

};

/**
 * @brief Dive Computer Finder Interface
 *
 * Extends IFinder<DiveComputer> to add a find-by-serial method, which is used
 * mainly for the dive computer transfer operation.
 */
struct IDiveComputerFinder: public IFinder<DiveComputer>
{
	typedef boost::shared_ptr<IDiveComputerFinder>	Ptr;
	virtual ~IDiveComputerFinder() { }

	/**
	 * @brief Find a Dive Computer by Driver/Serial Number
	 * @param[in] Driver Name
	 * @param[in] Serial Number
	 * @return Dive Computer
	 */
	virtual DiveComputer::Ptr findBySerial(const std::string & driver, const std::string & serial) = 0;

};

} } /* benthos::logbook */

#endif /* LOGBOOK_DIVE_COMPUTER_HPP_ */
