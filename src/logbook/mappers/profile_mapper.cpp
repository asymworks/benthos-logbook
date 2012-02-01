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

#include "profile_mapper.hpp"

#include <sstream>

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

#include <logbook/session.hpp>

using namespace logbook;
using namespace logbook::mappers;

std::string ProfileMapper::columns = "id, dive_id, computer_id, name, profile, vendor, imported, raw_profile";

std::string ProfileMapper::sql_insert = "insert into profiles values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8)";
std::string ProfileMapper::sql_update = "update profiles set dive_id=?2, computer_id=?3, name=?4, "
		"profile=?5, vendor=?6, imported=?7, raw_profile=?8 where id=?1";
std::string ProfileMapper::sql_delete = "delete from profiles where id=?1";

std::string ProfileMapper::sql_find_all = "select " + columns + " from profiles";
std::string ProfileMapper::sql_find_id = "select " + columns + " from profiles where id=?1";
std::string ProfileMapper::sql_find_dive = "select " + columns + " from profiles where dive_id=?1";
std::string ProfileMapper::sql_find_computer = "select " + columns + " from profiles where computer_id=?1";

ProfileMapper::ProfileMapper(boost::shared_ptr<Session> session)
	: Mapper<Profile>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
	m_find_dive_stmt = statement::ptr(new statement(m_conn, sql_find_dive));
	m_find_computer_stmt = statement::ptr(new statement(m_conn, sql_find_computer));
}

ProfileMapper::~ProfileMapper()
{
}

void ProfileMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	Profile::Ptr o = downcast(p);

	if (o->dive())
		s->bind(2, o->dive()->id());
	else
		s->bind(2);

	if (o->computer())
		s->bind(3, o->computer()->id());
	else
		s->bind(3);

	if (o->profile().empty())
		s->bind(5);
	else
		s->bind(5, profileToJSON(o->profile()));

	s->bind(4, o->name());
	s->bind(6, o->vendor());
	s->bind(7, o->imported());
	s->bind(8, o->raw_profile());
}

void ProfileMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	Profile::Ptr o = downcast(p);

	if (o->dive())
		s->bind(2, o->dive()->id());
	else
		s->bind(2);

	if (o->computer())
		s->bind(3, o->computer()->id());
	else
		s->bind(3);

	if (o->profile().empty())
		s->bind(5);
	else
		s->bind(5, profileToJSON(o->profile()));

	s->bind(4, o->name());
	s->bind(6, o->vendor());
	s->bind(7, o->imported());
	s->bind(8, o->raw_profile());
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t >())

Profile::Ptr ProfileMapper::doLoad(int64_t id, cursor::row_t r) const
{
	Profile::Ptr o(new logbook::Profile);

	IFinder<Dive>::Ptr dive_finder(m_session.lock()->finder<Dive>());
	IFinder<DiveComputer>::Ptr cmp_finder(m_session.lock()->finder<DiveComputer>());

	set_persistent_id(o, id);

	if (r[1].is_null())
		o->setDive(boost::none);
	else
		o->setDive(dive_finder->find(r[1].as<int64_t>()));

	if (r[2].is_null())
		o->setComputer(boost::none);
	else
		o->setComputer(cmp_finder->find(r[2].as<int64_t>()));

	if (r[4].is_null())
		o->setProfile(boost::none);
	else
		o->setProfile(profileFromJSON(r[4].as<std::string>()));

	SET_VARIANT(o, setName, r[3], std::string);
	SET_VARIANT(o, setVendor, r[5], std::string);
	SET_VARIANT(o, setImported, r[6], time_t);
	SET_VARIANT(o, setRawProfile, r[7], std::vector<unsigned char>);

	return o;
}

std::vector<Profile::Ptr> ProfileMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

Profile::Ptr ProfileMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Profile::Ptr();

	return load(r);
}

std::vector<Profile::Ptr> ProfileMapper::findByDive(int64_t dive_id)
{
	m_find_dive_stmt->reset();
	m_find_dive_stmt->bind(1, dive_id);
	dbapi::cursor::ptr c = m_find_dive_stmt->exec();

	return loadAll(c);
}

std::vector<Profile::Ptr> ProfileMapper::findByComputer(int64_t computer_id)
{
	m_find_computer_stmt->reset();
	m_find_computer_stmt->bind(1, computer_id);
	dbapi::cursor::ptr c = m_find_computer_stmt->exec();

	return loadAll(c);
}

struct pfj_parse_context
{
	IMixFinder::Ptr			mfinder;
	std::list<waypoint> *	profile;

