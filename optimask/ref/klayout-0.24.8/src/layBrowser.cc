
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


#include <QtGui/QDialog>

#include "layBrowser.h"
#include "layLayoutView.h"
#include "layMainWindow.h"

#include <vector>

namespace lay
{

Browser::Browser (lay::MainWindow *main, lay::LayoutView *view, const char *name, Qt::WFlags fl)
  : QDialog (0 /*main*/, fl),
    lay::Plugin (view),
    m_active (false),
    mp_view (view),
    mp_main_window (main)
{
  QObject::setObjectName (QString::fromAscii (name));
}

Browser::~Browser ()
{
  if (active ()) {
    deactivated ();
  }
}

void 
Browser::activate ()
{
  if (! active ()) {
    m_active = true;
    activated ();
    QDialog::show ();
  }
}

void 
Browser::deactivate ()
{
  if (active ()) {
    m_active = false;
    deactivated ();
    QDialog::hide ();
  }
}

void 
Browser::closeEvent (QCloseEvent *event)
{
  if (active ()) {
    m_active = false;
    deactivated ();
    QDialog::closeEvent (event);
  }
}

void
Browser::accept ()
{
  if (active ()) {
    m_active = false;
    deactivated ();
    QDialog::accept ();
  }
}

}

