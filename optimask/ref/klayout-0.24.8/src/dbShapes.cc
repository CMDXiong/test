
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


#include "dbShape.h"
#include "dbShapeRepository.h"
#include "dbShapes.h"
#include "dbShapes2.h"
#include "dbTrans.h"
#include "dbUserObject.h"
#include "dbLayout.h"

namespace db
{
  
// -------------------------------------------------------------------------------
//  some utilities

template <class Sh, class Iter>
inline db::Shape
iterator_to_shape (db::Shapes *shapes, const db::layer<Sh, db::unstable_layer_tag> &l, const Iter &iter)
{
  //  for unstable containers, we simply use the pointer as a reference
  return db::Shape (shapes, *iter);
}

template <class Sh, class Iter>
inline db::Shape
iterator_to_shape (db::Shapes *shapes, const db::layer<Sh, db::stable_layer_tag> &l, const Iter &iter)
{
  //  for stable containers, we derive the primitive iterator via the pointer 
  return db::Shape (shapes, l.iterator_from_pointer (&*iter));
}

template <class Sh, class Iter>
inline db::Shape
iterator_to_shape (const db::Shapes *shapes, const db::layer<Sh, db::unstable_layer_tag> &l, const Iter &iter)
{
  //  for unstable containers, we simply use the pointer as a reference
  return db::Shape (shapes, *iter);
}

template <class Sh, class Iter>
inline db::Shape
iterator_to_shape (const db::Shapes *shapes, const db::layer<Sh, db::stable_layer_tag> &l, const Iter &iter)
{
  //  for stable containers, we derive the primitive iterator via the pointer 
  return db::Shape (shapes, l.iterator_from_pointer (&*iter));
}

template <class Sh>
inline typename layer<Sh, db::stable_layer_tag>::iterator 
iterator_from_shape (const db::layer<Sh, db::stable_layer_tag> &layer, const db::Shape &shape)
{
  return shape.basic_iter (typename Sh::tag ());
}

template <class Sh>
inline typename layer<Sh, db::unstable_layer_tag>::iterator 
iterator_from_shape (const db::layer<Sh, db::unstable_layer_tag> &layer, const db::Shape &shape)
{
  //  compute the iterator by some pointer arithmetics assuming that layer uses an contiguous container 
  //  in unstable mode ...
  return layer.begin () + (shape.basic_ptr (typename Sh::tag ()) - &*layer.begin ());
}

/**
 *  @brief A predicate returning true if the shape needs translation through translate ()
 */
template <class Sh>
inline bool needs_translate (object_tag<Sh> tag)
{
  return tl::is_equal_type<typename shape_traits<Sh>::can_deref, tl::True> () || tl::is_equal_type<typename shape_traits<Sh>::is_array, tl::True> ();
}

// -------------------------------------------------------------------------------
//  ShapeIterator implementation

ShapeIterator::ShapeIterator ()
  : m_region_mode (None), 
    m_type (Null), 
    m_box (), 
    m_flags (0), 
    mp_shapes (0), 
    mp_prop_sel (0), 
    m_inv_prop_sel (false), 
    m_array_iterator_valid (false), 
    m_editable (false), 
    m_quad_id (0)
{
  m_valid = false;
  m_with_props = false;
}

ShapeIterator::ShapeIterator (const ShapeIterator &d)
  : m_region_mode (None), 
    m_type (Null), 
    m_box (), 
    m_flags (0), 
    mp_shapes (0), 
    mp_prop_sel (0), 
    m_inv_prop_sel (false), 
    m_array_iterator_valid (false), 
    m_editable (false), 
    m_quad_id (0)
{
  m_valid = false;
  m_with_props = false;
  operator= (d);
}

ShapeIterator::ShapeIterator (const shapes_type &shapes, unsigned int flags, const property_selector *prop_sel, bool inv_prop_sel)
  : m_region_mode (None), 
    m_type (object_type (0)), 
    m_box (), 
    m_flags (flags), 
    mp_shapes (&shapes), 
    mp_prop_sel (prop_sel), 
    m_inv_prop_sel (inv_prop_sel), 
    m_array_iterator_valid (false), 
    m_editable (shapes.is_editable ()), 
    m_quad_id (0)
{
  //  optimize: empty property selection plus inverse = no property selection at all
  //            any property selection and not inverse = only shapes with properties
  if (mp_prop_sel) {
    if (mp_prop_sel->empty () && m_inv_prop_sel) {
      mp_prop_sel = 0;
      m_inv_prop_sel = false;
    } else if (! m_inv_prop_sel) {
      m_flags |= Properties;
    }
  }
  m_valid = false;
  m_with_props = false;

  //  look for the first type selected
  for (unsigned int m = 1; m_type != Null && (m_flags & m) == 0; m <<= 1) { 
    m_type = object_type ((unsigned int) m_type + 1);
  }

  advance (0); // validate
}

ShapeIterator::ShapeIterator (const shapes_type &shapes, const box_type &box, region_mode mode, unsigned int flags, const property_selector *prop_sel, bool inv_prop_sel)
  : m_region_mode (mode), 
    m_type (object_type (0)), 
    m_box (box), 
    m_flags (flags), 
    mp_shapes (&shapes), 
    mp_prop_sel (prop_sel), 
    m_inv_prop_sel (inv_prop_sel), 
    m_array_iterator_valid (false), 
    m_editable (shapes.is_editable ()), 
    m_quad_id (0)
{
  //  optimize: empty property selection plus inverse = no property selection at all
  //            any property selection and not inverse = only shapes with properties
  if (mp_prop_sel) {
    if (mp_prop_sel->empty () && m_inv_prop_sel) {
      mp_prop_sel = 0;
      m_inv_prop_sel = false;
    } else if (! m_inv_prop_sel) {
      m_flags |= Properties;
    }
  }
  m_valid = false;
  m_with_props = false;

  //  look for the first type selected
  for (unsigned int m = 1; m_type != Null && (m_flags & m) == 0; m <<= 1) { 
    m_type = object_type ((unsigned int) m_type + 1);
  }

  advance (0); // validate
}

ShapeIterator &
ShapeIterator::operator= (const ShapeIterator &d)
{
  if (&d != this) {

    cleanup ();

    m_d = d.m_d;
    m_valid = d.m_valid;
    m_with_props = d.m_with_props;
    m_region_mode = d.m_region_mode;
    m_type = d.m_type;
    m_box = d.m_box;
    m_shape = d.m_shape;
    m_array = d.m_array;
    m_flags = d.m_flags;
    mp_shapes = d.mp_shapes;
    mp_prop_sel = d.mp_prop_sel;
    m_inv_prop_sel = d.m_inv_prop_sel;
    m_array_iterator_valid = d.m_array_iterator_valid;
    m_editable = d.m_editable;
    m_quad_id = d.m_quad_id;

    if (m_type != Null) {

      if (m_array_iterator_valid) {

        if (m_type == PolygonPtrArray) {

          polygon_ptr_array_iterator_type *d_arr_iter = (polygon_ptr_array_iterator_type *) d.m_ad.iter;
          polygon_ptr_array_iterator_type *arr_iter = (polygon_ptr_array_iterator_type *) m_ad.iter;
          new (arr_iter) polygon_ptr_array_iterator_type (*d_arr_iter);

        } else if (m_type == SimplePolygonPtrArray) {

          simple_polygon_ptr_array_iterator_type *d_arr_iter = (simple_polygon_ptr_array_iterator_type *) d.m_ad.iter;
          simple_polygon_ptr_array_iterator_type *arr_iter = (simple_polygon_ptr_array_iterator_type *) m_ad.iter;
          new (arr_iter) simple_polygon_ptr_array_iterator_type (*d_arr_iter);

        } else if (m_type == PathPtrArray) {

          path_ptr_array_iterator_type *d_arr_iter = (path_ptr_array_iterator_type *) d.m_ad.iter;
          path_ptr_array_iterator_type *arr_iter = (path_ptr_array_iterator_type *) m_ad.iter;
          new (arr_iter) path_ptr_array_iterator_type (*d_arr_iter);

        } else if (m_type == TextPtrArray) {

          text_ptr_array_iterator_type *d_arr_iter = (text_ptr_array_iterator_type *) d.m_ad.iter;
          text_ptr_array_iterator_type *arr_iter = (text_ptr_array_iterator_type *) m_ad.iter;
          new (arr_iter) text_ptr_array_iterator_type (*d_arr_iter);

        } else if (m_type == BoxArray) {

          box_array_iterator_type *d_arr_iter = (box_array_iterator_type *) d.m_ad.iter;
          box_array_iterator_type *arr_iter = (box_array_iterator_type *) m_ad.iter;
          new (arr_iter) box_array_iterator_type (*d_arr_iter);

        } else if (m_type == ShortBoxArray) {

          short_box_array_iterator_type *d_arr_iter = (short_box_array_iterator_type *) d.m_ad.iter;
          short_box_array_iterator_type *arr_iter = (short_box_array_iterator_type *) m_ad.iter;
          new (arr_iter) short_box_array_iterator_type (*d_arr_iter);

        }

      }

    }

  }
  return *this;
}

template <class Iter> 
inline void 
ShapeIterator::skip_array_iter ()
{
  Iter *arr_iter = (Iter *) m_ad.iter;
  arr_iter->~Iter ();
}

void 
ShapeIterator::skip_array ()
{
  if (m_array_iterator_valid) {
    if (m_type == PolygonPtrArray) {
      skip_array_iter<polygon_ptr_array_iterator_type> ();
    } else if (m_type == SimplePolygonPtrArray) {
      skip_array_iter<simple_polygon_ptr_array_iterator_type> ();
    } else if (m_type == PathPtrArray) {
      skip_array_iter<path_ptr_array_iterator_type> ();
    } else if (m_type == TextPtrArray) {
      skip_array_iter<text_ptr_array_iterator_type> ();
    } else if (m_type == BoxArray) {
      skip_array_iter<box_array_iterator_type> ();
    } else if (m_type == ShortBoxArray) {
      skip_array_iter<short_box_array_iterator_type> ();
    }
    m_array_iterator_valid = false;
  }
}

template <class Sh, class StableTag, class RegionTag>
struct advance_algorithm_traits;

template <class Sh, class StableTag>
struct advance_algorithm_traits<Sh, StableTag, ShapeIterator::NoRegionTag>
{
  typedef typename db::layer<Sh, StableTag>::flat_iterator iterator_type;
  typedef typename db::layer<db::object_with_properties<Sh>, StableTag >::flat_iterator iterator_with_props_type;

  inline static void advance (iterator_type *iter, int mode) 
  {
    ++*iter;
  }

  inline static void advance (iterator_with_props_type *iter, int mode) 
  {
    ++*iter;
  }

  inline static size_t quad_id (iterator_type *iter) 
  {
    return 0;
  }

  inline static size_t quad_id (iterator_with_props_type *iter) 
  {
    return 0;
  }

  inline static iterator_type begin (const db::Shapes *shapes, const db::Box &box) 
  {
    //  use get_layer().begin..() in order to suppress update() - this might change the container
    //  while iterating.
    return shapes->template get_layer <Sh, StableTag> ().begin_flat ();
  }

