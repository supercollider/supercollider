/****************************************************************************
** Meta object code from reading C++ file 'QcCanvas.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcCanvas.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcCanvas.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcCanvas_t {
    QByteArrayData data[20];
    char stringdata[213];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcCanvas_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcCanvas_t qt_meta_stringdata_QcCanvas = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QcCanvas"
QT_MOC_LITERAL(1, 9, 8), // "painting"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 9), // "QPainter*"
QT_MOC_LITERAL(4, 29, 7), // "refresh"
QT_MOC_LITERAL(5, 37, 5), // "clear"
QT_MOC_LITERAL(6, 43, 7), // "animate"
QT_MOC_LITERAL(7, 51, 6), // "toggle"
QT_MOC_LITERAL(8, 58, 18), // "setBackgroundImage"
QT_MOC_LITERAL(9, 77, 23), // "QtCollider::SharedImage"
QT_MOC_LITERAL(10, 101, 5), // "image"
QT_MOC_LITERAL(11, 107, 4), // "rect"
QT_MOC_LITERAL(12, 112, 8), // "tileMode"
QT_MOC_LITERAL(13, 121, 7), // "opacity"
QT_MOC_LITERAL(14, 129, 21), // "removeBackgroundImage"
QT_MOC_LITERAL(15, 151, 14), // "clearOnRefresh"
QT_MOC_LITERAL(16, 166, 14), // "drawingEnabled"
QT_MOC_LITERAL(17, 181, 9), // "frameRate"
QT_MOC_LITERAL(18, 191, 10), // "frameCount"
QT_MOC_LITERAL(19, 202, 10) // "background"

    },
    "QcCanvas\0painting\0\0QPainter*\0refresh\0"
    "clear\0animate\0toggle\0setBackgroundImage\0"
    "QtCollider::SharedImage\0image\0rect\0"
    "tileMode\0opacity\0removeBackgroundImage\0"
    "clearOnRefresh\0drawingEnabled\0frameRate\0"
    "frameCount\0background"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcCanvas[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       5,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   47,    2, 0x0a /* Public */,
       5,    0,   48,    2, 0x0a /* Public */,
       6,    1,   49,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
       8,    4,   52,    2, 0x02 /* Public */,
      14,    0,   61,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    7,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 9, QMetaType::QRectF, QMetaType::Int, QMetaType::Double,   10,   11,   12,   13,
    QMetaType::Void,

 // properties: name, type, flags
      15, QMetaType::Bool, 0x00095103,
      16, QMetaType::Bool, 0x00095103,
      17, QMetaType::Float, 0x00095103,
      18, QMetaType::Int, 0x00095001,
      19, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcCanvas::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcCanvas *_t = static_cast<QcCanvas *>(_o);
        switch (_id) {
        case 0: _t->painting((*reinterpret_cast< QPainter*(*)>(_a[1]))); break;
        case 1: _t->refresh(); break;
        case 2: _t->clear(); break;
        case 3: _t->animate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->setBackgroundImage((*reinterpret_cast< const QtCollider::SharedImage(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 5: _t->removeBackgroundImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QtCollider::SharedImage >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcCanvas::*_t)(QPainter * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcCanvas::painting)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject QcCanvas::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcCanvas.data,
      qt_meta_data_QcCanvas,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcCanvas::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcCanvas::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcCanvas.stringdata))
        return static_cast<void*>(const_cast< QcCanvas*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcCanvas::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = clearOnRefresh(); break;
        case 1: *reinterpret_cast< bool*>(_v) = drawingEnabled(); break;
        case 2: *reinterpret_cast< float*>(_v) = frameRate(); break;
        case 3: *reinterpret_cast< int*>(_v) = frameCount(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = background(); break;
        default: break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setClearOnRefresh(*reinterpret_cast< bool*>(_v)); break;
        case 1: setDrawingEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setFrameRate(*reinterpret_cast< float*>(_v)); break;
        case 4: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
        _id -= 5;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcCanvas::painting(QPainter * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
