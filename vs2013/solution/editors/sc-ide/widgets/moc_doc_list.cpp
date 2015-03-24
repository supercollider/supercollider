/****************************************************************************
** Meta object code from reading C++ file 'doc_list.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/doc_list.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'doc_list.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__DocumentListWidget_t {
    QByteArrayData data[11];
    char stringdata[132];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__DocumentListWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__DocumentListWidget_t qt_meta_stringdata_ScIDE__DocumentListWidget = {
    {
QT_MOC_LITERAL(0, 0, 25), // "ScIDE::DocumentListWidget"
QT_MOC_LITERAL(1, 26, 7), // "clicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 9), // "Document*"
QT_MOC_LITERAL(4, 45, 10), // "setCurrent"
QT_MOC_LITERAL(5, 56, 6), // "onOpen"
QT_MOC_LITERAL(6, 63, 7), // "onClose"
QT_MOC_LITERAL(7, 71, 7), // "onSaved"
QT_MOC_LITERAL(8, 79, 21), // "onModificationChanged"
QT_MOC_LITERAL(9, 101, 13), // "onItemClicked"
QT_MOC_LITERAL(10, 115, 16) // "QListWidgetItem*"

    },
    "ScIDE::DocumentListWidget\0clicked\0\0"
    "Document*\0setCurrent\0onOpen\0onClose\0"
    "onSaved\0onModificationChanged\0"
    "onItemClicked\0QListWidgetItem*"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__DocumentListWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   52,    2, 0x0a /* Public */,
       5,    3,   55,    2, 0x08 /* Private */,
       6,    1,   62,    2, 0x08 /* Private */,
       7,    1,   65,    2, 0x08 /* Private */,
       8,    1,   68,    2, 0x08 /* Private */,
       9,    1,   71,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::QObjectStar,    2,
    QMetaType::Void, 0x80000000 | 10,    2,

       0        // eod
};

void ScIDE::DocumentListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DocumentListWidget *_t = static_cast<DocumentListWidget *>(_o);
        switch (_id) {
        case 0: _t->clicked((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 1: _t->setCurrent((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 2: _t->onOpen((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->onClose((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 4: _t->onSaved((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 5: _t->onModificationChanged((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 6: _t->onItemClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
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
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (DocumentListWidget::*_t)(Document * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&DocumentListWidget::clicked)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::DocumentListWidget::staticMetaObject = {
    { &QListWidget::staticMetaObject, qt_meta_stringdata_ScIDE__DocumentListWidget.data,
      qt_meta_data_ScIDE__DocumentListWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::DocumentListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::DocumentListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__DocumentListWidget.stringdata))
        return static_cast<void*>(const_cast< DocumentListWidget*>(this));
    return QListWidget::qt_metacast(_clname);
}

int ScIDE::DocumentListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::DocumentListWidget::clicked(Document * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_ScIDE__DocumentsDocklet_t {
    QByteArrayData data[1];
    char stringdata[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__DocumentsDocklet_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__DocumentsDocklet_t qt_meta_stringdata_ScIDE__DocumentsDocklet = {
    {
QT_MOC_LITERAL(0, 0, 23) // "ScIDE::DocumentsDocklet"

    },
    "ScIDE::DocumentsDocklet"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__DocumentsDocklet[] = {

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

void ScIDE::DocumentsDocklet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::DocumentsDocklet::staticMetaObject = {
    { &Docklet::staticMetaObject, qt_meta_stringdata_ScIDE__DocumentsDocklet.data,
      qt_meta_data_ScIDE__DocumentsDocklet,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::DocumentsDocklet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::DocumentsDocklet::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__DocumentsDocklet.stringdata))
        return static_cast<void*>(const_cast< DocumentsDocklet*>(this));
    return Docklet::qt_metacast(_clname);
}

int ScIDE::DocumentsDocklet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Docklet::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
