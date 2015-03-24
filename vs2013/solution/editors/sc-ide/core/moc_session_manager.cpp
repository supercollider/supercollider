/****************************************************************************
** Meta object code from reading C++ file 'session_manager.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/core/session_manager.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'session_manager.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__SessionManager_t {
    QByteArrayData data[7];
    char stringdata[106];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__SessionManager_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__SessionManager_t qt_meta_stringdata_ScIDE__SessionManager = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ScIDE::SessionManager"
QT_MOC_LITERAL(1, 22, 18), // "saveSessionRequest"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 8), // "Session*"
QT_MOC_LITERAL(4, 51, 7), // "session"
QT_MOC_LITERAL(5, 59, 20), // "switchSessionRequest"
QT_MOC_LITERAL(6, 80, 25) // "currentSessionNameChanged"

    },
    "ScIDE::SessionManager\0saveSessionRequest\0"
    "\0Session*\0session\0switchSessionRequest\0"
    "currentSessionNameChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__SessionManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       5,    1,   32,    2, 0x06 /* Public */,
       6,    0,   35,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

       0        // eod
};

void ScIDE::SessionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SessionManager *_t = static_cast<SessionManager *>(_o);
        switch (_id) {
        case 0: _t->saveSessionRequest((*reinterpret_cast< Session*(*)>(_a[1]))); break;
        case 1: _t->switchSessionRequest((*reinterpret_cast< Session*(*)>(_a[1]))); break;
        case 2: _t->currentSessionNameChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SessionManager::*_t)(Session * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionManager::saveSessionRequest)) {
                *result = 0;
            }
        }
        {
            typedef void (SessionManager::*_t)(Session * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionManager::switchSessionRequest)) {
                *result = 1;
            }
        }
        {
            typedef void (SessionManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SessionManager::currentSessionNameChanged)) {
                *result = 2;
            }
        }
    }
}

const QMetaObject ScIDE::SessionManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__SessionManager.data,
      qt_meta_data_ScIDE__SessionManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::SessionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::SessionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__SessionManager.stringdata))
        return static_cast<void*>(const_cast< SessionManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::SessionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void ScIDE::SessionManager::saveSessionRequest(Session * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::SessionManager::switchSessionRequest(Session * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScIDE::SessionManager::currentSessionNameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
