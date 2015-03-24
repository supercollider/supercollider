/****************************************************************************
** Meta object code from reading C++ file 'QcLevelIndicator.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcLevelIndicator.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcLevelIndicator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcLevelIndicator_t {
    QByteArrayData data[11];
    char stringdata[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcLevelIndicator_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcLevelIndicator_t qt_meta_stringdata_QcLevelIndicator = {
    {
QT_MOC_LITERAL(0, 0, 16), // "QcLevelIndicator"
QT_MOC_LITERAL(1, 17, 11), // "clipTimeout"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 5), // "value"
QT_MOC_LITERAL(4, 36, 7), // "warning"
QT_MOC_LITERAL(5, 44, 8), // "critical"
QT_MOC_LITERAL(6, 53, 4), // "peak"
QT_MOC_LITERAL(7, 58, 8), // "drawPeak"
QT_MOC_LITERAL(8, 67, 5), // "ticks"
QT_MOC_LITERAL(9, 73, 10), // "majorTicks"
QT_MOC_LITERAL(10, 84, 11) // "grooveColor"

    },
    "QcLevelIndicator\0clipTimeout\0\0value\0"
    "warning\0critical\0peak\0drawPeak\0ticks\0"
    "majorTicks\0grooveColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcLevelIndicator[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       8,   20, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Float, 0x00095103,
       4, QMetaType::Float, 0x00095103,
       5, QMetaType::Float, 0x00095103,
       6, QMetaType::Float, 0x00095103,
       7, QMetaType::Bool, 0x00095103,
       8, QMetaType::Int, 0x00095103,
       9, QMetaType::Int, 0x00095103,
      10, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcLevelIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcLevelIndicator *_t = static_cast<QcLevelIndicator *>(_o);
        switch (_id) {
        case 0: _t->clipTimeout(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject QcLevelIndicator::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcLevelIndicator.data,
      qt_meta_data_QcLevelIndicator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcLevelIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcLevelIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcLevelIndicator.stringdata))
        return static_cast<void*>(const_cast< QcLevelIndicator*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcLevelIndicator*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcLevelIndicator*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcLevelIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 0: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 1: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 2: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 3: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 4: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 5: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 6: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = grooveColor(); break;
        default: break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setValue(*reinterpret_cast< float*>(_v)); break;
        case 1: setWarning(*reinterpret_cast< float*>(_v)); break;
        case 2: setCritical(*reinterpret_cast< float*>(_v)); break;
        case 3: setPeak(*reinterpret_cast< float*>(_v)); break;
        case 4: setDrawPeak(*reinterpret_cast< bool*>(_v)); break;
        case 5: setTicks(*reinterpret_cast< int*>(_v)); break;
        case 6: setMajorTicks(*reinterpret_cast< int*>(_v)); break;
        case 7: setGrooveColor(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 8;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 8;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
