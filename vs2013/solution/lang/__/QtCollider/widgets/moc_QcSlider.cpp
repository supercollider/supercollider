/****************************************************************************
** Meta object code from reading C++ file 'QcSlider.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcSlider.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcSlider_t {
    QByteArrayData data[18];
    char stringdata[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcSlider_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcSlider_t qt_meta_stringdata_QcSlider = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QcSlider"
QT_MOC_LITERAL(1, 9, 6), // "action"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 9), // "preAction"
QT_MOC_LITERAL(4, 27, 9), // "increment"
QT_MOC_LITERAL(5, 37, 6), // "factor"
QT_MOC_LITERAL(6, 44, 9), // "decrement"
QT_MOC_LITERAL(7, 54, 10), // "shiftScale"
QT_MOC_LITERAL(8, 65, 9), // "ctrlScale"
QT_MOC_LITERAL(9, 75, 8), // "altScale"
QT_MOC_LITERAL(10, 84, 4), // "step"
QT_MOC_LITERAL(11, 89, 9), // "pixelStep"
QT_MOC_LITERAL(12, 99, 5), // "value"
QT_MOC_LITERAL(13, 105, 11), // "orientation"
QT_MOC_LITERAL(14, 117, 12), // "handleLength"
QT_MOC_LITERAL(15, 130, 11), // "grooveColor"
QT_MOC_LITERAL(16, 142, 10), // "focusColor"
QT_MOC_LITERAL(17, 153, 9) // "knobColor"

    },
    "QcSlider\0action\0\0preAction\0increment\0"
    "factor\0decrement\0shiftScale\0ctrlScale\0"
    "altScale\0step\0pixelStep\0value\0orientation\0"
    "handleLength\0grooveColor\0focusColor\0"
    "knobColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcSlider[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
      11,   44, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    1,   35,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   38,    2, 0x0a /* Public */,
       6,    1,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void, QMetaType::Double,    5,

 // properties: name, type, flags
       7, QMetaType::Double, 0x00095103,
       8, QMetaType::Double, 0x00095103,
       9, QMetaType::Double, 0x00095103,
      10, QMetaType::Double, 0x00095103,
      11, QMetaType::Double, 0x00095001,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Int, 0x00095103,
      14, QMetaType::Int, 0x00095103,
      15, QMetaType::QColor, 0x00095103,
      16, QMetaType::QColor, 0x00095103,
      17, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcSlider *_t = static_cast<QcSlider *>(_o);
        switch (_id) {
        case 0: _t->action(); break;
        case 1: _t->preAction((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->increment((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->decrement((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcSlider::action)) {
                *result = 0;
            }
        }
        {
            typedef void (QcSlider::*_t)(double );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcSlider::preAction)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QcSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcSlider.data,
      qt_meta_data_QcSlider,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcSlider.stringdata))
        return static_cast<void*>(const_cast< QcSlider*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcSlider*>(this));
    if (!strcmp(_clname, "QcAbstractStepValue"))
        return static_cast< QcAbstractStepValue*>(const_cast< QcSlider*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 1: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 2: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 3: *reinterpret_cast< double*>(_v) = step(); break;
        case 4: *reinterpret_cast< double*>(_v) = pixelStep(); break;
        case 5: *reinterpret_cast< double*>(_v) = value(); break;
        case 6: *reinterpret_cast< int*>(_v) = orientation(); break;
        case 7: *reinterpret_cast< int*>(_v) = handleLength(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = grooveColor(); break;
        case 9: *reinterpret_cast< QColor*>(_v) = focusColor(); break;
        case 10: *reinterpret_cast< QColor*>(_v) = knobColor(); break;
        default: break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setShiftScale(*reinterpret_cast< double*>(_v)); break;
        case 1: setCtrlScale(*reinterpret_cast< double*>(_v)); break;
        case 2: setAltScale(*reinterpret_cast< double*>(_v)); break;
        case 3: setStep(*reinterpret_cast< double*>(_v)); break;
        case 5: setValue(*reinterpret_cast< double*>(_v)); break;
        case 6: setOrientation(*reinterpret_cast< int*>(_v)); break;
        case 7: setHandleLength(*reinterpret_cast< int*>(_v)); break;
        case 8: setGrooveColor(*reinterpret_cast< QColor*>(_v)); break;
        case 9: setFocusColor(*reinterpret_cast< QColor*>(_v)); break;
        case 10: setKnobColor(*reinterpret_cast< QColor*>(_v)); break;
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

// SIGNAL 0
void QcSlider::action()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QcSlider::preAction(double _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
