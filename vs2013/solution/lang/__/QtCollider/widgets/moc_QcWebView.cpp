/****************************************************************************
** Meta object code from reading C++ file 'QcWebView.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcWebView.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcWebView.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QtCollider__WebView_t {
    QByteArrayData data[29];
    char stringdata[358];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QtCollider__WebView_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QtCollider__WebView_t qt_meta_stringdata_QtCollider__WebView = {
    {
QT_MOC_LITERAL(0, 0, 19), // "QtCollider::WebView"
QT_MOC_LITERAL(1, 20, 13), // "linkActivated"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 15), // "reloadTriggered"
QT_MOC_LITERAL(4, 51, 9), // "interpret"
QT_MOC_LITERAL(5, 61, 4), // "code"
QT_MOC_LITERAL(6, 66, 12), // "jsConsoleMsg"
QT_MOC_LITERAL(7, 79, 8), // "findText"
QT_MOC_LITERAL(8, 88, 10), // "searchText"
QT_MOC_LITERAL(9, 99, 8), // "reversed"
QT_MOC_LITERAL(10, 108, 13), // "onLinkClicked"
QT_MOC_LITERAL(11, 122, 12), // "onPageReload"
QT_MOC_LITERAL(12, 135, 14), // "updateEditable"
QT_MOC_LITERAL(13, 150, 2), // "ok"
QT_MOC_LITERAL(14, 153, 7), // "setHtml"
QT_MOC_LITERAL(15, 161, 4), // "html"
QT_MOC_LITERAL(16, 166, 7), // "baseUrl"
QT_MOC_LITERAL(17, 174, 18), // "evaluateJavaScript"
QT_MOC_LITERAL(18, 193, 6), // "script"
QT_MOC_LITERAL(19, 200, 13), // "setFontFamily"
QT_MOC_LITERAL(20, 214, 17), // "genericFontFamily"
QT_MOC_LITERAL(21, 232, 10), // "fontFamily"
QT_MOC_LITERAL(22, 243, 3), // "url"
QT_MOC_LITERAL(23, 247, 9), // "plainText"
QT_MOC_LITERAL(24, 257, 20), // "linkDelegationPolicy"
QT_MOC_LITERAL(25, 278, 30), // "QWebPage::LinkDelegationPolicy"
QT_MOC_LITERAL(26, 309, 14), // "delegateReload"
QT_MOC_LITERAL(27, 324, 24), // "enterInterpretsSelection"
QT_MOC_LITERAL(28, 349, 8) // "editable"

    },
    "QtCollider::WebView\0linkActivated\0\0"
    "reloadTriggered\0interpret\0code\0"
    "jsConsoleMsg\0findText\0searchText\0"
    "reversed\0onLinkClicked\0onPageReload\0"
    "updateEditable\0ok\0setHtml\0html\0baseUrl\0"
    "evaluateJavaScript\0script\0setFontFamily\0"
    "genericFontFamily\0fontFamily\0url\0"
    "plainText\0linkDelegationPolicy\0"
    "QWebPage::LinkDelegationPolicy\0"
    "delegateReload\0enterInterpretsSelection\0"
    "editable"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtCollider__WebView[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       7,  126, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   79,    2, 0x06 /* Public */,
       3,    1,   82,    2, 0x06 /* Public */,
       4,    1,   85,    2, 0x06 /* Public */,
       6,    3,   88,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    2,   95,    2, 0x0a /* Public */,
       7,    1,  100,    2, 0x2a /* Public | MethodCloned */,
      10,    1,  103,    2, 0x08 /* Private */,
      11,    0,  106,    2, 0x08 /* Private */,
      12,    1,  107,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      14,    2,  110,    2, 0x02 /* Public */,
      14,    1,  115,    2, 0x22 /* Public | MethodCloned */,
      17,    1,  118,    2, 0x02 /* Public */,
      19,    2,  121,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::QString,    2,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    8,    9,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QUrl,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,

 // methods: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   15,   16,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::QString,   18,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,   20,   21,

 // properties: name, type, flags
      22, QMetaType::QString, 0x00095103,
      15, QMetaType::QString, 0x00095001,
      23, QMetaType::QString, 0x00095001,
      24, 0x80000000 | 25, 0x0009510b,
      26, QMetaType::Bool, 0x00095103,
      27, QMetaType::Bool, 0x00095003,
      28, QMetaType::Bool, 0x00095103,

       0        // eod
};

void QtCollider::WebView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WebView *_t = static_cast<WebView *>(_o);
        switch (_id) {
        case 0: _t->linkActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->reloadTriggered((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->interpret((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->jsConsoleMsg((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 4: _t->findText((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->findText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->onLinkClicked((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 7: _t->onPageReload(); break;
        case 8: _t->updateEditable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setHtml((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 10: _t->setHtml((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: _t->evaluateJavaScript((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->setFontFamily((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (WebView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WebView::linkActivated)) {
                *result = 0;
            }
        }
        {
            typedef void (WebView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WebView::reloadTriggered)) {
                *result = 1;
            }
        }
        {
            typedef void (WebView::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WebView::interpret)) {
                *result = 2;
            }
        }
        {
            typedef void (WebView::*_t)(const QString & , int , const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&WebView::jsConsoleMsg)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject QtCollider::WebView::staticMetaObject = {
    { &QWebView::staticMetaObject, qt_meta_stringdata_QtCollider__WebView.data,
      qt_meta_data_QtCollider__WebView,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QtCollider::WebView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtCollider::WebView::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QtCollider__WebView.stringdata))
        return static_cast<void*>(const_cast< WebView*>(this));
    return QWebView::qt_metacast(_clname);
}

int QtCollider::WebView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWebView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = url(); break;
        case 1: *reinterpret_cast< QString*>(_v) = html(); break;
        case 2: *reinterpret_cast< QString*>(_v) = plainText(); break;
        case 3: *reinterpret_cast< QWebPage::LinkDelegationPolicy*>(_v) = linkDelegationPolicy(); break;
        case 4: *reinterpret_cast< bool*>(_v) = delegateReload(); break;
        case 5: *reinterpret_cast< bool*>(_v) = interpretSelection(); break;
        case 6: *reinterpret_cast< bool*>(_v) = editable(); break;
        default: break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setUrl(*reinterpret_cast< QString*>(_v)); break;
        case 3: setLinkDelegationPolicy(*reinterpret_cast< QWebPage::LinkDelegationPolicy*>(_v)); break;
        case 4: setDelegateReload(*reinterpret_cast< bool*>(_v)); break;
        case 5: setInterpretSelection(*reinterpret_cast< bool*>(_v)); break;
        case 6: setEditable(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
        _id -= 7;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QtCollider::WebView::linkActivated(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QtCollider::WebView::reloadTriggered(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QtCollider::WebView::interpret(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QtCollider::WebView::jsConsoleMsg(const QString & _t1, int _t2, const QString & _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
