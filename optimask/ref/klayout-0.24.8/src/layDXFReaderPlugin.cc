
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
#include "dbDXFReader.h"
#include "dbLoadLayoutOptions.h"
#include "layMainWindow.h"
#include "layDXFReaderPlugin.h"
#include "DXFReaderOptionPage.h"
#include "gsiDecl.h"

#include <QtGui/QFrame>

namespace lay
{

const std::string cfg_dxf_dbu ("dxf-dbu");
const std::string cfg_dxf_unit ("dxf-unit");
const std::string cfg_dxf_text_scaling ("dxf-text-scaling");
const std::string cfg_dxf_polyline_mode ("dxf-polyline-mode");
const std::string cfg_dxf_circle_points ("dxf-circle-points");
const std::string cfg_dxf_render_texts_as_polygons ("dxf-render-texts-as-polygons");
const std::string cfg_dxf_keep_other_cells ("dxf-keep-other-cells");

// ---------------------------------------------------------------
//  DXFReaderOptionPage definition and implementation

DXFReaderOptionPage::DXFReaderOptionPage (QWidget *parent)
  : StreamReaderOptionsPage (parent)
{
  mp_ui = new Ui::DXFReaderOptionPage ();
  mp_ui->setupUi (this);
}

DXFReaderOptionPage::~DXFReaderOptionPage ()
{
  delete mp_ui;
  mp_ui = 0;
}

void 
DXFReaderOptionPage::setup (const db::FormatSpecificReaderOptions *o)
{
  const db::DXFReaderOptions *options = dynamic_cast<const db::DXFReaderOptions *> (o);
  if (options) {
    mp_ui->dbu_le->setText (tl::to_qstring (tl::to_string (options->dbu)));
    mp_ui->unit_le->setText (tl::to_qstring (tl::to_string (options->unit)));
    mp_ui->text_scaling_le->setText (tl::to_qstring (tl::to_string (options->text_scaling)));
    mp_ui->circle_points_le->setText (tl::to_qstring (tl::to_string (options->circle_points)));
    mp_ui->render_texts_as_polygons_cbx->setChecked (options->render_texts_as_polygons);
    mp_ui->keep_other_cells_cbx->setChecked (options->keep_other_cells);
    mp_ui->polyline2poly_cbx->setCurrentIndex (options->polyline_mode);
  }
}

void 
DXFReaderOptionPage::commit (db::FormatSpecificReaderOptions *o)
{
  db::DXFReaderOptions *options = dynamic_cast<db::DXFReaderOptions *> (o);
  if (options) {
    tl::from_string (tl::to_string (mp_ui->dbu_le->text ()), options->dbu);
    if (options->dbu > 1000.0 || options->dbu < 1e-9) {
      throw tl::Exception (tl::to_string (QObject::tr ("Invalid value for database unit")));
    }
    tl::from_string (tl::to_string (mp_ui->unit_le->text ()), options->unit);
    if (options->unit > 1e9 || options->unit < 1e-9) {
      throw tl::Exception (tl::to_string (QObject::tr ("Invalid value for the unit")));
    }
    tl::from_string (tl::to_string (mp_ui->text_scaling_le->text ()), options->text_scaling);
    if (options->text_scaling > 10000 || options->text_scaling < 1) {
      throw tl::Exception (tl::to_string (QObject::tr ("Invalid value for the text scaling")));
    }
    tl::from_string (tl::to_string(mp_ui->circle_points_le->text ()), options->circle_points);
    if (options->circle_points < 4 || options->circle_points > 1000000) {
      throw tl::Exception (tl::to_string (QObject::tr ("Invalid value for the number of points for arc interpolation")));
    }
    options->polyline_mode = mp_ui->polyline2poly_cbx->currentIndex ();
    options->render_texts_as_polygons = mp_ui->render_texts_as_polygons_cbx->isChecked ();
    options->keep_other_cells = mp_ui->keep_other_cells_cbx->isChecked ();
  }
}

// ---------------------------------------------------------------
//  DXFReaderPluginDeclaration definition and implementation

class DXFReaderPluginDeclaration
  : public StreamReaderPluginDeclaration
{
public:
  DXFReaderPluginDeclaration () 
    : StreamReaderPluginDeclaration ("DXF" /*same than delivered by the stream format decl*/)
  {
    // .. nothing yet ..
  }

  StreamReaderOptionsPage *format_specific_options_page (QWidget *parent) const
  {
    return new DXFReaderOptionPage (parent);
  }

  void get_options (std::vector < std::pair<std::string, std::string> > &options) const
  {
    db::DXFReaderOptions m_options;
    options.push_back (std::make_pair (cfg_dxf_dbu, tl::to_string (m_options.dbu)));
    options.push_back (std::make_pair (cfg_dxf_unit, tl::to_string (m_options.unit)));
    options.push_back (std::make_pair (cfg_dxf_text_scaling, tl::to_string (m_options.text_scaling)));
    options.push_back (std::make_pair (cfg_dxf_polyline_mode, tl::to_string (m_options.polyline_mode)));
    options.push_back (std::make_pair (cfg_dxf_circle_points, tl::to_string (m_options.circle_points)));
    options.push_back (std::make_pair (cfg_dxf_render_texts_as_polygons, tl::to_string (m_options.render_texts_as_polygons)));
    options.push_back (std::make_pair (cfg_dxf_keep_other_cells, tl::to_string (m_options.keep_other_cells)));
  }

  db::FormatSpecificReaderOptions *create_specific_options (lay::PluginRoot *config_root) const
  {
    db::DXFReaderOptions *options = new db::DXFReaderOptions ();

    //  no initialization if config_root is not set.
    if (config_root) {
      try {
        config_root->config_get (cfg_dxf_dbu, options->dbu);
        config_root->config_get (cfg_dxf_unit, options->unit);
        config_root->config_get (cfg_dxf_text_scaling, options->text_scaling);
        config_root->config_get (cfg_dxf_circle_points, options->circle_points);
        config_root->config_get (cfg_dxf_polyline_mode, options->polyline_mode);
        config_root->config_get (cfg_dxf_render_texts_as_polygons, options->render_texts_as_polygons);
        config_root->config_get (cfg_dxf_keep_other_cells, options->keep_other_cells);
      } catch (...) {
        delete options;
        throw;
      }
    }

    return options;
  }

  void commit_to_config (const db::FormatSpecificReaderOptions *o, lay::PluginRoot *config_root) const 
  {
    const db::DXFReaderOptions *options = dynamic_cast<const db::DXFReaderOptions *> (o);
    if (options) {
      config_root->config_set (cfg_dxf_dbu, options->dbu);
      config_root->config_set (cfg_dxf_unit, options->unit);
      config_root->config_set (cfg_dxf_text_scaling, options->text_scaling);
      config_root->config_set (cfg_dxf_circle_points, options->circle_points);
      config_root->config_set (cfg_dxf_polyline_mode, options->polyline_mode);
      config_root->config_set (cfg_dxf_render_texts_as_polygons, options->render_texts_as_polygons);
      config_root->config_set (cfg_dxf_keep_other_cells, options->keep_other_cells);
    }
  }
};

static tl::RegisteredClass<lay::PluginDeclaration> plugin_decl (new lay::DXFReaderPluginDeclaration (), 10000, "DXFReader");

// ---------------------------------------------------------------
//  gsi Implementation of specific methods

static void get_dxf_options (const db::LoadLayoutOptions *options, db::DXFReaderOptions &dxf_options)
{
  const db::DXFReaderOptions *current_dxf_options = dynamic_cast <const db::DXFReaderOptions *> (options->get_specific_options ("DXF"));
  if (current_dxf_options) {
    dxf_options = *current_dxf_options;
  }
}

static void set_dxf_dbu (db::LoadLayoutOptions *options, double dbu)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.dbu = dbu;
  options->set_specific_options (dxf_options, "DXF");
}

