
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


#include "tlExpression.h"
#include "gsiDecl.h"
#include "gsiExpression.h"

#include <set>
#include <map>
#include <list>
#include <cstdio>
#include <algorithm>

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>

namespace gsi
{

// -------------------------------------------------------------------
//  Method table implementation

/**
 *  @brief A single entry in the method table
 *  This class provides an entry for one name. It provides flags
 *  (ctor) for the method and a list of implementations
 *  (gsi::MethodBase objects)
 */
class ExpressionMethodTableEntry
{
public:
  typedef std::vector<const gsi::MethodBase *>::const_iterator method_iterator;

  ExpressionMethodTableEntry (const std::string &name)
    : m_name (name)
  { }

  const std::string &name () const
  {
    return m_name;
  }

  void add (const gsi::MethodBase *m)
  {
    m_methods.push_back (m);
  }

  void finish ()
  {
    //  remove duplicate entries in the method list
    std::vector<const gsi::MethodBase *> m = m_methods;
    std::sort(m.begin (), m.end ());
    m_methods.assign (m.begin (), std::unique (m.begin (), m.end ()));
  }

  method_iterator begin () const
  {
    return m_methods.begin ();
  }

  method_iterator end () const
  {
    return m_methods.end ();
  }

private:
  std::string m_name;
  std::vector<const gsi::MethodBase *> m_methods;
};

/**
 *  @brief The method table for a class
 *  The method table will provide the methods associated with a native method, i.e.
 *  a certain name. It only provides the methods, not a overload resolution strategy.
 */
class ExpressionMethodTable
  : public gsi::PerClassClientSpecificData
{
public:
  /**
   *  @brief Find a method by name and static flag
   *  This method will return a pair of true and the method ID if a method with 
   *  the static attribute and the name is found. Otherwise the first value of
   *  the returned pair will be false.
   */
  std::pair<bool, size_t> find (bool st, const std::string &name) const
  {
    std::map<std::pair<bool, std::string>, size_t>::const_iterator t = m_name_map.find (std::make_pair (st, name));
    if (t != m_name_map.end ()) {
      return std::make_pair (true, t->second);
    } else {
      return std::make_pair (false, 0);
    }
  }

  /**
   *  @brief Returns the name of the method with ID mid
   */
  const std::string &name (size_t mid) const
  {
    return m_table [mid].name ();
  }

  /**
   *  @brief Begin iterator for the overloaded methods for method ID mid
   */
  ExpressionMethodTableEntry::method_iterator begin (size_t mid) const
  {
    return m_table[mid].begin ();
  }

  /**
   *  @brief End iterator for the overloaded methods for method ID mid
   */
  ExpressionMethodTableEntry::method_iterator end (size_t mid) const
  {
    return m_table[mid].end ();
  }

  static const ExpressionMethodTable *method_table_by_class (const gsi::ClassBase *cls_decl)
  {
    const ExpressionMethodTable *mt = dynamic_cast<const ExpressionMethodTable *>(cls_decl->gsi_data ());
    tl_assert (mt != 0);
    return mt;
  }

  static void initialize_class (const gsi::ClassBase *cls_decl)
  {
    ExpressionMethodTable *mtnc = new ExpressionMethodTable (cls_decl);
    cls_decl->set_gsi_data (mtnc);
  }

private:
  const gsi::ClassBase *mp_cls_decl;
  std::map<std::pair<bool, std::string>, size_t> m_name_map;
  std::vector<ExpressionMethodTableEntry> m_table;

  /**
   *  @brief Adds the given method with the given name to the list of methods registered under that name
   */
  void add_method (const std::string &name, const gsi::MethodBase *mb) 
  {
    bool st = mb->is_static ();

    std::map<std::pair<bool, std::string>, size_t>::iterator n = m_name_map.find (std::make_pair (st, name));
    if (n == m_name_map.end ()) {

      m_name_map.insert (std::make_pair (std::make_pair(st, name), m_table.size ()));
      m_table.push_back (ExpressionMethodTableEntry (name));
      m_table.back ().add (mb);

    } else {

      m_table [n->second].add (mb);

    }
  }

  /**
   *  @brief Private ctor - no construction from the outside
   */
  ExpressionMethodTable ();

  /**
   *  @brief Private ctor - no construction from the outside
   *  This constructor will create the method table for the given class.
   */
  ExpressionMethodTable (const gsi::ClassBase *cls_decl)
    : mp_cls_decl (cls_decl)
  { 
    for (gsi::ClassBase::method_iterator m = cls_decl->begin_methods (); m != cls_decl->end_methods (); ++m) {

      if (! (*m)->is_callback ()) {

        for (gsi::MethodBase::synonym_iterator syn = (*m)->begin_synonyms (); syn != (*m)->end_synonyms (); ++syn) {
          if (syn->is_setter) {
            add_method (syn->name + "=", *m);
          } else {
            add_method (syn->name, *m);
          }
        }

      }

    }

    //  do some cleanup
    for (std::vector<ExpressionMethodTableEntry>::iterator m = m_table.begin (); m != m_table.end (); ++m) {
      m->finish ();
    }
  }
};



// -------------------------------------------------------------------
//  Test if an argument can be converted to the given type

bool test_arg (const gsi::ArgType &atype, const tl::Variant &arg);

template <class R>
struct test_arg_func
{
  void operator () (bool *ret, const tl::Variant &arg, const gsi::ArgType &atype)
  {
    *ret = arg.can_convert_to<R> ();
  }
};

template <>
struct test_arg_func<gsi::VoidType>
{
  void operator () (bool *ret, const tl::Variant &arg, const gsi::ArgType &atype)
  {
    *ret = true;
  }
};

template <>
struct test_arg_func<gsi::ObjectType>
{
  void operator () (bool *ret, const tl::Variant &arg, const gsi::ArgType &atype)
  {
    //  allow nil of pointers
    if ((atype.is_ptr () || atype.is_cptr ()) && arg.is_nil ()) {
      *ret = true;
      return;
    }

    if (! arg.is_user ()) {
      *ret = false;
      return;
    }

    const tl::VariantUserClassBase *cls = arg.user_cls ();
    if (atype.is_cptr () || atype.is_cref ()) {
      *ret = cls && cls->gsi_cls ()->is_derived_from (atype.cls ());
    } else {
      *ret = cls && !cls->is_const () && cls->gsi_cls ()->is_derived_from (atype.cls ());
    }
  }
};

template <>
struct test_arg_func<gsi::VectorType>
{
  void operator () (bool *ret, const tl::Variant &arg, const gsi::ArgType &atype)
  {
    if (! arg.is_list ()) {
      *ret = false;
      return;
    }

    tl_assert (atype.inner () != 0);
    const ArgType &ainner = *atype.inner ();

    *ret = true;
    for (tl::Variant::const_iterator v = arg.begin (); v != arg.end () && *ret; ++v) { 
      if (! test_arg (ainner, *v)) {
        *ret = false;
      }
    }
  }
};

template <>
struct test_arg_func<gsi::MapType>
{
  void operator () (bool *ret, const tl::Variant &arg, const gsi::ArgType &atype)
  {
    //  Note: delegating that to the function avoids "injected class name used as template template expression" warning
    if (! arg.is_array ()) {
      *ret = false;
      return;
    }

    tl_assert (atype.inner () != 0);
    tl_assert (atype.inner_k () != 0);
    const ArgType &ainner = *atype.inner ();
    const ArgType &ainner_k = *atype.inner_k ();

    if (! arg.is_list ()) {
      *ret = false;
      return;
    }

    *ret = true;
    for (tl::Variant::const_array_iterator a = arg.begin_array (); a != arg.end_array () && *ret; ++a) { 
      if (! test_arg (ainner_k, a->first)) {
        *ret = false;
      } else if (! test_arg (ainner, a->second)) {
        *ret = false;
      }
    }
  }
};

bool
test_arg (const gsi::ArgType &atype, const tl::Variant &arg)
{
  //  for const X * or X *, nil is an allowed value
  if ((atype.is_cptr () || atype.is_ptr ()) && arg.is_nil ()) {
    return true;
  }

  bool ret = false;
  gsi::do_on_type<test_arg_func> () (atype.type (), &ret, arg, atype);
  return ret;
}
      
// -------------------------------------------------------------------
//  Variant to C conversion

template <class R>
struct var2c
{
  static R get (const tl::Variant &rval)
  {
    return rval.to<R> ();
  }
};

template <>
struct var2c<tl::Variant>
{
  static const tl::Variant &get (const tl::Variant &rval)
  {
    return rval;
  }
};

// ---------------------------------------------------------------------
//  Serialization helpers

/**
 *  @brief A wrapper for the call arguments
 *
 *  Atop of gsi::SerialArgs this class handles the temporary object stack
 *  and the conversion of the client type (tl::Variant) to the serial 
 *  representation
 */
class AdaptedSerialArgs
  : public gsi::SerialArgs
{
public:
  /**
   *  @brief Constructor
   */
  AdaptedSerialArgs (size_t buffer_len)
    : gsi::SerialArgs (buffer_len)
  { }

  /**
   *  @brief Writes an argument into the buffer
   *
   *  "atype" gives the detailed type and controls how the object will be represented in the
   *  buffer. "arg" is the value of the argument.
   */
  void write_arg (const gsi::ArgType &atype, const tl::Variant &arg);

  /**
   *  @brief Reads an argument from the buffer
   *
   *  The value is read from the buffer using "atype" to specify the exact format in the buffer.
   *  The return value will be written to "out".
   */
  void read_arg (tl::Variant &out, const gsi::ArgType &atype);

private:
  tl::Heap m_heap;
};

/** 
 *  @brief A writer function that helps writing a type to a buffer
 *  It's used inside the function that dispatches the push operation to the various types.
 */
template <class R, class T>
struct writer_impl;

template <class R>
struct writer_impl<R, tl::false_tag>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg.is_nil () && atype.type () != gsi::T_var) {

      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments of reference or direct type cannot be passed nil")));
      } else if (atype.is_ptr ()) {
        aa->write<R *> ((R *)0);
      } else {
        aa->write<const R *> ((const R *)0);
      }

    } else {

      if (atype.is_ref () || atype.is_ptr ()) {

        // TODO: morph the variant to the requested type and pass it's pointer (requires a non-const reference for arg)
        // -> we would have a reference that can modify the argument (out parameter).
        R *v = new R (var2c<R>::get (arg));
        heap->push (v);

        aa->write<void *> (v);

      } else if (atype.is_cref ()) {
        //  Note: POD's are written as copies for const refs, so we can pass a temporary here:
        //  (avoids having to create a temp object)
        aa->write<const R &> (var2c<R>::get (arg));
      } else if (atype.is_cptr ()) {
        //  Note: POD's are written as copies for const ptrs, so we can pass a temporary here:
        //  (avoids having to create a temp object)
        R r = var2c<R>::get (arg);
        aa->write<const R *> (&r);
      } else {
        aa->write<R> (var2c<R>::get (arg));
      }

    }
  }
};

