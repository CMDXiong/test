
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
//  box binding

template <class C>
struct box_defs 
{
  typedef typename C::coord_type coord_type;
  typedef typename C::point_type point_type;
  typedef db::simple_trans<coord_type> simple_trans_type;
  typedef db::complex_trans<coord_type, double> complex_trans_type;

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

  static C *new_lbrt (coord_type l, coord_type b, coord_type r, coord_type t)
  {
    return new C (l, b, r, t);
  }

  static C *new_pp (const point_type &ll, const point_type &ur)
  {
    return new C (ll, ur);
  }

  static C join_with_point (const C *box, const point_type &p)
  {
    C b (*box);
    b += p;
    return b;
  }

  static bool contains (const C *box, coord_type x, coord_type y)
  {
    return box->contains (point_type (x, y));
  }

  static C &enlarge (C *box, coord_type x, coord_type y)
  {
    return box->enlarge (point_type (x, y));
  }

  static C enlarged (const C *box, coord_type x, coord_type y)
  {
    return box->enlarged (point_type (x, y));
  }

  static C &move (C *box, coord_type x, coord_type y)
  {
    return box->move (point_type (x, y));
  }

  static C moved (const C *box, coord_type x, coord_type y)
  {
    return box->moved (point_type (x, y));
  }

  static gsi::Methods methods ()
  {
    return
    constructor ("new", &new_v, 
      "@brief Default constructor: creates an empty (invalid) box"
    ) +
    constructor ("new|#new_lbrt", &new_lbrt, 
      "@brief Constructor with four coordinates\n"
      "\n"
      "@args left, bottom, right, top\n"
      "\n"
      "Four coordinates are given to create a new box. If the coordinates "
      "are not provided in the correct order (i.e. right < left), these are "
      "swapped."
    ) +
    constructor ("new|#new_pp", &new_pp, 
      "@brief Constructor with two points\n"
      "\n"
      "@args lower_left, upper_right\n"
      "\n"
      "Two points are given to create a new box.  If the coordinates "
      "are not provided in the correct order (i.e. right < left), these are "
      "swapped."
    ) +
    method ("p1", &C::p1,
      "@brief The lower left point of the box\n"
    ) +
    method ("p2", &C::p2,
      "@brief The upper right point of the box\n"
    ) +
    method ("center", &C::center,
      "@brief The center of the box\n"
    ) +
    method ("left", &C::left,
      "@brief The left coordinate of the box\n"
    ) +
    method ("right", &C::right,
      "@brief The right coordinate of the box\n"
    ) +
    method ("bottom", &C::bottom,
      "@brief The bottom coordinate of the box\n"
    ) +
    method ("top", &C::top,
      "@brief The top coordinate of the box\n"
    ) +
    method ("width", &C::width,
      "@brief The width of the box\n"
    ) +
    method ("height", &C::height,
      "@brief The height of the box\n"
    ) +
    method ("left=", &C::set_left,
      "@brief Set the left coordinate of the box\n"
      "@args c\n"
    ) +
    method ("right=", &C::set_right,
      "@brief Set the right coordinate of the box\n"
      "@args c\n"
    ) +
    method ("bottom=", &C::set_bottom,
      "@brief Set the bottom coordinate of the box\n"
      "@args c\n"
    ) +
    method ("top=", &C::set_top,
      "@brief Set the top coordinate of the box\n"
      "@args c\n"
    ) +
    method ("p1=", &C::set_p1,
      "@brief Set the lower left point of the box\n"
      "@args p\n"
    ) +
    method ("p2=", &C::set_p2,
      "@brief Set the upper right point of the box\n"
      "@args p\n"
    ) +
    method_ext ("contains?", &box_defs<C>::contains,
      "@brief Returns true if the box contains the given point\n"
      "\n"
      "@args x, y"
      "\n"
      "Tests whether a point (x, y) is inside the box.\n"
      "It also returns true if the point is exactly on the box contour.\n"
      "\n"
      "@return true if the point is inside the box.\n"
    ) +
    method ("contains?", &C::contains,
      "@brief Returns true if the box contains the given point\n"
      "\n"
      "@args point"
      "\n"
      "Tests whether a point is inside the box.\n"
      "It also returns true if the point is exactly on the box contour.\n"
      "\n"
      "@param p The point to test against.\n"
      "\n"
      "@return true if the point is inside the box.\n"
    ) +
    method ("empty?", &C::empty,
      "@brief Empty predicate\n"
      "\n"
      "An empty box may be created with the default constructor for example. "
      "Such a box is neutral when combining it with other boxes and renders empty boxes "
      "if used in box intersections and false in geometrical relationship tests. "
    ) +
    method ("inside?", &C::inside,
      "@brief Test if this box is inside the argument box\n"
      "\n"
      "@args box\n"
      "\n"
      "Returns true, if this box is inside the given box, i.e. the box intersection renders this box"
    ) +
    method ("touches?", &C::touches,
      "@brief Test if this box touches the argument box\n"
      "\n"
      "@args box\n"
      "\n"
      "Returns true, if this box has at least one point common with the argument box"
    ) +
    method ("overlaps?", &C::overlaps,
      "@brief Test if this box overlaps the argument box\n"
      "\n"
      "@args box\n"
      "\n"
      "Returns true, if the intersection box of this box with the argument box exists and has a non-vanishing area"
    ) +
    method ("area", &C::double_area,
      "@brief Compute the box area\n"
      "\n"
      "Returns the box area or 0 if the box is empty"
    ) +
    method ("is_point?", &C::is_point,
      "@brief Return true, if the box is a single point\n"
    ) +
    method ("perimeter", &C::perimeter,
      "@brief Returns the perimeter of the box\n"
      "\n"
      "This method is equivalent to 2*(width+height). For empty boxes, this method returns 0.\n"
      "\n"
      "This method has been introduced in version 0.23."
    ) + 
    method_ext ("+", &box_defs<C>::join_with_point,
      "@brief Join box with a point\n"
      "\n"
      "@args point\n"
      "\n"
      "The + operator joins a point with the box. The resulting box will enclose both the original "
      "box and the point.\n"
      "\n"
      "@param point The point to join with this box.\n"
      "\n"
      "@return The box joined with the point\n"
    ) +
    method ("+", &C::joined,
      "@brief Joining of boxes\n"
      "\n"
      "@args box\n"
      "\n"
      "The + operator joins the first box with the one given as \n"
      "the second argument. Joining constructs a box that encloses\n"
      "both boxes given. Empty boxes are neutral: they do not\n"
      "change another box when joining. Overwrites this box\n"
      "with the result.\n"
      "\n"
      "@param box The box to join with this box.\n"
      "\n"
      "@return The joined box\n"
    ) +
    method ("&", &C::intersection,
      "@brief Intersection of boxes\n"
      "\n"
      "@args box\n"
      "\n"
      "The intersection of two boxes is the largest\n"
      "box common to both boxes. The intersection may be \n"
      "empty if both boxes to not touch. If the boxes do\n"
      "not overlap but touch the result may be a single\n"
      "line or point with an area of zero. Overwrites this box\n"
      "with the result.\n"
      "\n"
      "@param box The box to take the intersection with\n"
      "\n"
      "@return The intersection box\n"
    ) +
    method ("*", &C::convolved,
      "@brief Convolve boxes\n"
      "\n"
      "@args box\n"
      "\n"
      "The * operator convolves the firstbox with the one given as \n"
      "the second argument. The box resulting from \"convolution\" is the\n"
      "outer boundary of the union set formed by placing \n"
      "the second box at every point of the first. In other words,\n"
      "the returned box of (p1,p2)*(q1,q2) is (p1+q1,p2+q2).\n"
      "\n"
      "@param box The box to convolve with this box.\n"
      "\n"
      "@return The convolved box\n"
    ) +
    method ("*", &C::scaled,
      "@brief Scale box\n"
      "\n"
      "@args scale_factor\n"
      "\n"
      "The * operator scales the box with the given factor and returns the result.\n"
      "\n"
      "This method has been introduced in version 0.22.\n"
      "\n"
      "@param scale_factor The scaling factor\n"
      "\n"
      "@return The scaled box\n"
    ) +
    method_ext ("move", &box_defs<C>::move,
      "@brief Moves the box by a certain distance\n"
      "\n"
      "@args dx, dy\n"
      "\n"
      "This is a convenience method which takes two values instead of a Point object.\n"
      "This method has been introduced in version 0.23.\n"
      "\n"
      "@return A reference to this box.\n"
    ) +
    method_ext ("moved", &box_defs<C>::moved,
      "@brief Moves the box by a certain distance\n"
      "\n"
      "@args dx, dy\n"
      "\n"
      "This is a convenience method which takes two values instead of a Point object.\n"
      "This method has been introduced in version 0.23.\n"
      "\n"
      "@return The enlarged box.\n"
    ) +
    method ("move", &C::move,
      "@brief Moves the box by a certain distance\n"
      "\n"
      "@args distance\n"
      "\n"
      "Moves the box by a given offset and returns the moved\n"
      "box. Does not check for coordinate overflows.\n"
      "\n"
      "@param distance The offset to move the box.\n"
      "\n"
      "@return A reference to this box.\n"
    ) +
    method ("moved", &C::moved,
      "@brief Returns the box moved by a certain distance\n"
      "\n"
      "@args distance\n"
      "\n"
      "Moves the box by a given offset and returns the moved\n"
      "box. Does not modify this box. Does not check for coordinate\n"
      "overflows.\n"
      "\n"
      "@param distance The offset to move the box.\n"
      "\n"
      "@return The moved box.\n"
    ) +
    method_ext ("enlarge", &box_defs<C>::enlarge,
      "@brief Enlarges the box by a certain amount.\n"
      "\n"
      "@args dx, dy\n"
      "\n"
      "This is a convenience method which takes two values instead of a Point object.\n"
      "This method has been introduced in version 0.23.\n"
      "\n"
      "@return A reference to this box.\n"
    ) +
    method_ext ("enlarged", &box_defs<C>::enlarged,
      "@brief Enlarges the box by a certain amount.\n"
      "\n"
      "@args dx, dy\n"
      "\n"
      "This is a convenience method which takes two values instead of a Point object.\n"
      "This method has been introduced in version 0.23.\n"
      "\n"
      "@return The enlarged box.\n"
    ) +
    method ("enlarge", &C::enlarge,
      "@brief Enlarges the box by a certain amount.\n"
      "\n"
      "@args enlargement\n"
      "\n"
      "Enlarges the box by x and y value specified in the vector\n"
      "passed. Positive values with grow the box, negative ones\n"
      "will shrink the box. The result may be an empty box if the\n"
      "box disappears. The amount specifies the grow or shrink\n"
      "per edge. The width and height will change by twice the\n"
      "amount.\n"
      "Does not check for coordinate\n"
      "overflows.\n"
      "\n"
      "@param enlargement The grow or shrink amount in x and y direction\n"
      "\n"
      "@return A reference to this box.\n"
    ) +
    method ("enlarged", &C::enlarged,
      "@brief Enlarges the box by a certain amount.\n"
      "\n"
      "@args enlargement\n"
      "\n"
      "Enlarges the box by x and y value specified in the vector\n"
      "passed. Positive values with grow the box, negative ones\n"
      "will shrink the box. The result may be an empty box if the\n"
      "box disappears. The amount specifies the grow or shrink\n"
      "per edge. The width and height will change by twice the\n"
      "amount.\n"
      "Does not modify this box. Does not check for coordinate\n"
      "overflows.\n"
      "\n"
      "@param enlargement The grow or shrink amount in x and y direction\n"
      "\n"
      "@return The enlarged box.\n"
    ) +
    method ("transformed", &C::template transformed<simple_trans_type>,
      "@brief Transform the box with the given simple transformation\n"
      "\n"
      "@args t\n"
      "\n"
      "@param t The transformation to apply\n"
      "@return The transformed box\n"
    ) +
    method ("transformed|#transformed_cplx", &C::template transformed<complex_trans_type>,
      "@brief Transform the box with the given complex transformation\n"
      "\n"
      "@args t\n"
      "\n"
      "@param t The magnifying transformation to apply\n"
      "@return The transformed box (a DBox now)\n"
    ) +
    method ("<", &C::operator<,
      "@brief Less operator\n"
      "@args box\n"
      "Return true, if this box is 'less' with respect to first and second point (in this order)"
    ) +
    method ("==", &C::operator==,
      "@brief Equality\n"
      "@args box\n"
      "Return true, if this box and the given box are equal "
    ) +
    method ("!=", &C::operator!=,
      "@brief Inequality\n"
      "@args box\n"
      "Return true, if this box and the given box are not equal "
    ) +
    constructor ("from_s", &from_string,
      "@brief Creates an object from a string\n"
      "@args s\n"
      "Creates the object from a string representation (as returned by \\to_s)\n"
      "\n"
      "This method has been added in version 0.23.\n"
    ) +
    method ("to_s", (std::string (C::*) () const) &C::to_string,
      "@brief Convert to a string\n"
    );
  }
};

