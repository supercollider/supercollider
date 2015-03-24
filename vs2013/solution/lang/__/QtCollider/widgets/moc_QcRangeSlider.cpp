/****************************************************************************
** Meta object code from reading C++ file 'QcRangeSlider.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcRangeSlider.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcRangeSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcRangeSlider_t {
    QByteArrayData data[17];
    char stringdata[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcRangeSlider_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcRangeSlider_t qt_meta_stringdata_QcRangeSlider = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QcRangeSlider"
QT_MOC_LITERAL(1, 14, 6), // "action"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 9), // "increment"
QT_MOC_LITERAL(4, 32, 6), // "factor"
QT_MOC_LITERAL(5, 39, 9), // "decrement"
QT_MOC_LITERAL(6, 49, 11), // "orientation"
QT_MOC_LITERAL(7, 61, 15), // "Qt::Orientation"
QT_MOC_LITERAL(8, 77, 7), // "loValue"
QT_MOC_LITERAL(9, 85, 7), // "hiValue"
QT_MOC_LITERAL(10, 93, 10), // "shiftScale"
QT_MOC_LITERAL(11, 104, 9), // "ctrlScale"
QT_MOC_LITERAL(12, 114, 8), // "altScale"
QT_MOC_LITERAL(13, 123, 4), // "step"
QT_MOC_LITERAL(14, 128, 11), // "grooveColor"
QT_MOC_LITERAL(15, 140, 10), // "focusColor"
QT_MOC_LITERAL(16, 151, 9) // "knobColor"

    },
    "QcRangeSlider\0action\0\0increment\0factor\0"
    "decrement\0orientation\0Qt::Orientation\0"
    "loValue\0hiValue\0shiftScale\0ctrlScale\0"
    "altScale\0step\0grooveColor\0focusColor\0"
    "knobColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcRangeSlider[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
      10,   36, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   30,    2, 0x0a /* Public */,
       5,    1,   33,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    4,
    QMetaType::Void, QMetaType::Double,    4,

 // properties: name, type, flags
       6, 0x80000000 | 7, 0x0009510b,
       8, QMetaType::Double, 0x00095103,
       9, QMetaType::Double, 0x00095103,
      10, QMetaType::Double, 0x00095103,
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Double, 0x00095103,
      14, QMetaType::QColor, 0x00095103,
      15, QMetaType::QColor, 0x00095103,
      16, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcRangeSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcRangeSlider *_t = static_cast<QcRangeSlider *>(_o);
        switch (_id) {
        case 0: _t->action(); break;
        case 1: _t->increment((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: _t->decrement((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcRangeSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcRangeSlider::action)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject QcRangeSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcRangeSlider.data,
      qt_meta_data_QcRangeSlider,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcRangeSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcRangeSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcRangeSlider.stringdata))
        return static_cast<void*>(const_cast< QcRangeSlider*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcRangeSlider*>(this));
    if (!strcmp(_clname, "QcAbstractStepValue"))
        return static_cast< QcAbstractStepValue*>(const_cast< QcRangeSlider*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcRangeSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcRangeSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 0: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 1: *reinterpret_cast< double*>(_v) = loValue(); break;
        case 2: *reinterpret_cast< double*>(_v) = hiValue(); break;
        case 3: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 4: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 5: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 6: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = grooveColor(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = focusColor(); break;
        case 9: *reinterpret_cast< QColor*>(_v) = knobColor(); break;
        default: break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 1: setLoValue(*reinterpret_cast< double*>(_v)); break;
        case 2: setHiValue(*reinterpret_cast< double*>(_v)); break;
        case 3: setShiftScale(*reinterpret_cast< double*>(_v)); break;
        case 4: setCtrlScale(*reinterpret_cast< double*>(_v)); break;
        case 5: setAltScale(*reinterpret_cast< double*>(_v)); break;
        case 6: setStep(*reinterpret_cast< double*>(_v)); break;
        case 7: setGrooveColor(*reinterpret_cast< QColor*>(_v)); break;
        case 8: setFocusColor(*reinterpret_cast< QColor*>(_v)); break;
        case 9: setKnobColor(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
        _id -= 10;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 10;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 10;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcRangeSlider::action()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
