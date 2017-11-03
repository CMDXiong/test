
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


#include "layLayoutView.h"
#include "laySaveLayoutOptionsDialog.h"
#include "layStream.h"
#include "layExceptions.h"
#include "dbSaveLayoutOptions.h"
#include "dbStream.h"
#include "tlClassRegistry.h"

#include <QtGui/QScrollArea>

#include <memory>

namespace lay
{

static const StreamWriterPluginDeclaration *plugin_for_format (const std::string &format_name)
{
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamWriterPluginDeclaration *decl = dynamic_cast <const StreamWriterPluginDeclaration *> (&*cls);
    if (decl && decl->format_name () == format_name) {
      return decl;
    }
  }
  return 0;
}

static unsigned int om_to_index (tl::OutputStream::OutputStreamMode om)
{
  if (om == tl::OutputStream::OM_Plain) {
    return 1;
  } else if (om == tl::OutputStream::OM_Zlib) {
    return 2; 
  } else {
    return 0;
  }
}

static tl::OutputStream::OutputStreamMode index_to_om (unsigned int i)
{
  if (i == 1) {
    return tl::OutputStream::OM_Plain;
  } else if (i == 2) {
    return tl::OutputStream::OM_Zlib;
  } else {
    return tl::OutputStream::OM_Auto;
  }
}

// -----------------------------------------------------------------
//  SaveLayoutOptionsDialog implementation

SaveLayoutOptionsDialog::SaveLayoutOptionsDialog (QWidget *parent, const std::string &title)
  : QDialog (parent), Ui::SaveLayoutOptionsDialog ()
{
  setObjectName (QString::fromAscii ("save_options_dialog"));

  Ui::SaveLayoutOptionsDialog::setupUi (this);

  setWindowTitle (tl::to_qstring (title));
  
  options_stack->clear ();

  for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end (); ++fmt) {

    if (fmt->can_write ()) {

      StreamWriterOptionsPage *page = 0;

      //  obtain the config page from the plugin which we identify by format name
      const StreamWriterPluginDeclaration *decl = plugin_for_format (fmt->format_name ());
      if (decl) {
        page = decl->format_specific_options_page (options_stack);
      }

      if (page) {
        m_pages.push_back (std::make_pair (page, fmt->format_name ()));
        options_stack->addTab (page, QString::fromUtf8 (fmt->format_name ().c_str ()));
      }

    }

  }

  mp_current_tab = options_stack->currentWidget ();

  connect (buttonBox, SIGNAL (accepted ()), this, SLOT (ok_button_pressed ()));
  connect (options_stack, SIGNAL (currentChanged (int)), this, SLOT (current_tab_changed (int)));
}

SaveLayoutOptionsDialog::~SaveLayoutOptionsDialog ()
{
  //  .. nothing yet ..
}

