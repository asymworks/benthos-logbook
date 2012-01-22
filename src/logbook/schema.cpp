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

#include "logbook/schema.hpp"

using namespace logbook;

#define SCHEMA_VERSION	1

Schema::Schema()
{
}

Schema::~Schema()
{
}

void Schema::create(dbapi::connection::ptr conn) const
{
	create_version_tbl(conn);
	create_computers_tbl(conn);
	create_sites_tbl(conn);
	create_mixes_tbl(conn);
	create_dives_tbl(conn);
	create_divetags_tbl(conn);
	create_profiles_tbl(conn);
}

void Schema::drop(dbapi::connection::ptr conn) const
{
	conn->exec_sql("drop table computers");
	conn->exec_sql("drop table dives");
	conn->exec_sql("drop table divetags");
	conn->exec_sql("drop table mixes");
	conn->exec_sql("drop table profiles");
	conn->exec_sql("drop table sites");
	conn->exec_sql("drop table version");
}

void Schema::upgrade(dbapi::connection::ptr conn, int version) const
{

}

void Schema::downgrade(dbapi::connection::ptr conn, int version) const
{

}

void Schema::create_computers_tbl(dbapi::connection::ptr conn) const
{
	conn->exec_sql("create table computers ("
		"id integer primary key, "
		"driver varchar not null, "
		"serial varchar not null, "
		"parser varchar, "
		"token varchar, "
		"last_transfer datetime, "
		"driver_args varchar, "
		"parser_args varchar, "
		"name varchar, "
		"manufacturer varchar, "
		"model varchar, "
		"hw_version varchar, "
		"sw_version varchar "
	")");

	conn->exec_sql("create unique index computers_device "
		"on computers ("
			"driver, serial"
		")");
}

void Schema::create_dives_tbl(dbapi::connection::ptr conn) const
{
	conn->exec_sql("create table dives ("
		"id integer primary key, "
		"dive_datetime datetime not null, "
		"dive_utcoffset integer, "
		"dive_number integer, "
		"site_id integer, "
		"computer_id integer, "
		"repetition integer not null default 1, "
		"interval integer not null default 0, "
		"duration integer not null check (duration > 0), "
		"max_depth float not null check (max_depth > 0), "
		"avg_depth float, "
		"air_temp float, "
		"max_temp float, "
		"min_temp float, "
		"px_start float, "
		"px_end float, "
		"mix_id integer, "
		"salinity text check (salinity in (\"fresh\",\"salt\")), "
		"comments text, "
		"rating integer, "
		"safety_stop integer check (safety_stop in (0,1)), "
		"stop_depth float check (stop_depth > 0), "
		"stop_time integer check (stop_time > 0), "
		"weight float, "
		"visibility_cat varchar check (visibility_cat in (\"excellent\",\"good\",\"fair\",\"poor\")), "
		"visibility_dist integer check (visibility_dist > 0), "
		"pg_start varchar, "
		"pg_end varchar, "
		"rnt_start integer check (rnt_start > 0), "
		"desat integer, "
		"nofly integer, "
		"algorithm varchar, "
		"foreign key (site_id) references sites(id) on delete set null deferrable initially deferred, "
		"foreign key (computer_id) references computers(id) on delete set null deferrable initially deferred, "
		"foreign key (mix_id) references mixes(id) on delete set null deferrable initially deferred"
	")");

	conn->exec_sql("create index dive_site on dives (site_id)");
	conn->exec_sql("create index dive_computer on dives (computer_id)");
	conn->exec_sql("create index dive_mix on dives (mix_id)");

	conn->exec_sql("create index dive_datetime on dives (dive_datetime)");
	conn->exec_sql("create index dive_number on dives (dive_number)");
}

void Schema::create_divetags_tbl(dbapi::connection::ptr conn) const
{
	conn->exec_sql("create table divetags ("
		"dive_id integer not null, "
		"tag varchar not null, "
		"foreign key (dive_id) references dives(id) on delete cascade deferrable initially deferred"
	")");

	conn->exec_sql("create unique index divetags_index on divetags (dive_id, tag)");
	conn->exec_sql("create index divetags_dive on divetags (dive_id)");
	conn->exec_sql("create index divetags_tag on divetags (tag)");
}

void Schema::create_mixes_tbl(dbapi::connection::ptr conn) const
{
	conn->exec_sql("create table mixes ("
		"id integer primary key, "
		"name varchar, "
		"o2 integer not null default 0, "
		"he integer not null default 0, "
		"h2 integer not null default 0, "
		"ar integer not null default 0, "
		"check (o2 + he + h2 + ar <= 1000)"
	")");
}

void Schema::create_profiles_tbl(dbapi::connection::ptr conn) const
{
	conn->exec_sql("create table profiles ("
		"id integer primary key, "
		"dive_id integer, "
		"computer_id integer, "
		"name varchar, "
		"profile text, "
		"vendor text, "
		"imported datetime, "
		"raw_profile blob, "
		"foreign key (dive_id) references dives(id) on delete set null deferrable initially deferred, "
		"foreign key (computer_id) references computers(id) on delete set null deferrable initially deferred"
	")");

	conn->exec_sql("create unique index profiles_device on profiles (dive_id, computer_id)");
	conn->exec_sql("create index profiles_dive on profiles (dive_id)");
	conn->exec_sql("create index profiles_computer on profiles (computer_id)");
}

void Schema::create_sites_tbl(dbapi::connection::ptr conn) const
{
	conn->exec_sql("create table sites ("
		"id integer primary key, "
		"name varchar not null, "
		"place varchar, "
		"country varchar, "
		"latitude float, "
		"longitude float, "
		"platform varchar, "
		"waterbody varchar, "
		"bottom varchar, "
		"salinity varchar check (salinity in (\"fresh\",\"salt\")), "
		"timezone varchar, "
		"comments text"
	")");
}

void Schema::create_version_tbl(dbapi::connection::ptr conn) const
{
	// Create the Table
	conn->exec_sql("create table version ("
		"program varchar, "
		"version integer, "
		"schema integer not null"
	")");

	// Fill in the singleton row with the creator, library version and schema version
	dbapi::statement::ptr s(new dbapi::statement(conn, "insert into version values (?1, ?2, ?3)"));
	s->bind(1);
	s->bind(2);
	s->bind(3, SCHEMA_VERSION);
	s->exec();

	// Create the Insert/Delete Constraints to ensure only one row
	conn->exec_sql("create trigger version_insert "
		"before insert on version "
		"begin "
			"select raise(ABORT, 'Cannot insert rows into table \"version\"'); "
		"end");

	conn->exec_sql("create trigger version_delete "
		"before delete on version "
		"begin "
			"select raise(ABORT, 'Cannot delete rows from table \"version\"');"
		"end");
}


