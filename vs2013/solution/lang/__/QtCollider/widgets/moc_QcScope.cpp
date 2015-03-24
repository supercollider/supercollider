/****************************************************************************
** Meta object code from reading C++ file 'QcScope.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcScope.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcScope.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcScope_t {
    QByteArrayData data[12];
    char stringdata[105];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcScope_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcScope_t qt_meta_stringdata_QcScope = {
    {
QT_MOC_LITERAL(0, 0, 7), // "QcScope"
QT_MOC_LITERAL(1, 8, 11), // "updateScope"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 12), // "bufferNumber"
QT_MOC_LITERAL(4, 34, 7), // "xOffset"
QT_MOC_LITERAL(5, 42, 7), // "yOffset"
QT_MOC_LITERAL(6, 50, 5), // "xZoom"
QT_MOC_LITERAL(7, 56, 5), // "yZoom"
QT_MOC_LITERAL(8, 62, 5), // "style"
QT_MOC_LITERAL(9, 68, 10), // "waveColors"
QT_MOC_LITERAL(10, 79, 10), // "background"
QT_MOC_LITERAL(11, 90, 14) // "updateInterval"

    },
    "QcScope\0updateScope\0\0bufferNumber\0"
    "xOffset\0yOffset\0xZoom\0yZoom\0style\0"
    "waveColors\0background\0updateInterval"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcScope[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       9,   20, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Int, 0x00095103,
       4, QMetaType::Float, 0x00095103,
       5, QMetaType::Float, 0x00095103,
       6, QMetaType::Float, 0x00095103,
       7, QMetaType::Float, 0x00095103,
       8, QMetaType::Int, 0x00095103,
       9, QMetaType::QVariantList, 0x00095103,
      10, QMetaType::QColor, 0x00095103,
      11, QMetaType::Int, 0x00095103,

       0        // eod
};

void QcScope::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcScope *_t = static_cast<QcScope *>(_o);
        switch (_id) {
        case 0: _t->updateScope(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject QcScope::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcScope.data,
      qt_meta_data_QcScope,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcScope::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcScope::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcScope.stringdata))
        return static_cast<void*>(const_cast< QcScope*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcScope*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcScope::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 1: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 2: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 3: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 4: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 5: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 6: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = background(); break;
        case 8: *reinterpret_cast< int*>(_v) = updateInterval(); break;
        default: break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBufferNumber(*reinterpret_cast< int*>(_v)); break;
        case 1: setXOffset(*reinterpret_cast< float*>(_v)); break;
        case 2: setYOffset(*reinterpret_cast< float*>(_v)); break;
        case 3: setXZoom(*reinterpret_cast< float*>(_v)); break;
        case 4: setYZoom(*reinterpret_cast< float*>(_v)); break;
        case 5: setStyle(*reinterpret_cast< int*>(_v)); break;
        case 6: setWaveColors(*reinterpret_cast< QVariantList*>(_v)); break;
        case 7: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 8: setUpdateInterval(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
