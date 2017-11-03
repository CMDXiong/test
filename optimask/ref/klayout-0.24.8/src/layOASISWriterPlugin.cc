
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


#include "dbOASIS.h"
#include "dbOASISWriter.h"
#include "dbSaveLayoutOptions.h"
#include "layMainWindow.h"
#include "layStream.h"
#include "gsiDecl.h"

#include "OASISWriterOptionPage.h"

#include <QtGui/QFrame>

namespace lay
{

const std::string cfg_oasis_compression ("oasis-compression");
const std::string cfg_oasis_write_cblocks ("oasis-write-cblocks");
const std::string cfg_oasis_strict_mode ("oasis-strict-mode");
const std::string cfg_oasis_write_std_properties ("oasis-write-std-properties");
const std::string cfg_oasis_substitution_char ("oasis-subst-char");

// ---------------------------------------------------------------
//  OASISWriterOptionPage definition and implementation

class OASISWriterOptionPage 
  : public StreamWriterOptionsPage
{
public:
  OASISWriterOptionPage (QWidget *parent);
  ~OASISWriterOptionPage ();

  void setup (const db::FormatSpecificWriterOptions *options);
  void commit (db::FormatSpecificWriterOptions *options, bool gzip);

private:
  Ui::OASISWriterOptionPage *mp_ui;
};

OASISWriterOptionPage::OASISWriterOptionPage (QWidget *parent)
  : StreamWriterOptionsPage (parent)
{
  mp_ui = new Ui::OASISWriterOptionPage ();
  mp_ui->setupUi (this);
}

OASISWriterOptionPage::~OASISWriterOptionPage ()
{
  delete mp_ui;
}

void 
OASISWriterOptionPage::setup (const db::FormatSpecificWriterOptions *o)
{
  const db::OASISWriterOptions *options = dynamic_cast<const db::OASISWriterOptions *> (o);
  if (options) {
    mp_ui->compression_slider->setValue (options->compression_level);
    mp_ui->write_cblocks->setChecked (options->write_cblocks);
    mp_ui->strict_mode->setChecked (options->strict_mode);
    mp_ui->std_prop_mode->setCurrentIndex (options->write_std_properties);
    mp_ui->subst_char->setText (tl::to_qstring (options->subst_char));
  }
}

void 
OASISWriterOptionPage::commit (db::FormatSpecificWriterOptions *o, bool gzip)
{
  if (gzip && mp_ui->write_cblocks->isChecked ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("gzip compression cannot be used with CBLOCK compression")));
  }

  if (mp_ui->subst_char->text ().size () > 1) {
    throw tl::Exception (tl::to_string (QObject::tr ("Substitution character must be either empty or exactly one character")));
  }

  db::OASISWriterOptions *options = dynamic_cast<db::OASISWriterOptions *> (o);
  if (options) {
    options->compression_level = mp_ui->compression_slider->value ();
    options->write_cblocks = mp_ui->write_cblocks->isChecked ();
    options->strict_mode = mp_ui->strict_mode->isChecked ();
    options->write_std_properties = mp_ui->std_prop_mode->currentIndex ();
    options->subst_char = tl::to_string (mp_ui->subst_char->text ());
  }
}

// ---------------------------------------------------------------
//  OASISWriterPluginDeclaration definition and implementation

