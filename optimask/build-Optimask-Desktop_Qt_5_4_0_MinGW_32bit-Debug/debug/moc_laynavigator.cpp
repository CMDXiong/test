/****************************************************************************
** Meta object code from reading C++ file 'laynavigator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Layer/laynavigator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'laynavigator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Navigator_t {
    QByteArrayData data[8];
    char stringdata[77];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Navigator_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Navigator_t qt_meta_stringdata_Navigator = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Navigator"
QT_MOC_LITERAL(1, 10, 14), // "viewBoxchanged"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 11), // "scaleFactor"
QT_MOC_LITERAL(4, 38, 9), // "tranSlate"
QT_MOC_LITERAL(5, 48, 5), // "point"
QT_MOC_LITERAL(6, 54, 11), // "viewBoxZoom"
QT_MOC_LITERAL(7, 66, 10) // "setBoxzoom"

    },
    "Navigator\0viewBoxchanged\0\0scaleFactor\0"
    "tranSlate\0point\0viewBoxZoom\0setBoxzoom"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Navigator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   40,    2, 0x0a /* Public */,
       7,    1,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::QPointF,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    3,

       0        // eod
};

void Navigator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Navigator *_t = static_cast<Navigator *>(_o);
        switch (_id) {
        case 0: _t->viewBoxchanged((*reinterpret_cast< const float(*)>(_a[1]))); break;
        case 1: _t->tranSlate((*reinterpret_cast< const QPointF(*)>(_a[1]))); break;
        case 2: _t->viewBoxZoom((*reinterpret_cast< const float(*)>(_a[1]))); break;
        case 3: _t->setBoxzoom((*reinterpret_cast< const float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Navigator::*_t)(const float & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Navigator::viewBoxchanged)) {
                *result = 0;
            }
        }
        {
            typedef void (Navigator::*_t)(const QPointF & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Navigator::tranSlate)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject Navigator::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_Navigator.data,
      qt_meta_data_Navigator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Navigator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Navigator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Navigator.stringdata))
        return static_cast<void*>(const_cast< Navigator*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int Navigator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void Navigator::viewBoxchanged(const float & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Navigator::tranSlate(const QPointF & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