/** 
 *  @brief specialization for non-POD types 
 *  non-POD types are not passed by copy through the buffer for const ref and const ptr and hence
 *  a temporary object must be created.
 */
template <class R>
struct writer_impl<R, tl::true_tag>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  Cannot pass ownership currently
    tl_assert (!atype.pass_obj ());

    if (arg.is_nil () && atype.type () != gsi::T_var) {

      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments of reference or direct type cannot be passed nil")));
      } else if (atype.is_ptr ()) {
        aa->write<R *> ((R *)0);
      } else {
        aa->write<const R *> ((const R *)0);
      }

    } else {

      if (atype.is_ref () || atype.is_ptr () || atype.is_cref () || atype.is_cptr ()) {

        // TODO: morph the variant to the requested type and pass it's pointer (requires a non-const reference for arg)
        // -> we would have a reference that can modify the argument (out parameter).
        R *v = new R (var2c<R>::get (arg));
        heap->push (v);

        aa->write<void *> (v);

      } else {
        aa->write<R> (var2c<R>::get (arg));
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
 *  @brief Specialization for tl::Variant
 */
template <>
struct writer<tl::Variant>
{
  //  TODO: simplify all non-POD types ...
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  TODO: clarify: is nil a zero-pointer to a variant or a pointer to a "nil" variant
    if (atype.is_ref () || atype.is_ptr () || atype.is_cref () || atype.is_cptr ()) {

      // TODO: better to simply pass the tl::Variant's pointer? Would require a non-const Variant ref for example.
      tl::Variant *v = new tl::Variant (arg);
      heap->push (v);

      aa->write<void *> (v);

    } else {
      aa->write<tl::Variant> (var2c<tl::Variant>::get (arg));
    }

  }
};

/**
 *  @brief A writer function assuming a vector of R objects
 */
template <class R>
struct vector_writer
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (atype.is_ptr () || atype.is_cptr ()) {

      std::vector<R> *v = new std::vector<R> ();
      for (tl::Variant::const_iterator a = arr.begin (); a != arr.end (); ++a) { 
        v->push_back (var2c<R>::get (*a));
      }
      heap->push (v);

      if (atype.is_cptr ()) {
        aa->write<const std::vector<R> *> (v);
      } else {
        aa->write<std::vector<R> *> (v);
      }

    } else if (atype.is_ref () || atype.is_cref ()) {

      std::vector<R> *v = new std::vector<R> ();
      for (tl::Variant::const_iterator a = arr.begin (); a != arr.end (); ++a) { 
        v->push_back (var2c<R>::get (*a));
      }
      heap->push (v);

      if (atype.is_cref ()) {
        aa->write<const std::vector<R> &> (*v);
      } else {
        aa->write<std::vector<R> &> (*v);
      }

    } else {
      std::vector<R> *v = reinterpret_cast<std::vector<R> *> (aa->wptr ());
      aa->write<std::vector<R> > (std::vector<R> ());
      for (tl::Variant::const_iterator a = arr.begin (); a != arr.end (); ++a) { 
        v->push_back (var2c<R>::get (*a));
      }
    }
  }
};

