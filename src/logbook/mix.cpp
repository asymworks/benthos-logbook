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

#include <cmath>
#include <stdexcept>

#include "benthos/logbook/mix.hpp"

using namespace logbook;

Mix::Mix()
	: TypedPersistent<Mix>(), m_fO2(0), m_fHe(0), m_fH2(0), m_fAr(0)
{
}

Mix::~Mix()
{
}

double Mix::ar() const
{
	return ((double)m_fAr) / 1000;
}

unsigned int Mix::ar_permil() const
{
	return m_fAr;
}

double Mix::h2() const
{
	return ((double)m_fH2) / 1000;
}

unsigned int Mix::h2_permil() const
{
	return m_fH2;
}

double Mix::he() const
{
	return ((double)m_fHe) / 1000;
}

unsigned int Mix::he_permil() const
{
	return m_fHe;
}

double Mix::n2() const
{
	return ((double)n2_permil()) / 1000;
}

unsigned int Mix::n2_permil() const
{
	return 1000 - (m_fAr + m_fHe + m_fH2 + m_fO2);
}

const boost::optional<std::string> & Mix::name() const
{
	return m_name;
}

double Mix::o2() const
{
	return ((double)m_fO2) / 1000;
}

unsigned int Mix::o2_permil() const
{
	return m_fO2;
}

void Mix::setAr(double value)
{
	if (value < 0)
		throw std::invalid_argument("Gas Fractions may not be negative");
	int v = (int)round(value * 1000);
	if (m_fH2 + m_fHe + m_fO2 + v > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fAr = v;
	mark_dirty();
}

void Mix::setArPerMil(unsigned int value)
{
	if (m_fH2 + m_fHe + m_fO2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fAr = value;
	mark_dirty();
}

void Mix::setH2(double value)
{
	if (value < 0)
		throw std::invalid_argument("Gas Fractions may not be negative");
	int v = (int)round(value * 1000);
	if (m_fAr + m_fHe + m_fO2 + v > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fH2 = v;
	mark_dirty();
}

void Mix::setH2PerMil(unsigned int value)
{
	if (m_fAr + m_fHe + m_fO2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fH2 = value;
	mark_dirty();
}

void Mix::setHe(double value)
{
	if (value < 0)
		throw std::invalid_argument("Gas Fractions may not be negative");
	int v = (int)round(value * 1000);
	if (m_fAr + m_fH2 + m_fO2 + v > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fHe = v;
	mark_dirty();
}

void Mix::setHePerMil(unsigned int value)
{
	if (m_fAr + m_fH2 + m_fO2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fHe = value;
	mark_dirty();
}

void Mix::setName(const boost::none_t &)
{
	m_name.reset();
	mark_dirty();
}

void Mix::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
}

void Mix::setO2(double value)
{
	if (value < 0)
		throw std::invalid_argument("Gas Fractions may not be negative");
	int v = (int)round(value * 1000);
	if (m_fAr + m_fHe + m_fH2 + v > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fO2 = v;
	mark_dirty();
}

void Mix::setO2PerMil(unsigned int value)
{
	if (m_fAr + m_fHe + m_fH2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fO2 = value;
	mark_dirty();
}

