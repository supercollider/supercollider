/****************************************************************************
** Meta object code from reading C++ file 'volume_widget.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/util/volume_widget.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'volume_widget.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__VolumeWidget_t {
    QByteArrayData data[5];
    char stringdata[64];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__VolumeWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__VolumeWidget_t qt_meta_stringdata_ScIDE__VolumeWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ScIDE::VolumeWidget"
QT_MOC_LITERAL(1, 20, 13), // "volumeChanged"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 7), // "volumet"
QT_MOC_LITERAL(4, 43, 20) // "onVolumeSliderAction"

    },
    "ScIDE::VolumeWidget\0volumeChanged\0\0"
    "volumet\0onVolumeSliderAction"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__VolumeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   27,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void ScIDE::VolumeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        VolumeWidget *_t = static_cast<VolumeWidget *>(_o);
        switch (_id) {
        case 0: _t->volumeChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->onVolumeSliderAction(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (VolumeWidget::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&VolumeWidget::volumeChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::VolumeWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__VolumeWidget.data,
      qt_meta_data_ScIDE__VolumeWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::VolumeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::VolumeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__VolumeWidget.stringdata))
        return static_cast<void*>(const_cast< VolumeWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::VolumeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::VolumeWidget::volumeChanged(float _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
