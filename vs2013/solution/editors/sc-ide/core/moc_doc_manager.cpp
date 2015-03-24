/****************************************************************************
** Meta object code from reading C++ file 'doc_manager.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/core/doc_manager.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'doc_manager.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__Document_t {
    QByteArrayData data[8];
    char stringdata[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__Document_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__Document_t qt_meta_stringdata_ScIDE__Document = {
    {
QT_MOC_LITERAL(0, 0, 15), // "ScIDE::Document"
QT_MOC_LITERAL(1, 16, 18), // "defaultFontChanged"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 13), // "applySettings"
QT_MOC_LITERAL(4, 50, 18), // "Settings::Manager*"
QT_MOC_LITERAL(5, 69, 16), // "resetDefaultFont"
QT_MOC_LITERAL(6, 86, 12), // "storeTmpFile"
QT_MOC_LITERAL(7, 99, 14) // "onTmpCoalUsecs"

    },
    "ScIDE::Document\0defaultFontChanged\0\0"
    "applySettings\0Settings::Manager*\0"
    "resetDefaultFont\0storeTmpFile\0"
    "onTmpCoalUsecs"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__Document[] = {

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
       1,    0,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   40,    2, 0x0a /* Public */,
       5,    0,   43,    2, 0x0a /* Public */,
       6,    0,   44,    2, 0x0a /* Public */,
       7,    0,   45,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::Document::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Document *_t = static_cast<Document *>(_o);
        switch (_id) {
        case 0: _t->defaultFontChanged(); break;
        case 1: _t->applySettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 2: _t->resetDefaultFont(); break;
        case 3: _t->storeTmpFile(); break;
        case 4: _t->onTmpCoalUsecs(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (Document::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&Document::defaultFontChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::Document::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__Document.data,
      qt_meta_data_ScIDE__Document,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::Document::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::Document::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__Document.stringdata))
        return static_cast<void*>(const_cast< Document*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::Document::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void ScIDE::Document::defaultFontChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
struct qt_meta_stringdata_ScIDE__DocumentManager_t {
    QByteArrayData data[30];
    char stringdata[355];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__DocumentManager_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__DocumentManager_t qt_meta_stringdata_ScIDE__DocumentManager = {
    {
QT_MOC_LITERAL(0, 0, 22), // "ScIDE::DocumentManager"
QT_MOC_LITERAL(1, 23, 6), // "opened"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 9), // "Document*"
QT_MOC_LITERAL(4, 41, 14), // "cursorPosition"
QT_MOC_LITERAL(5, 56, 15), // "selectionLength"
QT_MOC_LITERAL(6, 72, 6), // "closed"
QT_MOC_LITERAL(7, 79, 5), // "saved"
QT_MOC_LITERAL(8, 85, 11), // "showRequest"
QT_MOC_LITERAL(9, 97, 3), // "pos"
QT_MOC_LITERAL(10, 101, 17), // "changedExternally"
QT_MOC_LITERAL(11, 119, 14), // "recentsChanged"
QT_MOC_LITERAL(12, 134, 12), // "titleChanged"
QT_MOC_LITERAL(13, 147, 4), // "open"
QT_MOC_LITERAL(14, 152, 4), // "path"
QT_MOC_LITERAL(15, 157, 21), // "initialCursorPosition"
QT_MOC_LITERAL(16, 179, 11), // "addToRecent"
QT_MOC_LITERAL(17, 191, 2), // "id"
QT_MOC_LITERAL(18, 194, 8), // "syncLang"
QT_MOC_LITERAL(19, 203, 12), // "clearRecents"
QT_MOC_LITERAL(20, 216, 13), // "storeSettings"
QT_MOC_LITERAL(21, 230, 18), // "Settings::Manager*"
QT_MOC_LITERAL(22, 249, 19), // "handleScLangMessage"
QT_MOC_LITERAL(23, 269, 8), // "selector"
QT_MOC_LITERAL(24, 278, 4), // "data"
QT_MOC_LITERAL(25, 283, 13), // "onFileChanged"
QT_MOC_LITERAL(26, 297, 24), // "updateCurrentDocContents"
QT_MOC_LITERAL(27, 322, 8), // "position"
QT_MOC_LITERAL(28, 331, 12), // "charsRemoved"
QT_MOC_LITERAL(29, 344, 10) // "charsAdded"

    },
    "ScIDE::DocumentManager\0opened\0\0Document*\0"
    "cursorPosition\0selectionLength\0closed\0"
    "saved\0showRequest\0pos\0changedExternally\0"
    "recentsChanged\0titleChanged\0open\0path\0"
    "initialCursorPosition\0addToRecent\0id\0"
    "syncLang\0clearRecents\0storeSettings\0"
    "Settings::Manager*\0handleScLangMessage\0"
    "selector\0data\0onFileChanged\0"
    "updateCurrentDocContents\0position\0"
    "charsRemoved\0charsAdded"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__DocumentManager[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,  114,    2, 0x06 /* Public */,
       6,    1,  121,    2, 0x06 /* Public */,
       7,    1,  124,    2, 0x06 /* Public */,
       8,    3,  127,    2, 0x06 /* Public */,
       8,    2,  134,    2, 0x26 /* Public | MethodCloned */,
       8,    1,  139,    2, 0x26 /* Public | MethodCloned */,
      10,    1,  142,    2, 0x06 /* Public */,
      11,    0,  145,    2, 0x06 /* Public */,
      12,    1,  146,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    6,  149,    2, 0x0a /* Public */,
      13,    5,  162,    2, 0x2a /* Public | MethodCloned */,
      13,    4,  173,    2, 0x2a /* Public | MethodCloned */,
      13,    3,  182,    2, 0x2a /* Public | MethodCloned */,
      13,    2,  189,    2, 0x2a /* Public | MethodCloned */,
      13,    1,  194,    2, 0x2a /* Public | MethodCloned */,
      19,    0,  197,    2, 0x0a /* Public */,
      20,    1,  198,    2, 0x0a /* Public */,
      22,    2,  201,    2, 0x0a /* Public */,
      25,    1,  206,    2, 0x08 /* Private */,
      26,    3,  209,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    2,    4,    5,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    2,    9,    5,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,    9,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Bool, QMetaType::QByteArray, QMetaType::Bool,   14,   15,    5,   16,   17,   18,
    0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Bool, QMetaType::QByteArray,   14,   15,    5,   16,   17,
    0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Bool,   14,   15,    5,   16,
    0x80000000 | 3, QMetaType::QString, QMetaType::Int, QMetaType::Int,   14,   15,    5,
    0x80000000 | 3, QMetaType::QString, QMetaType::Int,   14,   15,
    0x80000000 | 3, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 21,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   23,   24,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   27,   28,   29,

       0        // eod
};

void ScIDE::DocumentManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DocumentManager *_t = static_cast<DocumentManager *>(_o);
        switch (_id) {
        case 0: _t->opened((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->closed((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 2: _t->saved((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 3: _t->showRequest((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->showRequest((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->showRequest((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 6: _t->changedExternally((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 7: _t->recentsChanged(); break;
        case 8: _t->titleChanged((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 9: { Document* _r = _t->open((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< const QByteArray(*)>(_a[5])),(*reinterpret_cast< bool(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< Document**>(_a[0]) = _r; }  break;
        case 10: { Document* _r = _t->open((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< const QByteArray(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< Document**>(_a[0]) = _r; }  break;
        case 11: { Document* _r = _t->open((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< Document**>(_a[0]) = _r; }  break;
        case 12: { Document* _r = _t->open((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< Document**>(_a[0]) = _r; }  break;
        case 13: { Document* _r = _t->open((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Document**>(_a[0]) = _r; }  break;
        case 14: { Document* _r = _t->open((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< Document**>(_a[0]) = _r; }  break;
        case 15: _t->clearRecents(); break;
        case 16: _t->storeSettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 17: _t->handleScLangMessage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 18: _t->onFileChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->updateCurrentDocContents((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Document* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DocumentManager::*_t)(Document * , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::opened)) {
                *result = 0;
            }
        }
        {
            typedef void (DocumentManager::*_t)(Document * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::closed)) {
                *result = 1;
            }
        }
        {
            typedef void (DocumentManager::*_t)(Document * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::saved)) {
                *result = 2;
            }
        }
        {
            typedef void (DocumentManager::*_t)(Document * , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::showRequest)) {
                *result = 3;
            }
        }
        {
            typedef void (DocumentManager::*_t)(Document * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::changedExternally)) {
                *result = 6;
            }
        }
        {
            typedef void (DocumentManager::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::recentsChanged)) {
                *result = 7;
            }
        }
        {
            typedef void (DocumentManager::*_t)(Document * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentManager::titleChanged)) {
                *result = 8;
            }
        }
    }
}

const QMetaObject ScIDE::DocumentManager::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ScIDE__DocumentManager.data,
      qt_meta_data_ScIDE__DocumentManager,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::DocumentManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::DocumentManager::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__DocumentManager.stringdata))
        return static_cast<void*>(const_cast< DocumentManager*>(this));
    return QObject::qt_metacast(_clname);
}

int ScIDE::DocumentManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::DocumentManager::opened(Document * _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ScIDE::DocumentManager::closed(Document * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ScIDE::DocumentManager::saved(Document * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ScIDE::DocumentManager::showRequest(Document * _t1, int _t2, int _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 6
void ScIDE::DocumentManager::changedExternally(Document * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void ScIDE::DocumentManager::recentsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, Q_NULLPTR);
}

// SIGNAL 8
void ScIDE::DocumentManager::titleChanged(Document * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