  inline static iterator_with_props_type begin_with_props (const db::Shapes *shapes, const db::Box &box) 
  {
    //  use get_layer().begin..() in order to suppress update() - this might change the container
    //  while iterating.
    return shapes->template get_layer <db::object_with_properties <Sh>, StableTag> ().begin_flat ();
  }
};

template <class Sh, class StableTag>
struct advance_algorithm_traits<Sh, StableTag, ShapeIterator::TouchingRegionTag>
{
  typedef typename db::layer<Sh, StableTag>::touching_iterator iterator_type;
  typedef typename db::layer<db::object_with_properties<Sh>, StableTag >::touching_iterator iterator_with_props_type;

  inline static void advance (iterator_type *iter, int mode) 
  {
    if (mode > 0) {
      ++*iter;
    } else {
      iter->skip_quad ();
    }
  }

  inline static void advance (iterator_with_props_type *iter, int mode) 
  {
    if (mode > 0) {
      ++*iter;
    } else {
      iter->skip_quad ();
    }
  }

  inline static size_t quad_id (iterator_type *iter) 
  {
    return iter->quad_id ();
  }

  inline static size_t quad_id (iterator_with_props_type *iter) 
  {
    return iter->quad_id ();
  }

  inline static iterator_type begin (const db::Shapes *shapes, const db::Box &box) 
  {
    //  use get_layer().begin..() in order to suppress update() - this might change the container
    //  while iterating.
    return shapes->template get_layer <Sh, StableTag> ().begin_touching (box);
  }

  inline static iterator_with_props_type begin_with_props (const db::Shapes *shapes, const db::Box &box) 
  {
    //  use get_layer().begin..() in order to suppress update() - this might change the container
    //  while iterating.
    return shapes->template get_layer <db::object_with_properties <Sh>, StableTag> ().begin_touching (box);
  }
};

template <class Sh, class StableTag>
struct advance_algorithm_traits<Sh, StableTag, ShapeIterator::OverlappingRegionTag>
{
  typedef typename db::layer<Sh, StableTag>::overlapping_iterator iterator_type;
  typedef typename db::layer<db::object_with_properties<Sh>, StableTag >::overlapping_iterator iterator_with_props_type;

  inline static void advance (iterator_type *iter, int mode) 
  {
    if (mode > 0) {
      ++*iter;
    } else {
      iter->skip_quad ();
    }
  }

  inline static void advance (iterator_with_props_type *iter, int mode) 
  {
    if (mode > 0) {
      ++*iter;
    } else {
      iter->skip_quad ();
    }
  }

  inline static size_t quad_id (iterator_type *iter) 
  {
    return iter->quad_id ();
  }

  inline static size_t quad_id (iterator_with_props_type *iter) 
  {
    return iter->quad_id ();
  }

  inline static iterator_type begin (const db::Shapes *shapes, const db::Box &box) 
  {
    //  use get_layer().begin..() in order to suppress update() - this might change the container
    //  while iterating.
    return shapes->template get_layer <Sh, StableTag> ().begin_overlapping (box);
  }

