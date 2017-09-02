/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#define QT_NO_DEBUG_OUTPUT

#include "cmd_line.hpp"
#include "doc_list.hpp"
#include "documents_dialog.hpp"
#include "find_replace_tool.hpp"
#include "goto_line_tool.hpp"
#include "lookup_dialog.hpp"
#include "main_window.hpp"
#include "multi_editor.hpp"
#include "popup_text_input.hpp"
#include "post_window.hpp"
#include "help_browser.hpp"
#include "session_switch_dialog.hpp"
#include "sessions_dialog.hpp"
#include "tool_box.hpp"
#include "audio_status_box.hpp"
#include "lang_status_box.hpp"
#include "../core/main.hpp"
#include "../core/doc_manager.hpp"
#include "../core/session_manager.hpp"
#include "../core/sc_server.hpp"
#include "../core/util/standard_dirs.hpp"
#include "code_editor/sc_editor.hpp"
#include "settings/dialog.hpp"

#include "QtCollider/hacks/hacks_qt.hpp"

#include "SC_Version.hpp"

#include <QAction>
#include <QApplication>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPointer>
#include <QShortcut>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QUrl>
#include <QMimeData>
#include <QMetaMethod>

namespace ScIDE {

static QWidget * findFirstResponder
( QWidget *widget, const char * methodSignature, int & methodIndex )
{
    methodIndex = -1;
    while ( widget ) {
        methodIndex = widget->metaObject()->indexOfMethod( methodSignature );
        if (methodIndex != -1)
            break;
        if (widget->isWindow())
            break;
        widget = widget->parentWidget();
    }
    return widget;
}

static void invokeMethodOnFirstResponder(QByteArray const & signature)
{
    int methodIdx = -1;
    QWidget * widget = findFirstResponder(
                QApplication::focusWidget(), signature.constData(), methodIdx );
    if (widget && methodIdx != -1)
        widget->metaObject()->method(methodIdx).invoke( widget, Qt::DirectConnection );
}

MainWindow * MainWindow::mInstance = 0;

MainWindow::MainWindow(Main * main) :
    mMain(main),
    mClockLabel(0),
    mDocDialog(0)
{
    Q_ASSERT(!mInstance);
    mInstance = this;

    setAcceptDrops(true);

    // Construct status bar:

    mLangStatus = new LangStatusBox( main->scProcess() );
    mServerStatus = new AudioStatusBox( main->scServer() );

    mStatusBar = statusBar();
    mStatusBar->addPermanentWidget( new QLabel(tr("Interpreter:")) );
    mStatusBar->addPermanentWidget( mLangStatus );
    mStatusBar->addPermanentWidget( new QLabel(tr("Server:")) );
    mStatusBar->addPermanentWidget( mServerStatus );

    // Code editor
    mEditors = new MultiEditor(main);

    // Tools

    mCmdLine = new CmdLine(tr("Command Line:"));
    connect(mCmdLine, SIGNAL(invoked(QString, bool)),
            main->scProcess(), SLOT(evaluateCode(QString, bool)));

    mFindReplaceTool = new TextFindReplacePanel;

    mGoToLineTool = new GoToLineTool();
    connect(mGoToLineTool, SIGNAL(activated(int)), this, SLOT(hideToolBox()));

    mToolBox = new ToolBox;
    mToolBox->addWidget(mCmdLine);
    mToolBox->addWidget(mFindReplaceTool);
    mToolBox->addWidget(mGoToLineTool);
    mToolBox->hide();

    // Docks
    mDocumentsDocklet = new DocumentsDocklet(main->documentManager(), this);
    mDocumentsDocklet->setObjectName("documents-dock");
    addDockWidget(Qt::LeftDockWidgetArea, mDocumentsDocklet->dockWidget());
    mDocumentsDocklet->hide();

    mHelpBrowserDocklet = new HelpBrowserDocklet(this);
    mHelpBrowserDocklet->setObjectName("help-dock");
    addDockWidget(Qt::RightDockWidgetArea, mHelpBrowserDocklet->dockWidget());
    //mHelpBrowserDockable->hide();

    mPostDocklet = new PostDocklet(this);
    mPostDocklet->setObjectName("post-dock");
    addDockWidget(Qt::RightDockWidgetArea, mPostDocklet->dockWidget());

    // Layout
    QVBoxLayout *center_box = new QVBoxLayout;
    center_box->setContentsMargins(0,0,0,0);
    center_box->setSpacing(0);
    center_box->addWidget(mEditors);
    center_box->addWidget(mToolBox);

    QWidget *central = new QWidget;
    central->setLayout(center_box);
    setCentralWidget(central);

    // Session management
    connect(main->sessionManager(), SIGNAL(saveSessionRequest(Session*)),
            this, SLOT(saveSession(Session*)));
    connect(main->sessionManager(), SIGNAL(switchSessionRequest(Session*)),
            this, SLOT(switchSession(Session*)));
    connect(main->sessionManager(), SIGNAL(currentSessionNameChanged()),
            this, SLOT(updateWindowTitle()));
    // A system for easy evaluation of pre-defined code:
    connect(&mCodeEvalMapper, SIGNAL(mapped(QString)),
            this, SIGNAL(evaluateCode(QString)));
    connect(this, SIGNAL(evaluateCode(QString,bool)),
            main->scProcess(), SLOT(evaluateCode(QString,bool)));
    // Interpreter: post output
    connect(main->scProcess(), SIGNAL( scPost(QString) ),
            mPostDocklet->mPostWindow, SLOT( post(QString) ) );
    // Interpreter: monitor running state
    connect(main->scProcess(), SIGNAL( stateChanged(QProcess::ProcessState) ),
            this, SLOT( onInterpreterStateChanged(QProcess::ProcessState) ) );
    // Interpreter: forward status messages
    connect(main->scProcess(), SIGNAL(statusMessage(const QString&)),
            this, SLOT(showStatusMessage(const QString&)));

    // Document list interaction
    connect(mDocumentsDocklet->list(), SIGNAL(clicked(Document*)),
            mEditors, SLOT(setCurrent(Document*)));
    connect(mEditors, SIGNAL(currentDocumentChanged(Document*)),
            mDocumentsDocklet->list(), SLOT(setCurrent(Document*)),
            Qt::QueuedConnection);
    connect(mDocumentsDocklet->list(), SIGNAL(updateTabsOrder(QList<Document*>)),
            mEditors, SLOT(updateTabsOrder(QList<Document*>)));
    connect(mEditors, SIGNAL(updateDockletOrder(int, int)),
            mDocumentsDocklet->list(), SLOT(updateDockletOrder(int, int)),
            Qt::QueuedConnection);

    // Update actions on document change
    connect(mEditors, SIGNAL(currentDocumentChanged(Document*)),
            this, SLOT(onCurrentDocumentChanged(Document*)));
    // Document management
    DocumentManager *docMng = main->documentManager();
    connect(docMng, SIGNAL(changedExternally(Document*)),
            this, SLOT(onDocumentChangedExternally(Document*)));
    connect(docMng, SIGNAL(recentsChanged()),
            this, SLOT(updateRecentDocsMenu()));
    connect(docMng, SIGNAL(saved(Document*)), this, SLOT(updateWindowTitle()) );
    connect(docMng, SIGNAL(titleChanged(Document*)), this, SLOT(updateWindowTitle()) );

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));
    connect(main, SIGNAL(storeSettingsRequest(Settings::Manager*)),
            this, SLOT(storeSettings(Settings::Manager*)));

    // ToolBox
    connect(mToolBox->closeButton(), SIGNAL(clicked()), this, SLOT(hideToolBox()));

    createActions();
    createMenus();

    // Must be called after createAtions(), because it accesses an action:
    toggleInterpreterActions(false);

    // Initialize recent documents menu
    updateRecentDocsMenu();

    QIcon icon;
    icon.addFile(":/icons/sc-ide-svg");
    icon.addFile(":/icons/sc-ide-16");
    icon.addFile(":/icons/sc-ide-24");
    icon.addFile(":/icons/sc-ide-32");
    icon.addFile(":/icons/sc-ide-48");
    icon.addFile(":/icons/sc-ide-64");
    icon.addFile(":/icons/sc-ide-128");
    icon.addFile(":/icons/sc-ide-256");
    icon.addFile(":/icons/sc-ide-512");
    icon.addFile(":/icons/sc-ide-1024");
    QApplication::setWindowIcon(icon);

    updateWindowTitle();

    applyCursorBlinkingSettings(main->settings());

    // Custom event handling:
    qApp->installEventFilter(this);
}

