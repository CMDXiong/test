
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
*  @file gsiDeclQWindowStateChangeEvent.cc 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQtTypeTraits.h"
#include "gsiDeclQWindowStateChangeEvent_Enums.h"
#include <memory>

// -----------------------------------------------------------------------
// class QWindowStateChangeEvent

// bool QWindowStateChangeEvent::isOverride()


static void _init_f_isOverride_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<bool > ();
}

static void _call_f_isOverride_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<bool > ((bool)((QWindowStateChangeEvent *)cls)->isOverride ());
}

static qt_gsi::GenericMethod *_create_f_isOverride_c0 ()
{
  return new qt_gsi::GenericMethod ("isOverride?", "@brief Method bool QWindowStateChangeEvent::isOverride()\n", true, &_init_f_isOverride_c0, &_call_f_isOverride_c0);
}

// QFlags<Qt::WindowState> QWindowStateChangeEvent::oldState()


static void _init_f_oldState_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type > ();
}

static void _call_f_oldState_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type > ((qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type)qt_gsi::CppToQtAdaptor<QFlags<Qt::WindowState> >(((QWindowStateChangeEvent *)cls)->oldState ()));
}

static qt_gsi::GenericMethod *_create_f_oldState_c0 ()
{
  return new qt_gsi::GenericMethod ("oldState", "@brief Method QFlags<Qt::WindowState> QWindowStateChangeEvent::oldState()\n", true, &_init_f_oldState_c0, &_call_f_oldState_c0);
}

namespace gsi
{
extern gsi::Class<QEvent> decl_QEvent;

gsi::Class<QWindowStateChangeEvent> decl_QWindowStateChangeEvent (decl_QEvent, "QWindowStateChangeEvent_Native",
  gsi::Methods(_create_f_isOverride_c0 ()) +
  gsi::Methods(_create_f_oldState_c0 ()),
  "@hide\n@alias QWindowStateChangeEvent");
}


class QWindowStateChangeEvent_Adaptor : public QWindowStateChangeEvent, public qt_gsi::QtObjectBase
{
public:

  virtual ~QWindowStateChangeEvent_Adaptor();

  //  [adaptor ctor] QWindowStateChangeEvent::QWindowStateChangeEvent(QFlags<Qt::WindowState> aOldState)
  QWindowStateChangeEvent_Adaptor(QFlags<Qt::WindowState> aOldState) : QWindowStateChangeEvent(aOldState)
  {
    qt_gsi::QtObjectBase::init (this);
  }

  //  [adaptor ctor] QWindowStateChangeEvent::QWindowStateChangeEvent(QFlags<Qt::WindowState> aOldState, bool isOverride)
  QWindowStateChangeEvent_Adaptor(QFlags<Qt::WindowState> aOldState, bool isOverride) : QWindowStateChangeEvent(aOldState, isOverride)
  {
    qt_gsi::QtObjectBase::init (this);
  }

  
};

QWindowStateChangeEvent_Adaptor::~QWindowStateChangeEvent_Adaptor() { }

//  Constructor QWindowStateChangeEvent::QWindowStateChangeEvent(QFlags<Qt::WindowState> aOldState) (adaptor class)

static void _init_ctor_QWindowStateChangeEvent_Adaptor_2590 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("aOldState");
  decl->add_arg<const qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type & > (argspec_0);
  decl->set_return_new<QWindowStateChangeEvent_Adaptor *> ();
}

static void _call_ctor_QWindowStateChangeEvent_Adaptor_2590 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type & > ();
  ret.write<QWindowStateChangeEvent_Adaptor *> (new QWindowStateChangeEvent_Adaptor (qt_gsi::QtToCppAdaptor<QFlags<Qt::WindowState> >(arg1).cref()));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QWindowStateChangeEvent_Adaptor_2590 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QWindowStateChangeEvent::QWindowStateChangeEvent(QFlags<Qt::WindowState> aOldState)\nThis method creates an object of class QWindowStateChangeEvent.", &_init_ctor_QWindowStateChangeEvent_Adaptor_2590, &_call_ctor_QWindowStateChangeEvent_Adaptor_2590);
}

//  Constructor QWindowStateChangeEvent::QWindowStateChangeEvent(QFlags<Qt::WindowState> aOldState, bool isOverride) (adaptor class)

static void _init_ctor_QWindowStateChangeEvent_Adaptor_3346 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("aOldState");
  decl->add_arg<const qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type & > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("isOverride");
  decl->add_arg<bool > (argspec_1);
  decl->set_return_new<QWindowStateChangeEvent_Adaptor *> ();
}

static void _call_ctor_QWindowStateChangeEvent_Adaptor_3346 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<Qt::WindowState> >::target_type & > ();
  bool arg2 = args.read<bool > ();
  ret.write<QWindowStateChangeEvent_Adaptor *> (new QWindowStateChangeEvent_Adaptor (qt_gsi::QtToCppAdaptor<QFlags<Qt::WindowState> >(arg1).cref(), arg2));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QWindowStateChangeEvent_Adaptor_3346 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QWindowStateChangeEvent::QWindowStateChangeEvent(QFlags<Qt::WindowState> aOldState, bool isOverride)\nThis method creates an object of class QWindowStateChangeEvent.", &_init_ctor_QWindowStateChangeEvent_Adaptor_3346, &_call_ctor_QWindowStateChangeEvent_Adaptor_3346);
}

namespace gsi
{

extern gsi::Class<QWindowStateChangeEvent> decl_QWindowStateChangeEvent;

gsi::Class<QWindowStateChangeEvent_Adaptor> decl_QWindowStateChangeEvent_Adaptor (decl_QWindowStateChangeEvent, "QWindowStateChangeEvent",
  gsi::Methods(_create_ctor_QWindowStateChangeEvent_Adaptor_2590 ()) +
  gsi::Methods(_create_ctor_QWindowStateChangeEvent_Adaptor_3346 ()),
  "@qt\n@brief Binding of QWindowStateChangeEvent");

}
