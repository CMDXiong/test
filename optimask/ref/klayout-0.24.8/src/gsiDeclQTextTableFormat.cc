
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
*  @file gsiDeclQTextTableFormat.cc 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQtTypeTraits.h"
#include "gsiDeclQTextTableFormat_Enums.h"
#include <memory>

// -----------------------------------------------------------------------
// class QTextTableFormat

//  Constructor QTextTableFormat::QTextTableFormat()


static void _init_ctor_QTextTableFormat_0 (qt_gsi::GenericStaticMethod *decl)
{
  decl->set_return_new<QTextTableFormat *> ();
}

static void _call_ctor_QTextTableFormat_0 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QTextTableFormat *> (new QTextTableFormat ());
}

static qt_gsi::GenericStaticMethod *_create_ctor_QTextTableFormat_0 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QTextTableFormat::QTextTableFormat()\nThis method creates an object of class QTextTableFormat.", &_init_ctor_QTextTableFormat_0, &_call_ctor_QTextTableFormat_0);
}

// QFlags<Qt::AlignmentFlag> QTextTableFormat::alignment()


static void _init_f_alignment_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QFlags<Qt::AlignmentFlag> >::target_type > ();
}

static void _call_f_alignment_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QFlags<Qt::AlignmentFlag> >::target_type > ((qt_gsi::Converter<QFlags<Qt::AlignmentFlag> >::target_type)qt_gsi::CppToQtAdaptor<QFlags<Qt::AlignmentFlag> >(((QTextTableFormat *)cls)->alignment ()));
}

static qt_gsi::GenericMethod *_create_f_alignment_c0 ()
{
  return new qt_gsi::GenericMethod (":alignment", "@brief Method QFlags<Qt::AlignmentFlag> QTextTableFormat::alignment()\n", true, &_init_f_alignment_c0, &_call_f_alignment_c0);
}

// double QTextTableFormat::cellPadding()


static void _init_f_cellPadding_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<double > ();
}

static void _call_f_cellPadding_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<double > ((double)((QTextTableFormat *)cls)->cellPadding ());
}

static qt_gsi::GenericMethod *_create_f_cellPadding_c0 ()
{
  return new qt_gsi::GenericMethod (":cellPadding", "@brief Method double QTextTableFormat::cellPadding()\n", true, &_init_f_cellPadding_c0, &_call_f_cellPadding_c0);
}

// double QTextTableFormat::cellSpacing()


static void _init_f_cellSpacing_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<double > ();
}

static void _call_f_cellSpacing_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<double > ((double)((QTextTableFormat *)cls)->cellSpacing ());
}

static qt_gsi::GenericMethod *_create_f_cellSpacing_c0 ()
{
  return new qt_gsi::GenericMethod (":cellSpacing", "@brief Method double QTextTableFormat::cellSpacing()\n", true, &_init_f_cellSpacing_c0, &_call_f_cellSpacing_c0);
}

// void QTextTableFormat::clearColumnWidthConstraints()


static void _init_f_clearColumnWidthConstraints_0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<void > ();
}

static void _call_f_clearColumnWidthConstraints_0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ((QTextTableFormat *)cls)->clearColumnWidthConstraints ();
}

static qt_gsi::GenericMethod *_create_f_clearColumnWidthConstraints_0 ()
{
  return new qt_gsi::GenericMethod ("clearColumnWidthConstraints", "@brief Method void QTextTableFormat::clearColumnWidthConstraints()\n", false, &_init_f_clearColumnWidthConstraints_0, &_call_f_clearColumnWidthConstraints_0);
}

// QVector<QTextLength> QTextTableFormat::columnWidthConstraints()


static void _init_f_columnWidthConstraints_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QVector<QTextLength> >::target_type > ();
}

