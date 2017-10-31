
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


#if defined(HAVE_PYTHON)

#include "pyaMarshal.h"
#include "pyaObject.h"
#include "pyaConvert.h"
#include "pya.h"

#include "gsiTypes.h"

namespace pya
{

// -------------------------------------------------------------------
//  Cast operator helper

/**
 *  @brief Cast a PyObject * ot a PYAObjectBase of the given type
 *
 *  This function will throw an exception if the object is not of a class compatible with the
 *  expected class.
 *
 *  @param inside_array If true, a different error message is given saying the object is inside an array.
 */
static PYAObjectBase *
cast_to (PyObject *arg, const gsi::ClassBase *cls, bool inside_array = false)
{
  const gsi::ClassBase *cls_decl = PythonInterpreter::instance ()->cls_for_type (Py_TYPE (arg));
  if (cls_decl && ! cls_decl->is_derived_from (cls)) {
    cls_decl = 0;
  }
  if (! cls_decl) {
    if (inside_array) {
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type inside tuple (expected argument of class %s, got %s)")), cls->name (), Py_TYPE (arg)->tp_name));
    } else {
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s, got %s)")), cls->name (), Py_TYPE (arg)->tp_name));
    }
  }

  return (PYAObjectBase *) (arg); 
}

/**
 *  @brief Correct constness if a reference is const and a non-const reference is required
 *  HINT: this is a workaround for the fact that unlike C++, Python does not have const or non-const
 *  references. Since a reference is identical with the object it points to, there are only const or non-const
 *  objects. We deliver const objects first, but if a non-const version is requestes, the
 *  object turns into a non-const one. This may be confusing but provides a certain level
 *  of "constness", at least until there is another non-const reference for that object.
 */
void correct_constness (PyObject *obj, bool const_required)
{
  PYAObjectBase *p = (PYAObjectBase *) obj;
  if (p->const_ref () && ! const_required) {
    //  promote to non-const object
    p->set_const_ref (false);
  }
}

// -------------------------------------------------------------------
//  Return the boxed value pointer for a given basic type from the reference

template <class R>
struct get_boxed_value_func
{
  void operator() (void **ret, PyObject *arg, tl::Heap *heap)
  { 
    const gsi::ClassBase *cls_decl = PythonInterpreter::instance ()->cls_for_type (Py_TYPE (arg));
    if (! cls_decl) {

      R *v = new R (python2c<R> (arg, heap));
      heap->push (v);
      *ret = v;

    } else {

      const gsi::ClassBase *bt = gsi::cls_decl <gsi::Value> ();

      if (!cls_decl->is_derived_from (bt)) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Passing an object to pointer or reference requires a boxed type (pya.%s)")), bt->name ()));
      }

      PYAObjectBase *p = (PYAObjectBase *) arg; 
      gsi::Value *bo = reinterpret_cast<gsi::Value *> (p->obj ());
      if (bo) {
        *ret = bo->value ().template morph<R> ().native_ptr ();
      }

    }
  }
};

void *boxed_value_ptr (gsi::BasicType type, PyObject *arg, tl::Heap &heap)
{
  void *value = 0;
  gsi::do_on_type<get_boxed_value_func> () (type, &value, arg, &heap);
  return value;
}

// -------------------------------------------------------------------

/**
 *  @brief A serialization wrapper (write mode)
 *
 *  The generic class is for POD objects.
 */
template <class R, class T>
struct writer_impl;

template <class R>
struct writer_impl <R, tl::false_tag>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Py_None || arg == NULL) {

