
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
#include "rdb.h"
#include "layApplication.h"
#include "layMainWindow.h"
#include "layLayoutView.h"
#include "dbSaveLayoutOptions.h"
#include "tlStream.h"

namespace gsi
{

static void remove_stipple (lay::LayoutView *view, unsigned int index)
{
  lay::DitherPattern pattern (view->dither_pattern ());

  if (index >= (unsigned int) std::distance (pattern.begin (), pattern.begin_custom ()) && 
      index < (unsigned int) std::distance (pattern.begin (), pattern.end ())) {
    lay::DitherPatternInfo p;
    pattern.replace_pattern (index, p); 
    pattern.renumber ();
    view->set_dither_pattern (pattern);
  }
}

static void clear_stipples (lay::LayoutView *view)
{
  lay::DitherPattern no_stipples;
  view->set_dither_pattern (no_stipples);
}

static unsigned int add_stipple (lay::LayoutView *view, const std::string &name, const std::vector<unsigned int> &data, unsigned int bits)
{
  lay::DitherPattern pattern (view->dither_pattern ());

  lay::DitherPatternInfo p;
  p.set_name (name);
  if (bits > 0 && ! data.empty ()) {
    p.set_pattern (&(*data.begin ()), std::min (32u, bits), std::min (size_t (32), data.size ()));
  }
  unsigned int index = pattern.add_pattern (p);

  view->set_dither_pattern (pattern);

  return index;
}

static void transaction (lay::LayoutView *view, const std::string &desc)
{
  view->manager ()->transaction (desc);
}

static void commit (lay::LayoutView *view)
{
  view->manager ()->commit ();
}

static void clear_transactions (lay::LayoutView *view)
{
  view->manager ()->clear ();
}

static bool transacting (lay::LayoutView *view)
{
  return view->manager ()->transacting ();
}

static db::DCplxTrans viewport_trans (const lay::LayoutView *view)
{
  return view->viewport ().trans ();
}

static int viewport_width (const lay::LayoutView *view)
{
  return view->viewport ().width ();
}

static int viewport_height (const lay::LayoutView *view)
{
  return view->viewport ().height ();
}

static unsigned int create_rdb (lay::LayoutView *view, const std::string &name)
{
  rdb::Database *db = new rdb::Database ();
  db->set_name (name);
  return view->add_rdb (db);
}

//  this binding returns a const pointer which is not converted into a copy by RBA
static const lay::LayerPropertiesNode *insert_layer1 (lay::LayoutView *view, const lay::LayerPropertiesConstIterator &iter, const lay::LayerProperties &props)
{
  const lay::LayerPropertiesNode *lp = dynamic_cast<const lay::LayerPropertiesNode *> (&props);
  if (lp) {
    return &view->insert_layer (iter, *lp);
  } else {
    return &view->insert_layer (iter, props);
  }
}

static void replace_layer_node2 (lay::LayoutView *view, unsigned int index, const lay::LayerPropertiesConstIterator &iter, const lay::LayerProperties &props)
{
  const lay::LayerPropertiesNode *lp = dynamic_cast<const lay::LayerPropertiesNode *> (&props);
  if (lp) {
    view->replace_layer_node (index, iter, *lp);
  } else {
    view->replace_layer_node (index, iter, props);
  }
}

static void replace_layer_node1 (lay::LayoutView *view, const lay::LayerPropertiesConstIterator &iter, const lay::LayerProperties &props)
{
  const lay::LayerPropertiesNode *lp = dynamic_cast<const lay::LayerPropertiesNode *> (&props);
  if (lp) {
    view->replace_layer_node (iter, *lp);
  } else {
    view->replace_layer_node (iter, props);
  }
}

static const lay::LayerPropertiesNode *insert_layer2 (lay::LayoutView *view, unsigned int index, const lay::LayerPropertiesConstIterator &iter, const lay::LayerProperties &props)
{
  const lay::LayerPropertiesNode *lp = dynamic_cast<const lay::LayerPropertiesNode *> (&props);
  if (lp) {
    return &view->insert_layer (index, iter, *lp);
  } else {
    return &view->insert_layer (index, iter, props);
  }
}

static void delete_layers1 (lay::LayoutView *view, const std::vector<lay::LayerPropertiesConstIterator> &iters)
{
  std::vector<lay::LayerPropertiesConstIterator> sorted (iters);
  std::sort (sorted.begin (), sorted.end (), lay::CompareLayerIteratorBottomUp ());
  for (std::vector<lay::LayerPropertiesConstIterator>::iterator s = sorted.begin (); s != sorted.end (); ++s) {
    view->delete_layer (*s);
  }
}

static unsigned int show_layout1 (lay::LayoutView *view, db::Layout *layout, bool add_cellview)
{
  //  the layout gets held by the LayoutHandle object
  layout->keep ();
  lay::LayoutHandle *handle = new lay::LayoutHandle (layout, std::string ());
  return view->add_layout (handle, add_cellview);
}

static unsigned int show_layout2 (lay::LayoutView *view, db::Layout *layout, std::string &tech, bool add_cellview)
{
  //  the layout gets held by the LayoutHandle object
  layout->keep ();
  lay::LayoutHandle *handle = new lay::LayoutHandle (layout, std::string ());
  handle->set_tech_name (tech);
  return view->add_layout (handle, add_cellview);
}

static unsigned int show_layout3 (lay::LayoutView *view, db::Layout *layout, std::string &tech, bool add_cellview, bool initialize_layers)
{
  //  the layout gets held by the LayoutHandle object
  layout->keep ();
  lay::LayoutHandle *handle = new lay::LayoutHandle (layout, std::string ());
  handle->set_tech_name (tech);
  return view->add_layout (handle, add_cellview, initialize_layers);
}

static void delete_layers2 (lay::LayoutView *view, unsigned int index, const std::vector<lay::LayerPropertiesConstIterator> &iters)
{
  std::vector<lay::LayerPropertiesConstIterator> sorted (iters);
  std::sort (sorted.begin (), sorted.end (), lay::CompareLayerIteratorBottomUp ());
  for (std::vector<lay::LayerPropertiesConstIterator>::iterator s = sorted.begin (); s != sorted.end (); ++s) {
    view->delete_layer (index, *s);
  }
}

static lay::LayoutView *current ()
{
  return lay::MainWindow::instance () ? lay::MainWindow::instance ()->current_view () : 0;
}

static void save_as1 (lay::LayoutView *view, unsigned int index, const std::string &filename, const db::SaveLayoutOptions &options)
{
  view->save_as (index, filename, tl::OutputStream::OM_Auto, options, true);
}

static void save_as2 (lay::LayoutView *view, unsigned int index, const std::string &filename, bool gzip, const db::SaveLayoutOptions &options)
{
  //  because set_format_from_name always returns true now, we ignore the gzip option -
  //  it's probably used only in that context.
  view->save_as (index, filename, tl::OutputStream::OM_Auto, options, true);
}

static QImage get_image_with_options (lay::LayoutView *view, unsigned int width, unsigned int height, int linewidth, int oversampling, double resolution, const db::DBox &target_box, bool monochrome)
{
  return view->get_image_with_options (width, height, linewidth, oversampling, resolution, QColor (), QColor (), QColor (), target_box, monochrome); 
}

static void save_image_with_options (lay::LayoutView *view, const std::string &fn, unsigned int width, unsigned int height, int linewidth, int oversampling, double resolution, const db::DBox &target_box, bool monochrome)
{
  view->save_image_with_options (fn, width, height, linewidth, oversampling, resolution, QColor (), QColor (), QColor (), target_box, monochrome); 
}

Class<lay::LayoutView> decl_LayoutView ("LayoutView", 
  method ("current", &current,
    "@brief Returns the current view\n"
    "The current view is the one that is shown in the current tab. Returns nil if no layout is loaded.\n"
    "\n"
    "This method has been introduce in version 0.23.\n"
  ) +
  method ("stop_redraw", &lay::LayoutView::stop_redraw,
    "@brief Stop the redraw thread\n"
    "\n"
    "It is very important to stop the redraw thread before applying changes to the "
    "layout or the cell views and the LayoutView configuration. This is usually done automatically. "
    "For rare cases, where this is not the case, this method is provided.\n"
  ) +
  method ("title=|#set_title", &lay::LayoutView::set_title,
    "@brief Set the title of the view\n"
    "\n"
    "@args title\n"
    "@param title The title string to use\n"
    "\n"
    "Override the standard title of the view indicating the file names loaded by "
    "the specified title string. The title string can be reset with \\reset_title to "
    "the standard title again."
  ) +
  method ("reset_title", &lay::LayoutView::reset_title,
    "@brief Reset the title to the standard title\n"
    "\n"
    "See \\set_title and \\title for a description about how titles are handled."
  ) +
  method ("title", &lay::LayoutView::title,
    "@brief Return the view's title string\n"
    "\n"
    "@return The title string\n"
    "\n"
    "The title string is either a string composed of the file names loaded (in some "
    "\"readable\" manner) or a customized title string set by \\set_title."
  ) +
  method ("save_layer_props", &lay::LayoutView::save_layer_props, 
    "@brief Save the layer properties\n"
    "\n"
    "@args fn\n"
    "\n"
    "Save the layer properties to the file given in \"fn\""
  ) +
  method ("load_layer_props", (void (lay::LayoutView::*)(const std::string &)) &lay::LayoutView::load_layer_props, 
    "@brief Load the layer properties\n"
    "\n"
    "@args fn\n"
    "@param fn The file name of the .lyp file to load\n"
    "\n"
    "Load the layer properties from the file given in \"fn\""
  ) +
  method ("load_layer_props", (void (lay::LayoutView::*)(const std::string &, bool)) &lay::LayoutView::load_layer_props, 
    "@brief Load the layer properties with options\n"
    "\n"
    "@args fn, add_default\n"
    "@param fn The file name of the .lyp file to load\n"
    "@param add_default If true, default layers will be added for each other layer in the layout\n"
    "\n"
    "Load the layer properties from the file given in \"fn\".\n"
    "This version allows to specify whether defaults should be used for all other layers by "
    "setting \"add_default\" to true.\n"
    "\n"
    "This variant has been added on version 0.21."
  ) +
  method ("load_layer_props", (void (lay::LayoutView::*)(const std::string &, int, bool)) &lay::LayoutView::load_layer_props, 
    "@brief Load the layer properties with options\n"
    "\n"
    "@args fn, cv_index, add_default\n"
    "@param fn The file name of the .lyp file to load\n"
    "@param cv_index See description text\n"
    "@param add_default If true, default layers will be added for each other layer in the layout\n"
    "\n"
    "Load the layer properties from the file given in \"fn\".\n"
    "This version allows to specify whether defaults should be used for all other layers by "
    "setting \"add_default\" to true. It can be used to load the layer properties for a specific "
    "cellview by setting \"cv_index\" to the index for which the layer properties file should be applied. "
    "All present definitions for this layout will be removed before the properties file is loaded. \"cv_index\" can "
    "be set to -1. In that case, the layer properties file is applied to each of the layouts individually.\n"
    "\n"
    "Note that this version will override all cellview index definitions in the layer properties file.\n"
    "\n"
    "This variant has been added on version 0.21."
  ) +
  method ("min_hier_levels=", &lay::LayoutView::set_min_hier_levels,
    "@brief Set the minimum hierarchy level at which to display geometries\n"
    "\n"
    "@args level\n"
    "@param level The minimum level above which to display something\n"
    "\n"
    "This methods allows to set the minimum hierarchy level above which to display geometries."
    "This method may cause a redraw if required."
  ) +
  method ("min_hier_levels", &lay::LayoutView::get_min_hier_levels,
    "@brief Returns the minimum hierarchy level at which to display geometries\n"
    "\n"
    "@return The minimum level at which to display geometries"
  ) +
  method ("max_hier_levels=", &lay::LayoutView::set_max_hier_levels,
    "@brief Set the maximum hierarchy level up to which to display geometries\n"
    "\n"
    "@args level\n"
    "@param level The maximum level below which to display something\n"
    "\n"
    "This methods allows to set the maximum hierarchy below which to display geometries."
    "This method may cause a redraw if required."
  ) +
  method ("max_hier_levels", &lay::LayoutView::get_max_hier_levels,
    "@brief Returns the maximum hierarchy level up to which to display geometries\n"
    "\n"
    "@return The maximum level up to which to display geometries"
  ) +
  method ("reload_layout", &lay::LayoutView::reload_layout,
    "@brief Reload the given cellview\n"
    "\n"
    "@args cv\n"
    "\n"
    "@param cv The index of the cellview to reload"
  ) + 
  method ("create_layout", (unsigned int (lay::LayoutView::*) (bool)) &lay::LayoutView::create_layout,
    "@brief Create a new, empty layout\n"
    "\n"
    "@args add_cellview\n"
    "\n"
    "The add_cellview parameter controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false).\n"
    "\n"
    "This version will associate the new layout with the default technology.\n"
    "\n"
    "@return The index of the cellview created.\n"
  ) +
  method ("create_layout", (unsigned int (lay::LayoutView::*) (const std::string &, bool)) &lay::LayoutView::create_layout,
    "@brief Create a new, empty layout and associate it with the given technology\n"
    "\n"
    "@args tech,add_cellview\n"
    "\n"
    "The add_cellview parameter controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false).\n"
    "\n"
    "@return The index of the cellview created.\n"
    "\n"
    "This variant has been introduced in version 0.22.\n"
  ) +
  method ("create_layout", (unsigned int (lay::LayoutView::*) (const std::string &, bool, bool)) &lay::LayoutView::create_layout,
    "@brief Create a new, empty layout and associate it with the given technology\n"
    "\n"
    "@args tech,add_cellview,init_layers\n"
    "\n"
    "The add_cellview parameter controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false). This variant also allows to control whether the layer properties are\n"
    "initialized (init_layers = true) or not (init_layers = false).\n"
    "\n"
    "@return The index of the cellview created.\n"
    "\n"
    "This variant has been introduced in version 0.22.\n"
  ) +
  method_ext ("show_layout", &show_layout1,
    "@brief Shows an existing layout in the view\n"
    "@args layout, add_cellview\n"
    "\n"
    "Shows the given layout in the view. If add_cellview is true, the new layout is added to the list of "
    "cellviews in the view.\n"
    "\n"
    "Note: once a layout is passed to the view with show_layout, it is owned by the view and must not be "
    "destroyed with the 'destroy' method.\n"
    "\n"
    "@return The index of the cellview created.\n"
    "\n"
    "This method has been introduced in version 0.22.\n"
  ) +
  method_ext ("show_layout", &show_layout2,
    "@brief Shows an existing layout in the view\n"
    "@args layout, tech, add_cellview\n"
    "\n"
    "Shows the given layout in the view. If add_cellview is true, the new layout is added to the list of "
    "cellviews in the view.\n"
    "The technology to use for that layout can be specified as well with the 'tech' parameter. Depending "
    "on the definition of the technology, layer properties may be loaded for example.\n"
    "The technology string can be empty for the default technology.\n"
    "\n"
    "Note: once a layout is passed to the view with show_layout, it is owned by the view and must not be "
    "destroyed with the 'destroy' method.\n"
    "\n"
    "@return The index of the cellview created.\n"
    "\n"
    "This method has been introduced in version 0.22.\n"
  ) +
  method_ext ("show_layout", &show_layout3,
    "@brief Shows an existing layout in the view\n"
    "@args layout, tech, add_cellview, init_layers\n"
    "\n"
    "Shows the given layout in the view. If add_cellview is true, the new layout is added to the list of "
    "cellviews in the view.\n"
    "The technology to use for that layout can be specified as well with the 'tech' parameter. Depending "
    "on the definition of the technology, layer properties may be loaded for example.\n"
    "The technology string can be empty for the default technology.\n"
    "This variant also allows to control whether the layer properties are\n"
    "initialized (init_layers = true) or not (init_layers = false).\n"
    "\n"
    "Note: once a layout is passed to the view with show_layout, it is owned by the view and must not be "
    "destroyed with the 'destroy' method.\n"
    "\n"
    "@return The index of the cellview created.\n"
    "\n"
    "This method has been introduced in version 0.22.\n"
  ) +
  method ("erase_cellview", &lay::LayoutView::erase_cellview,
    "@brief Erase the cellview with the given index\n"
    "\n"
    "@args index\n"
    "\n"
    "This closes the given cellview and unloads the layout associated with it, unless referred to by another cellview."
  ) +
  method ("rename_cellview", &lay::LayoutView::rename_cellview,
    "@brief Rename the cellview with the given index\n"
    "\n"
    "@args name, index\n"
    "\n"
    "If the name is not unique, a unique name will be constructed from the name given.\n"
    "The name may be different from the filename but is associated with the layout object.\n"
    "If a layout is shared between multiple cellviews (which may happen due to a clone of the layout view\n"
    "for example), all cellviews are renamed.\n"
  ) +
  method ("load_layout", (unsigned int (lay::LayoutView::*) (const std::string &, const db::LoadLayoutOptions &, const std::string &, bool)) &lay::LayoutView::load_layout,
    "@brief Load a (new) file into the layout view with the given technology\n"
    "\n"
    "@args filename, options, technology, add_cellview\n"
    "\n"
    "Loads the file given by the \"filename\" parameter and associates it with the given technology.\n"
    "The options specify various options for reading the file.\n"
    "The add_cellview param controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false).\n"
    "\n"
    "@return The index of the cellview loaded.\n"
    "\n"
    "This version has been introduced in version 0.22.\n"
  ) +
  method ("load_layout", (unsigned int (lay::LayoutView::*) (const std::string &, const db::LoadLayoutOptions &, bool)) &lay::LayoutView::load_layout,
    "@brief Load a (new) file into the layout view\n"
    "\n"
    "@args filename, options, add_cellview\n"
    "\n"
    "Loads the file given by the \"filename\" parameter.\n"
    "The options specify various options for reading the file.\n"
    "The add_cellview param controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false).\n"
    "\n"
    "@return The index of the cellview loaded.\n"
    "\n"
    "This method has been introduced in version 0.18.\n"
  ) +
  method ("load_layout", (unsigned int (lay::LayoutView::*) (const std::string &, const std::string &, bool)) &lay::LayoutView::load_layout,
    "@brief Load a (new) file into the layout view with the given technology\n"
    "\n"
    "@args filename, technology, add_cellview\n"
    "\n"
    "Loads the file given by the \"filename\" parameter and associates it with the given technology.\n"
    "The add_cellview param controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false).\n"
    "\n"
    "@return The index of the cellview loaded.\n"
    "\n"
    "This version has been introduced in version 0.22.\n"
  ) +
  method ("load_layout", (unsigned int (lay::LayoutView::*) (const std::string &filename, bool add_cellview)) &lay::LayoutView::load_layout,
    "@brief Load a (new) file into the layout view\n"
    "\n"
    "@args filename, add_cellview\n"
    "\n"
    "Loads the file given by the \"filename\" parameter.\n"
    "The add_cellview param controls whether to create a new cellview (true)\n"
    "or clear all cellviews before (false).\n"
    "\n"
    "@return The index of the cellview loaded.\n"
  ) +
  method ("active_cellview", &lay::LayoutView::active_cellview,
    "@brief Get the active cellview (shown in hierarchy browser)\n"
    "\n"
    "This is a convenience method which is equivalent to cellview(active_cellview_index()).\n"
    "\n"
    "This method has been introduced in version 0.19.\n"
  ) +
  method ("active_cellview_index", &lay::LayoutView::active_cellview_index,
    "@brief Get the index of the active cellview (shown in hierarchy browser)\n"
  ) +
  method ("set_active_cellview_index", &lay::LayoutView::set_active_cellview_index,
    "@brief Make the cellview with the given index the active one (shown in hierarchy browser)\n"
    "@args index\n"
    "See \\active_cellview_index."
  ) +
  method ("get_current_cell_path", &lay::LayoutView::get_current_cell_path,
    "@brief Cell path of the current cell\n"
    "\n"
    "@args cv_index\n"
    "\n"
    "The current cell is the one highlighted in the browser with the focus rectangle. The \n"
    "current path is returned for the cellview given by cv_index.\n"
    "The cell path is a list of cell indices from the top cell to the current cell.\n"
    "\n"
    "@param cv_index The cellview index for which to get the current path from (usally this will be the active cellview index)"
  ) +
  method ("set_current_cell_path", (void (lay::LayoutView::*) (int, const lay::LayoutView::cell_path_type &)) &lay::LayoutView::set_current_cell_path,
    "@brief Set the path to the current cell\n"
    "\n"
    "@args cv_index\n"
    "\n"
    "The current cell is the one highlighted in the browser with the focus rectangle. The\n"
    "cell given by the path is hightlighted and scrolled into view.\n"
    "To select the cell to be drawn, use the \\select_cell or \\select_cell_path method.\n"
    "\n"
    "@param cv_index The cellview index for which to set the current path for (usally this will be the active cellview index)\n"
    "@param path The path to the current cell\n"
  ) +
  method ("cellviews", &lay::LayoutView::cellviews,
    "@brief Get the number of cellviews\n"
  ) + 
  method ("cellview", &lay::LayoutView::cellview,
    "@brief Get the cellview object for a given index\n"
    "\n"
    "@args cv_index\n"
    "\n"
    "@param cv_index The cellview index for which to get the object for"
  ) + 
  method ("zoom_fit", &lay::LayoutView::zoom_fit,
    "@brief Fit the contents of the current view into the window"
  ) +
  method ("zoom_box", &lay::LayoutView::zoom_box,
    "@brief Set the viewport to the given box\n"
    "\n"
    "@args box\n"
    "@param box The box to which to set the view in micron coordinates\n"
  ) +
  method ("zoom_in", &lay::LayoutView::zoom_in,
    "@brief Zoom in somewhat"
  ) +
  method ("zoom_out", &lay::LayoutView::zoom_out,
    "@brief Zoom out somewhat"
  ) +
  method ("pan_up", &lay::LayoutView::pan_up,
    "@brief Pan upward" 
  ) +
  method ("pan_down", &lay::LayoutView::pan_down,
    "@brief Pan down" 
  ) +
  method ("pan_left", &lay::LayoutView::pan_left,
    "@brief Pan to the left" 
  ) +
  method ("pan_right", &lay::LayoutView::pan_right,
    "@brief Pan to the right" 
  ) +
  method ("pan_center", &lay::LayoutView::pan_center,
    "@brief Pan to the given point\n" 
    "\n"
    "@args p\n"
    "\n"
    "The window is positioned such that \"p\" becomes the new center"
  ) +
  method ("box", &lay::LayoutView::box,
    "@brief Return the displayed box in micron space" 
  ) +
  method_ext ("viewport_trans", &viewport_trans,
    "@brief Return the transformation that converts micron coordinates to pixels\n"
    "Hint: the transformation returned will convert any point in micron coordinate space into "
    "a pixel coordinate. Contrary to usual convention, the y pixel coordinate is given in a mathematically "
    "oriented space - which means the bottom coordinate is 0.\n"
    "This method was introduced in version 0.18.\n"
  ) +
  method_ext ("viewport_width", &viewport_width,
    "@brief Return the viewport width in pixels\n"
    "This method was introduced in version 0.18.\n"
  ) +
  method_ext ("viewport_height", &viewport_height,
    "@brief Return the viewport height in pixels\n"
    "This method was introduced in version 0.18.\n"
  ) +
  method ("bookmark_view", &lay::LayoutView::bookmark_view,
    "@brief Bookmark the current view under the given name\n"
    "\n"
    "@args name\n"
    "\n"
    "@param name The name under which to bookmark the current state"
  ) +
  method ("add_missing_layers", &lay::LayoutView::add_missing_layers,
    "@brief Add new layers to layer list\n"
    "This method was introduced in version 0.19.\n"
  ) +
  method ("remove_unused_layers", &lay::LayoutView::remove_unused_layers,
    "@brief Remove unused layers from layer list\n"
    "This method was introduced in version 0.19.\n"
  ) +
  method ("init_layer_properties", (void (lay::LayoutView::*) (lay::LayerProperties &) const) &lay::LayoutView::init_layer_properties,
    "@brief Fill the layer properties for a new layer\n"
    "@args props\n"
    "\n"
    "This method initializes a layer properties object's color and stipples according to "
    "the defaults for the given layer source specification. The layer's source must be set already "
    "on the layer properties object.\n"
    "\n"
    "This method was introduced in version 0.19.\n"
    "\n"
    "@param props The layer properties object to initialize."
  ) +
  method ("cancel", &lay::LayoutView::cancel,
    "@brief Cancel all edit operations\n"
    "\n"
    "This method will stop all pending edit operations (i.e. drag and drop) and cancel the current "
    "selection. Calling this method is useful to ensure there are no potential interactions with the script's "
    "functionality.\n"
  ) +
  method ("stop", &lay::LayoutView::stop,
    "@brief Stop redraw thread and close any browsers\n"
    "This method usually does not need to be called explicitly. The redraw thread is stopped automatically."
  ) +
  method ("enable_edits", &lay::LayoutView::enable_edits,
    "@brief Enable or disable editing\n"
    "\n"
    "@args enable\n"
    "\n"
    "@param enable True to enable edits"
  ) +
  method ("select_cell_path", (void (lay::LayoutView::*) (const lay::LayoutView::cell_path_type &, int)) &lay::LayoutView::select_cell,
    "@brief Select a cell by cell index for a certain cell view\n"
    "\n"
    "@args cell_index, cv_index\n"
    "\n"
    "Select the current (top) cell by specifying a cell indexand the cellview index for which this cell should become the currently shown one. The path to the cell is constructed by "
    "selecting one that leads to a top cell.\n"
    "This method selects the cell to be drawn. In constrast, the \\set_current_cell_path method selects "
    "the cell that is highlighted in the cell tree (but not necessarily drawn)."
  ) +
  method ("select_cell", (void (lay::LayoutView::*) (lay::LayoutView::cell_index_type, int)) &lay::LayoutView::select_cell,
    "@brief Select a cell by index for a certain cell view\n"
    "\n"
    "@args cell_index, cv_index\n"
    "\n"
    "Select the current (top) cell by specifying a path (a list of cell indices from top to "
    "the actual cell) and the cellview index for which this cell should become the currently "
    "shown one.\n"
    "This method selects the cell to be drawn. In constrast, the \\set_current_cell_path method selects "
    "the cell that is highlighted in the cell tree (but not necessarily drawn)."
  ) +
  method ("descend", &lay::LayoutView::descend, 
    "@brief Descend further into the hierarchy.\n"
    "@args path, index\n"
    "Adds the given path (given as an array of InstElement objects) to the specific path of the "
    "cellview with the given index. In effect, the cell addressed by the terminal of the new path "
    "components can be shown in the context of the upper cells, if the minimum hierarchy level is "
    "set to a negative value.\n"
    "The path is assumed to originate from the current cell and contain specific instances sorted from "
    "top to bottom."
  ) +
  method ("ascend", &lay::LayoutView::ascend, 
    "@brief Ascend upwards in the hierarchy.\n"
    "@args index\n"
    "Removes one element from the specific path of the cellview with the given index. Returns the element "
    "removed."
  ) +
  method ("is_cell_hidden?", &lay::LayoutView::is_cell_hidden,
    "@brief Tell, if the cell is hidden\n"
    "\n"
    "@args cell_index, cv_index\n"
    "\n"
    "@return True, if the cell with \"cell_index\" is hidden for the cellview \"cv_index\""
  ) +
  method ("hide_cell", &lay::LayoutView::hide_cell,
    "@brief Hide the given cell for the given cellview\n"
    "\n"
    "@args cell_index, cv_index\n"
  ) +
  method ("show_cell", &lay::LayoutView::show_cell,
    "@brief Show the given cell for the given cellview (cancel effect of \\hide_cell)\n"
    "\n"
    "@args cell_index, cv_index\n"
  ) +
  method ("show_all_cells", &lay::LayoutView::show_all_cells,
    "@brief Make all cells shown (cancel effects of \\hide_cell)"
  ) +
  method ("update_content", &lay::LayoutView::force_update_content,
    "@brief Update the layout view to the current state\n"
    "\n"
    "This method triggers an update of the hierarchy tree and layer view tree. Usually, this "
    "method does not need to be called. The widgets are updated automatically in most cases.\n"
    "\n"
    "Currently, this method should be called however, after the layer view tree has been changed by "
    "the \\insert_layer, \\replace_layer_node or \\delete_layer methods.\n" 
  ) +
  method ("max_hier", &lay::LayoutView::max_hier,
    "@brief Select all hierarchy levels available\n"
    "\n"
    "Show the layout in full depth down to the deepest level of hierarchy. "
    "This method may cause a redraw."
  ) +
#if defined(HAVE_QTBINDINGS)
  method ("get_screenshot", &lay::LayoutView::get_screenshot,
    "@brief Gets a screenshot as a \\QImage\n"
    "\n"
    "Getting the image requires the drawing to be complete. Ideally, synchroneous mode is switched on "
    "for the application to guarantee this condition. The image will have the size of the viewport "
    "showing the current layout."
  ) +
  method ("get_image", &lay::LayoutView::get_image,
    "@brief Gets the layout image as a \\QImage\n"
    "\n"
    "@args width, height\n"
    "@param width The width of the image to render in pixel.\n"
    "@param height The height of the image to render in pixel.\n"
    "\n"
    "The image contains the current scene (layout, annotations etc.).\n"
    "The image is drawn synchroneously with the given width and height. Drawing may take some time. "
  ) +
  method_ext ("get_image_with_options", &get_image_with_options,
    "@brief Gets the layout image as a \\QImage (with options)\n"
    "\n"
    "@args width, height, linewidth, oversampling, resolution, target, monochrome\n"
    "@param width The width of the image to render in pixel.\n"
    "@param height The height of the image to render in pixel.\n"
    "@param linewidth The width of a line in pixels (usually 1) or 0 for default.\n"
    "@param oversampling The oversampling factor (1..3) or 0 for default.\n"
    "@param resolution The resolution (pixel size compared to a screen pixel size, i.e 1/oversampling) or 0 for default.\n"
    "@param target_box The box to draw or an empty box for default.\n"
    "@param monochrome If true, monochrome images will be produced.\n"
    "\n"
    "The image contains the current scene (layout, annotations etc.).\n"
    "The image is written as a PNG file to the given file. "
    "The image is drawn synchroneously with the given width and height. Drawing may take some time. "
    "Monochrome images don't have background or annotation objects currently.\n"
    "\n"
    "This method has been introduced in 0.23.10.\n"
  ) +
#endif
  method ("save_screenshot", &lay::LayoutView::save_screenshot,
    "@brief Save a screenshot to the given file\n"
    "\n"
    "@args filename\n"
    "@param filename The file to which to write the screenshot to.\n"
    "\n"
    "The screenshot is written as a PNG file to the given file. "
    "This requires the drawing to be complete. Ideally, synchroneous mode is switched on "
    "for the application to guarantee this condition. The image will have the size of the viewport "
    "showing the current layout."
  ) +
  method ("save_image", &lay::LayoutView::save_image,
    "@brief Save the layout as an image to the given file\n"
    "\n"
    "@args filename, width, height\n"
    "@param filename The file to which to write the screenshot to.\n"
    "@param width The width of the image to render in pixel.\n"
    "@param height The height of the image to render in pixel.\n"
    "\n"
    "The image contains the current scene (layout, annotations etc.).\n"
    "The image is written as a PNG file to the given file. "
    "The image is drawn synchroneously with the given width and height. Drawing may take some time. "
  ) +
  method_ext ("save_image_with_options", &save_image_with_options,
    "@brief Save the layout as an image to the given file (with options)\n"
    "\n"
    "@args filename, width, height, linewidth, oversampling, resolution, target, monochrome\n"
    "@param filename The file to which to write the screenshot to.\n"
    "@param width The width of the image to render in pixel.\n"
    "@param height The height of the image to render in pixel.\n"
    "@param linewidth The width of a line in pixels (usually 1) or 0 for default.\n"
    "@param oversampling The oversampling factor (1..3) or 0 for default.\n"
    "@param resolution The resolution (pixel size compared to a screen pixel, i.e 1/oversampling) or 0 for default.\n"
    "@param target_box The box to draw or an empty box for default.\n"
    "@param monochrome If true, monochrome images will be produced.\n"
    "\n"
    "The image contains the current scene (layout, annotations etc.).\n"
    "The image is written as a PNG file to the given file. "
    "The image is drawn synchroneously with the given width and height. Drawing may take some time. "
    "Monochrome images don't have background or annotation objects currently.\n"
    "\n"
    "This method has been introduced in 0.23.10.\n"
  ) +
  method_ext ("#save_as", &save_as2,
    "@brief Save a layout to the given stream file\n"
    "\n"
    "@args index, filename, gzip, options\n"
    "@param index The cellview index of the layout to save.\n"
    "@param filename The file to write.\n"
    "@param gzip Ignored.\n"
    "@param options Writer options.\n"
    "\n"
    "The layout with the given index is written to the stream file with the given options. "
    "'options' is a \\SaveLayoutOptions object that specifies which format to write and further options such "
    "as scaling factor etc.\n"
    "Calling this method is equivalent to calling 'write' on the respective layout object.\n"
    "\n"
    "This method is deprecated starting from version 0.23. The compression mode is "
    "determined from the file name automatically and the \\gzip parameter is ignored.\n"
  ) +
  method_ext ("save_as", &save_as1,
    "@brief Save a layout to the given stream file\n"
    "\n"
    "@args index, filename, options\n"
    "@param index The cellview index of the layout to save.\n"
    "@param filename The file to write.\n"
    "@param options Writer options.\n"
    "\n"
    "The layout with the given index is written to the stream file with the given options. "
    "'options' is a \\SaveLayoutOptions object that specifies which format to write and further options such "
    "as scaling factor etc.\n"
    "Calling this method is equivalent to calling 'write' on the respective layout object.\n"
    "\n"
    "If the file name ends with a suffix \".gz\" or \".gzip\", the file is compressed with the zlib "
    "algorithm.\n"
  ) +
  method ("set_layer_properties", (void (lay::LayoutView::*) (const lay::LayerPropertiesConstIterator &, const lay::LayerProperties &)) &lay::LayoutView::set_properties, 
    "@brief Set the layer properties of the layer pointed to by the iterator\n"
    "@args iter, props\n"
    "\n"
    "This method replaces the layer properties of the element pointed to by \"iter\" by the properties "
    "given by \"props\". It will not change the hierarchy but just the properties of the given node."
  ) +
  method ("set_layer_properties", (void (lay::LayoutView::*) (unsigned int index, const lay::LayerPropertiesConstIterator &, const lay::LayerProperties &)) &lay::LayoutView::set_properties, 
    "@brief Set the layer properties of the layer pointed to by the iterator\n"
    "@args index, iter, props\n"
    "\n"
    "This method replaces the layer properties of the element pointed to by \"iter\" by the properties "
    "given by \"props\" in the tab given by \"index\". It will not change the hierarchy but just the properties of the given node."
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("expand_layer_properties", (void (lay::LayoutView::*) ()) &lay::LayoutView::expand_properties, 
    "@brief Expands the layer properties for all tabs\n"
    "\n"
    "This method will expand all wildcard specifications in the layer properties by iterating over the specified objects (i.e. layers, cellviews) and "
    "by replacing default colors and stipples by the ones specified with the palettes.\n"
    "\n"
    "This method was introduced in version 0.21.\n"
  ) +
  method ("expand_layer_properties", (void (lay::LayoutView::*) (unsigned int)) &lay::LayoutView::expand_properties, 
    "@brief Expands the layer properties for the given tab\n"
    "@args index\n"
    "\n"
    "This method will expand all wildcard specifications in the layer properties by iterating over the specified objects (i.e. layers, cellviews) and "
    "by replacing default colors and stipples by the ones specified with the palettes.\n"
    "\n"
    "This method was introduced in version 0.21.\n"
  ) +
  method_ext ("replace_layer_node", &replace_layer_node1, 
    "@brief Replace the layer node at the position given by \"iter\" with a new one\n"
    "@args iter, node\n"
    "\n"
    "Since version 0.22, this method accepts LayerProperties and LayerPropertiesNode objects. A LayerPropertiesNode "
    "object can contain a hierarchy of further nodes."
  ) +
  method_ext ("replace_layer_node", &replace_layer_node2,
    "@brief Replace the layer node at the position given by \"iter\" with a new one\n"
    "@args index, iter, node\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "\n"
    "This method has been introduced in version 0.21.\n"
    "Since version 0.22, this method accepts LayerProperties and LayerPropertiesNode objects. A LayerPropertiesNode "
    "object can contain a hierarchy of further nodes."
  ) +
  method_ext ("insert_layer", &insert_layer1, 
    "@brief Insert the given layer properties node into the list before the given position\n"
    "@args iter, node\n"
    "\n"
    "This method inserts the new properties node before the position given by \"iter\" and returns "
    "a const reference to the element created. The iterator that specified the position will remain valid "
    "after the node was inserted and will point to the newly created node. It can be used to add futher nodes. "
    "To add children to the node inserted, use iter.last_child as insertion point for the next insert operations.\n"
    "\n"
    "Since version 0.22, this method accepts LayerProperties and LayerPropertiesNode objects. A LayerPropertiesNode "
    "object can contain a hierarchy of further nodes."
  ) +
  method_ext ("insert_layer", &insert_layer2, 
    "@brief Insert the given layer properties node into the list before the given position\n"
    "@args index, iter, node\n"
    "\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method inserts the new properties node before the position given by \"iter\" and returns "
    "a const reference to the element created. The iterator that specified the position will remain valid "
    "after the node was inserted and will point to the newly created node. It can be used to add futher nodes. "
    "\n"
    "This method has been introduced in version 0.21.\n"
    "Since version 0.22, this method accepts LayerProperties and LayerPropertiesNode objects. A LayerPropertiesNode "
    "object can contain a hierarchy of further nodes."
  ) +
  method_ext ("delete_layers", &delete_layers1, 
    "@brief Delete the layer properties nodes specified by the iterator\n"
    "@args iterators\n"
    "\n"
    "This method deletes the nodes specifies by the iterators. This method is the most convenient way to "
    "delete multiple entries.\n"
    "\n"
    "This method has been added in version 0.22.\n"
  ) +
  method_ext ("delete_layers", &delete_layers2,
    "@brief Delete the layer properties nodes specified by the iterator\n"
    "@args index, iterators\n"
    "\n"
    "This method deletes the nodes specifies by the iterators. This method is the most convenient way to "
    "delete multiple entries.\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method has been introduced in version 0.22.\n"
  ) +
  method ("delete_layer", (void (lay::LayoutView::*) (lay::LayerPropertiesConstIterator &iter)) &lay::LayoutView::delete_layer, 
    "@brief Delete the layer properties node specified by the iterator\n"
    "@args iter\n"
    "\n"
    "This method deletes the object that the iterator points to and invalidates\n"
    "the iterator since the object that the iterator points to is no longer valid.\n"
  ) +
  method ("delete_layer", (void (lay::LayoutView::*) (unsigned int index, lay::LayerPropertiesConstIterator &iter)) &lay::LayoutView::delete_layer, 
    "@brief Delete the layer properties node specified by the iterator\n"
    "@args index, iter\n"
    "\n"
    "This method deletes the object that the iterator points to and invalidates\n"
    "the iterator since the object that the iterator points to is no longer valid.\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("begin_layers", (lay::LayerPropertiesConstIterator (lay::LayoutView::*) () const) &lay::LayoutView::begin_layers, 
    "@brief Begin iterator for the layers\n"
    "\n"
    "This iterator delivers the layers of this view, either in a recursive or non-recursive\n"
    "fashion, depending which iterator increment methods are used.\n"
    "The iterator delivered by \\end_layers is the past-the-end iterator. It can be compared\n"
    "agains a current iterator to check, if there are no further elements.\n"
  ) +
  method ("end_layers", (lay::LayerPropertiesConstIterator (lay::LayoutView::*) () const) &lay::LayoutView::end_layers, 
    "@brief End iterator for the layers\n"
    "See \\begin_layers for a description about this iterator\n"
  ) +
  method ("begin_layers", (lay::LayerPropertiesConstIterator (lay::LayoutView::*) (unsigned int index) const) &lay::LayoutView::begin_layers, 
    "@brief Begin iterator for the layers\n"
    "@args index\n"
    "\n"
    "This iterator delivers the layers of this view, either in a recursive or non-recursive\n"
    "fashion, depending which iterator increment methods are used.\n"
    "The iterator delivered by \\end_layers is the past-the-end iterator. It can be compared\n"
    "agains a current iterator to check, if there are no further elements.\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("end_layers", (lay::LayerPropertiesConstIterator (lay::LayoutView::*) (unsigned int index) const) &lay::LayoutView::end_layers, 
    "@brief End iterator for the layers\n"
    "@args index\n"
    "See \\begin_layers for a description about this iterator\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("clear_layers", (void (lay::LayoutView::*) ()) &lay::LayoutView::clear_layers, 
    "@brief Clear all layers\n"
  ) +
  method ("clear_layers", (void (lay::LayoutView::*) (unsigned int index)) &lay::LayoutView::clear_layers, 
    "@brief Clear all layers for the given layer properties list\n"
    "@args index\n"
    "This version addresses a specific list in a multi-tab layer properties arrangement with the \"index\" parameter. "
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("delete_layer_list", (void (lay::LayoutView::*) (unsigned int index)) &lay::LayoutView::delete_layer_list, 
    "@brief Deletes the given properties list\n"
    "@args index\n"
    "At least one layer properties list must remain. This method may change the current properties list.\n"
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("insert_layer_list", (void (lay::LayoutView::*) (unsigned int index)) &lay::LayoutView::insert_layer_list, 
    "@brief Inserts a new layer properties list at the given index\n"
    "@args index\n"
    "This method inserts a new tab at the given position. The current layer properties list will be changed to "
    "the new list.\n"
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("num_layer_lists", &lay::LayoutView::layer_lists, 
    "@brief Gets the number of layer properties tabs present\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  method ("current_layer_list", &lay::LayoutView::current_layer_list, 
    "@brief Gets the index of the currently selected layer properties tab\n"
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("current_layer_list=|#set_current_layer_list", &lay::LayoutView::set_current_layer_list, 
    "@brief Sets the index of the currently selected layer properties tab\n"
    "@args index\n"
    "This method has been introduced in version 0.21.\n"
  ) +
  method ("rename_layer_list", &lay::LayoutView::rename_properties, 
    "@brief Sets the title of the given layer properties tab\n"
    "@args index, name\n"
    "This method has been introduced in version 0.21.\n"
  ) +
  method_ext ("remove_stipple", &remove_stipple,
    "@brief Remove the stipple pattern with the given index\n"
    "@args index\n"
    "The pattern with an index less than 16 cannot be removed. "
    "If a stipple pattern is removed that is still used, the results are not predictable. "
  ) +
  method_ext ("clear_stipples", &clear_stipples,
    "@brief Remove all stipple pattern\n"
    "All stipple pattern except the fixed ones are removed. If any of the custom stipple pattern is "
    "still used by the layers displayed, the results will not be predictable."
  ) +
  method_ext ("add_stipple", &add_stipple,
    "@brief Add a stipple pattern\n"
    "@args name, data, bits\n"
    "\n"
    "'data' is an array of unsigned integers describing the bits that make up the stipple "
    "pattern. If the array has less than 32 entries, the pattern will be repeated vertically. "
    "The number of bits used can be less than 32 bit which can be specified by the 'bits' parameter. "
    "Logically, the pattern will be put at the end of the list.\n"
    "\n"
    "@param name The name under which this pattern will appear in the stipple editor\n"
    "@param data See above\n"
    "@param bits See above\n"
    "@return The index of the newly created stipple pattern, which can be used as the dither pattern index of \\LayerProperties."
  ) +
  method ("current_layer", &lay::LayoutView::current_layer, 
    "@brief Gets the current layer view\n"
    "\n"
    "Returns the \\LayerPropertiesIterator pointing to the current layer view (the one that has the focus). "
    "If no layer view is active currently, a null iterator is returned.\n"
  ) +
  method ("current_layer=", (void (lay::LayoutView::*) (const lay::LayerPropertiesConstIterator &l)) &lay::LayoutView::set_current_layer, 
    "@brief Sets the current layer view\n"
    "@args iter\n"
    "\n"
    "Specifies an \\LayerPropertiesIterator pointing to the new current layer view.\n"
    "\n"
    "This method has been introduced in version 0.23.\n"
  ) +
  method ("selected_layers", &lay::LayoutView::selected_layers, 
    "@brief Get the selected layers\n"
    "\n"
    "Returns an array of \\LayerPropertiesIterator objects pointing to the currently selected layers. "
    "If no layer view is selected currently, an empty array is returned.\n"
  ) +
  method ("add_active_cellview_observer", &lay::LayoutView::add_active_cellview_observer,
    "@brief Add an observer for the active cellview\n"
    "@args observer\n"
    "\n"
    "If the active cellview is changed by selecting a new one from the drop-down list, this observer is triggered.\n"
    "\n"
    "This method has been added in version 0.23.\n"
  ) +
  method ("remove_active_cellview_observer", &lay::LayoutView::remove_active_cellview_observer,
    "@brief Remove an observer for the active cellview\n"
    "@args observer\n"
    "\n"
    "This method has been added in version 0.23.\n"
  ) +
  method ("add_cellview_list_observer", &lay::LayoutView::add_cellview_list_observer,
    "@brief Add a cellview list observer\n"
    "@args observer\n"
    "\n"
    "If a cellview is added or removed, this observer is triggered.\n"
  ) +
  method ("remove_cellview_list_observer", &lay::LayoutView::remove_cellview_list_observer,
    "@brief Remove a cellview list observer\n"
    "@args observer\n"
  ) +
  method ("add_cellview_observer", &lay::LayoutView::add_cellview_observer,
    "@brief Add a cellview observer\n"
    "@args observer\n"
    "\n"
    "If a cellview is changed (i.e. the cell is changed) this event is sent.\n"
    "The integer argument slot (\"signal_int\") of the observer will be triggered as well "
    "with the index of the cellview that has changed.\n"
  ) +
  method ("remove_cellview_observer", &lay::LayoutView::remove_cellview_observer,
    "@brief Remove a cellview observer\n"
    "@args observer\n"
  ) +
  method ("add_file_open_observer", &lay::LayoutView::add_file_open_observer,
    "@brief Add a file open observer\n"
    "@args observer\n"
    "\n"
    "If a new file is loaded, this observer is triggered.\n"
  ) +
  method ("remove_file_open_observer", &lay::LayoutView::remove_file_open_observer,
    "@brief Remove a file open observer\n"
    "@args observer\n"
  ) +
  method ("add_viewport_changed_observer", &lay::LayoutView::add_viewport_changed_observer,
    "@brief Add a viewport changed observer\n"
    "@args observer\n"
    "\n"
    "If the viewport (the rectangle that is shown) changes, this observer\n"
    "is triggered.\n"
  ) +
  method ("remove_viewport_changed_observer", &lay::LayoutView::remove_viewport_changed_observer,
    "@brief Remove a viewport changed observer\n"
    "@args observer\n"
  ) +
  method ("add_layer_list_observer", &lay::LayoutView::add_layer_list_observer,
    "@brief Add a layer list observer\n"
    "@args observer\n"
    "\n"
    "If the layer list changes, the observer's \"signal_int\" and \"signal\" slot is triggered. \n"
    "The int's bit 0 is set, if the properties have changed. If the arguments bit 1 is\n"
    "set, the hierarchy has changed.\n"
  ) +
  method ("remove_layer_list_observer", &lay::LayoutView::remove_layer_list_observer,
    "@brief Remove a layer list observer\n"
    "@args observer\n"
  ) +
  method ("add_cell_visibility_observer", &lay::LayoutView::add_cell_visibility_observer,
    "@brief Add a cell visibility observer\n"
    "@args observer\n"
    "\n"
    "If a cell is hidden or shown, this observer is triggered.\n"
  ) +
  method ("remove_cell_visibility_observer", &lay::LayoutView::remove_cell_visibility_observer,
    "@brief Remove a cell visibility observer\n"
    "@args observer\n"
  ) +
  //  HINT: the cast is important to direct GSI to the LayoutView method rather than the
  //  Editables method (in which case we get a segmentation violation ..)
  method ("add_transient_selection_changed_observer", (void (lay::LayoutView::*)(tl::Observer &)) &lay::LayoutView::add_transient_selection_changed_observer,
    "@brief Add a transient selection observer\n"
    "@args observer\n"
    "\n"
    "If the transient selection is changed, this observer is triggered.\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  //  HINT: the cast is important to direct GSI to the LayoutView method rather than the
  //  Editables method (in which case we get a segmentation violation ..)
  method ("remove_transient_selection_changed_observer", (void (lay::LayoutView::*)(tl::Observer &)) &lay::LayoutView::remove_transient_selection_changed_observer,
    "@brief Remove a transient selection observer\n"
    "@args observer\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  //  HINT: the cast is important to direct GSI to the LayoutView method rather than the
  //  Editables method (in which case we get a segmentation violation ..)
  method ("add_selection_changed_observer", (void (lay::LayoutView::*)(tl::Observer &)) &lay::LayoutView::add_selection_changed_observer,
    "@brief Add a selection observer\n"
    "@args observer\n"
    "\n"
    "If the selection is changed, this observer is triggered.\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  //  HINT: the cast is important to direct GSI to the LayoutView method rather than the
  //  Editables method (in which case we get a segmentation violation ..)
  method ("remove_selection_changed_observer", (void (lay::LayoutView::*)(tl::Observer &)) &lay::LayoutView::remove_selection_changed_observer,
    "@brief Remove a selection observer\n"
    "@args observer\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  method ("add_rdb_list_changed_observer", &lay::LayoutView::add_rdb_list_changed_observer,
    "@brief Add a observer for the list of report databases\n"
    "@args observer\n"
    "\n"
    "If a report database is added or removed, this observer is triggered.\n"
  ) +
  method ("remove_rdb_list_changed_observer", &lay::LayoutView::remove_rdb_list_changed_observer,
    "@brief Remove a observer for the list of report databases\n"
    "@args observer\n"
  ) +
  method ("num_rdbs", &lay::LayoutView::num_rdbs,
    "@brief Get the number of report databases loaded into this view\n"
    "@return The number of \\ReportDatabase objects present in this view\n"
  ) +
  method ("remove_rdb", &lay::LayoutView::remove_rdb,
    "@brief Remove a report database with the given index\n"
    "@args index\n"
    "@param The index of the report database to remove from this view"
  ) +
  method ("rdb", (rdb::Database *(lay::LayoutView::*) (int index)) &lay::LayoutView::get_rdb,
    "@brief Gets the report database with the given index\n"
    "@args index\n"
    "@return The \\ReportDatabase object or nil if the index is not valid"
  ) +
  method_ext ("create_rdb", &create_rdb,
    "@brief Creates a new report database and returns the index of the new database\n"
    "@args name\n"
    "@param name The name of the new report database\n"
    "@return The index of the new database\n"
    "This method returns an index of the new report database. Use \\rdb to get the actual object. "
    "If a report database with the given name already exists, a unique name will be created.\n"
    "The name will be replaced by the file name when a file is loaded into the report database.\n"
  ) +
  method ("show_rdb", &lay::LayoutView::open_rdb_browser,
    "@brief Shows a report database in the marker browser on a certain layout\n"
    "@args rdb_index, cv_index\n"
    "The marker browser is opened showing the report database with the index given by \"rdb_index\".\n"
    "It will be attached (i.e. navigate to) the layout with the given cellview index in \"cv_index\".\n"
  ) +
  //  HINT: the cast is important to direct GSI to the LayoutView method rather than the
  //  Plugin method (in which case we get a segmentation violation ..)
  method ("clear_config", (void (lay::LayoutView::*)()) &lay::LayoutView::clear_config,
    "@brief Clear the local configuration parameters\n"
    "\n"
    "See \\set_config for a description of the local configuration parameters."
  ) +
  method ("get_config", (std::string (lay::LayoutView::*)(const std::string &name) const) &lay::LayoutView::config_get,
    "@brief Get the value of a local configuration parameter\n"
    "\n"
    "@args name\n"
    "@param name The name of the configuration parameter whose value shall be obtained (a string)\n"
    "\n"
    "@return The value of the parameter\n"
    "\n"
    "See \\set_config for a description of the local configuration parameters."
  ) +
  method ("set_config", (void (lay::LayoutView::*)(const std::string &name, const std::string &value)) &lay::LayoutView::config_set,
    "@brief Set a local configuration parameter with the given name to the given value\n"
    "\n"
    "@args name, value\n"
    "@param name The name of the configuration parameter to set\n"
    "@param value The value to which to set the configuration parameter\n"
    "\n"
    "This method sets a local configuration parameter with the given name to the given value. "
    "Values can only be strings. Numerical values have to be converted into strings first. "
    "Local configuration parameters override global configurations for this specific view. "
    "This allows for example to override global settings of background colors. "
    "Any local settings are not written to the configuration file. "
  ) +
  method_ext ("transaction", &gsi::transaction,
    "@brief Begin a transaction\n"
    "\n"
    "@args description\n"
    "@param description A text that appears in the 'undo' description\n"
    "\n"
    "A transaction brackets a sequence of database modifications that appear as a single "
    "undo action. Only modifications that are wrapped inside a transaction..commit call pair "
    "can be undone.\n"
    "Each transaction must be terminated with a \\commit method call, even if some error occured. "
    "It is advisable therefore to catch errors and issue a commit call in this case.\n"
    "\n"
    "This method was introduced in version 0.16."
  ) +
  method_ext ("commit", &gsi::commit,
    "@brief End a transaction\n"
    "\n"
    "See \\transaction for a detailed description of transactions. "
    "\n"
    "This method was introduced in version 0.16."
  ) +
  method_ext ("is_transacting?", &gsi::transacting,
    "@brief Tell if a transaction is ongoing\n"
    "\n"
    "See \\transaction for a detailed description of transactions. "
    "\n"
    "This method was introduced in version 0.16."
  ) +
  method_ext ("clear_transactions", &gsi::clear_transactions,
    "@brief Clear all transactions\n"
    "\n"
    "Discard all actions in the undo buffer. After clearing that buffer, no undo is available. "
    "It is important to clear the buffer when making database modifications outside transactions, i.e "
    "after that modifications have been done. If failing to do so, 'undo' operations are likely to produce "
    "invalid results."
    "\n"
    "This method was introduced in version 0.16."
  ),
  "@brief The view object presenting one or more layout objects\n"
  "\n"
  "The visual part of the view is the tab panel in the main window. The non-visual part "
  "are the redraw thread, the layout handles, cell lists, layer view lists etc. "
  "This object controls these aspects of the view and controls the appearance of the data."
);

static db::Layout &get_layout (const lay::CellView *cv)
{
  return (*cv)->layout ();
}

static const std::string &name (const lay::CellView *cv)
{
  return (*cv)->name ();
}

static const std::string &filename (const lay::CellView *cv)
{
  return (*cv)->filename ();
}

static void apply_technology (const lay::CellView *cv, const std::string &tech)
{
  (*cv)->apply_technology (tech);
}

static std::string get_technology (const lay::CellView *cv)
{
  return (*cv)->tech_name ();
}

static const lay::CellView *get_active_cellview ()
{
  lay::MainWindow *mw = lay::Application::instance ()->main_window ();
  if (! mw) {
    return 0;
  }
  lay::LayoutView *view = mw->current_view ();
  if (! view) {
    return 0;
  }
  if (view->active_cellview_index () >= 0) {
    return &view->active_cellview ();
  } else {
    return 0;
  }
}

static void set_cell (lay::CellView *cv, db::Cell *cell)
{
  if (! cell) {
    cv->reset_cell ();
  } else {
    cv->set_cell (cell->cell_index ());
  }
}

static std::string get_cell_name (const lay::CellView *cv)
{
  if (cv->cell () == 0) {
    return std::string ();
  } else {
    return (*cv)->layout ().cell_name (cv->cell_index ());
  }
}

Class<lay::CellView> decl_CellView ("CellView", 
  method ("==", &lay::CellView::operator==,
    "@brief Equality: compares the cell the view points to, not the path\n"
    "@args other\n"
  ) +
  method ("active", &get_active_cellview,
    "@brief Gets the active CellView\n"
    "The active CellView is the one that is selected in the current layout view. This method is "
    "equivalent to\n"
    "@code\n"
    "RBA::Application::instance.main_window.current_view.active_cellview\n"
    "@/code\n"
    "If no CellView is active, this method returns nil.\n"
    "\n"
    "This method has been introduced in version 0.23."
  ) + 
  method ("is_valid?", &lay::CellView::is_valid,
    "@brief Test if the view points to a valid cell\n"
  ) +
  method ("path=|set_path", &lay::CellView::set_unspecific_path,
    "@brief Set the unspecific part of the path explicitly\n"
    "@args path\n"
    "\n"
    "Setting the unspecific part of the path will clear the context path component and\n"
    "update the context and target cell.\n"
  ) +
  method ("context_path=|set_context_path", &lay::CellView::set_specific_path,
    "@brief Set the context path explicitly\n"
    "@args path\n"
    "\n"
    "This method assumes that the unspecific part of the path \n"
    "is established already and that the context path starts\n"
    "from the context cell.\n"
  ) +
  method ("cell_index=|set_cell", (void (lay::CellView::*) (lay::CellView::cell_index_type)) &lay::CellView::set_cell,
    "@brief Set the path to the given cell\n"
    "@args cell_index\n"
    "\n"
    "This method will construct any path to this cell, not a \n"
    "particular one. It will clear the context path\n"
    "and update the context and target cell. Note that the cell is specified by it's index.\n"
  ) +
  method ("cell_name=|set_cell_name", (void (lay::CellView::*) (const std::string &)) &lay::CellView::set_cell,
    "@brief Set the cell by name\n"
    "@args cell_name\n"
    "\n"
    "If the name is not a valid one, the cellview will become\n"
    "invalid.\n"
    "This method will construct any path to this cell, not a \n"
    "particular one. It will clear the context path\n"
    "and update the context and target cell.\n"
  ) +
  method_ext ("cell=", set_cell,
    "@brief Set the cell by reference to a Cell object\n"
    "@args cell\n"
    "Setting the cell reference to nil invalidates the cellview. "
    "This method will construct any path to this cell, not a \n"
    "particular one. It will clear the context path\n"
    "and update the context and target cell.\n"
  ) + 
  method ("reset_cell", &lay::CellView::reset_cell,
    "@brief Reset the cell \n"
    "\n"
    "The cellview will become invalid. The layout object will\n"
    "still be attached to the cellview.\n"
  ) +
  method ("ctx_cell_index", &lay::CellView::ctx_cell_index,
    "@brief Get the context cell's index\n"
  ) +
  method ("ctx_cell", &lay::CellView::ctx_cell,
    "@brief Get the reference to the context cell currently addressed\n"
  ) +
  method ("cell_index", &lay::CellView::cell_index,
    "@brief Get the target cell's index\n"
  ) +
  method ("cell", &lay::CellView::cell,
    "@brief Get the reference to the target cell currently addressed\n"
  ) +
  method_ext ("cell_name", &get_cell_name,
    "@brief Get the name of the target cell currently addressed\n"
  ) +
  method_ext ("filename", &gsi::filename,
    "@brief Get filename associated with the layout behind the cellview\n"
  ) +
  method_ext ("name", &gsi::name,
    "@brief Get the unique name associated with the layout behind the cellview\n"
  ) +
  method ("path", &lay::CellView::unspecific_path,
    "@brief Get the cell's unspecific part of the path leading to the context cell\n"
  ) +
  method ("context_path", &lay::CellView::specific_path,
    "@brief Get the cell's context path\n"
    "The context path leads from the context cell to the target cell in a specific "
    "fashion, i.e. describing each instance in detail, not just be cell indices. If "
    "the context and target cell are identical, the context path is empty."
  ) +
  method_ext ("technology", &get_technology,
    "@brief Returns the technology name for the layout behind the given cell view\n"
    "This method has been added in version 0.23.\n"
  ) + 
  method_ext ("technology=", &apply_technology,
    "@brief Sets the technology for the layout behind the given cell view\n"
    "@args tech_name\n"
    "According to the specification of the technology, new layer properties may be loaded "
    "or the net tracer may be reconfigured. If the layout is shown in multiple views, the "
    "technology is updated for all views.\n"
    "This method has been added in version 0.22.\n"
  ) + 
  method_ext ("layout", &get_layout,
    "@brief Get the reference to the layout object addressed by this view\n"
  ),
  "@brief A class describing what is shown inside a layout view\n"
  "\n"
  "The cell view points to a specific cell within a certain layout and a hierarchical context.\n"
  "For that, first of all a layout pointer is provided. The cell itself\n"
  "is addressed by an cell_index or a cell object reference.\n"
  "The layout pointer can be nil, indicating that the cell view is invalid.\n"
  "\n"
  "The cell is not only identified by it's index or object but also \n"
  "by the path leading to that cell. This path indicates how to find the\n"
  "cell in the hierarchical context of it's parent cells. \n"
  "\n"
  "The path is in fact composed of two parts: first in an unspecific fashion,\n"
  "just describing which parent cells are used. The target of this path\n"
  "is called the \"context cell\". It is accessible by the \\ctx_cell_index\n"
  "or \\ctx_cell methods. In the viewer, the unspecific part of the path is\n"
  "the location of the cell in the cell tree.\n"
  "\n"
  "Additionally the path's second part may further identify a specific instance of a certain\n"
  "subcell in the context cell. This is done through a set of \\InstElement\n"
  "objects. The target of this specific path is the actual cell addressed by the\n"
  "cellview. This target cell is accessible by the \\cell_index or \\cell methods.\n"
  "In the viewer, the target cell is shown in the context of the context cell.\n"
  "The hierarchy levels are counted from the context cell, which is on level 0.\n"
  "If the context path is empty, the context cell is identical with the target cell.\n"
  "\n"
  "See @<a href=\"/programming/application_api.xml\">The Application API@</a> for more details about the "
  "application objects."
); 

}