void MainWindow::createActions()
{
    Settings::Manager *settings = mMain->settings();

    QAction *action;
    const QString ideCategory("IDE");
    const QString editorCategory(tr("Text Editor"));
    const QString helpCategory(tr("Help"));

    // File
    mActions[Quit] = action = new QAction(
        QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
    action->setShortcut(tr("Ctrl+Q", "Quit application"));
    action->setStatusTip(tr("Quit SuperCollider IDE"));
    QObject::connect( action, SIGNAL(triggered()), this, SLOT(onQuit()) );
    settings->addAction( action, "ide-quit", ideCategory);

    mActions[DocNew] = action = new QAction(
        QIcon::fromTheme("document-new"), tr("&New"), this);
    action->setShortcut(tr("Ctrl+N", "New document"));
    action->setStatusTip(tr("Create a new document"));
    connect(action, SIGNAL(triggered()), this, SLOT(newDocument()));
    settings->addAction( action, "ide-document-new", ideCategory);

    mActions[DocOpen] = action = new QAction(
        QIcon::fromTheme("document-open"), tr("&Open..."), this);
    action->setShortcut(tr("Ctrl+O", "Open document"));
    action->setStatusTip(tr("Open an existing file"));
    connect(action, SIGNAL(triggered()), this, SLOT(openDocument()));
    settings->addAction( action, "ide-document-open", ideCategory);

    mActions[DocOpenStartup] = action = new QAction(
        QIcon::fromTheme("document-open"), tr("Open startup file"), this);
    action->setStatusTip(tr("Open startup file"));
    connect(action, SIGNAL(triggered()), this, SLOT(openStartupFile()));
    settings->addAction( action, "ide-document-open-startup", ideCategory);

    mActions[DocOpenSupportDir] = action = new QAction(
        QIcon::fromTheme("document-open"), tr("Open user support directory"), this);
    action->setStatusTip(tr("Open user support directory"));
    connect(action, SIGNAL(triggered()), this, SLOT(openUserSupportDirectory()));
    settings->addAction( action, "ide-document-open-support-directory", ideCategory);

    mActions[DocSave] = action = new QAction(
        QIcon::fromTheme("document-save"), tr("&Save"), this);
    action->setShortcut(tr("Ctrl+S", "Save document"));
    action->setStatusTip(tr("Save the current document"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveDocument()));
    settings->addAction( action, "ide-document-save", ideCategory);

    mActions[DocSaveAs] = action = new QAction(
        QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    action->setShortcut(tr("Ctrl+Shift+S", "Save &As..."));
    action->setStatusTip(tr("Save the current document into a different file"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));
    settings->addAction( action, "ide-document-save-as", ideCategory);

    mActions[DocSaveAsExtension] = action = new QAction(
        QIcon::fromTheme("document-save-as"), tr("Save As Extension..."), this);
    action->setStatusTip(tr("Save the current document into a different file in the extensions folder"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveDocumentAsExtension()));
    settings->addAction( action, "ide-document-save-as-extension", ideCategory);

    mActions[DocSaveAll] = action = new QAction(
        QIcon::fromTheme("document-save"), tr("Save All..."), this);
    action->setShortcut(tr("Ctrl+Alt+S", "Save all documents"));
    action->setStatusTip(tr("Save all open documents"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveAllDocuments()));
    settings->addAction( action, "ide-document-save-all", ideCategory);

    mActions[DocCloseAll] = action = new QAction(
        QIcon::fromTheme("window-close"), tr("Close All..."), this);
    action->setShortcut(tr("Ctrl+Shift+W", "Close all documents"));
    action->setStatusTip(tr("Close all documents"));
    connect(action, SIGNAL(triggered()), this, SLOT(closeAllDocuments()));
    settings->addAction( action, "ide-document-close-all", ideCategory);

    mActions[DocReload] = action = new QAction(
        QIcon::fromTheme("view-refresh"), tr("&Reload"), this);
    action->setShortcut(tr("F5", "Reload document"));
    action->setStatusTip(tr("Reload the current document"));
    connect(action, SIGNAL(triggered()), this, SLOT(reloadDocument()));
    settings->addAction( action, "ide-document-reload", ideCategory);

    mActions[ClearRecentDocs] = action = new QAction(tr("Clear", "Clear recent documents"), this);
    action->setStatusTip(tr("Clear list of recent documents"));
    connect(action, SIGNAL(triggered()),
            Main::instance()->documentManager(), SLOT(clearRecents()));
    settings->addAction( action, "ide-clear-recent-documents", ideCategory);

    // Sessions
    mActions[NewSession] = action = new QAction(
        QIcon::fromTheme("document-new"), tr("&New Session"), this);
    action->setStatusTip(tr("Open a new session"));
    connect(action, SIGNAL(triggered()), this, SLOT(newSession()));
    settings->addAction( action, "ide-session-new", ideCategory);

    mActions[SaveSessionAs] = action = new QAction(
        QIcon::fromTheme("document-save-as"), tr("Save Session &As..."), this);
    action->setStatusTip(tr("Save the current session with a different name"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveCurrentSessionAs()));
    settings->addAction( action, "ide-session-save-as", ideCategory);

    mActions[ManageSessions] = action = new QAction(
        tr("&Manage Sessions..."), this);
    connect(action, SIGNAL(triggered()), this, SLOT(openSessionsDialog()));
    settings->addAction( action, "ide-session-manage", ideCategory);

    mActions[OpenSessionSwitchDialog] = action = new QAction(
        tr("&Switch Session..."), this);
    connect(action, SIGNAL(triggered()), this, SLOT(showSwitchSessionDialog()));
    action->setShortcut(tr("Ctrl+Shift+Q", "Switch Session"));
    settings->addAction( action, "ide-session-switch", ideCategory);

    // Edit
    mActions[Find] = action = new QAction(
        QIcon::fromTheme("edit-find"), tr("&Find..."), this);
    action->setShortcut(tr("Ctrl+F", "Find"));
    action->setStatusTip(tr("Find text in document"));
    connect(action, SIGNAL(triggered()), this, SLOT(showFindTool()));
    settings->addAction( action, "editor-find", editorCategory);

    mActions[Replace] = action = new QAction(
        QIcon::fromTheme("edit-replace"), tr("&Replace..."), this);
    action->setShortcut(tr("Ctrl+R", "Replace"));
    action->setStatusTip(tr("Find and replace text in document"));
    connect(action, SIGNAL(triggered()), this, SLOT(showReplaceTool()));
    settings->addAction( action, "editor-replace", editorCategory);

    // View
    mActions[ShowCmdLine] = action = new QAction(tr("&Command Line"), this);
    action->setStatusTip(tr("Command line for quick code evaluation"));
    action->setShortcut(tr("Ctrl+E", "Show command line"));
    connect(action, SIGNAL(triggered()), this, SLOT(showCmdLine()));
    settings->addAction( action, "ide-command-line-show", ideCategory);

    mActions[CmdLineForCursor] = action = new QAction(tr("&Command Line from selection"), this);
    action->setShortcut(tr("Ctrl+Shift+E", "Fill command line with current selection"));
    connect(action, SIGNAL(triggered()), this, SLOT(cmdLineForCursor()));
    settings->addAction( action, "ide-command-line-fill", ideCategory);


    mActions[ShowGoToLineTool] = action = new QAction(tr("&Go To Line"), this);
    action->setStatusTip(tr("Tool to jump to a line by number"));
    action->setShortcut(tr("Ctrl+L", "Show go-to-line tool"));
    connect(action, SIGNAL(triggered()), this, SLOT(showGoToLineTool()));
    settings->addAction( action, "editor-go-to-line", editorCategory);

    mActions[CloseToolBox] = action = new QAction(
        QIcon::fromTheme("window-close"), tr("&Close Tool Panel"), this);
    action->setStatusTip(tr("Close any open tool panel"));
    action->setShortcut(tr("Esc", "Close tool box"));
    connect(action, SIGNAL(triggered()), this, SLOT(hideToolBox()));
    settings->addAction( action, "ide-tool-panel-hide", ideCategory);

    mActions[ShowFullScreen] = action = new QAction(tr("&Full Screen"), this);
    action->setCheckable(false);
    action->setShortcut(tr("Ctrl+Shift+F", "Show ScIDE in Full Screen"));
    connect(action, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));
    settings->addAction( action, "ide-show-fullscreen", ideCategory);

    mActions[FocusPostWindow] = action = new QAction( tr("Focus Post Window"), this);
    action->setStatusTip(tr("Focus post window"));
    action->setShortcut(tr("Ctrl+P", "Focus post window"));
    connect(action, SIGNAL(triggered()), mPostDocklet, SLOT(focus()));
    settings->addAction( action, "post-focus", ideCategory);

    // Language
    mActions[LookupImplementation] = action = new QAction(
        QIcon::fromTheme("window-lookupdefinition"), tr("Look Up Implementations..."), this);
    action->setShortcut(tr("Ctrl+Shift+I", "Look Up Implementations"));
    action->setStatusTip(tr("Open dialog to look up implementations of a class or a method"));
    connect(action, SIGNAL(triggered()), this, SLOT(lookupImplementation()));
    settings->addAction( action, "ide-lookup-implementation", ideCategory);

    mActions[LookupImplementationForCursor] = action = new QAction(tr("Look Up Implementations for Cursor"), this);
    action->setShortcut(tr("Ctrl+I", "Look Up Implementations for Cursor"));
    action->setStatusTip(tr("Look up implementations of class or method under cursor"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(lookupImplementationForCursor()));
    settings->addAction( action, "ide-lookup-implementation-for-cursor", ideCategory);

    mActions[LookupReferences] = action = new QAction(
        QIcon::fromTheme("window-lookupreferences"), tr("Look Up References..."), this);
    action->setShortcut(tr("Ctrl+Shift+U", "Look Up References"));
    action->setStatusTip(tr("Open dialog to look up references to a class or a method"));
    connect(action, SIGNAL(triggered()), this, SLOT(lookupReferences()));
    settings->addAction( action, "ide-lookup-references", ideCategory);

    mActions[LookupReferencesForCursor] = action = new QAction(tr("Look Up References for Cursor"), this);
    action->setShortcut(tr("Ctrl+U", "Look Up References For Selection"));
    action->setStatusTip(tr("Look up references to class or method under cursor"));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(lookupReferencesForCursor()));
    settings->addAction( action, "ide-lookup-references-for-cursor", ideCategory);

    // Settings
    mActions[ShowSettings] = action = new QAction(tr("Preferences"), this);
#ifdef Q_OS_MAC
    action->setShortcut(tr("Ctrl+,", "Show configuration dialog"));
#endif
    action->setStatusTip(tr("Show configuration dialog"));
    connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
    settings->addAction( action, "ide-settings-dialog", ideCategory);

    // Help
    mActions[Help] = action = new QAction(tr("Show &Help Browser"), this);
    action->setStatusTip(tr("Show and focus the Help Browser"));
    connect(action, SIGNAL(triggered()), this, SLOT(openHelp()));
    settings->addAction( action, "help-browser", helpCategory);

    mActions[HelpAboutIDE]  = action =
            new QAction(QIcon::fromTheme("system-help"), tr("How to Use SuperCollider IDE"), this);
    action->setStatusTip(tr("Open the SuperCollider IDE guide"));
    connect(action, SIGNAL(triggered()), this, SLOT(openHelpAboutIDE()));

    mActions[ReportABug]  = action =
        new QAction(QIcon::fromTheme("system-help"), tr("Report a bug..."), this);
    action->setStatusTip(tr("Report a bug"));
    connect(action, SIGNAL(triggered()), this, SLOT(doBugReport()));
    
    mActions[LookupDocumentationForCursor] = action =
            new QAction(tr("Look Up Documentation for Cursor"), this);
    action->setShortcut(tr("Ctrl+D", "Look Up Documentation for Cursor"));
    action->setStatusTip(tr("Look up documentation for text under cursor"));
    connect(action, SIGNAL(triggered()), this, SLOT(lookupDocumentationForCursor()));
    settings->addAction( action, "help-lookup-for-cursor", helpCategory);

    mActions[LookupDocumentation] = action =
            new QAction(tr("Look Up Documentation..."), this);
    action->setShortcut(tr("Ctrl+Shift+D", "Look Up Documentation"));
    action->setStatusTip(tr("Enter text to look up in documentation"));
    connect(action, SIGNAL(triggered()), this, SLOT(lookupDocumentation()));
    settings->addAction( action, "help-lookup", helpCategory);

    mActions[ShowAbout] = action = new QAction(
        QIcon::fromTheme("help-about"), tr("&About SuperCollider"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(showAbout()));
    settings->addAction( action, "ide-about", ideCategory);

    mActions[ShowAboutQT] = action = new QAction(
        QIcon::fromTheme("show-about-qt"), tr("About &Qt"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(showAboutQT()));
    settings->addAction( action, "ide-about-qt", ideCategory);

    // Add external actions to settings:
    action = mPostDocklet->toggleViewAction();
    action->setIcon( QIcon::fromTheme("utilities-terminal"));
    action->setStatusTip(tr("Show/hide Post docklet"));
    settings->addAction( mPostDocklet->toggleViewAction(),
                         "ide-docklet-post", ideCategory );

    action = mDocumentsDocklet->toggleViewAction();
    action->setIcon( QIcon::fromTheme("text-x-generic") );
    action->setStatusTip(tr("Show/hide Documents docklet"));
    settings->addAction( mDocumentsDocklet->toggleViewAction(),
                         "ide-docklet-documents", ideCategory );

    action = mHelpBrowserDocklet->toggleViewAction();
    action->setIcon( QIcon::fromTheme("system-help") );
    action->setStatusTip(tr("Show/hide Help browser docklet"));
    settings->addAction( mHelpBrowserDocklet->toggleViewAction(),
                         "ide-docklet-help", ideCategory );

    // In Mac OS, all menu item shortcuts need a modifier, so add the action with
    // the "Escape" default shortcut to the main window widget.
    // FIXME: This is not perfect, as any other action customized to "Escape" will
    // still not work.
    addAction( mActions[CloseToolBox] );

    // Add actions to docklets, so shortcuts work when docklets detached:

    mPostDocklet->widget()->addAction(mActions[LookupDocumentation]);
    mPostDocklet->widget()->addAction(mActions[LookupDocumentationForCursor]);
    mPostDocklet->widget()->addAction(mActions[LookupImplementation]);
    mPostDocklet->widget()->addAction(mActions[LookupImplementationForCursor]);
    mPostDocklet->widget()->addAction(mActions[LookupReferences]);
    mPostDocklet->widget()->addAction(mActions[LookupReferencesForCursor]);

    mHelpBrowserDocklet->widget()->addAction(mActions[LookupDocumentation]);
    mHelpBrowserDocklet->widget()->addAction(mActions[LookupDocumentationForCursor]);
    mHelpBrowserDocklet->widget()->addAction(mActions[LookupImplementation]);
    mHelpBrowserDocklet->widget()->addAction(mActions[LookupImplementationForCursor]);
    mHelpBrowserDocklet->widget()->addAction(mActions[LookupReferences]);
    mHelpBrowserDocklet->widget()->addAction(mActions[LookupReferencesForCursor]);

}

void MainWindow::createMenus()
{
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *submenu;

    // On Mac, create a parent-less menu bar to be shared by all windows:
#ifdef Q_OS_MAC
    menuBar = new QMenuBar(0);
#else
    menuBar = this->menuBar();
#endif

    menu = new QMenu(tr("&File"), this);
    menu->addAction( mActions[DocNew] );
    menu->addAction( mActions[DocOpen] );
    mRecentDocsMenu = menu->addMenu(tr("Open Recent", "Open a recent document"));
    connect(mRecentDocsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(onOpenRecentDocument(QAction*)));
    menu->addAction( mActions[DocOpenStartup] );
    menu->addAction( mActions[DocOpenSupportDir] );
    menu->addAction( mActions[DocSave] );
    menu->addAction( mActions[DocSaveAs] );
    menu->addAction( mActions[DocSaveAsExtension] );
    menu->addAction( mActions[DocSaveAll] );
    menu->addSeparator();
    menu->addAction( mActions[DocReload] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::DocClose) );
    menu->addAction( mActions[DocCloseAll] );
    menu->addSeparator();
    menu->addAction( mActions[Quit] );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Session"), this);
    menu->addAction( mActions[NewSession] );
    menu->addAction( mActions[SaveSessionAs] );
    submenu = menu->addMenu(tr("&Open Session"));
    connect(submenu, SIGNAL(triggered(QAction*)),
            this, SLOT(onOpenSessionAction(QAction*)));
    mSessionsMenu = submenu;
    updateSessionsMenu();
    menu->addSeparator();
    menu->addAction( mActions[ManageSessions] );
    menu->addAction( mActions[OpenSessionSwitchDialog] );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Edit"), this);
    menu->addAction( mEditors->action(MultiEditor::Undo) );
    menu->addAction( mEditors->action(MultiEditor::Redo) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::Cut) );
    menu->addAction( mEditors->action(MultiEditor::Copy) );
    menu->addAction( mEditors->action(MultiEditor::Paste) );
    menu->addSeparator();
    menu->addAction( mActions[Find] );
    menu->addAction( mFindReplaceTool->action(TextFindReplacePanel::FindNext) );
    menu->addAction( mFindReplaceTool->action(TextFindReplacePanel::FindPrevious) );
    menu->addAction( mActions[Replace] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::IndentWithSpaces) );
    menu->addAction( mEditors->action(MultiEditor::IndentLineOrRegion) );
    menu->addAction( mEditors->action(MultiEditor::ToggleComment) );
    menu->addAction( mEditors->action(MultiEditor::ToggleOverwriteMode) );
    menu->addAction( mEditors->action(MultiEditor::SelectRegion) );
    menu->addAction( mEditors->action(MultiEditor::SelectEnclosingBlock) );

    menu->addSeparator();
    menu->addAction( mActions[ShowSettings] );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&View"), this);
    submenu = new QMenu(tr("&Docklets"), this);
    submenu->addAction( mPostDocklet->toggleViewAction() );
    submenu->addAction( mDocumentsDocklet->toggleViewAction() );
    submenu->addAction( mHelpBrowserDocklet->toggleViewAction() );
    menu->addMenu(submenu);
    menu->addSeparator();
    submenu = menu->addMenu(tr("&Tool Panels"));
    submenu->addAction( mActions[Find] );
    submenu->addAction( mActions[Replace] );
    submenu->addAction( mActions[ShowCmdLine] );
    submenu->addAction( mActions[CmdLineForCursor] );
    submenu->addAction( mActions[ShowGoToLineTool] );
    submenu->addSeparator();
    submenu->addAction( mActions[CloseToolBox] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EnlargeFont) );
    menu->addAction( mEditors->action(MultiEditor::ShrinkFont) );
    menu->addAction( mEditors->action(MultiEditor::ResetFontSize) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ShowWhitespace) );
    menu->addAction( mEditors->action(MultiEditor::ShowLinenumber) );
    menu->addSeparator();
    menu->addAction(mEditors->action(MultiEditor::ShowAutocompleteHelp));
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::NextDocument) );
    menu->addAction( mEditors->action(MultiEditor::PreviousDocument) );
    menu->addAction( mEditors->action(MultiEditor::SwitchDocument) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::SplitHorizontally) );
    menu->addAction( mEditors->action(MultiEditor::SplitVertically) );
    menu->addAction( mEditors->action(MultiEditor::RemoveCurrentSplit) );
    menu->addAction( mEditors->action(MultiEditor::RemoveAllSplits) );
    menu->addSeparator();
    menu->addAction( mActions[FocusPostWindow] );
    menu->addSeparator();
    menu->addAction( mActions[ShowFullScreen] );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(ScProcess::ToggleRunning) );
    menu->addAction( mMain->scProcess()->action(ScProcess::Restart) );
    menu->addAction( mMain->scProcess()->action(ScProcess::RecompileClassLibrary) );
    menu->addSeparator();
    menu->addAction( mMain->scProcess()->action(ScProcess::ShowQuarks) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EvaluateCurrentDocument) );
    menu->addAction( mEditors->action(MultiEditor::EvaluateRegion) );
    menu->addAction( mEditors->action(MultiEditor::EvaluateLine) );
    menu->addAction( mMain->scProcess()->action(ScIDE::ScProcess::StopMain) );
    menu->addSeparator();
    menu->addAction( mActions[LookupImplementationForCursor] );
    menu->addAction( mActions[LookupImplementation] );
    menu->addAction( mActions[LookupReferencesForCursor] );
    menu->addAction( mActions[LookupReferences] );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("Se&rver"), this);
    menu->addAction( mMain->scServer()->action(ScServer::ToggleRunning) );
    menu->addAction( mMain->scServer()->action(ScServer::Reboot) );
    menu->addAction( mMain->scServer()->action(ScServer::KillAll) );
    menu->addSeparator();
    menu->addAction( mMain->scServer()->action(ScServer::ShowMeters) );
    menu->addAction( mMain->scServer()->action(ScServer::ShowScope) );
    menu->addAction( mMain->scServer()->action(ScServer::ShowFreqScope) );
    menu->addAction( mMain->scServer()->action(ScServer::DumpNodeTree) );
    menu->addAction( mMain->scServer()->action(ScServer::DumpNodeTreeWithControls) );
    menu->addAction( mMain->scServer()->action(ScServer::PlotTree) );
    menu->addAction( mMain->scServer()->action(ScServer::DumpOSC) );
    menu->addAction( mMain->scServer()->action(ScServer::Record) );
    menu->addAction( mMain->scServer()->action(ScServer::PauseRecord) );
    menu->addAction( mMain->scServer()->action(ScServer::VolumeUp) );
    menu->addAction( mMain->scServer()->action(ScServer::VolumeDown) );
    menu->addAction( mMain->scServer()->action(ScServer::VolumeRestore) );
    menu->addAction( mMain->scServer()->action(ScServer::Mute) );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Help"), this);
    menu->addAction( mActions[HelpAboutIDE] );
    menu->addAction( mActions[ReportABug] );
    menu->addSeparator();
    menu->addAction( mActions[Help] );
    menu->addAction( mActions[LookupDocumentationForCursor] );
    menu->addAction( mActions[LookupDocumentation] );
    menu->addSeparator();
    menu->addAction( mActions[ShowAbout] );
    menu->addAction( mActions[ShowAboutQT] );

    menuBar->addMenu(menu);
}