      if (atype.is_ref () || atype.is_cref ()) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference type cannot be passed None")));
      } else if (atype.is_ptr ()) {
        aa->write<R *> ((R *)0);
      } else if (atype.is_cptr ()) {
        aa->write<const R *> ((const R *)0);
      } else {
        aa->write<R> ((R)0);
      }

    } else {

      if (atype.is_ref () || atype.is_ptr ()) {
        // references or pointers require a boxed object. Pointers also allow None.
        void *vc = boxed_value_ptr (atype.type (), arg, *heap);
        if (! vc && atype.is_ref ()) {
          throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference or direct type cannot be passed None or an empty boxed value object")));
        }
        aa->write<void *> (vc);
      } else if (atype.is_cref ()) {
        //  Note: POD's are written as copies for const refs, so we can pass a temporary here:
        //  (avoids having to create a temp object)
        aa->write<const R &> (python2c<R> (arg, heap));
      } else if (atype.is_cptr ()) {
        //  Note: POD's are written as copies for const ptrs, so we can pass a temporary here:
        //  (avoids having to create a temp object)
        R r = python2c<R> (arg, heap);
        aa->write<const R *> (&r);
      } else {
        aa->write<R> (python2c<R> (arg, heap));
      }

    }
  }
};

/**
 *  @brief A serialization wrapper (write mode)
 *
 *  Specialisation for non-POD objects. Those are not passed by value for const ref's and
 *  const pointers, hence we have to create temp objects in that case.
 */
template <class R>
struct writer_impl <R, tl::true_tag>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Py_None || arg == NULL) {

      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference or direct type cannot be passed null")));
      } else if (atype.is_ptr ()) {
        aa->write<R *> ((R *)0);
      } else {
        aa->write<const R *> ((const R *)0);
      }

    } else {

      if (atype.is_ref () || atype.is_ptr ()) {

        // references or pointers require a boxed object. Pointers also allow null.
        void *vc = boxed_value_ptr (atype.type (), arg, *heap);
        if (! vc && atype.is_ref ()) {
          throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference or direct type cannot be passed null or an empty boxed value object")));
        }
        aa->write<void *> (vc);

      } else if (atype.is_cref () || atype.is_cptr ()) {

        R *v = new R (python2c<R> (arg, heap));
        heap->push (v);

        if (atype.is_cref ()) {
          aa->write<const R &> (*v);
        } else if (atype.is_cptr ()) {
          aa->write<const R *> (v);
        }

      } else {
        aa->write<R> (python2c<R> (arg, heap));
      }

    }
  }
};

/**
 *  @brief The generalized writer function
 */
template <class R>
struct writer
  : public writer_impl<R, typename gsi::type_traits<R>::tag::is_npod>
{
};

/**
 *  @brief A serialization wrapper (write mode)
 *  Specialisation for variants 
 */
template <>
struct writer<tl::Variant>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if ((arg == Py_None || arg == NULL) && (atype.is_ptr () || atype.is_cptr ())) {

      if (atype.is_ptr ()) {
        aa->write<tl::Variant *> ((tl::Variant *)0);
      } else {
        aa->write<const tl::Variant *> ((const tl::Variant *)0);
      }

    } else {

      if (atype.is_cref () || atype.is_ref () || atype.is_cptr () || atype.is_ptr ()) {

        tl::Variant *v = new tl::Variant (python2c<tl::Variant> (arg, heap));
        heap->push (v);

        if (atype.is_ref ()) {
          aa->write<tl::Variant &> (*v);
        } else if (atype.is_cref ()) {
          aa->write<const tl::Variant &> (*v);
        } else if (atype.is_ptr ()) {
          aa->write<tl::Variant *> (v);
        } else if (atype.is_cptr ()) {
          aa->write<const tl::Variant *> (v);
        }

      } else {
        aa->write<tl::Variant> (python2c<tl::Variant> (arg, heap));
      }

    }
  }
};

/**
 *  @brief A serialization wrapper (write mode)
 *  Specialisation for void 
 */
template <>
struct writer<gsi::VoidType>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  ignore void
  }
};

