
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
*  @file gsiDeclQPageSetupDialog.cc 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtGui.h"
#include "gsiDeclQtTypeTraits.h"
#include "gsiDeclQPageSetupDialog_Enums.h"
#include <memory>

// -----------------------------------------------------------------------
// class QPageSetupDialog

//  get static meta object

static void _init_smo (qt_gsi::GenericStaticMethod *decl)
{
  decl->set_return<const QMetaObject &> ();
}

static void _call_smo (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<const QMetaObject &> (QPageSetupDialog::staticMetaObject);
}

static qt_gsi::GenericStaticMethod *_create_smo ()
{
  return new qt_gsi::GenericStaticMethod ("staticMetaObject", "@brief Obtains the static MetaObject for this class.", &_init_smo, &_call_smo);
}

// void QPageSetupDialog::addEnabledOption(QPageSetupDialog::PageSetupDialogOption option)


static void _init_f_addEnabledOption_4270 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("option");
  decl->add_arg<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_addEnabledOption_4270 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & arg1 = args.read<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > ();
  ((QPageSetupDialog *)cls)->addEnabledOption (qt_gsi::QtToCppAdaptor<QPageSetupDialog::PageSetupDialogOption>(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_addEnabledOption_4270 ()
{
  return new qt_gsi::GenericMethod ("addEnabledOption", "@brief Method void QPageSetupDialog::addEnabledOption(QPageSetupDialog::PageSetupDialogOption option)\n", false, &_init_f_addEnabledOption_4270, &_call_f_addEnabledOption_4270);
}

// QFlags<QPageSetupDialog::PageSetupDialogOption> QPageSetupDialog::enabledOptions()


static void _init_f_enabledOptions_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type > ();
}

static void _call_f_enabledOptions_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type > ((qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type)qt_gsi::CppToQtAdaptor<QFlags<QPageSetupDialog::PageSetupDialogOption> >(((QPageSetupDialog *)cls)->enabledOptions ()));
}

static qt_gsi::GenericMethod *_create_f_enabledOptions_c0 ()
{
  return new qt_gsi::GenericMethod (":enabledOptions", "@brief Method QFlags<QPageSetupDialog::PageSetupDialogOption> QPageSetupDialog::enabledOptions()\n", true, &_init_f_enabledOptions_c0, &_call_f_enabledOptions_c0);
}

// int QPageSetupDialog::exec()


static void _init_f_exec_0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<int > ();
}

static void _call_f_exec_0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<int > ((int)((QPageSetupDialog *)cls)->exec ());
}

static qt_gsi::GenericMethod *_create_f_exec_0 ()
{
  return new qt_gsi::GenericMethod ("exec", "@brief Method int QPageSetupDialog::exec()\n", false, &_init_f_exec_0, &_call_f_exec_0);
}

// bool QPageSetupDialog::isOptionEnabled(QPageSetupDialog::PageSetupDialogOption option)


static void _init_f_isOptionEnabled_c4270 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("option");
  decl->add_arg<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > (argspec_0);
  decl->set_return<bool > ();
}

static void _call_f_isOptionEnabled_c4270 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & arg1 = args.read<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > ();
  ret.write<bool > ((bool)((QPageSetupDialog *)cls)->isOptionEnabled (qt_gsi::QtToCppAdaptor<QPageSetupDialog::PageSetupDialogOption>(arg1).cref()));
}

static qt_gsi::GenericMethod *_create_f_isOptionEnabled_c4270 ()
{
  return new qt_gsi::GenericMethod ("isOptionEnabled?", "@brief Method bool QPageSetupDialog::isOptionEnabled(QPageSetupDialog::PageSetupDialogOption option)\n", true, &_init_f_isOptionEnabled_c4270, &_call_f_isOptionEnabled_c4270);
}

// void QPageSetupDialog::open()


static void _init_f_open_0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<void > ();
}

static void _call_f_open_0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ((QPageSetupDialog *)cls)->open ();
}

static qt_gsi::GenericMethod *_create_f_open_0 ()
{
  return new qt_gsi::GenericMethod ("open", "@brief Method void QPageSetupDialog::open()\n", false, &_init_f_open_0, &_call_f_open_0);
}

