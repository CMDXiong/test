
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


#ifndef _HDR_gsiClass
#define _HDR_gsiClass

#include "tlTypeTraits.h"
#include "tlHeap.h"

#include <memory>

//  For a comprehensive documentation see gsi.h

namespace gsi
{

// -----------------------------------------------------------------------------
//  Variant binding

/**
 *  @brief A helper function to implement equal as efficiently as possible
 */
template<class T, class I>
bool _var_user_equal_impl (const T *a, const T *b, const VariantUserClassImpl *delegate, I);

template<class T>
bool _var_user_equal_impl (const T *a, const T *b, const VariantUserClassImpl *delegate, tl::true_tag)
{
  return *a == *b;
}

template<class T>
bool _var_user_equal_impl (const T *a, const T *b, const VariantUserClassImpl *delegate, tl::false_tag)
{
  return delegate->equal_impl ((void *) a, (void *) b);
}

/**
 *  @brief A helper function to implement less as efficiently as possible
 */
template<class T, class I>
bool _var_user_less_impl (const T *a, const T *b, const VariantUserClassImpl *delegate, I);

template<class T>
bool _var_user_less_impl (const T *a, const T *b, const VariantUserClassImpl *delegate, tl::true_tag)
{
  return *a < *b;
}

template<class T>
bool _var_user_less_impl (const T *a, const T *b, const VariantUserClassImpl *delegate, tl::false_tag)
{
  return delegate->less_impl ((void *) a, (void *) b);
}

/**
 *  @brief A helper function to implement to_string as efficiently as possible
 */
template<class T, class I>
std::string _var_user_to_string_impl (const T *a, const VariantUserClassImpl *delegate, I);

template<class T>
std::string _var_user_to_string_impl (const T *a, const VariantUserClassImpl *delegate, tl::true_tag)
{
  return a->to_string ();
}

template<class T>
std::string _var_user_to_string_impl (const T *a, const VariantUserClassImpl *delegate, tl::false_tag)
{
  return delegate->to_string_impl ((void *) a);
}

/**
 *  @brief A helper function to implement read as efficiently as possible
 */
template<class T, class I>
void _var_user_read_impl (T *a, tl::Extractor &ex, I);

template<class T>
void _var_user_read_impl (T *a, tl::Extractor &ex, tl::true_tag)
{
  ex.read (*a);
}

template<class T>
void _var_user_read_impl (T *a, tl::Extractor &ex, tl::false_tag)
{
  tl_assert (false);
}

/**
 *  @brief A VariantUserClassBase specialization that links GSI classes and Variant classes
 */
template <class T>
class VariantUserClass
  : public tl::VariantUserClass<T>, private VariantUserClassImpl
{
public:
  VariantUserClass ()
    : VariantUserClassImpl (),
      mp_cls (0), mp_object_cls (0), m_reference (false), m_is_const (false)
  {
    //  .. nothing yet ..
  }

  ~VariantUserClass ()
  {
    mp_cls = 0;
    tl::VariantUserClass<T>::unregister_instance (this, m_reference, m_is_const);
  }

  void initialize (const gsi::ClassBase *cls, const tl::VariantUserClassBase *object_cls, bool reference, bool is_const)
  {
    mp_cls = cls; 
    mp_object_cls = object_cls;
    m_reference = reference;
    m_is_const = is_const;

    VariantUserClassImpl::initialize (cls, this, object_cls, reference, is_const);

    tl::VariantUserClass<T>::register_instance (this, m_reference, m_is_const);
  }

  const tl::EvalClass *eval_cls () const 
  {
    return this;
  }

  virtual bool equal (void *a, void *b) const
  {
    typename tl::type_traits<T>::has_equal_operator f;
    return gsi::_var_user_equal_impl ((const T *) a, (const T *) b, this, f);
  }

  virtual bool less (void *a, void *b) const
  {
    typename tl::type_traits<T>::has_less_operator f;
    return gsi::_var_user_less_impl ((const T *) a, (const T *) b, this, f);
  }

  virtual std::string to_string (void *a) const
  {
    typename tl::type_traits<T>::supports_to_string f;
    return gsi::_var_user_to_string_impl ((const T *) a, this, f);
  }

  void *clone (void *obj) const 
  {
    if (m_reference) {
      return obj;
    } else {
      void *new_obj = mp_cls->create ();
      mp_cls->assign (new_obj, obj);
      return new_obj;
    }
  }

  void *create () const 
  {
    tl_assert (!m_reference);
    return mp_cls->create ();
  }

  void destroy (void *obj) const 
  {
    if (obj && !m_reference) {
      mp_cls->destroy (obj);
    }
  }

  const char *name () const
  {
    return mp_cls ? mp_cls->name ().c_str() : 0;
  }

  void read (void *a, tl::Extractor &ex) const
  {
    typename tl::type_traits<T>::supports_extractor f;
    gsi::_var_user_read_impl ((T *) a, ex, f);
  }

  const gsi::ClassBase *gsi_cls () const
  {
    return mp_cls;
  }

  bool is_class () const 
  { 
    return mp_object_cls != 0; 
  }

  bool is_ref () const 
  { 
    return m_reference; 
  }

  bool is_const () const 
  { 
    return m_is_const; 
  }

private:
  const gsi::ClassBase *mp_cls;
  const tl::VariantUserClassBase *mp_object_cls;
  bool m_reference, m_is_const;

  //  no copying currently
  VariantUserClass &operator= (const VariantUserClass &);
  VariantUserClass (const VariantUserClass &);
};

// -----------------------------------------------------------------------------
//  GSI implementation

template <class X> 
void *_take_and_create (SerialArgs &args, tl::false_tag /*has_copy_ctor*/)
{
  tl_assert (false); // cannot copy object of this type
  return 0;
}

template <class X> 
void *_take_and_create (SerialArgs &from, tl::true_tag /*has_copy_ctor*/)
{
  return from.template take_object<X> ();
}

template <class X> 
void *_get_vector_of (SerialArgs &from, const ArgType &a, void *data, void (*cb) (void *data, void *obj), tl::false_tag /*has_copy_ctor*/) 
{
  tl_assert (false); // cannot copy object of this type
  return 0;
}

template <class X> 
void _get_vector_of (SerialArgs &from, const ArgType &a, void *data, void (*cb) (void *data, void *obj), tl::true_tag /*has_copy_ctor*/) 
{
  std::vector<X> vv;
  const std::vector<X> *v = &vv;
  if (a.is_cref ()) {
    v = &from.template read<const std::vector<X> &> ();
  } else if (a.is_cptr ()) {
    v = from.template read<const std::vector<X> *> ();
  } else if (a.is_ref ()) {
    v = &from.template read<std::vector<X> &> ();
  } else if (a.is_ptr ()) {
    v = from.template read<std::vector<X> *> ();
  } else {
    vv = from.template read< std::vector<X> > ();
  }
  for (typename std::vector<X>::const_iterator o = v->begin (); o != v->end (); ++o) {
    (*cb) (data, new X (*o));
  }
}

template <class X> 
void _get_cptr_vector_of (SerialArgs &from, const ArgType &a, void *data, void (*cb) (void *data, void *obj))
{
  std::vector<const X *> vv;
  const std::vector<const X *> *v = &vv;
  if (a.is_cref ()) {
    v = &from.template read<const std::vector<const X *> &> ();
  } else if (a.is_cptr ()) {
    v = from.template read<const std::vector<const X *> *> ();
  } else if (a.is_ref ()) {
    v = &from.template read<std::vector<const X *> &> ();
  } else if (a.is_ptr ()) {
    v = from.template read<std::vector<const X *> *> ();
  } else {
    vv = from.template read< std::vector<const X *> > ();
  }
  for (typename std::vector<const X *>::const_iterator o = v->begin (); o != v->end (); ++o) {
    (*cb) (data, (void *) *o);
  }
}

template <class X> 
void _get_ptr_vector_of (SerialArgs &from, const ArgType &a, void *data, void (*cb) (void *data, void *obj))
{
  std::vector<X *> vv;
  const std::vector<X *> *v = &vv;
  if (a.is_cref ()) {
    v = &from.template read<const std::vector<X *> &> ();
  } else if (a.is_cptr ()) {
    v = from.template read<const std::vector<X *> *> ();
  } else if (a.is_ref ()) {
    v = &from.template read<std::vector<X *> &> ();
  } else if (a.is_ptr ()) {
    v = from.template read<std::vector<X *> *> ();
  } else {
    vv = from.template read< std::vector<X *> > ();
  }
  for (typename std::vector<X *>::const_iterator o = v->begin (); o != v->end (); ++o) {
    (*cb) (data, *o);
  }
}

template <class X> 
void _destroy (X *x, tl::false_tag /*has public dtor*/)
{
  tl_assert (false); // cannot delete object of this type
}

template <class X> 
void _destroy (X *x, tl::true_tag /*has public dtor*/)
{
  delete x;
}

template <class X> 
void _push_vector_of (SerialArgs &to, const ArgType &a, tl::Heap &heap, const std::vector<void *> &objects, tl::false_tag /*has_copy_ctor*/) 
{
  tl_assert (false); // cannot copy object of this type
}

template <class X> 
void _push_vector_of (SerialArgs &to, const ArgType &a, tl::Heap &heap, const std::vector<void *> &objects, tl::true_tag /*has_copy_ctor*/) 
{
  tl_assert (a.inner () != 0);

  std::vector<X> vv;
  std::vector<X> *v;
  if (a.is_ref () || a.is_cref () || a.is_ptr () || a.is_cptr ()) {
    v = new std::vector<X> ();
    heap.push (v);
  } else {
    v = &vv;
  }

  v->reserve (objects.size ());
  for (std::vector<void *>::const_iterator o = objects.begin (); o != objects.end (); ++o) {
    v->push_back (*(X *)*o);
  }

  if (a.is_cref ()) {
    to.write<const std::vector<X> &> (*v);
  } else if (a.is_cptr ()) {
    to.write<const std::vector<X> *> (v);
  } else if (a.is_ref ()) {
    to.write<std::vector<X> &> (*v);
  } else if (a.is_ptr ()) {
    to.write<std::vector<X> *> (v);
  } else {
    to.write<std::vector<X> > (vv);
  }
}

template <class X> 
void _push_cptr_vector_of (SerialArgs &to, const ArgType &a, tl::Heap &heap, const std::vector<void *> &objects)
{
  tl_assert (a.inner () != 0);

  std::vector<const X *> vv;
  std::vector<const X *> *v;
  if (a.is_ref () || a.is_cref () || a.is_ptr () || a.is_cptr ()) {
    v = new std::vector<const X *> ();
    heap.push (v);
  } else {
    v = &vv;
  }

  v->reserve (objects.size ());
  for (std::vector<void *>::const_iterator o = objects.begin (); o != objects.end (); ++o) {
    v->push_back ((const X *)*o);
  }

  if (a.is_cref ()) {
    to.write<const std::vector<const X *> &> (*v);
  } else if (a.is_cptr ()) {
    to.write<const std::vector<const X *> *> (v);
  } else if (a.is_ref ()) {
    to.write<std::vector<const X *> &> (*v);
  } else if (a.is_ptr ()) {
    to.write<std::vector<const X *> *> (v);
  } else {
    to.write<std::vector<const X *> > (vv);
  }
}

template <class X> 
void _push_ptr_vector_of (SerialArgs &to, const ArgType &a, tl::Heap &heap, const std::vector<void *> &objects)
{
  tl_assert (a.inner () != 0);

  std::vector<X *> vv;
  std::vector<X *> *v;
  if (a.is_ref () || a.is_cref () || a.is_ptr () || a.is_cptr ()) {
    v = new std::vector<X *> ();
    heap.push (v);
  } else {
    v = &vv;
  }

  v->reserve (objects.size ());
  for (std::vector<void *>::const_iterator o = objects.begin (); o != objects.end (); ++o) {
    v->push_back ((X *)*o);
  }

  if (a.is_cref ()) {
    to.write<const std::vector<X *> &> (*v);
  } else if (a.is_cptr ()) {
    to.write<const std::vector<X *> *> (v);
  } else if (a.is_ref ()) {
    to.write<std::vector<X *> &> (*v);
  } else if (a.is_ptr ()) {
    to.write<std::vector<X *> *> (v);
  } else {
    to.write<std::vector<X *> > (vv);
  }
}

template <class X> 
void *_create (tl::false_tag)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Object cannot be created here")));
  return 0;
}