template <class R>
struct vector_writer
{
  void operator() (gsi::SerialArgs *aa, PyObject *arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    bool is_tuple = PyTuple_Check (arr);
    size_t len = is_tuple ? PyTuple_Size (arr) : PyList_Size (arr);

    if (atype.is_cref () || atype.is_ref () || atype.is_cptr () || atype.is_ptr ()) {

      std::vector<R> *v = new std::vector<R> ();
      v->reserve (len);
      for (size_t i = 0; i < len; ++i) {
        v->push_back (python2c<R> (is_tuple ? PyTuple_GetItem (arr, i) : PyList_GetItem (arr, i), heap));
      }
      heap->push (v);

      if (atype.is_ref ()) {
        aa->write<std::vector<R> &> (*v);
      } else if (atype.is_cref ()) {
        aa->write<const std::vector<R> &> (*v);
      } else if (atype.is_ptr ()) {
        aa->write<std::vector<R> *> (v);
      } else if (atype.is_cptr ()) {
        aa->write<const std::vector<R> *> (v);
      }

    } else {

      std::vector<R> *v = reinterpret_cast<std::vector<R> *> (aa->wptr ());
      aa->write< std::vector<R> > (std::vector<R> ());
      v->reserve (len);
      for (size_t i = 0; i < len; ++i) {
        v->push_back (python2c<R> (is_tuple ? PyTuple_GetItem (arr, i) : PyList_GetItem (arr, i), heap));
      }

    }

  }
};

template <>
struct vector_writer<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    const gsi::ArgType &ainner = *atype.inner ();

    bool is_tuple = PyTuple_Check (arr);
    size_t len = (is_tuple ? PyTuple_Size (arr) : PyList_Size (arr));

    std::vector<void *> vobj;
    vobj.reserve (len);

    for (size_t i = 0; i < len; ++i) {
      PYAObjectBase *p = cast_to (is_tuple ? PyTuple_GetItem (arr, i) : PyList_GetItem (arr, i), ainner.cls (), true /*inside array*/);
      //  TODO: const check?
      vobj.push_back (p->obj ());
    }

    ainner.cls ()->push_vector_of (*aa, atype, *heap, vobj);
  }
};

/**
 *  @brief A serialization wrapper (write mode)
 *  Specialisation for vectors 
 */
template <>
struct writer<gsi::VectorType>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Py_None || arg == NULL) {
      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments of reference or direct type cannot be passed null")));
      } else {
        aa->write<void *> ((void *) 0);
        return;
      }
    }

    if (! PyTuple_Check (arg) && ! PyList_Check (arg)) {
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Expected a list or tuple for argument or return type"))));
    }

    tl_assert (atype.inner () != 0);
    const gsi::ArgType &ainner = *atype.inner ();

    gsi::do_on_type<vector_writer> () (ainner.type (), aa, arg, atype, heap);
  }
};

template <class X, class Y>
struct map_writer
{
  void operator() (gsi::SerialArgs *aa, PyObject *dict, const gsi::ArgType &atype, tl::Heap *heap)
  {
    typedef std::map<X, Y> map_repr_type;

    map_repr_type *map;
    if (atype.is_cref () || atype.is_ref () || atype.is_cptr () || atype.is_ptr ()) {
      map = new map_repr_type ();
      heap->push (map);
    } else {
      map = reinterpret_cast<map_repr_type *> (aa->wptr ());
      aa->write<map_repr_type> (map_repr_type ());
    }

    Py_ssize_t pos = 0;
    PyObject *key, *value;
    while (PyDict_Next (dict, &pos, &key, &value)) {
      map->insert (std::make_pair (python2c<X> (key), python2c<Y> (value)));
    }

    if (atype.is_ref ()) {
      aa->write<map_repr_type &> (*map);
    } else if (atype.is_cref ()) {
      aa->write<const map_repr_type &> (*map);
    } else if (atype.is_ptr ()) {
      aa->write<map_repr_type *> (map);
    } else if (atype.is_cptr ()) {
      aa->write<const map_repr_type *> (map);
    }

  }
};

struct map_writer_error
{
  void operator() (gsi::SerialArgs *aa, PyObject *dict, const gsi::ArgType &atype, tl::Heap *heap)
  {
    tl_assert (false);
  }
};

