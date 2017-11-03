
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
*  @file gsiDeclQGraphicsSceneWheelEvent.cc 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQtTypeTraits.h"
#include "gsiDeclQGraphicsSceneWheelEvent_Enums.h"
#include <memory>

// -----------------------------------------------------------------------
// class QGraphicsSceneWheelEvent

// QFlags<Qt::MouseButton> QGraphicsSceneWheelEvent::buttons()


static void _init_f_buttons_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QFlags<Qt::MouseButton> >::target_type > ();
}

static void _call_f_buttons_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QFlags<Qt::MouseButton> >::target_type > ((qt_gsi::Converter<QFlags<Qt::MouseButton> >::target_type)qt_gsi::CppToQtAdaptor<QFlags<Qt::MouseButton> >(((QGraphicsSceneWheelEvent *)cls)->buttons ()));
}

static qt_gsi::GenericMethod *_create_f_buttons_c0 ()
{
  return new qt_gsi::GenericMethod (":buttons", "@brief Method QFlags<Qt::MouseButton> QGraphicsSceneWheelEvent::buttons()\n", true, &_init_f_buttons_c0, &_call_f_buttons_c0);
}

// int QGraphicsSceneWheelEvent::delta()


static void _init_f_delta_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<int > ();
}

static void _call_f_delta_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<int > ((int)((QGraphicsSceneWheelEvent *)cls)->delta ());
}

static qt_gsi::GenericMethod *_create_f_delta_c0 ()
{
  return new qt_gsi::GenericMethod (":delta", "@brief Method int QGraphicsSceneWheelEvent::delta()\n", true, &_init_f_delta_c0, &_call_f_delta_c0);
}

// QFlags<Qt::KeyboardModifier> QGraphicsSceneWheelEvent::modifiers()


static void _init_f_modifiers_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QFlags<Qt::KeyboardModifier> >::target_type > ();
}

static void _call_f_modifiers_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QFlags<Qt::KeyboardModifier> >::target_type > ((qt_gsi::Converter<QFlags<Qt::KeyboardModifier> >::target_type)qt_gsi::CppToQtAdaptor<QFlags<Qt::KeyboardModifier> >(((QGraphicsSceneWheelEvent *)cls)->modifiers ()));
}

static qt_gsi::GenericMethod *_create_f_modifiers_c0 ()
{
  return new qt_gsi::GenericMethod (":modifiers", "@brief Method QFlags<Qt::KeyboardModifier> QGraphicsSceneWheelEvent::modifiers()\n", true, &_init_f_modifiers_c0, &_call_f_modifiers_c0);
}

// Qt::Orientation QGraphicsSceneWheelEvent::orientation()


static void _init_f_orientation_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<Qt::Orientation>::target_type > ();
}

static void _call_f_orientation_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<Qt::Orientation>::target_type > ((qt_gsi::Converter<Qt::Orientation>::target_type)qt_gsi::CppToQtAdaptor<Qt::Orientation>(((QGraphicsSceneWheelEvent *)cls)->orientation ()));
}

static qt_gsi::GenericMethod *_create_f_orientation_c0 ()
{
  return new qt_gsi::GenericMethod (":orientation", "@brief Method Qt::Orientation QGraphicsSceneWheelEvent::orientation()\n", true, &_init_f_orientation_c0, &_call_f_orientation_c0);
}

// QPointF QGraphicsSceneWheelEvent::pos()


static void _init_f_pos_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QPointF > ();
}

static void _call_f_pos_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QPointF > ((QPointF)((QGraphicsSceneWheelEvent *)cls)->pos ());
}

static qt_gsi::GenericMethod *_create_f_pos_c0 ()
{
  return new qt_gsi::GenericMethod (":pos", "@brief Method QPointF QGraphicsSceneWheelEvent::pos()\n", true, &_init_f_pos_c0, &_call_f_pos_c0);
}

// QPointF QGraphicsSceneWheelEvent::scenePos()


static void _init_f_scenePos_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QPointF > ();
}

static void _call_f_scenePos_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QPointF > ((QPointF)((QGraphicsSceneWheelEvent *)cls)->scenePos ());
}

static qt_gsi::GenericMethod *_create_f_scenePos_c0 ()
{
  return new qt_gsi::GenericMethod (":scenePos", "@brief Method QPointF QGraphicsSceneWheelEvent::scenePos()\n", true, &_init_f_scenePos_c0, &_call_f_scenePos_c0);
}

// QPoint QGraphicsSceneWheelEvent::screenPos()


static void _init_f_screenPos_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QPoint > ();
}

static void _call_f_screenPos_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QPoint > ((QPoint)((QGraphicsSceneWheelEvent *)cls)->screenPos ());
}

static qt_gsi::GenericMethod *_create_f_screenPos_c0 ()
{
  return new qt_gsi::GenericMethod (":screenPos", "@brief Method QPoint QGraphicsSceneWheelEvent::screenPos()\n", true, &_init_f_screenPos_c0, &_call_f_screenPos_c0);
}