static void _call_f_columnWidthConstraints_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QVector<QTextLength> >::target_type > ((qt_gsi::Converter<QVector<QTextLength> >::target_type)qt_gsi::CppToQtAdaptor<QVector<QTextLength> >(((QTextTableFormat *)cls)->columnWidthConstraints ()));
}

static qt_gsi::GenericMethod *_create_f_columnWidthConstraints_c0 ()
{
  return new qt_gsi::GenericMethod (":columnWidthConstraints", "@brief Method QVector<QTextLength> QTextTableFormat::columnWidthConstraints()\n", true, &_init_f_columnWidthConstraints_c0, &_call_f_columnWidthConstraints_c0);
}

// int QTextTableFormat::columns()


static void _init_f_columns_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<int > ();
}

static void _call_f_columns_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<int > ((int)((QTextTableFormat *)cls)->columns ());
}

static qt_gsi::GenericMethod *_create_f_columns_c0 ()
{
  return new qt_gsi::GenericMethod (":columns", "@brief Method int QTextTableFormat::columns()\n", true, &_init_f_columns_c0, &_call_f_columns_c0);
}

// int QTextTableFormat::headerRowCount()


static void _init_f_headerRowCount_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<int > ();
}

static void _call_f_headerRowCount_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<int > ((int)((QTextTableFormat *)cls)->headerRowCount ());
}

static qt_gsi::GenericMethod *_create_f_headerRowCount_c0 ()
{
  return new qt_gsi::GenericMethod (":headerRowCount", "@brief Method int QTextTableFormat::headerRowCount()\n", true, &_init_f_headerRowCount_c0, &_call_f_headerRowCount_c0);
}

// bool QTextTableFormat::isValid()


static void _init_f_isValid_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<bool > ();
}

static void _call_f_isValid_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<bool > ((bool)((QTextTableFormat *)cls)->isValid ());
}

static qt_gsi::GenericMethod *_create_f_isValid_c0 ()
{
  return new qt_gsi::GenericMethod ("isValid?", "@brief Method bool QTextTableFormat::isValid()\n", true, &_init_f_isValid_c0, &_call_f_isValid_c0);
}

// void QTextTableFormat::setAlignment(QFlags<Qt::AlignmentFlag> alignment)


static void _init_f_setAlignment_2750 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("alignment");
  decl->add_arg<const qt_gsi::Converter<QFlags<Qt::AlignmentFlag> >::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setAlignment_2750 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<Qt::AlignmentFlag> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<Qt::AlignmentFlag> >::target_type & > ();
  ((QTextTableFormat *)cls)->setAlignment (qt_gsi::QtToCppAdaptor<QFlags<Qt::AlignmentFlag> >(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setAlignment_2750 ()
{
  return new qt_gsi::GenericMethod ("setAlignment|alignment=", "@brief Method void QTextTableFormat::setAlignment(QFlags<Qt::AlignmentFlag> alignment)\n", false, &_init_f_setAlignment_2750, &_call_f_setAlignment_2750);
}

// void QTextTableFormat::setCellPadding(double padding)


static void _init_f_setCellPadding_1071 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("padding");
  decl->add_arg<double > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setCellPadding_1071 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  double arg1 = args.read<double > ();
  ((QTextTableFormat *)cls)->setCellPadding (arg1);
}

static qt_gsi::GenericMethod *_create_f_setCellPadding_1071 ()
{
  return new qt_gsi::GenericMethod ("setCellPadding|cellPadding=", "@brief Method void QTextTableFormat::setCellPadding(double padding)\n", false, &_init_f_setCellPadding_1071, &_call_f_setCellPadding_1071);
}

// void QTextTableFormat::setCellSpacing(double spacing)


static void _init_f_setCellSpacing_1071 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("spacing");
  decl->add_arg<double > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setCellSpacing_1071 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  double arg1 = args.read<double > ();
  ((QTextTableFormat *)cls)->setCellSpacing (arg1);
}

