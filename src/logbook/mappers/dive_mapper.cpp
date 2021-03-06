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

#include <benthos/logbook/dive_tank.hpp>
#include <benthos/logbook/profile.hpp>
#include <benthos/logbook/session.hpp>

using namespace benthos::logbook;
using namespace benthos::logbook::mappers;

std::string DiveMapper::columns = "id, dive_datetime, dive_utcoffset, dive_number, "
		"site_id, computer_id, repetition, interval, duration, max_depth, avg_depth, "
		"air_temp, max_temp, min_temp, px_start, px_end, mix_id, tank_id, salinity, comments, "
		"rating, safety_stop, stop_depth, stop_time, weight, visibility_cat, "
		"visibility_dist, pg_start, pg_end, rnt, desat, nofly, algorithm";

std::string DiveMapper::sql_insert = "insert into dives values (?1, ?2, ?3, ?4, ?5, ?6, "
		"?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15, ?16, ?17, ?18, ?19, ?20, ?21, ?22, "
		"?23, ?24, ?25, ?26, ?27, ?28, ?29, ?30, ?31, ?32, ?33)";
std::string DiveMapper::sql_update = "update dives set dive_datetime=?2, dive_utcoffset=?3, "
		"dive_number=?4, site_id=?5, computer_id=?6, repetition=?7, interval=?8, "
		"duration=?9, max_depth=?10, avg_depth=?11, air_temp=?12, max_temp=?13, min_temp=?14, "
		"px_start=?15, px_end=?16, mix_id=?17, tank_id=?18, salinity=?19, comments=?20, rating=?21, "
		"safety_stop=?22, stop_depth=?23, stop_time=?24, weight=?25, visibility_cat=?26, "
		"visibility_dist=?27, pg_start=?28, pg_end=?29, rnt=?30, desat=?31, nofly=?32, "
		"algorithm=?33 where id=?1";
std::string DiveMapper::sql_delete = "delete from dives where id=?1";

std::string DiveMapper::sql_find_all = "select " + columns + " from dives";
std::string DiveMapper::sql_find_id = "select " + columns + " from dives where id=?1";
std::string DiveMapper::sql_find_site = "select " + columns + " from dives where site_id=?1";
std::string DiveMapper::sql_find_cpu = "select " + columns + " from dives where computer_id=?1";

std::string DiveMapper::sql_find_ctry = "select " + columns + " from dives where site_id in (select id from sites where country=?1)";
std::string DiveMapper::sql_find_dates = "select " + columns + " from dives where dive_datetime >= ?1 and dive_datetime <= ?2";

std::string DiveMapper::sql_find_recent = "select " + columns + " from dives where id in (select distinct dive_id from profiles where imported is not null and dive_id is not null and imported >= ?1 order by imported desc limit ?2)";

std::string DiveMapper::sql_count_site = "select count(*) from dives where site_id=?1";
std::string DiveMapper::sql_count_cpu = "select count(*) from dives where computer_id=?1";

