
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


#include "tlObserver.h"

namespace tl
{

// --------------------------------------------------------------------
//  Observer implementation

Observer::Observer ()
{ 
  // .. nothing yet ..
}

Observer::Observer (const Observer &d)
{
  operator= (d);
}

Observer &
Observer::operator= (const Observer &d)
{
  if (this != &d) {
    detach_all ();
    for (observed_list_type::const_iterator o = d.m_observed.begin (); o != d.m_observed.end (); ++o) {
      (*o)->add_observer (*this);
    }
  }
  return *this;
}

void 
Observer::detach_all ()
{
  while (! m_observed.empty ()) {
    m_observed.front ()->remove_observer (*this);
  }
}

Observer::~Observer ()
{
  detach_all ();
}

void
Observer::observer_attach (Observed *observed)
{
  if (m_observed.empty ()) {
    keep ();
  }
  m_observed.push_front (observed);
}

void 
Observer::observer_detach (Observed *observed)
{
  for (observed_list_type::iterator o = m_observed.begin (); o != m_observed.end (); ++o) {
    if ((*o) == observed) {
      m_observed.erase (o);
      break;
    }
  }
  if (m_observed.empty ()) {
    release ();
  }
}

void 
Observer::signal_observed () 
{
  //  base implementation does nothing
}

void 
Observer::signal_observed_int (int) 
{
  //  base implementation does nothing
}

void 
Observer::signal_observed_voidp (void *) 
{
  //  base implementation does nothing
}

unsigned int 
Observer::observes () const 
{
  return m_observed.size ();
}

// --------------------------------------------------------------------
//  Observed implementation

Observed::Observed ()
{
  //  .. nothing yet ..
}

Observed::~Observed ()
{
  remove_observers ();
}

void
Observed::remove_observers ()
{
  while (! m_observers.empty ()) {
    remove_observer (*m_observers.front ());
  }
}

void 
Observed::set_observer (Observer &observer)
{
  remove_observers ();
  add_observer (observer);
}

void 
Observed::add_observer (Observer &observer)
{
  m_observers.push_front (&observer);
  observer.observer_attach (this);
}

void 
Observed::remove_observer (Observer &observer)
{
  for (observed_list_type::iterator o = observer.m_observed.begin (); o != observer.m_observed.end (); ++o) {
    if (*o == this) {
      for (observer_list_type::iterator oo = m_observers.begin (); oo != m_observers.end (); ++oo) {
        if ((*oo) == &observer) {
          m_observers.erase (oo);
          break;
        }
      }
      observer.observer_detach (this);
      break;
    }
  }
}

void 
Observed::signal_observers () const
{
  for (observer_list_type::const_iterator o = m_observers.begin (); o != m_observers.end (); ) {
    //  Note: by using that scheme, signal_observed can call detach for example:
    observer_list_type::const_iterator oo = o;
    ++oo;
    (*o)->signal_observed ();
    o = oo;
  }
}

void 
Observed::signal_observers (int arg) const
{
  for (observer_list_type::const_iterator o = m_observers.begin (); o != m_observers.end (); ) {
    //  Note: by using that scheme, signal_observed can call detach for example:
    observer_list_type::const_iterator oo = o;
    ++oo;
    (*o)->signal_observed_int (arg);
    o = oo;
  }
}

void 
Observed::signal_observers (void *arg) const
{
  for (observer_list_type::const_iterator o = m_observers.begin (); o != m_observers.end (); ) {
    //  Note: by using that scheme, signal_observed can call detach for example:
    observer_list_type::const_iterator oo = o;
    ++oo;
    (*o)->signal_observed_voidp (arg);
    o = oo;
  }
}

unsigned int 
Observed::observers () const 
{
  return m_observers.size ();
}

} // namespace tl


