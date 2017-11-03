
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

/**
*  @file generated/gsiDeclQSystemTrayIcon_EventAdaptor.cc
*  @brief Event adaptor implementations for QSystemTrayIcon
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQSystemTrayIcon_EventAdaptor.h"

// -----------------------------------------------------------------------
// class QSystemTrayIcon_EventAdaptor

QSystemTrayIcon_EventAdaptor::QSystemTrayIcon_EventAdaptor (QObject *host)
  : QObject ()
{
  connect (host, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(activated(QSystemTrayIcon::ActivationReason)));
  connect (host, SIGNAL(destroyed(QObject *)), this, SLOT(destroyed(QObject *)));
  connect (host, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
}

//  Event QSystemTrayIcon::activated(QSystemTrayIcon::ActivationReason reason)
void QSystemTrayIcon_EventAdaptor::activated(QSystemTrayIcon::ActivationReason reason) {
BEGIN_PROTECTED
  activated_e_3745(qt_gsi::CppToQtAdaptor<QSystemTrayIcon::ActivationReason>(reason));
END_PROTECTED
}

//  Event QSystemTrayIcon::destroyed(QObject *arg1)
void QSystemTrayIcon_EventAdaptor::destroyed(QObject *arg1) {
BEGIN_PROTECTED
  destroyed_e_1302(arg1);
END_PROTECTED
}

//  Event QSystemTrayIcon::messageClicked()
void QSystemTrayIcon_EventAdaptor::messageClicked() {
BEGIN_PROTECTED
  messageClicked_e_0();
END_PROTECTED
}