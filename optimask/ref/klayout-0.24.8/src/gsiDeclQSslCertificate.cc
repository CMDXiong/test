
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
*  @file gsiDeclQSslCertificate.cc 
*
*  DO NOT EDIT THIS FILE. 
*  This file has been created automatically
*/

#include "gsiQtNetwork.h"
#include "gsiDeclQtTypeTraits.h"
#include "gsiDeclQSslCertificate_Enums.h"
#include <memory>

// -----------------------------------------------------------------------
// class QSslCertificate

//  Constructor QSslCertificate::QSslCertificate(QIODevice *device, QSsl::EncodingFormat format)


static void _init_ctor_QSslCertificate_3702 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("device");
  decl->add_arg<QIODevice * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("format", true, "QSsl::Pem");
  decl->add_arg<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > (argspec_1);
  decl->set_return_new<QSslCertificate *> ();
}

static void _call_ctor_QSslCertificate_3702 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  QIODevice *arg1 = args.read<QIODevice * > ();
  tl::Heap heap;
  const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & arg2 = args ? args.read<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > () : (const qt_gsi::Converter<QSsl::EncodingFormat>::target_type &)(qt_gsi::CppToQtReadAdaptor<QSsl::EncodingFormat>(heap, QSsl::Pem));
  ret.write<QSslCertificate *> (new QSslCertificate (arg1, qt_gsi::QtToCppAdaptor<QSsl::EncodingFormat>(arg2).cref()));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QSslCertificate_3702 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QSslCertificate::QSslCertificate(QIODevice *device, QSsl::EncodingFormat format)\nThis method creates an object of class QSslCertificate.", &_init_ctor_QSslCertificate_3702, &_call_ctor_QSslCertificate_3702);
}

//  Constructor QSslCertificate::QSslCertificate(const QByteArray &encoded, QSsl::EncodingFormat format)


static void _init_ctor_QSslCertificate_4564 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("encoded", true, "QByteArray()");
  decl->add_arg<const QByteArray & > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("format", true, "QSsl::Pem");
  decl->add_arg<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > (argspec_1);
  decl->set_return_new<QSslCertificate *> ();
}

static void _call_ctor_QSslCertificate_4564 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QByteArray &arg1 = args ? args.read<const QByteArray & > () : (const QByteArray &)(QByteArray());
  tl::Heap heap;
  const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & arg2 = args ? args.read<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > () : (const qt_gsi::Converter<QSsl::EncodingFormat>::target_type &)(qt_gsi::CppToQtReadAdaptor<QSsl::EncodingFormat>(heap, QSsl::Pem));
  ret.write<QSslCertificate *> (new QSslCertificate (arg1, qt_gsi::QtToCppAdaptor<QSsl::EncodingFormat>(arg2).cref()));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QSslCertificate_4564 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QSslCertificate::QSslCertificate(const QByteArray &encoded, QSsl::EncodingFormat format)\nThis method creates an object of class QSslCertificate.", &_init_ctor_QSslCertificate_4564, &_call_ctor_QSslCertificate_4564);
}

//  Constructor QSslCertificate::QSslCertificate(const QSslCertificate &other)


static void _init_ctor_QSslCertificate_2823 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("other");
  decl->add_arg<const QSslCertificate & > (argspec_0);
  decl->set_return_new<QSslCertificate *> ();
}

static void _call_ctor_QSslCertificate_2823 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QSslCertificate &arg1 = args.read<const QSslCertificate & > ();
  ret.write<QSslCertificate *> (new QSslCertificate (arg1));
}

static qt_gsi::GenericStaticMethod *_create_ctor_QSslCertificate_2823 ()
{
  return new qt_gsi::GenericStaticMethod ("new", "@brief Constructor QSslCertificate::QSslCertificate(const QSslCertificate &other)\nThis method creates an object of class QSslCertificate.", &_init_ctor_QSslCertificate_2823, &_call_ctor_QSslCertificate_2823);
}

// void QSslCertificate::clear()


static void _init_f_clear_0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<void > ();
}

static void _call_f_clear_0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ((QSslCertificate *)cls)->clear ();
}

static qt_gsi::GenericMethod *_create_f_clear_0 ()
{
  return new qt_gsi::GenericMethod ("clear", "@brief Method void QSslCertificate::clear()\n", false, &_init_f_clear_0, &_call_f_clear_0);
}

// QByteArray QSslCertificate::digest(QCryptographicHash::Algorithm algorithm)


