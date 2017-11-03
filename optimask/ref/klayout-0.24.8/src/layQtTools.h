
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


#ifndef HDR_layQtTools
#define HDR_layQtTools

#include "config.h"

#include <QtGui/QWidget>

namespace lay
{

/**
 *  @brief Save the given Widget's (dialog) state to the string
 *
 *  The state can be recovered from the string using restore_dialog_state;
 */
KLAYOUT_DLL std::string save_dialog_state (QWidget *dialog);

/**
 *  @brief Restore the dialog's state from the given string
 */
KLAYOUT_DLL void restore_dialog_state (QWidget *dialog, const std::string &s);


} // namespace lay

#endif

