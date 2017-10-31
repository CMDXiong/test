
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


#ifndef _HDR_gsiEvents
#define _HDR_gsiEvents

#include "gsiSerialisation.h"

//  For a comprehensive documentation see gsi.h

namespace gsi
{

template <class A1 = void, class A2 = void, class A3 = void, class A4 = void, 
          class A5 = void, class A6 = void, class A7 = void, class A8 = void, 
          class A9 = void, class A10 = void, class A11 = void, class A12 = void,
          class A13 = void, class A14 = void, class A15 = void, class A16 = void> 
class event;

template <class R,
          class A1 = void, class A2 = void, class A3 = void, class A4 = void, 
          class A5 = void, class A6 = void, class A7 = void, class A8 = void, 
          class A9 = void, class A10 = void, class A11 = void, class A12 = void,
          class A13 = void, class A14 = void, class A15 = void, class A16 = void> 
class event_with_return;

// 0 arguments

#define  _COUNT 0
#define _TMPLARGS     
#define _TMPLARGPART  
#define _TMPLARGLIST  void, void, void, void, void, void, void, void, void, void, void
#define _FUNCARGLIST  
#define _CALLARGLIST  
#define _SETVALUE     

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 1 argument

#define  _COUNT 1
#define _TMPLARGS     class A1
#define _TMPLARGPART  , class A1
#define _TMPLARGLIST  A1, void, void, void, void, void, void, void, void, void, void
#define _FUNCARGLIST  A1
#define _CALLARGLIST  A1 a1
#define _SETVALUE     args.write<A1> ((A1) a1); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 2 arguments

#define  _COUNT 2
#define _TMPLARGS     class A1, class A2
#define _TMPLARGPART  , class A1, class A2
#define _TMPLARGLIST  A1, A2, void, void, void, void, void, void, void, void, void
#define _FUNCARGLIST  A1, A2
#define _CALLARGLIST  A1 a1, A2 a2
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 3 arguments

#define  _COUNT 3
#define _TMPLARGS     class A1, class A2, class A3
#define _TMPLARGPART  , class A1, class A2, class A3
#define _TMPLARGLIST  A1, A2, A3, void, void, void, void, void, void, void, void
#define _FUNCARGLIST  A1, A2, A3
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 4 arguments

#define  _COUNT 4
#define _TMPLARGS     class A1, class A2, class A3, class A4
#define _TMPLARGPART  , class A1, class A2, class A3, class A4
#define _TMPLARGLIST  A1, A2, A3, A4, void, void, void, void, void, void, void
#define _FUNCARGLIST  A1, A2, A3, A4
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 5 arguments

#define  _COUNT 5
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5
#define _TMPLARGLIST  A1, A2, A3, A4, A5, void, void, void, void, void, void
#define _FUNCARGLIST  A1, A2, A3, A4, A5
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 6 arguments

#define  _COUNT 6
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, void, void, void, void, void
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 7 arguments

#define  _COUNT 7
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, void, void, void, void
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 8 arguments

#define  _COUNT 8
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, void, void, void
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 9 arguments

#define  _COUNT 9
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, void, void
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 10 arguments

#define  _COUNT 10
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, void
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 11 arguments

#define  _COUNT 11
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \
                      args.write<A11> ((A11) a11); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 12 arguments

#define  _COUNT 12
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \
                      args.write<A11> ((A11) a11); \
                      args.write<A12> ((A12) a12); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 13 arguments

#define  _COUNT 13
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \
                      args.write<A11> ((A11) a11); \
                      args.write<A12> ((A12) a12); \
                      args.write<A13> ((A13) a13); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 14 arguments

#define  _COUNT 14
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \
                      args.write<A11> ((A11) a11); \
                      args.write<A12> ((A12) a12); \
                      args.write<A13> ((A13) a13); \
                      args.write<A14> ((A14) a14); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 15 arguments

#define  _COUNT 15
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, A15 a15
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \
                      args.write<A11> ((A11) a11); \
                      args.write<A12> ((A12) a12); \
                      args.write<A13> ((A13) a13); \
                      args.write<A14> ((A14) a14); \
                      args.write<A15> ((A15) a15); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

// 16 arguments

#define  _COUNT 16
#define _TMPLARGS     class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16
#define _TMPLARGPART  , class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class A11, class A12, class A13, class A14, class A15, class A16
#define _TMPLARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16
#define _FUNCARGLIST  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16
#define _CALLARGLIST  A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10, A11 a11, A12 a12, A13 a13, A14 a14, A15 a15, A16 a16
#define _SETVALUE     args.write<A1> ((A1) a1); \
                      args.write<A2> ((A2) a2); \
                      args.write<A3> ((A3) a3); \
                      args.write<A4> ((A4) a4); \
                      args.write<A5> ((A5) a5); \
                      args.write<A6> ((A6) a6); \
                      args.write<A7> ((A7) a7); \
                      args.write<A8> ((A8) a8); \
                      args.write<A9> ((A9) a9); \
                      args.write<A10> ((A10) a10); \
                      args.write<A11> ((A11) a11); \
                      args.write<A12> ((A12) a12); \
                      args.write<A13> ((A13) a13); \
                      args.write<A14> ((A14) a14); \
                      args.write<A15> ((A15) a15); \
                      args.write<A16> ((A16) a16); \

#include "gsiEventsVar.h"

#undef _SETVALUE
#undef _CALLARGLIST
#undef _FUNCARGLIST
#undef _TMPLARGLIST
#undef _TMPLARGPART
#undef _TMPLARGS
#undef _COUNT

typedef event<void, void, void, void, void, void, void, void, void, void, void, void, void, void, void, void>  Event;

}

#endif