std::string DiveMapper::sql_avgrating = "select avg(rating) from dives where site_id=?1";
std::string DiveMapper::sql_maxdepth = "select max(max_depth) from dives where site_id=?1";
std::string DiveMapper::sql_avgdepth = "select avg(max_depth) from dives where site_id=?1";
std::string DiveMapper::sql_avgtemp = "select avg(min_temp) from dives where site_id=?1";

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

	m_find_ctry_stmt = statement::ptr(new statement(m_conn, sql_find_ctry));
	m_find_dates_stmt = statement::ptr(new statement(m_conn, sql_find_dates));

	m_find_recent_stmt = statement::ptr(new statement(m_conn, sql_find_recent));

	m_count_site_stmt = statement::ptr(new statement(m_conn, sql_count_site));
	m_count_cpu_stmt = statement::ptr(new statement(m_conn, sql_count_cpu));

	m_avgrating_stmt = statement::ptr(new statement(m_conn, sql_avgrating));
	m_maxdepth_stmt = statement::ptr(new statement(m_conn, sql_maxdepth));
	m_avgdepth_stmt = statement::ptr(new statement(m_conn, sql_avgdepth));
	m_avgtemp_stmt = statement::ptr(new statement(m_conn, sql_avgtemp));

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

	dbapi::cursor::ptr c = m_find_tags_stmt->exec();
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
	bindUpdate(s, p);
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

	if (o->tank())
		s->bind(18, o->tank()->id());
	else
		s->bind(18, boost::none);

	s->bind(19, o->salinity());
	s->bind(20, o->comments());
	s->bind(21, o->rating());
	s->bind(22, o->safety_stop() ? 1 : 0);
	s->bind(23, o->stop_depth());
	s->bind(24, o->stop_time());
	s->bind(25, o->weight());
	s->bind(26, o->visibility_category());
	s->bind(27, o->visibility_distance());
	s->bind(28, o->start_pressure_group());
	s->bind(29, o->end_pressure_group());
	s->bind(30, o->rnt());
	s->bind(31, o->desat_time());
	s->bind(32, o->nofly_time());
	s->bind(33, o->algorithm());
}

std::list<Persistent::Ptr> DiveMapper::cascade_add(Persistent::Ptr p)
{
	std::list<Persistent::Ptr> result;
	Dive::Ptr o = downcast(p);

	if (! o)
		return result;

	std::list<Profile::Ptr> profiles = o->profiles()->all();
	result.assign(profiles.begin(), profiles.end());

	std::list<DiveTank::Ptr> tanks = o->tanks()->all();
	result.insert(result.end(), tanks.begin(), tanks.end());

	if (o->computer())
		result.push_back(o->computer());
	if (o->site())
		result.push_back(o->site());

	return result;
}

std::list<Persistent::Ptr> DiveMapper::cascade_delete(Persistent::Ptr p)
{
	std::list<Persistent::Ptr> result;
	Dive::Ptr o = downcast(p);

	if (! o)
		return result;

	std::list<DiveTank::Ptr> tanks = o->tanks()->all();
	result.assign(tanks.begin(), tanks.end());

	return result;
}

unsigned int DiveMapper::countByComputer(int64_t computer_id) const
{
	m_count_cpu_stmt->reset();
	m_count_cpu_stmt->bind(1, computer_id);
	variant res = m_count_cpu_stmt->exec_scalar();
	int ires = res.get<int>();
	if (ires <= 0)
		return 0;
	return (unsigned int)(ires);
}

unsigned int DiveMapper::countBySite(int64_t site_id) const
{
	m_count_site_stmt->reset();
	m_count_site_stmt->bind(1, site_id);
	variant res = m_count_site_stmt->exec_scalar();
	int ires = res.get<int>();
	if (ires <= 0)
		return 0;
	return (unsigned int)(ires);
}

#define SET_VARIANT(o, f, v, t) if ((v).is_null()) o->f(boost::none); else o->f(v.as<t>())

