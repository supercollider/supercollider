/****************************************************************************
** Meta object code from reading C++ file 'QtDownload.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../QtCollider/QtDownload.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtDownload.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QtDownload_t {
    QByteArrayData data[16];
    char stringdata[175];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtDownload_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtDownload_t qt_meta_stringdata_QtDownload = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QtDownload"
QT_MOC_LITERAL(1, 11, 10), // "doFinished"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 7), // "doError"
QT_MOC_LITERAL(4, 31, 10), // "doProgress"
QT_MOC_LITERAL(5, 42, 16), // "downloadFinished"
QT_MOC_LITERAL(6, 59, 16), // "downloadProgress"
QT_MOC_LITERAL(7, 76, 8), // "recieved"
QT_MOC_LITERAL(8, 85, 5), // "total"
QT_MOC_LITERAL(9, 91, 10), // "replyError"
QT_MOC_LITERAL(10, 102, 27), // "QNetworkReply::NetworkError"
QT_MOC_LITERAL(11, 130, 9), // "errorCode"
QT_MOC_LITERAL(12, 140, 6), // "cancel"
QT_MOC_LITERAL(13, 147, 8), // "download"
QT_MOC_LITERAL(14, 156, 6), // "source"
QT_MOC_LITERAL(15, 163, 11) // "destination"

    },
    "QtDownload\0doFinished\0\0doError\0"
    "doProgress\0downloadFinished\0"
    "downloadProgress\0recieved\0total\0"
    "replyError\0QNetworkReply::NetworkError\0"
    "errorCode\0cancel\0download\0source\0"
    "destination"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtDownload[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       2,   72, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x06 /* Public */,
       3,    0,   55,    2, 0x06 /* Public */,
       4,    2,   56,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   61,    2, 0x0a /* Public */,
       6,    2,   62,    2, 0x0a /* Public */,
       9,    1,   67,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      12,    0,   70,    2, 0x02 /* Public */,
      13,    0,   71,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::LongLong, QMetaType::LongLong,    7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      14, QMetaType::QString, 0x00095103,
      15, QMetaType::QString, 0x00095103,

       0        // eod
};

void QtDownload::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QtDownload *_t = static_cast<QtDownload *>(_o);
        switch (_id) {
        case 0: _t->doFinished(); break;
        case 1: _t->doError(); break;
        case 2: _t->doProgress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->downloadFinished(); break;
        case 4: _t->downloadProgress((*reinterpret_cast< qint64(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2]))); break;
        case 5: _t->replyError((*reinterpret_cast< QNetworkReply::NetworkError(*)>(_a[1]))); break;
        case 6: _t->cancel(); break;
        case 7: _t->download(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QtDownload::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QtDownload::doFinished)) {
                *result = 0;
            }
        }
        {
            typedef void (QtDownload::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QtDownload::doError)) {
                *result = 1;
            }
        }
        {
            typedef void (QtDownload::*_t)(int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QtDownload::doProgress)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject QtDownload::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QtDownload.data,
      qt_meta_data_QtDownload,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QtDownload::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtDownload::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QtDownload.stringdata))
        return static_cast<void*>(const_cast< QtDownload*>(this));
    return QObject::qt_metacast(_clname);
}

int QtDownload::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = source(); break;
        case 1: *reinterpret_cast< QString*>(_v) = destination(); break;
        default: break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setSource(*reinterpret_cast< QString*>(_v)); break;
        case 1: setDestination(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QtDownload::doFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QtDownload::doError()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void QtDownload::doProgress(int _t1, int _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
