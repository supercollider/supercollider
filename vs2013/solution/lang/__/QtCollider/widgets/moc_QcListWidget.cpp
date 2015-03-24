/****************************************************************************
** Meta object code from reading C++ file 'QcListWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcListWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcListWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcListWidget_t {
    QByteArrayData data[9];
    char stringdata[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcListWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcListWidget_t qt_meta_stringdata_QcListWidget = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcListWidget"
QT_MOC_LITERAL(1, 13, 6), // "action"
QT_MOC_LITERAL(2, 20, 0), // ""
QT_MOC_LITERAL(3, 21, 13), // "returnPressed"
QT_MOC_LITERAL(4, 35, 20), // "onCurrentItemChanged"
QT_MOC_LITERAL(5, 56, 5), // "items"
QT_MOC_LITERAL(6, 62, 6), // "colors"
QT_MOC_LITERAL(7, 69, 10), // "currentRow"
QT_MOC_LITERAL(8, 80, 9) // "selection"

    },
    "QcListWidget\0action\0\0returnPressed\0"
    "onCurrentItemChanged\0items\0colors\0"
    "currentRow\0selection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcListWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       4,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   31,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::QVariantList, 0x00095103,
       6, QMetaType::QVariantList, 0x00095103,
       7, QMetaType::Int, 0x00095003,
       8, QMetaType::QVariantList, 0x00095103,

       0        // eod
};

void QcListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcListWidget *_t = static_cast<QcListWidget *>(_o);
        switch (_id) {
        case 0: _t->action(); break;
        case 1: _t->returnPressed(); break;
        case 2: _t->onCurrentItemChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcListWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcListWidget::action)) {
                *result = 0;
            }
        }
        {
            typedef void (QcListWidget::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcListWidget::returnPressed)) {
                *result = 1;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject QcListWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_QcListWidget.data,
      qt_meta_data_QcListWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcListWidget.stringdata))
        return static_cast<void*>(const_cast< QcListWidget*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcListWidget*>(this));
    return QListWidget::qt_metacast(_clname);
}

int QcListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 1: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 2: *reinterpret_cast< int*>(_v) = currentRow(); break;
        case 3: *reinterpret_cast< QVariantList*>(_v) = selection(); break;
        default: break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setItems(*reinterpret_cast< QVariantList*>(_v)); break;
        case 1: setColors(*reinterpret_cast< QVariantList*>(_v)); break;
        case 2: setCurrentRowWithoutAction(*reinterpret_cast< int*>(_v)); break;
        case 3: setSelection(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
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
void QcListWidget::action()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QcListWidget::returnPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