static double get_dxf_dbu (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.dbu;
}

static void set_dxf_text_scaling (db::LoadLayoutOptions *options, double text_scaling)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.text_scaling = text_scaling;
  options->set_specific_options (dxf_options, "DXF");
}

static double get_dxf_text_scaling (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.text_scaling;
}

static void set_dxf_unit (db::LoadLayoutOptions *options, double unit)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.unit = unit;
  options->set_specific_options (dxf_options, "DXF");
}

static double get_dxf_unit (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.unit;
}

static void set_dxf_render_texts_as_polygons (db::LoadLayoutOptions *options, bool value)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.render_texts_as_polygons = value;
  options->set_specific_options (dxf_options, "DXF");
}

static bool get_dxf_render_texts_as_polygons (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.render_texts_as_polygons;
}

static void set_dxf_keep_other_cells (db::LoadLayoutOptions *options, bool value)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.keep_other_cells = value;
  options->set_specific_options (dxf_options, "DXF");
}

static bool get_dxf_keep_other_cells (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.keep_other_cells;
}

static void set_dxf_circle_points (db::LoadLayoutOptions *options, int circle_points)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.circle_points = circle_points;
  options->set_specific_options (dxf_options, "DXF");
}

static int get_dxf_circle_points (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.circle_points;
}

