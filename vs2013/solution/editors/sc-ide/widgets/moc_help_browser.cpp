/****************************************************************************
** Meta object code from reading C++ file 'help_browser.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/help_browser.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'help_browser.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__LoadProgressIndicator_t {
    QByteArrayData data[5];
    char stringdata[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__LoadProgressIndicator_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__LoadProgressIndicator_t qt_meta_stringdata_ScIDE__LoadProgressIndicator = {
    {
QT_MOC_LITERAL(0, 0, 28), // "ScIDE::LoadProgressIndicator"
QT_MOC_LITERAL(1, 29, 5), // "start"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 3), // "msg"
QT_MOC_LITERAL(4, 40, 4) // "stop"

    },
    "ScIDE::LoadProgressIndicator\0start\0\0"
    "msg\0stop"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__LoadProgressIndicator[] = {

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
       1,    0,   32,    2, 0x2a /* Public | MethodCloned */,
       4,    0,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::LoadProgressIndicator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LoadProgressIndicator *_t = static_cast<LoadProgressIndicator *>(_o);
        switch (_id) {
        case 0: _t->start((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->start(); break;
        case 2: _t->stop(); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::LoadProgressIndicator::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_ScIDE__LoadProgressIndicator.data,
      qt_meta_data_ScIDE__LoadProgressIndicator,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::LoadProgressIndicator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::LoadProgressIndicator::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__LoadProgressIndicator.stringdata))
        return static_cast<void*>(const_cast< LoadProgressIndicator*>(this));
    return QLabel::qt_metacast(_clname);
}

int ScIDE::LoadProgressIndicator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_ScIDE__HelpBrowser_t {
    QByteArrayData data[26];
    char stringdata[305];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__HelpBrowser_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__HelpBrowser_t qt_meta_stringdata_ScIDE__HelpBrowser = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ScIDE::HelpBrowser"
QT_MOC_LITERAL(1, 19, 10), // "urlChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 13), // "applySettings"
QT_MOC_LITERAL(4, 45, 18), // "Settings::Manager*"
QT_MOC_LITERAL(5, 64, 6), // "goHome"
QT_MOC_LITERAL(6, 71, 13), // "closeDocument"
QT_MOC_LITERAL(7, 85, 6), // "zoomIn"
QT_MOC_LITERAL(8, 92, 7), // "zoomOut"
QT_MOC_LITERAL(9, 100, 9), // "resetZoom"
QT_MOC_LITERAL(10, 110, 17), // "evaluateSelection"
QT_MOC_LITERAL(11, 128, 8), // "findText"
QT_MOC_LITERAL(12, 137, 4), // "text"
QT_MOC_LITERAL(13, 142, 9), // "backwards"
QT_MOC_LITERAL(14, 152, 17), // "openDocumentation"
QT_MOC_LITERAL(15, 170, 14), // "openDefinition"
QT_MOC_LITERAL(16, 185, 15), // "openCommandLine"
QT_MOC_LITERAL(17, 201, 14), // "findReferences"
QT_MOC_LITERAL(18, 216, 20), // "onContextMenuRequest"
QT_MOC_LITERAL(19, 237, 3), // "pos"
QT_MOC_LITERAL(20, 241, 13), // "onLinkClicked"
QT_MOC_LITERAL(21, 255, 8), // "onReload"
QT_MOC_LITERAL(22, 264, 12), // "onScResponse"
QT_MOC_LITERAL(23, 277, 7), // "command"
QT_MOC_LITERAL(24, 285, 4), // "data"
QT_MOC_LITERAL(25, 290, 14) // "onJsConsoleMsg"

    },
    "ScIDE::HelpBrowser\0urlChanged\0\0"
    "applySettings\0Settings::Manager*\0"
    "goHome\0closeDocument\0zoomIn\0zoomOut\0"
    "resetZoom\0evaluateSelection\0findText\0"
    "text\0backwards\0openDocumentation\0"
    "openDefinition\0openCommandLine\0"
    "findReferences\0onContextMenuRequest\0"
    "pos\0onLinkClicked\0onReload\0onScResponse\0"
    "command\0data\0onJsConsoleMsg"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__HelpBrowser[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,  110,    2, 0x0a /* Public */,
       5,    0,  113,    2, 0x0a /* Public */,
       6,    0,  114,    2, 0x0a /* Public */,
       7,    0,  115,    2, 0x0a /* Public */,
       8,    0,  116,    2, 0x0a /* Public */,
       9,    0,  117,    2, 0x0a /* Public */,
      10,    0,  118,    2, 0x0a /* Public */,
      11,    2,  119,    2, 0x0a /* Public */,
      11,    1,  124,    2, 0x2a /* Public | MethodCloned */,
      14,    0,  127,    2, 0x0a /* Public */,
      15,    0,  128,    2, 0x0a /* Public */,
      16,    0,  129,    2, 0x0a /* Public */,
      17,    0,  130,    2, 0x0a /* Public */,
      18,    1,  131,    2, 0x08 /* Private */,
      20,    1,  134,    2, 0x08 /* Private */,
      21,    0,  137,    2, 0x08 /* Private */,
      22,    2,  138,    2, 0x08 /* Private */,
      25,    3,  143,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,   12,   13,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,   19,
    QMetaType::Void, QMetaType::QUrl,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   23,   24,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    2,    2,    2,

       0        // eod
};

