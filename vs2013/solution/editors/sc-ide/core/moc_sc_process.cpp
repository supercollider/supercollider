/****************************************************************************
** Meta object code from reading C++ file 'sc_process.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/core/sc_process.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sc_process.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__ScProcess_t {
    QByteArrayData data[29];
    char stringdata[431];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__ScProcess_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__ScProcess_t qt_meta_stringdata_ScIDE__ScProcess = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ScIDE::ScProcess"
QT_MOC_LITERAL(1, 17, 6), // "scPost"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 13), // "statusMessage"
QT_MOC_LITERAL(4, 39, 8), // "response"
QT_MOC_LITERAL(5, 48, 8), // "selector"
QT_MOC_LITERAL(6, 57, 4), // "data"
QT_MOC_LITERAL(7, 62, 22), // "classLibraryRecompiled"
QT_MOC_LITERAL(8, 85, 24), // "introspectionAboutToSwap"
QT_MOC_LITERAL(9, 110, 13), // "toggleRunning"
QT_MOC_LITERAL(10, 124, 13), // "startLanguage"
QT_MOC_LITERAL(11, 138, 12), // "stopLanguage"
QT_MOC_LITERAL(12, 151, 15), // "restartLanguage"
QT_MOC_LITERAL(13, 167, 21), // "recompileClassLibrary"
QT_MOC_LITERAL(14, 189, 8), // "stopMain"
QT_MOC_LITERAL(15, 198, 12), // "evaluateCode"
QT_MOC_LITERAL(16, 211, 13), // "commandString"
QT_MOC_LITERAL(17, 225, 6), // "silent"
QT_MOC_LITERAL(18, 232, 17), // "swapIntrospection"
QT_MOC_LITERAL(19, 250, 26), // "ScLanguage::Introspection*"
QT_MOC_LITERAL(20, 277, 16), // "newIntrospection"
QT_MOC_LITERAL(21, 294, 18), // "onNewIpcConnection"
QT_MOC_LITERAL(22, 313, 9), // "onIpcData"
QT_MOC_LITERAL(23, 323, 18), // "finalizeConnection"
QT_MOC_LITERAL(24, 342, 21), // "onProcessStateChanged"
QT_MOC_LITERAL(25, 364, 22), // "QProcess::ProcessState"
QT_MOC_LITERAL(26, 387, 5), // "state"
QT_MOC_LITERAL(27, 393, 11), // "onReadyRead"
QT_MOC_LITERAL(28, 405, 25) // "updateToggleRunningAction"

    },
    "ScIDE::ScProcess\0scPost\0\0statusMessage\0"
    "response\0selector\0data\0classLibraryRecompiled\0"
    "introspectionAboutToSwap\0toggleRunning\0"
    "startLanguage\0stopLanguage\0restartLanguage\0"
    "recompileClassLibrary\0stopMain\0"
    "evaluateCode\0commandString\0silent\0"
    "swapIntrospection\0ScLanguage::Introspection*\0"
    "newIntrospection\0onNewIpcConnection\0"
    "onIpcData\0finalizeConnection\0"
    "onProcessStateChanged\0QProcess::ProcessState\0"
    "state\0onReadyRead\0updateToggleRunningAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__ScProcess[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  114,    2, 0x06 /* Public */,
       3,    1,  117,    2, 0x06 /* Public */,
       4,    2,  120,    2, 0x06 /* Public */,
       7,    0,  125,    2, 0x06 /* Public */,
       8,    0,  126,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  127,    2, 0x0a /* Public */,
      10,    0,  128,    2, 0x0a /* Public */,
      11,    0,  129,    2, 0x0a /* Public */,
      12,    0,  130,    2, 0x0a /* Public */,
      13,    0,  131,    2, 0x0a /* Public */,
      14,    0,  132,    2, 0x0a /* Public */,
      15,    2,  133,    2, 0x0a /* Public */,
      15,    1,  138,    2, 0x2a /* Public | MethodCloned */,
      18,    1,  141,    2, 0x08 /* Private */,
      21,    0,  144,    2, 0x08 /* Private */,
      22,    0,  145,    2, 0x08 /* Private */,
      23,    0,  146,    2, 0x08 /* Private */,
      24,    1,  147,    2, 0x08 /* Private */,
      27,    0,  150,    2, 0x08 /* Private */,
      28,    0,  151,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   16,   17,
    QMetaType::Void, QMetaType::QString,   16,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25,   26,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::ScProcess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScProcess *_t = static_cast<ScProcess *>(_o);
        switch (_id) {
        case 0: _t->scPost((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->response((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->classLibraryRecompiled(); break;
        case 4: _t->introspectionAboutToSwap(); break;
        case 5: _t->toggleRunning(); break;
        case 6: _t->startLanguage(); break;
        case 7: _t->stopLanguage(); break;
        case 8: _t->restartLanguage(); break;
        case 9: _t->recompileClassLibrary(); break;
        case 10: _t->stopMain(); break;
        case 11: _t->evaluateCode((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 12: _t->evaluateCode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->swapIntrospection((*reinterpret_cast< ScLanguage::Introspection*(*)>(_a[1]))); break;
        case 14: _t->onNewIpcConnection(); break;
        case 15: _t->onIpcData(); break;
        case 16: _t->finalizeConnection(); break;
        case 17: _t->onProcessStateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1]))); break;
        case 18: _t->onReadyRead(); break;
        case 19: _t->updateToggleRunningAction(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ScProcess::*_t)(QString const & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScProcess::scPost)) {
                *result = 0;
            }
        }
        {
            typedef void (ScProcess::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScProcess::statusMessage)) {
                *result = 1;
            }
        }
        {
            typedef void (ScProcess::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScProcess::response)) {
                *result = 2;
            }
        }
        {
            typedef void (ScProcess::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScProcess::classLibraryRecompiled)) {
                *result = 3;
            }
        }
        {
            typedef void (ScProcess::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScProcess::introspectionAboutToSwap)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject ScIDE::ScProcess::staticMetaObject = {
    { &QProcess::staticMetaObject, qt_meta_stringdata_ScIDE__ScProcess.data,
      qt_meta_data_ScIDE__ScProcess,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::ScProcess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::ScProcess::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__ScProcess.stringdata))
        return static_cast<void*>(const_cast< ScProcess*>(this));
    return QProcess::qt_metacast(_clname);
}