void
SaveLayoutOptionsDialog::commit_current_page (lay::PluginRoot *config_root)
{
  //  the pages might share configuration parameters so we must transfer them: for this,
  //  we first commit all pages to the configuration in the first pass, then commit the active 
  //  page, setting it's own configuration parameters. Then we update the new page from that
  //  configuration

  for (int pass = 0; pass < 2; ++pass) {

    for (std::vector< std::pair<StreamWriterOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {

      if (!pass || page->first == mp_current_tab) {

        const StreamWriterPluginDeclaration *decl = plugin_for_format (page->second);
        std::auto_ptr<db::FormatSpecificWriterOptions> specific_options;
        specific_options.reset (decl->create_specific_options (config_root));

        if (specific_options.get ()) {
          page->first->commit (specific_options.get (), false);
          decl->commit_to_config (specific_options.get (), config_root);
        }

      }

    }

  }
}

void 
SaveLayoutOptionsDialog::current_tab_changed (int index)
{
  BEGIN_PROTECTED

  try {

    lay::PluginRoot config_root;
    commit_current_page (&config_root);

    mp_current_tab = options_stack->widget (index);

    for (std::vector< std::pair<StreamWriterOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {

      if (page->first == mp_current_tab) {
      
        const StreamWriterPluginDeclaration *decl = plugin_for_format (page->second);
        std::auto_ptr<db::FormatSpecificWriterOptions> specific_options;
        specific_options.reset (decl->create_specific_options (&config_root));

        if (specific_options.get ()) {
          page->first->setup (specific_options.get ());
        }

      }

    }

  } catch (...) {

    //  On error, we stay in the current page
    options_stack->blockSignals (true);
    options_stack->setCurrentWidget (mp_current_tab);
    options_stack->blockSignals (false);

    throw;

  }

  END_PROTECTED
}

void
SaveLayoutOptionsDialog::ok_button_pressed ()
{
  BEGIN_PROTECTED

  lay::PluginRoot config_root;
  commit_current_page (&config_root);

  accept ();

  END_PROTECTED
}

bool 
SaveLayoutOptionsDialog::exec (lay::PluginRoot *config_root)
{
  for (std::vector< std::pair<StreamWriterOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {

    const StreamWriterPluginDeclaration *decl = plugin_for_format (page->second);
    std::auto_ptr<db::FormatSpecificWriterOptions> specific_options;
    specific_options.reset (decl->create_specific_options (config_root));

    if (specific_options.get ()) {
      page->first->setup (specific_options.get ());
    }

  }

  bool ret = false;

  if (QDialog::exec ()) {

    //  Finally commit the pages to the configuration 
    commit_current_page (config_root);
    config_root->config_end ();

    ret = true;

  }

  return ret;
}

// -----------------------------------------------------------------
//  SaveLayoutAsOptionsDialog implementation

SaveLayoutAsOptionsDialog::SaveLayoutAsOptionsDialog (QWidget *parent, const std::string &title)
  : QDialog (parent), Ui::SaveLayoutAsOptionsDialog ()
{
  setObjectName (QString::fromAscii ("save_layout_options_dialog"));

  Ui::SaveLayoutAsOptionsDialog::setupUi (this);

  setWindowTitle (tl::to_qstring (title));

  QWidget *empty_widget = new QWidget (options_stack);
  int empty_widget_index = options_stack->addWidget (empty_widget);

  for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end (); ++fmt) {

    if (fmt->can_write ()) {

      fmt_cbx->addItem (tl::to_qstring (fmt->format_desc ()));

      StreamWriterOptionsPage *page = 0;

      //  obtain the config page from the plugin which we identify by format name
      const StreamWriterPluginDeclaration *decl = plugin_for_format (fmt->format_name ());
      if (decl) {
        page = decl->format_specific_options_page (options_stack);
      }

      m_pages.push_back (std::make_pair (page, fmt->format_name ()));
      m_tab_positions.push_back (page ? options_stack->addWidget (page) : empty_widget_index);

    }

  }

  connect (buttonBox, SIGNAL (accepted ()), this, SLOT (ok_button_pressed ()));
  connect (fmt_cbx, SIGNAL (activated (int)), this, SLOT (fmt_cbx_changed (int)));
}

SaveLayoutAsOptionsDialog::~SaveLayoutAsOptionsDialog ()
{
  //  .. nothing yet ..
}

void
SaveLayoutAsOptionsDialog::ok_button_pressed ()
{
  BEGIN_PROTECTED

  //  get the name of the currently selected format 
  int index = fmt_cbx->currentIndex ();
  std::string fmt_name;
  for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end () && index >= 0; ++fmt) {
    if (fmt->can_write ()) {
      if (index-- == 0) {
        fmt_name = fmt->format_name ();
        break;
      }
    }
  }

  // locate the options page for that and try to commit this 
  const StreamWriterPluginDeclaration *decl = plugin_for_format (fmt_name);
  if (decl) {
    for (std::vector< std::pair<StreamWriterOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {
      if (page->second == fmt_name) {
        if (page->first) {
          std::auto_ptr<db::FormatSpecificWriterOptions> options (decl->create_specific_options (0));
          if (options.get ()) {
            page->first->commit (options.get (), tl::OutputStream::output_mode_from_filename (m_filename, index_to_om (compression->currentIndex ())) != tl::OutputStream::OM_Plain);
          }
        }
        break;
      }
    }
  }

  double x = 0.0;
  tl::from_string (tl::to_string (dbu_le->text ()), x);
  tl::from_string (tl::to_string (sf_le->text ()), x);

  accept ();

  END_PROTECTED
}

bool 
SaveLayoutAsOptionsDialog::get_options (lay::LayoutView *view, unsigned int cv_index, const std::string &fn, tl::OutputStream::OutputStreamMode &om, db::SaveLayoutOptions &options, lay::PluginRoot *config_root, bool save_permanently)
{
  const lay::CellView &cv = view->cellview (cv_index);
  if (! cv.is_valid ()) {
    return false;
  }

  const db::Layout &layout = cv->layout ();

  m_filename = fn;
  filename_lbl->setText (tl::to_qstring (fn));
  compression->setCurrentIndex (om_to_index (om));

  dbu_le->setText (tl::to_qstring (tl::to_string (options.dbu ())));

  fmt_cbx->setCurrentIndex (0);
  fmt_cbx_changed (0);

  unsigned int i = 0;
  for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end (); ++fmt) {
    if (fmt->can_write ()) {
      if (fmt->format_name () == options.format ()) {
        fmt_cbx->setCurrentIndex (i);
        fmt_cbx_changed (i);
        break;
      }
      ++i;
    }
  }

  for (std::vector< std::pair<StreamWriterOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {

    const StreamWriterPluginDeclaration *decl = plugin_for_format (page->second);
    if (decl) {

      std::auto_ptr<db::FormatSpecificWriterOptions> specific_options;
      if (options.format () == page->second && options.get_specific_options ()) {
        specific_options.reset (options.get_specific_options ()->clone ());
      } else {
        specific_options.reset (decl->create_specific_options (config_root));
      }

      decl->initialize_options_from_layout_handle (specific_options.get (), *cv.handle ());

      page->first->setup (specific_options.get ());

    }

  }

  bool ret = false;

  if (exec ()) {

    om = index_to_om (compression->currentIndex ());

    int index = fmt_cbx->currentIndex ();
    for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end () && index >= 0; ++fmt) {
      if (fmt->can_write ()) {
        if (index-- == 0) {
          options.set_format (fmt->format_name ());
        }
      }
    }

    double dbu = 0.0;
    tl::from_string (tl::to_string (dbu_le->text ()), dbu);

    double sf = 1.0;
    tl::from_string (tl::to_string (sf_le->text ()), sf);

    options.set_dbu (dbu);
    options.set_scale_factor (sf);

    options.set_dont_write_empty_cells (no_empty_cells_cb->isChecked ());
    options.set_keep_instances (keep_instances_cb->isChecked ());
    options.set_write_context_info (store_context_cb->isChecked ());

    if (no_hidden_cells_cb->isChecked ()) {
      options.clear_cells ();
      for (db::Layout::const_iterator c = layout.begin (); c != layout.end (); ++c) {
        if (! view->is_cell_hidden (c->cell_index (), cv_index)) {
          options.add_this_cell (c->cell_index ());
        }
      }
    } else {
      options.select_all_cells ();
    }

    if (layersel_cbx->currentIndex () == 0 /*all*/) {
      options.select_all_layers ();
    } else if (layersel_cbx->currentIndex () == 1 /*shown layers*/) {
      options.deselect_all_layers ();
      for (LayerPropertiesConstIterator layer = view->begin_layers (); layer != view->end_layers (); ++layer) {
        if (layer->cellview_index () == int (cv_index)) {
          options.add_layer (layer->layer_index ());
        }
      }
    } else if (layersel_cbx->currentIndex () == 2 /*visible layers*/) {
      options.deselect_all_layers ();
      for (LayerPropertiesConstIterator layer = view->begin_layers (); layer != view->end_layers (); ++layer) {
        if (layer->cellview_index () == int (cv_index) && layer->visible (true)) {
          options.add_layer (layer->layer_index ());
        }
      }
    }

    //  identify the plugin in charge of creating the particular options
    const StreamWriterPluginDeclaration *decl = plugin_for_format (options.format ());
    if (decl) {
      for (std::vector< std::pair<StreamWriterOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {
        if (page->second == options.format ()) {

          if (page->first) {

            std::auto_ptr<db::FormatSpecificWriterOptions> specific_options;
            specific_options.reset (decl->create_specific_options (config_root));

            if (specific_options.get ()) {
              page->first->commit (specific_options.get (), tl::OutputStream::output_mode_from_filename (m_filename, index_to_om (compression->currentIndex ())) != tl::OutputStream::OM_Plain);
              options.set_specific_options (specific_options.release ());
            }

          }

          break;

        }
      }
    }

    //  If requested, save the current setup permanently
    if (save_permanently) {
      commit_to_config (options, config_root);
    }

    ret = true;

  }

  return ret;
}

void 
SaveLayoutAsOptionsDialog::fmt_cbx_changed (int index)
{
  if (index >= 0 && index < int (m_tab_positions.size ())) {
    options_stack->setCurrentIndex (m_tab_positions[index]);
  }
}

}