template <class Y> struct map_writer<gsi::ObjectType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::VectorType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::MapType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::VoidType, Y> : map_writer_error { };

/**
 *  @brief A serialization wrapper (write mode)
 *  Specialisation for map 
 */
template <>
struct writer<gsi::MapType>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Py_None || arg == NULL) {
      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments of reference or direct type cannot be passed null")));
      } else {
        aa->write<void *> ((void *) 0);
        return;
      }
    }

    if (! PyDict_Check (arg)) {
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Expected a dict for argument or return type"))));
    }

    tl_assert (atype.inner () != 0);
    tl_assert (atype.inner_k () != 0);
    const gsi::ArgType &ainner = *atype.inner ();
    const gsi::ArgType &ainner_k = *atype.inner_k ();

    gsi::do_on_type2<map_writer> () (ainner_k.type (), ainner.type (), aa, arg, atype, heap);
  }
};

/**
 *  @brief A serialization wrapper (write mode)
 *  Specialisation for objects 
 */
template <>
struct writer<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *aa, PyObject *arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Py_None || arg == NULL) {
      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments of reference or direct type cannot be passed null")));
      } else {
        aa->write<void *> ((void *) 0);
        return;
      }
    }

    if (atype.is_ptr () || atype.is_cptr () || atype.is_ref () || atype.is_cref ()) {

      PYAObjectBase *p = cast_to (arg, atype.cls ());
      if (p->const_ref () && (atype.is_ptr () || atype.is_ref ())) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Cannot pass a const reference of class %s to a non-const reference parameter")), atype.cls ()->name ()));
      }
      aa->write<void *> (p->obj ());

    } else {

      atype.cls ()->copy_to (*aa, cast_to (arg, atype.cls ())->obj ());

    }

  }
};

void
push_arg (const gsi::ArgType &atype, gsi::SerialArgs &aserial, PyObject *arg, tl::Heap &heap)
{
  gsi::do_on_type<writer> () (atype.type (), &aserial, arg, atype, &heap);
}

/**
 *  @brief Deseralisation wrapper 
 *
 *  The default implementation is for POD types, strings and variants
 */
template <class R> 
struct reader
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &arg)
  {
    if (arg.is_ref ()) {
      *ret = c2python<R> (rr->template read<R &> ());
    } else if (arg.is_cref ()) {
      *ret = c2python<R> (rr->template read<const R &> ());
    } else if (arg.is_ptr ()) {
      R *p = rr->template read<R *> ();
      if (p) {
        *ret = c2python<R> (*p);
      } else {
        *ret = PythonRef (Py_None, false /*borrowed*/);
      }
    } else if (arg.is_cptr ()) {
      const R *p = rr->template read<const R *> ();
      if (p) {
        *ret = c2python<R> (*p);
      } else {
        *ret = PythonRef (Py_None, false /*borrowed*/);
      }
    } else {
      *ret = c2python<R> (rr->template read<R> ());
    }
  }
};

/**
 *  @brief Deseralisation wrapper: specialization for const char *
 *
 *  Without that would would have to handle void *&, void * const &, ...
 *  TODO: right now these types are not supported.
 */
template <> 
struct reader<void *>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &arg)
  {
    tl_assert (! arg.is_cref ());
    tl_assert (! arg.is_ref ());
    tl_assert (! arg.is_cptr ());
    tl_assert (! arg.is_ptr ());
    *ret = c2python<void *> (rr->read<void *> ());
  }
};

/**
 *  @brief Deseralisation wrapper: specialization for const char *
 *
 *  Without that would would have to handle const char *&, const char * const &, ...
 *  TODO: right now these types are not supported.
 */
template <> 
struct reader<const char *>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &arg)
  {
    tl_assert (! arg.is_cref ());
    tl_assert (! arg.is_ref ());
    tl_assert (! arg.is_cptr ());
    tl_assert (! arg.is_ptr ());
    *ret = c2python<const char *> (rr->read<const char *> ());
  }
};

