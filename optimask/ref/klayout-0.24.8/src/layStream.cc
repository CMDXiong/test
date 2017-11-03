
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


#include "layStream.h"
#include "layPlugin.h"
#include "layConfig.h"
#include "dbStream.h"
#include "dbLoadLayoutOptions.h"
#include "dbSaveLayoutOptions.h"
#include "tlAssert.h"
#include "tlString.h"

namespace lay
{

// ------------------------------------------------------------------
//  StreamPluginDeclarationBase implementation

db::StreamFormatDeclaration &
StreamPluginDeclarationBase::stream_fmt () 
{
  if (! mp_stream_fmt) {
    for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end (); ++fmt) {
      if (fmt->format_name () == m_format_name) {
        mp_stream_fmt = const_cast <db::StreamFormatDeclaration *> (&*fmt);
        break;
      }
    }
  }

  tl_assert (mp_stream_fmt);

  return *mp_stream_fmt;
}

void commit_to_config (const db::LoadLayoutOptions &options, lay::PluginRoot *config_root)
{
  //  Store all format specific options to the configuration
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamReaderPluginDeclaration *decl = dynamic_cast <const StreamReaderPluginDeclaration *> (&*cls);
    if (decl) {
      const db::FormatSpecificReaderOptions *specific_options = options.get_specific_options (decl->format_name ());
      if (specific_options) {
        decl->commit_to_config (specific_options, config_root);
      }
    }
  }

  std::vector<unsigned int> layers = options.layer_map ().get_layers ();
  config_root->config_set (cfg_reader_layer_map, options.layer_map ().to_string ());

  //  Establish persistency for the persistent properties that MainWindow is responsible for.
  //  Hint: this will again set the parameters to the same values, but that is not easy to avoid ...
  config_root->config_set (cfg_reader_enable_text_objects, options.enable_text_objects ());
  config_root->config_set (cfg_reader_enable_properties, options.enable_properties ());
  config_root->config_set (cfg_reader_create_other_layers, options.create_other_layers ());

  config_root->config_end ();
}

void commit_to_config (const db::SaveLayoutOptions &options, lay::PluginRoot *config_root)
{
  //  Store all format specific options to the configuration
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamWriterPluginDeclaration *decl = dynamic_cast <const StreamWriterPluginDeclaration *> (&*cls);
    if (decl && decl->format_name () == options.format ()) {
      const db::FormatSpecificWriterOptions *specific_options = options.get_specific_options ();
      if (specific_options) {
        decl->commit_to_config (specific_options, config_root);
      }
      break;
    }
  }

  //  Establish persistency for the persistent properties that MainWindow is responsible for.
  //  TODO: implement, once there are some defined ...

  config_root->config_end ();
}

void 
get_from_config (db::LoadLayoutOptions &options, lay::PluginRoot *config_root)
{
  std::string s;
  config_root->config_get (cfg_reader_layer_map, s);
  options.layer_map () = db::LayerMap::from_string (s);

  bool f;
  f = false;
  config_root->config_get (cfg_reader_enable_text_objects, f);
  options.enable_text_objects (f);

  f = false;
  config_root->config_get (cfg_reader_enable_properties, f);
  options.enable_properties (f);

  f = false;
  config_root->config_get (cfg_reader_create_other_layers, f);
  options.create_other_layers (f);

  //  Store all format specific options to the configuration
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamReaderPluginDeclaration *decl = dynamic_cast <const StreamReaderPluginDeclaration *> (&*cls);
    if (decl) {
      options.set_specific_options (decl->create_specific_options (config_root), decl->format_name ());
    }
  }
}

void 
get_from_config (db::SaveLayoutOptions &options, lay::PluginRoot *config_root)
{
  //  Store all format specific options to the configuration
  for (tl::Registrar<lay::PluginDeclaration>::iterator cls = tl::Registrar<lay::PluginDeclaration>::begin (); cls != tl::Registrar<lay::PluginDeclaration>::end (); ++cls) {
    const StreamWriterPluginDeclaration *decl = dynamic_cast <const StreamWriterPluginDeclaration *> (&*cls);
    if (decl) {
      options.set_specific_options (decl->create_specific_options (config_root));
    }
  }
}

// ------------------------------------------------------------------
//  A "basic" reader plugin to declare the global reader configuration options

const std::string cfg_reader_create_other_layers ("reader-create-other-layers");
const std::string cfg_reader_layer_map ("reader-layer-map");
const std::string cfg_reader_enable_text_objects ("reader-enable-text-objects");
const std::string cfg_reader_enable_properties ("reader-enable-properties");

class BasicStreamReaderPlugin
  : public lay::PluginDeclaration
{
public:
  virtual void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    options.push_back (std::pair<std::string, std::string> (cfg_reader_layer_map, ""));
    options.push_back (std::pair<std::string, std::string> (cfg_reader_create_other_layers, "true"));
    options.push_back (std::pair<std::string, std::string> (cfg_reader_enable_text_objects, "true"));
    options.push_back (std::pair<std::string, std::string> (cfg_reader_enable_properties, "true"));
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> config_decl_reader (new BasicStreamReaderPlugin(), 1100, "BasicStreamReader");


}