static db::Box box_from_dbox (const db::DBox &b)
{
  return db::Box::from_double (b);
}

Class<db::Box> decl_Box ("Box", 
  method ("from_dbox", &box_from_dbox, 
    "@brief Construct an integer box from a floating-point coordinate box\n"
    "\n"
    "@args double_box\n"
    "\n"
    "Create a integer coordinate box from a floating-point coordinate box"
  ) +
  method ("transformed|#transformed_cplx", &db::Box::transformed<db::ICplxTrans>,
    "@brief Transform the box with the given complex transformation\n"
    "\n"
    "@args t\n"
    "\n"
    "@param t The magnifying transformation to apply\n"
    "@return The transformed box (in this case an integer coordinate box)\n"
    "\n"
    "This method has been introduced in version 0.18.\n"
  ) +
  box_defs<db::Box>::methods (),
  "@brief A box class with floating-point coordinates\n"
  "\n"
  "This object represents a box (a rectangular shape).\n"
  "\n"
  "The definition of the attributes is: p1 is the lower left point, p2 the \n"
  "upper right one. If a box is constructed from two points (or four coordinates), the \n"
  "coordinates are sorted accordingly.\n"
  "\n"
  "A box can be empty. An empty box represents no area\n"
  "(not even a point). Empty boxes behave neutral with respect to most operations. \n"
  "Empty boxes return true on \\empty?.\n"
  "\n"
  "A box can be a point or a single\n"
  "line. In this case, the area is zero but the box still\n"
  "can overlap other boxes for example and it is not empty. \n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

static db::DBox dbox_from_ibox (const db::Box &b)
{
  return db::DBox (b);
}

Class<db::DBox> decl_DBox ("DBox", 
  method ("from_ibox", &dbox_from_ibox, 
    "@brief Construct a floating-point coordinate box from an integer coordinate box\n"
    "\n"
    "@args int_box\n"
    "\n"
    "Create a floating-point coordinate box from an integer coordinate box"
  ) +
  box_defs<db::DBox>::methods (),
  "@brief A box class\n"
  "\n"
  "This object represents a box (a rectangular shape).\n"
  "\n"
  "The definition of the attributes is: p1 is the lower left point, p2 the \n"
  "upper right one. If a box is constructed from two points (or four coordinates), the \n"
  "coordinates are sorted accordingly.\n"
  "\n"
  "A box can be empty. An empty box represents no area\n"
  "(not even a point). Empty boxes behave neutral with respect to most operations. \n"
  "Empty boxes return true on \\empty?.\n"
  "\n"
  "A box can be a point or a single\n"
  "line. In this case, the area is zero but the box still\n"
  "can overlap other boxes for example and it is not empty. \n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

}