static void _init_f_digest_c3331 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("algorithm", true, "QCryptographicHash::Md5");
  decl->add_arg<const qt_gsi::Converter<QCryptographicHash::Algorithm>::target_type & > (argspec_0);
  decl->set_return<QByteArray > ();
}

static void _call_f_digest_c3331 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  tl::Heap heap;
  const qt_gsi::Converter<QCryptographicHash::Algorithm>::target_type & arg1 = args ? args.read<const qt_gsi::Converter<QCryptographicHash::Algorithm>::target_type & > () : (const qt_gsi::Converter<QCryptographicHash::Algorithm>::target_type &)(qt_gsi::CppToQtReadAdaptor<QCryptographicHash::Algorithm>(heap, QCryptographicHash::Md5));
  ret.write<QByteArray > ((QByteArray)((QSslCertificate *)cls)->digest (qt_gsi::QtToCppAdaptor<QCryptographicHash::Algorithm>(arg1).cref()));
}

static qt_gsi::GenericMethod *_create_f_digest_c3331 ()
{
  return new qt_gsi::GenericMethod ("digest", "@brief Method QByteArray QSslCertificate::digest(QCryptographicHash::Algorithm algorithm)\n", true, &_init_f_digest_c3331, &_call_f_digest_c3331);
}

// QDateTime QSslCertificate::effectiveDate()


static void _init_f_effectiveDate_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QDateTime > ();
}

static void _call_f_effectiveDate_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QDateTime > ((QDateTime)((QSslCertificate *)cls)->effectiveDate ());
}

static qt_gsi::GenericMethod *_create_f_effectiveDate_c0 ()
{
  return new qt_gsi::GenericMethod ("effectiveDate", "@brief Method QDateTime QSslCertificate::effectiveDate()\n", true, &_init_f_effectiveDate_c0, &_call_f_effectiveDate_c0);
}

// QDateTime QSslCertificate::expiryDate()


static void _init_f_expiryDate_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QDateTime > ();
}

static void _call_f_expiryDate_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QDateTime > ((QDateTime)((QSslCertificate *)cls)->expiryDate ());
}

static qt_gsi::GenericMethod *_create_f_expiryDate_c0 ()
{
  return new qt_gsi::GenericMethod ("expiryDate", "@brief Method QDateTime QSslCertificate::expiryDate()\n", true, &_init_f_expiryDate_c0, &_call_f_expiryDate_c0);
}

// Qt::HANDLE QSslCertificate::handle()


static void _init_f_handle_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<Qt::HANDLE > ();
}

static void _call_f_handle_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<Qt::HANDLE > ((Qt::HANDLE)((QSslCertificate *)cls)->handle ());
}

static qt_gsi::GenericMethod *_create_f_handle_c0 ()
{
  return new qt_gsi::GenericMethod ("handle", "@brief Method Qt::HANDLE QSslCertificate::handle()\n", true, &_init_f_handle_c0, &_call_f_handle_c0);
}

// bool QSslCertificate::isNull()


static void _init_f_isNull_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<bool > ();
}

static void _call_f_isNull_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<bool > ((bool)((QSslCertificate *)cls)->isNull ());
}

static qt_gsi::GenericMethod *_create_f_isNull_c0 ()
{
  return new qt_gsi::GenericMethod ("isNull?", "@brief Method bool QSslCertificate::isNull()\n", true, &_init_f_isNull_c0, &_call_f_isNull_c0);
}

// bool QSslCertificate::isValid()


static void _init_f_isValid_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<bool > ();
}

static void _call_f_isValid_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<bool > ((bool)((QSslCertificate *)cls)->isValid ());
}

static qt_gsi::GenericMethod *_create_f_isValid_c0 ()
{
  return new qt_gsi::GenericMethod ("isValid?", "@brief Method bool QSslCertificate::isValid()\n", true, &_init_f_isValid_c0, &_call_f_isValid_c0);
}

// QString QSslCertificate::issuerInfo(QSslCertificate::SubjectInfo info)


static void _init_f_issuerInfo_c3178 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("info");
  decl->add_arg<const qt_gsi::Converter<QSslCertificate::SubjectInfo>::target_type & > (argspec_0);
  decl->set_return<QString > ();
}

