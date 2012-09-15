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
#include "session_switch_dialog.hpp"
#include "sessions_dialog.hpp"
#include "tool_box.hpp"
#include "../core/main.hpp"
#include "../core/doc_manager.hpp"
#include "../core/session_manager.hpp"
#include "../core/sc_server.hpp"
#include "code_editor/sc_editor.hpp"
#include "settings/dialog.hpp"

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

    setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );

    // Construct status bar:

    mLangStatus = new StatusLabel();
    mLangStatus->setText("Inactive");
    mServerStatus = new StatusLabel();
    onServerStatusReply(0, 0, 0, 0, 0, 0);

    mStatusBar = statusBar();
    mStatusBar->addPermanentWidget( new QLabel("Interpreter:") );
    mStatusBar->addPermanentWidget( mLangStatus );
    mStatusBar->addPermanentWidget( new QLabel("Server:") );
    mStatusBar->addPermanentWidget( mServerStatus );

    // Code editor
    mEditors = new MultiEditor(main);

    // Tools

    mCmdLine = new CmdLine("Command Line:");
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
    mDocListDock = new DocumentsDock(main->documentManager(), this);
    mDocListDock->setObjectName("documents-dock");
    addDockWidget(Qt::RightDockWidgetArea, mDocListDock);
    mDocListDock->hide();


    mPostDock = new PostDock(this);
    mPostDock->setObjectName("post-dock");
    addDockWidget(Qt::LeftDockWidgetArea, mPostDock);

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
            mPostDock->mPostWindow, SLOT( post(QString) ) );
    // Interpreter: monitor running state
    connect(main->scProcess(), SIGNAL( stateChanged(QProcess::ProcessState) ),
            this, SLOT( onInterpreterStateChanged(QProcess::ProcessState) ) );
    // Interpreter: forward status messages
    connect(main->scProcess(), SIGNAL(statusMessage(const QString&)),
            this, SLOT(showStatusMessage(const QString&)));

    // Document list interaction
    connect(mDocListDock->list(), SIGNAL(clicked(Document*)),
            mEditors, SLOT(setCurrent(Document*)));
    connect(mEditors, SIGNAL(currentDocumentChanged(Document*)),
            mDocListDock->list(), SLOT(setCurrent(Document*)),
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

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));

    // ToolBox
    connect(mToolBox->closeButton(), SIGNAL(clicked()), this, SLOT(hideToolBox()));

    connect(main->scResponder(), SIGNAL(serverRunningChanged(bool,QString,int)), this, SLOT(onServerRunningChanged(bool,QString,int)));
    connect(main->scServer(), SIGNAL(updateServerStatus(int,int,int,int,float,float)), this, SLOT(onServerStatusReply(int,int,int,int,float,float)));

    createActions();
    createMenus();

    // Must be called after createAtions(), because it accesses an action:
    onServerRunningChanged(false, "", 0);

    mServerStatus->addAction( action(ServerToggleRunning) );
    mServerStatus->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Initialize recent documents menu
    updateRecentDocsMenu();

    QIcon icon;
    icon.addFile(":/icons/sc-cube-128");
    icon.addFile(":/icons/sc-cube-48");
    icon.addFile(":/icons/sc-cube-32");
    icon.addFile(":/icons/sc-cube-16");
    QApplication::setWindowIcon(icon);

    updateWindowTitle();
}

