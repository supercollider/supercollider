/****************************************************************************
** Meta object code from reading C++ file 'highlighter.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/code_editor/highlighter.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'highlighter.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__SyntaxHighlighterGlobals_t {
    QByteArrayData data[5];
    char stringdata[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__SyntaxHighlighterGlobals_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__SyntaxHighlighterGlobals_t qt_meta_stringdata_ScIDE__SyntaxHighlighterGlobals = {
    {
QT_MOC_LITERAL(0, 0, 31), // "ScIDE::SyntaxHighlighterGlobals"
QT_MOC_LITERAL(1, 32, 20), // "syntaxFormatsChanged"
QT_MOC_LITERAL(2, 53, 0), // ""
QT_MOC_LITERAL(3, 54, 13), // "applySettings"
QT_MOC_LITERAL(4, 68, 18) // "Settings::Manager*"

    },
    "ScIDE::SyntaxHighlighterGlobals\0"
    "syntaxFormatsChanged\0\0applySettings\0"
    "Settings::Manager*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__SyntaxHighlighterGlobals[] = {

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
       1,    0,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   25,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    2,

       0        // eod
};

void ScIDE::SyntaxHighlighterGlobals::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SyntaxHighlighterGlobals *_t = static_cast<SyntaxHighlighterGlobals *>(_o);
        switch (_id) {
        case 0: _t->syntaxFormatsChanged(); break;
        case 1: _t->applySettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SyntaxHighlighterGlobals::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SyntaxHighlighterGlobals::syntaxFormatsChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::SyntaxHighlighterGlobals::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__SyntaxHighlighterGlobals.data,
      qt_meta_data_ScIDE__SyntaxHighlighterGlobals,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::SyntaxHighlighterGlobals::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::SyntaxHighlighterGlobals::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__SyntaxHighlighterGlobals.stringdata))
        return static_cast<void*>(const_cast< SyntaxHighlighterGlobals*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::SyntaxHighlighterGlobals::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ScIDE::SyntaxHighlighterGlobals::syntaxFormatsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_ScIDE__SyntaxHighlighter_t {
    QByteArrayData data[1];
    char stringdata[25];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__SyntaxHighlighter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__SyntaxHighlighter_t qt_meta_stringdata_ScIDE__SyntaxHighlighter = {
    {
QT_MOC_LITERAL(0, 0, 24) // "ScIDE::SyntaxHighlighter"

    },
    "ScIDE::SyntaxHighlighter"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__SyntaxHighlighter[] = {

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

void ScIDE::SyntaxHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::SyntaxHighlighter::staticMetaObject = {
    { &QSyntaxHighlighter::staticMetaObject, qt_meta_stringdata_ScIDE__SyntaxHighlighter.data,
      qt_meta_data_ScIDE__SyntaxHighlighter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::SyntaxHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::SyntaxHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__SyntaxHighlighter.stringdata))
        return static_cast<void*>(const_cast< SyntaxHighlighter*>(this));
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int ScIDE::SyntaxHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