static void _call_f_issuerInfo_c3178 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QSslCertificate::SubjectInfo>::target_type & arg1 = args.read<const qt_gsi::Converter<QSslCertificate::SubjectInfo>::target_type & > ();
  ret.write<QString > ((QString)((QSslCertificate *)cls)->issuerInfo (qt_gsi::QtToCppAdaptor<QSslCertificate::SubjectInfo>(arg1).cref()));
}

static qt_gsi::GenericMethod *_create_f_issuerInfo_c3178 ()
{
  return new qt_gsi::GenericMethod ("issuerInfo", "@brief Method QString QSslCertificate::issuerInfo(QSslCertificate::SubjectInfo info)\n", true, &_init_f_issuerInfo_c3178, &_call_f_issuerInfo_c3178);
}

// QString QSslCertificate::issuerInfo(const QByteArray &tag)


static void _init_f_issuerInfo_c2309 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("tag");
  decl->add_arg<const QByteArray & > (argspec_0);
  decl->set_return<QString > ();
}

static void _call_f_issuerInfo_c2309 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QByteArray &arg1 = args.read<const QByteArray & > ();
  ret.write<QString > ((QString)((QSslCertificate *)cls)->issuerInfo (arg1));
}

static qt_gsi::GenericMethod *_create_f_issuerInfo_c2309 ()
{
  return new qt_gsi::GenericMethod ("issuerInfo", "@brief Method QString QSslCertificate::issuerInfo(const QByteArray &tag)\n", true, &_init_f_issuerInfo_c2309, &_call_f_issuerInfo_c2309);
}

// bool QSslCertificate::operator!=(const QSslCertificate &other)


static void _init_f_operator_excl__eq__c2823 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("other");
  decl->add_arg<const QSslCertificate & > (argspec_0);
  decl->set_return<bool > ();
}

static void _call_f_operator_excl__eq__c2823 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QSslCertificate &arg1 = args.read<const QSslCertificate & > ();
  ret.write<bool > ((bool)((QSslCertificate *)cls)->operator!= (arg1));
}

static qt_gsi::GenericMethod *_create_f_operator_excl__eq__c2823 ()
{
  return new qt_gsi::GenericMethod ("!=", "@brief Method bool QSslCertificate::operator!=(const QSslCertificate &other)\n", true, &_init_f_operator_excl__eq__c2823, &_call_f_operator_excl__eq__c2823);
}

// QSslCertificate &QSslCertificate::operator=(const QSslCertificate &other)


static void _init_f_operator_eq__2823 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("other");
  decl->add_arg<const QSslCertificate & > (argspec_0);
  decl->set_return<QSslCertificate & > ();
}

static void _call_f_operator_eq__2823 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QSslCertificate &arg1 = args.read<const QSslCertificate & > ();
  ret.write<QSslCertificate & > ((QSslCertificate &)((QSslCertificate *)cls)->operator= (arg1));
}

static qt_gsi::GenericMethod *_create_f_operator_eq__2823 ()
{
  return new qt_gsi::GenericMethod ("assign", "@brief Method QSslCertificate &QSslCertificate::operator=(const QSslCertificate &other)\n", false, &_init_f_operator_eq__2823, &_call_f_operator_eq__2823);
}

// bool QSslCertificate::operator==(const QSslCertificate &other)


static void _init_f_operator_eq__eq__c2823 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("other");
  decl->add_arg<const QSslCertificate & > (argspec_0);
  decl->set_return<bool > ();
}

static void _call_f_operator_eq__eq__c2823 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QSslCertificate &arg1 = args.read<const QSslCertificate & > ();
  ret.write<bool > ((bool)((QSslCertificate *)cls)->operator== (arg1));
}

static qt_gsi::GenericMethod *_create_f_operator_eq__eq__c2823 ()
{
  return new qt_gsi::GenericMethod ("==", "@brief Method bool QSslCertificate::operator==(const QSslCertificate &other)\n", true, &_init_f_operator_eq__eq__c2823, &_call_f_operator_eq__eq__c2823);
}

// QSslKey QSslCertificate::publicKey()


static void _init_f_publicKey_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QSslKey > ();
}

static void _call_f_publicKey_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QSslKey > ((QSslKey)((QSslCertificate *)cls)->publicKey ());
}

static qt_gsi::GenericMethod *_create_f_publicKey_c0 ()
{
  return new qt_gsi::GenericMethod ("publicKey", "@brief Method QSslKey QSslCertificate::publicKey()\n", true, &_init_f_publicKey_c0, &_call_f_publicKey_c0);
}

// QByteArray QSslCertificate::serialNumber()


