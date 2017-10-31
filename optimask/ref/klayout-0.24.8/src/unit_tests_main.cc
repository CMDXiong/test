
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


#include "utHead.h"
#include "rba.h"
#include "gsiExpression.h"
#include "gsiExternalMain.h"
#include "dbStatic.h"

#include "dbLayoutDiff.h"
#include "dbWriter.h"
#include "dbGDS2Writer.h"
#include "dbOASISWriter.h"
#include "dbGDS2Reader.h"

#include <stdio.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <QtCore/QRegExp>
#include <QtCore/QCoreApplication>

namespace ut 
{

ut::Registrar *ut::Registrar::ms_instance = 0;

static bool s_verbose_flag = false;
static bool s_debug_mode = false;
static bool s_continue_flag = false;
static pya::PythonInterpreter *sp_python_interpreter = 0;
static rba::RubyInterpreter *sp_ruby_interpreter = 0;

bool verbose ()
{
  return s_verbose_flag;
}

pya::PythonInterpreter *python_interpreter () 
{
  return sp_python_interpreter;
}

rba::RubyInterpreter *ruby_interpreter ()
{
  return sp_ruby_interpreter;
}

bool is_debug_mode ()
{
  return s_debug_mode;
}

std::string testsrc ()
{
  const char *ts = getenv ("TESTSRC");
  if (! ts) {
    throw tl::Exception ("TESTSRC undefined");
  }
  return ts;
}

void compare_layouts (const db::Layout &layout, const std::string &au_file, bool oas_format)
{
  db::Layout layout_au;

  //  read all layers from the original layout into the golden layout (at least)
  for (db::Layout::layer_iterator l = layout.begin_layers (); l != layout.end_layers (); ++l) {
    layout_au.insert_layer ((*l).first, *(*l).second);
  }
  
  tl::InputStream stream (au_file);
  db::Reader reader (stream);
  reader.read (layout_au);

  compare_layouts (layout, layout_au, au_file, oas_format);
}

void compare_layouts (const db::Layout &layout, const db::Layout &layout_au, const std::string &au_file, bool oas_format)
{
  db::Layout layout2;

  //  normalize the layout by writing to GDS and reading from ..

  const char *tmp_file = oas_format ? "tmp.oas" : "tmp.gds";

  if (oas_format) {
    tl::OutputStream stream (tmp_file);
    db::OASISWriter writer;
    db::SaveLayoutOptions options;
    writer.write (layout, stream, options);
  } else {
    tl::OutputStream stream (tmp_file);
    db::GDS2Writer writer;
    db::SaveLayoutOptions options;
    writer.write (layout, stream, options);
  }

  {
    //  read all layers from the original layout, so the layer table is the same 
    for (db::Layout::layer_iterator l = layout.begin_layers (); l != layout.end_layers (); ++l) {
      layout2.insert_layer ((*l).first, *(*l).second);
    }
    
    tl::InputStream stream (tmp_file);
    db::Reader reader (stream);
    reader.read (layout2);
  }

  bool equal = db::compare_layouts (layout2, layout_au, db::layout_diff::f_verbose | db::layout_diff::f_flatten_array_insts);

  if (ut::is_debug_mode () && ! equal) {
    debug_error (tl::sprintf ("test failed - see %s vs %s\n", tmp_file, au_file));
    exit (0);
  }

  EXPECT_EQ (equal, true);
}

void debug_print (const std::string &s) 
{
  printf("%s\n", s.c_str());
  fflush(stdout);
#if defined(_WIN32)
  OutputDebugStringA (tl::to_qstring (s).toLocal8Bit ().constData ());
  OutputDebugStringA ("\n");
#endif
}

void debug_error (const std::string &s) 
{
  printf("*** ERROR: %s\n", s.c_str());
  fflush(stdout);
#if defined(_WIN32)
  OutputDebugStringA ("*** ERROR: ");
  OutputDebugStringA (tl::to_qstring (s).toLocal8Bit ().constData ());
  OutputDebugStringA ("\n");
#endif
}

void raise (const std::string &msg)
{
  if (s_continue_flag) {
    debug_print (msg);
  } else {
    throw ut::Exception (msg); 
  }
}

} // namespace ut

