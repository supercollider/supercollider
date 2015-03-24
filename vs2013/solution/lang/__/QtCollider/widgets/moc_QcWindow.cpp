/****************************************************************************
** Meta object code from reading C++ file 'QcWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcWindow_t {
    QByteArrayData data[6];
    char stringdata[37];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcWindow_t qt_meta_stringdata_QcWindow = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QcWindow"
QT_MOC_LITERAL(1, 9, 0), // ""
QT_MOC_LITERAL(2, 10, 5), // "title"
QT_MOC_LITERAL(3, 16, 4), // "geom"
QT_MOC_LITERAL(4, 21, 9), // "resizable"
QT_MOC_LITERAL(5, 31, 5) // "frame"

    },
    "QcWindow\0\0title\0geom\0resizable\0frame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       1,   23, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, QMetaType::QString, QMetaType::QRectF, QMetaType::Bool, QMetaType::Bool,    2,    3,    4,    5,

 // constructors: name, argc, parameters, tag, flags
       0,    4,   14,    1, 0x0e /* Public */,

       0        // eod
};

void QcWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QcWindow *_r = new QcWindow((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

const QMetaObject QcWindow::staticMetaObject = {
    { &QcCustomPainted::staticMetaObject, qt_meta_stringdata_QcWindow.data,
      qt_meta_data_QcWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcWindow.stringdata))
        return static_cast<void*>(const_cast< QcWindow*>(this));
    return QcCustomPainted::qt_metacast(_clname);
}

int QcWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcCustomPainted::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_QcScrollWindow_t {
    QByteArrayData data[6];
    char stringdata[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcScrollWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcScrollWindow_t qt_meta_stringdata_QcScrollWindow = {
    {
QT_MOC_LITERAL(0, 0, 14), // "QcScrollWindow"
QT_MOC_LITERAL(1, 15, 0), // ""
QT_MOC_LITERAL(2, 16, 5), // "title"
QT_MOC_LITERAL(3, 22, 4), // "geom"
QT_MOC_LITERAL(4, 27, 9), // "resizable"
QT_MOC_LITERAL(5, 37, 5) // "frame"

    },
    "QcScrollWindow\0\0title\0geom\0resizable\0"
    "frame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcScrollWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       1,   23, // constructors
       0,       // flags
       0,       // signalCount

 // constructors: parameters
    0x80000000 | 1, QMetaType::QString, QMetaType::QRectF, QMetaType::Bool, QMetaType::Bool,    2,    3,    4,    5,

 // constructors: name, argc, parameters, tag, flags
       0,    4,   14,    1, 0x0e /* Public */,

       0        // eod
};

void QcScrollWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QcScrollWindow *_r = new QcScrollWindow((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QRectF(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    }
    Q_UNUSED(_o);
}

const QMetaObject QcScrollWindow::staticMetaObject = {
    { &QcScrollArea::staticMetaObject, qt_meta_stringdata_QcScrollWindow.data,
      qt_meta_data_QcScrollWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcScrollWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcScrollWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcScrollWindow.stringdata))
        return static_cast<void*>(const_cast< QcScrollWindow*>(this));
    return QcScrollArea::qt_metacast(_clname);
}

int QcScrollWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