static void _init_f_serialNumber_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QByteArray > ();
}

static void _call_f_serialNumber_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QByteArray > ((QByteArray)((QSslCertificate *)cls)->serialNumber ());
}

static qt_gsi::GenericMethod *_create_f_serialNumber_c0 ()
{
  return new qt_gsi::GenericMethod ("serialNumber", "@brief Method QByteArray QSslCertificate::serialNumber()\n", true, &_init_f_serialNumber_c0, &_call_f_serialNumber_c0);
}

// QString QSslCertificate::subjectInfo(QSslCertificate::SubjectInfo info)


static void _init_f_subjectInfo_c3178 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("info");
  decl->add_arg<const qt_gsi::Converter<QSslCertificate::SubjectInfo>::target_type & > (argspec_0);
  decl->set_return<QString > ();
}

static void _call_f_subjectInfo_c3178 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const qt_gsi::Converter<QSslCertificate::SubjectInfo>::target_type & arg1 = args.read<const qt_gsi::Converter<QSslCertificate::SubjectInfo>::target_type & > ();
  ret.write<QString > ((QString)((QSslCertificate *)cls)->subjectInfo (qt_gsi::QtToCppAdaptor<QSslCertificate::SubjectInfo>(arg1).cref()));
}

static qt_gsi::GenericMethod *_create_f_subjectInfo_c3178 ()
{
  return new qt_gsi::GenericMethod ("subjectInfo", "@brief Method QString QSslCertificate::subjectInfo(QSslCertificate::SubjectInfo info)\n", true, &_init_f_subjectInfo_c3178, &_call_f_subjectInfo_c3178);
}

// QString QSslCertificate::subjectInfo(const QByteArray &tag)


static void _init_f_subjectInfo_c2309 (qt_gsi::GenericMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("tag");
  decl->add_arg<const QByteArray & > (argspec_0);
  decl->set_return<QString > ();
}

static void _call_f_subjectInfo_c2309 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QByteArray &arg1 = args.read<const QByteArray & > ();
  ret.write<QString > ((QString)((QSslCertificate *)cls)->subjectInfo (arg1));
}

static qt_gsi::GenericMethod *_create_f_subjectInfo_c2309 ()
{
  return new qt_gsi::GenericMethod ("subjectInfo", "@brief Method QString QSslCertificate::subjectInfo(const QByteArray &tag)\n", true, &_init_f_subjectInfo_c2309, &_call_f_subjectInfo_c2309);
}

// QByteArray QSslCertificate::toDer()


static void _init_f_toDer_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QByteArray > ();
}

static void _call_f_toDer_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QByteArray > ((QByteArray)((QSslCertificate *)cls)->toDer ());
}

static qt_gsi::GenericMethod *_create_f_toDer_c0 ()
{
  return new qt_gsi::GenericMethod ("toDer", "@brief Method QByteArray QSslCertificate::toDer()\n", true, &_init_f_toDer_c0, &_call_f_toDer_c0);
}

// QByteArray QSslCertificate::toPem()


static void _init_f_toPem_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QByteArray > ();
}

static void _call_f_toPem_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QByteArray > ((QByteArray)((QSslCertificate *)cls)->toPem ());
}

static qt_gsi::GenericMethod *_create_f_toPem_c0 ()
{
  return new qt_gsi::GenericMethod ("toPem", "@brief Method QByteArray QSslCertificate::toPem()\n", true, &_init_f_toPem_c0, &_call_f_toPem_c0);
}

// QByteArray QSslCertificate::version()


static void _init_f_version_c0 (qt_gsi::GenericMethod *decl)
{
  decl->set_return<QByteArray > ();
}

static void _call_f_version_c0 (const qt_gsi::GenericMethod *decl, void *cls, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  ret.write<QByteArray > ((QByteArray)((QSslCertificate *)cls)->version ());
}

static qt_gsi::GenericMethod *_create_f_version_c0 ()
{
  return new qt_gsi::GenericMethod ("version", "@brief Method QByteArray QSslCertificate::version()\n", true, &_init_f_version_c0, &_call_f_version_c0);
}

// static QList<QSslCertificate> QSslCertificate::fromData(const QByteArray &data, QSsl::EncodingFormat format)


static void _init_f_fromData_4564 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("data");
  decl->add_arg<const QByteArray & > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("format", true, "QSsl::Pem");
  decl->add_arg<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > (argspec_1);
  decl->set_return<qt_gsi::Converter<QList<QSslCertificate> >::target_type > ();
}

