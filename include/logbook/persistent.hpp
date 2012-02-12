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

#ifndef LOGBOOK_PERSISTENT_HPP_
#define LOGBOOK_PERSISTENT_HPP_

/**
 * @file include/logbook/persistent.hpp
 * @brief Persistent Object Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

namespace logbook {

// Forward Definition of Session class
class Session;

typedef boost::shared_ptr<Session>	SessionPtr;
typedef boost::weak_ptr<Session>	SessionWPtr;

/**
 * @brief Persistent Object Class
 *
 * Base class for all Logbook objects which are able to be persisted to the
 * Logbook database.  All Persistent objects must have a 64-bit identifier
 * field which is read-only except during insert and load operations, where
 * it is set by the mapper class.  An identifier of -1 is invalid and signifies
 * an object which is not present in the database.
 *
 * Persistent objects contain a weak reference back to the underlying database
 * Session which can be used to access templated finders and other mapper
 * functionality from within the Persistent.
 *
 * Note that Persistent objects should never be instantiated as concrete
 * objects.  Rather they should always be accessed through shared pointers.
 * To enforce this paradigm, Persistent objects and their descendents should
 * always derive from boost::noncopyable.
 */
class Persistent: public boost::noncopyable,
	public boost::enable_shared_from_this<Persistent>
{
public:
	typedef boost::shared_ptr<Persistent>	Ptr;
	typedef boost::weak_ptr<Persistent>		WeakPtr;

public:

	//! Class Constructor
	Persistent();

	//! Class Destructor
	virtual ~Persistent();

public:

	//! @return Identifier
	int64_t id() const;

	//! @return If the Persistent has been Deleted
	inline bool is_deleted() const { return m_deleted; }

	//! @return If the Persistent has been Modified
	inline bool is_dirty() const { return m_dirty; }

	//! @return If the Persistent is currently Loading
	inline bool is_loading() const { return m_loading; }

	//! @return Owning Session
	SessionPtr session() const;

	//! @return Type Information for the Domain Model
	virtual const std::type_info * type_info() const = 0;

protected:

	//! Mark the Persistent as Clean
	virtual void mark_clean();

	//! Mark the Persistent as Deleted
	virtual void mark_deleted();

	//! Mark the Persistent as Dirty
	virtual void mark_dirty();

	//! Mark the Persistent as Loading
	virtual void mark_loading();

	/**
	 * @brief Set the Identifier Field
	 * @param[in] New Identifier
	 *
	 * Sets the Persistent's Identifier.  This method is used only by the
	 * mapper interface when inserting and loading Persistents.
	 */
	void set_id(int64_t value);

	/**
	 * @brief Set the Owning Session
	 * @param[in] Owning Session
	 *
	 * Sets the Persistent's Owning Session.  This method is only used by the
	 * mapper interface when inserting and loading Persistents
	 */
	void set_session(SessionPtr p);

public:

	/**
	 * @brief Persistent Access Class
	 *
	 * This class enables write access to Persistent protected information for
	 * mapper objects (clean/dirty and identifier).
	 */
	class Access
	{
	public:
		Access() { }
		virtual ~Access() { }
	public:
		void mark_persistent_clean(Ptr o) const { o->mark_clean(); }
		void mark_persistent_deleted(Ptr o) const { o->mark_deleted(); }
		void mark_persistent_dirty(Ptr o) const { o->mark_dirty(); }
		void mark_persistent_loading(Ptr o) const { o->mark_loading(); }
		void set_persistent_id(Ptr o, int64_t id) const { o->set_id(id); }
		void set_persistent_session(Ptr o, SessionPtr p) const { o->set_session(p); }
	};

private:
	bool				m_deleted;
	bool				m_dirty;
	bool				m_loading;

	int64_t				m_id;
	SessionWPtr			m_session;

};

/**
 * @brief Typed Persistent Class
 */
template <typename D>
class TypedPersistent: public Persistent
{
public:

	//! Class Constructor
	TypedPersistent(): Persistent() { }

	//! Class Destructor
	virtual ~TypedPersistent() { }

public:

	//! @return Type Info for the Mapped Class
	virtual const std::type_info * type_info() const
	{
		static const std::type_info * ti = & typeid(D);
		return ti;
	}

};

/**
 * @brief Templated Finder Interface Supertype
 *
 * Provides a templated base type for finders.  This class introduces the find()
 * and findAll() methods which find a single and all objects.
 */
template <class D>
struct IFinder
{
	typedef boost::shared_ptr<IFinder<D> >	Ptr;
	virtual ~IFinder() { }

	/**
	 * @brief Find all Mapped Objects of this Class
	 * @return List of Mapped Objects
	 */
	virtual std::vector<typename D::Ptr> find() = 0;

	/**
	 * @brief Find a single Mapped Object by Id
	 * @param[in] Object Id
	 * @return Mapped Object
	 */
	virtual typename D::Ptr find(int64_t id) = 0;

};

} /* logbook */

#endif /* LOGBOOK_PERSISTENT_HPP_ */