class OASISWriterPluginDeclaration
  : public StreamWriterPluginDeclaration
{
public:
  OASISWriterPluginDeclaration () 
    : StreamWriterPluginDeclaration ("OASIS" /*same than delivered by the stream format decl*/)
  {
    // .. nothing yet ..
  }

  StreamWriterOptionsPage *format_specific_options_page (QWidget *parent) const
  {
    return new OASISWriterOptionPage (parent);
  }

  void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    db::OASISWriterOptions m_options;
    options.push_back (std::make_pair (cfg_oasis_compression, tl::to_string (m_options.compression_level)));
    options.push_back (std::make_pair (cfg_oasis_write_cblocks, tl::to_string (m_options.write_cblocks)));
    options.push_back (std::make_pair (cfg_oasis_strict_mode, tl::to_string (m_options.strict_mode)));
    options.push_back (std::make_pair (cfg_oasis_write_std_properties, tl::to_string (m_options.write_std_properties)));
    options.push_back (std::make_pair (cfg_oasis_substitution_char, m_options.subst_char));
  }

  db::FormatSpecificWriterOptions *create_specific_options (lay::PluginRoot *config_root) const
  {
    db::OASISWriterOptions *options = new db::OASISWriterOptions ();

    //  no initialization if config_root is not set.
    if (config_root) {
      try {
        config_root->config_get (cfg_oasis_compression, options->compression_level);
        config_root->config_get (cfg_oasis_write_cblocks, options->write_cblocks);
        config_root->config_get (cfg_oasis_strict_mode, options->strict_mode);
        config_root->config_get (cfg_oasis_write_std_properties, options->write_std_properties);
        config_root->config_get (cfg_oasis_substitution_char, options->subst_char);
      } catch (...) {
        delete options;
        throw;
      }
    }

    return options;
  }

  void commit_to_config (const db::FormatSpecificWriterOptions *o, lay::PluginRoot *config_root) const 
  {
    const db::OASISWriterOptions *options = dynamic_cast<const db::OASISWriterOptions *> (o);
    if (options) {
      config_root->config_set (cfg_oasis_compression, options->compression_level);
      config_root->config_set (cfg_oasis_write_cblocks, options->write_cblocks);
      config_root->config_set (cfg_oasis_strict_mode, options->strict_mode);
      config_root->config_set (cfg_oasis_write_std_properties, options->write_std_properties);
      config_root->config_set (cfg_oasis_substitution_char, options->subst_char);
    }
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> plugin_decl (new lay::OASISWriterPluginDeclaration (), 10000, "OASISWriter");

// ---------------------------------------------------------------
//  gsi Implementation of specific methods

static void get_oasis_options (const db::SaveLayoutOptions *options, db::OASISWriterOptions &oasis_options)
{
  const db::OASISWriterOptions *current_oasis_options = dynamic_cast <const db::OASISWriterOptions *> (options->get_specific_options ());
  if (current_oasis_options) {
    oasis_options = *current_oasis_options;
  }
}

static void set_oasis_compression (db::SaveLayoutOptions *options, int comp)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  oasis_options.compression_level = comp;
  options->set_specific_options (oasis_options);
}

static int get_oasis_compression (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.compression_level;
}

static void set_oasis_recompress (db::SaveLayoutOptions *options, bool f)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  oasis_options.recompress = f;
  options->set_specific_options (oasis_options);
}

static bool get_oasis_recompress (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.recompress;
}

static void set_oasis_write_std_properties (db::SaveLayoutOptions *options, bool f)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  if (f && oasis_options.write_std_properties == 0) {
    oasis_options.write_std_properties = 1;
    options->set_specific_options (oasis_options);
  } else if (!f && oasis_options.write_std_properties != 0) {
    oasis_options.write_std_properties = 0;
    options->set_specific_options (oasis_options);
  }
}

static bool get_oasis_write_std_properties (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.write_std_properties != 0;
}

static void set_oasis_write_cell_bounding_boxes (db::SaveLayoutOptions *options, bool f)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  if (f && oasis_options.write_std_properties < 2) {
    oasis_options.write_std_properties = 2;
    options->set_specific_options (oasis_options);
  } else if (!f && oasis_options.write_std_properties >= 2) {
    oasis_options.write_std_properties = 1;
    options->set_specific_options (oasis_options);
  }
}

static bool get_oasis_write_cell_bounding_boxes (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.write_std_properties >= 2;
}

static void set_oasis_write_cblocks (db::SaveLayoutOptions *options, bool f)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  oasis_options.write_cblocks = f;
  options->set_specific_options (oasis_options);
}

static bool get_oasis_write_cblocks (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.write_cblocks;
}

static void set_oasis_strict_mode (db::SaveLayoutOptions *options, bool f)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  oasis_options.strict_mode = f;
  options->set_specific_options (oasis_options);
}

static bool get_oasis_strict_mode (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.strict_mode;
}

static void set_oasis_subst_char (db::SaveLayoutOptions *options, const std::string &sc)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  oasis_options.subst_char = sc;
  options->set_specific_options (oasis_options);
}

static std::string get_oasis_subst_char (const db::SaveLayoutOptions *options)
{
  db::OASISWriterOptions oasis_options;
  get_oasis_options (options, oasis_options);
  return oasis_options.subst_char;
}

