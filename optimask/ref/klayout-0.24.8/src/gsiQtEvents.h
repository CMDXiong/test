
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


#ifndef _HDR_gsiQtEvents
#define _HDR_gsiQtEvents

#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace qt_gsi
{

// ---------------------------------------------------------------------------
//  A base class for the event adaptor

class EventAdaptorBase
{
public:
  /**
   *  @brief Default ctor
   */
  EventAdaptorBase ()   
  {
    //  .. nothing yet ..
  }

  /**
   *  @brief Destructor - makes this class polymorphic
   */
  virtual ~EventAdaptorBase ()
  {
    //  .. nothing yet ..
  }
};

// ---------------------------------------------------------------------------

/**
 *  @brief Associates an EventAdaptorReference object with a QObject
 */
void attach_ea_ref (QObject *obj, EventAdaptorBase *ref);

/**
 *  @brief Resolves the association of the EventAdaptorReference with the QObject
 */
void detach_ea_ref (QObject *obj);

/**
 *  @brief Gets the EventAdaptorReference for a QObject
 */
EventAdaptorBase *get_ea_ref (QObject *obj);

/**
 *  A specialization of ObjectConnector that handles detachment and attachment
 *  of client data to a C++ object via delegation and in the Qt context implements
 *  signal/slot connection.
 *
 *  "E" must be derived from EventAdaptorBase
 */
template <class X, class E>
class QObjectConnector
  : public gsi::ObjectConnector
{
public:
  void on_attach (void *p) const
  {
    QObject *o = static_cast<QObject *> ((X *) p);
    attach_ea_ref (o, new E (o));
  }

  void on_detach (void *p) const
  {
    QObject *o = static_cast<QObject *> ((X *) p);
    detach_ea_ref (o);
  }

  void *client_data (void *p, int ci) const
  {
    QObject *o = static_cast<QObject *> ((X *) p);
    E *ea = dynamic_cast<E *> (get_ea_ref (o));
    if (ea) {
      return gsi::get_client_data (ea, ci);
    } else {
      return 0;
    }
  }
};

// ---------------------------------------------------------------------------
//  QtEventAdaptor variants using various numbers of arguments

#define _TMPLARGPART  
#define _TMPLARGLIST  
#define _ADDARGS      
#define _ARGSPEC
#define _ARGSPECINIT
#define _ARGSPECMEM
#define _ARGSPECARGS
#define _NAME(x) x##1
#define _COMMA

#include "gsiQtEventsVar.h"

#undef _TMPLARGPART
#undef _TMPLARGLIST
#undef _ADDARGS
#undef _ARGSPEC
#undef _ARGSPECINIT
#undef _ARGSPECMEM
#undef _ARGSPECARGS
#undef _NAME
#undef _COMMA

#define _TMPLARGPART  class A1
#define _TMPLARGLIST  A1
#define _ADDARGS      this->template add_arg<A1> (m_s1); 
#define _ARGSPEC      const gsi::ArgSpec<A1> &s1, 
#define _ARGSPECINIT  m_s1 = s1;
#define _ARGSPECMEM   gsi::ArgSpec<A1> m_s1;
#define _ARGSPECARGS  s1,
#define _NAME(x) x##2
#define _COMMA ,

#include "gsiQtEventsVar.h"

#undef _TMPLARGPART
#undef _TMPLARGLIST
#undef _ADDARGS
#undef _ARGSPEC
#undef _ARGSPECINIT
#undef _ARGSPECMEM
#undef _ARGSPECARGS
#undef _NAME
#undef _COMMA

#define _TMPLARGPART  class A1, class A2
#define _TMPLARGLIST  A1, A2
#define _ADDARGS      this->template add_arg<A1> (m_s1); \
                      this->template add_arg<A2> (m_s2); 
#define _ARGSPEC      const gsi::ArgSpec<A1> &s1, \
                      const gsi::ArgSpec<A2> &s2, 
#define _ARGSPECINIT  m_s1 = s1; \
                      m_s2 = s2;
#define _ARGSPECMEM   gsi::ArgSpec<A1> m_s1; \
                      gsi::ArgSpec<A2> m_s2; 
#define _ARGSPECARGS  s1, s2,
#define _NAME(x) x##3
#define _COMMA ,

#include "gsiQtEventsVar.h"

#undef _TMPLARGPART
#undef _TMPLARGLIST
#undef _ADDARGS
#undef _ARGSPEC
#undef _ARGSPECINIT
#undef _ARGSPECMEM
#undef _ARGSPECARGS
#undef _NAME
#undef _COMMA

#define _TMPLARGPART  class A1, class A2, class A3
#define _TMPLARGLIST  A1, A2, A3
#define _ADDARGS      this->template add_arg<A1> (m_s1); \
                      this->template add_arg<A2> (m_s2); \
                      this->template add_arg<A3> (m_s3); 
#define _ARGSPEC      const gsi::ArgSpec<A1> &s1, \
                      const gsi::ArgSpec<A2> &s2, \
                      const gsi::ArgSpec<A3> &s3, 
#define _ARGSPECINIT  m_s1 = s1; \
                      m_s2 = s2; \
                      m_s3 = s3; 
#define _ARGSPECMEM   gsi::ArgSpec<A1> m_s1; \
                      gsi::ArgSpec<A2> m_s2; \
                      gsi::ArgSpec<A3> m_s3; 
#define _ARGSPECARGS  s1, s2, s3,
#define _NAME(x) x##4
#define _COMMA ,

#include "gsiQtEventsVar.h"

#undef _TMPLARGPART
#undef _TMPLARGLIST
#undef _ADDARGS
#undef _ARGSPEC
#undef _ARGSPECINIT
#undef _ARGSPECMEM
#undef _ARGSPECARGS
#undef _NAME
#undef _COMMA

#define _TMPLARGPART  class A1, class A2, class A3, class A4
#define _TMPLARGLIST  A1, A2, A3, A4
#define _ADDARGS      this->template add_arg<A1> (m_s1); \
                      this->template add_arg<A2> (m_s2); \
                      this->template add_arg<A3> (m_s3); \
                      this->template add_arg<A4> (m_s4); 
#define _ARGSPEC      const gsi::ArgSpec<A1> &s1, \
                      const gsi::ArgSpec<A2> &s2, \
                      const gsi::ArgSpec<A3> &s3, \
                      const gsi::ArgSpec<A4> &s4, 
#define _ARGSPECINIT  m_s1 = s1; \
                      m_s2 = s2; \
                      m_s3 = s3; \
                      m_s4 = s4; 
#define _ARGSPECMEM   gsi::ArgSpec<A1> m_s1; \
                      gsi::ArgSpec<A2> m_s2; \
                      gsi::ArgSpec<A3> m_s3; \
                      gsi::ArgSpec<A4> m_s4; 
#define _ARGSPECARGS  s1, s2, s3, s4,
#define _NAME(x) x##5
#define _COMMA ,

#include "gsiQtEventsVar.h"

#undef _TMPLARGPART
#undef _TMPLARGLIST
#undef _ADDARGS
#undef _ARGSPEC
#undef _ARGSPECINIT
#undef _ARGSPECMEM
#undef _ARGSPECARGS
#undef _NAME
#undef _COMMA

#define _TMPLARGPART  class A1, class A2, class A3, class A4, class A5
#define _TMPLARGLIST  A1, A2, A3, A4, A5
#define _ADDARGS      this->template add_arg<A1> (m_s1); \
                      this->template add_arg<A2> (m_s2); \
                      this->template add_arg<A3> (m_s3); \
                      this->template add_arg<A4> (m_s4); \
                      this->template add_arg<A5> (m_s5); 
#define _ARGSPEC      const gsi::ArgSpec<A1> &s1, \
                      const gsi::ArgSpec<A2> &s2, \
                      const gsi::ArgSpec<A3> &s3, \
                      const gsi::ArgSpec<A4> &s4, \
                      const gsi::ArgSpec<A5> &s5, 
#define _ARGSPECINIT  m_s1 = s1; \
                      m_s2 = s2; \
                      m_s3 = s3; \
                      m_s4 = s4; \
                      m_s5 = s5; 
#define _ARGSPECMEM   gsi::ArgSpec<A1> m_s1; \
                      gsi::ArgSpec<A2> m_s2; \
                      gsi::ArgSpec<A3> m_s3; \
                      gsi::ArgSpec<A4> m_s4; \
                      gsi::ArgSpec<A5> m_s5;
#define _ARGSPECARGS  s1, s2, s3, s4, s5,
#define _NAME(x) x##6
#define _COMMA ,

#include "gsiQtEventsVar.h"

#undef _TMPLARGPART
#undef _TMPLARGLIST
#undef _ADDARGS
#undef _ARGSPEC
#undef _ARGSPECINIT
#undef _ARGSPECMEM
#undef _ARGSPECARGS
#undef _NAME
#undef _COMMA

}

#endif