template <class X> 
void *_create (tl::true_tag)
{
  return new X ();
}

template <class X> 
void *_clone (tl::false_tag, const void *other)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Object cannot be copied here")));
  return 0;
}

template <class X> 
void *_clone (tl::true_tag, const void *other)
{
  return new X (*(const X *)other);
}

template <class X> 
void _assign (tl::false_tag /*has_copy_ctor*/, void *, const void *)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Object cannot be copied here")));
}

template <class X> 
void _assign (tl::true_tag /*has_copy_ctor*/, void *dest, const void *src)
{
  *(X *)dest = *(const X *)src;
}

template <class X> 
void _copy_to (SerialArgs &at, void *from, tl::false_tag /*has_copy_ctor*/) 
{
  throw tl::Exception (tl::to_string (QObject::tr ("Object cannot be copied here")));
}

template <class X> 
void _copy_to (SerialArgs &at, void *from, tl::true_tag /*has_copy_ctor*/) 
{
  at.write<X> (*((const X *)from));
}

inline void *get_client_data (void *, int ci) 
{
  return 0;
}

inline void *get_client_data (gsi::ObjectBase *x, int ci) 
{
  return x->data (ci);
}

/**
 *  @brief A class predicate telling us whether X is polymorphic
 *
 *  The trick was taken from boost - if a class is made polymorphic and it
 *  has not been before, the size of the object changes since a virtual 
 *  function table will be added.
 */
