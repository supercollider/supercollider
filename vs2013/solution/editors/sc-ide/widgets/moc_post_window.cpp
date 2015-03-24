/****************************************************************************
** Meta object code from reading C++ file 'post_window.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/post_window.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'post_window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__PostWindow_t {
    QByteArrayData data[18];
    char stringdata[212];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__PostWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__PostWindow_t qt_meta_stringdata_ScIDE__PostWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ScIDE::PostWindow"
QT_MOC_LITERAL(1, 18, 21), // "scrollToBottomRequest"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 4), // "post"
QT_MOC_LITERAL(4, 46, 4), // "text"
QT_MOC_LITERAL(5, 51, 14), // "scrollToBottom"
QT_MOC_LITERAL(6, 66, 6), // "zoomIn"
QT_MOC_LITERAL(7, 73, 5), // "steps"
QT_MOC_LITERAL(8, 79, 7), // "zoomOut"
QT_MOC_LITERAL(9, 87, 9), // "resetZoom"
QT_MOC_LITERAL(10, 97, 13), // "closeDocument"
QT_MOC_LITERAL(11, 111, 17), // "openDocumentation"
QT_MOC_LITERAL(12, 129, 14), // "openDefinition"
QT_MOC_LITERAL(13, 144, 15), // "openCommandLine"
QT_MOC_LITERAL(14, 160, 14), // "findReferences"
QT_MOC_LITERAL(15, 175, 21), // "onAutoScrollTriggered"
QT_MOC_LITERAL(16, 197, 11), // "setLineWrap"
QT_MOC_LITERAL(17, 209, 2) // "on"

    },
    "ScIDE::PostWindow\0scrollToBottomRequest\0"
    "\0post\0text\0scrollToBottom\0zoomIn\0steps\0"
    "zoomOut\0resetZoom\0closeDocument\0"
    "openDocumentation\0openDefinition\0"
    "openCommandLine\0findReferences\0"
    "onAutoScrollTriggered\0setLineWrap\0on"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__PostWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   90,    2, 0x0a /* Public */,
       5,    0,   93,    2, 0x0a /* Public */,
       6,    1,   94,    2, 0x0a /* Public */,
       6,    0,   97,    2, 0x2a /* Public | MethodCloned */,
       8,    1,   98,    2, 0x0a /* Public */,
       8,    0,  101,    2, 0x2a /* Public | MethodCloned */,
       9,    0,  102,    2, 0x0a /* Public */,
      10,    0,  103,    2, 0x0a /* Public */,
      11,    0,  104,    2, 0x0a /* Public */,
      12,    0,  105,    2, 0x0a /* Public */,
      13,    0,  106,    2, 0x0a /* Public */,
      14,    0,  107,    2, 0x0a /* Public */,
      15,    1,  108,    2, 0x08 /* Private */,
      16,    1,  111,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,   17,

       0        // eod
};

void ScIDE::PostWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PostWindow *_t = static_cast<PostWindow *>(_o);
        switch (_id) {
        case 0: _t->scrollToBottomRequest(); break;
        case 1: _t->post((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->scrollToBottom(); break;
        case 3: _t->zoomIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->zoomIn(); break;
        case 5: _t->zoomOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->zoomOut(); break;
        case 7: _t->resetZoom(); break;
        case 8: _t->closeDocument(); break;
        case 9: { bool _r = _t->openDocumentation();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 10: _t->openDefinition(); break;
        case 11: _t->openCommandLine(); break;
        case 12: _t->findReferences(); break;
        case 13: _t->onAutoScrollTriggered((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 14: _t->setLineWrap((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (PostWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&PostWindow::scrollToBottomRequest)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::PostWindow::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_ScIDE__PostWindow.data,
      qt_meta_data_ScIDE__PostWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::PostWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::PostWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__PostWindow.stringdata))
        return static_cast<void*>(const_cast< PostWindow*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int ScIDE::PostWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::PostWindow::scrollToBottomRequest()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_ScIDE__PostDocklet_t {
    QByteArrayData data[4];
    char stringdata[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__PostDocklet_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__PostDocklet_t qt_meta_stringdata_ScIDE__PostDocklet = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ScIDE::PostDocklet"
QT_MOC_LITERAL(1, 19, 17), // "onFloatingChanged"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 8) // "floating"

    },
    "ScIDE::PostDocklet\0onFloatingChanged\0"
    "\0floating"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__PostDocklet[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void ScIDE::PostDocklet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PostDocklet *_t = static_cast<PostDocklet *>(_o);
        switch (_id) {
        case 0: _t->onFloatingChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::PostDocklet::staticMetaObject = {
    { &Docklet::staticMetaObject, qt_meta_stringdata_ScIDE__PostDocklet.data,
      qt_meta_data_ScIDE__PostDocklet,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::PostDocklet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::PostDocklet::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__PostDocklet.stringdata))
        return static_cast<void*>(const_cast< PostDocklet*>(this));
    return Docklet::qt_metacast(_clname);
}

int ScIDE::PostDocklet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Docklet::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