static void saveDetachedState( Docklet *docklet,  QVariantMap & data )
{
    data.insert( docklet->objectName(), docklet->saveDetachedState().toBase64() );
}

template <class T>
void MainWindow::saveWindowState(T * settings)
{
    QVariantMap detachedData;
    saveDetachedState( mPostDocklet, detachedData );
    saveDetachedState( mDocumentsDocklet, detachedData );
    saveDetachedState( mHelpBrowserDocklet, detachedData );

    settings->beginGroup("mainWindow");
    settings->setValue("geometry", this->saveGeometry().toBase64());
    settings->setValue("state", this->saveState().toBase64());
    settings->setValue("detached", QVariant::fromValue(detachedData));
    settings->endGroup();
}

void MainWindow::saveWindowState()
{
    Settings::Manager *settings = Main::settings();
    settings->beginGroup("IDE");
    saveWindowState(settings);
    settings->endGroup();
}

static void restoreDetachedState( Docklet *docklet,  const QVariantMap & data )
{
    QByteArray base64data = data.value( docklet->objectName() ).value<QByteArray>();
    docklet->restoreDetachedState( QByteArray::fromBase64( base64data ) );
}

template <class T>
void MainWindow::restoreWindowState( T * settings )
{
    qDebug("------------ restore window state ------------");

    settings->beginGroup("mainWindow");
    QVariant varGeom = settings->value("geometry");
    QVariant varState = settings->value("state");
    QVariant varDetached = settings->value("detached");
    settings->endGroup();

    QByteArray geom = QByteArray::fromBase64( varGeom.value<QByteArray>() );
    QByteArray state = QByteArray::fromBase64( varState.value<QByteArray>() );
    QVariantMap detachedData = varDetached.value<QVariantMap>();

    if (!geom.isEmpty()) {
        // Workaround for Qt bug 4397:
        setWindowState(Qt::WindowNoState);
        restoreGeometry(geom);
    }
    else
        setWindowState( windowState() & ~Qt::WindowFullScreen | Qt::WindowMaximized );

    restoreDetachedState( mPostDocklet, detachedData );
    restoreDetachedState( mDocumentsDocklet, detachedData );
    restoreDetachedState( mHelpBrowserDocklet, detachedData );

    qDebug("restoring state");

    if (!state.isEmpty())
        restoreState(state);

    qDebug("setting dock area corners");

    setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

    updateClockWidget(isFullScreen());

    qDebug("------------ END restore window state ------------");
}