void MainWindow::createActions()
{
    Settings::Manager *settings = mMain->settings();
    settings->beginGroup("IDE/shortcuts");

    QAction *act;

    // File
    mActions[Quit] = act = new QAction(
        QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
    act->setShortcut(tr("Ctrl+Q", "Quit application"));
    act->setStatusTip(tr("Quit SuperCollider IDE"));
    QObject::connect( act, SIGNAL(triggered()), this, SLOT(onQuit()) );

    mActions[DocNew] = act = new QAction(
        QIcon::fromTheme("document-new"), tr("&New"), this);
    act->setShortcut(tr("Ctrl+N", "New document"));
    act->setStatusTip(tr("Create a new document"));
    connect(act, SIGNAL(triggered()), this, SLOT(newDocument()));

    mActions[DocOpen] = act = new QAction(
        QIcon::fromTheme("document-open"), tr("&Open..."), this);
    act->setShortcut(tr("Ctrl+O", "Open document"));
    act->setStatusTip(tr("Open an existing file"));
    connect(act, SIGNAL(triggered()), this, SLOT(openDocument()));

    mActions[DocSave] = act = new QAction(
        QIcon::fromTheme("document-save"), tr("&Save"), this);
    act->setShortcut(tr("Ctrl+S", "Save document"));
    act->setStatusTip(tr("Save the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocument()));

    mActions[DocSaveAs] = act = new QAction(
        QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    act->setShortcut(tr("Ctrl+Shift+S", "Save &As..."));
    act->setStatusTip(tr("Save the current document into a different file"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));

    mActions[DocSaveAll] = act = new QAction(
        QIcon::fromTheme("document-save"), tr("Save All..."), this);
    act->setShortcut(tr("Ctrl+Alt+S", "Save all documents"));
    act->setStatusTip(tr("Save all open documents"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveAllDocuments()));

    mActions[DocClose] = act = new QAction(
        QIcon::fromTheme("window-close"), tr("&Close"), this);
    act->setShortcut(tr("Ctrl+W", "Close document"));
    act->setStatusTip(tr("Close the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(closeDocument()));

    mActions[DocCloseAll] = act = new QAction(
        QIcon::fromTheme("window-close"), tr("Close All..."), this);
    act->setShortcut(tr("Ctrl+Shift+W", "Close all documents"));
    act->setStatusTip(tr("Close all documents"));
    connect(act, SIGNAL(triggered()), this, SLOT(closeAllDocuments()));

    mActions[DocReload] = act = new QAction(
        QIcon::fromTheme("view-refresh"), tr("&Reload"), this);
    act->setShortcut(tr("F5", "Reload document"));
    act->setStatusTip(tr("Reload the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(reloadDocument()));

    mActions[ClearRecentDocs] = act = new QAction(tr("Clear", "Clear recent documents"), this);
    connect(act, SIGNAL(triggered()),
            Main::instance()->documentManager(), SLOT(clearRecents()));

    // Sessions
    mActions[NewSession] = act = new QAction(
        QIcon::fromTheme("document-new"), tr("&New Session"), this);
    act->setStatusTip(tr("Open a new session"));
    connect(act, SIGNAL(triggered()), this, SLOT(newSession()));

    mActions[SaveSessionAs] = act = new QAction(
        QIcon::fromTheme("document-save-as"), tr("Save Session &As..."), this);
    act->setStatusTip(tr("Save the current session with a different name"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveCurrentSessionAs()));

    mActions[ManageSessions] = act = new QAction(
        tr("&Manage Sessions..."), this);
    connect(act, SIGNAL(triggered()), this, SLOT(openSessionsDialog()));

    mActions[OpenSessionSwitchDialog] = act = new QAction(
        tr("&Switch Session..."), this);
    connect(act, SIGNAL(triggered()), this, SLOT(showSwitchSessionDialog()));
    act->setShortcut(tr("Ctrl+Shift+Q", "Switch Session"));

    // Edit
    mActions[Find] = act = new QAction(
        QIcon::fromTheme("edit-find"), tr("&Find..."), this);
    act->setShortcut(tr("Ctrl+F", "Find"));
    act->setStatusTip(tr("Find text in document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showFindTool()));

    mActions[Replace] = act = new QAction(
        QIcon::fromTheme("edit-replace"), tr("&Replace..."), this);
    act->setShortcut(tr("Ctrl+R", "Replace"));
    act->setStatusTip(tr("Find and replace text in document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showReplaceTool()));

    // View
    mActions[ShowCmdLine] = act = new QAction(tr("&Command Line"), this);
    act->setStatusTip(tr("Command line for quick code evaluation"));
    act->setShortcut(tr("Ctrl+E", "Show command line"));
    connect(act, SIGNAL(triggered()), this, SLOT(showCmdLine()));

    mActions[ShowGoToLineTool] = act = new QAction(tr("&Go To Line"), this);
    act->setStatusTip(tr("Tool to jump to a line by number"));
    act->setShortcut(tr("Ctrl+G", "Show go-to-line tool"));
    connect(act, SIGNAL(triggered()), this, SLOT(showGoToLineTool()));

    mActions[CloseToolBox] = act = new QAction(
        QIcon::fromTheme("window-close"), tr("&Close Tool Panel"), this);
    act->setStatusTip(tr("Close any open tool panel"));
    act->setShortcut(tr("Esc", "Close tool box"));
    connect(act, SIGNAL(triggered()), this, SLOT(hideToolBox()));

    mActions[ShowFullScreen] = act = new QAction(tr("&Full Screen"), this);
    act->setCheckable(false);
    act->setShortcut(tr("Ctrl+Shift+F", "Show ScIDE in Full Screen"));
    connect(act, SIGNAL(triggered()), this, SLOT(toggleFullScreen()));

    mActions[ClearPostWindow] = act = new QAction(
        QIcon::fromTheme("window-clearpostwindow"), tr("Clear Post Window"), this);
    act->setStatusTip(tr("Clear Post Window"));
    act->setShortcut(tr("Ctrl+Shift+C", "Clear Post Window"));
    connect(act, SIGNAL(triggered()), mPostDock->mPostWindow, SLOT(clear()));

    mActions[FocusPostWindow] = act = new QAction( tr("Focus Post Window"), this);
    act->setStatusTip(tr("Focus Post Window"));
    act->setShortcut(tr("Ctrl+L", "Focus Post Window"));
    connect(act, SIGNAL(triggered()), mPostDock->mPostWindow, SLOT(setFocus()));

    mActions[LookupReferences] = act = new QAction(
        QIcon::fromTheme("window-lookupreferences"), tr("Look Up References"), this);
    act->setShortcut(tr("Ctrl+Shift+U", "Look Up References"));
    connect(act, SIGNAL(triggered()), this, SLOT(lookupReferences()));

    mActions[LookupDefinition] = act = new QAction(
        QIcon::fromTheme("window-lookupdefinition"), tr("Look Up Definition"), this);
    act->setShortcut(tr("Ctrl+Shift+I", "Look Up Definition"));
    connect(act, SIGNAL(triggered()), this, SLOT(lookupDefinition()));

    mActions[LookupDocumentation] = act = new QAction(
        QIcon::fromTheme("window-lookupDocumentation"), tr("Look Up Documentation"), this);
    act->setShortcut(tr("Ctrl+Shift+D", "Look Up Documentation"));
    connect(act, SIGNAL(triggered()), this, SLOT(lookupDocumentation()));

    // Language
    mActions[OpenDefinition] = act = new QAction(tr("Open Class/Method Definition"), this);
    act->setShortcut(tr("Ctrl+I", "Open definition of selected class or method"));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(openDefinition()));

    mActions[FindReferences] = act = new QAction(tr("Find References"), this);
    act->setShortcut(tr("Ctrl+U", "Find References"));
    connect(act, SIGNAL(triggered(bool)), this, SLOT(findReferences()));

    // Settings
    mActions[ShowSettings] = act = new QAction(tr("&Configure IDE..."), this);
    act->setStatusTip(tr("Show configuration dialog"));
    connect(act, SIGNAL(triggered()), this, SLOT(showSettings()));


    // Help
    mActions[Help] = act = new QAction(
    QIcon::fromTheme("system-help"), tr("Open Help Browser"), this);
    act->setStatusTip(tr("Open help."));
    connect(act, SIGNAL(triggered()), this, SLOT(openHelp()));

    mActions[HelpForSelection] = act = new QAction(
    QIcon::fromTheme("system-help"), tr("&Help for Selection"), this);
    act->setShortcut(tr("Ctrl+D", "Help for selection"));
    act->setStatusTip(tr("Find help for selected text"));
    connect(act, SIGNAL(triggered()), this, SLOT(openDocumentation()));

    // Server
    mActions[ServerToggleRunning] = act = new QAction(tr("Boot or quit server"), this);
    act->setShortcut(tr("Ctrl+B", "Boot or quit default server"));
    connect(act, SIGNAL(triggered()), this, SLOT(serverToggleRunning()));

    mActions[ServerReboot] = act = new QAction(tr("Reboot server"), this);
    act->setShortcut(tr("Ctrl+Shift+B", "Reboot default server"));
    connect(act, SIGNAL(triggered()), this, SLOT(serverReboot()));

    mActions[ServerShowMeters] = act = new QAction(tr("Show server meter"), this);
    act->setShortcut(tr("Ctrl+M", "Show server meter"));
    connect(act, SIGNAL(triggered()), this, SLOT(serverShowMeters()));

    mActions[ServerDumpNodeTree] = act = new QAction(tr("Dump node tree"), this);
    act->setShortcut(tr("Ctrl+T", "Dump node tree"));
    connect(act, SIGNAL(triggered()), this, SLOT(serverDumpNodeTree()));

    mActions[ServerDumpNodeTreeWithControls] = act = new QAction(tr("Dump node tree with controls"), this);
    act->setShortcut(tr("Ctrl+Shift+T", "Dump node tree with controls"));
    connect(act, SIGNAL(triggered()), this, SLOT(serverDumpNodeTreeWithControls()));

    settings->endGroup(); // IDE/shortcuts;


    // Add actions to settings
    for (int i = 0; i < ActionCount; ++i)
        settings->addAction( mActions[i] );
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
    menu->addAction( mActions[Replace] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::IndentLineOrRegion) );
    menu->addAction( mEditors->action(MultiEditor::ToggleComment) );
    menu->addAction( mEditors->action(MultiEditor::ToggleOverwriteMode) );
    menu->addAction( mEditors->action(MultiEditor::SelectRegion) );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&View"), this);
    submenu = new QMenu(tr("&Docks"), this);
    submenu->addAction( mPostDock->toggleViewAction() );
    submenu->addAction( mDocListDock->toggleViewAction() );
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
    menu->addAction( mActions[ClearPostWindow] );
    menu->addSeparator();
    menu->addAction( mActions[ShowFullScreen] );
    menu->addSeparator();
    menu->addAction( mActions[OpenDefinition] );
    menu->addAction( mActions[LookupDefinition] );
    menu->addAction( mActions[FindReferences] );
    menu->addAction( mActions[LookupReferences] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(SCProcess::StartSCLang) );
    menu->addAction( mMain->scProcess()->action(SCProcess::StopSCLang) );
    menu->addAction( mMain->scProcess()->action(SCProcess::RestartSCLang) );
    menu->addAction( mMain->scProcess()->action(SCProcess::RecompileClassLibrary) );
    menu->addSeparator();
    menu->addAction( mActions[ServerToggleRunning] );
    menu->addAction( mActions[ServerReboot] );
    menu->addAction( mActions[ServerShowMeters] );
    menu->addAction( mActions[ServerDumpNodeTree] );
    menu->addAction( mActions[ServerDumpNodeTreeWithControls] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EvaluateCurrentDocument) );
    menu->addAction( mEditors->action(MultiEditor::EvaluateRegion) );
    menu->addAction( mEditors->action(MultiEditor::EvaluateLine) );
    menu->addSeparator();
    menu->addAction( mMain->scProcess()->action(ScIDE::SCProcess::RunMain) );
    menu->addAction( mMain->scProcess()->action(ScIDE::SCProcess::StopMain) );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Settings"), this);
    menu->addAction( mActions[ShowSettings] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Help"), this);
    menu->addAction( mActions[Help] );
    menu->addAction( mActions[HelpForSelection] );
    menu->addAction( mActions[LookupDocumentation] );

    menuBar()->addMenu(menu);
}

void MainWindow::saveWindowState()
{
    Settings::Manager *settings = Main::settings();
    settings->beginGroup("IDE/mainWindow");
    settings->setValue("geometry", this->saveGeometry().toBase64());
    settings->setValue("state", this->saveState().toBase64());
    settings->endGroup();
}

void MainWindow::restoreWindowState()
{
    Settings::Manager *settings = Main::settings();
    settings->beginGroup("IDE/mainWindow");

    QByteArray geom = QByteArray::fromBase64( settings->value("geometry").value<QByteArray>() );
    if (!geom.isEmpty())
        restoreGeometry(geom);
    else
        setWindowState( windowState() & ~Qt::WindowFullScreen | Qt::WindowMaximized );

    QByteArray state = QByteArray::fromBase64( settings->value("state").value<QByteArray>() );
    if (!state.isEmpty())
        restoreState(state);

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
    if (promptSaveDocs())
        mMain->sessionManager()->newSession();
}

void MainWindow::saveCurrentSessionAs()
{
    QString name = QInputDialog::getText( this,
                                          "Save Current Session",
                                          "Enter a name for the session:" );

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
    if (session) {
        session->beginGroup("mainWindow");
        QByteArray geom = QByteArray::fromBase64( session->value("geometry").value<QByteArray>() );
        QByteArray state = QByteArray::fromBase64( session->value("state").value<QByteArray>() );
        session->endGroup();

        // Workaround for Qt bug 4397:
        setWindowState(Qt::WindowNoState);

        if (!geom.isEmpty())
            restoreGeometry(geom);
        if (!state.isEmpty())
            restoreState(state);

        updateClockWidget(isFullScreen());
    }

    mEditors->switchSession(session);

    updateWindowTitle();
}

void MainWindow::saveSession( Session *session )
{
    session->beginGroup("mainWindow");
    session->setValue("geometry", this->saveGeometry().toBase64());
    session->setValue("state", this->saveState().toBase64());
    session->endGroup();

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

    mActions[DocSave]->setEnabled(doc);
    mActions[DocSaveAs]->setEnabled(doc);
    mActions[DocClose]->setEnabled(doc);

    ScCodeEditor *editor = mEditors->currentEditor();
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
        text = "Inactive";
        color = Qt::white;
        break;
    case QProcess::Starting:
        text = "Booting";
        color = QColor(255,255,0);
        break;
    case QProcess::Running:
        text = "Active";
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
    QAction *serverStatusAction = mActions[ServerToggleRunning];

    mServerStatus->setTextColor( running ? Qt::green : Qt::white);
    if (!running) {
        onServerStatusReply(0, 0, 0, 0, 0, 0);

        serverStatusAction->setText( tr("&Boot Server") );
        serverStatusAction->setStatusTip(tr("Boot sound synthesis server"));
    } else {
        serverStatusAction->setText( tr("&Quit Server") );
        serverStatusAction->setStatusTip(tr("Quit sound synthesis server"));
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
    DocumentManager *mng = Main::instance()->documentManager();
    if (forceChoose || doc->filePath().isEmpty()) {
        QFileDialog dialog(mInstance);
        dialog.setAcceptMode( QFileDialog::AcceptSave );

        QStringList filters = (QStringList()
                               << "SuperCollider Document(*.scd)"
                               << "SuperCollider Class file(*.sc)"
                               << "SCDoc(*.schelp)"
                               << "All files(*)");

        dialog.setNameFilters(filters);
        dialog.setDefaultSuffix("scd");

        if (dialog.exec() == QDialog::Accepted)
            return mng->saveAs(doc, dialog.selectedFiles()[0]);
        else
            return false;
    } else
        return mng->save(doc);
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

    ScCodeEditor * currentEditor = mEditors->currentEditor();
    if (currentEditor) {
        Document * currentDocument = currentEditor->document();
        QFileInfo filePath (currentDocument->filePath());
        dialog.setDirectory(filePath.dir());
    }

    QStringList filters;
    filters
        << "All files(*)"
        << "SuperCollider(*.scd *.sc)"
        << "SCDoc(*.schelp)";
    dialog.setNameFilters(filters);

    if (dialog.exec())
    {
        QStringList filenames = dialog.selectedFiles();
        foreach(QString filename, filenames)
            mMain->documentManager()->open(filename);
    }
}

void MainWindow::saveDocument()
{
    ScCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    Q_ASSERT(doc);

    MainWindow::save(doc);
}

void MainWindow::saveDocumentAs()
{
    ScCodeEditor *editor = mEditors->currentEditor();
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
    ScCodeEditor *editor = mEditors->currentEditor();
    if(!editor) return;

    Q_ASSERT(editor->document());
    MainWindow::reload(editor->document());
}

void MainWindow::closeDocument()
{
    ScCodeEditor *editor = mEditors->currentEditor();
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
    QList<Document*> docs = mMain->documentManager()->documents();
    QList<Document*> unsavedDocs;
    foreach(Document* doc, docs)
        if(doc->textDocument()->isModified())
            unsavedDocs.append(doc);

    if (unsavedDocs.count())
    {
        DocumentsDialog dialog(unsavedDocs, DocumentsDialog::Quit, this);

        if (!dialog.exec())
            return false;
    }

    return true;
}

void MainWindow::updateWindowTitle()
{
    Session *session = mMain->sessionManager()->currentSession();
    ScCodeEditor *editor = mEditors->currentEditor();
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
        } else
            title.append( "Untitled" );
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
    if (promptSaveDocs())
        mMain->sessionManager()->openSession( sessionName );
}

void MainWindow::openDefinition()
{
    QWidget * focussedWidget = QApplication::focusWidget();

    int indexOfMethod = focussedWidget->metaObject()->indexOfMethod("openDefinition()");
    if (indexOfMethod != -1)
        QMetaObject::invokeMethod( focussedWidget, "openDefinition", Qt::DirectConnection );
}

void MainWindow::lookupDefinition()
{
    LookupDialog dialog(mEditors);
    dialog.exec();
}

void MainWindow::lookupDocumentation()
{
    PopupTextInput * dialog = new PopupTextInput(tr("Look up Documentation For"), this);

    bool success = dialog->exec();
    if (success)
        Main::openDocumentation(dialog->textValue());

    delete dialog;
}

void MainWindow::findReferences()
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
    mEditors->applySettings(settings);
    mPostDock->mPostWindow->applySettings(settings);
    mCmdLine->applySettings(settings);
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

void MainWindow::showCmdLine()
{
    mToolBox->setCurrentWidget( mCmdLine );
    mToolBox->show();

    mCmdLine->setFocus(Qt::OtherFocusReason);
}

void MainWindow::showGoToLineTool()
{
    ScCodeEditor *editor = mEditors->currentEditor();
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
    ScCodeEditor *editor = mEditors->currentEditor();
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

void MainWindow::openDocumentation()
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
    QString code = QString("Help.gui");
    Main::scProcess()->evaluateCode(code, true);
}

void MainWindow::serverToggleRunning()
{
    ScServer *scServer = Main::scServer();

    if (scServer->isRunning())
        scServer->quit();
    else
        scServer->boot();
}

void MainWindow::serverReboot()
{
    ScServer *scServer = Main::scServer();
    scServer->reboot();
}

void MainWindow::serverShowMeters()
{
    Main::evaluateCode("ScIDE.defaultServer.meter", true);
}

void MainWindow::serverDumpNodeTree()
{
    ScServer *scServer = Main::scServer();
    scServer->queryAllNodes(false);
}

void MainWindow::serverDumpNodeTreeWithControls()
{
    ScServer *scServer = Main::scServer();
    scServer->queryAllNodes(true);
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

void MainWindow::dropEvent( QDropEvent * event )
{
    const QMimeData * data = event->mimeData();
    if (data->hasUrls()) {
        foreach (QUrl url, data->urls()) {
            if (url.scheme() == QString("file")) // LATER: use isLocalFile
                Main::documentManager()->open(url.toLocalFile());
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
