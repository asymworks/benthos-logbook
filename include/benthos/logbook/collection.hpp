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

#ifndef LOGBOOK_COLLECTION_HPP_
#define LOGBOOK_COLLECTION_HPP_

/**
 * @file include/benthos/logbook/collection.hpp
 * @brief Object Collection/Relation Interface
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstring>
#include <list>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <benthos/logbook/persistent.hpp>

namespace benthos { namespace logbook {

/**
 * @brief Related Object Collection Interface
 *
 * Defines an interface to Many-to-Many and One-To-Many object collections.
 * Items may be queried, added and removed.  The collection does not own the
 * contained objects; rather, weak pointers are used internally and converted
 * on add/remove and load to shared pointers.
 */
template <class D>
struct IObjectCollection
{
	typedef boost::shared_ptr<IObjectCollection<D> > Ptr;
	typedef boost::shared_ptr<const IObjectCollection<D> > ConstPtr;

	virtual ~IObjectCollection() { }

	/**
	 * @brief Return the List of Related Objects
	 * @return List of Related Objects
	 *
	 * Returns the list of objects in the collection.  The sort order is set
	 * by the Finder instance used to query the database.
	 */
	virtual std::list<boost::shared_ptr<D> > all() const = 0;

	/**
	 * @brief Add a Related Object to the Collection
	 * @param[in] Related Object
	 *
	 * Adds a new related object to the collection.  If the object is not valid
	 * or already exists in the collection, this method is a no-op.
	 */
	virtual void add(boost::shared_ptr<D> item) = 0;

	/**
	 * @brief Clear the Collection
	 *
	 * Clears the collection.  Conceptually the same as calling remove() on each
	 * item in the collection.  This method always cascades updates.
	 */
	virtual void clear() = 0;

	//! @return Number of Items in the Collection
	virtual size_t count() const = 0;

	/**
	 * @brief Load the list of Items from the Database
	 *
	 * Loads the list of related items from the database, clearing any items
	 * currently in the collection.  This should be called only once, after the
	 * collection object is constructed.  This method does not raise the
	 * attr_append or attr_remove events.
	 */
	virtual void load() = 0;

	/**
	 * @brief Remove a Related Object from the Collection
	 * @param[in] Related Object
	 *
	 * Removes a related object to the collection.  If the object is not valid
	 * or does not exist in the collection, this method is a no-op.
	 */
	virtual void remove(boost::shared_ptr<D> item) = 0;

};

} } /* benthos::logbook */

#endif /* COLLECTION_HPP_ */
