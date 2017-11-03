
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


#ifndef HDR_dbTrans
#define HDR_dbTrans

#include "config.h"

#include "dbPoint.h"
#include "dbVector.h"
#include "dbMatrix.h"
#include "tlString.h"
#include "tlAssert.h"

#include <string>
#include <iostream>
#include <math.h>

namespace tl {
  class Extractor;
}

namespace db {

template <class I, class F, class R = double> class complex_trans;
template <class C> class simple_trans;
template <class C> class disp_trans;
template <class C> class fixpoint_trans;

/**
 *  @brief Provide the default predicates and properties for transformations for the coordinate type C
 */

template <class C>
struct default_trans
{
  typedef C coord_type;
  typedef C target_coord_type;
  typedef point<C> displacement_type;

  /**
   *  @brief Mirror predicate
   *
   *  If this predicate is true, the transformation will first mirror the coordinates
   *  at the x-axis before applying transformations.
   */
  bool is_mirror () const
  {
    return false;
  }

  /**
   *  @brief This transformation is always unity
   */
  bool is_unity () const
  {
    return true;
  }

  /**
   *  @brief Orthogonal predicate
   *
   *  This predicate tells if the transformation is orthogonal, i.e. does only provide rotations by 
   *  multiple of 90 degree.
   */
  bool is_ortho () const
  {
    return true;
  }

  /**
   *  @brief Magnification predicate
   *
   *  This predicate tells if the transformation is magnifying
   */
  bool is_mag () const
  {
    return false;
  }

  /**
   *  @brief The default rotation code
   */
  int rot () const
  {
    return 0;
  }

  /**
   *  @brief The default rotation code
   */
  fixpoint_trans<C> fp_trans () const
  {
    return fixpoint_trans<C> (0);
  }