/**
 *  @brief A writer function assuming a vector of objects
 */
template <>
struct vector_writer<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    tl_assert (atype.inner () != 0);
    const ArgType &ainner = *atype.inner ();

    std::vector<void *> vobj;
    for (tl::Variant::const_iterator a = arr.begin (); a != arr.end (); ++a) { 
      if (! a->is_user ()) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s)")), ainner.cls ()->name ()));
      }
      const tl::VariantUserClassBase *cls = a->user_cls ();
      if (!cls || !cls->gsi_cls ()->is_derived_from (ainner.cls ())) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s)")), ainner.cls ()->name ()));
      }
      //  TODO: get rid of that const_cast
      vobj.push_back (const_cast<void *> (a->to_user_object ()));
    }

    ainner.cls ()->push_vector_of (*aa, atype, *heap, vobj);
  }
};

/**
 *  @brief Specialization for vector
 */
template <>
struct writer<gsi::VectorType>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if ((atype.is_cptr () || atype.is_ptr ()) && arg.is_nil ()) {

      //  nil is a valid value of a list if a pointer is requested
      aa->write<void *> (0);
      return;

    } else {
      
      if (! arg.is_list ()) {
        throw tl::Exception (tl::to_string (QObject::tr ("List expected instead of")) + " '" + arg.to_string () + "'");
      }

      tl_assert (atype.inner () != 0);
      const ArgType &ainner = *atype.inner ();

      gsi::do_on_type<vector_writer> () (ainner.type (), aa, arg, atype, heap);

    }
  }
};

