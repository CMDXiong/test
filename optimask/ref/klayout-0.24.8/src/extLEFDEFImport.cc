
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


#include "extLEFImporter.h"
#include "extDEFImporter.h"
#include "extLEFDEFImportDialogs.h"

#include "layExceptions.h"
#include "layPlugin.h"
#include "layMainWindow.h"
#include "layFileDialog.h"
#include "tlLog.h"
#include "tlTimer.h"
#include "tlXMLParser.h"

#include <QtCore/QFileInfo>

namespace ext
{

// -----------------------------------------------------------------------------------------------
//  Plugin declaration

//  TODO: there should be a tl::from_string/tl::to_string overload for db::LayerMap to or from string conversion
struct LayerMapConverter
{
  std::string to_string (const db::LayerMap &lm) const
  {
    return lm.to_string ();
  }

  void from_string (const std::string &s, db::LayerMap &lm) const
  {
    lm = db::LayerMap::from_string (s);
  }
};

//  TODO: there should be a tl::from_string/tl::to_string overload for tl::Variant to or from string conversion
struct VariantConverter
{
  std::string to_string (const tl::Variant &v) const
  {
    return v.to_parsable_string ();
  }

  void from_string (const std::string &s, tl::Variant &v) const
  {
    tl::Extractor ex (s.c_str ());
    ex.read (v);
  }
};

static const std::string cfg_lef_import_spec ("lef-import-spec");
static const std::string cfg_def_import_spec ("def-import-spec");

class LEFDEFImportPluginDeclaration
  : public lay::PluginDeclaration,
    public lay::TechnologyComponentProvider
{
public:
  LEFDEFImportPluginDeclaration () 
  {
    //  .. nothing yet ..
  }
  
  virtual void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    options.push_back (std::pair<std::string, std::string> (cfg_lef_import_spec, ""));
    options.push_back (std::pair<std::string, std::string> (cfg_def_import_spec, ""));
  }

  virtual lay::ConfigPage *config_page (QWidget *parent, std::string &title) const
  {
    // .. nothing yet ..
    return 0;
  }

  virtual void get_menu_entries (std::vector<lay::MenuEntry> &menu_entries) const
  {
    lay::PluginDeclaration::get_menu_entries (menu_entries);
    menu_entries.push_back (lay::MenuEntry ("ext::import_lef", "import_lef:edit", "file_menu.import_menu.end", tl::to_string (QObject::tr ("LEF"))));
    menu_entries.push_back (lay::MenuEntry ("ext::import_def", "import_def:edit", "file_menu.import_menu.end", tl::to_string (QObject::tr ("DEF/LEF"))));
  }

  virtual bool configure (const std::string &name, const std::string &value)
  {
    if (name == cfg_lef_import_spec) {
      m_lef_spec = value;
      return true;
    } else if (name == cfg_def_import_spec) {
      m_def_spec = value;
      return true;
    } else {
      return false;
    }
  }

  virtual void config_finalize ()
  {
    // .. nothing yet ..
  }

