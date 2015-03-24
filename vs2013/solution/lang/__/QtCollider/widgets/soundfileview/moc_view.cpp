/****************************************************************************
** Meta object code from reading C++ file 'view.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../QtCollider/widgets/soundfileview/view.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'view.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QcWaveform_t {
    QByteArrayData data[68];
    char stringdata[711];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QcWaveform_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QcWaveform_t qt_meta_stringdata_QcWaveform = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QcWaveform"
QT_MOC_LITERAL(1, 11, 12), // "loadProgress"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 11), // "loadingDone"
QT_MOC_LITERAL(4, 37, 6), // "action"
QT_MOC_LITERAL(5, 44, 10), // "metaAction"
QT_MOC_LITERAL(6, 55, 6), // "zoomTo"
QT_MOC_LITERAL(7, 62, 8), // "fraction"
QT_MOC_LITERAL(8, 71, 6), // "zoomBy"
QT_MOC_LITERAL(9, 78, 6), // "factor"
QT_MOC_LITERAL(10, 85, 10), // "zoomAllOut"
QT_MOC_LITERAL(11, 96, 13), // "zoomSelection"
QT_MOC_LITERAL(12, 110, 14), // "selectionIndex"
QT_MOC_LITERAL(13, 125, 8), // "scrollTo"
QT_MOC_LITERAL(14, 134, 5), // "frame"
QT_MOC_LITERAL(15, 140, 8), // "scrollBy"
QT_MOC_LITERAL(16, 149, 6), // "frames"
QT_MOC_LITERAL(17, 156, 12), // "setScrollPos"
QT_MOC_LITERAL(18, 169, 13), // "scrollToStart"
QT_MOC_LITERAL(19, 183, 11), // "scrollToEnd"
QT_MOC_LITERAL(20, 195, 8), // "setYZoom"
QT_MOC_LITERAL(21, 204, 8), // "setXZoom"
QT_MOC_LITERAL(22, 213, 7), // "seconds"
QT_MOC_LITERAL(23, 221, 6), // "redraw"
QT_MOC_LITERAL(24, 228, 4), // "load"
QT_MOC_LITERAL(25, 233, 8), // "filename"
QT_MOC_LITERAL(26, 242, 9), // "beginning"
QT_MOC_LITERAL(27, 252, 8), // "duration"
QT_MOC_LITERAL(28, 261, 15), // "QVector<double>"
QT_MOC_LITERAL(29, 277, 4), // "data"
QT_MOC_LITERAL(30, 282, 6), // "offset"
QT_MOC_LITERAL(31, 289, 8), // "channels"
QT_MOC_LITERAL(32, 298, 10), // "samplerate"
QT_MOC_LITERAL(33, 309, 8), // "allocate"
QT_MOC_LITERAL(34, 318, 5), // "write"
QT_MOC_LITERAL(35, 324, 9), // "selection"
QT_MOC_LITERAL(36, 334, 5), // "index"
QT_MOC_LITERAL(37, 340, 12), // "setSelection"
QT_MOC_LITERAL(38, 353, 17), // "setSelectionStart"
QT_MOC_LITERAL(39, 371, 1), // "i"
QT_MOC_LITERAL(40, 373, 10), // "sf_count_t"
QT_MOC_LITERAL(41, 384, 15), // "setSelectionEnd"
QT_MOC_LITERAL(42, 400, 20), // "setSelectionEditable"
QT_MOC_LITERAL(43, 421, 8), // "editable"
QT_MOC_LITERAL(44, 430, 17), // "setSelectionColor"
QT_MOC_LITERAL(45, 448, 3), // "clr"
QT_MOC_LITERAL(46, 452, 12), // "readProgress"
QT_MOC_LITERAL(47, 465, 10), // "startFrame"
QT_MOC_LITERAL(48, 476, 10), // "viewFrames"
QT_MOC_LITERAL(49, 487, 14), // "viewStartFrame"
QT_MOC_LITERAL(50, 502, 9), // "scrollPos"
QT_MOC_LITERAL(51, 512, 16), // "currentSelection"
QT_MOC_LITERAL(52, 529, 10), // "selections"
QT_MOC_LITERAL(53, 540, 5), // "yZoom"
QT_MOC_LITERAL(54, 546, 5), // "xZoom"
QT_MOC_LITERAL(55, 552, 13), // "cursorVisible"
QT_MOC_LITERAL(56, 566, 14), // "cursorEditable"
QT_MOC_LITERAL(57, 581, 14), // "cursorPosition"
QT_MOC_LITERAL(58, 596, 11), // "gridVisible"
QT_MOC_LITERAL(59, 608, 10), // "gridOffset"
QT_MOC_LITERAL(60, 619, 14), // "gridResolution"
QT_MOC_LITERAL(61, 634, 13), // "drawsWaveform"
QT_MOC_LITERAL(62, 648, 10), // "background"
QT_MOC_LITERAL(63, 659, 9), // "peakColor"
QT_MOC_LITERAL(64, 669, 8), // "rmsColor"
QT_MOC_LITERAL(65, 678, 11), // "cursorColor"
QT_MOC_LITERAL(66, 690, 9), // "gridColor"
QT_MOC_LITERAL(67, 700, 10) // "waveColors"

    },
    "QcWaveform\0loadProgress\0\0loadingDone\0"
    "action\0metaAction\0zoomTo\0fraction\0"
    "zoomBy\0factor\0zoomAllOut\0zoomSelection\0"
    "selectionIndex\0scrollTo\0frame\0scrollBy\0"
    "frames\0setScrollPos\0scrollToStart\0"
    "scrollToEnd\0setYZoom\0setXZoom\0seconds\0"
    "redraw\0load\0filename\0beginning\0duration\0"
    "QVector<double>\0data\0offset\0channels\0"
    "samplerate\0allocate\0write\0selection\0"
    "index\0setSelection\0setSelectionStart\0"
    "i\0sf_count_t\0setSelectionEnd\0"
    "setSelectionEditable\0editable\0"
    "setSelectionColor\0clr\0readProgress\0"
    "startFrame\0viewFrames\0viewStartFrame\0"
    "scrollPos\0currentSelection\0selections\0"
    "yZoom\0xZoom\0cursorVisible\0cursorEditable\0"
    "cursorPosition\0gridVisible\0gridOffset\0"
    "gridResolution\0drawsWaveform\0background\0"
    "peakColor\0rmsColor\0cursorColor\0gridColor\0"
    "waveColors"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QcWaveform[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      32,   14, // methods
      23,  290, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  174,    2, 0x06 /* Public */,
       3,    0,  177,    2, 0x06 /* Public */,
       4,    0,  178,    2, 0x06 /* Public */,
       5,    0,  179,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,  180,    2, 0x0a /* Public */,
       8,    1,  183,    2, 0x0a /* Public */,
      10,    0,  186,    2, 0x0a /* Public */,
      11,    1,  187,    2, 0x0a /* Public */,
      13,    1,  190,    2, 0x0a /* Public */,
      15,    1,  193,    2, 0x0a /* Public */,
      17,    1,  196,    2, 0x0a /* Public */,
      18,    0,  199,    2, 0x0a /* Public */,
      19,    0,  200,    2, 0x0a /* Public */,
      20,    1,  201,    2, 0x0a /* Public */,
      21,    1,  204,    2, 0x0a /* Public */,
      23,    0,  207,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      24,    1,  208,    2, 0x02 /* Public */,
      24,    3,  211,    2, 0x02 /* Public */,
      24,    4,  218,    2, 0x02 /* Public */,
      24,    3,  227,    2, 0x22 /* Public | MethodCloned */,
      24,    2,  234,    2, 0x22 /* Public | MethodCloned */,
      24,    1,  239,    2, 0x22 /* Public | MethodCloned */,
      33,    3,  242,    2, 0x02 /* Public */,
      33,    2,  249,    2, 0x22 /* Public | MethodCloned */,
      33,    1,  254,    2, 0x22 /* Public | MethodCloned */,
      34,    2,  257,    2, 0x02 /* Public */,
      35,    1,  262,    2, 0x02 /* Public */,
      37,    2,  265,    2, 0x02 /* Public */,
      38,    2,  270,    2, 0x02 /* Public */,
      41,    2,  275,    2, 0x02 /* Public */,
      42,    2,  280,    2, 0x02 /* Public */,
      44,    2,  285,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Double,   14,
    QMetaType::Void, QMetaType::Double,   16,
    QMetaType::Void, QMetaType::Double,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,    9,
    QMetaType::Void, QMetaType::Double,   22,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::QString,   25,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int,   25,   26,   27,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int, QMetaType::Int, QMetaType::Int,   29,   30,   31,   32,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int, QMetaType::Int,   29,   30,   31,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   16,   31,   32,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   16,   31,
    QMetaType::Void, QMetaType::Int,   16,
    QMetaType::Void, 0x80000000 | 28, QMetaType::Int,   29,   30,
    QMetaType::QVariantList, QMetaType::Int,   36,
    QMetaType::Void, QMetaType::Int, QMetaType::QVariantList,   36,   29,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 40,   39,   14,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 40,   39,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::Bool,   36,   43,
    QMetaType::Void, QMetaType::Int, QMetaType::QColor,   36,   45,

 // properties: name, type, flags
      46, QMetaType::Float, 0x00095001,
      47, QMetaType::Int, 0x00095001,
      16, QMetaType::Int, 0x00095001,
      48, QMetaType::Double, 0x00095001,
      49, QMetaType::Double, 0x00095003,
      50, QMetaType::Float, 0x00095103,
      51, QMetaType::Int, 0x00095103,
      52, QMetaType::QVariantList, 0x00095001,
      53, QMetaType::Float, 0x00095103,
      54, QMetaType::Float, 0x00095103,
      55, QMetaType::Bool, 0x00095103,
      56, QMetaType::Bool, 0x00095103,
      57, QMetaType::Int, 0x00095103,
      58, QMetaType::Bool, 0x00095103,
      59, QMetaType::Float, 0x00095103,
      60, QMetaType::Float, 0x00095103,
      61, QMetaType::Bool, 0x00095103,
      62, QMetaType::QColor, 0x00095103,
      63, QMetaType::QColor, 0x00095103,
      64, QMetaType::QColor, 0x00095103,
      65, QMetaType::QColor, 0x00095103,
      66, QMetaType::QColor, 0x00095103,
      67, QMetaType::QVariantList, 0x00095103,

       0        // eod
};

