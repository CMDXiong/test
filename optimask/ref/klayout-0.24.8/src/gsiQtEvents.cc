
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


#include "gsiDecl.h"
#include "gsiDeclBasic.h"
#include "gsiQtEvents.h"

#include "tlStaticObjects.h"

namespace qt_gsi
{

class EventAdaptorReferenceMap
{
public:
  EventAdaptorReferenceMap ()
  {
    //  .. nothing yet ..
  }

  ~EventAdaptorReferenceMap () 
  {
    clear ();
  }

  void clear ()
  {
    for (std::map<QObject *, EventAdaptorBase *>::const_iterator m = m_map.begin (); m != m_map.end (); ++m) {
      delete m->second;
    }
    m_map.clear ();
  }

  void add (QObject *obj, EventAdaptorBase *ea)
  {
    std::map<QObject *, EventAdaptorBase *>::iterator m = m_map.find (obj);
    if (m != m_map.end ()) {
      delete m->second;
      m->second = ea;
    } else {
      m_map.insert (std::make_pair (obj, ea));
    }
  }

  void erase (QObject *obj)
  {
    std::map<QObject *, EventAdaptorBase *>::iterator m = m_map.find (obj);
    if (m != m_map.end ()) {
      delete m->second;
      m_map.erase (m);
    }
  }

  EventAdaptorBase *get (QObject *obj) const
  {
    std::map<QObject *, EventAdaptorBase *>::const_iterator m = m_map.find (obj);
    if (m != m_map.end ()) {
      return m->second;
    } else {
      return 0;
    }
  }

  static EventAdaptorReferenceMap *instance ()
  {
    static EventAdaptorReferenceMap *sp_instance;
    if (! sp_instance) {
      sp_instance = new EventAdaptorReferenceMap ();
      tl::StaticObjects::reg (&sp_instance);
    }
    return sp_instance;
  }

private:
  std::map<QObject *, EventAdaptorBase *> m_map;
};

void attach_ea_ref (QObject *obj, EventAdaptorBase *ea)
{
  EventAdaptorReferenceMap::instance ()->add (obj, ea);
}

void detach_ea_ref (QObject *obj)
{
  EventAdaptorReferenceMap::instance ()->erase (obj);
}

EventAdaptorBase *get_ea_ref (QObject *obj)
{
  return EventAdaptorReferenceMap::instance ()->get (obj);
}

}

