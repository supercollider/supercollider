/****************************************************************************
** Meta object code from reading C++ file 'layouts.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/layouts/layouts.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'layouts.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcHBoxLayout_t {
    QByteArrayData data[15];
    char stringdata[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcHBoxLayout_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcHBoxLayout_t qt_meta_stringdata_QcHBoxLayout = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcHBoxLayout"
QT_MOC_LITERAL(1, 13, 7), // "addItem"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "data"
QT_MOC_LITERAL(4, 27, 10), // "insertItem"
QT_MOC_LITERAL(5, 38, 10), // "setStretch"
QT_MOC_LITERAL(6, 49, 5), // "index"
QT_MOC_LITERAL(7, 55, 7), // "stretch"
QT_MOC_LITERAL(8, 63, 13), // "QObjectProxy*"
QT_MOC_LITERAL(9, 77, 1), // "p"
QT_MOC_LITERAL(10, 79, 12), // "setAlignment"
QT_MOC_LITERAL(11, 92, 1), // "i"
QT_MOC_LITERAL(12, 94, 1), // "a"
QT_MOC_LITERAL(13, 96, 5), // "items"
QT_MOC_LITERAL(14, 102, 7) // "margins"

    },
    "QcHBoxLayout\0addItem\0\0data\0insertItem\0"
    "setStretch\0index\0stretch\0QObjectProxy*\0"
    "p\0setAlignment\0i\0a\0items\0margins"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcHBoxLayout[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   73, // properties
       0,    0, // enums/sets
       1,   76, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x02 /* Public */,
       4,    1,   47,    2, 0x02 /* Public */,
       5,    2,   50,    2, 0x02 /* Public */,
       5,    2,   55,    2, 0x02 /* Public */,
      10,    2,   60,    2, 0x02 /* Public */,
      10,    2,   65,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantList,    3,
    QMetaType::Void, QMetaType::QVariantList,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    7,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   11,   12,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,   12,

 // constructors: parameters
    0x80000000 | 2, QMetaType::QVariantList,   13,

 // properties: name, type, flags
      14, QMetaType::QVariantList, 0x00095103,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   70,    2, 0x0e /* Public */,

       0        // eod
};

