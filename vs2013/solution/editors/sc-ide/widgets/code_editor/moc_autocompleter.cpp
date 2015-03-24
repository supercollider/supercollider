/****************************************************************************
** Meta object code from reading C++ file 'autocompleter.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/code_editor/autocompleter.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'autocompleter.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__AutoCompleter_t {
    QByteArrayData data[11];
    char stringdata[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__AutoCompleter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__AutoCompleter_t qt_meta_stringdata_ScIDE__AutoCompleter = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ScIDE::AutoCompleter"
QT_MOC_LITERAL(1, 21, 16), // "onContentsChange"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 3), // "pos"
QT_MOC_LITERAL(4, 43, 7), // "removed"
QT_MOC_LITERAL(5, 51, 5), // "added"
QT_MOC_LITERAL(6, 57, 15), // "onCursorChanged"
QT_MOC_LITERAL(7, 73, 24), // "onCompletionMenuFinished"
QT_MOC_LITERAL(8, 98, 6), // "result"
QT_MOC_LITERAL(9, 105, 20), // "clearMethodCallStack"
QT_MOC_LITERAL(10, 126, 11) // "hideWidgets"

    },
    "ScIDE::AutoCompleter\0onContentsChange\0"
    "\0pos\0removed\0added\0onCursorChanged\0"
    "onCompletionMenuFinished\0result\0"
    "clearMethodCallStack\0hideWidgets"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__AutoCompleter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    3,   39,    2, 0x08 /* Private */,
       6,    0,   46,    2, 0x08 /* Private */,
       7,    1,   47,    2, 0x08 /* Private */,
       9,    0,   50,    2, 0x08 /* Private */,
      10,    0,   51,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::AutoCompleter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AutoCompleter *_t = static_cast<AutoCompleter *>(_o);
        switch (_id) {
        case 0: _t->onContentsChange((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->onCursorChanged(); break;
        case 2: _t->onCompletionMenuFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->clearMethodCallStack(); break;
        case 4: _t->hideWidgets(); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::AutoCompleter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__AutoCompleter.data,
      qt_meta_data_ScIDE__AutoCompleter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::AutoCompleter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::AutoCompleter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__AutoCompleter.stringdata))
        return static_cast<void*>(const_cast< AutoCompleter*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::AutoCompleter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
