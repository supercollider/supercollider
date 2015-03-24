/****************************************************************************
** Meta object code from reading C++ file 'QcMultiSlider.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcMultiSlider.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcMultiSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcMultiSlider_t {
    QByteArrayData data[31];
    char stringdata[311];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcMultiSlider_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcMultiSlider_t qt_meta_stringdata_QcMultiSlider = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QcMultiSlider"
QT_MOC_LITERAL(1, 14, 8), // "modified"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "interacted"
QT_MOC_LITERAL(4, 35, 6), // "action"
QT_MOC_LITERAL(5, 42, 10), // "metaAction"
QT_MOC_LITERAL(6, 53, 8), // "doAction"
QT_MOC_LITERAL(7, 62, 11), // "sliderCount"
QT_MOC_LITERAL(8, 74, 6), // "values"
QT_MOC_LITERAL(9, 81, 15), // "QVector<double>"
QT_MOC_LITERAL(10, 97, 9), // "reference"
QT_MOC_LITERAL(11, 107, 5), // "value"
QT_MOC_LITERAL(12, 113, 4), // "step"
QT_MOC_LITERAL(13, 118, 5), // "index"
QT_MOC_LITERAL(14, 124, 13), // "selectionSize"
QT_MOC_LITERAL(15, 138, 11), // "orientation"
QT_MOC_LITERAL(16, 150, 15), // "Qt::Orientation"
QT_MOC_LITERAL(17, 166, 7), // "elastic"
QT_MOC_LITERAL(18, 174, 14), // "indexThumbSize"
QT_MOC_LITERAL(19, 189, 14), // "valueThumbSize"
QT_MOC_LITERAL(20, 204, 3), // "gap"
QT_MOC_LITERAL(21, 208, 9), // "drawLines"
QT_MOC_LITERAL(22, 218, 9), // "drawRects"
QT_MOC_LITERAL(23, 228, 8), // "isFilled"
QT_MOC_LITERAL(24, 237, 9), // "highlight"
QT_MOC_LITERAL(25, 247, 8), // "editable"
QT_MOC_LITERAL(26, 256, 10), // "startIndex"
QT_MOC_LITERAL(27, 267, 10), // "background"
QT_MOC_LITERAL(28, 278, 9), // "fillColor"
QT_MOC_LITERAL(29, 288, 11), // "strokeColor"
QT_MOC_LITERAL(30, 300, 10) // "focusColor"

    },
    "QcMultiSlider\0modified\0\0interacted\0"
    "action\0metaAction\0doAction\0sliderCount\0"
    "values\0QVector<double>\0reference\0value\0"
    "step\0index\0selectionSize\0orientation\0"
    "Qt::Orientation\0elastic\0indexThumbSize\0"
    "valueThumbSize\0gap\0drawLines\0drawRects\0"
    "isFilled\0highlight\0editable\0startIndex\0"
    "background\0fillColor\0strokeColor\0"
    "focusColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcMultiSlider[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
      22,   44, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x06 /* Public */,
       3,    0,   40,    2, 0x06 /* Public */,
       4,    0,   41,    2, 0x06 /* Public */,
       5,    0,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   43,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::Int, 0x00095103,
       8, 0x80000000 | 9, 0x0009510b,
      10, 0x80000000 | 9, 0x0009510b,
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Int, 0x00095103,
      14, QMetaType::Int, 0x00095103,
      15, 0x80000000 | 16, 0x0009510b,
      17, QMetaType::Bool, 0x00095103,
      18, QMetaType::Int, 0x00095103,
      19, QMetaType::Float, 0x00095103,
      20, QMetaType::Int, 0x00095103,
      21, QMetaType::Bool, 0x00095103,
      22, QMetaType::Bool, 0x00095103,
      23, QMetaType::Bool, 0x00095103,
      24, QMetaType::Bool, 0x00095103,
      25, QMetaType::Bool, 0x00095103,
      26, QMetaType::Int, 0x00095103,
      27, QMetaType::QColor, 0x00095103,
      28, QMetaType::QColor, 0x00095103,
      29, QMetaType::QColor, 0x00095103,
      30, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcMultiSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcMultiSlider *_t = static_cast<QcMultiSlider *>(_o);
        switch (_id) {
        case 0: _t->modified(); break;
        case 1: _t->interacted(); break;
        case 2: _t->action(); break;
        case 3: _t->metaAction(); break;
        case 4: _t->doAction(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcMultiSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcMultiSlider::modified)) {
                *result = 0;
            }
        }
        {
            typedef void (QcMultiSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcMultiSlider::interacted)) {
                *result = 1;
            }
        }
        {
            typedef void (QcMultiSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcMultiSlider::action)) {
                *result = 2;
            }
        }
        {
            typedef void (QcMultiSlider::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcMultiSlider::metaAction)) {
                *result = 3;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
        case 1:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
        }
    }

}