  inline static iterator_with_props_type begin_with_props (const db::Shapes *shapes, const db::Box &box) 
  {
    //  use get_layer().begin..() in order to suppress update() - this might change the container
    //  while iterating.
    return shapes->template get_layer <db::object_with_properties <Sh>, StableTag> ().begin_overlapping (box);
  }
};

template <class Sh, class StableTag, class RegionTag>
bool 
ShapeIterator::advance_shape (int &mode)
{
  typedef advance_algorithm_traits<Sh, StableTag, RegionTag> algorithm_traits;
  typedef typename algorithm_traits::iterator_type iterator_type;
  typedef typename algorithm_traits::iterator_with_props_type iterator_with_props_type;

  if (mode) {

    tl_assert (m_valid);

    if (! m_with_props) {
      iterator_type *iter = (iterator_type *) m_d.iter;
      algorithm_traits::advance (iter, mode);
    } else {
      iterator_with_props_type *iter = (iterator_with_props_type *) m_d.iter;
      do {
        algorithm_traits::advance (iter, mode);
      } while (mp_prop_sel && ! iter->at_end () && (mp_prop_sel->find ((*iter)->properties_id ()) == mp_prop_sel->end ()) != m_inv_prop_sel);
    }

    //  further steps are validation only
    mode = 0;

  }

  bool sel = (m_flags & (1 << (unsigned int) m_type)) != 0;
  bool props_only = (m_flags & Properties) != 0;

  if (! m_with_props && ! props_only) {
  
    iterator_type *iter = (iterator_type *) m_d.iter;

    if (!m_valid && sel) {
      iterator_type i = algorithm_traits::begin (mp_shapes, m_box);
      if (! i.at_end ()) {
        new (iter) iterator_type (i);
        m_valid = true;
      }
    } 

    if (m_valid) {
      if (!sel || iter->at_end ()) {
        m_valid = false;
      } else {
        m_shape = iterator_to_shape (mp_shapes, mp_shapes->template get_layer<Sh, StableTag> (), *iter);
        m_quad_id = algorithm_traits::quad_id (iter);
        return true;
      }
    }

  }

  m_with_props = true;
  
  {

    iterator_with_props_type *iter = (iterator_with_props_type *) m_d.iter;

    if (!m_valid && sel) {
      //  use get_layer().begin_flat() in order to suppress update() - this might change the container
      //  while iterating.
      iterator_with_props_type i = algorithm_traits::begin_with_props (mp_shapes, m_box);
      if (mp_prop_sel) {
        while (! i.at_end () && (mp_prop_sel->find (i->properties_id ()) == mp_prop_sel->end ()) != m_inv_prop_sel) {
          ++i;
        }
      }
      if (! i.at_end ()) {
        new (iter) iterator_with_props_type (i);
        m_valid = true;
      }
    } 

    if (m_valid) {
      if (!sel || iter->at_end ()) {
        m_valid = false;
      } else {
        m_shape = iterator_to_shape (mp_shapes, mp_shapes->template get_layer<db::object_with_properties<Sh>, StableTag> (), *iter);
        m_quad_id = algorithm_traits::quad_id (iter);
        return true;
      }
    }

  }

  m_with_props = false;
  return false;
}

template <class Array>
void 
ShapeIterator::init_array_iter (typename ShapeIterator::NoRegionTag) 
{
  typedef typename Array::iterator array_iterator;

  array_iterator *arr_iter = (array_iterator *) m_ad.iter;
  if (m_with_props) { 
    new (arr_iter) array_iterator (m_array.basic_ptr (typename db::object_with_properties<Array>::tag ())->begin ());
  } else {
    new (arr_iter) array_iterator (m_array.basic_ptr (typename Array::tag ())->begin ());
  }
}

template <class Array>
void 
ShapeIterator::init_array_iter (typename ShapeIterator::TouchingRegionTag) 
{
  typedef typename Array::iterator array_iterator;
  typedef typename Array::object_type shape_ptr_type;

  array_iterator *arr_iter = (array_iterator *) m_ad.iter;
  db::box_convert<shape_ptr_type> bc;
  new (arr_iter) array_iterator (m_array.basic_ptr (typename Array::tag ())->begin_touching (m_box, bc));
}

template <class Array>
void 
ShapeIterator::init_array_iter (typename ShapeIterator::OverlappingRegionTag) 
{
  typedef typename Array::iterator array_iterator;
  typedef typename Array::object_type shape_ptr_type;

  array_iterator *arr_iter = (array_iterator *) m_ad.iter;
  db::box_convert<shape_ptr_type> bc;
  box_type box (m_box);
  box.enlarge (point_type (-1, -1));
  new (arr_iter) array_iterator (m_array.basic_ptr (typename Array::tag ())->begin_touching (m_box, bc));
}

template <class Array, class StableTag, class RegionTag>
bool 
ShapeIterator::advance_aref (int &mode)
{
  typedef typename Array::iterator array_iterator;
  typedef Array array_type;

  if (mode && m_array_iterator_valid) {

    if (mode > 0) {
      array_iterator *arr_iter = (array_iterator *) m_ad.iter;
      ++*arr_iter;
    } else {
      //  skip quad -> skip rest of array and move to next shape array
      skip_array ();  //  sets m_array_iterator_valid = false
    }

  } 

  while (true) {

    if (m_array_iterator_valid) {
      array_iterator *arr_iter = (array_iterator *) m_ad.iter;
      if (! arr_iter->at_end ()) {
        break;
      } else {
        arr_iter->~array_iterator ();
        m_array_iterator_valid = false;
        mode = 1; // force move to next item in increment mode
      }
    }

    //  move to next item (increment on mode == 1, skip quad on mode == -1) or validate this one (if mode == 0)
    if (! advance_shape<Array, StableTag, RegionTag> (mode)) {
      return false;
    }

    m_array = m_shape;
    RegionTag region_tag;
    init_array_iter <Array> (region_tag);
    m_array_iterator_valid = true;

  }

  array_iterator *arr_iter = (array_iterator *) m_ad.iter;
  typename array_iterator::result_type t = **arr_iter;

  //  HINT: since the array references store "pointers" without an intrinsic 
  //        transformation, we can drop this:
  //  t = t * (*iter)->obj ().trans (); 

  //  This creates a local reference object to reference an array member
  if (m_editable) {
    if (m_with_props) {
      m_shape = shape_type (mp_shapes, m_array.basic_iter (typename db::object_with_properties<Array>::tag ()), t);
    } else {
      m_shape = shape_type (mp_shapes, m_array.basic_iter (typename Array::tag ()), t);
    }
  } else {
    if (m_with_props) {
      m_shape = shape_type (mp_shapes, *m_array.basic_ptr (typename db::object_with_properties<Array>::tag ()), t);
    } else {
      m_shape = shape_type (mp_shapes, *m_array.basic_ptr (typename Array::tag ()), t);
    }
  }

  return true;
}

template <class RegionTag, class StableTag>
void 
ShapeIterator::advance_generic (int mode)
{
  while (m_type != Null) {

    switch (m_type) {
    case Polygon:
      if (advance_shape<polygon_type, StableTag, RegionTag> (mode)) return;
      break;
    case PolygonRef:
      if (advance_shape<polygon_ref_type, StableTag, RegionTag> (mode)) return;
      break;
    case PolygonPtrArray:
      if (advance_aref<polygon_ptr_array_type, StableTag, RegionTag> (mode)) return;
      break;
    case SimplePolygon:
      if (advance_shape<simple_polygon_type, StableTag, RegionTag> (mode)) return;
      break;
    case SimplePolygonRef:
      if (advance_shape<simple_polygon_ref_type, StableTag, RegionTag> (mode)) return;
      break;
    case SimplePolygonPtrArray:
      if (advance_aref<simple_polygon_ptr_array_type, StableTag, RegionTag> (mode)) return;
      break;
    case Edge:
      if (advance_shape<edge_type, StableTag, RegionTag> (mode)) return;
      break;
    case Path:
      if (advance_shape<path_type, StableTag, RegionTag> (mode)) return;
      break;
    case PathRef:
      if (advance_shape<path_ref_type, StableTag, RegionTag> (mode)) return;
      break;
    case PathPtrArray:
      if (advance_aref<path_ptr_array_type, StableTag, RegionTag> (mode)) return;
      break;
    case Box:
      if (advance_shape<box_type, StableTag, RegionTag> (mode)) return;
      break;
    case BoxArray:
      if (advance_aref<box_array_type, StableTag, RegionTag> (mode)) return;
      break;
    case ShortBox:
      if (advance_shape<short_box_type, StableTag, RegionTag> (mode)) return;
      break;
    case ShortBoxArray:
      if (advance_aref<short_box_array_type, StableTag, RegionTag> (mode)) return;
      break;
    case Text:
      if (advance_shape<text_type, StableTag, RegionTag> (mode)) return;
      break;
    case TextRef:
      if (advance_shape<text_ref_type, StableTag, RegionTag> (mode)) return;
      break;
    case TextPtrArray:
      if (advance_aref<text_ptr_array_type, StableTag, RegionTag> (mode)) return;
      break;
    case UserObject:
      if (advance_shape<user_object_type, StableTag, RegionTag> (mode)) return;
      break;
    default:
      break;
    }

    //  look for the next type selected
    m_type = object_type ((unsigned int) m_type + 1);
    for (unsigned int m = 1 << (unsigned int) m_type; m_type != Null && (m_flags & m) == 0; m <<= 1) { 
      m_type = object_type ((unsigned int) m_type + 1);
    }

  }
}

void
ShapeIterator::finish_array ()
{
  skip_array ();
  advance (1);
}

void 
ShapeIterator::advance (int mode)
{
  if (m_editable) {
    if (m_region_mode == None) {
      advance_generic<NoRegionTag, db::stable_layer_tag> (mode);
    } else if (m_region_mode == Touching) {
      advance_generic<TouchingRegionTag, db::stable_layer_tag> (mode);
    } else if (m_region_mode == Overlapping) {
      advance_generic<OverlappingRegionTag, db::stable_layer_tag> (mode);
    }
  } else {
    if (m_region_mode == None) {
      advance_generic<NoRegionTag, db::unstable_layer_tag> (mode);
    } else if (m_region_mode == Touching) {
      advance_generic<TouchingRegionTag, db::unstable_layer_tag> (mode);
    } else if (m_region_mode == Overlapping) {
      advance_generic<OverlappingRegionTag, db::unstable_layer_tag> (mode);
    }
  }
}

template <class Sh, class StableTag>
db::Box 
ShapeIterator::quad_box_by_shape (typename ShapeIterator::TouchingRegionTag) const
{
  tl_assert (m_valid);
  if (! m_with_props) {
    typename db::layer<Sh, StableTag>::touching_iterator *iter = (typename db::layer<Sh, StableTag>::touching_iterator *) m_d.iter;
    return iter->quad_box ();
  } else {
    typename db::layer<db::object_with_properties<Sh>, StableTag >::touching_iterator *iter = (typename db::layer< db::object_with_properties<Sh>, StableTag>::touching_iterator *) m_d.iter;
    return iter->quad_box ();
  }
}

template <class Sh, class StableTag>
db::Box 
ShapeIterator::quad_box_by_shape (typename ShapeIterator::OverlappingRegionTag) const
{
  tl_assert (m_valid);
  if (! m_with_props) {
    typename db::layer<Sh, StableTag>::overlapping_iterator *iter = (typename db::layer<Sh, StableTag>::overlapping_iterator *) m_d.iter;
    return iter->quad_box ();
  } else {
    typename db::layer<db::object_with_properties<Sh>, StableTag >::overlapping_iterator *iter = (typename db::layer< db::object_with_properties<Sh>, StableTag>::overlapping_iterator *) m_d.iter;
    return iter->quad_box ();
  }
}

template <class RegionTag, class StableTag>
db::Box 
ShapeIterator::quad_box_generic () const
{
  RegionTag region_tag = RegionTag ();

  switch (m_type) {
  case Polygon:
    return (quad_box_by_shape<polygon_type, StableTag> (region_tag));
  case PolygonRef:
    return (quad_box_by_shape<polygon_ref_type, StableTag> (region_tag));
  case PolygonPtrArray:
    return (quad_box_by_shape<polygon_ptr_array_type, StableTag> (region_tag));
  case SimplePolygon:
    return (quad_box_by_shape<simple_polygon_type, StableTag> (region_tag));
  case SimplePolygonRef:
    return (quad_box_by_shape<simple_polygon_ref_type, StableTag> (region_tag));
  case SimplePolygonPtrArray:
    return (quad_box_by_shape<simple_polygon_ptr_array_type, StableTag> (region_tag));
  case Edge:
    return (quad_box_by_shape<edge_type, StableTag> (region_tag));
  case Path:
    return (quad_box_by_shape<path_type, StableTag> (region_tag));
  case PathRef:
    return (quad_box_by_shape<path_ref_type, StableTag> (region_tag));
  case PathPtrArray:
    return (quad_box_by_shape<path_ptr_array_type, StableTag> (region_tag));
  case Box:
    return (quad_box_by_shape<box_type, StableTag> (region_tag));
  case BoxArray:
    return (quad_box_by_shape<box_array_type, StableTag> (region_tag));
  case ShortBox:
    return (quad_box_by_shape<short_box_type, StableTag> (region_tag));
  case ShortBoxArray:
    return (quad_box_by_shape<short_box_array_type, StableTag> (region_tag));
  case Text:
    return (quad_box_by_shape<text_type, StableTag> (region_tag));
  case TextRef:
    return (quad_box_by_shape<text_ref_type, StableTag> (region_tag));
  case TextPtrArray:
    return (quad_box_by_shape<text_ptr_array_type, StableTag> (region_tag));
  case UserObject:
    return (quad_box_by_shape<user_object_type, StableTag> (region_tag));
  default:
    return db::Box ();
  }
}

db::Box 
ShapeIterator::quad_box () const
{
  if (m_editable) {
    if (m_region_mode == None) {
      return db::Box::world ();
    } else if (m_region_mode == Touching) {
      return quad_box_generic<TouchingRegionTag, db::stable_layer_tag> ();
    } else if (m_region_mode == Overlapping) {
      return quad_box_generic<OverlappingRegionTag, db::stable_layer_tag> ();
    }
  } else {
    if (m_region_mode == None) {
      return db::Box::world ();
    } else if (m_region_mode == Touching) {
      return quad_box_generic<TouchingRegionTag, db::unstable_layer_tag> ();
    } else if (m_region_mode == Overlapping) {
      return quad_box_generic<OverlappingRegionTag, db::unstable_layer_tag> ();
    }
  }
  return db::Box ();
}

void 
ShapeIterator::cleanup ()
{
  //  this trick destroys all iterators that have been allocated in the generic union
  if (m_type != Null) {

    skip_array ();
    m_flags = 0;
    advance (0);
    tl_assert (m_type == Null);

  }
}

// ---------------------------------------------------------------------------------------
//  layer_op implementation

template <class Sh, class StableTag>
void 
layer_op<Sh, StableTag>::insert (Shapes *shapes)
{
  shapes->insert (m_shapes.begin (), m_shapes.end ());
}

template <class Sh, class StableTag>
void 
layer_op<Sh, StableTag>::erase (Shapes *shapes)
{
  if (shapes->size (typename Sh::tag (), StableTag ()) <= m_shapes.size ()) {
    //  If all shapes are to be removed, just clear the shapes
    shapes->erase (typename Sh::tag (), StableTag (), shapes->begin (typename Sh::tag (), StableTag ()), shapes->end (typename Sh::tag (), StableTag ()));
  } else {

    //  look up the shapes to delete and collect them in a sorted list. Then pass this to 
    //  the erase method of the shapes object
    std::vector<bool> done;
    done.resize (m_shapes.size (), false);

    std::sort (m_shapes.begin (), m_shapes.end ());

    typename std::vector<Sh>::const_iterator s_begin = m_shapes.begin ();
    typename std::vector<Sh>::const_iterator s_end = m_shapes.end ();

    std::vector<typename db::layer<Sh, StableTag>::iterator> to_erase;
    to_erase.reserve (m_shapes.size ());

    //  This is not quite effective but seems to be the simpliest way
    //  of implementing this: search for each element and erase these.
    for (typename db::layer<Sh, StableTag>::iterator lsh = shapes->begin (typename Sh::tag (), StableTag ()); lsh != shapes->end (typename Sh::tag (), StableTag ()); ++lsh) {
      typename std::vector<Sh>::const_iterator s = std::lower_bound (s_begin, s_end, *lsh);
      while (s != s_end && done [std::distance(s_begin, s)] && *s == *lsh) {
        ++s;
      }
      if (s != s_end && *s == *lsh) {
        done [std::distance(s_begin, s)] = true;
        to_erase.push_back (lsh);
      }
    }

    shapes->erase_positions (typename Sh::tag (), StableTag (), to_erase.begin (), to_erase.end ());

  }
}

// ---------------------------------------------------------------------------------------
//  Shapes implementation

Shapes &
Shapes::operator= (const Shapes &d)
{
  if (&d != this) {
    clear ();
    do_insert (d);
  }

  return *this;
}

db::Layout *
Shapes::layout () const 
{
  db::Cell *c = cell ();
  return c ? c->layout () : 0;
}

void
Shapes::insert (const Shapes &d)
{
  //  no undo support for this currently
  tl_assert (! manager () || ! manager ()->transacting ());
  do_insert (d);
}

void
Shapes::do_insert (const Shapes &d)
{
  //  shortcut for "nothing to do"
  if (d.empty ()) {
    return;
  }

  if (layout () == d.layout ()) {

    //  both shape containers reside in the same repository space - simply copy
    m_layers.reserve (d.m_layers.size ());
    for (tl::vector<LayerBase *>::const_iterator l = d.m_layers.begin (); l != d.m_layers.end (); ++l) {
      m_layers.push_back ((*l)->clone (this, manager ()));
    }

  } else if (layout () == 0) {

    //  the target is standalone - dereference
    for (tl::vector<LayerBase *>::const_iterator l = d.m_layers.begin (); l != d.m_layers.end (); ++l) {
      (*l)->deref_into (this);
    }

  } else {

    //  both shape containers are in separate spaces - translate
    for (tl::vector<LayerBase *>::const_iterator l = d.m_layers.begin (); l != d.m_layers.end (); ++l) {
      (*l)->translate_into (this, shape_repository (), array_repository ());
    }

  }
}

//  get the shape repository associated with this container
db::GenericRepository &
Shapes::shape_repository () const 
{
  return layout ()->shape_repository ();
}

//  get the array repository associated with this container
db::ArrayRepository &
Shapes::array_repository () const 
{
  return layout ()->array_repository ();
}

void
Shapes::invalidate_state ()
{
  if (! is_dirty ()) {
    set_dirty (true);
    if (layout ()) {
      layout ()->invalidate_bboxes ();
    }
  }
}

void  
Shapes::swap (Shapes &d)
{
  // HINT: undo support for swap is implemented one level above (i.e. in the cell) since
  // two Shapes objects are involved.
  d.invalidate_state ();  //  HINT: must come before the change is done!
  invalidate_state ();
  m_layers.swap (d.m_layers);
}

Shapes::shape_type 
Shapes::do_insert (const Shapes::shape_type &shape, const Shapes::unit_trans_type &t, tl::func_delegate_base <db::properties_id_type> &pm)
{
  switch (shape.m_type) {
  case shape_type::Null:
  default:
    return shape_type ();
  case shape_type::Polygon:
    return shape_type (insert_by_tag (shape_type::polygon_type::tag (), shape, pm));
  case shape_type::PolygonRef:
  case shape_type::PolygonPtrArrayMember:
    if (! layout ()) {
      shape_type::polygon_type p;
      shape.polygon (p);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::polygon_type> (p, pm (shape.prop_id ())));
      }
    } else if (shape.m_type == shape_type::PolygonRef) {
      return shape_type (insert_by_tag (shape_type::polygon_ref_type::tag (), shape, shape_repository (), pm));
    } else {
      shape_type::polygon_ref_type s (shape.polygon_ref ());
      if (! shape.has_prop_id ()) {
        return insert (shape_type::polygon_ref_type (s, shape_repository ()));
      } else {
        typedef db::object_with_properties<shape_type::polygon_ref_type> swp_type;
        return insert (swp_type (shape_type::polygon_ref_type (s, shape_repository ()), pm (shape.prop_id ())));
      }
    }
  case shape_type::PolygonPtrArray:
    tl_assert (layout () != 0);  //  cannot translate the array members
    return shape_type (insert_array_by_tag (shape_type::polygon_ptr_array_type::tag (), shape, shape_repository (), pm));
  case shape_type::SimplePolygon:
    return (insert_by_tag (shape_type::simple_polygon_type::tag (), shape, pm));
  case shape_type::SimplePolygonRef:
  case shape_type::SimplePolygonPtrArrayMember:
    if (! layout ()) {
      shape_type::simple_polygon_type p;
      shape.simple_polygon (p);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::simple_polygon_type> (p, pm (shape.prop_id ())));
      }
    } else if (shape.m_type == shape_type::SimplePolygonRef) {
      return (insert_by_tag (shape_type::simple_polygon_ref_type::tag (), shape, shape_repository (), pm));
    } else {
      shape_type::simple_polygon_ref_type s (shape.simple_polygon_ref ());
      if (! shape.has_prop_id ()) {
        return insert (shape_type::simple_polygon_ref_type (s, shape_repository ()));
      } else {
        typedef db::object_with_properties<shape_type::simple_polygon_ref_type> swp_type;
        return insert (swp_type (shape_type::simple_polygon_ref_type (s, shape_repository ()), pm (shape.prop_id ())));
      }
    }
  case shape_type::SimplePolygonPtrArray:
    tl_assert (layout () != 0);  //  cannot translate the array members
    return (insert_array_by_tag (shape_type::simple_polygon_ptr_array_type::tag (), shape, shape_repository (), pm));
  case shape_type::Edge:
    return (insert_by_tag (shape_type::edge_type::tag (), shape, pm));
  case shape_type::Path:
    return (insert_by_tag (shape_type::path_type::tag (), shape, pm));
  case shape_type::PathRef:
  case shape_type::PathPtrArrayMember:
    if (! layout ()) {
      shape_type::path_type p;
      shape.path (p);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::path_type> (p, pm (shape.prop_id ())));
      }
    } else if (shape.m_type == shape_type::PathRef) {
      return (insert_by_tag (shape_type::path_ref_type::tag (), shape, shape_repository (), pm));
    } else {
      shape_type::path_ref_type s (shape.path_ref ());
      if (! shape.has_prop_id ()) {
        return insert (shape_type::path_ref_type (s, shape_repository ()));
      } else {
        typedef db::object_with_properties<shape_type::path_ref_type> swp_type;
        return insert (swp_type (shape_type::path_ref_type (s, shape_repository ()), pm (shape.prop_id ())));
      }
    }
  case shape_type::PathPtrArray:
    tl_assert (layout () != 0);  //  cannot translate the array members
    return (insert_array_by_tag (shape_type::path_ptr_array_type::tag (), shape, shape_repository (), pm));
  case shape_type::Box:
    return (insert_by_tag (shape_type::box_type::tag (), shape, pm));
  case shape_type::BoxArrayMember:
    {
      shape_type::box_type s (shape.box ());
      if (! shape.has_prop_id ()) {
        return insert (s);
      } else {
        typedef db::object_with_properties<shape_type::box_type> swp_type;
        return insert (swp_type (s, pm (shape.prop_id ())));
      }
    }
  case shape_type::BoxArray:
    return (insert_by_tag (shape_type::box_array_type::tag (), shape, pm));
  case shape_type::ShortBox:
    return (insert_by_tag (shape_type::short_box_type::tag (), shape, pm));
  case shape_type::ShortBoxArrayMember:
    {
      shape_type::short_box_type s (shape.box ());
      if (! shape.has_prop_id ()) {
        return insert (s);
      } else {
        typedef db::object_with_properties<shape_type::short_box_type> swp_type;
        return insert (swp_type (s, pm (shape.prop_id ())));
      }
    }
  case shape_type::ShortBoxArray:
    return (insert_by_tag (shape_type::short_box_array_type::tag (), shape, pm));
  case shape_type::Text:
  case shape_type::TextRef:
  case shape_type::TextPtrArrayMember:
    {
      //  because texts can refer to a string repository we go the safe way and 
      //  simply instantiate and re-insert the text:
      shape_type::text_type p;
      shape.text (p);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::text_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::TextPtrArray:
    tl_assert (layout () != 0);  //  cannot translate the array members
    return insert_array_by_tag (shape_type::text_ptr_array_type::tag (), shape, shape_repository (), pm);
  case shape_type::UserObject:
    return insert_by_tag (shape_type::user_object_type::tag (), shape, pm);
  };
}

