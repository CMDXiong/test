
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


#ifndef HDR_tlObserver
#define HDR_tlObserver

#include "config.h"

#include "gsiObject.h"

#include <list>

namespace tl
{

class Observed;
class Observer;

typedef std::list<Observed *> observed_list_type;
typedef std::list<Observer *> observer_list_type;

/**
 *  @brief A simple observer pattern implementation: the observer 
 */
class KLAYOUT_DLL Observer 
  : public gsi::ObjectBase
{
public:
  /**
   *  @brief Constructor
   */
  Observer ();

  /**
   *  @brief The copy constructor
   */
  Observer (const Observer &d);

  /**
   *  @brief The destructor
   *
   *  Will unregister the observer from the observed object's observer list.
   */ 
  virtual ~Observer ();

  /**
   *  @brief The assignment operator
   */
  Observer &operator= (const Observer &d);

  /**
   *  @brief Detach from all observed objects
   */
  void detach_all ();

  /**
   *  @brief The simple signal event handler 
   */
  virtual void signal_observed ();

  /**
   *  @brief The simple signal event handler with an int argument
   */
  virtual void signal_observed_int (int arg);

  /**
   *  @brief The simple signal event handler with an void* argument
   */
  virtual void signal_observed_voidp (void *arg);

  /**
   *  @brief Query the state of the observation
   *
   *  @return The number of observed objects
   */
  unsigned int observes () const;

private:
  friend class Observed;

  observed_list_type m_observed;

  void observer_attach (Observed *observed);
  void observer_detach (Observed *observed);
};


/**
 *  @brief A observer delegate that aids implementation of objects that implement multiple observers
 *
 *  This observer can forward the triggered event to another object.
 *  In order to implement an observer "slot" in a class T, instantiate an object of type tl::ObserverDelegate<T>
 *  and pass a method pointer of type "void (T::*)(args)" to the constructor of this member.
 */
template <class T>
class ObserverDelegate
  : public tl::Observer
{
public:
  ObserverDelegate (T *t, void (T::*member) ())
    : mp_t (t), m_member_void (member), m_member_int (0), m_member_voidp (0)
  { }

  ObserverDelegate (T *t, void (T::*member) (int))
    : mp_t (t), m_member_void (0), m_member_int (member), m_member_voidp (0)
  { }

  ObserverDelegate (T *t, void (T::*member) (void *))
    : mp_t (t), m_member_void (0), m_member_int (0), m_member_voidp (member)
  { }

  virtual void signal_observed ()
  {
    if (mp_t && m_member_void) {
      (mp_t->*m_member_void) ();
    }
  }

  virtual void signal_observed_int (int arg)
  {
    if (mp_t && m_member_int) {
      (mp_t->*m_member_int) (arg);
    }
  }

  virtual void signal_observed_voidp (void *vp)
  {
    if (mp_t && m_member_voidp) {
      (mp_t->*m_member_voidp) (vp);
    }
  }

private:
  T *mp_t;
  void (T::*m_member_void) ();
  void (T::*m_member_int) (int);
  void (T::*m_member_voidp) (void *);
};

/**
 *  @brief A simple observer pattern implementation: the observed object 
 */
class KLAYOUT_DLL Observed
{
public:
  /**
   *  @brief Constructor
   */
  Observed ();

  /**
   *  @basic Destructor: will detach from all observers
   */
  virtual ~Observed ();

  /**
   *  @basic Set an observer
   *
   *  This will first detach all registered observers and replace them by
   *  the given one.
   *  If the observer is registered at another observable, it is detached
   *  from there before being added here.
   */
  void set_observer (Observer &observer);

  /**
   *  @basic Remove all observers
   *
   *  This will first detach all registered observers.
   */
  void remove_observers ();

  /**
   *  @basic Add an observer 
   *
   *  The observer will be added to the list of observers for this event.
   *  If the observer added is registered at another observable, it is detached
   *  from there before being added here.
   */
  void add_observer (Observer &observer);

  /**
   *  @remove Remove an observer object
   *
   *  This will detach the given observer from this observable.
   */
  void remove_observer (Observer &observer);

  /**
   *  @brief Signal to all observers
   *
   *  Will call the signal method on all observers
   */
  void signal_observers () const;

  /**
   *  @brief Signal to all observers
   *
   *  Will call the signal(int) method on all observers
   */
  void signal_observers (int arg) const;

  /**
   *  @brief Signal to all observers
   *
   *  Will call the signal(void *) method on all observers
   */
  void signal_observers (void *arg) const;

  /**
   *  @brief Count the number of observers attached to this observed object
   */
  unsigned int observers () const;

private:
  observer_list_type m_observers;

  //  No copy semantics for observed objects - we cannot tell, if it is requested to
  //  copy the observer links as well ..
  Observed (const Observed &d);
  Observed &operator= (const Observed &d);
};

} // namespace tl

#endif