template <class R> 
struct vector_reader
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &arg)
  {
    if (arg.is_ref ()) {

      std::vector<R> &v = rr->template read<std::vector<R> &> ();

      *ret = PyList_New (v.size ());
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        PyList_SetItem (ret->get (), i - v.begin (), c2python<R> (*i));
      }

    } else if (arg.is_cref ()) {

      const std::vector<R> &v = rr->template read<const std::vector<R> &> ();

      *ret = PyList_New (v.size ());
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        PyList_SetItem (ret->get (), i - v.begin (), c2python<R> (*i));
      }

    } else if (arg.is_ptr ()) {

      std::vector<R> *v = rr->template read<std::vector<R> *> ();
      if (! v) {
        *ret = PythonRef (Py_None, false /*borrowed*/);
      } else {

        *ret = PyList_New (v->size ());
        for (typename std::vector<R>::const_iterator i = v->begin (); i != v->end (); ++i) {
          PyList_SetItem (ret->get (), i - v->begin (), c2python<R> (*i));
        }

      }

    } else if (arg.is_cptr ()) {

      const std::vector<R> *v = rr->template read<const std::vector<R> *> ();
      if (! v) {
        *ret = PythonRef (Py_None, false /*borrowed*/);
      } else {

        *ret = PyList_New (v->size ());
        for (typename std::vector<R>::const_iterator i = v->begin (); i != v->end (); ++i) {
          PyList_SetItem (ret->get (), i - v->begin (), c2python<R> (*i));
        }

      }

    } else {

      std::vector<R> v = rr->template read< std::vector<R> > ();

      *ret = PyList_New (v.size ());
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        PyList_SetItem (ret->get (), i - v.begin (), c2python<R> (*i));
      }

    }
  }
};

struct ArrayData
{
  ArrayData (const gsi::ArgType &_ainner)
    : ainner (_ainner)
  { }

  PyObject *create_list () 
  {
    PyObject *list = PyList_New (objects.size ());
    for (std::vector<PythonRef>::iterator i = objects.begin (); i != objects.end (); ++i) {
      PyList_SetItem (list, i - objects.begin (), i->release ());
    }
    return list;
  }

  std::vector<PythonRef> objects;
  const gsi::ArgType &ainner;
};

static void
make_array_f (void *data, void *cobj)
{
  ArrayData *ad = (ArrayData *) data;
  
  const gsi::ClassBase *clsact = ad->ainner.cls ()->subclass_decl (cobj);
  PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (clsact);
  tl_assert (type != NULL);

  ad->objects.push_back (PythonRef (Py_None, false /*borrowed*/));

  if (ad->ainner.is_ptr () || ad->ainner.is_cptr ()) {

    if (cobj) {

      if (clsact->client_data (cobj, client_index) != 0 && 
          ((ClientData *) clsact->client_data (cobj, client_index))->get_ref () != NULL) {

        //  we have a that is located in C++ space but is supposed to get attached
        //  a Python object. If it already has, we simply return a reference to this
        PyObject *obj = ((ClientData *) clsact->client_data (cobj, client_index))->get_ref ();
        ad->objects.back () = PythonRef (obj, false /*borrowed*/);

        //  correct constness if the object is not supposed to be const
        correct_constness (obj, ad->ainner.is_cptr () /*const obj*/);

      } else {

        //  initialize this instance
        PYAObjectBase *obj = (PYAObjectBase *) type->tp_alloc (type, 0);
        new (obj) PYAObjectBase (clsact);
        ad->objects.back () = PythonRef (obj);

        obj->set (cobj, false /*owned*/, ad->ainner.is_cptr () /*const obj*/, ad->ainner.is_ptr () /*can destroy*/);

      }

    }

  } else {

    //  create a new managed instance 
    PYAObjectBase *obj = (PYAObjectBase *) type->tp_alloc (type, 0);
    new (obj) PYAObjectBase (clsact);
    ad->objects.back () = PythonRef (obj);

    obj->set (cobj, true /*owned*/, false /*const obj*/, false /*can destroy*/);

  }
}

