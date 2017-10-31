/****************************************************************************
** Meta object code from reading C++ file 'graphicscene.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Draw/graphicscene.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graphicscene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PaintingWidget_t {
    QByteArrayData data[11];
    char stringdata[128];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PaintingWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PaintingWidget_t qt_meta_stringdata_PaintingWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "PaintingWidget"
QT_MOC_LITERAL(1, 15, 9), // "slotClear"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 12), // "DrawLineItem"
QT_MOC_LITERAL(4, 39, 4), // "rect"
QT_MOC_LITERAL(5, 44, 15), // "DrawEllipseItem"
QT_MOC_LITERAL(6, 60, 15), // "DrawPolygonItem"
QT_MOC_LITERAL(7, 76, 12), // "DrawPathItem"
QT_MOC_LITERAL(8, 89, 12), // "DrawTextItem"
QT_MOC_LITERAL(9, 102, 12), // "DrawRectItem"
QT_MOC_LITERAL(10, 115, 12) // "slotLogoItem"

    },
    "PaintingWidget\0slotClear\0\0DrawLineItem\0"
    "rect\0DrawEllipseItem\0DrawPolygonItem\0"
    "DrawPathItem\0DrawTextItem\0DrawRectItem\0"
    "slotLogoItem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PaintingWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    1,   65,    2, 0x0a /* Public */,
       5,    1,   68,    2, 0x0a /* Public */,
       6,    1,   71,    2, 0x0a /* Public */,
       7,    1,   74,    2, 0x0a /* Public */,
       7,    0,   77,    2, 0x2a /* Public | MethodCloned */,
       8,    1,   78,    2, 0x0a /* Public */,
       9,    0,   81,    2, 0x0a /* Public */,
      10,    1,   82,    2, 0x0a /* Public */,
      10,    0,   85,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect,    4,
    QMetaType::Void,

       0        // eod
};

void PaintingWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PaintingWidget *_t = static_cast<PaintingWidget *>(_o);
        switch (_id) {
        case 0: _t->slotClear(); break;
        case 1: _t->DrawLineItem((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 2: _t->DrawEllipseItem((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 3: _t->DrawPolygonItem((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 4: _t->DrawPathItem((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 5: _t->DrawPathItem(); break;
        case 6: _t->DrawTextItem((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 7: _t->DrawRectItem(); break;
        case 8: _t->slotLogoItem((*reinterpret_cast< QRect(*)>(_a[1]))); break;
        case 9: _t->slotLogoItem(); break;
        default: ;
        }
    }
}

const QMetaObject PaintingWidget::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_PaintingWidget.data,
      qt_meta_data_PaintingWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PaintingWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PaintingWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PaintingWidget.stringdata))
        return static_cast<void*>(const_cast< PaintingWidget*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int PaintingWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
struct qt_meta_stringdata_CGraphicsItem_t {
    QByteArrayData data[1];
    char stringdata[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CGraphicsItem_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CGraphicsItem_t qt_meta_stringdata_CGraphicsItem = {
    {
QT_MOC_LITERAL(0, 0, 13) // "CGraphicsItem"

    },
    "CGraphicsItem"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CGraphicsItem[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CGraphicsItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject CGraphicsItem::staticMetaObject = {
    { &QGraphicsObject::staticMetaObject, qt_meta_stringdata_CGraphicsItem.data,
      qt_meta_data_CGraphicsItem,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CGraphicsItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CGraphicsItem::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CGraphicsItem.stringdata))
        return static_cast<void*>(const_cast< CGraphicsItem*>(this));
    return QGraphicsObject::qt_metacast(_clname);
}

int CGraphicsItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
