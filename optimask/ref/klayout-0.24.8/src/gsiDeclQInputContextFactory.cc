
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
*  @file gsiDeclQInputContextFactory.cc 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQtTypeTraits.h"
#include "gsiDeclQInputContextFactory_Enums.h"
#include <memory>

// -----------------------------------------------------------------------
// class QInputContextFactory

//  Constructor QInputContextFactory::QInputContextFactory()


static void _init_ctor_QInputContextFactory_0 (qt_gsi::GenericStaticMethod *decl)
{
  decl->set_return_new<QInputContextFactory *> ();
}

static void _call_ctor_QInputContextFactory_0 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QInputContextFactory *> (new QInputContextFactory ());
}

static qt_gsi::GenericStaticMethod *_create_ctor_QInputContextFactory_0 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QInputContextFactory::QInputContextFactory()\nThis method creates an object of class QInputContextFactory.", &_init_ctor_QInputContextFactory_0, &_call_ctor_QInputContextFactory_0);
}

// static QInputContext *QInputContextFactory::create(const QString &key, QObject *parent)


static void _init_f_create_3219 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("key");
  decl->add_arg<const QString & > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("parent");
  decl->add_arg<QObject * > (argspec_1);
  decl->set_return<QInputContext * > ();
}

static void _call_f_create_3219 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QString &arg1 = args.read<const QString & > ();
  QObject *arg2 = args.read<QObject * > ();
  ret.write<QInputContext * > ((QInputContext *)QInputContextFactory::create (arg1, arg2));
}

static qt_gsi::GenericStaticMethod *_create_f_create_3219 ()
{
  return new qt_gsi::GenericStaticMethod ("qt_create", "@brief Static method QInputContext *QInputContextFactory::create(const QString &key, QObject *parent)\nThis method is static and can be called without an instance.", &_init_f_create_3219, &_call_f_create_3219);
}

// static QString QInputContextFactory::description(const QString &key)


static void _init_f_description_2025 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("key");
  decl->add_arg<const QString & > (argspec_0);
  decl->set_return<QString > ();
}

static void _call_f_description_2025 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QString &arg1 = args.read<const QString & > ();
  ret.write<QString > ((QString)QInputContextFactory::description (arg1));
}

static qt_gsi::GenericStaticMethod *_create_f_description_2025 ()
{
  return new qt_gsi::GenericStaticMethod ("description", "@brief Static method QString QInputContextFactory::description(const QString &key)\nThis method is static and can be called without an instance.", &_init_f_description_2025, &_call_f_description_2025);
}

// static QString QInputContextFactory::displayName(const QString &key)


static void _init_f_displayName_2025 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("key");
  decl->add_arg<const QString & > (argspec_0);
  decl->set_return<QString > ();
}

static void _call_f_displayName_2025 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QString &arg1 = args.read<const QString & > ();
  ret.write<QString > ((QString)QInputContextFactory::displayName (arg1));
}

static qt_gsi::GenericStaticMethod *_create_f_displayName_2025 ()
{
  return new qt_gsi::GenericStaticMethod ("displayName", "@brief Static method QString QInputContextFactory::displayName(const QString &key)\nThis method is static and can be called without an instance.", &_init_f_displayName_2025, &_call_f_displayName_2025);
}

// static QStringList QInputContextFactory::keys()


static void _init_f_keys_0 (qt_gsi::GenericStaticMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QStringList>::target_type > ();
}

static void _call_f_keys_0 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QStringList>::target_type > ((qt_gsi::Converter<QStringList>::target_type)qt_gsi::CppToQtAdaptor<QStringList>(QInputContextFactory::keys ()));
}

static qt_gsi::GenericStaticMethod *_create_f_keys_0 ()
{
  return new qt_gsi::GenericStaticMethod ("keys", "@brief Static method QStringList QInputContextFactory::keys()\nThis method is static and can be called without an instance.", &_init_f_keys_0, &_call_f_keys_0);
}

// static QStringList QInputContextFactory::languages(const QString &key)


static void _init_f_languages_2025 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("key");
  decl->add_arg<const QString & > (argspec_0);
  decl->set_return<qt_gsi::Converter<QStringList>::target_type > ();
}

static void _call_f_languages_2025 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QString &arg1 = args.read<const QString & > ();
  ret.write<qt_gsi::Converter<QStringList>::target_type > ((qt_gsi::Converter<QStringList>::target_type)qt_gsi::CppToQtAdaptor<QStringList>(QInputContextFactory::languages (arg1)));
}

static qt_gsi::GenericStaticMethod *_create_f_languages_2025 ()
{
  return new qt_gsi::GenericStaticMethod ("languages", "@brief Static method QStringList QInputContextFactory::languages(const QString &key)\nThis method is static and can be called without an instance.", &_init_f_languages_2025, &_call_f_languages_2025);
}


namespace gsi
{
gsi::Class<QInputContextFactory> decl_QInputContextFactory ("QInputContextFactory",
  gsi::Methods(_create_ctor_QInputContextFactory_0 ()) +
  gsi::Methods(_create_f_create_3219 ()) +
  gsi::Methods(_create_f_description_2025 ()) +
  gsi::Methods(_create_f_displayName_2025 ()) +
  gsi::Methods(_create_f_keys_0 ()) +
  gsi::Methods(_create_f_languages_2025 ()),
  "@qt\n@brief Binding of QInputContextFactory");

}