  /**
   *  @brief The default displacement
   */
  point<C> disp () const
  {
    return point<C> ();
  }
};

/**
 *  @brief A dummy unit transformation
 *
 *  This transformation is supplied in order to allow generic transformation
 *  parameters being passed a "dummy" transformation.
 *  Even though this transformation does not require a coordinate type, it is provided
 *  to fulfil the contract.
 */

template <class C>
struct unit_trans
  : public default_trans<C>
{
  typedef unit_trans inverse_trans;

  /**
   *  @brief The default constructor (unity transformation)
   */
  unit_trans ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief Copy ctor (which basically does nothing)
   */
  template <class D>
  unit_trans (const unit_trans<D> &) 
  {
    // .. nothing else ..
  }

  /**
   *  @brief Downcast: does nothing
   */
  template <class D>
  unit_trans (const simple_trans<D> &) 
  {
    // .. nothing else ..
  }

  /**
   *  @brief Downcast: does nothing
   */
  template <class D>
  unit_trans (const disp_trans<D> &) 
  {
    // .. nothing else ..
  }

  /**
   *  @brief Downcast: does nothing
   */
  template <class I, class F, class R>
  unit_trans (const complex_trans<I, F, R> &) 
  {
    // .. nothing else ..
  }

  /**
   *  @brief (dummy) equality
   */
  bool operator== (unit_trans b) const
  {
    return true;
  }

  /**
   *  @brief (dummy) fuzzy equality
   */
  bool equal (unit_trans b) const
  {
    return true;
  }

  /**
   *  @brief (dummy) inequality
   */
  bool operator!= (unit_trans b) const
  {
    return false;
  }

  /**
   *  @brief (dummy) comparison
   */
  bool operator< (unit_trans b) const
  {
    return false;
  }

  /**
   *  @brief (dummy) fuzzy less comparison
   */
  bool less (unit_trans b) const
  {
    return true;
  }

  /**
   *  @brief Assignment (which basically does nothing)
   */
  unit_trans &operator= (const unit_trans &) 
  {
    return *this;
  }

  /**
   *  @brief Inversion
   */
  unit_trans inverted () const
  {
    return *this;
  }

  /** 
   *  @brief In-place inversion
   */
  unit_trans invert ()
  {
    return *this;
  }

  /**
   *  @brief The transformation of a point 
   */
  template <class D>
  point<D> operator() (const point<D> &p) const
  {
    return p;
  }

  /**
   *  @brief The transformation of a vector 
   */
  template <class D>
  vector<D> operator() (const vector<D> &p) const
  {
    return p;
  }

  /**
   *  @brief The transformation of a point (non-operator version)
   */
  template <class D>
  point<D> trans (const point<D> &p) const
  {
    return p;
  }

  /**
   *  @brief The transformation of a vector (non-operator version)
   */
  template <class D>
  vector<D> trans (const vector<D> &p) const
  {
    return p;
  }

  /**
   *  @brief Transform a distance
   */
  template <class D>
  D ctrans (D d) const
  {
    return d;
  }

  /**
   *  @brief String conversion
   */
  std::string to_string () const
  {
    return std::string ("");
  }
};

/**
 *  @brief Dummy Multiplication (concatenation) of unit transformations
 */
template <class C>
inline unit_trans<C> 
operator* (const unit_trans<C> &t1, const unit_trans<C> &t2)
{
  return unit_trans<C>();
}

/**
 *  @brief A fixpoint transformation
 *
 *  The fixpoint transformation applies a rotation and/or mirroring operation.
 *  Even though this transformation does not require a coordinate type, it is provided
 *  to fulfil the contract.
 */

template <class C>
class fixpoint_trans
  : public default_trans<C>
{
public:
  typedef fixpoint_trans inverse_trans;

  /**
   *  @brief The default constructor (unity transformation)
   */
  fixpoint_trans ()
    : m_f (0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Returns true, if the transformation is unity
   */
  bool is_unity () const
  {
    return m_f == 0;
  }

  /**
   *  @brief The standard constructor using angle and mirror flag
   *
   *  The sequence of operations is: mirroring at x axis,
   *  rotation, application of displacement.
   *  
   *  @param rot The rotation in units of 90 degree
   *  @param mirrx True, if mirrored at x axis
   */
  fixpoint_trans (int rot, bool mirrx)
    : m_f ((rot & 3) + (mirrx ? 4 : 0))
  {
    // .. nothing else ..
  }
  
  /**
   *  @brief The copy constructor 
   *
   *  @param d The source from which to copy
   */
  template <class D>
  fixpoint_trans (const fixpoint_trans<D> &d) 
    : m_f (d.rot ())
  { }

  /**
   *  @brief The standard constructor using a code rather than angle and mirror and no displacement
   *  
   *  @param f The rotation/mirror code (r0 .. m135 constants)
   */
  fixpoint_trans (int f)
    : m_f (f)
  {
    // .. nothing else ..
  }

  /** 
   *  @brief The rotation/mirror codes
   */
  static const int r0   = 0;  //  No rotation
  static const int r90  = 1;  //  Rotation by 90 degree counterclockwise
  static const int r180 = 2;  //  Rotation by 180 degree counterclockwise
  static const int r270 = 3;  //  Rotation by 270 degree counterclockwise
  static const int m0   = 4;  //  Mirroring at x-axis
  static const int m45  = 5;  //  Mirroring at 45-degree axis
  static const int m90  = 6;  //  Mirroring at y-axis
  static const int m135 = 7;  //  Mirroring at 135-degree axis

  /**
   *  @brief Inversion
   *
   *  Returns the inverted transformation
   *
   *  @return The inverted transformation
   */
  fixpoint_trans<C> inverted () const
  {
    fixpoint_trans t (*this);
    t.invert ();
    return t;
  }

  /** 
   *  @brief In-place inversion
   *
   *  Inverts the transformation and replaces *this by the
   *  inverted one.
   *
   *  @return The inverted transformation
   */
  fixpoint_trans<C> invert ()
  {
    if (m_f < 4) {
      m_f = (4 - m_f) & 3;
    }
    return *this;
  }

  /**
   *  @brief The transformation of a point 
   *
   *  The operator() method transforms the given point.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed point
   */
  template <class D>
  point<D> operator() (const point<D> &p) const
  {
    switch (m_f) {
    default:
      return point<D> (p.x (), p.y ());
    case 1:
      return point<D> (-p.y (), p.x ());
    case 2:
      return point<D> (-p.x (), -p.y ());
    case 3:
      return point<D> (p.y (), -p.x ());
    case 4:
      return point<D> (p.x (), -p.y ());
    case 5:
      return point<D> (p.y (), p.x ());
    case 6:
      return point<D> (-p.x (), p.y ());
    case 7:
      return point<D> (-p.y (), -p.x ());
    }
  }

  /**
   *  @brief The transformation of a point (non-operator version)
   */
  template <class D>
  point<D> trans (const point<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief The transformation of a vector 
   *
   *  The operator() method transforms the given vector.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed point
   */
  template <class D>
  vector<D> operator() (const vector<D> &p) const
  {
    switch (m_f) {
    default:
      return vector<D> (p.x (), p.y ());
    case 1:
      return vector<D> (-p.y (), p.x ());
    case 2:
      return vector<D> (-p.x (), -p.y ());
    case 3:
      return vector<D> (p.y (), -p.x ());
    case 4:
      return vector<D> (p.x (), -p.y ());
    case 5:
      return vector<D> (p.y (), p.x ());
    case 6:
      return vector<D> (-p.x (), p.y ());
    case 7:
      return vector<D> (-p.y (), -p.x ());
    }
  }

  /**
   *  @brief The transformation of a vector (non-operator version)
   */
  template <class D>
  vector<D> trans (const vector<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief Transform a fixpoint transformation
   */
  fixpoint_trans ftrans (fixpoint_trans t) const
  {
    fixpoint_trans tt (*this);
    tt *= t;
    return tt;
  }

  /** 
   *  @brief Extract the fixpoint transformation part (this is identical to *this in this case)
   */
  const fixpoint_trans &fp_trans () const
  {
    return *this;
  }

  /**
   *  @brief Transform a distance
   */
  template <class D>
  D ctrans (D d) const
  {
    return d;
  }

  /**
   *  @brief Multiplication (concatenation) of transformations
   *
   *  The *= operator modifies the transformation by 
   *  replacing *this with *this * t (t is applied before *this).
   *
   *  @param t The transformation to apply before
   *  @return The modified transformation
   */
  fixpoint_trans &operator*= (const fixpoint_trans &t)
  {
    m_f = ((m_f + (1 - ((m_f & 4) >> 1)) * t.m_f) & 3) + ((m_f ^ t.m_f) & 4);
    return *this;
  }

  /**
   *  @brief A sorting criterion
   */
  bool operator< (const fixpoint_trans &t) const
  {
    return m_f < t.m_f;
  }

  /**
   *  @brief A (dummy) fuzzy less criterion
   */
  bool less (const fixpoint_trans &t) const
  {
    return m_f < t.m_f;
  }

  /**
   *  @brief Equality test
   */
  bool operator== (const fixpoint_trans &t) const
  {
    return m_f == t.m_f;
  }

  /**
   *  @brief Inequality test
   */
  bool operator!= (const fixpoint_trans &t) const
  {
    return !operator== (t);
  }

  /**
   *  @brief A (dummy) fuzzy equality test
   */
  bool equal (const fixpoint_trans &t) const
  {
    return m_f == t.m_f;
  }

  /**
   *  @brief String conversion
   */
  std::string to_string () const
  {
    const char *ms [] = { "r0", "r90", "r180", "r270", 
                          "m0", "m45", "m90",  "m135" };

    return std::string ((m_f < 0 || m_f >= 8) ? "*" : ms [m_f]);
  }

  /**
   *  @brief Accessor to the rotation/mirror code
   */
  int rot () const 
  {
    return m_f;
  }

  /**
   *  @brief Accessor to the angle (in units of 90 degree)
   */
  int angle () const 
  {
    return m_f & 3;
  }

  /**
   *  @brief Mirror flag.
   *
   *  The result of this operation is true, if the transformation is
   *  mirroring, i.e. det(M) == -1.
   */
  bool is_mirror () const
  {
    return m_f >= 4;
  }

private:
  int m_f;
};

/**
 *  @brief Multiplication (concatenation) of transformations
 *
 *  t = t1 * t2 is the resulting transformation that is effectively
 *  applied if first t2 and then t1 is applied.
 *
 *  @param t1 The transformation to apply last
 *  @param t2 The transformation to apply first
 *  @return t1 * t2
 */
template <class C>
inline fixpoint_trans<C> 
operator* (const fixpoint_trans<C> &t1, const fixpoint_trans<C> &t2)
{
  fixpoint_trans<C> t (t1);
  t *= t2;
  return t;
}

/**
 *  @brief Output stream insertion operator
 */
template <class C>
inline std::ostream &
operator<< (std::ostream &os, const fixpoint_trans<C> &t)
{
  return (os << t.to_string ());
}

/**
 *  @brief A displacement transformation
 *
 *  The displacement transformation applies a displacement vector, but no rotation.
 */

template <class C>
class disp_trans
  : public default_trans<C>
{
public:
  typedef C coord_type;
  typedef C target_coord_type;
  typedef point<C> displacement_type;
  typedef disp_trans<C> inverse_trans;

  /**
   *  @brief The default constructor (unity transformation)
   */
  disp_trans ()
    : m_u ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief The "conversion" from the unit transformation to a displacement
   */
  disp_trans (unit_trans<C>)
    : m_u ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief The copy constructor that converts also
   *
   *  The copy constructor allows to convert between different
   *  coordinate types, if possible.
   *
   *  @param d The source from which to copy
   */
  template <class D>
  disp_trans (const disp_trans<D> &d) 
    : m_u (d.disp ())
  { }

  /**
   *  @brief The standard constructor using a displacement only
   *  
   *  @param u The displacement
   */
  disp_trans (const point<C> &u)
    : m_u (u)
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using a displacement only (as vector)
   *  
   *  @param u The displacement
   */
  disp_trans (const vector<C> &u)
    : m_u (u.x (), u.y ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief Downcast: extracts the displacement part of a complex transformation
   */
  disp_trans (const simple_trans<C> &st) 
    : m_u (st.disp ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief Downcast: extracts the displacement part of a complex transformation
   */
  template <class I, class F, class R>
  disp_trans (const complex_trans<I, F, R> &ct) 
    : m_u (ct.disp ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief Returns true, if the transformation is unity
   */
  bool is_unity () const
  {
    return m_u.equal (displacement_type ());
  }

  /**
   *  @brief Inversion
   *
   *  Returns the inverted transformation
   *
   *  @return The inverted transformation
   */
  disp_trans<C> inverted () const
  {
    disp_trans<C> t (*this);
    t.invert ();
    return t;
  }

  /** 
   *  @brief In-place inversion
   *
   *  Inverts the transformation and replaces *this by the
   *  inverted one.
   *
   *  @return The inverted transformation
   */
  disp_trans<C> invert ()
  {
    m_u = -m_u;
    return *this;
  }

  /**
   *  @brief The transformation of a distance
   *
   *  The ctrans method transforms the given distance.
   *  e = t(d). For the displacement transformations, there
   *  is no magnification and no modification of the distance
   *  therefore.
   *  
   *  @param d The distance to transform
   *  @return The transformed distance
   */
  template <class D>
  C ctrans (D d) const
  {
    return d;
  }
  
  /**
   *  @brief The transformation of a point 
   *
   *  The operator() method transforms the given point.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed point
   */
  template <class D>
  point<C> operator() (const point<D> &p) const
  {
    return point<C> (p) + m_u;
  }

  /**
   *  @brief The transformation of a point (non-operator version)
   */
  template <class D>
  point<C> trans (const point<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief The transformation of a vector 
   *
   *  The operator() method transforms the given vector.
   *  q = t(p)
   *  
   *  @param p The vector to transform
   *  @return The transformed vector
   */
  template <class D>
  vector<C> operator() (const vector<D> &p) const
  {
    return p;
  }

  /**
   *  @brief The transformation of a vector (non-operator version)
   */
  template <class D>
  vector<C> trans (const vector<D> &p) const
  {
    return p;
  }

  /**
   *  @brief Multiplication (concatenation) of transformations
   *
   *  The *= operator modifies the transformation by 
   *  replacing *this with *this * t (t is applied before *this).
   *
   *  @param t The transformation to apply before
   *  @return The modified transformation
   */
  disp_trans<C> &operator*= (const disp_trans<C> &t)
  {
    m_u += t.m_u;
    return *this;
  }
  
  /**
   *  @brief A sorting criterion
   */
  bool operator< (const disp_trans<C> &t) const
  {
    return m_u < t.m_u;
  }

  /**
   *  @brief A fuzzy sorting criterion
   */
  bool less (const disp_trans<C> &t) const
  {
    return m_u.less (t.m_u);
  }

  /**
   *  @brief Equality test
   */
  bool operator== (const disp_trans<C> &t) const
  {
    return m_u == t.m_u;
  }

  /**
   *  @brief Inequality test
   */
  bool operator!= (const disp_trans<C> &t) const
  {
    return !operator== (t);
  }

  /**
   *  @brief A fuzzy equality test
   */
  bool equal (const disp_trans<C> &t) const
  {
    return m_u.equal (t.m_u);
  }

  /**
   *  @brief String conversion
   */
  std::string to_string () const
  {
    return m_u.to_string ();
  }

  /** 
   *  @brief Gets the displacement
   */
  const displacement_type &disp () const
  {
    return m_u;
  }

  /** 
   *  @brief Sets the displacement
   */
  void disp (const displacement_type &u) 
  {
    m_u = u;
  }

  /**
   *  @brief Mirror predicate
   */
  bool is_mirror () const
  {
    return false;
  }

  /** 
   *  @brief Extract the fixpoint transformation part (this is a r0 contribution in this case)
   */
  fixpoint_trans<C> fp_trans () const
  {
    return fixpoint_trans<C> ();
  }

private:
  point<C> m_u;
};

/**
 *  @brief Multiplication (concatenation) of transformations
 *
 *  t = t1 * t2 is the resulting transformation that is effectively
 *  applied if first t2 and then t1 is applied.
 *
 *  @param t1 The transformation to apply last
 *  @param t2 The transformation to apply first
 *  @return t1 * t2
 */
template <class C>
inline disp_trans<C> 
operator* (const disp_trans<C> &t1, const disp_trans<C> &t2)
{
  disp_trans<C> t (t1);
  t *= t2;
  return t;
}

/**
 *  @brief Output stream insertion operator
 */
template <class C>
inline std::ostream &
operator<< (std::ostream &os, const disp_trans<C> &t)
{
  return (os << t.to_string ());
}

/**
 *  @brief A simple transformation
 *
 *  The simple transformation applies a displacement vector
 *  and a simple fixpoint transformation. 
 */

template <class C>
class simple_trans
  : public fixpoint_trans<C>
{
public:
  typedef C coord_type;
  typedef C target_coord_type;
  typedef point<C> displacement_type;
  typedef simple_trans<C> inverse_trans;

  /**
   *  @brief The default constructor (unity transformation)
   */
  simple_trans ()
    : fixpoint_trans<C> (0), m_u ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief Conversion constructor from a fixpoint transformation
   */
  simple_trans (fixpoint_trans<C> f)
    : fixpoint_trans<C> (f), m_u ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief Conversion constructor from a unit transformation
   */
  simple_trans (unit_trans<C>)
    : fixpoint_trans<C> (0), m_u ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief Conversion constructor from a displacement transformation
   */
  template <class D>
  simple_trans (const disp_trans<D> &d)
    : fixpoint_trans<C> (0), m_u (d.disp ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using angle and mirror flag
   *
   *  The sequence of operations is: mirroring at x axis,
   *  rotation, application of displacement.
   *  
   *  @param rot The rotation in units of 90 degree
   *  @param mirrx True, if mirrored at x axis
   *  @param u The displacement
   */
  template <class D>
  simple_trans (int rot, bool mirrx, const point<D> &u)
    : fixpoint_trans<C> (rot, mirrx), m_u (u)
  {
    // .. nothing else ..
  }
  
  /**
   *  @brief The standard constructor using angle and mirror flag
   *
   *  The sequence of operations is: mirroring at x axis,
   *  rotation, application of displacement.
   *  
   *  @param rot The rotation in units of 90 degree
   *  @param mirrx True, if mirrored at x axis
   *  @param u The displacement (as vector)
   */
  template <class D>
  simple_trans (int rot, bool mirrx, const vector<D> &u)
    : fixpoint_trans<C> (rot, mirrx), m_u (u.x (), u.y ())
  {
    // .. nothing else ..
  }
  
  /**
   *  @brief The copy constructor that converts also
   *
   *  The copy constructor allows to convert between different
   *  coordinate types, if possible.
   *
   *  @param d The source from which to copy
   */
  template <class D>
  simple_trans (const simple_trans<D> &d) 
    : fixpoint_trans<C> (d.rot ()), m_u (d.disp ())
  { }

  /**
   *  @brief Conversion from a double-representation
   *
   *  @param d The source from which to copy
   */
  static simple_trans from_double (const simple_trans<double> &d) 
  { 
    return simple_trans (fixpoint_trans<C> (d.rot ()).rot (), displacement_type::from_double (d.disp ()));
  }

  /**
   *  @brief The standard constructor using a code rather than angle and mirror
   *  
   *  @param f The rotation/mirror code (r0 .. m135 constants)
   *  @param u The displacement
   */
  simple_trans (int f, const point<C> &u)
    : fixpoint_trans<C> (f), m_u (u)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Downcast: extracts the simple transformation part of a complex transformation
   */
  template <class I, class F, class R>
  simple_trans (const complex_trans<I, F, R> &ct) 
    : fixpoint_trans<C> (ct.fp_trans ()), m_u (ct.disp ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using a code rather than angle and mirror
   *  
   *  @param f The rotation/mirror code (r0 .. m135 constants)
   *  @param u The displacement (as vector)
   */
  simple_trans (int f, const vector<C> &u)
    : fixpoint_trans<C> (f), m_u (u.x (), u.y ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using a displacement only
   *  
   *  @param u The displacement
   */
  simple_trans (const point<C> &u)
    : fixpoint_trans<C> (0), m_u (u)
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using a displacement only
   *  
   *  @param u The displacement (as vector)
   */
  simple_trans (const vector<C> &u)
    : fixpoint_trans<C> (0), m_u (u.x (), u.y ())
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using a code rather than angle and mirror and no displacement
   *  
   *  @param f The rotation/mirror code (r0 .. m135 constants)
   */
  simple_trans (int f)
    : fixpoint_trans<C> (f), m_u (0, 0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Inversion
   *
   *  Returns the inverted transformation
   *
   *  @return The inverted transformation
   */
  simple_trans<C> inverted () const
  {
    simple_trans<C> t (*this);
    t.invert ();
    return t;
  }

  /** 
   *  @brief In-place inversion
   *
   *  Inverts the transformation and replaces *this by the
   *  inverted one.
   *
   *  @return The inverted transformation
   */
  simple_trans<C> invert ()
  {
    fixpoint_trans<C>::invert ();
    point<C> u = m_u;
    m_u = point<C> ();
    m_u = -operator() (u);
    return *this;
  }

  /**
   *  @brief Returns true, if the transformation is unity
   */
  bool is_unity () const
  {
    return m_u.equal (displacement_type ()) && fixpoint_trans<C>::is_unity ();
  }

  /**
   *  @brief The transformation of a distance
   *
   *  The ctrans method transforms the given distance.
   *  e = t(d). For the simple transformations, there
   *  is no magnification and no modification of the distance
   *  therefore.
   *  
   *  @param d The distance to transform
   *  @return The transformed distance
   */
  template <class D>
  D ctrans (D d) const
  {
    return d;
  }
  
  /**
   *  @brief The transformation of a point 
   *
   *  The operator() method transforms the given point.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed point
   */
  template <class D>
  point<C> operator() (const point<D> &p) const
  {
    return point<C> (fixpoint_trans<C>::operator() (p)) + m_u;
  }

  /**
   *  @brief The transformation of a point (non-operator version)
   */
  template <class D>
  point<C> trans (const point<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief The transformation of a vector 
   *
   *  The operator() method transforms the given vector.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed vector
   */
  template <class D>
  vector<C> operator() (const vector<D> &p) const
  {
    return vector<C> (fixpoint_trans<C>::operator() (p));
  }

  /**
   *  @brief The transformation of a vector (non-operator version)
   */
  template <class D>
  vector<C> trans (const vector<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief Transform a fixpoint transformation
   */
  fixpoint_trans<C> ftrans (fixpoint_trans<C> t) const
  {
    return fixpoint_trans<C>::ftrans (t);
  }

  /**
   *  @brief Multiplication (concatenation) of transformations
   *
   *  The *= operator modifies the transformation by 
   *  replacing *this with *this * t (t is applied before *this).
   *
   *  @param t The transformation to apply before
   *  @return The modified transformation
   */
  simple_trans<C> &operator*= (const simple_trans<C> &t)
  {
    m_u = operator() (t.m_u); 
    fixpoint_trans<C>::operator*= (t);
    return *this;
  }

  /**
   *  @brief A method version of operator*, mainly for automation purposes
   */
  simple_trans<C> concat (const simple_trans<C> &t) const
  {
    simple_trans<C> r (*this);
    r *= t;
    return r;
  }

  /**
   *  @brief A sorting criterion
   */
  bool operator< (const simple_trans<C> &t) const
  {
    return fixpoint_trans<C>::operator< (t) || (fixpoint_trans<C>::operator== (t) && m_u < t.m_u);
  }

  /**
   *  @brief A fuzzy sorting criterion
   */
  bool less (const simple_trans<C> &t) const
  {
    return fixpoint_trans<C>::operator< (t) || (fixpoint_trans<C>::operator== (t) && m_u.less (t.m_u));
  }

  /**
   *  @brief Equality test
   */
  bool operator== (const simple_trans<C> &t) const
  {
    return fixpoint_trans<C>::operator== (t) && m_u == t.m_u;
  }

  /**
   *  @brief Inequality test
   */
  bool operator!= (const simple_trans<C> &t) const
  {
    return !operator== (t);
  }

  /*
   *  @brief A fuzzy equality test
   */
  bool equal (const simple_trans<C> &t) const
  {
    return fixpoint_trans<C>::operator== (t) && m_u.equal (t.m_u);
  }

  /**
   *  @brief String conversion
   */
  std::string to_string () const
  {
    std::string s1 = fixpoint_trans<C>::to_string ();
    std::string s2 = m_u.to_string ();
    if (! s1.empty () && ! s2.empty ()) {
      return s1 + " " + s2;
    } else {
      return s1 + s2;
    }
  }

  /** 
   *  @brief Gets the displacement
   */
  const point<C> &disp () const
  {
    return m_u;
  }

  /** 
   *  @brief Sets the displacement
   */
  void disp (const point<C> &u) 
  {
    m_u = u;
  }

  /** 
   *  @brief Accessor to the fp_trans
   */
  const fixpoint_trans<C> &fp_trans () const
  {
    return *this;
  }

private:
  point<C> m_u;
};

/**
 *  @brief Multiplication (concatenation) of transformations
 *
 *  t = t1 * t2 is the resulting transformation that is effectively
 *  applied if first t2 and then t1 is applied.
 *
 *  @param t1 The transformation to apply last
 *  @param t2 The transformation to apply first
 *  @return t1 * t2
 */
template <class C>
inline simple_trans<C> 
operator* (const simple_trans<C> &t1, const simple_trans<C> &t2)
{
  simple_trans<C> t (t1);
  t *= t2;
  return t;
}

/**
 *  @brief Output stream insertion operator
 */
template <class C>
inline std::ostream &
operator<< (std::ostream &os, const simple_trans<C> &t)
{
  return (os << t.to_string ());
}

/**
 *  @brief A complex transformation
 *
 *  A complex transformation provides magnification, mirroring at the x-axis, rotation by an arbitary
 *  angle and a displacement. The template parameters for this transformation are
 *  I (the input coordinate type), F (the output coordinate type) and M (the representation
 *  type used internally for representing the floating-point members).
 */
template <class I, class F, class R>
class complex_trans
{
public:
  typedef I coord_type;
  typedef F target_coord_type;
  typedef point<F> displacement_type;
  //  HINT: the inverse transformation would be complex_trans<F, I, R>. However, this imposes 
  //  some problems with in-place inversion etc. We simply assume for now, that inversion is only
  //  performed, when F == I.
  typedef complex_trans<I, F, R> inverse_trans;
  typedef epsilon_f<R> eps_f;

  /**
   *  @brief The default constructor (unity transformation)
   */
  complex_trans ()
    : m_sin (0.0), m_cos (1.0), m_mag (1.0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Conversion constructor from a unit transformation
   */
  template <class D>
  complex_trans (unit_trans<D> f)
    : m_sin (0.0), m_cos (1.0), m_mag (1.0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Conversion constructor from a fixpoint transformation code
   */
  complex_trans (int fc)
  {
    db::fixpoint_trans<I> f (fc);
    db::point<R> t (1.0, 0.0);
    t = f (t);
    m_cos = t.x ();
    m_sin = t.y ();
    m_mag = f.is_mirror () ? -1.0 : 1.0;
  }

  /**
   *  @brief Conversion constructor from a fixpoint transformation
   */
  template <class D>
  complex_trans (fixpoint_trans<D> f)
  {
    db::point<R> t (1.0, 0.0);
    t = f (t);
    m_cos = t.x ();
    m_sin = t.y ();
    m_mag = f.is_mirror () ? -1.0 : 1.0;
  }

  /**
   *  @brief Conversion constructor from a displacement transformation
   */
  template <class D>
  complex_trans (const disp_trans<D> &d)
    : m_u (d.disp ()), m_sin (0.0), m_cos (1.0), m_mag (1.0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Conversion constructor from a simple transformation
   */
  template <class D>
  complex_trans (const simple_trans<D> &s)
    : m_u (s.disp ())
  {
    db::point<R> t (1.0, 0.0);
    t = s.fp_trans () (t);
    m_cos = t.x ();
    m_sin = t.y ();
    m_mag = s.is_mirror () ? -1.0 : 1.0;
  }

  /**
   *  @brief Migration constructor from a simple transformation to a complex transformation
   *
   *  This constructor is supposed to support migration from a simple to a complex transformation
   *  in an incremental fashion. Basically that means, that everything that is missing in the 
   *  simple transformation is added by additional parameters.
   *  Specifically this added information is the magnification and the incremental rotation angle
   *  to fill up the 90 degree multiples of the simple transformation to the desired rotation.
   *  To save a trigonometric function computation, the added angle is given as the cosine of the
   *  angle (a value from 1.0 representing 0 degree to 0.0 representing 90 degree).
   *
   *  @param s The simple transformation to derive this transformation from
   *  @param acos The cosine of the additional rotation angle
   *  @param mag The magnification
   */
  template <class D>
  complex_trans (const simple_trans<D> &s, double acos, double mag)
    : m_u (s.disp ())
  {
    db::point<R> t (1.0, 0.0);
    t = s.fp_trans () (t);
    double asin = sqrt (1.0 - acos * acos); // we may to this since we know that the angle is between 0 and 90 degree
    m_cos = t.x () * acos - t.y () * asin;
    m_sin = t.x () * asin + t.y () * acos;
    m_mag = s.is_mirror () ? -mag : mag;
  }

  /**
   *  @brief The standard constructor using a Matrix2d and a displacement
   *
   *  @param m The matrix to take the rotation part of the transformation from
   *  @param u The displacement
   *
   *  The matrix must not contain shear components.
   */
  template <class D>
  complex_trans (double mag, double rot, bool mirrx, const point<D> &u)
    : m_u (u)
  {
    tl_assert (mag > 0.0);
    m_mag = mirrx ? -mag : mag;
    rot *= M_PI / 180.0;
    m_sin = sin (rot);
    m_cos = cos (rot);
  }
  
  /**
   *  @brief The standard constructor using a Matrix3d object
   *
   *  @param m The matrix to take the transformation from
   *
   *  The matrix must not represent perspective distortion nor shear.
   */
  complex_trans (const Matrix3d &m)
    : m_u (displacement_type::from_double (m.disp ()))
  {
    tl_assert (! m.has_shear ());
    tl_assert (! m.has_perspective ());
    std::pair<double, double> mag = m.mag ();
    tl_assert (fabs (mag.first - mag.second) < 1e-10);
    double rot = m.angle () * M_PI / 180.0;
    m_mag = m.is_mirror () ? -mag.first : mag.first;
    m_sin = sin (rot);
    m_cos = cos (rot);
  }
  
  /**
   *  @brief The standard constructor using a Matrix2d object
   *
   *  The sequence of operations is: magnification, mirroring at x axis,
   *  rotation, application of displacement.
   *  
   *  @param mag The magnification
   *  @param rot The rotation angle in units of degree
   *  @param mirrx True, if mirrored at x axis
   *  @param u The displacement
   */
  template <class D>
  complex_trans (const Matrix2d &m, const point<D> &u)
    : m_u (u)
  {
    tl_assert (! m.has_shear ());
    std::pair<double, double> mag = m.mag ();
    tl_assert (fabs (mag.first - mag.second) < 1e-10);
    double rot = m.angle () * M_PI / 180.0;
    m_mag = m.is_mirror () ? -mag.first : mag.first;
    m_sin = sin (rot);
    m_cos = cos (rot);
  }
  
  /**
   *  @brief The standard constructor using magnification, angle and mirror flag
   *
   *  The sequence of operations is: magnification, mirroring at x axis,
   *  rotation, application of displacement.
   *  
   *  @param mag The magnification
   *  @param rot The rotation angle in units of degree
   *  @param mirrx True, if mirrored at x axis
   *  @param u The displacement (as vector)
   */
  template <class D>
  complex_trans (double mag, double rot, bool mirrx, const vector<D> &u)
    : m_u (u.x (), u.y ())
  {
    tl_assert (mag > 0.0);
    m_mag = mirrx ? -mag : mag;
    rot *= M_PI / 180.0;
    m_sin = sin (rot);
    m_cos = cos (rot);
  }
  
  /**
   *  @brief The standard constructor using magnification only
   *
   *  @param mag The magnification
   */
  complex_trans (double mag)
  {
    tl_assert (mag > 0.0);
    m_mag = mag;
    m_sin = 0.0;
    m_cos = 1.0;
  }
  
  /**
   *  @brief The copy constructor that converts also
   *
   *  The copy constructor allows to convert between different
   *  coordinate types, if possible.
   *
   *  @param d The source from which to copy
   */
  template <class II, class FF, class RR>
  complex_trans (const complex_trans<II, FF, RR> &d) 
    : m_u (coord_traits<F>::rounded (d.m_u.x ()), coord_traits<F>::rounded (d.m_u.y ())), m_sin (d.m_sin), m_cos (d.m_cos), m_mag (d.m_mag)
  { }

  /**
   *  @brief The standard constructor using a displacement only
   *  
   *  @param u The displacement
   */
  template <class D>
  complex_trans (const point<D> &u)
    : m_u (u), m_sin (0.0), m_cos (1.0), m_mag (1.0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief The standard constructor using a displacement only
   *  
   *  @param u The displacement (as vector)
   */
  template <class D>
  complex_trans (const vector<D> &u)
    : m_u (u.x (), u.y ()), m_sin (0.0), m_cos (1.0), m_mag (1.0)
  {
    // .. nothing else ..
  }

  /**
   *  @brief Inversion
   *
   *  Returns the inverted transformation
   *
   *  @return The inverted transformation
   */
  inverse_trans inverted () const
  {
    complex_trans t (*this);
    t.invert ();
    return t;
  }

  /** 
   *  @brief In-place inversion
   *
   *  Inverts the transformation and replaces *this by the
   *  inverted one.
   *
   *  @return The inverted transformation
   */
  complex_trans invert ()
  {
    m_mag = 1.0 / m_mag;
    m_sin = -m_sin * (m_mag < 0.0 ? -1.0 : 1.0);
    db::point<F> u (-m_u.x (), -m_u.y ());
    m_u = db::point<F> ();
    m_u = this->operator () (u);
    return *this;
  }

  /**
   *  @brief The transformation of a distance
   *
   *  The ctrans method transforms the given distance.
   *  
   *  @param d The distance to transform
   *  @return The transformed distance
   */
  template <class D>
  F ctrans (D d) const
  {
    return coord_traits<F>::rounded (d * fabs (m_mag));
  }
  
  /**
   *  @brief The transformation of a point 
   *
   *  The operator() method transforms the given point.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed point
   */
  template <class D>
  point<F> operator() (const point<D> &p) const
  {
    db::point<F> mp (coord_traits<F>::rounded (m_cos * p.x () * fabs (m_mag) - m_sin * p.y () * m_mag), 
                     coord_traits<F>::rounded (m_sin * p.x () * fabs (m_mag) + m_cos * p.y () * m_mag));
    return mp + m_u;
  }

  /**
   *  @brief The transformation of a point (non-operator version)
   */
  template <class D>
  point<F> trans (const point<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief The transformation of a vector 
   *
   *  The operator() method transforms the given vector.
   *  q = t(p)
   *  
   *  @param p The vector to transform
   *  @return The transformed vector
   */
  template <class D>
  vector<F> operator() (const vector<D> &p) const
  {
    db::vector<F> mp (coord_traits<F>::rounded (m_cos * p.x () * fabs (m_mag) - m_sin * p.y () * m_mag), 
                      coord_traits<F>::rounded (m_sin * p.x () * fabs (m_mag) + m_cos * p.y () * m_mag));
    return mp;
  }

  /**
   *  @brief The transformation of a vector (non-operator version)
   */
  template <class D>
  vector<F> trans (const vector<D> &p) const
  {
    return operator() (p);
  }

  /**
   *  @brief Test, whether this is a unit transformation
   */
  bool is_unity () const
  {
    if (fabs (m_mag - 1.0) > eps_f ()) {
      return false;
    }
    if (fabs (m_sin) > eps_f ()) {
      return false;
    }
    if (fabs (m_cos - 1.0) > eps_f ()) {
      return false;
    }
    return fabs (double (m_u.x ())) <= db::epsilon_f<F> () && 
           fabs (double (m_u.y ())) <= db::epsilon_f<F> ();
  }

  /**
   *  @brief Test, if the transformation is an orthogonal transformation
   *
   *  If the rotation is by a multiple of 90 degree, this method will return true.
   */
  bool is_ortho () const
  {
    return fabs (m_sin * m_cos) <= eps_f ();
  }

  /**
   *  @brief Return the respective rotation code if possible
   *
   *  If this transformation is orthogonal (is_ortho () == true), then this method
   *  will return the corresponding fixpoint transformation, not taking into account
   *  magnification and displacement. If the transformation is not orthogonal, the result
   *  reflects the quadrant the rotation goes into with the guarantee to reproduce the 
   *  correct quadrant in the exact case.
   */
  int rot () const
  {
    return fp_trans ().rot ();
  }

  /**
   *  @brief Return the respective fixpoint trans if possible 
   *
   *  If this transformation is orthogonal (is_ortho () == true), then this method
   *  will return the corresponding fixpoint transformation, not taking into account
   *  magnification and displacement. If the transformation is not orthogonal, the result
   *  reflects the quadrant the rotation goes into with the guarantee to reproduce the 
   *  correct quadrant in the exact case.
   */
  fixpoint_trans<I> fp_trans () const
  {
    int c;
    if (m_cos > eps_f () && m_sin >= -eps_f ()) {
      c = 0 /*r0*/;
    } else if (m_cos <= eps_f () && m_sin > eps_f ()) {
      c = 1 /*r90*/;
    } else if (m_cos < -eps_f () && m_sin <= eps_f ()) {
      c = 2 /*r180*/;
    } else {
      c = 3 /*r270*/;
    }
    return fixpoint_trans<I> (c + (m_mag < 0.0 ? 4 : 0));
  }

  /**
   *  @brief Read accessor for the angle
   *
   *  To check, if the transformation represents a rotation by a angle that
   *  is a multiple of 90 degree, use is_ortho.
   *
   *  @return The rotation angle this transformation provides in degree units (0..360 deg).
   */
  double angle () const
  {
    double a = atan2 (m_sin, m_cos) * (180.0 / M_PI);
    if (a < -eps_f ()) {
      a += 360.0;
    } else if (a <= eps_f ()) {
      a = 0.0;
    }
    return a;
  }

  /**
   *  @brief Write accessor for the angle
   */
  void angle (double rot)
  {
    rot *= M_PI / 180.0;
    m_sin = sin (rot);
    m_cos = cos (rot);
  }

  /**
   *  @brief Read accessor to the cosine part of the transformation matrix
   */
  double mcos () const
  {
    return m_cos;
  }

  /**
   *  @brief Read accessor to the sine part of the transformation matrix
   */
  double msin () const
  {
    return m_sin;
  }

  /** 
   *  @brief Read accessor to the magnification
   */
  double mag () const
  {
    return fabs (m_mag);
  }

  /** 
   *  @brief Test, if the transformation is a magnifying one
   *
   *  This is the recommended test for checking if the transformation represents
   *  a magnification.
   */
  bool is_mag () const
  {
    return fabs (fabs (m_mag) - 1.0) > eps_f ();
  }

  /** 
   *  @brief Write accessor to the magnification
   */
  void mag (double m)
  {
    tl_assert (m > 0.0);
    m_mag = m_mag < 0.0 ? -m : m;
  }

  /** 
   *  @brief Test, if the transformation is mirroring
   */
  bool is_mirror () const
  {
    return m_mag < 0.0;
  }

  /** 
   *  @brief Write accessor to the mirror flag
   */
  void mirror (bool m)
  {
    m_mag = m ? -fabs (m_mag) : fabs (m_mag);
  }

  /** 
   *  @brief Read accessor to the displacement
   */
  const point<F> &disp () const
  {
    return m_u;
  }

  /** 
   *  @brief Write accessor to the displacement
   */
  void disp (const point<F> &u)
  {
    m_u = u;
  }

  /** 
   *  @brief Write accessor to the displacement (as vector)
   */
  void disp (const vector<F> &u)
  {
    m_u = point<F> (u.x (), u.y ());
  }

  /**
   *  @brief Multiplication (concatenation) of transformations
   *
   *  The *= operator modifies the transformation by 
   *  replacing *this with *this * t (t is applied before *this).
   *
   *  @param t The transformation to apply before
   *  @return The modified transformation
   */
  complex_trans &operator*= (const complex_trans &t)
  {
    double s1 = m_mag < 0.0 ? -1.0 : 1.0;

    m_u = operator() (t.m_u); 
    m_mag *= t.m_mag;
    double c = m_cos * t.m_cos - s1 * m_sin * t.m_sin;
    double s = m_sin * t.m_cos + s1 * m_cos * t.m_sin;
    m_cos = c;
    m_sin = s;
    return *this;
  }

  /**
   *  @brief Retrieve the residual part of the angle 
   *
   *  The residual part is the cosine of the angle difference to the 
   *  lower next multiple of 90 degree. I.e. the residual part of 135 degree
   *  would be cos(45 deg).
   */
  double rcos () const
  {
    if (m_cos > eps_f () && m_sin >= -eps_f ()) {
      return m_cos;
    } else if (m_cos <= eps_f () && m_sin > eps_f ()) {
      return m_sin;
    } else if (m_cos < -eps_f () && m_sin <= eps_f ()) {
      return -m_cos;
    } else {
      return -m_sin;
    }
  }

  /**
   *  @brief A method version of operator*, mainly for automation purposes
   */
  complex_trans concat (const complex_trans &t) const
  {
    complex_trans r (*this);
    r *= t;
    return r;
  }

  /**
   *  @brief A sorting criterion
   */
  bool operator< (const complex_trans &t) const
  {
    if (m_u != t.m_u) {
      return m_u < t.m_u;
    }
    if (fabs (m_sin - t.m_sin) > eps_f ()) {
      return m_sin < t.m_sin;
    }
    if (fabs (m_cos - t.m_cos) > eps_f ()) {
      return m_cos < t.m_cos;
    }
    if (fabs (m_mag - t.m_mag) > eps_f ()) {
      return m_mag < t.m_mag;
    }
    return false;
  }

  /**
   *  @brief A (fuzzy) sorting criterion
   */
  bool less (const complex_trans &t) const
  {
    if (! m_u.equal (t.m_u)) {
      return m_u.less (t.m_u);
    }
    if (fabs (m_sin - t.m_sin) > eps_f ()) {
      return m_sin < t.m_sin;
    }
    if (fabs (m_cos - t.m_cos) > eps_f ()) {
      return m_cos < t.m_cos;
    }
    if (fabs (m_mag - t.m_mag) > eps_f ()) {
      return m_mag < t.m_mag;
    }
    return false;
  }

  /**
   *  @brief Equality test
   */
  bool operator== (const complex_trans &t) const
  {
    return m_u == t.m_u && 
           fabs (m_sin - t.m_sin) <= eps_f () &&
           fabs (m_cos - t.m_cos) <= eps_f () &&
           fabs (m_mag - t.m_mag) <= eps_f ();
  }

  /**
   *  @brief Inequality test
   */
  bool operator!= (const complex_trans &t) const
  {
    return !operator== (t);
  }

  /**
   *  @brief A (fuzzy) equality test
   */
  bool equal (const complex_trans &t) const
  {
    return m_u.equal (t.m_u) && 
           fabs (m_sin - t.m_sin) <= eps_f () &&
           fabs (m_cos - t.m_cos) <= eps_f () &&
           fabs (m_mag - t.m_mag) <= eps_f ();
  }

  /**
   *  @brief Default string conversion
   */
  std::string to_string () const
  {
    return to_string (false);
  }

  /**
   *  @brief String conversion
   *
   *  The lazy and micron flags allow to customize the output somewhat.
   *  When lazy is set to true, output that is not required (i.e. magnification when 1)
   *  is dropped. If dbu is set, the coordinates are multiplied with this factor to render micron units.
   */
  std::string to_string (bool lazy, double dbu = 0.0) const
  {
    std::string s;
    if (is_mirror ()) {
      s += "m";
      s += tl::to_string (angle () * 0.5);
    } else {
      s += "r";
      s += tl::to_string (angle ());
    }
    if (! lazy || is_mag ()) {
      s += tl::sprintf (" *%.9g", mag ());
    }
    s += " ";
    s += m_u.to_string (dbu);
    return s;
  }

private:
  template <class FF, class II, class RR> friend class complex_trans;

  point<F> m_u;
  R m_sin, m_cos;
  R m_mag;
};

/**
 *  @brief Multiplication (concatenation) of transformations
 *
 *  t = t1 * t2 is the resulting transformation that is effectively
 *  applied if first t2 and then t1 is applied.
 *
 *  @param t1 The transformation to apply last
 *  @param t2 The transformation to apply first
 *  @return t1 * t2
 */
template <class I, class F, class R>
inline complex_trans<I, F, R> 
operator* (const complex_trans<I, F, R> &t1, const complex_trans<I, F, R> &t2)
{
  complex_trans<I, F, R> t (t1);
  t *= t2;
  return t;
}

/**
 *  @brief Scaling of a complex transformation with a scalar
 *
 *  The resulting complex transformation will reflex the original one plus
 *  an additional magnification given by the factor m.
 *
 *  @param t The original transformation
 *  @param m The additional magnification
 *  @return t1 * m
 */
template <class I, class F, class R>
inline complex_trans<I, F, R> 
operator* (const complex_trans<I, F, R> &t1, double m)
{
  complex_trans<I, F, R> t (t1);
  t.mag (t.mag () * m);
  return t;
}

/**
 *  @brief Output stream insertion operator
 */
template <class I, class F, class R>
inline std::ostream &
operator<< (std::ostream &os, const complex_trans<I, F, R> &t)
{
  return (os << t.to_string ());
}

/**
 *  @brief A combined transformation
 *  
 *  A combined transformation is the combination of two 
 *  transformations T1 and T2 (T=T1*T2). Although the multiplication
 *  of two transformations may render the same result, but
 *  usually is more efficient. To combine two different
 *  transformations however, the combined_trans template
 *  is better suited.
 */

template <class T1, class T2>
struct combined_trans
{
  typedef typename T2::coord_type coord_type;
  typedef typename T1::target_coord_type target_coord_type;
  typedef combined_trans<typename T2::inverse_trans, typename T1::inverse_trans> inverse_trans;

  /**
   *  @brief Default constructor
   *
   *  Creates a unity transformation
   */
  combined_trans ()
    : t1 (), t2 ()
  {
    // .. nothing else ..
  }

  /**
   *  @brief Standard constructor
   *
   *  Takes two transformations and combines both.
   */
  combined_trans (const T1 &_t1, const T2 &_t2)
    : t1 (_t1), t2 (_t2)
  {
    // .. nothing else ..
  }

  /** 
   *  @brief Inversion
   *
   *  Inverts the transformation and returns the inverted
   *  transformation which swaps T1 and T2 in the type definition.
   *
   *  @return The inverted transformation
   */
  inverse_trans inverted () const
  {
    return inverse_trans (t2.inverted (), t1.inverted ());
  }

  /**
   *  @brief The transformation of a point 
   *
   *  The operator() method transforms the given point.
   *  q = t(p)
   *  
   *  @param p The point to transform
   *  @return The transformed point
   */
  template <class C>
  point<target_coord_type> operator() (const point<C> &p) const
  {
    typedef typename T2::target_coord_type intern_coord_type;
    point<intern_coord_type> q = t2.operator() (p);
    return t1.operator() (q);
  }

  /**
   *  @brief The transformation of a distance 
   *
   *  The ctrans method transforms the given distance.
   *  s = t(d)
   *  
   *  @param d The distance to transform
   *  @return The transformed distance
   */
  template <class C>
  target_coord_type ctrans (C p) const
  {
    typedef typename T2::target_coord_type intern_coord_type;
    intern_coord_type q = t2.ctrans (p);
    return t1.ctrans (q);
  }

  /**
   *  @brief The transformation of a fixpoint transformation 
   *
   *  The ftrans method transforms the given fixpoint transformation.
   *  f = t(f)
   *  
   *  @param f The fixpoint transformation to transform
   *  @return The transformed fixpoint transformation
   */
  template <class D>
  fixpoint_trans<D> ftrans (fixpoint_trans<D> f) const
  {
    return t1 (t2 (f));
  }

  /**
   *  @brief A sorting criterion
   */
  bool operator< (const combined_trans<T1, T2> &t) const
  {
    return t1 < t.t1 || (t1 == t.t1 && t2 < t.t2);
  }

  /**
   *  @brief Equality test
   */
  bool operator== (const combined_trans<T1, T2> &t) const
  {
    return t1 == t.t1 && t2 == t.t2;
  }

  /**
   *  @brief Inequality test
   */
  bool operator!= (const combined_trans<T1, T2> &t) const
  {
    return !operator== (t);
  }

  /**
   *  @brief String conversion
   */
  std::string to_string () const
  {
    std::string s1 = t1.to_string ();
    std::string s2 = t2.to_string ();
    if (! s1.empty () && ! s2.empty ()) {
      return s1 + " " + s2;
    } else {
      return s1 + s2;
    }
  }

  /**
   *  @brief Accessor to the first part of the combined transformation
   */
  const T1 &first () const
  {
    return t1;
  }

  /**
   *  @brief Accessor to the second part of the combined transformation
   */
  const T2 &second () const
  {
    return t2;
  }

  T1 t1;
  T2 t2;
};

/**
 *  @brief Output stream insertion operator
 */
template <class T1, class T2>
inline std::ostream &
operator<< (std::ostream &os, const combined_trans<T1, T2> &t)
{
  return (os << t.to_string ());
}

/**
 *  @brief The standard unit transformation
 */
typedef unit_trans<db::Coord> UnitTrans;

/**
 *  @brief The standard unit transformation for double coordinates
 */
typedef unit_trans<db::DCoord> DUnitTrans;

/**
 *  @brief The standard fixpoint transformation
 */
typedef fixpoint_trans<db::Coord> FTrans;

/**
 *  @brief The standard fixpoint transformation for double types
 */
typedef fixpoint_trans<db::DCoord> DFTrans;

/**
 *  @brief The standard displacement transformation
 */
typedef disp_trans<db::Coord> Disp;

/**
 *  @brief The double coordinate displacement transformation
 */
typedef disp_trans<db::DCoord> DDisp;

/**
 *  @brief The standard transformation
 */
typedef simple_trans<db::Coord> Trans;

/**
 *  @brief The double coordinate transformation
 */
typedef simple_trans<db::DCoord> DTrans;

/**
 *  @brief The standard complex transformation that converts integer to integer coordinates
 */
typedef complex_trans<db::Coord, db::Coord> ICplxTrans;

/**
 *  @brief The standard complex transformation
 */
typedef complex_trans<db::Coord, db::DCoord> CplxTrans;

/**
 *  @brief The double coordinate complex transformation
 */
typedef complex_trans<db::DCoord, db::DCoord> DCplxTrans;

/**
 *  @brief Transformation operator
 *
 *  Transforms the point with the given transformation
 *  (q = T * p).
 *
 *  @param t The transformation to apply
 *  @param p The point to transform
 *  @return The transformed point
 */
template <class C, class Tr> 
inline point<typename Tr::target_coord_type> 
operator* (const Tr &t, const point<C> &p)
{
  return t.operator() (p);
}

/**
 *  @brief Transformation operator
 *
 *  Transforms the vector with the given transformation
 *  (q = T * p).
 *
 *  @param t The transformation to apply
 *  @param p The vector to transform
 *  @return The transformed vector
 */
template <class C, class Tr> 
inline vector<typename Tr::target_coord_type> 
operator* (const Tr &t, const vector<C> &p)
{
  return t.operator() (p);
}

/**
 *  @brief Fuzzy compare function for transformation objects
 */
template <class T>
class trans_less_func 
{
public:
  bool operator() (const T &a, const T &b) const
  {
    return a.less (b);
  }
};

/**
 *  @brief Fuzzy compare function for transformation objects
 */
template <class T>
class trans_equal_func 
{
public:
  bool operator() (const T &a, const T &b) const
  {
    return a.equal (b);
  }
};

} // namespace db

/**
 *  @brief Special extractors for the transformations
 */

namespace tl 
{
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::UnitTrans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::DUnitTrans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::FTrans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::DFTrans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::Trans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::DTrans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::Disp &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::DDisp &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::CplxTrans &t);
  template<> KLAYOUT_DLL void extractor_impl (tl::Extractor &ex, db::DCplxTrans &t);

  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::UnitTrans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::DUnitTrans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::FTrans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::DFTrans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::Trans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::DTrans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::Disp &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::DDisp &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::CplxTrans &t);
  template<> KLAYOUT_DLL bool test_extractor_impl (tl::Extractor &ex, db::DCplxTrans &t);

} // namespace tl

#endif