// void QPageSetupDialog::open(QObject *receiver, const char *member)


static void _init_f_open_2925 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("receiver");
  decl->add_arg<QObject * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("member");
  decl->add_arg<const char * > (argspec_1);
  decl->set_return<void > ();
}

static void _call_f_open_2925 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  QObject *arg1 = args.read<QObject * > ();
  const char *arg2 = args.read<const char * > ();
  ((QPageSetupDialog *)cls)->open (arg1, arg2);
}

static qt_gsi::GenericMethod *_create_f_open_2925 ()
{
  return new qt_gsi::GenericMethod ("open", "@brief Method void QPageSetupDialog::open(QObject *receiver, const char *member)\n", false, &_init_f_open_2925, &_call_f_open_2925);
}

// QFlags<QPageSetupDialog::PageSetupDialogOption> QPageSetupDialog::options()


static void _init_f_options_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type > ();
}

static void _call_f_options_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type > ((qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type)qt_gsi::CppToQtAdaptor<QFlags<QPageSetupDialog::PageSetupDialogOption> >(((QPageSetupDialog *)cls)->options ()));
}

static qt_gsi::GenericMethod *_create_f_options_c0 ()
{
  return new qt_gsi::GenericMethod (":options", "@brief Method QFlags<QPageSetupDialog::PageSetupDialogOption> QPageSetupDialog::options()\n", true, &_init_f_options_c0, &_call_f_options_c0);
}

// void QPageSetupDialog::setEnabledOptions(QFlags<QPageSetupDialog::PageSetupDialogOption> options)


static void _init_f_setEnabledOptions_4966 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("options");
  decl->add_arg<const qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setEnabledOptions_4966 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type & > ();
  ((QPageSetupDialog *)cls)->setEnabledOptions (qt_gsi::QtToCppAdaptor<QFlags<QPageSetupDialog::PageSetupDialogOption> >(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setEnabledOptions_4966 ()
{
  return new qt_gsi::GenericMethod ("setEnabledOptions|enabledOptions=", "@brief Method void QPageSetupDialog::setEnabledOptions(QFlags<QPageSetupDialog::PageSetupDialogOption> options)\n", false, &_init_f_setEnabledOptions_4966, &_call_f_setEnabledOptions_4966);
}

// void QPageSetupDialog::setOption(QPageSetupDialog::PageSetupDialogOption option, bool on)


static void _init_f_setOption_5026 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("option");
  decl->add_arg<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("on", true, "true");
  decl->add_arg<bool > (argspec_1);
  decl->set_return<void > ();
}

static void _call_f_setOption_5026 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & arg1 = args.read<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > ();
  bool arg2 = args ? args.read<bool > () : (bool)(true);
  ((QPageSetupDialog *)cls)->setOption (qt_gsi::QtToCppAdaptor<QPageSetupDialog::PageSetupDialogOption>(arg1).cref(), arg2);
}

static qt_gsi::GenericMethod *_create_f_setOption_5026 ()
{
  return new qt_gsi::GenericMethod ("setOption", "@brief Method void QPageSetupDialog::setOption(QPageSetupDialog::PageSetupDialogOption option, bool on)\n", false, &_init_f_setOption_5026, &_call_f_setOption_5026);
}

// void QPageSetupDialog::setOptions(QFlags<QPageSetupDialog::PageSetupDialogOption> options)


static void _init_f_setOptions_4966 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("options");
  decl->add_arg<const qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type & > (argspec_0);
  decl->set_return<void > ();
}

static void _call_f_setOptions_4966 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type & arg1 = args.read<const qt_gsi::Converter<QFlags<QPageSetupDialog::PageSetupDialogOption> >::target_type & > ();
  ((QPageSetupDialog *)cls)->setOptions (qt_gsi::QtToCppAdaptor<QFlags<QPageSetupDialog::PageSetupDialogOption> >(arg1).cref());
}

static qt_gsi::GenericMethod *_create_f_setOptions_4966 ()
{
  return new qt_gsi::GenericMethod ("setOptions|options=", "@brief Method void QPageSetupDialog::setOptions(QFlags<QPageSetupDialog::PageSetupDialogOption> options)\n", false, &_init_f_setOptions_4966, &_call_f_setOptions_4966);
}

