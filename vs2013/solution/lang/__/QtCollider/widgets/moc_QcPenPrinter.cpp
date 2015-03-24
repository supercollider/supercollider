/****************************************************************************
** Meta object code from reading C++ file 'QcPenPrinter.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcPenPrinter.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcPenPrinter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcPenPrinter_t {
    QByteArrayData data[11];
    char stringdata[89];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcPenPrinter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcPenPrinter_t qt_meta_stringdata_QcPenPrinter = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcPenPrinter"
QT_MOC_LITERAL(1, 13, 10), // "dialogDone"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "printFunc"
QT_MOC_LITERAL(4, 35, 4), // "show"
QT_MOC_LITERAL(5, 40, 5), // "print"
QT_MOC_LITERAL(6, 46, 7), // "newPage"
QT_MOC_LITERAL(7, 54, 8), // "pageRect"
QT_MOC_LITERAL(8, 63, 9), // "paperRect"
QT_MOC_LITERAL(9, 73, 8), // "fromPage"
QT_MOC_LITERAL(10, 82, 6) // "toPage"

    },
    "QcPenPrinter\0dialogDone\0\0printFunc\0"
    "show\0print\0newPage\0pageRect\0paperRect\0"
    "fromPage\0toPage"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcPenPrinter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       4,   46, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   43,    2, 0x08 /* Private */,
       5,    0,   44,    2, 0x08 /* Private */,
       6,    0,   45,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::QRect, 0x00095001,
       8, QMetaType::QRect, 0x00095001,
       9, QMetaType::Int, 0x00095001,
      10, QMetaType::Int, 0x00095001,

       0        // eod
};

void QcPenPrinter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcPenPrinter *_t = static_cast<QcPenPrinter *>(_o);
        switch (_id) {
        case 0: _t->dialogDone((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->printFunc(); break;
        case 2: _t->show(); break;
        case 3: _t->print(); break;
        case 4: _t->newPage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcPenPrinter::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcPenPrinter::dialogDone)) {
                *result = 0;
            }
        }
        {
            typedef void (QcPenPrinter::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcPenPrinter::printFunc)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QcPenPrinter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QcPenPrinter.data,
      qt_meta_data_QcPenPrinter,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcPenPrinter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcPenPrinter::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcPenPrinter.stringdata))
        return static_cast<void*>(const_cast< QcPenPrinter*>(this));
    return QObject::qt_metacast(_clname);
}

int QcPenPrinter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QRect*>(_v) = pageRect(); break;
        case 1: *reinterpret_cast< QRect*>(_v) = paperRect(); break;
        case 2: *reinterpret_cast< int*>(_v) = fromPage(); break;
        case 3: *reinterpret_cast< int*>(_v) = toPage(); break;
        default: break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcPenPrinter::dialogDone(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QcPenPrinter::printFunc()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
