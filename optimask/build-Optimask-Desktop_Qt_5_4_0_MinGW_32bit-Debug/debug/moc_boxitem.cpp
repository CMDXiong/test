/****************************************************************************
** Meta object code from reading C++ file 'boxitem.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Item/boxitem.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'boxitem.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_BoxItem_t {
    QByteArrayData data[19];
    char stringdata[198];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BoxItem_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BoxItem_t qt_meta_stringdata_BoxItem = {
    {
QT_MOC_LITERAL(0, 0, 7), // "BoxItem"
QT_MOC_LITERAL(1, 8, 5), // "dirty"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 6), // "setPen"
QT_MOC_LITERAL(4, 22, 3), // "pen"
QT_MOC_LITERAL(5, 26, 8), // "setBrush"
QT_MOC_LITERAL(6, 35, 5), // "brush"
QT_MOC_LITERAL(7, 41, 8), // "setAngle"
QT_MOC_LITERAL(8, 50, 5), // "angle"
QT_MOC_LITERAL(9, 56, 18), // "setShearHorizontal"
QT_MOC_LITERAL(10, 75, 15), // "shearHorizontal"
QT_MOC_LITERAL(11, 91, 16), // "setShearVertical"
QT_MOC_LITERAL(12, 108, 13), // "shearVertical"
QT_MOC_LITERAL(13, 122, 8), // "setShear"
QT_MOC_LITERAL(14, 131, 16), // "setShowPenWidget"
QT_MOC_LITERAL(15, 148, 2), // "on"
QT_MOC_LITERAL(16, 151, 18), // "setShowBrushWidget"
QT_MOC_LITERAL(17, 170, 22), // "setshowTransfromWidget"
QT_MOC_LITERAL(18, 193, 4) // "edit"

    },
    "BoxItem\0dirty\0\0setPen\0pen\0setBrush\0"
    "brush\0setAngle\0angle\0setShearHorizontal\0"
    "shearHorizontal\0setShearVertical\0"
    "shearVertical\0setShear\0setShowPenWidget\0"
    "on\0setShowBrushWidget\0setshowTransfromWidget\0"
    "edit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BoxItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       5,  100, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   70,    2, 0x0a /* Public */,
       5,    1,   73,    2, 0x0a /* Public */,
       7,    1,   76,    2, 0x0a /* Public */,
       9,    1,   79,    2, 0x0a /* Public */,
      11,    1,   82,    2, 0x0a /* Public */,
      13,    2,   85,    2, 0x0a /* Public */,
      14,    1,   90,    2, 0x0a /* Public */,
      16,    1,   93,    2, 0x0a /* Public */,
      17,    1,   96,    2, 0x0a /* Public */,
      18,    0,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QPen,    4,
    QMetaType::Void, QMetaType::QBrush,    6,
    QMetaType::Void, QMetaType::Double,    8,
    QMetaType::Void, QMetaType::Double,   10,
    QMetaType::Void, QMetaType::Double,   12,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,   10,   12,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void,

 // properties: name, type, flags
       6, QMetaType::QBrush, 0x00095103,
       4, QMetaType::QPen, 0x00095103,
       8, QMetaType::Double, 0x00095103,
      10, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,

       0        // eod
};

void BoxItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BoxItem *_t = static_cast<BoxItem *>(_o);
        switch (_id) {
        case 0: _t->dirty(); break;
        case 1: _t->setPen((*reinterpret_cast< const QPen(*)>(_a[1]))); break;
        case 2: _t->setBrush((*reinterpret_cast< const QBrush(*)>(_a[1]))); break;
        case 3: _t->setAngle((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->setShearHorizontal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->setShearVertical((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->setShear((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 7: _t->setShowPenWidget((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setShowBrushWidget((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setshowTransfromWidget((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->edit(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (BoxItem::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&BoxItem::dirty)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject BoxItem::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BoxItem.data,
      qt_meta_data_BoxItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *BoxItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BoxItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_BoxItem.stringdata))
        return static_cast<void*>(const_cast< BoxItem*>(this));
    if (!strcmp(_clname, "QGraphicsRectItem"))
        return static_cast< QGraphicsRectItem*>(const_cast< BoxItem*>(this));
    return QObject::qt_metacast(_clname);
}

int BoxItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QBrush*>(_v) = brush(); break;
        case 1: *reinterpret_cast< QPen*>(_v) = pen(); break;
        case 2: *reinterpret_cast< double*>(_v) = angle(); break;
        case 3: *reinterpret_cast< double*>(_v) = shearHorizontal(); break;
        case 4: *reinterpret_cast< double*>(_v) = shearVertical(); break;
        default: break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBrush(*reinterpret_cast< QBrush*>(_v)); break;
        case 1: setPen(*reinterpret_cast< QPen*>(_v)); break;
        case 2: setAngle(*reinterpret_cast< double*>(_v)); break;
        case 3: setShearHorizontal(*reinterpret_cast< double*>(_v)); break;
        case 4: setShearVertical(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void BoxItem::dirty()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