	waypoint				cur_wp;
	std::string				cur_key;
	std::string				err;

	unsigned int pmO2;
	unsigned int pmHe;

	bool isDone;
	bool inProfile;
	bool inWaypoint;
	bool inMix;
	bool readO2;
	bool readHe;
};

int pfj_parse_int(void * ctx, long long i)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	if (! jctx->inProfile || ! jctx->inWaypoint || jctx->isDone || jctx->cur_key.empty())
	{
		jctx->err = "Unexpected integer";
		return 0;
	}

	if (jctx->cur_key == "time")
	{
		jctx->cur_wp.time = i;
		jctx->cur_key.clear();
		return 1;
	}

	if (jctx->cur_key == "alarms")
	{
		jctx->err = "Found integer value for alarms";
		return 0;
	}

	jctx->cur_wp.data[jctx->cur_key] = i;
	jctx->cur_key.clear();
	return 1;
}

int pfj_parse_dbl(void * ctx, double d)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	if (! jctx->inProfile || ! jctx->inWaypoint || jctx->isDone || jctx->cur_key.empty())
	{
		jctx->err = "Unexpected double";
		return 0;
	}

	if (jctx->cur_key == "time")
	{
		jctx->err = "Found floating point value for time";
		return 0;
	}

	if (jctx->cur_key == "alarms")
	{
		jctx->err = "Found floating point value for alarms";
		return 0;
	}

	if (jctx->inMix)
	{
		if (! jctx->readO2)
		{
			jctx->pmO2 = (int)floor(d * 1000);
			jctx->readO2 = true;
			return 1;
		}
		else if (! jctx->readHe)
		{
			jctx->pmHe = (int)floor(d * 1000);
			jctx->readHe = true;
			return 1;
		}
		else
		{
			jctx->err = "Too many values in a Mix item";
			return 0;
		}
	}

	jctx->cur_wp.data[jctx->cur_key] = d;
	jctx->cur_key.clear();
	return 1;
}

int pfj_parse_str(void * ctx, const unsigned char * stringVal, size_t stringLen)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	if (! jctx->inProfile || ! jctx->inWaypoint || jctx->isDone || jctx->cur_key.empty())
	{
		jctx->err = "Unexpected string";
		return 0;
	}

	if (jctx->cur_key != "alarms")
	{
		jctx->err = "Found string for non-alarm value";
		return 0;
	}

	char * buffer = (char *)malloc(stringLen + 1);
	memcpy(buffer, stringVal, stringLen);
	buffer[stringLen] = '\0';

	char * alarm = strtok(buffer, ",");
	if (! alarm)
	{
		jctx->cur_wp.alarms.clear();
		jctx->cur_key.clear();
		return 1;
	}

	while (alarm)
	{
		jctx->cur_wp.alarms.insert(alarm);
		alarm = strtok(NULL, ",");
	}

	jctx->cur_key.clear();
	return 1;
}

int pfj_start_map(void * ctx)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	// This only happens between waypoints
	if (! jctx->inProfile || jctx->inWaypoint || jctx->isDone)
	{
		jctx->err = "Unexpected start of map";
		return 0;
	}

	jctx->inWaypoint = true;
	return 1;
}

int pfj_map_key(void * ctx, const unsigned char * stringVal, size_t stringLen)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	char * buffer = (char *)malloc(stringLen + 1);
	memcpy(buffer, stringVal, stringLen);
	buffer[stringLen] = '\0';

	std::string key(buffer);
	std::transform(key.begin(), key.end(), key.begin(), tolower);

	jctx->cur_key = buffer;
	return 1;
}

int pfj_end_map(void * ctx)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	jctx->profile->push_back(jctx->cur_wp);

	jctx->cur_wp.alarms.clear();
	jctx->cur_wp.data.clear();
	jctx->cur_wp.mix.reset();
	jctx->cur_wp.time = 0;

	jctx->inWaypoint = false;
	return 1;
}

int pfj_start_array(void * ctx)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	// This should only occur at the beginning or in a mix key
	if ((jctx->inProfile || jctx->isDone) && ! (jctx->inWaypoint && (jctx->cur_key == "mix")))
	{
		jctx->err = "Unexpected start of array";
		return 0;
	}

	if (jctx->inWaypoint)
	{
		jctx->inMix = true;
		jctx->readO2 = false;
		jctx->readHe = false;
		jctx->pmO2 = 0;
		jctx->pmHe = 0;

		return 1;
	}

	jctx->inProfile = true;
	return 1;
}

