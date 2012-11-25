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
#include "../core/main.hpp"
#include "../core/doc_manager.hpp"
#include "../core/session_manager.hpp"
#include "../core/sc_server.hpp"
#include "code_editor/sc_editor.hpp"
#include "settings/dialog.hpp"

#include "SC_DirUtils.h"

#include <QAction>
#include <QApplication>
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

namespace ScIDE {

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

    mLangStatus = new StatusLabel();
    mLangStatus->setText(tr("Inactive"));
    mServerStatus = new StatusLabel();
    onServerStatusReply(0, 0, 0, 0, 0, 0);

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

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));
    connect(main, SIGNAL(storeSettingsRequest(Settings::Manager*)),
            this, SLOT(storeSettings(Settings::Manager*)));

    // ToolBox
    connect(mToolBox->closeButton(), SIGNAL(clicked()), this, SLOT(hideToolBox()));

    connect(main->scServer(), SIGNAL(runningStateChange(bool,QString,int)),
            this, SLOT(onServerRunningChanged(bool,QString,int)));
    connect(main->scServer(), SIGNAL(updateServerStatus(int,int,int,int,float,float)),
            this, SLOT(onServerStatusReply(int,int,int,int,float,float)));

    createActions();
    createMenus();

    // Must be called after createAtions(), because it accesses an action:
    onServerRunningChanged(false, "", 0);
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

    mActions[DocSaveAll] = action = new QAction(
        QIcon::fromTheme("document-save"), tr("Save All..."), this);
    action->setShortcut(tr("Ctrl+Alt+S", "Save all documents"));
    action->setStatusTip(tr("Save all open documents"));
    connect(action, SIGNAL(triggered()), this, SLOT(saveAllDocuments()));
    settings->addAction( action, "ide-document-save-all", ideCategory);

    mActions[DocClose] = action = new QAction(
        QIcon::fromTheme("window-close"), tr("&Close"), this);
    action->setShortcut(tr("Ctrl+W", "Close document"));
    action->setStatusTip(tr("Close the current document"));
    connect(action, SIGNAL(triggered()), this, SLOT(closeDocument()));
    settings->addAction( action, "ide-document-close", ideCategory);

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
    connect(action, SIGNAL(triggered()), mPostDocklet, SLOT(raiseAndFocus()));
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
    mActions[ShowSettings] = action = new QAction(tr("&Preferences"), this);
#ifdef Q_OS_MAC
    action->setShortcut(tr("Ctrl+,", "Show configuration dialog"));
#endif
    action->setStatusTip(tr("Show configuration dialog"));
    connect(action, SIGNAL(triggered()), this, SLOT(showSettings()));
    settings->addAction( action, "ide-settings-dialog", ideCategory);

    // Help
    mActions[Help] = action = new QAction(
        QIcon::fromTheme("system-help"), tr("Open &Help Browser"), this);
    action->setStatusTip(tr("Open help"));
    connect(action, SIGNAL(triggered()), this, SLOT(openHelp()));
    settings->addAction( action, "help-show", helpCategory);

    mActions[LookupDocumentationForCursor] = action = new QAction(
        QIcon::fromTheme("system-help"), tr("Look Up Documentation for Cursor"), this);
    action->setShortcut(tr("Ctrl+D", "Look Up Documentation for Cursor"));
    action->setStatusTip(tr("Look up documentation for text under cursor"));
    connect(action, SIGNAL(triggered()), this, SLOT(lookupDocumentationForCursor()));
    settings->addAction( action, "help-lookup-for-cursor", helpCategory);

    mActions[LookupDocumentation] = action = new QAction(
        QIcon::fromTheme("system-help"), tr("Look Up Documentation..."), this);
    action->setShortcut(tr("Ctrl+Shift+D", "Look Up Documentation"));
    action->setStatusTip(tr("Enter text to look up in documentation"));
    connect(action, SIGNAL(triggered()), this, SLOT(lookupDocumentation()));
    settings->addAction( action, "help-lookup", helpCategory);

    mActions[ShowAbout] = action = new QAction(
        QIcon::fromTheme("show-about"), tr("&About SuperCollider"), this);
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
}

