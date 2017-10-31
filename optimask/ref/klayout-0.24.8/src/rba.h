
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


#ifndef _HDR_rba
#define _HDR_rba

#include "gsi.h"
#include "gsiRuntimeError.h"
#include "gsiInterpreter.h"

namespace rba
{

/**
 *  @brief The client slot for GSI
 *  This number must be between 0 and gsi::num_clients and must be unique.
 */
const int client_index = 0;

/**
 *  @brief A class encapsulating a ruby exception
 */
class KLAYOUT_DLL RubyError
  : public gsi::RuntimeError
{
public:
  RubyError (const char *msg, const char *cls, const std::vector <gsi::BacktraceElement> &backtrace)
    : gsi::RuntimeError (msg, cls, backtrace)
  { }

  RubyError (const char *msg, const char *sourcefile, int line, const char *cls, const std::vector <gsi::BacktraceElement> &backtrace)
    : gsi::RuntimeError (msg, sourcefile, line, cls, backtrace)
  { }

  RubyError (const RubyError &d)
    : gsi::RuntimeError (d)
  { }
};

/**
 *  @brief The ruby interpreter wrapper class
 */
class KLAYOUT_DLL RubyInterpreter
  : public gsi::Interpreter
{
public:
  RubyInterpreter ();
  ~RubyInterpreter ();

  /**
   *  @brief Add the given path to the search path ($: in ruby)
   */
  void add_path (const std::string &path);

  /**
   *  @brief Requires the given module (ruby "require")
   */
  void require (const std::string &filename);

  /**
   *  @brief Set the given debugger scope
   *
   *  The debugger scope is the element to which the back trace will be reduced.
   *  Specifically this suppresses calls from inner functions called from that file.
   *  This is useful for DSL implementations.
   */
  void set_debugger_scope (const std::string &filename);

  /**
   *  @brief Removes the debugger scope
   */
  void remove_debugger_scope ();

  /**
   *  @brief Ignores the next exception
   *
   *  This is useful for suppressing reraised exceptions in the debugger.
   */
  void ignore_next_exception ();

  /**
   *  @brief Load the given file (ruby "load")
   */
  void load_file (const std::string &filename);

  /**
   *  @brief Implementatiom of gsi::Interpreter::eval_string
   */
  void eval_string (const char *string, const char *filename = 0, int line = 1, int context = -1);

  /**
   *  @brief Implementatiom of gsi::Interpreter::eval_expr
   */
  tl::Variant eval_expr (const char *string, const char *filename = 0, int line = 1, int context = -1);
 
  /**
   *  @brief Implementatiom of gsi::Interpreter::eval_string_and_print
   */ 
  void eval_string_and_print (const char *string, const char *filename = 0, int line = 1, int context = -1);

  /**
   *  @brief Returns an inspector for the given context
   */
  virtual gsi::Inspector *inspector (int context = -1);

  /**
   *  @brief Defines a global variable with the given name and value 
   */
  void define_variable (const std::string &name, const std::string &value);

  /**
   *  @brief Gets a value indicating whether the interpreter is available
   */
  bool available () const;

  /**
   *  @brief Installs the given console for output
   */
  void push_console (gsi::Console *console);

  /**
   *  @brief Removes the given console
   */
  void remove_console (gsi::Console *console);

  /**
   *  @brief Installs the given execution handler
   *
   *  The execution handler is informed when the interpreter enters code execution (also
   *  from the outside, i.e. a method reimplementing a C++ method or a event handler).
   *  During execution, the handler receives trace events which allow him to intercept
   *  execution.
   */
  void push_exec_handler (gsi::ExecutionHandler *exec_handler);

  /**
   *  @brief Removes the given execution handler
   */
  void remove_exec_handler (gsi::ExecutionHandler *exec_handler);

  /**
   *  @brief Fetch the version string
   *
   *  Returns an empty string when no Ruby interpreter is installed.
   */
  std::string version () const;

  /**
   *  @brief Provide a first (basic) initialization
   */
  static void initialize ();
};

}

#endif