void MainWindow::restoreWindowState()
{
    Settings::Manager *settings = Main::settings();
    settings->beginGroup("IDE");
    restoreWindowState(settings);
    settings->endGroup();
}

void MainWindow::focusCodeEditor()
{
    if (mEditors->currentEditor())
        mEditors->currentEditor()->setFocus();
    else
        mEditors->setFocus();
}

void MainWindow::newSession()
{
    mMain->sessionManager()->newSession();
}

void MainWindow::saveCurrentSessionAs()
{
    QString name = QInputDialog::getText( this,
                                          tr("Save Current Session"),
                                          tr("Enter a name for the session:") );

    if (name.isEmpty()) return;

    mMain->sessionManager()->saveSessionAs(name);

    updateSessionsMenu();
}

void MainWindow::onOpenSessionAction( QAction * action )
{
    openSession(action->text());
}

void MainWindow::switchSession( Session *session )
{
    if (session)
        restoreWindowState(session);

    updateWindowTitle();

    mEditors->switchSession(session);
}

void MainWindow::saveSession( Session *session )
{
    saveWindowState(session);

    mEditors->saveSession(session);
}

void MainWindow::openSessionsDialog()
{
    QPointer<MainWindow> mainwin(this);
    SessionsDialog dialog(mMain->sessionManager(), this);
    dialog.exec();
    if (mainwin)
        mainwin->updateSessionsMenu();
}

