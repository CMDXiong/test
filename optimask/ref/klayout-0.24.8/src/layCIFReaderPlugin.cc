
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



#include "dbCIF.h"
#include "dbCIFReader.h"
#include "dbLoadLayoutOptions.h"
#include "layMainWindow.h"
#include "layCIFReaderPlugin.h"
#include "CIFReaderOptionPage.h"
#include "gsiDecl.h"

#include <QtGui/QFrame>

namespace lay
{

const std::string cfg_cif_wire_mode ("cif-wire-mode");
const std::string cfg_cif_dbu ("cif-dbu");

// ---------------------------------------------------------------
//  CIFReaderOptionPage definition and implementation

CIFReaderOptionPage::CIFReaderOptionPage (QWidget *parent)
  : StreamReaderOptionsPage (parent)
{
  mp_ui = new Ui::CIFReaderOptionPage ();
  mp_ui->setupUi (this);
}

CIFReaderOptionPage::~CIFReaderOptionPage ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
CIFReaderOptionPage::setup (const db::FormatSpecificReaderOptions *o)
{
  const db::CIFReaderOptions *options = dynamic_cast<const db::CIFReaderOptions *> (o);
  if (options) {
    mp_ui->dbu_le->setText (tl::to_qstring (tl::to_string (options->dbu)));
    mp_ui->wire_mode_cb->setCurrentIndex (options->wire_mode);
  }
}

void 
CIFReaderOptionPage::commit (db::FormatSpecificReaderOptions *o)
{
  db::CIFReaderOptions *options = dynamic_cast<db::CIFReaderOptions *> (o);
  if (options) {
    tl::from_string (tl::to_string (mp_ui->dbu_le->text ()), options->dbu);
    if (options->dbu > 1000.0 || options->dbu < 1e-9) {
      throw tl::Exception (tl::to_string (QObject::tr ("Invalid value for database unit")));
    }
    options->wire_mode = mp_ui->wire_mode_cb->currentIndex ();
  }
}

// ---------------------------------------------------------------
//  CIFReaderPluginDeclaration definition and implementation

class CIFReaderPluginDeclaration
  : public StreamReaderPluginDeclaration
{
public:
  CIFReaderPluginDeclaration () 
    : StreamReaderPluginDeclaration ("CIF" /*same than delivered by the stream format decl*/)
  {
    // .. nothing yet ..
  }

  StreamReaderOptionsPage *format_specific_options_page (QWidget *parent) const
  {
    return new CIFReaderOptionPage (parent);
  }

  void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    db::CIFReaderOptions m_options;
    options.push_back (std::make_pair (cfg_cif_wire_mode, tl::to_string (m_options.wire_mode)));
    options.push_back (std::make_pair (cfg_cif_dbu, tl::to_string (m_options.dbu)));
  }

  db::FormatSpecificReaderOptions *create_specific_options (lay::PluginRoot *config_root) const
  {
    db::CIFReaderOptions *options = new db::CIFReaderOptions ();

    //  no initialization if config_root is not set.
    if (config_root) {
      try {
        config_root->config_get (cfg_cif_wire_mode, options->wire_mode);
        config_root->config_get (cfg_cif_dbu, options->dbu);
      } catch (...) {
        delete options;
        throw;
      }
    }

    return options;
  }

  void commit_to_config (const db::FormatSpecificReaderOptions *o, lay::PluginRoot *config_root) const 
  {
    const db::CIFReaderOptions *options = dynamic_cast<const db::CIFReaderOptions *> (o);
    if (options) {
      config_root->config_set (cfg_cif_wire_mode, options->wire_mode);
      config_root->config_set (cfg_cif_dbu, options->dbu);
    }
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> plugin_decl (new lay::CIFReaderPluginDeclaration (), 10000, "CIFReader");

// ---------------------------------------------------------------
//  gsi Implementation of specific methods

static void get_cif_options (const db::LoadLayoutOptions *options, db::CIFReaderOptions &cif_options)
{
  const db::CIFReaderOptions *current_cif_options = dynamic_cast <const db::CIFReaderOptions *> (options->get_specific_options ("CIF"));
  if (current_cif_options) {
    cif_options = *current_cif_options;
  }
}

static void set_cif_wire_mode (db::LoadLayoutOptions *options, unsigned int n)
{
  db::CIFReaderOptions cif_options;
  get_cif_options (options, cif_options);
  cif_options.wire_mode = n;
  options->set_specific_options (cif_options, "CIF");
}

static unsigned int get_cif_wire_mode (const db::LoadLayoutOptions *options)
{
  db::CIFReaderOptions cif_options;
  get_cif_options (options, cif_options);
  return cif_options.wire_mode;
}

static void set_cif_dbu (db::LoadLayoutOptions *options, double dbu)
{
  db::CIFReaderOptions cif_options;
  get_cif_options (options, cif_options);
  cif_options.dbu = dbu;
  options->set_specific_options (cif_options, "CIF");
}

static double get_cif_dbu (const db::LoadLayoutOptions *options)
{
  db::CIFReaderOptions cif_options;
  get_cif_options (options, cif_options);
  return cif_options.dbu;
}

//  extend lay::LoadLayoutOptions with the CIF options 
static
gsi::ClassExt<db::LoadLayoutOptions> cif_reader_options (
  gsi::method_ext ("cif_wire_mode=", &set_cif_wire_mode,
    "@brief How to read 'W' objects\n"
    "\n"
    "This property specifies how to read 'W' (wire) objects.\n"
    "Allowed values are 0 (as square ended paths), 1 (as flush ended paths), 2 (as round paths)\n"
    "\nThis property has been added in version 0.21.\n"
  ) +
  gsi::method_ext ("wire_mode", &get_cif_wire_mode,
    "@brief Specifies how to read 'W' objects\n"
    "See \\cif_wire_mode= method for a description of this mode."
    "\nThis property has been added in version 0.21.\n"
  ) +
  gsi::method_ext ("cif_dbu=", &set_cif_dbu,
    "@brief Specifies the database unit which the reader uses and produces\n"
    "\nThis property has been added in version 0.21.\n"
  ) +
  gsi::method_ext ("cif_dbu", &get_cif_dbu,
    "@brief Specifies the database unit which the reader uses and produces\n"
    "See \\cif_dbu= method for a description of this property."
    "\nThis property has been added in version 0.21.\n"
  ),
  ""
);

}





