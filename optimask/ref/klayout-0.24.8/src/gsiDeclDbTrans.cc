
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
//  simple_trans binding

template <class C>
struct trans_defs 
{
  typedef typename C::coord_type coord_type;
  typedef typename C::displacement_type displacement_type;
  typedef typename db::point<coord_type> point_type;

  static int trans_r0 ()   { return db::fixpoint_trans<C>::r0;   }
  static int trans_r90 ()  { return db::fixpoint_trans<C>::r90;  }
  static int trans_r180 () { return db::fixpoint_trans<C>::r180; }
  static int trans_r270 () { return db::fixpoint_trans<C>::r270; }
  static int trans_m0 ()   { return db::fixpoint_trans<C>::m0;   }
  static int trans_m45 ()  { return db::fixpoint_trans<C>::m45;  }
  static int trans_m90 ()  { return db::fixpoint_trans<C>::m90;  }
  static int trans_m135 () { return db::fixpoint_trans<C>::m135; }

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

  static C *new_f (int f)
  {
    return new C (db::fixpoint_trans<C> (f));
  }

  static C *new_xy (coord_type x, coord_type y)
  {
    return new C (displacement_type (x, y));
  }

  static C *new_fxy (int f, coord_type x, coord_type y)
  {
    return new C (f, displacement_type (x, y));
  }

  static C *new_rmxy (int r, bool m, coord_type x, coord_type y)
  {
    return new C (r, m, displacement_type (x, y));
  }

  static C *new_u (const displacement_type &u)
  {
    return new C (u);
  }

  static C *new_fu (int f, const displacement_type &u)
  {
    return new C (f, u);
  }

  static C *new_rmu (int r, bool m, displacement_type &u)
  {
    return new C (r, m, u);
  }

  static void set_angle (C *trans, int angle)
  {
    *trans = C (angle, trans->is_mirror (), trans->disp ());
  }

  static void set_rot (C *trans, int rot)
  {
    *trans = C (rot, trans->disp ());
  }

  static void set_mirror (C *trans, bool mirror)
  {
    *trans = C (trans->angle (), mirror, trans->disp ());
  }

