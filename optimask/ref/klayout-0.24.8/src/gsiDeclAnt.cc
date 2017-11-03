
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



#include "gsiDecl.h"
#include "antObject.h"
#include "antService.h"
#include "layLayoutView.h"

namespace gsi
{

static ant::Object *new_ruler ()
{
  return new ant::Object ();
}

static int style_ruler ()       { return int (ant::Object::STY_ruler); }
static int style_arrow_end ()   { return int (ant::Object::STY_arrow_end); }
static int style_arrow_start () { return int (ant::Object::STY_arrow_start); }
static int style_arrow_both ()  { return int (ant::Object::STY_arrow_both); }
static int style_line ()        { return int (ant::Object::STY_line); }

static int outline_diag ()      { return int (ant::Object::OL_diag); }
static int outline_xy ()        { return int (ant::Object::OL_xy); }
static int outline_diag_xy ()   { return int (ant::Object::OL_diag_xy); }
static int outline_yx ()        { return int (ant::Object::OL_yx); }
static int outline_diag_yx ()   { return int (ant::Object::OL_diag_yx); }
static int outline_box ()       { return int (ant::Object::OL_box); }

static int angle_any ()         { return int (lay::AC_Any); }
static int angle_diagonal ()    { return int (lay::AC_Diagonal); }
static int angle_ortho ()       { return int (lay::AC_Ortho); }
static int angle_horizontal ()  { return int (lay::AC_Horizontal); }
static int angle_vertical ()    { return int (lay::AC_Vertical); }
static int angle_global ()      { return int (lay::AC_Global); }

static int get_style (const ant::Object *obj)
{
  return int (obj->style ());
}

static void set_style (ant::Object *obj, int style)
{
  obj->style (ant::Object::style_type (style));
}

static int get_outline (const ant::Object *obj)
{
  return int (obj->outline ());
}

static void set_outline (ant::Object *obj, int outline)
{
  obj->outline (ant::Object::outline_type (outline));
}

static int get_angle_constraint (const ant::Object *obj)
{
  return int (obj->angle_constraint ());
}

static void set_angle_constraint (ant::Object *obj, int angle_constraint)
{
  obj->angle_constraint (lay::angle_constraint_type (angle_constraint));
}

gsi::Class<ant::Object> decl_Annotation ("Annotation", 
  gsi::constructor ("new", &gsi::new_ruler,
    "@brief Create a new ruler or marker with the default attributes"
  ) +
  gsi::method ("StyleRuler|#style_ruler", &gsi::style_ruler,
    "@brief Gets the ruler style code for use the \\style method\n"
    "When this style is specified, the annotation will show a ruler with "
    "some ticks at distances indicating a decade of units and a suitable "
    "subdivision into minor ticks at intervals of 1, 2 or 5 units."
  ) +
  gsi::method ("StyleArrowEnd|#style_arrow_end", &gsi::style_arrow_end,
    "@brief Gets the end arrow style code for use the \\style method\n"
    "When this style is specified, an arrow is drawn pointing from the start to the end point."
  ) +
  gsi::method ("StyleArrowStart|#style_arrow_start", &gsi::style_arrow_start,
    "@brief Gets the start arrow style code for use the \\style method\n"
    "When this style is specified, an arrow is drawn pointing from the end to the start point."
  ) +
  gsi::method ("StyleArrowBoth|#style_arrow_both", &gsi::style_arrow_both,
    "@brief Gets the both arrow ends style code for use the \\style method\n"
    "When this style is specified, a two-headed arrow is drawn."
  ) +
  gsi::method ("StyleLine|#style_line", &gsi::style_line,
    "@brief Gets the line style code for use with the \\style method\n"
    "When this style is specified, plain line is drawn."
  ) +
  gsi::method ("OutlineDiag|#outline_diag", &gsi::outline_diag,
    "@brief Gets the diagonal output code for use with the \\outline method\n"
    "When this outline style is specified, a line connecting start and "
    "end points in the given style (ruler, arrow or plain line) is drawn."
  ) +
  gsi::method ("OutlineXY|#outline_xy", &gsi::outline_xy,
    "@brief Gets the xy outline code for use with the \\outline method\n"
    "When this outline style is specified, two lines are drawn: one horizontal from left "
    "to right and attached to the end of that a line from the bottom to the top. The lines "
    "are drawn in the specified style (see \\style method)."
  ) +
  gsi::method ("OutlineDiagXY|#outline_diag_xy", &gsi::outline_diag_xy,
    "@brief Gets the xy plus diagonal outline code for use with the \\outline method\n"
    "@brief outline_xy code used by the \\outline method\n"
    "When this outline style is specified, three lines are drawn: one horizontal from left "
    "to right and attached to the end of that a line from the bottom to the top. Another line "
    "is drawn connecting the start and end points directly. The lines "
    "are drawn in the specified style (see \\style method)."
  ) +
  gsi::method ("OutlineYX|#outline_yx ", &gsi::outline_yx ,
    "@brief Gets the yx outline code for use with the \\outline method\n"
    "When this outline style is specified, two lines are drawn: one vertical from bottom "
    "to top and attached to the end of that a line from the left to the right. The lines "
    "are drawn in the specified style (see \\style method)."
  ) +
  gsi::method ("OutlineDiagYX|#outline_diag_yx ", &gsi::outline_diag_yx ,
    "@brief Gets the yx plus diagonal outline code for use with the \\outline method\n"
    "When this outline style is specified, three lines are drawn: one vertical from bottom "
    "to top and attached to the end of that a line from the left to the right. Another line "
    "is drawn connecting the start and end points directly. The lines "
    "are drawn in the specified style (see \\style method)."
  ) +
  gsi::method ("OutlineBox|#outline_box", &gsi::outline_box,
    "@brief Gets the box outline code for use with the \\outline method\n"
    "When this outline style is specified, a box is drawn with the corners specified by the "
    "start and end point. All box edges are drawn in the style specified with the \\style "
    "attribute."
  ) +
  gsi::method ("AngleAny|#angle_any", &gsi::angle_any,
    "@brief Gets the any angle code for use with the \\angle_constraint method\n"
    "If this value is specified for the angle constraint, all angles will be allowed."
  ) +
  gsi::method ("AngleDiagonal|#angle_diagonal", &gsi::angle_diagonal,
    "@brief Gets the diagonal angle code for use with the \\angle_constraint method\n"
    "If this value is specified for the angle constraint, only multiples of 45 degree are allowed."
  ) +
  gsi::method ("AngleOrtho|#angle_ortho", &gsi::angle_ortho,
    "@brief Gets the ortho angle code for use with the \\angle_constraint method\n"
    "If this value is specified for the angle constraint, only multiples of 90 degree are allowed."
  ) +
  gsi::method ("AngleHorizontal|#angle_horizontal", &gsi::angle_horizontal,
    "@brief Gets the horizontal angle code for use with the \\angle_constraint method\n"
    "If this value is specified for the angle constraint, only horizontal rulers are allowed."
  ) +
  gsi::method ("AngleVertical|#angle_vertical", &gsi::angle_vertical,
    "@brief Gets the vertical angle code for use with the \\angle_constraint method\n"
    "If this value is specified for the angle constraint, only vertical rulers are allowed."
  ) +
  gsi::method ("AngleGlobal|#angle_global", &gsi::angle_global,
    "@brief Gets the global angle code for use with the \\angle_constraint method.\n"
    "This code will tell the ruler or marker to use the angle constraint defined globally."
  ) +
  gsi::method ("p1", (const db::DPoint & (ant::Object::*) () const) &ant::Object::p1,
    "@brief Gets the first point of the ruler or marker\n"
    "The points of the ruler or marker are always given in micron units in floating-point "
    "coordinates.\n"
    "@return The first point\n"
  ) +
  gsi::method ("p2", (const db::DPoint & (ant::Object::*) () const) &ant::Object::p2,
    "@brief Gets the second point of the ruler or marker\n"
    "The points of the ruler or marker are always given in micron units in floating-point "
    "coordinates.\n"
    "@return The second point\n"
  ) +
  gsi::method ("p1=", (void (ant::Object::*) (const db::DPoint &)) &ant::Object::p1,
    "@brief Sets the first point of the ruler or marker\n"
    "The points of the ruler or marker are always given in micron units in floating-point "
    "coordinates.\n"
    "@args point\n"
  ) +
  gsi::method ("p2=", (void (ant::Object::*) (const db::DPoint &)) &ant::Object::p2,
    "@brief Sets the second point of the ruler or marker\n"
    "The points of the ruler or marker are always given in micron units in floating-point "
    "coordinates.\n"
    "@args point\n"
  ) +
  gsi::method ("box", &ant::Object::box,
    "@brief Gets the bounding box of the object (not including text)\n"
    "@return The bounding box\n"
  ) +
  gsi::method ("transformed", &ant::Object::transformed<db::DTrans>,
    "@brief Transforms the ruler or marker with the given simple transformation\n"
    "@args t\n"
    "@param t The transformation to apply\n"
    "@return The transformed object\n"
  ) +
  gsi::method ("transformed_cplx", &ant::Object::transformed<db::DCplxTrans>,
    "@brief Transforms the ruler or marker with the given complex transformation\n"
    "@args t\n"
    "@param t The magnifying transformation to apply\n"
    "@return The transformed object\n"
  ) +
  gsi::method ("transformed_cplx", &ant::Object::transformed<db::ICplxTrans>,
    "@brief Transforms the ruler or marker with the given complex transformation\n"
    "@args t\n"
    "@param t The magnifying transformation to apply\n"
    "@return The transformed object (in this case an integer coordinate object)\n"
    "\n"
    "This method has been introduced in version 0.18.\n"
  ) +
  gsi::method ("fmt=", (void (ant::Object::*) (const std::string &)) &ant::Object::fmt,
    "@brief Sets the format used for the label\n"
    "@args format\n"
    "@param format The format string\n"
    "Format strings can contain placeholders for values and formulas for computing derived "
    "values. See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
    "more details."
  ) +
  gsi::method ("fmt", (const std::string & (ant::Object::*) () const) &ant::Object::fmt,
    "@brief Returns the format used for the label\n"
    "@return The format string\n"
    "Format strings can contain placeholders for values and formulas for computing derived "
    "values. See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
    "more details."
  ) +
  gsi::method ("fmt_x=", (void (ant::Object::*) (const std::string &)) &ant::Object::fmt_x,
    "@brief Sets the format used for the x-axis label\n"
    "@args format\n"
    "X-axis labels are only used for styles that have a horizontal component. "
    "@param format The format string\n"
    "Format strings can contain placeholders for values and formulas for computing derived "
    "values. See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
    "more details."
  ) +
  gsi::method ("fmt_x", (const std::string & (ant::Object::*) () const) &ant::Object::fmt_x,
    "@brief Returns the format used for the x-axis label\n"
    "@return The format string\n"
    "Format strings can contain placeholders for values and formulas for computing derived "
    "values. See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
    "more details."
  ) +
  gsi::method ("fmt_y=", (void (ant::Object::*) (const std::string &)) &ant::Object::fmt_y,
    "@brief Sets the format used for the y-axis label\n"
    "@args format\n"
    "Y-axis labels are only used for styles that have a vertical component. "
    "@param format The format string\n"
    "Format strings can contain placeholders for values and formulas for computing derived "
    "values. See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
    "more details."
  ) +
  gsi::method ("fmt_y", (const std::string & (ant::Object::*) () const) &ant::Object::fmt_y,
    "@brief Returns the format used for the y-axis label\n"
    "@return The format string\n"
    "Format strings can contain placeholders for values and formulas for computing derived "
    "values. See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
    "more details."
  ) +
  gsi::method_ext ("style=", &gsi::set_style, 
    "@brief Sets the style used for drawing the annotation object\n"
    "@args style\n"
    "The Style... values can be used for defining the annotation object's style. The style determines "
    "if ticks or arrows are drawn."
  ) +
  gsi::method_ext ("style", &gsi::get_style,
    "@brief Returns the style of the annotation object\n"
  ) +
  gsi::method_ext ("outline=", &gsi::set_outline,
    "@brief Sets the outline style used for drawing the annotation object\n"
    "@args outline\n"
    "The Outline... values can be used for defining the annotation object's outline. The "
    "outline style determines what components are drawn. "
  ) +
  gsi::method_ext ("outline", &gsi::get_outline,
    "@brief Returns the outline style of the annotation object\n"
  ) +
  gsi::method ("snap=", (void (ant::Object::*) (bool)) &ant::Object::snap,
    "@brief Sets the 'snap to objects' attribute\n"
    "@args flag\n"
    "If this attribute is set to true, the ruler or marker snaps to other objects when moved. "
  ) +
  gsi::method ("snap?", (bool (ant::Object::*) () const) &ant::Object::snap,
    "@brief Returns the 'snap to objects' attribute\n"
  ) +
  gsi::method_ext ("angle_constraint=", &gsi::set_angle_constraint,
    "@brief Sets the angle constraint attribute\n"
    "@args flag\n"
    "This attribute controls if an angle constraint is applied when moving one of the ruler's "
    "points. The Angle... values can be used for this purpose." 
  ) +
  gsi::method_ext ("angle_constraint", &gsi::get_angle_constraint,
    "@brief Returns the angle constraint attribute\n"
    "See \\angle_constraint= for a more detailed description."
  ) +
  gsi::method ("text_x", (std::string (ant::Object::*)() const) &ant::Object::text_x,
    "@brief Returns the formatted text for the x-axis label"
  ) +
  gsi::method ("text_y", (std::string (ant::Object::*)() const) &ant::Object::text_y,
    "@brief Returns the formatted text for the y-axis label"
  ) +
  gsi::method ("text", (std::string (ant::Object::*)() const) &ant::Object::text,
    "@brief Returns the formatted text for the main label"
  ) +
  gsi::method ("id", (int (ant::Object::*)() const) &ant::Object::id,
    "@brief Returns the annotation's ID"
    "\n"
    "The annotation ID is an integer that uniquely identifies an annotation inside a view.\n"
    "The ID is used for replacing an annotation (see \\LayoutView#replace_annotation).\n"
    "\n"
    "This method was introduced in version 0.24."
  ) +
  gsi::method ("to_s", &ant::Object::to_string,
    "@brief Returns the string representation of the ruler"
    "\n"
    "This method was introduced in version 0.19."
  ) +
  gsi::method ("==", &ant::Object::operator==,
    "@brief Equality operator\n"
    "@args other"
  ) +
  gsi::method ("!=", &ant::Object::operator!=,
    "@brief Inequality operator\n"
    "@args other"
  ),
  "@brief A layout annotation (i.e. ruler)\n"
  "\n"
  "Annotation objects provide a way to attach measurements or descriptive informations to a layout view. "
  "Annotation objects can appear as rulers for example. Annotation objects can be configured in "
  "different ways using the styles provided. By configuring an annotation object properly, it can appear "
  "as a rectangle or a plain line for example.\n"
  "See @<a href=\"/manual/ruler_properties.xml\">Ruler properties@</a> for "
  "more details about the appearance options.\n"
  "\n"
  "Annotations are inserted into a layout view using \\LayoutView#insert_annotation. Here is some sample code "
  "in Ruby:\n"
  "\n"
  "@code\n"
  "app = RBA::Application.instance\n"
  "mw = app.main_window\n"
  "view = mw.current_view\n"
  "\n"
  "ant = RBA::Annotation::new\n"
  "ant.p1 = RBA::DPoint::new(0, 0)\n"
  "ant.p2 = RBA::DPoint::new(100, 0)\n"
  "ant.style = RBA::Annotation::StyleRuler\n"
  "view.insert_annotation(ant)\n"
  "@/code\n"
  "\n"
  "Annotations can be retrieved from a view with \\LayoutView#each_annotation and all "
  "annotations can be cleared with \\LayoutView#clear_annotations.\n"
  "\n"
  "To change an existing annotation, employ the ID of the existing annotation and use \\LayoutView#replace_annotation "
  "to replace it with a new one. Changing the annotation's properties directly won't have an immediate effect.\n"
  "Please note that \\LayoutView#each_annotation returns a const object, so that we have to create a copy to modify them. "
  "Here is some sample code for changing the style of all annotations present in the view:\n"
  "\n"
  "@code\n"
  "view = RBA::LayoutView::current\n"
  "\n"
  "begin\n"
  "\n"
  "  view.transaction(\"Restyle annotations\")\n"
  "\n"
  "  view.each_annotation do |a|\n"
  "    adup = a.dup\n"
  "    adup.style = RBA::Annotation::StyleArrowBoth\n"
  "    view.replace_annotation(a.id, adup)\n"
  "  end\n"
  "  \n"
  "ensure\n"
  "  view.commit\n"
  "end\n"
  "@/code\n"
);

static void clear_annotations (lay::LayoutView *view)
{
  ant::Service *ant_service = view->get_plugin <ant::Service> ();
  if (ant_service) {
    ant_service->clear_rulers ();
  }
}

static void insert_annotation (lay::LayoutView *view, const ant::Object &obj)
{
  ant::Service *ant_service = view->get_plugin <ant::Service> ();
  if (ant_service) {
    ant_service->insert_ruler (obj, false /*do not observe the ruler count limit*/);
  }
}

static void erase_annotation (lay::LayoutView *view, int id)
{
  ant::Service *ant_service = view->get_plugin <ant::Service> ();
  if (ant_service) {
    for (ant::AnnotationIterator a = ant_service->begin_annotations (); ! a.at_end (); ++a) {
      if (a->id () == id) {
        ant_service->delete_ruler (a.current ());
        break;
      }
    }
  }
}

static void replace_annotation (lay::LayoutView *view, int id, const ant::Object &obj)
{
  ant::Service *ant_service = view->get_plugin <ant::Service> ();
  if (ant_service) {
    for (ant::AnnotationIterator a = ant_service->begin_annotations (); ! a.at_end (); ++a) {
      if (a->id () == id) {
        ant_service->change_ruler (a.current (), obj);
        break;
      }
    }
  }
}

static ant::AnnotationIterator begin_annotations (lay::LayoutView *view)
{
  ant::Service *ant_service = view->get_plugin <ant::Service> ();
  if (ant_service) {
    return ant_service->begin_annotations ();
  } else {
    return ant::AnnotationIterator ();
  }
}

static
gsi::ClassExt<lay::LayoutView> layout_view_decl (
  gsi::method_ext ("clear_annotations", &gsi::clear_annotations, 
    "@brief Clears all annotations on this view"
  ) +
  gsi::method_ext ("insert_annotation", &gsi::insert_annotation, 
    "@brief Inserts an annotation object into the given view\n"
    "@args obj\n"
    "Inserts a new annotation into the view. Existing annotation will remain. Use \\clear_annotations to "
    "delete them before inserting new ones. Use \\replace_annotation to replace an existing one with a new one. "
  ) +
  gsi::method_ext ("erase_annotation", &gsi::erase_annotation, 
    "@brief Erases the annotation given by the id\n"
    "@args id\n"
    "Deletes an existing annotation given by the id parameter. The id of an annotation "
    "can be obtained through \\Annotation#id.\n"
    "\n"
    "This method has been introduced in version 0.24.\n"
  ) +
  gsi::method_ext ("replace_annotation", &gsi::replace_annotation, 
    "@brief Replaces the annotation given by the id with the new one\n"
    "@args id, obj\n"
    "Replaces an existing annotation given by the id parameter with the new one. The id of an annotation "
    "can be obtained through \\Annotation#id.\n"
    "\n"
    "This method has been introduced in version 0.24.\n"
  ) +
  gsi::iterator_ext ("each_annotation", &gsi::begin_annotations, 
    "@brief Iterates over all annotations attached to this view"
  ),
  ""
);

class AnnotationSelectionIterator 
{
public:
  typedef ant::Object value_type;
  typedef std::map<ant::Service::obj_iterator, unsigned int>::const_iterator iterator_type;
  typedef void pointer; 
  typedef const value_type &reference;
  typedef std::forward_iterator_tag iterator_category;
  typedef void difference_type;

