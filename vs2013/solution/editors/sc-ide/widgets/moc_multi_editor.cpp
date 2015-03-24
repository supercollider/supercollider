/****************************************************************************
** Meta object code from reading C++ file 'multi_editor.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/multi_editor.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'multi_editor.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__EditorTabBar_t {
    QByteArrayData data[5];
    char stringdata[71];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__EditorTabBar_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__EditorTabBar_t qt_meta_stringdata_ScIDE__EditorTabBar = {
    {
QT_MOC_LITERAL(0, 0, 19), // "ScIDE::EditorTabBar"
QT_MOC_LITERAL(1, 20, 10), // "onCloseTab"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "onCloseOtherTabs"
QT_MOC_LITERAL(4, 49, 21) // "onCloseTabsToTheRight"

    },
    "ScIDE::EditorTabBar\0onCloseTab\0\0"
    "onCloseOtherTabs\0onCloseTabsToTheRight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__EditorTabBar[] = {

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
       1,    0,   29,    2, 0x08 /* Private */,
       3,    0,   30,    2, 0x08 /* Private */,
       4,    0,   31,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::EditorTabBar::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        EditorTabBar *_t = static_cast<EditorTabBar *>(_o);
        switch (_id) {
        case 0: _t->onCloseTab(); break;
        case 1: _t->onCloseOtherTabs(); break;
        case 2: _t->onCloseTabsToTheRight(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject ScIDE::EditorTabBar::staticMetaObject = {
    { &QTabBar::staticMetaObject, qt_meta_stringdata_ScIDE__EditorTabBar.data,
      qt_meta_data_ScIDE__EditorTabBar,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::EditorTabBar::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::EditorTabBar::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__EditorTabBar.stringdata))
        return static_cast<void*>(const_cast< EditorTabBar*>(this));
    return QTabBar::qt_metacast(_clname);
}

int ScIDE::EditorTabBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabBar::qt_metacall(_c, _id, _a);
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
struct qt_meta_stringdata_ScIDE__MultiEditor_t {
    QByteArrayData data[33];
    char stringdata[481];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__MultiEditor_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__MultiEditor_t qt_meta_stringdata_ScIDE__MultiEditor = {
    {
QT_MOC_LITERAL(0, 0, 18), // "ScIDE::MultiEditor"
QT_MOC_LITERAL(1, 19, 22), // "currentDocumentChanged"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 9), // "Document*"
QT_MOC_LITERAL(4, 53, 10), // "setCurrent"
QT_MOC_LITERAL(5, 64, 16), // "showNextDocument"
QT_MOC_LITERAL(6, 81, 20), // "showPreviousDocument"
QT_MOC_LITERAL(7, 102, 14), // "switchDocument"
QT_MOC_LITERAL(8, 117, 17), // "splitHorizontally"
QT_MOC_LITERAL(9, 135, 15), // "splitVertically"
QT_MOC_LITERAL(10, 151, 18), // "removeCurrentSplit"
QT_MOC_LITERAL(11, 170, 15), // "removeAllSplits"
QT_MOC_LITERAL(12, 186, 17), // "setShowWhitespace"
QT_MOC_LITERAL(13, 204, 2), // "on"
QT_MOC_LITERAL(14, 207, 17), // "setShowLinenumber"
QT_MOC_LITERAL(15, 225, 13), // "applySettings"
QT_MOC_LITERAL(16, 239, 18), // "Settings::Manager*"
QT_MOC_LITERAL(17, 258, 6), // "onOpen"
QT_MOC_LITERAL(18, 265, 21), // "initialCursorPosition"
QT_MOC_LITERAL(19, 287, 15), // "selectionLength"
QT_MOC_LITERAL(20, 303, 7), // "onClose"
QT_MOC_LITERAL(21, 311, 4), // "show"
QT_MOC_LITERAL(22, 316, 14), // "cursorPosition"
QT_MOC_LITERAL(23, 331, 15), // "selectionLenght"
QT_MOC_LITERAL(24, 347, 6), // "update"
QT_MOC_LITERAL(25, 354, 14), // "onCloseRequest"
QT_MOC_LITERAL(26, 369, 5), // "index"
QT_MOC_LITERAL(27, 375, 19), // "onCurrentTabChanged"
QT_MOC_LITERAL(28, 395, 22), // "onCurrentEditorChanged"
QT_MOC_LITERAL(29, 418, 18), // "GenericCodeEditor*"
QT_MOC_LITERAL(30, 437, 14), // "onBoxActivated"
QT_MOC_LITERAL(31, 452, 14), // "CodeEditorBox*"
QT_MOC_LITERAL(32, 467, 13) // "onDocModified"

    },
    "ScIDE::MultiEditor\0currentDocumentChanged\0"
    "\0Document*\0setCurrent\0showNextDocument\0"
    "showPreviousDocument\0switchDocument\0"
    "splitHorizontally\0splitVertically\0"
    "removeCurrentSplit\0removeAllSplits\0"
    "setShowWhitespace\0on\0setShowLinenumber\0"
    "applySettings\0Settings::Manager*\0"
    "onOpen\0initialCursorPosition\0"
    "selectionLength\0onClose\0show\0"
    "cursorPosition\0selectionLenght\0update\0"
    "onCloseRequest\0index\0onCurrentTabChanged\0"
    "onCurrentEditorChanged\0GenericCodeEditor*\0"
    "onBoxActivated\0CodeEditorBox*\0"
    "onDocModified"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__MultiEditor[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  129,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,  132,    2, 0x0a /* Public */,
       5,    0,  135,    2, 0x0a /* Public */,
       6,    0,  136,    2, 0x0a /* Public */,
       7,    0,  137,    2, 0x0a /* Public */,
       8,    0,  138,    2, 0x0a /* Public */,
       9,    0,  139,    2, 0x0a /* Public */,
      10,    0,  140,    2, 0x0a /* Public */,
      11,    0,  141,    2, 0x0a /* Public */,
      12,    1,  142,    2, 0x0a /* Public */,
      14,    1,  145,    2, 0x0a /* Public */,
      15,    1,  148,    2, 0x08 /* Private */,
      17,    3,  151,    2, 0x08 /* Private */,
      20,    1,  158,    2, 0x08 /* Private */,
      21,    3,  161,    2, 0x08 /* Private */,
      21,    2,  168,    2, 0x28 /* Private | MethodCloned */,
      21,    1,  173,    2, 0x28 /* Private | MethodCloned */,
      24,    1,  176,    2, 0x08 /* Private */,
      25,    1,  179,    2, 0x08 /* Private */,
      27,    1,  182,    2, 0x08 /* Private */,
      28,    1,  185,    2, 0x08 /* Private */,
      30,    1,  188,    2, 0x08 /* Private */,
      32,    1,  191,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, QMetaType::Bool,   13,
    QMetaType::Void, 0x80000000 | 16,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    2,   18,   19,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int, QMetaType::Int,    2,   22,   23,
    QMetaType::Void, 0x80000000 | 3, QMetaType::Int,    2,   22,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void, 0x80000000 | 29,    2,
    QMetaType::Void, 0x80000000 | 31,    2,
    QMetaType::Void, QMetaType::QObjectStar,    2,

       0        // eod
};

void ScIDE::MultiEditor::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MultiEditor *_t = static_cast<MultiEditor *>(_o);
        switch (_id) {
        case 0: _t->currentDocumentChanged((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 1: _t->setCurrent((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 2: _t->showNextDocument(); break;
        case 3: _t->showPreviousDocument(); break;
        case 4: _t->switchDocument(); break;
        case 5: _t->splitHorizontally(); break;
        case 6: _t->splitVertically(); break;
        case 7: _t->removeCurrentSplit(); break;
        case 8: _t->removeAllSplits(); break;
        case 9: _t->setShowWhitespace((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->setShowLinenumber((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->applySettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 12: _t->onOpen((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 13: _t->onClose((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 14: _t->show((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 15: _t->show((*reinterpret_cast< Document*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 16: _t->show((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 17: _t->update((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 18: _t->onCloseRequest((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->onCurrentTabChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->onCurrentEditorChanged((*reinterpret_cast< GenericCodeEditor*(*)>(_a[1]))); break;
        case 21: _t->onBoxActivated((*reinterpret_cast< CodeEditorBox*(*)>(_a[1]))); break;
        case 22: _t->onDocModified((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MultiEditor::*_t)(Document * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MultiEditor::currentDocumentChanged)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::MultiEditor::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ScIDE__MultiEditor.data,
      qt_meta_data_ScIDE__MultiEditor,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::MultiEditor::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::MultiEditor::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__MultiEditor.stringdata))
        return static_cast<void*>(const_cast< MultiEditor*>(this));
    return QWidget::qt_metacast(_clname);
}

int ScIDE::MultiEditor::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void ScIDE::MultiEditor::currentDocumentChanged(Document * _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
