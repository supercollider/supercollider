/****************************************************************************
** Meta object code from reading C++ file 'documents_dialog.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/documents_dialog.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'documents_dialog.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__DocumentsDialog_t {
    QByteArrayData data[10];
    char stringdata[140];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__DocumentsDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__DocumentsDialog_t qt_meta_stringdata_ScIDE__DocumentsDialog = {
    {
QT_MOC_LITERAL(0, 0, 22), // "ScIDE::DocumentsDialog"
QT_MOC_LITERAL(1, 23, 9), // "selectAll"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 10), // "selectNone"
QT_MOC_LITERAL(4, 45, 12), // "saveSelected"
QT_MOC_LITERAL(5, 58, 14), // "reloadSelected"
QT_MOC_LITERAL(6, 73, 14), // "ignoreSelected"
QT_MOC_LITERAL(7, 88, 13), // "closeSelected"
QT_MOC_LITERAL(8, 102, 27), // "onDocumentChangedExternally"
QT_MOC_LITERAL(9, 130, 9) // "Document*"

    },
    "ScIDE::DocumentsDialog\0selectAll\0\0"
    "selectNone\0saveSelected\0reloadSelected\0"
    "ignoreSelected\0closeSelected\0"
    "onDocumentChangedExternally\0Document*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__DocumentsDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x08 /* Private */,
       5,    0,   52,    2, 0x08 /* Private */,
       6,    0,   53,    2, 0x08 /* Private */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    1,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,    2,

       0        // eod
};

void ScIDE::DocumentsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DocumentsDialog *_t = static_cast<DocumentsDialog *>(_o);
        switch (_id) {
        case 0: _t->selectAll(); break;
        case 1: _t->selectNone(); break;
        case 2: _t->saveSelected(); break;
        case 3: _t->reloadSelected(); break;
        case 4: _t->ignoreSelected(); break;
        case 5: _t->closeSelected(); break;
        case 6: _t->onDocumentChangedExternally((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        }
    }
}

const QMetaObject ScIDE::DocumentsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ScIDE__DocumentsDialog.data,
      qt_meta_data_ScIDE__DocumentsDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::DocumentsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::DocumentsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__DocumentsDialog.stringdata))
        return static_cast<void*>(const_cast< DocumentsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ScIDE::DocumentsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
