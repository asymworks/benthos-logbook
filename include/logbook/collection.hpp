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
 * @file include/logbook/collection.hpp
 * @brief Object Collection/Relation Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstring>
#include <list>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <logbook/persistent.hpp>

namespace logbook {

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
	 * @param[in] Cascade
	 *
	 * Adds a new related object to the collection.  If the object is not valid
	 * or already exists in the collection, this method is a no-op.  If the
	 * cascade parameter is set to true, the corresponding setter will be called
	 * for the related object's Many-To-One property.
	 */
	virtual void add(boost::shared_ptr<D> item, bool cascade = true) = 0;

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
	 * @brief Check if the Collection is in a Cascaded Update
	 * @return True if the Collection is in a Cascaded Update
	 *
	 * This method checks to see whether the collection is currently performing
	 * a cascaded update of a Many-to-One relation.  This method is used by the
	 * Many-to-One relation setter on the remote object to prevent loops.
	 */
	virtual bool in_cascade() const = 0;

	/**
	 * @brief Remove a Related Object from the Collection
	 * @param[in] Related Object
	 * @param[in] Cascade
	 *
	 * Removes a related object to the collection.  If the object is not valid
	 * or does not exist in the collection, this method is a no-op.  If the
	 * cascade parameter is set to true, the corresponding setter will be called
	 * for the related object's Many-To-One property.
	 */
	virtual void remove(boost::shared_ptr<D> item, bool cascade = true) = 0;

};

/**
 * @brief Related Object Collection Template
 *
 * Templated partial implementation of IObjectCollection.  Uses a lazy-load
 * strategy to load database entries.
 */
