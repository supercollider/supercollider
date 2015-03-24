/****************************************************************************
** Meta object code from reading C++ file 'BasicWidgets.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/BasicWidgets.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BasicWidgets.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcSimpleWidget_t {
    QByteArrayData data[10];
    char stringdata[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcSimpleWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcSimpleWidget_t qt_meta_stringdata_QcSimpleWidget = {
    {
QT_MOC_LITERAL(0, 0, 14), // "QcSimpleWidget"
QT_MOC_LITERAL(1, 15, 18), // "setBackgroundImage"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 23), // "QtCollider::SharedImage"
QT_MOC_LITERAL(4, 59, 5), // "image"
QT_MOC_LITERAL(5, 65, 4), // "rect"
QT_MOC_LITERAL(6, 70, 8), // "tileMode"
QT_MOC_LITERAL(7, 79, 7), // "opacity"
QT_MOC_LITERAL(8, 87, 21), // "removeBackgroundImage"
QT_MOC_LITERAL(9, 109, 10) // "background"

    },
    "QcSimpleWidget\0setBackgroundImage\0\0"
    "QtCollider::SharedImage\0image\0rect\0"
    "tileMode\0opacity\0removeBackgroundImage\0"
    "background"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcSimpleWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       1,   34, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    4,   24,    2, 0x02 /* Public */,
       8,    0,   33,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::QRectF, QMetaType::Int, QMetaType::Double,    4,    5,    6,    7,
    QMetaType::Void,

 // properties: name, type, flags
       9, QMetaType::QColor, 0x00095103,

       0        // eod
};

void QcSimpleWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcSimpleWidget *_t = static_cast<QcSimpleWidget *>(_o);
        switch (_id) {
        case 0: _t->setBackgroundImage((*reinterpret_cast< const QtCollider::SharedImage(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 1: _t->removeBackgroundImage(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QtCollider::SharedImage >(); break;
            }
            break;
        }
    }
}

const QMetaObject QcSimpleWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcSimpleWidget.data,
      qt_meta_data_QcSimpleWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcSimpleWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcSimpleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcSimpleWidget.stringdata))
        return static_cast<void*>(const_cast< QcSimpleWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcSimpleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = background(); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_QcDefaultWidget_t {
    QByteArrayData data[1];
    char stringdata[16];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcDefaultWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcDefaultWidget_t qt_meta_stringdata_QcDefaultWidget = {
    {
QT_MOC_LITERAL(0, 0, 15) // "QcDefaultWidget"

    },
    "QcDefaultWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcDefaultWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void QcDefaultWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject QcDefaultWidget::staticMetaObject = {
    { &QcSimpleWidget::staticMetaObject, qt_meta_stringdata_QcDefaultWidget.data,
      qt_meta_data_QcDefaultWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcDefaultWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcDefaultWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcDefaultWidget.stringdata))
        return static_cast<void*>(const_cast< QcDefaultWidget*>(this));
    return QcSimpleWidget::qt_metacast(_clname);
}

int QcDefaultWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcSimpleWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_QcHLayoutWidget_t {
    QByteArrayData data[1];
    char stringdata[16];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcHLayoutWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcHLayoutWidget_t qt_meta_stringdata_QcHLayoutWidget = {
    {
QT_MOC_LITERAL(0, 0, 15) // "QcHLayoutWidget"

    },
    "QcHLayoutWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcHLayoutWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void QcHLayoutWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject QcHLayoutWidget::staticMetaObject = {
    { &QcSimpleWidget::staticMetaObject, qt_meta_stringdata_QcHLayoutWidget.data,
      qt_meta_data_QcHLayoutWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcHLayoutWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcHLayoutWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcHLayoutWidget.stringdata))
        return static_cast<void*>(const_cast< QcHLayoutWidget*>(this));
    return QcSimpleWidget::qt_metacast(_clname);
}

int QcHLayoutWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcSimpleWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_QcVLayoutWidget_t {
    QByteArrayData data[1];
    char stringdata[16];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcVLayoutWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcVLayoutWidget_t qt_meta_stringdata_QcVLayoutWidget = {
    {
QT_MOC_LITERAL(0, 0, 15) // "QcVLayoutWidget"

    },
    "QcVLayoutWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcVLayoutWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void QcVLayoutWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject QcVLayoutWidget::staticMetaObject = {
    { &QcSimpleWidget::staticMetaObject, qt_meta_stringdata_QcVLayoutWidget.data,
      qt_meta_data_QcVLayoutWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcVLayoutWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcVLayoutWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcVLayoutWidget.stringdata))
        return static_cast<void*>(const_cast< QcVLayoutWidget*>(this));
    return QcSimpleWidget::qt_metacast(_clname);
}

int QcVLayoutWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcSimpleWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_QcCustomPainted_t {
    QByteArrayData data[1];
    char stringdata[16];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcCustomPainted_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcCustomPainted_t qt_meta_stringdata_QcCustomPainted = {
    {
QT_MOC_LITERAL(0, 0, 15) // "QcCustomPainted"

    },
    "QcCustomPainted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcCustomPainted[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void QcCustomPainted::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject QcCustomPainted::staticMetaObject = {
    { &QcCanvas::staticMetaObject, qt_meta_stringdata_QcCustomPainted.data,
      qt_meta_data_QcCustomPainted,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcCustomPainted::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcCustomPainted::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcCustomPainted.stringdata))
        return static_cast<void*>(const_cast< QcCustomPainted*>(this));
    return QcCanvas::qt_metacast(_clname);
}

int QcCustomPainted::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcCanvas::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
