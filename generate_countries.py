#!/usr/bin/env python2
# -*- coding: utf-8 -*-

"""
regenerate_countries.py
Adapted from django_countries: 
https://bitbucket.org/smileychris/django-countries/overview
"""

import os
import re
import string
import urllib2

TEMPLATE = u'''/*
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

/*
 * This file is generated automatically by generate_countries.py
 */

#include "logbook/countries.hpp"

class _libdata
{
public:
	country_map_t _COUNTRIES;
	country_map_t _COUNTRIES_PLUS;
	country_map_t _OFFICIAL_COUNTRIES;

	_libdata()
	{
%(countries)s
%(countries_plus)s
%(official_countries)s
	}
};

_libdata __libdata;

const country_map_t & ISO3166_COUNTRIES = __libdata._COUNTRIES;
const country_map_t & ISO3166_COUNTRIES_PLUS = __libdata._COUNTRIES_PLUS;
const country_map_t & ISO3166_OFFICIAL_COUNTRIES = __libdata._OFFICIAL_COUNTRIES;

'''
OFFICIAL_COUNTRIES_LINE = u'		_OFFICIAL_COUNTRIES["%(code)s"] = %(name)s;'
COUNTRIES_PLUS_LINE = u'		_COUNTRIES_PLUS["%(code)s"] = %(name)s;'
COUNTRIES_LINE = u'		_COUNTRIES["%(code)s"] = %(name)s;'
RE_VALID_LINE = re.compile(r'\s*(?P<name>.+);(?P<code>[A-Z]{2})\s*$')
RE_ACRONYM = re.compile(r'\b[A-Z](\.[A-Za-z])+\b')
RE_MC = re.compile(r'\b(Mc)(\w)')
RE_FIX_STRLIT = re.compile(r'(\\x[0-9A-Fa-f]{2})([0-9A-Fa-f])')

def _cmp_value(value):
    """
    Ensure the countries are sorted correctly by replacing unicode characters
    with the basic English letter equivalent.

    """
    value = value.replace(u'\xe9', 'e').replace(u'\xf4', 'o')
    if value.startswith(u'\xc5'):
        value = 'A%s' % value[1:]
    return value


def c_unicodestr(value):
	"""
	Generate a C-style Unicode String Constant from the Python Unicode String
	"""
	#return '"%s"' % (repr(value)[2:-1])
	
	utf8 = repr(value.encode('utf-8'))[1:-1]
	match = RE_FIX_STRLIT.search(utf8)
	if match:
		was_ = '%s%s' % (match.group(1), match.group(2))
		is_ = '%s""%s' % (match.group(1), match.group(2))
		utf8 = utf8.replace(was_, is_)
	return '"%s"' % (utf8)

def regenerate(location='http://www.iso.org/iso/list-en1-semic-3.txt',
               filename=None, default_encoding='ISO-8859-1'):
    """
    Generate the countries Python module from a semicolon delimited file.
    
    """
    # Get the country list.
    data = urllib2.urlopen(location)
    official_countries = []
    if ('content-type' in data.headers and
                'charset=' in data.headers['content-type']):
        encoding = data.headers['content-type'].split('charset=')[-1]
    else:
        encoding = default_encoding
    for line in data:
        match = RE_VALID_LINE.match(unicode(line, encoding))
        if not match:
            continue
        country = match.groupdict()
        official_countries.append((str(country['code']), country['name']))

    # Generate template output (and the nicely titled country names).
    official_countries_lines = []
    countries_lines = []
    countries_plus = []
    for code, name in official_countries:
        country_data = {'code': code, 'name': name}
        country_data2 = {'code': code, 'name': c_unicodestr(name)}
        official_countries_lines.append(OFFICIAL_COUNTRIES_LINE % country_data2)
        name = string.capwords(name)
        name = RE_ACRONYM.sub(lambda match: match.group().upper(), name)
        if ', ' in name:
            important, rest = name.split(', ', 1)
            important = important.title()
            # Temporarily add on a space so titlecase doesn't think that ending
            # shortwords should be titled.
            rest = ('%s ' % rest)[:-1].title()
            plus_name = '%s %s' % (rest, important)
            plus_name = RE_MC.sub(lambda match: '%s%s' %
                                     (match.group(1), match.group(2).upper()),
                                  plus_name)
            countries_plus.append((c_unicodestr(plus_name), code))
            name = '%s, %s' % (important, rest)
        else:
            name = name.title()
        name = RE_MC.sub(lambda match: '%s%s' % (match.group(1),
                                                 match.group(2).upper()), name)
        country_data['name'] = c_unicodestr(name)
        countries_lines.append(COUNTRIES_LINE % country_data)
        countries_plus.append((c_unicodestr(name), code))
    # Order the countries_plus list of countries.
    countries_plus.sort(cmp=lambda a, b: cmp((_cmp_value(a[0]), a[1]),
                                             (_cmp_value(b[0]), b[1])))
    countries_plus_lines = []
    for name, code in countries_plus:
        country_data = {'code': code, 'name': name}
        countries_plus_lines.append(COUNTRIES_PLUS_LINE % country_data)

    # Generate and save the file.
    if not filename:
        filename = 'src/logbook/countries.cpp'
    # TODO: first make a backup of the file if it exists already.
    f = open(filename, 'w')
    f.write(TEMPLATE % {
        'official_countries': '\n'.join(official_countries_lines),
        'countries': '\n'.join(countries_lines),
        'countries_plus': '\n'.join(countries_plus_lines)
    })
    f.close()


if __name__ == '__main__':
    # TODO: use parseopt for location / filename
    regenerate()
