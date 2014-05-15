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
#include "code_editor/editor.hpp"
#include "editor_box.hpp"

#include "QtCollider/hacks/hacks_qt.hpp"

#include "SC_Version.hpp"

#include <QAction>
#include <QApplication>
#include <QDesktopServices>
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
#include <QString>

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
    connect(docMng, SIGNAL(titleChanged(Document*)), this, SLOT(updateWindowTitle()) );

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));
    connect(main, SIGNAL(storeSettingsRequest(Settings::Manager*)),
            this, SLOT(storeSettings(Settings::Manager*)));

    createActions();
    mainMenu = createMenus();
//    center_box->setMenuBar(mainMenu);

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
    mActions[ClearRecentDocs] = action = new QAction(tr("Clear", "Clear recent documents"), this);
    action->setStatusTip(tr("Clear list of recent documents"));
    connect(action, SIGNAL(triggered()),
            Main::instance()->documentManager(), SLOT(clearRecents()));
    settings->addAction( action, "ide-clear-recent-documents", ideCategory);

    // Help
    mActions[HelpAboutIDE]  = action =
            new QAction(QIcon::fromTheme("system-help"), tr("How to Use SuperCollider IDE"), this);
    action->setStatusTip(tr("Open the SuperCollider IDE guide"));
    connect(action, SIGNAL(triggered()), this, SLOT(openHelpAboutIDE()));

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
    //addAction( mActions[CloseToolBox] );

    // Add actions to docklets, so shortcuts work when docklets detached:

    mPostDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupDocumentation));
    mPostDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupDocumentationForCursor));
    mPostDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupImplementation));
    mPostDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupImplementationForCursor));
    mPostDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupReferences));
    mPostDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupReferencesForCursor));

    mHelpBrowserDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupDocumentation));
    mHelpBrowserDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupDocumentationForCursor));
    mHelpBrowserDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupImplementation));
    mHelpBrowserDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupImplementationForCursor));
    mHelpBrowserDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupReferences));
    mHelpBrowserDocklet->widget()->addAction(mEditors->action(MultiEditor::LookupReferencesForCursor));

}

