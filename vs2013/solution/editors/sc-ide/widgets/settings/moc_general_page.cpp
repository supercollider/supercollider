/****************************************************************************
** Meta object code from reading C++ file 'general_page.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/settings/general_page.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'general_page.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__Settings__GeneralPage_t {
    QByteArrayData data[7];
    char stringdata[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__Settings__GeneralPage_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__Settings__GeneralPage_t qt_meta_stringdata_ScIDE__Settings__GeneralPage = {
    {
QT_MOC_LITERAL(0, 0, 28), // "ScIDE::Settings::GeneralPage"
QT_MOC_LITERAL(1, 29, 4), // "load"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 8), // "Manager*"
QT_MOC_LITERAL(4, 44, 5), // "store"
QT_MOC_LITERAL(5, 50, 25), // "onStartSessionNameChanged"
QT_MOC_LITERAL(6, 76, 4) // "text"

    },
    "ScIDE::Settings::GeneralPage\0load\0\0"
    "Manager*\0store\0onStartSessionNameChanged\0"
    "text"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__Settings__GeneralPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       4,    1,   32,    2, 0x0a /* Public */,
       5,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QString,    6,

       0        // eod
};

void ScIDE::Settings::GeneralPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GeneralPage *_t = static_cast<GeneralPage *>(_o);
        switch (_id) {
        case 0: _t->load((*reinterpret_cast< Manager*(*)>(_a[1]))); break;
        case 1: _t->store((*reinterpret_cast< Manager*(*)>(_a[1]))); break;
        case 2: _t->onStartSessionNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::Settings::GeneralPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__Settings__GeneralPage.data,
      qt_meta_data_ScIDE__Settings__GeneralPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::Settings::GeneralPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::Settings::GeneralPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__Settings__GeneralPage.stringdata))
        return static_cast<void*>(const_cast< GeneralPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::Settings::GeneralPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_END_MOC_NAMESPACE