// bool QPageSetupDialog::testOption(QPageSetupDialog::PageSetupDialogOption option)


static void _init_f_testOption_c4270 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("option");
  decl->add_arg<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > (argspec_0);
  decl->set_return<bool > ();
}

static void _call_f_testOption_c4270 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & arg1 = args.read<const qt_gsi::Converter<QPageSetupDialog::PageSetupDialogOption>::target_type & > ();
  ret.write<bool > ((bool)((QPageSetupDialog *)cls)->testOption (qt_gsi::QtToCppAdaptor<QPageSetupDialog::PageSetupDialogOption>(arg1).cref()));
}

static qt_gsi::GenericMethod *_create_f_testOption_c4270 ()
{
  return new qt_gsi::GenericMethod ("testOption", "@brief Method bool QPageSetupDialog::testOption(QPageSetupDialog::PageSetupDialogOption option)\n", true, &_init_f_testOption_c4270, &_call_f_testOption_c4270);
}

// static QString QPageSetupDialog::tr(const char *s, const char *c)


static void _init_f_tr_3354 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("s");
  decl->add_arg<const char * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("c", true, "0");
  decl->add_arg<const char * > (argspec_1);
  decl->set_return<QString > ();
}

static void _call_f_tr_3354 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const char *arg1 = args.read<const char * > ();
  const char *arg2 = args ? args.read<const char * > () : (const char *)(0);
  ret.write<QString > ((QString)QPageSetupDialog::tr (arg1, arg2));
}

static qt_gsi::GenericStaticMethod *_create_f_tr_3354 ()
{
  return new qt_gsi::GenericStaticMethod ("tr", "@brief Static method QString QPageSetupDialog::tr(const char *s, const char *c)\nThis method is static and can be called without an instance.", &_init_f_tr_3354, &_call_f_tr_3354);
}

// static QString QPageSetupDialog::tr(const char *s, const char *c, int n)


static void _init_f_tr_4013 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("s");
  decl->add_arg<const char * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("c");
  decl->add_arg<const char * > (argspec_1);
  static gsi::ArgSpecBase argspec_2 ("n");
  decl->add_arg<int > (argspec_2);
  decl->set_return<QString > ();
}

static void _call_f_tr_4013 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const char *arg1 = args.read<const char * > ();
  const char *arg2 = args.read<const char * > ();
  int arg3 = args.read<int > ();
  ret.write<QString > ((QString)QPageSetupDialog::tr (arg1, arg2, arg3));
}

static qt_gsi::GenericStaticMethod *_create_f_tr_4013 ()
{
  return new qt_gsi::GenericStaticMethod ("tr", "@brief Static method QString QPageSetupDialog::tr(const char *s, const char *c, int n)\nThis method is static and can be called without an instance.", &_init_f_tr_4013, &_call_f_tr_4013);
}

// static QString QPageSetupDialog::trUtf8(const char *s, const char *c)


static void _init_f_trUtf8_3354 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("s");
  decl->add_arg<const char * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("c", true, "0");
  decl->add_arg<const char * > (argspec_1);
  decl->set_return<QString > ();
}

static void _call_f_trUtf8_3354 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const char *arg1 = args.read<const char * > ();
  const char *arg2 = args ? args.read<const char * > () : (const char *)(0);
  ret.write<QString > ((QString)QPageSetupDialog::trUtf8 (arg1, arg2));
}

static qt_gsi::GenericStaticMethod *_create_f_trUtf8_3354 ()
{
  return new qt_gsi::GenericStaticMethod ("trUtf8", "@brief Static method QString QPageSetupDialog::trUtf8(const char *s, const char *c)\nThis method is static and can be called without an instance.", &_init_f_trUtf8_3354, &_call_f_trUtf8_3354);
}

// static QString QPageSetupDialog::trUtf8(const char *s, const char *c, int n)


static void _init_f_trUtf8_4013 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("s");
  decl->add_arg<const char * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("c");
  decl->add_arg<const char * > (argspec_1);
  static gsi::ArgSpecBase argspec_2 ("n");
  decl->add_arg<int > (argspec_2);
  decl->set_return<QString > ();
}

