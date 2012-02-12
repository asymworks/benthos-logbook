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

#include "dive_mapper.hpp"

#include <logbook/profile.hpp>
#include <logbook/session.hpp>

using namespace logbook;
using namespace logbook::mappers;

std::string DiveMapper::columns = "id, dive_datetime, dive_utcoffset, dive_number, "
		"site_id, computer_id, repetition, interval, duration, max_depth, avg_depth, "
		"air_temp, max_temp, min_temp, px_start, px_end, mix_id, salinity, comments, "
		"rating, safety_stop, stop_depth, stop_time, weight, visibility_cat, "
		"visibility_dist, pg_start, pg_end, rnt, desat, nofly, algorithm";

std::string DiveMapper::sql_insert = "insert into dives values (?1, ?2, ?3, ?4, ?5, ?6, "
		"?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20, ?21, ?22, "
		"?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30, ?31, ?32)";
std::string DiveMapper::sql_update = "update dives set dive_datetime=?2, dive_utcoffset=?3, "
		"dive_number=?4, site_id=?5, computer_id=?6, repetition=?7, interval=?8, "
		"duration=?9, max_depth=?10, avg_depth=?11, air_temp=?12, max_temp=?13, min_temp=?14, "
		"px_start=?15, px_end=?16, mix_id=?17, salinity=?18, comments=?19, rating=?20, "
		"safety_stop=?21, stop_depth=?22, stop_time=?23, weight=?24, visibility_cat=?25, "
		"visibility_dist=?26, pg_start=?27, pg_end=?28, rnt=?29, desat=?30, nofly=?31, "
		"algorithm=?32 where id=?1";
std::string DiveMapper::sql_delete = "delete from dives where id=?1";

std::string DiveMapper::sql_find_all = "select " + columns + " from dives";
std::string DiveMapper::sql_find_id = "select " + columns + " from dives where id=?1";
std::string DiveMapper::sql_find_site = "select " + columns + " from dives where site_id=?1";
std::string DiveMapper::sql_find_cpu = "select " + columns + " from dives where computer_id=?1";

std::string DiveMapper::sql_find_tags = "select tag from divetags where dive_id=?1 order by tag asc";
std::string DiveMapper::sql_drop_tags = "delete from divetags where dive_id=?1";
std::string DiveMapper::sql_add_tags = "insert into divetags values (?1, ?2)";
std::string DiveMapper::sql_all_tags = "select distinct(tag) from divetags order by tag asc";

DiveMapper::DiveMapper(boost::shared_ptr<Session> session)
	: Mapper<Dive>(session)
{
	m_insert_stmt = statement::ptr(new statement(m_conn, sql_insert));
	m_update_stmt = statement::ptr(new statement(m_conn, sql_update));
	m_delete_stmt = statement::ptr(new statement(m_conn, sql_delete));

	m_find_all_stmt = statement::ptr(new statement(m_conn, sql_find_all));
	m_find_id_stmt = statement::ptr(new statement(m_conn, sql_find_id));
	m_find_site_stmt = statement::ptr(new statement(m_conn, sql_find_site));
	m_find_cpu_stmt = statement::ptr(new statement(m_conn, sql_find_cpu));

	m_find_tags_stmt = statement::ptr(new statement(m_conn, sql_find_tags));
	m_drop_tags_stmt = statement::ptr(new statement(m_conn, sql_drop_tags));
	m_add_tags_stmt = statement::ptr(new statement(m_conn, sql_add_tags));
	m_all_tags_stmt = statement::ptr(new statement(m_conn, sql_all_tags));
}

DiveMapper::~DiveMapper()
{
}

void DiveMapper::afterDelete(Persistent::Ptr o, int64_t oldId)
{
	m_drop_tags_stmt->reset();
	m_drop_tags_stmt->bind(1, oldId);
	m_drop_tags_stmt->exec();
}

void DiveMapper::afterInsert(Persistent::Ptr o)
{
	Dive::Ptr d = downcast(o);

	std::list<std::string> tags = d->tags()->all();
	std::list<std::string>::const_iterator it;
	for (it = tags.begin(); it != tags.end(); it++)
	{
		m_add_tags_stmt->reset();
		m_add_tags_stmt->bind(1, d->id());
		m_add_tags_stmt->bind(2, * it);
		m_add_tags_stmt->exec();
	}
}