static void _call_f_fromData_4564 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QByteArray &arg1 = args.read<const QByteArray & > ();
  tl::Heap heap;
  const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & arg2 = args ? args.read<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > () : (const qt_gsi::Converter<QSsl::EncodingFormat>::target_type &)(qt_gsi::CppToQtReadAdaptor<QSsl::EncodingFormat>(heap, QSsl::Pem));
  ret.write<qt_gsi::Converter<QList<QSslCertificate> >::target_type > ((qt_gsi::Converter<QList<QSslCertificate> >::target_type)qt_gsi::CppToQtAdaptor<QList<QSslCertificate> >(QSslCertificate::fromData (arg1, qt_gsi::QtToCppAdaptor<QSsl::EncodingFormat>(arg2).cref())));
}

static qt_gsi::GenericStaticMethod *_create_f_fromData_4564 ()
{
  return new qt_gsi::GenericStaticMethod ("fromData", "@brief Static method QList<QSslCertificate> QSslCertificate::fromData(const QByteArray &data, QSsl::EncodingFormat format)\nThis method is static and can be called without an instance.", &_init_f_fromData_4564, &_call_f_fromData_4564);
}

// static QList<QSslCertificate> QSslCertificate::fromDevice(QIODevice *device, QSsl::EncodingFormat format)


static void _init_f_fromDevice_3702 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("device");
  decl->add_arg<QIODevice * > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("format", true, "QSsl::Pem");
  decl->add_arg<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > (argspec_1);
  decl->set_return<qt_gsi::Converter<QList<QSslCertificate> >::target_type > ();
}

static void _call_f_fromDevice_3702 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  QIODevice *arg1 = args.read<QIODevice * > ();
  tl::Heap heap;
  const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & arg2 = args ? args.read<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > () : (const qt_gsi::Converter<QSsl::EncodingFormat>::target_type &)(qt_gsi::CppToQtReadAdaptor<QSsl::EncodingFormat>(heap, QSsl::Pem));
  ret.write<qt_gsi::Converter<QList<QSslCertificate> >::target_type > ((qt_gsi::Converter<QList<QSslCertificate> >::target_type)qt_gsi::CppToQtAdaptor<QList<QSslCertificate> >(QSslCertificate::fromDevice (arg1, qt_gsi::QtToCppAdaptor<QSsl::EncodingFormat>(arg2).cref())));
}

static qt_gsi::GenericStaticMethod *_create_f_fromDevice_3702 ()
{
  return new qt_gsi::GenericStaticMethod ("fromDevice", "@brief Static method QList<QSslCertificate> QSslCertificate::fromDevice(QIODevice *device, QSsl::EncodingFormat format)\nThis method is static and can be called without an instance.", &_init_f_fromDevice_3702, &_call_f_fromDevice_3702);
}

// static QList<QSslCertificate> QSslCertificate::fromPath(const QString &path, QSsl::EncodingFormat format, QRegExp::PatternSyntax syntax)


static void _init_f_fromPath_6773 (qt_gsi::GenericStaticMethod *decl)
{
  static gsi::ArgSpecBase argspec_0 ("path");
  decl->add_arg<const QString & > (argspec_0);
  static gsi::ArgSpecBase argspec_1 ("format", true, "QSsl::Pem");
  decl->add_arg<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > (argspec_1);
  static gsi::ArgSpecBase argspec_2 ("syntax", true, "QRegExp::FixedString");
  decl->add_arg<const qt_gsi::Converter<QRegExp::PatternSyntax>::target_type & > (argspec_2);
  decl->set_return<qt_gsi::Converter<QList<QSslCertificate> >::target_type > ();
}