template <class D, class T>
class ObjectCollection: public IObjectCollection<D>,
	public boost::noncopyable
{
public:

	//! Class Constructor
	ObjectCollection(typename T::Ptr obj)
		: m_obj(obj), m_items(), m_loaded(false), m_cascade(false)
	{
	}

	//! Class Destructor
	virtual ~ObjectCollection()
	{
	}

public:

	/**
	 * @brief Return the List of Related Objects
	 * @return List of Related Objects
	 *
	 * Returns the list of objects in the collection.  The sort order is set
	 * by the Finder instance used to query the database.
	 */
	virtual std::list<typename D::Ptr> all() const
	{
		if (! m_loaded)
		{
			typename T::Ptr pobj = m_obj.lock();

			if (! pobj)
				throw std::runtime_error("Collection is not bound to an object");

			if (pobj->session())
			{
				std::vector<typename D::Ptr> items;

				try
				{
					m_cascade = true;
					items = load(pobj);
					m_cascade = false;
				}
				catch (std::exception & e)
				{
					m_cascade = false;
					throw;
				}

				typename std::vector<typename D::Ptr>::iterator itl;
				for (itl = items.begin(); itl != items.end(); itl++)
					m_items.push_back((typename D::WeakPtr)(* itl));
			}

			m_loaded = true;
		}

		std::list<typename D::Ptr> result;
		typename std::list<typename D::WeakPtr>::iterator it;
		for (it = m_items.begin(); it != m_items.end(); it++)
			result.push_back(it->lock());

		return result;
	}

	/**
	 * @brief Add a Related Object to the Collection
	 * @param[in] Related Object
	 * @param[in] Cascade
	 *
	 * Adds a new related object to the collection.  If the object is not valid
	 * or already exists in the collection, this method is a no-op.  If the
	 * cascade parameter is set to true, the corresponding setter will be called
	 * for the related object's Many-To-One property.
	 */
	virtual void add(typename D::Ptr item, bool cascade = true)
	{
		if (find(item) != m_items.end())
			return;

		typename T::Ptr pobj = m_obj.lock();
		if (! pobj)
			throw std::runtime_error("Collection is not bound to an object");

		if (item->session() && pobj->session() && (item->session() != pobj->session()))
			throw std::runtime_error("Item belongs to a different session");

		m_items.push_back((typename D::WeakPtr)(item));

		if (cascade)
		{
			try
			{
				m_cascade = true;
				link(item, pobj);
				m_cascade = false;
			}
			catch (std::exception & e)
			{
				m_cascade = false;
				throw;
			}
		}
	}

	/**
	 * @brief Clear the Collection
	 *
	 * Clears the collection.  Conceptually the same as calling remove() on each
	 * item in the collection.  This method always cascades updates.
	 */
	virtual void clear()
	{
		typename std::list<typename D::WeakPtr>::iterator it;

		typename T::Ptr pobj = m_obj.lock();
		if (! pobj)
			throw std::runtime_error("Collection is not bound to an object");

		try
		{
			m_cascade = true;
			for (it = m_items.begin(); it != m_items.end(); )
			{
				typename D::Ptr item = it->lock();
				if (item)
					unlink(item, pobj);
				it = m_items.erase(it);
			}
			m_cascade = false;
		}
		catch (std::exception & e)
		{
			m_cascade = false;
			throw;
		}
	}

	//! @return Number of Items in the Collection
	virtual size_t count() const
	{
		return m_items.size();
	}

	/**
	 * @brief Check if the Collection is in a Cascaded Update
	 * @return True if the Collection is in a Cascaded Update
	 *
	 * This method checks to see whether the collection is currently performing
	 * a cascaded update of a Many-to-One relation.  This method is used by the
	 * Many-to-One relation setter on the remote object to prevent loops.
	 */
	virtual bool in_cascade() const
	{
		return m_cascade;
	}

	/**
	 * @brief Remove a Related Object from the Collection
	 * @param[in] Related Object
	 * @param[in] Cascade
	 *
	 * Removes a related object to the collection.  If the object is not valid
	 * or does not exist in the collection, this method is a no-op.  If the
	 * cascade parameter is set to true, the corresponding setter will be called
	 * for the related object's Many-To-One property.
	 */
	virtual void remove(typename D::Ptr item, bool cascade = true)
	{
		typename std::list<typename D::WeakPtr>::iterator it = find(item);
		if (it == m_items.end())
			return;

		typename T::Ptr pobj = m_obj.lock();
		if (! pobj)
			throw std::runtime_error("Collection is not bound to an object");

		m_items.erase(it);

		if (cascade)
		{
			try
			{
				m_cascade = true;
				unlink(item, pobj);
				m_cascade = false;
			}
			catch (std::exception & e)
			{
				m_cascade = false;
				throw;
			}
		}
	}

protected:

	//! @return Iterator of Associated Item
	virtual typename std::list<typename D::WeakPtr>::iterator find(typename D::Ptr item) const
	{
		typename std::list<typename D::WeakPtr>::iterator it;
		for (it = m_items.begin(); it != m_items.end(); it++)
			if (it->lock() == item)
				return it;
		return m_items.end();
	}

	//! @return List of Associated Items from the Database
	virtual std::vector<typename D::Ptr> load(typename T::Ptr obj) const = 0;

	//! @brief Link this Object to the Owning Object
	virtual void link(typename D::Ptr d, typename T::Ptr t) const = 0;

	//! @brief Check if the Owning Object is linked to this Object
	virtual bool linked(typename D::Ptr d, typename T::Ptr t) const = 0;

	//! @brief Unlink this Object to the Owning Object
	virtual void unlink(typename D::Ptr d, typename T::Ptr t) const = 0;

private:
	typename T::WeakPtr						m_obj;
	mutable std::list<typename D::WeakPtr>	m_items;
	mutable bool							m_loaded;
	mutable bool							m_cascade;

};

//! @brief Case-Insensitive String Compare
struct cicmp
{
	bool operator() (const std::string & lhs, const std::string & rhs) const
	{
		return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
	}
};

} /* logbook */

#endif /* COLLECTION_HPP_ */
