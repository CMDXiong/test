
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


#ifndef _HDR_pyaObject
#define _HDR_pyaObject

#if defined(HAVE_PYTHON)

#include "Python.h"

#include "gsiDecl.h"
#include "gsiDeclBasic.h"

#include "pyaRefs.h"

namespace pya
{

class PYAObjectBase;

/**
 *  @brief A storage object for a function to callback
 */
struct CallbackFunction
{
  CallbackFunction (PyObject *pym, const gsi::MethodBase *m);

  PythonRef callable () const;
  const gsi::MethodBase *method () const;

private:
  PythonRef m_callable;
  PythonRef m_weak_self;
  PythonRef m_class;
  const gsi::MethodBase *mp_method;
};

/**
 *  @brief The data structure representing the link from a C++ object to the Python object
 *  The client data object is the piece of information stored along with the C++ object.
 *  It stores information specific for the Python interpreter's native object and is 
 *  attached to the C++ object using the "ClientHandler" singleton provided by the interpreter.
 */
class ClientData
{
public:
  /**
   *  @brief Default constructor
   */
  ClientData ();

  /**
   *  @brief Destructor
   */
  ~ClientData ();

  /**
   *  @brief Resets the Python reference to nil
   */
  void reset_ref ();

  /**
   *  @brief Sets the Python reference
   *
   *  @arg s The Python reference
   *  @arg lock true, if the Python object should be locked, i.e. a reference should be kept
   *
   *  Using lock = true is equivalent to call keep after set_ref
   */
  void set_ref (PyObject *s, bool lock);

  /**
   *  @brief Returns true, if the object has a C++ reference 
   */
  bool locked () const;

  /**
   *  @brief Gets the Python reference
   */
  PyObject *get_ref () const;

  /**
   *  @brief Release the Python reference
   *  This method is called when any C++ object wishes to release a reference to 
   *  the Python object.
   */
  void release ();

  /**
   *  @brief Keep a Python reference - increment the reference count
   *  This method is called when any C++ object wishes to keep a reference to 
   *  the Python object.
   */
  void keep ();

private:
  PyObject *m_self;
  int m_lock_count;
};

/**
 *  @brief An adaptor class for the callback mechanism
 */
class Callee
  : public gsi::Callee
{
public:
  /**
   *  @brief Constructor for a Callee object pointing the to given Python object
   */
  Callee (PYAObjectBase *obj);
  
  /**
   *  @brief Destructor
   */
  ~Callee ();

  /**
   *  @brief Adds a callback (given by the CallbackFunction)
   *  This method returns a callback ID which can be used to register the callback
   *  at an GSI object.
   */
  int add_callback (const CallbackFunction &vf);

  /**
   *  @brief Clears all callbacks registered 
   */
  void clear_callbacks ();

  /**
   *  @brief Attaches an event with a Python callable
   *  This method will connect the event given by "meth" with the Python callable
   *  object given by "proc".
   *  Internally this will create a Callback object that establishes
   *  the link. The Callee object will act as a container for these callback objects.
   */
  void register_event_proc (const gsi::MethodBase *meth, PyObject *proc);

  /**
   *  @brief Implementation of the Callee interface
   */
  virtual void call (int id, gsi::SerialArgs &args, gsi::SerialArgs &ret);

private:
  PYAObjectBase *mp_obj;
  std::vector<CallbackFunction> m_cbfuncs; 
};

/**
 *  @brief The Python object representing a GSI object
 *
 *  Note: the PYAObjectBase must be directly derived from PyObject so that
 *  a PyObject pointer can be cast to a PYAObjectBase pointer.
 */
class PYAObjectBase
  : public PyObject
{
public:
  /**
   *  @brief Constructor - creates a new object for the given GSI class
   */
  PYAObjectBase (const gsi::ClassBase *_cls_decl);

  /**
   *  @brief Destructor
   */
  ~PYAObjectBase ();

  /**
   *  @brief Indicates that a C++ object is present
   */
  bool is_attached () const
  {
    return m_obj != 0;
  }

  /**
   *  @brief Explicitly destroy the C++ object
   *  If the C++ object is owned by the Python object, this method will delete the C++
   *  object and the \destroyed attribute will become true.
   *  The reference is no longer valid.
   */
  void destroy ();

  /**
   *  @brief Links the Python object with a C++ object
   *  The "owned" attribute indicates that the reference will be owned by the Python object.
   *  That means that the C++ object is being destroyed when the Python object expires. 
   *  If "const_ref" is true, the Python object is said to be a const object which means
   *  only const methods can be called on it. That is a somewhat weak emulation for the 
   *  constness concept in C++ since there is only one Python object representing multiple
   *  references. If one of these references goes to non-const, the Python object will accept
   *  non-const method calls.
   *  "can_destroy" indicates that the C++ object can be destroyed (has a destructor).
   */
  void set (void *obj, bool owned, bool const_ref, bool can_destroy);

  /**
   *  @brief Unlinks the C++ object from the Python object
   *  This method can be called to make the Python object cut the link to the C++ object.
   *  After that operation, the \destroyed attribute will be come true, even though the 
   *  C++ object may not actually be destroyed.
   *  The reference will become invalid.
   */
  void detach ();

  /**
   *  @brief Gets the GSI class object
   */
  const gsi::ClassBase *cls_decl () const 
  {
    return m_cls_decl;
  }

  /**
   *  @brief Gets a flag indicating that the corresponding C++ object expired
   *  If the Python object acts as a weak reference to a foreign object (owned = false),
   *  the foreign object may expire before the Python object is deleted.
   *  In that case, destroyed becomes true.
   */
  bool destroyed () const 
  {
    return m_destroyed;
  }

  /**
   *  @brief Returns a flag indicating that this Python object is a const reference to a C++ object
   *  See \set for a description of that flag
   */
  bool const_ref () const 
  {
    return m_const_ref;
  }

  /**
   *  @brief Sets a flag indicating that this Python object is a const reference to the C++ object
   *  See \set for a description of that flag.
   */
  void set_const_ref (bool c) 
  {
    m_const_ref = c;
  }

  /**
   *  @brief Returns the C++ object reference
   */
  void *obj ();

  /**
   *  @brief Puts this object under C++ management (release from script management)
   */
  void keep ();

  /**
   *  @brief Puts this object under script management again
   */
  void release ();

  /**
   *  @brief Returns true, if the C++ object is owned by the Python object
   *  See \set for details about this flag
   */
  bool owned () const 
  {
    return m_owned;
  }

  /** 
   *  @brief The callee interface
   */
  Callee &callee () 
  {
    return m_callee;
  }

  /** 
   *  @brief The callee interface (const pointer)
   */
  const Callee &callee () const 
  {
    return m_callee;
  }

  /**
   *  @brief Clears the callbacks cache
   */
  static void clear_callbacks_cache ();

private:
  typedef std::vector<const gsi::MethodBase *> callback_methods_type;
  typedef std::map<PythonRef, callback_methods_type> callbacks_cache;
  static callbacks_cache s_callbacks_cache;

  void detach_callbacks ();
  void initialize_callbacks ();

  Callee m_callee;
  const gsi::ClassBase *m_cls_decl;
  void *m_obj;
  bool m_owned : 1;
  bool m_const_ref : 1;
  bool m_destroyed : 1;
  bool m_can_destroy : 1;
};

}

#endif

#endif

