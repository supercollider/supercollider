/****************************************************************************
** Meta object code from reading C++ file 'QcFileDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcFileDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcFileDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcFileDialog_t {
    QByteArrayData data[10];
    char stringdata[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcFileDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcFileDialog_t qt_meta_stringdata_QcFileDialog = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcFileDialog"
QT_MOC_LITERAL(1, 13, 8), // "accepted"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 6), // "result"
QT_MOC_LITERAL(4, 30, 8), // "rejected"
QT_MOC_LITERAL(5, 39, 4), // "show"
QT_MOC_LITERAL(6, 44, 10), // "onFinished"
QT_MOC_LITERAL(7, 55, 3), // "res"
QT_MOC_LITERAL(8, 59, 8), // "fileMode"
QT_MOC_LITERAL(9, 68, 10) // "acceptMode"

    },
    "QcFileDialog\0accepted\0\0result\0rejected\0"
    "show\0onFinished\0res\0fileMode\0acceptMode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcFileDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       3,   51, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       4,    0,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   38,    2, 0x08 /* Private */,
       6,    1,   39,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QVariantList,    3,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,

 // constructors: parameters
    0x80000000 | 2, QMetaType::Int, QMetaType::Int,    8,    9,
    0x80000000 | 2, QMetaType::Int,    8,
    0x80000000 | 2,

 // constructors: name, argc, parameters, tag, flags
       0,    2,   42,    2, 0x0e /* Public */,
       0,    1,   47,    2, 0x2e /* Public | MethodCloned */,
       0,    0,   50,    2, 0x2e /* Public | MethodCloned */,

       0        // eod
};

void QcFileDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QcFileDialog *_r = new QcFileDialog((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 1: { QcFileDialog *_r = new QcFileDialog((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        case 2: { QcFileDialog *_r = new QcFileDialog();
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        QcFileDialog *_t = static_cast<QcFileDialog *>(_o);
        switch (_id) {
        case 0: _t->accepted((*reinterpret_cast< QVariantList(*)>(_a[1]))); break;
        case 1: _t->rejected(); break;
        case 2: _t->show(); break;
        case 3: _t->onFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcFileDialog::*_t)(QVariantList );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcFileDialog::accepted)) {
                *result = 0;
            }
        }
        {
            typedef void (QcFileDialog::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcFileDialog::rejected)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QcFileDialog::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QcFileDialog.data,
      qt_meta_data_QcFileDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcFileDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcFileDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcFileDialog.stringdata))
        return static_cast<void*>(const_cast< QcFileDialog*>(this));
    return QObject::qt_metacast(_clname);
}

int QcFileDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void QcFileDialog::accepted(QVariantList _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QcFileDialog::rejected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
