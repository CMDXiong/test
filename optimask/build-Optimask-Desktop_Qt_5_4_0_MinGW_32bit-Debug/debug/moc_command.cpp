/****************************************************************************
** Meta object code from reading C++ file 'command.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Script/command.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'command.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Command_t {
    QByteArrayData data[21];
    char stringdata[271];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Command_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Command_t qt_meta_stringdata_Command = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Command"
QT_MOC_LITERAL(1, 8, 11), // "drawRectInf"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 6), // "CGeoPt"
QT_MOC_LITERAL(4, 28, 11), // "drawLineInf"
QT_MOC_LITERAL(5, 40, 14), // "drawPolygonInf"
QT_MOC_LITERAL(6, 55, 19), // "std::vector<CGeoPt>"
QT_MOC_LITERAL(7, 75, 14), // "drawEllipseInf"
QT_MOC_LITERAL(8, 90, 10), // "drawArcInf"
QT_MOC_LITERAL(9, 101, 7), // "redoInf"
QT_MOC_LITERAL(10, 109, 7), // "undoInf"
QT_MOC_LITERAL(11, 117, 17), // "slotCommandResize"
QT_MOC_LITERAL(12, 135, 14), // "slotAutoScroll"
QT_MOC_LITERAL(13, 150, 18), // "slotOpenFileDialog"
QT_MOC_LITERAL(14, 169, 15), // "slotSaveCmdEidt"
QT_MOC_LITERAL(15, 185, 16), // "slotSaveHistEdit"
QT_MOC_LITERAL(16, 202, 13), // "slotExcuteCmd"
QT_MOC_LITERAL(17, 216, 15), // "slotShowCommand"
QT_MOC_LITERAL(18, 232, 13), // "CCommandBase*"
QT_MOC_LITERAL(19, 246, 15), // "slotUndoAndRedo"
QT_MOC_LITERAL(20, 262, 8) // "QString&"

    },
    "Command\0drawRectInf\0\0CGeoPt\0drawLineInf\0"
    "drawPolygonInf\0std::vector<CGeoPt>\0"
    "drawEllipseInf\0drawArcInf\0redoInf\0"
    "undoInf\0slotCommandResize\0slotAutoScroll\0"
    "slotOpenFileDialog\0slotSaveCmdEidt\0"
    "slotSaveHistEdit\0slotExcuteCmd\0"
    "slotShowCommand\0CCommandBase*\0"
    "slotUndoAndRedo\0QString&"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Command[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   89,    2, 0x06 /* Public */,
       4,    2,   94,    2, 0x06 /* Public */,
       5,    2,   99,    2, 0x06 /* Public */,
       7,    3,  104,    2, 0x06 /* Public */,
       8,    4,  111,    2, 0x06 /* Public */,
       9,    0,  120,    2, 0x06 /* Public */,
      10,    0,  121,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,  122,    2, 0x08 /* Private */,
      12,    0,  123,    2, 0x08 /* Private */,
      13,    0,  124,    2, 0x08 /* Private */,
      14,    0,  125,    2, 0x08 /* Private */,
      15,    0,  126,    2, 0x08 /* Private */,
      16,    0,  127,    2, 0x08 /* Private */,
      17,    1,  128,    2, 0x08 /* Private */,
      19,    2,  131,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    2,    2,
    QMetaType::Void, 0x80000000 | 6, QMetaType::Bool,    2,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Double, QMetaType::Double,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Double, QMetaType::Double, QMetaType::Double,    2,    2,    2,    2,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 18,    2,
    QMetaType::Void, 0x80000000 | 20, 0x80000000 | 18,    2,    2,

       0        // eod
};

void Command::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Command *_t = static_cast<Command *>(_o);
        switch (_id) {
        case 0: _t->drawRectInf((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< CGeoPt(*)>(_a[2]))); break;
        case 1: _t->drawLineInf((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< CGeoPt(*)>(_a[2]))); break;
        case 2: _t->drawPolygonInf((*reinterpret_cast< std::vector<CGeoPt>(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->drawEllipseInf((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3]))); break;
        case 4: _t->drawArcInf((*reinterpret_cast< CGeoPt(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 5: _t->redoInf(); break;
        case 6: _t->undoInf(); break;
        case 7: _t->slotCommandResize(); break;
        case 8: _t->slotAutoScroll(); break;
        case 9: _t->slotOpenFileDialog(); break;
        case 10: _t->slotSaveCmdEidt(); break;
        case 11: _t->slotSaveHistEdit(); break;
        case 12: _t->slotExcuteCmd(); break;
        case 13: _t->slotShowCommand((*reinterpret_cast< CCommandBase*(*)>(_a[1]))); break;
        case 14: _t->slotUndoAndRedo((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< CCommandBase*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Command::*_t)(CGeoPt , CGeoPt );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::drawRectInf)) {
                *result = 0;
            }
        }
        {
            typedef void (Command::*_t)(CGeoPt , CGeoPt );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::drawLineInf)) {
                *result = 1;
            }
        }
        {
            typedef void (Command::*_t)(std::vector<CGeoPt> , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::drawPolygonInf)) {
                *result = 2;
            }
        }
        {
            typedef void (Command::*_t)(CGeoPt , double , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::drawEllipseInf)) {
                *result = 3;
            }
        }
        {
            typedef void (Command::*_t)(CGeoPt , double , double , double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::drawArcInf)) {
                *result = 4;
            }
        }
        {
            typedef void (Command::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::redoInf)) {
                *result = 5;
            }
        }
        {
            typedef void (Command::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Command::undoInf)) {
                *result = 6;
            }
        }
    }
}

const QMetaObject Command::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Command.data,
      qt_meta_data_Command,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Command::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Command::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Command.stringdata))
        return static_cast<void*>(const_cast< Command*>(this));
    return QWidget::qt_metacast(_clname);
}

int Command::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Command::drawRectInf(CGeoPt _t1, CGeoPt _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Command::drawLineInf(CGeoPt _t1, CGeoPt _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Command::drawPolygonInf(std::vector<CGeoPt> _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Command::drawEllipseInf(CGeoPt _t1, double _t2, double _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Command::drawArcInf(CGeoPt _t1, double _t2, double _t3, double _t4)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Command::redoInf()
{
    QMetaObject::activate(this, &staticMetaObject, 5, Q_NULLPTR);
}

// SIGNAL 6
void Command::undoInf()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
