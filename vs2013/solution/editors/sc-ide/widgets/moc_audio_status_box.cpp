/****************************************************************************
** Meta object code from reading C++ file 'audio_status_box.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/audio_status_box.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'audio_status_box.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__AudioStatusBox_t {
    QByteArrayData data[20];
    char stringdata[225];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__AudioStatusBox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__AudioStatusBox_t qt_meta_stringdata_ScIDE__AudioStatusBox = {
    {
QT_MOC_LITERAL(0, 0, 21), // "ScIDE::AudioStatusBox"
QT_MOC_LITERAL(1, 22, 19), // "onServerStatusReply"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 5), // "ugens"
QT_MOC_LITERAL(4, 49, 6), // "synths"
QT_MOC_LITERAL(5, 56, 6), // "groups"
QT_MOC_LITERAL(6, 63, 9), // "synthDefs"
QT_MOC_LITERAL(7, 73, 6), // "avgCPU"
QT_MOC_LITERAL(8, 80, 7), // "peakCPU"
QT_MOC_LITERAL(9, 88, 22), // "onServerRunningChanged"
QT_MOC_LITERAL(10, 111, 7), // "running"
QT_MOC_LITERAL(11, 119, 8), // "hostName"
QT_MOC_LITERAL(12, 128, 4), // "port"
QT_MOC_LITERAL(13, 133, 16), // "updateStatistics"
QT_MOC_LITERAL(14, 150, 17), // "updateVolumeLabel"
QT_MOC_LITERAL(15, 168, 6), // "volume"
QT_MOC_LITERAL(16, 175, 15), // "updateMuteLabel"
QT_MOC_LITERAL(17, 191, 5), // "muted"
QT_MOC_LITERAL(18, 197, 17), // "updateRecordLabel"
QT_MOC_LITERAL(19, 215, 9) // "recording"

    },
    "ScIDE::AudioStatusBox\0onServerStatusReply\0"
    "\0ugens\0synths\0groups\0synthDefs\0avgCPU\0"
    "peakCPU\0onServerRunningChanged\0running\0"
    "hostName\0port\0updateStatistics\0"
    "updateVolumeLabel\0volume\0updateMuteLabel\0"
    "muted\0updateRecordLabel\0recording"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__AudioStatusBox[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    6,   44,    2, 0x08 /* Private */,
       9,    3,   57,    2, 0x08 /* Private */,
      13,    0,   64,    2, 0x08 /* Private */,
      14,    1,   65,    2, 0x08 /* Private */,
      16,    1,   68,    2, 0x08 /* Private */,
      18,    1,   71,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Float, QMetaType::Float,    3,    4,    5,    6,    7,    8,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString, QMetaType::Int,   10,   11,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,   15,
    QMetaType::Void, QMetaType::Bool,   17,
    QMetaType::Void, QMetaType::Bool,   19,

       0        // eod
};

void ScIDE::AudioStatusBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AudioStatusBox *_t = static_cast<AudioStatusBox *>(_o);
        switch (_id) {
        case 0: _t->onServerStatusReply((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< float(*)>(_a[5])),(*reinterpret_cast< float(*)>(_a[6]))); break;
        case 1: _t->onServerRunningChanged((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: _t->updateStatistics(); break;
        case 3: _t->updateVolumeLabel((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 4: _t->updateMuteLabel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->updateRecordLabel((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::AudioStatusBox::staticMetaObject = {
    { &StatusBox::staticMetaObject, qt_meta_stringdata_ScIDE__AudioStatusBox.data,
      qt_meta_data_ScIDE__AudioStatusBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::AudioStatusBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::AudioStatusBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__AudioStatusBox.stringdata))
        return static_cast<void*>(const_cast< AudioStatusBox*>(this));
    return StatusBox::qt_metacast(_clname);
}

int ScIDE::AudioStatusBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = StatusBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
