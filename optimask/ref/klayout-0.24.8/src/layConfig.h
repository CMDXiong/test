
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



#ifndef HDR_layConfig
#define HDR_layConfig

#include "config.h"

#include <string>

namespace lay
{

/** 
 *  @brief Declaration of the configuration names
 */
extern KLAYOUT_DLL const std::string cfg_abs_units;
extern KLAYOUT_DLL const std::string cfg_circle_points;
extern KLAYOUT_DLL const std::string cfg_apply_text_trans;
extern KLAYOUT_DLL const std::string cfg_global_trans;
extern KLAYOUT_DLL const std::string cfg_no_stipple;
extern KLAYOUT_DLL const std::string cfg_stipple_offset;
extern KLAYOUT_DLL const std::string cfg_array_border_instances;
extern KLAYOUT_DLL const std::string cfg_background_color;                                 
extern KLAYOUT_DLL const std::string cfg_cell_box_color;
extern KLAYOUT_DLL const std::string cfg_cell_box_text_font;
extern KLAYOUT_DLL const std::string cfg_cell_box_text_transform;
extern KLAYOUT_DLL const std::string cfg_cell_box_visible;
extern KLAYOUT_DLL const std::string cfg_clear_ruler_new_cell;
extern KLAYOUT_DLL const std::string cfg_color_palette;
extern KLAYOUT_DLL const std::string cfg_ctx_color;                                        
extern KLAYOUT_DLL const std::string cfg_ctx_dimming;                                      
extern KLAYOUT_DLL const std::string cfg_ctx_hollow;                                      
extern KLAYOUT_DLL const std::string cfg_child_ctx_color;
extern KLAYOUT_DLL const std::string cfg_child_ctx_dimming;
extern KLAYOUT_DLL const std::string cfg_child_ctx_hollow;
extern KLAYOUT_DLL const std::string cfg_child_ctx_enabled;
extern KLAYOUT_DLL const std::string cfg_abstract_mode_enabled;
extern KLAYOUT_DLL const std::string cfg_abstract_mode_width;
extern KLAYOUT_DLL const std::string cfg_current_ruler_template;                                     
extern KLAYOUT_DLL const std::string cfg_current_technology;                                     
extern KLAYOUT_DLL const std::string cfg_dbu_units;
extern KLAYOUT_DLL const std::string cfg_default_dbu;
extern KLAYOUT_DLL const std::string cfg_default_grids;
extern KLAYOUT_DLL const std::string cfg_default_lyp_file;
extern KLAYOUT_DLL const std::string cfg_default_add_other_layers;
extern KLAYOUT_DLL const std::string cfg_default_text_size;
extern KLAYOUT_DLL const std::string cfg_layers_always_show_source;
extern KLAYOUT_DLL const std::string cfg_layers_always_show_ld;
extern KLAYOUT_DLL const std::string cfg_layers_always_show_layout_index;
extern KLAYOUT_DLL const std::string cfg_drop_small_cells;
extern KLAYOUT_DLL const std::string cfg_drop_small_cells_cond;
extern KLAYOUT_DLL const std::string cfg_drop_small_cells_value;
extern KLAYOUT_DLL const std::string cfg_drawing_workers;
extern KLAYOUT_DLL const std::string cfg_edit_mode;
extern KLAYOUT_DLL const std::string cfg_custom_macro_paths;
extern KLAYOUT_DLL const std::string cfg_fit_new_cell;
extern KLAYOUT_DLL const std::string cfg_full_hier_new_cell;
extern KLAYOUT_DLL const std::string cfg_initial_hier_depth;
extern KLAYOUT_DLL const std::string cfg_grid;
extern KLAYOUT_DLL const std::string cfg_grid_color;                                                
extern KLAYOUT_DLL const std::string cfg_grid_micron;                                               
extern KLAYOUT_DLL const std::string cfg_grid_style0;                                               
extern KLAYOUT_DLL const std::string cfg_grid_style1;                                               
extern KLAYOUT_DLL const std::string cfg_grid_style2;                                               
extern KLAYOUT_DLL const std::string cfg_grid_visible;                                              
extern KLAYOUT_DLL const std::string cfg_bitmap_oversampling;
extern KLAYOUT_DLL const std::string cfg_image_cache_size;
extern KLAYOUT_DLL const std::string cfg_default_font_size;
extern KLAYOUT_DLL const std::string cfg_key_bindings;
extern KLAYOUT_DLL const std::string cfg_min_inst_label_size;
extern KLAYOUT_DLL const std::string cfg_mouse_wheel_mode;
extern KLAYOUT_DLL const std::string cfg_mru;
extern KLAYOUT_DLL const std::string cfg_technologies;
extern KLAYOUT_DLL const std::string cfg_reader_create_other_layers;
extern KLAYOUT_DLL const std::string cfg_reader_layer_map;
extern KLAYOUT_DLL const std::string cfg_reader_enable_properties;
extern KLAYOUT_DLL const std::string cfg_reader_enable_text_objects;
extern KLAYOUT_DLL const std::string cfg_reader_options_show_always;
extern KLAYOUT_DLL const std::string cfg_initial_technology;
extern KLAYOUT_DLL const std::string cfg_sel_color;                                        
extern KLAYOUT_DLL const std::string cfg_sel_halo;
extern KLAYOUT_DLL const std::string cfg_sel_line_width;                                   
extern KLAYOUT_DLL const std::string cfg_sel_transient_mode;
extern KLAYOUT_DLL const std::string cfg_sel_inside_pcells_mode;
extern KLAYOUT_DLL const std::string cfg_sel_vertex_size;
extern KLAYOUT_DLL const std::string cfg_sel_dither_pattern;
extern KLAYOUT_DLL const std::string cfg_show_layer_toolbox; 
extern KLAYOUT_DLL const std::string cfg_show_hierarchy_panel; 
extern KLAYOUT_DLL const std::string cfg_show_layer_panel;                                      
extern KLAYOUT_DLL const std::string cfg_show_properties;
extern KLAYOUT_DLL const std::string cfg_show_toolbar;
extern KLAYOUT_DLL const std::string cfg_show_navigator;
extern KLAYOUT_DLL const std::string cfg_window_state;
extern KLAYOUT_DLL const std::string cfg_window_geometry;
extern KLAYOUT_DLL const std::string cfg_navigator_all_hier_levels;
extern KLAYOUT_DLL const std::string cfg_navigator_show_images;
extern KLAYOUT_DLL const std::string cfg_stipple_palette;
extern KLAYOUT_DLL const std::string cfg_guiding_shape_visible;
extern KLAYOUT_DLL const std::string cfg_guiding_shape_color;
extern KLAYOUT_DLL const std::string cfg_guiding_shape_line_width;
extern KLAYOUT_DLL const std::string cfg_guiding_shape_vertex_size;
extern KLAYOUT_DLL const std::string cfg_synchronized_views;
extern KLAYOUT_DLL const std::string cfg_pan_distance;
extern KLAYOUT_DLL const std::string cfg_paste_display_mode;
extern KLAYOUT_DLL const std::string cfg_text_color;
extern KLAYOUT_DLL const std::string cfg_text_font;
extern KLAYOUT_DLL const std::string cfg_text_lazy_rendering;
extern KLAYOUT_DLL const std::string cfg_bitmap_caching;
extern KLAYOUT_DLL const std::string cfg_text_visible;
extern KLAYOUT_DLL const std::string cfg_hide_empty_layers;
extern KLAYOUT_DLL const std::string cfg_test_shapes_in_view;
extern KLAYOUT_DLL const std::string cfg_flat_cell_list;
extern KLAYOUT_DLL const std::string cfg_split_cell_list;
extern KLAYOUT_DLL const std::string cfg_cell_list_sorting;
extern KLAYOUT_DLL const std::string cfg_tip_window_hidden;
extern KLAYOUT_DLL const std::string cfg_micron_digits;
extern KLAYOUT_DLL const std::string cfg_dbu_digits;

}

#endif

