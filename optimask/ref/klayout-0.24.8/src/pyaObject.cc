
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

#include "pyaObject.h"
#include "pyaMarshal.h"
#include "pyaUtils.h"
#include "pya.h"

#include "tlLog.h"

namespace pya
{

// --------------------------------------------------------------------------
//  Implementation of ClientData

ClientData::ClientData ()
  : m_self (NULL), m_lock_count (0)
{
  //  .. nothing yet ..
}

/**
 *  @brief Destructor
 */
ClientData::~ClientData () 
{
  if (m_self != NULL) {

    PYAObjectBase *p = (PYAObjectBase *) m_self;

    //  doing the detach before the object is released will avoid recursive destructor calls
    p->detach ();

    //  this is a safety measure - if the object is destroyed when the Python interpreter is down, we must not
    //  decrement the reference count
    if (PythonInterpreter::instance ()) {
      while (m_lock_count-- > 0) {
        Py_DECREF (m_self);
      }
    }

  }

  m_self = NULL;
  m_lock_count = 0;
}

/**
 *  @brief Resets the Python reference to nil
 */
void ClientData::reset_ref ()
{
  //  this is a safety measure - if the object is reference is reset the Python interpreter is down, we must not
  //  decrement the reference count
  if (PythonInterpreter::instance ()) {
    while (m_lock_count-- > 0) {
      Py_DECREF (m_self);
    }
  }

  m_self = NULL;
  m_lock_count = 0;
}

/**
 *  @brief Sets the Python reference
 *
 *  @arg s The Python reference
 *  @arg lock true, if the Python object should be locked, i.e. a reference should be kept
 *
 *  Using lock = true is equivalent to call keep after set_ref
 */
void ClientData::set_ref (PyObject *s, bool lock)
{
  tl_assert (m_self == NULL);
  tl_assert (s != NULL);

  m_self = s;

  //  Consider the case of "lock before assign"
  while (m_lock_count-- > 0) {
    Py_INCREF (m_self);
  }
  m_lock_count = 0;

  //  this avoids that our Python reference is deleted by the GC
  if (lock) {
    keep ();
  }
}

/**
 *  @brief Returns true, if the object has a C++ reference 
 */
bool ClientData::locked () const
{
  return m_lock_count > 0;
}

/**
 *  @brief Gets the Python reference
 */
PyObject *ClientData::get_ref () const
{
  return m_self;
}

/**
 *  @brief Release the Python reference
 *  This method is called when any C++ object wishes to release a reference to 
 *  the Python object.
 */
void ClientData::release () 
{
  Py_XDECREF (m_self);
  --m_lock_count;
}

/**
 *  @brief Keep a Python reference - increment the reference count
 *  This method is called when any C++ object wishes to keep a reference to 
 *  the Python object.
 */
void ClientData::keep () 
{
  ++m_lock_count;
  Py_XINCREF (m_self);
}

// --------------------------------------------------------------------------
//  Implementation of CallbackFunction

CallbackFunction::CallbackFunction (PyObject *pym, const gsi::MethodBase *m)
  : mp_method (m)
{
  //  We have a problem here with cyclic references. Bound instances methods can 
  //  create reference cycles if their target objects somehow points back to us
  //  (or worse, to some parent of us, i.e. inside a QWidget hierarchy).
  //  A solution is to take a bound instance method apart and store a weak
  //  reference to self plus a real reference to the function.

  if (pym && PyMethod_Check (pym) && PyMethod_Self (pym) != NULL) {

    m_weak_self = PythonRef (PyWeakref_NewRef (PyMethod_Self (pym), NULL));
    m_callable = PythonRef (PyMethod_Function (pym), false /* borrowed ref */);
#if PY_MAJOR_VERSION < 3
    m_class = PythonRef (PyMethod_Class (pym), false /* borrowed ref */);
#endif

    //  don't need the instance method any longer -> we took it apart already.
    Py_DECREF (pym);

  } else {
    m_callable = PythonRef (pym);
  }
}

const gsi::MethodBase *CallbackFunction::method () const
{
  return mp_method;
}

PythonRef CallbackFunction::callable () const
{
  if (m_callable && m_weak_self) {

    PyObject *self = PyWeakref_GetObject (m_weak_self.get ());
    if (self == Py_None) {
      //  object expired - no callback possible
      return PythonRef ();
    }

#if PY_MAJOR_VERSION < 3
    return PythonRef (PyMethod_New (m_callable.get (), self, m_class.get ()));
#else
    return PythonRef (PyMethod_New (m_callable.get (), self));
#endif

  } else {
    return m_callable;
  }
}

// --------------------------------------------------------------------------
//  Implementation of Callee

Callee::Callee (PYAObjectBase *obj)
  : mp_obj (obj)
{
  //  .. nothing yet ..
}

Callee::~Callee ()
{
  //  .. nothing yet ..
}

int 
Callee::add_callback (const CallbackFunction &vf)
{
  m_cbfuncs.push_back (vf);
  return int (m_cbfuncs.size () - 1);
}

void 
Callee::clear_callbacks ()
{
  m_cbfuncs.clear ();
}

void 
Callee::register_event_proc (const gsi::MethodBase *meth, PyObject *python_meth)
{
  for (std::vector<CallbackFunction>::iterator cb = m_cbfuncs.begin (); cb != m_cbfuncs.end (); ++cb) {
    if (cb->method () == meth) {
      *cb = CallbackFunction (python_meth, meth);
      return;
    }
  }
  Py_DECREF (python_meth);
}

void 
Callee::call (int id, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const gsi::MethodBase *meth = m_cbfuncs [id].method ();

  try {

    PythonRef callable (m_cbfuncs [id].callable ());
    bool is_event = (meth->is_event ());

    if (is_event && ! callable) {

      //  event not bound: create a default return value if required.
      tl::Heap heap;
      push_arg (meth->ret_type (), ret, NULL, heap);

      //  a Python callback must not leave temporary objects
      tl_assert (heap.empty ());

      return;

    }

    if (callable) {

      PYTHON_BEGIN_EXEC

        size_t arg4self = (is_event ? 0 : 1);

        //  One argument for "self"
        PythonRef argv (PyTuple_New (arg4self + std::distance (meth->begin_arguments (), meth->end_arguments ())));

        //  Put self into first argument
        if (! is_event) {
          PyTuple_SetItem (argv.get (), 0, mp_obj);
          Py_INCREF (mp_obj);
        }

        //  TODO: callbacks with default arguments?
        for (gsi::MethodBase::argument_iterator a = meth->begin_arguments (); args && a != meth->end_arguments (); ++a) {
          PyTuple_SetItem (argv.get (), arg4self + (a - meth->begin_arguments ()), pop_arg (*a, args));
        }

        PythonRef result (PyObject_CallObject (callable.get (), argv.get ()));
        if (! result) {
          check_error ();
        }

        tl::Heap heap;
        push_arg (meth->ret_type (), ret, result.get (), heap);

        
        //  a Python callback must not leave temporary objects
        tl_assert (heap.empty ());

      PYTHON_END_EXEC

    }

  } catch (PythonError &err) {
    PythonError err_with_context (err);
    err_with_context.set_context (mp_obj->cls_decl ()->name () + "." + meth->names ());
    throw err_with_context;
  } catch (gsi::ExitException &) {
    throw;
  } catch (tl::Exception &ex) {
    throw tl::Exception (tl::to_string (QObject::tr ("Error calling method")) + " '" + mp_obj->cls_decl ()->name () + "." + meth->names () + "': " + ex.msg ());
  } catch (...) {
    throw;
  }
}

// --------------------------------------------------------------------------
//  Implementation of PYAObjectBase

PYAObjectBase::PYAObjectBase(const gsi::ClassBase *_cls_decl)
  : m_callee (this), m_cls_decl (_cls_decl), m_obj (0), m_owned (false), m_const_ref (false), m_destroyed (false), m_can_destroy (false)
{
}

PYAObjectBase::~PYAObjectBase ()
{
  try {
    set (0, false, false, false);
  } catch (std::exception &ex) {
    tl::warn << "Caught exception in object destructor: " << ex.what ();
  } catch (tl::Exception &ex) {
    tl::warn << "Caught exception in object destructor: " << ex.msg ();
  } catch (...) {
    tl::warn << "Caught unspecified exception in object destructor";
  }
  m_destroyed = true;
}

void 
PYAObjectBase::release ()
{
  //  See the notes in keep
  const gsi::ClassBase *cls = cls_decl ();
  if (cls) {
    void *o = obj ();
    if (o) {
      ClientData *cd = (ClientData *) cls->client_data (o, client_index);
      if (cd) {
        cd->release ();
      } else {
        m_owned = true;
      }
    }
  }
}

void 
PYAObjectBase::keep ()
{
  const gsi::ClassBase *cls = cls_decl ();
  if (cls) {
    void *o = obj ();
    if (o) {
      ClientData *cd = (ClientData *) cls->client_data (o, client_index);
      if (cd) {
        //  The object is gsi-enabled: it has a Python object attached. In that case we
        //  use "keep" on that object to keep the Python object alive and hence preserve 
        //  signal handlers, virtual overrides and so forth.
        cd->keep ();
      } else {
        //  Fallback: the object is not gsi-enabled: we use the ownership flag in this
        //  case to keep it alive.
        m_owned = false;
      }
    }
  }
}

void 
PYAObjectBase::detach ()
{
  if (m_obj) {

    detach_callbacks ();

    m_obj = 0;
    m_destroyed = true;
    m_const_ref = false;
    m_owned = false;
    m_can_destroy = false;

  }
}

void 
PYAObjectBase::set (void *obj, bool owned, bool const_ref, bool can_destroy) 
{
  bool prev_owned = m_owned;
  const gsi::ClassBase *cls = cls_decl ();

  m_owned = owned;
  m_can_destroy = can_destroy;

  m_const_ref = const_ref;
  if (obj != m_obj) {

    //  cleanup
    if (m_obj && cls) {

      ClientData *cd = (ClientData *) cls->client_data (m_obj, client_index);

      //  Destroy the object if we are owner. We don't destroy the object if it was locked
      //  (either because we are not owner or from C++ side using keep())
      if (prev_owned && (! cd || ! cd->locked ())) {

        void *o = m_obj;
        cls->detach (o);
        m_obj = 0;
        cls->destroy (o);

      } else {

        //  Reset the reference
        if (cd) {
          cd->reset_ref ();
        }

        //  we'll get a new object - we have to detach event handlers.
        detach_callbacks ();

        cls->detach (m_obj);

      }

    }

    m_obj = obj;

    if (m_obj && cls) {

      cls->attach (m_obj);

      //  manage attachement to the C++ object if required
      //  Hold a reference unless we own the object (means: if we control the lifetime ourselves)
      if (cls->client_data (m_obj, client_index) != 0) {
        ((ClientData *) cls->client_data (m_obj, client_index))->set_ref (this, !owned);
      }

      //  initialize the callbacks according to the methods which need some
      initialize_callbacks ();

    }

  }

  //  now we have a valid object (or None) - we can reset "destroyed" state. Note: this has to be done 
  //  here because before detach might be called on *this which resets m_destroyed.
  m_destroyed = false;
}

//  TODO: a static (singleton) instance is not thread-safe
PYAObjectBase::callbacks_cache PYAObjectBase::s_callbacks_cache;

void
PYAObjectBase::initialize_callbacks ()
{
//  1 to enable caching, 0 to disable it.
//  TODO: caching appears to create some leaks ...
#if 1

  PythonRef type_ref ((PyObject *) Py_TYPE (this), false /*borrowed*/);

  //  Locate the callback-enabled methods set by Python tpye object (pointer)
  //  NOTE: I'm not quite sure whether the type object pointer is a good key
  //  for the cache. It may change since class objects may expire too if
  //  classes are put on the heap. Hence we have to keep a reference which is
  //  a pity, but hard to avoid.
  callbacks_cache::iterator cb = s_callbacks_cache.find (type_ref);
  if (cb == s_callbacks_cache.end ()) {

    cb = s_callbacks_cache.insert (std::make_pair (type_ref, callback_methods_type ())).first;
    
    const gsi::ClassBase *cls = cls_decl ();

    //  TODO: cache this .. this is taking too much time if done on every instance
    //  we got a new object - hence we have to attach event handlers.
    //  We don't need to install virtual function callbacks because in that case, no overload is possible
    //  (the object has been created on C++ side).
    while (cls) {

      for (gsi::ClassBase::method_iterator m = cls->begin_methods (); m != cls->end_methods (); ++m) {

        if ((*m)->is_event ()) {

          //  An event is given an anonymous callback for later attachment to an external method
          cb->second.push_back (*m);

        } else if ((*m)->is_callback () && m_owned) {

          //  NOTE: only Python-implemented classes can reimplement methods. Since we
          //  take the attribute from the class object, only Python instances can overwrite 
          //  the methods and owned indicates that. owned == true indicates that.

          //  NOTE: a callback may not have aliases nor overloads
          const char *nstr = (*m)->primary_name ().c_str ();

          //  NOTE: we just take attributes from the class object. That implies that it's not
          //  possible to reimplement a method through instance attributes (rare case, I hope).
          //  In addition, if we'd use instance attributes we create circular references 
          //  (self/callback to method, method to self).
          //  TOOD: That may happen too often, i.e. if the Python class does not reimplement the virtual
          //  method, but the C++ class defines a method hook that the reimplementation can call. 
          //  We don't want to produce a lot of overhead for the Qt classes here.
          PythonRef py_attr = PyObject_GetAttrString ((PyObject *) Py_TYPE (this), nstr);
          if (! py_attr) {

            //  because PyObject_GetAttrString left an error
            PyErr_Clear ();

          } else {

            //  Only if a Python-level class defines that method we can link the virtual method call to the 
            //  Python method. We should not create callbacks which we refer to C class implementations because that
            //  may create issues with callbacks during destruction (i.e. QWidget-destroyed signal)
            if (! PyCFunction_Check (py_attr.get ())) {
              cb->second.push_back (*m);
            }

          }

        }

      }

      //  consider base classes as well.
      cls = cls->base ();

    }

  }

  for (callback_methods_type::const_iterator m = cb->second.begin (); m != cb->second.end (); ++m) {

    PythonRef py_attr;
    if (!(*m)->is_event ()) {
      const char *nstr = (*m)->primary_name ().c_str ();
      py_attr = PyObject_GetAttrString ((PyObject *) Py_TYPE (this), nstr);
    }

    int id = m_callee.add_callback (CallbackFunction (py_attr.release (), *m));
    (*m)->set_callback (m_obj, gsi::Callback (id, &m_callee, (*m)->argsize (), (*m)->retsize ()));

  }

#else

  const gsi::ClassBase *cls = cls_decl ();

  //  TODO: cache this .. this is taking too much time if done on every instance
  //  we got a new object - hence we have to attach event handlers.
  //  We don't need to install virtual function callbacks because in that case, no overload is possible
  //  (the object has been created on C++ side).
  while (cls) {

    for (gsi::ClassBase::method_iterator m = cls->begin_methods (); m != cls->end_methods (); ++m) {

      if ((*m)->is_event ()) {

        //  An event is given an anonymous callback for later attachment to an external method
        int id = m_callee.add_callback (CallbackFunction (NULL, *m));
        (*m)->set_callback (m_obj, gsi::Callback (id, &m_callee, (*m)->argsize (), (*m)->retsize ()));

      } else if ((*m)->is_callback () && m_owned) {

        //  NOTE: only Python-implemented classes can reimplement methods. Since we
        //  take the attribute from the class object, only Python instances can overwrite 
        //  the methods and owned indicates that. owned == true indicates that.

        //  NOTE: a callback may not have aliases nor overloads
        const char *nstr = (*m)->primary_name ().c_str ();

        //  NOTE: we just take attributes from the class object. That implies that it's not
        //  possible to reimplement a method through instance attributes (rare case, I hope).
        //  In addition, if we'd use instance attributes we create circular references 
        //  (self/callback to method, method to self).
        //  TOOD: That may happen too often, i.e. if the Python class does not reimplement the virtual
        //  method, but the C++ class defines a method hook that the reimplementation can call. 
        //  We don't want to produce a lot of overhead for the Qt classes here.
        PythonRef py_attr = PyObject_GetAttrString ((PyObject *) Py_TYPE (this), nstr);
        if (! py_attr) {

          //  because PyObject_GetAttrString left an error
          PyErr_Clear ();

        } else {

          //  Only if a Python-level class defines that method we can link the virtual method call to the 
          //  Python method. We should not create callbacks which we refer to C class implementations because that
          //  may create issues with callbacks during destruction (i.e. QWidget-destroyed signal)
          if (! PyCFunction_Check (py_attr.get ())) {

            PyObject *py_attr = PyObject_GetAttrString ((PyObject *) Py_TYPE (this), nstr);
            tl_assert (py_attr != NULL);
            int id = m_callee.add_callback (CallbackFunction (py_attr, *m));
            (*m)->set_callback (m_obj, gsi::Callback (id, &m_callee, (*m)->argsize (), (*m)->retsize ()));

          }

        }

      }

    }

    //  consider base classes as well.
    cls = cls->base ();

  }

#endif
}

void 
PYAObjectBase::clear_callbacks_cache ()
{
  s_callbacks_cache.clear ();
}

void
PYAObjectBase::detach_callbacks ()
{
  PythonRef type_ref ((PyObject *) Py_TYPE (this), false /*borrowed*/);

  callbacks_cache::iterator cb = s_callbacks_cache.find (type_ref);
  if (cb != s_callbacks_cache.end ()) {
    for (callback_methods_type::const_iterator m = cb->second.begin (); m != cb->second.end (); ++m) {
      (*m)->set_callback (m_obj, gsi::Callback ());
    }
  }

  m_callee.clear_callbacks ();
}

void 
PYAObjectBase::destroy ()
{
  if (! m_cls_decl) {
    m_obj = 0;
    return;
  }

  if (!(m_owned || m_can_destroy) && m_obj) {
    throw tl::Exception (tl::to_string (QObject::tr ("Object cannot be destroyed explicitly - it is either a const reference or a direct object")));
  }

  //  first create the object if it was not created yet and check if it has not been 
  //  destroyed already (the former is to ensure that the object is created at least)
  if (! m_obj) {
    if (m_destroyed) {
      throw tl::Exception (tl::to_string (QObject::tr ("Object has been destroyed already")));
    } else {
      m_obj = m_cls_decl->create ();
      if (m_obj) {
        m_cls_decl->attach (m_obj);
      }
      m_owned = true;
    }
  }

  void *o = 0;
  if (m_owned || m_can_destroy) {
    o = m_obj;
  }
  if (m_obj) {
    m_cls_decl->detach (m_obj);
  }

  detach ();

  if (o) {
    m_cls_decl->destroy (o);
  }
}

void *
PYAObjectBase::obj () 
{
  if (! m_obj) {
    if (m_destroyed) {
      throw tl::Exception (tl::to_string (QObject::tr ("Object has been destroyed already")));
    } else {
      //  delayed creation of a detached C++ object ..
      set(cls_decl ()->create (), true, m_const_ref, m_can_destroy);
    }
  }

  return m_obj;
}

}

#endif

