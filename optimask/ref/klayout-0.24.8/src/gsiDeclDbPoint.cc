
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
#include "dbPoint.h"
#include "dbBox.h"

namespace gsi
{

// ---------------------------------------------------------------
//  point binding

template <class C>
struct point_defs 
{
  typedef typename C::coord_type coord_type;

  static C *from_string (const char *s)
  {
    tl::Extractor ex (s);
    std::auto_ptr<C> c (new C ());
    ex.read (*c.get ());
    return c.release ();
  }

  static C *new_v ()
  {
    return new C ();
  }

  static C *new_xy (coord_type x, coord_type y)
  {
    return new C (x, y);
  }

  static C scale (const C *p, double s)
  {
    return C (*p * s);
  }

  static C negate (const C *p)
  {
    return -*p;
  }

  static gsi::Methods methods ()
  {
    return
    constructor ("new", &new_v,
      "@brief Default constructor: creates a point at 0,0"
    ) +
    constructor ("new|#new_xy", &new_xy,
      "@brief Constructor for a point from two coordinate values\n"
      "\n"
      "@args x, y\n"
    ) +
    method_ext ("-@", &negate,
      "@brief Compute the negative of a point\n"
      "\n"
      "@args p\n"
      "\n"
      "Returns a new point with -x, -y.\n"
      "\n"
      "This method has been added in version 0.23."
    ) +
    method ("+", (C (C::*) (const C &) const) &C::add,
      "@brief Add one point to another\n"
      "\n"
      "@args p\n"
      "\n"
      "Add point p to self by adding the coordinates"
    ) +
    method ("-", (C (C::*) (const C &) const) &C::subtract,
      "@brief Subtract one point from another\n"
      "\n"
      "@args p\n"
      "\n"
      "Subtract point p from self by subtracting the coordinates"
    ) +
    method ("<", &C::operator<,
      "@brief \"less\" comparison operator\n"
      "\n"
      "@args p\n"
      "\n"
      "This operator is provided to establish a sorting\n"
      "order\n"
    ) +
    method ("==", &C::operator==,
      "@brief Equality test operator\n"
      "\n"
      "@args p\n"
    ) +
    method ("!=", &C::operator!=,
      "@brief Inequality test operator\n"
      "\n"
      "@args p\n"
    ) +
    method ("x", &C::x,
      "@brief Accessor to the x coordinate\n"
    ) +
    method ("y", &C::y,
      "@brief Accessor to the y coordinate\n"
    ) +
    method ("x=", &C::set_x,
      "@brief Write accessor to the x coordinate\n"
      "@args coord\n"
    ) +
    method ("y=", &C::set_y,
      "@brief Write accessor to the y coordinate\n"
      "@args coord\n"
    ) +
    method_ext ("*", &scale,
      "@brief Scaling by some factor\n"
      "\n"
      "@args f\n"
      "\n"
      "Returns the scaled object. All coordinates are multiplied with the given factor and if "
      "necessary rounded."
    ) +
    method ("distance", (double (C::*) (const C &) const) &C::double_distance,
      "@brief The Euclidian distance to another point\n"
      "\n"
      "@args d\n"
      "\n"
      "@param d The other point to compute the distance to.\n"
    ) +
    method ("sq_distance", (double (C::*) (const C &) const) &C::sq_double_distance,
      "@brief The square Euclidian distance to another point\n"
      "\n"
      "@args d\n"
      "\n"
      "@param d The other point to compute the distance to.\n"
    ) +
    method ("abs", (double (C::*) () const) &C::double_distance,
      "@brief The absolute value of the point (Euclidian distance to 0,0)\n"
      "\n"
      "The returned value is 'sqrt(x*x+y*y)'.\n"
      "\n"
      "This method has been introduced in version 0.23."
    ) +
    method ("sq_abs", (double (C::*) () const) &C::sq_double_distance,
      "@brief The square of the absolute value of the point (Euclidian distance to 0,0)\n"
      "\n"
      "The returned value is 'x*x+y*y'.\n"
      "\n"
      "This method has been introduced in version 0.23."
    ) +
    constructor ("from_s", &from_string,
      "@brief Creates an object from a string\n"
      "@args s\n"
      "Creates the object from a string representation (as returned by \\to_s)\n"
      "\n"
      "This method has been added in version 0.23.\n"
    ) +
    method ("to_s", (std::string (C::*) () const) &C::to_string,
      "@brief String conversion\n"
    );
  }

};

static db::DPoint dpoint_from_ipoint (const db::Point &p)
{
  return db::DPoint (p);
}

Class<db::DPoint> decl_DPoint ("DPoint", 
  method ("from_ipoint", &dpoint_from_ipoint,
    "@brief Create a floating-point coordinate point from an integer coordinate point\n"
    "\n"
    "@args p\n"
  ) +
  point_defs<db::DPoint>::methods (),
  "@brief A point class with double (floating-point) coordinates\n"
  "Points represent a coordinate in the two-dimensional coordinate space of layout. "
  "They are not geometrical objects by itself. But they are frequently used in the database API "
  "for various purposes. Other than the integer variant (\\Point), points with floating-point coordinates can represent fractions of "
  "a database unit.\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

static db::Point point_from_dpoint (const db::DPoint &p)
{
  return db::Point::from_double (p);
}

Class<db::Point> decl_Point ("Point", 
  method ("from_dpoint", &point_from_dpoint,
    "@brief Create a double point from an integer point\n"
    "\n"
    "@args p\n"
    "\n"
    "Creates an double point from an integer point p" 
  ) +
  point_defs<db::Point>::methods (),
  "@brief An integer point class\n"
  "Points represent a coordinate in the two-dimensional coordinate space of layout. "
  "They are not geometrical objects by itself. But they are frequently used in the database API "
  "for various purposes.\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

}

