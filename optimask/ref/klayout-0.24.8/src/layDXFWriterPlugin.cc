
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


#include "dbDXF.h"
#include "dbDXFWriter.h"
#include "dbSaveLayoutOptions.h"
#include "layMainWindow.h"
#include "layDXFWriterPlugin.h"
#include "DXFWriterOptionPage.h"
#include "gsiDecl.h"

#include <QtGui/QFrame>

namespace lay
{

const std::string cfg_dxf_polygon_mode ("dxf-polygon-mode");

// ---------------------------------------------------------------
//  DXFWriterOptionPage definition and implementation

DXFWriterOptionPage::DXFWriterOptionPage (QWidget *parent)
  : StreamWriterOptionsPage (parent)
{
  mp_ui = new Ui::DXFWriterOptionPage ();
  mp_ui->setupUi (this);
}

DXFWriterOptionPage::~DXFWriterOptionPage ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
DXFWriterOptionPage::setup (const db::FormatSpecificWriterOptions *o)
{
  const db::DXFWriterOptions *options = dynamic_cast<const db::DXFWriterOptions *> (o);
  if (options) {
    mp_ui->polygon_mode_cbx->setCurrentIndex (options->polygon_mode);
  }
}

void 
DXFWriterOptionPage::commit (db::FormatSpecificWriterOptions *o, bool gzip)
{
  db::DXFWriterOptions *options = dynamic_cast<db::DXFWriterOptions *> (o);
  if (options) {
    options->polygon_mode = mp_ui->polygon_mode_cbx->currentIndex ();
  }
}

// ---------------------------------------------------------------
//  DXFWriterPluginDeclaration definition and implementation

class DXFWriterPluginDeclaration
  : public StreamWriterPluginDeclaration
{
public:
  DXFWriterPluginDeclaration () 
    : StreamWriterPluginDeclaration ("DXF" /*same than delivered by the stream format decl*/)
  {
    // .. nothing yet ..
  }

  StreamWriterOptionsPage *format_specific_options_page (QWidget *parent) const
  {
    return new DXFWriterOptionPage (parent);
  }

  void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    db::DXFWriterOptions m_options;
    options.push_back (std::make_pair (cfg_dxf_polygon_mode, tl::to_string (m_options.polygon_mode)));
  }

  db::FormatSpecificWriterOptions *create_specific_options (lay::PluginRoot *config_root) const
  {
    db::DXFWriterOptions *options = new db::DXFWriterOptions ();

    //  no initialization if config_root is not set.
    if (config_root) {
      try {
        config_root->config_get (cfg_dxf_polygon_mode, options->polygon_mode);
      } catch (...) {
        delete options;
        throw;
      }
    }

    return options;
  }

  void commit_to_config (const db::FormatSpecificWriterOptions *o, lay::PluginRoot *config_root) const 
  {
    const db::DXFWriterOptions *options = dynamic_cast<const db::DXFWriterOptions *> (o);
    if (options) {
      config_root->config_set (cfg_dxf_polygon_mode, options->polygon_mode);
    }
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> plugin_decl (new lay::DXFWriterPluginDeclaration (), 10000, "DXFWriter");

// ---------------------------------------------------------------
//  gsi Implementation of specific methods

static void get_dxf_options (const db::SaveLayoutOptions *options, db::DXFWriterOptions &dxf_options)
{
  const db::DXFWriterOptions *current_dxf_options = dynamic_cast <const db::DXFWriterOptions *> (options->get_specific_options ());
  if (current_dxf_options) {
    dxf_options = *current_dxf_options;
  }
}

static void set_dxf_polygon_mode (db::SaveLayoutOptions *options, int mode)
{
  if (mode < 0 || mode > 3) {
    throw tl::Exception (tl::to_string (QObject::tr ("Invalid polygon mode")));
  }

  db::DXFWriterOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.polygon_mode = mode;
  options->set_specific_options (dxf_options);
}

static int get_dxf_polygon_mode (const db::SaveLayoutOptions *options)
{
  db::DXFWriterOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.polygon_mode;
}

//  extend lay::SaveLayoutOptions with the DXF options 
static
gsi::ClassExt<db::SaveLayoutOptions> dxf_writer_options (
  gsi::method_ext ("dxf_polygon_mode=", &set_dxf_polygon_mode,
    "@brief Specifies how to write polygons.\n"
    "@args mode\n"
    "The mode is 0 (write POLYLINE entities), 1 (write LWPOLYLINE entities), 2 (decompose into SOLID entities) or "
    "or 3 (write HATCH entities).\n"
    "\nThis property has been added in version 0.21.3.\n"
  ) +
  gsi::method_ext ("dxf_polygon_mode", &get_dxf_polygon_mode,
    "@brief Specifies how to write polygons.\n"
    "See \\dxf_polygon_mode= for a description of this property.\n"
    "\nThis property has been added in version 0.21.3.\n"
  ),
  ""
);

}





