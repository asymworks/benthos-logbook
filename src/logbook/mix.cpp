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

using namespace benthos::logbook;

#ifdef _MSC_VER
#define round(var)	(std::floor(var + 0.5))
#endif

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
	events().attr_set(ptr(), "ar", boost::any(v / (double)1000));
	events().attr_set(ptr(), "ar_permil", boost::any(v));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
}

void Mix::setArPerMil(unsigned int value)
{
	if (m_fH2 + m_fHe + m_fO2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fAr = value;
	mark_dirty();
	events().attr_set(ptr(), "ar", boost::any(value / (double)1000));
	events().attr_set(ptr(), "ar_permil", boost::any(value));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
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
	events().attr_set(ptr(), "h2", boost::any(v / (double)1000));
	events().attr_set(ptr(), "h2_permil", boost::any(v));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
}

void Mix::setH2PerMil(unsigned int value)
{
	if (m_fAr + m_fHe + m_fO2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fH2 = value;
	mark_dirty();
	events().attr_set(ptr(), "h2", boost::any(value / (double)1000));
	events().attr_set(ptr(), "h2_permil", boost::any(value));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
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
	events().attr_set(ptr(), "he", boost::any(v / (double)1000));
	events().attr_set(ptr(), "he_permil", boost::any(v));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
}

void Mix::setHePerMil(unsigned int value)
{
	if (m_fAr + m_fH2 + m_fO2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fHe = value;
	mark_dirty();
	events().attr_set(ptr(), "he", boost::any(value / (double)1000));
	events().attr_set(ptr(), "he_permil", boost::any(value));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
}

void Mix::setName(const boost::none_t &)
{
	m_name.reset();
	mark_dirty();
	events().attr_set(ptr(), "name", boost::any());
}

void Mix::setName(const std::string & value)
{
	m_name = value;
	mark_dirty();
	events().attr_set(ptr(), "name", boost::any(value));
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
	events().attr_set(ptr(), "o2", boost::any(v / (double)1000));
	events().attr_set(ptr(), "o2_permil", boost::any(v));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
}

void Mix::setO2PerMil(unsigned int value)
{
	if (m_fAr + m_fHe + m_fH2 + value > 1000)
		throw std::invalid_argument("Total gas fraction may not exceed 100%");
	m_fO2 = value;
	mark_dirty();
	events().attr_set(ptr(), "o2", boost::any(value / (double)1000));
	events().attr_set(ptr(), "o2_permil", boost::any(value));
	events().attr_set(ptr(), "n2", boost::any(n2_permil() / (double)1000));
	events().attr_set(ptr(), "n2_permil", boost::any(n2_permil()));
}

std::string Mix::toString() const
{
	char buf[1000] = { 0 };

	if (m_name)
		return m_name.get();

	// Check for Pure Gasses
	if (m_fO2 == 1000)
		return "<Oxygen 100%>";
	else if (m_fH2 == 1000)
		return "<Hydrogen 100%>";
	else if (m_fHe == 1000)
		return "<Helium 100%>";
	else if (m_fAr == 1000)
		return "<Argon 100%>";
	else if ((m_fAr + m_fH2 + m_fHe + m_fO2) == 0)
		return "<Nitrogen 100%>";

	// Check for Nitrogen in the mix
	if (m_fAr + m_fH2 + m_fHe + m_fO2 < 1000)
	{
		// Nitrox
		if (m_fAr + m_fH2 + m_fHe == 0)
		{
			if (m_fO2 % 10 != 0)
			{
				sprintf(buf, "<Nitrox %.1f>", m_fO2 / (double)10);
			}
			else
			{
				sprintf(buf, "<Nitrox %d>", m_fO2 / 10);
			}
		}

		// Trimix
		else if (m_fAr + m_fH2 == 0)
		{
			if (m_fO2 % 10 != 0)
			{
				if (m_fHe % 10 != 0)
				{
					sprintf(buf, "<Trimix %.1f/%.1f>", m_fO2 / (double)10, m_fHe / (double)10);
				}
				else
				{
					sprintf(buf, "<Trimix %.1f/%d>", m_fO2 / (double)10, m_fHe / 10);
				}
			}
			else
			{
				if (m_fHe % 10 != 0)
				{
					sprintf(buf, "<Trimix %d/%.1f>", m_fO2 / 10, m_fHe / (double)10);
				}
				else
				{
					sprintf(buf, "<Trimix %d/%d>", m_fO2 / 10, m_fHe / 10);
				}
			}
		}

		// Quad-Mix or Penta-Mix
		else
		{
			sprintf(buf, "<%.1f%% O2 / %.1f%% H2 / %.1f%% He / %.1f%% Ar>", m_fO2 / (double)10, m_fH2 / (double)10, m_fHe / (double)10, m_fAr / (double)10);
		}
	}
	else
	{
		// Argox
		if (m_fAr + m_fO2 == 1000)
		{
			if (m_fO2 % 10 != 0)
			{
				sprintf(buf, "<Argox %.1f>", m_fO2 / (double)10);
			}
			else
			{
				sprintf(buf, "<Argox %d>", m_fO2 / 10);
			}
		}

		// Heliox
		else if (m_fHe + m_fO2 == 1000)
		{
			if (m_fO2 % 10 != 0)
			{
				sprintf(buf, "<Heliox %.1f>", m_fO2 / (double)10);
			}
			else
			{
				sprintf(buf, "<Heliox %d>", m_fO2 / 10);
			}
		}

		// Hydrox
		else if (m_fH2 + m_fO2 == 1000)
		{
			if (m_fO2 % 10 != 0)
			{
				sprintf(buf, "<Hydrox %.1f>", m_fO2 / (double)10);
			}
			else
			{
				sprintf(buf, "<Hydrox %d>", m_fO2 / 10);
			}
		}

		// Hydreliox
		else if (m_fH2 + m_fHe + m_fO2 == 1000)
		{
			sprintf(buf, "<Hydreliox %.1f%% O2 / %.1f%% H2 / %.1f%% He>", m_fO2 / (double)10, m_fH2 / (double)10, m_fHe / (double)10);
		}

		// Quad Mix
		else
		{
			sprintf(buf, "<%.1f%% O2 / %.1f%% H2 / %.1f%% He / %.1f%% Ar>", m_fO2 / (double)10, m_fH2 / (double)10, m_fHe / (double)10, m_fAr / (double)10);
		}
	}

	return std::string(buf);
}

