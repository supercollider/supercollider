/****************************************************************************
** Meta object code from reading C++ file 'lookup_dialog.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/lookup_dialog.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lookup_dialog.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__GenericLookupDialog_t {
    QByteArrayData data[5];
    char stringdata[70];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__GenericLookupDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__GenericLookupDialog_t qt_meta_stringdata_ScIDE__GenericLookupDialog = {
    {
QT_MOC_LITERAL(0, 0, 26), // "ScIDE::GenericLookupDialog"
QT_MOC_LITERAL(1, 27, 17), // "openDocumentation"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 10), // "onAccepted"
QT_MOC_LITERAL(4, 57, 12) // "performQuery"

    },
    "ScIDE::GenericLookupDialog\0openDocumentation\0"
    "\0onAccepted\0performQuery"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__GenericLookupDialog[] = {

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
       1,    0,   29,    2, 0x0a /* Public */,
       3,    1,   30,    2, 0x09 /* Protected */,
       4,    0,   33,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QModelIndex,    2,
    QMetaType::Void,

       0        // eod
};

void ScIDE::GenericLookupDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GenericLookupDialog *_t = static_cast<GenericLookupDialog *>(_o);
        switch (_id) {
        case 0: { bool _r = _t->openDocumentation();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: _t->onAccepted((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        case 2: _t->performQuery(); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::GenericLookupDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ScIDE__GenericLookupDialog.data,
      qt_meta_data_ScIDE__GenericLookupDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::GenericLookupDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::GenericLookupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__GenericLookupDialog.stringdata))
        return static_cast<void*>(const_cast< GenericLookupDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ScIDE::GenericLookupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_ScIDE__LookupDialog_t {
    QByteArrayData data[4];
    char stringdata[45];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__LookupDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__LookupDialog_t qt_meta_stringdata_ScIDE__LookupDialog = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ScIDE::LookupDialog"
QT_MOC_LITERAL(1, 20, 12), // "performQuery"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 10) // "onAccepted"

    },
    "ScIDE::LookupDialog\0performQuery\0\0"
    "onAccepted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__LookupDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    1,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QModelIndex,    2,

       0        // eod
};

void ScIDE::LookupDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LookupDialog *_t = static_cast<LookupDialog *>(_o);
        switch (_id) {
        case 0: _t->performQuery(); break;
        case 1: _t->onAccepted((*reinterpret_cast< QModelIndex(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::LookupDialog::staticMetaObject = {
    { &GenericLookupDialog::staticMetaObject, qt_meta_stringdata_ScIDE__LookupDialog.data,
      qt_meta_data_ScIDE__LookupDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::LookupDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::LookupDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__LookupDialog.stringdata))
        return static_cast<void*>(const_cast< LookupDialog*>(this));
    return GenericLookupDialog::qt_metacast(_clname);
}

int ScIDE::LookupDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GenericLookupDialog::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_ScIDE__SymbolReferenceRequest_t {
    QByteArrayData data[1];
    char stringdata[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__SymbolReferenceRequest_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__SymbolReferenceRequest_t qt_meta_stringdata_ScIDE__SymbolReferenceRequest = {
    {
QT_MOC_LITERAL(0, 0, 29) // "ScIDE::SymbolReferenceRequest"

    },
    "ScIDE::SymbolReferenceRequest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__SymbolReferenceRequest[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void ScIDE::SymbolReferenceRequest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::SymbolReferenceRequest::staticMetaObject = {
    { &ScRequest::staticMetaObject, qt_meta_stringdata_ScIDE__SymbolReferenceRequest.data,
      qt_meta_data_ScIDE__SymbolReferenceRequest,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::SymbolReferenceRequest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::SymbolReferenceRequest::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__SymbolReferenceRequest.stringdata))
        return static_cast<void*>(const_cast< SymbolReferenceRequest*>(this));
    return ScRequest::qt_metacast(_clname);
}

int ScIDE::SymbolReferenceRequest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ScRequest::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_ScIDE__ReferencesDialog_t {
    QByteArrayData data[9];
    char stringdata[124];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__ReferencesDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__ReferencesDialog_t qt_meta_stringdata_ScIDE__ReferencesDialog = {
    {
QT_MOC_LITERAL(0, 0, 23), // "ScIDE::ReferencesDialog"
QT_MOC_LITERAL(1, 24, 16), // "requestCancelled"
QT_MOC_LITERAL(2, 41, 0), // ""
QT_MOC_LITERAL(3, 42, 12), // "performQuery"
QT_MOC_LITERAL(4, 55, 21), // "onResposeFromLanguage"
QT_MOC_LITERAL(5, 77, 7), // "command"
QT_MOC_LITERAL(6, 85, 12), // "responseData"
QT_MOC_LITERAL(7, 98, 5), // "parse"
QT_MOC_LITERAL(8, 104, 19) // "QStandardItemModel*"

    },
    "ScIDE::ReferencesDialog\0requestCancelled\0"
    "\0performQuery\0onResposeFromLanguage\0"
    "command\0responseData\0parse\0"
    "QStandardItemModel*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__ReferencesDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    0,   35,    2, 0x08 /* Private */,
       4,    2,   36,    2, 0x08 /* Private */,
       7,    1,   41,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    5,    6,
    0x80000000 | 8, QMetaType::QString,    6,

       0        // eod
};

void ScIDE::ReferencesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ReferencesDialog *_t = static_cast<ReferencesDialog *>(_o);
        switch (_id) {
        case 0: _t->requestCancelled(); break;
        case 1: _t->performQuery(); break;
        case 2: _t->onResposeFromLanguage((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: { QStandardItemModel* _r = _t->parse((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QStandardItemModel**>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::ReferencesDialog::staticMetaObject = {
    { &LookupDialog::staticMetaObject, qt_meta_stringdata_ScIDE__ReferencesDialog.data,
      qt_meta_data_ScIDE__ReferencesDialog,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::ReferencesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::ReferencesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__ReferencesDialog.stringdata))
        return static_cast<void*>(const_cast< ReferencesDialog*>(this));
    return LookupDialog::qt_metacast(_clname);
}

int ScIDE::ReferencesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = LookupDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