  AnnotationSelectionIterator (const std::vector<ant::Service *> &services) 
    : m_services (services), m_service (0)
  {
    if (! m_services.empty ()) {
      m_iter = m_services [m_service]->selection ().begin ();
      next ();
    }
  }

  bool at_end () const
  {
    return (m_service >= m_services.size ());
  }

  AnnotationSelectionIterator &operator++ ()
  {
    ++m_iter;
    next ();
    return *this;
  }

  const value_type &operator* () const
  {
    return *(dynamic_cast<const ant::Object *> (m_iter->first->ptr ()));
  }

private:
  std::vector<ant::Service *> m_services;
  unsigned int m_service;
  iterator_type m_iter;

  void next ()
  {
    while (m_iter == m_services [m_service]->selection ().end ()) {
      ++m_service;
      if (m_service < m_services.size ()) {
        m_iter = m_services [m_service]->selection ().begin ();
      } else {
        break;
      }
    }
  }
};

//  extend the layout view by "edtService" specific methods 

static bool has_annotation_selection (const lay::LayoutView *view)
{
  std::vector<ant::Service *> ant_services = view->get_plugins <ant::Service> ();
  for (std::vector<ant::Service *>::const_iterator s = ant_services.begin (); s != ant_services.end (); ++s) {
    if ((*s)->selection_size () > 0) {
      return true;
    }
  }
  return false;
}

static AnnotationSelectionIterator begin_annotations_selected (const lay::LayoutView *view)
{
  return AnnotationSelectionIterator (view->get_plugins <ant::Service> ());
}


static
gsi::ClassExt<lay::LayoutView> layout_view_decl2 (
  gsi::method_ext ("has_annotation_selection?", &has_annotation_selection, 
    "@brief Returns true, if annotations (rulers) are selected in this view"
    "\n"
    "This method was introduced in version 0.19."
  ) +
  gsi::iterator_ext ("each_annotation_selected", &begin_annotations_selected,
    "@brief Iterate over each selected annotation objects, yielding a \\Annotation object for each of them"
    "\n"
    "This method was introduced in version 0.19."
  ),
  ""
);

}

