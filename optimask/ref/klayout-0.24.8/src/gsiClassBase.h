
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


#ifndef _HDR_gsiClassBase
#define _HDR_gsiClassBase

#include "tlCollection.h"
#include "tlException.h"
#include "tlExpression.h"
#include "tlAssert.h"
#include "tlHeap.h"

#include "gsiExpression.h"
#include "gsiObject.h"
#include "gsiSerialisation.h"

#include <list>
#include <map>
#include <vector>
#include <typeinfo>
#include <memory>

namespace gsi
{

/**
 *  @brief A base class for client-specific data per class
 *
 *  Objects of this type are used inside the ClassBase object to store 
 *  information specific for certain clients.
 */
class PerClassClientSpecificData
{
public:
  PerClassClientSpecificData ()
  {
    //  .. nothing yet ..
  }

  virtual ~PerClassClientSpecificData()
  {
    //  .. nothing yet ..
  }
};

/**
 *  @brief A class that allows to implement specific behaviour when creating or deleting an object
 *
 *  This is delegate that implements the client-data / C++ object interface by delegation 
 *  rather then by inheritance like gsi::ObjectBase does.
 *
 *  Both, the connector and the gsi::ObjectBase mechanism provide a connection between the 
 *  C++ object and the script object. It supplies the client data, which is a piece
 *  of information which the client (script) implementation uses to store specific information.
 */
class ObjectConnector
{
public:
  /**
   *  @brief Destructor
   */
  virtual ~ObjectConnector () { }

  /**
   *  @brief If a handler is registered, this method is called when a client object is bound to the given C++ object
   *
   *  @param p The C++ object
   */
  virtual void on_attach (void *) const { }

  /**
   *  @brief If a handler is registered, this method is called when a client object is deleted and the given C++ object is released
   *
   *  @param p The C++ object
   */
  virtual void on_detach (void *) const { }

