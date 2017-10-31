
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

#include "pyaHelpers.h"
#include "pyaUtils.h"
#include "pyaMarshal.h"
#include "pyaObject.h"
#include "pyaConvert.h"
#include "pya.h"

namespace pya
{

// -------------------------------------------------------------------
//  Helper objects: PYAChannelObject

PyTypeObject *PYAChannelObject::cls = 0;

/**
 *  @brief Implementation of the write method of the channel object
 */
static PyObject *
pya_channel_write (PyObject *self, PyObject *args)
{
  const char *msg = 0;
  if (! PyArg_ParseTuple (args, "s", &msg)) {
    return NULL;
  }

  PYAChannelObject *channel = (PYAChannelObject *) self;
  if (PythonInterpreter::instance ()->current_console ()) {
    PythonInterpreter::instance ()->current_console ()->write_str (msg, channel->channel);
  }

  Py_RETURN_NONE;
}

/**
 *  @brief Implementation of the flush method of the channel object
 */
static PyObject *
pya_channel_flush (PyObject * /*self*/, PyObject *args)
{
  if (! PyArg_ParseTuple (args, "")) {
    return NULL;
  }

  if (PythonInterpreter::instance ()->current_console ()) {
    PythonInterpreter::instance ()->current_console ()->flush ();
  }

  Py_RETURN_NONE;
}

/**
 *  @brief Initialization of the channel object
 */
static int
pya_channel_init (PyObject *self, PyObject *, PyObject *)
{
  PYAChannelObject *channel = (PYAChannelObject *) self;
  channel->channel = gsi::Console::OS_none;
  return 0;
}

void
PYAChannelObject::make_class (PyObject *module)
{
  static PyTypeObject channel_type = {
    PyVarObject_HEAD_INIT (&PyType_Type, 0)
    "pya._Channel",             // tp_name
    sizeof (PYAChannelObject)   // tp_size
  };

  static PyMethodDef channel_methods[] = {
      {"write", (PyCFunction) &pya_channel_write, METH_VARARGS, "internal stdout/stderr redirection object: write method" },
      {"flush", (PyCFunction) &pya_channel_flush, METH_VARARGS, "internal stdout/stderr redirection object: flush method" },
      {NULL,	NULL},
  };

  channel_type.tp_flags = Py_TPFLAGS_DEFAULT;
  channel_type.tp_methods = channel_methods;
  channel_type.tp_init = pya_channel_init;

  PyType_Ready (&channel_type);
  Py_INCREF (&channel_type);

  PyModule_AddObject (module, "_Channel", (PyObject *) &channel_type);

  cls = &channel_type;
}

PYAChannelObject *
PYAChannelObject::create (gsi::Console::output_stream chn)
{
  tl_assert (cls != 0);
  PYAChannelObject *channel = (PYAChannelObject *) cls->tp_alloc (cls, 0);
  if (channel == NULL) {
    check_error ();
  } else {
    channel->channel = chn;
  }
  return channel;
}

// -------------------------------------------------------------------
//  Helper objects: PYAStaticAttributeDescriptorObject

PyTypeObject *PYAStaticAttributeDescriptorObject::cls = 0;

/**
 *  @brief Implementation of the static attribute getter
 */
static PyObject *
pya_static_attribute_descriptor_get (PyObject *self, PyObject * /*obj*/, PyObject * /*type*/)
{
  PYAStaticAttributeDescriptorObject *attr = (PYAStaticAttributeDescriptorObject *) self;
  if (attr->getter) {
    return (*(attr->getter)) ((PyObject *) attr->type, NULL);
  } else {
    std::string msg;
    msg += tl::to_string (QObject::tr ("Attribute not readable"));
    msg += ": ";
    msg += attr->type->tp_name;
    msg += ".";
    msg += attr->name;
    PyErr_SetString (PyExc_AttributeError, msg.c_str ());
    return NULL;
  }
}

/**
 *  @brief Implementation of the static attribute setter
 */
static int
pya_static_attribute_descriptor_set (PyObject *self, PyObject * /*obj*/, PyObject *value)
{
  PYAStaticAttributeDescriptorObject *attr = (PYAStaticAttributeDescriptorObject *) self;
  if (attr->setter) {
    PythonRef args (PyTuple_Pack (1, value));
    PyObject *ret = (*(attr->setter)) ((PyObject *) attr->type, args.get ());
    if (ret) {
      Py_DECREF(ret);
      return 0;
    } else {
      return -1;
    }
  } else {
    std::string msg;
    msg += tl::to_string (QObject::tr ("Attribute cannot be changed"));
    msg += ": ";
    msg += attr->type->tp_name;
    msg += ".";
    msg += attr->name;
    PyErr_SetString (PyExc_AttributeError, msg.c_str ());
    return -1;
  }
}

/**
 *  @brief Initialization of the static attribute object
 */
static int
pya_static_attribute_descriptor_init (PyObject *self, PyObject *, PyObject *)
{
  PYAStaticAttributeDescriptorObject *attr = (PYAStaticAttributeDescriptorObject *) self;
  attr->getter = 0;
  attr->setter = 0;
  attr->name = 0;
  attr->type = 0;
  return 0;
}

void
PYAStaticAttributeDescriptorObject::make_class (PyObject *module)
{
  static PyTypeObject static_attribute_descriptor_type = {
    PyVarObject_HEAD_INIT (&PyType_Type, 0)
    "pya._StaticAttribute",                       // tp_name
    sizeof (PYAStaticAttributeDescriptorObject)   // tp_size
  };

  static_attribute_descriptor_type.tp_flags = Py_TPFLAGS_DEFAULT;
  static_attribute_descriptor_type.tp_init = pya_static_attribute_descriptor_init;
  static_attribute_descriptor_type.tp_descr_get = pya_static_attribute_descriptor_get;
  static_attribute_descriptor_type.tp_descr_set = pya_static_attribute_descriptor_set;
  static_attribute_descriptor_type.tp_setattro = PyObject_GenericSetAttr;
  static_attribute_descriptor_type.tp_getattro = PyObject_GenericGetAttr;

  PyType_Ready (&static_attribute_descriptor_type);
  Py_INCREF (&static_attribute_descriptor_type);

  PyModule_AddObject (module, "_StaticAttribute", (PyObject *) &static_attribute_descriptor_type);

  cls = &static_attribute_descriptor_type;
}

PYAStaticAttributeDescriptorObject *
PYAStaticAttributeDescriptorObject::create (const char *n)
{
  tl_assert (cls != 0);
  PYAStaticAttributeDescriptorObject *desc = (PYAStaticAttributeDescriptorObject *) cls->tp_alloc (cls, 0);
  if (desc == NULL) {
    check_error ();
  } else {
    desc->name = n;
  }
  return desc;
}

// -------------------------------------------------------------------
//  Helper objects: PYAAmbiguousMethodDispatcher

PyTypeObject *PYAAmbiguousMethodDispatcher::cls = 0;

static PyObject *
pya_ambiguous_method_dispatcher_get (PyObject *self, PyObject *obj, PyObject *type)
{
  PYAAmbiguousMethodDispatcher *attr = (PYAAmbiguousMethodDispatcher *) self;
  PyObject *descr;
  if (obj == NULL || obj == Py_None) {
    descr = attr->attr_class;
  } else {
    descr = attr->attr_inst;
  }

  //  taken from object.c, PyObject_GenericGetAttrWithDict
#if PY_MAJOR_VERSION < 3
  tl_assert (PyType_HasFeature (Py_TYPE (descr), Py_TPFLAGS_HAVE_CLASS)); 
#endif
  descrgetfunc f = Py_TYPE (descr)->tp_descr_get;
  if (f == NULL) {
    Py_INCREF (descr);
    return descr;
  } else {
    return (*f) (descr, obj, type);
  }
}

static int
pya_ambiguous_method_dispatcher_set (PyObject *self, PyObject * /*obj*/, PyObject *value)
{
  PyErr_SetNone (PyExc_AttributeError);
  return -1;
}

static void
pya_ambiguous_method_dispatcher_deallocate (PyObject *self)
{
  PYAAmbiguousMethodDispatcher *attr = (PYAAmbiguousMethodDispatcher *) self;
  Py_XDECREF (attr->attr_inst);
  Py_XDECREF (attr->attr_class);
  Py_TYPE (self)->tp_free ((PyObject *) self);
}

void
PYAAmbiguousMethodDispatcher::make_class (PyObject *module)
{
  static PyTypeObject static_ambiguous_method_dispatcher_type = {
    PyVarObject_HEAD_INIT (&PyType_Type, 0)
    "pya._AmbiguousMethodDispatcher",       // tp_name
    sizeof (PYAAmbiguousMethodDispatcher)   // tp_size
  };

  static_ambiguous_method_dispatcher_type.tp_flags = Py_TPFLAGS_DEFAULT;
  static_ambiguous_method_dispatcher_type.tp_dealloc = pya_ambiguous_method_dispatcher_deallocate;
  static_ambiguous_method_dispatcher_type.tp_descr_get = pya_ambiguous_method_dispatcher_get;
  static_ambiguous_method_dispatcher_type.tp_descr_set = pya_ambiguous_method_dispatcher_set;
  static_ambiguous_method_dispatcher_type.tp_setattro = PyObject_GenericSetAttr;
  static_ambiguous_method_dispatcher_type.tp_getattro = PyObject_GenericGetAttr;

  PyType_Ready (&static_ambiguous_method_dispatcher_type);
  Py_INCREF (&static_ambiguous_method_dispatcher_type);

  PyModule_AddObject (module, "_AmbiguousMethodDispatcher", (PyObject *) &static_ambiguous_method_dispatcher_type);

  cls = &static_ambiguous_method_dispatcher_type;
}

PYAAmbiguousMethodDispatcher *
PYAAmbiguousMethodDispatcher::create (PyObject *ai, PyObject *ac)
{
  tl_assert (cls != 0);
  PYAAmbiguousMethodDispatcher* desc = (PYAAmbiguousMethodDispatcher *) cls->tp_alloc (cls, 0);
  if (desc == NULL) {
    Py_XDECREF (ai);
    Py_XDECREF (ac);
    check_error ();
  } else {
    desc->attr_inst = ai;
    desc->attr_class = ac;
  }
  return desc;
}


// -------------------------------------------------------------------
//  Helper objects: PYAIteratorObject

PyTypeObject *PYAIteratorObject::cls = 0;

/**
 *  @brief Gets the iterator object (reflective)
 */
static PyObject *
pya_iterator_iter (PyObject *self)
{
  //  we have to return a new reference -> when using self, we have to increment out count
  Py_INCREF (self);
  return self;
}

/**
 *  @brief Increments the iterator
 */
static PyObject *
pya_iterator_next (PyObject *self)
{
  PYAIteratorObject *iter = (PYAIteratorObject *) self;

  if (! iter->iter || iter->cls_decl->obj_iterator_at_end (iter->iter)) {
    PyErr_SetNone (PyExc_StopIteration);
    return NULL;
  }

  if (iter->value_type->is_ref () || iter->value_type->is_cref () || iter->value_type->is_ptr () || iter->value_type->is_cptr ()) {

    void *cobj = const_cast<void *> (iter->cls_decl->get_iterator_obj (iter->iter));
    if (cobj == 0) {

      iter->cls_decl->inc_obj_iterator (iter->iter);
      Py_RETURN_NONE;

    } else {

      const gsi::ClassBase *cls_act = iter->cls_decl->subclass_decl (cobj);

      if (iter->value_type->pass_obj ()) {

        PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (cls_act);
        tl_assert (type != NULL);

        //  create an new managed instance
        PYAObjectBase *new_object = (PYAObjectBase *) type->tp_alloc (type, 0);
        PythonRef obj (new_object);
        new (new_object) PYAObjectBase (cls_act);
        new_object->set (cobj, true, false, false);

        //  increment the iterator and return the object
        iter->cls_decl->inc_obj_iterator (iter->iter);
        return obj.release ();

      } else if (iter->value_type->is_cref () && cls_act->can_copy () && cls_act->client_data (cobj, client_index) == 0) {

        //  We copy objects passed by const reference if they are not managed
        //  (derived from gsi::ObjectBase, in that case, client_data is not 0).
        //  Such objects are often exposed internals. First we can't
        //  guarantee the const correctness of references. Second, we
        //  can't guarantee the lifetime of the container will exceed that
        //  of the exposed property. Hence copying is safer.

        PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (cls_act);
        tl_assert (type != NULL);

        //  create a instance and copy the value
        PYAObjectBase *new_object = (PYAObjectBase *) type->tp_alloc (type, 0);
        PythonRef obj (new_object);
        new (new_object) PYAObjectBase (cls_act);
        cls_act->assign (new_object->obj (), cobj);

        //  increment the iterator and return the object
        iter->cls_decl->inc_obj_iterator (iter->iter);
        return obj.release ();

      } else if (cls_act->client_data (cobj, client_index) != 0 && 
          ((ClientData *) cls_act->client_data (cobj, client_index))->get_ref () != NULL) {

        //  we have a that is located in C++ space but is supposed to get attached
        //  a Python object. If it already has, we simply return a reference to this
        PyObject *obj = ((ClientData *) cls_act->client_data (cobj, client_index))->get_ref ();
        Py_INCREF (obj);

        //  correct constness if the object is not supposed to be const
        correct_constness (obj, iter->value_type->is_cref () || iter->value_type->is_cptr ());

        //  increment the iterator and return the object
        iter->cls_decl->inc_obj_iterator (iter->iter);

        return obj;

      } else {

        //  the object returned can be of a derived class. Hence we need to look up the class again
        PyTypeObject *type = PythonInterpreter::instance ()->type_for_cls (cls_act);
        tl_assert (type != NULL);

        //  create the object
        PYAObjectBase *new_object = (PYAObjectBase *) iter->ret_type->tp_alloc (iter->ret_type, 0);
        PythonRef obj (new_object);
        new (new_object) PYAObjectBase (iter->cls_decl);
        new_object->set (cobj, false, iter->value_type->is_cref () || iter->value_type->is_cptr (), iter->value_type->is_ptr ());

        //  increment the iterator and return the object
        iter->cls_decl->inc_obj_iterator (iter->iter);
        return obj.release ();

      }

    }

  } else {

    PYAObjectBase *new_object = (PYAObjectBase *) iter->ret_type->tp_alloc (iter->ret_type, 0);
    PythonRef obj (new_object);
    new (new_object) PYAObjectBase (iter->cls_decl);
    new_object->set (iter->cls_decl->take_iterator_obj (iter->iter), true, false, false);

    //  increment the iterator and return the object
    iter->cls_decl->inc_obj_iterator (iter->iter);
    return obj.release ();

  }
}

static void
pya_iterator_deallocate (PyObject *self)
{
  PYAIteratorObject *p = (PYAIteratorObject *) self;
  if (p->origin) {
    Py_DECREF (p->origin);
    p->origin = 0;
  }
  if (p->iter) {
    p->cls_decl->delete_obj_iterator (p->iter);
    p->iter = 0;
  }
  Py_TYPE (self)->tp_free ((PyObject *) self);
}

void
PYAIteratorObject::make_class (PyObject *module)
{
  static PyTypeObject iterator_type = {
    PyVarObject_HEAD_INIT (&PyType_Type, 0)
    "pya._Iterator",            // tp_name
    sizeof (PYAIteratorObject)  // tp_size
  };

  iterator_type.tp_flags = Py_TPFLAGS_DEFAULT;
  iterator_type.tp_dealloc = pya_iterator_deallocate;
  iterator_type.tp_iter = pya_iterator_iter;
  iterator_type.tp_iternext = pya_iterator_next;

  PyType_Ready (&iterator_type);
  Py_INCREF (&iterator_type);

  PyModule_AddObject (module, "_Iterator", (PyObject *) &iterator_type);

  cls = &iterator_type;
}

PYAIteratorObject *
PYAIteratorObject::create (PyObject *origin, const gsi::ClassBase *cls_decl, void *iter, const gsi::ArgType *value_type, PyTypeObject *ret_type)
{
  tl_assert (cls != 0);
  PYAIteratorObject *iter_obj = (PYAIteratorObject *) cls->tp_alloc (cls, 0);
  if (iter_obj == NULL) {
    check_error ();
  } else {
    if (origin) {
      //  The iterator will keep a reference to the origin object of the iterator
      Py_INCREF (origin);
    }
    iter_obj->origin = origin;
    iter_obj->cls_decl = cls_decl;
    iter_obj->iter = iter;
    iter_obj->value_type = value_type;
    iter_obj->ret_type = ret_type;
  }
  return iter_obj;
}

// -------------------------------------------------------------------
//  Helper objects: PYAPlainIteratorObject

template <class R>
struct convert_iterated_func
{
  void operator() (PythonRef *obj, gsi::IterAdaptorAbstractBase *iter) 
  { 
    *obj = PythonRef (c2python<R> (*((const R *) iter->v_get ())));
  }
};

PyTypeObject *PYAPlainIteratorObject::cls = 0;

/**
 *  @brief Gets the iterator object (reflective)
 */
static PyObject *
pya_plain_iterator_iter (PyObject *self)
{
  //  we have to return a new reference -> when using self, we have to increment out count
  Py_INCREF (self);
  return self;
}

/**
 *  @brief Increments the iterator
 */
static PyObject *
pya_plain_iterator_next (PyObject *self)
{
  PYAPlainIteratorObject *iter = (PYAPlainIteratorObject *) self;

  if (! iter->iter || iter->iter->at_end ()) {
    PyErr_SetNone (PyExc_StopIteration);
    return NULL;
  }

  PythonRef obj;
  gsi::do_on_type<convert_iterated_func> () (iter->value_type->type (), &obj, iter->iter);
  iter->iter->inc ();
  return obj.release ();
}

static void
pya_plain_iterator_deallocate (PyObject *self)
{
  PYAPlainIteratorObject *p = (PYAPlainIteratorObject *) self;
  if (p->origin) {
    Py_DECREF (p->origin);
    p->origin = 0;
  }
  if (p->iter) {
    delete p->iter;
    p->iter = 0;
  }
  Py_TYPE (self)->tp_free ((PyObject *) self);
}

void
PYAPlainIteratorObject::make_class (PyObject *module)
{
  static PyTypeObject iterator_type = {
    PyVarObject_HEAD_INIT (&PyType_Type, 0)
    "pya._PlainIterator",            // tp_name
    sizeof (PYAPlainIteratorObject)  // tp_size
  };

  iterator_type.tp_flags = Py_TPFLAGS_DEFAULT;
  iterator_type.tp_dealloc = pya_plain_iterator_deallocate;
  iterator_type.tp_iter = pya_plain_iterator_iter;
  iterator_type.tp_iternext = pya_plain_iterator_next;

  PyType_Ready (&iterator_type);
  Py_INCREF (&iterator_type);

  PyModule_AddObject (module, "_PlainIterator", (PyObject *) &iterator_type);

  cls = &iterator_type;
}

PYAPlainIteratorObject *
PYAPlainIteratorObject::create (PyObject *origin, gsi::IterAdaptorAbstractBase *iter, const gsi::ArgType *value_type)
{
  tl_assert (cls != 0);
  PYAPlainIteratorObject *iter_obj = (PYAPlainIteratorObject *) cls->tp_alloc (cls, 0);
  if (iter_obj == NULL) {
    check_error ();
  } else {
    if (origin) {
      //  The iterator will keep a reference to the origin object of the iterator
      Py_INCREF (origin);
    }
    iter_obj->origin = origin;
    iter_obj->iter = iter;
    iter_obj->value_type = value_type;
  }
  return iter_obj;
}

}

#endif