template <class Trans>
Shapes::shape_type 
Shapes::do_insert (const Shapes::shape_type &shape, const Trans &t, tl::func_delegate_base <db::properties_id_type> &pm)
{
  switch (shape.m_type) {
  case shape_type::Null:
  default:
    return shape;
  case shape_type::Polygon:
    {
      shape_type::polygon_type p (shape.polygon ());
      //  Hint: we don't compress so we don't loose information
      p.transform (t, false);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::polygon_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::PolygonRef:
  case shape_type::PolygonPtrArrayMember:
    {
      shape_type::polygon_type p;
      shape.polygon (p);
      //  Hint: we don't compress so we don't loose information
      p.transform (t, false);
      //  TODO: could create a reference again, but this is what a transform would to as well.
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::polygon_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::SimplePolygon:
    {
      shape_type::simple_polygon_type p (shape.simple_polygon ());
      //  Hint: we don't compress so we don't loose information
      p.transform (t, false);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::simple_polygon_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::SimplePolygonRef:
  case shape_type::SimplePolygonPtrArrayMember:
    {
      shape_type::simple_polygon_type p;
      shape.simple_polygon (p);
      //  Hint: we don't compress so we don't loose information
      p.transform (t, false);
      //  TODO: could create a reference again, but this is what a transform would to as well.
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::simple_polygon_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::Edge:
    {
      shape_type::edge_type p (shape.edge ());
      p.transform (t);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::edge_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::Path:
    {
      shape_type::path_type p (shape.path ());
      p.transform (t);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::path_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::PathRef:
  case shape_type::PathPtrArrayMember:
    {
      shape_type::path_type p;
      shape.path (p);
      p.transform (t);
      //  TODO: could create a reference again, but this is what a transform would to as well.
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::path_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::Box:
  case shape_type::BoxArrayMember:
  case shape_type::ShortBox:
  case shape_type::ShortBoxArrayMember:
    {
      if (t.is_ortho ()) {
        shape_type::box_type p (shape.box ());
        p.transform (t);
        if (! shape.has_prop_id ()) {
          return insert (p);
        } else {
          return insert (db::object_with_properties<shape_type::box_type> (p, pm (shape.prop_id ())));
        }
      } else {
        //  A box cannot stay a box in this case ...
        shape_type::simple_polygon_type p (shape.box ());
        p.transform (t);
        if (! shape.has_prop_id ()) {
          return insert (p);
        } else {
          return insert (db::object_with_properties<shape_type::simple_polygon_type> (p, pm (shape.prop_id ())));
        }
      }
    }
  case shape_type::Text:
    {
      shape_type::text_type p (shape.text ());
      p.transform (t);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::text_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::TextRef:
  case shape_type::TextPtrArrayMember:
    {
      shape_type::text_type p;
      shape.text (p);
      p.transform (t);
      //  TODO: could create a reference again, but this is what a transform would to as well.
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::text_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::UserObject:
    {
      shape_type::user_object_type p (shape.user_object ());
      p.transform (t);
      if (! shape.has_prop_id ()) {
        return insert (p);
      } else {
        return insert (db::object_with_properties<shape_type::user_object_type> (p, pm (shape.prop_id ())));
      }
    }
  case shape_type::PolygonPtrArray:
  case shape_type::SimplePolygonPtrArray:
  case shape_type::PathPtrArray:
  case shape_type::BoxArray:
  case shape_type::ShortBoxArray:
  case shape_type::TextPtrArray:
    //  Arrays are not supported yet 
    //  TODO: implement
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'insert' with transformation does not support shape arrays")));
  };
}

Shapes::shape_type 
Shapes::find (const Shapes::shape_type &shape) const
{
  switch (shape.m_type) {
  case shape_type::Null:
  default:
    return shape_type ();
  case shape_type::Polygon:
    return find_shape_by_tag (shape_type::polygon_type::tag (), shape);
  case shape_type::PolygonRef:
    return find_shape_by_tag (shape_type::polygon_ref_type::tag (), shape);
  case shape_type::PolygonPtrArrayMember:
  case shape_type::PolygonPtrArray:
    return find_shape_by_tag (shape_type::polygon_ptr_array_type::tag (), shape);
  case shape_type::SimplePolygon:
    return find_shape_by_tag (shape_type::simple_polygon_type::tag (), shape);
  case shape_type::SimplePolygonRef:
    return find_shape_by_tag (shape_type::simple_polygon_ref_type::tag (), shape);
  case shape_type::SimplePolygonPtrArrayMember:
  case shape_type::SimplePolygonPtrArray:
    return find_shape_by_tag (shape_type::simple_polygon_ptr_array_type::tag (), shape);
  case shape_type::Edge:
    return find_shape_by_tag (shape_type::edge_type::tag (), shape);
  case shape_type::Path:
    return find_shape_by_tag (shape_type::path_type::tag (), shape);
  case shape_type::PathRef:
    return find_shape_by_tag (shape_type::path_ref_type::tag (), shape);
  case shape_type::PathPtrArrayMember:
  case shape_type::PathPtrArray:
    return find_shape_by_tag (shape_type::path_ptr_array_type::tag (), shape);
  case shape_type::Box:
    return find_shape_by_tag (shape_type::box_type::tag (), shape);
  case shape_type::BoxArrayMember:
  case shape_type::BoxArray:
    return find_shape_by_tag (shape_type::box_array_type::tag (), shape);
  case shape_type::ShortBox:
    return find_shape_by_tag (shape_type::short_box_type::tag (), shape);
  case shape_type::ShortBoxArrayMember:
  case shape_type::ShortBoxArray:
    return find_shape_by_tag (shape_type::short_box_array_type::tag (), shape);
  case shape_type::Text:
    return find_shape_by_tag (shape_type::text_type::tag (), shape);
  case shape_type::TextRef:
    return find_shape_by_tag (shape_type::text_ref_type::tag (), shape);
  case shape_type::TextPtrArrayMember:
  case shape_type::TextPtrArray:
    return find_shape_by_tag (shape_type::text_ptr_array_type::tag (), shape);
  case shape_type::UserObject:
    return find_shape_by_tag (shape_type::user_object_type::tag (), shape);
  };
}

bool 
Shapes::is_valid (const Shapes::shape_type &shape) const
{
  switch (shape.m_type) {
  case shape_type::Null:
  default:
    return false;
  case shape_type::Polygon:
    return is_valid_shape_by_tag (shape_type::polygon_type::tag (), shape);
  case shape_type::PolygonRef:
    return is_valid_shape_by_tag (shape_type::polygon_ref_type::tag (), shape);
  case shape_type::PolygonPtrArrayMember:
  case shape_type::PolygonPtrArray:
    return is_valid_shape_by_tag (shape_type::polygon_ptr_array_type::tag (), shape);
  case shape_type::SimplePolygon:
    return is_valid_shape_by_tag (shape_type::simple_polygon_type::tag (), shape);
  case shape_type::SimplePolygonRef:
    return is_valid_shape_by_tag (shape_type::simple_polygon_ref_type::tag (), shape);
  case shape_type::SimplePolygonPtrArrayMember:
  case shape_type::SimplePolygonPtrArray:
    return is_valid_shape_by_tag (shape_type::simple_polygon_ptr_array_type::tag (), shape);
  case shape_type::Edge:
    return is_valid_shape_by_tag (shape_type::edge_type::tag (), shape);
  case shape_type::Path:
    return is_valid_shape_by_tag (shape_type::path_type::tag (), shape);
  case shape_type::PathRef:
    return is_valid_shape_by_tag (shape_type::path_ref_type::tag (), shape);
  case shape_type::PathPtrArrayMember:
  case shape_type::PathPtrArray:
    return is_valid_shape_by_tag (shape_type::path_ptr_array_type::tag (), shape);
  case shape_type::Box:
    return is_valid_shape_by_tag (shape_type::box_type::tag (), shape);
  case shape_type::BoxArrayMember:
  case shape_type::BoxArray:
    return is_valid_shape_by_tag (shape_type::box_array_type::tag (), shape);
  case shape_type::ShortBox:
    return is_valid_shape_by_tag (shape_type::short_box_type::tag (), shape);
  case shape_type::ShortBoxArrayMember:
  case shape_type::ShortBoxArray:
    return is_valid_shape_by_tag (shape_type::short_box_array_type::tag (), shape);
  case shape_type::Text:
    return is_valid_shape_by_tag (shape_type::text_type::tag (), shape);
  case shape_type::TextRef:
    return is_valid_shape_by_tag (shape_type::text_ref_type::tag (), shape);
  case shape_type::TextPtrArrayMember:
  case shape_type::TextPtrArray:
    return is_valid_shape_by_tag (shape_type::text_ptr_array_type::tag (), shape);
  case shape_type::UserObject:
    return is_valid_shape_by_tag (shape_type::user_object_type::tag (), shape);
  };
}

void 
Shapes::erase_shape (const Shapes::shape_type &shape)
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'erase' is permitted only in editable mode")));
  }

  switch (shape.m_type) {
  case shape_type::Null:
    break;
  case shape_type::Polygon:
    erase_shape_by_tag (shape_type::polygon_type::tag (), shape);
    break;
  case shape_type::PolygonRef:
    erase_shape_by_tag (shape_type::polygon_ref_type::tag (), shape);
    break;
  case shape_type::PolygonPtrArrayMember:
    //  HINT: since we are in editing mode, this should never happen:
  case shape_type::PolygonPtrArray:
    //  HINT: since we are in editing mode, this should never happen:
    erase_shape_by_tag (shape_type::polygon_ptr_array_type::tag (), shape);
    break;
  case shape_type::SimplePolygon:
    erase_shape_by_tag (shape_type::simple_polygon_type::tag (), shape);
    break;
  case shape_type::SimplePolygonRef:
    erase_shape_by_tag (shape_type::simple_polygon_ref_type::tag (), shape);
    break;
  case shape_type::SimplePolygonPtrArrayMember:
    //  HINT: since we are in editing mode, this should never happen:
  case shape_type::SimplePolygonPtrArray:
    //  HINT: since we are in editing mode, this should never happen:
    erase_shape_by_tag (shape_type::simple_polygon_ptr_array_type::tag (), shape);
    break;
  case shape_type::Edge:
    erase_shape_by_tag (shape_type::edge_type::tag (), shape);
    break;
  case shape_type::Path:
    erase_shape_by_tag (shape_type::path_type::tag (), shape);
    break;
  case shape_type::PathRef:
    erase_shape_by_tag (shape_type::path_ref_type::tag (), shape);
    break;
  case shape_type::PathPtrArrayMember:
    //  HINT: since we are in editing mode, this should never happen:
  case shape_type::PathPtrArray:
    //  HINT: since we are in editing mode, this should never happen:
    erase_shape_by_tag (shape_type::path_ptr_array_type::tag (), shape);
    break;
  case shape_type::Box:
    erase_shape_by_tag (shape_type::box_type::tag (), shape);
    break;
  case shape_type::BoxArrayMember:
    //  HINT: since we are in editing mode, this should never happen:
  case shape_type::BoxArray:
    //  HINT: since we are in editing mode, this should never happen:
    erase_shape_by_tag (shape_type::box_array_type::tag (), shape);
    break;
  case shape_type::ShortBox:
    erase_shape_by_tag (shape_type::short_box_type::tag (), shape);
    break;
  case shape_type::ShortBoxArrayMember:
    //  HINT: since we are in editing mode, this should never happen:
  case shape_type::ShortBoxArray:
    //  HINT: since we are in editing mode, this should never happen:
    erase_shape_by_tag (shape_type::short_box_array_type::tag (), shape);
    break;
  case shape_type::Text:
    erase_shape_by_tag (shape_type::text_type::tag (), shape);
    break;
  case shape_type::TextRef:
    erase_shape_by_tag (shape_type::text_ref_type::tag (), shape);
    break;
  case shape_type::TextPtrArrayMember:
    //  HINT: since we are in editing mode, this should never happen:
  case shape_type::TextPtrArray:
    //  HINT: since we are in editing mode, this should never happen:
    erase_shape_by_tag (shape_type::text_ptr_array_type::tag (), shape);
    break;
  case shape_type::UserObject:
    erase_shape_by_tag (shape_type::user_object_type::tag (), shape);
    break;
  };
}

void 
Shapes::erase_shapes (const std::vector<Shapes::shape_type> &shapes)
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'erase' is permitted only in editable mode")));
  }

  for (std::vector<shape_type>::const_iterator s = shapes.begin (); s != shapes.end (); ) {

    std::vector<shape_type>::const_iterator snext = s;
    while (snext != shapes.end () && snext->type () == s->type () && snext->has_prop_id () == s->has_prop_id ()) {
      ++snext;
    }

    switch (s->m_type) {
    case shape_type::Null:
      break;
    case shape_type::Polygon:
      erase_shapes_by_tag (shape_type::polygon_type::tag (), s, snext);
      break;
    case shape_type::PolygonRef:
      erase_shapes_by_tag (shape_type::polygon_ref_type::tag (), s, snext);
      break;
    case shape_type::PolygonPtrArrayMember:
      //  HINT: since we are in editing mode, this should never happen:
    case shape_type::PolygonPtrArray:
      //  HINT: since we are in editing mode, this should never happen:
      erase_shapes_by_tag (shape_type::polygon_ptr_array_type::tag (), s, snext);
      break;
    case shape_type::SimplePolygon:
      erase_shapes_by_tag (shape_type::simple_polygon_type::tag (), s, snext);
      break;
    case shape_type::SimplePolygonRef:
      erase_shapes_by_tag (shape_type::simple_polygon_ref_type::tag (), s, snext);
      break;
    case shape_type::SimplePolygonPtrArrayMember:
      //  HINT: since we are in editing mode, this should never happen:
    case shape_type::SimplePolygonPtrArray:
      //  HINT: since we are in editing mode, this should never happen:
      erase_shapes_by_tag (shape_type::simple_polygon_ptr_array_type::tag (), s, snext);
      break;
    case shape_type::Edge:
      erase_shapes_by_tag (shape_type::edge_type::tag (), s, snext);
      break;
    case shape_type::Path:
      erase_shapes_by_tag (shape_type::path_type::tag (), s, snext);
      break;
    case shape_type::PathRef:
      erase_shapes_by_tag (shape_type::path_ref_type::tag (), s, snext);
      break;
    case shape_type::PathPtrArrayMember:
      //  HINT: since we are in editing mode, this should never happen:
    case shape_type::PathPtrArray:
      //  HINT: since we are in editing mode, this should never happen:
      erase_shapes_by_tag (shape_type::path_ptr_array_type::tag (), s, snext);
      break;
    case shape_type::Box:
      erase_shapes_by_tag (shape_type::box_type::tag (), s, snext);
      break;
    case shape_type::BoxArrayMember:
      //  HINT: since we are in editing mode, this should never happen:
    case shape_type::BoxArray:
      //  HINT: since we are in editing mode, this should never happen:
      erase_shapes_by_tag (shape_type::box_array_type::tag (), s, snext);
      break;
    case shape_type::ShortBox:
      erase_shapes_by_tag (shape_type::short_box_type::tag (), s, snext);
      break;
    case shape_type::ShortBoxArrayMember:
      //  HINT: since we are in editing mode, this should never happen:
    case shape_type::ShortBoxArray:
      //  HINT: since we are in editing mode, this should never happen:
      erase_shapes_by_tag (shape_type::short_box_array_type::tag (), s, snext);
      break;
    case shape_type::Text:
      erase_shapes_by_tag (shape_type::text_type::tag (), s, snext);
      break;
    case shape_type::TextRef:
      erase_shapes_by_tag (shape_type::text_ref_type::tag (), s, snext);
      break;
    case shape_type::TextPtrArrayMember:
      //  HINT: since we are in editing mode, this should never happen:
    case shape_type::TextPtrArray:
      //  HINT: since we are in editing mode, this should never happen:
      erase_shapes_by_tag (shape_type::text_ptr_array_type::tag (), s, snext);
      break;
    case shape_type::UserObject:
      erase_shapes_by_tag (shape_type::user_object_type::tag (), s, snext);
      break;
    };

    s = snext;

  }
}

Shapes::shape_type 
Shapes::replace_prop_id (const Shapes::shape_type &ref, db::properties_id_type prop_id)
{
  tl_assert (! ref.is_array_member ());
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace_prop_id' is permitted only in editable mode")));
  }

  if (ref.with_props ()) {

    //  this assumes we can simply patch the properties ID ..
    switch (ref.m_type) {
    case shape_type::Null:
      break;
    case shape_type::Polygon:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::polygon_type>::tag ()), prop_id);
      break;
    case shape_type::PolygonRef:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::polygon_ref_type>::tag ()), prop_id);
      break;
    case shape_type::PolygonPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::polygon_ptr_array_type>::tag ()), prop_id);
      break;
    case shape_type::SimplePolygon:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::simple_polygon_type>::tag ()), prop_id);
      break;
    case shape_type::SimplePolygonRef:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::simple_polygon_ref_type>::tag ()), prop_id);
      break;
    case shape_type::SimplePolygonPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::simple_polygon_ptr_array_type>::tag ()), prop_id);
      break;
    case shape_type::Edge:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::edge_type>::tag ()), prop_id);
      break;
    case shape_type::Path:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::path_type>::tag ()), prop_id);
      break;
    case shape_type::PathRef:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::path_ref_type>::tag ()), prop_id);
      break;
    case shape_type::PathPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::path_ptr_array_type>::tag ()), prop_id);
      break;
    case shape_type::Box:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::box_type>::tag ()), prop_id);
      break;
    case shape_type::BoxArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::box_array_type>::tag ()), prop_id);
      break;
    case shape_type::ShortBox:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::short_box_type>::tag ()), prop_id);
      break;
    case shape_type::ShortBoxArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::short_box_array_type>::tag ()), prop_id);
      break;
    case shape_type::Text:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::text_type>::tag ()), prop_id);
      break;
    case shape_type::TextRef:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::text_ref_type>::tag ()), prop_id);
      break;
    case shape_type::TextPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::text_ptr_array_type>::tag ()), prop_id);
      break;
    case shape_type::UserObject:
      replace_prop_id (ref.basic_ptr (object_with_properties<shape_type::user_object_type>::tag ()), prop_id);
    default:
      break;
    };

    return ref;

  } else {

    switch (ref.m_type) {
    case shape_type::Null:
      return ref;
    case shape_type::Polygon:
      return replace_prop_id_iter (shape_type::polygon_type::tag (), ref.basic_iter (shape_type::polygon_type::tag ()), prop_id);
    case shape_type::PolygonRef:
      return replace_prop_id_iter (shape_type::polygon_ref_type::tag (), ref.basic_iter (shape_type::polygon_ref_type::tag ()), prop_id);
    case shape_type::PolygonPtrArray:
      return replace_prop_id_iter (shape_type::polygon_ptr_array_type::tag (), ref.basic_iter (shape_type::polygon_ptr_array_type::tag ()), prop_id);
    case shape_type::SimplePolygon:
      return replace_prop_id_iter (shape_type::simple_polygon_type::tag (), ref.basic_iter (shape_type::simple_polygon_type::tag ()), prop_id);
    case shape_type::SimplePolygonRef:
      return replace_prop_id_iter (shape_type::simple_polygon_ref_type::tag (), ref.basic_iter (shape_type::simple_polygon_ref_type::tag ()), prop_id);
    case shape_type::SimplePolygonPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      return replace_prop_id_iter (shape_type::simple_polygon_ptr_array_type::tag (), ref.basic_iter (shape_type::simple_polygon_ptr_array_type::tag ()), prop_id);
    case shape_type::Edge:
      return replace_prop_id_iter (shape_type::edge_type::tag (), ref.basic_iter (shape_type::edge_type::tag ()), prop_id);
    case shape_type::Path:
      return replace_prop_id_iter (shape_type::path_type::tag (), ref.basic_iter (shape_type::path_type::tag ()), prop_id);
    case shape_type::PathRef:
      return replace_prop_id_iter (shape_type::path_ref_type::tag (), ref.basic_iter (shape_type::path_ref_type::tag ()), prop_id);
    case shape_type::PathPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      return replace_prop_id_iter (shape_type::path_ptr_array_type::tag (), ref.basic_iter (shape_type::path_ptr_array_type::tag ()), prop_id);
    case shape_type::Box:
      return replace_prop_id_iter (shape_type::box_type::tag (), ref.basic_iter (shape_type::box_type::tag ()), prop_id);
    case shape_type::BoxArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      return replace_prop_id_iter (shape_type::box_array_type::tag (), ref.basic_iter (shape_type::box_array_type::tag ()), prop_id);
    case shape_type::ShortBox:
      return replace_prop_id_iter (shape_type::short_box_type::tag (), ref.basic_iter (shape_type::short_box_type::tag ()), prop_id);
    case shape_type::ShortBoxArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      return replace_prop_id_iter (shape_type::short_box_array_type::tag (), ref.basic_iter (shape_type::short_box_array_type::tag ()), prop_id);
    case shape_type::Text:
      return replace_prop_id_iter (shape_type::text_type::tag (), ref.basic_iter (shape_type::text_type::tag ()), prop_id);
    case shape_type::TextRef:
      return replace_prop_id_iter (shape_type::text_ref_type::tag (), ref.basic_iter (shape_type::text_ref_type::tag ()), prop_id);
    case shape_type::TextPtrArray:
      //  HINT: since we are in editing mode, this type should not appear ..
      return replace_prop_id_iter (shape_type::text_ptr_array_type::tag (), ref.basic_iter (shape_type::text_ptr_array_type::tag ()), prop_id);
    case shape_type::UserObject:
      return replace_prop_id_iter (shape_type::user_object_type::tag (), ref.basic_iter (shape_type::user_object_type::tag ()), prop_id);
    default:
      return ref;
    };

  }
}

