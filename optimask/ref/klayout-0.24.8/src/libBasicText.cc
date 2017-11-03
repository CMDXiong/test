
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


#include "libBasicText.h"
#include "dbStream.h"
#include "dbReader.h"
#include "dbEdgeProcessor.h"
#include "tlStream.h"
#include "layApplication.h"

#include <cctype>

#include <QtCore/QResource>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

namespace lib
{

// --------------------------------------------------------------------------
//  Font class and declaration

class BasicTextFont
{
public:
  BasicTextFont ();

  void load_from_resource (const std::string &name);

  void load_from_file (const std::string &filename);

  const std::string &description () const
  {
    return m_description;
  }

  const std::vector<db::Polygon> &glyph (char c) const
  {
    std::map<char, std::vector<db::Polygon> >::const_iterator dc = m_data.find (m_lowercase_supported ? c : toupper(c));
    if (dc != m_data.end ()) {
      return dc->second;
    } else {
      static std::vector<db::Polygon> empty_polygons;
      return empty_polygons;
    }
  }

  db::Coord line_width () const
  {
    return m_line_width;
  }

  db::Coord design_grid () const
  {
    return m_design_grid;
  }

  db::Coord width () const
  {
    return m_width;
  }

  db::Coord height () const
  {
    return m_height;
  }

  const db::Box &background () const
  {
    return m_background;
  }

  double dbu () const
  {
    return m_dbu;
  }

  static const std::vector<BasicTextFont> &fonts ();

private:
  std::map<char, std::vector<db::Polygon> > m_data;
  db::Coord m_width, m_height, m_line_width, m_design_grid;
  db::Box m_background;
  std::string m_description;
  double m_dbu;
  bool m_lowercase_supported;