//  extend lay::SaveLayoutOptions with the OASIS options 
static
gsi::ClassExt<db::SaveLayoutOptions> oasis_writer_options (
  gsi::method_ext ("oasis_write_cblocks=", &set_oasis_write_cblocks,
    "@brief Sets a value indicating whether to write compressed CBLOCKS per cell\n"
    "@args flag\n"
    "Setting this property clears all format specific options for other formats such as GDS.\n"
  ) +
  gsi::method_ext ("oasis_write_cblocks?", &get_oasis_write_cblocks,
    "@brief Gets a value indicating whether to write compressed CBLOCKS per cell\n"
  ) +
  gsi::method_ext ("oasis_strict_mode=", &set_oasis_strict_mode,
    "@brief Sets a value indicating whether to write strict-mode OASIS files\n"
    "@args flag\n"
    "Setting this property clears all format specific options for other formats such as GDS.\n"
  ) +
  gsi::method_ext ("oasis_strict_mode?", &get_oasis_strict_mode,
    "@brief Gets a value indicating whether to write strict-mode OASIS files\n"
  ) +
  gsi::method_ext ("oasis_substitution_char=", &set_oasis_subst_char,
    "@brief Sets the substitution character for a-strings and n-strings\n"
    "@args char\n"
    "The substitution character is used in place of invalid characters. The value of this "
    "attribute is a string which is either empty or a single character. If the string is "
    "empty, no substitution is made at the risk of producing invalid OASIS files.\n"
    "\n"
    "This attribute has been introduce in version 0.23.\n"
  ) +
  gsi::method_ext ("oasis_substitution_char", &get_oasis_subst_char,
    "@brief Gets the substitution character\n"
    "\n"
    "See \\oasis_substitution_char for details. This attribute has been introduced in version 0.23.\n"
  ) +
  gsi::method_ext ("oasis_recompress=", &set_oasis_recompress,
    "@brief Set OASIS recompression mode\n"
    "@args flag\n"
    "If this flag is true, shape arrays already existing will be resolved and compression is applied "
    "to the individual shapes again. If this flag is false (the default), shape arrays already existing "
    "will be written as such.\n"
    "\n"
    "Setting this property clears all format specific options for other formats such as GDS.\n"
    "\n"
    "This method has been introduced in version 0.23."
  ) +
  gsi::method_ext ("oasis_recompress?", &get_oasis_recompress,
    "@brief Get the OASIS recompression mode\n"
    "See \\oasis_recompression= method for a description of the OASIS compression level."
    "\n"
    "This method has been introduced in version 0.23."
  ) +
  gsi::method_ext ("oasis_write_cell_bounding_boxes=", &set_oasis_write_cell_bounding_boxes,
    "@brief Sets a value indicating whether cell bounding boxes are written\n"
    "@args flag\n"
    "If this value is set to true, cell bounding boxes are written (S_BOUNDING_BOX). "
    "The S_BOUNDING_BOX properties will be attached to the CELLNAME records.\n"
    "\n"
    "Setting this value to true will also enable writing of other standard properties like "
    "S_TOP_CELL (see \\oasis_write_std_properties=).\n"
    "By default, cell bounding boxes are not written, but standard properties are.\n"
    "\n"
    "Setting this property clears all format specific options for other formats such as GDS.\n"
    "\n"
    "This method has been introduced in version 0.24.3."
  ) +
  gsi::method_ext ("oasis_write_cell_bounding_boxes?", &get_oasis_write_cell_bounding_boxes,
    "@brief Gets a value indicating whether cell bounding boxes are written\n"
    "See \\oasis_write_cell_bounding_boxes= method for a description of this flag."
    "\n"
    "This method has been introduced in version 0.24.3."
  ) +
  gsi::method_ext ("oasis_write_std_properties=", &set_oasis_write_std_properties,
    "@brief Sets a value indicating whether standard properties will be written\n"
    "@args flag\n"
    "If this value is false, no standard properties are written. If true, S_TOP_CELL and some other global "
    "standard properties are written. In addition, \\oasis_write_cell_bounding_boxes= can be used to "
    "write cell bounding boxes using S_BOUNDING_BOX.\n"
    "\n"
    "By default, this flag is true and standard properties are written.\n"
    "\n"
    "Setting this property clears all format specific options for other formats such as GDS.\n"
    "Setting this property to false clears the oasis_write_cell_bounding_boxes flag too.\n"
    "\n"
    "This method has been introduced in version 0.24."
  ) +
  gsi::method_ext ("oasis_write_std_properties?", &get_oasis_write_std_properties,
    "@brief Gets a value indicating whether standard properties will be written\n"
    "See \\oasis_write_std_properties= method for a description of this flag."
    "\n"
    "This method has been introduced in version 0.24."
  ) +
  gsi::method_ext ("oasis_compression_level=", &set_oasis_compression,
    "@brief Set the OASIS compression level\n"
    "@args level\n"
    "The OASIS compression level is an integer number between 0 and 10. 0 basically is no compression, "
    "1 produces shape arrays in a simple fashion. 2 and higher compression levels will use a more elaborate "
    "algorithm to find shape arrays which uses 2nd and futher neighbor distances. The higher the level, the "
    "higher the memory requirements and run times.\n"
    "Setting this property clears all format specific options for other formats such as GDS.\n"
  ) +
  gsi::method_ext ("oasis_compression_level", &get_oasis_compression,
    "@brief Get the OASIS compression level\n"
    "See \\oasis_compression_level= method for a description of the OASIS compression level."
  ),
  ""
);

}




