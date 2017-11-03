
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


#ifndef _HDR_gsiIterators
#define _HDR_gsiIterators

#include "tlException.h"
#include "tlTypeTraits.h"

//  For a comprehensive documentation see gsi.h

namespace gsi
{

template <class X, class Y> struct address_of;

template<class X> 
struct address_of<X, X> 
{
  address_of<X, X> () : b () { }
  const void *operator() (const X &x) const { b = x; return &b; }
  mutable X b;
};

template<class X> 
struct address_of<X &, X> 
{
  const void *operator() (X &x) const { return &x; }
};

template<class X> 
struct address_of<const X &, X> 
{
  const void *operator() (const X &x) const { return &x; }
};

template<class X> 
struct address_of<X *, X *> 
{
  const void *operator() (X *x) const { return x; }
};

template<class X> 
struct address_of<const X *, const X *> 
{
  const void *operator() (const X *x) const { return x; }
};

template<class X> 
struct address_of<X * const &, X *> 
{
  const void *operator() (X *x) const { return x; }
};

template<class X> 
struct address_of<const X * const &, const X *> 
{
  const void *operator() (const X *x) const { return x; }
};

template<class X> 
struct address_of<X * &, X *> 
{
  const void *operator() (X *x) const { return x; }
};

template<class X> 
struct address_of<const X * &, const X *> 
{
  const void *operator() (const X *x) const { return x; }
};

/**
 *  @brief The basic iterator abstraction 
 */
class IterAdaptorAbstractBase
{
public:
  virtual ~IterAdaptorAbstractBase () { }

  virtual const void *v_get () const = 0;
  virtual void *v_get_copy () const = 0;
  virtual bool at_end () const = 0;
  virtual void inc () = 0;
};

template <class V>
void *_create_copy (tl::false_tag, const V &v)
{
  tl_assert (false);
  return 0;
}

template <class V>
void *_create_copy (tl::true_tag, const V &v)
{
  return new V (v);
}

/**
 *  @brief The template providing a binding of a specific C++ iterator to the abstraction
 */
template <class R, class V>
class IterPtrAdaptor 
  : public IterAdaptorAbstractBase
{
public:
  typedef R return_type;
  typedef V value_type;
  
  IterPtrAdaptor (V *b, V *e) 
    : m_b (b), m_e (e)
  {
    //  .. nothing yet ..
  }

  virtual const void *v_get () const 
  {
    return m_ao (*m_b);
  }

  virtual void *v_get_copy () const 
  {
    return _create_copy (typename tl::type_traits<V>::has_copy_constructor (), *m_b);
  }

  virtual bool at_end () const 
  {
    return m_b == m_e;
  }

  virtual void inc () 
  {
    ++m_b;
  }

private:
  V *m_b, *m_e;
  address_of<R, V> m_ao;
};

/**
 *  @brief The template providing a binding of a specific C++ iterator to the abstraction
 */
template <class R, class V>
class ConstIterPtrAdaptor 
  : public IterAdaptorAbstractBase
{
public:
  typedef R return_type;
  typedef V value_type;
  
  ConstIterPtrAdaptor (const V *b, const V *e) 
    : m_b (b), m_e (e)
  {
    //  .. nothing yet ..
  }

  virtual const void *v_get () const 
  {
    return m_ao (*m_b);
  }

  virtual void *v_get_copy () const 
  {
    return _create_copy (typename tl::type_traits<V>::has_copy_constructor (), *m_b);
  }

  virtual bool at_end () const 
  {
    return m_b == m_e;
  }

  virtual void inc () 
  {
    ++m_b;
  }

private:
  const V *m_b, *m_e;
  address_of<R, V> m_ao;
};

/**
 *  @brief The template providing a binding of a specific C++ iterator to the abstraction
 */
template <class I, class R, class V>
class IterAdaptor 
  : public IterAdaptorAbstractBase
{
public:
  typedef R return_type;
  typedef V value_type;
  
  IterAdaptor (const I &b, const I &e) 
    : m_b (b), m_e (e)
  {
    //  .. nothing yet ..
  }

  virtual const void *v_get () const 
  {
    return m_ao (*m_b);
  }

  virtual void *v_get_copy () const 
  {
    return _create_copy (typename tl::type_traits<V>::has_copy_constructor (), *m_b);
  }

  virtual bool at_end () const 
  {
    return m_b == m_e;
  }

  virtual void inc () 
  {
    ++m_b;
  }

private:
  I m_b, m_e;
  address_of<R, V> m_ao;
};

/**
 *  @brief The template providing a binding of a "free iterator" (one that provides it's own at_end method)
 */
template <class I, class R, class V>
class FreeIterAdaptor 
  : public IterAdaptorAbstractBase
{
public:
  typedef R return_type;
  typedef V value_type;
  
  FreeIterAdaptor (const I &i) 
    : m_i (i)
  {
    //  .. nothing yet ..
  }

  virtual const void *v_get () const 
  {
    return m_ao (*m_i);
  }

  virtual void *v_get_copy () const 
  {
    return _create_copy (typename tl::type_traits<V>::has_copy_constructor (), *m_i);
  }

  virtual bool at_end () const 
  {
    return m_i.at_end ();
  }

  virtual void inc () 
  {
    ++m_i;
  }

private:
  I m_i;
  address_of<R, V> m_ao;
};

}

#endif