void DiveMapper::afterLoaded(Persistent::Ptr o)
{
	Dive::Ptr d = downcast(o);

	m_find_tags_stmt->reset();
	m_find_tags_stmt->bind(1, d->id());

	dbapi::cursor::ptr c = m_all_tags_stmt->exec();
	std::vector<cursor::row_t> rs = c->fetchall();
	std::vector<cursor::row_t>::const_iterator it;
	for (it = rs.begin(); it != rs.end(); it++)
		d->tags()->add((* it)[0].as<std::string>());
}

void DiveMapper::afterUpdate(Persistent::Ptr o)
{
	m_drop_tags_stmt->reset();
	m_drop_tags_stmt->bind(1, o->id());
	m_drop_tags_stmt->exec();

	afterInsert(o);
}

std::vector<std::string> DiveMapper::allTags() const
{
	std::vector<std::string> result;

	dbapi::cursor::ptr c = m_all_tags_stmt->exec();
	std::vector<cursor::row_t> rs = c->fetchall();
	std::vector<cursor::row_t>::const_iterator it;
	for (it = rs.begin(); it != rs.end(); it++)
		result.push_back((*it)[0].as<std::string>());

	return result;
}

void DiveMapper::bindInsert(statement::ptr s, Persistent::Ptr p) const
{
	Dive::Ptr o = downcast(p);

	s->bind(2, o->datetime());
	s->bind(3, o->utc_offset());
	s->bind(4, o->number());

	if (o->site())
		s->bind(5, o->site()->id());
	else
		s->bind(5, boost::none);

	if (o->computer())
		s->bind(6, o->computer()->id());
	else
		s->bind(6, boost::none);

	s->bind(7, o->repetition());
	s->bind(8, o->interval());
	s->bind(9, o->duration());
	s->bind(10, o->max_depth());
	s->bind(11, o->avg_depth());
	s->bind(12, o->air_temp());
	s->bind(13, o->max_temp());
	s->bind(14, o->min_temp());
	s->bind(15, o->start_pressure());
	s->bind(16, o->end_pressure());

	if (o->mix())
		s->bind(17, o->mix()->id());
	else
		s->bind(17, boost::none);

	s->bind(18, o->salinity());
	s->bind(19, o->comments());
	s->bind(20, o->rating());
	s->bind(21, o->safety_stop() ? 1 : 0);
	s->bind(22, o->stop_depth());
	s->bind(23, o->stop_time());
	s->bind(24, o->weight());
	s->bind(25, o->visibility_category());
	s->bind(26, o->visibility_distance());
	s->bind(27, o->start_pressure_group());
	s->bind(28, o->end_pressure_group());
	s->bind(29, o->rnt());
	s->bind(30, o->desat_time());
	s->bind(31, o->nofly_time());
	s->bind(32, o->algorithm());
}

void DiveMapper::bindUpdate(statement::ptr s, Persistent::Ptr p) const
{
	Dive::Ptr o = downcast(p);

	s->bind(2, o->datetime());
	s->bind(3, o->utc_offset());
	s->bind(4, o->number());

	if (o->site())
		s->bind(5, o->site()->id());
	else
		s->bind(5, boost::none);

	if (o->computer())
		s->bind(6, o->computer()->id());
	else
		s->bind(6, boost::none);

	s->bind(7, o->repetition());
	s->bind(8, o->interval());
	s->bind(9, o->duration());
	s->bind(10, o->max_depth());
	s->bind(11, o->avg_depth());
	s->bind(12, o->air_temp());
	s->bind(13, o->max_temp());
	s->bind(14, o->min_temp());
	s->bind(15, o->start_pressure());
	s->bind(16, o->end_pressure());

	if (o->mix())
		s->bind(17, o->mix()->id());
	else
		s->bind(17, boost::none);

	s->bind(18, o->salinity());
	s->bind(19, o->comments());
	s->bind(20, o->rating());
	s->bind(21, o->safety_stop() ? 1 : 0);
	s->bind(22, o->stop_depth());
	s->bind(23, o->stop_time());
	s->bind(24, o->weight());
	s->bind(25, o->visibility_category());
	s->bind(26, o->visibility_distance());
	s->bind(27, o->start_pressure_group());
	s->bind(28, o->end_pressure_group());
	s->bind(29, o->rnt());
	s->bind(30, o->desat_time());
	s->bind(31, o->nofly_time());
	s->bind(32, o->algorithm());
}