/**
 *  @brief A writer function assuming a map of X and Y objects
 */
template <class X, class Y>
struct map_writer
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    typedef std::map<X, Y> map_repr_type;

    if (atype.is_ptr () || atype.is_cptr ()) {
      
      map_repr_type *v = new map_repr_type ();
      for (tl::Variant::const_array_iterator a = arr.begin_array (); a != arr.end_array (); ++a) { 
        v->insert (std::make_pair (var2c<X>::get (a->first), var2c<Y>::get (a->second)));
      }
      heap->push (v);

      if (atype.is_cptr ()) {
        aa->write<const map_repr_type *> (v);
      } else {
        aa->write<map_repr_type *> (v);
      }

    } else if (atype.is_ref () || atype.is_cref ()) {

      map_repr_type *v = new map_repr_type ();
      for (tl::Variant::const_array_iterator a = arr.begin_array (); a != arr.end_array (); ++a) { 
        v->insert (std::make_pair (var2c<X>::get (a->first), var2c<Y>::get (a->second)));
      }
      heap->push (v);

      if (atype.is_cref ()) {
        aa->write<const map_repr_type &> (*v);
      } else {
        aa->write<map_repr_type &> (*v);
      }

    } else {
      map_repr_type *v = reinterpret_cast<map_repr_type *> (aa->wptr ());
      aa->write<map_repr_type > (map_repr_type ());
      for (tl::Variant::const_array_iterator a = arr.begin_array (); a != arr.end_array (); ++a) { 
        v->insert (std::make_pair (var2c<X>::get (a->first), var2c<Y>::get (a->second)));
      }
    }
  }
};

//  disable abstract types for keys for now
struct map_writer_error
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    tl_assert (false);
  }
};

template <class Y> struct map_writer<gsi::ObjectType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::VectorType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::MapType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::VoidType, Y> : map_writer_error { };

/**
 *  @brief Specialization for map
 */
template <>
struct writer<gsi::MapType>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  nil is a valid value of an array if a pointer is requested
    if ((atype.is_cptr () || atype.is_ptr ()) && arg.is_nil ()) {
      aa->write<void *> (0);
    } else {
      
      if (! arg.is_array ()) {
        throw tl::Exception (tl::to_string (QObject::tr ("Array expected instead of")) + " '" + arg.to_string () + "'");
      }

      tl_assert (atype.inner () != 0);
      tl_assert (atype.inner_k () != 0);
      const ArgType &ainner = *atype.inner ();
      const ArgType &ainner_k = *atype.inner_k ();

      gsi::do_on_type2<map_writer> () (ainner_k.type (), ainner.type (), aa, arg, atype, heap);

    }
  }
};

/**
 *  @brief Specialization for void
 */
template <>
struct writer<gsi::VoidType>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  nothing - void type won't be serialized
  }
};

/**
 *  @brief Specialization for void
 */
template <>
struct writer<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *aa, const tl::Variant &arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (atype.is_ref () || atype.is_cref () || atype.is_ptr () || atype.is_cptr ()) {