QAction *MainWindow::action( ActionRole role )
{
    Q_ASSERT( role < ActionCount );
    return mActions[role];
}

bool MainWindow::quit()
{
    if (!promptSaveDocs())
        return false;

    Main::instance()->documentManager()->deleteRestore();

    saveWindowState();

    mMain->quit();

    return true;
}

void MainWindow::onQuit()
{
    quit();
}

void MainWindow::onCurrentDocumentChanged( Document * doc )
{
    updateWindowTitle();

    mActions[DocCloseAll]->setEnabled(doc);
    mActions[DocReload]->setEnabled(doc);
    mActions[DocSave]->setEnabled(doc);
    mActions[DocSaveAs]->setEnabled(doc);
    mActions[DocSaveAsExtension]->setEnabled(doc);

    GenericCodeEditor *editor = mEditors->currentEditor();
    mFindReplaceTool->setEditor( editor );
    mGoToLineTool->setEditor( editor );
}

void MainWindow::onDocumentChangedExternally( Document *doc )
{
    if (mDocDialog)
        return;

    mDocDialog = new DocumentsDialog(DocumentsDialog::ExternalChange, this);
    mDocDialog->addDocument(doc);
    connect(mDocDialog, SIGNAL(finished(int)), this, SLOT(onDocDialogFinished()));
    mDocDialog->open();
}

void MainWindow::onDocDialogFinished()
{
    mDocDialog->deleteLater();
    mDocDialog = 0;
}

void MainWindow::updateRecentDocsMenu()
{
    mRecentDocsMenu->clear();

    const QStringList &recent = mMain->documentManager()->recents();

    foreach( const QString & path, recent )
        mRecentDocsMenu->addAction(path);

    if (!recent.isEmpty()) {
        mRecentDocsMenu->addSeparator();
        mRecentDocsMenu->addAction(mActions[ClearRecentDocs]);
    }
}

void MainWindow::onOpenRecentDocument( QAction *action )
{
    mMain->documentManager()->open(action->text());
}