template <class X>
struct is_polymorphic
{
private:
  struct P1 : public X
  {
    P1 ();
    ~P1 ();
    int something;
  };

  struct P2 : public X
  {
    P2 ();
    virtual ~P2 ();
    int something;
  };

public:
  typedef typename tl::boolean_value<sizeof (P1) == sizeof (P2)>::value value;
};

/**
 *  @brief A helper class which tests whether a given object can be upcast 
 */
class SubClassTesterBase
{
public:
  SubClassTesterBase () { }
  virtual ~SubClassTesterBase () { }

  virtual bool can_upcast (const void *p) const = 0;
};

/**
 *  @brief A specific implementation of the upcast tester
 *  
 *  The can_upcast method will return true, if the object (which has at least to 
 *  be of type B) can be upcast to X.
 */
template <class X, class B, class B_IS_POLYMORPHIC>
class SubClassTester;

/**
 *  @brief Specialization for polymorphic types - we can use dynamic_cast to tests whether B object can be cast to X
 */
template <class X, class B>
class SubClassTester<X, B, tl::true_tag>
  : public SubClassTesterBase 
{
public:
  virtual bool can_upcast (const void *p) const 
  {
    return dynamic_cast<const X *>((const B *)p) != 0;
  }
};

/**
 *  @brief Specialization for non-polymorphic types
 */