void QcHBoxLayout::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QcHBoxLayout *_r = new QcHBoxLayout((*reinterpret_cast< const QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        QcHBoxLayout *_t = static_cast<QcHBoxLayout *>(_o);
        switch (_id) {
        case 0: _t->addItem((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 1: _t->insertItem((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 2: _t->setStretch((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->setStretch((*reinterpret_cast< QObjectProxy*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->setAlignment((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->setAlignment((*reinterpret_cast< QObjectProxy*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QObjectProxy* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QObjectProxy* >(); break;
            }
            break;
        }
    }
}

const QMetaObject QcHBoxLayout::staticMetaObject = {
    { &QcBoxLayout<QHBoxLayout>::staticMetaObject, qt_meta_stringdata_QcHBoxLayout.data,
      qt_meta_data_QcHBoxLayout,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcHBoxLayout::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcHBoxLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcHBoxLayout.stringdata))
        return static_cast<void*>(const_cast< QcHBoxLayout*>(this));
    return QcBoxLayout<QHBoxLayout>::qt_metacast(_clname);
}

int QcHBoxLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcBoxLayout<QHBoxLayout>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = margins(); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMargins(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_QcVBoxLayout_t {
    QByteArrayData data[15];
    char stringdata[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcVBoxLayout_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcVBoxLayout_t qt_meta_stringdata_QcVBoxLayout = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcVBoxLayout"
QT_MOC_LITERAL(1, 13, 7), // "addItem"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "data"
QT_MOC_LITERAL(4, 27, 10), // "insertItem"
QT_MOC_LITERAL(5, 38, 10), // "setStretch"
QT_MOC_LITERAL(6, 49, 5), // "index"
QT_MOC_LITERAL(7, 55, 7), // "stretch"
QT_MOC_LITERAL(8, 63, 13), // "QObjectProxy*"
QT_MOC_LITERAL(9, 77, 1), // "p"
QT_MOC_LITERAL(10, 79, 12), // "setAlignment"
QT_MOC_LITERAL(11, 92, 1), // "i"
QT_MOC_LITERAL(12, 94, 1), // "a"
QT_MOC_LITERAL(13, 96, 5), // "items"
QT_MOC_LITERAL(14, 102, 7) // "margins"

    },
    "QcVBoxLayout\0addItem\0\0data\0insertItem\0"
    "setStretch\0index\0stretch\0QObjectProxy*\0"
    "p\0setAlignment\0i\0a\0items\0margins"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcVBoxLayout[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       1,   73, // properties
       0,    0, // enums/sets
       1,   76, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x02 /* Public */,
       4,    1,   47,    2, 0x02 /* Public */,
       5,    2,   50,    2, 0x02 /* Public */,
       5,    2,   55,    2, 0x02 /* Public */,
      10,    2,   60,    2, 0x02 /* Public */,
      10,    2,   65,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantList,    3,
    QMetaType::Void, QMetaType::QVariantList,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    6,    7,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,    7,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   11,   12,
    QMetaType::Void, 0x80000000 | 8, QMetaType::Int,    9,   12,

 // constructors: parameters
    0x80000000 | 2, QMetaType::QVariantList,   13,

 // properties: name, type, flags
      14, QMetaType::QVariantList, 0x00095103,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   70,    2, 0x0e /* Public */,

       0        // eod
};

void QcVBoxLayout::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QcVBoxLayout *_r = new QcVBoxLayout((*reinterpret_cast< const QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        QcVBoxLayout *_t = static_cast<QcVBoxLayout *>(_o);
        switch (_id) {
        case 0: _t->addItem((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 1: _t->insertItem((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 2: _t->setStretch((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->setStretch((*reinterpret_cast< QObjectProxy*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->setAlignment((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->setAlignment((*reinterpret_cast< QObjectProxy*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QObjectProxy* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QObjectProxy* >(); break;
            }
            break;
        }
    }
}

const QMetaObject QcVBoxLayout::staticMetaObject = {
    { &QcBoxLayout<QVBoxLayout>::staticMetaObject, qt_meta_stringdata_QcVBoxLayout.data,
      qt_meta_data_QcVBoxLayout,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcVBoxLayout::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcVBoxLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcVBoxLayout.stringdata))
        return static_cast<void*>(const_cast< QcVBoxLayout*>(this));
    return QcBoxLayout<QVBoxLayout>::qt_metacast(_clname);
}

int QcVBoxLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcBoxLayout<QVBoxLayout>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = margins(); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMargins(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_QcGridLayout_t {
    QByteArrayData data[25];
    char stringdata[227];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcGridLayout_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcGridLayout_t qt_meta_stringdata_QcGridLayout = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcGridLayout"
QT_MOC_LITERAL(1, 13, 7), // "addItem"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 8), // "dataList"
QT_MOC_LITERAL(4, 31, 13), // "setRowStretch"
QT_MOC_LITERAL(5, 45, 3), // "row"
QT_MOC_LITERAL(6, 49, 6), // "factor"
QT_MOC_LITERAL(7, 56, 16), // "setColumnStretch"
QT_MOC_LITERAL(8, 73, 6), // "column"
QT_MOC_LITERAL(9, 80, 12), // "setAlignment"
QT_MOC_LITERAL(10, 93, 1), // "r"
QT_MOC_LITERAL(11, 95, 1), // "c"
QT_MOC_LITERAL(12, 97, 1), // "a"
QT_MOC_LITERAL(13, 99, 13), // "QObjectProxy*"
QT_MOC_LITERAL(14, 113, 1), // "p"
QT_MOC_LITERAL(15, 115, 12), // "minRowHeight"
QT_MOC_LITERAL(16, 128, 14), // "minColumnWidth"
QT_MOC_LITERAL(17, 143, 3), // "col"
QT_MOC_LITERAL(18, 147, 15), // "setMinRowHeight"
QT_MOC_LITERAL(19, 163, 1), // "h"
QT_MOC_LITERAL(20, 165, 17), // "setMinColumnWidth"
QT_MOC_LITERAL(21, 183, 1), // "w"
QT_MOC_LITERAL(22, 185, 7), // "margins"
QT_MOC_LITERAL(23, 193, 15), // "verticalSpacing"
QT_MOC_LITERAL(24, 209, 17) // "horizontalSpacing"

    },
    "QcGridLayout\0addItem\0\0dataList\0"
    "setRowStretch\0row\0factor\0setColumnStretch\0"
    "column\0setAlignment\0r\0c\0a\0QObjectProxy*\0"
    "p\0minRowHeight\0minColumnWidth\0col\0"
    "setMinRowHeight\0h\0setMinColumnWidth\0"
    "w\0margins\0verticalSpacing\0horizontalSpacing"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcGridLayout[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       3,  100, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x02 /* Public */,
       4,    2,   62,    2, 0x02 /* Public */,
       7,    2,   67,    2, 0x02 /* Public */,
       9,    3,   72,    2, 0x02 /* Public */,
       9,    2,   79,    2, 0x02 /* Public */,
      15,    1,   84,    2, 0x02 /* Public */,
      16,    1,   87,    2, 0x02 /* Public */,
      18,    2,   90,    2, 0x02 /* Public */,
      20,    2,   95,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, QMetaType::QVariantList,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    8,    6,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   10,   11,   12,
    QMetaType::Void, 0x80000000 | 13, QMetaType::Int,   14,   12,
    QMetaType::Int, QMetaType::Int,    5,
    QMetaType::Int, QMetaType::Int,   17,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   17,   21,

 // properties: name, type, flags
      22, QMetaType::QVariantList, 0x00095103,
      23, QMetaType::Int, 0x00095103,
      24, QMetaType::Int, 0x00095103,

       0        // eod
};

void QcGridLayout::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcGridLayout *_t = static_cast<QcGridLayout *>(_o);
        switch (_id) {
        case 0: _t->addItem((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 1: _t->setRowStretch((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->setColumnStretch((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 3: _t->setAlignment((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 4: _t->setAlignment((*reinterpret_cast< QObjectProxy*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: { int _r = _t->minRowHeight((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 6: { int _r = _t->minColumnWidth((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: _t->setMinRowHeight((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->setMinColumnWidth((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 4:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QObjectProxy* >(); break;
            }
            break;
        }
    }
}

const QMetaObject QcGridLayout::staticMetaObject = {
    { &QcLayout<QGridLayout>::staticMetaObject, qt_meta_stringdata_QcGridLayout.data,
      qt_meta_data_QcGridLayout,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcGridLayout::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcGridLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcGridLayout.stringdata))
        return static_cast<void*>(const_cast< QcGridLayout*>(this));
    return QcLayout<QGridLayout>::qt_metacast(_clname);
}

int QcGridLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcLayout<QGridLayout>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = margins(); break;
        case 1: *reinterpret_cast< int*>(_v) = verticalSpacing(); break;
        case 2: *reinterpret_cast< int*>(_v) = horizontalSpacing(); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMargins(*reinterpret_cast< QVariantList*>(_v)); break;
        case 1: setVerticalSpacing(*reinterpret_cast< int*>(_v)); break;
        case 2: setHorizontalSpacing(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_QcStackLayout_t {
    QByteArrayData data[8];
    char stringdata[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcStackLayout_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcStackLayout_t qt_meta_stringdata_QcStackLayout = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QcStackLayout"
QT_MOC_LITERAL(1, 14, 12), // "insertWidget"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 5), // "index"
QT_MOC_LITERAL(4, 34, 13), // "QObjectProxy*"
QT_MOC_LITERAL(5, 48, 5), // "proxy"
QT_MOC_LITERAL(6, 54, 5), // "items"
QT_MOC_LITERAL(7, 60, 7) // "margins"

    },
    "QcStackLayout\0insertWidget\0\0index\0"
    "QObjectProxy*\0proxy\0items\0margins"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcStackLayout[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       1,   27, // properties
       0,    0, // enums/sets
       1,   30, // constructors
       0,       // flags
       0,       // signalCount

 // methods: name, argc, parameters, tag, flags
       1,    2,   19,    2, 0x02 /* Public */,

 // methods: parameters
    QMetaType::Void, QMetaType::Int, 0x80000000 | 4,    3,    5,

 // constructors: parameters
    0x80000000 | 2, QMetaType::QVariantList,    6,

 // properties: name, type, flags
       7, QMetaType::QVariantList, 0x00095103,

 // constructors: name, argc, parameters, tag, flags
       0,    1,   24,    2, 0x0e /* Public */,

       0        // eod
};

void QcStackLayout::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::CreateInstance) {
        switch (_id) {
        case 0: { QcStackLayout *_r = new QcStackLayout((*reinterpret_cast< const QVariantList(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;
        default: break;
        }
    } else if (_c == QMetaObject::InvokeMetaMethod) {
        QcStackLayout *_t = static_cast<QcStackLayout *>(_o);
        switch (_id) {
        case 0: _t->insertWidget((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QObjectProxy*(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QObjectProxy* >(); break;
            }
            break;
        }
    }
}

const QMetaObject QcStackLayout::staticMetaObject = {
    { &QcLayout<QtCollider::StackLayout>::staticMetaObject, qt_meta_stringdata_QcStackLayout.data,
      qt_meta_data_QcStackLayout,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcStackLayout::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcStackLayout::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcStackLayout.stringdata))
        return static_cast<void*>(const_cast< QcStackLayout*>(this));
    return QcLayout<QtCollider::StackLayout>::qt_metacast(_clname);
}

int QcStackLayout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QcLayout<QtCollider::StackLayout>::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = margins(); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMargins(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
        _id -= 1;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE
