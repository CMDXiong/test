/****************************************************************************
** Meta object code from reading C++ file 'textitem.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Item/textitem.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textitem.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TextItem_t {
    QByteArrayData data[11];
    char stringdata[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TextItem_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TextItem_t qt_meta_stringdata_TextItem = {
    {
QT_MOC_LITERAL(0, 0, 8), // "TextItem"
QT_MOC_LITERAL(1, 9, 5), // "dirty"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 8), // "setAngle"
QT_MOC_LITERAL(4, 25, 5), // "angle"
QT_MOC_LITERAL(5, 31, 18), // "setShearHorizontal"
QT_MOC_LITERAL(6, 50, 15), // "shearHorizontal"
QT_MOC_LITERAL(7, 66, 16), // "setShearVertical"
QT_MOC_LITERAL(8, 83, 13), // "shearVertical"
QT_MOC_LITERAL(9, 97, 8), // "setShear"
QT_MOC_LITERAL(10, 106, 4) // "edit"

    },
    "TextItem\0dirty\0\0setAngle\0angle\0"
    "setShearHorizontal\0shearHorizontal\0"
    "setShearVertical\0shearVertical\0setShear\0"
    "edit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TextItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       3,   60, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   45,    2, 0x0a /* Public */,
       5,    1,   48,    2, 0x0a /* Public */,
       7,    1,   51,    2, 0x0a /* Public */,
       9,    2,   54,    2, 0x0a /* Public */,
      10,    0,   59,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void, QMetaType::Double,    6,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    6,    8,
    QMetaType::Void,

 // properties: name, type, flags
       4, QMetaType::Double, 0x00095103,
       6, QMetaType::Double, 0x00095103,
       8, QMetaType::Double, 0x00095103,

       0        // eod
};

void TextItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextItem *_t = static_cast<TextItem *>(_o);
        switch (_id) {
        case 0: _t->dirty(); break;
        case 1: _t->setAngle((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->setShearHorizontal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->setShearVertical((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->setShear((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: _t->edit(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TextItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextItem::dirty)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject TextItem::staticMetaObject = {
    { &QGraphicsTextItem::staticMetaObject, qt_meta_stringdata_TextItem.data,
      qt_meta_data_TextItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TextItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TextItem.stringdata))
        return static_cast<void*>(const_cast< TextItem*>(this));
    return QGraphicsTextItem::qt_metacast(_clname);
}

int TextItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsTextItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = angle(); break;
        case 1: *reinterpret_cast< double*>(_v) = shearHorizontal(); break;
        case 2: *reinterpret_cast< double*>(_v) = shearVertical(); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAngle(*reinterpret_cast< double*>(_v)); break;
        case 1: setShearHorizontal(*reinterpret_cast< double*>(_v)); break;
        case 2: setShearVertical(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void TextItem::dirty()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
