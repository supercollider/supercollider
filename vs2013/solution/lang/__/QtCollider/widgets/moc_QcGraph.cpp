/****************************************************************************
** Meta object code from reading C++ file 'QcGraph.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../QtCollider/widgets/QcGraph.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QcGraph.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcGraphModel_t {
    QByteArrayData data[5];
    char stringdata[47];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcGraphModel_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcGraphModel_t qt_meta_stringdata_QcGraphModel = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QcGraphModel"
QT_MOC_LITERAL(1, 13, 8), // "appended"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 15), // "QcGraphElement*"
QT_MOC_LITERAL(4, 39, 7) // "removed"

    },
    "QcGraphModel\0appended\0\0QcGraphElement*\0"
    "removed"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcGraphModel[] = {

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
       1,    1,   24,    2, 0x06 /* Public */,
       4,    1,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,

       0        // eod
};

void QcGraphModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcGraphModel *_t = static_cast<QcGraphModel *>(_o);
        switch (_id) {
        case 0: _t->appended((*reinterpret_cast< QcGraphElement*(*)>(_a[1]))); break;
        case 1: _t->removed((*reinterpret_cast< QcGraphElement*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcGraphModel::*_t)(QcGraphElement * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcGraphModel::appended)) {
                *result = 0;
            }
        }
        {
            typedef void (QcGraphModel::*_t)(QcGraphElement * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcGraphModel::removed)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QcGraphModel::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QcGraphModel.data,
      qt_meta_data_QcGraphModel,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcGraphModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcGraphModel::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcGraphModel.stringdata))
        return static_cast<void*>(const_cast< QcGraphModel*>(this));
    return QObject::qt_metacast(_clname);
}

int QcGraphModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void QcGraphModel::appended(QcGraphElement * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QcGraphModel::removed(QcGraphElement * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_QcGraph_t {
    QByteArrayData data[50];
    char stringdata[510];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcGraph_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcGraph_t qt_meta_stringdata_QcGraph = {
    {
QT_MOC_LITERAL(0, 0, 7), // "QcGraph"
QT_MOC_LITERAL(1, 8, 6), // "action"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 10), // "metaAction"
QT_MOC_LITERAL(4, 27, 6), // "select"
QT_MOC_LITERAL(5, 34, 5), // "index"
QT_MOC_LITERAL(6, 40, 9), // "exclusive"
QT_MOC_LITERAL(7, 50, 8), // "deselect"
QT_MOC_LITERAL(8, 59, 11), // "deselectAll"
QT_MOC_LITERAL(9, 71, 16), // "onElementRemoved"
QT_MOC_LITERAL(10, 88, 15), // "QcGraphElement*"
QT_MOC_LITERAL(11, 104, 1), // "e"
QT_MOC_LITERAL(12, 106, 15), // "connectElements"
QT_MOC_LITERAL(13, 122, 11), // "setStringAt"
QT_MOC_LITERAL(14, 134, 14), // "setFillColorAt"
QT_MOC_LITERAL(15, 149, 13), // "setEditableAt"
QT_MOC_LITERAL(16, 163, 16), // "setThumbHeightAt"
QT_MOC_LITERAL(17, 180, 15), // "setThumbWidthAt"
QT_MOC_LITERAL(18, 196, 14), // "setThumbSizeAt"
QT_MOC_LITERAL(19, 211, 9), // "setCurves"
QT_MOC_LITERAL(20, 221, 9), // "curvature"
QT_MOC_LITERAL(21, 231, 4), // "type"
QT_MOC_LITERAL(22, 236, 6), // "curves"
QT_MOC_LITERAL(23, 243, 5), // "value"
QT_MOC_LITERAL(24, 249, 7), // "strings"
QT_MOC_LITERAL(25, 257, 9), // "lastIndex"
QT_MOC_LITERAL(26, 267, 16), // "selectionIndexes"
QT_MOC_LITERAL(27, 284, 9), // "thumbSize"
QT_MOC_LITERAL(28, 294, 10), // "thumbWidth"
QT_MOC_LITERAL(29, 305, 11), // "thumbHeight"
QT_MOC_LITERAL(30, 317, 10), // "background"
QT_MOC_LITERAL(31, 328, 11), // "strokeColor"
QT_MOC_LITERAL(32, 340, 9), // "fillColor"
QT_MOC_LITERAL(33, 350, 9), // "gridColor"
QT_MOC_LITERAL(34, 360, 10), // "focusColor"
QT_MOC_LITERAL(35, 371, 14), // "selectionColor"
QT_MOC_LITERAL(36, 386, 9), // "drawLines"
QT_MOC_LITERAL(37, 396, 9), // "drawRects"
QT_MOC_LITERAL(38, 406, 5), // "style"
QT_MOC_LITERAL(39, 412, 12), // "ElementStyle"
QT_MOC_LITERAL(40, 425, 8), // "editable"
QT_MOC_LITERAL(41, 434, 4), // "step"
QT_MOC_LITERAL(42, 439, 13), // "selectionForm"
QT_MOC_LITERAL(43, 453, 15), // "horizontalOrder"
QT_MOC_LITERAL(44, 469, 1), // "x"
QT_MOC_LITERAL(45, 471, 1), // "y"
QT_MOC_LITERAL(46, 473, 4), // "grid"
QT_MOC_LITERAL(47, 478, 6), // "gridOn"
QT_MOC_LITERAL(48, 485, 11), // "DotElements"
QT_MOC_LITERAL(49, 497, 12) // "RectElements"

    },
    "QcGraph\0action\0\0metaAction\0select\0"
    "index\0exclusive\0deselect\0deselectAll\0"
    "onElementRemoved\0QcGraphElement*\0e\0"
    "connectElements\0setStringAt\0setFillColorAt\0"
    "setEditableAt\0setThumbHeightAt\0"
    "setThumbWidthAt\0setThumbSizeAt\0setCurves\0"
    "curvature\0type\0curves\0value\0strings\0"
    "lastIndex\0selectionIndexes\0thumbSize\0"
    "thumbWidth\0thumbHeight\0background\0"
    "strokeColor\0fillColor\0gridColor\0"
    "focusColor\0selectionColor\0drawLines\0"
    "drawRects\0style\0ElementStyle\0editable\0"
    "step\0selectionForm\0horizontalOrder\0x\0"
    "y\0grid\0gridOn\0DotElements\0RectElements"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcGraph[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
      25,  160, // properties
       1,  235, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   99,    2, 0x06 /* Public */,
       3,    0,  100,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    2,  101,    2, 0x0a /* Public */,
       4,    1,  106,    2, 0x2a /* Public | MethodCloned */,
       7,    1,  109,    2, 0x0a /* Public */,
       8,    0,  112,    2, 0x0a /* Public */,
       9,    1,  113,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      12,    2,  116,    2, 0x02 /* Public */,
      13,    2,  121,    2, 0x02 /* Public */,
      14,    2,  126,    2, 0x02 /* Public */,
      15,    2,  131,    2, 0x02 /* Public */,
      16,    2,  136,    2, 0x02 /* Public */,
      17,    2,  141,    2, 0x02 /* Public */,
      18,    2,  146,    2, 0x02 /* Public */,
      19,    1,  151,    2, 0x02 /* Public */,
      19,    1,  154,    2, 0x02 /* Public */,
      19,    1,  157,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    5,    6,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,

 // methods: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QVariantList,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    2,    2,
    QMetaType::Void, QMetaType::Double,   20,
    QMetaType::Void, QMetaType::Int,   21,
    QMetaType::Void, QMetaType::QVariantList,   22,

 // properties: name, type, flags
      23, QMetaType::QVariantList, 0x00095103,
      24, QMetaType::QVariantList, 0x00095103,
       5, QMetaType::Int, 0x00095103,
      25, QMetaType::Int, 0x00095001,
      26, QMetaType::QVariantList, 0x00095001,
      27, QMetaType::Int, 0x00095103,
      28, QMetaType::Int, 0x00095103,
      29, QMetaType::Int, 0x00095103,
      30, QMetaType::QColor, 0x00095103,
      31, QMetaType::QColor, 0x00095103,
      32, QMetaType::QColor, 0x00095103,
      33, QMetaType::QColor, 0x00095103,
      34, QMetaType::QColor, 0x00095103,
      35, QMetaType::QColor, 0x00095103,
      36, QMetaType::Bool, 0x00095103,
      37, QMetaType::Bool, 0x00095103,
      38, 0x80000000 | 39, 0x0009500b,
      40, QMetaType::Bool, 0x00095103,
      41, QMetaType::Double, 0x00095103,
      42, QMetaType::Int, 0x00095103,
      43, QMetaType::Int, 0x00095103,
      44, QMetaType::Float, 0x00095003,
      45, QMetaType::Float, 0x00095003,
      46, QMetaType::QPointF, 0x00095103,
      47, QMetaType::Bool, 0x00095103,

 // enums: name, flags, count, data
      39, 0x0,    2,  239,

 // enum data: key, value
      48, uint(QcGraph::DotElements),
      49, uint(QcGraph::RectElements),

       0        // eod
};

void QcGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcGraph *_t = static_cast<QcGraph *>(_o);
        switch (_id) {
        case 0: _t->action(); break;
        case 1: _t->metaAction(); break;
        case 2: _t->select((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 3: _t->select((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->deselect((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->deselectAll(); break;
        case 6: _t->onElementRemoved((*reinterpret_cast< QcGraphElement*(*)>(_a[1]))); break;
        case 7: _t->connectElements((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVariantList(*)>(_a[2]))); break;
        case 8: _t->setStringAt((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 9: _t->setFillColorAt((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        case 10: _t->setEditableAt((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 11: _t->setThumbHeightAt((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->setThumbWidthAt((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 13: _t->setThumbSizeAt((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 14: _t->setCurves((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->setCurves((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->setCurves((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcGraph::action)) {
                *result = 0;
            }
        }
        {
            typedef void (QcGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcGraph::metaAction)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject QcGraph::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcGraph.data,
      qt_meta_data_QcGraph,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcGraph.stringdata))
        return static_cast<void*>(const_cast< QcGraph*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcGraph*>(this));
    if (!strcmp(_clname, "QtCollider::Style::Client"))
        return static_cast< QtCollider::Style::Client*>(const_cast< QcGraph*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 17)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 17;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QVariantList*>(_v) = value(); break;
        case 1: *reinterpret_cast< QVariantList*>(_v) = dummyVariantList(); break;
        case 2: *reinterpret_cast< int*>(_v) = index(); break;
        case 3: *reinterpret_cast< int*>(_v) = lastIndex(); break;
        case 4: *reinterpret_cast< QVariantList*>(_v) = selectionIndexes(); break;
        case 5: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 6: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 7: *reinterpret_cast< int*>(_v) = dummyInt(); break;
        case 8: *reinterpret_cast< QColor*>(_v) = background(); break;
        case 9: *reinterpret_cast< QColor*>(_v) = strokeColor(); break;
        case 10: *reinterpret_cast< QColor*>(_v) = dummyColor(); break;
        case 11: *reinterpret_cast< QColor*>(_v) = gridColor(); break;
        case 12: *reinterpret_cast< QColor*>(_v) = focusColor(); break;
        case 13: *reinterpret_cast< QColor*>(_v) = selectionColor(); break;
        case 14: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 15: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 16: *reinterpret_cast< ElementStyle*>(_v) = elementStyle(); break;
        case 17: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        case 18: *reinterpret_cast< double*>(_v) = step(); break;
        case 19: *reinterpret_cast< int*>(_v) = selectionForm(); break;
        case 20: *reinterpret_cast< int*>(_v) = horizontalOrder(); break;
        case 21: *reinterpret_cast< float*>(_v) = currentX(); break;
        case 22: *reinterpret_cast< float*>(_v) = currentY(); break;
        case 23: *reinterpret_cast< QPointF*>(_v) = grid(); break;
        case 24: *reinterpret_cast< bool*>(_v) = dummyBool(); break;
        default: break;
        }
        _id -= 25;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setValue(*reinterpret_cast< QVariantList*>(_v)); break;
        case 1: setStrings(*reinterpret_cast< QVariantList*>(_v)); break;
        case 2: setIndex(*reinterpret_cast< int*>(_v)); break;
        case 5: setThumbSize(*reinterpret_cast< int*>(_v)); break;
        case 6: setThumbWidth(*reinterpret_cast< int*>(_v)); break;
        case 7: setThumbHeight(*reinterpret_cast< int*>(_v)); break;
        case 8: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 9: setStrokeColor(*reinterpret_cast< QColor*>(_v)); break;
        case 10: setFillColor(*reinterpret_cast< QColor*>(_v)); break;
        case 11: setGridColor(*reinterpret_cast< QColor*>(_v)); break;
        case 12: setFocusColor(*reinterpret_cast< QColor*>(_v)); break;
        case 13: setSelectionColor(*reinterpret_cast< QColor*>(_v)); break;
        case 14: setDrawLines(*reinterpret_cast< bool*>(_v)); break;
        case 15: setDrawRects(*reinterpret_cast< bool*>(_v)); break;
        case 16: setElementStyle(*reinterpret_cast< ElementStyle*>(_v)); break;
        case 17: setEditable(*reinterpret_cast< bool*>(_v)); break;
        case 18: setStep(*reinterpret_cast< double*>(_v)); break;
        case 19: setSelectionForm(*reinterpret_cast< int*>(_v)); break;
        case 20: setHorizontalOrder(*reinterpret_cast< int*>(_v)); break;
        case 21: setCurrentX(*reinterpret_cast< float*>(_v)); break;
        case 22: setCurrentY(*reinterpret_cast< float*>(_v)); break;
        case 23: setGrid(*reinterpret_cast< QPointF*>(_v)); break;
        case 24: setGridOn(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
        _id -= 25;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 25;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 25;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 25;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 25;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 25;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 25;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 25)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 25;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcGraph::action()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void QcGraph::metaAction()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
