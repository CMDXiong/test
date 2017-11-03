
/*

  KLayout Layout Viewer
  Copyright (C) 2006-2016 Matthias Koefferlein

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/



#ifndef HDR_tlCollection
#define HDR_tlCollection

#include "config.h"

#include <iterator>

namespace tl 
{

//  HINT: SunCC wants this forward declaration:
template <class T> class Collection;

class Collectable;
class CollectionBase;

/**
 *  @brief The collection monitor
 *
 *  The collection monitor receives method calls if a collection is 
 *  added an object or removed one. A pointer to the monitor can be
 *  passed to the collection constructor.
 */
class KLAYOUT_DLL CollectionMonitor 
{
public:
  virtual ~CollectionMonitor () { }

  virtual void added (CollectionBase *collection, Collectable *obj) = 0;
  virtual void removed (CollectionBase *collection, Collectable *obj) = 0;
};

/**
 *  @brief The base class for all collections
 */
class KLAYOUT_DLL CollectionBase
{
public:
  friend class Collectable;

protected:
  //  this object can only be instantiated or destroyed by a derived class
  CollectionBase () { }
  virtual ~CollectionBase () { }

private:
  template <class X> friend class Collection;

  virtual void remove_base (Collectable *) = 0;
};

/**
 *  @brief The base class for objects that can be put into the collection
 *
 *  Each object that wants to be put into a Collection must be derived
 *  from this class:
 */
class KLAYOUT_DLL Collectable
{
protected:
  //  A collectable can only be initialized by a parent object
  Collectable ()
    : mp_next (0), mp_last (0), mp_collection (0), m_owned (false)
  {
    // .. nothing yet ..
  }

  //  A collectable can only be destroyed by a parent object
  //  HINT: in order to dynamic_cast<> from tl::Collectable to the parent
  //  class, we need at least one virtual function.
  virtual ~Collectable ()
  {
    if (mp_collection) {
      mp_collection->remove_base (this);
    }
  }

public:
  Collectable *next () 
  {
    return mp_next;
  }

  const Collectable *next () const
  {
    return mp_next;
  }

  const CollectionBase *collection () const
  {
    return mp_collection;
  }

private:
  template <class X> friend class Collection;

  Collectable *mp_next, *mp_last;
  CollectionBase *mp_collection;
  bool m_owned;
};

/**
 *  @brief An iterator over the collection
 */
template <class Obj>
class CollectionIterator
{
public:
  typedef Obj value_type;
  typedef value_type *pointer;
  typedef value_type &reference; 
  typedef std::forward_iterator_tag iterator_category;
  typedef void difference_type;

  CollectionIterator ()
    : mp_ptr (0)
  {
    //  ..  nothing else ..
  }

  CollectionIterator (Collectable *ptr)
    : mp_ptr (ptr)
  {
    //  ..  nothing else ..
  }

  bool 
  operator== (CollectionIterator<Obj> d) const
  {
    return mp_ptr == d.mp_ptr;
  }
 
  bool 
  operator!= (CollectionIterator<Obj> d) const
  {
    return mp_ptr != d.mp_ptr;
  }
 
  CollectionIterator<Obj> &
  operator++ () 
  {
    mp_ptr = mp_ptr->next ();
    return *this;
  }

  Obj *operator-> () const
  {
    return dynamic_cast<Obj *> (mp_ptr);
  }

  Obj &operator* () const
  {
    return dynamic_cast<Obj &> (*mp_ptr);
  }

private:
  Collectable *mp_ptr; 
};

/**
 *  @brief An const iterator over the collection
 */
template <class Obj>
class CollectionConstIterator
{
public:
  typedef Obj value_type;
  typedef const value_type *pointer;
  typedef const value_type &reference; 
  typedef std::forward_iterator_tag iterator_category;
  typedef void difference_type;

  CollectionConstIterator ()
    : mp_ptr (0)
  {
    //  ..  nothing else ..
  }

  CollectionConstIterator (const Collectable *ptr)
    : mp_ptr (ptr)
  {
    //  ..  nothing else ..
  }