      if (arg.is_nil ()) {
        if (atype.is_ref () || atype.is_cref ()) {
          throw tl::Exception (tl::to_string (QObject::tr ("Cannot pass nil to reference parameters")));
        }
        aa->write<void *> ((void *) 0);
      } else {
        if (! arg.is_user ()) {
          throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s)")), atype.cls ()->name ()));
        }
        const tl::VariantUserClassBase *cls = arg.user_cls ();
        if (!cls || !cls->gsi_cls ()->is_derived_from (atype.cls ())) {
          throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s)")), atype.cls ()->name ()));
        }
        if (cls->is_const () && (atype.is_ref () || atype.is_ptr ())) {
          throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Cannot pass a const reference of class %s to a non-const reference or pointer parameter")), atype.cls ()->name ()));
        }
        //  TODO: get rid of that cast
        aa->write<void *> ((void *) arg.to_user_object ());
      }

    } else {

      if (! arg.is_user ()) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s)")), atype.cls ()->name ()));
      }
      const tl::VariantUserClassBase *cls = arg.user_cls ();
      if (!cls || !cls->gsi_cls ()->is_derived_from (atype.cls ())) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s)")), atype.cls ()->name ()));
      }
      //  TODO: get rid of that const_cast
      atype.cls ()->copy_to (*aa, const_cast<void *> (arg.to_user_object ()));

    }
  }
};

/**
 *  @brief A reader function 
 */
template <class R> 
struct reader
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    if (atype.is_ref ()) {
      *out = rr->template read<R &> ();
    } else if (atype.is_cref ()) {
      *out = rr->template read<const R &> ();
    } else if (atype.is_ptr ()) {
      R *p = rr->template read<R *> ();
      if (p == 0) {
        *out = tl::Variant ();
      } else {
        *out = *p;
      }
    } else if (atype.is_cptr ()) {
      const R *p = rr->template read<const R *> ();
      if (p == 0) {
        *out = tl::Variant ();
      } else {
        *out = *p;
      }
    } else {
      *out = rr->template read<R> ();
    }
  }
};

/**
 *  @brief A reader specialization for void * 
 */
template <> 
struct reader<void *>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (!atype.is_ref ());
    tl_assert (!atype.is_cref ());
    tl_assert (!atype.is_ptr ());
    tl_assert (!atype.is_cptr ());
    *out = size_t (rr->read<void *> ());
  }
};

/**
 *  @brief A reader specialization for const char * 
 */
template <> 
struct reader<const char *>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (!atype.is_ref ());
    tl_assert (!atype.is_cref ());
    tl_assert (!atype.is_ptr ());
    tl_assert (!atype.is_cptr ());
    *out = rr->read<const char *> ();
  }
};

/**
 *  @brief A vector reader function
 */
template <class R> 
struct vector_reader
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    if (atype.is_ref ()) {

      std::vector<R> &v = rr->template read<std::vector<R> &> ();

      *out = tl::Variant::empty_list ();
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        out->push (tl::Variant ());
        out->back () = (const R &) *i;
      }

    } else if (atype.is_cref ()) {

      const std::vector<R> &v = rr->template read<const std::vector<R> &> ();

      *out = tl::Variant::empty_list ();
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        out->push (tl::Variant ());
        out->back () = (const R &) *i;
      }

    } else if (atype.is_ptr ()) {

      std::vector<R> *v = rr->template read<std::vector<R> *> ();

      if (v) {
        *out = tl::Variant::empty_list ();
        for (typename std::vector<R>::const_iterator i = v->begin (); i != v->end (); ++i) {
          out->push (tl::Variant ());
          out->back () = (const R &) *i;
        }
      } else {
        *out = tl::Variant ();
      }

    } else if (atype.is_cptr ()) {

      const std::vector<R> *v = rr->template read<const std::vector<R> *> ();

      if (v) {
        *out = tl::Variant::empty_list ();
        for (typename std::vector<R>::const_iterator i = v->begin (); i != v->end (); ++i) {
          out->push (tl::Variant ());
          out->back () = (const R &) *i;
        }
      } else {
        *out = tl::Variant ();
      }

    } else {

      std::vector<R> v = rr->template read< std::vector<R> > ();

      *out = tl::Variant::empty_list ();
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        out->push (tl::Variant ());
        out->back () = (const R &) *i;
      }

    }
  }
};

/**
 *  @brief Vector reader specialization for std::vector<void *> 
 */
template <> 
struct vector_reader<void *>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (false); //  not implemented
  }
};

/**
 *  @brief Vector reader specialization for std::vector<const char *> 
 */
template <> 
struct vector_reader<const char *>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (false); //  not implemented
  }
};

/**
 *  @brief Some helper stuff for extracting arrays
 */
struct MakeArrayData
{
  MakeArrayData (tl::Variant *_ret, const tl::VariantUserClassBase *_cls)
    : ret (_ret), cls (_cls)
  { }

  tl::Variant *ret;
  const tl::VariantUserClassBase *cls;
};

static void
make_array_f (void *data, void *cobj)
{
  MakeArrayData *ad = (MakeArrayData *)data;
  ad->ret->push (tl::Variant ());
  ad->ret->back ().set_user (0, cobj, ad->cls);
}

/**
 *  @brief Vector reader specialization for objects
 */
