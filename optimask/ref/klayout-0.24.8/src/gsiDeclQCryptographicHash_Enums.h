
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
*  @file gsiDeclQCryptographicHash_Enums.h 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#ifndef _HDR_gsiDeclQCryptographicHash_Enums
#define _HDR_gsiDeclQCryptographicHash_Enums

#include <QtCore/QFlags>

namespace qt_gsi
{

//  Wrapper class for QFlags<enum QCryptographicHash::Algorithm>
class QCryptographicHash_Algorithm_Enums : public QFlags<enum QCryptographicHash::Algorithm> {
public:
  QCryptographicHash_Algorithm_Enums () : m_flags () { }
  QCryptographicHash_Algorithm_Enums (enum QCryptographicHash::Algorithm v) : m_flags (v) { }
  QCryptographicHash_Algorithm_Enums (const QFlags<enum QCryptographicHash::Algorithm> &v) : m_flags (v) { }
  QFlags<enum QCryptographicHash::Algorithm> flags () const { return m_flags; }
private:
  QFlags<QCryptographicHash::Algorithm> m_flags;
};

//  Wrapper class for enum QCryptographicHash::Algorithm
class QCryptographicHash_Algorithm_Enum : public QCryptographicHash_Algorithm_Enums {
public:
  QCryptographicHash_Algorithm_Enum () { }
  QCryptographicHash_Algorithm_Enum (enum QCryptographicHash::Algorithm v) : QCryptographicHash_Algorithm_Enums (v) { }
  enum QCryptographicHash::Algorithm value () const { return (enum QCryptographicHash::Algorithm) flags ().operator int (); }
};

//  Converter for QFlags<enum QCryptographicHash::Algorithm>
template <>
struct Converter<QFlags<enum QCryptographicHash::Algorithm> >
{
public:
  typedef QFlags<enum QCryptographicHash::Algorithm> source_type;
  typedef QCryptographicHash_Algorithm_Enums target_type;
  static source_type toq (const target_type &c) { return c.flags (); }
  static target_type toc (const source_type &qc) { return target_type (qc); }
};

//  Converter for enum QCryptographicHash::Algorithm
template <>
struct Converter<enum QCryptographicHash::Algorithm>
{
public:
  typedef enum QCryptographicHash::Algorithm source_type;
  typedef QCryptographicHash_Algorithm_Enum target_type;
  static source_type toq (const target_type &c) { return c.value (); }
  static target_type toc (const source_type &qc) { return target_type (qc); }
};

}

#endif