int ScIDE::ScProcess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProcess::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::ScProcess::scPost(QString const & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::ScProcess::statusMessage(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScIDE::ScProcess::response(const QString & _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ScIDE::ScProcess::classLibraryRecompiled()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void ScIDE::ScProcess::introspectionAboutToSwap()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
struct qt_meta_stringdata_ScIDE__ScRequest_t {
    QByteArrayData data[10];
    char stringdata[92];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__ScRequest_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__ScRequest_t qt_meta_stringdata_ScIDE__ScRequest = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ScIDE::ScRequest"
QT_MOC_LITERAL(1, 17, 8), // "response"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 7), // "command"
QT_MOC_LITERAL(4, 35, 4), // "data"
QT_MOC_LITERAL(5, 40, 9), // "cancelled"
QT_MOC_LITERAL(6, 50, 6), // "cancel"
QT_MOC_LITERAL(7, 57, 10), // "onResponse"
QT_MOC_LITERAL(8, 68, 10), // "responseId"
QT_MOC_LITERAL(9, 79, 12) // "responseData"

    },
    "ScIDE::ScRequest\0response\0\0command\0"
    "data\0cancelled\0cancel\0onResponse\0"
    "responseId\0responseData"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__ScRequest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   40,    2, 0x0a /* Public */,
       7,    2,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    8,    9,

       0        // eod
};

void ScIDE::ScRequest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScRequest *_t = static_cast<ScRequest *>(_o);
        switch (_id) {
        case 0: _t->response((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->cancelled(); break;
        case 2: _t->cancel(); break;
        case 3: _t->onResponse((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ScRequest::*_t)(const QString & , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScRequest::response)) {
                *result = 0;
            }
        }
        {
            typedef void (ScRequest::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScRequest::cancelled)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ScIDE::ScRequest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__ScRequest.data,
      qt_meta_data_ScIDE__ScRequest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::ScRequest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::ScRequest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__ScRequest.stringdata))
        return static_cast<void*>(const_cast< ScRequest*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::ScRequest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::ScRequest::response(const QString & _t1, const QString & _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::ScRequest::cancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
struct qt_meta_stringdata_ScIDE__ScIntrospectionParserWorker_t {
    QByteArrayData data[8];
    char stringdata[94];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__ScIntrospectionParserWorker_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__ScIntrospectionParserWorker_t qt_meta_stringdata_ScIDE__ScIntrospectionParserWorker = {
    {
QT_MOC_LITERAL(0, 0, 34), // "ScIDE::ScIntrospectionParserW..."
QT_MOC_LITERAL(1, 35, 4), // "done"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 26), // "ScLanguage::Introspection*"
QT_MOC_LITERAL(4, 68, 6), // "output"
QT_MOC_LITERAL(5, 75, 7), // "process"
QT_MOC_LITERAL(6, 83, 5), // "input"
QT_MOC_LITERAL(7, 89, 4) // "quit"

    },
    "ScIDE::ScIntrospectionParserWorker\0"
    "done\0\0ScLanguage::Introspection*\0"
    "output\0process\0input\0quit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__ScIntrospectionParserWorker[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    1,   32,    2, 0x08 /* Private */,
       7,    0,   35,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,

       0        // eod
};

void ScIDE::ScIntrospectionParserWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScIntrospectionParserWorker *_t = static_cast<ScIntrospectionParserWorker *>(_o);
        switch (_id) {
        case 0: _t->done((*reinterpret_cast< ScLanguage::Introspection*(*)>(_a[1]))); break;
        case 1: _t->process((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->quit(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ScIntrospectionParserWorker::*_t)(ScLanguage::Introspection * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScIntrospectionParserWorker::done)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::ScIntrospectionParserWorker::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__ScIntrospectionParserWorker.data,
      qt_meta_data_ScIDE__ScIntrospectionParserWorker,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::ScIntrospectionParserWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::ScIntrospectionParserWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__ScIntrospectionParserWorker.stringdata))
        return static_cast<void*>(const_cast< ScIntrospectionParserWorker*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::ScIntrospectionParserWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void ScIDE::ScIntrospectionParserWorker::done(ScLanguage::Introspection * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ScIDE__ScIntrospectionParser_t {
    QByteArrayData data[6];
    char stringdata[88];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__ScIntrospectionParser_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__ScIntrospectionParser_t qt_meta_stringdata_ScIDE__ScIntrospectionParser = {
    {
QT_MOC_LITERAL(0, 0, 28), // "ScIDE::ScIntrospectionParser"
QT_MOC_LITERAL(1, 29, 20), // "newIntrospectionData"
QT_MOC_LITERAL(2, 50, 0), // ""
QT_MOC_LITERAL(3, 51, 4), // "data"
QT_MOC_LITERAL(4, 56, 4), // "done"
QT_MOC_LITERAL(5, 61, 26) // "ScLanguage::Introspection*"

    },
    "ScIDE::ScIntrospectionParser\0"
    "newIntrospectionData\0\0data\0done\0"
    "ScLanguage::Introspection*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__ScIntrospectionParser[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    2,

       0        // eod
};

void ScIDE::ScIntrospectionParser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScIntrospectionParser *_t = static_cast<ScIntrospectionParser *>(_o);
        switch (_id) {
        case 0: _t->newIntrospectionData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->done((*reinterpret_cast< ScLanguage::Introspection*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ScIntrospectionParser::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScIntrospectionParser::newIntrospectionData)) {
                *result = 0;
            }
        }
        {
            typedef void (ScIntrospectionParser::*_t)(ScLanguage::Introspection * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScIntrospectionParser::done)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject ScIDE::ScIntrospectionParser::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ScIDE__ScIntrospectionParser.data,
      qt_meta_data_ScIDE__ScIntrospectionParser,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::ScIntrospectionParser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::ScIntrospectionParser::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__ScIntrospectionParser.stringdata))
        return static_cast<void*>(const_cast< ScIntrospectionParser*>(this));
    return QThread::qt_metacast(_clname);
}

int ScIDE::ScIntrospectionParser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ScIDE::ScIntrospectionParser::newIntrospectionData(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::ScIntrospectionParser::done(ScLanguage::Introspection * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
