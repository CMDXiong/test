
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
*  @file generated/gsiDeclQGraphicsScale_EventAdaptor.cc
*  @brief Event adaptor implementations for QGraphicsScale
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQGraphicsScale_EventAdaptor.h"

// -----------------------------------------------------------------------
// class QGraphicsScale_EventAdaptor

QGraphicsScale_EventAdaptor::QGraphicsScale_EventAdaptor (QObject *host)
  : QObject ()
{
  connect (host, SIGNAL(destroyed(QObject *)), this, SLOT(destroyed(QObject *)));
  connect (host, SIGNAL(originChanged()), this, SLOT(originChanged()));
  connect (host, SIGNAL(scaleChanged()), this, SLOT(scaleChanged()));
}

//  Event QGraphicsScale::destroyed(QObject *arg1)
void QGraphicsScale_EventAdaptor::destroyed(QObject *arg1) {
BEGIN_PROTECTED
  destroyed_e_1302(arg1);
END_PROTECTED
}

//  Event QGraphicsScale::originChanged()
void QGraphicsScale_EventAdaptor::originChanged() {
BEGIN_PROTECTED
  originChanged_e_0();
END_PROTECTED
}

//  Event QGraphicsScale::scaleChanged()
void QGraphicsScale_EventAdaptor::scaleChanged() {
BEGIN_PROTECTED
  scaleChanged_e_0();
END_PROTECTED
}