template <> 
struct vector_reader<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &atype)
  {
    const gsi::ArgType &ainner = *atype.inner ();

    ArrayData ad (ainner);
    ainner.cls ()->get_vector_of (*rr, atype, (void *) &ad, &make_array_f);
    *ret = ad.create_list ();
  }
};

/**
 *  @brief Deseralisation wrapper: specialization for vector 
 */
template <> 
struct reader<gsi::VectorType>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &atype)
  {
    tl_assert (atype.inner () != 0);
    gsi::do_on_type<vector_reader> () (atype.inner ()->type (), rr, ret, atype);
  }
};

template <class X, class Y> 
struct map_reader
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &arg)
  {
    typedef std::map<X, Y> map_repr_type;

    if (arg.is_ref ()) {

      map_repr_type &v = rr->template read<map_repr_type &> ();

      *ret = PyDict_New ();
      for (typename map_repr_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        PyDict_SetItem (ret->get (), c2python<X> (i->first), c2python<Y> (i->second));
      }

    } else if (arg.is_cref ()) {

      const map_repr_type &v = rr->template read<const map_repr_type &> ();

      *ret = PyDict_New ();
      for (typename map_repr_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        PyDict_SetItem (ret->get (), c2python<X> (i->first), c2python<Y> (i->second));
      }

    } else if (arg.is_ptr ()) {

      map_repr_type *v = rr->template read<map_repr_type *> ();
      if (! v) {
        *ret = PythonRef (Py_None, false /*borrowed*/);
      } else {

        *ret = PyDict_New ();
        for (typename map_repr_type::const_iterator i = v->begin (); i != v->end (); ++i) {
          PyDict_SetItem (ret->get (), c2python<X> (i->first), c2python<Y> (i->second));
        }

      }

    } else if (arg.is_cptr ()) {

      const map_repr_type *v = rr->template read<const map_repr_type *> ();
      if (! v) {
        *ret = PythonRef (Py_None, false /*borrowed*/);
      } else {

        *ret = PyDict_New ();
        for (typename map_repr_type::const_iterator i = v->begin (); i != v->end (); ++i) {
          PyDict_SetItem (ret->get (), c2python<X> (i->first), c2python<Y> (i->second));
        }

      }

    } else {

      map_repr_type v = rr->template read< map_repr_type > ();

      *ret = PyDict_New ();
      for (typename map_repr_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        PyDict_SetItem (ret->get (), c2python<X> (i->first), c2python<Y> (i->second));
      }

    }
  }
};

struct map_reader_error
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &atype)
  {
    tl_assert (false);
  }
};

template <class Y> struct map_reader<gsi::ObjectType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::VectorType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::MapType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::VoidType, Y> : map_reader_error { };

/**
 *  @brief Deseralisation wrapper: specialization for map 
 */
template <> 
struct reader<gsi::MapType>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &atype)
  {
    tl_assert (atype.inner_k () != 0);
    tl_assert (atype.inner () != 0);
    gsi::do_on_type2<map_reader> () (atype.inner_k ()->type (), atype.inner ()->type (), rr, ret, atype);
  }
};

/**
 *  @brief Deseralisation wrapper: specialization for object 
 */
