/****************************************************************************
** Meta object code from reading C++ file 'viewportbox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Layer/viewportbox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewportbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_NavigatorViewBox_t {
    QByteArrayData data[14];
    char stringdata[159];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_NavigatorViewBox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_NavigatorViewBox_t qt_meta_stringdata_NavigatorViewBox = {
    {
QT_MOC_LITERAL(0, 0, 16), // "NavigatorViewBox"
QT_MOC_LITERAL(1, 17, 18), // "currentViewBoxZoom"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 11), // "scaleFactor"
QT_MOC_LITERAL(4, 49, 8), // "setAngle"
QT_MOC_LITERAL(5, 58, 5), // "angle"
QT_MOC_LITERAL(6, 64, 18), // "setShearHorizontal"
QT_MOC_LITERAL(7, 83, 15), // "shearHorizontal"
QT_MOC_LITERAL(8, 99, 16), // "setShearVertical"
QT_MOC_LITERAL(9, 116, 13), // "shearVertical"
QT_MOC_LITERAL(10, 130, 8), // "setShear"
QT_MOC_LITERAL(11, 139, 6), // "zoomIn"
QT_MOC_LITERAL(12, 146, 7), // "zoomOut"
QT_MOC_LITERAL(13, 154, 4) // "zoom"

    },
    "NavigatorViewBox\0currentViewBoxZoom\0"
    "\0scaleFactor\0setAngle\0angle\0"
    "setShearHorizontal\0shearHorizontal\0"
    "setShearVertical\0shearVertical\0setShear\0"
    "zoomIn\0zoomOut\0zoom"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_NavigatorViewBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   57,    2, 0x0a /* Public */,
       6,    1,   60,    2, 0x0a /* Public */,
       8,    1,   63,    2, 0x0a /* Public */,
      10,    2,   66,    2, 0x0a /* Public */,
      11,    0,   71,    2, 0x0a /* Public */,
      12,    0,   72,    2, 0x0a /* Public */,
      13,    1,   73,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    7,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    3,

       0        // eod
};

void NavigatorViewBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        NavigatorViewBox *_t = static_cast<NavigatorViewBox *>(_o);
        switch (_id) {
        case 0: _t->currentViewBoxZoom((*reinterpret_cast< const float(*)>(_a[1]))); break;
        case 1: _t->setAngle((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->setShearHorizontal((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->setShearVertical((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: _t->setShear((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: _t->zoomIn(); break;
        case 6: _t->zoomOut(); break;
        case 7: _t->zoom((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (NavigatorViewBox::*_t)(const float & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&NavigatorViewBox::currentViewBoxZoom)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject NavigatorViewBox::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NavigatorViewBox.data,
      qt_meta_data_NavigatorViewBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *NavigatorViewBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NavigatorViewBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_NavigatorViewBox.stringdata))
        return static_cast<void*>(const_cast< NavigatorViewBox*>(this));
    if (!strcmp(_clname, "QGraphicsRectItem"))
        return static_cast< QGraphicsRectItem*>(const_cast< NavigatorViewBox*>(this));
    return QObject::qt_metacast(_clname);
}

int NavigatorViewBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void NavigatorViewBox::currentViewBoxZoom(const float & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