QMenuBar *MainWindow::createMenus()
{
    MultiSplitter *curWindow = mEditors->currentBox()->multiSplitter();

    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *submenu;

    // On Mac, create a parent-less menu bar to be shared by all windows:
#ifdef Q_OS_MAC
    menuBar = new QMenuBar(0);
#else
    menuBar = this->menuBar();
    this->setMenuBar(menuBar);
#endif

    menu = new QMenu(tr("&File"), this);
    menu->addAction( mEditors->action(MultiEditor::DocNew));
    menu->addAction( mEditors->action(MultiEditor::DocOpen) );
    mRecentDocsMenu = menu->addMenu(tr("Open Recent", "Open a recent document"));
    connect(mRecentDocsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(onRecentDocAction(QAction*)));
    menu->addAction( mEditors->action(MultiEditor::DocOpenStartup) );
    menu->addAction( mEditors->action(MultiEditor::DocOpenSupportDir) );
    menu->addAction( mEditors->action(MultiEditor::DocSave) );
    menu->addAction( mEditors->action(MultiEditor::DocSaveAs) );
    menu->addAction( mEditors->action(MultiEditor::DocSaveAll) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::DocReload) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::DocClose) );
    menu->addAction( mEditors->action(MultiEditor::DocCloseAll) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::Quit) );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Session"), this);
    menu->addAction( mEditors->action(MultiEditor::NewSession) );
    menu->addAction( mEditors->action(MultiEditor::SaveSessionAs) );
    submenu = menu->addMenu(tr("&Open Session"));
    connect(submenu, SIGNAL(triggered(QAction*)),
            this, SLOT(onOpenSessionAction(QAction*)));
    mSessionsMenu = submenu;
    updateSessionsMenu();
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ManageSessions) );
    menu->addAction( mEditors->action(MultiEditor::OpenSessionSwitchDialog) );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Edit"), this);
    menu->addAction( mEditors->action(MultiEditor::Undo) );
    menu->addAction( mEditors->action(MultiEditor::Redo) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::Cut) );
    menu->addAction( mEditors->action(MultiEditor::Copy) );
    menu->addAction( mEditors->action(MultiEditor::Paste) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::Find) );
    menu->addAction( mEditors->action(MultiEditor::FindNext) );
    menu->addAction( mEditors->action(MultiEditor::FindPrevious) );
    menu->addAction( mEditors->action(MultiEditor::Replace) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::IndentWithSpaces) );
    menu->addAction( mEditors->action(MultiEditor::IndentLineOrRegion) );
    menu->addAction( mEditors->action(MultiEditor::ToggleComment) );
    menu->addAction( mEditors->action(MultiEditor::ToggleOverwriteMode) );
    menu->addAction( mEditors->action(MultiEditor::SelectRegion) );
    menu->addAction( mEditors->action(MultiEditor::SelectEnclosingBlock) );

    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ShowSettings) );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&View"), this);
    submenu = new QMenu(tr("&Docklets"), this);
    submenu->addAction( mPostDocklet->toggleViewAction() );
    submenu->addAction( mDocumentsDocklet->toggleViewAction() );
    submenu->addAction( mHelpBrowserDocklet->toggleViewAction() );
    menu->addMenu(submenu);
    menu->addSeparator();
    submenu = menu->addMenu(tr("&Tool Panels"));
    submenu->addAction( mEditors->action(MultiEditor::Find) );
    submenu->addAction( mEditors->action(MultiEditor::Replace) );
    submenu->addAction( mEditors->action(MultiEditor::ShowCmdLine) );
    submenu->addAction( mEditors->action(MultiEditor::CmdLineForCursor) );
    submenu->addAction( mEditors->action(MultiEditor::ShowGoToLineTool) );
    submenu->addSeparator();
    submenu->addAction( mEditors->action(MultiEditor::CloseToolBox) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EnlargeFont) );
    menu->addAction( mEditors->action(MultiEditor::ShrinkFont) );
    menu->addAction( mEditors->action(MultiEditor::ResetFontSize) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ShowWhitespace) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::NextDocument) );
    menu->addAction( mEditors->action(MultiEditor::PreviousDocument) );
    menu->addAction( mEditors->action(MultiEditor::SwitchDocument) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::SplitHorizontally) );
    menu->addAction( mEditors->action(MultiEditor::SplitVertically) );
    menu->addAction( mEditors->action(MultiEditor::SplitNewWindow) );
    menu->addAction( mEditors->action(MultiEditor::RemoveCurrentSplit) );
    menu->addAction( mEditors->action(MultiEditor::RemoveCurrentWindow) );
    menu->addAction( mEditors->action(MultiEditor::RemoveAllSplits) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::SwitchSplit) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::FocusPostWindow) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::ShowFullScreen) );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(ScProcess::ToggleRunning) );
    menu->addAction( mMain->scProcess()->action(ScProcess::Restart) );
    menu->addAction( mMain->scProcess()->action(ScProcess::RecompileClassLibrary) );
    menu->addSeparator();
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
    menu->addAction( mMain->scServer()->action(ScServer::VolumeUp) );
    menu->addAction( mMain->scServer()->action(ScServer::VolumeDown) );
    menu->addAction( mMain->scServer()->action(ScServer::VolumeRestore) );
    menu->addAction( mMain->scServer()->action(ScServer::Mute) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::EvaluateCurrentDocument) );
    menu->addAction( mEditors->action(MultiEditor::EvaluateRegion) );
    menu->addAction( mEditors->action(MultiEditor::EvaluateLine) );
    menu->addAction( mMain->scProcess()->action(ScIDE::ScProcess::StopMain) );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::LookupImplementationForCursor) );
    menu->addAction( mEditors->action(MultiEditor::LookupImplementation) );
    menu->addAction( mEditors->action(MultiEditor::LookupReferencesForCursor) );
    menu->addAction( mEditors->action(MultiEditor::LookupReferences) );

    menuBar->addMenu(menu);

    menu = new QMenu(tr("&Help"), this);
    menu->addAction( mActions[HelpAboutIDE] );
    menu->addSeparator();
    menu->addAction( mEditors->action(MultiEditor::Help) );
    menu->addAction( mEditors->action(MultiEditor::LookupDocumentationForCursor) );
    menu->addAction( mEditors->action(MultiEditor::LookupDocumentation) );
    menu->addSeparator();
    menu->addAction( mActions[ShowAbout] );
    menu->addAction( mActions[ShowAboutQT] );

    menuBar->addMenu(menu);
    return menuBar;
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