template <> 
struct reader<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &atype)
  {
    if (atype.is_ptr () || atype.is_ref () || atype.is_cptr () || atype.is_cref ()) {

      void *obj = rr->read<void *> ();

      const gsi::ClassBase *clsact = atype.cls ()->subclass_decl (obj);

      ClientData *cd = 0;
      if (clsact && obj) {
        cd = (ClientData *) clsact->client_data (obj, client_index);
      }

      if (obj == 0) {

        //  will be translated to None
        *ret = PythonRef (Py_None, false /*borrowed*/);

      } else if (atype.pass_obj ()) {

        if (! cd || cd->get_ref () == NULL) {

          PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (clsact);
          tl_assert (type != NULL);

          //  create an new managed instance
          PYAObjectBase *new_object = (PYAObjectBase *) type->tp_alloc (type, 0);
          new (new_object) PYAObjectBase (clsact);
          new_object->set (obj, true, false, false);
          *ret = new_object;

        } else {

          //  this object had a Python object attached already (maybe because it was
          //  created on Python side initially) -> reuse that one. NOTE: we don't touch 
          //  the reference count since in that case, the reference count was incremented
          //  when the ownership was passed to the C++ owner.
          *ret = cd->get_ref ();

          //  correct constness if the object is not supposed to be const
          correct_constness (ret->get (), atype.is_cref () || atype.is_cptr ());

        }

      } else if (atype.is_cref () && clsact->can_copy () && cd == 0) {

        //  We copy objects passed by const reference if they are not managed
        //  (derived from gsi::ObjectBase, in that case, client_data is not 0).
        //  Such objects are often exposed internals. First we can't
        //  guarantee the const correctness of references. Second, we
        //  can't guarantee the lifetime of the container will exceed that
        //  of the exposed property. Hence copying is safer.

        PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (clsact);
        tl_assert (type != NULL);

        //  create a instance and copy the value
        PYAObjectBase *new_object = (PYAObjectBase *) type->tp_alloc (type, 0);
        new (new_object) PYAObjectBase (clsact);
        clsact->assign (new_object->obj (), obj);
        *ret = new_object;

      } else if (cd != 0 && cd->get_ref () != NULL) {

        //  we have a that is located in C++ space but is supposed to get attached
        //  a Python object. If it already has, we simply return a reference to this
        *ret = cd->get_ref ();
        Py_INCREF (ret->get ());

        //  correct constness if the object is not supposed to be const
        correct_constness (ret->get (), atype.is_cref () || atype.is_cptr ());

      } else {

        PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (clsact);
        tl_assert (type != NULL);

        //  create a instance 
        //  TODO: we will create a fresh object here, delete it again and link the 
        //  reference to the exisiting object to the Python object. This is not quite
        //  efficient - we should avoid creating and deleting a dummy object first.
        PYAObjectBase *new_object = (PYAObjectBase *) type->tp_alloc (type, 0);
        new (new_object) PYAObjectBase (clsact);
        new_object->set (obj, false, atype.is_cref () || atype.is_cptr (), atype.is_ptr ());
        *ret = new_object;

      }

    } else {

      PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (atype.cls ());
      tl_assert (type != NULL);

      //  create a instance without attaching it
      PYAObjectBase *new_object = (PYAObjectBase *) type->tp_alloc (type, 0);
      new (new_object) PYAObjectBase (atype.cls ());
      new_object->set (atype.cls ()->take_and_create (*rr), true, false, false);
      *ret = new_object;

    }
  }
};

/**
 *  @brief Deseralisation wrapper: specialization for void
 */
template <> 
struct reader<gsi::VoidType>
{
  void operator() (gsi::SerialArgs *rr, PythonRef *ret, const gsi::ArgType &atype)
  {
    //  .. nothing: void is not serialized
  }
};

PyObject *
pop_arg (const gsi::ArgType &atype, gsi::SerialArgs &aserial)
{
  tl_assert (! atype.is_iter ()); // not implemented here 

  PythonRef ret;
  gsi::do_on_type<reader> () (atype.type (), &aserial, &ret, atype);
  return ret.release ();
}

// -------------------------------------------------------------------
//  Test if an argument can be converted to the given type

//  if atype is a vector:
//      argument must be an array of the given type
//  if atype is a ref:
//      argument must be a boxed type of the required type or an object of the requested class
//  if atype is a ptr:
//      argument must be a boxed type of the required type or an object of the requested class or null
//  if atype is a cptr:
//      argument must be of requested type or null
//  otherwise:
//      argument must be of the requested type

