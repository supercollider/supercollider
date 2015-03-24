/****************************************************************************
** Meta object code from reading C++ file 'editor_box.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/editor_box.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editor_box.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__CodeEditorBox_t {
    QByteArrayData data[12];
    char stringdata[148];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__CodeEditorBox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__CodeEditorBox_t qt_meta_stringdata_ScIDE__CodeEditorBox = {
    {
QT_MOC_LITERAL(0, 0, 20), // "ScIDE::CodeEditorBox"
QT_MOC_LITERAL(1, 21, 14), // "currentChanged"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 18), // "GenericCodeEditor*"
QT_MOC_LITERAL(4, 56, 9), // "activated"
QT_MOC_LITERAL(5, 66, 14), // "CodeEditorBox*"
QT_MOC_LITERAL(6, 81, 2), // "me"
QT_MOC_LITERAL(7, 84, 13), // "activeChanged"
QT_MOC_LITERAL(8, 98, 6), // "active"
QT_MOC_LITERAL(9, 105, 16), // "onDocumentClosed"
QT_MOC_LITERAL(10, 122, 9), // "Document*"
QT_MOC_LITERAL(11, 132, 15) // "onDocumentSaved"

    },
    "ScIDE::CodeEditorBox\0currentChanged\0"
    "\0GenericCodeEditor*\0activated\0"
    "CodeEditorBox*\0me\0activeChanged\0active\0"
    "onDocumentClosed\0Document*\0onDocumentSaved"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__CodeEditorBox[] = {

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
       4,    1,   42,    2, 0x06 /* Public */,
       7,    1,   45,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   48,    2, 0x08 /* Private */,
      11,    1,   51,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, QMetaType::Bool,    8,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 10,    2,
    QMetaType::Void, 0x80000000 | 10,    2,

       0        // eod
};

void ScIDE::CodeEditorBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CodeEditorBox *_t = static_cast<CodeEditorBox *>(_o);
        switch (_id) {
        case 0: _t->currentChanged((*reinterpret_cast< GenericCodeEditor*(*)>(_a[1]))); break;
        case 1: _t->activated((*reinterpret_cast< CodeEditorBox*(*)>(_a[1]))); break;
        case 2: _t->activeChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->onDocumentClosed((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 4: _t->onDocumentSaved((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< CodeEditorBox* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CodeEditorBox::*_t)(GenericCodeEditor * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CodeEditorBox::currentChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (CodeEditorBox::*_t)(CodeEditorBox * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CodeEditorBox::activated)) {
                *result = 1;
            }
        }
        {
            typedef void (CodeEditorBox::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CodeEditorBox::activeChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject ScIDE::CodeEditorBox::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__CodeEditorBox.data,
      qt_meta_data_ScIDE__CodeEditorBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::CodeEditorBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::CodeEditorBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__CodeEditorBox.stringdata))
        return static_cast<void*>(const_cast< CodeEditorBox*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::CodeEditorBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::CodeEditorBox::currentChanged(GenericCodeEditor * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::CodeEditorBox::activated(CodeEditorBox * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScIDE::CodeEditorBox::activeChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