std::list<Persistent::Ptr> DiveMapper::cascade_add(Persistent::Ptr p)
{
	std::list<Persistent::Ptr> result;
	Dive::Ptr o = downcast(p);

	if (! o)
		return result;

	std::list<Profile::Ptr> profiles = o->profiles()->all();
	result.assign(profiles.begin(), profiles.end());

	if (o->computer())
		result.push_back(o->computer());
	if (o->site())
		result.push_back(o->site());

	return result;
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

Dive::Ptr DiveMapper::doLoad(int64_t id, cursor::row_t r) const
{
	Dive::Ptr o(new logbook::Dive);

	mark_persistent_loading(o);

	IFinder<DiveComputer>::Ptr cmp_finder(m_session.lock()->finder<DiveComputer>());
	IFinder<DiveSite>::Ptr site_finder(m_session.lock()->finder<DiveSite>());
	IFinder<Mix>::Ptr mix_finder(m_session.lock()->finder<Mix>());

	set_persistent_id(o, id);
	o->setDateTime(r[1].as<time_t>());
	SET_VARIANT(o, setUTCOffset, r[2], int);
	SET_VARIANT(o, setNumber, r[3], int);

	if (r[4].is_null())
		o->setSite(boost::none);
	else
		o->setSite(site_finder->find(r[4].as<int64_t>()));

	if (r[5].is_null())
		o->setComputer(boost::none);
	else
		o->setComputer(cmp_finder->find(r[5].as<int64_t>()));

	o->setRepetition(r[6].as<int>());
	o->setInterval(r[7].as<int>());
	o->setDuration(r[8].as<int>());
	o->setMaxDepth(r[9].as<double>());
	SET_VARIANT(o, setAvgDepth, r[10], double);
	SET_VARIANT(o, setAirTemp, r[11], double);
	SET_VARIANT(o, setMaxTemp, r[12], double);
	SET_VARIANT(o, setMinTemp, r[13], double);
	SET_VARIANT(o, setStartPressure, r[14], double);
	SET_VARIANT(o, setEndPressure, r[15], double);

	if (r[16].is_null())
		o->setMix(boost::none);
	else
		o->setMix(mix_finder->find(r[16].as<int64_t>()));

	SET_VARIANT(o, setSalinity, r[17], std::string);
	SET_VARIANT(o, setComments, r[18], std::string);
	SET_VARIANT(o, setRating, r[19], int);

	o->setSafetyStop(r[20].as<int>() == 1);
	SET_VARIANT(o, setStopDepth, r[21], double);
	SET_VARIANT(o, setStopTime, r[22], int);
	SET_VARIANT(o, setWeight, r[23], double);
	SET_VARIANT(o, setVisibilityCategory, r[24], std::string);
	SET_VARIANT(o, setVisibilityDistance, r[25], double);
	SET_VARIANT(o, setStartPressureGroup, r[26], std::string);
	SET_VARIANT(o, setEndPressureGroup, r[27], std::string);
	SET_VARIANT(o, setRNT, r[28], int);
	SET_VARIANT(o, setDesatTime, r[29], int);
	SET_VARIANT(o, setNoFlyTime, r[30], int);
	SET_VARIANT(o, setAlgorithm, r[31], std::string);

	return o;
}

std::vector<Dive::Ptr> DiveMapper::find()
{
	m_find_all_stmt->reset();
	dbapi::cursor::ptr c = m_find_all_stmt->exec();

	return loadAll(c);
}

Dive::Ptr DiveMapper::find(int64_t id)
{
	m_find_id_stmt->reset();
	m_find_id_stmt->bind(1, id);

	dbapi::cursor::ptr c = m_find_id_stmt->exec();
	dbapi::cursor::row_t r = c->fetchone();
	if (r.size() == 0)
		return logbook::Dive::Ptr();

	return load(r);
}

std::vector<Dive::Ptr> DiveMapper::findByComputer(int64_t computer_id)
{
	m_find_cpu_stmt->reset();
	m_find_cpu_stmt->bind(1, computer_id);
	dbapi::cursor::ptr c = m_find_cpu_stmt->exec();

	return loadAll(c);
}

std::vector<Dive::Ptr> DiveMapper::findBySite(int64_t site_id)
{
	m_find_site_stmt->reset();
	m_find_site_stmt->bind(1, site_id);
	dbapi::cursor::ptr c = m_find_site_stmt->exec();

	return loadAll(c);
}
