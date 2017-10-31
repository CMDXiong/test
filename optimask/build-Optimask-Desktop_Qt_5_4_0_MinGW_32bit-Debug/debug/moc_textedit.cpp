/****************************************************************************
** Meta object code from reading C++ file 'textedit.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/Item/textedit.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'textedit.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TextEdit_t {
    QByteArrayData data[17];
    char stringdata[164];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TextEdit_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TextEdit_t qt_meta_stringdata_TextEdit = {
    {
QT_MOC_LITERAL(0, 0, 8), // "TextEdit"
QT_MOC_LITERAL(1, 9, 11), // "textChanged"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 7), // "setHtml"
QT_MOC_LITERAL(4, 30, 4), // "html"
QT_MOC_LITERAL(5, 35, 7), // "setBold"
QT_MOC_LITERAL(6, 43, 2), // "on"
QT_MOC_LITERAL(7, 46, 8), // "setColor"
QT_MOC_LITERAL(8, 55, 11), // "updateColor"
QT_MOC_LITERAL(9, 67, 5), // "color"
QT_MOC_LITERAL(10, 73, 16), // "setFontPointSize"
QT_MOC_LITERAL(11, 90, 6), // "points"
QT_MOC_LITERAL(12, 97, 13), // "setFontFamily"
QT_MOC_LITERAL(13, 111, 4), // "font"
QT_MOC_LITERAL(14, 116, 24), // "currentCharFormatChanged"
QT_MOC_LITERAL(15, 141, 15), // "QTextCharFormat"
QT_MOC_LITERAL(16, 157, 6) // "format"

    },
    "TextEdit\0textChanged\0\0setHtml\0html\0"
    "setBold\0on\0setColor\0updateColor\0color\0"
    "setFontPointSize\0points\0setFontFamily\0"
    "font\0currentCharFormatChanged\0"
    "QTextCharFormat\0format"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TextEdit[] = {

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
       1,    0,   54,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   55,    2, 0x0a /* Public */,
       5,    1,   58,    2, 0x08 /* Private */,
       7,    0,   61,    2, 0x08 /* Private */,
       8,    1,   62,    2, 0x08 /* Private */,
      10,    1,   65,    2, 0x08 /* Private */,
      12,    1,   68,    2, 0x08 /* Private */,
      14,    1,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QColor,    9,
    QMetaType::Void, QMetaType::Double,   11,
    QMetaType::Void, QMetaType::QFont,   13,
    QMetaType::Void, 0x80000000 | 15,   16,

       0        // eod
};

void TextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextEdit *_t = static_cast<TextEdit *>(_o);
        switch (_id) {
        case 0: _t->textChanged(); break;
        case 1: _t->setHtml((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->setBold((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->setColor(); break;
        case 4: _t->updateColor((*reinterpret_cast< const QColor(*)>(_a[1]))); break;
        case 5: _t->setFontPointSize((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->setFontFamily((*reinterpret_cast< const QFont(*)>(_a[1]))); break;
        case 7: _t->currentCharFormatChanged((*reinterpret_cast< const QTextCharFormat(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TextEdit::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TextEdit::textChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject TextEdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TextEdit.data,
      qt_meta_data_TextEdit,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *TextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_TextEdit.stringdata))
        return static_cast<void*>(const_cast< TextEdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int TextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void TextEdit::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
