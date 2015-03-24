/****************************************************************************
** Meta object code from reading C++ file 'lang_status_box.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/lang_status_box.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lang_status_box.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__LangStatusBox_t {
    QByteArrayData data[4];
    char stringdata[71];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__LangStatusBox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__LangStatusBox_t qt_meta_stringdata_ScIDE__LangStatusBox = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ScIDE::LangStatusBox"
QT_MOC_LITERAL(1, 21, 25), // "onInterpreterStateChanged"
QT_MOC_LITERAL(2, 47, 0), // ""
QT_MOC_LITERAL(3, 48, 22) // "QProcess::ProcessState"

    },
    "ScIDE::LangStatusBox\0onInterpreterStateChanged\0"
    "\0QProcess::ProcessState"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__LangStatusBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void ScIDE::LangStatusBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LangStatusBox *_t = static_cast<LangStatusBox *>(_o);
        switch (_id) {
        case 0: _t->onInterpreterStateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::LangStatusBox::staticMetaObject = {
    { &StatusBox::staticMetaObject, qt_meta_stringdata_ScIDE__LangStatusBox.data,
      qt_meta_data_ScIDE__LangStatusBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::LangStatusBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::LangStatusBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__LangStatusBox.stringdata))
        return static_cast<void*>(const_cast< LangStatusBox*>(this));
    return StatusBox::qt_metacast(_clname);
}

int ScIDE::LangStatusBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StatusBox::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
