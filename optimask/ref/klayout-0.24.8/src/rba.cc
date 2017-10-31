
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

#include "gsiDecl.h"
#include "gsiDeclBasic.h"
#include "gsiExpression.h"
#include "rba.h"
#include "tlString.h"
#include "tlInternational.h"
#include "tlException.h"
#include "tlAssert.h"
#include "tlLog.h"

#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <cctype>
#include <algorithm>

#include <QtCore/QString>
#include <QtCore/QByteArray>

#if !defined(HAVE_RUBY_VERSION_CODE)
#  define HAVE_RUBY_VERSION_CODE 10901
#endif

// -------------------------------------------------------------------
//  This part is available only if Ruby is enabled

#ifdef HAVE_RUBY

#include <ruby.h>
#include <signal.h>

//  define getter for top_self
static VALUE ruby_top_self = Qnil;
static VALUE get_top_self ()
{
  return ruby_top_self;
}

//  define a setter for $0
static void set_progname (VALUE pn)
{
  rb_gv_set ("PROGRAM_NAME", pn);
}

//  Add some compatibility definitions for Ruby 1.8
#if HAVE_RUBY_VERSION_CODE < 10900

#  include "node.h"

//  Compatibility macros for Ruby 1.8
#  if !defined(RSTRING_PTR)
#    define RSTRING_PTR(s) (RSTRING(s)->ptr)
#  endif

#  if !defined(RSTRING_LEN)
#    define RSTRING_LEN(s) (RSTRING(s)->len)
#  endif

#  if !defined(RARRAY_PTR)
#    define RARRAY_PTR(s) (RARRAY(s)->ptr)
#  endif

#  if !defined(RARRAY_LEN)
#    define RARRAY_LEN(s) (RARRAY(s)->len)
#  endif

#  if !defined(RCLASS_SUPER)
#    define RCLASS_SUPER(k) (RCLASS (k)->super)
#  endif

//  Ruby 1.8 does not define NUL2ULL
#  if !defined(NUM2ULL) 
#    define NUM2ULL(x) rb_num2ull((VALUE)x)
#  endif

//  add rb_errinfo and rb_set_errinfo for Ruby 1.8
static VALUE rb_errinfo()
{
  return rb_gv_get ("$!");
}

static void rb_set_errinfo(VALUE v)
{
  rb_gv_set ("$!", v);
}

//  add rb_sourcefile and rb_sourceline for Ruby 1.8
static const char *rb_sourcefile ()
{
  ruby_set_current_source ();
  return ruby_sourcefile;
}

static int rb_sourceline ()
{
  ruby_set_current_source ();
  return ruby_sourceline;
}

//  add an emulation of rb_binding_new
static VALUE rb_binding_new ()
{
  return rb_funcall (get_top_self(), rb_intern ("binding"), 0);
}

#endif

//  Define a method to create a Ruby const char * string from a UTF-8 strings.
//  This method is used for rb_require, rb_load.
static std::string rb_cstring_from_utf8 (const std::string &utf8)
{
	// TODO: implement (how?)
	return utf8;
}

namespace rba
{

static void rba_get_backtrace_from_array (VALUE backtrace, std::vector<gsi::BacktraceElement> &bt, unsigned int skip = 0);

static VALUE alloc_proxy (VALUE klass);

// -------------------------------------------------------------------
//  RubyStackTraceProvider definition and implementation

class KLAYOUT_DLL RubyStackTraceProvider
  : public gsi::StackTraceProvider
{
public:
  RubyStackTraceProvider (const std::string &scope)
    : m_scope (scope) 
  { }

  virtual std::vector<gsi::BacktraceElement> stack_trace () const 
  {
    std::vector<gsi::BacktraceElement> bt;
    bt.push_back (gsi::BacktraceElement (rb_sourcefile (), rb_sourceline ()));
    rba_get_backtrace_from_array (rb_funcall (rb_mKernel, rb_intern ("caller"), 0), bt, 0);
    return bt;
  }


  virtual size_t scope_index () const
  {
    if (! m_scope.empty ()) {
      std::vector<gsi::BacktraceElement> bt = stack_trace ();
      for (size_t i = 0; i < bt.size (); ++i) {
        if (bt[i].file == m_scope) {
          return i;
        }
      }
    }
    return 0;
  }

  virtual int stack_depth () const
  {
    // TODO: there should be a more efficient implementation!!
    std::vector<gsi::BacktraceElement> bt;
    bt.push_back (gsi::BacktraceElement (rb_sourcefile (), rb_sourceline ()));
    rba_get_backtrace_from_array (rb_funcall (rb_mKernel, rb_intern ("caller"), 0), bt, 0);
    return int (bt.size ());
  }

  //  we could use this for ruby >= 1.9.3
#if 0
  static int
  count_stack_levels(void *arg, VALUE file, int line, VALUE method)
  {
    *(int *)arg += 1;
    return 0;
  }

  extern "C" int rb_backtrace_each (int (*iter)(void *arg, VALUE file, int line, VALUE method), void *arg);

  virutal int stack_depth ()
  {
    int l = 0;
    rb_backtrace_each(count_stack_levels, &l);
    return l;
  }
#endif

private:
  std::string m_scope;
};

// --------------------------------------------------------------------------
//  Global variables

static std::map <VALUE, const gsi::ClassBase *> cls_map;
static std::map <const gsi::ClassBase *, VALUE> rev_cls_map;

static RubyInterpreter *rba_interpreter = 0;
static bool rba_ruby_destroyed = false;

static VALUE saved_stderr = Qnil;
static VALUE saved_stdout = Qnil;
static VALUE stdout_klass = Qnil, stderr_klass = Qnil;

//  TODO: move this into a singleton instance and provide thread-safetiness
static gsi::Console *sp_current_console = 0;
static std::vector<gsi::Console *> s_consoles;
static gsi::ExecutionHandler *sp_current_exec_handler = 0;
static int s_current_exec_level = 0;
static bool s_in_trace = false;
static bool s_exit_on_next = false;
static bool s_block_exceptions = false;
static bool s_ignore_next_exception = false;
static std::string s_debugger_scope;
static VALUE s_current_binding = Qnil;
static std::map<const char *, size_t> s_file_id_map;
static std::vector<gsi::ExecutionHandler *> s_exec_handlers;

#define RUBY_BEGIN_EXEC \
  try { \
    s_file_id_map.clear (); \
    s_exit_on_next = false; \
    s_block_exceptions = false; \
    if (s_current_exec_level++ == 0 && sp_current_exec_handler) { \
      sp_current_exec_handler->start_exec (rba_interpreter); \
    }

#define RUBY_END_EXEC \
    if (s_current_exec_level > 0 && --s_current_exec_level == 0 && sp_current_exec_handler) { \
      sp_current_exec_handler->end_exec (rba_interpreter); \
    } \
  } catch (...) { \
    if (s_current_exec_level > 0 && --s_current_exec_level == 0 && sp_current_exec_handler) { \
      sp_current_exec_handler->end_exec (rba_interpreter); \
    } \
    if (! s_exit_on_next) { \
      throw; \
    } \
  } \
  if (s_exit_on_next) { \
    s_exit_on_next = false; \
    throw gsi::ExitException (0); \
  }

// -------------------------------------------------------------------
//  Utilities to generate C++ exceptions from Ruby exceptions

static gsi::BacktraceElement 
rba_split_bt_information (const char *m, size_t l)
{
  for (size_t i = 0; i + 1 < l; ++i) {

    if (m[i] == ':' && isdigit (m[i + 1])) {

      size_t j = i + 1;
      int line = 0;
      while (j < l && isdigit (m[j])) {
        line = (line * 10) + (int)(m[j] - '0');
        ++j;
      }

      std::string file;
      if (line > 0) {
        file = std::string (m, i);
      }
      if (j == l) {
        return gsi::BacktraceElement (file, line);
      } else if (m[j] == ':') {
        return gsi::BacktraceElement (file, line, std::string (m, j + 1, l - (j + 1)));
      }

    }

  }

  return gsi::BacktraceElement (std::string (), 0, std::string (m, 0, l));
}

static void
rba_get_backtrace_from_array (VALUE backtrace, std::vector<gsi::BacktraceElement> &bt, unsigned int skip)
{
  if (TYPE (backtrace) == T_ARRAY) {
    unsigned int len = RARRAY_LEN(backtrace);
    VALUE *el = RARRAY_PTR(backtrace);
    bt.reserve (bt.size () + (len >= skip ? len - skip : 0));
    while (len-- > 0) {
      if (skip > 0) {
        ++el;
        --skip;
      } else {
        VALUE str = StringValue (*el++);
        bt.push_back (rba_split_bt_information (RSTRING_PTR(str), RSTRING_LEN(str)));
      }
    }
  }
}

static void
rba_check_error (unsigned int drop = 0)
{
  VALUE lasterr = rb_errinfo ();
  if (lasterr != Qnil) {

    if (CLASS_OF (lasterr) == rb_eSystemExit) {
      int status = NUM2INT (rb_funcall (lasterr, rb_intern ("status"), 0));
      throw gsi::ExitException (status);
    }

    VALUE klass = rb_class_path (CLASS_OF (lasterr));
    std::string eclass = std::string (RSTRING_PTR(klass), RSTRING_LEN(klass));
    VALUE message = rb_obj_as_string(lasterr);
    std::string emsg = std::string (RSTRING_PTR(message), RSTRING_LEN(message));

    std::vector <gsi::BacktraceElement> bt;
    rba_get_backtrace_from_array (rb_funcall (lasterr, rb_intern ("backtrace"), 0), bt, 0);
    while (! bt.empty () && drop-- > 0) {
      bt.pop_back ();
    }

    if (! s_debugger_scope.empty ()) {
      for (size_t i = 0; i < bt.size (); ++i) {
        if (bt [i].file == s_debugger_scope) {
          bt.erase (bt.begin (), bt.begin () + i);
          break;
        }
      }
      s_debugger_scope.clear ();
    }

    //  parse the backtrace to get the line number
    gsi::BacktraceElement info;
    if (! bt.empty ()) {
      //  use the backtrace
      info = bt.front ();
    } else {
      //  use the error message
      info = rba_split_bt_information (emsg.c_str (), emsg.size ());
    }

    if (info.line > 0) {
      throw RubyError (emsg.c_str (), info.file.c_str (), info.line, eclass.c_str (), bt);
    } else {
      throw RubyError (emsg.c_str (), eclass.c_str (), bt);
    }
  }
}

/**
 *  @brief needed because StringValue is a macro:
 */
static VALUE
rba_string_value_f (VALUE obj)
{
  return StringValue (obj);
}

/**
 *  @brief string value retrieval with check 
 */
static VALUE
rba_safe_string_value (VALUE obj)
{
  VALUE ret = Qnil;

  RUBY_BEGIN_EXEC 

    rb_set_errinfo (Qnil);
    int error = 0;
    ret = rb_protect (rba_string_value_f, obj, &error);
    if (error) {
      rba_check_error ();
    }

  RUBY_END_EXEC

  return ret;
}

/**
 *  @brief object to string with check 
 */
static VALUE
rba_safe_obj_as_string (VALUE obj)
{
  VALUE ret = Qnil;

  RUBY_BEGIN_EXEC 

    rb_set_errinfo (Qnil);
    int error = 0;
    ret = rb_protect (rb_obj_as_string, obj, &error);
    if (error) {
      rba_check_error ();
    }

  RUBY_END_EXEC

  return ret;
}

/**
 *  @brief Find the class declaration from the Ruby object
 */
const gsi::ClassBase *
find_cclass (VALUE k)
{
  std::map <VALUE, const gsi::ClassBase *>::const_iterator cls;

  //  find the class that is bound to C++ (maybe a super class)
  while (k != rb_cObject) { 
    cls = cls_map.find (k);
    if (cls == cls_map.end ()) {
      //  if not found advance to super class
      k = RCLASS_SUPER (k);
    } else {
      break;
    }
  }

  tl_assert (cls != cls_map.end ());
  return cls->second;
}

// -------------------------------------------------------------------
//  The lookup table for the method overload resolution

/**
 *  @brief A single entry in the method table
 *  This class provides an entry for one name. It provides flags
 *  (ctor, static, protected) for the method and a list of implementations
 *  (gsi::MethodBase objects)
 */
class MethodTableEntry
{
public:
  typedef std::vector<const gsi::MethodBase *>::const_iterator method_iterator;

  MethodTableEntry (const std::string &name, bool ctor, bool st, bool prot)
    : m_name (name), m_is_ctor (ctor), m_is_static (st), m_is_protected (prot)
  { }

  const std::string &name () const
  {
    return m_name;
  }

  bool is_ctor () const
  {
    return m_is_ctor;
  }

  bool is_static () const
  {
    return m_is_static;
  }

  bool is_protected () const
  {
    return m_is_protected;
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
  bool m_is_ctor : 1;
  bool m_is_static : 1;
  bool m_is_protected : 1;
  std::vector<const gsi::MethodBase *> m_methods;
};

/**
 *  @brief The method table for a class
 *  The method table will provide the methods associated with a native method, i.e.
 *  a certain name. It only provides the methods, not a overload resolution strategy.
 */
class MethodTable
  : public gsi::PerClassClientSpecificData
{
public:
  /**
   *  @brief Returns the lowest method ID within the space of this table
   *  Method IDs below this one are reserved for base class methods
   */
  size_t bottom_mid () const
  {
    return m_method_offset;
  }

  /**
   *  @brief Returns the topmost + 1 method ID.
   */
  size_t top_mid () const
  {
    return m_method_offset + m_table.size ();
  }

  /**
   *  @brief Adds a method to the table 
   *  The ctor flag indicates that instance method to static call translation needs to
   *  be performed in order to implement "initialize" which basically is a method called
   *  on the already constructed object.
   */
  void add_method_generic (const std::string &name, const gsi::MethodBase *mb, bool ctor) 
  {
    bool st = mb->is_static ();

    std::map<std::pair<bool, std::string>, size_t>::iterator n = m_name_map.find (std::make_pair (st, name));
    if (n == m_name_map.end ()) {

      m_name_map.insert (std::make_pair (std::make_pair(st, name), m_table.size ()));
      m_table.push_back (MethodTableEntry (name, ctor, mb->is_static (), mb->is_protected ()));
      m_table.back ().add (mb);

    } else {

      if (ctor && ! m_table [n->second].is_ctor ()) {
        tl::warn << "Class " << mp_cls_decl->name () << ": method '" << name << " is both a constructor and non-constructor";
      }
      if (m_table [n->second].is_protected () != mb->is_protected ()) {
        tl::warn << "Class " << mp_cls_decl->name () << ": method '" << name << " is both a protected and non-protected";
      }

      m_table [n->second].add (mb);

    }
  }

  /**
   *  @brief Convenience wrapper
   */
  void add_ctor_method (const std::string &name, const gsi::MethodBase *mb) 
  {
    add_method_generic (name, mb, true);
  }

  /**
   *  @brief Convenience wrapper
   */
  void add_method (const std::string &name, const gsi::MethodBase *mb) 
  {
    add_method_generic (name, mb, false);
  }

  /**
   *  @brief Returns true if the method with the given ID has "ctor" semantics (see above)
   */
  bool is_ctor (size_t mid) const
  {
    return m_table [mid - m_method_offset].is_ctor ();
  }

  /**
   *  @brief Returns true if the method with the given ID is static
   */
  bool is_static (size_t mid) const
  {
    return m_table [mid - m_method_offset].is_static ();
  }

  /**
   *  @brief Returns true if the method with the given ID is protected
   */
  bool is_protected (size_t mid) const
  {
    return m_table [mid - m_method_offset].is_protected ();
  }

  /**
   *  @brief Returns the name of the method with the given ID
   */
  const std::string &name (size_t mid) const
  {
    return m_table [mid - m_method_offset].name ();
  }

  /**
   *  @brief Begins iteration of the overload variants for method ID mid
   */
  MethodTableEntry::method_iterator begin (size_t mid) const
  {
    return m_table[mid - m_method_offset].begin ();
  }

  /**
   *  @brief Ends iteration of the overload variants for method ID mid
   */
  MethodTableEntry::method_iterator end (size_t mid) const
  {
    return m_table[mid - m_method_offset].end ();
  }

  /**
   *  @brief Finishes construction of the table
   *  This method must be called after the add_method calls have been used
   *  to fill the table. It will remove duplicate entries and clean up memory.
   */
  void finish () 
  {
    for (std::vector<MethodTableEntry>::iterator m = m_table.begin (); m != m_table.end (); ++m) {
      m->finish ();
    }
    m_name_map.clear ();
  }

  /**
   *  @brief Obtain a method table for a given class
   */
  static MethodTable *method_table_by_class (const gsi::ClassBase *cls_decl, bool force_init = false)
  {
    MethodTable *mt = dynamic_cast<MethodTable *>(cls_decl->data (client_index));
    if (! mt || force_init) {
      MethodTable *mtnc = new MethodTable (cls_decl);
      mt = mtnc;
      cls_decl->set_data (client_index, mtnc);
    }
    return mt;
  }

private:
  size_t m_method_offset;
  const gsi::ClassBase *mp_cls_decl;
  std::map<std::pair<bool, std::string>, size_t> m_name_map;
  std::vector<MethodTableEntry> m_table;

  /**
   *  @brief Constructor
   *  This constructor will create a method table for the given class and register
   *  this table under this class. 
   *  It is used by method_table_by_class only, hence it's private.
   */
  MethodTable (const gsi::ClassBase *cls_decl)
    : m_method_offset (0), mp_cls_decl (cls_decl)
  { 
    if (cls_decl->base ()) {
      const MethodTable *base_mt = method_table_by_class (cls_decl->base ());
      tl_assert (base_mt);
      m_method_offset = base_mt->top_mid ();
    }
  }
};

// -------------------------------------------------------------------
//  Some utilities for handling Ruby errors

/**
 *  @brief Initialize the interpreter
 *
 *  It was learned (by bitter experience), that in particular rb_protect cannot simply be 
 *  called but requires some safety measures, at least in Ruby 1.8.5 as possibly 1.8.x. 
 *  What happens in rb_protect is that a RNode object is instantiated in some kind
 *  of compile steps that builds a temporary executable statement. Deep inside this
 *  functionality, the rb_sourcefile () variable is copied into the RNode object, as this
 *  variable is usually updated with the current sourcefile name. 
 *  In effect, the string referenced by this variable participates in the GC mark&sweep
 *  steps which leads to unpredicable results, if this variable is not set to a valid 
 *  string (ruby) buffer or 0.
 *
 *  As a consequence, this function must be called before rb_protect and likely other xx_protect
 *  derivatives.
 */
static void rb_protect_init ()
{
#if HAVE_RUBY_VERSION_CODE<10900
  ruby_sourcefile = 0;
  ruby_sourceline = 0;
#endif
}

//  A protect wrapper for rb_funcall2

struct rb_funcall2_params
{
  VALUE obj;
  ID id;
  int argc;
  VALUE *args;
};

static VALUE
rb_funcall2_wrap (VALUE args)
{
  //  HINT: this ugly cast is required since there is only one form of rb_protect
  const rb_funcall2_params *p = (const rb_funcall2_params *) args;
  return rb_funcall2 (p->obj, p->id, p->argc, p->args);
}

static VALUE rb_funcall2_checked (VALUE obj, ID id, int argc, VALUE *args)
{
#if HAVE_RUBY_VERSION_CODE>=10900
  //  Hint: calling of methods on terminated objects cannot really be avoided in all cases - 
  //  for example when the destructor triggers some callback as is the case in Qt events
  //  (i.e. childEvent is triggered when a child is removed and may happen on a parent which
  //  is terminated but not destroyed yet). To avoid problems in that case we simply ignore the 
  //  call.
  if (TYPE (obj) == T_ZOMBIE) {
    return Qnil;
  }
#endif

  VALUE ret = Qnil;

  RUBY_BEGIN_EXEC

    rb_funcall2_params p;
    p.obj  = obj;
    p.id   = id;
    p.argc = argc;
    p.args = args;

    int error = 0;
    //  HINT: the ugly (VALUE) cast is required since there is only one form of rb_protect
    rb_protect_init (); // see above
    ret = rb_protect (&rb_funcall2_wrap, (VALUE) &p, &error);
    if (error) {
      rba_check_error ();
    }

  RUBY_END_EXEC

  return ret;
}

//  A protect wrapper for calling "eval" on an object

struct rb_f_eval_params
{
  int argc;
  VALUE *argv;
  VALUE self;
};

static VALUE
rb_f_eval_wrap (VALUE args)
{
  //  HINT: this ugly cast is required since there is only one form of rb_protect
  const rb_f_eval_params *p = (const rb_f_eval_params *) args;
  return rb_funcall2 (p->self, rb_intern ("eval"), p->argc, p->argv);
}

static VALUE 
rb_f_eval_checked (int argc, VALUE *argv, VALUE self)
{
  VALUE ret = Qnil;

  RUBY_BEGIN_EXEC 

    rb_f_eval_params p;
    p.argc = argc;
    p.argv = argv;
    p.self = self;

    int error = 0;
    //  HINT: the ugly (VALUE) cast is required since there is only one form of rb_protect
    rb_protect_init (); // see above
    ret = rb_protect (&rb_f_eval_wrap, (VALUE) &p, &error);
    if (error) {
      //  drop the eval call inside rb_f_eval_wrap from the stack, hence the "1" parameter
      rba_check_error (1);
    }

  RUBY_END_EXEC

  return ret;
}

static VALUE do_eval_string (const char *expr, const char *file, int line, int context)
{
  rb_set_errinfo (Qnil);

  if (file) {
    set_progname (rb_str_new (file, strlen (file)));
  } else {
    const char *e = "-e";
    set_progname (rb_str_new (e, strlen (e)));
  }

  int argc;
  VALUE args[4];
  args[0] = rb_str_new (expr, strlen (expr));
  //  use the current binding if there is one. This allows to eval in the context of a current trace callback
  //  when eval is called from the trace handler.
  if (! s_in_trace || context < 0) {
    args[1] = rb_const_get (rb_cObject, rb_intern ("TOPLEVEL_BINDING")); // taken from ruby.c
  } else {
    //  TODO: throw an exception if context > 0? Or is there a way to access bindings up the stack?
    if (s_current_binding == Qnil) {
      s_current_binding = rb_binding_new ();
      rb_gc_register_address (&s_current_binding);
    }
    args[1] = s_current_binding;
  }
  if (file) {
    args[2] = rb_str_new (file, strlen (file));
    args[3] = INT2NUM(line);
    argc = 4;
  } else {
    argc = 2;
  }

  return rb_f_eval_checked (argc, args, get_top_self ());
}

//  A protect wrapper for rb_class_new_instance

struct rb_class_new_instance_param
{
  int argc;
  VALUE *argv;
  VALUE klass;
};

static VALUE
rb_class_new_instance_wrap (VALUE args)
{
  //  HINT: this ugly cast is required since there is only one form of rb_protect
  const rb_class_new_instance_param *p = (const rb_class_new_instance_param *) args;
  return rb_class_new_instance (p->argc, p->argv, p->klass);
}

static VALUE 
rb_class_new_instance_checked (int argc, VALUE *argv, VALUE klass)
{
  VALUE ret = Qnil;

  RUBY_BEGIN_EXEC 

    rb_class_new_instance_param p;
    p.argc = argc;
    p.argv = argv;
    p.klass = klass;

    int error = 0;
    //  HINT: the ugly (VALUE) cast is required since there is only one form of rb_protect
    rb_protect_init (); // see above
    ret = rb_protect (&rb_class_new_instance_wrap, (VALUE) &p, &error);
    if (error) {
      rba_check_error ();
    }

  RUBY_END_EXEC

  return ret;
}

/**
 *  @brief Gets the class as a string
 */
std::string 
rb_class_name (VALUE self)
{
  if (TYPE (self) != T_CLASS) {
    self = rb_class_of (self);
  }

  VALUE str = rb_obj_as_string (self);
  return std::string (RSTRING_PTR (str), RSTRING_LEN (str));
}

// -------------------------------------------------------------------
//  The GSI object proxy stored within the Ruby object

class Proxy 
  : public gsi::Callee
{
public:
  struct CallbackFunction
  {
    CallbackFunction (ID id, VALUE o, const gsi::MethodBase *m)
      : method_id (id), rb_obj (o), method (m)
    {
      //  .. nothing yet ..
    }

    ID method_id;
    VALUE rb_obj;
    const gsi::MethodBase *method;
  };

  Proxy (const gsi::ClassBase *_cls_decl)
    : m_cls_decl (_cls_decl), m_obj (0), m_owned (false), m_const_ref (false), m_destroyed (false), m_can_destroy (false)
  { }

  ~Proxy ();

  void keep (VALUE self);
  void release (VALUE self);
  void destroy ();
  void set (void *obj, bool owned, bool const_ref, bool can_destroy, VALUE self);
  void detach ();

  const gsi::ClassBase *cls_decl () const 
  {
    return m_cls_decl;
  }

  bool destroyed () const 
  {
    return m_destroyed;
  }

  bool const_ref () const 
  {
    return m_const_ref;
  }

  void set_const_ref (bool c) 
  {
    m_const_ref = c;
  }

  void *obj (VALUE self);

  bool owned () const 
  {
    return m_owned;
  }

  int add_callback (const CallbackFunction &vf)
  {
    m_cbfuncs.push_back (vf);
    return int (m_cbfuncs.size () - 1);
  }

  void clear_callbacks ()
  {
    m_cbfuncs.clear ();
  }

  void register_event_proc (const gsi::MethodBase *meth, VALUE proc);

  virtual void call (int id, gsi::SerialArgs &args, gsi::SerialArgs &ret);

private:
  const gsi::ClassBase *m_cls_decl;
  void *m_obj;
  bool m_owned : 1;
  bool m_const_ref : 1;
  bool m_destroyed : 1;
  bool m_can_destroy : 1;

  std::vector<CallbackFunction> m_cbfuncs; 
  std::map<const gsi::MethodBase *, VALUE> m_proc_table;

  typedef std::vector<const gsi::MethodBase *> callback_methods_type;
  typedef std::map<VALUE, callback_methods_type> callbacks_cache;
  static callbacks_cache s_callbacks_cache;

  void initialize_callbacks (VALUE self);
};

// --------------------------------------------------------------------------
//  Ruby client data 

/**
 *  @brief The Ruby data structure representing the link from a C++ object to the Ruby object
 */
class RBAClientData
{
public:
  /**
   *  @brief Default constructor
   */
  RBAClientData ()
    : m_self (Qnil), m_locked (0)
  {
    //  .. nothing yet ..
  }

