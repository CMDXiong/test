
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


//  This header by intention does not have a include guard
//  It is used multiple times inside gsiQtEvent.h
//
//  It needs these macros to be defined (example for the one argument case)
//   _TMPLARGPART  "class A1"
//   _TMPLARGLIST  "A1"
//   _ADDARGS      "this->template add_arg<A1> (m_s1);"
//   _ARGSPEC      "const ArgSpec<A1> &s1" 
//   _ARGSPECINIT  "m_s1 = s1;"
//   _ARGSPECMEM   "ArgSpec<A1> m_s1;"

template <class X, class E _COMMA _TMPLARGPART>
class _NAME(QtSignalToEventBridge)
  : public gsi::EventBase
{
public:
  _NAME(QtSignalToEventBridge) (const std::string &name, gsi::event<_TMPLARGLIST> E::*event, _ARGSPEC const std::string &doc)
    : gsi::EventBase (name, doc), m_event (event)
  { 
    _ARGSPECINIT
  }

  virtual void initialize()
  {
    this->clear ();
    _ADDARGS
  }

  virtual MethodBase *clone () const 
  {
    return new _NAME(QtSignalToEventBridge) (*this);
  }

  virtual void set_callback (void *p, const gsi::Callback &cb) const
  {
    X *o = (X *) p;
    E *ea = dynamic_cast<E *> (get_ea_ref (o));
    if (ea) {
      ea->*m_event = cb;
    }
  }

  virtual void call (void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) const 
  {
    tl_assert (false);
  }

private:
   gsi::event<_TMPLARGLIST> E::*m_event;
   _ARGSPECMEM
};

template <class X, class E _COMMA _TMPLARGPART>
gsi::Methods 
qt_signal (const std::string &name, gsi::event<_TMPLARGLIST> (E::*event), _ARGSPEC const std::string &doc = "")
{
  return gsi::Methods (new _NAME(QtSignalToEventBridge)<X, E _COMMA _TMPLARGLIST> (name, event, _ARGSPECARGS doc));
}