void MainWindow::onInterpreterStateChanged( QProcess::ProcessState state )
{
    switch(state) {
    case QProcess::NotRunning:
        toggleInterpreterActions(false);

    case QProcess::Starting:
        break;

    case QProcess::Running:
        toggleInterpreterActions(true);
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!quit()) event->ignore();
}

bool MainWindow::close( Document *doc )
{
    if (doc->textDocument()->isModified() && doc->promptsToSave())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(
            mInstance,
            tr("SuperCollider IDE"),
            tr("There are unsaved changes in document '%1'.\n\n"
                "Do you want to save it?").arg(doc->title()),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save // the default
        );

        switch (ret) {
        case QMessageBox::Cancel:
            return false;
        case QMessageBox::Save:
            if (!MainWindow::save(doc))
                return false;
            break;
        default:;
        }
    }

    Main::instance()->documentManager()->close(doc);
    return true;
}

bool MainWindow::reload( Document *doc )
{
    if (doc->filePath().isEmpty())
        return false;

    if (doc->textDocument()->isModified())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(
            mInstance,
            tr("SuperCollider IDE"),
            tr("There are unsaved changes in document '%1'.\n\n"
                "Do you want to reload it?").arg(doc->title()),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No // the default
        );
        if (ret == QMessageBox::No)
            return false;
    }

    return Main::instance()->documentManager()->reload(doc);
}

QString MainWindow::documentSavePath( Document *document ) const
{
    if (!document->filePath().isEmpty())
        return document->filePath();

    if (!mLastDocumentSavePath.isEmpty())
        return QFileInfo(mLastDocumentSavePath).path();

    QString interpreterWorkingDir =
            Main::settings()->value("IDE/interpreter/runtimeDir").toString();
    if (!interpreterWorkingDir.isEmpty())
        return interpreterWorkingDir;

    return QStandardPaths::standardLocations( QStandardPaths::HomeLocation )[0];
}

bool MainWindow::save( Document *doc, bool forceChoose, bool saveInExtensionFolder )
{
    const bool documentHasPath = !doc->filePath().isEmpty();

    if (!forceChoose && !(doc->isModified()) && documentHasPath)
        return true;

    DocumentManager *documentManager = Main::instance()->documentManager();

    bool fileIsWritable = true;
    if ((!forceChoose) && documentHasPath) {
        QFileInfo fileInfo(doc->filePath());
        fileIsWritable = fileInfo.isWritable();

        if (!fileIsWritable) {
            QMessageBox::warning(instance(), tr("Saving read-only file"),
                                 tr("File is read-only. Please select a new location to save to."),
                                 QMessageBox::Ok, QMessageBox::NoButton);

        }
    }

    if (forceChoose || !documentHasPath || !fileIsWritable) {

        QFileDialog dialog(mInstance);
        dialog.setAcceptMode( QFileDialog::AcceptSave );
        dialog.setFileMode( QFileDialog::AnyFile );

        QStringList filters = QStringList()
                              << tr("All Files (*)")
                              << tr("SuperCollider Document (*.scd)")
                              << tr("SuperCollider Class File (*.sc)")
                              << tr("SuperCollider Help Source (*.schelp)");

        dialog.setNameFilters(filters);

        if (saveInExtensionFolder) {
            dialog.setDirectory( standardDirectory(ScExtensionUserDir) );
        } else {
            QString path = mInstance->documentSavePath(doc);
            QFileInfo path_info(path);

            if (path_info.isDir())
                // FIXME:
                // KDE native file dialog shows parent directory instead (KDE bug 229375)
                dialog.setDirectory(path);
            else
                dialog.selectFile(path);

            // NOTE: do not use QFileDialog::setDefaultSuffix(), because it only adds
            // the suffix after the dialog is closed, without showing a warning if the
            // filepath with added suffix already exists!
        }

#ifdef Q_OS_MAC
        QWidget *last_active_window = QApplication::activeWindow();
#endif

        int result = dialog.exec();

        // FIXME: workaround for Qt bug 25295
        // See SC issue #678
#ifdef Q_OS_MAC
        if (last_active_window)
            last_active_window->activateWindow();
#endif

        QString save_path;

        if (result == QDialog::Accepted) {
            save_path = dialog.selectedFiles()[0];

            if (save_path.indexOf('.') == -1 && !QFile::exists(save_path)) {
                save_path.append(".scd");
                QFileInfo save_path_info(save_path);
                if (save_path_info.exists())
                {
                    QString msg =
                            tr("Extenstion \".scd\" was automatically added to the "
                               "selected file name, but the file \"%1\" already exists.\n\n"
                               "Do you wish to overwrite it?")
                            .arg(save_path_info.fileName());
                    QMessageBox::StandardButton result =
                            QMessageBox::warning(mInstance,
                                                 tr("Overwrite File?"),
                                                 msg,
                                                 QMessageBox::Yes | QMessageBox::No);
                    if (result != QMessageBox::Yes)
                        save_path.clear();
                }
            }
        }

        if (!save_path.isEmpty()) {
            if( !saveInExtensionFolder )
                mInstance->mLastDocumentSavePath = save_path;
            return documentManager->saveAs(doc, save_path);
        } else {
            return false;
        }
    } else
        return documentManager->save(doc);
}

void MainWindow::newDocument()
{
    mMain->documentManager()->create();
}

QString MainWindow::documentOpenPath() const
{
    GenericCodeEditor * currentEditor = mEditors->currentEditor();
    if (currentEditor) {
        QString currentEditorPath = currentEditor->document()->filePath();
        if (!currentEditorPath.isEmpty())
            return currentEditorPath;
    }

    const QStringList & recentDocuments = Main::documentManager()->recents();
    if (!recentDocuments.isEmpty())
        return recentDocuments[0];

    QString interpreterWorkingDir =
            Main::settings()->value("IDE/interpreter/runtimeDir").toString();
    if (!interpreterWorkingDir.isEmpty())
        return interpreterWorkingDir;

    return QStandardPaths::standardLocations( QStandardPaths::HomeLocation )[0];
}

void MainWindow::openDocument()
{
    QFileDialog dialog (this, Qt::Dialog);
    dialog.setModal(true);
    dialog.setWindowModality(Qt::ApplicationModal);

    dialog.setFileMode( QFileDialog::ExistingFiles );

    QString path = documentOpenPath();
    QFileInfo path_info(path);
    if (path_info.isDir())
        dialog.setDirectory(path);
    else
        dialog.setDirectory(path_info.dir());

    QStringList filters;
    filters
        << tr("All Files (*)")
        << tr("SuperCollider (*.scd *.sc)")
        << tr("SuperCollider Help Source (*.schelp)");
    dialog.setNameFilters(filters);

#ifdef Q_OS_MAC
    QWidget *last_active_window = QApplication::activeWindow();
#endif

    if (dialog.exec())
    {
        QStringList filenames = dialog.selectedFiles();
        foreach(QString filename, filenames)
            mMain->documentManager()->open(filename);
    }

    // FIXME: workaround for Qt bug 25295
    // See SC issue #678
#ifdef Q_OS_MAC
    if (last_active_window)
        last_active_window->activateWindow();
#endif
}

void MainWindow::restoreDocuments()
{
    DocumentManager *docMng = Main::instance()->documentManager();

    if (docMng->needRestore()) {
        QString msg = tr("Supercollider didn't quit properly last time\n"
                         "Do you want to restore files saved as temporary backups?");
        QMessageBox::StandardButton restore =
                          QMessageBox::warning(mInstance, tr("Restore files?"),
                                    msg, QMessageBox::Yes | QMessageBox::No);
        if (restore == QMessageBox::Yes)
            docMng->restore();
        else
            docMng->deleteRestore();
    }
}

