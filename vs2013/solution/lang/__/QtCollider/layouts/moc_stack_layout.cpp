/****************************************************************************
** Meta object code from reading C++ file 'stack_layout.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/layouts/stack_layout.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'stack_layout.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QtCollider__StackLayout_t {
    QByteArrayData data[7];
    char stringdata[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtCollider__StackLayout_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtCollider__StackLayout_t qt_meta_stringdata_QtCollider__StackLayout = {
    {
QT_MOC_LITERAL(0, 0, 23), // "QtCollider::StackLayout"
QT_MOC_LITERAL(1, 24, 12), // "currentIndex"
QT_MOC_LITERAL(2, 37, 12), // "stackingMode"
QT_MOC_LITERAL(3, 50, 12), // "StackingMode"
QT_MOC_LITERAL(4, 63, 5), // "count"
QT_MOC_LITERAL(5, 69, 8), // "StackOne"
QT_MOC_LITERAL(6, 78, 8) // "StackAll"

    },
    "QtCollider::StackLayout\0currentIndex\0"
    "stackingMode\0StackingMode\0count\0"
    "StackOne\0StackAll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtCollider__StackLayout[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   14, // properties
       1,   23, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x00095103,
       2, 0x80000000 | 3, 0x0009510b,
       4, QMetaType::Int, 0x00095001,

 // enums: name, flags, count, data
       3, 0x0,    2,   27,

 // enum data: key, value
       5, uint(QtCollider::StackLayout::StackOne),
       6, uint(QtCollider::StackLayout::StackAll),

       0        // eod
};

void QtCollider::StackLayout::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject QtCollider::StackLayout::staticMetaObject = {
    { &QLayout::staticMetaObject, qt_meta_stringdata_QtCollider__StackLayout.data,
      qt_meta_data_QtCollider__StackLayout,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QtCollider::StackLayout::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtCollider::StackLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QtCollider__StackLayout.stringdata))
        return static_cast<void*>(const_cast< StackLayout*>(this));
    return QLayout::qt_metacast(_clname);
}

int QtCollider::StackLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLayout::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
     if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = currentIndex(); break;
        case 1: *reinterpret_cast< StackingMode*>(_v) = stackingMode(); break;
        case 2: *reinterpret_cast< int*>(_v) = count(); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setCurrentIndex(*reinterpret_cast< int*>(_v)); break;
        case 1: setStackingMode(*reinterpret_cast< StackingMode*>(_v)); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
