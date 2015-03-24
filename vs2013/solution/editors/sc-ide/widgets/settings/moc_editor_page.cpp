/****************************************************************************
** Meta object code from reading C++ file 'editor_page.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/settings/editor_page.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editor_page.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__Settings__EditorPage_t {
    QByteArrayData data[15];
    char stringdata[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__Settings__EditorPage_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__Settings__EditorPage_t qt_meta_stringdata_ScIDE__Settings__EditorPage = {
    {
QT_MOC_LITERAL(0, 0, 27), // "ScIDE::Settings::EditorPage"
QT_MOC_LITERAL(1, 28, 4), // "load"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 8), // "Manager*"
QT_MOC_LITERAL(4, 43, 5), // "store"
QT_MOC_LITERAL(5, 49, 19), // "onCurrentTabChanged"
QT_MOC_LITERAL(6, 69, 17), // "onMonospaceToggle"
QT_MOC_LITERAL(7, 87, 17), // "updateFontPreview"
QT_MOC_LITERAL(8, 105, 20), // "updateTextFormatEdit"
QT_MOC_LITERAL(9, 126, 23), // "updateTextFormatDisplay"
QT_MOC_LITERAL(10, 150, 30), // "updateTextFormatDisplayCommons"
QT_MOC_LITERAL(11, 181, 11), // "updateTheme"
QT_MOC_LITERAL(12, 193, 4), // "name"
QT_MOC_LITERAL(13, 198, 15), // "dialogCopyTheme"
QT_MOC_LITERAL(14, 214, 11) // "deleteTheme"

    },
    "ScIDE::Settings::EditorPage\0load\0\0"
    "Manager*\0store\0onCurrentTabChanged\0"
    "onMonospaceToggle\0updateFontPreview\0"
    "updateTextFormatEdit\0updateTextFormatDisplay\0"
    "updateTextFormatDisplayCommons\0"
    "updateTheme\0name\0dialogCopyTheme\0"
    "deleteTheme"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__Settings__EditorPage[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x0a /* Public */,
       4,    1,   72,    2, 0x0a /* Public */,
       5,    1,   75,    2, 0x08 /* Private */,
       6,    1,   78,    2, 0x08 /* Private */,
       7,    0,   81,    2, 0x08 /* Private */,
       8,    0,   82,    2, 0x08 /* Private */,
       9,    0,   83,    2, 0x08 /* Private */,
      10,    0,   84,    2, 0x08 /* Private */,
      11,    1,   85,    2, 0x08 /* Private */,
      13,    0,   88,    2, 0x08 /* Private */,
      14,    0,   89,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::Settings::EditorPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorPage *_t = static_cast<EditorPage *>(_o);
        switch (_id) {
        case 0: _t->load((*reinterpret_cast< Manager*(*)>(_a[1]))); break;
        case 1: _t->store((*reinterpret_cast< Manager*(*)>(_a[1]))); break;
        case 2: _t->onCurrentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onMonospaceToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->updateFontPreview(); break;
        case 5: _t->updateTextFormatEdit(); break;
        case 6: _t->updateTextFormatDisplay(); break;
        case 7: _t->updateTextFormatDisplayCommons(); break;
        case 8: _t->updateTheme((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->dialogCopyTheme(); break;
        case 10: _t->deleteTheme(); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::Settings::EditorPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__Settings__EditorPage.data,
      qt_meta_data_ScIDE__Settings__EditorPage,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::Settings::EditorPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::Settings::EditorPage::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__Settings__EditorPage.stringdata))
        return static_cast<void*>(const_cast< EditorPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::Settings::EditorPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