void MainWindow::createMenus()
{
    QMenu *menu;
    QMenu *submenu;

    menu = new QMenu(tr("&File"), this);
    menu->addAction( mActions[DocNew] );
    menu->addAction( mActions[DocOpen] );
    mRecentDocsMenu = menu->addMenu(tr("Open Recent", "Open a recent document"));
    connect(mRecentDocsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(onRecentDocAction(QAction*)));
    menu->addAction( mActions[DocOpenStartup] );
    menu->addAction( mActions[DocSave] );
    menu->addAction( mActions[DocSaveAs] );
    menu->addAction( mActions[DocSaveAll] );
    menu->addSeparator();
    menu->addAction( mActions[DocReload] );
    menu->addSeparator();
    menu->addAction( mActions[DocClose] );
    menu->addAction( mActions[DocCloseAll] );
    menu->addSeparator();
    menu->addAction( mActions[Quit] );

    menuBar()->addMenu(menu);

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

    menuBar()->addMenu(menu);

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

    menu->addSeparator();
    menu->addAction( mActions[ShowSettings] );

    menuBar()->addMenu(menu);

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
    submenu->addAction( mActions[ShowGoToLineTool] );
    submenu->addSeparator();
    submenu->addAction( mActions[CloseToolBox] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EnlargeFont) );
    menu->addAction( mEditors->action(MultiEditor::ShrinkFont) );
    menu->addAction( mEditors->action(MultiEditor::ResetFontSize) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ShowWhitespace) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::NextDocument) );
    menu->addAction( mEditors->action(MultiEditor::PreviousDocument) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::SplitHorizontally) );
    menu->addAction( mEditors->action(MultiEditor::SplitVertically) );
    menu->addAction( mEditors->action(MultiEditor::RemoveCurrentSplit) );
    menu->addAction( mEditors->action(MultiEditor::RemoveAllSplits) );
    menu->addSeparator();
    menu->addAction( mActions[FocusPostWindow] );
    menu->addSeparator();
    menu->addAction( mActions[ShowFullScreen] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(ScProcess::ToggleRunning) );
    menu->addAction( mMain->scProcess()->action(ScProcess::Restart) );
    menu->addAction( mMain->scProcess()->action(ScProcess::RecompileClassLibrary) );
    menu->addSeparator();
    menu->addAction( mMain->scServer()->action(ScServer::ToggleRunning) );
    menu->addAction( mMain->scServer()->action(ScServer::Reboot) );
    menu->addAction( mMain->scServer()->action(ScServer::ShowMeters) );
    menu->addAction( mMain->scServer()->action(ScServer::DumpNodeTree) );
    menu->addAction( mMain->scServer()->action(ScServer::DumpNodeTreeWithControls) );
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

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Help"), this);
    menu->addAction( mActions[Help] );
    menu->addAction( mActions[LookupDocumentationForCursor] );
    menu->addAction( mActions[LookupDocumentation] );
    menu->addSeparator();
    menu->addAction( mActions[ShowAbout] );
    menu->addAction( mActions[ShowAboutQT] );

    menuBar()->addMenu(menu);

    mServerStatus->addAction( mMain->scServer()->action(ScServer::ToggleRunning) );
    mServerStatus->addAction( mMain->scServer()->action(ScServer::Reboot) );
    mServerStatus->addAction( mMain->scServer()->action(ScServer::ShowMeters) );
    mServerStatus->addAction( mMain->scServer()->action(ScServer::DumpNodeTree) );
    mServerStatus->addAction( mMain->scServer()->action(ScServer::DumpNodeTreeWithControls) );
    mServerStatus->setContextMenuPolicy(Qt::ActionsContextMenu);
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

    mActions[DocClose]->setEnabled(doc);
    mActions[DocCloseAll]->setEnabled(doc);
    mActions[DocReload]->setEnabled(doc);
    mActions[DocSave]->setEnabled(doc);
    mActions[DocSaveAs]->setEnabled(doc);

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

void MainWindow::onRecentDocAction( QAction *action )
{
    mMain->documentManager()->open(action->text());
}