  /**
   *  @brief Destructor
   */
  ~RBAClientData () 
  {
    //  once the object is destroyed, we may release the Ruby reference
    if (m_locked > 0) {
      rb_gc_unregister_address (&m_self);
    }

    if (m_self != Qnil) {
      //  Hint: we cannot use Data_Get_Struct here since this one checks the type and
      //  in some cases (i.e. Qt's recursive destruction of objects already marked in the GC),
      //  the type is T_ZOMBIE
      Proxy *p = (Proxy *) DATA_PTR (m_self);
      p->detach ();
    }

    m_self = Qnil;
    m_locked = 0;
  }

  /**
   *  @brief Resets the Ruby reference to nil
   */
  void reset_ref ()
  {
    //  release the object reference if required
    if (m_locked > 0) {
      rb_gc_unregister_address (&m_self);
    }

    m_self = Qnil;
    m_locked = 0;
  }

  /**
   *  @brief Sets the Ruby reference
   *
   *  @arg s The Ruby reference
   *  @arg lock true, if the Ruby object should be locked, i.e. a reference should be kept
   *
   *  Using lock = true is equivalent to call keep after set_ref
   */
  void set_ref (VALUE s, bool lock)
  {
    tl_assert (m_self == Qnil);
    m_self = s;

    //  this avoids that our Ruby reference is deleted by the GC
    if (lock) {
      keep ();
    }
  }

  /**
   *  @brief Gets the Ruby reference
   */
  VALUE get_ref () const
  {
    return m_self;
  }

  /**
   *  @brief Release the Ruby reference
   *
   *  After having released the ruby references, the GC will destroy the object.
   *  keep() and release() do reference counting, i.e. the object is released when release() is called
   *  as many times as keep().
   */
  void release () 
  {
    if (m_locked > 0) {
      --m_locked;
      if (m_locked == 0) {
        //  make the GC remove the proxy if no longer used
        rb_gc_unregister_address (&m_self);
      }
    }
  }

  /**
   *  @brief Keep a Ruby reference
   *
   *  After calling keep(), the GC will no longer destroy the object. keep() can be 
   *  cancelled with release().
   */
  void keep () 
  {
    if (m_locked == 0) {
      //  make the GC not remove the proxy if no longer used
      rb_gc_register_address (&m_self);
    }
    ++m_locked;
  }

  /**
   *  @brief Returns true, if the object is locked.
   */
  bool locked () const
  {
    return m_locked > 0;
  }

private:
  VALUE m_self;
  int m_locked;
};

// --------------------------------------------------------------------------
//  Ruby client data handler

class RBAClientHandler
  : public gsi::ClientHandler
{
public:
  virtual void attach (void *&data) 
  {
    data = new RBAClientData ();
  }

  virtual void detach (void *&data) 
  {
    void *o = data;
    data = 0;
    delete (RBAClientData *) o;
  }

  virtual void release (void *&data)
  {
    if (data) {
      ((RBAClientData *) data)->release ();
    }
  }

  virtual void keep (void *&data)
  {
    if (data) {
      ((RBAClientData *) data)->keep ();
    }
  }
};

// -------------------------------------------------------------------
//  Type checks 
//
//  The type checks are somewhat more picky than the ruby2c functions.
//  The type checks are used to resolve overridden methods, so being picky
//  might be more appropriate.

template <class T>
bool test_type (VALUE /*rval*/, bool /*loose*/)
{
  return false;
}

template <>
bool test_type<bool> (VALUE rval, bool loose)
{
  if (loose) {
    return true;  // everything can be converted to bool
  } else {
    unsigned int t = TYPE (rval);
    return (t == T_FALSE || t == T_TRUE || t == T_NIL);
  }
}

//  used for other integer types as well:
template <>
bool test_type<int> (VALUE rval, bool loose)
{
  unsigned int t = TYPE (rval);
  return (t == T_FIXNUM || t == T_BIGNUM || (loose && t == T_FLOAT));
}

template <>
bool test_type<char> (VALUE rval, bool loose) 
{ 
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<signed char> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose);
}

template <>
bool test_type<unsigned char> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<short> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose);
}

template <>
bool test_type<unsigned short> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<unsigned int> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<long> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<unsigned long> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<long long> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

template <>
bool test_type<unsigned long long> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}

#if defined(HAVE_64BIT_COORD)
template <>
bool test_type<__int128> (VALUE rval, bool loose)
{
  return test_type<int> (rval, loose); 
}
#endif

template <>
bool test_type<double> (VALUE rval, bool loose)
{
  unsigned int t = TYPE (rval);
  return (t == T_FLOAT || (loose && (t == T_FIXNUM || t == T_BIGNUM)));
}

template <>
bool test_type<float> (VALUE rval, bool loose)
{
  return test_type<double> (rval, loose); 
}

template <>
bool test_type<void *> (VALUE rval, bool loose)
{
  return test_type<size_t> (rval, loose);
}

//  used for strings in general:
template <>
bool test_type<const char *> (VALUE rval, bool loose)
{
  return TYPE (rval) == T_STRING;
}

template <>
bool test_type<std::string> (VALUE rval, bool loose)
{
  return test_type<const char *> (rval, loose); 
}

template <>
bool test_type<QByteArray> (VALUE rval, bool loose)
{
  return test_type<const char *> (rval, loose); 
}

template <>
bool test_type<QString> (VALUE rval, bool loose)
{
  return test_type<const char *> (rval, loose); 
}

template <>
bool test_type<tl::Variant> (VALUE rval, bool loose)
{
  //  assume we can translate everything into a variant
  return true;
}

template <>
bool test_type<gsi::ObjectType> (VALUE rval, bool)
{
  return TYPE (rval) == T_DATA;
}

template <>
bool test_type<gsi::VectorType> (VALUE rval, bool)
{
  return TYPE (rval) == T_ARRAY;
}

template <>
bool test_type<gsi::MapType> (VALUE rval, bool)
{
  return TYPE (rval) == T_HASH;
}

// -------------------------------------------------------------------
//  Ruby to C conversion

template <class T>
T ruby2c (VALUE v);

template <>
bool ruby2c<bool> (VALUE rval)
{
  return bool (RTEST (rval));
}

template <>
char ruby2c<char> (VALUE rval)
{
  return NUM2INT (rval);
}

template <>
signed char ruby2c<signed char> (VALUE rval)
{
  return NUM2INT (rval);
}

template <>
unsigned char ruby2c<unsigned char> (VALUE rval)
{
  return NUM2UINT (rval);
}

template <>
short ruby2c<short> (VALUE rval)
{
  return NUM2INT (rval);
}

template <>
unsigned short ruby2c<unsigned short> (VALUE rval)
{
  return NUM2UINT (rval);
}

template <>
int ruby2c<int> (VALUE rval)
{
  return NUM2INT (rval);
}

template <>
unsigned int ruby2c<unsigned int> (VALUE rval)
{
  return NUM2UINT (rval);
}

template <>
long ruby2c<long> (VALUE rval)
{
  return NUM2LONG (rval);
}

template <>
unsigned long ruby2c<unsigned long> (VALUE rval)
{
  return NUM2ULONG (rval);
}

template <>
long long ruby2c<long long> (VALUE rval)
{
  return NUM2LL (rval);
}

template <>
unsigned long long ruby2c<unsigned long long> (VALUE rval)
{
  return NUM2ULL (rval);
}

#if defined(HAVE_64BIT_COORD)
template <>
__int128 ruby2c<__int128> (VALUE rval)
{
  // TOOD: this is pretty simplistic
  return NUM2DBL (rval);
}
#endif

template <>
double ruby2c<double> (VALUE rval)
{
  return NUM2DBL (rval);
}

template <>
float ruby2c<float> (VALUE rval)
{
  return float (NUM2DBL (rval));
}

template <>
std::string ruby2c<std::string> (VALUE rval)
{
  VALUE str = rba_safe_string_value (rval);
  return std::string (RSTRING_PTR(str), RSTRING_LEN(str));
}

template <>
QByteArray ruby2c<QByteArray> (VALUE rval)
{
  VALUE str = rba_safe_string_value (rval);
  return QByteArray (RSTRING_PTR(str), RSTRING_LEN(str));
}

template <>
QString ruby2c<QString> (VALUE rval)
{
  VALUE str = rba_safe_string_value (rval);
  return tl::to_qstring (std::string (RSTRING_PTR(str), RSTRING_LEN(str)));
}

template <>
void *ruby2c<void *> (VALUE rval)
{
  return (void *) ruby2c<size_t> (rval);
}

template <>
const char *ruby2c<const char *> (VALUE rval)
{
  VALUE str = rba_safe_string_value (rval);
  return RSTRING_PTR(str);
}

template <>
tl::Variant ruby2c<tl::Variant> (VALUE rval)
{
  if (FIXNUM_P (rval)) {
    return tl::Variant (ruby2c<long> (rval));
  } else if (rval == Qnil) {
    return tl::Variant ();
  } else if (rval == Qfalse) {
    return tl::Variant (false);
  } else if (rval == Qtrue) {
    return tl::Variant (true);
  } else if (TYPE (rval) == T_BIGNUM) {
    return tl::Variant (ruby2c<long long> (rval));
  } else if (TYPE (rval) == T_FLOAT) {
    return tl::Variant (ruby2c<double> (rval));
  } else if (TYPE (rval) == T_ARRAY) {

    unsigned int len = RARRAY_LEN(rval);
    VALUE *el = RARRAY_PTR(rval);

    static std::vector<tl::Variant> empty;
    tl::Variant r (empty.begin (), empty.end ());
    r.get_list ().reserve (len);
    while (len-- > 0) {
      r.get_list ().push_back (ruby2c<tl::Variant> (*el++));
    }
    return r;

  } else if (TYPE (rval) == T_DATA) {

    //  some types are supported through "complex" tl::Variant's
    Proxy *p;
    Data_Get_Struct (rval, Proxy, p);

    //  employ the tl::Variant binding capabilities of the Expression binding to derive the 
    //  variant value. Currently we use deep copy mode always since any other solution would
    //  involve reference management.
    return tl::Variant (0, p->cls_decl ()->clone (p->obj (rval)), p->cls_decl ()->var_cls (false, false));

  } else {
    return tl::Variant (ruby2c<const char *> (rval));
  }
}

template <>
gsi::ObjectType ruby2c<gsi::ObjectType> (VALUE)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract object type written")));
}

template <>
gsi::VectorType ruby2c<gsi::VectorType> (VALUE)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract vector type written")));
}

template <>
gsi::MapType ruby2c<gsi::MapType> (VALUE)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract map type written")));
}

template <>
gsi::VoidType ruby2c<gsi::VoidType> (VALUE)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract void type written")));
}

// -------------------------------------------------------------------
//  Return the boxed value pointer for a given basic type from the reference

template <class R>
struct get_boxed_value_func
{
  void operator() (void **value, VALUE arg, tl::Heap *heap)
  { 
    if (TYPE (arg) != T_DATA) {

      R *v = new R (ruby2c<R> (arg));
      heap->push (v);
      *value = v;

    } else {

      const gsi::ClassBase *bt = gsi::cls_decl <gsi::Value> ();

      Proxy *p;
      Data_Get_Struct (arg, Proxy, p);
      if (!p->cls_decl ()->is_derived_from (bt)) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Passing an object to pointer or reference requires a boxed type (RBA::%s)")), bt->name ()));
      }

      gsi::Value *bo = reinterpret_cast<gsi::Value *> (p->obj (arg));
      if (bo) {
        *value = bo->value ().template morph<R> ().native_ptr ();
      }

    }
  }
};

struct get_boxed_value_func_error
{
  void operator() (void **value, VALUE arg, tl::Heap *heap)
  { 
    tl_assert (false);
  }
};

template <> struct get_boxed_value_func<gsi::ObjectType> : get_boxed_value_func_error { };
template <> struct get_boxed_value_func<gsi::VectorType> : get_boxed_value_func_error { };
template <> struct get_boxed_value_func<gsi::MapType> : get_boxed_value_func_error { };
template <> struct get_boxed_value_func<gsi::VoidType> : get_boxed_value_func_error { };

void *boxed_value_ptr (gsi::BasicType type, VALUE arg, tl::Heap &heap)
{
  void *ret = 0;
  gsi::do_on_type<get_boxed_value_func> () (type, &ret, arg, &heap);
  return ret;
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
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Qnil) {

      if (atype.is_ref () || atype.is_cref ()) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference type cannot be passed nil")));
      } else if (atype.is_ptr ()) {
        aa->write<R *> ((R *)0);
      } else if (atype.is_cptr ()) {
        aa->write<const R *> ((const R *)0);
      } else {
        aa->write<R> ((R)0);
      }

    } else {

      if (atype.is_ref () || atype.is_ptr ()) {
        // references or pointers require a boxed object. Pointers also allow nil.
        void *vc = boxed_value_ptr (atype.type (), arg, *heap);
        if (! vc && atype.is_ref ()) {
          throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference or direct type cannot be passed nil or an empty boxed value object")));
        }
        aa->write<void *> (vc);
      } else if (atype.is_cref ()) {
        aa->write<const R &> (ruby2c<R> (arg));
      } else if (atype.is_cptr ()) {
        R r = ruby2c<R> (arg);
        aa->write<const R *> (&r);
      } else {
        aa->write<R> (ruby2c<R> (arg));
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
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Qnil) {

      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference or direct type cannot be passed nil")));
      } else if (atype.is_ptr ()) {
        aa->write<R *> ((R *)0);
      } else {
        aa->write<const R *> ((const R *)0);
      }

    } else {

      if (atype.is_ref () || atype.is_ptr ()) {
        // references or pointers require a boxed object. Pointers also allow nil.
        void *vc = boxed_value_ptr (atype.type (), arg, *heap);
        if (! vc && atype.is_ref ()) {
          throw tl::Exception (tl::to_string (QObject::tr ("Arguments or return values of reference or direct type cannot be passed nil or an empty boxed value object")));
        }
        aa->write<void *> (vc);

      } else if (atype.is_cref () || atype.is_cptr ()) {

        R *v = new R (ruby2c<R> (arg));
        heap->push (v);

        if (atype.is_cref ()) {
          aa->write<const R &> (*v);
        } else if (atype.is_cptr ()) {
          aa->write<const R *> (v);
        }

      } else {
        aa->write<R> (ruby2c<R> (arg));
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
 *
 *  Specialisation for variants 
 */
template <>
struct writer <tl::Variant>
{
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Qnil && (atype.is_ptr () || atype.is_cptr ())) {

      if (atype.is_ptr ()) {
        aa->write<tl::Variant *> ((tl::Variant *)0);
      } else {
        aa->write<const tl::Variant *> ((const tl::Variant *)0);
      }

    } else {

      if (atype.is_cref () || atype.is_ref () || atype.is_cptr () || atype.is_ptr ()) {

        tl::Variant *v = new tl::Variant (ruby2c<tl::Variant> (arg));
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
        aa->write<tl::Variant> (ruby2c<tl::Variant> (arg));
      }

    }
  }
};

template <class R>
struct vector_writer
{
  void operator() (gsi::SerialArgs *aa, VALUE arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (TYPE (arr) != T_ARRAY) {
      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Expected array either as argument or return type"))));
    }
    unsigned int len = RARRAY_LEN(arr);
    VALUE *el = RARRAY_PTR(arr);

    if (atype.is_cref () || atype.is_ref () || atype.is_cptr () || atype.is_ptr ()) {

      std::vector<R> *v = new std::vector<R> ();
      v->reserve (len);
      while (len-- > 0) {
        v->push_back (ruby2c<R> (*el++));
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
      aa->write<std::vector<R> > (std::vector<R> ());
      v->reserve (len);
      while (len-- > 0) {
        v->push_back (ruby2c<R> (*el++));
      }
    }
  }
};

template <>
struct vector_writer<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *aa, VALUE arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    const gsi::ArgType &ainner = *atype.inner ();

    unsigned int len = RARRAY_LEN(arr);
    VALUE *el = RARRAY_PTR(arr);

    std::vector<void *> vobj;
    vobj.reserve (len);
    while (len-- > 0) {
      Proxy *p;
      if (TYPE (*el) != T_DATA) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type inside array (expected argument of class %s)")), ainner.cls ()->name ()));
      }
      Data_Get_Struct (*el, Proxy, p);
      if (!p->cls_decl ()->is_derived_from (ainner.cls ())) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type inside array (expected argument of class %s)")), ainner.cls ()->name ()));
      }
      vobj.push_back (p->obj (*el));
      ++el;
    }

    ainner.cls ()->push_vector_of (*aa, atype, *heap, vobj);

  }
};

struct vector_writer_error
{
  void operator() (gsi::SerialArgs *aa, VALUE arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    tl_assert (false);
  }
};

template <> struct vector_writer<gsi::VectorType> : vector_writer_error { };
template <> struct vector_writer<gsi::MapType> : vector_writer_error { };

/**
 *  @brief A specialisation of the write function for vector types
 */
template <>
struct writer <gsi::VectorType>
{
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  nil is an allowed value for ptr's
    if ((atype.is_cptr () || atype.is_ptr ()) && arg == Qnil) {
      aa->write<void *> (0);
    } else {

      if (TYPE (arg) != T_ARRAY) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Expected array either as argument or return type"))));
      }

      tl_assert (atype.inner () != 0);
      const gsi::ArgType &ainner = *atype.inner ();

      gsi::do_on_type<vector_writer> () (ainner.type (), aa, arg, atype, heap);

    }
  }
};

static int push_map_i (VALUE key, VALUE value, VALUE arg)
{
  std::vector<std::pair<VALUE, VALUE> > *v = (std::vector<std::pair<VALUE, VALUE> > *) arg;
  v->push_back (std::make_pair (key, value));
  return ST_CONTINUE;
}

/**
 *  @brief A serialization wrapper for maps (write mode)
 *
 *  Specialisation for map of any kind
 */
template <class X, class Y>
struct map_writer
{
  typedef std::map<X, Y> map_rep_type;

  void operator() (gsi::SerialArgs *aa, VALUE hash, const gsi::ArgType &atype, tl::Heap *heap)
  {
    map_rep_type *map;

    if (atype.is_cref () || atype.is_ref () || atype.is_cptr () || atype.is_ptr ()) {
      map = new map_rep_type ();
      heap->push (map);
    } else {
      map = reinterpret_cast<map_rep_type *> (aa->wptr ());
      aa->write<map_rep_type> (map_rep_type ());
    }

    std::vector<std::pair<VALUE, VALUE> > kv;
    kv.reserve (RHASH_SIZE (hash));
    //  Note: we can't directly fill the map inside push_map_i. When an exception is
    //  thrown there, we get an abort with "longjmp causes uninitialized stack frame"
    rb_hash_foreach (hash, (int (*)(...)) &push_map_i, (VALUE) &kv);

    for (std::vector<std::pair<VALUE, VALUE> >::const_iterator i = kv.begin (); i != kv.end (); ++i) {
      map->insert (std::make_pair (ruby2c<X> (i->first), ruby2c<Y> (i->second)));
    }

    if (atype.is_ref ()) {
      aa->write<map_rep_type &> (*map);
    } else if (atype.is_cref ()) {
      aa->write<const map_rep_type &> (*map);
    } else if (atype.is_ptr ()) {
      aa->write<map_rep_type *> (map);
    } else if (atype.is_cptr ()) {
      aa->write<const map_rep_type *> (map);
    }
  }
};

struct map_writer_error
{
  void operator() (gsi::SerialArgs *aa, VALUE arr, const gsi::ArgType &atype, tl::Heap *heap)
  {
    tl_assert (false);
  }
};

template <class Y> struct map_writer<gsi::ObjectType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::VectorType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::MapType, Y> : map_writer_error { };
template <class Y> struct map_writer<gsi::VoidType, Y> : map_writer_error { };

/**
 *  @brief A specialisation of the write function for vector types
 */
template <>
struct writer <gsi::MapType>
{
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  nil is an allowed value for ptr's
    if ((atype.is_cptr () || atype.is_ptr ()) && arg == Qnil) {
      aa->write<void *> (0);
    } else {

      if (TYPE (arg) != T_HASH) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Expected hash either as argument or return type"))));
      }

      tl_assert (atype.inner () != 0);
      tl_assert (atype.inner_k () != 0);
      const gsi::ArgType &ainner = *atype.inner ();
      const gsi::ArgType &ainner_k = *atype.inner_k ();

      //  type handled in switch
      gsi::do_on_type2<map_writer> () (ainner_k.type (), ainner.type (), aa, arg, atype, heap);

    }
  }
};

/**
 *  @brief A specialisation of the write function for vector types
 */
template <>
struct writer <gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    if (arg == Qnil) {
      if (! (atype.is_ptr () || atype.is_cptr ())) {
        throw tl::Exception (tl::to_string (QObject::tr ("Arguments of reference or direct type cannot be passed nil")));
      } else {
        aa->write<void *> ((void *) 0);
      }
    } else if (atype.is_ptr () || atype.is_cptr () || atype.is_ref () || atype.is_cref ()) {
      Proxy *p = 0;
      if (TYPE (arg) == T_DATA) {
        Data_Get_Struct (arg, Proxy, p);
        if (! p->cls_decl ()->is_derived_from (atype.cls ())) {
          p = 0;
        }
      }
      if (! p) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s, got %s)")), atype.cls ()->name (), rb_class_name (arg).c_str ()));
      }
      if (p->const_ref () && (atype.is_ptr () || atype.is_ref ())) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Cannot pass a const reference of class %s to a non-const reference parameter")), atype.cls ()->name ()));
      }
      aa->write<void *> (p->obj (arg));
    } else {
      Proxy *p = 0;
      if (TYPE (arg) == T_DATA) {
        Data_Get_Struct (arg, Proxy, p);
        if (! p->cls_decl ()->is_derived_from (atype.cls ())) {
          p = 0;
        }
      }
      if (! p) {
        throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Unexpected object type (expected argument of class %s, got %s)")), atype.cls ()->name (), rb_class_name (arg).c_str ()));
      }
      atype.cls ()->copy_to (*aa, p->obj (arg));
    }
  }
};

/**
 *  @brief T_void is ignored on writing
 */
template <>
struct writer <gsi::VoidType>
{
  void operator() (gsi::SerialArgs *aa, VALUE arg, const gsi::ArgType &atype, tl::Heap *heap)
  {
    //  .. nothing: void is not serialized
  }
};

// -------------------------------------------------------------------
//  Push an argument on the call or return stack

inline void
push_arg (const gsi::ArgType &atype, gsi::SerialArgs &aserial, VALUE arg, tl::Heap &heap)
{
  gsi::do_on_type<writer> () (atype.type (), &aserial, arg, atype, &heap);
}
      
// -------------------------------------------------------------------
//  Correct constness if a reference is const and a non-const reference is required

//  HINT: this is a workaround for the fact that unlike C++, Ruby does not have const or non-const
//  references. Since a reference is identical with the object it points to, there are only const or non-const
//  objects. We deliver const objects first, but if a non-const version is requestes, the
//  object turns into a non-const one. This may be confusing but provides a certain level
//  of "constness", at least until there is another non-const reference for that object.

void correct_constness (VALUE obj, bool const_required)
{
  Proxy *p;
  Data_Get_Struct (obj, Proxy, p);
  if (p && p->const_ref () && ! const_required) {
    //  promote to non-const object
    p->set_const_ref (false);
  }
}

// -------------------------------------------------------------------
//  C to Ruby conversion

template <class R>
VALUE c2ruby (const R &r);

template <>
VALUE c2ruby<bool> (const bool &c)
{
  return c ? Qtrue : Qfalse;
}

template <>
VALUE c2ruby<char> (const char &c)
{
  return INT2NUM (c);
}

template <>
VALUE c2ruby<signed char> (const signed char &c)
{
  return UINT2NUM (c);
}

template <>
VALUE c2ruby<unsigned char> (const unsigned char &c)
{
  return UINT2NUM (c);
}

template <>
VALUE c2ruby<short> (const short &c)
{
  return INT2NUM (c);
}

template <>
VALUE c2ruby<unsigned short> (const unsigned short &c)
{
  return UINT2NUM (c);
}

template <>
VALUE c2ruby<int> (const int &c)
{
  return INT2NUM (c);
}

template <>
VALUE c2ruby<unsigned int> (const unsigned int &c)
{
  return UINT2NUM (c);
}

template <>
VALUE c2ruby<long> (const long &c)
{
  return LONG2NUM (c);
}

template <>
VALUE c2ruby<unsigned long> (const unsigned long &c)
{
  return ULONG2NUM (c);
}

template <>
VALUE c2ruby<long long> (const long long &c)
{
  return LL2NUM (c);
}

template <>
VALUE c2ruby<unsigned long long> (const unsigned long long &c)
{
  return ULL2NUM (c);
}

#if defined(HAVE_64BIT_COORD)
template <>
VALUE c2ruby<__int128> (const __int128 &c)
{
  // TOOD: this is pretty simplistic
  return rb_float_new (double (c));
}
#endif