static void _call_f_fromPath_6773 (const qt_gsi::GenericStaticMethod *decl, gsi::SerialArgs &args, gsi::SerialArgs &ret) 
{
  const QString &arg1 = args.read<const QString & > ();
  tl::Heap heap;
  const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & arg2 = args ? args.read<const qt_gsi::Converter<QSsl::EncodingFormat>::target_type & > () : (const qt_gsi::Converter<QSsl::EncodingFormat>::target_type &)(qt_gsi::CppToQtReadAdaptor<QSsl::EncodingFormat>(heap, QSsl::Pem));
  const qt_gsi::Converter<QRegExp::PatternSyntax>::target_type & arg3 = args ? args.read<const qt_gsi::Converter<QRegExp::PatternSyntax>::target_type & > () : (const qt_gsi::Converter<QRegExp::PatternSyntax>::target_type &)(qt_gsi::CppToQtReadAdaptor<QRegExp::PatternSyntax>(heap, QRegExp::FixedString));
  ret.write<qt_gsi::Converter<QList<QSslCertificate> >::target_type > ((qt_gsi::Converter<QList<QSslCertificate> >::target_type)qt_gsi::CppToQtAdaptor<QList<QSslCertificate> >(QSslCertificate::fromPath (arg1, qt_gsi::QtToCppAdaptor<QSsl::EncodingFormat>(arg2).cref(), qt_gsi::QtToCppAdaptor<QRegExp::PatternSyntax>(arg3).cref())));
}

static qt_gsi::GenericStaticMethod *_create_f_fromPath_6773 ()
{
  return new qt_gsi::GenericStaticMethod ("fromPath", "@brief Static method QList<QSslCertificate> QSslCertificate::fromPath(const QString &path, QSsl::EncodingFormat format, QRegExp::PatternSyntax syntax)\nThis method is static and can be called without an instance.", &_init_f_fromPath_6773, &_call_f_fromPath_6773);
}


namespace gsi
{
gsi::Class<QSslCertificate> decl_QSslCertificate ("QSslCertificate",
  gsi::Methods(_create_ctor_QSslCertificate_3702 ()) +
  gsi::Methods(_create_ctor_QSslCertificate_4564 ()) +
  gsi::Methods(_create_ctor_QSslCertificate_2823 ()) +
  gsi::Methods(_create_f_clear_0 ()) +
  gsi::Methods(_create_f_digest_c3331 ()) +
  gsi::Methods(_create_f_effectiveDate_c0 ()) +
  gsi::Methods(_create_f_expiryDate_c0 ()) +
  gsi::Methods(_create_f_handle_c0 ()) +
  gsi::Methods(_create_f_isNull_c0 ()) +
  gsi::Methods(_create_f_isValid_c0 ()) +
  gsi::Methods(_create_f_issuerInfo_c3178 ()) +
  gsi::Methods(_create_f_issuerInfo_c2309 ()) +
  gsi::Methods(_create_f_operator_excl__eq__c2823 ()) +
  gsi::Methods(_create_f_operator_eq__2823 ()) +
  gsi::Methods(_create_f_operator_eq__eq__c2823 ()) +
  gsi::Methods(_create_f_publicKey_c0 ()) +
  gsi::Methods(_create_f_serialNumber_c0 ()) +
  gsi::Methods(_create_f_subjectInfo_c3178 ()) +
  gsi::Methods(_create_f_subjectInfo_c2309 ()) +
  gsi::Methods(_create_f_toDer_c0 ()) +
  gsi::Methods(_create_f_toPem_c0 ()) +
  gsi::Methods(_create_f_version_c0 ()) +
  gsi::Methods(_create_f_fromData_4564 ()) +
  gsi::Methods(_create_f_fromDevice_3702 ()) +
  gsi::Methods(_create_f_fromPath_6773 ()),
  "@qt\n@brief Binding of QSslCertificate");

}


