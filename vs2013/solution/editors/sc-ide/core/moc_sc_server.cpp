/****************************************************************************
** Meta object code from reading C++ file 'sc_server.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/core/sc_server.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sc_server.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__ScServer_t {
    QByteArrayData data[53];
    char stringdata[644];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__ScServer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__ScServer_t qt_meta_stringdata_ScIDE__ScServer = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ScIDE::ScServer"
QT_MOC_LITERAL(1, 16, 18), // "runningStateChange"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 7), // "running"
QT_MOC_LITERAL(4, 44, 8), // "hostName"
QT_MOC_LITERAL(5, 53, 4), // "port"
QT_MOC_LITERAL(6, 58, 18), // "updateServerStatus"
QT_MOC_LITERAL(7, 77, 9), // "ugenCount"
QT_MOC_LITERAL(8, 87, 10), // "synthCount"
QT_MOC_LITERAL(9, 98, 10), // "groupCount"
QT_MOC_LITERAL(10, 109, 8), // "defCount"
QT_MOC_LITERAL(11, 118, 6), // "avgCPU"
QT_MOC_LITERAL(12, 125, 7), // "peakCPU"
QT_MOC_LITERAL(13, 133, 13), // "volumeChanged"
QT_MOC_LITERAL(14, 147, 6), // "volume"
QT_MOC_LITERAL(15, 154, 12), // "mutedChanged"
QT_MOC_LITERAL(16, 167, 5), // "muted"
QT_MOC_LITERAL(17, 173, 16), // "recordingChanged"
QT_MOC_LITERAL(18, 190, 9), // "recording"
QT_MOC_LITERAL(19, 200, 4), // "boot"
QT_MOC_LITERAL(20, 205, 6), // "reboot"
QT_MOC_LITERAL(21, 212, 4), // "quit"
QT_MOC_LITERAL(22, 217, 7), // "killAll"
QT_MOC_LITERAL(23, 225, 13), // "toggleRunning"
QT_MOC_LITERAL(24, 239, 10), // "showMeters"
QT_MOC_LITERAL(25, 250, 9), // "showScope"
QT_MOC_LITERAL(26, 260, 13), // "showFreqScope"
QT_MOC_LITERAL(27, 274, 12), // "dumpNodeTree"
QT_MOC_LITERAL(28, 287, 24), // "dumpNodeTreeWithControls"
QT_MOC_LITERAL(29, 312, 8), // "plotTree"
QT_MOC_LITERAL(30, 321, 13), // "queryAllNodes"
QT_MOC_LITERAL(31, 335, 12), // "dumpControls"
QT_MOC_LITERAL(32, 348, 14), // "increaseVolume"
QT_MOC_LITERAL(33, 363, 14), // "decreaseVolume"
QT_MOC_LITERAL(34, 378, 12), // "changeVolume"
QT_MOC_LITERAL(35, 391, 13), // "restoreVolume"
QT_MOC_LITERAL(36, 405, 4), // "mute"
QT_MOC_LITERAL(37, 410, 6), // "unmute"
QT_MOC_LITERAL(38, 417, 12), // "setRecording"
QT_MOC_LITERAL(39, 430, 6), // "active"
QT_MOC_LITERAL(40, 437, 20), // "onScLangStateChanged"
QT_MOC_LITERAL(41, 458, 22), // "QProcess::ProcessState"
QT_MOC_LITERAL(42, 481, 15), // "onScLangReponse"
QT_MOC_LITERAL(43, 497, 8), // "selector"
QT_MOC_LITERAL(44, 506, 4), // "data"
QT_MOC_LITERAL(45, 511, 19), // "onServerDataArrived"
QT_MOC_LITERAL(46, 531, 25), // "updateToggleRunningAction"
QT_MOC_LITERAL(47, 557, 21), // "updateRecordingAction"
QT_MOC_LITERAL(48, 579, 20), // "updateEnabledActions"
QT_MOC_LITERAL(49, 600, 9), // "sendMuted"
QT_MOC_LITERAL(50, 610, 10), // "sendVolume"
QT_MOC_LITERAL(51, 621, 14), // "sendDumpingOSC"
QT_MOC_LITERAL(52, 636, 7) // "dumping"

    },
    "ScIDE::ScServer\0runningStateChange\0\0"
    "running\0hostName\0port\0updateServerStatus\0"
    "ugenCount\0synthCount\0groupCount\0"
    "defCount\0avgCPU\0peakCPU\0volumeChanged\0"
    "volume\0mutedChanged\0muted\0recordingChanged\0"
    "recording\0boot\0reboot\0quit\0killAll\0"
    "toggleRunning\0showMeters\0showScope\0"
    "showFreqScope\0dumpNodeTree\0"
    "dumpNodeTreeWithControls\0plotTree\0"
    "queryAllNodes\0dumpControls\0increaseVolume\0"
    "decreaseVolume\0changeVolume\0restoreVolume\0"
    "mute\0unmute\0setRecording\0active\0"
    "onScLangStateChanged\0QProcess::ProcessState\0"
    "onScLangReponse\0selector\0data\0"
    "onServerDataArrived\0updateToggleRunningAction\0"
    "updateRecordingAction\0updateEnabledActions\0"
    "sendMuted\0sendVolume\0sendDumpingOSC\0"
    "dumping"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__ScServer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,  179,    2, 0x06 /* Public */,
       6,    6,  186,    2, 0x06 /* Public */,
      13,    1,  199,    2, 0x06 /* Public */,
      15,    1,  202,    2, 0x06 /* Public */,
      17,    1,  205,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      19,    0,  208,    2, 0x0a /* Public */,
      20,    0,  209,    2, 0x0a /* Public */,
      21,    0,  210,    2, 0x0a /* Public */,
      22,    0,  211,    2, 0x0a /* Public */,
      23,    0,  212,    2, 0x0a /* Public */,
      24,    0,  213,    2, 0x0a /* Public */,
      25,    0,  214,    2, 0x0a /* Public */,
      26,    0,  215,    2, 0x0a /* Public */,
      27,    0,  216,    2, 0x0a /* Public */,
      28,    0,  217,    2, 0x0a /* Public */,
      29,    0,  218,    2, 0x0a /* Public */,
      30,    1,  219,    2, 0x0a /* Public */,
      32,    0,  222,    2, 0x0a /* Public */,
      33,    0,  223,    2, 0x0a /* Public */,
      34,    1,  224,    2, 0x0a /* Public */,
      35,    0,  227,    2, 0x0a /* Public */,
      36,    0,  228,    2, 0x0a /* Public */,
      37,    0,  229,    2, 0x0a /* Public */,
      38,    1,  230,    2, 0x0a /* Public */,
      40,    1,  233,    2, 0x08 /* Private */,
      42,    2,  236,    2, 0x08 /* Private */,
      45,    0,  241,    2, 0x08 /* Private */,
      46,    0,  242,    2, 0x08 /* Private */,
      47,    0,  243,    2, 0x08 /* Private */,
      48,    0,  244,    2, 0x08 /* Private */,
      49,    1,  245,    2, 0x08 /* Private */,
      50,    1,  248,    2, 0x08 /* Private */,
      51,    1,  251,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::Int,    3,    4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Float, QMetaType::Float,    7,    8,    9,   10,   11,   12,
    QMetaType::Void, QMetaType::Float,   14,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Bool,   18,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   39,
    QMetaType::Void, 0x80000000 | 41,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   43,   44,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   16,
    QMetaType::Void, QMetaType::Float,   14,
    QMetaType::Void, QMetaType::Bool,   52,

       0        // eod
};