  /**
   *  @brief Return the client data for client index ci for the object or 0 if there is no client data
   */
  virtual void *client_data (void *p, int ci) const { return 0; }
};

/**
 *  @brief The basic object to declare a class
 *
 *  This object represents a GSI class. It provides the methods to translate the typeless object
 *  pointer to a real C++ object and to perform some basic operations on the object.
 *  It also provides metainformation such as method declarations and similar.
 */
class KLAYOUT_DLL ClassBase
  : public tl::Collectable
{
public:
  typedef tl::Collection<ClassBase> class_collection; 
  typedef class_collection::const_iterator class_iterator;
  typedef Methods::iterator method_iterator;
  
  /**
   *  @brief Constructor
   *
   *  The constructor supplies information about the connector class (can be 0), an optional base class
   *  declaration object (can be 0 too), a documentation string and the method declarations.
   */
  ClassBase (const ObjectConnector *mp_connector, const ClassBase *base, const std::string &doc, const Methods &mm);

  /**
   *  @brief Destructor
   */
  virtual ~ClassBase ();

  /**
   *  @brief Gets the pointer to the base class declaration object (can be 0)
   */
  const ClassBase *base () const
  {
    return mp_base;
  }

  /**
   *  @brief Returns the "real" (consolidated) declaration object
   *  
   *  The actual declaration object may be different from this declaration because of the class extension
   *  mechanism. Using that mechanism, class declarations can extend over multiple ClassBase objects. The
   *  individual declarations are merged and a consolidated class declaration object is provided.
   */
  virtual const ClassBase *declaration () const 
  {
    return 0;
  }

  /**
   *  @brief Sets the declaration: establish a link to the real declaration objects
   *  Only ClassExt extensions support this feature.
   */
  virtual void set_declaration (const ClassBase *)
  {
    tl_assert (false);
  }

  /**
   *  @brief Gets the class name
   */
  const std::string &name () const
  {
    return m_name;
  }

  /**
   *  @brief Gets the method declaration collection
   */
  Methods &methods () 
  {
    return m_methods;
  }

  /**
   *  @brief Gets the documentation string
   */
  const std::string &doc () const
  {
    return m_doc;
  }

  /**
   *  @brief Merge base and extension declarations
   *
   *  This method must be called at least once!
   *  It produces the consolidates class declaration object.
   */
  static void merge_declarations ();

  /**
   *  @brief Gets the parent declaration object
   *  This returns the parent class if this class is a child class.
   */
  const ClassBase *parent () const 
  {
    return mp_parent;
  }

  /**
   *  @brief Gets the fully qualified name (A::B::..)
   */
  std::string qname () const;

  /**
   *  @brief Adds a child class
   */
  void add_child_class (const ClassBase *cls);

  /**
   *  @brief Iterates all child classes (begin)
   */
  std::vector<const ClassBase *>::const_iterator begin_child_classes () const
  {
    return m_child_classes.begin ();
  }
 
  /**
   *  @brief Iterates all subclasses (end)
   */
  std::vector<const ClassBase *>::const_iterator end_child_classes () const
  {
    return m_child_classes.end ();
  }
 
  /**
   *  @brief Iterates all classes present (begin)
   */
  static class_iterator begin_classes () 
  {
    return collection ().begin ();
  }
 
  /**
   *  @brief Iterates all classes present (end)
   */
  static class_iterator end_classes () 
  {
    return collection ().end ();
  }
 
  /**
   *  @brief Iterates the methods (begin)
   */
  method_iterator begin_methods () const
  {
    return m_methods.begin ();
  }

  /**
   *  @brief Iterates the methods (end)
   */
  method_iterator end_methods () const
  {
    return m_methods.end ();
  }

  /**
   *  @brief Returns true, if this class is derived from the given base class
   */
  bool is_derived_from (const ClassBase *base) const;

  /**
   *  @brief Callback: an object gets script attention
   *  
   *  This method is called when an object is begin brought alive in the script
   *  environment. Any client implementation must call this method when the object
   *  becomes known to the script interpreter. The class declaration will take
   *  this opportunity to initialize the object's environment related to scripting
   *  support. 
   */
  virtual void attach (void *obj) const { on_attach (obj); }

  /**
   *  @brief Callback: an object is removed from the script interpreter
   */
  virtual void detach (void *obj) const { on_detach (obj); }

  /**
   *  @brief Class implementation: destroys the given object
   *
   *  The implementation of this method is supposed to delete the object and
   *  free any resources associated with that object. After that operation,
   *  the object pointer is invalid.
   *
   *  This method may not be available if "can_destroy" is false.
   */
  virtual void destroy (void * /*obj*/) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation: creates an object (default constructor)
   *
   *  Creates and default-initializes the given object. This method may not be
   *  available if there is no default constructor (can_default_create is false). 
   *  In that case, some static factory method must be provided.
   */
  virtual void *create () const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class implementation: clones the given object
   *
   *  The implementation is supposed to create a deep copy of the source object.
   *  This method may not be available if can_copy is false.
   */
  virtual void *clone (const void * /*src*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class implementation: assigns the source to the target object
   *
   *  The implementation is supposed to overwrite the target with the contents
   *  of the source. This is a deep copy.
   *  This method may not be available if can_copy is false.
   */
  virtual void assign (void * /*target*/, const void * /*src*/) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation: deserialize an object and create a new one based on that
   */
  virtual void *take_and_create (SerialArgs & /*from*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class implementation: deserializes an vector and call the callback for each element
   *
   *  This method iterates a vector objects of this class and calls the "cb" callback for every of
   *  these elements. 
   *  The argument type must be supplied to determine that nature of the vector (a vector of pointers, 
   *  objects, constness etc.). data is an arbitrary value which is passed to the callback in the first
   *  argument.
   *  The callback receives pointer to the objects in the vector. Those are not new'd objects, but 
   *  references. Depedending on the context, these references may be const and the objects must not be
   *  changed.
   *  Not all possible variants of vectors may be supported if can_copy is false.
   */
  virtual void get_vector_of (SerialArgs & /*from*/, const ArgType & /*a*/, void * /*data*/, void (* /*cb*/) (void * /*data*/, void * /*obj*/)) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation: Adds a vector of objects of this class to the serial arguments buffer
   *
   *  The argument type must be supplied in order to determine that nature of the vector. temp_objects 
   *  is a buffer holding temporary objects which potentially need to be created. This list can be
   *  destroyed when the arguments are no longer required.
   *  the objects vector gives the objects to push on the argument stack. The object's references are 
   *  not taken, so their lifetime will still be controlled by the caller.
   *  Not all possible variants of vectors may be supported if can_copy is false.
   */
  virtual void push_vector_of (SerialArgs & /*to*/, const ArgType & /*a*/, tl::Heap & /*heap*/, const std::vector<void *> & /*objects*/) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation: Copies the given object to the argument stack
   *
   *  Creates an in-place copy of the object on the argument stack.
   *  This method may not be available if can_copy is false.
   */
  virtual void copy_to (SerialArgs & /*at*/, void * /*from*/) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation: Gets an iterator for objects of this type from the argument stack
   *
   *  This method is called when a method returns an iterator of objects of this type.
   */
  virtual void *create_obj_iterator (SerialArgs & /*from*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class implementation (iterator): Returns true if the iterator is at the end
   *
   *  The iterator is created with create_obj_iterator and delivers objects of this type.
   */
  virtual bool obj_iterator_at_end (void * /*iter*/) const 
  {
    tl_assert (false);
    return false;
  }

  /**
   *  @brief Class implementation (iterator): delivers a reference to a new object from the iterator.
   *
   *  The references delivered by this method are owned by the caller.
   */
  virtual void *take_iterator_obj (void * /*iter*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class implementation (iterator): delivers a reference to an object from the iterator.
   *
   *  The references delivered by this method are not owned by the caller, in 
   *  contrast to the "take_iterator_obj" method.
   */
  virtual const void *get_iterator_obj (void * /*iter*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class implementation (iterator): increments the iterator
   */
  virtual void inc_obj_iterator (void * /*iter*/) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation (iterator): frees the iterator
   */
  virtual void delete_obj_iterator (void * /*iter*/) const 
  {
    tl_assert (false);
  }

  /**
   *  @brief Class implementation: destruction supported predicate
   *
   *  This flag is true, if the class supports destruction of objects of this kind.
   */
  virtual bool can_destroy () const 
  {
    tl_assert (false);
    return false;
  }

  /**
   *  @brief Class implementation: copy supported predicate
   *
   *  This flag is true, if the class supports copying of objects of this kind.
   */
  virtual bool can_copy () const 
  {
    tl_assert (false);
    return false;
  }

  /**
   *  @brief Class implementation: default construction supported predicate
   *
   *  This flag is true, if the class supports default construction of objects of this kind.
   */
  virtual bool can_default_create () const 
  {
    tl_assert (false);
    return false;
  }

  /**
   *  @brief Class implementation: check C++ type of object
   *
   *  This method returns true, if objects of this class are compatible with 
   *  the given C++ type.
   */
  virtual bool is_of_type (const std::type_info & /*ti*/) const 
  {
    tl_assert (false);
    return false;
  }

  /**
   *  @brief Returns true, if the given object can be cast to this class
   *
   *  When this method is called, it is guaranteed that the object is at least of 
   *  the base class type. This implies that there is a base class when this method
   *  is called.
   */
  virtual bool can_upcast (const void * /*p*/) const
  {
    tl_assert (false);
    return false;
  }

  /**
   *  @brief Class implementation: gets C++ type of object
   *
   *  This method delivers the C++ type_info object of this class.
   */
  virtual const std::type_info &type () const 
  {
    tl_assert (false);
    return typeid (void);
  }

  /**
   *  @brief Class implementation: gets C++ type of object
   *
   *  This method delivers the class declaration for an subclassed object p. p must be of a 
   *  subclass (derived class) of this class. This method will the declaration object 
   *  of object p. 
   */
  virtual const ClassBase *subclass_decl (const void * /*p*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class Implementation: gets the client data of an object
   *
   *  The client data is a piece of information that the scripting client stores along 
   *  with objects (if possible). Not all objects support attachment of client data. In order
   *  to enable client data and related features (such as overloading or events), the 
   *  C++ object must either be derived from gsi::ObjectBase or a connector must be 
   *  provided which attaches client data through some other mechanism.
   *
   *  The client index is a fixed values used by the different scripting clients. Ruby
   *  uses 0, Python 1 etc.
   */
  virtual void *client_data (void * /*p*/, int /*ci*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class Implementation: provide variant binding information (value/references)
   */
  virtual const tl::VariantUserClassBase *var_cls (bool /*reference*/, bool /*is_const*/) const 
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Class Implementation: provide variant binding information (meta/class object)
   */
  virtual const tl::VariantUserClassBase *var_cls_cls () const
  {
    tl_assert (false);
    return 0;
  }

  /**
   *  @brief Post-construction initialization
   *
   *  This method will be called by the GSI system to provide initialization after 
   *  the static initialization. Some schemes cannot be implementation statically, plus
   *  the initialization order is indetermined for static initialization.
   *  In that case, this initialization step is useful. It will call the initialize
   *  method on all method declarations.
   */
  virtual void initialize () 
  {
    m_methods.initialize ();
  }

  /**
   *  @brief Adds a method to the class
   *  The class becomes owner of the method object. 
   *  This method is public to allow dynamic extension of the documentation
   *  through scripts.
   *  Don't use it for other purposes.
   */
  void add_method (MethodBase *method, bool base_class = false);

  /**
   *  @brief Sets the per-client data for the given client index 
   *  This method is const to preserve the general const semantics of the class 
   *  while allowing clients to register information.
   */
  void set_data (int ch, PerClassClientSpecificData *data) const
  {
    mp_data[ch].reset (data);
  }

  /**
   *  @brief Gets the per-client data for the given client index
   */
  PerClassClientSpecificData *data (int ch) const
  {
    return mp_data[ch].get ();
  }

  /**
   *  @brief Sets the per-client data for gsi::Expressions
   *  This is a special slot since no client index is available for gsi::Expressions.
   */
  void set_gsi_data (PerClassClientSpecificData *data) const
  {
    mp_gsi_data.reset (data);
  }

  /**
   *  @brief Gets the per-client data for gsi::Expressions
   */
  PerClassClientSpecificData *gsi_data () const
  {
    return mp_gsi_data.get ();
  }

protected:
  static const class_collection &collection ();

  const std::vector<const ClassBase *> &subclasses () const
  {
    return m_subclasses;
  }

  void set_name (const std::string &n) 
  {
    m_name = n;
  }

  void set_parent (const ClassBase *parent);

  void on_attach (void *p) const;
  void on_detach (void *p) const;

  void *basic_client_data (void *p, int ci) const
  { 
    if (mp_connector) {
      return mp_connector->client_data (p, ci);
    } else {
      return 0;
    }
  }

private:
  const ClassBase *mp_base;
  std::string m_doc;
  Methods m_methods;
  std::string m_name;
  std::vector<const ClassBase *> m_child_classes;
  const ClassBase *mp_parent;
  std::vector<const ClassBase *> m_subclasses;
  const ObjectConnector *mp_connector;
  mutable std::auto_ptr<PerClassClientSpecificData> mp_data[num_clients];
  mutable std::auto_ptr<PerClassClientSpecificData> mp_gsi_data;

  static class_collection *mp_class_collection;
  static unsigned int m_class_count;
};

/**
 *  @brief Accessor to a declaration through name
 */
KLAYOUT_DLL const ClassBase *class_by_name (const std::string &name);

/**
 *  @brief Returns true if there is a class with the given name
 */
KLAYOUT_DLL bool has_class (const std::string &name);

/**
 *  @brief Find a class declaration through the type info 
 */
KLAYOUT_DLL const ClassBase *class_by_typeinfo (const std::type_info &ti);

/**
 *  @brief Returns true if there is a class with the given type info
 */
KLAYOUT_DLL bool has_class (const std::type_info &ti);

}

#endif