void QcWaveform::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QcWaveform *_t = static_cast<QcWaveform *>(_o);
        switch (_id) {
        case 0: _t->loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->loadingDone(); break;
        case 2: _t->action(); break;
        case 3: _t->metaAction(); break;
        case 4: _t->zoomTo((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: _t->zoomBy((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: _t->zoomAllOut(); break;
        case 7: _t->zoomSelection((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->scrollTo((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->scrollBy((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 10: _t->setScrollPos((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 11: _t->scrollToStart(); break;
        case 12: _t->scrollToEnd(); break;
        case 13: _t->setYZoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 14: _t->setXZoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 15: _t->redraw(); break;
        case 16: _t->load((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->load((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 18: _t->load((*reinterpret_cast< const QVector<double>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 19: _t->load((*reinterpret_cast< const QVector<double>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 20: _t->load((*reinterpret_cast< const QVector<double>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->load((*reinterpret_cast< const QVector<double>(*)>(_a[1]))); break;
        case 22: _t->allocate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 23: _t->allocate((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 24: _t->allocate((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 25: _t->write((*reinterpret_cast< const QVector<double>(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 26: { QVariantList _r = _t->selection((*reinterpret_cast< int(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariantList*>(_a[0]) = _r; }  break;
        case 27: _t->setSelection((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QVariantList(*)>(_a[2]))); break;
        case 28: _t->setSelectionStart((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< sf_count_t(*)>(_a[2]))); break;
        case 29: _t->setSelectionEnd((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< sf_count_t(*)>(_a[2]))); break;
        case 30: _t->setSelectionEditable((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 31: _t->setSelectionColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QColor(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 18:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 19:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 20:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 21:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        case 25:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<double> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (QcWaveform::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcWaveform::loadProgress)) {
                *result = 0;
            }
        }
        {
            typedef void (QcWaveform::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcWaveform::loadingDone)) {
                *result = 1;
            }
        }
        {
            typedef void (QcWaveform::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcWaveform::action)) {
                *result = 2;
            }
        }
        {
            typedef void (QcWaveform::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&QcWaveform::metaAction)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject QcWaveform::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QcWaveform.data,
      qt_meta_data_QcWaveform,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QcWaveform::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QcWaveform::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QcWaveform.stringdata))
        return static_cast<void*>(const_cast< QcWaveform*>(this));
    if (!strcmp(_clname, "QcHelper"))
        return static_cast< QcHelper*>(const_cast< QcWaveform*>(this));
    return QWidget::qt_metacast(_clname);
}

int QcWaveform::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 32)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 32;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< float*>(_v) = loadProgress(); break;
        case 1: *reinterpret_cast< int*>(_v) = startFrame(); break;
        case 2: *reinterpret_cast< int*>(_v) = frames(); break;
        case 3: *reinterpret_cast< double*>(_v) = viewFrames(); break;
        case 4: *reinterpret_cast< double*>(_v) = viewStartFrame(); break;
        case 5: *reinterpret_cast< float*>(_v) = scrollPos(); break;
        case 6: *reinterpret_cast< int*>(_v) = currentSelection(); break;
        case 7: *reinterpret_cast< QVariantList*>(_v) = selections(); break;
        case 8: *reinterpret_cast< float*>(_v) = yZoom(); break;
        case 9: *reinterpret_cast< float*>(_v) = xZoom(); break;
        case 10: *reinterpret_cast< bool*>(_v) = cursorVisible(); break;
        case 11: *reinterpret_cast< bool*>(_v) = cursorEditable(); break;
        case 12: *reinterpret_cast< int*>(_v) = cursorPosition(); break;
        case 13: *reinterpret_cast< bool*>(_v) = gridVisible(); break;
        case 14: *reinterpret_cast< float*>(_v) = gridOffset(); break;
        case 15: *reinterpret_cast< float*>(_v) = gridResolution(); break;
        case 16: *reinterpret_cast< bool*>(_v) = drawsWaveform(); break;
        case 17: *reinterpret_cast< QColor*>(_v) = background(); break;
        case 18: *reinterpret_cast< QColor*>(_v) = peakColor(); break;
        case 19: *reinterpret_cast< QColor*>(_v) = rmsColor(); break;
        case 20: *reinterpret_cast< QColor*>(_v) = cursorColor(); break;
        case 21: *reinterpret_cast< QColor*>(_v) = gridColor(); break;
        case 22: *reinterpret_cast< QVariantList*>(_v) = waveColors(); break;
        default: break;
        }
        _id -= 23;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 4: scrollTo(*reinterpret_cast< double*>(_v)); break;
        case 5: setScrollPos(*reinterpret_cast< float*>(_v)); break;
        case 6: setCurrentSelection(*reinterpret_cast< int*>(_v)); break;
        case 8: setYZoom(*reinterpret_cast< float*>(_v)); break;
        case 9: setXZoom(*reinterpret_cast< float*>(_v)); break;
        case 10: setCursorVisible(*reinterpret_cast< bool*>(_v)); break;
        case 11: setCursorEditable(*reinterpret_cast< bool*>(_v)); break;
        case 12: setCursorPosition(*reinterpret_cast< int*>(_v)); break;
        case 13: setGridVisible(*reinterpret_cast< bool*>(_v)); break;
        case 14: setGridOffset(*reinterpret_cast< float*>(_v)); break;
        case 15: setGridResolution(*reinterpret_cast< float*>(_v)); break;
        case 16: setDrawsWaveform(*reinterpret_cast< bool*>(_v)); break;
        case 17: setBackground(*reinterpret_cast< QColor*>(_v)); break;
        case 18: setPeakColor(*reinterpret_cast< QColor*>(_v)); break;
        case 19: setRmsColor(*reinterpret_cast< QColor*>(_v)); break;
        case 20: setCursorColor(*reinterpret_cast< QColor*>(_v)); break;
        case 21: setGridColor(*reinterpret_cast< QColor*>(_v)); break;
        case 22: setWaveColors(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
        _id -= 23;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 23;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 23;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 23;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 23;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 23;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 23;
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void QcWaveform::loadProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QcWaveform::loadingDone()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void QcWaveform::action()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void QcWaveform::metaAction()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
struct qt_meta_stringdata_SoundCacheStream_t {
    QByteArrayData data[6];
    char stringdata[72];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SoundCacheStream_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SoundCacheStream_t qt_meta_stringdata_SoundCacheStream = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SoundCacheStream"
QT_MOC_LITERAL(1, 17, 12), // "loadProgress"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 11), // "loadingDone"
QT_MOC_LITERAL(4, 43, 14), // "onLoadProgress"
QT_MOC_LITERAL(5, 58, 13) // "onLoadingDone"

    },
    "SoundCacheStream\0loadProgress\0\0"
    "loadingDone\0onLoadProgress\0onLoadingDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SoundCacheStream[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       3,    0,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   38,    2, 0x08 /* Private */,
       5,    0,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void SoundCacheStream::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SoundCacheStream *_t = static_cast<SoundCacheStream *>(_o);
        switch (_id) {
        case 0: _t->loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->loadingDone(); break;
        case 2: _t->onLoadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onLoadingDone(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SoundCacheStream::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SoundCacheStream::loadProgress)) {
                *result = 0;
            }
        }
        {
            typedef void (SoundCacheStream::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SoundCacheStream::loadingDone)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject SoundCacheStream::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SoundCacheStream.data,
      qt_meta_data_SoundCacheStream,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SoundCacheStream::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SoundCacheStream::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SoundCacheStream.stringdata))
        return static_cast<void*>(const_cast< SoundCacheStream*>(this));
    if (!strcmp(_clname, "SoundStream"))
        return static_cast< SoundStream*>(const_cast< SoundCacheStream*>(this));
    return QObject::qt_metacast(_clname);
}

int SoundCacheStream::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void SoundCacheStream::loadProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SoundCacheStream::loadingDone()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
struct qt_meta_stringdata_SoundCacheLoader_t {
    QByteArrayData data[4];
    char stringdata[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SoundCacheLoader_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SoundCacheLoader_t qt_meta_stringdata_SoundCacheLoader = {
    {
QT_MOC_LITERAL(0, 0, 16), // "SoundCacheLoader"
QT_MOC_LITERAL(1, 17, 12), // "loadProgress"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 11) // "loadingDone"

    },
    "SoundCacheLoader\0loadProgress\0\0"
    "loadingDone"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SoundCacheLoader[] = {

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
       3,    0,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void SoundCacheLoader::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SoundCacheLoader *_t = static_cast<SoundCacheLoader *>(_o);
        switch (_id) {
        case 0: _t->loadProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->loadingDone(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SoundCacheLoader::*_t)(int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SoundCacheLoader::loadProgress)) {
                *result = 0;
            }
        }
        {
            typedef void (SoundCacheLoader::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SoundCacheLoader::loadingDone)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject SoundCacheLoader::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_SoundCacheLoader.data,
      qt_meta_data_SoundCacheLoader,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SoundCacheLoader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SoundCacheLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SoundCacheLoader.stringdata))
        return static_cast<void*>(const_cast< SoundCacheLoader*>(this));
    return QThread::qt_metacast(_clname);
}

int SoundCacheLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
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
void SoundCacheLoader::loadProgress(int _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SoundCacheLoader::loadingDone()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