  virtual bool menu_activated (const std::string &symbol) const
  {
    if (symbol == "ext::import_lef" || symbol == "ext::import_def") {

      bool import_lef = (symbol == "ext::import_lef");

      LEFDEFImportData data;
      try {
        if (import_lef) {
          data.from_string (m_lef_spec);
        } else {
          data.from_string (m_def_spec);
        }
      } catch (...) {
        data = LEFDEFImportData ();
      }

      LEFDEFImportOptionsDialog import_dialog (lay::MainWindow::instance (), import_lef);
      if (import_dialog.exec (data)) {

        //  clear selection 
        lay::MainWindow::instance ()->cancel ();

        //  store configuration
        lay::PluginRoot *config_root = lay::MainWindow::instance ();
        if (import_lef) {
          config_root->config_set (cfg_lef_import_spec, data.to_string ());
        } else {
          config_root->config_set (cfg_def_import_spec, data.to_string ());
        }
        config_root->config_end ();

        std::auto_ptr<db::Layout> layout (new db::Layout ());

        tl::InputStream stream (data.file);

        std::string tech_name = lay::MainWindow::instance ()->initial_technology ();
        if (! lay::Technologies::instance ()->has_technology (tech_name)) {
          tech_name.clear (); // use default technology
        }
        const lay::Technology *tech = lay::Technologies::instance ()->technology_by_name (tech_name);
        const ext::LEFDEFTechnologyComponent *tech_comp = 0;
        if (tech) {
          tech_comp = dynamic_cast<const ext::LEFDEFTechnologyComponent *>(tech->component_by_name (lef_def_technology_component_name));
          tl_assert (tech_comp != 0);
        }

        ext::LEFDEFLayerDelegate layers (tech_comp);
        layers.prepare (*layout);
        layout->dbu (tech_comp->dbu ());

        if (import_lef) {

          tl::SelfTimer timer (tl::verbosity () >= 11, tl::to_string (QObject::tr ("Reading LEF file")));

          ext::LEFImporter importer;

          if (tech_comp) {
            for (std::vector<std::string>::const_iterator l = tech_comp->begin_lef_files (); l != tech_comp->end_lef_files (); ++l) {
              tl::InputStream lef_stream (*l);
              tl::log << tl::to_string (QObject::tr ("Reading")) << " " << *l;
              importer.read (lef_stream, *layout, layers, *l);
            }
          }

          tl::log << tl::to_string (QObject::tr ("Reading")) << " " << data.file;
          importer.read (stream, *layout, layers, data.file);

        } else {

          tl::SelfTimer timer (tl::verbosity () >= 11, tl::to_string (QObject::tr ("Reading DEF file")));

          DEFImporter importer;

          QFileInfo def_fi (tl::to_qstring (data.file));

          std::vector<std::string> lef_files;
          if (tech_comp) {
            lef_files.insert (lef_files.end (), tech_comp->begin_lef_files (), tech_comp->end_lef_files ());
          } 
          lef_files.insert (lef_files.end (), data.lef_files.begin (), data.lef_files.end ());

          for (std::vector<std::string>::const_iterator l = lef_files.begin (); l != lef_files.end (); ++l) {

            QFileInfo fi (tl::to_qstring (*l));
            if (fi.isAbsolute ()) {
              tl::InputStream lef_stream (*l);
              tl::log << tl::to_string (QObject::tr ("Reading")) << " " << *l;
              importer.read_lef (lef_stream, *layout, layers, *l);
            } else {
              std::string ex_l = tl::to_string (def_fi.absoluteDir ().absoluteFilePath (tl::to_qstring (*l)));
              tl::InputStream lef_stream (ex_l);
              tl::log << tl::to_string (QObject::tr ("Reading")) << " " << *l;
              importer.read_lef (lef_stream, *layout, layers, *l);
            }

          }

          tl::log << tl::to_string (QObject::tr ("Reading")) << " " << data.file;

          importer.read (stream, *layout, layers, data.file);

        }

        layers.finish (*layout);

        lay::LayoutView *view = lay::MainWindow::instance ()->current_view ();
        if (! view || data.mode == 1) {
          view = lay::MainWindow::instance ()->view (lay::MainWindow::instance ()->create_view ());
        }

        lay::LayoutHandle *handle = new lay::LayoutHandle (layout.release (), std::string ());
        handle->rename (tl::to_string (QFileInfo (tl::to_qstring (data.file)).fileName ()));
        handle->set_tech_name (tech_name);
        view->add_layout (handle, data.mode == 2);

      }

      return true;

    } else {
      return false;
    }
  }

  virtual const lay::TechnologyComponentProvider *technology_component_provider () const 
  {
    return this;
  }

  virtual lay::TechnologyComponent *create_component () const
  {
    return new LEFDEFTechnologyComponent ();
  }