static void _call_f_trUtf8_4013 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const char *arg1 = args.read<const char * > ();
  const char *arg2 = args.read<const char * > ();
  int arg3 = args.read<int > ();
  ret.write<QString > ((QString)QPageSetupDialog::trUtf8 (arg1, arg2, arg3));
}

static qt_gsi::GenericStaticMethod *_create_f_trUtf8_4013 ()
{
  return new qt_gsi::GenericStaticMethod ("trUtf8", "@brief Static method QString QPageSetupDialog::trUtf8(const char *s, const char *c, int n)\nThis method is static and can be called without an instance.", &_init_f_trUtf8_4013, &_call_f_trUtf8_4013);
}

namespace gsi
{
gsi::Class<QPageSetupDialog> decl_QPageSetupDialog ("QPageSetupDialog_Native",
  gsi::Methods(_create_smo ()) +
  gsi::Methods(_create_f_addEnabledOption_4270 ()) +
  gsi::Methods(_create_f_enabledOptions_c0 ()) +
  gsi::Methods(_create_f_exec_0 ()) +
  gsi::Methods(_create_f_isOptionEnabled_c4270 ()) +
  gsi::Methods(_create_f_open_0 ()) +
  gsi::Methods(_create_f_open_2925 ()) +
  gsi::Methods(_create_f_options_c0 ()) +
  gsi::Methods(_create_f_setEnabledOptions_4966 ()) +
  gsi::Methods(_create_f_setOption_5026 ()) +
  gsi::Methods(_create_f_setOptions_4966 ()) +
  gsi::Methods(_create_f_testOption_c4270 ()) +
  gsi::Methods(_create_f_tr_3354 ()) +
  gsi::Methods(_create_f_tr_4013 ()) +
  gsi::Methods(_create_f_trUtf8_3354 ()) +
  gsi::Methods(_create_f_trUtf8_4013 ()),
  "@hide\n@alias QPageSetupDialog");
}


class QPageSetupDialog_Adaptor : public QPageSetupDialog, public qt_gsi::QtObjectBase
{
public:

  virtual ~QPageSetupDialog_Adaptor();

  //  [adaptor ctor] QPageSetupDialog::QPageSetupDialog(QPrinter *printer, QWidget *parent)
  QPageSetupDialog_Adaptor(QPrinter *printer) : QPageSetupDialog(printer)
  {
    qt_gsi::QtObjectBase::init (this);
  }

  //  [adaptor ctor] QPageSetupDialog::QPageSetupDialog(QPrinter *printer, QWidget *parent)
  QPageSetupDialog_Adaptor(QPrinter *printer, QWidget *parent) : QPageSetupDialog(printer, parent)
  {
    qt_gsi::QtObjectBase::init (this);
  }

  //  [adaptor ctor] QPageSetupDialog::QPageSetupDialog(QWidget *parent)
  QPageSetupDialog_Adaptor() : QPageSetupDialog()
  {
    qt_gsi::QtObjectBase::init (this);
  }

  //  [adaptor ctor] QPageSetupDialog::QPageSetupDialog(QWidget *parent)
  QPageSetupDialog_Adaptor(QWidget *parent) : QPageSetupDialog(parent)
  {
    qt_gsi::QtObjectBase::init (this);
  }

  //  [adaptor impl] int QPageSetupDialog::exec()
  int cbs_exec_0_0()
  {
    return QPageSetupDialog::exec();
  }

  virtual int exec()
  {
    if (cb_exec_0_0.can_issue()) {
      return cb_exec_0_0.issue<QPageSetupDialog_Adaptor, int>(&QPageSetupDialog_Adaptor::cbs_exec_0_0);
    } else {
      return QPageSetupDialog::exec();
    }
  }

  gsi::Callback cb_exec_0_0;
};

QPageSetupDialog_Adaptor::~QPageSetupDialog_Adaptor() { }

//  Constructor QPageSetupDialog::QPageSetupDialog(QPrinter *printer, QWidget *parent) (adaptor class)

static void _init_ctor_QPageSetupDialog_Adaptor_2650 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("printer");
  decl->add_arg<QPrinter * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("parent", true, "0");
  decl->add_arg<QWidget * > (argspec_1);
  decl->set_return_new<QPageSetupDialog_Adaptor *> ();
}

