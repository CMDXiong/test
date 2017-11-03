
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

#ifndef _HDR_gsiExternalMain
#define _HDR_gsiExternalMain

#ifdef _MSC_VER

// MSVC needs special declarations for all exports
#ifdef MAKE_KLAYOUT_GSI_DLL
#define KLAYOUT_GSI_DLL __declspec(dllexport)
#else
#define KLAYOUT_GSI_DLL __declspec(dllimport)
#endif

#else

// gcc does not need exports - all symbols are exported
#define KLAYOUT_GSI_DLL

#endif

namespace gsi
{

/**
 *  @brief Forces linking of the library
 */
KLAYOUT_GSI_DLL void initialize_external ();

}

#endif