void MainWindow::saveSessionChanged()
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

    mEditors->action(MultiEditor::DocCloseAll)->setEnabled(doc);
    mEditors->action(MultiEditor::DocReload)->setEnabled(doc);
    mEditors->action(MultiEditor::DocSave)->setEnabled(doc);
    mEditors->action(MultiEditor::DocSaveAs)->setEnabled(doc);

    GenericCodeEditor *editor = mEditors->currentEditor();
//    mEditors->currentFindReplacePanel()->setEditor( editor );
    MultiSplitter *curWindow = mEditors->currentBox()->multiSplitter();
    curWindow->goToLineTool()->setEditor( editor );
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
    // when closing, check if the file exists. If it does not exist (its path is empty), it means that the file you are closing
    // is a temporary document, and its path is associated with a document that has not been found when opening the session.
    // To prevent damage to your session, warn the user that his action will modify the current session.
    int docIndex = mInstance->editor()->indexForDocument( doc );
    if (mInstance->editor()->tempModeIsOn() && docIndex<mInstance->editor()->originalDocumentList().count()) {
        if (doc->filePath().isEmpty()) {
            QMessageBox::StandardButton sessionMessage;
            sessionMessage = QMessageBox::warning(
                mInstance,
                tr("SuperCollider IDE"),
                tr("This document is a temporary replacement, as the original file has not been found. \n"
                   "If you close it, you'll lose it."),
                QMessageBox::Discard | QMessageBox::Cancel,
                QMessageBox::Cancel // the default
            );

            switch (sessionMessage) {
            case QMessageBox::Cancel:
                return false;
            default:;
            }
        }
    }

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

    return QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
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
        dialog.setFileMode( QFileDialog::AnyFile );

        QStringList filters = QStringList()
                              << tr("All Files (*)")
                              << tr("SuperCollider Document (*.scd)")
                              << tr("SuperCollider Class File (*.sc)")
                              << tr("SuperCollider Help Source (*.schelp)");

        dialog.setNameFilters(filters);

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
            mInstance->mLastDocumentSavePath = save_path;
            return documentManager->saveAs(doc, save_path);
        } else {
            return false;
        }
    } else
        return documentManager->save(doc);
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

    return QDesktopServices::storageLocation( QDesktopServices::HomeLocation );
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
    MultiSplitter *curWindow = mEditors->currentBox()->multiSplitter();
    curWindow->cmdLine()->applySettings(settings);
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

void MainWindow::showAbout()
{
    QString aboutString =
            "<h3>SuperCollider %1</h3>"
            "&copy; James McCartney and others.<br>"
            "<h3>SuperCollider IDE</h3>"
            "&copy; Jakob Leben, Tim Blechmann and others.<br>"
            ;
    aboutString = aboutString.arg(SC_VersionString().c_str());

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

void MainWindow::cmdLineForCursor()
{
    static const QByteArray signature = QMetaObject::normalizedSignature("openCommandLine()");

    invokeMethodOnFirstResponder(signature);
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

void MainWindow::postFocus() {
    mPostDocklet->focus();
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