void MainWindow::onInterpreterStateChanged( QProcess::ProcessState state )
{
    QString text;
    QColor color;

    switch(state) {
    case QProcess::NotRunning:
        toggleInterpreterActions(false);

        text = tr("Inactive");
        color = Qt::white;
        break;

    case QProcess::Starting:
        text = tr("Booting");
        color = QColor(255,255,0);
        break;

    case QProcess::Running:
        toggleInterpreterActions(true);

        text = tr("Active");
        color = Qt::green;
        break;
    }

    mLangStatus->setText(text);
    mLangStatus->setTextColor(color);
}


void MainWindow::onServerStatusReply(int ugens, int synths, int groups, int synthDefs, float avgCPU, float peakCPU)
{
    QString statusString =
            QString("%1% %2% %3u %4s %5g %6d")
            .arg(avgCPU,  5, 'f', 2)
            .arg(peakCPU, 5, 'f', 2)
            .arg(ugens,     4)
            .arg(synths,    4)
            .arg(groups,    4)
            .arg(synthDefs, 4);

    mServerStatus->setText(statusString);
}

void MainWindow::onServerRunningChanged(bool running, const QString &, int)
{
    mServerStatus->setTextColor( running ? Qt::green : Qt::white);
    if (!running) {
        onServerStatusReply(0, 0, 0, 0, 0, 0);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!quit()) event->ignore();
}

bool MainWindow::close( Document *doc )
{
    if (doc->textDocument()->isModified())
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

bool MainWindow::save( Document *doc, bool forceChoose )
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

        QStringList filters = (QStringList()
                               << tr("SuperCollider Document (*.scd)")
                               << tr("SuperCollider Class file (*.sc)")
                               << "SCDoc (*.schelp)"
                               << tr("All files (*)"));

        dialog.setNameFilters(filters);

        if(doc->filePath().isEmpty()){
            dialog.setDefaultSuffix("scd");
        }else{
            QString fp = doc->filePath();
            if(fp.endsWith(".scd"))
                dialog.setFilter(filters[0]);
            else if(fp.endsWith(".sc"))
                dialog.setFilter(filters[1]);
            else if(fp.endsWith(".schelp"))
                dialog.setFilter(filters[2]);
            else
                dialog.setFilter(filters[3]);
            dialog.selectFile(fp);
        }

        if (dialog.exec() == QDialog::Accepted)
            return documentManager->saveAs(doc, dialog.selectedFiles()[0]);
        else
            return false;
    } else
        return documentManager->save(doc);
}

void MainWindow::newDocument()
{
    mMain->documentManager()->create();
}

void MainWindow::openDocument()
{
    QFileDialog dialog (this, Qt::Dialog);
    dialog.setModal(true);
    dialog.setWindowModality(Qt::ApplicationModal);

    dialog.setFileMode( QFileDialog::ExistingFiles );

    GenericCodeEditor * currentEditor = mEditors->currentEditor();
    if (currentEditor) {
        Document * currentDocument = currentEditor->document();
        QFileInfo filePath (currentDocument->filePath());
        dialog.setDirectory(filePath.dir());
    }

    QStringList filters;
    filters
        << tr("All files (*)")
        << tr("SuperCollider (*.scd *.sc)")
        << tr("SCDoc (*.schelp)");
    dialog.setNameFilters(filters);

    if (dialog.exec())
    {
        QStringList filenames = dialog.selectedFiles();
        foreach(QString filename, filenames)
            mMain->documentManager()->open(filename);
    }
}

void MainWindow::openStartupFile()
{
    char configDir[PATH_MAX];
    sc_GetUserConfigDirectory(configDir, PATH_MAX);

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

    mMain->documentManager()->open( filePath );
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
        if(doc->textDocument()->isModified())
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
                pathString.replace(0, homePath.size(), QString("~"));

            QString titleString = QString("%1 (%2)").arg(info.fileName(), pathString);

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
            mClockLabel = new StatusClockLabel(this);
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
    QWidget * focussedWidget = QApplication::focusWidget();

    int indexOfMethod = focussedWidget->metaObject()->indexOfMethod("openDefinition()");
    if (indexOfMethod != -1)
        QMetaObject::invokeMethod( focussedWidget, "openDefinition", Qt::DirectConnection );
}

void MainWindow::lookupImplementation()
{
    LookupDialog dialog(mEditors);
    dialog.exec();
}

