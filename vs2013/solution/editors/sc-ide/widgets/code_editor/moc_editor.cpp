/****************************************************************************
** Meta object code from reading C++ file 'editor.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../editors/sc-ide/widgets/code_editor/editor.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editor.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__GenericCodeEditor_t {
    QByteArrayData data[26];
    char stringdata[405];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__GenericCodeEditor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__GenericCodeEditor_t qt_meta_stringdata_ScIDE__GenericCodeEditor = {
    {
QT_MOC_LITERAL(0, 0, 24), // "ScIDE::GenericCodeEditor"
QT_MOC_LITERAL(1, 25, 13), // "applySettings"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 18), // "Settings::Manager*"
QT_MOC_LITERAL(4, 59, 13), // "closeDocument"
QT_MOC_LITERAL(5, 73, 6), // "zoomIn"
QT_MOC_LITERAL(6, 80, 5), // "steps"
QT_MOC_LITERAL(7, 86, 7), // "zoomOut"
QT_MOC_LITERAL(8, 94, 13), // "resetFontSize"
QT_MOC_LITERAL(9, 108, 17), // "setShowWhitespace"
QT_MOC_LITERAL(10, 126, 17), // "setShowLinenumber"
QT_MOC_LITERAL(11, 144, 23), // "clearSearchHighlighting"
QT_MOC_LITERAL(12, 168, 19), // "toggleOverwriteMode"
QT_MOC_LITERAL(13, 188, 10), // "copyLineUp"
QT_MOC_LITERAL(14, 199, 12), // "copyLineDown"
QT_MOC_LITERAL(15, 212, 10), // "moveLineUp"
QT_MOC_LITERAL(16, 223, 12), // "moveLineDown"
QT_MOC_LITERAL(17, 236, 21), // "gotoPreviousEmptyLine"
QT_MOC_LITERAL(18, 258, 17), // "gotoNextEmptyLine"
QT_MOC_LITERAL(19, 276, 19), // "setActiveAppearance"
QT_MOC_LITERAL(20, 296, 6), // "active"
QT_MOC_LITERAL(21, 303, 22), // "updateDocLastSelection"
QT_MOC_LITERAL(22, 326, 12), // "updateLayout"
QT_MOC_LITERAL(23, 339, 19), // "updateLineIndicator"
QT_MOC_LITERAL(24, 359, 21), // "onDocumentFontChanged"
QT_MOC_LITERAL(25, 381, 23) // "onCursorPositionChanged"

    },
    "ScIDE::GenericCodeEditor\0applySettings\0"
    "\0Settings::Manager*\0closeDocument\0"
    "zoomIn\0steps\0zoomOut\0resetFontSize\0"
    "setShowWhitespace\0setShowLinenumber\0"
    "clearSearchHighlighting\0toggleOverwriteMode\0"
    "copyLineUp\0copyLineDown\0moveLineUp\0"
    "moveLineDown\0gotoPreviousEmptyLine\0"
    "gotoNextEmptyLine\0setActiveAppearance\0"
    "active\0updateDocLastSelection\0"
    "updateLayout\0updateLineIndicator\0"
    "onDocumentFontChanged\0onCursorPositionChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__GenericCodeEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  129,    2, 0x0a /* Public */,
       4,    0,  132,    2, 0x0a /* Public */,
       5,    1,  133,    2, 0x0a /* Public */,
       5,    0,  136,    2, 0x2a /* Public | MethodCloned */,
       7,    1,  137,    2, 0x0a /* Public */,
       7,    0,  140,    2, 0x2a /* Public | MethodCloned */,
       8,    0,  141,    2, 0x0a /* Public */,
       9,    1,  142,    2, 0x0a /* Public */,
      10,    1,  145,    2, 0x0a /* Public */,
      11,    0,  148,    2, 0x0a /* Public */,
      12,    0,  149,    2, 0x0a /* Public */,
      13,    0,  150,    2, 0x0a /* Public */,
      14,    0,  151,    2, 0x0a /* Public */,
      15,    0,  152,    2, 0x0a /* Public */,
      16,    0,  153,    2, 0x0a /* Public */,
      17,    0,  154,    2, 0x0a /* Public */,
      18,    0,  155,    2, 0x0a /* Public */,
      19,    1,  156,    2, 0x0a /* Public */,
      21,    0,  159,    2, 0x0a /* Public */,
      22,    0,  160,    2, 0x09 /* Protected */,
      23,    2,  161,    2, 0x09 /* Protected */,
      24,    0,  166,    2, 0x09 /* Protected */,
      25,    0,  167,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRect, QMetaType::Int,    2,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::GenericCodeEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GenericCodeEditor *_t = static_cast<GenericCodeEditor *>(_o);
        switch (_id) {
        case 0: _t->applySettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 1: _t->closeDocument(); break;
        case 2: _t->zoomIn((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->zoomIn(); break;
        case 4: _t->zoomOut((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->zoomOut(); break;
        case 6: _t->resetFontSize(); break;
        case 7: _t->setShowWhitespace((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->setShowLinenumber((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->clearSearchHighlighting(); break;
        case 10: _t->toggleOverwriteMode(); break;
        case 11: _t->copyLineUp(); break;
        case 12: _t->copyLineDown(); break;
        case 13: _t->moveLineUp(); break;
        case 14: _t->moveLineDown(); break;
        case 15: _t->gotoPreviousEmptyLine(); break;
        case 16: _t->gotoNextEmptyLine(); break;
        case 17: _t->setActiveAppearance((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->updateDocLastSelection(); break;
        case 19: _t->updateLayout(); break;
        case 20: _t->updateLineIndicator((*reinterpret_cast< QRect(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->onDocumentFontChanged(); break;
        case 22: _t->onCursorPositionChanged(); break;
        default: ;
        }
    }
}

const QMetaObject ScIDE::GenericCodeEditor::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_ScIDE__GenericCodeEditor.data,
      qt_meta_data_ScIDE__GenericCodeEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::GenericCodeEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::GenericCodeEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__GenericCodeEditor.stringdata))
        return static_cast<void*>(const_cast< GenericCodeEditor*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int ScIDE::GenericCodeEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
