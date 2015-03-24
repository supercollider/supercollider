/****************************************************************************
** Meta object code from reading C++ file 'main_window.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../editors/sc-ide/widgets/main_window.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_ScIDE__MainWindow_t {
    QByteArrayData data[58];
    char stringdata[947];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ScIDE__MainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ScIDE__MainWindow_t qt_meta_stringdata_ScIDE__MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 17), // "ScIDE::MainWindow"
QT_MOC_LITERAL(1, 18, 12), // "evaluateCode"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 6), // "silent"
QT_MOC_LITERAL(4, 39, 10), // "newSession"
QT_MOC_LITERAL(5, 50, 20), // "saveCurrentSessionAs"
QT_MOC_LITERAL(6, 71, 18), // "openSessionsDialog"
QT_MOC_LITERAL(7, 90, 11), // "newDocument"
QT_MOC_LITERAL(8, 102, 12), // "openDocument"
QT_MOC_LITERAL(9, 115, 12), // "saveDocument"
QT_MOC_LITERAL(10, 128, 14), // "saveDocumentAs"
QT_MOC_LITERAL(11, 143, 23), // "saveDocumentAsExtension"
QT_MOC_LITERAL(12, 167, 16), // "saveAllDocuments"
QT_MOC_LITERAL(13, 184, 14), // "reloadDocument"
QT_MOC_LITERAL(14, 199, 13), // "closeDocument"
QT_MOC_LITERAL(15, 213, 17), // "closeAllDocuments"
QT_MOC_LITERAL(16, 231, 11), // "showCmdLine"
QT_MOC_LITERAL(17, 243, 12), // "showFindTool"
QT_MOC_LITERAL(18, 256, 15), // "showReplaceTool"
QT_MOC_LITERAL(19, 272, 16), // "showGoToLineTool"
QT_MOC_LITERAL(20, 289, 11), // "hideToolBox"
QT_MOC_LITERAL(21, 301, 12), // "showSettings"
QT_MOC_LITERAL(22, 314, 17), // "showStatusMessage"
QT_MOC_LITERAL(23, 332, 6), // "string"
QT_MOC_LITERAL(24, 339, 15), // "openStartupFile"
QT_MOC_LITERAL(25, 355, 24), // "openUserSupportDirectory"
QT_MOC_LITERAL(26, 380, 13), // "switchSession"
QT_MOC_LITERAL(27, 394, 8), // "Session*"
QT_MOC_LITERAL(28, 403, 7), // "session"
QT_MOC_LITERAL(29, 411, 11), // "saveSession"
QT_MOC_LITERAL(30, 423, 25), // "onInterpreterStateChanged"
QT_MOC_LITERAL(31, 449, 22), // "QProcess::ProcessState"
QT_MOC_LITERAL(32, 472, 6), // "onQuit"
QT_MOC_LITERAL(33, 479, 24), // "onCurrentDocumentChanged"
QT_MOC_LITERAL(34, 504, 9), // "Document*"
QT_MOC_LITERAL(35, 514, 27), // "onDocumentChangedExternally"
QT_MOC_LITERAL(36, 542, 19), // "onDocDialogFinished"
QT_MOC_LITERAL(37, 562, 20), // "updateRecentDocsMenu"
QT_MOC_LITERAL(38, 583, 20), // "onOpenRecentDocument"
QT_MOC_LITERAL(39, 604, 8), // "QAction*"
QT_MOC_LITERAL(40, 613, 19), // "onOpenSessionAction"
QT_MOC_LITERAL(41, 633, 17), // "updateWindowTitle"
QT_MOC_LITERAL(42, 651, 16), // "toggleFullScreen"
QT_MOC_LITERAL(43, 668, 20), // "lookupImplementation"
QT_MOC_LITERAL(44, 689, 29), // "lookupImplementationForCursor"
QT_MOC_LITERAL(45, 719, 16), // "lookupReferences"
QT_MOC_LITERAL(46, 736, 25), // "lookupReferencesForCursor"
QT_MOC_LITERAL(47, 762, 8), // "openHelp"
QT_MOC_LITERAL(48, 771, 16), // "openHelpAboutIDE"
QT_MOC_LITERAL(49, 788, 28), // "lookupDocumentationForCursor"
QT_MOC_LITERAL(50, 817, 19), // "lookupDocumentation"
QT_MOC_LITERAL(51, 837, 13), // "applySettings"
QT_MOC_LITERAL(52, 851, 18), // "Settings::Manager*"
QT_MOC_LITERAL(53, 870, 13), // "storeSettings"
QT_MOC_LITERAL(54, 884, 23), // "showSwitchSessionDialog"
QT_MOC_LITERAL(55, 908, 9), // "showAbout"
QT_MOC_LITERAL(56, 918, 11), // "showAboutQT"
QT_MOC_LITERAL(57, 930, 16) // "cmdLineForCursor"

    },
    "ScIDE::MainWindow\0evaluateCode\0\0silent\0"
    "newSession\0saveCurrentSessionAs\0"
    "openSessionsDialog\0newDocument\0"
    "openDocument\0saveDocument\0saveDocumentAs\0"
    "saveDocumentAsExtension\0saveAllDocuments\0"
    "reloadDocument\0closeDocument\0"
    "closeAllDocuments\0showCmdLine\0"
    "showFindTool\0showReplaceTool\0"
    "showGoToLineTool\0hideToolBox\0showSettings\0"
    "showStatusMessage\0string\0openStartupFile\0"
    "openUserSupportDirectory\0switchSession\0"
    "Session*\0session\0saveSession\0"
    "onInterpreterStateChanged\0"
    "QProcess::ProcessState\0onQuit\0"
    "onCurrentDocumentChanged\0Document*\0"
    "onDocumentChangedExternally\0"
    "onDocDialogFinished\0updateRecentDocsMenu\0"
    "onOpenRecentDocument\0QAction*\0"
    "onOpenSessionAction\0updateWindowTitle\0"
    "toggleFullScreen\0lookupImplementation\0"
    "lookupImplementationForCursor\0"
    "lookupReferences\0lookupReferencesForCursor\0"
    "openHelp\0openHelpAboutIDE\0"
    "lookupDocumentationForCursor\0"
    "lookupDocumentation\0applySettings\0"
    "Settings::Manager*\0storeSettings\0"
    "showSwitchSessionDialog\0showAbout\0"
    "showAboutQT\0cmdLineForCursor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ScIDE__MainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      50,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,  264,    2, 0x06 /* Public */,
       1,    1,  269,    2, 0x26 /* Public | MethodCloned */,

 // slots: name, argc, parameters, tag, flags
       4,    0,  272,    2, 0x0a /* Public */,
       5,    0,  273,    2, 0x0a /* Public */,
       6,    0,  274,    2, 0x0a /* Public */,
       7,    0,  275,    2, 0x0a /* Public */,
       8,    0,  276,    2, 0x0a /* Public */,
       9,    0,  277,    2, 0x0a /* Public */,
      10,    0,  278,    2, 0x0a /* Public */,
      11,    0,  279,    2, 0x0a /* Public */,
      12,    0,  280,    2, 0x0a /* Public */,
      13,    0,  281,    2, 0x0a /* Public */,
      14,    0,  282,    2, 0x0a /* Public */,
      15,    0,  283,    2, 0x0a /* Public */,
      16,    0,  284,    2, 0x0a /* Public */,
      16,    1,  285,    2, 0x0a /* Public */,
      17,    0,  288,    2, 0x0a /* Public */,
      18,    0,  289,    2, 0x0a /* Public */,
      19,    0,  290,    2, 0x0a /* Public */,
      20,    0,  291,    2, 0x0a /* Public */,
      21,    0,  292,    2, 0x0a /* Public */,
      22,    1,  293,    2, 0x0a /* Public */,
      24,    0,  296,    2, 0x08 /* Private */,
      25,    0,  297,    2, 0x08 /* Private */,
      26,    1,  298,    2, 0x08 /* Private */,
      29,    1,  301,    2, 0x08 /* Private */,
      30,    1,  304,    2, 0x08 /* Private */,
      32,    0,  307,    2, 0x08 /* Private */,
      33,    1,  308,    2, 0x08 /* Private */,
      35,    1,  311,    2, 0x08 /* Private */,
      36,    0,  314,    2, 0x08 /* Private */,
      37,    0,  315,    2, 0x08 /* Private */,
      38,    1,  316,    2, 0x08 /* Private */,
      40,    1,  319,    2, 0x08 /* Private */,
      41,    0,  322,    2, 0x08 /* Private */,
      42,    0,  323,    2, 0x08 /* Private */,
      43,    0,  324,    2, 0x08 /* Private */,
      44,    0,  325,    2, 0x08 /* Private */,
      45,    0,  326,    2, 0x08 /* Private */,
      46,    0,  327,    2, 0x08 /* Private */,
      47,    0,  328,    2, 0x08 /* Private */,
      48,    0,  329,    2, 0x08 /* Private */,
      49,    0,  330,    2, 0x08 /* Private */,
      50,    0,  331,    2, 0x08 /* Private */,
      51,    1,  332,    2, 0x08 /* Private */,
      53,    1,  335,    2, 0x08 /* Private */,
      54,    0,  338,    2, 0x08 /* Private */,
      55,    0,  339,    2, 0x08 /* Private */,
      56,    0,  340,    2, 0x08 /* Private */,
      57,    0,  341,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    3,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   23,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, 0x80000000 | 27,   28,
    QMetaType::Void, 0x80000000 | 31,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 34,    2,
    QMetaType::Void, 0x80000000 | 34,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 39,    2,
    QMetaType::Void, 0x80000000 | 39,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 52,    2,
    QMetaType::Void, 0x80000000 | 52,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ScIDE::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->evaluateCode((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->evaluateCode((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->newSession(); break;
        case 3: _t->saveCurrentSessionAs(); break;
        case 4: _t->openSessionsDialog(); break;
        case 5: _t->newDocument(); break;
        case 6: _t->openDocument(); break;
        case 7: _t->saveDocument(); break;
        case 8: _t->saveDocumentAs(); break;
        case 9: _t->saveDocumentAsExtension(); break;
        case 10: _t->saveAllDocuments(); break;
        case 11: _t->reloadDocument(); break;
        case 12: _t->closeDocument(); break;
        case 13: _t->closeAllDocuments(); break;
        case 14: _t->showCmdLine(); break;
        case 15: _t->showCmdLine((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->showFindTool(); break;
        case 17: _t->showReplaceTool(); break;
        case 18: _t->showGoToLineTool(); break;
        case 19: _t->hideToolBox(); break;
        case 20: _t->showSettings(); break;
        case 21: _t->showStatusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 22: _t->openStartupFile(); break;
        case 23: _t->openUserSupportDirectory(); break;
        case 24: _t->switchSession((*reinterpret_cast< Session*(*)>(_a[1]))); break;
        case 25: _t->saveSession((*reinterpret_cast< Session*(*)>(_a[1]))); break;
        case 26: _t->onInterpreterStateChanged((*reinterpret_cast< QProcess::ProcessState(*)>(_a[1]))); break;
        case 27: _t->onQuit(); break;
        case 28: _t->onCurrentDocumentChanged((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 29: _t->onDocumentChangedExternally((*reinterpret_cast< Document*(*)>(_a[1]))); break;
        case 30: _t->onDocDialogFinished(); break;
        case 31: _t->updateRecentDocsMenu(); break;
        case 32: _t->onOpenRecentDocument((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 33: _t->onOpenSessionAction((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 34: _t->updateWindowTitle(); break;
        case 35: _t->toggleFullScreen(); break;
        case 36: _t->lookupImplementation(); break;
        case 37: _t->lookupImplementationForCursor(); break;
        case 38: _t->lookupReferences(); break;
        case 39: _t->lookupReferencesForCursor(); break;
        case 40: _t->openHelp(); break;
        case 41: _t->openHelpAboutIDE(); break;
        case 42: _t->lookupDocumentationForCursor(); break;
        case 43: _t->lookupDocumentation(); break;
        case 44: _t->applySettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 45: _t->storeSettings((*reinterpret_cast< Settings::Manager*(*)>(_a[1]))); break;
        case 46: _t->showSwitchSessionDialog(); break;
        case 47: _t->showAbout(); break;
        case 48: _t->showAboutQT(); break;
        case 49: _t->cmdLineForCursor(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (MainWindow::*_t)(const QString & , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&MainWindow::evaluateCode)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject ScIDE::MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_ScIDE__MainWindow.data,
      qt_meta_data_ScIDE__MainWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *ScIDE::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ScIDE::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_ScIDE__MainWindow.stringdata))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int ScIDE::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 50)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 50;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 50)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 50;
    }
    return _id;
}

// SIGNAL 0
void ScIDE::MainWindow::evaluateCode(const QString & _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