template <> 
struct vector_reader<gsi::ObjectType>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (atype.inner () != 0);
    const ArgType &ainner = *atype.inner ();

    *out = tl::Variant::empty_list ();
    const tl::VariantUserClassBase *cls = ainner.cls ()->var_cls (false, false);
    tl_assert (cls != 0);
    MakeArrayData ad (out, cls);
    ainner.cls ()->get_vector_of (*rr, atype, (void *) &ad, &make_array_f);
  }
};

/**
 *  @brief A vector reader function
 */
template <class X, class Y> 
struct map_reader
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    typedef std::map<X, Y> map_repr_type;

    if (atype.is_ref ()) {

      map_repr_type &v = rr->template read<map_repr_type &> ();

      *out = tl::Variant::empty_array ();
      for (typename map_repr_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        out->insert (i->first, i->second);
      }

    } else if (atype.is_cref ()) {

      const map_repr_type &v = rr->template read<const map_repr_type &> ();

      *out = tl::Variant::empty_array ();
      for (typename map_repr_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        out->insert (i->first, i->second);
      }

    } else if (atype.is_ptr ()) {

      map_repr_type *v = rr->template read<map_repr_type *> ();

      if (v) {
        *out = tl::Variant::empty_array ();
        for (typename map_repr_type::const_iterator i = v->begin (); i != v->end (); ++i) {
          out->insert (i->first, i->second);
        }
      } else {
        *out = tl::Variant ();
      }

    } else if (atype.is_cptr ()) {

      const map_repr_type *v = rr->template read<const map_repr_type *> ();

      if (v) {
        *out = tl::Variant::empty_array ();
        for (typename map_repr_type::const_iterator i = v->begin (); i != v->end (); ++i) {
          out->insert (i->first, i->second);
        }
      } else {
        *out = tl::Variant ();
      }

    } else {

      map_repr_type v = rr->template read< map_repr_type > ();

      *out = tl::Variant::empty_array ();
      for (typename map_repr_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        out->insert (i->first, i->second);
      }

    }
  }
};

//  disable abstract types for keys for now
struct map_reader_error
{
  void operator() (tl::Variant *, gsi::SerialArgs *, const gsi::ArgType &)
  {
    tl_assert (false);
  }
};

template <class Y> struct map_reader<gsi::ObjectType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::VectorType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::MapType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::VoidType, Y> : map_reader_error { };

/**
 *  @brief A reader specialization for maps
 */
template <> 
struct reader<MapType>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (atype.inner () != 0);
    tl_assert (atype.inner_k () != 0);
    const ArgType &ainner = *atype.inner ();
    const ArgType &ainner_k = *atype.inner_k ();

    do_on_type2<map_reader> () (ainner_k.type (), ainner.type (), out, rr, atype);
  }
};

/**
 *  @brief A reader specialization for const char * 
 */
template <> 
struct reader<VectorType>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    tl_assert (atype.inner () != 0);
    const ArgType &ainner = *atype.inner ();

    do_on_type<vector_reader> () (ainner.type (), out, rr, atype);
  }
};

/**
 *  @brief A reader specialization for objects
 */
template <> 
struct reader<ObjectType>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    if (atype.is_ptr () || atype.is_cptr () || atype.is_ref () || atype.is_cref ()) {

      void *obj = rr->read<void *> ();
      if (obj == 0) {
        *out = tl::Variant ();
      } else {
        const tl::VariantUserClassBase *cls = atype.cls ()->var_cls (!atype.pass_obj (), atype.is_cref () || atype.is_cptr ());
        tl_assert (cls != 0);
        *out = tl::Variant ();
        out->set_user (0, obj, cls);
      }

    } else {

      const tl::VariantUserClassBase *cls = atype.cls ()->var_cls (false, false);
      tl_assert (cls != 0);
      *out = tl::Variant ();
      out->set_user (0, atype.cls ()->take_and_create (*rr), cls);

    }
  }
};

/**
 *  @brief A reader specialization for new objects
 */
template <> 
struct reader<VoidType>
{
  void
  operator() (tl::Variant *out, gsi::SerialArgs *rr, const gsi::ArgType &atype)
  {
    //  nothing - void type won't be serialized
  }
};

void
AdaptedSerialArgs::write_arg (const gsi::ArgType &atype, const tl::Variant &arg)
{
  gsi::do_on_type<writer> () (atype.type (), this, arg, atype, &m_heap);
}

void
AdaptedSerialArgs::read_arg (tl::Variant &out, const gsi::ArgType &atype)
{
  out = tl::Variant ();

  if (atype.is_iter ()) {
    tl_assert (false); // not implemented here 
  }

  do_on_type<reader> () (atype.type (), &out, this, atype);
}

// ---------------------------------------------------------------------
//  Implementation of initialize_expressions

