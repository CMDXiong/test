
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
#include "layLoadLayoutOptionsDialog.h"
#include "layStream.h"
#include "layExceptions.h"
#include "layMainWindow.h"
#include "layFileDialog.h"
#include "layConfig.h"
#include "dbLoadLayoutOptions.h"
#include "dbStream.h"
#include "tlClassRegistry.h"

#include <QtGui/QScrollArea>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include <memory>

namespace lay
{

static const StreamReaderPluginDeclaration *plugin_for_format (const std::string &format_name)
{
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamReaderPluginDeclaration *decl = dynamic_cast <const StreamReaderPluginDeclaration *> (&*cls);
    if (decl && decl->format_name () == format_name) {
      return decl;
    }
  }
  return 0;
}

LoadLayoutOptionsDialog::LoadLayoutOptionsDialog (QWidget *parent, const std::string &title)
  : QDialog (parent), Ui::LoadLayoutOptionsDialog (),
    m_show_always (false)
{
  setObjectName (QString::fromAscii ("load_layout_options_dialog"));

  Ui::LoadLayoutOptionsDialog::setupUi (this);

  setWindowTitle (tl::to_qstring (title));

  while (options_tab->count () > 0) {
    options_tab->removeTab (0);
  }

  bool any_option = false;

  for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end (); ++fmt) {

    StreamReaderOptionsPage *page = 0;

    //  obtain the config page from the plugin which we identify by format name
    const StreamReaderPluginDeclaration *decl = plugin_for_format (fmt->format_name ());
    if (decl) {
      page = decl->format_specific_options_page (options_tab);
      if (page) {
        options_tab->addTab (page, tl::to_qstring (fmt->format_name ()));
        any_option = true;
      }
    } else {
      QLabel *empty = new QLabel (options_tab);
      empty->setAlignment (Qt::AlignCenter);
      empty->setText (QObject::tr ("No specific options available for this format"));
      options_tab->addTab (empty, tl::to_qstring (fmt->format_name ()));
    }

    m_pages.push_back (std::make_pair (page, fmt->format_name ()));

  }

  if (! any_option) {
    options_tab->hide ();
  }

  connect (buttonBox, SIGNAL (accepted ()), this, SLOT (ok_button_pressed ()));
  connect (buttonBox, SIGNAL (clicked (QAbstractButton *)), this, SLOT (button_pressed (QAbstractButton *)));
  connect (layer_map, SIGNAL (layerListChanged ()), this, SLOT (layer_list_changed ()));
  connect (layer_map, SIGNAL (layerItemAdded ()), this, SLOT (layer_item_added ()));
  connect (layer_map, SIGNAL (layerItemDeleted ()), this, SLOT (layer_item_deleted ()));
  connect (tech_cbx, SIGNAL (currentIndexChanged (int)), this, SLOT (current_tech_changed (int)));
}

LoadLayoutOptionsDialog::~LoadLayoutOptionsDialog ()
{
  // .. nothing yet ..
}

void 
LoadLayoutOptionsDialog::button_pressed (QAbstractButton *button)
{
  if (button == buttonBox->button (QDialogButtonBox::Reset)) {
    reset_button_pressed ();
  }
}

void
LoadLayoutOptionsDialog::layer_list_changed ()
{
  read_all_cbx->setChecked (layer_map->is_empty ());
}

void
LoadLayoutOptionsDialog::layer_item_added ()
{
  read_all_cbx->setChecked (false);
}

void
LoadLayoutOptionsDialog::layer_item_deleted ()
{
  if (layer_map->is_empty ()) {
    read_all_cbx->setChecked (true);
  }
}

void
LoadLayoutOptionsDialog::current_tech_changed (int index)
{
  bool tech_has_layer_mapping = false;
  if (index >= 0 && index < int (lay::Technologies::instance ()->technologies ())) {
    tech_has_layer_mapping = ! lay::Technologies::instance ()->begin () [index].layer_map ().is_empty ();
  }
  mapping_lbl->setVisible (tech_has_layer_mapping);
}

void
LoadLayoutOptionsDialog::reset_button_pressed ()
{
  BEGIN_PROTECTED

  db::LoadLayoutOptions options;
  update (options, std::string (), 0);  // reset to defaults

  END_PROTECTED
}