//  Implementation of the enum wrapper class for QSslCertificate::SubjectInfo
namespace qt_gsi
{

static QSslCertificate_SubjectInfo_Enum *c_1_QSslCertificate_SubjectInfo_Enum () { return new QSslCertificate_SubjectInfo_Enum (QSslCertificate::Organization); }
static QSslCertificate_SubjectInfo_Enum *c_2_QSslCertificate_SubjectInfo_Enum () { return new QSslCertificate_SubjectInfo_Enum (QSslCertificate::CommonName); }
static QSslCertificate_SubjectInfo_Enum *c_3_QSslCertificate_SubjectInfo_Enum () { return new QSslCertificate_SubjectInfo_Enum (QSslCertificate::LocalityName); }
static QSslCertificate_SubjectInfo_Enum *c_4_QSslCertificate_SubjectInfo_Enum () { return new QSslCertificate_SubjectInfo_Enum (QSslCertificate::OrganizationalUnitName); }
static QSslCertificate_SubjectInfo_Enum *c_5_QSslCertificate_SubjectInfo_Enum () { return new QSslCertificate_SubjectInfo_Enum (QSslCertificate::CountryName); }
static QSslCertificate_SubjectInfo_Enum *c_6_QSslCertificate_SubjectInfo_Enum () { return new QSslCertificate_SubjectInfo_Enum (QSslCertificate::StateOrProvinceName); }

static gsi::Methods mkdefs_QSslCertificate_SubjectInfo () {
  return
    gsi::constant ("Organization", &qt_gsi::c_1_QSslCertificate_SubjectInfo_Enum, "@brief Enum constant QSslCertificate::Organization") +
    gsi::constant ("CommonName", &qt_gsi::c_2_QSslCertificate_SubjectInfo_Enum, "@brief Enum constant QSslCertificate::CommonName") +
    gsi::constant ("LocalityName", &qt_gsi::c_3_QSslCertificate_SubjectInfo_Enum, "@brief Enum constant QSslCertificate::LocalityName") +
    gsi::constant ("OrganizationalUnitName", &qt_gsi::c_4_QSslCertificate_SubjectInfo_Enum, "@brief Enum constant QSslCertificate::OrganizationalUnitName") +
    gsi::constant ("CountryName", &qt_gsi::c_5_QSslCertificate_SubjectInfo_Enum, "@brief Enum constant QSslCertificate::CountryName") +
    gsi::constant ("StateOrProvinceName", &qt_gsi::c_6_QSslCertificate_SubjectInfo_Enum, "@brief Enum constant QSslCertificate::StateOrProvinceName");
}

static std::string to_s_QSslCertificate_SubjectInfo_Enum (const QSslCertificate_SubjectInfo_Enum *e)
{
  if (e->value () == QSslCertificate::Organization) { return tl::sprintf ("Organization (%u)", (unsigned int) e->value ()); }
  if (e->value () == QSslCertificate::CommonName) { return tl::sprintf ("CommonName (%u)", (unsigned int) e->value ()); }
  if (e->value () == QSslCertificate::LocalityName) { return tl::sprintf ("LocalityName (%u)", (unsigned int) e->value ()); }
  if (e->value () == QSslCertificate::OrganizationalUnitName) { return tl::sprintf ("OrganizationalUnitName (%u)", (unsigned int) e->value ()); }
  if (e->value () == QSslCertificate::CountryName) { return tl::sprintf ("CountryName (%u)", (unsigned int) e->value ()); }
  if (e->value () == QSslCertificate::StateOrProvinceName) { return tl::sprintf ("StateOrProvinceName (%u)", (unsigned int) e->value ()); }
  return std::string ("(not a valid enum evalue)");
}

static unsigned int to_i_QSslCertificate_SubjectInfo_Enum (const QSslCertificate_SubjectInfo_Enum *e)
{
  return (unsigned int) e->value ();
}

static QSslCertificate_SubjectInfo_Enum *from_i_QSslCertificate_SubjectInfo_Enum (unsigned int i)
{
  return new QSslCertificate_SubjectInfo_Enum ((QSslCertificate::SubjectInfo) i);
}

static bool equalQSslCertificate_SubjectInfo_Enum (const QSslCertificate_SubjectInfo_Enum *e, QSslCertificate_SubjectInfo_Enum d)
{
  return e->value () == d.value ();
}

static bool equal2QSslCertificate_SubjectInfo_Enum (const QSslCertificate_SubjectInfo_Enum *e, unsigned int i)
{
  return (unsigned int) e->value () == i;
}

static QSslCertificate_SubjectInfo_Enums oropQSslCertificate_SubjectInfo_Enums (const QSslCertificate_SubjectInfo_Enums *e, const QSslCertificate_SubjectInfo_Enums &d)
{
  return e->flags () | d.flags ();
}

static QSslCertificate_SubjectInfo_Enums andopQSslCertificate_SubjectInfo_Enums (const QSslCertificate_SubjectInfo_Enums *e, const QSslCertificate_SubjectInfo_Enums &d)
{
  return e->flags () & d.flags ();
}

static QSslCertificate_SubjectInfo_Enums xoropQSslCertificate_SubjectInfo_Enums (const QSslCertificate_SubjectInfo_Enums *e, const QSslCertificate_SubjectInfo_Enums &d)
{
  return e->flags () ^ d.flags ();
}

static std::string to_s_QSslCertificate_SubjectInfo_Enums (const QSslCertificate_SubjectInfo_Enums *e)
{
  std::string l;
  if (e->testFlag (QSslCertificate::Organization)) { if (!l.empty ()) l += "|"; l += "Organization"; }
  if (e->testFlag (QSslCertificate::CommonName)) { if (!l.empty ()) l += "|"; l += "CommonName"; }
  if (e->testFlag (QSslCertificate::LocalityName)) { if (!l.empty ()) l += "|"; l += "LocalityName"; }
  if (e->testFlag (QSslCertificate::OrganizationalUnitName)) { if (!l.empty ()) l += "|"; l += "OrganizationalUnitName"; }
  if (e->testFlag (QSslCertificate::CountryName)) { if (!l.empty ()) l += "|"; l += "CountryName"; }
  if (e->testFlag (QSslCertificate::StateOrProvinceName)) { if (!l.empty ()) l += "|"; l += "StateOrProvinceName"; }
  return tl::sprintf ("%u (%s)", (unsigned int) (e->flags ()), l);
}

static int to_i_QSslCertificate_SubjectInfo_Enums (const QSslCertificate_SubjectInfo_Enums *e)
{
  return int (e->flags ());
}

static bool testFlag_QSslCertificate_SubjectInfo_Enums (const QSslCertificate_SubjectInfo_Enums *e, const QSslCertificate_SubjectInfo_Enum &d)
{
  return e->flags ().testFlag (d.value ());
}

}

