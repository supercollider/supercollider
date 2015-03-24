/****************************************************************************
** Meta object code from reading C++ file 'QcScopeShm.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcScopeShm.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcScopeShm.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcScopeShm_t {
    QByteArrayData data[16];
    char stringdata[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcScopeShm_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcScopeShm_t qt_meta_stringdata_QcScopeShm = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QcScopeShm"
QT_MOC_LITERAL(1, 11, 5), // "start"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 4), // "stop"
QT_MOC_LITERAL(4, 23, 11), // "updateScope"
QT_MOC_LITERAL(5, 35, 10), // "serverPort"
QT_MOC_LITERAL(6, 46, 12), // "bufferNumber"
QT_MOC_LITERAL(7, 59, 7), // "xOffset"
QT_MOC_LITERAL(8, 67, 7), // "yOffset"
QT_MOC_LITERAL(9, 75, 5), // "xZoom"
QT_MOC_LITERAL(10, 81, 5), // "yZoom"
QT_MOC_LITERAL(11, 87, 5), // "style"
QT_MOC_LITERAL(12, 93, 10), // "waveColors"
QT_MOC_LITERAL(13, 104, 10), // "background"
QT_MOC_LITERAL(14, 115, 14), // "updateInterval"
QT_MOC_LITERAL(15, 130, 7) // "running"

    },
    "QcScopeShm\0start\0\0stop\0updateScope\0"
    "serverPort\0bufferNumber\0xOffset\0yOffset\0"
    "xZoom\0yZoom\0style\0waveColors\0background\0"
    "updateInterval\0running"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcScopeShm[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
      11,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x0a /* Public */,
       3,    0,   30,    2, 0x0a /* Public */,
       4,    0,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00095103,
       6, QMetaType::Int, 0x00095103,
       7, QMetaType::Float, 0x00095103,
       8, QMetaType::Float, 0x00095103,
       9, QMetaType::Float, 0x00095103,
      10, QMetaType::Float, 0x00095103,
      11, QMetaType::Int, 0x00095103,
      12, QMetaType::QVariantList, 0x00095103,
      13, QMetaType::QColor, 0x00095103,
      14, QMetaType::Int, 0x00095103,
      15, QMetaType::Bool, 0x00095001,

       0        // eod
};

void QcScopeShm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcScopeShm *_t = static_cast<QcScopeShm *>(_o);
        switch (_id) {
        case 0: _t->start(); break;
        case 1: _t->stop(); break;
        case 2: _t->updateScope(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject QcScopeShm::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcScopeShm.data,
      qt_meta_data_QcScopeShm,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcScopeShm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcScopeShm::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcScopeShm.stringdata))
        return static_cast<void*>(const_cast< QcScopeShm*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcScopeShm*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcScopeShm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = serverPort(); break;
        case 1: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 2: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 3: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 4: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 5: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 6: *reinterpret_cast< int*>(_v) = style(); break;
        case 7: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = background(); break;
        case 9: *reinterpret_cast< int*>(_v) = updateInterval(); break;
        case 10: *reinterpret_cast< bool*>(_v) = running(); break;
        default: break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setServerPort(*reinterpret_cast< int*>(_v)); break;
        case 1: setBufferNumber(*reinterpret_cast< int*>(_v)); break;
        case 2: setXOffset(*reinterpret_cast< float*>(_v)); break;
        case 3: setYOffset(*reinterpret_cast< float*>(_v)); break;
        case 4: setXZoom(*reinterpret_cast< float*>(_v)); break;
        case 5: setYZoom(*reinterpret_cast< float*>(_v)); break;
        case 6: setStyle(*reinterpret_cast< int*>(_v)); break;
        case 7: setWaveColors(*reinterpret_cast< QVariantList*>(_v)); break;
        case 8: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 9: setUpdateInterval(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