int 
main (int argc, char **argv) 
{
  QCoreApplication app (argc, argv, false);

  bool editable = true, non_editable = true;
  bool exclude = false;
  std::vector<std::string> test_list;
  std::vector<std::string> exclude_test_list;

  for (int i = 1; i < argc; ++i) {

    std::string a = argv[i];
    if (a == "-h") {

      std::cout << "unit_test <Options> <Test list>" << std::endl
                << "Options:" << std::endl
                << "  -l    List tests and exit" << std::endl
                << "  -e    Editable mode only" << std::endl
                << "  -ne   Non-editable mode only" << std::endl
                << "  -c    Continue on error" << std::endl
                << "  -v    Verbose mode" << std::endl
                << "  -d    debug mode (stop on error, indicate fix instructions)" << std::endl
                << "  -x    Exclude following tests" << std::endl
                << "Test list: list of match strings selecting some tests (default: all)" << std::endl;
      exit (0);

    } else if (a == "-l") {

      std::cout << "List of installed tests:" << std::endl;
      for (std::vector<const ut::TestBase *>::const_iterator i = ut::Registrar::instance()->tests ().begin (); i != ut::Registrar::instance()->tests ().end (); ++i) {
        std::cout << "  " << (*i)->name () << std::endl;
      }
      exit (0);

    } else if (a == "-e") {

      non_editable = false;
      editable = true;

    } else if (a == "-ne") {

      non_editable = true;
      editable = false;

    } else if (a == "-c") {

      ut::s_continue_flag = true;

    } else if (a == "-d") {

      ut::s_debug_mode = true;

    } else if (a == "-v") {

      ut::s_verbose_flag = true;

    } else if (a == "-x") {

      exclude = true;

    } else {

      if (exclude) {
        exclude_test_list.push_back (a);
      } else {
        test_list.push_back (a);
      }

    }

  }

  int failed = 0;

  try {

    ut::debug_print ("KLayout test suite");
    ut::debug_print ("TESTSRC=" + ut::testsrc());

    std::vector<const ut::TestBase *> *selected_tests = 0;
    std::vector<const ut::TestBase *> subset;
    if (! test_list.empty ()) {

      selected_tests = &subset;
      std::cout << "Selected tests:" << std::endl;

      for (std::vector<const ut::TestBase *>::const_iterator i = ut::Registrar::instance()->tests ().begin (); i != ut::Registrar::instance()->tests ().end (); ++i) {

        bool exclude = false;
        for (std::vector<std::string>::const_iterator m = exclude_test_list.begin (); m != exclude_test_list.end (); ++m) {
          QRegExp re (tl::to_qstring (*m), Qt::CaseInsensitive, QRegExp::Wildcard);
          if (re.indexIn (tl::to_qstring ((*i)->name ())) >= 0) {
            exclude = true;
            break;
          }
        }

        for (std::vector<std::string>::const_iterator m = test_list.begin (); !exclude && m != test_list.end (); ++m) {
          QRegExp re (tl::to_qstring (*m), Qt::CaseInsensitive, QRegExp::Wildcard);
          if (re.indexIn (tl::to_qstring ((*i)->name ())) >= 0) {
            std::cout << "  " << (*i)->name () << std::endl;
            subset.push_back (*i);
            break;
          }
        }

      }

    }

    gsi::initialize ();
    gsi::initialize_expressions ();
    gsi::initialize_external ();

    //  With Ruby, "fclose" is redirected to the Ruby-DLL. Because of that, Ruby needs to be initialized before fclose can be used ...
    rba::RubyInterpreter::initialize ();
    pya::PythonInterpreter::initialize ();
    rba::RubyInterpreter rbinterp;
    ut::sp_ruby_interpreter = &rbinterp;
    pya::PythonInterpreter pyinterp;
    ut::sp_python_interpreter = &pyinterp;

    ut::s_verbose_flag = false;
    int failed_ne = 0, failed_e = 0;
    std::vector <const ut::TestBase *> failed_tests_e, failed_tests_ne;

    if (non_editable) {

      ut::debug_print ("------------------------------------------------------------------------------");
      ut::debug_print ("Running tests in non-editable mode ...");
      db::set_default_editable_mode (false);

      try {
        failed_ne = ut::Registrar::do_tests (selected_tests);
      } catch (tl::Exception &ex) {
        ut::debug_error ("Caught tl::exception: " + ex.msg ());
        failed_ne = 1;
      } catch (std::exception &ex) {
        ut::debug_error ("Caught std::exception: " + std::string (ex.what ()));
        failed_ne = 1;
      } catch (...) {
        ut::debug_error ("Caught unspecific exception");
        failed_ne = 1;
      }

      if (failed_ne > 0) {
        failed_tests_ne = ut::Registrar::instance ()->failed_tests ();
        ut::debug_print (std::string ("Summary: ") + tl::to_string (failed_ne) + " test(s) failed");
      } else {
        ut::debug_print ("All tests passed in non-editable mode.");
      }

    }

    if (editable) {

      ut::debug_print ("------------------------------------------------------------------------------");
      ut::debug_print ("Running tests in editable mode ...");
      db::set_default_editable_mode (true);

      try {
        failed_e = ut::Registrar::do_tests (selected_tests);
      } catch (tl::Exception &ex) {
        ut::debug_error ("Caught tl::exception: " + ex.msg ());
        failed_e = 1;
      } catch (std::exception &ex) {
        ut::debug_error ("Caught std::exception: " + std::string (ex.what ()));
        failed_e = 1;
      } catch (...) {
        ut::debug_error ("Caught unspecific exception");
        failed_e = 1;
      }

      if (failed_e > 0) {
        failed_tests_e = ut::Registrar::instance ()->failed_tests ();
        ut::debug_print (std::string ("Summary: ") + tl::to_string (failed_e) + " test(s) failed");
      } else {
        ut::debug_print ("All tests passed in editable mode.");
      }

    }

    ut::debug_print ("------------------------------------------------------------------------------");
    failed = failed_e + failed_ne;
    if (failed > 0) {
      if (non_editable) {
        ut::debug_print ("Failed in non-editable mode:");
        for (std::vector <const ut::TestBase *>::const_iterator f = failed_tests_ne.begin (); f != failed_tests_ne.end (); ++f) {
          ut::debug_print (std::string ("  ") + (*f)->name ());
        }
      }
      if (editable) {
        ut::debug_print ("Failed in editable mode:");
        for (std::vector <const ut::TestBase *>::const_iterator f = failed_tests_e.begin (); f != failed_tests_e.end (); ++f) {
          ut::debug_print (std::string ("  ") + (*f)->name ());
        }
      }
      ut::debug_print (std::string ("Grand summary: ") + tl::to_string (failed) + " test(s) failed");
    } else {
      ut::debug_print ("All tests passed.");
    }

  } catch (tl::Exception &ex) {
    ut::debug_error (ex.msg ());
    exit (-1);
  } catch (std::exception &ex) {
    ut::debug_error (ex.what ());
    exit (-1);
  } catch (...) {
    ut::debug_error ("Unspecific exception");
    exit (-1);
  }

  //  destroy all static objects to provide a way to detect leaks 
  tl::StaticObjects::cleanup ();

  return failed;

}