void MainWindow::lookupReferencesForCursor()
{
    QWidget * focussedWidget = QApplication::focusWidget();

    int indexOfMethod = focussedWidget->metaObject()->indexOfMethod("findReferences()");
    if (indexOfMethod != -1)
        QMetaObject::invokeMethod( focussedWidget, "findReferences", Qt::DirectConnection );
}

void MainWindow::lookupReferences()
{
    ReferencesDialog dialog(parentWidget());
    dialog.exec();
}


void MainWindow::showStatusMessage( QString const & string )
{
    mStatusBar->showMessage(string, 3000);
}

void MainWindow::applySettings( Settings::Manager * settings )
{
    mPostDocklet->mPostWindow->applySettings(settings);
    mHelpBrowserDocklet->browser()->applySettings(settings);
    mCmdLine->applySettings(settings);
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
            "&copy; James McCartney and others.<br>"
            "<h3>SuperCollider IDE</h3>"
            "&copy; Jakob Leben, Tim Blechmann and others.<br>"
            "Development partially funded by Kiberpipa."
            ;
    aboutString = aboutString.arg("3.6");

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
    mToolBox->hide();
    GenericCodeEditor *editor = mEditors->currentEditor();
    if (editor) {
        // This slot is mapped to Escape, so also clear highlighting
        // whenever invoked:
        editor->clearSearchHighlighting();
        if (!editor->hasFocus())
            editor->setFocus(Qt::OtherFocusReason);
    }
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
    PopupTextInput * dialog = new PopupTextInput(tr("Look up Documentation For"), this);

    bool success = dialog->exec();
    if (success)
        Main::openDocumentation(dialog->textValue());

    delete dialog;
}

void MainWindow::lookupDocumentationForCursor()
{
    QWidget * focussedWidget = QApplication::focusWidget();

    bool documentationOpened = false;

    int indexOfMethod = focussedWidget->metaObject()->indexOfMethod("openDocumentation()");
    if (indexOfMethod != -1)
        QMetaObject::invokeMethod( focussedWidget, "openDocumentation", Qt::DirectConnection,
                                   Q_RETURN_ARG(bool, documentationOpened) );

    if (!documentationOpened)
        openHelp();
}

void MainWindow::openHelp()
{
    if (mHelpBrowserDocklet->browser()->url().isEmpty())
        mHelpBrowserDocklet->browser()->goHome();
    mHelpBrowserDocklet->raiseAndFocus();
}

void MainWindow::dragEnterEvent( QDragEnterEvent * event )
{
    if (event->mimeData()->hasUrls()) {
        foreach (QUrl url, event->mimeData()->urls()) {
            if (url.scheme() == QString("file")) { // LATER: use isLocalFile
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
            if (url.scheme() == QString("file")) { // LATER: use isLocalFile
                QString fpath = url.toLocalFile();
                if (MainWindow::checkFileExtension(fpath))
                    Main::documentManager()->open(fpath);
            }
        }
    }
}

//////////////////////////// StatusLabel /////////////////////////////////

StatusLabel::StatusLabel(QWidget *parent) : QLabel(parent)
{
    setAutoFillBackground(true);
    setMargin(3);
    setAlignment(Qt::AlignCenter);
    setBackground(Qt::black);
    setTextColor(Qt::white);

    QFont font("Monospace");
    font.setStyleHint(QFont::Monospace);
    font.setBold(true);
    setFont(font);
}

void StatusLabel::setBackground(const QBrush & brush)
{
    QPalette plt(palette());
    plt.setBrush(QPalette::Window, brush);
    setPalette(plt);
}

void StatusLabel::setTextColor(const QColor & color)
{
    QPalette plt(palette());
    plt.setColor(QPalette::WindowText, color);
    setPalette(plt);
}

//////////////////////////// StatusClockLabel ////////////////////////////

StatusClockLabel::StatusClockLabel(QWidget * parent):
    StatusLabel(parent)
{
    setTextColor(Qt::green);
    mTimerId = startTimer(1000);
    updateTime();
}

StatusClockLabel::~StatusClockLabel()
{
    killTimer(mTimerId);
}

void StatusClockLabel::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mTimerId)
        updateTime();
}

void StatusClockLabel::updateTime()
{
    setText(QTime::currentTime().toString());
}

} // namespace ScIDE
