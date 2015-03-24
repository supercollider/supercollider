/****************************************************************************
** Meta object code from reading C++ file 'goto_line_tool.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/goto_line_tool.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'goto_line_tool.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__GoToLineTool_t {
    QByteArrayData data[9];
    char stringdata[87];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__GoToLineTool_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__GoToLineTool_t qt_meta_stringdata_ScIDE__GoToLineTool = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ScIDE::GoToLineTool"
QT_MOC_LITERAL(1, 20, 9), // "activated"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 10), // "lineNumber"
QT_MOC_LITERAL(4, 42, 10), // "setMaximum"
QT_MOC_LITERAL(5, 53, 3), // "max"
QT_MOC_LITERAL(6, 57, 8), // "setFocus"
QT_MOC_LITERAL(7, 66, 2), // "go"
QT_MOC_LITERAL(8, 69, 17) // "onEditingFinished"

    },
    "ScIDE::GoToLineTool\0activated\0\0"
    "lineNumber\0setMaximum\0max\0setFocus\0"
    "go\0onEditingFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__GoToLineTool[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   42,    2, 0x0a /* Public */,
       6,    0,   45,    2, 0x0a /* Public */,
       7,    0,   46,    2, 0x08 /* Private */,
       8,    0,   47,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::GoToLineTool::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GoToLineTool *_t = static_cast<GoToLineTool *>(_o);
        switch (_id) {
        case 0: _t->activated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->setMaximum((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setFocus(); break;
        case 3: _t->go(); break;
        case 4: _t->onEditingFinished(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GoToLineTool::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GoToLineTool::activated)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::GoToLineTool::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__GoToLineTool.data,
      qt_meta_data_ScIDE__GoToLineTool,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::GoToLineTool::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::GoToLineTool::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__GoToLineTool.stringdata))
        return static_cast<void*>(const_cast< GoToLineTool*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::GoToLineTool::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
void ScIDE::GoToLineTool::activated(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
