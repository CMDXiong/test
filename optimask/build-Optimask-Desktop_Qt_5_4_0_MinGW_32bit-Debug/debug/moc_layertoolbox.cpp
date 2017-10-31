/****************************************************************************
** Meta object code from reading C++ file 'layertoolbox.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Layer/layertoolbox.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'layertoolbox.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LCPRemitter_t {
    QByteArrayData data[5];
    char stringdata[39];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPRemitter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPRemitter_t qt_meta_stringdata_LCPRemitter = {
    {
QT_MOC_LITERAL(0, 0, 11), // "LCPRemitter"
QT_MOC_LITERAL(1, 12, 10), // "the_signal"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 5), // "index"
QT_MOC_LITERAL(4, 30, 8) // "the_slot"

    },
    "LCPRemitter\0the_signal\0\0index\0the_slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPRemitter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void LCPRemitter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPRemitter *_t = static_cast<LCPRemitter *>(_o);
        switch (_id) {
        case 0: _t->the_signal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->the_slot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPRemitter::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPRemitter::the_signal)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject LCPRemitter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LCPRemitter.data,
      qt_meta_data_LCPRemitter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPRemitter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPRemitter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPRemitter.stringdata))
        return static_cast<void*>(const_cast< LCPRemitter*>(this));
    return QObject::qt_metacast(_clname);
}

int LCPRemitter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void LCPRemitter::the_signal(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_LCPColorPalette_t {
    QByteArrayData data[9];
    char stringdata[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPColorPalette_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPColorPalette_t qt_meta_stringdata_LCPColorPalette = {
    {
QT_MOC_LITERAL(0, 0, 15), // "LCPColorPalette"
QT_MOC_LITERAL(1, 16, 14), // "color_selected"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 25), // "color_brightness_selected"
QT_MOC_LITERAL(4, 58, 10), // "frameColor"
QT_MOC_LITERAL(5, 69, 7), // "QColor&"
QT_MOC_LITERAL(6, 77, 14), // "button_clicked"
QT_MOC_LITERAL(7, 92, 5), // "index"
QT_MOC_LITERAL(8, 98, 9) // "slotColor"

    },
    "LCPColorPalette\0color_selected\0\0"
    "color_brightness_selected\0frameColor\0"
    "QColor&\0button_clicked\0index\0slotColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPColorPalette[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    1,   42,    2, 0x06 /* Public */,
       4,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   48,    2, 0x0a /* Public */,
       8,    1,   51,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QColor,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 5,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void LCPColorPalette::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPColorPalette *_t = static_cast<LCPColorPalette *>(_o);
        switch (_id) {
        case 0: _t->color_selected((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 1: _t->color_brightness_selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->frameColor((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 3: _t->button_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->slotColor((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPColorPalette::*_t)(QColor );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPColorPalette::color_selected)) {
                *result = 0;
            }
        }
        {
            typedef void (LCPColorPalette::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPColorPalette::color_brightness_selected)) {
                *result = 1;
            }
        }
        {
            typedef void (LCPColorPalette::*_t)(QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPColorPalette::frameColor)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject LCPColorPalette::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_LCPColorPalette.data,
      qt_meta_data_LCPColorPalette,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPColorPalette::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPColorPalette::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPColorPalette.stringdata))
        return static_cast<void*>(const_cast< LCPColorPalette*>(this));
    return QFrame::qt_metacast(_clname);
}

