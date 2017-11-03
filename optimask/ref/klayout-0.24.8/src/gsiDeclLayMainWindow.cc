
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
#include "gsiDeclQtExternal.h"
#include "layMainWindow.h"

//  Import Qt bindings for QMainWindow
QT_EXTERNAL (QMainWindow)

namespace gsi
{

Class<lay::MainWindow> decl_MainWindow (QT_EXTERNAL_BASE (QMainWindow) "MainWindow", 

  //  QMainWindow interface
  method ("menu", &lay::MainWindow::menu, 
    "@brief Return a reference to the abstract menu\n"
    "\n"
    "@return A reference to an \\AbstractMenu object representing the menu system"
  ) +
  method ("instance", &lay::MainWindow::instance, 
    "@brief Gets application's main window instance\n"
    "\n"
    "This method has been added in version 0.24."
  ) +
  method ("manager", &lay::MainWindow::manager, 
    "@brief Gets the \\Manager object of this window\n"
    "\n"
    "The manager object is responsible to managing the undo/redo stack. Usually this object "
    "is not required. It's more convenient and safer to use the related methods provided by "
    "\\LayoutView (\\LayoutView#transaction, \\LayoutView#commit) and \\MainWindow (such as "
    "\\MainWindow#cm_undo and \\MainWindow#cm_redo).\n"
    "\n"
    "This method has been added in version 0.24."
  ) +
  method ("message", &lay::MainWindow::message, 
    "@brief Display a message in the status bar\n"
    "\n"
    "@args message,time\n"
    "@param message The message to display\n"
    "@param time The time how long to display the message in ms\n"
    "\n"
    "This given message is shown in the status bar for the given time.\n" 
    "\n"
    "This method has been added in version 0.18."
  ) +
  method ("resize", (void (lay::MainWindow::*)(int, int)) &lay::MainWindow::resize, 
    "@brief Resize the window\n"
    "\n"
    "@args width, height\n"
    "@param width The new width of the window\n"
    "@param height The new width of the window\n"
    "\n"
    "This method resizes the window to the given target size including decoration such as menu bar "
    "and control panels"
  ) +
  //  MainWindow interface
  method ("grid_micron", &lay::MainWindow::grid_micron,
    "@brief Get the global grid in micron\n"
    "\n"
    "@return The global grid in micron\n"
    "\n"
    "The global grid is used at various places, i.e. for ruler snapping, for grid display etc."
  ) +
  method ("create_view", &lay::MainWindow::create_view,
    "@brief Create a new, empty view\n"
    "\n"
    "@return The index of the view that was created\n"
    "\n"
    "Creates an empty view that can be filled with layouts using the load_layout and create_layout "
    "methods on the view object. Use the \\view method to obtain the view object from the view index."
    "\n"
    "This method has been added in version 0.22.\n"
  ) +
  method ("create_layout", (const lay::CellView &(lay::MainWindow::*) (int)) &lay::MainWindow::create_layout,
    "@brief Create a new, empty layout\n"
    "\n"
    "@args mode\n"
    "@param mode An integer value of 0, 1 or 2 that determines how the layout is created\n"
    "@return The cellview of the layout that was created\n"
    "\n"
    "Create the layout in the current view, replacing the current layouts (mode 0), "
    "in a new view (mode 1) or adding it to the current view (mode 2).\n"
    "In mode 1, the new view is made the current one.\n"
    "\n"
    "This version uses the initial technology and associates it with the new layout."
  ) +
  method ("create_layout", (const lay::CellView &(lay::MainWindow::*) (const std::string &, int)) &lay::MainWindow::create_layout,
    "@brief Create a new, empty layout with the given technology\n"
    "\n"
    "@args tech, mode\n"
    "@param mode An integer value of 0, 1 or 2 that determines how the layout is created\n"
    "@param tech The name of the technology to use for that layout.\n"
    "@return The cellview of the layout that was created\n"
    "\n"
    "Create the layout in the current view, replacing the current layouts (mode 0), "
    "in a new view (mode 1) or adding it to the current view (mode 2).\n"
    "In mode 1, the new view is made the current one.\n"
    "\n"
    "If the technology name is not a valid technology name, the default technology will be used.\n"
    "\n"
    "This version was introduced in version 0.22."
  ) +
  method ("load_layout", (const lay::CellView &(lay::MainWindow::*) (const std::string &, int)) &lay::MainWindow::load_layout,
    "@brief Load a new layout\n"
    "\n"
    "@args filename, mode\n"
    "@param filename The name of the file to load\n"
    "@param mode An integer value of 0, 1 or 2 that determines how the file is loaded\n"
    "@return The cellview into which the layout was loaded\n"
    "\n"
    "Loads the given file into the current view, replacing the current layouts (mode 0), "
    "into a new view (mode 1) or adding the layout to the current view (mode 2).\n"
    "In mode 1, the new view is made the current one.\n"
    "\n"
    "This version will use the initial technology and the default reader options. "
    "Others versions are provided which allow specification of technology and reader options explicitly.\n"
  ) +
  method ("load_layout", (const lay::CellView &(lay::MainWindow::*) (const std::string &, const std::string &, int)) &lay::MainWindow::load_layout,
    "@brief Load a new layout and associate it with the given technology\n"
    "\n"
    "@args filename, tech, mode\n"
    "@param filename The name of the file to load\n"
    "@param tech The name of the technology to use for that layout.\n"
    "@param mode An integer value of 0, 1 or 2 that determines how the file is loaded\n"
    "@return The cellview into which the layout was loaded\n"
    "\n"
    "Loads the given file into the current view, replacing the current layouts (mode 0), "
    "into a new view (mode 1) or adding the layout to the current view (mode 2).\n"
    "In mode 1, the new view is made the current one.\n"
    "\n"
    "If the technology name is not a valid technology name, the default technology will be used.\n"
    "\n"
    "This version was introduced in version 0.22."
  ) +
  method ("load_layout", (const lay::CellView &(lay::MainWindow::*) (const std::string &, const db::LoadLayoutOptions &, int)) &lay::MainWindow::load_layout,
    "@brief Load a new layout with the given options\n"
    "\n"
    "@args filename, options, mode\n"
    "@param filename The name of the file to load\n"
    "@param options The reader options to use.\n"
    "@param mode An integer value of 0, 1 or 2 that determines how the file is loaded\n"
    "@return The cellview into which the layout was loaded\n"
    "\n"
    "Loads the given file into the current view, replacing the current layouts (mode 0), "
    "into a new view (mode 1) or adding the layout to the current view (mode 2).\n"
    "In mode 1, the new view is made the current one.\n"
    "\n"
    "This version was introduced in version 0.22."
  ) +
  method ("load_layout", (const lay::CellView &(lay::MainWindow::*) (const std::string &, const db::LoadLayoutOptions &, const std::string &, int)) &lay::MainWindow::load_layout,
    "@brief Load a new layout with the given options and associate it with the given technology\n"
    "\n"
    "@args filename, options, tech, mode\n"
    "@param filename The name of the file to load\n"
    "@param options The reader options to use.\n"
    "@param tech The name of the technology to use for that layout.\n"
    "@param mode An integer value of 0, 1 or 2 that determines how the file is loaded\n"
    "@return The cellview into which the layout was loaded\n"
    "\n"
    "Loads the given file into the current view, replacing the current layouts (mode 0), "
    "into a new view (mode 1) or adding the layout to the current view (mode 2).\n"
    "In mode 1, the new view is made the current one.\n"
    "\n"
    "If the technology name is not a valid technology name, the default technology will be used.\n"
    "\n"
    "This version was introduced in version 0.22."
  ) +
  method ("clone_current_view", &lay::MainWindow::clone_current_view,
    "@brief Clone the current view and make it current\n"
  ) +
  method ("save_session", &lay::MainWindow::save_session, 
    "@brief Save the session to the given file\n"
    "\n"
    "@args fn\n"
    "@param fn The path to the session file\n"
    "\n"
    "The session is saved to the given session file. Any existing layout edits are not automatically saved together with "
    "the session. The session just holds display settings and annotation objects. If layout edits exist, they have to be "
    "saved explicitly in a separate step.\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  method ("restore_session", &lay::MainWindow::restore_session, 
    "@brief Restore a session from the given file\n"
    "\n"
    "@args fn\n"
    "@param fn The path to the session file\n"
    "\n"
    "The session stored in the given session file is restored. All existing views are closed and all "
    "layout edits are discarded without notification.\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  method ("enable_edits", &lay::MainWindow::enable_edits,
    "@brief Enable or disable edits\n"
    "\n"
    "@args enable\n"
    "@param enable Enable edits if set to true\n"
    "\n"
    "This method allows putting the application into read-only mode by disabling all edit "
    "functions. For doing so, this method has to be called with a 'false' argument. Calling it "
    "with a 'true' parameter enables all edits again. This method must not be confused with the "
    "edit/viewer mode. That mode is a global option with heavy impact on the way, the database "
    "manages it's content. The MainWindow's enable_edits method is intended to temporarily disable "
    "all menu entries and functions which could allow the user to alter the database."
  ) +
  method ("synchronous=|#synchroneous", &lay::MainWindow::set_synchronous,
    "@brief Put the main window into synchronous mode\n"
    "\n"
    "@args sync_mode\n"
    "@param sync_mode 'true' if the application should behave synchronously\n"
    "\n"
    "In synchronous mode, an application is allowed to block on redraw. While redrawing, "
    "no user interactions are possible. Although this is not desirable for smooth operation, "
    "it can be beneficial for test or automation purposes, i.e. if a screenshot needs to be "
    "produced once the application has finished drawing."
  ) +
  method ("close_all", &lay::MainWindow::close_all,
    "@brief Closes all views\n"
    "\n"
    "This method unconditionally closes all views. No dialog will be opened if unsaved edits exist.\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  method ("close_current_view", &lay::MainWindow::close_current_view,
    "@brief Close the current view\n"
    "\n"
    "This method does not open a dialog to query which cell view to close if multiple cells "
    "are opened in the view but rather closes all cells."
  ) +
  method ("cancel", &lay::MainWindow::cancel,
    "@brief Cancel current editing operations\n"
    "\n"
    "This method call cancels all current editing operations and restores normal mouse mode."
  ) +
  method ("redraw", &lay::MainWindow::redraw,
    "@brief Redraw the current view\n"
    "\n"
    "Issues a redraw request to the current view. This usually happens automatically, so this method does not "
    "need to be called in most relevant cases. "
  ) +
  method ("exit", &lay::MainWindow::exit,
    "@brief Schedule an exit for the application\n"
    "\n"
    "This method does not immediately exit the application but sends an exit request "
    "to the application which will cause a clean shutdown of the GUI. "
  ) +
  method ("select_view", &lay::MainWindow::select_view,
    "@brief Select the view with the given index\n"
    "\n"
    "@args index\n"
    "@param index The index of the view to select (0 is the first)\n"
    "\n"
    "This method will make the view with the given index the current (front) view."
  ) +
  method ("current_view_index", &lay::MainWindow::current_view_index,
    "@brief Return the current view's index\n"
    "\n"
    "@return The index of the current view\n"
    "\n"
    "This method will return the index of the current view."
  ) +
  method ("current_view", (lay::LayoutView *(lay::MainWindow::*)()) &lay::MainWindow::current_view,
    "@brief Return a reference to the current view's object\n"
    "\n"
    "@return A reference to a \\LayoutView object representing the current view."
  ) +
  method ("views", &lay::MainWindow::views,
    "@brief Return the number of views\n"
    "\n"
    "@return The number of views available so far.\n"
  ) +
  method ("view", (lay::LayoutView *(lay::MainWindow::*)(int)) &lay::MainWindow::view,
    "@brief Return a reference to a view object by index\n"
    "@args n\n"
    "\n"
    "@return The view object's reference for the view with the given index.\n"
  ) +
  method ("reader_options", (db::LoadLayoutOptions &(lay::MainWindow::*)()) &lay::MainWindow::reader_options,
    "@brief Access to the current reader options\n"
    "\n"
    "Modifying the current reader options will have an effect on the next load_layout operation which does not specify options explicitly but "
    "might not be reflected correctly in the reader options dialog and changes will be reset when the "
    "application is restarted."
    "\n"
    "@return A reference to the current reader object.\n"
    "\n"
    "This method was added in version 0.18."
  ) +
  method ("initial_technology", &lay::MainWindow::initial_technology,
    "@brief Gets the technology used for creating or loading layouts (unless explicitly specified)\n"
    "\n"
    "@return The current initial technology"
    "\n"
    "This method was added in version 0.22."
  ) +
  method ("initial_technology=", &lay::MainWindow::set_initial_technology,
    "@brief Sets the technology used for creating or loading layouts (unless explicitly specified)\n"
    "\n"
    "Setting the technology will have an effect on the next load_layout or create_layout operation which does not explicitly specify the technology but "
    "might not be reflected correctly in the reader options dialog and changes will be reset when the "
    "application is restarted."
    "\n"
    "@args tech\n"
    "@param tech The new initial technology\n"
    "\n"
    "This method was added in version 0.22."
  ) +
  method ("add_current_view_observer", &lay::MainWindow::add_current_view_observer,
    "@brief Add an observer for the \"current view changed\" event\n"
    "@args observer\n"
    "\n"
    "If the current view changes, this observer is triggered. The integer slot of the\n"
    "observer will receive the number of the view active before. The current view's reference\n"
    "is already updated when this event is issued.\n"
  ) +
  method ("remove_current_view_observer", &lay::MainWindow::remove_current_view_observer,
    "@brief Remove an observer for the change of the \"current view changed\" event\n"
    "@args observer\n"
  ) +
  method ("add_new_view_observer", &lay::MainWindow::add_new_view_observer,
    "@brief Add an observer for a \"new view\" event\n"
    "@args observer\n"
    "\n"
    "If a new view is created, this observer will receive a signal.\n"
    "The integer slot of this observer will receive the index of the newly created view.\n"
  ) +
  method ("remove_new_view_observer", &lay::MainWindow::remove_new_view_observer,
    "@brief Remove an observer for a \"new view\" event\n"
    "@args observer\n"
  ) +
  method ("cm_select_all", &lay::MainWindow::cm_select_all,
    "@brief 'cm_select_all' action (bound to a menu)"
    "\n"
    "This method has been added in version 0.22.\n"
  ) +
  method ("cm_unselect_all", &lay::MainWindow::cm_unselect_all,
    "@brief 'cm_unselect_all' action (bound to a menu)"
    "\n"
    "This method has been added in version 0.22.\n"
  ) +
  method ("cm_undo", &lay::MainWindow::cm_undo,
    "@brief 'cm_undo' action (bound to a menu)"
  ) +
  method ("cm_redo", &lay::MainWindow::cm_redo,
    "@brief 'cm_redo' action (bound to a menu)"
  ) +
  method ("cm_delete", &lay::MainWindow::cm_delete,
    "@brief 'cm_delete' action (bound to a menu)"
  ) +
  method ("cm_show_properties", &lay::MainWindow::cm_show_properties,
    "@brief 'cm_show_properties' action (bound to a menu)"
  ) +
  method ("cm_copy", &lay::MainWindow::cm_copy,
    "@brief 'cm_copy' action (bound to a menu)"
  ) +
  method ("cm_paste", &lay::MainWindow::cm_paste,
    "@brief 'cm_paste' action (bound to a menu)"
  ) +
  method ("cm_cut", &lay::MainWindow::cm_cut,
    "@brief 'cm_cut' action (bound to a menu)"
  ) +
  method ("cm_zoom_fit_sel", &lay::MainWindow::cm_zoom_fit,
    "@brief 'cm_zoom_fit_sel' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_zoom_fit", &lay::MainWindow::cm_zoom_fit,
    "@brief 'cm_zoom_fit' action (bound to a menu)"
  ) +
  method ("cm_zoom_in", &lay::MainWindow::cm_zoom_in,
    "@brief 'cm_zoom_in' action (bound to a menu)"
  ) +
  method ("cm_zoom_out", &lay::MainWindow::cm_zoom_out,
    "@brief 'cm_zoom_out' action (bound to a menu)"
  ) +
  method ("cm_pan_up", &lay::MainWindow::cm_pan_up,
    "@brief 'cm_pan_up' action (bound to a menu)"
  ) +
  method ("cm_pan_down", &lay::MainWindow::cm_pan_down,
    "@brief 'cm_pan_down' action (bound to a menu)"
  ) +
  method ("cm_pan_left", &lay::MainWindow::cm_pan_left,
    "@brief 'cm_pan_left' action (bound to a menu)"
  ) +
  method ("cm_pan_right", &lay::MainWindow::cm_pan_right,
    "@brief 'cm_pan_right' action (bound to a menu)"
  ) +
  method ("cm_save_session", &lay::MainWindow::cm_save_session,
    "@brief 'cm_save_session' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_restore_session", &lay::MainWindow::cm_restore_session,
    "@brief 'cm_restore_session' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_setup", &lay::MainWindow::cm_setup,
    "@brief 'cm_setup' action (bound to a menu)"
  ) +
  method ("cm_save_as", &lay::MainWindow::cm_save_as,
    "@brief 'cm_save_as' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_save", &lay::MainWindow::cm_save,
    "@brief 'cm_save' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_save_all", &lay::MainWindow::cm_save_all,
    "@brief 'cm_save_all' action (bound to a menu)\n"
    "This method has been added in version 0.24.\n"
  ) +
  method ("cm_reload", &lay::MainWindow::cm_reload,
    "@brief 'cm_reload' action (bound to a menu)"
  ) +
  method ("cm_close", &lay::MainWindow::cm_close,
    "@brief 'cm_close' action (bound to a menu)"
  ) +
  method ("cm_close_all", &lay::MainWindow::cm_close_all,
    "@brief 'cm_close_all' action (bound to a menu)\n"
    "This method has been added in version 0.24.\n"
  ) +
  method ("cm_clone", &lay::MainWindow::cm_clone,
    "@brief 'cm_clone' action (bound to a menu)"
  ) +
  method ("cm_layout_props", &lay::MainWindow::cm_layout_props,
    "@brief 'cm_layout_props' action (bound to a menu)"
  ) +
  method ("cm_inc_max_hier", &lay::MainWindow::cm_inc_max_hier,
    "@brief 'cm_inc_max_hier' action (bound to a menu)"
  ) +
  method ("cm_dec_max_hier", &lay::MainWindow::cm_dec_max_hier,
    "@brief 'cm_dec_max_hier' action (bound to a menu)"
  ) +
  method ("cm_max_hier", &lay::MainWindow::cm_max_hier,
    "@brief 'cm_max_hier' action (bound to a menu)"
  ) +
  method ("cm_max_hier_0", &lay::MainWindow::cm_max_hier_0,
    "@brief 'cm_max_hier_0' action (bound to a menu)"
  ) +
  method ("cm_max_hier_1", &lay::MainWindow::cm_max_hier_1,
    "@brief 'cm_max_hier_1' action (bound to a menu)"
  ) +
  method ("cm_last_display_state", &lay::MainWindow::cm_last_display_state,
    "@brief 'cm_last_display_state' action (bound to a menu)"
  ) +
  method ("cm_next_display_state", &lay::MainWindow::cm_next_display_state,
    "@brief 'cm_next_display_state' action (bound to a menu)"
  ) +
  method ("cm_cancel", &lay::MainWindow::cm_cancel,
    "@brief 'cm_cancel' action (bound to a menu)"
  ) +
  method ("cm_redraw", &lay::MainWindow::cm_redraw,
    "@brief 'cm_redraw' action (bound to a menu)"
  ) +
  method ("cm_screenshot", &lay::MainWindow::cm_screenshot,
    "@brief 'cm_screenshot' action (bound to a menu)"
  ) +
  method ("cm_save_layer_props", &lay::MainWindow::cm_save_layer_props,
    "@brief 'cm_save_layer_props' action (bound to a menu)"
  ) +
  method ("cm_load_layer_props", &lay::MainWindow::cm_load_layer_props,
    "@brief 'cm_load_layer_props' action (bound to a menu)"
  ) +
  method ("cm_save_bookmarks", &lay::MainWindow::cm_save_bookmarks,
    "@brief 'cm_save_bookmarks' action (bound to a menu)"
  ) +
  method ("cm_load_bookmarks", &lay::MainWindow::cm_load_bookmarks,
    "@brief 'cm_load_bookmarks' action (bound to a menu)"
  ) +
  method ("cm_select_cell", &lay::MainWindow::cm_select_cell,
    "@brief 'cm_select_cell' action (bound to a menu)"
  ) +
  method ("cm_select_current_cell", &lay::MainWindow::cm_select_current_cell,
    "@brief 'cm_select_current_cell' action (bound to a menu)"
  ) +
  method ("cm_print", &lay::MainWindow::cm_print,
    "@brief 'cm_print' action (bound to a menu)\n"
    "This method has been added in version 0.21.13."
  ) +
  method ("cm_exit", &lay::MainWindow::cm_exit,
    "@brief 'cm_exit' action (bound to a menu)"
  ) +
  method ("cm_view_log", &lay::MainWindow::cm_view_log,
    "@brief 'cm_view_log' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_bookmark_view", &lay::MainWindow::cm_bookmark_view,
    "@brief 'cm_bookmark_view' action (bound to a menu)"
  ) +
  method ("cm_manage_bookmarks", &lay::MainWindow::cm_manage_bookmarks,
    "@brief 'cm_manage_bookmarks' action (bound to a menu)"
  ) +
  method ("cm_macro_editor", &lay::MainWindow::cm_macro_editor,
    "@brief 'cm_macro_editor' action (bound to a menu)"
  ) +
  method ("cm_goto_position", &lay::MainWindow::cm_goto_position,
    "@brief 'cm_goto_position' action (bound to a menu)"
  ) +
  method ("cm_help_about", &lay::MainWindow::cm_help_about,
    "@brief 'cm_help_about' action (bound to a menu)"
  ) +
  method ("cm_technologies", &lay::MainWindow::cm_technologies,
    "@brief 'cm_technologies' action (bound to a menu)"
    "\nThis method has been added in version 0.22."
  ) +
  method ("cm_open_too", &lay::MainWindow::cm_open_too,
    "@brief 'cm_open_too' action (bound to a menu)"
  ) +
  method ("cm_open_new_view", &lay::MainWindow::cm_open_new_view,
    "@brief 'cm_open_new_view' action (bound to a menu)"
  ) +
  method ("cm_open", &lay::MainWindow::cm_open,
    "@brief 'cm_open' action (bound to a menu)"
  ) +
  method ("cm_pull_in", &lay::MainWindow::cm_pull_in,
    "@brief 'cm_pull_in' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_reader_options", &lay::MainWindow::cm_reader_options,
    "@brief 'cm_reader_options' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_new_layout", &lay::MainWindow::cm_new_layout,
    "@brief 'cm_new_layout' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_new_panel", &lay::MainWindow::cm_new_panel,
    "@brief 'cm_new_panel' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_adjust_origin", &lay::MainWindow::cm_adjust_origin,
    "@brief 'cm_adjust_origin' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_new_cell", &lay::MainWindow::cm_new_cell,
    "@brief 'cm_new_cell' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_new_layer", &lay::MainWindow::cm_new_layer,
    "@brief 'cm_new_layer' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_clear_layer", &lay::MainWindow::cm_clear_layer,
    "@brief 'cm_clear_layer' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_delete_layer", &lay::MainWindow::cm_delete_layer,
    "@brief 'cm_delete_layer' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_edit_layer", &lay::MainWindow::cm_edit_layer,
    "@brief 'cm_edit_layer' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_copy_layer", &lay::MainWindow::cm_copy_layer,
    "@brief 'cm_copy_layer' action (bound to a menu)"
    "\nThis method has been added in version 0.22."
  ) +
  method ("cm_sel_flip_x", &lay::MainWindow::cm_sel_flip_x,
    "@brief 'cm_sel_flip_x' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_flip_y", &lay::MainWindow::cm_sel_flip_y,
    "@brief 'cm_sel_flip_y' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_rot_cw", &lay::MainWindow::cm_sel_rot_cw,
    "@brief 'cm_sel_rot_cw' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_rot_ccw", &lay::MainWindow::cm_sel_rot_ccw,
    "@brief 'cm_sel_rot_ccw' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_free_rot", &lay::MainWindow::cm_sel_free_rot,
    "@brief 'cm_sel_free_rot' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_scale", &lay::MainWindow::cm_sel_scale,
    "@brief 'cm_sel_scale' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_move", &lay::MainWindow::cm_sel_move,
    "@brief 'cm_sel_move' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_sel_move_to", &lay::MainWindow::cm_sel_move_to,
    "@brief 'cm_sel_move_to' action (bound to a menu)"
    "\nThis method has been added in version 0.24."
  ) +
  method ("cm_lv_new_tab", &lay::MainWindow::cm_lv_new_tab,
    "@brief 'cm_lv_new_tab' action (bound to a menu)"
  ) +
  method ("cm_lv_remove_tab", &lay::MainWindow::cm_lv_remove_tab,
    "@brief 'cm_lv_remove_tab' action (bound to a menu)"
  ) +
  method ("cm_lv_rename_tab", &lay::MainWindow::cm_lv_rename_tab,
    "@brief 'cm_lv_rename_tab' action (bound to a menu)"
  ) +
  method ("cm_lv_hide", &lay::MainWindow::cm_lv_hide,
    "@brief 'cm_lv_hide' action (bound to a menu)"
  ) +
  method ("cm_lv_hide_all", &lay::MainWindow::cm_lv_hide_all,
    "@brief 'cm_lv_hide_all' action (bound to a menu)"
  ) +
  method ("cm_lv_show", &lay::MainWindow::cm_lv_show,
    "@brief 'cm_lv_show' action (bound to a menu)"
  ) +
  method ("cm_lv_show_all", &lay::MainWindow::cm_lv_show_all,
    "@brief 'cm_lv_show_all' action (bound to a menu)"
  ) +
  method ("cm_lv_show_only", &lay::MainWindow::cm_lv_show_only,
    "@brief 'cm_lv_show_only' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_lv_rename", &lay::MainWindow::cm_lv_rename,
    "@brief 'cm_lv_rename' action (bound to a menu)"
  ) +
  method ("cm_lv_select_all", &lay::MainWindow::cm_lv_select_all,
    "@brief 'cm_lv_select_all' action (bound to a menu)"
  ) +
  method ("cm_lv_delete", &lay::MainWindow::cm_lv_delete,
    "@brief 'cm_lv_delete' action (bound to a menu)"
  ) +
  method ("cm_lv_insert", &lay::MainWindow::cm_lv_insert,
    "@brief 'cm_lv_insert' action (bound to a menu)"
  ) +
  method ("cm_lv_group", &lay::MainWindow::cm_lv_group,
    "@brief 'cm_lv_group' action (bound to a menu)"
  ) +
  method ("cm_lv_ungroup", &lay::MainWindow::cm_lv_ungroup,
    "@brief 'cm_lv_ungroup' action (bound to a menu)"
  ) +
  method ("cm_lv_source", &lay::MainWindow::cm_lv_source,
    "@brief 'cm_lv_source' action (bound to a menu)"
  ) +
  method ("cm_lv_sort_by_name", &lay::MainWindow::cm_lv_sort_by_name,
    "@brief 'cm_lv_sort_by_name' action (bound to a menu)"
  ) +
  method ("cm_lv_sort_by_ild", &lay::MainWindow::cm_lv_sort_by_ild,
    "@brief 'cm_lv_sort_by_ild' action (bound to a menu)"
  ) +
  method ("cm_lv_sort_by_idl", &lay::MainWindow::cm_lv_sort_by_idl,
    "@brief 'cm_lv_sort_by_idl' action (bound to a menu)"
  ) +
  method ("cm_lv_sort_by_ldi", &lay::MainWindow::cm_lv_sort_by_ldi,
    "@brief 'cm_lv_sort_by_ldi' action (bound to a menu)"
  ) +
  method ("cm_lv_sort_by_dli", &lay::MainWindow::cm_lv_sort_by_dli,
    "@brief 'cm_lv_sort_by_dli' action (bound to a menu)"
  ) +
  method ("cm_lv_regroup_by_index", &lay::MainWindow::cm_lv_regroup_by_index,
    "@brief 'cm_lv_regroup_by_index' action (bound to a menu)"
  ) +
  method ("cm_lv_regroup_by_datatype", &lay::MainWindow::cm_lv_regroup_by_datatype,
    "@brief 'cm_lv_regroup_by_datatype' action (bound to a menu)"
  ) +
  method ("cm_lv_regroup_by_layer", &lay::MainWindow::cm_lv_regroup_by_layer,
    "@brief 'cm_lv_regroup_by_layer' action (bound to a menu)"
  ) +
  method ("cm_lv_regroup_flatten", &lay::MainWindow::cm_lv_regroup_flatten,
    "@brief 'cm_lv_regroup_flatten' action (bound to a menu)"
  ) +
  method ("cm_lv_expand_all", &lay::MainWindow::cm_lv_expand_all,
    "@brief 'cm_lv_expand_all' action (bound to a menu)"
  ) +
  method ("cm_lv_add_missing", &lay::MainWindow::cm_lv_add_missing,
    "@brief 'cm_lv_add_missing' action (bound to a menu)"
  ) +
  method ("cm_lv_remove_unused", &lay::MainWindow::cm_lv_remove_unused,
    "@brief 'cm_lv_remove_unused' action (bound to a menu)"
  ) +
  method ("cm_cell_delete", &lay::MainWindow::cm_cell_delete,
    "@brief 'cm_cell_delete' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_cell_rename", &lay::MainWindow::cm_cell_rename,
    "@brief 'cm_cell_rename' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_cell_copy", &lay::MainWindow::cm_cell_copy,
    "@brief 'cm_cell_copy' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_cell_cut", &lay::MainWindow::cm_cell_cut,
    "@brief 'cm_cell_cut' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_cell_paste", &lay::MainWindow::cm_cell_paste,
    "@brief 'cm_cell_paste' action (bound to a menu)"
    "\nThis method has been added in version 0.20."
  ) +
  method ("cm_cell_select", &lay::MainWindow::cm_cell_select,
    "@brief 'cm_cell_select' action (bound to a menu)"
  ) +
  method ("cm_open_current_cell", &lay::MainWindow::cm_open_current_cell,
    "@brief 'cm_open_current_cell' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_save_current_cell_as", &lay::MainWindow::cm_save_current_cell_as,
    "@brief 'cm_save_current_cell_as' action (bound to a menu)"
    "\nThis method has been added in version 0.18."
  ) +
  method ("cm_cell_hide", &lay::MainWindow::cm_cell_hide,
    "@brief 'cm_cell_hide' action (bound to a menu)"
  ) +
  method ("cm_cell_flatten", &lay::MainWindow::cm_cell_flatten,
    "@brief 'cm_cell_flatten' action (bound to a menu)"
  ) +
  method ("cm_cell_show", &lay::MainWindow::cm_cell_show,
    "@brief 'cm_cell_show' action (bound to a menu)"
  ) +
  method ("cm_cell_show_all", &lay::MainWindow::cm_cell_show_all,
    "@brief 'cm_cell_show_all' action (bound to a menu)"
  ) +
  method ("cm_navigator_close", &lay::MainWindow::cm_navigator_close,
    "@brief 'cm_navigator_close' action (bound to a menu)"
  ) +
  method ("cm_navigator_freeze", &lay::MainWindow::cm_navigator_freeze,
    "@brief 'cm_navigator_freeze' action (bound to a menu)"
  ),

  "@brief The main application window and central controller object\n"
  "\n"
  "This object first is the main window but also the main controller. The main controller "
  "is the port by which access can be gained to all the data objects, view and other aspects "
  "of the program."
);

}

