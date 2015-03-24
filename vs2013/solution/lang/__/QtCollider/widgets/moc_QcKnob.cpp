/****************************************************************************
** Meta object code from reading C++ file 'QcKnob.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcKnob.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcKnob.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcKnob_t {
    QByteArrayData data[11];
    char stringdata[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcKnob_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcKnob_t qt_meta_stringdata_QcKnob = {
    {
QT_MOC_LITERAL(0, 0, 6), // "QcKnob"
QT_MOC_LITERAL(1, 7, 6), // "action"
QT_MOC_LITERAL(2, 14, 0), // ""
QT_MOC_LITERAL(3, 15, 10), // "shiftScale"
QT_MOC_LITERAL(4, 26, 9), // "ctrlScale"
QT_MOC_LITERAL(5, 36, 8), // "altScale"
QT_MOC_LITERAL(6, 45, 5), // "value"
QT_MOC_LITERAL(7, 51, 4), // "mode"
QT_MOC_LITERAL(8, 56, 4), // "step"
QT_MOC_LITERAL(9, 61, 8), // "centered"
QT_MOC_LITERAL(10, 70, 10) // "focusColor"

    },
    "QcKnob\0action\0\0shiftScale\0ctrlScale\0"
    "altScale\0value\0mode\0step\0centered\0"
    "focusColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcKnob[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       8,   20, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // properties: name, type, flags
       3, QMetaType::Double, 0x00095103,
       4, QMetaType::Double, 0x00095103,
       5, QMetaType::Double, 0x00095103,
       6, QMetaType::Double, 0x00095103,
       7, QMetaType::Int, 0x00095103,
       8, QMetaType::Double, 0x00095103,
       9, QMetaType::Bool, 0x00095103,
      10, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcKnob::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcKnob *_t = static_cast<QcKnob *>(_o);
        switch (_id) {
        case 0: _t->action(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcKnob::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcKnob::action)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject QcKnob::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcKnob.data,
      qt_meta_data_QcKnob,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcKnob::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcKnob::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcKnob.stringdata))
        return static_cast<void*>(const_cast< QcKnob*>(this));
    if (!strcmp(_clname, "QcAbstractStepValue"))
        return static_cast< QcAbstractStepValue*>(const_cast< QcKnob*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcKnob*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcKnob::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 0: *reinterpret_cast< double*>(_v) = shiftScale(); break;
        case 1: *reinterpret_cast< double*>(_v) = ctrlScale(); break;
        case 2: *reinterpret_cast< double*>(_v) = altScale(); break;
        case 3: *reinterpret_cast< double*>(_v) = value(); break;
        case 4: *reinterpret_cast< int*>(_v) = mode(); break;
        case 5: *reinterpret_cast< double*>(_v) = step(); break;
        case 6: *reinterpret_cast< bool*>(_v) = centered(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = focusColor(); break;
        default: break;
        }
        _id -= 8;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setShiftScale(*reinterpret_cast< double*>(_v)); break;
        case 1: setCtrlScale(*reinterpret_cast< double*>(_v)); break;
        case 2: setAltScale(*reinterpret_cast< double*>(_v)); break;
        case 3: setValue(*reinterpret_cast< double*>(_v)); break;
        case 4: setMode(*reinterpret_cast< int*>(_v)); break;
        case 5: setStep(*reinterpret_cast< double*>(_v)); break;
        case 6: setCentered(*reinterpret_cast< bool*>(_v)); break;
        case 7: setFocusColor(*reinterpret_cast< QColor*>(_v)); break;
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

// SIGNAL 0
void QcKnob::action()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
