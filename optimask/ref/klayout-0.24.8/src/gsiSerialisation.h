
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


#ifndef _HDR_gsiSerialisation
#define _HDR_gsiSerialisation

#include "gsiTypes.h"

#include <list>

//  For a comprehensive documentation see gsi.h

namespace gsi
{

struct ArglistUnderflowException
  : public tl::Exception
{
  ArglistUnderflowException ()
    : tl::Exception (tl::to_string (QObject::tr ("Too few arguments or no return value supplied")))
  { }
};

struct NilPointerToReference
  : public tl::Exception
{
  NilPointerToReference ()
    : tl::Exception (tl::to_string (QObject::tr ("nil object passed to a reference")))
  { }
};

/**
 *  @brief This class provides the basic argument serialization mechanism for the C++/scripting interface
 */
class SerialArgs 
{
public:
  /**
   *  @brief Default constructor
   *  Creates an empty list.
   */
  SerialArgs ()
    : mp_buffer (0)
  {
    mp_write = mp_read = mp_buffer;
  }

  /**
   *  @brief Constructor 
   *  Creates a buffer with space for len bytes.
   */
  SerialArgs (size_t len)
    : mp_buffer (0)
  {
    //  use the internal buffer for small argument lists (which are quite common)
    if (len > sizeof (m_buffer)) {
      mp_buffer = new char [len];
    } else if (len > 0) {
      mp_buffer = m_buffer;
    }
    mp_write = mp_read = mp_buffer;
  }

  /**
   *  @brief Destructor
   */
  ~SerialArgs ()
  {
    if (mp_buffer && mp_buffer != m_buffer) {
      delete [] mp_buffer;
    }
    mp_buffer = 0;
  }

  /**
   *  @brief gets a pointer to the buffer
   */
  char *cptr ()
  {
    return mp_buffer;
  }

  /**
   *  @brief gets a const pointer to the buffer
   */
  const char *cptr () const
  {
    return mp_buffer;
  }

  /**
   *  @brief Gets a pointer to the current write position
   */
  char *wptr () 
  {
    return mp_write;
  }
  
  /**
   *  @brief Gets a const pointer to the current write position
   */
  const char *wptr () const
  {
    return mp_write;
  }
  
  /**
   *  @brief Gets a pointer to the current read position
   */
  char *rptr () 
  {
    return mp_read;
  }
  
  /**
   *  @brief Gets a const pointer to the current read position
   */
  const char *rptr () const
  {
    return mp_read;
  }
  
  /**
   *  @brief Writes a value into the buffer
   */
  template <class X>
  inline void write (X x)
  {
    this->write_impl (typename type_traits<X>::tag (), x);
  }

  /**
   *  @brief Reads a value from the buffer
   */
  template <class X>
  inline X read ()
  {
    return this->read_impl<X> (typename type_traits<X>::tag ());
  }

  /**
   *  @brief Takes an object from the buffer
   *  This will create new object as a copy of the one in the buffer and 
   *  delete the object in the buffer.
   *  This is equivalent to reading a direct (pass-by-value) non-POD object
   *  but a little more efficient since it returns a new'd pointer, not a copy.
   */
  template <class X> 
  inline X *take_object ()
  {
    check_data ();
    X *x = new X (*((X *)mp_read));
    ((X *)mp_read)->~X ();
    mp_read += item_size<X> ();
    return x;
  }

  /**
   *  @brief returns true, if there is still data available for read
   */
  operator bool () const
  {
    return mp_read && mp_read < mp_write;
  }

private:
  char *mp_buffer;
  char *mp_read, *mp_write;
  char m_buffer[200];

  inline void check_data () const
  {
    if (! *this) {
      throw ArglistUnderflowException ();
    }
  }

  template <class X> 
  void write_impl (const direct_tag &, const X &x)
  {
    X *r = (X *)mp_write;
    new (r) X(x);
    mp_write += item_size<X> ();
  }

  template <class X> 
  void write_impl (const ref_tag &, X &x) 
  {
    *((X **)mp_write) = &x;
    mp_write += item_size<X *> ();
  }

  template <class X> 
  void write_impl (const ptr_tag &, X *x)
  {
    *((X **)mp_write) = x;
    mp_write += item_size<X *> ();
  }

  template <class X> 
  void write_impl (const pod_cref_tag &, const X &x)
  {
    X *r = (X *)mp_write;
    new (r) X(x);
    mp_write += item_size<X> ();
  }

  template <class X> 
  void write_impl (const npod_cref_tag &, const X &x)
  {
    *((X const **)mp_write) = &x;
    mp_write += item_size<const X *> ();
  }

  template <class X> 
  void write_impl (const x_cref_tag &, const X &x)
  {
    *((X const **)mp_write) = &x;
    mp_write += item_size<const X *> ();
  }

