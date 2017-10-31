/****************************************************************************
** Meta object code from reading C++ file 'graphicsview.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Draw/graphicsview.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graphicsview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_COptimaskView_t {
    QByteArrayData data[20];
    char stringdata[259];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_COptimaskView_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_COptimaskView_t qt_meta_stringdata_COptimaskView = {
    {
QT_MOC_LITERAL(0, 0, 13), // "COptimaskView"
QT_MOC_LITERAL(1, 14, 18), // "CurrentViewChanged"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 19), // "currentShapeChanged"
QT_MOC_LITERAL(4, 54, 9), // "shapeName"
QT_MOC_LITERAL(5, 64, 9), // "drawShape"
QT_MOC_LITERAL(6, 74, 4), // "type"
QT_MOC_LITERAL(7, 79, 6), // "zoomIn"
QT_MOC_LITERAL(8, 86, 7), // "zoomOut"
QT_MOC_LITERAL(9, 94, 4), // "zoom"
QT_MOC_LITERAL(10, 99, 11), // "scaleFactor"
QT_MOC_LITERAL(11, 111, 9), // "tranSlate"
QT_MOC_LITERAL(12, 121, 5), // "delta"
QT_MOC_LITERAL(13, 127, 19), // "DrawRectWithCommand"
QT_MOC_LITERAL(14, 147, 6), // "CGeoPt"
QT_MOC_LITERAL(15, 154, 19), // "DrawLineWithCommand"
QT_MOC_LITERAL(16, 174, 22), // "DrawPolygonWithCommand"
QT_MOC_LITERAL(17, 197, 19), // "std::vector<CGeoPt>"
QT_MOC_LITERAL(18, 217, 22), // "DrawEllipseWithCommand"
QT_MOC_LITERAL(19, 240, 18) // "DrawArcWithCommand"

    },
    "COptimaskView\0CurrentViewChanged\0\0"
    "currentShapeChanged\0shapeName\0drawShape\0"
    "type\0zoomIn\0zoomOut\0zoom\0scaleFactor\0"
    "tranSlate\0delta\0DrawRectWithCommand\0"
    "CGeoPt\0DrawLineWithCommand\0"
    "DrawPolygonWithCommand\0std::vector<CGeoPt>\0"
    "DrawEllipseWithCommand\0DrawArcWithCommand"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_COptimaskView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x06 /* Public */,
       3,    1,   75,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   78,    2, 0x0a /* Public */,
       7,    0,   81,    2, 0x0a /* Public */,
       8,    0,   82,    2, 0x0a /* Public */,
       9,    1,   83,    2, 0x0a /* Public */,
      11,    1,   86,    2, 0x0a /* Public */,
      13,    2,   89,    2, 0x0a /* Public */,
      15,    2,   94,    2, 0x0a /* Public */,
      16,    2,   99,    2, 0x0a /* Public */,
      18,    3,  104,    2, 0x0a /* Public */,
      19,    4,  111,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, QMetaType::QPointF,   12,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14,    2,    2,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14,    2,    2,
    QMetaType::Void, 0x80000000 | 17, QMetaType::Bool,    2,    2,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Double, QMetaType::Double,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 14, QMetaType::Double, QMetaType::Double, QMetaType::Double,    2,    2,    2,    2,

       0        // eod
};

void COptimaskView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        COptimaskView *_t = static_cast<COptimaskView *>(_o);
        switch (_id) {
        case 0: _t->CurrentViewChanged(); break;
        case 1: _t->currentShapeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->drawShape((*reinterpret_cast< const int(*)>(_a[1]))); break;
        case 3: _t->zoomIn(); break;
        case 4: _t->zoomOut(); break;
        case 5: _t->zoom((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 6: _t->tranSlate((*reinterpret_cast< QPointF(*)>(_a[1]))); break;
        case 7: _t->DrawRectWithCommand((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< CGeoPt(*)>(_a[2]))); break;
        case 8: _t->DrawLineWithCommand((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< CGeoPt(*)>(_a[2]))); break;
        case 9: _t->DrawPolygonWithCommand((*reinterpret_cast< std::vector<CGeoPt>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 10: _t->DrawEllipseWithCommand((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 11: _t->DrawArcWithCommand((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (COptimaskView::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&COptimaskView::CurrentViewChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (COptimaskView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&COptimaskView::currentShapeChanged)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject COptimaskView::staticMetaObject = {
    { &QGraphicsView::staticMetaObject, qt_meta_stringdata_COptimaskView.data,
      qt_meta_data_COptimaskView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *COptimaskView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *COptimaskView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_COptimaskView.stringdata))
        return static_cast<void*>(const_cast< COptimaskView*>(this));
    if (!strcmp(_clname, "CSceneRender"))
        return static_cast< CSceneRender*>(const_cast< COptimaskView*>(this));
    if (!strcmp(_clname, "CCommandReceiverBase"))
        return static_cast< CCommandReceiverBase*>(const_cast< COptimaskView*>(this));
    return QGraphicsView::qt_metacast(_clname);
}

int COptimaskView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void COptimaskView::CurrentViewChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void COptimaskView::currentShapeChanged(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