void MainWindow::openStartupFile()
{
    QString configDir = standardDirectory(ScConfigUserDir);

    QDir dir;
    // Create the config dir if non existent:
    dir.mkpath(configDir);
    if (!dir.cd(configDir)) {
        qWarning() << "Could not access config dir:" << configDir;
        return;
    }

    QString filePath = dir.filePath("startup.scd");
    // Try creating the file if non-existent:
    if (!QFile::exists(filePath)) {
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly)) {
            file.close();
            qWarning() << "Could not create startup file:" << filePath;
            return;
        }
        file.close();
    }

    mMain->documentManager()->open( filePath, -1, 0, false );
}

void MainWindow::openUserSupportDirectory()
{
    QUrl dirUrl = QUrl::fromLocalFile( standardDirectory(ScAppDataUserDir) );
    QDesktopServices::openUrl(dirUrl);
}

void MainWindow::saveDocument()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    Q_ASSERT(doc);

    MainWindow::save(doc);
}

void MainWindow::saveDocumentAs()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    Q_ASSERT(doc);

    MainWindow::save(doc, true);
}

void MainWindow::saveDocumentAsExtension()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    Q_ASSERT(doc);

    MainWindow::save(doc, true, true);
}

void MainWindow::saveAllDocuments()
{
    QList<Document*> docs = mMain->documentManager()->documents();
    foreach (Document *doc, docs)
        if (!MainWindow::save(doc))
            return;
}

void MainWindow::reloadDocument()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Q_ASSERT(editor->document());
    MainWindow::reload(editor->document());
}

void MainWindow::closeDocument()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Q_ASSERT(editor->document());
    MainWindow::close( editor->document() );
}

void MainWindow::closeAllDocuments()
{
    if (promptSaveDocs()) {
        QList<Document*> docs = mMain->documentManager()->documents();
        foreach (Document *doc, docs)
            mMain->documentManager()->close(doc);
    }
}

bool MainWindow::promptSaveDocs()
{
    // LATER: maybe this should go to the DocumentManager class?

    QList<Document*> docs = mMain->documentManager()->documents();
    QList<Document*> unsavedDocs;
    foreach(Document* doc, docs)
        if(doc->textDocument()->isModified() && doc->promptsToSave())
            unsavedDocs.append(doc);

    if (!unsavedDocs.isEmpty()) {
        DocumentsDialog dialog(unsavedDocs, DocumentsDialog::Quit, this);

        if (!dialog.exec())
            return false;
    }

    return true;
}

void MainWindow::updateWindowTitle()
{
    Session *session = mMain->sessionManager()->currentSession();
    GenericCodeEditor *editor = mEditors->currentEditor();
    Document *doc = editor ? editor->document() : 0;

    QString title;

    if (session) {
        title.append(session->name());
        if (doc) title.append(": ");
    }

    if (doc) {
        if (!doc->filePath().isEmpty()) {
            QFileInfo info = QFileInfo(doc->filePath());
            QString pathString = info.dir().path();

            QString homePath = QDir::homePath();
            if (pathString.startsWith(homePath))
                pathString.replace(0, homePath.size(), QStringLiteral("~"));

            QString titleString = QStringLiteral("%1 (%2)").arg(info.fileName(), pathString);

            title.append( titleString  );

            setWindowFilePath(doc->filePath());
        } else {
            title.append( tr("Untitled") );
            setWindowFilePath("");
        }
    } else {
            setWindowFilePath("");
    }

    if (!title.isEmpty())
        title.append(" - ");

    title.append("SuperCollider IDE");

    setWindowTitle(title);
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        setWindowState(windowState() & ~Qt::WindowFullScreen);

        updateClockWidget(false);
    } else {
        setWindowState(windowState() | Qt::WindowFullScreen);

        updateClockWidget(true);
    }
}

void MainWindow::updateClockWidget(bool isFullScreen)
{
    if (!isFullScreen) {
        if (mClockLabel) {
            delete mClockLabel;
            mClockLabel = NULL;
        }
    } else {
        if (mClockLabel == NULL) {
            mClockLabel = new ClockStatusBox(this);
            statusBar()->insertWidget(0, mClockLabel);
        }
    }
}

void MainWindow::openSession(const QString &sessionName)
{
    mMain->sessionManager()->openSession( sessionName );
}

void MainWindow::lookupImplementationForCursor()
{
    static const QByteArray signature = QMetaObject::normalizedSignature("openDefinition()");

    invokeMethodOnFirstResponder(signature);
}

void MainWindow::lookupImplementation()
{
    Main::openDefinition(QString(), QApplication::activeWindow());
}

void MainWindow::lookupReferencesForCursor()
{
    static const QByteArray signature = QMetaObject::normalizedSignature("findReferences()");

    invokeMethodOnFirstResponder(signature);
}

void MainWindow::lookupReferences()
{
    Main::findReferences(QString(), QApplication::activeWindow());
}

void MainWindow::showStatusMessage( QString const & string )
{
    mStatusBar->showMessage(string, 3000);
}

void MainWindow::applySettings( Settings::Manager * settings )
{
    applyCursorBlinkingSettings( settings );

    mPostDocklet->mPostWindow->applySettings(settings);
    mHelpBrowserDocklet->browser()->applySettings(settings);
    mCmdLine->applySettings(settings);
}

void MainWindow::applyCursorBlinkingSettings( Settings::Manager * settings )
{
    const bool disableBlinkingCursor = settings->value("IDE/editor/disableBlinkingCursor").toBool();
    const int defaultCursorFlashTime = settings->defaultCursorFlashTime();
    QApplication::setCursorFlashTime( disableBlinkingCursor ? 0 : defaultCursorFlashTime );
}

void MainWindow::storeSettings( Settings::Manager * settings )
{
    mPostDocklet->mPostWindow->storeSettings(settings);
}

void MainWindow::updateSessionsMenu()
{
    mSessionsMenu->clear();
    QStringList sessions = mMain->sessionManager()->availableSessions();
    foreach (const QString & session, sessions)
        mSessionsMenu->addAction( session );
}

void MainWindow::showSwitchSessionDialog()
{
    SessionSwitchDialog * dialog = new SessionSwitchDialog(this);
    int result = dialog->exec();

    if (result == QDialog::Accepted)
        openSession(dialog->activeElement());

    delete dialog;
}

void MainWindow::showAbout()
{
    QString aboutString =
            "<h3>SuperCollider %1</h3>"
            "<p>%2</p>"
            "&copy; James McCartney and others.<br>"
            "<h3>SuperCollider IDE</h3>"
            "&copy; Jakob Leben, Tim Blechmann and others.<br>"
            ;
    aboutString = aboutString.arg(SC_VersionString().c_str()).arg(SC_BuildString().c_str());

    QMessageBox::about(this, tr("About SuperCollider IDE"), aboutString);
}

void MainWindow::showAboutQT()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::toggleInterpreterActions(bool enabled)
{
    mEditors->action(MultiEditor::EvaluateCurrentDocument)->setEnabled(enabled);
    mEditors->action(MultiEditor::EvaluateLine)->setEnabled(enabled);
    mEditors->action(MultiEditor::EvaluateRegion)->setEnabled(enabled);
}


void MainWindow::showCmdLine()
{
    mToolBox->setCurrentWidget( mCmdLine );
    mToolBox->show();

    mCmdLine->setFocus(Qt::OtherFocusReason);
}