  static gsi::Methods methods ()
  {
    return
    constructor ("new", &new_v, 
      "@brief Creates a unit transformation\n"
    ) +
    constructor ("new|#new_f", &new_f, 
      "@brief Conversion constructor from a fixpoint transformation\n"
      "\n"
      "@args f\n"
      "\n"
      "This constructor will create a transformation with a fixpoint transformation\n"
      "but no displacement.\n"
      "\n"
      "@param f The rotation/mirror code (R0 .. M135 constants)\n"
    ) +
    constructor ("new|#new_rmu", &new_rmu,
      "@brief The standard constructor using angle and mirror flag\n"
      "\n"
      "@args rot, mirr, u\n"
      "\n"
      "The sequence of operations is: mirroring at x axis,\n"
      "rotation, application of displacement.\n"
      "\n"
      "@param rot The rotation in units of 90 degree\n"
      "@param mirrx True, if mirrored at x axis\n"
      "@param u The displacement\n"
    ) +
    constructor ("new|#new_rmxy", &new_rmxy,
      "@brief The standard constructor using angle and mirror flag and two coordinate values for displacement\n"
      "\n"
      "@args rot, mirr, x, y\n"
      "\n"
      "The sequence of operations is: mirroring at x axis,\n"
      "rotation, application of displacement.\n"
      "\n"
      "@param rot The rotation in units of 90 degree\n"
      "@param mirrx True, if mirrored at x axis\n"
      "@param x The horizontal displacement\n"
      "@param y The vertical displacement\n"
    ) +
    constructor ("new|#new_fu", &new_fu, 
      "@brief The standard constructor using a code rather than angle and mirror\n"
      "\n"
      "@args f, u\n"
      "\n"
      "@param f The rotation/mirror code (R0 .. M135 constants)\n"
      "@param u The displacement\n"
    ) +
    constructor ("new|#new_fxy", &new_fxy, 
      "@brief The standard constructor using a code rather than angle and mirror and two coordinate values for displacement\n"
      "\n"
      "@args f, x, y\n"
      "\n"
      "@param f The rotation/mirror code (R0 .. M135 constants)\n"
      "@param x The horizontal displacement\n"
      "@param y The vertical displacement\n"
    ) +
    constructor ("new|#new_u", &new_u,
      "@brief The standard constructor using a displacement only\n"
      "\n"
      "@args u\n"
      "\n"
      "@param u The displacement\n"
    ) +
    constructor ("new|#new_xy", &new_xy,
      "@brief The standard constructor using a displacement given as two coordinates\n"
      "\n"
      "@args x, y\n"
      "\n"
      "@param x The horizontal displacement\n"
      "@param y The vertical displacement\n"
    ) +
    method ("inverted", &C::inverted, 
      "@brief Inversion\n"
      "\n"
      "Returns the inverted transformation\n"
      "\n"
      "@return The inverted transformation\n"
    ) +
    method ("invert", &C::invert,
      "@brief In-place inversion\n"
      "\n"
      "Inverts the transformation and replaces this object by the\n"
      "inverted one.\n"
      "\n"
      "@return The inverted transformation\n"
    ) +
    method ("ctrans", &C::template ctrans<coord_type>,
      "@brief The transformation of a distance\n"
      "\n"
      "@args d\n"
      "\n"
      "The \"ctrans\" method transforms the given distance.\n"
      "e = t(d). For the simple transformations, there\n"
      "is no magnification and no modification of the distance\n"
      "therefore.\n"
      "\n"
      "@param d The distance to transform\n"
      "@return The transformed distance\n"
    ) +
    method ("trans", (point_type (C::*) (const point_type &) const) &C::template trans<coord_type>, 
      "@brief The transformation of a point \n"
      "\n"
      "@args p\n"
      "\n"
      "The \"trans\" method transforms the given point.\n"
      "q = t(p)\n"
      "\n"
      "@param p The point to transform\n"
      "@return The transformed point\n"
    ) +
    method ("*", &C::concat,
      "@brief Multiplication (concatenation) of transformations\n"
      "\n"
      "@args *\n"
      "\n"
      "The * operator returns self*t (\"t is applied before this transformation\").\n"
      "\n"
      "@param t The transformation to apply before\n"
      "@return The modified transformation\n"
    ) +
    method ("<", &C::operator<,
      "@brief A sorting criterion\n"
      "@args other\n"
      "This method is provided to implement a sorting order. The definition of 'less' is opaque and might change in "
      "future versions."
    ) +
    method ("==", &C::operator==,
      "@brief Equality test\n"
      "@args other\n"
    ) +
    method ("!=", &C::operator!=,
      "@brief Inequality test\n"
      "@args other\n"
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
    ) +
    method ("disp", (const point_type &(C::*) () const) &C::disp,
      "@brief Accessor to the point\n"
    ) +
    method ("rot", &C::rot,
      "@brief Gets the angle/mirror code\n"
      "\n"
      "The angle/mirror code is one of the constants R0, R90, R180, R270, M0, M45, M90 and M135. "
      "rx is the rotation by an angle of x counter clockwise. mx is the mirroring at the axis given "
      "by the angle x (to the x-axis). "
    ) +
    method ("is_mirror?", &C::is_mirror,
      "@brief Gets the mirror flag\n"
      "\n"
      "If this property is true, the transformation is composed of a mirroring at the x-axis followed by a rotation "
      "by the angle given by the \\angle property. "
    ) +
    method ("angle", &C::angle,
      "@brief Gets the angle in units of 90 degree\n"
      "\n"
      "This value delivers the rotation component. In addition, a mirroring at the x axis may be applied before "
      "if the \\is_mirror? property is true. "
    ) +
    method_ext ("angle=", &set_angle,
      "@brief Sets the angle in units of 90 degree\n"
      "@args a\n"
      "@param a The new angle\n"
      "\n"
      "This method was introduced in version 0.20.\n"
    ) +
    method ("disp=", (void (C::*) (const point_type &)) &C::disp,
      "@brief Sets the displacement\n"
      "@args u\n"
      "@param u The new displacement\n"
      "\n"
      "This method was introduced in version 0.20.\n"
    ) +
    method_ext ("mirror=", &set_mirror,
      "@brief Sets the mirror flag\n"
      "@args m\n"
      "\"mirroring\" describes a reflection at the x-axis which is included in the transformation prior to rotation."
      "@param m The new mirror flag\n"
      "\n"
      "This method was introduced in version 0.20.\n"
    ) +
    method_ext ("rot=", &set_rot,
      "@brief Sets the angle/mirror code\n"
      "@args r\n"
      "@param r The new angle/rotation code (see \\rot property)\n"
      "\n"
      "This method was introduced in version 0.20.\n"
    ) +
    method ("R0|#r0", &trans_r0,
      "@brief Angle/mirror code for the \"unrotated\" transformation\n"
    ) +
    method ("R90|#r90", &trans_r90,
      "@brief Angle/mirror code for the \"rotated by 90 degree counterclockwise\" transformation\n"
    ) +
    method ("R180|#r180", &trans_r180,
      "@brief Angle/mirror code for the \"rotated by 180 degree counterclockwise\" transformation\n"
    ) +
    method ("R270|#r270", &trans_r270,
      "@brief Angle/mirror code for the \"rotated by 270 degree counterclockwise\" transformation\n"
    ) +
    method ("M0|#m0", &trans_m0,
      "@brief Angle/mirror code for the \"mirrored at the x-axis\" transformation\n"
    ) +
    method ("M45|#m45", &trans_m45,
      "@brief Angle/mirror code for the \"mirrored at the 45 degree axis\" transformation\n"
    ) +
    method ("M90|#m90", &trans_m90,
      "@brief Angle/mirror code for the \"mirrored at the y (90 degree) axis\" transformation\n"
    ) +
    method ("M135|#m135", &trans_m135,
      "@brief Angle/mirror code for the \"mirrored at the 135 degree axis\" transformation\n"
    );
  }
};

