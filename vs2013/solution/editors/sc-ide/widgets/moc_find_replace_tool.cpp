/****************************************************************************
** Meta object code from reading C++ file 'find_replace_tool.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/find_replace_tool.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'find_replace_tool.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__TextFindReplacePanel_t {
    QByteArrayData data[9];
    char stringdata[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__TextFindReplacePanel_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__TextFindReplacePanel_t qt_meta_stringdata_ScIDE__TextFindReplacePanel = {
    {
QT_MOC_LITERAL(0, 0, 27), // "ScIDE::TextFindReplacePanel"
QT_MOC_LITERAL(1, 28, 8), // "findNext"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 12), // "findPrevious"
QT_MOC_LITERAL(4, 51, 7), // "findAll"
QT_MOC_LITERAL(5, 59, 7), // "replace"
QT_MOC_LITERAL(6, 67, 10), // "replaceAll"
QT_MOC_LITERAL(7, 78, 17), // "onFindFieldReturn"
QT_MOC_LITERAL(8, 96, 22) // "onFindFieldTextChanged"

    },
    "ScIDE::TextFindReplacePanel\0findNext\0"
    "\0findPrevious\0findAll\0replace\0replaceAll\0"
    "onFindFieldReturn\0onFindFieldTextChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__TextFindReplacePanel[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x0a /* Public */,
       3,    0,   50,    2, 0x0a /* Public */,
       4,    0,   51,    2, 0x0a /* Public */,
       5,    0,   52,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x0a /* Public */,
       7,    0,   54,    2, 0x08 /* Private */,
       8,    0,   55,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::TextFindReplacePanel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TextFindReplacePanel *_t = static_cast<TextFindReplacePanel *>(_o);
        switch (_id) {
        case 0: _t->findNext(); break;
        case 1: _t->findPrevious(); break;
        case 2: _t->findAll(); break;
        case 3: _t->replace(); break;
        case 4: _t->replaceAll(); break;
        case 5: _t->onFindFieldReturn(); break;
        case 6: _t->onFindFieldTextChanged(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::TextFindReplacePanel::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__TextFindReplacePanel.data,
      qt_meta_data_ScIDE__TextFindReplacePanel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::TextFindReplacePanel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::TextFindReplacePanel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__TextFindReplacePanel.stringdata))
        return static_cast<void*>(const_cast< TextFindReplacePanel*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::TextFindReplacePanel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