const QMetaObject QcMultiSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcMultiSlider.data,
      qt_meta_data_QcMultiSlider,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcMultiSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcMultiSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcMultiSlider.stringdata))
        return static_cast<void*>(const_cast< QcMultiSlider*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcMultiSlider*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcMultiSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcMultiSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = sliderCount(); break;
        case 1: *reinterpret_cast< QVector<double>*>(_v) = values(); break;
        case 2: *reinterpret_cast< QVector<double>*>(_v) = reference(); break;
        case 3: *reinterpret_cast< double*>(_v) = value(); break;
        case 4: *reinterpret_cast< double*>(_v) = step(); break;
        case 5: *reinterpret_cast< int*>(_v) = index(); break;
        case 6: *reinterpret_cast< int*>(_v) = selectionSize(); break;
        case 7: *reinterpret_cast< Qt::Orientation*>(_v) = orientation(); break;
        case 8: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 9: *reinterpret_cast< int*>(_v) = dummyFloat(); break;
        case 10: *reinterpret_cast< float*>(_v) = dummyFloat(); break;
        case 11: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 12: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 13: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 14: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 15: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 16: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 17: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 18: *reinterpret_cast< QColor*>(_v) = background(); break;
        case 19: *reinterpret_cast< QColor*>(_v) = fillColor(); break;
        case 20: *reinterpret_cast< QColor*>(_v) = strokeColor(); break;
        case 21: *reinterpret_cast< QColor*>(_v) = focusColor(); break;
        default: break;
        }
        _id -= 22;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSliderCount(*reinterpret_cast< int*>(_v)); break;
        case 1: setValues(*reinterpret_cast< QVector<double>*>(_v)); break;
        case 2: setReference(*reinterpret_cast< QVector<double>*>(_v)); break;
        case 3: setValue(*reinterpret_cast< double*>(_v)); break;
        case 4: setStep(*reinterpret_cast< double*>(_v)); break;
        case 5: setIndex(*reinterpret_cast< int*>(_v)); break;
        case 6: setSelectionSize(*reinterpret_cast< int*>(_v)); break;
        case 7: setOrientation(*reinterpret_cast< Qt::Orientation*>(_v)); break;
        case 8: setElastic(*reinterpret_cast< bool*>(_v)); break;
        case 9: setIndexThumbSize(*reinterpret_cast< int*>(_v)); break;
        case 10: setValueThumbSize(*reinterpret_cast< float*>(_v)); break;
        case 11: setGap(*reinterpret_cast< int*>(_v)); break;
        case 12: setDrawLines(*reinterpret_cast< bool*>(_v)); break;
        case 13: setDrawRects(*reinterpret_cast< bool*>(_v)); break;
        case 14: setIsFilled(*reinterpret_cast< bool*>(_v)); break;
        case 15: setHighlight(*reinterpret_cast< bool*>(_v)); break;
        case 16: setEditable(*reinterpret_cast< bool*>(_v)); break;
        case 17: setStartIndex(*reinterpret_cast< int*>(_v)); break;
        case 18: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 19: setFillColor(*reinterpret_cast< QColor*>(_v)); break;
        case 20: setStrokeColor(*reinterpret_cast< QColor*>(_v)); break;
        case 21: setFocusColor(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
        _id -= 22;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 22;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 22;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 22;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 22;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 22;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 22;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcMultiSlider::modified()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QcMultiSlider::interacted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void QcMultiSlider::action()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void QcMultiSlider::metaAction()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