void
LoadLayoutOptionsDialog::ok_button_pressed ()
{
  BEGIN_PROTECTED

  db::LoadLayoutOptions options;
  commit_global_options (options); // for testing

  for (std::vector< std::pair<StreamReaderOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {
    if (page->first) {
      const StreamReaderPluginDeclaration *decl = plugin_for_format (page->second);
      if (decl) {
        std::auto_ptr<db::FormatSpecificReaderOptions> options (decl->create_specific_options (0));
        if (options.get ()) {
          page->first->commit (options.get ()); // for testing
        }
      }
    }
  }

  accept ();

  END_PROTECTED
}

void
LoadLayoutOptionsDialog::commit_global_options (db::LoadLayoutOptions &options)
{
  //  Hint: persistent properties are configured through config_set calls in the 
  //  main window which will set the corresponding reader_options fields

  options.enable_text_objects (enable_text_cbx->isChecked ());
  options.enable_properties (enable_properties_cbx->isChecked ());
  options.create_other_layers (read_all_cbx->isChecked ());

  options.layer_map () = layer_map->get_layer_map ();
}

void 
LoadLayoutOptionsDialog::update (const db::LoadLayoutOptions &options, const std::string &technology, lay::PluginRoot *config_root)
{
  tech_cbx->clear ();
  unsigned int technology_index = 0;
  bool tech_has_layer_mapping = false;

  for (lay::Technologies::const_iterator t = lay::Technologies::instance ()->begin (); t != lay::Technologies::instance ()->end (); ++t, ++technology_index) {

    std::string d = t->name ();
    if (! d.empty () && ! t->description ().empty ()) {
      d += " - ";
    }
    d += t->description ();

    tech_cbx->addItem (tl::to_qstring (d));
    if (t->name () == technology) {
      tech_has_layer_mapping = ! t->layer_map ().is_empty ();
      tech_cbx->setCurrentIndex (technology_index);
    }

  }

  mapping_lbl->setVisible (tech_has_layer_mapping);

  enable_text_cbx->setChecked (options.enable_text_objects ());
  enable_properties_cbx->setChecked (options.enable_properties ());
  read_all_cbx->setChecked (options.create_other_layers ());

  layer_map->set_layer_map (options.layer_map ());

  for (std::vector< std::pair<StreamReaderOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {
    if (page->first) {
      const StreamReaderPluginDeclaration *decl = plugin_for_format (page->second);
      if (decl) {

        std::auto_ptr<db::FormatSpecificReaderOptions> specific_options;
        if (options.get_specific_options (page->second)) {
          specific_options.reset (options.get_specific_options (page->second)->clone ());
        } else {
          specific_options.reset (decl->create_specific_options (config_root));
        }

        page->first->setup (specific_options.get ());

      }
    }
  }
}

bool 
LoadLayoutOptionsDialog::get_global_options (db::LoadLayoutOptions &options, std::string &technology)
{
  return get_options_internal (options, technology, lay::MainWindow::instance (), true);
}

bool 
LoadLayoutOptionsDialog::get_options (db::LoadLayoutOptions &options)
{
  std::string technology; // dummy
  return get_options_internal (options, technology, lay::MainWindow::instance (), false);
}

bool 
LoadLayoutOptionsDialog::get_options_internal (db::LoadLayoutOptions &options, std::string &technology, lay::PluginRoot *config_root, bool global)
{
  update (options, technology, config_root);

  always_cbx->setChecked (m_show_always);

  always_cbx->setVisible (global);
  tech_cbx->setVisible (global);

  bool ret = false;

  if (exec ()) {

    //  read global options
    commit_global_options (options);

    //  get the selected technology name
    int technology_index = tech_cbx->currentIndex ();
    if (technology_index >= 0 && technology_index < (int) lay::Technologies::instance ()->technologies ()) {
      technology = lay::Technologies::instance ()->begin () [technology_index].name ();
    } else {
      technology = std::string ();
    }

    //  create the particular options for all formats
    for (std::vector< std::pair<StreamReaderOptionsPage *, std::string> >::iterator page = m_pages.begin (); page != m_pages.end (); ++page) {
      if (page->first) {
        const StreamReaderPluginDeclaration *decl = plugin_for_format (page->second);
        if (decl) {

          std::auto_ptr<db::FormatSpecificReaderOptions> specific_options;
          if (options.get_specific_options (page->second)) {
            specific_options.reset (options.get_specific_options (page->second)->clone ());
          } else {
            specific_options.reset (decl->create_specific_options (config_root));
          }

          if (specific_options.get ()) {
            page->first->commit (specific_options.get ()); 
            options.set_specific_options (specific_options.release (), page->second);
          }

        }
      }
    }

    m_show_always = always_cbx->isChecked ();

    //  If requested, save the current setup permanently
    if (global) {
      config_root->config_set (cfg_reader_options_show_always, tl::to_string (m_show_always));
      config_root->config_set (cfg_initial_technology, technology);
      commit_to_config (options, config_root);
    }

    ret = true;

  }

  return ret;
}

}