void ScIDE::HelpBrowser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpBrowser *_t = static_cast<HelpBrowser *>(_o);
        switch (_id) {
        case 0: _t->urlChanged(); break;
        case 1: _t->applySettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 2: _t->goHome(); break;
        case 3: _t->closeDocument(); break;
        case 4: _t->zoomIn(); break;
        case 5: _t->zoomOut(); break;
        case 6: _t->resetZoom(); break;
        case 7: _t->evaluateSelection(); break;
        case 8: _t->findText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 9: _t->findText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: { bool _r = _t->openDocumentation();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 11: _t->openDefinition(); break;
        case 12: _t->openCommandLine(); break;
        case 13: _t->findReferences(); break;
        case 14: _t->onContextMenuRequest((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 15: _t->onLinkClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 16: _t->onReload(); break;
        case 17: _t->onScResponse((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 18: _t->onJsConsoleMsg((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HelpBrowser::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HelpBrowser::urlChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::HelpBrowser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__HelpBrowser.data,
      qt_meta_data_ScIDE__HelpBrowser,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::HelpBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::HelpBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__HelpBrowser.stringdata))
        return static_cast<void*>(const_cast< HelpBrowser*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::HelpBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::HelpBrowser::urlChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_ScIDE__HelpBrowserFindBox_t {
    QByteArrayData data[5];
    char stringdata[48];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__HelpBrowserFindBox_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__HelpBrowserFindBox_t qt_meta_stringdata_ScIDE__HelpBrowserFindBox = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ScIDE::HelpBrowserFindBox"
QT_MOC_LITERAL(1, 26, 5), // "query"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 4), // "text"
QT_MOC_LITERAL(4, 38, 9) // "backwards"

    },
    "ScIDE::HelpBrowserFindBox\0query\0\0text\0"
    "backwards"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__HelpBrowserFindBox[] = {

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
       1,    2,   24,    2, 0x06 /* Public */,
       1,    1,   29,    2, 0x26 /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    3,    4,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void ScIDE::HelpBrowserFindBox::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpBrowserFindBox *_t = static_cast<HelpBrowserFindBox *>(_o);
        switch (_id) {
        case 0: _t->query((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->query((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (HelpBrowserFindBox::*_t)(const QString & , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&HelpBrowserFindBox::query)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::HelpBrowserFindBox::staticMetaObject = {
    { &QLineEdit::staticMetaObject, qt_meta_stringdata_ScIDE__HelpBrowserFindBox.data,
      qt_meta_data_ScIDE__HelpBrowserFindBox,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::HelpBrowserFindBox::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::HelpBrowserFindBox::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__HelpBrowserFindBox.stringdata))
        return static_cast<void*>(const_cast< HelpBrowserFindBox*>(this));
    return QLineEdit::qt_metacast(_clname);
}

int ScIDE::HelpBrowserFindBox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLineEdit::qt_metacall(_c, _id, _a);
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
void ScIDE::HelpBrowserFindBox::query(const QString & _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ScIDE__HelpBrowserDocklet_t {
    QByteArrayData data[3];
    char stringdata[46];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__HelpBrowserDocklet_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__HelpBrowserDocklet_t qt_meta_stringdata_ScIDE__HelpBrowserDocklet = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ScIDE::HelpBrowserDocklet"
QT_MOC_LITERAL(1, 26, 18), // "onInterpreterStart"
QT_MOC_LITERAL(2, 45, 0) // ""

    },
    "ScIDE::HelpBrowserDocklet\0onInterpreterStart\0"
    ""
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__HelpBrowserDocklet[] = {

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
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void ScIDE::HelpBrowserDocklet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpBrowserDocklet *_t = static_cast<HelpBrowserDocklet *>(_o);
        switch (_id) {
        case 0: _t->onInterpreterStart(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::HelpBrowserDocklet::staticMetaObject = {
    { &Docklet::staticMetaObject, qt_meta_stringdata_ScIDE__HelpBrowserDocklet.data,
      qt_meta_data_ScIDE__HelpBrowserDocklet,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::HelpBrowserDocklet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::HelpBrowserDocklet::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__HelpBrowserDocklet.stringdata))
        return static_cast<void*>(const_cast< HelpBrowserDocklet*>(this));
    return Docklet::qt_metacast(_clname);
}

int ScIDE::HelpBrowserDocklet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