namespace gsi
{

//  Declarations for QFlags<enum> wrapper class
gsi::Class<qt_gsi::QSslCertificate_SubjectInfo_Enums> decl_QSslCertificate_SubjectInfo_Enums ("QSslCertificate_QFlags_SubjectInfo",
  gsi::method_ext ("|", &qt_gsi::oropQSslCertificate_SubjectInfo_Enums, gsi::arg ("other"), "@brief 'or' operator between two flag sets") +
  gsi::method_ext ("&", &qt_gsi::andopQSslCertificate_SubjectInfo_Enums, gsi::arg ("other"), "@brief 'and' operator between two flag sets") +
  gsi::method_ext ("^", &qt_gsi::xoropQSslCertificate_SubjectInfo_Enums, gsi::arg ("other"), "@brief 'xor' operator between two flag sets") +
  gsi::method_ext ("to_s|inspect", &qt_gsi::to_s_QSslCertificate_SubjectInfo_Enums, "@brief Converts the flag set to a string") +
  gsi::method_ext ("to_i", &qt_gsi::to_i_QSslCertificate_SubjectInfo_Enums, "@brief Returns the integer value encoding the flag set") +
  gsi::method_ext ("testFlag", &qt_gsi::testFlag_QSslCertificate_SubjectInfo_Enums, gsi::arg ("flag"), "@brief Tests whether the flag is contained in the flag set"),
  "@qt\n@brief This class represents the QFlags<QSslCertificate::SubjectInfo> flag set");

//  Declarations for enum wrapper class
gsi::Class<qt_gsi::QSslCertificate_SubjectInfo_Enum> decl_QSslCertificate_SubjectInfo_Enum (decl_QSslCertificate_SubjectInfo_Enums, "QSslCertificate_SubjectInfo",
  qt_gsi::mkdefs_QSslCertificate_SubjectInfo () +
  gsi::method_ext ("to_s|inspect", &qt_gsi::to_s_QSslCertificate_SubjectInfo_Enum, "@brief Converts the enum class to a string") +
  gsi::method_ext ("to_i", &qt_gsi::to_i_QSslCertificate_SubjectInfo_Enum, "@brief Converts the enum class to an integer") +
  gsi::method_ext ("==", &qt_gsi::equalQSslCertificate_SubjectInfo_Enum, gsi::arg ("other"), "@brief Compares the enum class against an enum of it's own type") +
  gsi::method_ext ("==", &qt_gsi::equal2QSslCertificate_SubjectInfo_Enum, gsi::arg ("other"), "@brief Compares the enum class against an integer") +
  gsi::constructor ("new", &qt_gsi::from_i_QSslCertificate_SubjectInfo_Enum, gsi::arg ("value"), "@brief Creates the enum object from an integer"),
  "@qt\n@brief This class represents the QSslCertificate::SubjectInfo enum");

//  Inject the declarations into the parent
static gsi::ClassExt<QSslCertificate> inject_QSslCertificate_SubjectInfo_Enum_in_parent (qt_gsi::mkdefs_QSslCertificate_SubjectInfo ());
static gsi::ClassExt<QSslCertificate> decl_QSslCertificate_SubjectInfo_Enum_as_child (decl_QSslCertificate_SubjectInfo_Enum, "SubjectInfo");

}