static void _call_ctor_QPageSetupDialog_Adaptor_2650 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  QPrinter *arg1 = args.read<QPrinter * > ();
  QWidget *arg2 = args ? args.read<QWidget * > () : (QWidget *)(0);
  ret.write<QPageSetupDialog_Adaptor *> (new QPageSetupDialog_Adaptor (arg1, arg2));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QPageSetupDialog_Adaptor_2650 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QPageSetupDialog::QPageSetupDialog(QPrinter *printer, QWidget *parent)\nThis method creates an object of class QPageSetupDialog.", &_init_ctor_QPageSetupDialog_Adaptor_2650, &_call_ctor_QPageSetupDialog_Adaptor_2650);
}

//  Constructor QPageSetupDialog::QPageSetupDialog(QWidget *parent) (adaptor class)

static void _init_ctor_QPageSetupDialog_Adaptor_1315 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("parent", true, "0");
  decl->add_arg<QWidget * > (argspec_0);
  decl->set_return_new<QPageSetupDialog_Adaptor *> ();
}

static void _call_ctor_QPageSetupDialog_Adaptor_1315 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  QWidget *arg1 = args ? args.read<QWidget * > () : (QWidget *)(0);
  ret.write<QPageSetupDialog_Adaptor *> (new QPageSetupDialog_Adaptor (arg1));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QPageSetupDialog_Adaptor_1315 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QPageSetupDialog::QPageSetupDialog(QWidget *parent)\nThis method creates an object of class QPageSetupDialog.", &_init_ctor_QPageSetupDialog_Adaptor_1315, &_call_ctor_QPageSetupDialog_Adaptor_1315);
}

// int QPageSetupDialog::exec()

static void _init_cbs_exec_0_0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<int > ();
}

static void _call_cbs_exec_0_0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<int > ((int)((QPageSetupDialog_Adaptor *)cls)->cbs_exec_0_0 ());
}

static void _set_callback_cbs_exec_0_0 (void *cls, const gsi::Callback &cb)
{
  ((QPageSetupDialog_Adaptor *)cls)->cb_exec_0_0 = cb;
}

static qt_gsi::GenericMethod *_create_cbs_exec_0_0 ()
{
  return new qt_gsi::GenericMethod ("exec", "@hide", false, &_init_cbs_exec_0_0, &_call_cbs_exec_0_0);
}

static qt_gsi::GenericMethod *_create_callback_cbs_exec_0_0 ()
{
  return new qt_gsi::GenericMethod ("exec", "@brief Virtual method int QPageSetupDialog::exec()\nThis method can be reimplemented in a derived class.", false, &_init_cbs_exec_0_0, &_call_cbs_exec_0_0, &_set_callback_cbs_exec_0_0);
}


namespace gsi
{

extern gsi::Class<QPageSetupDialog> decl_QPageSetupDialog;

gsi::Class<QPageSetupDialog_Adaptor> decl_QPageSetupDialog_Adaptor (decl_QPageSetupDialog, "QPageSetupDialog",
  gsi::Methods(_create_cbs_exec_0_0 ()) +
  gsi::Methods(_create_callback_cbs_exec_0_0 ()) +
  gsi::Methods(_create_ctor_QPageSetupDialog_Adaptor_2650 ()) +
  gsi::Methods(_create_ctor_QPageSetupDialog_Adaptor_1315 ()),
  "@qt\n@brief Binding of QPageSetupDialog");

}