template <class Trans>
Shapes::shape_type 
Shapes::transform (const Shapes::shape_type &ref, const Trans &t)
{
  tl_assert (! ref.is_array_member ());
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'transform' is permitted only in editable mode")));
  }

  switch (ref.m_type) {
  case shape_type::Null:
    return ref;
  case shape_type::Polygon:
    {
      shape_type::polygon_type p (ref.polygon ());
      p.transform (t);
      return replace_member_with_props (shape_type::polygon_type::tag (), ref, p);
    }
  case shape_type::PolygonRef:
    {
      shape_type::polygon_type p;
      ref.polygon (p);
      p.transform (t);
      return replace_member_with_props (shape_type::polygon_ref_type::tag (), ref, p);
    }
  case shape_type::SimplePolygon:
    {
      shape_type::simple_polygon_type p (ref.simple_polygon ());
      p.transform (t);
      return replace_member_with_props (shape_type::simple_polygon_type::tag (), ref, p);
    }
  case shape_type::SimplePolygonRef:
    {
      shape_type::simple_polygon_type p;
      ref.simple_polygon (p);
      p.transform (t);
      return replace_member_with_props (shape_type::simple_polygon_ref_type::tag (), ref, p);
    }
  case shape_type::Edge:
    {
      shape_type::edge_type p (ref.edge ());
      p.transform (t);
      return replace_member_with_props (shape_type::edge_type::tag (), ref, p);
    }
  case shape_type::Path:
    {
      shape_type::path_type p (ref.path ());
      p.transform (t);
      return replace_member_with_props (shape_type::path_type::tag (), ref, p);
    }
  case shape_type::PathRef:
    {
      shape_type::path_type p;
      ref.path (p);
      p.transform (t);
      return replace_member_with_props (shape_type::path_ref_type::tag (), ref, p);
    }
  case shape_type::Box:
  case shape_type::ShortBox:
    {
      if (t.is_ortho ()) {
        shape_type::box_type p (ref.box ());
        p.transform (t);
        return replace_member_with_props (shape_type::box_type::tag (), ref, p);
      } else {
        //  A box cannot stay a box in this case ...
        shape_type::simple_polygon_type p (ref.box ());
        p.transform (t);
        return replace_member_with_props (shape_type::box_type::tag (), ref, p);
      }
    }
  case shape_type::Text:
    {
      shape_type::text_type p (ref.text ());
      p.transform (t);
      return replace_member_with_props (shape_type::text_type::tag (), ref, p);
    }
  case shape_type::TextRef:
    {
      shape_type::text_type p;
      ref.text (p);
      p.transform (t);
      return replace_member_with_props (shape_type::text_ref_type::tag (), ref, p);
    }
  case shape_type::UserObject:
    {
      shape_type::user_object_type p (ref.user_object ());
      p.transform (t);
      return replace_member_with_props (shape_type::user_object_type::tag (), ref, p);
    }
  case shape_type::PolygonPtrArray:
  case shape_type::SimplePolygonPtrArray:
  case shape_type::PathPtrArray:
  case shape_type::BoxArray:
  case shape_type::ShortBoxArray:
  case shape_type::TextPtrArray:
    tl_assert (false); // @@@ not supported yet
  default:
    return ref;
  };
}

