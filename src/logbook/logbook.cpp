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

#include "logbook/logbook.hpp"

using namespace logbook;

Logbook::Logbook(const std::string & filename, dbapi::connection::ptr conn)
	: m_filename(filename), m_conn(conn), m_session(Session::Create(conn))
{
}

Logbook::~Logbook()
{
}

Logbook::Ptr Logbook::Create(const std::string & filename, const std::string & creator, int version)
{
	dbapi::connection::ptr db(new dbapi::connection(filename.c_str()));

	// Create Database Schema
	//TODO: Make Schema a singleton
	Schema().create(db);

	// Update Creator Information
	if (! creator.empty() && (version > 0))
	{
		dbapi::statement::ptr update_version(new dbapi::statement(db, "update version set program=?1, version=?2"));
		update_version->bind(1, creator);
		update_version->bind(2, version);
		update_version->exec();
	}

	// Create the Logbook
	Logbook::Ptr lb(new Logbook(filename, db));

	// Add Standard Mixes: Air, EANx32 and EANx36
	Mix::Ptr mAir(new Mix);
	mAir->setName("Air");
	mAir->setO2PerMil(209);

	Mix::Ptr mEANx32(new Mix);
	mEANx32->setName("EANx32");
	mEANx32->setO2PerMil(320);

	Mix::Ptr mEANx36(new Mix);
	mEANx36->setName("EANx36");
	mEANx36->setO2PerMil(360);

	lb->session()->add(mAir);
	lb->session()->add(mEANx32);
	lb->session()->add(mEANx36);
	lb->session()->commit();

	// Return new Logbook
	return lb;
}

Logbook::Ptr Logbook::Open(const std::string & filename)
{
	dbapi::connection::ptr db(new dbapi::connection(filename.c_str()));

	//TODO: Add Schema Version Check
	return Logbook::Ptr(new Logbook(filename, db));
}

void Logbook::Upgrade(const std::string & filename)
{
	//TODO: Add Schema Upgrade Logic
}