  void read_from_layout (const db::Layout &layout, unsigned int ldata, unsigned int lborder, unsigned int lbackground);
};

BasicTextFont::BasicTextFont ()
  : m_width (0), m_height (0), m_line_width (0), m_design_grid (0), m_dbu (0.001), m_lowercase_supported (false)
{
  //  .. nothing yet ..
}

/**
 *  By convention the file must have two layers:
 *
 *    1/0 for the actual data
 *    2/0 for the borders
 *    3/0 for an optional additional background
 *
 *  Currently, all glyphs must be bottom-left aligned at 0, 0. The 
 *  border must be drawn in at least one glyph cell. The border is taken
 *  as the overall bbox of all borders.
 *
 *  The glyph cells must be named with a single character or "nnn" where "d" is the 
 *  ASCII code of the character (i.e. "032" for space). Allowed ASCII codes are 32 through 127. 
 *  If a lower-case "a" character is defined, lower-case letters are supported. 
 *  Otherwise, lowercase letters are mapped to uppercase letters.
 *
 *  Undefined characters are left blank.
 *
 *  A comment cell can be defined ("COMMENT") which must hold one text in layer 1
 *  stating the comment.
 */

void 
BasicTextFont::load_from_resource (const std::string &name)
{
  try {

    QResource res (tl::to_qstring (name));
    QByteArray data = qUncompress (QByteArray ((const char *) res.data (), int (res.size ())));

    db::Layout layout;
    tl::InputMemoryStream memory_stream (data.constData (), data.size ());
    tl::InputStream stream (memory_stream);
    db::Reader reader (stream);
    db::LayerMap map = reader.read (layout);

    m_description = name;

    std::pair<bool, unsigned int> l1 = map.logical (db::LDPair (1, 0));
    std::pair<bool, unsigned int> l2 = map.logical (db::LDPair (2, 0));
    std::pair<bool, unsigned int> l3 = map.logical (db::LDPair (3, 0));

    if (l1.first && l2.first) {
      read_from_layout (layout, l1.second, l2.second, l3.second);
    }

  } catch (tl::Exception &ex) {
    tl::error << "Error reading font resource '" << name << "': " << ex.msg ();
    throw;
  }
}

void 
BasicTextFont::load_from_file (const std::string &filename)
{
  try {

    db::Layout layout;
    tl::InputStream stream (filename);
    db::Reader reader (stream);
    db::LayerMap map = reader.read (layout);

    m_description = filename;

    std::pair<bool, unsigned int> l1 = map.logical (db::LDPair (1, 0));
    std::pair<bool, unsigned int> l2 = map.logical (db::LDPair (2, 0));
    std::pair<bool, unsigned int> l3 = map.logical (db::LDPair (3, 0));

    if (l1.first && l2.first) {
      read_from_layout (layout, l1.second, l2.second, l3.second);
    }

  } catch (tl::Exception &ex) {
    tl::error << "Error reading font file '" << filename << "': " << ex.msg ();
    throw;
  }
}

void 
BasicTextFont::read_from_layout (const db::Layout &layout, unsigned int l1, unsigned int l2, unsigned int l3)
{
  m_dbu = layout.dbu ();

  //  try to read the comment
  std::pair<bool, db::cell_index_type> cn = layout.cell_by_name ("COMMENT");
  if (cn.first) {

    db::Shapes::shape_iterator sh = layout.cell (cn.second).shapes (l1).begin (db::ShapeIterator::All);
    while (! sh.at_end ()) {

      if (sh->is_text ()) {

        std::string s = sh->text_string ();
        tl::Extractor ex (s.c_str ());

        if (ex.test ("line_width")) {

          ex.test ("=");
          m_line_width = 0;
          ex.try_read (m_line_width);

        } else if (ex.test ("design_grid")) {

          ex.test ("=");
          m_design_grid = 0;
          ex.try_read (m_design_grid);

        } else {
          m_description = sh->text_string ();
        }

      }

      ++sh;

    }

  }

  m_lowercase_supported = layout.cell_by_name ("a").first || layout.cell_by_name ("065").first;

  db::Box bbox, bg;

  //  read the data and determine the bounding box 
  for (int ch = 32; ch < 128; ++ch) {

    char n[32];
    n[0] = char (ch);
    n[1] = 0;

    std::pair<bool, db::cell_index_type> cn = layout.cell_by_name (n);
    if (! cn.first) {
      sprintf (n, "%03d", ch);
      cn = layout.cell_by_name (n);
    }

    if (cn.first) {

      std::vector<db::Polygon> &data = m_data.insert (std::make_pair (char (ch), std::vector<db::Polygon> ())).first->second;

      bbox += layout.cell (cn.second).bbox (l2);

      bg += layout.cell (cn.second).bbox (l2);
      bg += layout.cell (cn.second).bbox (l3);

      db::Shapes::shape_iterator sh = layout.cell (cn.second).shapes (l1).begin (db::ShapeIterator::All);
      while (! sh.at_end ()) {
        if (sh->is_box () || sh->is_path () || sh->is_polygon ()) {
          data.push_back (db::Polygon ());
          sh->instantiate (data.back ());
        }
        ++sh;
      }

    }

  }

  if (! bbox.empty ()) {
    m_width = bbox.width ();
    m_height = bbox.height ();
  }

  m_background = bg;
}

const std::vector<BasicTextFont> &
BasicTextFont::fonts ()
{
  static std::vector<BasicTextFont> m_fonts;

  if (m_fonts.empty ()) {

    const char *resources[] = { 
      ":/fonts/std_font.gds"
    };

    for (size_t i = 0 ; i < sizeof (resources) / sizeof (resources [0]); ++i) {
      try {
        m_fonts.push_back (BasicTextFont ());
        m_fonts.back ().load_from_resource (resources [i]);
      } catch (...) {
        m_fonts.pop_back ();
      }
    }

    if (lay::Application::instance ()) {

      //  scan for font files
      for (std::vector <std::string>::const_iterator p = lay::Application::instance ()->klayout_path ().begin (); p != lay::Application::instance ()->klayout_path ().end (); ++p) {

        QDir fp = QDir (tl::to_qstring (*p)).filePath (tl::to_qstring ("fonts"));

        QStringList name_filters;
        name_filters << QString::fromAscii ("*");

        QStringList font_files = fp.entryList (name_filters, QDir::Files);
        for (QStringList::const_iterator ff = font_files.begin (); ff != font_files.end (); ++ff) {

          try {
            m_fonts.push_back (BasicTextFont ());
            m_fonts.back ().load_from_file (tl::to_string (fp.filePath (*ff)));
          } catch (...) {
            m_fonts.pop_back ();
          }
        
        }

      }

    }

  }

  return m_fonts;
}

// --------------------------------------------------------------------------
//  Implementation

static const size_t p_text = 0;
static const size_t p_font = 1;
static const size_t p_layer = 2;
static const size_t p_magnification = 3;
static const size_t p_inverse = 4;
static const size_t p_bias = 5;
static const size_t p_char_spacing = 6;
static const size_t p_line_spacing = 7;
static const size_t p_eff_cell_width = 8;
static const size_t p_eff_cell_height = 9;
static const size_t p_eff_line_width = 10;
static const size_t p_eff_design_raster = 11;
static const size_t p_total = 12;

BasicText::BasicText ()
{
  //  .. nothing yet ..
}

bool 
BasicText::can_create_from_shape (const db::Layout &layout, const db::Shape &shape, unsigned int layer) const
{
  return shape.is_text ();
}

db::Trans
BasicText::transformation_from_shape (const db::Layout &layout, const db::Shape &shape, unsigned int layer) const
{
  //  use the displacement to define the center of the circle
  return shape.text_trans ();
}

db::pcell_parameters_type
BasicText::parameters_from_shape (const db::Layout &layout, const db::Shape &shape, unsigned int layer) const
{
  //  use map_parameters to create defaults for the other parameters
  std::map<size_t, tl::Variant> nm;
  nm.insert (std::make_pair (p_layer, tl::Variant (layout.get_properties (layer))));
  nm.insert (std::make_pair (p_text, tl::Variant (shape.text_string ())));
  if (shape.text_size () > 0) {
    double hfont = 1.0;
    if (! BasicTextFont::fonts ().empty ()) {
      hfont = BasicTextFont::fonts ().front ().height () * BasicTextFont::fonts ().front ().dbu ();
    }
    nm.insert (std::make_pair (p_magnification, tl::Variant (layout.dbu () * shape.text_size () / hfont)));
  }
  return map_parameters (nm);
}

std::vector<db::PCellLayerDeclaration> 
BasicText::get_layer_declarations (const db::pcell_parameters_type &parameters) const
{
  std::vector<db::PCellLayerDeclaration> layers;
  if (parameters.size () > p_layer && parameters [p_layer].is_user<db::LayerProperties> ()) {
    db::LayerProperties lp = parameters [p_layer].to_user<db::LayerProperties> ();
    if (lp != db::LayerProperties ()) {
      layers.push_back (lp);
    }
  }
  return layers;
}

void 
BasicText::coerce_parameters (const db::Layout &layout, db::pcell_parameters_type &parameters) const
{
  //  Compute the read-only parameters

  if (parameters.size () < p_total || BasicTextFont::fonts ().empty ()) {
    return;
  }

  std::string t = parameters [p_text].to_string ();
  int f = parameters [p_font].to_int ();
  if (f < 0 || f >= int (BasicTextFont::fonts ().size ())) {
    f = 0;
  }
  const BasicTextFont &font = BasicTextFont::fonts ()[f];
  double m = parameters [p_magnification].to_double ();
  double b = parameters [p_bias].to_double ();
  parameters [p_eff_cell_width] = font.width () * layout.dbu () * m;
  parameters [p_eff_cell_height] = font.height () * layout.dbu () * m;
  parameters [p_eff_line_width] = font.line_width () * layout.dbu () * m + 2.0 * b;
  parameters [p_eff_design_raster] = font.design_grid () * layout.dbu () * m;
}

void 
BasicText::produce (const db::Layout &layout, const std::vector<unsigned int> &layer_ids, const db::pcell_parameters_type &parameters, db::Cell &cell) const
{
  if (parameters.size () < 6 || layer_ids.size () < 1 || BasicTextFont::fonts ().empty ()) {
    return;
  }

  db::EdgeProcessor ep;

  std::string t = parameters [p_text].to_string ();
  int f = parameters [p_font].to_int ();
  if (f < 0 || f >= int (BasicTextFont::fonts ().size ())) {
    f = 0;
  }
  const BasicTextFont &font = BasicTextFont::fonts ()[f];
  double m = parameters [p_magnification].to_double () * font.dbu () / layout.dbu ();
  db::Coord b = db::coord_traits<db::Coord>::rounded (parameters [p_bias].to_double () / layout.dbu ());
  bool inv = parameters [p_inverse].to_bool ();

  db::Coord x = 0, y = 0;
  db::Coord dx = db::coord_traits<db::Coord>::rounded (m * font.width () + parameters [p_char_spacing].to_double () / layout.dbu ());
  db::Coord dy = db::coord_traits<db::Coord>::rounded (m * font.height () + parameters [p_line_spacing].to_double () / layout.dbu ());

  std::vector<db::Polygon> data;
  db::Box background;

  for (const char *cp = t.c_str (); *cp; ++cp) {

    char c = *cp;
    if (c == '\\' && cp [1]) {
      if (cp [1] == 'n') {
        ++cp;
        y -= dy;
        x = 0;
        c = 0;
      } else {
        ++cp;
        c = *cp;
      }
    }

    if (c) {

      db::ICplxTrans trans (m, 0.0, false, db::Point (x, y));

      const std::vector<db::Polygon> &glyph = font.glyph (c);
      for (std::vector<db::Polygon>::const_iterator d = glyph.begin (); d != glyph.end (); ++d) {
        data.push_back (d->transformed (trans));
      }

      background += font.background ().transformed (trans);

      x += dx;

    }

  }

  if (b != 0) {
    std::vector<db::Polygon> sized_data;
    ep.size (data, b, b, sized_data); 
    data.swap (sized_data);
  }

  if (inv && ! background.empty ()) {
    std::vector<db::Polygon> bg, in;
    bg.push_back (db::Polygon (background));
    data.swap (in);
    ep.boolean (bg, in, data, db::BooleanOp::ANotB, true, true); 
  }

  for (std::vector<db::Polygon>::const_iterator d = data.begin (); d != data.end (); ++d) {
    cell.shapes (layer_ids [0]).insert (*d);
  }
}

std::string 
BasicText::get_display_name (const db::pcell_parameters_type &parameters) const
{
  std::string t;
  if (! parameters.empty ()) {
    t = parameters [p_text].to_string ();
  }
  return std::string("TEXT('") + t + "')";
}

std::vector<db::PCellParameterDeclaration> 
BasicText::get_parameter_declarations () const
{
  std::vector<db::PCellParameterDeclaration> parameters;

  //  parameter: text 
  tl_assert (parameters.size () == p_text);
  parameters.push_back (db::PCellParameterDeclaration ("text"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_string);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Text")));
  parameters.back ().set_default ("");

  //  parameter: font
  tl_assert (parameters.size () == p_font);
  parameters.push_back (db::PCellParameterDeclaration ("font"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_int);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Font")));
  parameters.back ().set_default (0);

  std::vector<tl::Variant> choices;
  std::vector<std::string> choice_descriptions;
  for (std::vector<BasicTextFont>::const_iterator f = BasicTextFont::fonts ().begin (); f != BasicTextFont::fonts ().end (); ++f) {
    choice_descriptions.push_back (f->description ());
    choices.push_back (int (choices.size ()));
  }
  parameters.back ().set_choices (choices);
  parameters.back ().set_choice_descriptions (choice_descriptions);

  //  parameter: layer 
  tl_assert (parameters.size () == p_layer);
  parameters.push_back (db::PCellParameterDeclaration ("layer"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_layer);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Layer")));

  //  parameter: magnification
  tl_assert (parameters.size () == p_magnification);
  parameters.push_back (db::PCellParameterDeclaration ("mag"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Magnification")));
  parameters.back ().set_default (1.0);

  //  parameter: inverse 
  tl_assert (parameters.size () == p_inverse);
  parameters.push_back (db::PCellParameterDeclaration ("inverse"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_boolean);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Inverse")));
  parameters.back ().set_default (false);

  //  parameter: bias 
  tl_assert (parameters.size () == p_bias);
  parameters.push_back (db::PCellParameterDeclaration ("bias"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Bias")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));

  //  parameter: character spacing 
  tl_assert (parameters.size () == p_char_spacing);
  parameters.push_back (db::PCellParameterDeclaration ("cspacing"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Additional character spacing")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));

  //  parameter: line spacing 
  tl_assert (parameters.size () == p_line_spacing);
  parameters.push_back (db::PCellParameterDeclaration ("lspacing"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Additional line spacing")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));

  //  parameter: effective cell width
  tl_assert (parameters.size () == p_eff_cell_width);
  parameters.push_back (db::PCellParameterDeclaration ("eff_cw"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Computed parameters\tCell width")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));
  parameters.back ().set_readonly (true);

  //  parameter: effective cell height
  tl_assert (parameters.size () == p_eff_cell_height);
  parameters.push_back (db::PCellParameterDeclaration ("eff_ch"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Computed parameters\tCell height")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));
  parameters.back ().set_readonly (true);

  //  parameter: effective line width
  tl_assert (parameters.size () == p_eff_line_width);
  parameters.push_back (db::PCellParameterDeclaration ("eff_lw"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Computed parameters\tLine width")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));
  parameters.back ().set_readonly (true);

  //  parameter: effective design raster
  tl_assert (parameters.size () == p_eff_design_raster);
  parameters.push_back (db::PCellParameterDeclaration ("eff_dr"));
  parameters.back ().set_type (db::PCellParameterDeclaration::t_double);
  parameters.back ().set_description (tl::to_string (QObject::tr ("Computed parameters\tDesign raster")));
  parameters.back ().set_default (0.0);
  parameters.back ().set_unit (tl::to_string (QObject::tr ("micron")));
  parameters.back ().set_readonly (true);

  return parameters;
}

}