template <>
VALUE c2ruby<double> (const double &c)
{
  return rb_float_new (c);
}

template <>
VALUE c2ruby<float> (const float &c)
{
  return rb_float_new (c);
}

template <>
VALUE c2ruby<std::string> (const std::string &c)
{
  return rb_str_new (c.c_str (), c.size ());
}

template <>
VALUE c2ruby<QByteArray> (const QByteArray &qba)
{
  if (qba.isNull ()) {
    return Qnil;
  } else {
    return rb_str_new (qba.constData (), qba.size ());
  }
}

template <>
VALUE c2ruby<QString> (const QString &qs)
{
  if (qs.isNull ()) {
    return Qnil;
  } else {
    std::string c (tl::to_string (qs));
    return rb_str_new (c.c_str (), c.size ());
  }
}

template <>
VALUE c2ruby<void *> (void * const &s)
{
  return c2ruby<size_t> (size_t (s));
}

template <>
VALUE c2ruby<tl::Variant> (const tl::Variant &c)
{
  if (c.is_double ()) {
    return c2ruby<double> (c.to_double ());
  } else if (c.is_bool ()) {
    return c2ruby<bool> (c.to_bool ());
  } else if (c.is_a_string ()) {
    return c2ruby<std::string> (c.to_string ());
  } else if (c.is_long () || c.is_char ()) {
    return c2ruby<long> (c.to_long ());
  } else if (c.is_ulong ()) {
    return c2ruby<unsigned long> (c.to_ulong ());
  } else if (c.is_longlong ()) {
    return c2ruby<long long> (c.to_longlong ());
  } else if (c.is_ulonglong ()) {
    return c2ruby<unsigned long long> (c.to_ulonglong ());
  } else if (c.is_list ()) {
    VALUE ret = rb_ary_new ();
    for (std::vector<tl::Variant>::const_iterator i = c.get_list ().begin (); i != c.get_list ().end (); ++i) {
      rb_ary_push (ret, c2ruby<tl::Variant> (*i));
    }
    return ret;
  } else if (c.is_user ()) {

    const gsi::ClassBase *cls = c.gsi_cls ();
    if (cls) {

      void *obj = cls->clone (c.to_user_object ());

      std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (cls);
      tl_assert (c != rev_cls_map.end ());
      VALUE ret = rb_class_new_instance_checked (0, 0, c->second);
      Proxy *p;
      Data_Get_Struct (ret, Proxy, p);
      p->set (obj, true, false, false, ret);
      return ret;

    } else {
      //  not a known type -> return nil
      return Qnil;
    }

  } else {
    return Qnil;
  }
}

template <>
VALUE c2ruby<const char *> (const char * const & s)
{
  if (! s) {
    static const char null_string[] = "(null)";
    return rb_str_new (null_string, sizeof (null_string) - 1);
  } else {
    return rb_str_new (s, strlen (s));
  }
}

template <>
VALUE c2ruby<gsi::VoidType> (const gsi::VoidType &)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract void type read")));
}

template <>
VALUE c2ruby<gsi::ObjectType> (const gsi::ObjectType &)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract object type read")));
}

template <>
VALUE c2ruby<gsi::VectorType> (const gsi::VectorType &)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract vector type read")));
}

template <>
VALUE c2ruby<gsi::MapType> (const gsi::MapType &)
{
  throw tl::Exception (tl::to_string (QObject::tr ("Internal error: abstract map type read")));
}

/**
 *  @brief Deseralisation wrapper 
 *
 *  The default implementation is for POD types, strings and variants
 */
template <class R> 
struct reader
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &arg)
  {
    if (arg.is_ref ()) {
      *ret = c2ruby<R> (rr->template read<R &> ());
    } else if (arg.is_cref ()) {
      *ret = c2ruby<R> (rr->template read<const R &> ());
    } else if (arg.is_ptr ()) {
      R *p = rr->template read<R *> ();
      if (p) {
        *ret = c2ruby<R> (*p);
      } else {
        *ret = Qnil;
      }
    } else if (arg.is_cptr ()) {
      const R *p = rr->template read<const R *> ();
      if (p) {
        *ret = c2ruby<R> (*p);
      } else {
        *ret = Qnil;
      }
    } else {
      *ret = c2ruby<R> (rr->template read<R> ());
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
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &arg)
  {
    tl_assert (! arg.is_cref ());
    tl_assert (! arg.is_ref ());
    tl_assert (! arg.is_cptr ());
    tl_assert (! arg.is_ptr ());
    *ret = c2ruby<void *> (rr->read<void *> ());
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
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &arg)
  {
    tl_assert (! arg.is_cref ());
    tl_assert (! arg.is_ref ());
    tl_assert (! arg.is_cptr ());
    tl_assert (! arg.is_ptr ());
    *ret = c2ruby<const char *> (rr->read<const char *> ());
  }
};

template <class R> 
struct vector_reader
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &arg)
  {
    if (arg.is_ref ()) {

      std::vector<R> &v = rr->template read<std::vector<R> &> ();

      *ret = rb_ary_new ();
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        rb_ary_push (*ret, c2ruby<R> (*i));
      }

    } else if (arg.is_cref ()) {

      const std::vector<R> &v = rr->template read<const std::vector<R> &> ();

      *ret = rb_ary_new ();
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        rb_ary_push (*ret, c2ruby<R> (*i));
      }

    } else if (arg.is_ptr ()) {

      std::vector<R> *v = rr->template read<std::vector<R> *> ();
      if (! v) {
        *ret = Qnil;
      } else {

        *ret = rb_ary_new ();
        for (typename std::vector<R>::const_iterator i = v->begin (); i != v->end (); ++i) {
          rb_ary_push (*ret, c2ruby<R> (*i));
        }

      }

    } else if (arg.is_cptr ()) {

      const std::vector<R> *v = rr->template read<const std::vector<R> *> ();
      if (! v) {
        *ret = Qnil;
      } else {

        *ret = rb_ary_new ();
        for (typename std::vector<R>::const_iterator i = v->begin (); i != v->end (); ++i) {
          rb_ary_push (*ret, c2ruby<R> (*i));
        }

      }

    } else {

      std::vector<R> v = rr->template read< std::vector<R> > ();

      *ret = rb_ary_new ();
      for (typename std::vector<R>::const_iterator i = v.begin (); i != v.end (); ++i) {
        rb_ary_push (*ret, c2ruby<R> (*i));
      }

    }
  }
};

struct ArrayData
{
  ArrayData (VALUE _ret, const gsi::ArgType &_ainner)
    : ret (_ret), ainner (_ainner)
  { }

  VALUE ret;
  const gsi::ArgType &ainner;
};

static void
make_array_f (void *data, void *cobj)
{
  ArrayData *ad = (ArrayData *)data;
  
  const gsi::ClassBase *clsact = ad->ainner.cls ()->subclass_decl (cobj);
  std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (clsact);
  tl_assert (c != rev_cls_map.end ());

  VALUE obj = Qnil;

  if (ad->ainner.is_ptr () || ad->ainner.is_cptr ()) {

    if (cobj) {

      if (clsact->client_data (cobj, client_index) != 0 && 
          ((RBAClientData *) clsact->client_data (cobj, client_index))->get_ref () != Qnil) {

        //  we have a that is located in C++ space but is supposed to get attached
        //  a Ruby object. If it already has, we simply return a reference to this
        obj = ((RBAClientData *) clsact->client_data (cobj, client_index))->get_ref ();

        //  correct constness if the object is not supposed to be const
        correct_constness (obj, ad->ainner.is_cptr () /*const obj*/);

      } else {

        obj = rb_class_new_instance_checked (0, 0, c->second);
        Proxy *p;
        Data_Get_Struct (obj, Proxy, p);

        p->set (cobj, false, ad->ainner.is_cptr () /*const obj*/, ad->ainner.is_ptr () /*can destroy*/, obj);

      }

    }

  } else {

    //  create a instance without attaching it
    obj = rb_class_new_instance_checked (0, 0, c->second);
    Proxy *p;
    Data_Get_Struct (obj, Proxy, p);
    p->set (cobj, true, false, false, obj);

  }

  rb_ary_push (ad->ret, obj);
}

template <> 
struct vector_reader<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    *ret = rb_ary_new ();

    const gsi::ArgType &ainner = *atype.inner ();
    ArrayData ad (*ret, ainner);
    ainner.cls ()->get_vector_of (*rr, atype, (void *) &ad, &make_array_f);
  }
};

struct vector_reader_error
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  { 
    tl_assert (false);
  }
};

template <> struct vector_reader <gsi::VectorType> : vector_reader_error { };
template <> struct vector_reader <gsi::MapType> : vector_reader_error { };

template <> 
struct reader<gsi::VectorType>
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    const gsi::ArgType &ainner = *atype.inner ();
    gsi::do_on_type<vector_reader> () (ainner.type (), rr, ret, atype);
  }
};

template <class X, class Y> 
struct map_reader
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    typedef std::map<X, Y> map_rep_type;

    if (atype.is_ref ()) {

      map_rep_type &v = rr->template read<map_rep_type &> ();

      *ret = rb_hash_new ();
      for (typename map_rep_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        rb_hash_aset (*ret, c2ruby<X> (i->first), c2ruby<Y> (i->second));
      }

    } else if (atype.is_cref ()) {

      const map_rep_type &v = rr->template read<const map_rep_type &> ();

      *ret = rb_hash_new ();
      for (typename map_rep_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        rb_hash_aset (*ret, c2ruby<X> (i->first), c2ruby<Y> (i->second));
      }

    } else if (atype.is_ptr ()) {

      map_rep_type *v = rr->template read<map_rep_type *> ();
      if (! v) {
        *ret = Qnil;
      } else {

        *ret = rb_hash_new ();
        for (typename map_rep_type::const_iterator i = v->begin (); i != v->end (); ++i) {
          rb_hash_aset (*ret, c2ruby<X> (i->first), c2ruby<Y> (i->second));
        }

      }

    } else if (atype.is_cptr ()) {

      const map_rep_type *v = rr->template read<const map_rep_type *> ();
      if (! v) {
        *ret = Qnil;
      } else {

        *ret = rb_hash_new ();
        for (typename map_rep_type::const_iterator i = v->begin (); i != v->end (); ++i) {
          rb_hash_aset (*ret, c2ruby<X> (i->first), c2ruby<Y> (i->second));
        }

      }

    } else {

      map_rep_type v = rr->template read< map_rep_type > ();

      *ret = rb_hash_new ();
      for (typename map_rep_type::const_iterator i = v.begin (); i != v.end (); ++i) {
        rb_hash_aset (*ret, c2ruby<X> (i->first), c2ruby<Y> (i->second));
      }

    }
  }
};

struct map_reader_error
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    tl_assert (false);
  }
};

template <class Y> struct map_reader<gsi::ObjectType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::VectorType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::MapType, Y> : map_reader_error { };
template <class Y> struct map_reader<gsi::VoidType, Y> : map_reader_error { };

template <> 
struct reader<gsi::MapType>
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    tl_assert (atype.inner () != 0);
    tl_assert (atype.inner_k () != 0);
    const gsi::ArgType &ainner = *atype.inner ();
    const gsi::ArgType &ainner_k = *atype.inner_k ();

    gsi::do_on_type2<map_reader> () (ainner_k.type (), ainner.type (), rr, ret, atype);
  }
};

template <> 
struct reader<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    if (atype.is_ptr () || atype.is_ref () || atype.is_cptr () || atype.is_cref ()) {

      void *obj = rr->read<void *> ();
      const gsi::ClassBase *clsact = atype.cls ()->subclass_decl (obj);

      if (obj == 0) {

        *ret = Qnil;

      } else if (atype.pass_obj ()) {

        std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (atype.cls ());
        tl_assert (c != rev_cls_map.end ());

        //  The ownership is passed to the caller (constructor case):
        //  create a new managed instance 
        *ret = rb_class_new_instance_checked (0, 0, c->second);
        Proxy *p;
        Data_Get_Struct (*ret, Proxy, p);
        p->set (obj, true, false, false, *ret);

      } else if (atype.is_cref () && clsact->can_copy () && clsact->client_data (obj, client_index) == 0) {

        //  We copy objects passed by const reference if they are not managed
        //  (derived from gsi::ObjectBase, in that case, client_data is not 0).
        //  Such objects are often exposed internals. First we can't
        //  guarantee the const correctness of references. Second, we
        //  can't guarantee the lifetime of the container will exceed that
        //  of the exposed property. Hence copying is safer.

        std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (clsact);
        tl_assert (c != rev_cls_map.end ());

        //  create a instance and copy the value
        *ret = rb_class_new_instance_checked (0, 0, c->second);
        Proxy *p;
        Data_Get_Struct (*ret, Proxy, p);
        clsact->assign (p->obj (*ret), obj);

      } else if (clsact->client_data (obj, client_index) != 0 && 
                 ((RBAClientData *) clsact->client_data (obj, client_index))->get_ref () != Qnil) {

        //  we have a that is located in C++ space but is supposed to get attached
        //  a Ruby object. If it already has, we simply return a reference to this
        *ret = ((RBAClientData *) clsact->client_data (obj, client_index))->get_ref ();

        //  correct constness if the object is not supposed to be const
        correct_constness (*ret, atype.is_cref () || atype.is_cptr ());

      } else {

        std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (clsact);
        tl_assert (c != rev_cls_map.end ());

        //  create a instance 
        //  TODO: we will create a fresh object here, delete it again and link the 
        //  reference to the exisiting object to the Ruby object. This is not quite
        //  efficient - we should avoid creating and deleting a dummy object first.
        *ret = rb_class_new_instance_checked (0, 0, c->second);
        Proxy *p;
        Data_Get_Struct (*ret, Proxy, p);
        p->set (obj, false, atype.is_cref () || atype.is_cptr (), atype.is_ptr (), *ret);

      }

    } else {

      std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (atype.cls ());
      tl_assert (c != rev_cls_map.end ());
      //  create a instance without attaching it
      *ret = rb_class_new_instance_checked (0, 0, c->second);
      Proxy *p;
      Data_Get_Struct (*ret, Proxy, p);
      p->set (atype.cls ()->take_and_create (*rr), true, false, false, *ret);

    }
  }
};

template <> 
struct reader<gsi::VoidType>
{
  void operator() (gsi::SerialArgs *rr, VALUE *ret, const gsi::ArgType &atype)
  {
    //  .. nothing: void is not serialized
  }
};

// -------------------------------------------------------------------
//  Pop an argument from the call or return stack

VALUE
pop_arg (const gsi::ArgType &atype, gsi::SerialArgs &aserial)
{
  tl_assert (! atype.is_iter ()); // not implemented here 

  VALUE ret = Qnil;
  gsi::do_on_type<reader> () (atype.type (), &aserial, &ret, atype);
  return ret;
}

// -------------------------------------------------------------------
//  Test if an argument can be converted to the given type

//  if atype is a vector:
//      argument must be an array of the given type
//  if atype is a ref:
//      argument must be a boxed type of the required type or an object of the requested class
//  if atype is a ptr:
//      argument must be a boxed type of the required type or an object of the requested class or nil
//  if atype is a cptr:
//      argument must be of requested type or nil
//  otherwise:
//      argument must be of the requested type

bool
test_arg (const gsi::ArgType &atype, VALUE arg, bool loose);

