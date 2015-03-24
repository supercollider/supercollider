/****************************************************************************
** Meta object code from reading C++ file 'QcTextEdit.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcTextEdit.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcTextEdit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcTextEdit_t {
    QByteArrayData data[18];
    char stringdata[185];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcTextEdit_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcTextEdit_t qt_meta_stringdata_QcTextEdit = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QcTextEdit"
QT_MOC_LITERAL(1, 11, 9), // "interpret"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 4), // "code"
QT_MOC_LITERAL(4, 27, 6), // "select"
QT_MOC_LITERAL(5, 34, 5), // "start"
QT_MOC_LITERAL(6, 40, 4), // "size"
QT_MOC_LITERAL(7, 45, 8), // "document"
QT_MOC_LITERAL(8, 54, 14), // "selectionStart"
QT_MOC_LITERAL(9, 69, 13), // "selectionSize"
QT_MOC_LITERAL(10, 83, 14), // "selectedString"
QT_MOC_LITERAL(11, 98, 11), // "currentLine"
QT_MOC_LITERAL(12, 110, 8), // "textFont"
QT_MOC_LITERAL(13, 119, 9), // "textColor"
QT_MOC_LITERAL(14, 129, 10), // "rangeColor"
QT_MOC_LITERAL(15, 140, 9), // "rangeFont"
QT_MOC_LITERAL(16, 150, 9), // "rangeText"
QT_MOC_LITERAL(17, 160, 24) // "enterInterpretsSelection"

    },
    "QcTextEdit\0interpret\0\0code\0select\0"
    "start\0size\0document\0selectionStart\0"
    "selectionSize\0selectedString\0currentLine\0"
    "textFont\0textColor\0rangeColor\0rangeFont\0"
    "rangeText\0enterInterpretsSelection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcTextEdit[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
      11,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       4,    2,   27,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,

 // methods: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    5,    6,

 // properties: name, type, flags
       7, QMetaType::QString, 0x00095103,
       8, QMetaType::Int, 0x00095001,
       9, QMetaType::Int, 0x00095001,
      10, QMetaType::QString, 0x00095003,
      11, QMetaType::QString, 0x00095001,
      12, QMetaType::QFont, 0x00095103,
      13, QMetaType::QColor, 0x00095103,
      14, QMetaType::QVariantList, 0x00095103,
      15, QMetaType::QVariantList, 0x00095103,
      16, QMetaType::QVariantList, 0x00095103,
      17, QMetaType::Bool, 0x00095003,

       0        // eod
};

void QcTextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcTextEdit *_t = static_cast<QcTextEdit *>(_o);
        switch (_id) {
        case 0: _t->interpret((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->select((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcTextEdit::*_t)(const QString & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcTextEdit::interpret)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject QcTextEdit::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_QcTextEdit.data,
      qt_meta_data_QcTextEdit,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcTextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcTextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcTextEdit.stringdata))
        return static_cast<void*>(const_cast< QcTextEdit*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcTextEdit*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int QcTextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
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
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = documentFilename(); break;
        case 1: *reinterpret_cast< int*>(_v) = selectionStart(); break;
        case 2: *reinterpret_cast< int*>(_v) = selectionSize(); break;
        case 3: *reinterpret_cast< QString*>(_v) = selectedString(); break;
        case 4: *reinterpret_cast< QString*>(_v) = currentLine(); break;
        case 5: *reinterpret_cast< QFont*>(_v) = dummyFont(); break;
        case 6: *reinterpret_cast< QColor*>(_v) = dummyColor(); break;
        case 7: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 8: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 9: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 10: *reinterpret_cast< bool*>(_v) = interpretSelection(); break;
        default: break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setDocument(*reinterpret_cast< QString*>(_v)); break;
        case 3: replaceSelectedText(*reinterpret_cast< QString*>(_v)); break;
        case 5: setTextFont(*reinterpret_cast< QFont*>(_v)); break;
        case 6: setTextColor(*reinterpret_cast< QColor*>(_v)); break;
        case 7: setRangeColor(*reinterpret_cast< QVariantList*>(_v)); break;
        case 8: setRangeFont(*reinterpret_cast< QVariantList*>(_v)); break;
        case 9: setRangeText(*reinterpret_cast< QVariantList*>(_v)); break;
        case 10: setInterpretSelection(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
        _id -= 11;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 11;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 11;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcTextEdit::interpret(const QString & _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