template <class Sh>
Shapes::shape_type 
Shapes::replace (const Shapes::shape_type &ref, const Sh &sh)
{
  tl_assert (! ref.is_array_member ());
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace' is permitted only in editable mode")));
  }

  switch (ref.m_type) {
  case shape_type::Null:
    return ref;
  case shape_type::Polygon:
    return replace_member_with_props (shape_type::polygon_type::tag (), ref, sh);
  case shape_type::PolygonRef:
    return replace_member_with_props (shape_type::polygon_ref_type::tag (), ref, sh);
  case shape_type::PolygonPtrArray:
    //  HINT: since we are in editing mode, this type should not appear ..
    return replace_member_with_props (shape_type::polygon_ptr_array_type::tag (), ref, sh);
  case shape_type::SimplePolygon:
    return replace_member_with_props (shape_type::simple_polygon_type::tag (), ref, sh);
  case shape_type::SimplePolygonRef:
    return replace_member_with_props (shape_type::simple_polygon_ref_type::tag (), ref, sh);
  case shape_type::SimplePolygonPtrArray:
    //  HINT: since we are in editing mode, this type should not appear ..
    return replace_member_with_props (shape_type::simple_polygon_ptr_array_type::tag (), ref, sh);
  case shape_type::Edge:
    return replace_member_with_props (shape_type::edge_type::tag (), ref, sh);
  case shape_type::Path:
    return replace_member_with_props (shape_type::path_type::tag (), ref, sh);
  case shape_type::PathRef:
    return replace_member_with_props (shape_type::path_ref_type::tag (), ref, sh);
  case shape_type::PathPtrArray:
    //  HINT: since we are in editing mode, this type should not appear ..
    return replace_member_with_props (shape_type::path_ptr_array_type::tag (), ref, sh);
  case shape_type::Box:
    return replace_member_with_props (shape_type::box_type::tag (), ref, sh);
  case shape_type::BoxArray:
    //  HINT: since we are in editing mode, this type should not appear ..
    return replace_member_with_props (shape_type::box_array_type::tag (), ref, sh);
  case shape_type::ShortBox:
    return replace_member_with_props (shape_type::short_box_type::tag (), ref, sh);
  case shape_type::ShortBoxArray:
    //  HINT: since we are in editing mode, this type should not appear ..
    return replace_member_with_props (shape_type::short_box_array_type::tag (), ref, sh);
  case shape_type::Text:
    return replace_member_with_props (shape_type::text_type::tag (), ref, sh);
  case shape_type::TextRef:
    return replace_member_with_props (shape_type::text_ref_type::tag (), ref, sh);
  case shape_type::TextPtrArray:
    //  HINT: since we are in editing mode, this type should not appear ..
    return replace_member_with_props (shape_type::text_ptr_array_type::tag (), ref, sh);
  case shape_type::UserObject:
    return replace_member_with_props (shape_type::user_object_type::tag (), ref, sh);
  default:
    return ref;
  };
}

