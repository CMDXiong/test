
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
*  @file generated/gsiDeclQDockWidget_EventAdaptor.cc
*  @brief Event adaptor implementations for QDockWidget
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQDockWidget_EventAdaptor.h"

// -----------------------------------------------------------------------
// class QDockWidget_EventAdaptor

QDockWidget_EventAdaptor::QDockWidget_EventAdaptor (QObject *host)
  : QObject ()
{
  connect (host, SIGNAL(allowedAreasChanged(QFlags<Qt::DockWidgetArea>)), this, SLOT(allowedAreasChanged(QFlags<Qt::DockWidgetArea>)));
  connect (host, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(customContextMenuRequested(const QPoint &)));
  connect (host, SIGNAL(destroyed(QObject *)), this, SLOT(destroyed(QObject *)));
  connect (host, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(dockLocationChanged(Qt::DockWidgetArea)));
  connect (host, SIGNAL(featuresChanged(QFlags<QDockWidget::DockWidgetFeature>)), this, SLOT(featuresChanged(QFlags<QDockWidget::DockWidgetFeature>)));
  connect (host, SIGNAL(topLevelChanged(bool)), this, SLOT(topLevelChanged(bool)));
  connect (host, SIGNAL(visibilityChanged(bool)), this, SLOT(visibilityChanged(bool)));
}

//  Event QDockWidget::allowedAreasChanged(QFlags<Qt::DockWidgetArea> allowedAreas)
void QDockWidget_EventAdaptor::allowedAreasChanged(QFlags<Qt::DockWidgetArea> allowedAreas) {
BEGIN_PROTECTED
  allowedAreasChanged_e_2819(qt_gsi::CppToQtAdaptor<QFlags<Qt::DockWidgetArea> >(allowedAreas));
END_PROTECTED
}

//  Event QDockWidget::customContextMenuRequested(const QPoint &pos)
void QDockWidget_EventAdaptor::customContextMenuRequested(const QPoint &pos) {
BEGIN_PROTECTED
  customContextMenuRequested_e_1916(pos);
END_PROTECTED
}

//  Event QDockWidget::destroyed(QObject *arg1)
void QDockWidget_EventAdaptor::destroyed(QObject *arg1) {
BEGIN_PROTECTED
  destroyed_e_1302(arg1);
END_PROTECTED
}

//  Event QDockWidget::dockLocationChanged(Qt::DockWidgetArea area)
void QDockWidget_EventAdaptor::dockLocationChanged(Qt::DockWidgetArea area) {
BEGIN_PROTECTED
  dockLocationChanged_e_2123(qt_gsi::CppToQtAdaptor<Qt::DockWidgetArea>(area));
END_PROTECTED
}

//  Event QDockWidget::featuresChanged(QFlags<QDockWidget::DockWidgetFeature> features)
void QDockWidget_EventAdaptor::featuresChanged(QFlags<QDockWidget::DockWidgetFeature> features) {
BEGIN_PROTECTED
  featuresChanged_e_4039(qt_gsi::CppToQtAdaptor<QFlags<QDockWidget::DockWidgetFeature> >(features));
END_PROTECTED
}

//  Event QDockWidget::topLevelChanged(bool topLevel)
void QDockWidget_EventAdaptor::topLevelChanged(bool topLevel) {
BEGIN_PROTECTED
  topLevelChanged_e_864(topLevel);
END_PROTECTED
}

//  Event QDockWidget::visibilityChanged(bool visible)
void QDockWidget_EventAdaptor::visibilityChanged(bool visible) {
BEGIN_PROTECTED
  visibilityChanged_e_864(visible);
END_PROTECTED
}