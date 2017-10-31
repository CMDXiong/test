
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
//  It is used multiple times inside gsiCallback.h
//
//  It needs these macros to be defined (example for the one argument case)
//   _TMPLARGPART  ", class A1"
//   _TMPLARGLIST  "A1, void, void, void, .. (total max_param)"
//   _FUNCARGLIST  "A1"
//   _CALLARGLIST  ", A1 a1"
//   _SETVALUE     "args.template write<A1> ((A1) a1);"

template <_TMPLARGS>
#if _COUNT == 16
class event
#else
class event<_TMPLARGLIST> 
#endif
  : public gsi::Callback
{
public:
  event () { }
  void operator=(const gsi::Callback &cb) { gsi::Callback::operator=(cb); }

  void operator()(_CALLARGLIST) 
  {
    if (callee) {
      SerialArgs args (argsize), ret (retsize);
      _SETVALUE
      call_int (args, ret);
    }
  }
};

template <class R _TMPLARGPART>
#if _COUNT == 16
class event_with_return
#else
class event_with_return<R, _TMPLARGLIST> 
#endif
  : public gsi::Callback
{
public:
  event_with_return () { }
  void operator=(const gsi::Callback &cb) { gsi::Callback::operator=(cb); }

  R operator()(_CALLARGLIST) 
  {
    if (! callee) {
      return R();
    } else {
      SerialArgs args (argsize), ret (retsize);
      _SETVALUE
      call_int (args, ret);
      return ret.template read<R> ();
    }
  }
};

