/****************************************************************************
** Meta object code from reading C++ file 'sclang_page.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/settings/sclang_page.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sclang_page.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__Settings__SclangPage_t {
    QByteArrayData data[14];
    char stringdata[233];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__Settings__SclangPage_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__Settings__SclangPage_t qt_meta_stringdata_ScIDE__Settings__SclangPage = {
    {
QT_MOC_LITERAL(0, 0, 27), // "ScIDE::Settings::SclangPage"
QT_MOC_LITERAL(1, 28, 4), // "load"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "Manager*"
QT_MOC_LITERAL(4, 43, 5), // "store"
QT_MOC_LITERAL(5, 49, 14), // "addIncludePath"
QT_MOC_LITERAL(6, 64, 17), // "removeIncludePath"
QT_MOC_LITERAL(7, 82, 14), // "addExcludePath"
QT_MOC_LITERAL(8, 97, 17), // "removeExcludePath"
QT_MOC_LITERAL(9, 115, 21), // "markSclangConfigDirty"
QT_MOC_LITERAL(10, 137, 28), // "changeSelectedLanguageConfig"
QT_MOC_LITERAL(11, 166, 10), // "configPath"
QT_MOC_LITERAL(12, 177, 25), // "dialogCreateNewConfigFile"
QT_MOC_LITERAL(13, 203, 29) // "dialogDeleteCurrentConfigFile"

    },
    "ScIDE::Settings::SclangPage\0load\0\0"
    "Manager*\0store\0addIncludePath\0"
    "removeIncludePath\0addExcludePath\0"
    "removeExcludePath\0markSclangConfigDirty\0"
    "changeSelectedLanguageConfig\0configPath\0"
    "dialogCreateNewConfigFile\0"
    "dialogDeleteCurrentConfigFile"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__Settings__SclangPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x0a /* Public */,
       4,    1,   67,    2, 0x0a /* Public */,
       5,    0,   70,    2, 0x08 /* Private */,
       6,    0,   71,    2, 0x08 /* Private */,
       7,    0,   72,    2, 0x08 /* Private */,
       8,    0,   73,    2, 0x08 /* Private */,
       9,    0,   74,    2, 0x08 /* Private */,
      10,    1,   75,    2, 0x08 /* Private */,
      12,    0,   78,    2, 0x08 /* Private */,
      13,    0,   79,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::Settings::SclangPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SclangPage *_t = static_cast<SclangPage *>(_o);
        switch (_id) {
        case 0: _t->load((*reinterpret_cast< Manager*(*)>(_a[1]))); break;
        case 1: _t->store((*reinterpret_cast< Manager*(*)>(_a[1]))); break;
        case 2: _t->addIncludePath(); break;
        case 3: _t->removeIncludePath(); break;
        case 4: _t->addExcludePath(); break;
        case 5: _t->removeExcludePath(); break;
        case 6: _t->markSclangConfigDirty(); break;
        case 7: _t->changeSelectedLanguageConfig((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->dialogCreateNewConfigFile(); break;
        case 9: _t->dialogDeleteCurrentConfigFile(); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::Settings::SclangPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__Settings__SclangPage.data,
      qt_meta_data_ScIDE__Settings__SclangPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::Settings::SclangPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::Settings::SclangPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__Settings__SclangPage.stringdata))
        return static_cast<void*>(const_cast< SclangPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::Settings::SclangPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