static qt_gsi::GenericMethod *_create_f_setCellSpacing_1071 ()
{
  return new qt_gsi::GenericMethod ("setCellSpacing|cellSpacing=", "@brief Method void QTextTableFormat::setCellSpacing(double spacing)\n", false, &_init_f_setCellSpacing_1071, &_call_f_setCellSpacing_1071);
}

// void QTextTableFormat::setColumnWidthConstraints(const QVector<QTextLength> &constraints)


static void _init_f_setColumnWidthConstraints_3255 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("constraints");
  decl->add_arg<const qt_gsi::Converter<QVector<QTextLength> >::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setColumnWidthConstraints_3255 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QVector<QTextLength> >::target_type & arg1 = args.read<const qt_gsi::Converter<QVector<QTextLength> >::target_type & > ();
  ((QTextTableFormat *)cls)->setColumnWidthConstraints (qt_gsi::QtToCppAdaptor<QVector<QTextLength> >(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setColumnWidthConstraints_3255 ()
{
  return new qt_gsi::GenericMethod ("setColumnWidthConstraints|columnWidthConstraints=", "@brief Method void QTextTableFormat::setColumnWidthConstraints(const QVector<QTextLength> &constraints)\n", false, &_init_f_setColumnWidthConstraints_3255, &_call_f_setColumnWidthConstraints_3255);
}

// void QTextTableFormat::setColumns(int columns)


static void _init_f_setColumns_767 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("columns");
  decl->add_arg<int > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setColumns_767 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  int arg1 = args.read<int > ();
  ((QTextTableFormat *)cls)->setColumns (arg1);
}

static qt_gsi::GenericMethod *_create_f_setColumns_767 ()
{
  return new qt_gsi::GenericMethod ("setColumns|columns=", "@brief Method void QTextTableFormat::setColumns(int columns)\n", false, &_init_f_setColumns_767, &_call_f_setColumns_767);
}

// void QTextTableFormat::setHeaderRowCount(int count)


static void _init_f_setHeaderRowCount_767 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("count");
  decl->add_arg<int > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setHeaderRowCount_767 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  int arg1 = args.read<int > ();
  ((QTextTableFormat *)cls)->setHeaderRowCount (arg1);
}

static qt_gsi::GenericMethod *_create_f_setHeaderRowCount_767 ()
{
  return new qt_gsi::GenericMethod ("setHeaderRowCount|headerRowCount=", "@brief Method void QTextTableFormat::setHeaderRowCount(int count)\n", false, &_init_f_setHeaderRowCount_767, &_call_f_setHeaderRowCount_767);
}


namespace gsi
{
extern gsi::Class<QTextFrameFormat> decl_QTextFrameFormat;

gsi::Class<QTextTableFormat> decl_QTextTableFormat (decl_QTextFrameFormat, "QTextTableFormat",
  gsi::Methods(_create_ctor_QTextTableFormat_0 ()) +
  gsi::Methods(_create_f_alignment_c0 ()) +
  gsi::Methods(_create_f_cellPadding_c0 ()) +
  gsi::Methods(_create_f_cellSpacing_c0 ()) +
  gsi::Methods(_create_f_clearColumnWidthConstraints_0 ()) +
  gsi::Methods(_create_f_columnWidthConstraints_c0 ()) +
  gsi::Methods(_create_f_columns_c0 ()) +
  gsi::Methods(_create_f_headerRowCount_c0 ()) +
  gsi::Methods(_create_f_isValid_c0 ()) +
  gsi::Methods(_create_f_setAlignment_2750 ()) +
  gsi::Methods(_create_f_setCellPadding_1071 ()) +
  gsi::Methods(_create_f_setCellSpacing_1071 ()) +
  gsi::Methods(_create_f_setColumnWidthConstraints_3255 ()) +
  gsi::Methods(_create_f_setColumns_767 ()) +
  gsi::Methods(_create_f_setHeaderRowCount_767 ()),
  "@qt\n@brief Binding of QTextTableFormat");

}
