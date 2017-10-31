
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
#include "dbCIFWriter.h"
#include "dbSaveLayoutOptions.h"
#include "layMainWindow.h"
#include "layCIFWriterPlugin.h"
#include "CIFWriterOptionPage.h"
#include "gsiDecl.h"

#include <QtGui/QFrame>

namespace lay
{

const std::string cfg_cif_write_dummy_calls ("cif-write-dummy-calls");
const std::string cfg_cif_write_blank_separator ("cif-write-blank-separator");

// ---------------------------------------------------------------
//  CIFWriterOptionPage definition and implementation

CIFWriterOptionPage::CIFWriterOptionPage (QWidget *parent)
  : StreamWriterOptionsPage (parent)
{
  mp_ui = new Ui::CIFWriterOptionPage ();
  mp_ui->setupUi (this);

  // .. nothing yet ..
}

CIFWriterOptionPage::~CIFWriterOptionPage ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
CIFWriterOptionPage::setup (const db::FormatSpecificWriterOptions *o)
{
  const db::CIFWriterOptions *options = dynamic_cast<const db::CIFWriterOptions *> (o);
  if (options) {
    mp_ui->dummy_calls_cbx->setChecked (options->dummy_calls);
    mp_ui->blank_separator_cbx->setChecked (options->blank_separator);
  }
}

void 
CIFWriterOptionPage::commit (db::FormatSpecificWriterOptions *o, bool gzip)
{
  db::CIFWriterOptions *options = dynamic_cast<db::CIFWriterOptions *> (o);
  if (options) {
    options->dummy_calls = mp_ui->dummy_calls_cbx->isChecked ();
    options->blank_separator = mp_ui->blank_separator_cbx->isChecked ();
  }
}

// ---------------------------------------------------------------
//  CIFWriterPluginDeclaration definition and implementation

class CIFWriterPluginDeclaration
  : public StreamWriterPluginDeclaration
{
public:
  CIFWriterPluginDeclaration () 
    : StreamWriterPluginDeclaration ("CIF" /*same than delivered by the stream format decl*/)
  {
    // .. nothing yet ..
  }

  StreamWriterOptionsPage *format_specific_options_page (QWidget *parent) const
  {
    return new CIFWriterOptionPage (parent);
  }

  void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    db::CIFWriterOptions m_options;
    options.push_back (std::make_pair (cfg_cif_write_dummy_calls, tl::to_string (m_options.dummy_calls)));
    options.push_back (std::make_pair (cfg_cif_write_blank_separator, tl::to_string (m_options.blank_separator)));
  }

  db::FormatSpecificWriterOptions *create_specific_options (lay::PluginRoot *config_root) const
  {
    db::CIFWriterOptions *options = new db::CIFWriterOptions ();

    //  no initialization if config_root is not set.
    if (config_root) {
      try {
        config_root->config_get (cfg_cif_write_dummy_calls, options->dummy_calls);
        config_root->config_get (cfg_cif_write_blank_separator, options->blank_separator);
      } catch (...) {
        delete options;
        throw;
      }
    }

    return options;
  }

  void commit_to_config (const db::FormatSpecificWriterOptions *o, lay::PluginRoot *config_root) const 
  {
    const db::CIFWriterOptions *options = dynamic_cast<const db::CIFWriterOptions *> (o);
    if (options) {
      config_root->config_set (cfg_cif_write_dummy_calls, options->dummy_calls);
      config_root->config_set (cfg_cif_write_blank_separator, options->blank_separator);
    }
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> plugin_decl (new lay::CIFWriterPluginDeclaration (), 10000, "CIFWriter");

// ---------------------------------------------------------------
//  gsi Implementation of specific methods

static void get_cif_options (const db::SaveLayoutOptions *options, db::CIFWriterOptions &cif_options)
{
  const db::CIFWriterOptions *current_cif_options = dynamic_cast <const db::CIFWriterOptions *> (options->get_specific_options ());
  if (current_cif_options) {
    cif_options = *current_cif_options;
  }
}

static void set_cif_dummy_calls (db::SaveLayoutOptions *options, bool f)
{
  db::CIFWriterOptions cif_options;
  get_cif_options (options, cif_options);
  cif_options.dummy_calls = f;
  options->set_specific_options (cif_options);
}

static bool get_cif_dummy_calls (const db::SaveLayoutOptions *options)
{
  db::CIFWriterOptions cif_options;
  get_cif_options (options, cif_options);
  return cif_options.dummy_calls;
}

static void set_cif_blank_separator (db::SaveLayoutOptions *options, bool f)
{
  db::CIFWriterOptions cif_options;
  get_cif_options (options, cif_options);
  cif_options.blank_separator = f;
  options->set_specific_options (cif_options);
}

static bool get_cif_blank_separator (const db::SaveLayoutOptions *options)
{
  db::CIFWriterOptions cif_options;
  get_cif_options (options, cif_options);
  return cif_options.blank_separator;
}

//  extend lay::SaveLayoutOptions with the GDS2 options 
static
gsi::ClassExt<db::SaveLayoutOptions> cif_writer_options (
  gsi::method_ext ("cif_dummy_calls=", &set_cif_dummy_calls,
    "@brief Sets a flag indicating whether dummy calls shall be written\n"
    "If this property is set to true, dummy calls will be written in the top level entity "
    "of the CIF file calling every top cell.\n"
    "This option is useful for enhanced compatibility with other tools.\n"
    "\nThis property has been added in version 0.23.10.\n"
  ) +
  gsi::method_ext ("cif_dummy_calls", &get_cif_dummy_calls,
    "@brief Gets a flag indicating whether dummy calls shall be written\n"
    "See \\cif_dummy_calls= method for a description of that property."
    "\nThis property has been added in version 0.23.10.\n"
  ) +
  gsi::method_ext ("cif_blank_separator=", &set_cif_blank_separator,
    "@brief Sets a flag indicating whether blanks shall be used as x/y separator characters\n"
    "If this property is set to true, the x and y coordinates are separated with blank characters "
    "rather than comma characters."
    "\nThis property has been added in version 0.23.10.\n"
  ) +
  gsi::method_ext ("cif_blank_separator", &get_cif_blank_separator,
    "@brief Gets a flag indicating whether blanks shall be used as x/y separator characters\n"
    "See \\cif_blank_separator= method for a description of that property."
    "\nThis property has been added in version 0.23.10.\n"
  ),
  ""
);

}