  bool 
  operator== (CollectionConstIterator<Obj> d) const
  {
    return mp_ptr == d.mp_ptr;
  }
 
  bool 
  operator!= (CollectionConstIterator<Obj> d) const
  {
    return mp_ptr != d.mp_ptr;
  }
 
  CollectionConstIterator<Obj> &
  operator++ () 
  {
    mp_ptr = mp_ptr->next ();
    return *this;
  }

  const Obj *operator-> () const
  {
    return dynamic_cast<const Obj *> (mp_ptr);
  }

  const Obj &operator* () const
  {
    return dynamic_cast<const Obj &> (*mp_ptr);
  }

private:
  const Collectable *mp_ptr; 
};

/**
 *  @brief A collection class
 *
 *  A collection is a set of objects of which the collection
 *  knows and whose lifetime the collection can manage.
 *  The objects in the collection can be any class derived from
 *  tl::Collectable. 
 *  Objects put into the collection are owned by the collection, if
 *  inserted with the "owned" flag set.
 *  If this is deleted, these objects are deleted too.
 *  Deleting an object in the collection makes the object unknown
 *  by the collection automatically.
 */
template <class Obj>
class Collection
  : public CollectionBase
{
public:
  friend class Collectable;

  typedef CollectionIterator<Obj> iterator;
  typedef CollectionConstIterator<Obj> const_iterator;

  /**
   *  @brief The constructor
   */
  Collection (CollectionMonitor *monitor = 0)
    : mp_monitor (monitor)
  {
    m_first.mp_next = &m_last;
    m_last.mp_last = &m_first;
  }

  /**
   *  @brief The destructor: deletes all the collectable objects in it
   */
  ~Collection ()
  {
    clear ();
  }

  /**
   *  @brief Clear the collection
   */
  void clear ()
  {
    while (m_first.mp_next != &m_last) {
      Collectable *bp = m_first.mp_next;
      if (bp->m_owned) {
        delete bp;
      } else {
        unlink (bp);
      }
    }
  }

  /**
   *  @brief Insert a object in the collection
   */
  void insert (Obj *obj)
  {
    insert (obj, true);
  }

  /**
   *  @brief Insert a object in the collection
   *
   *  @param owned If set to true, the object is deleted when the collection is destroyed.
   */
  void insert (Obj *obj, bool owned)
  {
    if (obj->mp_collection) {
      obj->mp_collection->remove_base (obj);
    }

    obj->mp_collection = this;
    obj->m_owned = owned;
    obj->mp_next = &m_last;
    obj->mp_last = m_last.mp_last;
    m_last.mp_last->mp_next = obj;
    m_last.mp_last = obj;

    if (mp_monitor) {
      mp_monitor->added (this, obj);
    }
  }

  /**
   *  @brief Remove an object from the collection
   */
  void remove (Obj *obj)
  {
    remove_base (obj);
  }

  /**
   *  @brief Obtain begin iterator
   */
  iterator begin () 
  {
    return iterator (m_first.mp_next);
  }

  /**
   *  @brief Obtain end iterator
   */
  iterator end () 
  {
    return iterator (&m_last);
  }

  /**
   *  @brief Obtain begin iterator
   */
  const_iterator begin () const
  {
    return const_iterator (m_first.mp_next);
  }

  /**
   *  @brief Obtain end iterator
   */
  const_iterator end () const
  {
    return const_iterator (&m_last);
  }

private:
  Collectable m_first, m_last;
  CollectionMonitor *mp_monitor;

  void remove_base (Collectable *obj) 
  {
    if (obj->mp_collection != this) {
      return;
    }

    unlink (obj);

    if (mp_monitor) {
      mp_monitor->removed (this, obj);
    }
  }

  void unlink (Collectable *obj)
  {
    obj->mp_collection = 0;
    obj->mp_last->mp_next = obj->mp_next;
    obj->mp_next->mp_last = obj->mp_last;
    obj->mp_last = 0;
    obj->mp_next = 0;
  }
};

} // namespace tl

#endif