//  Implementation of the enum wrapper class for QPageSetupDialog::PageSetupDialogOption
namespace qt_gsi
{

static QPageSetupDialog_PageSetupDialogOption_Enum *c_1_QPageSetupDialog_PageSetupDialogOption_Enum () { return new QPageSetupDialog_PageSetupDialogOption_Enum (QPageSetupDialog::None); }
static QPageSetupDialog_PageSetupDialogOption_Enum *c_2_QPageSetupDialog_PageSetupDialogOption_Enum () { return new QPageSetupDialog_PageSetupDialogOption_Enum (QPageSetupDialog::DontUseSheet); }
static QPageSetupDialog_PageSetupDialogOption_Enum *c_3_QPageSetupDialog_PageSetupDialogOption_Enum () { return new QPageSetupDialog_PageSetupDialogOption_Enum (QPageSetupDialog::OwnsPrinter); }

static gsi::Methods mkdefs_QPageSetupDialog_PageSetupDialogOption () {
  return
    gsi::constant ("None", &qt_gsi::c_1_QPageSetupDialog_PageSetupDialogOption_Enum, "@brief Enum constant QPageSetupDialog::None") +
    gsi::constant ("DontUseSheet", &qt_gsi::c_2_QPageSetupDialog_PageSetupDialogOption_Enum, "@brief Enum constant QPageSetupDialog::DontUseSheet") +
    gsi::constant ("OwnsPrinter", &qt_gsi::c_3_QPageSetupDialog_PageSetupDialogOption_Enum, "@brief Enum constant QPageSetupDialog::OwnsPrinter");
}

static std::string to_s_QPageSetupDialog_PageSetupDialogOption_Enum (const QPageSetupDialog_PageSetupDialogOption_Enum *e)
{
  if (e->value () == QPageSetupDialog::None) { return tl::sprintf ("None (%u)", (unsigned int) e->value ()); }
  if (e->value () == QPageSetupDialog::DontUseSheet) { return tl::sprintf ("DontUseSheet (%u)", (unsigned int) e->value ()); }
  if (e->value () == QPageSetupDialog::OwnsPrinter) { return tl::sprintf ("OwnsPrinter (%u)", (unsigned int) e->value ()); }
  return std::string ("(not a valid enum evalue)");
}

static unsigned int to_i_QPageSetupDialog_PageSetupDialogOption_Enum (const QPageSetupDialog_PageSetupDialogOption_Enum *e)
{
  return (unsigned int) e->value ();
}

static QPageSetupDialog_PageSetupDialogOption_Enum *from_i_QPageSetupDialog_PageSetupDialogOption_Enum (unsigned int i)
{
  return new QPageSetupDialog_PageSetupDialogOption_Enum ((QPageSetupDialog::PageSetupDialogOption) i);
}

static bool equalQPageSetupDialog_PageSetupDialogOption_Enum (const QPageSetupDialog_PageSetupDialogOption_Enum *e, QPageSetupDialog_PageSetupDialogOption_Enum d)
{
  return e->value () == d.value ();
}

static bool equal2QPageSetupDialog_PageSetupDialogOption_Enum (const QPageSetupDialog_PageSetupDialogOption_Enum *e, unsigned int i)
{
  return (unsigned int) e->value () == i;
}

static QPageSetupDialog_PageSetupDialogOption_Enums oropQPageSetupDialog_PageSetupDialogOption_Enums (const QPageSetupDialog_PageSetupDialogOption_Enums *e, const QPageSetupDialog_PageSetupDialogOption_Enums &d)
{
  return e->flags () | d.flags ();
}

static QPageSetupDialog_PageSetupDialogOption_Enums andopQPageSetupDialog_PageSetupDialogOption_Enums (const QPageSetupDialog_PageSetupDialogOption_Enums *e, const QPageSetupDialog_PageSetupDialogOption_Enums &d)
{
  return e->flags () & d.flags ();
}

static QPageSetupDialog_PageSetupDialogOption_Enums xoropQPageSetupDialog_PageSetupDialogOption_Enums (const QPageSetupDialog_PageSetupDialogOption_Enums *e, const QPageSetupDialog_PageSetupDialogOption_Enums &d)
{
  return e->flags () ^ d.flags ();
}

static std::string to_s_QPageSetupDialog_PageSetupDialogOption_Enums (const QPageSetupDialog_PageSetupDialogOption_Enums *e)
{
  std::string l;
  if (e->testFlag (QPageSetupDialog::None)) { if (!l.empty ()) l += "|"; l += "None"; }
  if (e->testFlag (QPageSetupDialog::DontUseSheet)) { if (!l.empty ()) l += "|"; l += "DontUseSheet"; }
  if (e->testFlag (QPageSetupDialog::OwnsPrinter)) { if (!l.empty ()) l += "|"; l += "OwnsPrinter"; }
  return tl::sprintf ("%u (%s)", (unsigned int) (e->flags ()), l);
}

static int to_i_QPageSetupDialog_PageSetupDialogOption_Enums (const QPageSetupDialog_PageSetupDialogOption_Enums *e)
{
  return int (e->flags ());
}

static bool testFlag_QPageSetupDialog_PageSetupDialogOption_Enums (const QPageSetupDialog_PageSetupDialogOption_Enums *e, const QPageSetupDialog_PageSetupDialogOption_Enum &d)
{
  return e->flags ().testFlag (d.value ());
}

}

