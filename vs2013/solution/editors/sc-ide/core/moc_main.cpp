/****************************************************************************
** Meta object code from reading C++ file 'main.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/core/main.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__SingleInstanceGuard_t {
    QByteArrayData data[4];
    char stringdata[57];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__SingleInstanceGuard_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__SingleInstanceGuard_t qt_meta_stringdata_ScIDE__SingleInstanceGuard = {
    {
QT_MOC_LITERAL(0, 0, 26), // "ScIDE::SingleInstanceGuard"
QT_MOC_LITERAL(1, 27, 18), // "onNewIpcConnection"
QT_MOC_LITERAL(2, 46, 0), // ""
QT_MOC_LITERAL(3, 47, 9) // "onIpcData"

    },
    "ScIDE::SingleInstanceGuard\0"
    "onNewIpcConnection\0\0onIpcData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__SingleInstanceGuard[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x0a /* Public */,
       3,    0,   25,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::SingleInstanceGuard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SingleInstanceGuard *_t = static_cast<SingleInstanceGuard *>(_o);
        switch (_id) {
        case 0: _t->onNewIpcConnection(); break;
        case 1: _t->onIpcData(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::SingleInstanceGuard::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__SingleInstanceGuard.data,
      qt_meta_data_ScIDE__SingleInstanceGuard,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::SingleInstanceGuard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::SingleInstanceGuard::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__SingleInstanceGuard.stringdata))
        return static_cast<void*>(const_cast< SingleInstanceGuard*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::SingleInstanceGuard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_ScIDE__Main_t {
    QByteArrayData data[8];
    char stringdata[107];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__Main_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__Main_t qt_meta_stringdata_ScIDE__Main = {
    {
QT_MOC_LITERAL(0, 0, 11), // "ScIDE::Main"
QT_MOC_LITERAL(1, 12, 20), // "storeSettingsRequest"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 18), // "Settings::Manager*"
QT_MOC_LITERAL(4, 53, 20), // "applySettingsRequest"
QT_MOC_LITERAL(5, 74, 13), // "storeSettings"
QT_MOC_LITERAL(6, 88, 13), // "applySettings"
QT_MOC_LITERAL(7, 102, 4) // "quit"

    },
    "ScIDE::Main\0storeSettingsRequest\0\0"
    "Settings::Manager*\0applySettingsRequest\0"
    "storeSettings\0applySettings\0quit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__Main[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   45,    2, 0x0a /* Public */,
       6,    0,   46,    2, 0x0a /* Public */,
       7,    0,   47,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::Main::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Main *_t = static_cast<Main *>(_o);
        switch (_id) {
        case 0: _t->storeSettingsRequest((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 1: _t->applySettingsRequest((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 2: _t->storeSettings(); break;
        case 3: _t->applySettings(); break;
        case 4: _t->quit(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Main::*_t)(Settings::Manager * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Main::storeSettingsRequest)) {
                *result = 0;
            }
        }
        {
            typedef void (Main::*_t)(Settings::Manager * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Main::applySettingsRequest)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ScIDE::Main::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__Main.data,
      qt_meta_data_ScIDE__Main,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::Main::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::Main::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__Main.stringdata))
        return static_cast<void*>(const_cast< Main*>(this));
    if (!strcmp(_clname, "QAbstractNativeEventFilter"))
        return static_cast< QAbstractNativeEventFilter*>(const_cast< Main*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::Main::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void ScIDE::Main::storeSettingsRequest(Settings::Manager * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::Main::applySettingsRequest(Settings::Manager * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