Dive::Ptr DiveMapper::doLoad(int64_t id, cursor::row_t r) const
{
	Dive::Ptr o(new logbook::Dive);

	mark_persistent_loading(o);

	IFinder<DiveComputer>::Ptr cmp_finder(m_session.lock()->finder<DiveComputer>());
	IFinder<DiveSite>::Ptr site_finder(m_session.lock()->finder<DiveSite>());
	IFinder<Mix>::Ptr mix_finder(m_session.lock()->finder<Mix>());
	IFinder<Tank>::Ptr tank_finder(m_session.lock()->finder<Tank>());

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

	if (r[17].is_null())
		o->setTank(boost::none);
	else
		o->setTank(tank_finder->find(r[17].as<int64_t>()));

	SET_VARIANT(o, setSalinity, r[18], std::string);
	SET_VARIANT(o, setComments, r[19], std::string);
	SET_VARIANT(o, setRating, r[20], int);

	o->setSafetyStop(r[21].as<int>() == 1);
	SET_VARIANT(o, setStopDepth, r[22], double);
	SET_VARIANT(o, setStopTime, r[23], int);
	SET_VARIANT(o, setWeight, r[24], double);
	SET_VARIANT(o, setVisibilityCategory, r[25], std::string);
	SET_VARIANT(o, setVisibilityDistance, r[26], double);
	SET_VARIANT(o, setStartPressureGroup, r[27], std::string);
	SET_VARIANT(o, setEndPressureGroup, r[28], std::string);
	SET_VARIANT(o, setRNT, r[29], int);
	SET_VARIANT(o, setDesatTime, r[30], int);
	SET_VARIANT(o, setNoFlyTime, r[31], int);
	SET_VARIANT(o, setAlgorithm, r[32], std::string);

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

std::vector<Dive::Ptr> DiveMapper::findRecentlyImported(unsigned int days, int max)
{
	m_find_recent_stmt->reset();
	m_find_recent_stmt->bind(1, std::time(NULL) - (86400 * days));
	m_find_recent_stmt->bind(2, max);
	dbapi::cursor::ptr c = m_find_recent_stmt->exec();

	return loadAll(c);
}

std::vector<Dive::Ptr> DiveMapper::findByComputer(int64_t computer_id)
{
	m_find_cpu_stmt->reset();
	m_find_cpu_stmt->bind(1, computer_id);
	dbapi::cursor::ptr c = m_find_cpu_stmt->exec();

	return loadAll(c);
}

std::vector<Dive::Ptr> DiveMapper::findByCountry(const country & country_)
{
	m_find_ctry_stmt->reset();
	m_find_ctry_stmt->bind(1, country_.code());
	dbapi::cursor::ptr c = m_find_ctry_stmt->exec();

	return loadAll(c);
}

std::vector<Dive::Ptr> DiveMapper::findByDates(time_t start, time_t end)
{
	m_find_dates_stmt->reset();
	m_find_dates_stmt->bind(1, start);
	m_find_dates_stmt->bind(2, end);
	dbapi::cursor::ptr c = m_find_dates_stmt->exec();

	return loadAll(c);
}

std::vector<Dive::Ptr> DiveMapper::findBySite(int64_t site_id)
{
	m_find_site_stmt->reset();
	m_find_site_stmt->bind(1, site_id);
	dbapi::cursor::ptr c = m_find_site_stmt->exec();

	return loadAll(c);
}

boost::optional<double> DiveMapper::maxDepthForSite(int64_t site_id) const
{
	m_maxdepth_stmt->reset();
	m_maxdepth_stmt->bind(1, site_id);
	variant res = m_maxdepth_stmt->exec_scalar();
	if (res.is_null())
		return boost::optional<double>();
	return boost::optional<double>(res.as<double>());
}

boost::optional<double> DiveMapper::avgDepthForSite(int64_t site_id) const
{
	m_avgdepth_stmt->reset();
	m_avgdepth_stmt->bind(1, site_id);
	variant res = m_avgdepth_stmt->exec_scalar();
	if (res.is_null())
		return boost::optional<double>();
	return boost::optional<double>(res.as<double>());
}

boost::optional<double> DiveMapper::avgTempForSite(int64_t site_id) const
{
	m_avgtemp_stmt->reset();
	m_avgtemp_stmt->bind(1, site_id);
	variant res = m_avgtemp_stmt->exec_scalar();
	if (res.is_null())
		return boost::optional<double>();
	return boost::optional<double>(res.as<double>());
}

boost::optional<double> DiveMapper::ratingForSite(int64_t site_id) const
{
	m_avgrating_stmt->reset();
	m_avgrating_stmt->bind(1, site_id);
	variant res = m_avgrating_stmt->exec_scalar();
	if (res.is_null())
		return boost::optional<double>();
	return boost::optional<double>(res.as<double>());
}