class EvalClassFunction
  : public tl::EvalFunction
{
public:
  EvalClassFunction (const tl::VariantUserClassBase *var_cls)
    : mp_var_cls (var_cls)
  {
    //  .. nothing yet ..
  }

  void execute (const tl::ExpressionParserContext &context, tl::Variant &out, const std::vector<tl::Variant> &args) const 
  {
    if (! args.empty ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Class is not a function - use 'new' to create a new object")));
    }
    out = tl::Variant (0, 0, mp_var_cls);
  }

private:
  const tl::VariantUserClassBase *mp_var_cls;
};

void KLAYOUT_DLL 
initialize_expressions ()
{
  for (gsi::ClassBase::class_iterator c = gsi::ClassBase::begin_classes (); c != gsi::ClassBase::end_classes (); ++c) {

    //  install the method table:
    ExpressionMethodTable::initialize_class (&*c);

    //  register a function that creates a class object (use a function to avoid issues with
    //  late destruction of global variables which the class object is already gone)
    const tl::VariantUserClassBase *cc = c->var_cls_cls ();
    if (cc) {
      tl::Eval::define_global_function (c->name (), new EvalClassFunction (cc));
    }

  }
}

// -------------------------------------------------------------------------
//  VariantUserClassImpl implementation

VariantUserClassImpl::VariantUserClassImpl () 
  : mp_cls (0), mp_self (0), mp_object_cls (0), m_reference (false), m_is_const (false)
{ 
  //  .. nothing yet ..
}

void 
VariantUserClassImpl::initialize (const gsi::ClassBase *cls, const tl::VariantUserClassBase *self, const tl::VariantUserClassBase *object_cls, bool reference, bool is_const)
{
  mp_cls = cls;
  mp_self = self;
  mp_object_cls = object_cls;
  m_reference = reference; 
  m_is_const = is_const;
}

VariantUserClassImpl::~VariantUserClassImpl () 
{ 
  mp_cls = 0;
}

bool
VariantUserClassImpl::has_method (const std::string &method) const
{
  const gsi::ClassBase *cls = mp_cls;

  while (cls) {
    if (ExpressionMethodTable::method_table_by_class (cls)->find (false, method).first) {
      return true;
    }
    cls = cls->base ();
  }

  return false;
}

bool 
VariantUserClassImpl::equal_impl (void *obj, void *other) const 
{
  if (obj) {

    if (! has_method ("==")) {

      //  No == method - use object identity
      return (void *) this == other;

    } else {

      tl::ExpressionParserContext context;

      tl::Variant out;

      VariantUserClass<void> ref_cls;
      ref_cls.initialize (mp_cls, mp_object_cls, true, true);
      tl::Variant object (0, obj, &ref_cls);
      std::vector<tl::Variant> vv;
      vv.resize (1, tl::Variant ());
      vv[0].set_user (0, other, &ref_cls);

      execute_gsi (context, out, object, "==", vv);

      return out.to_bool ();

    }

  } else {
    return false; 
  }
}

bool 
VariantUserClassImpl::less_impl (void *obj, void *other) const 
{
  if (obj) {

    if (! has_method ("<")) {

      //  No < method - use object pointers
      return (void *) this < other;

    } else {

      tl::ExpressionParserContext context;

      tl::Variant out;

      VariantUserClass<void> ref_cls;
      ref_cls.initialize (mp_cls, mp_object_cls, true, true);
      tl::Variant object (0, obj, &ref_cls);
      std::vector<tl::Variant> vv;
      vv.resize (1, tl::Variant ());
      vv[0].set_user (0, other, &ref_cls);

      execute_gsi (context, out, object, "<", vv);

      return out.to_bool ();

    }

  } else {
    return false; 
  }
}

std::string 
VariantUserClassImpl::to_string_impl (void *obj) const 
{
  if (obj) {

    if (! has_method ("to_s")) {

      //  no method to convert the object to a string
      return std::string ();

    } else {

      tl::ExpressionParserContext context;

      tl::Variant out;

      VariantUserClass<void> ref_cls;
      ref_cls.initialize (mp_cls, mp_object_cls, true, true);
      tl::Variant object (0, obj, &ref_cls);
      std::vector<tl::Variant> vv;

      execute_gsi (context, out, object, "to_s", vv);

      return out.to_string ();

    }

  } else {
    return std::string (); 
  }
}

void
VariantUserClassImpl::execute (const tl::ExpressionParserContext &context, tl::Variant &out, tl::Variant &object, const std::string &method, const std::vector<tl::Variant> &args) const
{
  if (mp_object_cls == 0 && method == "is_a") {

    if (args.size () != 1) {
      throw tl::EvalError (tl::to_string (QObject::tr ("'is_a' method requires exactly one argument")), context);
    }

    bool ret = false;
    if (args [0].is_user ()) {
      const tl::VariantUserClassBase *ub = args [0].user_cls ();
      if (ub && ub->gsi_cls () == mp_cls) {
        ret = true;
      }
    }

    out = ret;

  } else if (mp_object_cls != 0 && method == "new" && args.size () == 0) {

    void *obj = mp_cls->create ();
    if (obj) {
      out.set_user (0, obj, mp_object_cls);
    } else {
      out.reset ();
    }

  } else if (mp_object_cls == 0 && method == "dup") {

    if (args.size () != 0) {
      throw tl::EvalError (tl::to_string (QObject::tr ("'dup' method does not allow arguments")), context);
    }

    void *obj = mp_cls->create ();
    if (obj) {
      mp_cls->assign (obj, object.to_user_object ());
      out.set_user (0, obj, mp_self);
    } else {
      out.reset ();
    }

  } else {
    try {
      execute_gsi (context, out, object, method, args);
    } catch (tl::EvalError &) {
      throw;
    } catch (tl::Exception &ex) {
      throw tl::EvalError (ex.msg (), context);
    }
  }
}

