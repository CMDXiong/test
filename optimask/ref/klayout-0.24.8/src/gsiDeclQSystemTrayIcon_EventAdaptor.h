
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
*  @file generated/gsiDeclQSystemTrayIcon_EventAdaptor.h
*  @brief Event adaptor implementations for QSystemTrayIcon
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#ifndef _HDR_gsiDeclQSystemTrayIcon_EventAdaptor
#define _HDR_gsiDeclQSystemTrayIcon_EventAdaptor

#include "gsiDecl.h"
#include "gsiQtEvents.h"
#include "gsiQtGui.h"
#include "gsiDeclQSystemTrayIcon_Enums.h"

#include "layExceptions.h"

#include <QtCore/QObject>

// -----------------------------------------------------------------------
// class QSystemTrayIcon_EventAdaptor

class QSystemTrayIcon_EventAdaptor
  : public QObject, public qt_gsi::EventAdaptorBase, public gsi::ObjectBase
{
Q_OBJECT

public:
  QSystemTrayIcon_EventAdaptor (QObject *host);

public slots:
  void activated(QSystemTrayIcon::ActivationReason);
  void destroyed(QObject *);
  void messageClicked();

public:
  gsi::event<const qt_gsi::Converter<QSystemTrayIcon::ActivationReason>::target_type &> activated_e_3745;
  gsi::event<QObject *> destroyed_e_1302;
  gsi::event<> messageClicked_e_0;
};

#endif