db::Trans trans_from_dtrans (const db::DTrans &dtrans)
{
  return db::Trans (dtrans.rot (), db::Point::from_double (dtrans.disp ()));
}

Class<db::Trans> decl_Trans ("Trans", 
  method ("from_dtrans", &trans_from_dtrans, 
    "@brief Conversion constructor from a floating-point transformation\n"
    "\n"
    "@args double_trans\n"
  ) +
  trans_defs<db::Trans>::methods (),
  "@brief A simple transformation\n"
  "\n"
  "Simple transformations only provide rotations about angles which a multiples of 90 degree.\n"
  "Together with the mirror options, this results in 8 distinct orientations (fixpoint transformations).\n"
  "These can be combined with a displacement which is applied after the rotation/mirror.\n"
  "This version acts on integer coordinates. A version for floating-point coordinates is \\DTrans.\n"
  "\n"
  "Here are some examples for using the Trans class:\n"
  "\n"
  "@code\n"
  "t = RBA::Trans::new(0, 100)  # displacement by 100 DBU in y direction\n"
  "# the inverse: -> \"r0 0,-100\"\n"
  "t.inverted.to_s\n"
  "# concatenation: -> \"r90 -100,0\"\n"
  "(RBA::Trans::new(RBA::Trans::R90) * t).to_s\n"
  "# apply to a point: -> \"0,100\"\n"
  "RBA::Trans::new(RBA::Trans::R90).trans(RBA::Point::new(100, 0))\n"
  "@/code\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

db::DTrans dtrans_from_itrans (const db::Trans &itrans)
{
  return db::DTrans (itrans);
}

Class<db::DTrans> decl_DTrans ("DTrans", 
  method ("from_itrans", &dtrans_from_itrans, 
    "@brief Conversion constructor from an integer coordinate transformation\n"
    "\n"
    "@args int_trans\n"
  ) +
  trans_defs<db::DTrans>::methods (),
  "@brief A simple transformation\n"
  "\n"
  "Simple transformations only provide rotations about angles which a multiples of 90 degree.\n"
  "Together with the mirror options, this results in 8 distinct orientations (fixpoint transformations).\n"
  "These can be combined with a displacement which is applied after the rotation/mirror.\n"
  "This version acts on floating-point coordinates. A version for integer coordinates is \\Trans.\n"
  "\n"
  "Here are some examples for using the DTrans class:\n"
  "\n"
  "@code\n"
  "t = RBA::DTrans::new(0, 100)  # displacement by 100 DBU in y direction\n"
  "# the inverse: -> \"r0 0,-100\"\n"
  "t.inverted.to_s\n"
  "# concatenation: -> \"r90 -100,0\"\n"
  "(RBA::DTrans::new(RBA::DTrans::R90) * t).to_s\n"
  "# apply to a point: -> \"0,100\"\n"
  "RBA::DTrans::new(RBA::DTrans::R90).trans(RBA::DPoint::new(100, 0))\n"
  "@/code\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

// ---------------------------------------------------------------
//  complex_trans binding

template <class C>
struct cplx_trans_defs 
{
  typedef typename C::coord_type coord_type;
  typedef typename C::target_coord_type target_coord_type;
  typedef typename C::displacement_type displacement_type;
  typedef db::simple_trans<coord_type> simple_trans_type;
  typedef db::point<coord_type> point_type;
  typedef db::point<target_coord_type> target_point_type;

  static int trans_r0 ()   { return db::fixpoint_trans<C>::r0;   }
  static int trans_r90 ()  { return db::fixpoint_trans<C>::r90;  }
  static int trans_r180 () { return db::fixpoint_trans<C>::r180; }
  static int trans_r270 () { return db::fixpoint_trans<C>::r270; }
  static int trans_m0 ()   { return db::fixpoint_trans<C>::m0;   }
  static int trans_m45 ()  { return db::fixpoint_trans<C>::m45;  }
  static int trans_m90 ()  { return db::fixpoint_trans<C>::m90;  }
  static int trans_m135 () { return db::fixpoint_trans<C>::m135; }

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

  static C *new_f (int f)
  {
    return new C (f);
  }

  static C *new_xy (target_coord_type x, target_coord_type y)
  {
    return new C (displacement_type (x, y));
  }

  static C *new_t (const simple_trans_type &t)
  {
    return new C (t, 1.0, 1.0);
  }

  static C *new_tm (const simple_trans_type &t, double m)
  {
    return new C (t, 1.0, m);
  }

  static C *new_m (double m)
  {
    return new C (m);
  }

  static C *new_mrmu (double mag, double r, bool m, const displacement_type &u)
  {
    return new C (mag, r, m, u);
  }

  static C *new_mrmxy (double mag, double r, bool m, target_coord_type x, target_coord_type y)
  {
    return new C (mag, r, m, displacement_type (x, y));
  }

  static simple_trans_type s_trans (const C *cplx_trans)
  {
    return simple_trans_type (*cplx_trans);
  }

  static gsi::Methods methods ()
  {
    return
    constructor ("new", &new_v, 
      "@brief Creates a unit transformation\n"
    ) +
    constructor ("new|#new_f", &new_f, 
      "@brief Conversion constructor from a fixpoint transformation\n"
      "\n"
      "@args f\n"
      "\n"
      "This constructor will create a transformation with a fixpoint transformation\n"
      "but no displacement.\n"
      "\n"
      "@param f The rotation/mirror code (R0 .. M135 constants)\n"
    ) +
    constructor ("new|#new_xy", &new_xy, 
      "@brief Constructor from a x and y displacement\n"
      "\n"
      "@args x, y\n"
      "\n"
      "This constructor will create a transformation with the specified displacement\n"
      "but no rotation.\n"
      "\n"
      "@param x The x displacement\n"
      "@param y The y displacement\n"
    ) +
    constructor ("new|#new_m", &new_m,
      "@brief Constructor from a magnification\n"
      "\n"
      "@args m\n"
      "\n"
      "Creates a magnifying transformation without displacement and rotation given the magnification m."
    ) +
    constructor ("new|#new_tm", &new_tm,
      "@brief Constructor from a simple transformation and a magnification\n"
      "\n"
      "@args t, m\n"
      "\n"
      "Creates a magnifying transformation from a simple transformation and a magnification."
    ) +
    constructor ("new|#new_t", &new_t,
      "@brief Constructor from a simple transformation alone\n"
      "\n"
      "@args t\n"
      "\n"
      "Creates a magnifying transformation from a simple transformation and a magnification of 1.0."
    ) +
    constructor ("new|#new_mrmu", &new_mrmu,
      "@brief The standard constructor using magnification, angle, mirror flag and displacement\n"
      "@args mag, rot, mirrx, u\n"
      "\n"
      "The sequence of operations is: magnification, mirroring at x axis,\n"
      "rotation, application of displacement.\n"
      "\n"
      "@param mag The magnification\n"
      "@param rot The rotation angle in units of degree\n"
      "@param mirrx True, if mirrored at x axis\n"
      "@param u The displacement\n"
    ) +
    constructor ("new|#new_mrmxy", &new_mrmxy,
      "@brief The standard constructor using magnification, angle, mirror flag and displacement\n"
      "@args mag, rot, mirrx, x, y\n"
      "\n"
      "The sequence of operations is: magnification, mirroring at x axis,\n"
      "rotation, application of displacement.\n"
      "\n"
      "@param mag The magnification\n"
      "@param rot The rotation angle in units of degree\n"
      "@param mirrx True, if mirrored at x axis\n"
      "@param x The x displacement\n"
      "@param y The y displacement\n"
    ) +
    method ("inverted", &C::inverted, 
      "@brief Inversion\n"
      "\n"
      "Returns the inverted transformation\n"
      "\n"
      "@return The inverted transformation\n"
    ) +
    method ("invert", &C::invert,
      "@brief In-place inversion\n"
      "\n"
      "Inverts the transformation and replaces this transformation by the\n"
      "inverted one.\n"
      "\n"
      "@return The inverted transformation\n"
    ) +
    method ("ctrans", &C::template ctrans<coord_type>,
      "@brief The transformation of a distance\n"
      "@args d\n"
      "\n"
      "The \"ctrans\" method transforms the given distance.\n"
      "e = t(d). For the simple transformations, there\n"
      "is no magnification and no modification of the distance\n"
      "therefore.\n"
      "\n"
      "@param d The distance to transform\n"
      "@return The transformed distance\n"
    ) +
    method ("trans", (target_point_type (C::*) (const point_type &) const) &C::template trans<coord_type>, 
      "@brief The transformation of a point \n"
      "@args p\n"
      "\n"
      "The \"trans\" method transforms the given point.\n"
      "q = t(p)\n"
      "\n"
      "@param p The point to transform\n"
      "@return The transformed point\n"
    ) +
    method ("*", &C::concat,
      "@brief Multiplication (concatenation) of transformations\n"
      "@args t\n"
      "\n"
      "The * operator returns self*t (\"t is applied before this transformation\").\n"
      "\n"
      "@param t The transformation to apply before\n"
      "@return The modified transformation\n"
    ) +
    method ("<", &C::operator<,
      "@brief A sorting criterion\n"
      "@args other\n"
      "This method is provided to implement a sorting order. The definition of 'less' is opaque and might change in "
      "future versions."
    ) +
    method ("==", &C::operator==,
      "@brief Equality test\n"
      "@args other\n"
    ) +
    method ("!=", &C::operator!=,
      "@brief Inequality test\n"
      "@args other\n"
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
    ) +
    method ("disp", (const displacement_type &(C::*)() const) &C::disp,
      "@brief Gets the displacement\n"
    ) +
    method ("disp=", (void (C::*) (const displacement_type &)) &C::disp,
      "@brief Sets the displacement\n"
      "@args u\n"
      "@param u The new displacement"
    ) +
    method ("rot", &C::rot,
      "@brief Returns the respective simple transformation equivalent rotation code if possible\n"
      "\n"
      "If this transformation is orthogonal (is_ortho () == true), then this method\n"
      "will return the corresponding fixpoint transformation, not taking into account\n"
      "magnification and displacement. If the transformation is not orthogonal, the result\n"
      "reflects the quadrant the rotation goes into.\n"
    ) +
    method ("is_mirror?", &C::is_mirror,
      "@brief Gets the mirror flag\n"
      "\n"
      "If this property is true, the transformation is composed of a mirroring at the x-axis followed by a rotation "
      "by the angle given by the \\angle property. "
    ) +
    method ("mirror=", &C::mirror,
      "@brief Sets the mirror flag\n"
      "@args m\n"
      "\"mirroring\" describes a reflection at the x-axis which is included in the transformation prior to rotation."
      "@param m The new mirror flag"
    ) +
    method ("is_unity?", &C::is_unity,
      "@brief Test, whether this is a unit transformation\n"
    ) +
    method ("is_ortho?", &C::is_ortho,
      "@brief Test, if the transformation is an orthogonal transformation\n"
      "\n"
      "If the rotation is by a multiple of 90 degree, this method will return true.\n"
    ) +
    method_ext ("s_trans", &s_trans,
      "@brief Extract the simple transformation part\n"
      "\n"
      "The simple transformation part does not reflect magnification or arbitrary angles.\n"
      "Rotation angles are rounded down to multiples of 90 degree. Magnification is fixed to 1.0.\n"
    ) +
    method ("angle", (double (C::*) () const) &C::angle,
      "@brief Gets the angle\n"
      "\n"
      "Note that the simple transformation returns the angle in units of 90 degree. Hence for "
      "a simple trans (i.e. \\Trans), a rotation angle of 180 degree delivers a value of 2 for "
      "the angle attribute. The complex transformation, supporting any rotation angle returns "
      "the angle in degree.\n"
      "\n"
      "@return The rotation angle this transformation provides in degree units (0..360 deg).\n"
    ) +
    method ("angle=", (void (C::*) (double)) &C::angle,
      "@brief Sets the angle\n"
      "@args a\n"
      "@param a The new angle"
      "\n"
      "See \\angle for a description of that attribute.\n"
    ) +
    method ("mag", (double (C::*) () const) &C::mag,
      "@brief Gets the magnification\n"
    ) +
    method ("is_mag?", &C::is_mag,
      "@brief Test, if the transformation is a magnifying one\n"
      "\n"
      "This is the recommended test for checking if the transformation represents\n"
      "a magnification.\n"
    ) +
    method ("mag=", (void (C::*) (double)) &C::mag,
      "@brief Sets the magnification\n"
      "@args m\n"
      "@param m The new magnification"
    ) +
    method ("R0|#r0", &trans_r0,
      "@brief Angle/mirror code for the \"unrotated\" transformation\n"
    ) +
    method ("R90|#r90", &trans_r90,
      "@brief Angle/mirror code for the \"rotated by 90 degree counterclockwise\" transformation\n"
    ) +
    method ("R180|#r180", &trans_r180,
      "@brief Angle/mirror code for the \"rotated by 180 degree counterclockwise\" transformation\n"
    ) +
    method ("R270|#r270", &trans_r270,
      "@brief Angle/mirror code for the \"rotated by 270 degree counterclockwise\" transformation\n"
    ) +
    method ("M0|#m0", &trans_m0,
      "@brief Angle/mirror code for the \"mirrored at the x-axis\" transformation\n"
    ) +
    method ("M45|#m45", &trans_m45,
      "@brief Angle/mirror code for the \"mirrored at the 45 degree axis\" transformation\n"
    ) +
    method ("M90|#m90", &trans_m90,
      "@brief Angle/mirror code for the \"mirrored at the y (90 degree) axis\" transformation\n"
    ) +
    method ("M135|#m135", &trans_m135,
      "@brief Angle/mirror code for the \"mirrored at the 135 degree axis\" transformation\n"
    );
  }
};

db::DCplxTrans dcplx_trans_from_itrans (const db::CplxTrans &itrans)
{
  return db::DCplxTrans (itrans);
}

Class<db::DCplxTrans> decl_DCplxTrans ("DCplxTrans", 
  method ("from_itrans", &dcplx_trans_from_itrans, 
    "@brief Conversion constructor from an integer coordinate transformation\n"
    "\n"
    "@args int_trans\n"
  ) +
  cplx_trans_defs<db::DCplxTrans>::methods (),
  "@brief A complex transformation\n"
  "\n"
  "A complex transformation provides magnification, mirroring at the x-axis, rotation by an arbitary\n"
  "angle and a displacement. This is also the order, the operations are applied.\n"
  "\n"
  "A complex transformation provides a superset of the simple transformation.\n"
  "In many applications, a complex transformation computes floating-point coordinates to minimize rounding effects.\n"
  "This version can transform floating-point coordinate objects.\n"
  "\n"
  "Complex transformations are extensions of the simple transformation classes (\\DTrans in that case) and behave similar.\n"
  "\n"
  "Transformations can be used to transform points or other objects. Transformations can be combined with the '*' operator "
  "to form the transformation which is equivalent to applying the second and then the first. Here is some code:\n"
  "\n"
  "@code\n"
  "# Create a transformation that applies a magnication of 1.5, a rotation by 90 degree\n"
  "# and displacement of 10 in x and 20 units in y direction:\n"
  "t = RBA::CplxTrans::new(1.5, 90, false, 10.0, 20.0)\n"
  "t.to_s            # r90 *1.5 10,20\n"
  "# compute the inverse:\n"
  "t.inverted.to_s   # r270 *0.666666667 -13,7\n"
  "# Combine with another displacement (applied after that):\n"
  "(RBA::CplxTrans::new(5, 5) * t).to_s    # r90 *1.5 15,25\n"
  "# Transform a point:\n"
  "t.trans(RBA::Point::new(100, 200)).to_s # -290,170\n"
  "@/code\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

db::CplxTrans cplx_trans_from_dtrans (const db::DCplxTrans &dtrans)
{
  return db::CplxTrans (dtrans);
}

Class<db::CplxTrans> decl_CplxTrans ("CplxTrans", 
  method ("from_dtrans", &cplx_trans_from_dtrans, 
    "@brief Conversion constructor from an floating-point transformation\n"
    "\n"
    "@args dbl_trans\n"
  ) +
  cplx_trans_defs<db::CplxTrans>::methods (),
  "@brief A complex transformation\n"
  "\n"
  "A complex transformation provides magnification, mirroring at the x-axis, rotation by an arbitary\n"
  "angle and a displacement. This is also the order, the operations are applied.\n"
  "This version can transform integer-coordinate objects into floating-point coordinate objects. "
  "This is the generic and exact case, for example for non-integer magnifications.\n"
  "\n"
  "Complex transformations are extensions of the simple transformation classes (\\Trans or \\DTrans in that case) and behave similar.\n"
  "\n"
  "Transformations can be used to transform points or other objects. Transformations can be combined with the '*' operator "
  "to form the transformation which is equivalent to applying the second and then the first. Here is some code:\n"
  "\n"
  "@code\n"
  "# Create a transformation that applies a magnication of 1.5, a rotation by 90 degree\n"
  "# and displacement of 10 in x and 20 units in y direction:\n"
  "t = RBA::DCplxTrans::new(1.5, 90, false, 10.0, 20.0)\n"
  "t.to_s            # r90 *1.5 10,20\n"
  "# compute the inverse:\n"
  "t.inverted.to_s   # r270 *0.666666667 -13,7\n"
  "# Combine with another displacement (applied after that):\n"
  "(RBA::DCplxTrans::new(5, 5) * t).to_s    # r90 *1.5 15,25\n"
  "# Transform a point:\n"
  "t.trans(RBA::DPoint::new(100, 200)).to_s # -290,170\n"
  "@/code\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

db::ICplxTrans icplx_trans_from_dtrans (const db::DCplxTrans &dtrans)
{
  return db::ICplxTrans (dtrans);
}

db::ICplxTrans icplx_trans_from_trans (const db::CplxTrans &trans)
{
  return db::ICplxTrans (trans);
}

Class<db::ICplxTrans> decl_ICplxTrans ("ICplxTrans", 
  method ("from_dtrans", &icplx_trans_from_dtrans, 
    "@brief Conversion constructor from a floating-point transformation\n"
    "\n"
    "@args dbl_trans\n"
  ) +
  method ("from_trans", &icplx_trans_from_trans, 
    "@brief Conversion constructor from an exact complex transformation\n"
    "\n"
    "@args trans\n"
  ) +
  cplx_trans_defs<db::ICplxTrans>::methods (),
  "@brief A complex transformation\n"
  "\n"
  "A complex transformation provides magnification, mirroring at the x-axis, rotation by an arbitary\n"
  "angle and a displacement. This is also the order, the operations are applied.\n"
  "This version can transform integer-coordinate objects into the same, which may involve rounding and can be inexact.\n"
  "\n"
  "Complex transformations are extensions of the simple transformation classes (\\Trans in that case) and behave similar.\n"
  "\n"
  "Transformations can be used to transform points or other objects. Transformations can be combined with the '*' operator "
  "to form the transformation which is equivalent to applying the second and then the first. Here is some code:\n"
  "\n"
  "@code\n"
  "# Create a transformation that applies a magnication of 1.5, a rotation by 90 degree\n"
  "# and displacement of 10 in x and 20 units in y direction:\n"
  "t = RBA::ICplxTrans::new(1.5, 90, false, 10.0, 20.0)\n"
  "t.to_s            # r90 *1.5 10,20\n"
  "# compute the inverse:\n"
  "t.inverted.to_s   # r270 *0.666666667 -13,7\n"
  "# Combine with another displacement (applied after that):\n"
  "(RBA::ICplxTrans::new(5, 5) * t).to_s    # r90 *1.5 15,25\n"
  "# Transform a point:\n"
  "t.trans(RBA::Point::new(100, 200)).to_s  # -290,170\n"
  "@/code\n"
  "\n"
  "This class has been introduced in version 0.18.\n"
  "\n"
  "See @<a href=\"/programming/database_api.xml\">The Database API@</a> for more details about the "
  "database objects."
);

}