void 
Shapes::clear ()
{
  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    (*l)->clear (this, manager ());
    delete *l;
  }
  invalidate_state ();  //  HINT: must come before the change is done!
  m_layers.clear ();
}

void Shapes::update_bbox ()
{
  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    (*l)->update_bbox ();
  }
  set_dirty (false);
}

void Shapes::update () 
{
  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    (*l)->sort ();
    (*l)->update_bbox ();
  }
  set_dirty (false);
}

bool Shapes::is_bbox_dirty () const
{
  if (is_dirty ()) {
    return true;
  }
  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    if ((*l)->is_bbox_dirty ()) {
      return true;
    }
  }
  return false;
}

Shapes::box_type Shapes::bbox () const
{
  box_type box;
  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    box += (*l)->bbox ();
  }
  return box;
}

void Shapes::sort () 
{
  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    (*l)->sort ();
  }
}

void 
Shapes::redo (db::Op *op)
{
  db::LayerOpBase *layop = dynamic_cast<db::LayerOpBase *> (op);
  if (layop) {
    layop->redo (this);
  } 
}

void 
Shapes::undo (db::Op *op)
{
  db::LayerOpBase *layop = dynamic_cast<db::LayerOpBase *> (op);
  if (layop) {
    layop->undo (this);
  } 
}

void
Shapes::collect_mem_stat (db::MemStatistics &m) const
{
  m.cell_info (m_layers);
  m.cell_info (mp_cell);

  for (tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    (*l)->collect_mem_stat (m);
  }
}

template <class Tag>
void 
Shapes::erase_shapes_by_tag (Tag tag, std::vector<Shapes::shape_type>::const_iterator s1, std::vector<Shapes::shape_type>::const_iterator s2)
{
  if (is_editable ()) {
    erase_shapes_by_tag_ws (tag, db::stable_layer_tag (), s1, s2);
  } else {
    erase_shapes_by_tag_ws (tag, db::unstable_layer_tag (), s1, s2);
  }
}

template <class Tag, class StableTag>
void 
Shapes::erase_shapes_by_tag_ws (Tag tag, StableTag stable_tag, std::vector<Shapes::shape_type>::const_iterator s1, std::vector<Shapes::shape_type>::const_iterator s2)
{
  if (! s1->has_prop_id ()) {

    std::vector<typename db::layer<typename Tag::object_type, StableTag>::iterator> iters;
    iters.reserve (std::distance (s1, s2));

    for (std::vector<shape_type>::const_iterator s = s1; s != s2; ++s) {
      typename db::layer<typename Tag::object_type, StableTag>::iterator i = iterator_from_shape (get_layer<typename Tag::object_type, StableTag> (), *s);
      //  in the "whole array" case it may happen that we delete one object multiple times ..
      if (iters.empty () || ! (iters.back () == i)) {
        iters.push_back (i);
      }
    }

    erase_positions (Tag (), StableTag (), iters.begin (), iters.end ());

  } else {

    typedef db::object_with_properties<typename Tag::object_type> swp_type;
    std::vector<typename db::layer<swp_type, StableTag>::iterator> iters;
    iters.reserve (std::distance (s1, s2));

    for (std::vector<shape_type>::const_iterator s = s1; s != s2; ++s) {
      typename db::layer<swp_type, StableTag>::iterator i = iterator_from_shape (get_layer<swp_type, StableTag> (), *s);
      //  in the "whole array" case it may happen that we delete one object multiple times ..
      if (iters.empty () || ! (iters.back () == i)) {
        iters.push_back (i);
      }
    }

    erase_positions (typename swp_type::tag (), StableTag (), iters.begin (), iters.end ());

  }
}

template <class Sh, class StableTag>
const db::layer<Sh, StableTag> &
Shapes::get_layer () const
{
  typedef layer_class<Sh, StableTag> lay_cls;
  
  for (typename tl::vector<LayerBase *>::const_iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    const lay_cls *lc = dynamic_cast <const lay_cls *> (*l);
    if (lc) {
      return lc->layer ();
    }
  }

  //  there seems to be a bug in gcc that disables
  //  the use of a simple static object here:
  static const db::layer<Sh, StableTag> *empty_layer = 0;
  if (! empty_layer) {
    empty_layer = new db::layer<Sh, StableTag> ();
  }
  return *empty_layer;
}

template <class Sh, class StableTag>
db::layer<Sh, StableTag> &
Shapes::get_layer () 
{
  typedef layer_class<Sh, StableTag> lay_cls;
  lay_cls *lc;
  
  for (typename tl::vector<LayerBase *>::iterator l = m_layers.begin (); l != m_layers.end (); ++l) {
    lc = dynamic_cast <lay_cls *> (*l);
    if (lc) {
      //  this is what optimizes access times for another access
      //  with this type 
      std::swap (m_layers.front (), *l);
      return lc->layer ();
    }
  }

  //  create 
  lc = new lay_cls ();
  m_layers.push_back (lc);
  std::swap (m_layers.front (), m_layers.back ());
  return lc->layer ();
}

template <class Tag, class PropIdMap>
Shapes::shape_type 
Shapes::insert_array_by_tag (Tag tag, const shape_type &shape, repository_type &rep, PropIdMap &pm)
{
  if (! shape.has_prop_id ()) {
    typename Tag::object_type n (*shape.basic_ptr (tag));
    n.object ().translate (rep);
    return insert (n);
  } else {
    db::object_with_properties<typename Tag::object_type> n (*shape.basic_ptr (tag), pm (shape.prop_id ()));
    n.object ().translate (rep);
    return insert (n);
  }
}

/** 
 *  @brief (Internal) Insert from a generic pointer
 */
template <class Tag, class PropIdMap>
Shapes::shape_type 
Shapes::insert_by_tag (Tag tag, const shape_type &shape, repository_type &rep, PropIdMap &pm)
{
  if (! shape.has_prop_id ()) {
    return insert (typename Tag::object_type (*shape.basic_ptr (tag), rep));
  } else {
    typedef db::object_with_properties<typename Tag::object_type> swp_type;
    return insert (swp_type (typename Tag::object_type (*shape.basic_ptr (tag), rep), pm (shape.prop_id ())));
  }
}

/** 
 *  @brief (Internal) Insert from a generic pointer
 */
template <class Tag, class PropIdMap>
Shapes::shape_type 
Shapes::insert_by_tag (Tag tag, const shape_type &shape, PropIdMap &pm)
{
  if (! shape.has_prop_id ()) {
    return insert (*shape.basic_ptr (tag));
  } else {
    typedef db::object_with_properties<typename Tag::object_type> swp_type;
    return insert (swp_type (*shape.basic_ptr (tag), pm (shape.prop_id ())));
  }
}

template <class Tag>
Shapes::shape_type  
Shapes::find_shape_by_tag (Tag tag, const shape_type &shape) const
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'find' is permitted only in editable mode")));
  }
  if (! shape.has_prop_id ()) {
    typename db::layer<typename Tag::object_type, db::stable_layer_tag>::iterator i = get_layer<typename Tag::object_type, db::stable_layer_tag> ().find (*shape.basic_ptr (tag));
    if (i == get_layer<typename Tag::object_type, db::stable_layer_tag> ().end ()) {
      return shape_type ();
    } else {
      return shape_type (this, i);
    }
  } else {
    typedef db::object_with_properties<typename Tag::object_type> swp_type;
    typename db::layer<swp_type, db::stable_layer_tag>::iterator i = get_layer<swp_type, db::stable_layer_tag> ().find (*shape.basic_ptr (typename swp_type::tag ()));
    if (i == get_layer<swp_type, db::stable_layer_tag> ().end ()) {
      return shape_type ();
    } else {
      return shape_type (this, i);
    }
  }
} 

template <class Tag>
bool  
Shapes::is_valid_shape_by_tag (Tag tag, const shape_type &shape) const
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'is_valid' is permitted only in editable mode")));
  }
  if (! shape.has_prop_id ()) {
    return iterator_from_shape (get_layer<typename Tag::object_type, db::stable_layer_tag> (), shape).is_valid ();
  } else {
    typedef db::object_with_properties<typename Tag::object_type> swp_type;
    return iterator_from_shape (get_layer<swp_type, db::stable_layer_tag> (), shape).is_valid ();
  }
} 

template <class Tag>
void 
Shapes::erase_shape_by_tag (Tag tag, const shape_type &shape)
{
  if (is_editable ()) {
    erase_shape_by_tag_ws (tag, db::stable_layer_tag (), shape);
  } else {
    erase_shape_by_tag_ws (tag, db::unstable_layer_tag (), shape);
  }
} 

template <class Tag, class StableTag>
void 
Shapes::erase_shape_by_tag_ws (Tag tag, StableTag stable_tag, const shape_type &shape)
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'erase' is permitted only in editable mode")));
  }
  if (! shape.has_prop_id ()) {

    db::layer<typename Tag::object_type, StableTag> &l = get_layer<typename Tag::object_type, StableTag> ();
    typename db::layer<typename Tag::object_type, StableTag>::iterator i = iterator_from_shape (l, shape);
    if (manager () && manager ()->transacting ()) {
      db::layer_op<typename Tag::object_type, StableTag>::queue_or_append (manager (), this, false /*not insert*/, *i);
    }
    invalidate_state ();  //  HINT: must come before the change is done!
    l.erase (i);

  } else {

    typedef db::object_with_properties<typename Tag::object_type> swp_type;

    db::layer<swp_type, StableTag> &l = get_layer<swp_type, StableTag> ();
    typename db::layer<swp_type, StableTag>::iterator i = iterator_from_shape (l, shape);
    if (manager () && manager ()->transacting ()) {
      db::layer_op<swp_type, StableTag>::queue_or_append (manager (), this, false /*not insert*/, *i);
    }
    invalidate_state ();  //  HINT: must come before the change is done!
    l.erase (i);

  }
}

template <class Sh>
void
Shapes::replace_prop_id (const Sh *pos, db::properties_id_type prop_id)
{
  if (pos->properties_id () != prop_id) {
    if (! is_editable ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace' is permitted only in editable mode")));
    }
    if (manager () && manager ()->transacting ()) {
      db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, false /*not insert*/, *pos);
    }
    invalidate_state ();  //  HINT: must come before the change is done!
    ((Sh *) pos)->properties_id (prop_id);
    if (manager () && manager ()->transacting ()) {
      db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, true /*insert*/, *pos);
    }
  }
}