template <class R>
struct test_arg_func
{
  void operator() (bool *ret, PyObject *arg, const gsi::ArgType &atype, bool loose)
  {
    if ((atype.is_cptr () || atype.is_ptr ()) && arg == Py_None) {

      //  for const X * or X *, null is an allowed value
      *ret = true;

    } else {

      *ret = false;

      if (atype.is_ptr () || atype.is_ref ()) {

        //  check if we have a boxed type
        const gsi::ClassBase *cls_decl = PythonInterpreter::instance ()->cls_for_type (Py_TYPE (arg));
        if (cls_decl) {
          const gsi::ClassBase *bc = gsi::cls_decl <gsi::Value> ();
          if (cls_decl->is_derived_from (bc)) {
            *ret = true;
          }
        }

      }

      if (! *ret) {
        //  otherwise try a normal match and let the serializer sort out the wrong arguments with
        //  a good error message.
        *ret = test_type<R> (arg, loose);
      }

    }
  }
};

template <>
struct test_arg_func<gsi::VectorType>
{
  void operator() (bool *ret, PyObject *arg, const gsi::ArgType &atype, bool loose)
  {
    if (! PyTuple_Check (arg) && ! PyList_Check (arg)) {
      *ret = false;
      return;
    }

    tl_assert (atype.inner () != 0);
    const gsi::ArgType &ainner = *atype.inner ();

    *ret = true;
    if (PyTuple_Check (arg)) {

      size_t n = PyTuple_Size (arg);
      for (size_t i = 0; i < n && *ret; ++i) {
        if (! test_arg (ainner, PyTuple_GetItem (arg, i), loose)) {
          *ret = false;
        }
      }

    } else {

      size_t n = PyList_Size (arg);
      for (size_t i = 0; i < n && *ret; ++i) {
        if (! test_arg (ainner, PyList_GetItem (arg, i), loose)) {
          *ret = false;
        }
      }

    }
  }
};

template <>
struct test_arg_func<gsi::MapType>
{
  void operator () (bool *ret, PyObject *arg, const gsi::ArgType &atype, bool loose)
  {
    if (! PyDict_Check (arg)) {
      *ret = false;
      return;
    }

    tl_assert (atype.inner () != 0);
    tl_assert (atype.inner_k () != 0);
    const gsi::ArgType &ainner = *atype.inner ();
    const gsi::ArgType &ainner_k = *atype.inner ();

    //  Note: we test key and value separately. That way we don't need to 
    //  instantiate a 2d template with do_on_type2.
    *ret = true;

    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(arg, &pos, &key, &value)) {
      if (! test_arg (ainner_k, key, loose)) {
        *ret = false;
        break;
      }
      if (! test_arg (ainner, value, loose)) {
        *ret = false;
        break;
      }
    }
  }
};
template <>
struct test_arg_func<gsi::ObjectType>
{
  void operator() (bool *ret, PyObject *arg, const gsi::ArgType &atype, bool loose)
  {
    //  for const X * or X *, null is an allowed value
    if ((atype.is_cptr () || atype.is_ptr ()) && arg == Py_None) {
      *ret = true;
      return;
    }

    const gsi::ClassBase *cls_decl = PythonInterpreter::instance ()->cls_for_type (Py_TYPE (arg));
    if (! cls_decl) {
      *ret = false;
      return;
    }
    if (! cls_decl->is_derived_from (atype.cls ())) {
      *ret = false;
      return;
    }
    if ((atype.is_ref () || atype.is_ptr ()) && ((PYAObjectBase *) arg)->const_ref ()) {
      *ret = false;
      return;
    }

    *ret = true;

  }
};

bool
test_arg (const gsi::ArgType &atype, PyObject *arg, bool loose)
{
  bool ret = false;
  gsi::do_on_type<test_arg_func> () (atype.type (), &ret, arg, atype, loose);
  return ret;
}
      
}

#endif

