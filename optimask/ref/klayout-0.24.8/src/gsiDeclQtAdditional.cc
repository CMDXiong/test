
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


#include "gsiQt.h"

#include <QtGui/QBitmap>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QCursor>
#include <QtGui/QFont>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QKeySequence>
#include <QtGui/QTransform>
#include <QtGui/QMatrix4x4>
#include <QtGui/QPalette>
#include <QtGui/QPen>
#include <QtGui/QPixmap>
#include <QtGui/QQuaternion>
#include <QtGui/QRegion>
#include <QtGui/QTextFormat>
#include <QtGui/QTextLength>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QVector4D>

namespace gsi
{

// -------------------------------------------------------------------------
//  Additional declarations for QVariant
//
//  These declarations are not generated automatically, since they
//  are supplied as global type cast operators.
//
//  These types are:
//    Type             Accessor
//    QBitmap          toBitmap
//    QBrush           toBrush
//    QColor           toColor
//    QCursor          toCursor
//    QFont            toFont
//    QIcon            toIcon
//    QImage           toImage
//    QKeySequence     toKeySequence
//    QTransform       toTransform
//    QMatrix4x4       toMatrix4x4
//    QPalette         toPalette
//    QPen             toPen
//    QPixmap          toPixmap
//    QQuaternion      toQuaternion
//    QRegion          toRegion
//    QTextFormat      toTextFormat
//    QTextLength      toTextLength
//    QVector2D        toVector2D
//    QVector3D        toVector3D
//    QVector4D        toVector4D

template <class T>
static QVariant *new_variant_from (const T &t)
{
  QVariant *v = new QVariant ();
  *v = t;
  return v;
}

static gsi::ClassExt<QVariant> add_decl_QVariant ( 
  //  TODO: needs a better ambiguity resolution:
  //  ambiguous to QPixmap constructor: gsi::constructor ("new", static_cast<QVariant *(*)(const QBitmap &)>(&new_variant_from<QBitmap>), "@brief Creates a QVariant object that holds a QBitmap") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QBrush &)>(&new_variant_from<QBrush>), "@brief Creates a QVariant object that holds a QBrush") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QColor &)>(&new_variant_from<QColor>), "@brief Creates a QVariant object that holds a QColor") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QCursor &)>(&new_variant_from<QCursor>), "@brief Creates a QVariant object that holds a QCursor") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QFont &)>(&new_variant_from<QFont>), "@brief Creates a QVariant object that holds a QFont") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QIcon &)>(&new_variant_from<QIcon>), "@brief Creates a QVariant object that holds a QIcon") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QImage &)>(&new_variant_from<QImage>), "@brief Creates a QVariant object that holds a QImage") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QKeySequence &)>(&new_variant_from<QKeySequence>), "@brief Creates a QVariant object that holds a QKeySequence") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QTransform &)>(&new_variant_from<QTransform>), "@brief Creates a QVariant object that holds a QTransform") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QMatrix4x4 &)>(&new_variant_from<QMatrix4x4>), "@brief Creates a QVariant object that holds a QMatrix4x4") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QPalette &)>(&new_variant_from<QPalette>), "@brief Creates a QVariant object that holds a QPalette") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QPen &)>(&new_variant_from<QPen>), "@brief Creates a QVariant object that holds a QPen") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QPixmap &)>(&new_variant_from<QPixmap>), "@brief Creates a QVariant object that holds a QPixmap") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QQuaternion &)>(&new_variant_from<QQuaternion>), "@brief Creates a QVariant object that holds a QQuaternion") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QRegion &)>(&new_variant_from<QRegion>), "@brief Creates a QVariant object that holds a QRegion") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QTextFormat &)>(&new_variant_from<QTextFormat>), "@brief Creates a QVariant object that holds a QTextFormat") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QTextLength &)>(&new_variant_from<QTextLength>), "@brief Creates a QVariant object that holds a QTextLength") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QVector2D &)>(&new_variant_from<QVector2D>), "@brief Creates a QVariant object that holds a QVector2D") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QVector3D &)>(&new_variant_from<QVector3D>), "@brief Creates a QVariant object that holds a QVector3D") +
  gsi::constructor ("new", static_cast<QVariant *(*)(const QVector4D &)>(&new_variant_from<QVector4D>), "@brief Creates a QVariant object that holds a QVector4D") +
  gsi::method ("toBitmap", static_cast<QBitmap (QVariant::*)() const>(&QVariant::value<QBitmap>), "@brief Returns the QBitmap value stored in the QVariant") +
  gsi::method ("toBrush", static_cast<QBrush (QVariant::*)() const>(&QVariant::value<QBrush>), "@brief Returns the QBrush value stored in the QVariant") +
  gsi::method ("toColor", static_cast<QColor (QVariant::*)() const>(&QVariant::value<QColor>), "@brief Returns the QColor value stored in the QVariant") +
  gsi::method ("toCursor", static_cast<QCursor (QVariant::*)() const>(&QVariant::value<QCursor>), "@brief Returns the QCursor value stored in the QVariant") +
  gsi::method ("toFont", static_cast<QFont (QVariant::*)() const>(&QVariant::value<QFont>), "@brief Returns the QFont value stored in the QVariant") +
  gsi::method ("toIcon", static_cast<QIcon (QVariant::*)() const>(&QVariant::value<QIcon>), "@brief Returns the QIcon value stored in the QVariant") +
  gsi::method ("toImage", static_cast<QImage (QVariant::*)() const>(&QVariant::value<QImage>), "@brief Returns the QImage value stored in the QVariant") +
  gsi::method ("toKeySequence", static_cast<QKeySequence (QVariant::*)() const>(&QVariant::value<QKeySequence>), "@brief Returns the QKeySequence value stored in the QVariant") +
  gsi::method ("toTransform", static_cast<QTransform (QVariant::*)() const>(&QVariant::value<QTransform>), "@brief Returns the QTransform value stored in the QVariant") +
  gsi::method ("toMatrix4x4", static_cast<QMatrix4x4 (QVariant::*)() const>(&QVariant::value<QMatrix4x4>), "@brief Returns the QMatrix4x4 value stored in the QVariant") +
  gsi::method ("toPalette", static_cast<QPalette (QVariant::*)() const>(&QVariant::value<QPalette>), "@brief Returns the QPalette value stored in the QVariant") +
  gsi::method ("toPen", static_cast<QPen (QVariant::*)() const>(&QVariant::value<QPen>), "@brief Returns the QPen value stored in the QVariant") +
  gsi::method ("toPixmap", static_cast<QPixmap (QVariant::*)() const>(&QVariant::value<QPixmap>), "@brief Returns the QPixmap value stored in the QVariant") +
  gsi::method ("toQuaternion", static_cast<QQuaternion (QVariant::*)() const>(&QVariant::value<QQuaternion>), "@brief Returns the QQuaternion value stored in the QVariant") +
  gsi::method ("toRegion", static_cast<QRegion (QVariant::*)() const>(&QVariant::value<QRegion>), "@brief Returns the QRegion value stored in the QVariant") +
  gsi::method ("toTextFormat", static_cast<QTextFormat (QVariant::*)() const>(&QVariant::value<QTextFormat>), "@brief Returns the QTextFormat value stored in the QVariant") +
  gsi::method ("toTextLength", static_cast<QTextLength (QVariant::*)() const>(&QVariant::value<QTextLength>), "@brief Returns the QTextLength value stored in the QVariant") +
  gsi::method ("toVector2D", static_cast<QVector2D (QVariant::*)() const>(&QVariant::value<QVector2D>), "@brief Returns the QVector2D value stored in the QVariant") +
  gsi::method ("toVector3D", static_cast<QVector3D (QVariant::*)() const>(&QVariant::value<QVector3D>), "@brief Returns the QVector3D value stored in the QVariant") +
  gsi::method ("toVector4D", static_cast<QVector4D (QVariant::*)() const>(&QVariant::value<QVector4D>), "@brief Returns the QVector4D value stored in the QVariant"),
  ""
);

}