template <class R>
struct test_arg_func
{
  void operator () (bool *ret, VALUE arg, const gsi::ArgType &atype, bool loose)
  {
    if ((atype.is_cptr () || atype.is_ptr ()) && arg == Qnil) {

      //  for const X * or X *, nil is an allowed value
      *ret = true;

    } else {

      *ret = false;

      if (atype.is_ptr () || atype.is_ref ()) {

        //  check if we have a boxed type
        if (TYPE (arg) == T_DATA) {
          const gsi::ClassBase *bc = gsi::cls_decl <gsi::Value> ();
          Proxy *p;
          Data_Get_Struct (arg, Proxy, p);
          if (p->cls_decl ()->is_derived_from (bc)) {
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

/**
 *  @brief Test a VALUE for compatibility with a vector of the given type R
 */
template <class R>
struct test_vector
{
  void operator() (bool *ret, VALUE arr, const gsi::ArgType &ainner, bool loose)
  {
    *ret = true;

    unsigned int len = RARRAY_LEN(arr);
    VALUE *el = RARRAY_PTR(arr);
    while (len-- > 0) {
      if (! test_arg (ainner, *el++, loose)) {
        *ret = false;
        break;
      }
    }
  }
};

template <>
struct test_arg_func<gsi::VectorType>
{
  void operator () (bool *ret, VALUE arg, const gsi::ArgType &atype, bool loose)
  {
    if (TYPE (arg) != T_ARRAY) {
      *ret = false;
    } else {

      tl_assert (atype.inner () != 0);
      const gsi::ArgType &ainner = *atype.inner ();

      gsi::do_on_type<test_vector> () (ainner.type (), ret, arg, ainner, loose);

    }
  }
};

struct HashTestKeyValueData
{
  const gsi::ArgType *ainner_k;
  const gsi::ArgType *ainner;
  bool *ret;
  bool loose;
};

static int hash_test_value_key (VALUE key, VALUE value, VALUE a)
{
  HashTestKeyValueData *args = (HashTestKeyValueData *)a;
  if (! test_arg (*args->ainner_k, key, args->loose)) {
    *(args->ret) = false;
    return ST_STOP;
  }
  if (! test_arg (*args->ainner, value, args->loose)) {
    *(args->ret) = false;
    return ST_STOP;
  }
  return ST_CONTINUE;
}

template <>
struct test_arg_func<gsi::MapType>
{
  void operator () (bool *ret, VALUE arg, const gsi::ArgType &atype, bool loose)
  {
    if (TYPE (arg) != T_HASH) {
      *ret = false;
    } else {

      tl_assert (atype.inner () != 0);
      tl_assert (atype.inner_k () != 0);

      HashTestKeyValueData args;
      args.ainner_k = atype.inner_k ();
      args.ainner = atype.inner ();
      args.ret = ret;
      args.loose = loose;

      *ret = true;
      rb_hash_foreach (arg, (int (*)(...)) &hash_test_value_key, (VALUE) &args);

    }
  }
};

template <>
struct test_arg_func<gsi::ObjectType>
{
  void operator () (bool *ret, VALUE arg, const gsi::ArgType &atype, bool loose)
  {
    if ((atype.is_cptr () || atype.is_ptr ()) && arg == Qnil) {

      //  for const X * or X *, nil is an allowed value
      *ret = true;

    } else {

      *ret = (TYPE (arg) == T_DATA);

      if (*ret) {

        //  additionally check, whether the object matches the class type
        Proxy *p;
        Data_Get_Struct (arg, Proxy, p);
        if (!p->cls_decl ()->is_derived_from (atype.cls ())) {
          *ret = false;
        } else {
          //  check constness
          if ((atype.is_ref () || atype.is_ptr ()) && p->const_ref ()) {
            *ret = false;
          }
        }

      }

    }

  }
};

bool
test_arg (const gsi::ArgType &atype, VALUE arg, bool loose)
{
  bool ret = false;
  gsi::do_on_type<test_arg_func> () (atype.type (), &ret, arg, atype, loose);
  return ret;
}
      
// -------------------------------------------------------------------
//  "each" implementation for iterators

template <class R>
struct yield_iter
{
  void operator() (gsi::SerialArgs *rr, const gsi::ArgType &)
  {
    gsi::IterAdaptorAbstractBase *iter = (gsi::IterAdaptorAbstractBase *) rr->template read<gsi::IterAdaptorAbstractBase *> ();
    try {
      while (! iter->at_end ()) {
        rb_yield (c2ruby<R> (*((const R *) iter->v_get ())));
        iter->inc ();
      }
      delete iter;
    } catch (...) {
      delete iter;
      throw;
    }
  }
};

template <>
struct yield_iter<gsi::ObjectType>
{
  void operator() (gsi::SerialArgs *rr, const gsi::ArgType &ret_type)
  {
    const gsi::ClassBase *cls = ret_type.cls ();
    std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (cls);
    tl_assert (c != rev_cls_map.end ());

    if (ret_type.is_ref () || ret_type.is_cref () || ret_type.is_ptr () || ret_type.is_cptr ()) {

      void *iter = c->first->create_obj_iterator (*rr);

      try {

        while (! c->first->obj_iterator_at_end (iter)) {

          void *cobj = const_cast<void *> (c->first->get_iterator_obj (iter));

          //  specialize the class if possible
          const gsi::ClassBase *cls_act = cls->subclass_decl (cobj);

          VALUE obj;
          if (cobj == 0) {

            obj = Qnil;

          } else if (ret_type.pass_obj ()) {

            std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (ret_type.cls ());
            tl_assert (c != rev_cls_map.end ());

            //  The ownership is passed to the caller (constructor case):
            //  create a new managed instance 
            obj = rb_class_new_instance_checked (0, 0, c->second);
            Proxy *p;
            Data_Get_Struct (obj, Proxy, p);
            p->set (cobj, true, false, false, obj);

          } else if (ret_type.is_cref () && cls_act->can_copy () && cls_act->client_data (cobj, client_index) == 0) {

            //  We copy objects passed by const reference if they are not managed
            //  (derived from gsi::ObjectBase, in that case, client_data is not 0).
            //  Such objects are often exposed internals. First we can't
            //  guarantee the const correctness of references. Second, we
            //  can't guarantee the lifetime of the container will exceed that
            //  of the exposed property. Hence copying is safer.

            std::map <const gsi::ClassBase *, VALUE>::const_iterator c = rev_cls_map.find (cls_act);
            tl_assert (c != rev_cls_map.end ());

            //  create a instance and copy the value
            obj = rb_class_new_instance_checked (0, 0, c->second);
            Proxy *p;
            Data_Get_Struct (obj, Proxy, p);
            cls_act->assign (p->obj (obj), cobj);

          } else if (cls_act->client_data (cobj, client_index) != 0 && 
                     ((RBAClientData *) cls_act->client_data (cobj, client_index))->get_ref () != Qnil) {

            //  we have a that is located in C++ space but is supposed to get attached
            //  a Ruby object. If it already has, we simply return a reference to this
            obj = ((RBAClientData *) cls_act->client_data (cobj, client_index))->get_ref ();

            //  correct constness if the object is not supposed to be const
            correct_constness (obj, ret_type.is_cref () || ret_type.is_cptr ());

          } else {

            //  the object returned can be of a derived class. Hence we need to look up the class again
            std::map <const gsi::ClassBase *, VALUE>::const_iterator cact = rev_cls_map.find (cls_act);
            tl_assert (cact != rev_cls_map.end ());

            //  create the object
            obj = rb_class_new_instance_checked (0, 0, cact->second);
            Proxy *p;
            Data_Get_Struct (obj, Proxy, p);
            p->set (cobj, false, ret_type.is_cref () || ret_type.is_cptr (), ret_type.is_ptr (), obj);

          }

          rb_yield (obj);
          c->first->inc_obj_iterator (iter);

        }

        c->first->delete_obj_iterator (iter);

      } catch (...) {
        c->first->delete_obj_iterator (iter);
        throw;
      }

    } else {

      void *iter = c->first->create_obj_iterator (*rr);

      try {

        while (! c->first->obj_iterator_at_end (iter)) {

          VALUE obj = rb_class_new_instance_checked (0, 0, c->second);
          Proxy *p;
          Data_Get_Struct (obj, Proxy, p);
          p->set (c->first->take_iterator_obj (iter), true, false, false, obj);
          rb_yield (obj);
          c->first->inc_obj_iterator (iter);

        }

        c->first->delete_obj_iterator (iter);

      } catch (...) {
        c->first->delete_obj_iterator (iter);
        throw;
      }

    }
  }
};

struct yield_iter_error
{
  void operator() (gsi::SerialArgs *rr, const gsi::ArgType &ret_type)
  {
    tl_assert (false);
  }
};

template <> struct yield_iter<gsi::VectorType> : yield_iter_error { };
template <> struct yield_iter<gsi::MapType> : yield_iter_error { };

// -------------------------------------------------------------------
//  Proxy implementation

Proxy::~Proxy ()
{
  //  unregister the Proc objects
  for (std::map <const gsi::MethodBase *, VALUE>::iterator pt = m_proc_table.begin (); pt != m_proc_table.end (); ++pt) {
    rb_gc_unregister_address (&pt->second);
  }
  m_proc_table.clear ();

  try {
    set (0, false, false, false, Qnil);
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
Proxy::register_event_proc (const gsi::MethodBase *meth, VALUE proc)
{
  std::map <const gsi::MethodBase *, VALUE>::iterator pt = m_proc_table.find (meth);

  //  unregister an existing Proc object 
  if (pt != m_proc_table.end ()) {
    rb_gc_unregister_address (&pt->second);
    m_proc_table.erase (pt);
  } 

  //  end register the new one
  VALUE *proc_addr = &m_proc_table.insert (std::make_pair (meth, proc)).first->second;
  rb_gc_register_address (proc_addr);
}

void 
Proxy::call (int id, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const gsi::MethodBase *meth = m_cbfuncs [id].method;

  try {

    VALUE obj;
    ID mid;

    if (meth->is_event ()) {

      //  look up the Proc object in the table and call that ..
      std::map <const gsi::MethodBase *, VALUE>::const_iterator pt = m_proc_table.find (meth);
      if (pt == m_proc_table.end ()) {

        //  event not bound: create a nil return value if required.
        tl::Heap heap;
        push_arg (meth->ret_type (), ret, Qnil, heap);

        //  a Ruby callback must not leave temporary objects
        tl_assert (heap.empty ());

        return;

      }

      obj = pt->second;
      mid  = rb_intern("call");

    } else {

      tl_assert (id < int (m_cbfuncs.size ()) && id >= 0);
      obj = m_cbfuncs [id].rb_obj;
      mid  = m_cbfuncs [id].method_id;

    }

    std::vector<VALUE> argv;
    argv.reserve (std::distance (meth->begin_arguments (), meth->end_arguments ()));

    //  TODO: callbacks with default arguments?
    for (gsi::MethodBase::argument_iterator a = meth->begin_arguments (); args && a != meth->end_arguments (); ++a) {
      argv.push_back (pop_arg (*a, args));
    }

    VALUE rb_ret = rb_funcall2_checked (obj, mid, argv.size (), argv.empty () ? 0 : &argv [0]);
    
    tl::Heap heap;
    push_arg (meth->ret_type (), ret, rb_ret, heap);

    //  a Ruby callback must not leave temporary objects
    tl_assert (heap.empty ());

  } catch (rba::RubyError &err) {
    rba::RubyError err_with_context (err);
    err_with_context.set_context (m_cls_decl->name () + "::" + meth->names ());
    throw err_with_context;
  } catch (gsi::ExitException &ex) {
    throw;
  } catch (tl::Exception &ex) {
    throw tl::Exception (tl::to_string (QObject::tr ("Error calling method")) + " '" + m_cls_decl->name () + "::" + meth->names () + "': " + ex.msg ());
  } catch (...) {
    throw;
  }
}

void 
Proxy::destroy ()
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

void 
Proxy::detach ()
{
  m_obj = 0;
  m_destroyed = true;
  m_const_ref = false;
  m_owned = false;
  m_can_destroy = false;
}

Proxy::callbacks_cache Proxy::s_callbacks_cache;

void
Proxy::initialize_callbacks (VALUE self)
{
#if 0

  callbacks_cache::iterator cb = s_callbacks_cache.find (CLASS_OF (self));
  if (cb == s_callbacks_cache.end ()) {

    cb = s_callbacks_cache.insert (std::make_pair (CLASS_OF (self), callback_methods_type ())).first;

    const gsi::ClassBase *cls = m_cls_decl;

    // we got a new object - hence we have to attach event handlers.
    // We don't need to install virtual function callbacks because in that case, no overload is possible
    // (the object has been created on C++ side).
    while (cls) {

      for (gsi::ClassBase::method_iterator m = cls->begin_methods (); m != cls->end_methods (); ++m) {

        if ((*m)->is_event ()) {

          //  An event is given an anonymous callback for later attachment to an external method
          cb->second.push_back (*m);

        } else if ((*m)->is_callback ()) {

          //  HINT: all callback may not have aliases nor overloads
          const char *nstr = (*m)->primary_name ().c_str ();

          //  There is no place in the ruby API to determine whether a method is defined.
          //  Instead we explicitly call "method_defined?" to check, whether the given method 
          //  is defined.
          VALUE name = rb_str_new (nstr, strlen (nstr));

          for (int prot = 0; prot < 2; ++prot) {

            VALUE rb_ret;
            if (prot) {
              rb_ret = rb_funcall2_checked (rb_class_of (self), rb_intern ("protected_method_defined?"), 1, &name);
            } else {
              rb_ret = rb_funcall2_checked (rb_class_of (self), rb_intern ("method_defined?"), 1, &name);
            }
            if (RTEST (rb_ret)) {
              //  Only if the class defines that method we can link the virtual method call to the 
              //  Ruby method
              cb->second.push_back (*m);
              break;
            }

          }

        }

      }

      //  consider base classes as well.
      cls = cls->base ();

    }

  }

  for (callback_methods_type::const_iterator m = cb->second.begin (); m != cb->second.end (); ++m) {

    VALUE obj = Qnil;
    ID method_id = 0;

    if (!(*m)->is_event ()) {

      bool prot = false;
      const char *nstr = extract_protected((*m)->name ().c_str (), prot);

      method_id = rb_intern (nstr);
      obj = self;

    }

    int id = add_callback (Proxy::CallbackFunction (method_id, obj, *m));
    (*m)->set_callback (m_obj, gsi::Callback (id, this, (*m)->argsize (), (*m)->retsize ()));

  }

#else

  const gsi::ClassBase *cls = m_cls_decl;

  // we got a new object - hence we have to attach event handlers.
  // We don't need to install virtual function callbacks because in that case, no overload is possible
  // (the object has been created on C++ side).
  while (cls) {

    for (gsi::ClassBase::method_iterator m = cls->begin_methods (); m != cls->end_methods (); ++m) {

      if ((*m)->is_event ()) {

        //  An event is given an anonymous callback for later attachment to an external method
        int id = add_callback (Proxy::CallbackFunction (0, Qnil, *m));
        (*m)->set_callback (m_obj, gsi::Callback (id, this, (*m)->argsize (), (*m)->retsize ()));

      } else if ((*m)->is_callback ()) {

        //  HINT: all callback may not have aliases nor overloads
        const char *nstr = (*m)->primary_name ().c_str ();

        //  There is no place in the ruby API to determine whether a method is defined.
        //  Instead we explicitly call "method_defined?" to check, whether the given method 
        //  is defined.
        VALUE name = rb_str_new (nstr, strlen (nstr));

        for (int prot = 0; prot < 2; ++prot) {

          VALUE rb_ret;
          if (prot) {
            rb_ret = rb_funcall2_checked (rb_class_of (self), rb_intern ("protected_method_defined?"), 1, &name);
          } else {
            rb_ret = rb_funcall2_checked (rb_class_of (self), rb_intern ("method_defined?"), 1, &name);
          }
          if (RTEST (rb_ret)) {

            //  Only if the class defines that method we can link the virtual method call to the 
            //  Ruby method
            int id = add_callback (Proxy::CallbackFunction (rb_intern (nstr), self, *m));
            (*m)->set_callback (m_obj, gsi::Callback (id, this, (*m)->argsize (), (*m)->retsize ()));

            break;

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
Proxy::keep (VALUE self)
{
  const gsi::ClassBase *cls = cls_decl ();
  if (cls) {
    void *o = obj (self);
    if (o) {
      RBAClientData *cd = (RBAClientData *) cls->client_data (o, client_index);
      if (cd) {
        //  The object is gsi-enabled: it has a Ruby object attached. In that case we
        //  use "keep" on that object to keep the Ruby object alive and hence preserve 
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
Proxy::release (VALUE self)
{
  //  See the notes in keep
  const gsi::ClassBase *cls = cls_decl ();
  if (cls) {
    void *o = obj (self);
    if (o) {
      RBAClientData *cd = (RBAClientData *) cls->client_data (o, client_index);
      if (cd) {
        cd->release ();
      } else {
        m_owned = true;
      }
    }
  }
}

void 
Proxy::set (void *obj, bool owned, bool const_ref, bool can_destroy, VALUE self) 
{
  bool prev_owned = m_owned;

  m_owned = owned;
  m_can_destroy = can_destroy;

  m_const_ref = const_ref;
  if (obj != m_obj) {

    //  cleanup
    {

      const gsi::ClassBase *cls = cls_decl ();
      RBAClientData *cd = 0;
      if (m_obj && cls) {
        cd = (RBAClientData *) cls->client_data (m_obj, client_index);
      }

      //  Destroy the object if we are owner. We don't destroy the object if it was locked
      //  (either because we are not owner or from C++ side using keep())
      if (m_obj && cls && prev_owned && (! cd || ! cd->locked ())) {
        void *o = m_obj;
        cls->detach (o);
        m_obj = 0;
        cls->destroy (o);
      }

      if (m_obj) {

        //  Reset the reference
        if (cd) {
          cd->reset_ref ();
        }

        //  we'll get a new object - we have to detach event handlers.
        while (cls) {
          for (gsi::ClassBase::method_iterator m = cls->begin_methods (); m != cls->end_methods (); ++m) {
            if ((*m)->is_event ()) {
              (*m)->set_callback (m_obj, gsi::Callback ());
            }
          }
          //  consider base classes as well.
          cls = cls->base ();
        }

      }

    }

    clear_callbacks ();

    if (m_cls_decl && m_obj) {
      m_cls_decl->detach (m_obj);
    }

    m_obj = obj;

    if (m_cls_decl && m_obj) {
      m_cls_decl->attach (m_obj);
    }

    if (m_obj) {

      const gsi::ClassBase *cls = m_cls_decl;

      //  manage attachement to the C++ object if required
      //  Hold a reference for the GC unless we own the object (read: control the lifetime ourselves)
      if (cls && cls->client_data (m_obj, client_index) != 0) {
        ((RBAClientData *) cls->client_data (m_obj, client_index))->set_ref (self, !owned);
      }

      initialize_callbacks (self);

    }

  }

  //  now we have a valid object (or nil) - we can reset "destroyed" state. Note: this has to be done 
  //  here because before detach might be called on *this which resets m_destroyed.
  m_destroyed = false;
}

void *
Proxy::obj (VALUE self) 
{
  if (! m_obj) {
    if (m_destroyed) {
      throw tl::Exception (tl::to_string (QObject::tr ("Object has been destroyed already")));
    } else {
      //  delayed creation of a detached C++ object ..
      set(cls_decl ()->create (), true, m_const_ref, m_can_destroy, self);
    }
  }

  return m_obj;
}

// -------------------------------------------------------------------
//  Ruby API 

#define RBA_TRY \
  VALUE __error_msg = Qnil; \
  int __estatus = 0; \
  VALUE __eclass = Qnil; \
  { \
    try { 

#define RBA_CATCH(where) \
    } catch (std::exception &ex) { \
      __eclass = rb_eRuntimeError; \
      __error_msg = rb_str_new2 ((std::string(ex.what ()) + tl::to_string (QObject::tr (" in ")) + (where)).c_str ()); \
    } catch (gsi::ExitException &ex) { \
      __estatus = ex.status (); \
      __eclass = rb_eSystemExit; \
      __error_msg = rb_str_new2 ((ex.msg () + tl::to_string (QObject::tr (" in ")) + (where)).c_str ()); \
    } catch (tl::Exception &ex) { \
      __eclass = rb_eRuntimeError; \
      __error_msg = rb_str_new2 ((ex.msg () + tl::to_string (QObject::tr (" in ")) + (where)).c_str ()); \
    } catch (...) { \
      __eclass = rb_eRuntimeError; \
      __error_msg = rb_str_new2 ((tl::to_string (QObject::tr ("Unspecific exception in ")) + (where)).c_str ()); \
    } \
  } \
  if (__eclass == rb_eSystemExit) { \
    /*  HINT: we do the rb_raise outside any destructor code - sometimes this longjmp seems not to work properly */ \
    VALUE args [2]; \
    args [0] = INT2NUM (__estatus); \
    args [1] = __error_msg; \
    rb_exc_raise (rb_class_new_instance(2, args, __eclass)); \
  } else if (__eclass != Qnil) { \
    /*  HINT: we do the rb_raise outside any destructor code - sometimes this longjmp seems not to work properly */ \
    VALUE args [1]; \
    args [0] = __error_msg; \
    rb_exc_raise (rb_class_new_instance(1, args, __eclass)); \
  }

VALUE gsi_init (VALUE self)
{
  /* 
   * This was supposed to be a check if the object can be created with new.
   * Unfortunately we can't do this, because a initialization happens even if an object is 
   * returned as reference: 
   
  RBA_TRY

    Proxy *p;
    Data_Get_Struct (self, Proxy, p);
    const gsi::ClassBase *cls_decl_self = p->cls_decl ();
    if (! cls_decl_self->can_default_create ()) {
        throw tl::Exception (tl::to_string (QObject::tr ("No constructor provided for class '%s'")).c_str (), cls_decl_self->name ().c_str ());
    }

  RBA_CATCH(rb_class_name (self) + "::initialize")

  */

  return self;
}

static VALUE
destroy (VALUE self)
{
  //  Destroy the object
  Proxy *p;
  Data_Get_Struct (self, Proxy, p);
  p->destroy ();
  return Qnil;
}

static VALUE
keep (VALUE self)
{
  //  Makes the object kept by another instance
  Proxy *p;
  Data_Get_Struct (self, Proxy, p);
  p->keep (self);
  return Qnil;
}

static VALUE
release (VALUE self)
{
  //  Release any other ownership of the object
  Proxy *p;
  Data_Get_Struct (self, Proxy, p);
  p->release (self);
  return Qnil;
}

static VALUE
create (VALUE self)
{
  Proxy *p;
  Data_Get_Struct (self, Proxy, p);
  //  this potentially instantiates the object if not done yet
  p->obj (self);
  return self;
}

static VALUE
destroyed (VALUE self)
{
  //  return true if the object was destroyed already
  Proxy *p;
  Data_Get_Struct (self, Proxy, p);
  return c2ruby<bool> (p->destroyed ());
}

static VALUE
is_const (VALUE self)
{
  //  return true if the object was destroyed already
  Proxy *p;
  Data_Get_Struct (self, Proxy, p);
  return c2ruby<bool> (p->const_ref ());
}

static VALUE
assign (VALUE self, VALUE src)
{
  //  Compare if the classes are identical
  Proxy *p;

  Data_Get_Struct (src, Proxy, p);
  const gsi::ClassBase *cls_decl_src = p->cls_decl ();
  void *obj_src = p->obj (src);

  Data_Get_Struct (self, Proxy, p);
  const gsi::ClassBase *cls_decl_self = p->cls_decl ();
  void *obj_self = p->obj (self);

  if (cls_decl_src != cls_decl_self) {
    throw tl::Exception (tl::to_string (QObject::tr ("Type is not identical on copy")));
  } 
  if (! cls_decl_self->can_copy ()) {
    throw tl::Exception (tl::to_string (QObject::tr ("No assignment provided for class '%s'")), cls_decl_self->name ());
  }
  cls_decl_self->assign (obj_self, obj_src);

  return self;
}

static VALUE
special_method_impl (const gsi::MethodBase *meth, int argc, VALUE *argv, VALUE self, bool ctor)
{
  gsi::MethodBase::special_method_type smt = meth->smt ();

  if (smt == gsi::MethodBase::DefaultCtor) {

    //  Must be called in the ctor context and does nothing since the object is 
    //  automatically default-created
    //  It is mapped to the non-static(!) initialize method.
    tl_assert (ctor);
    return Qnil;

  } else if (smt == gsi::MethodBase::Destroy) {
    tl_assert (!ctor);
    return destroy (self);
  } else if (smt == gsi::MethodBase::Keep) {
    tl_assert (!ctor);
    return keep (self);
  } else if (smt == gsi::MethodBase::Release) {
    tl_assert (!ctor);
    return release (self);
  } else if (smt == gsi::MethodBase::Create) {
    tl_assert (!ctor);
    return create (self);
  } else if (smt == gsi::MethodBase::IsConst) {
    tl_assert (!ctor);
    return is_const (self);
  } else if (smt == gsi::MethodBase::Destroyed) {
    tl_assert (!ctor);
    return destroyed (self);
  } else if (smt == gsi::MethodBase::Assign) {

    //  this is either assign or dup in diguise
    tl_assert (argc == 1);
    return assign (self, argv [0]);

  } else if (smt == gsi::MethodBase::Dup) {

    //  dup is disguised as assign in ctor context
    tl_assert (false); 

  } else {
    return Qnil;
  }
}

void free_proxy (void *p)
{
  delete ((Proxy *) p);
}

VALUE alloc_proxy (VALUE klass)
{
  tl_assert (TYPE (klass) == T_CLASS);

  const gsi::ClassBase *cls = find_cclass (klass);
  Proxy *proxy = new Proxy (cls);
  return Data_Wrap_Struct (klass, 0, free_proxy, proxy);
}

/**
 *  @brief Gets the method name from a method id
 */
std::string 
method_name_from_id (int mid, VALUE self)
{
  const gsi::ClassBase *cls_decl;
  Proxy *p = 0;

  if (TYPE (self) == T_CLASS) {
    //  we have a static method
    cls_decl = find_cclass (self);
  } else {
    //  we have an instance method
    Data_Get_Struct (self, Proxy, p);
    cls_decl = p->cls_decl ();
  }

  const MethodTable *mt = MethodTable::method_table_by_class (cls_decl);
  tl_assert (mt);

  //  locate the method in the base classes method table if necessary
  while (mid < int (mt->bottom_mid ())) {

    tl_assert (cls_decl->base ());
    cls_decl = cls_decl->base ();
    mt = MethodTable::method_table_by_class (cls_decl);
    tl_assert (mt);

  }

  return cls_decl->name () + "::" + mt->name (mid);
}

VALUE
method_adaptor (int mid, int argc, VALUE *argv, VALUE self, bool ctor)
{
  VALUE ret = Qnil;

  RBA_TRY

    tl::Heap heap;

    const gsi::ClassBase *cls_decl;
    Proxy *p = 0;

    if (TYPE (self) == T_CLASS) {
      //  we have a static method
      cls_decl = find_cclass (self);
    } else {
      //  we have an instance method
      Data_Get_Struct (self, Proxy, p);
      cls_decl = p->cls_decl ();
    }

    const MethodTable *mt = MethodTable::method_table_by_class (cls_decl);
    tl_assert (mt);

    //  locate the method in the base classes method table if necessary
    while (mid < int (mt->bottom_mid ())) {

      tl_assert (cls_decl->base ());
      cls_decl = cls_decl->base ();
      mt = MethodTable::method_table_by_class (cls_decl);
      tl_assert (mt);

    }

    //  get number of candidates by argument count
    const gsi::MethodBase *meth = 0;
    unsigned int candidates = 0;
    for (MethodTableEntry::method_iterator m = mt->begin (mid); m != mt->end (mid); ++m) {

      if ((*m)->is_event()) {

        if (rb_block_given_p ()) {
          //  events do not have parameters, but accept a Proc object -> no overloading -> take this one.
          candidates = 1;
          meth = *m;
          break;
        } else {
          throw tl::Exception (tl::to_string (QObject::tr ("An event needs a block")));
        }

      } else if ((*m)->is_callback()) {

        //  ignore callbacks
      
      } else if ((*m)->compatible_with_num_args (argc)) {

        ++candidates;
        meth = *m;

      }

    }

    //  no method found, but the ctor was requested - implement that method as replacement for the default "initialize"
    if (! meth && argc == 0 && ctor) {
      return Qnil;
    }

    //  no candidate -> error
    if (! meth) {

      std::set<unsigned int> nargs;
      for (MethodTableEntry::method_iterator m = mt->begin (mid); m != mt->end (mid); ++m) {
        if (! (*m)->is_callback () && ! (*m)->is_event ()) {
          nargs.insert (std::distance ((*m)->begin_arguments (), (*m)->end_arguments ()));
        }
      }

      std::string nargs_s;
      for (std::set<unsigned int>::const_iterator na = nargs.begin (); na != nargs.end (); ++na) {
        if (na != nargs.begin ()) {
          nargs_s += "/";
        }
        nargs_s += tl::to_string (*na);
      }

      throw tl::Exception (tl::sprintf (tl::to_string (QObject::tr ("Invalid number of arguments (got %d, expected %s)")), argc, nargs_s));

    }

    //  more than one candidate -> refine by checking the arguments
    if (candidates > 1) {

      //  two passes where the second is with loose checking
      int pass = 0;

      do {

        meth = 0;
        candidates = 0;
        bool const_matching = true;

        for (MethodTableEntry::method_iterator m = mt->begin (mid); m != mt->end (mid); ++m) {

          if (! (*m)->is_callback () && ! (*m)->is_event ()) {

            //  check arguments (count and type)
            bool is_valid = (*m)->compatible_with_num_args (argc);
            VALUE *av = argv;
            for (gsi::MethodBase::argument_iterator a = (*m)->begin_arguments (); is_valid && av < argv + argc && a != (*m)->end_arguments (); ++a, ++av) {
              if (! test_arg (*a, *av, pass != 0 /*loose in the second pass*/)) {
                is_valid = false;
              }
            }

            if (is_valid && p) {

              //  constness matching candidates have precedence
              if ((*m)->is_const () != p->const_ref ()) {
                if (const_matching && candidates > 0) {
                  is_valid = false;
                } else {
                  const_matching = false;
                }
              } else if (! const_matching) {
                const_matching = true;
                candidates = 0;
              }

            }

            if (is_valid) {
              ++candidates;
              meth = *m;
            }

          }

        }

        ++pass;

      } while (! meth && pass < 2);

    }

    if (! meth) {
      throw tl::Exception (tl::to_string (QObject::tr ("No overload with matching arguments")));
    }

    if (candidates > 1) {
      throw tl::Exception (tl::to_string (QObject::tr ("Ambiguous overload variants - multiple method declarations match arguments")));
    }

    if (p && p->const_ref () && ! meth->is_const ()) {
      throw tl::Exception (tl::to_string (QObject::tr ("Cannot call non-const method on a const reference")));
    }

    if (meth->smt () != gsi::MethodBase::None) {

      ret = special_method_impl (meth, argc, argv, self, ctor);

    } else if (meth->is_event ()) {

      if (p) {
        //  Hint: this potentially instantiates the object
        p->obj (self);
        p->register_event_proc(meth, rb_block_proc ());
      }

    } else if (ctor && meth->ret_type ().type () == gsi::T_object && meth->ret_type ().pass_obj ()) {

      tl_assert (p != 0);

      //  This is a non-static constructor ("new" renamed to "initialize"): it does not create a 
      //  new Ruby object but just a new C++ object which replaces the old one.

      //  TODO: when the yield block raises an error, the buffer allocated here does not get cleaned up!
      gsi::SerialArgs retlist (meth->retsize ());

      {
        gsi::SerialArgs arglist (meth->argsize ());
        VALUE *av = argv;
        for (gsi::MethodBase::argument_iterator a = meth->begin_arguments (); a != meth->end_arguments () && av < argv + argc; ++a, ++av) {
          push_arg (*a, arglist, *av, heap);
        }

        meth->call (0, arglist, retlist);
      }

      void *obj = retlist.read<void *> ();
      if (obj == 0) {
        p->set (0, false, false, false, Qnil);
      } else {
        p->set (obj, true, false, false, self);
      }

    } else {

      void *obj = 0;
      if (p) {
        //  Hint: this potentially instantiates the object
        obj = p->obj (self);
      }

      //  TODO: when the yield block raises an error, the buffer allocated here does not get cleaned up!
      gsi::SerialArgs retlist (meth->retsize ());

      {
        gsi::SerialArgs arglist (meth->argsize ());
        VALUE *av = argv;
        for (gsi::MethodBase::argument_iterator a = meth->begin_arguments (); a != meth->end_arguments () && av < argv + argc; ++a, ++av) {
          push_arg (*a, arglist, *av, heap);
        }

        meth->call (obj, arglist, retlist);
      }

      if (meth->ret_type ().is_iter ()) {
        gsi::do_on_type<yield_iter> () (meth->ret_type ().type (), &retlist, meth->ret_type ());
      } else {
        ret = pop_arg (meth->ret_type (), retlist);
      }

    }

  RBA_CATCH(method_name_from_id (mid, self))

  return ret;
}

VALUE method_adaptor_n (int mid, int argc, VALUE *argv, VALUE self, bool ctor)
{
  return method_adaptor (mid, argc, argv, self, ctor);
}

template <int N> 
VALUE method_adaptor (int argc, VALUE *argv, VALUE self)
{
  return method_adaptor_n (N, argc, argv, self, false);
}

VALUE (*(method_adaptors [])) (int, VALUE *, VALUE) =
{
  &method_adaptor<0x000>, &method_adaptor<0x001>, &method_adaptor<0x002>, &method_adaptor<0x003>, &method_adaptor<0x004>, &method_adaptor<0x005>, &method_adaptor<0x006>, &method_adaptor<0x007>,
  &method_adaptor<0x008>, &method_adaptor<0x009>, &method_adaptor<0x00a>, &method_adaptor<0x00b>, &method_adaptor<0x00c>, &method_adaptor<0x00d>, &method_adaptor<0x00e>, &method_adaptor<0x00f>,
  &method_adaptor<0x010>, &method_adaptor<0x011>, &method_adaptor<0x012>, &method_adaptor<0x013>, &method_adaptor<0x014>, &method_adaptor<0x015>, &method_adaptor<0x016>, &method_adaptor<0x017>,
  &method_adaptor<0x018>, &method_adaptor<0x019>, &method_adaptor<0x01a>, &method_adaptor<0x01b>, &method_adaptor<0x01c>, &method_adaptor<0x01d>, &method_adaptor<0x01e>, &method_adaptor<0x01f>,
  &method_adaptor<0x020>, &method_adaptor<0x021>, &method_adaptor<0x022>, &method_adaptor<0x023>, &method_adaptor<0x024>, &method_adaptor<0x025>, &method_adaptor<0x026>, &method_adaptor<0x027>,
  &method_adaptor<0x028>, &method_adaptor<0x029>, &method_adaptor<0x02a>, &method_adaptor<0x02b>, &method_adaptor<0x02c>, &method_adaptor<0x02d>, &method_adaptor<0x02e>, &method_adaptor<0x02f>,
  &method_adaptor<0x030>, &method_adaptor<0x031>, &method_adaptor<0x032>, &method_adaptor<0x033>, &method_adaptor<0x034>, &method_adaptor<0x035>, &method_adaptor<0x036>, &method_adaptor<0x037>,
  &method_adaptor<0x038>, &method_adaptor<0x039>, &method_adaptor<0x03a>, &method_adaptor<0x03b>, &method_adaptor<0x03c>, &method_adaptor<0x03d>, &method_adaptor<0x03e>, &method_adaptor<0x03f>,
  &method_adaptor<0x040>, &method_adaptor<0x041>, &method_adaptor<0x042>, &method_adaptor<0x043>, &method_adaptor<0x044>, &method_adaptor<0x045>, &method_adaptor<0x046>, &method_adaptor<0x047>,
  &method_adaptor<0x048>, &method_adaptor<0x049>, &method_adaptor<0x04a>, &method_adaptor<0x04b>, &method_adaptor<0x04c>, &method_adaptor<0x04d>, &method_adaptor<0x04e>, &method_adaptor<0x04f>,
  &method_adaptor<0x050>, &method_adaptor<0x051>, &method_adaptor<0x052>, &method_adaptor<0x053>, &method_adaptor<0x054>, &method_adaptor<0x055>, &method_adaptor<0x056>, &method_adaptor<0x057>,
  &method_adaptor<0x058>, &method_adaptor<0x059>, &method_adaptor<0x05a>, &method_adaptor<0x05b>, &method_adaptor<0x05c>, &method_adaptor<0x05d>, &method_adaptor<0x05e>, &method_adaptor<0x05f>,
  &method_adaptor<0x060>, &method_adaptor<0x061>, &method_adaptor<0x062>, &method_adaptor<0x063>, &method_adaptor<0x064>, &method_adaptor<0x065>, &method_adaptor<0x066>, &method_adaptor<0x067>,
  &method_adaptor<0x068>, &method_adaptor<0x069>, &method_adaptor<0x06a>, &method_adaptor<0x06b>, &method_adaptor<0x06c>, &method_adaptor<0x06d>, &method_adaptor<0x06e>, &method_adaptor<0x06f>,
  &method_adaptor<0x070>, &method_adaptor<0x071>, &method_adaptor<0x072>, &method_adaptor<0x073>, &method_adaptor<0x074>, &method_adaptor<0x075>, &method_adaptor<0x076>, &method_adaptor<0x077>,
  &method_adaptor<0x078>, &method_adaptor<0x079>, &method_adaptor<0x07a>, &method_adaptor<0x07b>, &method_adaptor<0x07c>, &method_adaptor<0x07d>, &method_adaptor<0x07e>, &method_adaptor<0x07f>,
  &method_adaptor<0x080>, &method_adaptor<0x081>, &method_adaptor<0x082>, &method_adaptor<0x083>, &method_adaptor<0x084>, &method_adaptor<0x085>, &method_adaptor<0x086>, &method_adaptor<0x087>,
  &method_adaptor<0x088>, &method_adaptor<0x089>, &method_adaptor<0x08a>, &method_adaptor<0x08b>, &method_adaptor<0x08c>, &method_adaptor<0x08d>, &method_adaptor<0x08e>, &method_adaptor<0x08f>,
  &method_adaptor<0x090>, &method_adaptor<0x091>, &method_adaptor<0x092>, &method_adaptor<0x093>, &method_adaptor<0x094>, &method_adaptor<0x095>, &method_adaptor<0x096>, &method_adaptor<0x097>,
  &method_adaptor<0x098>, &method_adaptor<0x099>, &method_adaptor<0x09a>, &method_adaptor<0x09b>, &method_adaptor<0x09c>, &method_adaptor<0x09d>, &method_adaptor<0x09e>, &method_adaptor<0x09f>,
  &method_adaptor<0x0a0>, &method_adaptor<0x0a1>, &method_adaptor<0x0a2>, &method_adaptor<0x0a3>, &method_adaptor<0x0a4>, &method_adaptor<0x0a5>, &method_adaptor<0x0a6>, &method_adaptor<0x0a7>,
  &method_adaptor<0x0a8>, &method_adaptor<0x0a9>, &method_adaptor<0x0aa>, &method_adaptor<0x0ab>, &method_adaptor<0x0ac>, &method_adaptor<0x0ad>, &method_adaptor<0x0ae>, &method_adaptor<0x0af>,
  &method_adaptor<0x0b0>, &method_adaptor<0x0b1>, &method_adaptor<0x0b2>, &method_adaptor<0x0b3>, &method_adaptor<0x0b4>, &method_adaptor<0x0b5>, &method_adaptor<0x0b6>, &method_adaptor<0x0b7>,
  &method_adaptor<0x0b8>, &method_adaptor<0x0b9>, &method_adaptor<0x0ba>, &method_adaptor<0x0bb>, &method_adaptor<0x0bc>, &method_adaptor<0x0bd>, &method_adaptor<0x0be>, &method_adaptor<0x0bf>,
  &method_adaptor<0x0c0>, &method_adaptor<0x0c1>, &method_adaptor<0x0c2>, &method_adaptor<0x0c3>, &method_adaptor<0x0c4>, &method_adaptor<0x0c5>, &method_adaptor<0x0c6>, &method_adaptor<0x0c7>,
  &method_adaptor<0x0c8>, &method_adaptor<0x0c9>, &method_adaptor<0x0ca>, &method_adaptor<0x0cb>, &method_adaptor<0x0cc>, &method_adaptor<0x0cd>, &method_adaptor<0x0ce>, &method_adaptor<0x0cf>,
  &method_adaptor<0x0d0>, &method_adaptor<0x0d1>, &method_adaptor<0x0d2>, &method_adaptor<0x0d3>, &method_adaptor<0x0d4>, &method_adaptor<0x0d5>, &method_adaptor<0x0d6>, &method_adaptor<0x0d7>,
  &method_adaptor<0x0d8>, &method_adaptor<0x0d9>, &method_adaptor<0x0da>, &method_adaptor<0x0db>, &method_adaptor<0x0dc>, &method_adaptor<0x0dd>, &method_adaptor<0x0de>, &method_adaptor<0x0df>,
  &method_adaptor<0x0e0>, &method_adaptor<0x0e1>, &method_adaptor<0x0e2>, &method_adaptor<0x0e3>, &method_adaptor<0x0e4>, &method_adaptor<0x0e5>, &method_adaptor<0x0e6>, &method_adaptor<0x0e7>,
  &method_adaptor<0x0e8>, &method_adaptor<0x0e9>, &method_adaptor<0x0ea>, &method_adaptor<0x0eb>, &method_adaptor<0x0ec>, &method_adaptor<0x0ed>, &method_adaptor<0x0ee>, &method_adaptor<0x0ef>,
  &method_adaptor<0x0f0>, &method_adaptor<0x0f1>, &method_adaptor<0x0f2>, &method_adaptor<0x0f3>, &method_adaptor<0x0f4>, &method_adaptor<0x0f5>, &method_adaptor<0x0f6>, &method_adaptor<0x0f7>,
  &method_adaptor<0x0f8>, &method_adaptor<0x0f9>, &method_adaptor<0x0fa>, &method_adaptor<0x0fb>, &method_adaptor<0x0fc>, &method_adaptor<0x0fd>, &method_adaptor<0x0fe>, &method_adaptor<0x0ff>,
  &method_adaptor<0x100>, &method_adaptor<0x101>, &method_adaptor<0x102>, &method_adaptor<0x103>, &method_adaptor<0x104>, &method_adaptor<0x105>, &method_adaptor<0x106>, &method_adaptor<0x107>,
  &method_adaptor<0x108>, &method_adaptor<0x109>, &method_adaptor<0x10a>, &method_adaptor<0x10b>, &method_adaptor<0x10c>, &method_adaptor<0x10d>, &method_adaptor<0x10e>, &method_adaptor<0x10f>,
  &method_adaptor<0x110>, &method_adaptor<0x111>, &method_adaptor<0x112>, &method_adaptor<0x113>, &method_adaptor<0x114>, &method_adaptor<0x115>, &method_adaptor<0x116>, &method_adaptor<0x117>,
  &method_adaptor<0x118>, &method_adaptor<0x119>, &method_adaptor<0x11a>, &method_adaptor<0x11b>, &method_adaptor<0x11c>, &method_adaptor<0x11d>, &method_adaptor<0x11e>, &method_adaptor<0x11f>,
  &method_adaptor<0x120>, &method_adaptor<0x121>, &method_adaptor<0x122>, &method_adaptor<0x123>, &method_adaptor<0x124>, &method_adaptor<0x125>, &method_adaptor<0x126>, &method_adaptor<0x127>,
  &method_adaptor<0x128>, &method_adaptor<0x129>, &method_adaptor<0x12a>, &method_adaptor<0x12b>, &method_adaptor<0x12c>, &method_adaptor<0x12d>, &method_adaptor<0x12e>, &method_adaptor<0x12f>,
  &method_adaptor<0x130>, &method_adaptor<0x131>, &method_adaptor<0x132>, &method_adaptor<0x133>, &method_adaptor<0x134>, &method_adaptor<0x135>, &method_adaptor<0x136>, &method_adaptor<0x137>,
  &method_adaptor<0x138>, &method_adaptor<0x139>, &method_adaptor<0x13a>, &method_adaptor<0x13b>, &method_adaptor<0x13c>, &method_adaptor<0x13d>, &method_adaptor<0x13e>, &method_adaptor<0x13f>,
  &method_adaptor<0x140>, &method_adaptor<0x141>, &method_adaptor<0x142>, &method_adaptor<0x143>, &method_adaptor<0x144>, &method_adaptor<0x145>, &method_adaptor<0x146>, &method_adaptor<0x147>,
  &method_adaptor<0x148>, &method_adaptor<0x149>, &method_adaptor<0x14a>, &method_adaptor<0x14b>, &method_adaptor<0x14c>, &method_adaptor<0x14d>, &method_adaptor<0x14e>, &method_adaptor<0x14f>,
  &method_adaptor<0x150>, &method_adaptor<0x151>, &method_adaptor<0x152>, &method_adaptor<0x153>, &method_adaptor<0x154>, &method_adaptor<0x155>, &method_adaptor<0x156>, &method_adaptor<0x157>,
  &method_adaptor<0x158>, &method_adaptor<0x159>, &method_adaptor<0x15a>, &method_adaptor<0x15b>, &method_adaptor<0x15c>, &method_adaptor<0x15d>, &method_adaptor<0x15e>, &method_adaptor<0x15f>,
  &method_adaptor<0x160>, &method_adaptor<0x161>, &method_adaptor<0x162>, &method_adaptor<0x163>, &method_adaptor<0x164>, &method_adaptor<0x165>, &method_adaptor<0x166>, &method_adaptor<0x167>,
  &method_adaptor<0x168>, &method_adaptor<0x169>, &method_adaptor<0x16a>, &method_adaptor<0x16b>, &method_adaptor<0x16c>, &method_adaptor<0x16d>, &method_adaptor<0x16e>, &method_adaptor<0x16f>,
  &method_adaptor<0x170>, &method_adaptor<0x171>, &method_adaptor<0x172>, &method_adaptor<0x173>, &method_adaptor<0x174>, &method_adaptor<0x175>, &method_adaptor<0x176>, &method_adaptor<0x177>,
  &method_adaptor<0x178>, &method_adaptor<0x179>, &method_adaptor<0x17a>, &method_adaptor<0x17b>, &method_adaptor<0x17c>, &method_adaptor<0x17d>, &method_adaptor<0x17e>, &method_adaptor<0x17f>,
  &method_adaptor<0x180>, &method_adaptor<0x181>, &method_adaptor<0x182>, &method_adaptor<0x183>, &method_adaptor<0x184>, &method_adaptor<0x185>, &method_adaptor<0x186>, &method_adaptor<0x187>,
  &method_adaptor<0x188>, &method_adaptor<0x189>, &method_adaptor<0x18a>, &method_adaptor<0x18b>, &method_adaptor<0x18c>, &method_adaptor<0x18d>, &method_adaptor<0x18e>, &method_adaptor<0x18f>,
  &method_adaptor<0x190>, &method_adaptor<0x191>, &method_adaptor<0x192>, &method_adaptor<0x193>, &method_adaptor<0x194>, &method_adaptor<0x195>, &method_adaptor<0x196>, &method_adaptor<0x197>,
  &method_adaptor<0x198>, &method_adaptor<0x199>, &method_adaptor<0x19a>, &method_adaptor<0x19b>, &method_adaptor<0x19c>, &method_adaptor<0x19d>, &method_adaptor<0x19e>, &method_adaptor<0x19f>,
  &method_adaptor<0x1a0>, &method_adaptor<0x1a1>, &method_adaptor<0x1a2>, &method_adaptor<0x1a3>, &method_adaptor<0x1a4>, &method_adaptor<0x1a5>, &method_adaptor<0x1a6>, &method_adaptor<0x1a7>,
  &method_adaptor<0x1a8>, &method_adaptor<0x1a9>, &method_adaptor<0x1aa>, &method_adaptor<0x1ab>, &method_adaptor<0x1ac>, &method_adaptor<0x1ad>, &method_adaptor<0x1ae>, &method_adaptor<0x1af>,
  &method_adaptor<0x1b0>, &method_adaptor<0x1b1>, &method_adaptor<0x1b2>, &method_adaptor<0x1b3>, &method_adaptor<0x1b4>, &method_adaptor<0x1b5>, &method_adaptor<0x1b6>, &method_adaptor<0x1b7>,
  &method_adaptor<0x1b8>, &method_adaptor<0x1b9>, &method_adaptor<0x1ba>, &method_adaptor<0x1bb>, &method_adaptor<0x1bc>, &method_adaptor<0x1bd>, &method_adaptor<0x1be>, &method_adaptor<0x1bf>,
  &method_adaptor<0x1c0>, &method_adaptor<0x1c1>, &method_adaptor<0x1c2>, &method_adaptor<0x1c3>, &method_adaptor<0x1c4>, &method_adaptor<0x1c5>, &method_adaptor<0x1c6>, &method_adaptor<0x1c7>,
  &method_adaptor<0x1c8>, &method_adaptor<0x1c9>, &method_adaptor<0x1ca>, &method_adaptor<0x1cb>, &method_adaptor<0x1cc>, &method_adaptor<0x1cd>, &method_adaptor<0x1ce>, &method_adaptor<0x1cf>,
  &method_adaptor<0x1d0>, &method_adaptor<0x1d1>, &method_adaptor<0x1d2>, &method_adaptor<0x1d3>, &method_adaptor<0x1d4>, &method_adaptor<0x1d5>, &method_adaptor<0x1d6>, &method_adaptor<0x1d7>,
  &method_adaptor<0x1d8>, &method_adaptor<0x1d9>, &method_adaptor<0x1da>, &method_adaptor<0x1db>, &method_adaptor<0x1dc>, &method_adaptor<0x1dd>, &method_adaptor<0x1de>, &method_adaptor<0x1df>,
  &method_adaptor<0x1e0>, &method_adaptor<0x1e1>, &method_adaptor<0x1e2>, &method_adaptor<0x1e3>, &method_adaptor<0x1e4>, &method_adaptor<0x1e5>, &method_adaptor<0x1e6>, &method_adaptor<0x1e7>,
  &method_adaptor<0x1e8>, &method_adaptor<0x1e9>, &method_adaptor<0x1ea>, &method_adaptor<0x1eb>, &method_adaptor<0x1ec>, &method_adaptor<0x1ed>, &method_adaptor<0x1ee>, &method_adaptor<0x1ef>,
  &method_adaptor<0x1f0>, &method_adaptor<0x1f1>, &method_adaptor<0x1f2>, &method_adaptor<0x1f3>, &method_adaptor<0x1f4>, &method_adaptor<0x1f5>, &method_adaptor<0x1f6>, &method_adaptor<0x1f7>,
  &method_adaptor<0x1f8>, &method_adaptor<0x1f9>, &method_adaptor<0x1fa>, &method_adaptor<0x1fb>, &method_adaptor<0x1fc>, &method_adaptor<0x1fd>, &method_adaptor<0x1fe>, &method_adaptor<0x1ff>,
  &method_adaptor<0x200>, &method_adaptor<0x201>, &method_adaptor<0x202>, &method_adaptor<0x203>, &method_adaptor<0x204>, &method_adaptor<0x205>, &method_adaptor<0x206>, &method_adaptor<0x207>,
  &method_adaptor<0x208>, &method_adaptor<0x209>, &method_adaptor<0x20a>, &method_adaptor<0x20b>, &method_adaptor<0x20c>, &method_adaptor<0x20d>, &method_adaptor<0x20e>, &method_adaptor<0x20f>,
  &method_adaptor<0x210>, &method_adaptor<0x211>, &method_adaptor<0x212>, &method_adaptor<0x213>, &method_adaptor<0x214>, &method_adaptor<0x215>, &method_adaptor<0x216>, &method_adaptor<0x217>,
  &method_adaptor<0x218>, &method_adaptor<0x219>, &method_adaptor<0x21a>, &method_adaptor<0x21b>, &method_adaptor<0x21c>, &method_adaptor<0x21d>, &method_adaptor<0x21e>, &method_adaptor<0x21f>,
  &method_adaptor<0x220>, &method_adaptor<0x221>, &method_adaptor<0x222>, &method_adaptor<0x223>, &method_adaptor<0x224>, &method_adaptor<0x225>, &method_adaptor<0x226>, &method_adaptor<0x227>,
  &method_adaptor<0x228>, &method_adaptor<0x229>, &method_adaptor<0x22a>, &method_adaptor<0x22b>, &method_adaptor<0x22c>, &method_adaptor<0x22d>, &method_adaptor<0x22e>, &method_adaptor<0x22f>,
  &method_adaptor<0x230>, &method_adaptor<0x231>, &method_adaptor<0x232>, &method_adaptor<0x233>, &method_adaptor<0x234>, &method_adaptor<0x235>, &method_adaptor<0x236>, &method_adaptor<0x237>,
  &method_adaptor<0x238>, &method_adaptor<0x239>, &method_adaptor<0x23a>, &method_adaptor<0x23b>, &method_adaptor<0x23c>, &method_adaptor<0x23d>, &method_adaptor<0x23e>, &method_adaptor<0x23f>,
  &method_adaptor<0x240>, &method_adaptor<0x241>, &method_adaptor<0x242>, &method_adaptor<0x243>, &method_adaptor<0x244>, &method_adaptor<0x245>, &method_adaptor<0x246>, &method_adaptor<0x247>,
  &method_adaptor<0x248>, &method_adaptor<0x249>, &method_adaptor<0x24a>, &method_adaptor<0x24b>, &method_adaptor<0x24c>, &method_adaptor<0x24d>, &method_adaptor<0x24e>, &method_adaptor<0x24f>,
  &method_adaptor<0x250>, &method_adaptor<0x251>, &method_adaptor<0x252>, &method_adaptor<0x253>, &method_adaptor<0x254>, &method_adaptor<0x255>, &method_adaptor<0x256>, &method_adaptor<0x257>,
  &method_adaptor<0x258>, &method_adaptor<0x259>, &method_adaptor<0x25a>, &method_adaptor<0x25b>, &method_adaptor<0x25c>, &method_adaptor<0x25d>, &method_adaptor<0x25e>, &method_adaptor<0x25f>,
  &method_adaptor<0x260>, &method_adaptor<0x261>, &method_adaptor<0x262>, &method_adaptor<0x263>, &method_adaptor<0x264>, &method_adaptor<0x265>, &method_adaptor<0x266>, &method_adaptor<0x267>,
  &method_adaptor<0x268>, &method_adaptor<0x269>, &method_adaptor<0x26a>, &method_adaptor<0x26b>, &method_adaptor<0x26c>, &method_adaptor<0x26d>, &method_adaptor<0x26e>, &method_adaptor<0x26f>,
  &method_adaptor<0x270>, &method_adaptor<0x271>, &method_adaptor<0x272>, &method_adaptor<0x273>, &method_adaptor<0x274>, &method_adaptor<0x275>, &method_adaptor<0x276>, &method_adaptor<0x277>,
  &method_adaptor<0x278>, &method_adaptor<0x279>, &method_adaptor<0x27a>, &method_adaptor<0x27b>, &method_adaptor<0x27c>, &method_adaptor<0x27d>, &method_adaptor<0x27e>, &method_adaptor<0x27f>,
  &method_adaptor<0x280>, &method_adaptor<0x281>, &method_adaptor<0x282>, &method_adaptor<0x283>, &method_adaptor<0x284>, &method_adaptor<0x285>, &method_adaptor<0x286>, &method_adaptor<0x287>,
  &method_adaptor<0x288>, &method_adaptor<0x289>, &method_adaptor<0x28a>, &method_adaptor<0x28b>, &method_adaptor<0x28c>, &method_adaptor<0x28d>, &method_adaptor<0x28e>, &method_adaptor<0x28f>,
  &method_adaptor<0x290>, &method_adaptor<0x291>, &method_adaptor<0x292>, &method_adaptor<0x293>, &method_adaptor<0x294>, &method_adaptor<0x295>, &method_adaptor<0x296>, &method_adaptor<0x297>,
  &method_adaptor<0x298>, &method_adaptor<0x299>, &method_adaptor<0x29a>, &method_adaptor<0x29b>, &method_adaptor<0x29c>, &method_adaptor<0x29d>, &method_adaptor<0x29e>, &method_adaptor<0x29f>,
  &method_adaptor<0x2a0>, &method_adaptor<0x2a1>, &method_adaptor<0x2a2>, &method_adaptor<0x2a3>, &method_adaptor<0x2a4>, &method_adaptor<0x2a5>, &method_adaptor<0x2a6>, &method_adaptor<0x2a7>,
  &method_adaptor<0x2a8>, &method_adaptor<0x2a9>, &method_adaptor<0x2aa>, &method_adaptor<0x2ab>, &method_adaptor<0x2ac>, &method_adaptor<0x2ad>, &method_adaptor<0x2ae>, &method_adaptor<0x2af>,
  &method_adaptor<0x2b0>, &method_adaptor<0x2b1>, &method_adaptor<0x2b2>, &method_adaptor<0x2b3>, &method_adaptor<0x2b4>, &method_adaptor<0x2b5>, &method_adaptor<0x2b6>, &method_adaptor<0x2b7>,
  &method_adaptor<0x2b8>, &method_adaptor<0x2b9>, &method_adaptor<0x2ba>, &method_adaptor<0x2bb>, &method_adaptor<0x2bc>, &method_adaptor<0x2bd>, &method_adaptor<0x2be>, &method_adaptor<0x2bf>,
  &method_adaptor<0x2c0>, &method_adaptor<0x2c1>, &method_adaptor<0x2c2>, &method_adaptor<0x2c3>, &method_adaptor<0x2c4>, &method_adaptor<0x2c5>, &method_adaptor<0x2c6>, &method_adaptor<0x2c7>,
  &method_adaptor<0x2c8>, &method_adaptor<0x2c9>, &method_adaptor<0x2ca>, &method_adaptor<0x2cb>, &method_adaptor<0x2cc>, &method_adaptor<0x2cd>, &method_adaptor<0x2ce>, &method_adaptor<0x2cf>,
  &method_adaptor<0x2d0>, &method_adaptor<0x2d1>, &method_adaptor<0x2d2>, &method_adaptor<0x2d3>, &method_adaptor<0x2d4>, &method_adaptor<0x2d5>, &method_adaptor<0x2d6>, &method_adaptor<0x2d7>,
  &method_adaptor<0x2d8>, &method_adaptor<0x2d9>, &method_adaptor<0x2da>, &method_adaptor<0x2db>, &method_adaptor<0x2dc>, &method_adaptor<0x2dd>, &method_adaptor<0x2de>, &method_adaptor<0x2df>,
  &method_adaptor<0x2e0>, &method_adaptor<0x2e1>, &method_adaptor<0x2e2>, &method_adaptor<0x2e3>, &method_adaptor<0x2e4>, &method_adaptor<0x2e5>, &method_adaptor<0x2e6>, &method_adaptor<0x2e7>,
  &method_adaptor<0x2e8>, &method_adaptor<0x2e9>, &method_adaptor<0x2ea>, &method_adaptor<0x2eb>, &method_adaptor<0x2ec>, &method_adaptor<0x2ed>, &method_adaptor<0x2ee>, &method_adaptor<0x2ef>,
  &method_adaptor<0x2f0>, &method_adaptor<0x2f1>, &method_adaptor<0x2f2>, &method_adaptor<0x2f3>, &method_adaptor<0x2f4>, &method_adaptor<0x2f5>, &method_adaptor<0x2f6>, &method_adaptor<0x2f7>,
  &method_adaptor<0x2f8>, &method_adaptor<0x2f9>, &method_adaptor<0x2fa>, &method_adaptor<0x2fb>, &method_adaptor<0x2fc>, &method_adaptor<0x2fd>, &method_adaptor<0x2fe>, &method_adaptor<0x2ff>,
  &method_adaptor<0x300>, &method_adaptor<0x301>, &method_adaptor<0x302>, &method_adaptor<0x303>, &method_adaptor<0x304>, &method_adaptor<0x305>, &method_adaptor<0x306>, &method_adaptor<0x307>,
  &method_adaptor<0x308>, &method_adaptor<0x309>, &method_adaptor<0x30a>, &method_adaptor<0x30b>, &method_adaptor<0x30c>, &method_adaptor<0x30d>, &method_adaptor<0x30e>, &method_adaptor<0x30f>,
  &method_adaptor<0x310>, &method_adaptor<0x311>, &method_adaptor<0x312>, &method_adaptor<0x313>, &method_adaptor<0x314>, &method_adaptor<0x315>, &method_adaptor<0x316>, &method_adaptor<0x317>,
  &method_adaptor<0x318>, &method_adaptor<0x319>, &method_adaptor<0x31a>, &method_adaptor<0x31b>, &method_adaptor<0x31c>, &method_adaptor<0x31d>, &method_adaptor<0x31e>, &method_adaptor<0x31f>,
  &method_adaptor<0x320>, &method_adaptor<0x321>, &method_adaptor<0x322>, &method_adaptor<0x323>, &method_adaptor<0x324>, &method_adaptor<0x325>, &method_adaptor<0x326>, &method_adaptor<0x327>,
  &method_adaptor<0x328>, &method_adaptor<0x329>, &method_adaptor<0x32a>, &method_adaptor<0x32b>, &method_adaptor<0x32c>, &method_adaptor<0x32d>, &method_adaptor<0x32e>, &method_adaptor<0x32f>,
  &method_adaptor<0x330>, &method_adaptor<0x331>, &method_adaptor<0x332>, &method_adaptor<0x333>, &method_adaptor<0x334>, &method_adaptor<0x335>, &method_adaptor<0x336>, &method_adaptor<0x337>,
  &method_adaptor<0x338>, &method_adaptor<0x339>, &method_adaptor<0x33a>, &method_adaptor<0x33b>, &method_adaptor<0x33c>, &method_adaptor<0x33d>, &method_adaptor<0x33e>, &method_adaptor<0x33f>,
  &method_adaptor<0x340>, &method_adaptor<0x341>, &method_adaptor<0x342>, &method_adaptor<0x343>, &method_adaptor<0x344>, &method_adaptor<0x345>, &method_adaptor<0x346>, &method_adaptor<0x347>,
  &method_adaptor<0x348>, &method_adaptor<0x349>, &method_adaptor<0x34a>, &method_adaptor<0x34b>, &method_adaptor<0x34c>, &method_adaptor<0x34d>, &method_adaptor<0x34e>, &method_adaptor<0x34f>,
  &method_adaptor<0x350>, &method_adaptor<0x351>, &method_adaptor<0x352>, &method_adaptor<0x353>, &method_adaptor<0x354>, &method_adaptor<0x355>, &method_adaptor<0x356>, &method_adaptor<0x357>,
  &method_adaptor<0x358>, &method_adaptor<0x359>, &method_adaptor<0x35a>, &method_adaptor<0x35b>, &method_adaptor<0x35c>, &method_adaptor<0x35d>, &method_adaptor<0x35e>, &method_adaptor<0x35f>,
  &method_adaptor<0x360>, &method_adaptor<0x361>, &method_adaptor<0x362>, &method_adaptor<0x363>, &method_adaptor<0x364>, &method_adaptor<0x365>, &method_adaptor<0x366>, &method_adaptor<0x367>,
  &method_adaptor<0x368>, &method_adaptor<0x369>, &method_adaptor<0x36a>, &method_adaptor<0x36b>, &method_adaptor<0x36c>, &method_adaptor<0x36d>, &method_adaptor<0x36e>, &method_adaptor<0x36f>,
  &method_adaptor<0x370>, &method_adaptor<0x371>, &method_adaptor<0x372>, &method_adaptor<0x373>, &method_adaptor<0x374>, &method_adaptor<0x375>, &method_adaptor<0x376>, &method_adaptor<0x377>,
  &method_adaptor<0x378>, &method_adaptor<0x379>, &method_adaptor<0x37a>, &method_adaptor<0x37b>, &method_adaptor<0x37c>, &method_adaptor<0x37d>, &method_adaptor<0x37e>, &method_adaptor<0x37f>,
  &method_adaptor<0x380>, &method_adaptor<0x381>, &method_adaptor<0x382>, &method_adaptor<0x383>, &method_adaptor<0x384>, &method_adaptor<0x385>, &method_adaptor<0x386>, &method_adaptor<0x387>,
  &method_adaptor<0x388>, &method_adaptor<0x389>, &method_adaptor<0x38a>, &method_adaptor<0x38b>, &method_adaptor<0x38c>, &method_adaptor<0x38d>, &method_adaptor<0x38e>, &method_adaptor<0x38f>,
  &method_adaptor<0x390>, &method_adaptor<0x391>, &method_adaptor<0x392>, &method_adaptor<0x393>, &method_adaptor<0x394>, &method_adaptor<0x395>, &method_adaptor<0x396>, &method_adaptor<0x397>,
  &method_adaptor<0x398>, &method_adaptor<0x399>, &method_adaptor<0x39a>, &method_adaptor<0x39b>, &method_adaptor<0x39c>, &method_adaptor<0x39d>, &method_adaptor<0x39e>, &method_adaptor<0x39f>,
  &method_adaptor<0x3a0>, &method_adaptor<0x3a1>, &method_adaptor<0x3a2>, &method_adaptor<0x3a3>, &method_adaptor<0x3a4>, &method_adaptor<0x3a5>, &method_adaptor<0x3a6>, &method_adaptor<0x3a7>,
  &method_adaptor<0x3a8>, &method_adaptor<0x3a9>, &method_adaptor<0x3aa>, &method_adaptor<0x3ab>, &method_adaptor<0x3ac>, &method_adaptor<0x3ad>, &method_adaptor<0x3ae>, &method_adaptor<0x3af>,
  &method_adaptor<0x3b0>, &method_adaptor<0x3b1>, &method_adaptor<0x3b2>, &method_adaptor<0x3b3>, &method_adaptor<0x3b4>, &method_adaptor<0x3b5>, &method_adaptor<0x3b6>, &method_adaptor<0x3b7>,
  &method_adaptor<0x3b8>, &method_adaptor<0x3b9>, &method_adaptor<0x3ba>, &method_adaptor<0x3bb>, &method_adaptor<0x3bc>, &method_adaptor<0x3bd>, &method_adaptor<0x3be>, &method_adaptor<0x3bf>,
  &method_adaptor<0x3c0>, &method_adaptor<0x3c1>, &method_adaptor<0x3c2>, &method_adaptor<0x3c3>, &method_adaptor<0x3c4>, &method_adaptor<0x3c5>, &method_adaptor<0x3c6>, &method_adaptor<0x3c7>,
  &method_adaptor<0x3c8>, &method_adaptor<0x3c9>, &method_adaptor<0x3ca>, &method_adaptor<0x3cb>, &method_adaptor<0x3cc>, &method_adaptor<0x3cd>, &method_adaptor<0x3ce>, &method_adaptor<0x3cf>,
  &method_adaptor<0x3d0>, &method_adaptor<0x3d1>, &method_adaptor<0x3d2>, &method_adaptor<0x3d3>, &method_adaptor<0x3d4>, &method_adaptor<0x3d5>, &method_adaptor<0x3d6>, &method_adaptor<0x3d7>,
  &method_adaptor<0x3d8>, &method_adaptor<0x3d9>, &method_adaptor<0x3da>, &method_adaptor<0x3db>, &method_adaptor<0x3dc>, &method_adaptor<0x3dd>, &method_adaptor<0x3de>, &method_adaptor<0x3df>,
  &method_adaptor<0x3e0>, &method_adaptor<0x3e1>, &method_adaptor<0x3e2>, &method_adaptor<0x3e3>, &method_adaptor<0x3e4>, &method_adaptor<0x3e5>, &method_adaptor<0x3e6>, &method_adaptor<0x3e7>,
  &method_adaptor<0x3e8>, &method_adaptor<0x3e9>, &method_adaptor<0x3ea>, &method_adaptor<0x3eb>, &method_adaptor<0x3ec>, &method_adaptor<0x3ed>, &method_adaptor<0x3ee>, &method_adaptor<0x3ef>,
  &method_adaptor<0x3f0>, &method_adaptor<0x3f1>, &method_adaptor<0x3f2>, &method_adaptor<0x3f3>, &method_adaptor<0x3f4>, &method_adaptor<0x3f5>, &method_adaptor<0x3f6>, &method_adaptor<0x3f7>,
  &method_adaptor<0x3f8>, &method_adaptor<0x3f9>, &method_adaptor<0x3fa>, &method_adaptor<0x3fb>, &method_adaptor<0x3fc>, &method_adaptor<0x3fd>, &method_adaptor<0x3fe>, &method_adaptor<0x3ff>,
};

//  zero-arguments method adaptors

template <int N> 
VALUE method_adaptor_ctor (int argc, VALUE *argv, VALUE self)
{
  return method_adaptor_n (N, argc, argv, self, true);
}

VALUE (*(method_adaptors_ctor [])) (int, VALUE *, VALUE) =
{
  &method_adaptor_ctor<0x000>, &method_adaptor_ctor<0x001>, &method_adaptor_ctor<0x002>, &method_adaptor_ctor<0x003>, &method_adaptor_ctor<0x004>, &method_adaptor_ctor<0x005>, &method_adaptor_ctor<0x006>, &method_adaptor_ctor<0x007>,
  &method_adaptor_ctor<0x008>, &method_adaptor_ctor<0x009>, &method_adaptor_ctor<0x00a>, &method_adaptor_ctor<0x00b>, &method_adaptor_ctor<0x00c>, &method_adaptor_ctor<0x00d>, &method_adaptor_ctor<0x00e>, &method_adaptor_ctor<0x00f>,
  &method_adaptor_ctor<0x010>, &method_adaptor_ctor<0x011>, &method_adaptor_ctor<0x012>, &method_adaptor_ctor<0x013>, &method_adaptor_ctor<0x014>, &method_adaptor_ctor<0x015>, &method_adaptor_ctor<0x016>, &method_adaptor_ctor<0x017>,
  &method_adaptor_ctor<0x018>, &method_adaptor_ctor<0x019>, &method_adaptor_ctor<0x01a>, &method_adaptor_ctor<0x01b>, &method_adaptor_ctor<0x01c>, &method_adaptor_ctor<0x01d>, &method_adaptor_ctor<0x01e>, &method_adaptor_ctor<0x01f>,
  &method_adaptor_ctor<0x020>, &method_adaptor_ctor<0x021>, &method_adaptor_ctor<0x022>, &method_adaptor_ctor<0x023>, &method_adaptor_ctor<0x024>, &method_adaptor_ctor<0x025>, &method_adaptor_ctor<0x026>, &method_adaptor_ctor<0x027>,
  &method_adaptor_ctor<0x028>, &method_adaptor_ctor<0x029>, &method_adaptor_ctor<0x02a>, &method_adaptor_ctor<0x02b>, &method_adaptor_ctor<0x02c>, &method_adaptor_ctor<0x02d>, &method_adaptor_ctor<0x02e>, &method_adaptor_ctor<0x02f>,
  &method_adaptor_ctor<0x030>, &method_adaptor_ctor<0x031>, &method_adaptor_ctor<0x032>, &method_adaptor_ctor<0x033>, &method_adaptor_ctor<0x034>, &method_adaptor_ctor<0x035>, &method_adaptor_ctor<0x036>, &method_adaptor_ctor<0x037>,
  &method_adaptor_ctor<0x038>, &method_adaptor_ctor<0x039>, &method_adaptor_ctor<0x03a>, &method_adaptor_ctor<0x03b>, &method_adaptor_ctor<0x03c>, &method_adaptor_ctor<0x03d>, &method_adaptor_ctor<0x03e>, &method_adaptor_ctor<0x03f>,
  &method_adaptor_ctor<0x040>, &method_adaptor_ctor<0x041>, &method_adaptor_ctor<0x042>, &method_adaptor_ctor<0x043>, &method_adaptor_ctor<0x044>, &method_adaptor_ctor<0x045>, &method_adaptor_ctor<0x046>, &method_adaptor_ctor<0x047>,
  &method_adaptor_ctor<0x048>, &method_adaptor_ctor<0x049>, &method_adaptor_ctor<0x04a>, &method_adaptor_ctor<0x04b>, &method_adaptor_ctor<0x04c>, &method_adaptor_ctor<0x04d>, &method_adaptor_ctor<0x04e>, &method_adaptor_ctor<0x04f>,
  &method_adaptor_ctor<0x050>, &method_adaptor_ctor<0x051>, &method_adaptor_ctor<0x052>, &method_adaptor_ctor<0x053>, &method_adaptor_ctor<0x054>, &method_adaptor_ctor<0x055>, &method_adaptor_ctor<0x056>, &method_adaptor_ctor<0x057>,
  &method_adaptor_ctor<0x058>, &method_adaptor_ctor<0x059>, &method_adaptor_ctor<0x05a>, &method_adaptor_ctor<0x05b>, &method_adaptor_ctor<0x05c>, &method_adaptor_ctor<0x05d>, &method_adaptor_ctor<0x05e>, &method_adaptor_ctor<0x05f>,
  &method_adaptor_ctor<0x060>, &method_adaptor_ctor<0x061>, &method_adaptor_ctor<0x062>, &method_adaptor_ctor<0x063>, &method_adaptor_ctor<0x064>, &method_adaptor_ctor<0x065>, &method_adaptor_ctor<0x066>, &method_adaptor_ctor<0x067>,
  &method_adaptor_ctor<0x068>, &method_adaptor_ctor<0x069>, &method_adaptor_ctor<0x06a>, &method_adaptor_ctor<0x06b>, &method_adaptor_ctor<0x06c>, &method_adaptor_ctor<0x06d>, &method_adaptor_ctor<0x06e>, &method_adaptor_ctor<0x06f>,
  &method_adaptor_ctor<0x070>, &method_adaptor_ctor<0x071>, &method_adaptor_ctor<0x072>, &method_adaptor_ctor<0x073>, &method_adaptor_ctor<0x074>, &method_adaptor_ctor<0x075>, &method_adaptor_ctor<0x076>, &method_adaptor_ctor<0x077>,
  &method_adaptor_ctor<0x078>, &method_adaptor_ctor<0x079>, &method_adaptor_ctor<0x07a>, &method_adaptor_ctor<0x07b>, &method_adaptor_ctor<0x07c>, &method_adaptor_ctor<0x07d>, &method_adaptor_ctor<0x07e>, &method_adaptor_ctor<0x07f>,
  &method_adaptor_ctor<0x080>, &method_adaptor_ctor<0x081>, &method_adaptor_ctor<0x082>, &method_adaptor_ctor<0x083>, &method_adaptor_ctor<0x084>, &method_adaptor_ctor<0x085>, &method_adaptor_ctor<0x086>, &method_adaptor_ctor<0x087>,
  &method_adaptor_ctor<0x088>, &method_adaptor_ctor<0x089>, &method_adaptor_ctor<0x08a>, &method_adaptor_ctor<0x08b>, &method_adaptor_ctor<0x08c>, &method_adaptor_ctor<0x08d>, &method_adaptor_ctor<0x08e>, &method_adaptor_ctor<0x08f>,
  &method_adaptor_ctor<0x090>, &method_adaptor_ctor<0x091>, &method_adaptor_ctor<0x092>, &method_adaptor_ctor<0x093>, &method_adaptor_ctor<0x094>, &method_adaptor_ctor<0x095>, &method_adaptor_ctor<0x096>, &method_adaptor_ctor<0x097>,
  &method_adaptor_ctor<0x098>, &method_adaptor_ctor<0x099>, &method_adaptor_ctor<0x09a>, &method_adaptor_ctor<0x09b>, &method_adaptor_ctor<0x09c>, &method_adaptor_ctor<0x09d>, &method_adaptor_ctor<0x09e>, &method_adaptor_ctor<0x09f>,
  &method_adaptor_ctor<0x0a0>, &method_adaptor_ctor<0x0a1>, &method_adaptor_ctor<0x0a2>, &method_adaptor_ctor<0x0a3>, &method_adaptor_ctor<0x0a4>, &method_adaptor_ctor<0x0a5>, &method_adaptor_ctor<0x0a6>, &method_adaptor_ctor<0x0a7>,
  &method_adaptor_ctor<0x0a8>, &method_adaptor_ctor<0x0a9>, &method_adaptor_ctor<0x0aa>, &method_adaptor_ctor<0x0ab>, &method_adaptor_ctor<0x0ac>, &method_adaptor_ctor<0x0ad>, &method_adaptor_ctor<0x0ae>, &method_adaptor_ctor<0x0af>,
  &method_adaptor_ctor<0x0b0>, &method_adaptor_ctor<0x0b1>, &method_adaptor_ctor<0x0b2>, &method_adaptor_ctor<0x0b3>, &method_adaptor_ctor<0x0b4>, &method_adaptor_ctor<0x0b5>, &method_adaptor_ctor<0x0b6>, &method_adaptor_ctor<0x0b7>,
  &method_adaptor_ctor<0x0b8>, &method_adaptor_ctor<0x0b9>, &method_adaptor_ctor<0x0ba>, &method_adaptor_ctor<0x0bb>, &method_adaptor_ctor<0x0bc>, &method_adaptor_ctor<0x0bd>, &method_adaptor_ctor<0x0be>, &method_adaptor_ctor<0x0bf>,
  &method_adaptor_ctor<0x0c0>, &method_adaptor_ctor<0x0c1>, &method_adaptor_ctor<0x0c2>, &method_adaptor_ctor<0x0c3>, &method_adaptor_ctor<0x0c4>, &method_adaptor_ctor<0x0c5>, &method_adaptor_ctor<0x0c6>, &method_adaptor_ctor<0x0c7>,
  &method_adaptor_ctor<0x0c8>, &method_adaptor_ctor<0x0c9>, &method_adaptor_ctor<0x0ca>, &method_adaptor_ctor<0x0cb>, &method_adaptor_ctor<0x0cc>, &method_adaptor_ctor<0x0cd>, &method_adaptor_ctor<0x0ce>, &method_adaptor_ctor<0x0cf>,
  &method_adaptor_ctor<0x0d0>, &method_adaptor_ctor<0x0d1>, &method_adaptor_ctor<0x0d2>, &method_adaptor_ctor<0x0d3>, &method_adaptor_ctor<0x0d4>, &method_adaptor_ctor<0x0d5>, &method_adaptor_ctor<0x0d6>, &method_adaptor_ctor<0x0d7>,
  &method_adaptor_ctor<0x0d8>, &method_adaptor_ctor<0x0d9>, &method_adaptor_ctor<0x0da>, &method_adaptor_ctor<0x0db>, &method_adaptor_ctor<0x0dc>, &method_adaptor_ctor<0x0dd>, &method_adaptor_ctor<0x0de>, &method_adaptor_ctor<0x0df>,
  &method_adaptor_ctor<0x0e0>, &method_adaptor_ctor<0x0e1>, &method_adaptor_ctor<0x0e2>, &method_adaptor_ctor<0x0e3>, &method_adaptor_ctor<0x0e4>, &method_adaptor_ctor<0x0e5>, &method_adaptor_ctor<0x0e6>, &method_adaptor_ctor<0x0e7>,
  &method_adaptor_ctor<0x0e8>, &method_adaptor_ctor<0x0e9>, &method_adaptor_ctor<0x0ea>, &method_adaptor_ctor<0x0eb>, &method_adaptor_ctor<0x0ec>, &method_adaptor_ctor<0x0ed>, &method_adaptor_ctor<0x0ee>, &method_adaptor_ctor<0x0ef>,
  &method_adaptor_ctor<0x0f0>, &method_adaptor_ctor<0x0f1>, &method_adaptor_ctor<0x0f2>, &method_adaptor_ctor<0x0f3>, &method_adaptor_ctor<0x0f4>, &method_adaptor_ctor<0x0f5>, &method_adaptor_ctor<0x0f6>, &method_adaptor_ctor<0x0f7>,
  &method_adaptor_ctor<0x0f8>, &method_adaptor_ctor<0x0f9>, &method_adaptor_ctor<0x0fa>, &method_adaptor_ctor<0x0fb>, &method_adaptor_ctor<0x0fc>, &method_adaptor_ctor<0x0fd>, &method_adaptor_ctor<0x0fe>, &method_adaptor_ctor<0x0ff>,
  &method_adaptor_ctor<0x100>, &method_adaptor_ctor<0x101>, &method_adaptor_ctor<0x102>, &method_adaptor_ctor<0x103>, &method_adaptor_ctor<0x104>, &method_adaptor_ctor<0x105>, &method_adaptor_ctor<0x106>, &method_adaptor_ctor<0x107>,
  &method_adaptor_ctor<0x108>, &method_adaptor_ctor<0x109>, &method_adaptor_ctor<0x10a>, &method_adaptor_ctor<0x10b>, &method_adaptor_ctor<0x10c>, &method_adaptor_ctor<0x10d>, &method_adaptor_ctor<0x10e>, &method_adaptor_ctor<0x10f>,
  &method_adaptor_ctor<0x110>, &method_adaptor_ctor<0x111>, &method_adaptor_ctor<0x112>, &method_adaptor_ctor<0x113>, &method_adaptor_ctor<0x114>, &method_adaptor_ctor<0x115>, &method_adaptor_ctor<0x116>, &method_adaptor_ctor<0x117>,
  &method_adaptor_ctor<0x118>, &method_adaptor_ctor<0x119>, &method_adaptor_ctor<0x11a>, &method_adaptor_ctor<0x11b>, &method_adaptor_ctor<0x11c>, &method_adaptor_ctor<0x11d>, &method_adaptor_ctor<0x11e>, &method_adaptor_ctor<0x11f>,
  &method_adaptor_ctor<0x120>, &method_adaptor_ctor<0x121>, &method_adaptor_ctor<0x122>, &method_adaptor_ctor<0x123>, &method_adaptor_ctor<0x124>, &method_adaptor_ctor<0x125>, &method_adaptor_ctor<0x126>, &method_adaptor_ctor<0x127>,
  &method_adaptor_ctor<0x128>, &method_adaptor_ctor<0x129>, &method_adaptor_ctor<0x12a>, &method_adaptor_ctor<0x12b>, &method_adaptor_ctor<0x12c>, &method_adaptor_ctor<0x12d>, &method_adaptor_ctor<0x12e>, &method_adaptor_ctor<0x12f>,
  &method_adaptor_ctor<0x130>, &method_adaptor_ctor<0x131>, &method_adaptor_ctor<0x132>, &method_adaptor_ctor<0x133>, &method_adaptor_ctor<0x134>, &method_adaptor_ctor<0x135>, &method_adaptor_ctor<0x136>, &method_adaptor_ctor<0x137>,
  &method_adaptor_ctor<0x138>, &method_adaptor_ctor<0x139>, &method_adaptor_ctor<0x13a>, &method_adaptor_ctor<0x13b>, &method_adaptor_ctor<0x13c>, &method_adaptor_ctor<0x13d>, &method_adaptor_ctor<0x13e>, &method_adaptor_ctor<0x13f>,
  &method_adaptor_ctor<0x140>, &method_adaptor_ctor<0x141>, &method_adaptor_ctor<0x142>, &method_adaptor_ctor<0x143>, &method_adaptor_ctor<0x144>, &method_adaptor_ctor<0x145>, &method_adaptor_ctor<0x146>, &method_adaptor_ctor<0x147>,
  &method_adaptor_ctor<0x148>, &method_adaptor_ctor<0x149>, &method_adaptor_ctor<0x14a>, &method_adaptor_ctor<0x14b>, &method_adaptor_ctor<0x14c>, &method_adaptor_ctor<0x14d>, &method_adaptor_ctor<0x14e>, &method_adaptor_ctor<0x14f>,
  &method_adaptor_ctor<0x150>, &method_adaptor_ctor<0x151>, &method_adaptor_ctor<0x152>, &method_adaptor_ctor<0x153>, &method_adaptor_ctor<0x154>, &method_adaptor_ctor<0x155>, &method_adaptor_ctor<0x156>, &method_adaptor_ctor<0x157>,
  &method_adaptor_ctor<0x158>, &method_adaptor_ctor<0x159>, &method_adaptor_ctor<0x15a>, &method_adaptor_ctor<0x15b>, &method_adaptor_ctor<0x15c>, &method_adaptor_ctor<0x15d>, &method_adaptor_ctor<0x15e>, &method_adaptor_ctor<0x15f>,
  &method_adaptor_ctor<0x160>, &method_adaptor_ctor<0x161>, &method_adaptor_ctor<0x162>, &method_adaptor_ctor<0x163>, &method_adaptor_ctor<0x164>, &method_adaptor_ctor<0x165>, &method_adaptor_ctor<0x166>, &method_adaptor_ctor<0x167>,
  &method_adaptor_ctor<0x168>, &method_adaptor_ctor<0x169>, &method_adaptor_ctor<0x16a>, &method_adaptor_ctor<0x16b>, &method_adaptor_ctor<0x16c>, &method_adaptor_ctor<0x16d>, &method_adaptor_ctor<0x16e>, &method_adaptor_ctor<0x16f>,
  &method_adaptor_ctor<0x170>, &method_adaptor_ctor<0x171>, &method_adaptor_ctor<0x172>, &method_adaptor_ctor<0x173>, &method_adaptor_ctor<0x174>, &method_adaptor_ctor<0x175>, &method_adaptor_ctor<0x176>, &method_adaptor_ctor<0x177>,
  &method_adaptor_ctor<0x178>, &method_adaptor_ctor<0x179>, &method_adaptor_ctor<0x17a>, &method_adaptor_ctor<0x17b>, &method_adaptor_ctor<0x17c>, &method_adaptor_ctor<0x17d>, &method_adaptor_ctor<0x17e>, &method_adaptor_ctor<0x17f>,
  &method_adaptor_ctor<0x180>, &method_adaptor_ctor<0x181>, &method_adaptor_ctor<0x182>, &method_adaptor_ctor<0x183>, &method_adaptor_ctor<0x184>, &method_adaptor_ctor<0x185>, &method_adaptor_ctor<0x186>, &method_adaptor_ctor<0x187>,
  &method_adaptor_ctor<0x188>, &method_adaptor_ctor<0x189>, &method_adaptor_ctor<0x18a>, &method_adaptor_ctor<0x18b>, &method_adaptor_ctor<0x18c>, &method_adaptor_ctor<0x18d>, &method_adaptor_ctor<0x18e>, &method_adaptor_ctor<0x18f>,
  &method_adaptor_ctor<0x190>, &method_adaptor_ctor<0x191>, &method_adaptor_ctor<0x192>, &method_adaptor_ctor<0x193>, &method_adaptor_ctor<0x194>, &method_adaptor_ctor<0x195>, &method_adaptor_ctor<0x196>, &method_adaptor_ctor<0x197>,
  &method_adaptor_ctor<0x198>, &method_adaptor_ctor<0x199>, &method_adaptor_ctor<0x19a>, &method_adaptor_ctor<0x19b>, &method_adaptor_ctor<0x19c>, &method_adaptor_ctor<0x19d>, &method_adaptor_ctor<0x19e>, &method_adaptor_ctor<0x19f>,
  &method_adaptor_ctor<0x1a0>, &method_adaptor_ctor<0x1a1>, &method_adaptor_ctor<0x1a2>, &method_adaptor_ctor<0x1a3>, &method_adaptor_ctor<0x1a4>, &method_adaptor_ctor<0x1a5>, &method_adaptor_ctor<0x1a6>, &method_adaptor_ctor<0x1a7>,
  &method_adaptor_ctor<0x1a8>, &method_adaptor_ctor<0x1a9>, &method_adaptor_ctor<0x1aa>, &method_adaptor_ctor<0x1ab>, &method_adaptor_ctor<0x1ac>, &method_adaptor_ctor<0x1ad>, &method_adaptor_ctor<0x1ae>, &method_adaptor_ctor<0x1af>,
  &method_adaptor_ctor<0x1b0>, &method_adaptor_ctor<0x1b1>, &method_adaptor_ctor<0x1b2>, &method_adaptor_ctor<0x1b3>, &method_adaptor_ctor<0x1b4>, &method_adaptor_ctor<0x1b5>, &method_adaptor_ctor<0x1b6>, &method_adaptor_ctor<0x1b7>,
  &method_adaptor_ctor<0x1b8>, &method_adaptor_ctor<0x1b9>, &method_adaptor_ctor<0x1ba>, &method_adaptor_ctor<0x1bb>, &method_adaptor_ctor<0x1bc>, &method_adaptor_ctor<0x1bd>, &method_adaptor_ctor<0x1be>, &method_adaptor_ctor<0x1bf>,
  &method_adaptor_ctor<0x1c0>, &method_adaptor_ctor<0x1c1>, &method_adaptor_ctor<0x1c2>, &method_adaptor_ctor<0x1c3>, &method_adaptor_ctor<0x1c4>, &method_adaptor_ctor<0x1c5>, &method_adaptor_ctor<0x1c6>, &method_adaptor_ctor<0x1c7>,
  &method_adaptor_ctor<0x1c8>, &method_adaptor_ctor<0x1c9>, &method_adaptor_ctor<0x1ca>, &method_adaptor_ctor<0x1cb>, &method_adaptor_ctor<0x1cc>, &method_adaptor_ctor<0x1cd>, &method_adaptor_ctor<0x1ce>, &method_adaptor_ctor<0x1cf>,
  &method_adaptor_ctor<0x1d0>, &method_adaptor_ctor<0x1d1>, &method_adaptor_ctor<0x1d2>, &method_adaptor_ctor<0x1d3>, &method_adaptor_ctor<0x1d4>, &method_adaptor_ctor<0x1d5>, &method_adaptor_ctor<0x1d6>, &method_adaptor_ctor<0x1d7>,
  &method_adaptor_ctor<0x1d8>, &method_adaptor_ctor<0x1d9>, &method_adaptor_ctor<0x1da>, &method_adaptor_ctor<0x1db>, &method_adaptor_ctor<0x1dc>, &method_adaptor_ctor<0x1dd>, &method_adaptor_ctor<0x1de>, &method_adaptor_ctor<0x1df>,
  &method_adaptor_ctor<0x1e0>, &method_adaptor_ctor<0x1e1>, &method_adaptor_ctor<0x1e2>, &method_adaptor_ctor<0x1e3>, &method_adaptor_ctor<0x1e4>, &method_adaptor_ctor<0x1e5>, &method_adaptor_ctor<0x1e6>, &method_adaptor_ctor<0x1e7>,
  &method_adaptor_ctor<0x1e8>, &method_adaptor_ctor<0x1e9>, &method_adaptor_ctor<0x1ea>, &method_adaptor_ctor<0x1eb>, &method_adaptor_ctor<0x1ec>, &method_adaptor_ctor<0x1ed>, &method_adaptor_ctor<0x1ee>, &method_adaptor_ctor<0x1ef>,
  &method_adaptor_ctor<0x1f0>, &method_adaptor_ctor<0x1f1>, &method_adaptor_ctor<0x1f2>, &method_adaptor_ctor<0x1f3>, &method_adaptor_ctor<0x1f4>, &method_adaptor_ctor<0x1f5>, &method_adaptor_ctor<0x1f6>, &method_adaptor_ctor<0x1f7>,
  &method_adaptor_ctor<0x1f8>, &method_adaptor_ctor<0x1f9>, &method_adaptor_ctor<0x1fa>, &method_adaptor_ctor<0x1fb>, &method_adaptor_ctor<0x1fc>, &method_adaptor_ctor<0x1fd>, &method_adaptor_ctor<0x1fe>, &method_adaptor_ctor<0x1ff>,
  &method_adaptor_ctor<0x200>, &method_adaptor_ctor<0x201>, &method_adaptor_ctor<0x202>, &method_adaptor_ctor<0x203>, &method_adaptor_ctor<0x204>, &method_adaptor_ctor<0x205>, &method_adaptor_ctor<0x206>, &method_adaptor_ctor<0x207>,
  &method_adaptor_ctor<0x208>, &method_adaptor_ctor<0x209>, &method_adaptor_ctor<0x20a>, &method_adaptor_ctor<0x20b>, &method_adaptor_ctor<0x20c>, &method_adaptor_ctor<0x20d>, &method_adaptor_ctor<0x20e>, &method_adaptor_ctor<0x20f>,
  &method_adaptor_ctor<0x210>, &method_adaptor_ctor<0x211>, &method_adaptor_ctor<0x212>, &method_adaptor_ctor<0x213>, &method_adaptor_ctor<0x214>, &method_adaptor_ctor<0x215>, &method_adaptor_ctor<0x216>, &method_adaptor_ctor<0x217>,
  &method_adaptor_ctor<0x218>, &method_adaptor_ctor<0x219>, &method_adaptor_ctor<0x21a>, &method_adaptor_ctor<0x21b>, &method_adaptor_ctor<0x21c>, &method_adaptor_ctor<0x21d>, &method_adaptor_ctor<0x21e>, &method_adaptor_ctor<0x21f>,
  &method_adaptor_ctor<0x220>, &method_adaptor_ctor<0x221>, &method_adaptor_ctor<0x222>, &method_adaptor_ctor<0x223>, &method_adaptor_ctor<0x224>, &method_adaptor_ctor<0x225>, &method_adaptor_ctor<0x226>, &method_adaptor_ctor<0x227>,
  &method_adaptor_ctor<0x228>, &method_adaptor_ctor<0x229>, &method_adaptor_ctor<0x22a>, &method_adaptor_ctor<0x22b>, &method_adaptor_ctor<0x22c>, &method_adaptor_ctor<0x22d>, &method_adaptor_ctor<0x22e>, &method_adaptor_ctor<0x22f>,
  &method_adaptor_ctor<0x230>, &method_adaptor_ctor<0x231>, &method_adaptor_ctor<0x232>, &method_adaptor_ctor<0x233>, &method_adaptor_ctor<0x234>, &method_adaptor_ctor<0x235>, &method_adaptor_ctor<0x236>, &method_adaptor_ctor<0x237>,
  &method_adaptor_ctor<0x238>, &method_adaptor_ctor<0x239>, &method_adaptor_ctor<0x23a>, &method_adaptor_ctor<0x23b>, &method_adaptor_ctor<0x23c>, &method_adaptor_ctor<0x23d>, &method_adaptor_ctor<0x23e>, &method_adaptor_ctor<0x23f>,
  &method_adaptor_ctor<0x240>, &method_adaptor_ctor<0x241>, &method_adaptor_ctor<0x242>, &method_adaptor_ctor<0x243>, &method_adaptor_ctor<0x244>, &method_adaptor_ctor<0x245>, &method_adaptor_ctor<0x246>, &method_adaptor_ctor<0x247>,
  &method_adaptor_ctor<0x248>, &method_adaptor_ctor<0x249>, &method_adaptor_ctor<0x24a>, &method_adaptor_ctor<0x24b>, &method_adaptor_ctor<0x24c>, &method_adaptor_ctor<0x24d>, &method_adaptor_ctor<0x24e>, &method_adaptor_ctor<0x24f>,
  &method_adaptor_ctor<0x250>, &method_adaptor_ctor<0x251>, &method_adaptor_ctor<0x252>, &method_adaptor_ctor<0x253>, &method_adaptor_ctor<0x254>, &method_adaptor_ctor<0x255>, &method_adaptor_ctor<0x256>, &method_adaptor_ctor<0x257>,
  &method_adaptor_ctor<0x258>, &method_adaptor_ctor<0x259>, &method_adaptor_ctor<0x25a>, &method_adaptor_ctor<0x25b>, &method_adaptor_ctor<0x25c>, &method_adaptor_ctor<0x25d>, &method_adaptor_ctor<0x25e>, &method_adaptor_ctor<0x25f>,
  &method_adaptor_ctor<0x260>, &method_adaptor_ctor<0x261>, &method_adaptor_ctor<0x262>, &method_adaptor_ctor<0x263>, &method_adaptor_ctor<0x264>, &method_adaptor_ctor<0x265>, &method_adaptor_ctor<0x266>, &method_adaptor_ctor<0x267>,
  &method_adaptor_ctor<0x268>, &method_adaptor_ctor<0x269>, &method_adaptor_ctor<0x26a>, &method_adaptor_ctor<0x26b>, &method_adaptor_ctor<0x26c>, &method_adaptor_ctor<0x26d>, &method_adaptor_ctor<0x26e>, &method_adaptor_ctor<0x26f>,
  &method_adaptor_ctor<0x270>, &method_adaptor_ctor<0x271>, &method_adaptor_ctor<0x272>, &method_adaptor_ctor<0x273>, &method_adaptor_ctor<0x274>, &method_adaptor_ctor<0x275>, &method_adaptor_ctor<0x276>, &method_adaptor_ctor<0x277>,
  &method_adaptor_ctor<0x278>, &method_adaptor_ctor<0x279>, &method_adaptor_ctor<0x27a>, &method_adaptor_ctor<0x27b>, &method_adaptor_ctor<0x27c>, &method_adaptor_ctor<0x27d>, &method_adaptor_ctor<0x27e>, &method_adaptor_ctor<0x27f>,
  &method_adaptor_ctor<0x280>, &method_adaptor_ctor<0x281>, &method_adaptor_ctor<0x282>, &method_adaptor_ctor<0x283>, &method_adaptor_ctor<0x284>, &method_adaptor_ctor<0x285>, &method_adaptor_ctor<0x286>, &method_adaptor_ctor<0x287>,
  &method_adaptor_ctor<0x288>, &method_adaptor_ctor<0x289>, &method_adaptor_ctor<0x28a>, &method_adaptor_ctor<0x28b>, &method_adaptor_ctor<0x28c>, &method_adaptor_ctor<0x28d>, &method_adaptor_ctor<0x28e>, &method_adaptor_ctor<0x28f>,
  &method_adaptor_ctor<0x290>, &method_adaptor_ctor<0x291>, &method_adaptor_ctor<0x292>, &method_adaptor_ctor<0x293>, &method_adaptor_ctor<0x294>, &method_adaptor_ctor<0x295>, &method_adaptor_ctor<0x296>, &method_adaptor_ctor<0x297>,
  &method_adaptor_ctor<0x298>, &method_adaptor_ctor<0x299>, &method_adaptor_ctor<0x29a>, &method_adaptor_ctor<0x29b>, &method_adaptor_ctor<0x29c>, &method_adaptor_ctor<0x29d>, &method_adaptor_ctor<0x29e>, &method_adaptor_ctor<0x29f>,
  &method_adaptor_ctor<0x2a0>, &method_adaptor_ctor<0x2a1>, &method_adaptor_ctor<0x2a2>, &method_adaptor_ctor<0x2a3>, &method_adaptor_ctor<0x2a4>, &method_adaptor_ctor<0x2a5>, &method_adaptor_ctor<0x2a6>, &method_adaptor_ctor<0x2a7>,
  &method_adaptor_ctor<0x2a8>, &method_adaptor_ctor<0x2a9>, &method_adaptor_ctor<0x2aa>, &method_adaptor_ctor<0x2ab>, &method_adaptor_ctor<0x2ac>, &method_adaptor_ctor<0x2ad>, &method_adaptor_ctor<0x2ae>, &method_adaptor_ctor<0x2af>,
  &method_adaptor_ctor<0x2b0>, &method_adaptor_ctor<0x2b1>, &method_adaptor_ctor<0x2b2>, &method_adaptor_ctor<0x2b3>, &method_adaptor_ctor<0x2b4>, &method_adaptor_ctor<0x2b5>, &method_adaptor_ctor<0x2b6>, &method_adaptor_ctor<0x2b7>,
  &method_adaptor_ctor<0x2b8>, &method_adaptor_ctor<0x2b9>, &method_adaptor_ctor<0x2ba>, &method_adaptor_ctor<0x2bb>, &method_adaptor_ctor<0x2bc>, &method_adaptor_ctor<0x2bd>, &method_adaptor_ctor<0x2be>, &method_adaptor_ctor<0x2bf>,
  &method_adaptor_ctor<0x2c0>, &method_adaptor_ctor<0x2c1>, &method_adaptor_ctor<0x2c2>, &method_adaptor_ctor<0x2c3>, &method_adaptor_ctor<0x2c4>, &method_adaptor_ctor<0x2c5>, &method_adaptor_ctor<0x2c6>, &method_adaptor_ctor<0x2c7>,
  &method_adaptor_ctor<0x2c8>, &method_adaptor_ctor<0x2c9>, &method_adaptor_ctor<0x2ca>, &method_adaptor_ctor<0x2cb>, &method_adaptor_ctor<0x2cc>, &method_adaptor_ctor<0x2cd>, &method_adaptor_ctor<0x2ce>, &method_adaptor_ctor<0x2cf>,
  &method_adaptor_ctor<0x2d0>, &method_adaptor_ctor<0x2d1>, &method_adaptor_ctor<0x2d2>, &method_adaptor_ctor<0x2d3>, &method_adaptor_ctor<0x2d4>, &method_adaptor_ctor<0x2d5>, &method_adaptor_ctor<0x2d6>, &method_adaptor_ctor<0x2d7>,
  &method_adaptor_ctor<0x2d8>, &method_adaptor_ctor<0x2d9>, &method_adaptor_ctor<0x2da>, &method_adaptor_ctor<0x2db>, &method_adaptor_ctor<0x2dc>, &method_adaptor_ctor<0x2dd>, &method_adaptor_ctor<0x2de>, &method_adaptor_ctor<0x2df>,
  &method_adaptor_ctor<0x2e0>, &method_adaptor_ctor<0x2e1>, &method_adaptor_ctor<0x2e2>, &method_adaptor_ctor<0x2e3>, &method_adaptor_ctor<0x2e4>, &method_adaptor_ctor<0x2e5>, &method_adaptor_ctor<0x2e6>, &method_adaptor_ctor<0x2e7>,
  &method_adaptor_ctor<0x2e8>, &method_adaptor_ctor<0x2e9>, &method_adaptor_ctor<0x2ea>, &method_adaptor_ctor<0x2eb>, &method_adaptor_ctor<0x2ec>, &method_adaptor_ctor<0x2ed>, &method_adaptor_ctor<0x2ee>, &method_adaptor_ctor<0x2ef>,
  &method_adaptor_ctor<0x2f0>, &method_adaptor_ctor<0x2f1>, &method_adaptor_ctor<0x2f2>, &method_adaptor_ctor<0x2f3>, &method_adaptor_ctor<0x2f4>, &method_adaptor_ctor<0x2f5>, &method_adaptor_ctor<0x2f6>, &method_adaptor_ctor<0x2f7>,
  &method_adaptor_ctor<0x2f8>, &method_adaptor_ctor<0x2f9>, &method_adaptor_ctor<0x2fa>, &method_adaptor_ctor<0x2fb>, &method_adaptor_ctor<0x2fc>, &method_adaptor_ctor<0x2fd>, &method_adaptor_ctor<0x2fe>, &method_adaptor_ctor<0x2ff>,
  &method_adaptor_ctor<0x300>, &method_adaptor_ctor<0x301>, &method_adaptor_ctor<0x302>, &method_adaptor_ctor<0x303>, &method_adaptor_ctor<0x304>, &method_adaptor_ctor<0x305>, &method_adaptor_ctor<0x306>, &method_adaptor_ctor<0x307>,
  &method_adaptor_ctor<0x308>, &method_adaptor_ctor<0x309>, &method_adaptor_ctor<0x30a>, &method_adaptor_ctor<0x30b>, &method_adaptor_ctor<0x30c>, &method_adaptor_ctor<0x30d>, &method_adaptor_ctor<0x30e>, &method_adaptor_ctor<0x30f>,
  &method_adaptor_ctor<0x310>, &method_adaptor_ctor<0x311>, &method_adaptor_ctor<0x312>, &method_adaptor_ctor<0x313>, &method_adaptor_ctor<0x314>, &method_adaptor_ctor<0x315>, &method_adaptor_ctor<0x316>, &method_adaptor_ctor<0x317>,
  &method_adaptor_ctor<0x318>, &method_adaptor_ctor<0x319>, &method_adaptor_ctor<0x31a>, &method_adaptor_ctor<0x31b>, &method_adaptor_ctor<0x31c>, &method_adaptor_ctor<0x31d>, &method_adaptor_ctor<0x31e>, &method_adaptor_ctor<0x31f>,
  &method_adaptor_ctor<0x320>, &method_adaptor_ctor<0x321>, &method_adaptor_ctor<0x322>, &method_adaptor_ctor<0x323>, &method_adaptor_ctor<0x324>, &method_adaptor_ctor<0x325>, &method_adaptor_ctor<0x326>, &method_adaptor_ctor<0x327>,
  &method_adaptor_ctor<0x328>, &method_adaptor_ctor<0x329>, &method_adaptor_ctor<0x32a>, &method_adaptor_ctor<0x32b>, &method_adaptor_ctor<0x32c>, &method_adaptor_ctor<0x32d>, &method_adaptor_ctor<0x32e>, &method_adaptor_ctor<0x32f>,
  &method_adaptor_ctor<0x330>, &method_adaptor_ctor<0x331>, &method_adaptor_ctor<0x332>, &method_adaptor_ctor<0x333>, &method_adaptor_ctor<0x334>, &method_adaptor_ctor<0x335>, &method_adaptor_ctor<0x336>, &method_adaptor_ctor<0x337>,
  &method_adaptor_ctor<0x338>, &method_adaptor_ctor<0x339>, &method_adaptor_ctor<0x33a>, &method_adaptor_ctor<0x33b>, &method_adaptor_ctor<0x33c>, &method_adaptor_ctor<0x33d>, &method_adaptor_ctor<0x33e>, &method_adaptor_ctor<0x33f>,
  &method_adaptor_ctor<0x340>, &method_adaptor_ctor<0x341>, &method_adaptor_ctor<0x342>, &method_adaptor_ctor<0x343>, &method_adaptor_ctor<0x344>, &method_adaptor_ctor<0x345>, &method_adaptor_ctor<0x346>, &method_adaptor_ctor<0x347>,
  &method_adaptor_ctor<0x348>, &method_adaptor_ctor<0x349>, &method_adaptor_ctor<0x34a>, &method_adaptor_ctor<0x34b>, &method_adaptor_ctor<0x34c>, &method_adaptor_ctor<0x34d>, &method_adaptor_ctor<0x34e>, &method_adaptor_ctor<0x34f>,
  &method_adaptor_ctor<0x350>, &method_adaptor_ctor<0x351>, &method_adaptor_ctor<0x352>, &method_adaptor_ctor<0x353>, &method_adaptor_ctor<0x354>, &method_adaptor_ctor<0x355>, &method_adaptor_ctor<0x356>, &method_adaptor_ctor<0x357>,
  &method_adaptor_ctor<0x358>, &method_adaptor_ctor<0x359>, &method_adaptor_ctor<0x35a>, &method_adaptor_ctor<0x35b>, &method_adaptor_ctor<0x35c>, &method_adaptor_ctor<0x35d>, &method_adaptor_ctor<0x35e>, &method_adaptor_ctor<0x35f>,
  &method_adaptor_ctor<0x360>, &method_adaptor_ctor<0x361>, &method_adaptor_ctor<0x362>, &method_adaptor_ctor<0x363>, &method_adaptor_ctor<0x364>, &method_adaptor_ctor<0x365>, &method_adaptor_ctor<0x366>, &method_adaptor_ctor<0x367>,
  &method_adaptor_ctor<0x368>, &method_adaptor_ctor<0x369>, &method_adaptor_ctor<0x36a>, &method_adaptor_ctor<0x36b>, &method_adaptor_ctor<0x36c>, &method_adaptor_ctor<0x36d>, &method_adaptor_ctor<0x36e>, &method_adaptor_ctor<0x36f>,
  &method_adaptor_ctor<0x370>, &method_adaptor_ctor<0x371>, &method_adaptor_ctor<0x372>, &method_adaptor_ctor<0x373>, &method_adaptor_ctor<0x374>, &method_adaptor_ctor<0x375>, &method_adaptor_ctor<0x376>, &method_adaptor_ctor<0x377>,
  &method_adaptor_ctor<0x378>, &method_adaptor_ctor<0x379>, &method_adaptor_ctor<0x37a>, &method_adaptor_ctor<0x37b>, &method_adaptor_ctor<0x37c>, &method_adaptor_ctor<0x37d>, &method_adaptor_ctor<0x37e>, &method_adaptor_ctor<0x37f>,
  &method_adaptor_ctor<0x380>, &method_adaptor_ctor<0x381>, &method_adaptor_ctor<0x382>, &method_adaptor_ctor<0x383>, &method_adaptor_ctor<0x384>, &method_adaptor_ctor<0x385>, &method_adaptor_ctor<0x386>, &method_adaptor_ctor<0x387>,
  &method_adaptor_ctor<0x388>, &method_adaptor_ctor<0x389>, &method_adaptor_ctor<0x38a>, &method_adaptor_ctor<0x38b>, &method_adaptor_ctor<0x38c>, &method_adaptor_ctor<0x38d>, &method_adaptor_ctor<0x38e>, &method_adaptor_ctor<0x38f>,
  &method_adaptor_ctor<0x390>, &method_adaptor_ctor<0x391>, &method_adaptor_ctor<0x392>, &method_adaptor_ctor<0x393>, &method_adaptor_ctor<0x394>, &method_adaptor_ctor<0x395>, &method_adaptor_ctor<0x396>, &method_adaptor_ctor<0x397>,
  &method_adaptor_ctor<0x398>, &method_adaptor_ctor<0x399>, &method_adaptor_ctor<0x39a>, &method_adaptor_ctor<0x39b>, &method_adaptor_ctor<0x39c>, &method_adaptor_ctor<0x39d>, &method_adaptor_ctor<0x39e>, &method_adaptor_ctor<0x39f>,
  &method_adaptor_ctor<0x3a0>, &method_adaptor_ctor<0x3a1>, &method_adaptor_ctor<0x3a2>, &method_adaptor_ctor<0x3a3>, &method_adaptor_ctor<0x3a4>, &method_adaptor_ctor<0x3a5>, &method_adaptor_ctor<0x3a6>, &method_adaptor_ctor<0x3a7>,
  &method_adaptor_ctor<0x3a8>, &method_adaptor_ctor<0x3a9>, &method_adaptor_ctor<0x3aa>, &method_adaptor_ctor<0x3ab>, &method_adaptor_ctor<0x3ac>, &method_adaptor_ctor<0x3ad>, &method_adaptor_ctor<0x3ae>, &method_adaptor_ctor<0x3af>,
  &method_adaptor_ctor<0x3b0>, &method_adaptor_ctor<0x3b1>, &method_adaptor_ctor<0x3b2>, &method_adaptor_ctor<0x3b3>, &method_adaptor_ctor<0x3b4>, &method_adaptor_ctor<0x3b5>, &method_adaptor_ctor<0x3b6>, &method_adaptor_ctor<0x3b7>,
  &method_adaptor_ctor<0x3b8>, &method_adaptor_ctor<0x3b9>, &method_adaptor_ctor<0x3ba>, &method_adaptor_ctor<0x3bb>, &method_adaptor_ctor<0x3bc>, &method_adaptor_ctor<0x3bd>, &method_adaptor_ctor<0x3be>, &method_adaptor_ctor<0x3bf>,
  &method_adaptor_ctor<0x3c0>, &method_adaptor_ctor<0x3c1>, &method_adaptor_ctor<0x3c2>, &method_adaptor_ctor<0x3c3>, &method_adaptor_ctor<0x3c4>, &method_adaptor_ctor<0x3c5>, &method_adaptor_ctor<0x3c6>, &method_adaptor_ctor<0x3c7>,
  &method_adaptor_ctor<0x3c8>, &method_adaptor_ctor<0x3c9>, &method_adaptor_ctor<0x3ca>, &method_adaptor_ctor<0x3cb>, &method_adaptor_ctor<0x3cc>, &method_adaptor_ctor<0x3cd>, &method_adaptor_ctor<0x3ce>, &method_adaptor_ctor<0x3cf>,
  &method_adaptor_ctor<0x3d0>, &method_adaptor_ctor<0x3d1>, &method_adaptor_ctor<0x3d2>, &method_adaptor_ctor<0x3d3>, &method_adaptor_ctor<0x3d4>, &method_adaptor_ctor<0x3d5>, &method_adaptor_ctor<0x3d6>, &method_adaptor_ctor<0x3d7>,
  &method_adaptor_ctor<0x3d8>, &method_adaptor_ctor<0x3d9>, &method_adaptor_ctor<0x3da>, &method_adaptor_ctor<0x3db>, &method_adaptor_ctor<0x3dc>, &method_adaptor_ctor<0x3dd>, &method_adaptor_ctor<0x3de>, &method_adaptor_ctor<0x3df>,
  &method_adaptor_ctor<0x3e0>, &method_adaptor_ctor<0x3e1>, &method_adaptor_ctor<0x3e2>, &method_adaptor_ctor<0x3e3>, &method_adaptor_ctor<0x3e4>, &method_adaptor_ctor<0x3e5>, &method_adaptor_ctor<0x3e6>, &method_adaptor_ctor<0x3e7>,
  &method_adaptor_ctor<0x3e8>, &method_adaptor_ctor<0x3e9>, &method_adaptor_ctor<0x3ea>, &method_adaptor_ctor<0x3eb>, &method_adaptor_ctor<0x3ec>, &method_adaptor_ctor<0x3ed>, &method_adaptor_ctor<0x3ee>, &method_adaptor_ctor<0x3ef>,
  &method_adaptor_ctor<0x3f0>, &method_adaptor_ctor<0x3f1>, &method_adaptor_ctor<0x3f2>, &method_adaptor_ctor<0x3f3>, &method_adaptor_ctor<0x3f4>, &method_adaptor_ctor<0x3f5>, &method_adaptor_ctor<0x3f6>, &method_adaptor_ctor<0x3f7>,
  &method_adaptor_ctor<0x3f8>, &method_adaptor_ctor<0x3f9>, &method_adaptor_ctor<0x3fa>, &method_adaptor_ctor<0x3fb>, &method_adaptor_ctor<0x3fc>, &method_adaptor_ctor<0x3fd>, &method_adaptor_ctor<0x3fe>, &method_adaptor_ctor<0x3ff>,
};

// --------------------------------------------------------------------------
//  RubyInterpreter implementation

static VALUE 
stdout_write (VALUE self, VALUE a)
{
  if (sp_current_console) {
    sp_current_console->write_str (StringValuePtr (a), gsi::Console::OS_stdout);
  }
  return Qnil;
}

static VALUE 
stdout_flush (VALUE self)
{
  //  nothing yet ..
  return Qnil;
}

static VALUE 
stderr_write (VALUE self, VALUE a)
{
  if (sp_current_console) {
    sp_current_console->write_str (StringValuePtr (a), gsi::Console::OS_stderr);
  }
  return Qnil;
}

static VALUE 
stderr_flush (VALUE self)
{
  //  nothing yet ..
  return Qnil;
}

struct RubyConstDescriptor 
{
  VALUE klass;
  const gsi::MethodBase *meth;
  std::string name;
};


extern "C" void ruby_prog_init();
static void
rba_init ()
{
  const char *program_name = "klayout";

  int argc = 3;
  char *argv[3];
  argv[0] = (char *)program_name;
  // Hint: to keep ruby_options from reading stdin, we simulate a "-e" option with an empty script
  argv[1] = (char *)"-e";
  argv[2] = (char *)"";

#if HAVE_RUBY_VERSION_CODE>=10900
  //  Make sure we call ruby_sysinit because otherwise the program will crash
  char **argvp = argv;
  int argcp = argc;
  ruby_sysinit (&argcp, &argvp);
#endif

  //  Keep ruby_init from changing the SIGINT handler.
  //  The reason for ruby_init doing this is probably to enable the input methods
  //  to receive Ctrl+C. We however do not need this functionality and users complain 
  //  about now having Ctrl+C to kill the application.
  void (*org_sigint) (int) = signal (SIGINT, SIG_DFL);
  ruby_init ();
  signal (SIGINT, org_sigint);

  //  Remove setters for $0 and $PROGRAM_NAME (still both are linked) because
  //  the setter does strange things with the process and the argv, specifically argv[0] above. 
  static VALUE argv0 = Qnil;
  argv0 = rb_gv_get("0");
  rb_define_hooked_variable("$0", &argv0, 0, 0);
  rb_define_hooked_variable("$PROGRAM_NAME", &argv0, 0, 0);
  
  ruby_options(argc, argv);

  typedef VALUE (*ruby_func)(ANYARGS);

  VALUE module = rb_define_module ("RBA");

  //  register the client handler at the GSI side
  gsi::ObjectBase::register_client_handler (new RBAClientHandler (), client_index);

  //  save all constants for later (we cannot declare them while we are still producing classes
  //  because of the enum representative classes and enum constants are important)
  std::vector <RubyConstDescriptor> constants;

  bool more_classes = true;
  while (more_classes) {

    more_classes = false;
    for (gsi::ClassBase::class_iterator c = gsi::ClassBase::begin_classes (); c != gsi::ClassBase::end_classes (); ++c) {

      //  don't handle classes twice
      if (rev_cls_map.find (&*c) != rev_cls_map.end ()) {
        continue;
      }

      bool all_children_available = true;
      for (std::vector<const gsi::ClassBase *>::const_iterator cc = c->begin_child_classes (); cc != c->end_child_classes (); ++cc) {
        tl_assert ((*cc)->declaration () != 0);
        if (rev_cls_map.find ((*cc)->declaration ()) == rev_cls_map.end ()) {
          all_children_available = false;
          break;
        }
      }
      if (! all_children_available) {
        //  can't produce this class yet - the children are not available yet.
        more_classes = true;
        continue;
      }

      VALUE super = rb_cObject;
      if (c->base () != 0) {
        std::map <const gsi::ClassBase *, VALUE>::const_iterator cb = rev_cls_map.find (c->base ());
        if (cb == rev_cls_map.end ()) {
          //  can't produce this class yet. The base class needs to be handled first.
          more_classes = true;
          continue;
        } else {
          super = cb->second;
        }
      }

      //  there should be only main declarations since we merged
      tl_assert (c->declaration () == &*c);

      VALUE klass = rb_define_class_under (module, c->name ().c_str (), super);
      cls_map.insert (std::make_pair (klass, &*c));
      rev_cls_map.insert (std::make_pair (&*c, klass));

      rb_define_alloc_func (klass, alloc_proxy);

      bool has_default_initialize = false;

      MethodTable *mt = MethodTable::method_table_by_class (&*c, true /*force init*/);

      for (gsi::ClassBase::method_iterator m = c->begin_methods (); m != c->end_methods (); ++m) {

        if (! (*m)->is_callback ()) {

          if (! (*m)->is_static ()) {

            bool drop_method = false;
            if ((*m)->smt () == gsi::MethodBase::Dup) {
              //  drop dup method -> replaced by Assign in ctor context
              drop_method = true;
            } else if ((*m)->smt () == gsi::MethodBase::Assign) {
              mt->add_ctor_method ("initialize_copy", *m);
            }

            if (! drop_method) {

              for (gsi::MethodBase::synonym_iterator syn = (*m)->begin_synonyms (); syn != (*m)->end_synonyms (); ++syn) {
                if (syn->is_predicate) {
                  mt->add_method (syn->name, *m);
                  mt->add_method (syn->name + "?", *m);
                } else if (syn->is_setter) {
                  mt->add_method (syn->name + "=", *m);
                } else {
                  mt->add_method (syn->name, *m);
                }
              }

            }

          } else {

            for (gsi::MethodBase::synonym_iterator syn = (*m)->begin_synonyms (); syn != (*m)->end_synonyms (); ++syn) {

              if (isupper (syn->name [0]) && (*m)->begin_arguments () == (*m)->end_arguments ()) {

                //  Static const methods are constants.
                //  Methods without arguments which start with a capital letter are treated as constants
                //  for backward compatibility
                constants.push_back (RubyConstDescriptor ());
                constants.back ().klass = klass;
                constants.back ().meth = *m;
                constants.back ().name = (*m)->begin_synonyms ()->name;

              } else if ((*m)->ret_type ().type () == gsi::T_object && (*m)->ret_type ().pass_obj () && syn->name == "new") {

                //  "new" is mapped to "initialize" member function (special translation of 
                //  member to static is indicated through the "ctor" attribute.
                has_default_initialize = true;
                mt->add_ctor_method ("initialize", *m);

              } else if (syn->is_predicate) {

                mt->add_method (syn->name, *m);
                mt->add_method (syn->name + "?", *m);

              } else if (syn->is_setter) {

                mt->add_method (syn->name + "=", *m);

              } else {

                mt->add_method (syn->name, *m);

              }
            }

          }

        }

      }

      //  clean up the method table
      mt->finish ();

      //  Hint: we need to do static methods before the non-static ones because 
      //  rb_define_module_function creates an private instance method.
      //  If we do the non-static methods afterwards we will make it a public once again.
      //  The order of the names will be "name(non-static), name(static), ..." because 
      //  the static flag is the second member of the key (string, bool) pair.
      for (size_t mid = mt->bottom_mid (); mid < mt->top_mid (); ++mid) {

        if (mt->is_static (mid)) {

          tl_assert (mid < size_t (sizeof (method_adaptors) / sizeof (method_adaptors [0])));

          /* Note: Ruby does not support static protected functions, hence we have them (i.e. QThread::usleep).
           *       Do we silently create public ones from them:
          if (mt->is_protected (mid)) {
            tl::warn << "static '" << mt->name (mid) << "' method cannot be protected in class " << c->name ();
          }
          */

          rb_define_module_function (klass, mt->name (mid).c_str (), (ruby_func) method_adaptors[mid], -1);

        }

      }

      for (size_t mid = mt->bottom_mid (); mid < mt->top_mid (); ++mid) {

        if (mt->is_ctor (mid)) {

          tl_assert (mid < size_t (sizeof (method_adaptors_ctor) / sizeof (method_adaptors_ctor [0])));

          if (! mt->is_protected (mid)) {
            rb_define_method (klass, mt->name (mid).c_str (), (ruby_func) method_adaptors_ctor[mid], -1);
          } else {
            //  a protected constructor needs to be provided in both protected and non-protected mode
            rb_define_method (klass, mt->name (mid).c_str (), (ruby_func) method_adaptors_ctor[mid], -1);
            rb_define_protected_method (klass, mt->name (mid).c_str (), (ruby_func) method_adaptors_ctor[mid], -1);
          }

        } else if (! mt->is_static (mid)) {

          tl_assert (mid < size_t (sizeof (method_adaptors) / sizeof (method_adaptors [0])));

          if (! mt->is_protected (mid)) {
            rb_define_method (klass, mt->name (mid).c_str (), (ruby_func) method_adaptors[mid], -1);
#if HAVE_RUBY_VERSION_CODE>=20000
            //  Ruby 2.x does no longer alias "inspect" to "to_s" automatically, so we have to do this:
            if (mt->name (mid) == "to_s") {
              rb_define_alias (klass, "inspect", "to_s");
            }
#endif
          } else {
            rb_define_protected_method (klass, mt->name (mid).c_str (), (ruby_func) method_adaptors[mid], -1);
          }

        }

      }

      if (! has_default_initialize) {
        rb_define_method (klass, "initialize", (ruby_func) &gsi_init, 0);
      }

    }

  }

  //  produce the child classes as constants
  for (gsi::ClassBase::class_iterator c = gsi::ClassBase::begin_classes (); c != gsi::ClassBase::end_classes (); ++c) {
    for (std::vector<const gsi::ClassBase *>::const_iterator cc = c->begin_child_classes (); cc != c->end_child_classes (); ++cc) {

      std::map <const gsi::ClassBase *, VALUE>::const_iterator cm = rev_cls_map.find (&*c);
      std::map <const gsi::ClassBase *, VALUE>::const_iterator ccm = rev_cls_map.find ((*cc)->declaration ());
      tl_assert (cm != rev_cls_map.end ());
      tl_assert (ccm != rev_cls_map.end ());

      rb_define_const (cm->second, (*cc)->name ().c_str (), ccm->second);

    }
  }

  //  now make the constants
  for (std::vector <RubyConstDescriptor>::const_iterator c = constants.begin (); c != constants.end (); ++c) {

    try {

      gsi::SerialArgs retlist (c->meth->retsize ());
      gsi::SerialArgs arglist (c->meth->argsize ());
      c->meth->call (0, arglist, retlist);
      VALUE ret = pop_arg (c->meth->ret_type (), retlist);
      rb_define_const (c->klass, c->name.c_str (), ret);

    } catch (tl::Exception &ex) {
      tl::warn << "Got exception '" << ex.msg () << "' while defining constant " << c->name;
    }

  }

  //  define new classes for handling stdout and stderr.
  //  use IO as the basis for that.
  stdout_klass = rb_define_class_under (module, "RBAstdout", rb_cIO);
  rb_define_method (stdout_klass, "write", (ruby_func) &stdout_write, 1);
  rb_define_method (stdout_klass, "flush", (ruby_func) &stdout_flush, 0);
  stderr_klass = rb_define_class_under (module, "RBAstderr", rb_cIO);
  rb_define_method (stderr_klass, "write", (ruby_func) &stderr_write, 1);
  rb_define_method (stderr_klass, "flush", (ruby_func) &stderr_flush, 0);

  //  register the saved stdout/stderr objects with the garbage collector
  rb_global_variable (& saved_stdout);
  rb_global_variable (& saved_stderr);

  //  create the handler objects
  VALUE empty_args [] = { INT2NUM (0) };
  saved_stderr = rb_class_new_instance_checked (1, empty_args, stderr_klass);
  saved_stdout = rb_class_new_instance_checked (1, empty_args, stdout_klass);
}

RubyInterpreter::RubyInterpreter () 
{ 
  tl_assert (! rba_ruby_destroyed);
  tl_assert (! rba_interpreter);
  rba_interpreter = this;
  rba_init ();

  //  hopefully this one always remains there:
  ruby_top_self = rb_eval_string ("self");
  rb_gc_register_address (&ruby_top_self);
}

RubyInterpreter::~RubyInterpreter () 
{
  rb_gc_unregister_address (&ruby_top_self);
  ruby_top_self = Qnil;

  ruby_finalize ();
  gsi::ObjectBase::register_client_handler (0, client_index);
  rba_ruby_destroyed = true;
  rba_interpreter = 0;
}

std::string
RubyInterpreter::version () const
{
  try {
    return const_cast<RubyInterpreter *> (this)->eval_expr ("RUBY_VERSION.to_s+'-p'+RUBY_PATCHLEVEL.to_s+' ('+RUBY_PLATFORM+')'").to_string ();
  } catch (...) {
    return std::string ("unknown");
  }
}

void 
RubyInterpreter::initialize ()
{
  // Hint: this macro must be called from as high as possible on the stack. 
  // Ruby does not work properly if any ruby function is called from above the
  // stack. Therefore this method must be called as high as possible in the call
  // stack (i.e. from main).
  // One effect of the fail is that a "require" does not work properly and will
  // abort with a segmentation fault.
#if defined(RUBY_INIT_STACK) // for ruby <=1.8.5.
  RUBY_INIT_STACK;
#endif
}

void 
RubyInterpreter::set_debugger_scope (const std::string &filename)
{
  s_debugger_scope = filename;
}

void 
RubyInterpreter::remove_debugger_scope ()
{
  s_debugger_scope.clear ();
}

void 
RubyInterpreter::ignore_next_exception ()
{
  if (sp_current_exec_handler) {
    s_ignore_next_exception = true;
  }
}

void 
RubyInterpreter::add_path (const std::string &path)
{
  VALUE pv = rb_gv_get ("$:");
  if (pv != Qnil && TYPE (pv) == T_ARRAY) {
    rb_ary_push (pv, rb_str_new (path.c_str (), path.size ()));
  }
}

void
RubyInterpreter::require (const std::string &filename_utf8)
{
  RUBY_BEGIN_EXEC 

    std::string fl (rb_cstring_from_utf8 (filename_utf8));

    rb_set_errinfo (Qnil);
    int error = 0;
    rb_protect_init (); // see above
    rb_protect ((VALUE (*)(VALUE))rb_require, (VALUE)fl.c_str (), &error);
    if (error) {
      rba_check_error ();
    }

  RUBY_END_EXEC 
}

void
RubyInterpreter::load_file (const std::string &filename_utf8)
{
  RUBY_BEGIN_EXEC 

    std::string fl (rb_cstring_from_utf8 (filename_utf8));

    set_progname (rb_str_new (fl.c_str (), fl.size ()));

    rb_set_errinfo (Qnil);
    int error = 0;
    int wrap = 0; // TODO: make variable?
    rb_protect_init (); // see above
    rb_load_protect (rb_str_new (fl.c_str (), fl.size ()), wrap, &error);
    if (error) {
      rba_check_error ();
    }

  RUBY_END_EXEC 
}

void
RubyInterpreter::eval_string (const char *expr, const char *file, int line, int context)
{
  do_eval_string (expr, file, line, context);
}

tl::Variant
RubyInterpreter::eval_expr (const char *expr, const char *file, int line, int context)
{
  VALUE res = do_eval_string (expr, file, line, context);
  if (res != Qnil) {
    return ruby2c<tl::Variant> (res);
  } else {
    return tl::Variant ();
  }
}

void
RubyInterpreter::eval_string_and_print (const char *expr, const char *file, int line, int context)
{
  VALUE res = do_eval_string (expr, file, line, context);
  if (sp_current_console && res != Qnil) {
    VALUE res_s = rba_safe_obj_as_string (res);
    sp_current_console->write_str (StringValuePtr (res_s), gsi::Console::OS_stdout);
    sp_current_console->write_str ("\n", gsi::Console::OS_stdout);
  }
}

void
RubyInterpreter::define_variable (const std::string &name, const std::string &value)
{
  rb_gv_set (name.c_str (), rb_str_new (value.c_str (), value.size ()));
}

gsi::Inspector *
RubyInterpreter::inspector (int context)
{
  //  TODO: Ruby does not provide inspection currently
  return 0;
}

bool
RubyInterpreter::available () const
{
  return true;
}

void 
RubyInterpreter::push_console (gsi::Console *console)
{
  if (! sp_current_console) { 
    std::swap (saved_stderr, rb_stderr);
    std::swap (saved_stdout, rb_stdout);
  } else {
    s_consoles.push_back (sp_current_console);
  }

  sp_current_console = console;
}

void 
RubyInterpreter::remove_console (gsi::Console *console)
{
  if (sp_current_console == console) {

    if (s_consoles.empty ()) {
      sp_current_console = 0;
      std::swap (saved_stderr, rb_stderr);
      std::swap (saved_stdout, rb_stdout);
    } else {
      sp_current_console = s_consoles.back ();
      s_consoles.pop_back ();
    }

  } else {

    for (std::vector<gsi::Console *>::iterator c = s_consoles.begin (); c != s_consoles.end (); ++c) {
      if (*c == console) {
        s_consoles.erase (c);
        break;
      }
    }

  }
}

static size_t
prepare_trace (const char *fn)
{
  s_in_trace = true;
  if (s_current_binding != Qnil) {
    rb_gc_unregister_address (&s_current_binding);
    s_current_binding = Qnil;
  }

  std::map<const char *, size_t>::const_iterator f = s_file_id_map.find (fn);
  if (f == s_file_id_map.end ()) {
    f = s_file_id_map.insert (std::make_pair (fn, sp_current_exec_handler->id_for_path (rba_interpreter, fn))).first;
  } 

  return f->second;
}

static void 
release_binding ()
{
  s_in_trace = false;
  if (s_current_binding != Qnil) {
    rb_gc_unregister_address (&s_current_binding);
    s_current_binding = Qnil;
  }
}

static void
#if HAVE_RUBY_VERSION_CODE<10900
trace_callback (rb_event_t event, NODE *, VALUE self, ID id, VALUE klass)
#else
trace_callback (rb_event_flag_t event, VALUE data, VALUE self, ID id, VALUE klass)
#endif
{
  if (sp_current_exec_handler && !s_in_trace) {

    if ((event & RUBY_EVENT_LINE) != 0) {

      //  see below for a description of s_block_exceptions
      s_block_exceptions = false;

      int line = rb_sourceline ();
      size_t file_id = prepare_trace (rb_sourcefile ());

      RBA_TRY

        if (s_exit_on_next) {
          release_binding ();
          throw gsi::ExitException (0);
        }

        try {
          RubyStackTraceProvider st_provider (s_debugger_scope);
          sp_current_exec_handler->trace (rba_interpreter, file_id, line, &st_provider);
          release_binding ();
        } catch (...) {
          release_binding ();
          throw;
        }

      RBA_CATCH("trace callback")

    } else if ((event & RUBY_EVENT_CALL) != 0) {
      sp_current_exec_handler->push_call_stack (rba_interpreter);
    } else if ((event & RUBY_EVENT_RETURN) != 0) {
      sp_current_exec_handler->pop_call_stack (rba_interpreter);
    } else if ((event & RUBY_EVENT_RAISE) != 0 && ! s_block_exceptions) {

      VALUE lasterr = rb_errinfo ();

      //  Don't catch syntax errors (it does not make sense to stop in the debugger for them).
      //  Sometimes lasterr is nil - we must suppress these errors as well.
      if (lasterr != Qnil && CLASS_OF (lasterr) != rb_eSyntaxError) {

        //  If the next exception shall be ignored, do so
        if (s_ignore_next_exception) {
          s_ignore_next_exception = false;
        } else {

          try {

            int line = rb_sourceline ();
            size_t file_id = prepare_trace (rb_sourcefile ());

            std::string eclass = "<unknown>";
            std::string emsg = "<unknown>";

            VALUE klass = rb_class_path (CLASS_OF (lasterr));
            eclass = std::string (RSTRING_PTR(klass), RSTRING_LEN(klass));
            VALUE message = rb_obj_as_string(lasterr);
            emsg = std::string (RSTRING_PTR(message), RSTRING_LEN(message));

            RubyStackTraceProvider st_provider (s_debugger_scope);
            sp_current_exec_handler->exception_thrown (rba_interpreter, file_id, line, eclass, emsg, &st_provider);

            release_binding ();

          } catch (gsi::ExitException &) {
            release_binding ();
            s_exit_on_next = true; // delayed handling of gsi::ExitException
          } catch (...) {
            //  ignore other exceptions - we can't raise a ruby exception from the trace callback
            //  because this causes a fatal error (exception reentered)
            release_binding ();
          }

        }

        //  Ruby tends to call this callback twice - once from rb_f_raise and then 
        //  from rb_exc_raise. We use the s_block_exceptions flag to suppress the 
        //  second one
        s_block_exceptions = true;

      }

    }

  }

}

void 
RubyInterpreter::push_exec_handler (gsi::ExecutionHandler *h)
{
  if (sp_current_exec_handler) {

    s_exec_handlers.push_back (sp_current_exec_handler);

  } else {

    rb_remove_event_hook(trace_callback);
#if HAVE_RUBY_VERSION_CODE<10900
    rb_add_event_hook(trace_callback, RUBY_EVENT_ALL);
#else
    rb_add_event_hook(trace_callback, RUBY_EVENT_ALL, Qnil);
#endif

  }

  sp_current_exec_handler = h;
  s_file_id_map.clear ();

  //  if we happen to push the exec handler inside the execution, 
  //  signal start of execution
  if (s_current_exec_level > 0) {
    sp_current_exec_handler->start_exec (rba_interpreter);
  }
}

void 
RubyInterpreter::remove_exec_handler (gsi::ExecutionHandler *exec_handler)
{
  if (sp_current_exec_handler == exec_handler) {

    //  if we happen to remove the exec handler inside the execution, 
    //  signal end of execution
    if (s_current_exec_level > 0) {
      sp_current_exec_handler->end_exec (rba_interpreter);
    }

    if (s_exec_handlers.empty ()) {
      sp_current_exec_handler = 0;
      rb_remove_event_hook(trace_callback);
    } else {
      sp_current_exec_handler = s_exec_handlers.back ();
      s_exec_handlers.pop_back ();
    }

  } else {

    for (std::vector<gsi::ExecutionHandler *>::iterator eh = s_exec_handlers.begin (); eh != s_exec_handlers.end (); ++eh) {
      if (*eh == exec_handler) {
        s_exec_handlers.erase (eh);
        break;
      }
    }

  }

}

}

#else

namespace rba
{

static void fail (const char *file, int line)
{
  throw gsi::RuntimeError (tl::to_string (QObject::tr ("Ruby support not compiled in")).c_str (), file, line, "missing_feature", std::vector<gsi::BacktraceElement> ());
}

RubyInterpreter::RubyInterpreter () 
{ 
  // .. nothing ..
}

RubyInterpreter::~RubyInterpreter () 
{
  // .. nothing ..
}

void 
RubyInterpreter::add_path (const std::string &path)
{
  // .. nothing ..
}

void
RubyInterpreter::require (const std::string &filename)
{
  // .. nothing ..
}

void 
RubyInterpreter::set_debugger_scope (const std::string &filename)
{
  // .. nothing ..
}

void 
RubyInterpreter::remove_debugger_scope ()
{
  // .. nothing ..
}

void 
RubyInterpreter::ignore_next_exception ()
{
  // .. nothing ..
}

void
RubyInterpreter::load_file (const std::string &filename)
{
  // .. nothing ..
}

void
RubyInterpreter::eval_string (const char *expr, const char *file, int line, int context)
{
  fail (file, line);
}

void
RubyInterpreter::eval_string_and_print (const char *expr, const char *file, int line, int context)
{
  fail (file, line);
}

void
RubyInterpreter::define_variable (const std::string &name, const std::string &value)
{
  // .. nothing ..
}

gsi::Inspector *RubyInterpreter::inspector (int context)
{
  return 0;
}

bool
RubyInterpreter::available () const
{
  return false;
}

void 
RubyInterpreter::initialize ()
{
  // .. nothing ..
}

void 
RubyInterpreter::push_exec_handler (gsi::ExecutionHandler *exec_handler)
{
  // .. nothing ..
}

void
RubyInterpreter::remove_exec_handler (gsi::ExecutionHandler *exec_handler)
{
  // .. nothing ..
}

void 
RubyInterpreter::push_console (gsi::Console *console)
{
  // .. nothing ..
}

void 
RubyInterpreter::remove_console (gsi::Console *console)
{
  // .. nothing ..
}

std::string
RubyInterpreter::version () const
{
  return std::string ();
}

tl::Variant
RubyInterpreter::eval_expr (const char *expr, const char *file, int line, int context)
{
  fail (file, line);
  return tl::Variant ();
}

}

#endif

