/****************************************************************************
** Meta object code from reading C++ file 'QcScrollArea.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcScrollArea.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcScrollArea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcScrollWidget_t {
    QByteArrayData data[1];
    char stringdata[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcScrollWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcScrollWidget_t qt_meta_stringdata_QcScrollWidget = {
    {
QT_MOC_LITERAL(0, 0, 14) // "QcScrollWidget"

    },
    "QcScrollWidget"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcScrollWidget[] = {

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

void QcScrollWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject QcScrollWidget::staticMetaObject = {
    { &QcCanvas::staticMetaObject, qt_meta_stringdata_QcScrollWidget.data,
      qt_meta_data_QcScrollWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcScrollWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcScrollWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcScrollWidget.stringdata))
        return static_cast<void*>(const_cast< QcScrollWidget*>(this));
    return QcCanvas::qt_metacast(_clname);
}

int QcScrollWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcCanvas::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_QcScrollArea_t {
    QByteArrayData data[11];
    char stringdata[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcScrollArea_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcScrollArea_t qt_meta_stringdata_QcScrollArea = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcScrollArea"
QT_MOC_LITERAL(1, 13, 8), // "scrolled"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 9), // "setWidget"
QT_MOC_LITERAL(4, 33, 13), // "QObjectProxy*"
QT_MOC_LITERAL(5, 47, 8), // "addChild"
QT_MOC_LITERAL(6, 56, 8), // "QWidget*"
QT_MOC_LITERAL(7, 65, 1), // "w"
QT_MOC_LITERAL(8, 67, 9), // "hasBorder"
QT_MOC_LITERAL(9, 77, 11), // "innerBounds"
QT_MOC_LITERAL(10, 89, 13) // "visibleOrigin"

    },
    "QcScrollArea\0scrolled\0\0setWidget\0"
    "QObjectProxy*\0addChild\0QWidget*\0w\0"
    "hasBorder\0innerBounds\0visibleOrigin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcScrollArea[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       3,   36, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       3,    1,   30,    2, 0x02 /* Public */,
       5,    1,   33,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void, 0x80000000 | 6,    7,

 // properties: name, type, flags
       8, QMetaType::Bool, 0x00095103,
       9, QMetaType::QRectF, 0x00095001,
      10, QMetaType::QPointF, 0x00095103,

       0        // eod
};

void QcScrollArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcScrollArea *_t = static_cast<QcScrollArea *>(_o);
        switch (_id) {
        case 0: _t->scrolled(); break;
        case 1: _t->setWidget((*reinterpret_cast< QObjectProxy*(*)>(_a[1]))); break;
        case 2: _t->addChild((*reinterpret_cast< QWidget*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcScrollArea::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcScrollArea::scrolled)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject QcScrollArea::staticMetaObject = {
    { &QScrollArea::staticMetaObject, qt_meta_stringdata_QcScrollArea.data,
      qt_meta_data_QcScrollArea,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcScrollArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcScrollArea::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcScrollArea.stringdata))
        return static_cast<void*>(const_cast< QcScrollArea*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcScrollArea*>(this));
    return QScrollArea::qt_metacast(_clname);
}

int QcScrollArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScrollArea::qt_metacall(_c, _id, _a);
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
        case 0: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 1: *reinterpret_cast< QRectF*>(_v) = innerBounds(); break;
        case 2: *reinterpret_cast< QPointF*>(_v) = visibleOrigin(); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setHasBorder(*reinterpret_cast< bool*>(_v)); break;
        case 2: setVisibleOrigin(*reinterpret_cast< QPointF*>(_v)); break;
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

// SIGNAL 0
void QcScrollArea::scrolled()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
