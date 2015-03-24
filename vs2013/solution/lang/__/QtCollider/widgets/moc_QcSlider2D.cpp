/****************************************************************************
** Meta object code from reading C++ file 'QcSlider2D.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcSlider2D.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcSlider2D.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcSlider2D_t {
    QByteArrayData data[18];
    char stringdata[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcSlider2D_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcSlider2D_t qt_meta_stringdata_QcSlider2D = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QcSlider2D"
QT_MOC_LITERAL(1, 11, 6), // "action"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 9), // "randomize"
QT_MOC_LITERAL(4, 29, 10), // "incrementX"
QT_MOC_LITERAL(5, 40, 6), // "factor"
QT_MOC_LITERAL(6, 47, 10), // "decrementX"
QT_MOC_LITERAL(7, 58, 10), // "incrementY"
QT_MOC_LITERAL(8, 69, 10), // "decrementY"
QT_MOC_LITERAL(9, 80, 6), // "xValue"
QT_MOC_LITERAL(10, 87, 6), // "yValue"
QT_MOC_LITERAL(11, 94, 10), // "shiftScale"
QT_MOC_LITERAL(12, 105, 9), // "ctrlScale"
QT_MOC_LITERAL(13, 115, 8), // "altScale"
QT_MOC_LITERAL(14, 124, 4), // "step"
QT_MOC_LITERAL(15, 129, 11), // "grooveColor"
QT_MOC_LITERAL(16, 141, 10), // "focusColor"
QT_MOC_LITERAL(17, 152, 9) // "knobColor"

    },
    "QcSlider2D\0action\0\0randomize\0incrementX\0"
    "factor\0decrementX\0incrementY\0decrementY\0"
    "xValue\0yValue\0shiftScale\0ctrlScale\0"
    "altScale\0step\0grooveColor\0focusColor\0"
    "knobColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcSlider2D[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       9,   82, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x06 /* Public */,
       3,    0,   65,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   66,    2, 0x0a /* Public */,
       4,    0,   69,    2, 0x2a /* Public | MethodCloned */,
       6,    1,   70,    2, 0x0a /* Public */,
       6,    0,   73,    2, 0x2a /* Public | MethodCloned */,
       7,    1,   74,    2, 0x0a /* Public */,
       7,    0,   77,    2, 0x2a /* Public | MethodCloned */,
       8,    1,   78,    2, 0x0a /* Public */,
       8,    0,   81,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    5,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::Double, 0x00095103,
      10, QMetaType::Double, 0x00095103,
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Double, 0x00095103,
      14, QMetaType::Double, 0x00095103,
      15, QMetaType::QColor, 0x00095103,
      16, QMetaType::QColor, 0x00095103,
      17, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcSlider2D::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcSlider2D *_t = static_cast<QcSlider2D *>(_o);
        switch (_id) {
        case 0: _t->action(); break;
        case 1: _t->randomize(); break;
        case 2: _t->incrementX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: _t->incrementX(); break;
        case 4: _t->decrementX((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->decrementX(); break;
        case 6: _t->incrementY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: _t->incrementY(); break;
        case 8: _t->decrementY((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->decrementY(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcSlider2D::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcSlider2D::action)) {
                *result = 0;
            }
        }
        {
            typedef void (QcSlider2D::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcSlider2D::randomize)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QcSlider2D::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcSlider2D.data,
      qt_meta_data_QcSlider2D,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcSlider2D::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcSlider2D::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcSlider2D.stringdata))
        return static_cast<void*>(const_cast< QcSlider2D*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcSlider2D*>(this));
    if (!strcmp(_clname, "QcAbstractStepValue"))
        return static_cast< QcAbstractStepValue*>(const_cast< QcSlider2D*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcSlider2D*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcSlider2D::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = xValue(); break;
        case 1: *reinterpret_cast< double*>(_v) = yValue(); break;
        case 2: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 3: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 4: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 5: *reinterpret_cast< double*>(_v) = dummyFloat(); break;
        case 6: *reinterpret_cast< QColor*>(_v) = grooveColor(); break;
        case 7: *reinterpret_cast< QColor*>(_v) = focusColor(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = knobColor(); break;
        default: break;
        }
        _id -= 9;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setXValue(*reinterpret_cast< double*>(_v)); break;
        case 1: setYValue(*reinterpret_cast< double*>(_v)); break;
        case 2: setShiftScale(*reinterpret_cast< double*>(_v)); break;
        case 3: setCtrlScale(*reinterpret_cast< double*>(_v)); break;
        case 4: setAltScale(*reinterpret_cast< double*>(_v)); break;
        case 5: setStep(*reinterpret_cast< double*>(_v)); break;
        case 6: setGrooveColor(*reinterpret_cast< QColor*>(_v)); break;
        case 7: setFocusColor(*reinterpret_cast< QColor*>(_v)); break;
        case 8: setKnobColor(*reinterpret_cast< QColor*>(_v)); break;
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

// SIGNAL 0
void QcSlider2D::action()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QcSlider2D::randomize()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