void MainWindow::showCmdLine( const QString & cmd)
{
    mCmdLine->setText(cmd);
    showCmdLine();
}

void MainWindow::cmdLineForCursor()
{
    static const QByteArray signature = QMetaObject::normalizedSignature("openCommandLine()");

    invokeMethodOnFirstResponder(signature);
}

void MainWindow::showGoToLineTool()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    mGoToLineTool->setValue( editor ? editor->textCursor().blockNumber() + 1 : 0 );

    mToolBox->setCurrentWidget( mGoToLineTool );
    mToolBox->show();

    mGoToLineTool->setFocus();
}

void MainWindow::showFindTool()
{
    mFindReplaceTool->setMode( TextFindReplacePanel::Find );
    mFindReplaceTool->initiate();

    mToolBox->setCurrentWidget( mFindReplaceTool );
    mToolBox->show();

    mFindReplaceTool->setFocus(Qt::OtherFocusReason);
}

void MainWindow::showReplaceTool()
{
    mFindReplaceTool->setMode( TextFindReplacePanel::Replace );
    mFindReplaceTool->initiate();

    mToolBox->setCurrentWidget( mFindReplaceTool );
    mToolBox->show();

    mFindReplaceTool->setFocus(Qt::OtherFocusReason);
}

void MainWindow::hideToolBox()
{
    GenericCodeEditor *editor = mEditors->currentEditor();
    if (editor) {
        // This slot is mapped to Escape, so also clear highlighting
        // whenever invoked:
        editor->clearSearchHighlighting();
        if (!editor->hasFocus())
            editor->setFocus(Qt::OtherFocusReason);
    }

    mToolBox->hide();
}

void MainWindow::showSettings()
{
    Settings::Dialog dialog(mMain->settings());
    dialog.resize(700,400);
    int result = dialog.exec();
    if( result == QDialog::Accepted )
        mMain->applySettings();
}


void MainWindow::lookupDocumentation()
{
    PopupTextInput * dialog = new PopupTextInput(tr("Look up Documentation For"),
                                                 QApplication::activeWindow());

    bool success = dialog->exec();
    if (success)
        Main::openDocumentation(dialog->textValue());

    delete dialog;
}

void MainWindow::lookupDocumentationForCursor()
{
    static const QByteArray signature = QMetaObject::normalizedSignature("openDocumentation()");

    bool documentationOpened = false;
    QWidget * widget = QApplication::focusWidget();
    int methodIdx = -1;

    widget = findFirstResponder( widget, signature.constData(), methodIdx );

    if (widget && methodIdx != -1) {
        widget->metaObject()->method(methodIdx).invoke(
                    widget, Qt::DirectConnection,
                    Q_RETURN_ARG(bool, documentationOpened) );
    };

    if (!documentationOpened)
        openHelp();
}

void MainWindow::openHelp()
{
    if (mHelpBrowserDocklet->browser()->url().isEmpty())
        mHelpBrowserDocklet->browser()->goHome();
    mHelpBrowserDocklet->focus();
}

void MainWindow::openHelpAboutIDE()
{
    mHelpBrowserDocklet->browser()->gotoHelpFor("Guides/SCIde");
    mHelpBrowserDocklet->focus();
}
    
void MainWindow::doBugReport()
{
    Settings::Manager *settings = mMain->settings();
    bool useGitHubBugReport = false;
    
    if (settings->contains("IDE/useGitHubBugReport")) {
        
        useGitHubBugReport = settings->value("IDE/useGitHubBugReport").toBool();
        
    } else {
        
        QMessageBox* dialog = new QMessageBox();
        dialog->setText("Do you want to submit bugs using <a href=\"https://www.github.com\">GitHub</a>?");
        dialog->setInformativeText("This requires a GitHub account.");
        dialog->addButton("Submit using GitHub", QMessageBox::YesRole);
        dialog->addButton("Submit anonymously", QMessageBox::NoRole);
        dialog->addButton("Cancel", QMessageBox::RejectRole);
        dialog->exec();
        QMessageBox::ButtonRole clicked = dialog->buttonRole(dialog->clickedButton());
        
        if (clicked == QMessageBox::YesRole || clicked == QMessageBox::NoRole) {
            useGitHubBugReport = (clicked == QMessageBox::YesRole);
            settings->setValue("IDE/useGitHubBugReport", useGitHubBugReport);
        } else {
            // Dialog was cancelled, so bail
            return;
        }
    }
    
    if (useGitHubBugReport) {
        QString url("https://github.com/supercollider/supercollider/issues/new");
        QString formData("?labels=bug&body=Bug%20description%3A%0A%0ASteps%20to%20reproduce%3A%0A1.%0A2.%0A3.%0A%0AActual%20result%3A%0A%0AExpected%20result%3A%0A");
        QDesktopServices::openUrl(url + formData);
    } else {
        QDesktopServices::openUrl(QStringLiteral("https://gitreports.com/issue/supercollider/supercollider"));
    }
}

void MainWindow::dragEnterEvent( QDragEnterEvent * event )
{
    if (event->mimeData()->hasUrls()) {
        foreach (QUrl url, event->mimeData()->urls()) {
            if ( QURL_IS_LOCAL_FILE(url) ) {
                // LATER: check mime type ?
                event->acceptProposedAction();
                return;
            }
        }
    }
}

bool MainWindow::checkFileExtension( const QString & fpath ) {
    if (fpath.endsWith(".sc") || fpath.endsWith(".scd") || fpath.endsWith(".txt") ||
        fpath.endsWith(".schelp")) {
        return true;
    }
    int ret = QMessageBox::question(this, tr("Open binary file?"), fpath +
                tr("\n\nThe file has an unrecognized extension. It may be a binary file. Would you still like to open it?"),
                QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if(ret != QMessageBox::Ok)
        return false;

    return true;
}

void MainWindow::dropEvent( QDropEvent * event )
{
    const QMimeData * data = event->mimeData();
    if (data->hasUrls()) {
        foreach (QUrl url, data->urls()) {
            if ( QURL_IS_LOCAL_FILE(url) ) {
                QString fpath = url.toLocalFile();
                if (MainWindow::checkFileExtension(fpath))
                    Main::documentManager()->open(fpath);
            }
        }
    }
}

bool MainWindow::eventFilter( QObject *object, QEvent *event )
{
    switch(event->type()) {
    case QEvent::ShortcutOverride:
    {
        QKeyEvent *key_event = static_cast<QKeyEvent*>(event);
        if (key_event->key() == 0) {
            // FIXME:
            // On Mac OS, for some global menu items, there is a  ShortcutOverride event with
            // key == 0, which seems like a Qt bug.
            // Text widgets override all events with key < Qt::Key_Escape, which includes 0.
            // Instead, prevent overriding such events:
            event->ignore();
            return true;
        }
        break;
    }
    default:
        break;
    }

    return QMainWindow::eventFilter(object, event);
}

//////////////////////////// ClockStatusBox ////////////////////////////

ClockStatusBox::ClockStatusBox(QWidget * parent):
    StatusLabel(parent)
{
    setTextColor(Qt::green);
    mTimerId = startTimer(1000);
    updateTime();
}

ClockStatusBox::~ClockStatusBox()
{
    killTimer(mTimerId);
}

void ClockStatusBox::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mTimerId)
        updateTime();
}

void ClockStatusBox::updateTime()
{
    setText(QTime::currentTime().toString());
}

} // namespace ScIDE
