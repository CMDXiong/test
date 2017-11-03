
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


#include "layConfig.h"

namespace lay
{

// ------------------------------------------------------------
//  Declaration of the configuration options
//  The configuration pages are declared via two "dummy" plugins

const std::string cfg_background_color ("background-color");

const std::string cfg_ctx_color ("context-color");
const std::string cfg_ctx_dimming ("context-dimming");
const std::string cfg_ctx_hollow ("context-hollow");

const std::string cfg_child_ctx_color ("child-context-color");
const std::string cfg_child_ctx_dimming ("child-context-dimming");
const std::string cfg_child_ctx_hollow ("child-context-hollow");
const std::string cfg_child_ctx_enabled ("child-context-enabled");

const std::string cfg_abstract_mode_enabled ("abstract-mode-enabled");
const std::string cfg_abstract_mode_width ("abstract-mode-width");

const std::string cfg_sel_color ("sel-color");
const std::string cfg_sel_line_width ("sel-line-width");
const std::string cfg_sel_vertex_size ("sel-vertex-size");
const std::string cfg_sel_halo ("sel-halo");
const std::string cfg_sel_dither_pattern ("sel-dither-pattern");
const std::string cfg_sel_transient_mode ("sel-transient-mode");
const std::string cfg_sel_inside_pcells_mode ("sel-inside-pcells-mode");

const std::string cfg_min_inst_label_size ("min-inst-label-size");
const std::string cfg_cell_box_text_font ("inst-label-font");
const std::string cfg_cell_box_text_transform ("inst-label-transform");
const std::string cfg_cell_box_color ("inst-color");
const std::string cfg_cell_box_visible ("inst-visible");
const std::string cfg_text_color ("text-color");
const std::string cfg_text_visible ("text-visible");
const std::string cfg_text_lazy_rendering ("text-lazy-rendering");
const std::string cfg_bitmap_caching ("bitmap-caching");
const std::string cfg_show_properties ("show-properties");
const std::string cfg_apply_text_trans ("apply-text-trans");
const std::string cfg_global_trans ("global-trans");
const std::string cfg_no_stipple ("no-stipple");
const std::string cfg_stipple_offset ("stipple-offset");
const std::string cfg_default_text_size ("default-text-size");
const std::string cfg_text_font ("text-font");
const std::string cfg_full_hier_new_cell ("full-hierarchy-new-cell");
const std::string cfg_initial_hier_depth ("initial-hier-depth");
const std::string cfg_clear_ruler_new_cell ("clear-ruler-new-cell");
const std::string cfg_fit_new_cell ("fit-new-cell");
const std::string cfg_mouse_wheel_mode ("mouse-wheel-mode");
const std::string cfg_color_palette ("color-palette");
const std::string cfg_stipple_palette ("stipple-palette");
const std::string cfg_dbu_units ("dbu-units");
const std::string cfg_abs_units ("absolute-units");
const std::string cfg_drawing_workers ("drawing-workers");
const std::string cfg_drop_small_cells ("drop-small-cells");
const std::string cfg_drop_small_cells_cond ("drop-small-cells-condition");
const std::string cfg_drop_small_cells_value ("drop-small-cells-value");
const std::string cfg_array_border_instances ("draw-array-border-instances");
const std::string cfg_default_lyp_file ("default-layer-properties");
const std::string cfg_default_add_other_layers ("default-add-other-layers");
const std::string cfg_layers_always_show_source ("layers-always-show-source");
const std::string cfg_layers_always_show_ld ("layers-always-show-ld");
const std::string cfg_layers_always_show_layout_index ("layers-always-show-layout-index");

const std::string cfg_bitmap_oversampling ("bitmap-oversampling");
const std::string cfg_image_cache_size ("image-cache-size");
const std::string cfg_default_font_size ("default-font-size");

const std::string cfg_hide_empty_layers ("hide-empty-layers");
const std::string cfg_test_shapes_in_view ("test-shapes-in-view");

const std::string cfg_flat_cell_list ("flat-cell-list");
const std::string cfg_split_cell_list ("split-cell-list");
const std::string cfg_cell_list_sorting ("cell-list-sorting");

const std::string cfg_pan_distance ("pan-distance");
const std::string cfg_paste_display_mode ("paste-display-mode");

const std::string cfg_guiding_shape_visible ("guiding-shape-visible");
const std::string cfg_guiding_shape_color ("guiding-shape-color");
const std::string cfg_guiding_shape_line_width ("guiding-shape-line-width");
const std::string cfg_guiding_shape_vertex_size ("guiding-shape-vertex-size");

}