namespace gsi
{

//  Declarations for QFlags<enum> wrapper class
gsi::Class<qt_gsi::QPageSetupDialog_PageSetupDialogOption_Enums> decl_QPageSetupDialog_PageSetupDialogOption_Enums ("QPageSetupDialog_QFlags_PageSetupDialogOption",
  gsi::method_ext ("|", &qt_gsi::oropQPageSetupDialog_PageSetupDialogOption_Enums, gsi::arg ("other"), "@brief 'or' operator between two flag sets") +
  gsi::method_ext ("&", &qt_gsi::andopQPageSetupDialog_PageSetupDialogOption_Enums, gsi::arg ("other"), "@brief 'and' operator between two flag sets") +
  gsi::method_ext ("^", &qt_gsi::xoropQPageSetupDialog_PageSetupDialogOption_Enums, gsi::arg ("other"), "@brief 'xor' operator between two flag sets") +
  gsi::method_ext ("to_s|inspect", &qt_gsi::to_s_QPageSetupDialog_PageSetupDialogOption_Enums, "@brief Converts the flag set to a string") +
  gsi::method_ext ("to_i", &qt_gsi::to_i_QPageSetupDialog_PageSetupDialogOption_Enums, "@brief Returns the integer value encoding the flag set") +
  gsi::method_ext ("testFlag", &qt_gsi::testFlag_QPageSetupDialog_PageSetupDialogOption_Enums, gsi::arg ("flag"), "@brief Tests whether the flag is contained in the flag set"),
  "@qt\n@brief This class represents the QFlags<QPageSetupDialog::PageSetupDialogOption> flag set");

//  Declarations for enum wrapper class
gsi::Class<qt_gsi::QPageSetupDialog_PageSetupDialogOption_Enum> decl_QPageSetupDialog_PageSetupDialogOption_Enum (decl_QPageSetupDialog_PageSetupDialogOption_Enums, "QPageSetupDialog_PageSetupDialogOption",
  qt_gsi::mkdefs_QPageSetupDialog_PageSetupDialogOption () +
  gsi::method_ext ("to_s|inspect", &qt_gsi::to_s_QPageSetupDialog_PageSetupDialogOption_Enum, "@brief Converts the enum class to a string") +
  gsi::method_ext ("to_i", &qt_gsi::to_i_QPageSetupDialog_PageSetupDialogOption_Enum, "@brief Converts the enum class to an integer") +
  gsi::method_ext ("==", &qt_gsi::equalQPageSetupDialog_PageSetupDialogOption_Enum, gsi::arg ("other"), "@brief Compares the enum class against an enum of it's own type") +
  gsi::method_ext ("==", &qt_gsi::equal2QPageSetupDialog_PageSetupDialogOption_Enum, gsi::arg ("other"), "@brief Compares the enum class against an integer") +
  gsi::constructor ("new", &qt_gsi::from_i_QPageSetupDialog_PageSetupDialogOption_Enum, gsi::arg ("value"), "@brief Creates the enum object from an integer"),
  "@qt\n@brief This class represents the QPageSetupDialog::PageSetupDialogOption enum");

//  Inject the declarations into the parent
static gsi::ClassExt<QPageSetupDialog> inject_QPageSetupDialog_PageSetupDialogOption_Enum_in_parent (qt_gsi::mkdefs_QPageSetupDialog_PageSetupDialogOption ());
static gsi::ClassExt<QPageSetupDialog> decl_QPageSetupDialog_PageSetupDialogOption_Enum_as_child (decl_QPageSetupDialog_PageSetupDialogOption_Enum, "PageSetupDialogOption");

}