int pfj_end_array(void * ctx)
{
	pfj_parse_context * jctx = (pfj_parse_context *)(ctx);

	if (jctx->inMix)
	{
		if (! jctx->readO2)
		{
			jctx->err = "Too few values in a Mix item";
			return 0;
		}

		if (! jctx->readHe)
		{
			jctx->pmHe = 0;
		}

		Mix::Ptr mix = jctx->mfinder->findByMix(jctx->pmO2, jctx->pmHe);
		if (! mix)
		{
			//NOTE: THIS DOES NOT ADD THE NEW MIX TO THE SESSION!

			mix = Mix::Ptr(new Mix);
			mix->setO2PerMil(jctx->pmO2);
			mix->setHePerMil(jctx->pmHe);
		}

		jctx->cur_wp.mix = mix;

		jctx->inMix = false;
		jctx->readO2 = false;
		jctx->readHe = false;
	}
	else
	{
		jctx->inProfile = false;
		jctx->isDone = true;
	}

	return 1;
}

static yajl_callbacks pfj_cb = {
	NULL,
	NULL,
	pfj_parse_int,
	pfj_parse_dbl,
	NULL,
	pfj_parse_str,
	pfj_start_map,
	pfj_map_key,
	pfj_end_map,
	pfj_start_array,
	pfj_end_array
};

std::string ProfileMapper::profileToJSON(const std::list<waypoint> & profile) const
{
	yajl_gen g = yajl_gen_alloc(NULL);
	yajl_gen_config(g, yajl_gen_beautify, 0);
	yajl_gen_config(g, yajl_gen_validate_utf8, 1);

	yajl_gen_array_open(g);

	std::list<waypoint>::const_iterator it;
	for (it = profile.begin(); it != profile.end(); it++)
	{
		yajl_gen_map_open(g);

		// Write the Time
		yajl_gen_string(g, (const unsigned char *)"time", 4);
		yajl_gen_integer(g, it->time);

		// Write the Mix (O2 and He only)
		if (it->mix)
		{
			yajl_gen_string(g, (const unsigned char *)"mix", 3);
			yajl_gen_array_open(g);
			yajl_gen_double(g, it->mix->o2());
			yajl_gen_double(g, it->mix->he());
			yajl_gen_array_close(g);
		}

		// Write the Alarms
		if (! it->alarms.empty())
		{
			std::stringstream ss;

			std::set<std::string, cicmp>::const_iterator ait = it->alarms.begin();
			ss << (* (ait++));
			for ( ; ait != it->alarms.end(); ait++)
			{
				ss << "," << (* ait);
			}

			std::string alarms = ss.str();

			yajl_gen_string(g, (const unsigned char *)"alarms", 6);
			yajl_gen_string(g, (const unsigned char *)alarms.c_str(), alarms.size());
		}

		// Write the Data
		std::map<std::string, double, cicmp>::const_iterator dit;
		for (dit = it->data.begin(); dit != it->data.end(); dit++)
		{
			yajl_gen_string(g, (const unsigned char *)(dit->first.c_str()), dit->first.size());
			yajl_gen_double(g, dit->second);
		}

		yajl_gen_map_close(g);
	}

	yajl_gen_array_close(g);

	const unsigned char * buf;
	size_t buflen;

	yajl_gen_get_buf(g, & buf, & buflen);
	std::string result((const char *)buf);
	yajl_gen_clear(g);

	return result;
}

std::list<waypoint> ProfileMapper::profileFromJSON(const std::string & json) const
{
	std::list<waypoint> profile;
	pfj_parse_context ctx;

	ctx.mfinder = boost::dynamic_pointer_cast<IMixFinder>(m_session.lock()->finder<Mix>());
	ctx.profile = & profile;
	ctx.isDone = false;
	ctx.inProfile = false;
	ctx.inWaypoint = false;
	ctx.inMix = false;
	ctx.readO2 = false;
	ctx.readHe = false;

	ctx.cur_key = "";
	ctx.err = "";

	yajl_handle hand = yajl_alloc(& pfj_cb, NULL, & ctx);
	yajl_status stat = yajl_parse(hand, (const unsigned char *)json.c_str(), json.size());

	if (stat != yajl_status_ok)
	{
		if (stat == yajl_status_client_canceled)
		{
			if (ctx.err.empty())
				throw std::runtime_error("Failed to load JSON profile data: Unknown Error");
			else
				throw std::runtime_error("Failed to load JSON profile data: " + ctx.err);
		}
		else
		{
			std::string yajl_err(yajl_status_to_string(stat));
			throw std::runtime_error("Failed to parse JSON profile data: " + yajl_err);
		}
	}

	return profile;
}