template <class X, class B>
class SubClassTester<X, B, tl::false_tag>
  : public SubClassTesterBase 
{
public:
  virtual bool can_upcast (const void *p) const 
  {
    //  Non-polymorphic classes can't be upcast, hence we always return false here
    return false;
  }
};

/**
 *  @brief An extension declaration
 *
 *  Instantiating an object of this kind will extend the class X with 
 *  the given methods.
 */
template <class X>
class ClassExt
  : public ClassBase
{
public:
  typedef typename tl::type_traits<X>::has_copy_constructor has_copy_ctor;
  typedef typename tl::type_traits<X>::has_default_constructor has_default_ctor;
  typedef typename tl::type_traits<X>::has_public_destructor has_public_dtor;

  ClassExt (const Methods &mm, const std::string &doc = std::string ())
    : ClassBase (0, 0, doc, mm), mp_declaration (0)
  {
    //  .. nothing yet ..
  }

  ClassExt (const std::string &doc = std::string ())
    : ClassBase (0, 0, doc, Methods ()), mp_declaration (0)
  {
    //  .. nothing yet ..
  }

  template <class I>
  ClassExt (const Class<I> &import, const std::string &name, const std::string &doc = std::string ())
    : ClassBase (0, 0, doc, Methods ()), mp_declaration (&import)
  {
    set_name (name);
  }

  virtual bool is_of_type (const std::type_info &ti) const 
  {
    return (ti == typeid (X));
  }

  virtual const std::type_info &type () const
  {
    return typeid (X);
  }

  /**
   *  @brief Gets the real (main) declaration object
   *  The main declaration object is 0 initially indicating that the classes
   *  have not been merged.
   */
  virtual const ClassBase *declaration () const 
  {
    return mp_declaration;
  }

private:
  const ClassBase *mp_declaration;
};

/**
 *  @brief The declaration of a specific class
 *
 *  This class declares all methods that are required to instantiate or copy an object
 *  or to call it's methods in some generic way.
 */