static void set_dxf_polyline_mode (db::LoadLayoutOptions *options, int mode)
{
  if (mode < 0 || mode > 4) {
    throw tl::Exception (tl::to_string (QObject::tr ("Invalid polygon mode")));
  }

  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  dxf_options.polyline_mode = mode;
  options->set_specific_options (dxf_options, "DXF");
}

static int get_dxf_polyline_mode (const db::LoadLayoutOptions *options)
{
  db::DXFReaderOptions dxf_options;
  get_dxf_options (options, dxf_options);
  return dxf_options.polyline_mode;
}

//  extend lay::LoadLayoutOptions with the DXF options 
static
gsi::ClassExt<db::LoadLayoutOptions> dxf_reader_options (
  gsi::method_ext ("dxf_dbu=", &set_dxf_dbu,
    "@brief Specifies the database unit which the reader uses and produces\n"
    "@args dbu\n"
    "\nThis property has been added in version 0.21.\n"
  ) +
  gsi::method_ext ("dxf_dbu", &get_dxf_dbu,
    "@brief Specifies the database unit which the reader uses and produces\n"
    "\nThis property has been added in version 0.21.\n"
  ) +
  gsi::method_ext ("dxf_text_scaling=", &set_dxf_text_scaling,
    "@brief Specifies the text scaling in percent of the default scaling\n"
    "@args unit\n"
    "\n"
    "The default value 100, meaning that the letter pitch is roughly 92 percent of the specified text height. "
    "Decrease this value to get smaller fonts and increase it to get larger fonts.\n"
    "\nThis property has been added in version 0.21.20.\n"
  ) +
  gsi::method_ext ("dxf_text_scaling", &get_dxf_text_scaling,
    "@brief Gets the text scaling factor (see \\dxf_text_scaling=)\n"
    "\nThis property has been added in version 0.21.20.\n"
  ) +
  gsi::method_ext ("dxf_unit=", &set_dxf_unit,
    "@brief Specifies the unit in which the DXF file is drawn.\n"
    "@args unit\n"
    "\nThis property has been added in version 0.21.3.\n"
  ) +
  gsi::method_ext ("dxf_unit", &get_dxf_unit,
    "@brief Specifies the unit in which the DXF file is drawn\n"
    "\nThis property has been added in version 0.21.3.\n"
  ) +
  gsi::method_ext ("dxf_circle_points=", &set_dxf_circle_points,
    "@brief Specifies the number of points used per full circle for arc interpolation\n"
    "@args points\n"
    "\nThis property has been added in version 0.21.6.\n"
  ) +
  gsi::method_ext ("dxf_circle_points", &get_dxf_circle_points,
    "@brief Specifies the number of points used per full circle for arc interpolation\n"
    "\nThis property has been added in version 0.21.6.\n"
  ) +
  gsi::method_ext ("dxf_render_texts_as_polygons=", &set_dxf_render_texts_as_polygons,
    "@brief If this option is set to true, text objects are rendered as polygons\n"
    "@args value\n"
    "\nThis property has been added in version 0.21.15.\n"
  ) +
  gsi::method_ext ("dxf_render_texts_as_polygons", &get_dxf_render_texts_as_polygons,
    "@brief If this option is true, text objects are rendered as polygons\n"
    "\nThis property has been added in version 0.21.15.\n"
  ) +
  gsi::method_ext ("dxf_keep_other_cells=", &set_dxf_keep_other_cells,
    "@brief If this option is set to true, all cells are kept, not only the top cell and it's children\n"
    "@args value\n"
    "\nThis property has been added in version 0.21.15.\n"
  ) +
  gsi::method_ext ("dxf_keep_other_cells", &get_dxf_keep_other_cells,
    "@brief If this option is true, all cells are kept, not only the top cell and it's children\n"
    "\nThis property has been added in version 0.21.15.\n"
  ) +
  gsi::method_ext ("dxf_polyline_mode=", &set_dxf_polyline_mode,
    "@brief Specifies how to treat POLYLINE/LWPOLYLINE entities.\n"
    "@args mode\n"
    "The mode is 0 (automatic), 1 (keep lines), 2 (create polygons from closed polylines with width = 0), "
    "3 (merge all lines with width = 0 into polygons), 4 (as 3 plus auto-close open contours).\n"
    "\nThis property has been added in version 0.21.3.\n"
  ) +
  gsi::method_ext ("dxf_polyline_mode", &get_dxf_polyline_mode,
    "@brief Specifies whether closed POLYLINE and LWPOLYLINE entities with width 0 are converted to polygons.\n"
    "See \\dxf_polyline_mode= for a description of this property.\n"
    "\nThis property has been added in version 0.21.3.\n"
  ),
  ""
);

}