// void QGraphicsSceneWheelEvent::setButtons(QFlags<Qt::MouseButton> buttons)


static void _init_f_setButtons_2602 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("buttons");
  decl->add_arg<const qt_gsi::Converter<QFlags<Qt::MouseButton> >::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setButtons_2602 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<Qt::MouseButton> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<Qt::MouseButton> >::target_type & > ();
  ((QGraphicsSceneWheelEvent *)cls)->setButtons (qt_gsi::QtToCppAdaptor<QFlags<Qt::MouseButton> >(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setButtons_2602 ()
{
  return new qt_gsi::GenericMethod ("setButtons|buttons=", "@brief Method void QGraphicsSceneWheelEvent::setButtons(QFlags<Qt::MouseButton> buttons)\n", false, &_init_f_setButtons_2602, &_call_f_setButtons_2602);
}

// void QGraphicsSceneWheelEvent::setDelta(int delta)


static void _init_f_setDelta_767 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("delta");
  decl->add_arg<int > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setDelta_767 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  int arg1 = args.read<int > ();
  ((QGraphicsSceneWheelEvent *)cls)->setDelta (arg1);
}

static qt_gsi::GenericMethod *_create_f_setDelta_767 ()
{
  return new qt_gsi::GenericMethod ("setDelta|delta=", "@brief Method void QGraphicsSceneWheelEvent::setDelta(int delta)\n", false, &_init_f_setDelta_767, &_call_f_setDelta_767);
}

// void QGraphicsSceneWheelEvent::setModifiers(QFlags<Qt::KeyboardModifier> modifiers)


static void _init_f_setModifiers_3077 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("modifiers");
  decl->add_arg<const qt_gsi::Converter<QFlags<Qt::KeyboardModifier> >::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setModifiers_3077 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<Qt::KeyboardModifier> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<Qt::KeyboardModifier> >::target_type & > ();
  ((QGraphicsSceneWheelEvent *)cls)->setModifiers (qt_gsi::QtToCppAdaptor<QFlags<Qt::KeyboardModifier> >(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setModifiers_3077 ()
{
  return new qt_gsi::GenericMethod ("setModifiers|modifiers=", "@brief Method void QGraphicsSceneWheelEvent::setModifiers(QFlags<Qt::KeyboardModifier> modifiers)\n", false, &_init_f_setModifiers_3077, &_call_f_setModifiers_3077);
}

// void QGraphicsSceneWheelEvent::setOrientation(Qt::Orientation orientation)


static void _init_f_setOrientation_1913 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("orientation");
  decl->add_arg<const qt_gsi::Converter<Qt::Orientation>::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setOrientation_1913 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<Qt::Orientation>::target_type & arg1 = args.read<const qt_gsi::Converter<Qt::Orientation>::target_type & > ();
  ((QGraphicsSceneWheelEvent *)cls)->setOrientation (qt_gsi::QtToCppAdaptor<Qt::Orientation>(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setOrientation_1913 ()
{
  return new qt_gsi::GenericMethod ("setOrientation|orientation=", "@brief Method void QGraphicsSceneWheelEvent::setOrientation(Qt::Orientation orientation)\n", false, &_init_f_setOrientation_1913, &_call_f_setOrientation_1913);
}

// void QGraphicsSceneWheelEvent::setPos(const QPointF &pos)


static void _init_f_setPos_1986 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("pos");
  decl->add_arg<const QPointF & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setPos_1986 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QPointF &arg1 = args.read<const QPointF & > ();
  ((QGraphicsSceneWheelEvent *)cls)->setPos (arg1);
}

static qt_gsi::GenericMethod *_create_f_setPos_1986 ()
{
  return new qt_gsi::GenericMethod ("setPos|pos=", "@brief Method void QGraphicsSceneWheelEvent::setPos(const QPointF &pos)\n", false, &_init_f_setPos_1986, &_call_f_setPos_1986);
}

// void QGraphicsSceneWheelEvent::setScenePos(const QPointF &pos)


static void _init_f_setScenePos_1986 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("pos");
  decl->add_arg<const QPointF & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setScenePos_1986 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QPointF &arg1 = args.read<const QPointF & > ();
  ((QGraphicsSceneWheelEvent *)cls)->setScenePos (arg1);
}

static qt_gsi::GenericMethod *_create_f_setScenePos_1986 ()
{
  return new qt_gsi::GenericMethod ("setScenePos|scenePos=", "@brief Method void QGraphicsSceneWheelEvent::setScenePos(const QPointF &pos)\n", false, &_init_f_setScenePos_1986, &_call_f_setScenePos_1986);
}

// void QGraphicsSceneWheelEvent::setScreenPos(const QPoint &pos)


static void _init_f_setScreenPos_1916 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("pos");
  decl->add_arg<const QPoint & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setScreenPos_1916 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QPoint &arg1 = args.read<const QPoint & > ();
  ((QGraphicsSceneWheelEvent *)cls)->setScreenPos (arg1);
}

static qt_gsi::GenericMethod *_create_f_setScreenPos_1916 ()
{
  return new qt_gsi::GenericMethod ("setScreenPos|screenPos=", "@brief Method void QGraphicsSceneWheelEvent::setScreenPos(const QPoint &pos)\n", false, &_init_f_setScreenPos_1916, &_call_f_setScreenPos_1916);
}

namespace gsi
{
extern gsi::Class<QGraphicsSceneEvent> decl_QGraphicsSceneEvent;

gsi::Class<QGraphicsSceneWheelEvent> decl_QGraphicsSceneWheelEvent (decl_QGraphicsSceneEvent, "QGraphicsSceneWheelEvent_Native",
  gsi::Methods(_create_f_buttons_c0 ()) +
  gsi::Methods(_create_f_delta_c0 ()) +
  gsi::Methods(_create_f_modifiers_c0 ()) +
  gsi::Methods(_create_f_orientation_c0 ()) +
  gsi::Methods(_create_f_pos_c0 ()) +
  gsi::Methods(_create_f_scenePos_c0 ()) +
  gsi::Methods(_create_f_screenPos_c0 ()) +
  gsi::Methods(_create_f_setButtons_2602 ()) +
  gsi::Methods(_create_f_setDelta_767 ()) +
  gsi::Methods(_create_f_setModifiers_3077 ()) +
  gsi::Methods(_create_f_setOrientation_1913 ()) +
  gsi::Methods(_create_f_setPos_1986 ()) +
  gsi::Methods(_create_f_setScenePos_1986 ()) +
  gsi::Methods(_create_f_setScreenPos_1916 ()),
  "@hide\n@alias QGraphicsSceneWheelEvent");
}


class QGraphicsSceneWheelEvent_Adaptor : public QGraphicsSceneWheelEvent, public qt_gsi::QtObjectBase
{
public:

  virtual ~QGraphicsSceneWheelEvent_Adaptor();

  //  [adaptor ctor] QGraphicsSceneWheelEvent::QGraphicsSceneWheelEvent(QEvent::Type type)
  QGraphicsSceneWheelEvent_Adaptor() : QGraphicsSceneWheelEvent()
  {
    qt_gsi::QtObjectBase::init (this);
  }

  //  [adaptor ctor] QGraphicsSceneWheelEvent::QGraphicsSceneWheelEvent(QEvent::Type type)
  QGraphicsSceneWheelEvent_Adaptor(QEvent::Type type) : QGraphicsSceneWheelEvent(type)
  {
    qt_gsi::QtObjectBase::init (this);
  }

  
};

QGraphicsSceneWheelEvent_Adaptor::~QGraphicsSceneWheelEvent_Adaptor() { }

//  Constructor QGraphicsSceneWheelEvent::QGraphicsSceneWheelEvent(QEvent::Type type) (adaptor class)

static void _init_ctor_QGraphicsSceneWheelEvent_Adaptor_1565 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("type", true, "QEvent::None");
  decl->add_arg<const qt_gsi::Converter<QEvent::Type>::target_type & > (argspec_0);
  decl->set_return_new<QGraphicsSceneWheelEvent_Adaptor *> ();
}

static void _call_ctor_QGraphicsSceneWheelEvent_Adaptor_1565 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  tl::Heap heap;
  const qt_gsi::Converter<QEvent::Type>::target_type & arg1 = args ? args.read<const qt_gsi::Converter<QEvent::Type>::target_type & > () : (const qt_gsi::Converter<QEvent::Type>::target_type &)(qt_gsi::CppToQtReadAdaptor<QEvent::Type>(heap, QEvent::None));
  ret.write<QGraphicsSceneWheelEvent_Adaptor *> (new QGraphicsSceneWheelEvent_Adaptor (qt_gsi::QtToCppAdaptor<QEvent::Type>(arg1).cref()));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QGraphicsSceneWheelEvent_Adaptor_1565 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QGraphicsSceneWheelEvent::QGraphicsSceneWheelEvent(QEvent::Type type)\nThis method creates an object of class QGraphicsSceneWheelEvent.", &_init_ctor_QGraphicsSceneWheelEvent_Adaptor_1565, &_call_ctor_QGraphicsSceneWheelEvent_Adaptor_1565);
}

namespace gsi
{

extern gsi::Class<QGraphicsSceneWheelEvent> decl_QGraphicsSceneWheelEvent;

gsi::Class<QGraphicsSceneWheelEvent_Adaptor> decl_QGraphicsSceneWheelEvent_Adaptor (decl_QGraphicsSceneWheelEvent, "QGraphicsSceneWheelEvent",
  gsi::Methods(_create_ctor_QGraphicsSceneWheelEvent_Adaptor_1565 ()),
  "@qt\n@brief Binding of QGraphicsSceneWheelEvent");

}
