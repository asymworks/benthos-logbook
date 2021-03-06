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

#ifndef LOGBOOK_MIX_HPP_
#define LOGBOOK_MIX_HPP_

/**
 * @file include/benthos/logbook/mix.hpp
 * @brief Gas Mix Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/persistent.hpp>

namespace benthos { namespace logbook {

/**
 * @brief Gas Mixture Model Class
 *
 * Represents a breathing gas as a mixture of oxygen, helium, hydrogen, argon
 * and nitrogen.  The O2, He, H2 and Ar percentages are specified and the
 * remainder is assumed to be N2.
 *
 * Internally this class uses fixed-point arithmetic and stores gas fractions
 * in mils (0.1%).  Use the setXXPerMil() setter methods to ensure accuracy
 * when changing gas fractions.  Double-precision helper methods are also
 * provided; however, these methods may reduce accuracy due to rounding.
 *
 * The total gas fraction (O2 + He + H2 + Ar) may not exceed 100%.
 */
class Mix: public TypedPersistent<Mix>
{
public:
	typedef boost::shared_ptr<Mix>	Ptr;
	typedef boost::weak_ptr<Mix>	WeakPtr;

public:

	//! Class Constructor
	Mix();

	//! Class Destructor
	virtual ~Mix();

public:

	/**
	 * @brief Return a String Representation of the Gas Mix
	 * @return String
	 *
	 * Returns the Mix Name, if set, otherwise returns an auto-generated
	 * human-readable name.
	 */
	std::string toString() const;

public:

	//! @return Fraction of Argon
	double ar() const;

	//! @return Fraction of Argon in mils
	unsigned int ar_permil() const;

	//! @return Fraction of Hydrogen
	double h2() const;

	//! @return Fraction of Hydrogen in mils
	unsigned int h2_permil() const;

	//! @return Fraction of Helium
	double he() const;

	//! @return Fraction of Helium in mils
	unsigned int he_permil() const;

	//! @return Fraction of Nitrogen
	double n2() const;

	//! @return Fraction of Nitrogen in mils
	unsigned int n2_permil() const;

	//! @return Gas Mixture Name
	const boost::optional<std::string> & name() const;

	//! @return Fraction of Oxygen
	double o2() const;

	//! @return Fraction of Oxygen in mils
	unsigned int o2_permil() const;

	//! @param[in] Fraction of Argon
	void setAr(double value);

	//! @param[in] Fraction of Argon in mils
	void setArPerMil(unsigned int value);

	//! @param[in] Fraction of Hydrogen
	void setH2(double value);

	//! @param[in] Fraction of Hydrogen in mils
	void setH2PerMil(unsigned int value);

	//! @param[in] Fraction of Helium
	void setHe(double value);

	//! @param[in] Fraction of Helium in mils
	void setHePerMil(unsigned int value);

	//! @brief Set the Gas Mixture Name to NULL
	void setName(const boost::none_t &);

	//! @param[in] Gas Mixture Name
	void setName(const std::string & value);

	//! @param[in] Fraction of Oxygen
	void setO2(double value);

	//! @param[in] Fraction of Oxygen in mils
	void setO2PerMil(unsigned int value);

private:
	boost::optional<std::string>	m_name;		///< Mix Name
	unsigned int					m_fO2;		///< Oxygen Fraction
	unsigned int					m_fHe;		///< Helium Fraction
	unsigned int					m_fH2;		///< Hydrogen Fraction
	unsigned int					m_fAr;		///< Argon Fraction

};

/**
 * @brief Breathing Mix Finder Interface
 *
 * Extends IFinder<Mix> to add a find-by-name method and a find-by-gas method.
 * The Find-by-gas method is used by the profile loader to find the "best mix"
 * given a O2 and He fraction.
 */
struct IMixFinder: public IFinder<Mix>
{
	typedef boost::shared_ptr<IMixFinder>	Ptr;
	virtual ~IMixFinder() { }

	/**
	 * @brief Find a Breathing Mix by Name (case insensitive)
	 * @param[in] Mix Name
	 * @return Breathing Mix
	 */
	virtual Mix::Ptr findByName(const std::string & name) = 0;

	/**
	 * @brief Find a Breathing Mix by O2/He fraction
	 * @param[in] O2 Per Mil
	 * @param[in] He Per Mil
	 * @return Breathing Mix
	 *
	 * Finds a breathing mix that is within 0.5% of the given mix.  If multiple
	 * mixes are within 0.5%, this will choose the one with the highest oxygen
	 * fraction.
	 */
	virtual Mix::Ptr findByMix(unsigned int pmO2, unsigned int pmHe) = 0;

};

} } /* benthos::logbook */

#endif /* LOGBOOK_MIX_HPP_ */
