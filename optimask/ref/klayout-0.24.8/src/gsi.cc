
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

namespace gsi
{

// --------------------------------------------------------------------------------
//  Implementation of gsi::initialize

void KLAYOUT_DLL 
initialize ()
{
  for (gsi::ClassBase::class_iterator c = gsi::ClassBase::begin_classes (); c != gsi::ClassBase::end_classes (); ++c) {
    //  TODO: get rid of that const cast
    (const_cast<gsi::ClassBase *> (&*c))->initialize ();
  }

  //  merge the extensions to the main declaration
  gsi::ClassBase::merge_declarations ();

  for (gsi::ClassBase::class_iterator c = gsi::ClassBase::begin_classes (); c != gsi::ClassBase::end_classes (); ++c) {
    //  Initialize the method table once again after we have merged the declarations
    //  TODO: get rid of that const cast
    (const_cast<gsi::ClassBase *> (&*c))->initialize ();

    //  there should be only main declarations since we merged
    tl_assert (c->declaration () == &*c);
  }

  tl::initialize_variant_class_table ();
}

}