template <class Sh, class Iter>
Shapes::shape_type
Shapes::replace_prop_id_iter (typename db::object_tag<Sh>, const Iter &iter, db::properties_id_type prop_id)
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace' is permitted only in editable mode")));
  }

  if (manager () && manager ()->transacting ()) {
    db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, false /*not insert*/, *iter);
  }
  db::object_with_properties <Sh> wp (*iter, prop_id);
  invalidate_state ();  //  HINT: must come before the change is done!
  get_layer<Sh, db::stable_layer_tag> ().erase (iter); 
  if (manager () && manager ()->transacting ()) {
    db::layer_op<db::object_with_properties <Sh>, db::stable_layer_tag>::queue_or_append (manager (), this, true /*insert*/, wp);
  }
  return shape_type (this, get_layer <db::object_with_properties <Sh>, db::stable_layer_tag> ().insert (wp)); 
}

template <class Sh1, class Sh2>
Shapes::shape_type 
Shapes::reinsert_member_with_props (typename db::object_tag<Sh1>, const shape_type &ref, const Sh2 &sh)
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace' is permitted only in editable mode")));
  }

  //  the shape types are not equal - resolve into erase and insert (of new)
  if (! ref.with_props ()) {
    erase_shape (ref);
    return insert (sh);
  } else {
    db::properties_id_type pid = ref.prop_id ();
    erase_shape (ref);
    return insert (db::object_with_properties<Sh2> (sh, pid));
  }
}

template <class Sh1, class Sh2>
Shapes::shape_type 
Shapes::replace_member_with_props (typename db::object_tag<Sh1>, const shape_type &ref, const Sh2 &sh)
{
  if (! is_editable ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace' is permitted only in editable mode")));
  }

  //  the shape types are not equal - resolve into erase and insert (of new)
  if (! ref.with_props ()) {
    erase_shape (ref);
    return insert (sh);
  } else {
    db::properties_id_type pid = ref.prop_id ();
    erase_shape (ref);
    return insert (db::object_with_properties<Sh2> (sh, pid));
  }
}

template <class Sh>
Shapes::shape_type 
Shapes::replace_member_with_props (typename db::object_tag<Sh> tag, const shape_type &ref, const Sh &sh)
{
  //  avoid creating a undo entry if the shape is equal to the current one
  if (*ref.basic_ptr (tag) == sh) {
    return ref;
  }
  
  if (! layout ()) {

    if (needs_translate (tag)) {
      return reinsert_member_with_props (tag, ref, sh);
    } else {

      //  simple replace case

      if (manager () && manager ()->transacting ()) {
        db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, false /*not insert*/, *ref.basic_ptr (tag));
      }

      invalidate_state ();  //  HINT: must come before the change is done!

      get_layer<Sh, db::stable_layer_tag> ().replace (ref.basic_iter (tag), sh);

      if (manager () && manager ()->transacting ()) {
        db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, true /*insert*/, sh);
      }

      return ref;

    }

  } else {

    if (! is_editable ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Function 'replace' is permitted only in editable mode")));
    }

    if (! ref.with_props ()) {

      if (manager () && manager ()->transacting ()) {
        db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, false /*not insert*/, *ref.basic_ptr (tag));
      }

      invalidate_state ();  //  HINT: must come before the change is done!

      if (needs_translate (tag)) {

        Sh sh_trans;
        sh_trans.translate (sh, shape_repository (), array_repository ());
        get_layer<Sh, db::stable_layer_tag> ().replace (ref.basic_iter (tag), sh_trans);

        if (manager () && manager ()->transacting ()) {
          db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, true /*insert*/, sh_trans);
        }

      } else {

        get_layer<Sh, db::stable_layer_tag> ().replace (ref.basic_iter (tag), sh);

        if (manager () && manager ()->transacting ()) {
          db::layer_op<Sh, db::stable_layer_tag>::queue_or_append (manager (), this, true /*insert*/, sh);
        }

      }

    } else {

      if (manager () && manager ()->transacting ()) {
        db::layer_op<db::object_with_properties<Sh>, db::stable_layer_tag>::queue_or_append (manager (), this, false /*not insert*/, *ref.basic_ptr (typename db::object_with_properties<Sh>::tag ()));
      }

      invalidate_state ();  //  HINT: must come before the change is done!

      db::object_with_properties<Sh> swp;
      swp.translate (db::object_with_properties<Sh> (sh, ref.prop_id ()), shape_repository (), array_repository ());
      get_layer<db::object_with_properties<Sh>, db::stable_layer_tag> ().replace (ref.basic_iter (typename db::object_with_properties<Sh>::tag ()), swp);

      if (manager () && manager ()->transacting ()) {
        db::layer_op<db::object_with_properties<Sh>, db::stable_layer_tag>::queue_or_append (manager (), this, true /*insert*/, swp);
      }

    }

    return ref;

  }
}

//  explicit instantiations

template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const Box &);
template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const ShortBox &);
template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const Path &);
template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const Polygon &);
template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const SimplePolygon &);
template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const Text &);
template KLAYOUT_DLL Shape Shapes::replace<>(const Shape &, const Edge &);

template KLAYOUT_DLL Shape Shapes::transform<> (const Shape &, const ICplxTrans &);
template KLAYOUT_DLL Shape Shapes::transform<> (const Shape &, const Trans &);

template KLAYOUT_DLL Shape Shapes::do_insert<> (const Shape &, const ICplxTrans &, tl::func_delegate_base <db::properties_id_type> &);
template KLAYOUT_DLL Shape Shapes::do_insert<> (const Shape &, const Trans &, tl::func_delegate_base <db::properties_id_type> &);

template KLAYOUT_DLL layer<db::Shape::polygon_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::polygon_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::polygon_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::simple_polygon_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::polygon_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ref_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::simple_polygon_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ref_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::polygon_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ptr_array_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::simple_polygon_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ptr_array_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::path_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::path_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::path_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::path_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::path_ref_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::path_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ref_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::path_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::path_ptr_array_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::edge_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::edge_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::edge_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::edge_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::text_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::text_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::text_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::text_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::text_ref_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::text_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ref_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::text_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::text_ptr_array_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::box_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::box_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::box_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::box_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::box_array_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::box_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_array_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::short_box_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::short_box_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::short_box_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::short_box_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::short_box_array_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::short_box_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_array_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::user_object_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::user_object_type, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::user_object_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::user_object_type>, db::stable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::polygon_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::polygon_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::polygon_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::simple_polygon_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::polygon_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ref_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::simple_polygon_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ref_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::polygon_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ptr_array_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::simple_polygon_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ptr_array_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::path_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::path_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::path_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::path_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::path_ref_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::path_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ref_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::path_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::path_ptr_array_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::edge_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::edge_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::edge_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::edge_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::text_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::text_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::text_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::text_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::text_ref_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::text_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ref_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::text_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::text_ptr_array_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::box_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::box_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::box_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::box_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::box_array_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::box_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_array_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::short_box_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::short_box_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::short_box_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::short_box_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::short_box_array_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::short_box_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_array_type>, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::Shape::user_object_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::user_object_type, db::unstable_layer_tag> ();
template KLAYOUT_DLL layer<db::object_with_properties<db::Shape::user_object_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::user_object_type>, db::unstable_layer_tag> ();

template KLAYOUT_DLL const layer<db::Shape::polygon_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::polygon_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::polygon_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::simple_polygon_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::polygon_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ref_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::simple_polygon_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ref_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::polygon_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ptr_array_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::simple_polygon_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ptr_array_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::path_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::path_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::path_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::path_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::path_ref_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::path_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ref_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::path_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::path_ptr_array_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::edge_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::edge_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::edge_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::edge_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::text_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::text_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::text_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::text_ref_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::text_ref_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::text_ref_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ref_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::text_ptr_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::text_ptr_array_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::box_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::box_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::box_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::box_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::box_array_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::box_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_array_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::short_box_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::short_box_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::short_box_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::short_box_array_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::short_box_array_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::short_box_array_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_array_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::user_object_type, db::stable_layer_tag> &Shapes::get_layer<db::Shape::user_object_type, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::user_object_type>, db::stable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::user_object_type>, db::stable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::polygon_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::polygon_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::polygon_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::simple_polygon_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::polygon_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ref_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ref_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::simple_polygon_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ref_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::polygon_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::polygon_ptr_array_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::simple_polygon_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::simple_polygon_ptr_array_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::path_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::path_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::path_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::path_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::path_ref_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::path_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ref_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::path_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::path_ptr_array_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::path_ptr_array_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::edge_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::edge_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::edge_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::edge_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::text_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::text_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::text_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::text_ref_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::text_ref_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::text_ref_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ref_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::text_ptr_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::text_ptr_array_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::text_ptr_array_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::box_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::box_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::box_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::box_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::box_array_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::box_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::box_array_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::short_box_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::short_box_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::short_box_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::short_box_array_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::short_box_array_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::short_box_array_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::short_box_array_type>, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::Shape::user_object_type, db::unstable_layer_tag> &Shapes::get_layer<db::Shape::user_object_type, db::unstable_layer_tag> () const;
template KLAYOUT_DLL const layer<db::object_with_properties<db::Shape::user_object_type>, db::unstable_layer_tag> &Shapes::get_layer<db::object_with_properties<db::Shape::user_object_type>, db::unstable_layer_tag> () const;

template class layer_op<db::Shape::polygon_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::polygon_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::simple_polygon_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::simple_polygon_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::polygon_ref_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::polygon_ref_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::simple_polygon_ref_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::polygon_ptr_array_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::simple_polygon_ptr_array_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::path_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::path_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::path_ref_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::path_ref_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::path_ptr_array_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::path_ptr_array_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::edge_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::edge_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::text_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::text_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::text_ref_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::text_ref_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::text_ptr_array_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::text_ptr_array_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::box_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::box_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::box_array_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::box_array_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::short_box_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::short_box_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::short_box_array_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::short_box_array_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::user_object_type, db::stable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::user_object_type>, db::stable_layer_tag>;
template class layer_op<db::Shape::polygon_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::polygon_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::simple_polygon_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::simple_polygon_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::polygon_ref_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::polygon_ref_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::simple_polygon_ref_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::simple_polygon_ref_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::polygon_ptr_array_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::polygon_ptr_array_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::simple_polygon_ptr_array_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::simple_polygon_ptr_array_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::path_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::path_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::path_ref_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::path_ref_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::path_ptr_array_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::path_ptr_array_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::edge_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::edge_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::text_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::text_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::text_ref_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::text_ref_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::text_ptr_array_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::text_ptr_array_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::box_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::box_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::box_array_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::box_array_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::short_box_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::short_box_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::short_box_array_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::short_box_array_type>, db::unstable_layer_tag>;
template class layer_op<db::Shape::user_object_type, db::unstable_layer_tag>;
template class layer_op<db::object_with_properties<db::Shape::user_object_type>, db::unstable_layer_tag>;

}