  template <class X> 
  void write_impl (const pod_cptr_tag &, const X *x)
  {
    *(bool *)mp_write = (x != 0);
    mp_write += item_size<bool> ();
    if (x) {
      *(X *)mp_write = *x;
    }
    mp_write += item_size<X> ();
  }

  //  see notes on the serialization for this type:
  template <class X> 
  void write_impl (const npod_cptr_tag &, const X *x)
  {
    *((X const **)mp_write) = x;
    mp_write += item_size<const X *> ();
  }

  template <class X> 
  void write_impl (const x_cptr_tag &, const X *x)
  {
    *((X const **)mp_write) = x;
    mp_write += item_size<const X *> ();
  }

  void write_impl (const vptr_tag &, void *x)
  {
    *((void **)mp_write) = x;
    mp_write += item_size<void *> ();
  }

  void write_impl (const vptr_tag &, const void *x)
  {
    *((const void **)mp_write) = x;
    mp_write += item_size<const void *> ();
  }

  void write_impl (const ccptr_tag &, const char *x)
  {
    *((const char **)mp_write) = x;
    mp_write += item_size<const char *> ();
  }

  void write_impl (const ccptr_tag &, const signed char *x)
  {
    *((const signed char **)mp_write) = x;
    mp_write += item_size<const signed char *> ();
  }

  void write_impl (const ccptr_tag &, const unsigned char *x)
  {
    *((const unsigned char **)mp_write) = x;
    mp_write += item_size<const unsigned char *> ();
  }

  template <class X>
  X read_impl (const pod_direct_tag &)
  {
    check_data ();
    X r = *((X *)mp_read);
    mp_read += item_size<X> ();
    return r;
  }

  template <class X> 
  X read_impl (const npod_direct_tag &)
  {
    check_data ();
    X x = *(X *)mp_read;
    ((X *)mp_read)->~X ();
    mp_read += item_size<X> ();
    return x;
  }

  template <class X> 
  X read_impl (const x_tag &)
  {
    check_data ();
    X x = *(X *)mp_read;
    ((X *)mp_read)->~X ();
    mp_read += item_size<X> ();
    return x;
  }

  template <class X> 
  X read_impl (const vptr_tag &)
  {
    void *r = *((void **)mp_read);
    mp_read += item_size<void *> ();
    // Hint: X can be const unsigned char *, const signed char * as well.
    return (X)r;
  }

  template <class X> 
  X read_impl (const ccptr_tag &)
  {
    const char *r = *((const char **)mp_read);
    mp_read += item_size<const char *> ();
    // Hint: X can be const unsigned char *, const signed char * as well.
    return (X)r;
  }

  template <class X> 
  X read_impl (const ref_tag &)
  {
    typedef typename type_traits<X>::value_type value_type;
    check_data ();
    value_type *r = *((value_type **)mp_read);
    mp_read += item_size<value_type *> ();
    if (! r) {
      throw NilPointerToReference ();
    }
    return *r;
  }

  template <class X>
  X read_impl (const pod_cref_tag &)
  {
    //  X is actually an (const X &)
    typedef typename type_traits<X>::value_type value_type;
    check_data ();
    const value_type *r = ((const value_type *)mp_read);
    mp_read += item_size<value_type> ();
    return *r;
  }

  template <class X> 
  X read_impl (const npod_cref_tag &)
  {
    //  X is actually an (const X &)
    typedef typename type_traits<X>::value_type value_type;
    check_data ();
    const value_type *r = *((const value_type **)mp_read);
    mp_read += item_size<const value_type *> ();
    if (! r) {
      throw NilPointerToReference ();
    }
    return *r;
  }

  template <class X> 
  X read_impl (const x_cref_tag &)
  {
    //  X is actually an (const X &)
    typedef typename type_traits<X>::value_type value_type;
    check_data ();
    const value_type *r = *((const value_type **)mp_read);
    mp_read += item_size<const value_type *> ();
    if (! r) {
      throw NilPointerToReference ();
    }
    return *r;
  }

  template <class X> 
  X read_impl (const ptr_tag &)
  {
    //  X is actually an (X *)
    check_data ();
    X r = *((X *)mp_read);
    mp_read += item_size<X> ();
    return r;
  }

  template <class X>
  X read_impl (const pod_cptr_tag &)
  {
    //  X is actually an (const X *)
    check_data ();
    bool h = *(bool *)mp_read;
    mp_read += item_size<bool> ();
    X r = h ? (X)mp_read : (X)0;
    mp_read += item_size<X> ();
    return r;
  }

  //  see notes on the serialization for this type:
  template <class X> 
  X read_impl (const npod_cptr_tag &)
  {
    //  X is actually an (const X *)
    check_data ();
    X r = *((X *)mp_read);
    mp_read += item_size<X> ();
    return r;
  }

  template <class X> 
  X read_impl (const x_cptr_tag &)
  {
    //  X is actually an (const X *)
    check_data ();
    X r = *((X *)mp_read);
    mp_read += item_size<X> ();
    return r;
  }
};

}

#endif