void
VariantUserClassImpl::execute_gsi (const tl::ExpressionParserContext &context, tl::Variant &out, tl::Variant &object, const std::string &method, const std::vector<tl::Variant> &args) const
{
  const gsi::ClassBase *cls = mp_cls;

  const ExpressionMethodTable *mt = 0;
  size_t mid = 0;
  
  while (cls) {

    mt = ExpressionMethodTable::method_table_by_class (cls);
    std::pair<bool, size_t> t = mt->find (mp_object_cls != 0 /*static*/, method);
    if (t.first) {
      mid = t.second;
      break;
    }

    cls = cls->base ();

  }

  if (cls == 0) {
    throw tl::Exception (tl::to_string (QObject::tr ("Unknown method")) + " '" + method + "'");
  }

  const gsi::MethodBase *meth = 0;
  int candidates = 0;

  for (ExpressionMethodTableEntry::method_iterator m = mt->begin (mid); m != mt->end (mid); ++m) {

    if ((*m)->is_event()) {
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Events are not supported inside expressions (event %s)")), method.c_str ()));
    } else if ((*m)->is_callback()) {
      //  ignore callbacks
    } else if ((*m)->compatible_with_num_args (args.size ())) {
      ++candidates;
      meth = *m;
    }

  }

  //  no candidate -> error
  if (! meth) {

    std::set<unsigned int> nargs;
    for (ExpressionMethodTableEntry::method_iterator m = mt->begin (mid); m != mt->end (mid); ++m) {
      nargs.insert (std::distance ((*m)->begin_arguments (), (*m)->end_arguments ()));
    }
    std::string nargs_s;
    for (std::set<unsigned int>::const_iterator na = nargs.begin (); na != nargs.end (); ++na) {
      if (na != nargs.begin ()) {
        nargs_s += "/";
      }
      nargs_s += tl::to_string (*na);
    }

    throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Invalid number of arguments for method %s, class %s (got %d, expected %s)")), method.c_str (), mp_cls->name (), int (args.size ()), nargs_s));
  }

  //  more than one candidate -> refine by checking the arguments
  if (candidates > 1) {
    meth = 0;
    candidates = 0;
    for (ExpressionMethodTableEntry::method_iterator m = mt->begin (mid); m != mt->end (mid); ++m) {
      bool is_valid = (*m)->compatible_with_num_args (args.size ());
      size_t narg = 0;
      for (gsi::MethodBase::argument_iterator a = (*m)->begin_arguments (); is_valid && a != (*m)->end_arguments () && narg < args.size (); ++a, ++narg) {
        if (! test_arg (*a, args [narg])) {
          is_valid = false;
        }
      }
      if (is_valid) {
        ++candidates;
        meth = (*m);
      }
    }
  }

  if (! meth) {
    throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("No method with matching arguments for method %s, class %s")), method.c_str (), mp_cls->name ()));
  }

  if (candidates > 1) {
    throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Ambiguous overload variants for method %s, class %s - multiple method declarations match arguments")), method.c_str (), mp_cls->name ()));
  }

  if (m_is_const && ! meth->is_const ()) {
    throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Cannot call non-const method %s, class %s on a const reference")), method.c_str (), mp_cls->name ()));
  }

  if (meth->is_event ()) {
    //  TODO: events not supported yet
    throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Events not supported yet (method %s, class %s)")), method.c_str (), mp_cls->name ()));
  } else {

    AdaptedSerialArgs arglist (meth->argsize ());
    size_t narg = 0;
    for (gsi::MethodBase::argument_iterator a = meth->begin_arguments (); a != meth->end_arguments () && narg < args.size (); ++a, ++narg) {
      arglist.write_arg (*a, args [narg]);
    }

    AdaptedSerialArgs retlist (meth->retsize ());
    meth->call (object.to_user_object (), arglist, retlist);

    if (meth->ret_type ().is_iter ()) {
      //  TODO: iterators not supported yet
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Iterators not supported yet (method %s, class %s)")), method.c_str (), mp_cls->name ()));
    } else {
      retlist.read_arg (out, meth->ret_type ());
    }

  }
}

}

