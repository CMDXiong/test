
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


#include "layExceptions.h"
#include "layMainWindow.h"
#include "layApplication.h"

#include "tlException.h"
#include "tlInternational.h"
#include "tlLog.h"

#include "gsiRuntimeError.h"
#include "layRuntimeErrorForm.h"
#include "layMacroEditorDialog.h"

#include <stdexcept>

#include <QtGui/QMessageBox>

void handle_exception_silent (const tl::Exception &ex)
{
  const tl::CancelException *cancel_excpt = dynamic_cast <const tl::CancelException *> (&ex);
  const gsi::RuntimeError *gsi_excpt = dynamic_cast <const gsi::RuntimeError *> (&ex);

  if (cancel_excpt) {
    //  ignored.
  } else if (gsi_excpt) {
    if (gsi_excpt->line () > 0) {
      tl::error << gsi_excpt->sourcefile () << ":" << gsi_excpt->line () << ": " 
                << gsi_excpt->msg () << tl::to_string (QObject::tr (" (class ")) << gsi_excpt->cls () << ")";
    } else {
      tl::error << gsi_excpt->msg () << tl::to_string (QObject::tr (" (class ")) << gsi_excpt->cls () << ")";
    }
  } else {
    tl::error << ex.msg (); 
  }
}

void handle_exception (const tl::Exception &ex)
{
  if (lay::Application::instance ()->has_gui ()) {
    handle_exception_ui (ex);
  } else {
    handle_exception_silent (ex);
    // TODO: offer a strict mode where this takes place:
    // lay::Application::instance ()->exit (1);
  }
}

void handle_exception_ui (const tl::Exception &ex, QWidget *parent)
{
  //  if any transaction is pending (this may happen when an operation threw an exception)
  //  close transactions.
  if (lay::MainWindow::instance () && lay::MainWindow::instance ()->manager ().transacting ()) {
    lay::MainWindow::instance ()->manager ().commit ();
  }

  const tl::CancelException *cancel_excpt = dynamic_cast <const tl::CancelException *> (&ex);
  const gsi::ExitException *gsi_exit = dynamic_cast <const gsi::ExitException *> (&ex);
  const gsi::RuntimeError *gsi_excpt = dynamic_cast <const gsi::RuntimeError *> (&ex);

  if (gsi_exit) {
    //  exit exceptions are not shown - they are issued when a script is aborted
  } else if (cancel_excpt) {
    //  cancel exceptions are not shown either
  } else if (gsi_excpt) {

    //  show and GSI errors in the context of the macro editor if that is open
    if (! parent && lay::MacroEditorDialog::instance () && lay::MacroEditorDialog::instance ()->isVisible ()) {
      parent = lay::MacroEditorDialog::instance ();
    } 
    if (! parent) {
      parent = QApplication::activeWindow () ? QApplication::activeWindow () : lay::MainWindow::instance ();
    }

    if (gsi_excpt->line () > 0) {
      tl::error << gsi_excpt->sourcefile () << ":" << gsi_excpt->line () << ": " 
                << gsi_excpt->msg () << tl::to_string (QObject::tr (" (class ")) << gsi_excpt->cls () << ")";
    } else {
      tl::error << gsi_excpt->msg () << tl::to_string (QObject::tr (" (class ")) << gsi_excpt->cls () << ")";
    }

    lay::RuntimeErrorForm error_dialog (parent, "ruby_error_form", gsi_excpt);
    error_dialog.exec ();

  } else {
    tl::error << ex.msg (); 
    if (! parent) {
      parent = QApplication::activeWindow () ? QApplication::activeWindow () : lay::MainWindow::instance ();
    }
    QMessageBox::critical (parent, QObject::tr ("Error"), tl::to_qstring (ex.msg ())); 
  }
}

void handle_exception_silent (const std::exception &ex)
{
  tl::error << ex.what (); 
}

void handle_exception (const std::exception &ex)
{
  if (lay::Application::instance ()->has_gui ()) {
    handle_exception_ui (ex);
  } else {
    handle_exception_silent (ex);
    // TODO: offer a strict mode where this takes place:
    // lay::Application::instance ()->exit (1);
  }
}

void handle_exception_ui (const std::exception &ex, QWidget *parent)
{
  //  if any transaction is pending (this may happen when an operation threw an exception)
  //  close transactions.
  if (lay::MainWindow::instance () && lay::MainWindow::instance ()->manager ().transacting ()) {
    lay::MainWindow::instance ()->manager ().commit ();
  }

  tl::error << ex.what (); 
  if (! parent) {
    parent = QApplication::activeWindow () ? QApplication::activeWindow () : lay::MainWindow::instance ();
  }
  QMessageBox::critical (parent, QObject::tr ("Error"), tl::to_qstring (ex.what ())); 
}

void handle_exception_silent ()
{
  tl::error << tl::to_string (QObject::tr ("An unspecific error occured"));
}

void handle_exception ()
{
  if (lay::Application::instance ()->has_gui ()) {
    handle_exception_ui ();
  } else {
    handle_exception_silent ();
    // TODO: offer a strict mode where this takes place:
    // lay::Application::instance ()->exit (1);
  }
}

void handle_exception_ui (QWidget *parent)
{
  //  if any transaction is pending (this may happen when an operation threw an exception)
  //  close transactions.
  if (lay::MainWindow::instance () && lay::MainWindow::instance ()->manager ().transacting ()) {
    lay::MainWindow::instance ()->manager ().commit ();
  }

  if (! parent) {
    parent = QApplication::activeWindow () ? QApplication::activeWindow () : lay::MainWindow::instance ();
  }
  QMessageBox::critical (parent, QObject::tr ("Error"), QObject::tr ("An unspecific error occured")); 
}