int LCPColorPalette::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void LCPColorPalette::color_selected(QColor _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LCPColorPalette::color_brightness_selected(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void LCPColorPalette::frameColor(QColor & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
struct qt_meta_stringdata_LCPActiveLabel_t {
    QByteArrayData data[3];
    char stringdata[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPActiveLabel_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPActiveLabel_t qt_meta_stringdata_LCPActiveLabel = {
    {
QT_MOC_LITERAL(0, 0, 14), // "LCPActiveLabel"
QT_MOC_LITERAL(1, 15, 7), // "clicked"
QT_MOC_LITERAL(2, 23, 0) // ""

    },
    "LCPActiveLabel\0clicked\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPActiveLabel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void LCPActiveLabel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPActiveLabel *_t = static_cast<LCPActiveLabel *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPActiveLabel::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPActiveLabel::clicked)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject LCPActiveLabel::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_LCPActiveLabel.data,
      qt_meta_data_LCPActiveLabel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPActiveLabel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPActiveLabel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPActiveLabel.stringdata))
        return static_cast<void*>(const_cast< LCPActiveLabel*>(this));
    return QLabel::qt_metacast(_clname);
}

int LCPActiveLabel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void LCPActiveLabel::clicked(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_LCPAnimationPalette_t {
    QByteArrayData data[6];
    char stringdata[66];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPAnimationPalette_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPAnimationPalette_t qt_meta_stringdata_LCPAnimationPalette = {
    {
QT_MOC_LITERAL(0, 0, 19), // "LCPAnimationPalette"
QT_MOC_LITERAL(1, 20, 18), // "animation_selected"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 4), // "mode"
QT_MOC_LITERAL(4, 45, 14), // "button_clicked"
QT_MOC_LITERAL(5, 60, 5) // "index"

    },
    "LCPAnimationPalette\0animation_selected\0"
    "\0mode\0button_clicked\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPAnimationPalette[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   27,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,

       0        // eod
};

void LCPAnimationPalette::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPAnimationPalette *_t = static_cast<LCPAnimationPalette *>(_o);
        switch (_id) {
        case 0: _t->animation_selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->button_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPAnimationPalette::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPAnimationPalette::animation_selected)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject LCPAnimationPalette::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_LCPAnimationPalette.data,
      qt_meta_data_LCPAnimationPalette,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPAnimationPalette::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPAnimationPalette::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPAnimationPalette.stringdata))
        return static_cast<void*>(const_cast< LCPAnimationPalette*>(this));
    return QFrame::qt_metacast(_clname);
}

int LCPAnimationPalette::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void LCPAnimationPalette::animation_selected(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_LCPStylePalette_t {
    QByteArrayData data[8];
    char stringdata[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPStylePalette_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPStylePalette_t qt_meta_stringdata_LCPStylePalette = {
    {
QT_MOC_LITERAL(0, 0, 15), // "LCPStylePalette"
QT_MOC_LITERAL(1, 16, 14), // "width_selected"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 5), // "width"
QT_MOC_LITERAL(4, 38, 15), // "marked_selected"
QT_MOC_LITERAL(5, 54, 6), // "marked"
QT_MOC_LITERAL(6, 61, 14), // "button_clicked"
QT_MOC_LITERAL(7, 76, 5) // "index"

    },
    "LCPStylePalette\0width_selected\0\0width\0"
    "marked_selected\0marked\0button_clicked\0"
    "index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPStylePalette[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Bool,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void LCPStylePalette::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPStylePalette *_t = static_cast<LCPStylePalette *>(_o);
        switch (_id) {
        case 0: _t->width_selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->marked_selected((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->button_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPStylePalette::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPStylePalette::width_selected)) {
                *result = 0;
            }
        }
        {
            typedef void (LCPStylePalette::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPStylePalette::marked_selected)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject LCPStylePalette::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_LCPStylePalette.data,
      qt_meta_data_LCPStylePalette,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPStylePalette::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPStylePalette::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPStylePalette.stringdata))
        return static_cast<void*>(const_cast< LCPStylePalette*>(this));
    return QFrame::qt_metacast(_clname);
}

int LCPStylePalette::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LCPStylePalette::width_selected(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LCPStylePalette::marked_selected(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_LCPDitherPalette_t {
    QByteArrayData data[8];
    char stringdata[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPDitherPalette_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPDitherPalette_t qt_meta_stringdata_LCPDitherPalette = {
    {
QT_MOC_LITERAL(0, 0, 16), // "LCPDitherPalette"
QT_MOC_LITERAL(1, 17, 15), // "dither_selected"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 12), // "dither_index"
QT_MOC_LITERAL(4, 47, 15), // "pattern_changed"
QT_MOC_LITERAL(5, 63, 13), // "DitherPattern"
QT_MOC_LITERAL(6, 77, 14), // "button_clicked"
QT_MOC_LITERAL(7, 92, 5) // "index"

    },
    "LCPDitherPalette\0dither_selected\0\0"
    "dither_index\0pattern_changed\0DitherPattern\0"
    "button_clicked\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPDitherPalette[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, 0x80000000 | 5,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void LCPDitherPalette::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPDitherPalette *_t = static_cast<LCPDitherPalette *>(_o);
        switch (_id) {
        case 0: _t->dither_selected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->pattern_changed((*reinterpret_cast< const DitherPattern(*)>(_a[1]))); break;
        case 2: _t->button_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPDitherPalette::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPDitherPalette::dither_selected)) {
                *result = 0;
            }
        }
        {
            typedef void (LCPDitherPalette::*_t)(const DitherPattern & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPDitherPalette::pattern_changed)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject LCPDitherPalette::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_LCPDitherPalette.data,
      qt_meta_data_LCPDitherPalette,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPDitherPalette::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPDitherPalette::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPDitherPalette.stringdata))
        return static_cast<void*>(const_cast< LCPDitherPalette*>(this));
    return QFrame::qt_metacast(_clname);
}

int LCPDitherPalette::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LCPDitherPalette::dither_selected(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LCPDitherPalette::pattern_changed(const DitherPattern & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_LayerToolbox_t {
    QByteArrayData data[30];
    char stringdata[391];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LayerToolbox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LayerToolbox_t qt_meta_stringdata_LayerToolbox = {
    {
QT_MOC_LITERAL(0, 0, 12), // "LayerToolbox"
QT_MOC_LITERAL(1, 13, 13), // "frameColorInf"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 7), // "QColor&"
QT_MOC_LITERAL(4, 36, 12), // "fillColorInf"
QT_MOC_LITERAL(5, 49, 20), // "panel_button_clicked"
QT_MOC_LITERAL(6, 70, 5), // "index"
QT_MOC_LITERAL(7, 76, 18), // "fill_color_changed"
QT_MOC_LITERAL(8, 95, 1), // "c"
QT_MOC_LITERAL(9, 97, 19), // "frame_color_changed"
QT_MOC_LITERAL(10, 117, 21), // "fill_color_brightness"
QT_MOC_LITERAL(11, 139, 5), // "delta"
QT_MOC_LITERAL(12, 145, 22), // "frame_color_brightness"
QT_MOC_LITERAL(13, 168, 14), // "dither_changed"
QT_MOC_LITERAL(14, 183, 2), // "di"
QT_MOC_LITERAL(15, 186, 22), // "dither_pattern_changed"
QT_MOC_LITERAL(16, 209, 13), // "DitherPattern"
QT_MOC_LITERAL(17, 223, 7), // "pattern"
QT_MOC_LITERAL(18, 231, 18), // "visibility_changed"
QT_MOC_LITERAL(19, 250, 7), // "visible"
QT_MOC_LITERAL(20, 258, 20), // "transparency_changed"
QT_MOC_LITERAL(21, 279, 11), // "transparent"
QT_MOC_LITERAL(22, 291, 13), // "width_changed"
QT_MOC_LITERAL(23, 305, 5), // "width"
QT_MOC_LITERAL(24, 311, 14), // "marked_changed"
QT_MOC_LITERAL(25, 326, 6), // "marked"
QT_MOC_LITERAL(26, 333, 17), // "animation_changed"
QT_MOC_LITERAL(27, 351, 4), // "mode"
QT_MOC_LITERAL(28, 356, 17), // "slotframeColorInf"
QT_MOC_LITERAL(29, 374, 16) // "slotfillColorInf"

    },
    "LayerToolbox\0frameColorInf\0\0QColor&\0"
    "fillColorInf\0panel_button_clicked\0"
    "index\0fill_color_changed\0c\0"
    "frame_color_changed\0fill_color_brightness\0"
    "delta\0frame_color_brightness\0"
    "dither_changed\0di\0dither_pattern_changed\0"
    "DitherPattern\0pattern\0visibility_changed\0"
    "visible\0transparency_changed\0transparent\0"
    "width_changed\0width\0marked_changed\0"
    "marked\0animation_changed\0mode\0"
    "slotframeColorInf\0slotfillColorInf"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LayerToolbox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   94,    2, 0x06 /* Public */,
       4,    1,   97,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,  100,    2, 0x09 /* Protected */,
       7,    1,  103,    2, 0x09 /* Protected */,
       9,    1,  106,    2, 0x09 /* Protected */,
      10,    1,  109,    2, 0x09 /* Protected */,
      12,    1,  112,    2, 0x09 /* Protected */,
      13,    1,  115,    2, 0x09 /* Protected */,
      15,    1,  118,    2, 0x09 /* Protected */,
      18,    1,  121,    2, 0x09 /* Protected */,
      20,    1,  124,    2, 0x09 /* Protected */,
      22,    1,  127,    2, 0x09 /* Protected */,
      24,    1,  130,    2, 0x09 /* Protected */,
      26,    1,  133,    2, 0x09 /* Protected */,
      28,    1,  136,    2, 0x08 /* Private */,
      29,    1,  139,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::QColor,    8,
    QMetaType::Void, QMetaType::QColor,    8,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   11,
    QMetaType::Void, QMetaType::Int,   14,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void, QMetaType::Bool,   19,
    QMetaType::Void, QMetaType::Bool,   21,
    QMetaType::Void, QMetaType::Int,   23,
    QMetaType::Void, QMetaType::Bool,   25,
    QMetaType::Void, QMetaType::Int,   27,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void LayerToolbox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LayerToolbox *_t = static_cast<LayerToolbox *>(_o);
        switch (_id) {
        case 0: _t->frameColorInf((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 1: _t->fillColorInf((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 2: _t->panel_button_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->fill_color_changed((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 4: _t->frame_color_changed((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 5: _t->fill_color_brightness((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->frame_color_brightness((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->dither_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->dither_pattern_changed((*reinterpret_cast< const DitherPattern(*)>(_a[1]))); break;
        case 9: _t->visibility_changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->transparency_changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->width_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->marked_changed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->animation_changed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->slotframeColorInf((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        case 15: _t->slotfillColorInf((*reinterpret_cast< QColor(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LayerToolbox::*_t)(QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LayerToolbox::frameColorInf)) {
                *result = 0;
            }
        }
        {
            typedef void (LayerToolbox::*_t)(QColor & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LayerToolbox::fillColorInf)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject LayerToolbox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LayerToolbox.data,
      qt_meta_data_LayerToolbox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LayerToolbox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LayerToolbox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LayerToolbox.stringdata))
        return static_cast<void*>(const_cast< LayerToolbox*>(this));
    return QWidget::qt_metacast(_clname);
}

int LayerToolbox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void LayerToolbox::frameColorInf(QColor & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LayerToolbox::fillColorInf(QColor & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_LCPVisibilityPalette_t {
    QByteArrayData data[8];
    char stringdata[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LCPVisibilityPalette_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LCPVisibilityPalette_t qt_meta_stringdata_LCPVisibilityPalette = {
    {
QT_MOC_LITERAL(0, 0, 20), // "LCPVisibilityPalette"
QT_MOC_LITERAL(1, 21, 17), // "visibility_change"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 7), // "visible"
QT_MOC_LITERAL(4, 48, 19), // "transparency_change"
QT_MOC_LITERAL(5, 68, 11), // "transparent"
QT_MOC_LITERAL(6, 80, 14), // "button_clicked"
QT_MOC_LITERAL(7, 95, 5) // "index"

    },
    "LCPVisibilityPalette\0visibility_change\0"
    "\0visible\0transparency_change\0transparent\0"
    "button_clicked\0index"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LCPVisibilityPalette[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       4,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void LCPVisibilityPalette::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LCPVisibilityPalette *_t = static_cast<LCPVisibilityPalette *>(_o);
        switch (_id) {
        case 0: _t->visibility_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->transparency_change((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->button_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LCPVisibilityPalette::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPVisibilityPalette::visibility_change)) {
                *result = 0;
            }
        }
        {
            typedef void (LCPVisibilityPalette::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LCPVisibilityPalette::transparency_change)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject LCPVisibilityPalette::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_LCPVisibilityPalette.data,
      qt_meta_data_LCPVisibilityPalette,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LCPVisibilityPalette::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LCPVisibilityPalette::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LCPVisibilityPalette.stringdata))
        return static_cast<void*>(const_cast< LCPVisibilityPalette*>(this));
    return QFrame::qt_metacast(_clname);
}

int LCPVisibilityPalette::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void LCPVisibilityPalette::visibility_change(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LCPVisibilityPalette::transparency_change(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
