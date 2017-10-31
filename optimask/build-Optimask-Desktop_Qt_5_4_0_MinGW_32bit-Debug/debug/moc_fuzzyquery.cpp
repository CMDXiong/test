/****************************************************************************
** Meta object code from reading C++ file 'fuzzyquery.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Script/fuzzyquery.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'fuzzyquery.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FuzzyQuery_t {
    QByteArrayData data[9];
    char stringdata[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FuzzyQuery_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FuzzyQuery_t qt_meta_stringdata_FuzzyQuery = {
    {
QT_MOC_LITERAL(0, 0, 10), // "FuzzyQuery"
QT_MOC_LITERAL(1, 11, 14), // "textChangedInf"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 6), // "string"
QT_MOC_LITERAL(4, 34, 12), // "setCompleter"
QT_MOC_LITERAL(5, 47, 4), // "text"
QT_MOC_LITERAL(6, 52, 12), // "completeText"
QT_MOC_LITERAL(7, 65, 5), // "index"
QT_MOC_LITERAL(8, 71, 15) // "slotTextChanged"

    },
    "FuzzyQuery\0textChangedInf\0\0string\0"
    "setCompleter\0text\0completeText\0index\0"
    "slotTextChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FuzzyQuery[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   37,    2, 0x0a /* Public */,
       6,    1,   40,    2, 0x0a /* Public */,
       8,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QModelIndex,    7,
    QMetaType::Void,

       0        // eod
};

void FuzzyQuery::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FuzzyQuery *_t = static_cast<FuzzyQuery *>(_o);
        switch (_id) {
        case 0: _t->textChangedInf((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->setCompleter((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->completeText((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 3: _t->slotTextChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (FuzzyQuery::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&FuzzyQuery::textChangedInf)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject FuzzyQuery::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_FuzzyQuery.data,
      qt_meta_data_FuzzyQuery,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *FuzzyQuery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FuzzyQuery::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_FuzzyQuery.stringdata))
        return static_cast<void*>(const_cast< FuzzyQuery*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int FuzzyQuery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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
void FuzzyQuery::textChangedInf(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
