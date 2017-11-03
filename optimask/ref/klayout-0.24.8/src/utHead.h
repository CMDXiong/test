
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

 
#ifndef HDR_utHead
#define HDR_utHead

#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>

#include "tlString.h"
#include "tlException.h"
#include "tlStaticObjects.h"
#include "dbStatic.h"
#include "rba.h"
#include "pya.h"
#include "gsiExpression.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>

namespace db {
  class Layout;
}

namespace ut {

/**
 *  @brief Print a message to the unit test debug stream
 */
void debug_print (const std::string &s);

/**
 *  @brief Print a error message to the unit test debug stream
 */
void debug_error (const std::string &s);

/**
 *  @brief Raises an exception with the given string
 */
void raise (const std::string &s);

/**
 *  @brief Returns true, if the test is run in verbose mode
 *  Verbose mode is enabled through the "-v" option
 */
bool verbose ();

/**
 *  @brief Returns the Ruby interpreter
 */
rba::RubyInterpreter *ruby_interpreter ();

/**
 *  @brief Returns the Python interpreter
 */
pya::PythonInterpreter *python_interpreter ();

/**
 *  @brief Returns true, if the unit test is run in debug mode
 *  In debug mode, the unit tests shall offer information on how to fix the 
 *  test. Specifically if layout compare is involved, it shall display the golden
 *  file name and the actual one and terminate to allow updating the files. 
 */
bool is_debug_mode ();

/**
 *  @brief Compares a layout with a golden layout file
 *  @param layout The layout to compare
 *  @au_file The golden file path 
 *  @oas_format If true, uses OASIS format for normalization
 */
void compare_layouts (const db::Layout &layout, const std::string &au_file, bool oas_format = false);

/**
 *  @brief Compares a layout with a golden layout object 
 *  @param layout The layout to compare
 *  @au_layout The layout object to compare against
 *  @au_file The golden layout file (for printing)
 *  @oas_format If true, uses OASIS format for normalization
 *  The layout is normalized by writing to a file and reading back
 */
void compare_layouts (const db::Layout &layout, const db::Layout &au_layout, const std::string &au_file, bool oas_format = false);

/**
 *  @brief Gets the path of the test data
 *  This path is specified through the environment variable $TESTSRC
 */
std::string testsrc ();

/**
 *  @brief A basic exception for the unit test framework
 */
struct Exception 
  : public tl::Exception
{
  Exception (const std::string &msg)
    : tl::Exception (msg)
  { }
};

/**
 *  @brief The base class for tests
 */
struct TestBase 
{
  TestBase (const std::string &file, const std::string &name);
  virtual ~TestBase () { }

  bool do_test () const
  {
    debug_print (std::string ("Running ") + m_test + " ..");
    try {
      execute ();
      return true;
    } catch (tl::Exception &ex) {
      debug_print (std::string ("*** Test ") + m_test + " failed:");
      debug_print (ex.msg ());
      return false;
    }
  }

  const std::string &name () const
  {
    return m_test;
  }

private:
  virtual void execute () const throw (tl::Exception) = 0;
  std::string m_test;
};

/**
 *  @brief The registration facility for tests
 */
struct Registrar
{
  static void reg (const ut::TestBase *t) 
  {
    if (! ms_instance) {
      ms_instance = new Registrar ();
    }
    ms_instance->m_tests.push_back (t);
  }

  static int do_tests (const std::vector<const ut::TestBase *> *selected = 0) 
  {
    if (ms_instance) {
      return ms_instance->run_tests (selected);
    } else {
      return 0;
    }
  }

  int run_tests (const std::vector<const ut::TestBase *> *selected)
  {
    int failed = 0;
    m_failed.clear ();
    if (! selected) {
      selected = &m_tests;
    }
    for (std::vector <const ut::TestBase *>::const_iterator t = selected->begin (); t != selected->end (); ++t) {
      if (! (*t)->do_test ()) {
        m_failed.push_back (*t);
        ++failed;
      }
    }
    return failed;
  }

  static Registrar *instance ()
  {
    return ms_instance;
  }

  const std::vector <const ut::TestBase *> &failed_tests () const
  {
    return m_failed;
  }

  const std::vector <const ut::TestBase *> &tests () const
  {
    return m_tests;
  }
  
private:
  static Registrar *ms_instance;

  Registrar () : m_tests () { }

  std::vector <const ut::TestBase *> m_tests;
  std::vector <const ut::TestBase *> m_failed;
};

inline TestBase::TestBase (const std::string &file, const std::string &name)
{
  QFileInfo f (tl::to_qstring (file));
  m_test = tl::to_string (f.baseName ()) + ":" + name;
  ut::Registrar::reg (this);
}

} // namespace ut

#define TEST(NAME) \
  static void TestImpl_##NAME##_execute () throw (tl::Exception); \
  namespace {\
struct TestImpl##NAME \
      : public ut::TestBase \
    { \
      TestImpl##NAME () : TestBase (__FILE__, #NAME) { } \
      virtual void execute () const throw (tl::Exception) { TestImpl_##NAME##_execute (); }; \
    }; \
    static TestImpl##NAME TestImpl_Inst##NAME; \
  } \
  static void TestImpl_##NAME##_execute () throw (tl::Exception)

#define EXPECT_EQ(WHAT,EQUALS) \
  if (!((WHAT) == (EQUALS))) { \
    std::ostringstream sstr; \
    sstr << __LINE__ << ": " << #WHAT << " does not equal " << #EQUALS << " (actual value is " << (WHAT) << ")"; \
    ut::raise (sstr.str ()); \
  } 

#define EXPECT_NE(WHAT,EQUALS) \
  if (!((WHAT) != (EQUALS))) { \
    std::ostringstream sstr; \
    sstr << __LINE__ << ": " << #WHAT << " does equal " << #EQUALS << " (actual value is " << (WHAT) << ")"; \
    ut::raise (sstr.str ()); \
  } 

#define EXPECT(WHAT) \
  if (!(WHAT)) { \
    std::ostringstream sstr; \
    sstr << __LINE__ << ": " << #WHAT << " is not true"; \
    ut::raise (sstr.str ()); \
  } 

#define FAIL_ARG(MSG,WHAT) \
  { \
    std::ostringstream sstr; \
    sstr << __LINE__ << ": " << MSG << ", value is " << (WHAT); \
    ut::raise (sstr.str ()); \
  } 

#endif

