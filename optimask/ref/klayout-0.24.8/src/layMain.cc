
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


#include "layApplication.h"
#include "layExceptions.h"
#include "layFileDialog.h"
#include "tlInternational.h"
#include "tlException.h"
#include "tlLog.h"
#include "gsiRuntimeError.h"
#include "rba.h"
#include "pya.h"
#include "gsiExternalMain.h"

#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtCore/QTextCodec>

#include <iostream>
#include <cstdlib>

int klayout_main (int argc, char **argv);

#ifdef _MSC_VER // for VC++ 

//  for VC++ provide a wrapper for main.
#include <Windows.h>

int WINAPI 
wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
  int argCount = 0;
  LPWSTR *szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);

  //  fail safe behaviour
  if (!szArgList) {
	  MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
	  return 10;
  }

  char **argv = new char *[argCount];
  for (int i = 0; i < argCount; i++) {
	  QString a;
	  for (WCHAR *wc = szArgList [i]; *wc; ++wc) {
		  a += QChar (*wc);
	  }
	  QByteArray aa = a.toUtf8 ();
	  argv [i] = new char [aa.size () + 1];
	  strcpy (argv [i], aa.constData ());
  }

  int ret = klayout_main (argCount, argv);

  for (int i = 0; i < argCount; i++) {
	  delete[] argv [i];
  }
  delete[] argv;

  LocalFree(szArgList);
  return ret;
}

#else

int  
main(int a_argc, const char **a_argv)
{
  char **argv = new char *[a_argc];
  for (int i = 0; i < a_argc; i++) {
	  tl::string aa = tl::system_to_string (a_argv[i]);
	  argv [i] = new char [aa.size () + 1];
	  strcpy (argv [i], aa.c_str ());
  }

  int ret = klayout_main (a_argc, argv);

  for (int i = 0; i < a_argc; i++) {
	  delete[] argv [i];
  }
  delete[] argv;

  return ret;
}

#endif

void myMessageOutput(QtMsgType type, const char *msg)
{
  switch (type) {
  case QtDebugMsg:
    fprintf(stderr, "Debug: %s\n", msg);
    break;
  case QtWarningMsg:
    fprintf(stderr, "Warning: %s\n", msg);
    break;
  case QtCriticalMsg:
    fprintf(stderr, "Critical: %s\n", msg);
    break;
  case QtFatalMsg:
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();
  }
}

/**
 *  @brief The basic entry point
 *
 *  Note that by definition, klayout_main receives arguments in UTF-8
 */
int 
klayout_main (int argc, char **argv)
{
  qInstallMsgHandler (myMessageOutput);

  int result = 0;

  try {

    //  see comment there why this call is here.
    //  TODO: do this through some auto-registration scheme
    rba::RubyInterpreter::initialize ();
    pya::PythonInterpreter::initialize ();

    //  this registers the gsi definitions
	  gsi::initialize_external ();

    bool non_ui_mode = false;

    //  If we have a -zz option, initialize a QCore application. Otherwise create a QApplication.
    //  That way we can use KLayout as a non-windows application with -zz or -b.
    for (int i = 1; i < argc; ++i) {
      if (argv [i] == std::string ("-zz") || argv [i] == std::string ("-b")) {
        non_ui_mode = true;
        break;
      }
    }

    lay::Application app (argc, argv, non_ui_mode);

    QString locale = QLocale::system().name();

    /* @@@ kills valgrind
    QTranslator translator;
    if (translator.load (QString::fromUtf8 ("klayout_") + locale)) {
      app.installTranslator (&translator);
    }
    */

    QTextCodec::setCodecForTr (QTextCodec::codecForName ("utf8"));

    if (app.has_gui ()) {

      BEGIN_PROTECTED_CLEANUP

      result = app.run ();

      END_PROTECTED_CLEANUP {
        result = 1;
      }

    } else {
      result = app.run ();
    }

  } catch (gsi::ExitException &ex) {
    result = ex.status ();
  } catch (std::exception &ex) {
    tl::error << ex.what ();
    result = 1;
  } catch (tl::Exception &ex) {
    tl::error << ex.msg ();
    result = 1;
  } catch (...) {
    tl::error << tl::to_string (QObject::tr ("unspecific error"));
    result = 1;
  }

  return result;

}