void ScIDE::ScServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ScServer *_t = static_cast<ScServer *>(_o);
        switch (_id) {
        case 0: _t->runningStateChange((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->updateServerStatus((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6]))); break;
        case 2: _t->volumeChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: _t->mutedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->recordingChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->boot(); break;
        case 6: _t->reboot(); break;
        case 7: _t->quit(); break;
        case 8: _t->killAll(); break;
        case 9: _t->toggleRunning(); break;
        case 10: _t->showMeters(); break;
        case 11: _t->showScope(); break;
        case 12: _t->showFreqScope(); break;
        case 13: _t->dumpNodeTree(); break;
        case 14: _t->dumpNodeTreeWithControls(); break;
        case 15: _t->plotTree(); break;
        case 16: _t->queryAllNodes((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->increaseVolume(); break;
        case 18: _t->decreaseVolume(); break;
        case 19: _t->changeVolume((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 20: _t->restoreVolume(); break;
        case 21: _t->mute(); break;
        case 22: _t->unmute(); break;
        case 23: _t->setRecording((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->onScLangStateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1]))); break;
        case 25: _t->onScLangReponse((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 26: _t->onServerDataArrived(); break;
        case 27: _t->updateToggleRunningAction(); break;
        case 28: _t->updateRecordingAction(); break;
        case 29: _t->updateEnabledActions(); break;
        case 30: _t->sendMuted((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 31: _t->sendVolume((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 32: _t->sendDumpingOSC((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (ScServer::*_t)(bool , QString const & , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScServer::runningStateChange)) {
                *result = 0;
            }
        }
        {
            typedef void (ScServer::*_t)(int , int , int , int , float , float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScServer::updateServerStatus)) {
                *result = 1;
            }
        }
        {
            typedef void (ScServer::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScServer::volumeChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (ScServer::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScServer::mutedChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (ScServer::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&ScServer::recordingChanged)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject ScIDE::ScServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__ScServer.data,
      qt_meta_data_ScIDE__ScServer,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::ScServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::ScServer::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__ScServer.stringdata))
        return static_cast<void*>(const_cast< ScServer*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::ScServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 33;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::ScServer::runningStateChange(bool _t1, QString const & _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::ScServer::updateServerStatus(int _t1, int _t2, int _t3, int _t4, float _t5, float _t6)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScIDE::ScServer::volumeChanged(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ScIDE::ScServer::mutedChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ScIDE::ScServer::recordingChanged(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