template <class X>
class Class
  : public ClassBase
{
public:
  typedef typename tl::type_traits<X>::has_copy_constructor has_copy_ctor;
  typedef typename tl::type_traits<X>::has_default_constructor has_default_ctor;
  typedef typename tl::type_traits<X>::has_public_destructor has_public_dtor;

  Class (const std::string &name, const Methods &mm, const std::string &doc = std::string ())
    : ClassBase (0, 0, doc, mm)
  {
    set_name (name);
  }

  template <class B>
  Class (const Class<B> &base, const std::string &name, const Methods &mm, const std::string &doc = std::string ())
    : ClassBase (0, &base, doc, mm), m_subclass_tester (new SubClassTester<X, B, typename is_polymorphic<B>::value> ())
  {
    set_name (name);
  }

  Class (const std::string &name, const std::string &doc = std::string ())
    : ClassBase (0, 0, doc, Methods ())
  {
    set_name (name);
  }

  template <class B>
  Class (const Class<B> &base, const std::string &name, const std::string &doc = std::string ())
    : ClassBase (0, &base, doc, Methods ()), m_subclass_tester (new SubClassTester<X, B, typename is_polymorphic<B>::value> ())
  {
    set_name (name);
  }

  Class (const ObjectConnector &connector, const std::string &name, const Methods &mm, const std::string &doc = std::string ())
    : ClassBase (&connector, 0, doc, mm) 
  {
    set_name (name);
  }

  template <class B>
  Class (const ObjectConnector &connector, const Class<B> &base, const std::string &name, const Methods &mm, const std::string &doc = std::string ())
    : ClassBase (&connector, &base, doc, mm), m_subclass_tester (new SubClassTester<X, B, typename is_polymorphic<B>::value> ())
  {
    set_name (name);
  }

  Class (const ObjectConnector &connector, const std::string &name, const std::string &doc = std::string ())
    : ClassBase (&connector, 0, doc, Methods ())
  {
    set_name (name);
  }

  template <class B>
  Class (const ObjectConnector &connector, const Class<B> &base, const std::string &name, const std::string &doc = std::string ())
    : ClassBase (&connector, &base, doc, Methods ()), m_subclass_tester (new SubClassTester<X, B, typename is_polymorphic<B>::value> ())
  {
    set_name (name);
  }

  virtual const ClassBase *declaration () const
  {
    return this;
  }

  void initialize ()
  {
    ClassBase::initialize ();
    m_var_cls.initialize (this, 0, false, false);
    m_var_cls_r.initialize (this, 0, true, false);
    m_var_cls_c.initialize (this, 0, false, true);
    m_var_cls_rc.initialize (this, 0, true, true);
    m_var_cls_cls.initialize (this, &m_var_cls, true, false);
  }

  void *client_data (void *p, int ci) const
  {
    void *cd = basic_client_data (p, ci);
    if (cd) {
      return cd;
    } else {
      X *x = (X *)p;
      return get_client_data (x, ci);
    }
  }

  virtual void destroy (void *p) const
  {
    X *x = (X *)p;
    has_public_dtor hpd;
    _destroy (x, hpd);
  }

  virtual void *create () const
  {
    has_default_ctor cst;
    void *r = _create<X> (cst);
    return r;
  }

  virtual void *clone (const void *other) const
  {
    has_copy_ctor cst;
    void *r = _clone<X> (cst, other);
    return r;
  }

  virtual void assign (void *dest, const void *src) const
  {
    has_copy_ctor cst;
    _assign<X> (cst, dest, src);
  }

  virtual void get_vector_of (SerialArgs &from, const ArgType &a, void *data, void (*cb) (void *data, void *obj)) const 
  {
    has_copy_ctor cpt;
    tl_assert (a.inner () != 0);
    if (a.inner ()->is_cptr ()) {
      _get_cptr_vector_of<X> (from, a, data, cb);
    } else if (a.inner ()->is_ptr ()) {
      _get_ptr_vector_of<X> (from, a, data, cb);
    } else {
      _get_vector_of<X> (from, a, data, cb, cpt);
    }
  }

  void push_vector_of (SerialArgs &to, const ArgType &a, tl::Heap &heap, const std::vector<void *> &objects) const 
  {
    has_copy_ctor cpt;
    tl_assert (a.inner () != 0);
    if (a.inner ()->is_cptr ()) {
      _push_cptr_vector_of<X> (to, a, heap, objects);
    } else if (a.inner ()->is_ptr ()) {
      _push_ptr_vector_of<X> (to, a, heap, objects);
    } else {
      _push_vector_of<X> (to, a, heap, objects, cpt);
    }
  }

  virtual void *take_and_create (SerialArgs &from) const
  {
    has_copy_ctor cpt;
    return _take_and_create<X> (from, cpt);
  }

  virtual void copy_to (SerialArgs &at, void *from) const 
  {
    has_copy_ctor cpt;
    _copy_to<X> (at, from, cpt);
  }

  virtual void *create_obj_iterator (SerialArgs &from) const
  {
    return from.template read<gsi::IterAdaptorAbstractBase *> ();
  }

  virtual bool obj_iterator_at_end (void *iter) const
  {
    gsi::IterAdaptorAbstractBase *i = (gsi::IterAdaptorAbstractBase *) iter;
    return i->at_end ();
  }

  virtual void *take_iterator_obj (void *iter) const
  {
    gsi::IterAdaptorAbstractBase *i = (gsi::IterAdaptorAbstractBase *) iter;
    return i->v_get_copy ();
  }

  virtual const void *get_iterator_obj (void *iter) const
  {
    gsi::IterAdaptorAbstractBase *i = (gsi::IterAdaptorAbstractBase *) iter;
    return i->v_get ();
  }

  virtual void inc_obj_iterator (void *iter) const
  {
    gsi::IterAdaptorAbstractBase *i = (gsi::IterAdaptorAbstractBase *) iter;
    i->inc ();
  }

  virtual void delete_obj_iterator (void *iter) const
  {
    gsi::IterAdaptorAbstractBase *i = (gsi::IterAdaptorAbstractBase *) iter;
    delete i;
  }

  virtual bool can_destroy () const
  {
    has_public_dtor hpd;
    return tl::value_of (hpd);
  }

  virtual bool can_copy () const
  {
    has_copy_ctor cpt;
    return tl::value_of (cpt);
  }

  virtual bool can_default_create () const
  {
    has_default_ctor cpt;
    return tl::value_of (cpt);
  }

  virtual const ClassBase *subclass_decl (const void *p) const 
  {
    if (p) {
      for (std::vector <const ClassBase *>::const_iterator s = subclasses ().begin (); s != subclasses ().end (); ++s) {
        if ((*s)->can_upcast (p)) {
          return (*s)->subclass_decl (p);
        }
      }
    }

    return this;
  }

  virtual bool can_upcast (const void *p) const 
  {
    return m_subclass_tester.get () && m_subclass_tester->can_upcast (p);
  }

  virtual bool is_of_type (const std::type_info &ti) const 
  {
    return (ti == typeid (X));
  }

  virtual const std::type_info &type () const
  {
    return typeid (X);
  }

  virtual const tl::VariantUserClassBase *var_cls_cls () const
  {
    return &m_var_cls_cls;
  }

  virtual const tl::VariantUserClassBase *var_cls (bool reference, bool is_const) const
  {
    if (reference && is_const) {
      return &m_var_cls_rc;
    } else if (reference) {
      return &m_var_cls_r;
    } else if (is_const) {
      return &m_var_cls_c;
    } else {
      return &m_var_cls;
    }
  }

private:
  gsi::VariantUserClass<X> m_var_cls;
  gsi::VariantUserClass<X> m_var_cls_r;
  gsi::VariantUserClass<X> m_var_cls_c;
  gsi::VariantUserClass<X> m_var_cls_rc;
  gsi::VariantUserClass<X> m_var_cls_cls;
  std::auto_ptr<SubClassTesterBase> m_subclass_tester;
};

/**
 *  @brief Obtain the class declaration for a given class
 * 
 *  This method looks up the declaration object for a given type.
 *  It does so dynamically, since declarations may be located in different
 *  libraries. However, for performance reasons, the definitions are cached. 
 */
template <class X>
const ClassBase *cls_decl ()
{
  //  TODO: needs thread safety? It's rather unlikely that two threads enter this
  //  piece of code at the same time and they interfere when storing the results.
  static const ClassBase *cd = 0;
  if (! cd) {
    for (ClassBase::class_iterator c = ClassBase::begin_classes (); c != ClassBase::end_classes (); ++c) {
      if (c->declaration () == &*c && c->is_of_type (typeid (X))) {
        //  assert: duplicate declaration objects for that class
        tl_assert (cd == 0);
        cd = &*c;
      }
    }
    //  assert, if no GSI binding available for that class
    tl_assert (cd != 0); 
  }
  return cd;
}

}

#endif
