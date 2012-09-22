/*
 * Copyright (C) 2012 Asymworks, LLC.  All Rights Reserved.
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

#ifndef LOGBOOK_OBJECT_COLLECTION_HPP_
#define LOGBOOK_OBJECT_COLLECTION_HPP_

#include <list>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/signals2.hpp>
#include <boost/utility.hpp>

#include <benthos/logbook/collection.hpp>
#include <benthos/logbook/persistent.hpp>
#include <benthos/logbook/proxy_object.hpp>

namespace benthos { namespace logbook {

/**
 * @brief Object Collection Template
 *
 * Templated class to hold a collection of related objects.  Uses proxy objects
 * to avoid holding extra strong references to objects.  Note: this relies on
 * the event system being wired up correctly, in particular that the linked
 * object will fire the attr_set event when link() or unlink() is called.
 */
template <class D>
class ObjectCollection: public IObjectCollection<D>,
	public Persistent::EventSource,
	public boost::noncopyable
{
public:

	/**
	 * @brief Class Constructor
	 * @param[in] Owning Object
	 * @param[in] Collection Attribute Name
	 * @param[in] Linked Object Attribute Name
	 *
	 * Creates a new Object Collection.  The collection name is the attribute
	 * name on the owning object; the linked object name is the attribute name
	 * on the linked object which refers back to the owning object.
	 *
	 * The attribute names are used to monitor and send object attribute events
	 * in response to collection changes.
	 */
	ObjectCollection(Persistent::Ptr obj, const std::string & coll_name, const std::string & link_name)
		: m_items(), m_obj(obj), m_collName(coll_name), m_linkName(link_name),
		  m_cLinkSet()
	{
		if (! obj)
			throw std::runtime_error("Owning object cannot be empty");

		if (coll_name.empty() || link_name.empty())
			throw std::runtime_error("Attribute name cannot be empty");

		m_cLinkSet = TypedPersistent<D>::ClassEvents().attr_set.connect(boost::bind(& ObjectCollection<D>::on_link_update, this, _1, _2, _3));
	}

	//! Class Destructor
	virtual ~ObjectCollection()
	{
		if (m_cLinkSet.connected())
			m_cLinkSet.disconnect();
	}

public:

	/**
	 * @brief Return the List of Related Objects
	 * @return List of Related Objects
	 *
	 * Returns the list of objects in the collection.  The sort order is set
	 * by the Finder instance used to query the database.
	 */
	virtual std::list<boost::shared_ptr<D> > all() const
	{
		std::list<boost::shared_ptr<D> > result;
		std::list<ProxyObject::Ptr>::const_iterator it;
		for (it = m_items.begin(); it != m_items.end(); it++)
			result.push_back(boost::shared_polymorphic_downcast<D>((* it)->obj()));
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
	virtual void add(boost::shared_ptr<D> item)
	{
		Persistent::Ptr obj = boost::shared_polymorphic_cast<Persistent>(item);
		std::list<ProxyObject::Ptr>::iterator it = find(obj);
		if (it != m_items.end())
			return;
		link(obj, m_obj);
	}

	/**
	 * @brief Clear the Collection
	 *
	 * Clears the collection.  Conceptually the same as calling remove() on each
	 * item in the collection.  This method always cascades updates.
	 */
	virtual void clear()
	{
		std::list<ProxyObject::Ptr>::iterator it;
		for (it = m_items.begin(); it != m_items.end(); it++)
		{
			Persistent::Ptr obj = (* it)->obj();
			if (obj)
				unlink(obj, m_obj);
		}

		m_items.clear();
	}

	//! @return Number of Items in the Collection
	virtual size_t count() const
	{
		return m_items.size();
	}

	/**
	 * @brief Load the list of Items from the Database
	 *
	 * Loads the list of related items from the database, clearing any items
	 * currently in the collection.  This should be called only once, after the
	 * collection object is constructed.  This method does not raise the
	 * attr_append or attr_remove events.
	 */
	virtual void load()
	{
		m_items.clear();
		std::vector<boost::shared_ptr<D> > items = doLoad(m_obj);
		typename std::vector<boost::shared_ptr<D> >::iterator it;
		for (it = items.begin(); it != items.end(); it++)
			m_items.push_back(ProxyObject::Ptr(new TypedProxyObject<D>(* it)));
	}

	/**
	 * @brief Remove a Related Object from the Collection
	 * @param[in] Related Object
	 *
	 * Removes a related object to the collection.  If the object is not valid
	 * or does not exist in the collection, this method is a no-op.
	 */
	virtual void remove(boost::shared_ptr<D> item)
	{
		Persistent::Ptr obj = boost::shared_polymorphic_cast<Persistent>(item);
		std::list<ProxyObject::Ptr>::iterator it = find(obj);
		if (it == m_items.end())
			return;
		unlink(obj, m_obj);
	}

public:

	/**
	 * @brief Linked Attribute Update Handler
	 * @param[in] Linked Object
	 * @param[in] Attribute Name
	 * @param[in] Attribute Value
	 *
	 * This function contains the majority of the object collection logic.  The
	 * attr_set event should be raised by the linked object when its end of the
	 * link (the single reference) is set or cleared.  The collection object
	 * then will update its collection state.
	 */
	void on_link_update(Persistent::Ptr o, const std::string & attr, const boost::any & value)
	{
		if (attr != m_linkName)
			return;

		/*
		 * Translate the value into a Persistent pointer.  If the value is not
		 * empty and not a Persistent object, throw an exception.
		 */
		Persistent::Ptr obj;
		if (! value.empty())
		{
			try
			{
				obj = boost::any_cast<Persistent::Ptr>(value);
			}
			catch (boost::bad_any_cast &)
			{
				throw std::runtime_error("Got attr_set with a non-persistent value for link attribute " + m_linkName);
			}
		}

		// See if the source object is in the collection
		std::list<ProxyObject::Ptr>::iterator it = find(o);

		if (it != m_items.end())
		{
			if (! obj || (obj != m_obj))
			{
				/*
				 * The link is either being cleared or set to another item,
				 * so remove the object from this collection
				 */
				m_items.erase(it);
				raise_attr_remove(m_obj, m_collName, boost::any(o));
			}
		}
		else
		{
			if (obj && (obj == m_obj))
			{
				/*
				 * The link was set to the owning object, so add the object to the
				 * collection
				 */
				m_items.push_back(ProxyObject::Ptr(new TypedProxyObject<D>(o)));
				raise_attr_append(m_obj, m_collName, boost::any(o));
			}
		}
	}

protected:

	//! @param[in] Persistent to Find in the Collection
	std::list<ProxyObject::Ptr>::iterator find(Persistent::Ptr obj)
	{
		std::list<ProxyObject::Ptr>::iterator it;
		for (it = m_items.begin(); it != m_items.end(); it++)
			if ((* it)->operator==(obj))
				return it;

		return m_items.end();
	}

	//! @brief Load Objects from the Database
	virtual std::vector<boost::shared_ptr<D> > doLoad(Persistent::Ptr obj) = 0;

	//! @brief Link this Object to the Owning Object
	virtual void link(Persistent::Ptr d, Persistent::Ptr obj) = 0;

	//! @brief Unlink this Object from the Owning Object
	virtual void unlink(Persistent::Ptr d, Persistent::Ptr obj) = 0;

protected:
	std::list<ProxyObject::Ptr>			m_items;		///< List of Collection Items
	Persistent::Ptr						m_obj;			///< Owning Object
	std::string							m_collName;		///< Collection Attribute Name
	std::string							m_linkName;		///< Link Attribute Name

	boost::signals2::connection			m_cLinkSet;		///< Link Attribute Update Event

};

} } /* benthos::logbook */


#endif /* LOGBOOK_OBJECT_COLLECTION_HPP_ */
