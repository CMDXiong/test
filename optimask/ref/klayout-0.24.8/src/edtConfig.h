
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


#ifndef HDR_edtConfig
#define HDR_edtConfig

#include <string>

#include "laySnap.h"
#include "tlString.h"
#include "dbPoint.h"
#include "dbHersheyFont.h"

namespace edt
{

/** 
 *  @brief Declaration of the configuration names
 */
extern KLAYOUT_DLL std::string cfg_edit_grid;
extern KLAYOUT_DLL std::string cfg_edit_snap_to_objects;
extern KLAYOUT_DLL std::string cfg_edit_move_angle_mode;
extern KLAYOUT_DLL std::string cfg_edit_connect_angle_mode;
extern KLAYOUT_DLL std::string cfg_edit_text_string;
extern KLAYOUT_DLL std::string cfg_edit_text_size;
extern KLAYOUT_DLL std::string cfg_edit_text_halign;
extern KLAYOUT_DLL std::string cfg_edit_text_valign;
extern KLAYOUT_DLL std::string cfg_edit_path_width;
extern KLAYOUT_DLL std::string cfg_edit_path_ext_type;
extern KLAYOUT_DLL std::string cfg_edit_path_ext_var_begin;
extern KLAYOUT_DLL std::string cfg_edit_path_ext_var_end;
extern KLAYOUT_DLL std::string cfg_edit_global_grid;
extern KLAYOUT_DLL std::string cfg_edit_show_shapes_of_instances;
extern KLAYOUT_DLL std::string cfg_edit_max_shapes_of_instances;
extern KLAYOUT_DLL std::string cfg_edit_inst_cell_name;
extern KLAYOUT_DLL std::string cfg_edit_inst_lib_name;
extern KLAYOUT_DLL std::string cfg_edit_inst_pcell_parameters;
extern KLAYOUT_DLL std::string cfg_edit_inst_angle;
extern KLAYOUT_DLL std::string cfg_edit_inst_mirror;
extern KLAYOUT_DLL std::string cfg_edit_inst_scale;
extern KLAYOUT_DLL std::string cfg_edit_inst_array;
extern KLAYOUT_DLL std::string cfg_edit_inst_rows;
extern KLAYOUT_DLL std::string cfg_edit_inst_row_x;
extern KLAYOUT_DLL std::string cfg_edit_inst_row_y;
extern KLAYOUT_DLL std::string cfg_edit_inst_columns;
extern KLAYOUT_DLL std::string cfg_edit_inst_column_x;
extern KLAYOUT_DLL std::string cfg_edit_inst_column_y;
extern KLAYOUT_DLL std::string cfg_edit_inst_place_origin;
extern KLAYOUT_DLL std::string cfg_edit_top_level_selection;
extern KLAYOUT_DLL std::string cfg_edit_combine_mode;

// ------------------------------------------------------------
//  Helper functions to get and set the configuration

enum combine_mode_type { CM_Add = 0, CM_Merge, CM_Erase, CM_Mask, CM_Diff };

struct CMConverter 
{
  std::string to_string (const edt::combine_mode_type &m);
  void from_string (const std::string &s, edt::combine_mode_type &m);
};

enum path_ext_type { Flush = 0, Square, Variable, Round, NumPEModes };

struct ACConverter 
{
  std::string to_string (const lay::angle_constraint_type &m);
  void from_string (const std::string &s, lay::angle_constraint_type &m);
};

struct PathExtConverter 
{
  std::string to_string (const edt::path_ext_type &m);
  void from_string (const std::string &s, edt::path_ext_type &m);
};

struct EditGridConverter
{
  std::string to_string (const db::DVector &eg);
  void from_string (const std::string &s, db::DVector &eg);
  void from_string_picky (const std::string &s, db::DVector &eg);
};

struct HAlignConverter
{
  std::string to_string (db::HAlign a);
  void from_string (const std::string &s, db::HAlign &a);
};

struct VAlignConverter
{
  std::string to_string (db::VAlign a);
  void from_string (const std::string &s, db::VAlign &a);
};

}

#endif