  virtual tl::XMLElementBase *xml_element () const 
  {
    return new lay::TechnologyComponentXMLElement<LEFDEFTechnologyComponent> (lef_def_technology_component_name, 
      tl::make_member (&LEFDEFTechnologyComponent::read_all_layers, &LEFDEFTechnologyComponent::set_read_all_layers, "read-all-layers") + 
      tl::make_member (&LEFDEFTechnologyComponent::layer_map, &LEFDEFTechnologyComponent::set_layer_map, "layer-map", LayerMapConverter ()) +
      tl::make_member (&LEFDEFTechnologyComponent::dbu, &LEFDEFTechnologyComponent::set_dbu, "dbu") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_net_names, &LEFDEFTechnologyComponent::set_produce_net_names, "produce-net-names") +
      tl::make_member (&LEFDEFTechnologyComponent::net_property_name, &LEFDEFTechnologyComponent::set_net_property_name, "net-property-name", VariantConverter ()) +
      tl::make_member (&LEFDEFTechnologyComponent::produce_cell_outlines, &LEFDEFTechnologyComponent::set_produce_cell_outlines, "produce-cell-outlines") +
      tl::make_member (&LEFDEFTechnologyComponent::cell_outline_layer, &LEFDEFTechnologyComponent::set_cell_outline_layer, "cell-outline-layer") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_placement_blockages, &LEFDEFTechnologyComponent::set_produce_placement_blockages, "produce-placement-blockages") +
      tl::make_member (&LEFDEFTechnologyComponent::placement_blockage_layer, &LEFDEFTechnologyComponent::set_placement_blockage_layer, "placement-blockage-layer") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_via_geometry, &LEFDEFTechnologyComponent::set_produce_via_geometry, "produce-via-geometry") +
      tl::make_member (&LEFDEFTechnologyComponent::via_geometry_suffix, &LEFDEFTechnologyComponent::set_via_geometry_suffix, "via-geometry-suffix") +
      tl::make_member (&LEFDEFTechnologyComponent::via_geometry_datatype, &LEFDEFTechnologyComponent::set_via_geometry_datatype, "via-geometry-datatype") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_pins, &LEFDEFTechnologyComponent::set_produce_pins, "produce-pins") +
      tl::make_member (&LEFDEFTechnologyComponent::pins_suffix, &LEFDEFTechnologyComponent::set_pins_suffix, "pins-suffix") +
      tl::make_member (&LEFDEFTechnologyComponent::pins_datatype, &LEFDEFTechnologyComponent::set_pins_datatype, "pins-datatype") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_obstructions, &LEFDEFTechnologyComponent::set_produce_obstructions, "produce-obstructions") +
      tl::make_member (&LEFDEFTechnologyComponent::obstructions_suffix, &LEFDEFTechnologyComponent::set_obstructions_suffix, "obstructions-suffix") +
      tl::make_member (&LEFDEFTechnologyComponent::obstructions_datatype, &LEFDEFTechnologyComponent::set_obstructions_datatype, "obstructions-datatype") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_blockages, &LEFDEFTechnologyComponent::set_produce_blockages, "produce-blockages") +
      tl::make_member (&LEFDEFTechnologyComponent::blockages_suffix, &LEFDEFTechnologyComponent::set_blockages_suffix, "blockages-suffix") +
      tl::make_member (&LEFDEFTechnologyComponent::blockages_datatype, &LEFDEFTechnologyComponent::set_blockages_datatype, "blockages-datatype") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_labels, &LEFDEFTechnologyComponent::set_produce_labels, "produce-labels") +
      tl::make_member (&LEFDEFTechnologyComponent::labels_suffix, &LEFDEFTechnologyComponent::set_labels_suffix, "labels-suffix") +
      tl::make_member (&LEFDEFTechnologyComponent::labels_datatype, &LEFDEFTechnologyComponent::set_labels_datatype, "labels-datatype") +
      tl::make_member (&LEFDEFTechnologyComponent::produce_routing, &LEFDEFTechnologyComponent::set_produce_routing, "produce-routing") +
      tl::make_member (&LEFDEFTechnologyComponent::routing_suffix, &LEFDEFTechnologyComponent::set_routing_suffix, "routing-suffix") +
      tl::make_member (&LEFDEFTechnologyComponent::routing_datatype, &LEFDEFTechnologyComponent::set_routing_datatype, "routing-datatype") +
      tl::make_member (&LEFDEFTechnologyComponent::begin_lef_files, &LEFDEFTechnologyComponent::end_lef_files, &LEFDEFTechnologyComponent::push_lef_file, "lef-files") 
    );
  }

private:
  std::string m_lef_spec;
  std::string m_def_spec;
};

static tl::RegisteredClass<lay::PluginDeclaration> config_decl (new ext::LEFDEFImportPluginDeclaration (), 1400, "ext::LEFDEFImportPlugin");

}

