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

/*
    Every Window is set with a Multisplitter, which contains various Boxes, which contain various Editors.
    Every Editor has one document.
*/

#include "multi_editor.hpp"
#include "editor_box.hpp"
#include "main_window.hpp"
#include "lookup_dialog.hpp"
#include "code_editor/sc_editor.hpp"
#include "util/multi_splitter.hpp"
#include "../core/doc_manager.hpp"
#include "../core/sig_mux.hpp"
#include "../core/main.hpp"
#include "../core/sc_process.hpp"
#include "../core/session_manager.hpp"
#include "post_window.hpp"
#include "cmd_line.hpp"
#include "find_replace_tool.hpp"
#include "goto_line_tool.hpp"
#include "tool_box.hpp"
#include "../core/util/standard_dirs.hpp"
#include "session_switch_dialog.hpp"
#include "sessions_dialog.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QShortcut>
#include <QStyle>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMetaType>
#include <QMessageBox>
#include <QFileDialog>
#include <QUrl>
#include <QDesktopServices>


namespace ScIDE {

MultiEditor::MultiEditor( Main *main, QWidget * parent ) :
    QWidget(parent),
    mEditorSigMux(new SignalMultiplexer(this)),
    mBoxSigMux(new SignalMultiplexer(this)),
#ifdef SC_DARWIN
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon) )
#else
    mDocModifiedIcon( QIcon::fromTheme("document-save") )
#endif
{
    mMain = main;
    tempMode = false;
    sessionChanged = false;
    mCurrentEditorBox = 0;

    mDocs.clear();

    mSplitter = new MultiSplitter(0);
    editorList << mSplitter;
    defaultBox = newBox(mSplitter);
    mSplitter->addWidget(defaultBox);

    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    l->addWidget(mSplitter);
    l->addWidget(mSplitter->toolBox());
    setLayout(l);

    makeSignalConnections();

    connect(mSplitter->toolBox()->closeButton(), SIGNAL(clicked()), this, SLOT(hideToolBox()));

    mBoxSigMux->connect(SIGNAL(currentChanged(GenericCodeEditor*)),
                        this, SLOT(onCurrentEditorChanged(GenericCodeEditor*)));

    connect( &mDocModifiedSigMap, SIGNAL(mapped(QObject*)), this, SLOT(onDocModified(QObject*)) );

    connect( main, SIGNAL(applySettingsRequest(Settings::Manager*)),
             this, SLOT(applySettings(Settings::Manager*)) );

    createActions();

    setCurrentBox( defaultBox ); // will updateActions();

    applySettings( main->settings() );
}

void MultiEditor::makeSignalConnections()
{
    DocumentManager *docManager = Main::documentManager();

    connect(docManager, SIGNAL(opened(Document*, int, int)),
            this, SLOT(onOpen(Document*, int, int)));
    connect(docManager, SIGNAL(closed(Document*)),
            this, SLOT(onClose(Document*)));
    connect(docManager, SIGNAL(saved(Document*)),
            this, SLOT(update(Document*)));
    connect(docManager, SIGNAL(showRequest(Document*, int, int)),
            this, SLOT(show(Document*, int, int)));
    connect(docManager, SIGNAL(titleChanged(Document*)),
            this, SLOT(update(Document*)) );
}

void MultiEditor::breakSignalConnections()
{
    DocumentManager *docManager = Main::documentManager();
    docManager->disconnect(this);
}

void MultiEditor::createActions()
{
    Settings::Manager *settings = Main::settings();

    QAction * action;
    const QString ideCategory("IDE");
    const QString editorCategory(tr("Text Editor"));
    const QString helpCategory(tr("Help"));

    // Edit

    mActions[Undo] = action = new QAction(
        QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    action->setShortcut(tr("Ctrl+Z", "Undo"));
    action->setStatusTip(tr("Undo last editing action"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(undo()));
    mEditorSigMux->connect(SIGNAL(undoAvailable(bool)), action, SLOT(setEnabled(bool)));
    settings->addAction( action, "editor-undo", editorCategory);

    mActions[Redo] = action = new QAction(
        QIcon::fromTheme("edit-redo"), tr("Re&do"), this);
    action->setShortcut(tr("Ctrl+Shift+Z", "Redo"));
    action->setStatusTip(tr("Redo next editing action"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(redo()));
    mEditorSigMux->connect(SIGNAL(redoAvailable(bool)), action, SLOT(setEnabled(bool)));
    settings->addAction( action, "editor-redo", editorCategory);

    mActions[Cut] = action = new QAction(
        QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
    action->setShortcut(tr("Ctrl+X", "Cut"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Cut text to clipboard"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(cut()));
    mEditorSigMux->connect(SIGNAL(copyAvailable(bool)), action, SLOT(setEnabled(bool)));
    settings->addAction( action, "editor-cut", editorCategory);

    mActions[Copy] = action = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    action->setShortcut(tr("Ctrl+C", "Copy"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Copy text to clipboard"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(copy()));
    mEditorSigMux->connect(SIGNAL(copyAvailable(bool)), action, SLOT(setEnabled(bool)));
    settings->addAction( action, "editor-copy", editorCategory);

    mActions[Paste] = action = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    action->setShortcut(tr("Ctrl+V", "Paste"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Paste text from clipboard"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(paste()));
    settings->addAction( action, "editor-paste", editorCategory);

    mActions[IndentLineOrRegion] = action = new QAction(
        QIcon::fromTheme("format-indent-line"), tr("Autoindent Line or Region"), this);
    action->setStatusTip(tr("Autoindent Line or Region"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(indent()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-indent-auto", editorCategory);

    mActions[TriggerAutoCompletion] = action = new QAction(tr("Trigger Autocompletion"), this);
    action->setStatusTip(tr("Suggest possible completions of text at cursor"));
    action->setShortcut(tr("Ctrl+Space", "Trigger Autocompletion"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(triggerAutoCompletion()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-autocompletion", editorCategory);

    mActions[TriggerMethodCallAid] = action = new QAction(tr("Trigger Method Call Aid"), this);
    action->setStatusTip(tr("Show arguments for currently typed method call"));
    action->setShortcut(tr("Ctrl+Shift+Space", "Trigger Method Call Aid"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(triggerMethodCallAid()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-method-call-assist", editorCategory);

    mActions[ToggleComment] = action = new QAction(
        QIcon::fromTheme("edit-comment"), tr("Toggle &Comment"), this);
    action->setShortcut(tr("Ctrl+/", "Toggle Comment"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Toggle Comment"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(toggleComment()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-toggle-comment", editorCategory);

    mActions[ToggleOverwriteMode] = action = new QAction(
        QIcon::fromTheme("edit-overwrite"), tr("Toggle &Overwrite Mode"), this);
    action->setShortcut(tr("Insert", "Toggle Overwrite Mode"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(toggleOverwriteMode()));
    settings->addAction( action, "editor-toggle-overwrite", editorCategory);

    mActions[CopyLineUp] = action = new QAction(
        QIcon::fromTheme("edit-copylineup"), tr("Copy Line Up"), this);
    action->setShortcut(tr("Ctrl+Alt+Up", "Copy Line Up"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(copyLineUp()));
    settings->addAction( action, "editor-copy-line-up", editorCategory);

    mActions[CopyLineDown] = action = new QAction(
        QIcon::fromTheme("edit-copylinedown"), tr("Copy Line Down"), this);
    action->setShortcut(tr("Ctrl+Alt+Down", "Copy Line Up"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(copyLineDown()));
    settings->addAction( action, "editor-copy-line-down", editorCategory);

    mActions[MoveLineUp] = action = new QAction(
        QIcon::fromTheme("edit-movelineup"), tr("Move Line Up"), this);
    action->setShortcut(tr("Ctrl+Shift+Up", "Move Line Up"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(moveLineUp()));
    settings->addAction( action, "editor-move-line-up", editorCategory);

    mActions[MoveLineDown] = action = new QAction(
        QIcon::fromTheme("edit-movelinedown"), tr("Move Line Down"), this);
    action->setShortcut(tr("Ctrl+Shift+Down", "Move Line Up"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(moveLineDown()));
    settings->addAction( action, "editor-move-line-down", editorCategory);

    mActions[GotoPreviousBlock] = action = new QAction(
        QIcon::fromTheme("edit-gotopreviousblock"), tr("Go to Previous Block"), this);
    action->setShortcut(tr("Ctrl+[", "Go to Previous Block"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(gotoPreviousBlock()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-go-to-prev-block", editorCategory);

    mActions[GotoNextBlock] = action = new QAction(
        QIcon::fromTheme("edit-gotonextblock"), tr("Go to Next Block"), this);
    action->setShortcut(tr("Ctrl+]", "Go to Next Block"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(gotoNextBlock()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-go-to-next-block", editorCategory);

    mActions[SelectEnclosingBlock] = action = new QAction(tr("Select Enclosing Block"), this);
    action->setShortcut(tr("Ctrl+Shift+B", "Select Enclosing Block"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Select everything between brackets that contain cursor"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(selectBlockAroundCursor()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-select-enclosing-block", editorCategory);

    mActions[GotoPreviousRegion] = action = new QAction(
        QIcon::fromTheme("edit-gotopreviousregion"), tr("Go to Previous Region"), this);
    action->setShortcut(tr("Ctrl+Alt+[", "Go to Previous Region"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(gotoPreviousRegion()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-go-to-prev-region", editorCategory);

    mActions[GotoNextRegion] = action = new QAction(
        QIcon::fromTheme("edit-gotonextregion"), tr("Go to Next Region"), this);
    action->setShortcut(tr("Ctrl+Alt+]", "Go to Next Region"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(gotoNextRegion()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-go-to-next-region", editorCategory);

    mActions[GotoPreviousEmptyLine] = action = new QAction( tr("Go to Previous Empty Line"), this);
    action->setShortcut(tr("Ctrl+Up", "Go to Previous Empty Line"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(gotoPreviousEmptyLine()));
    settings->addAction( action, "editor-go-to-prev-empty-line", editorCategory);

    mActions[GotoNextEmptyLine] = action = new QAction( tr("Go to Next Empty Line"), this);
    action->setShortcut(tr("Ctrl+Down", "Go to Next Empty Line"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(gotoNextEmptyLine()));
    settings->addAction( action, "editor-go-to-next-empty-line", editorCategory);

    mActions[SelectRegion] = action = new QAction( tr("Select Region"), this);
    action->setShortcut(tr("Ctrl+Shift+R", "Select Region"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(selectCurrentRegion()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-select-region", editorCategory);

    // View

    mActions[EnlargeFont] = action = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    action->setShortcut(tr("Ctrl++", "Enlarge font"));
    action->setStatusTip(tr("Increase displayed font size"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(zoomIn()));
    settings->addAction( action, "editor-enlarge-font", editorCategory);

    mActions[ShrinkFont] = action = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    action->setShortcut( tr("Ctrl+-", "Shrink font"));
    action->setStatusTip(tr("Decrease displayed font size"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(zoomOut()));
    settings->addAction( action, "editor-shrink-font", editorCategory);

    mActions[ResetFontSize] = action = new QAction(
        QIcon::fromTheme("zoom-reset"), tr("&Reset Font Size"), this);
    action->setShortcut( tr("Ctrl+0", "Reset font"));
    action->setStatusTip(tr("Reset displayed font size"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(resetFontSize()));
    settings->addAction( action, "editor-reset-font-size", editorCategory);

    mActions[ShowWhitespace] = action = new QAction(tr("Show Spaces and Docs"), this);
    action->setCheckable(true);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setShowWhitespace(bool)));
    settings->addAction( action, "editor-toggle-show-whitespace", editorCategory);

    mActions[IndentWithSpaces] = action = new QAction(tr("Use Spaces for Indentation"), this);
    action->setCheckable(true);
    action->setStatusTip( tr("Indent with spaces instead of docs") );
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered(bool)), SLOT(setSpaceIndent(bool)),
                           SignalMultiplexer::ConnectionOptional );
    settings->addAction( action, "editor-toggle-space-indent", editorCategory );

    mActions[NextDocument] = action = new QAction(tr("Next Document"), this);
#ifndef Q_OS_MAC
    action->setShortcut( tr("Alt+Right", "Next Document"));
#else
    action->setShortcut( tr("Ctrl+Right", "Next Document"));
#endif
    connect(action, SIGNAL(triggered()), this, SLOT(showNextDocument()));
    settings->addAction( action, "editor-document-next", editorCategory);

    mActions[PreviousDocument] = action = new QAction(tr("Previous Document"), this);
#ifndef Q_OS_MAC
    action->setShortcut( tr("Alt+Left", "Previous Document"));
#else
    action->setShortcut( tr("Ctrl+Left", "Previous Document"));
#endif
    connect(action, SIGNAL(triggered()), this, SLOT(showPreviousDocument()));
    settings->addAction( action, "editor-document-previous", editorCategory);

    mActions[SwitchDocument] = action = new QAction(tr("Switch Document"), this);
#ifndef Q_OS_MAC
    action->setShortcut( tr("Ctrl+Tab", "Switch Document"));
#else
    action->setShortcut( tr("Alt+Tab", "Switch Document"));
#endif
    connect(action, SIGNAL(triggered()), this, SLOT(switchDocument()));
    settings->addAction( action, "editor-document-switch", editorCategory);

    mActions[SwitchSplit] = action = new QAction(tr("&Switch Editor"), this);
    action->setShortcut( tr("Ctrl+Alt+E", "Switch Editor"));
    action->setStatusTip(tr("Show editors popup menu"));
    connect(action, SIGNAL(triggered()), this, SLOT(switchSplit()));
    settings->addAction( action, "editor-split-switch", editorCategory);

    mActions[SplitHorizontally] = action = new QAction(tr("Split To Right"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(splitHorizontally()));
    settings->addAction( action, "editor-split-right", editorCategory);

    mActions[SplitVertically] = action = new QAction(tr("Split To Bottom"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(splitVertically()));
    settings->addAction( action, "editor-split-bottom", editorCategory);

    mActions[SplitNewWindow] = action = new QAction(tr("Split To New Window"), this);
    action->setStatusTip(tr("Open split in a new window"));
    action->setShortcut( tr("Ctrl+Alt+N", "Split To New Window"));
    connect(action, SIGNAL(triggered()), this, SLOT(splitNewWindow()));
    settings->addAction( action, "editor-split-new", editorCategory);

    mActions[RemoveCurrentSplit] = action = new QAction(tr("Remove Current Split"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(removeCurrentSplit()));
    settings->addAction( action, "editor-split-remove", editorCategory);

    mActions[RemoveCurrentWindow] = action = new QAction(tr("&Close Window"), this);
    action->setStatusTip(tr("Close the active window"));
    action->setShortcut( tr("Ctrl+Alt+W", "Close Window"));
    connect(action, SIGNAL(triggered()), this, SLOT(removeWindow()));
    settings->addAction( action, "window-remove", editorCategory);

    mActions[RemoveAllSplits] = action = new QAction(tr("Remove All Splits"), this);
    connect(action, SIGNAL(triggered()), this, SLOT(removeAllSplits()));
    settings->addAction( action, "editor-split-remove-all", editorCategory);

    // Language

    mActions[EvaluateCurrentDocument] = action = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("Evaluate &File"), this);
    action->setStatusTip(tr("Evaluate current File"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(evaluateDocument()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-eval-file", editorCategory);

    mActions[EvaluateRegion] = action = new QAction(
    QIcon::fromTheme("media-playback-start"), tr("&Evaluate Selection, Line or Region"), this);
    action->setShortcut(tr("Ctrl+Return", "Evaluate region"));
    action->setStatusTip(tr("Evaluate current region"));
    //action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(evaluateRegion()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-eval-smart", editorCategory);

    mActions[EvaluateLine] = action = new QAction(
    QIcon::fromTheme("media-playback-start"), tr("&Evaluate Selection or Line"), this);
    action->setShortcut(tr("Shift+Return", "Evaluate selection/line"));
    action->setStatusTip(tr("Evaluate current selection/line"));
    //action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(evaluateLine()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-eval-line", editorCategory);

    //  Reimplementation of main_window actions:
    //  Actions have been moved to this class to allow shortcuts to work on the MainWindow when other windows are opened.

    // File
    mActions[Quit] = action = new QAction(
        QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
    action->setShortcut(tr("Ctrl+Q", "Quit application"));
    action->setStatusTip(tr("Quit SuperCollider IDE"));
    QObject::connect( action, SIGNAL(triggered()), MainWindow::instance(), SLOT(onQuit()) );
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
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(closeDocument()));
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

    //edit

    mActions[Find] = action = new QAction(
        QIcon::fromTheme("edit-find"), tr("&Find..."), this);
    action->setShortcut(tr("Ctrl+F", "Find"));
    action->setStatusTip(tr("Find text in document"));
    connect(action, SIGNAL(triggered()), this, SLOT(showFindTool()));
    settings->addAction( action, "editor-find", editorCategory);

    action = mActions[FindNext] = new QAction(tr("Find Next"), this);
    action->setShortcut(tr("Ctrl+G", "Find Next"));
    connect( action, SIGNAL(triggered()), this, SLOT(findNext()) );
    settings->addAction( action, "editor-find-next", tr("Text Editor") );

    action = mActions[FindPrevious] = new QAction(tr("Find Previous"), this);
    action->setShortcut(tr("Ctrl+Shift+G", "Find Previous"));
    connect( action, SIGNAL(triggered()), this, SLOT(findPrevious()) );
    settings->addAction( action, "editor-find-previous", tr("Text Editor") );

    mActions[Replace] = action = new QAction(
        QIcon::fromTheme("edit-replace"), tr("&Replace..."), this);
    action->setShortcut(tr("Ctrl+R", "Replace"));
    action->setStatusTip(tr("Find and replace text in document"));
    connect(action, SIGNAL(triggered()), this, SLOT(showReplaceTool()));
    settings->addAction( action, "editor-replace", editorCategory);

    //View

    mActions[ShowCmdLine] = action = new QAction(tr("&Command Line"), this);
    action->setStatusTip(tr("Command line for quick code evaluation"));
    action->setShortcut(tr("Ctrl+E", "Show command line"));
    connect(action, SIGNAL(triggered()), this, SLOT(showCmdLine()));
    settings->addAction( action, "ide-command-line-show", ideCategory);

    mActions[CmdLineForCursor] = action = new QAction(tr("&Command Line from selection"), this);
    action->setShortcut(tr("Ctrl+Shift+E", "Fill command line with current selection"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(cmdLineForCursor()));
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
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(toggleFullScreen()));
    settings->addAction( action, "ide-show-fullscreen", ideCategory);

    mActions[FocusPostWindow] = action = new QAction( tr("Focus Post Window"), this);
    action->setStatusTip(tr("Focus post window"));
    action->setShortcut(tr("Ctrl+Alt+P", "Focus post window"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(postFocus()));
    settings->addAction( action, "post-focus", ideCategory);

    // Language

    mActions[LookupImplementation] = action = new QAction(
        QIcon::fromTheme("window-lookupdefinition"), tr("Look Up Implementations..."), this);
    action->setShortcut(tr("Ctrl+Shift+I", "Look Up Implementations"));
    action->setStatusTip(tr("Open dialog to look up implementations of a class or a method"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(lookupImplementation()));
    settings->addAction( action, "ide-lookup-implementation", ideCategory);

    mActions[LookupImplementationForCursor] = action = new QAction(tr("Look Up Implementations for Cursor"), this);
    action->setShortcut(tr("Ctrl+I", "Look Up Implementations for Cursor"));
    action->setStatusTip(tr("Look up implementations of class or method under cursor"));
    connect(action, SIGNAL(triggered(bool)), MainWindow::instance(), SLOT(lookupImplementationForCursor()));
    settings->addAction( action, "ide-lookup-implementation-for-cursor", ideCategory);

    mActions[LookupReferences] = action = new QAction(
        QIcon::fromTheme("window-lookupreferences"), tr("Look Up References..."), this);
    action->setShortcut(tr("Ctrl+Shift+U", "Look Up References"));
    action->setStatusTip(tr("Open dialog to look up references to a class or a method"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(lookupReferences()));
    settings->addAction( action, "ide-lookup-references", ideCategory);

    mActions[LookupReferencesForCursor] = action = new QAction(tr("Look Up References for Cursor"), this);
    action->setShortcut(tr("Ctrl+U", "Look Up References For Selection"));
    action->setStatusTip(tr("Look up references to class or method under cursor"));
    connect(action, SIGNAL(triggered(bool)), MainWindow::instance(), SLOT(lookupReferencesForCursor()));
    settings->addAction( action, "ide-lookup-references-for-cursor", ideCategory);

    // Settings

    mActions[ShowSettings] = action = new QAction(tr("&Preferences"), this);
#ifdef Q_OS_MAC
    action->setShortcut(tr("Ctrl+,", "Show configuration dialog"));
#endif
    action->setStatusTip(tr("Show configuration dialog"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(showSettings()));
    settings->addAction( action, "ide-settings-dialog", ideCategory);

    // Help

    mActions[Help] = action = new QAction(tr("Show &Help Browser"), this);
    action->setStatusTip(tr("Show and focus the Help Browser"));
    action->setShortcut(tr("Ctrl+Alt+D", "Focus post window"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(openHelp()));
    settings->addAction( action, "help-browser", helpCategory);

    mActions[LookupDocumentationForCursor] = action =
            new QAction(tr("Look Up Documentation for Cursor"), this);
    action->setShortcut(tr("Ctrl+D", "Look Up Documentation for Cursor"));
    action->setStatusTip(tr("Look up documentation for text under cursor"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(lookupDocumentationForCursor()));
    settings->addAction( action, "help-lookup-for-cursor", helpCategory);

    mActions[LookupDocumentation] = action =
            new QAction(tr("Look Up Documentation..."), this);
    action->setShortcut(tr("Ctrl+Shift+D", "Look Up Documentation"));
    action->setStatusTip(tr("Enter text to look up in documentation"));
    connect(action, SIGNAL(triggered()), MainWindow::instance(), SLOT(lookupDocumentation()));
    settings->addAction( action, "help-lookup", helpCategory);

    // These actions are not added to any menu, so they have to be added
    // at least to this widget, in order for the shortcuts to always respond:
    addAction(mActions[TriggerAutoCompletion]);
    addAction(mActions[TriggerMethodCallAid]);
    addAction(mActions[SwitchDocument]);

    // These actions have to be added because to the widget because they have
    // Qt::WidgetWithChildrenShortcut context:
    addAction(mActions[Cut]);
    addAction(mActions[Copy]);
    addAction(mActions[Paste]);
    addAction(mActions[ShowWhitespace]);
    addAction(mActions[IndentWithSpaces]);
    addAction(mActions[EvaluateCurrentDocument]);
    addAction(mActions[EvaluateRegion]);
    addAction(mActions[EvaluateLine]);
    addAction(mActions[ToggleComment]);
    addAction(mActions[ToggleOverwriteMode]);
    addAction(mActions[CopyLineUp]);
    addAction(mActions[CopyLineDown]);
    addAction(mActions[MoveLineUp]);
    addAction(mActions[MoveLineDown]);
    addAction(mActions[GotoPreviousBlock]);
    addAction(mActions[GotoNextBlock]);
    addAction(mActions[SelectEnclosingBlock]);
    addAction(mActions[GotoPreviousRegion]);
    addAction(mActions[GotoNextRegion]);
    addAction(mActions[GotoPreviousEmptyLine]);
    addAction(mActions[GotoNextEmptyLine]);
    addAction(mActions[SelectRegion]);
    addAction(mActions[EnlargeFont]);
    addAction(mActions[ShrinkFont]);
    addAction(mActions[ResetFontSize]);

    addAction(mActions[Quit]);
    addAction(mActions[DocNew]);
    addAction(mActions[DocOpen]);
    addAction(mActions[DocOpenStartup]);
    addAction(mActions[DocOpenSupportDir]);
    addAction(mActions[DocSave]);
    addAction(mActions[DocSaveAs]);
    addAction(mActions[DocSaveAll]);
    addAction(mActions[DocClose]);
    addAction(mActions[DocCloseAll]);
    addAction(mActions[DocReload]);
    addAction(mActions[NewSession]);
    addAction(mActions[SaveSessionAs]);
    addAction(mActions[ManageSessions]);
    addAction(mActions[OpenSessionSwitchDialog]);
    addAction(mActions[Find]);
    addAction(mActions[FindNext]);
    addAction(mActions[FindPrevious]);
    addAction(mActions[Replace]);
    addAction(mActions[ShowCmdLine]);
    addAction(mActions[CmdLineForCursor]);
    addAction(mActions[ShowGoToLineTool]);
    addAction(mActions[CloseToolBox]);
    addAction(mActions[ShowFullScreen]);
    addAction(mActions[FocusPostWindow]);
    addAction(mActions[LookupImplementation]);
    addAction(mActions[LookupImplementationForCursor]);
    addAction(mActions[LookupReferences]);
    addAction(mActions[LookupReferencesForCursor]);
    addAction(mActions[ShowSettings]);
    addAction(mActions[Help]);
    addAction(mActions[LookupDocumentationForCursor]);
    addAction(mActions[LookupDocumentation]);
    addAction(mActions[SplitNewWindow]);
    addAction(mActions[SwitchSplit]);
}

void MultiEditor::updateActions()
{
    GenericCodeEditor *editor = currentEditor();
    ScCodeEditor *scEditor = qobject_cast<ScCodeEditor*>(editor);
    QTextDocument *doc = editor ? editor->textDocument() : 0;

    mActions[Undo]->setEnabled( doc && doc->isUndoAvailable() );
    mActions[Redo]->setEnabled( doc && doc->isRedoAvailable() );
    mActions[Copy]->setEnabled( editor && editor->textCursor().hasSelection() );
    mActions[Cut]->setEnabled( mActions[Copy]->isEnabled() );
    mActions[Paste]->setEnabled( editor );
    mActions[ToggleOverwriteMode]->setEnabled( editor );
    mActions[CopyLineUp]->setEnabled( editor );
    mActions[CopyLineDown]->setEnabled( editor );
    mActions[MoveLineUp]->setEnabled( editor );
    mActions[MoveLineDown]->setEnabled( editor );
    mActions[GotoPreviousEmptyLine]->setEnabled( editor );
    mActions[GotoNextEmptyLine]->setEnabled( editor );
    mActions[DocClose]->setEnabled( editor );
    mActions[DocNew]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
    mActions[ResetFontSize]->setEnabled( editor );
    mActions[IndentWithSpaces]->setEnabled( scEditor );
    mActions[IndentWithSpaces]->setChecked( scEditor && scEditor->spaceIndent() );

    // ScLang-specific actions
    bool editorIsScCodeEditor = qobject_cast<ScCodeEditor*>(editor); // NOOP at the moment, but
    mActions[ToggleComment]->setEnabled( editor && editorIsScCodeEditor );
    mActions[GotoPreviousBlock]->setEnabled( editor && editorIsScCodeEditor );
    mActions[GotoNextBlock]->setEnabled( editor && editorIsScCodeEditor );
    mActions[SelectEnclosingBlock]->setEnabled( editor && editorIsScCodeEditor );
    mActions[GotoPreviousRegion]->setEnabled( editor && editorIsScCodeEditor );
    mActions[GotoNextRegion]->setEnabled( editor && editorIsScCodeEditor );
    mActions[SelectRegion]->setEnabled( editor && editorIsScCodeEditor );
    mActions[IndentLineOrRegion]->setEnabled( editor && editorIsScCodeEditor );

    mActions[EvaluateCurrentDocument]->setEnabled( editor && editorIsScCodeEditor );
    mActions[EvaluateRegion]->setEnabled( editor && editorIsScCodeEditor );
    mActions[EvaluateLine]->setEnabled( editor && editorIsScCodeEditor );
}

void MultiEditor::applySettings( Settings::Manager * settings )
{
    bool show_whitespace = settings->value("IDE/editor/showWhitespace").toBool();
    mActions[ShowWhitespace]->setChecked( show_whitespace );
}

static QVariantList saveBoxState( CodeEditorBox *box, const QList<Document*> & documentList )
{
    // save editors in reverse order - first one is last shown.
    QVariantList boxData;
    int idx = box->history().count();
    while(idx--) {
        GenericCodeEditor *editor = box->history()[idx];
        int documentIndex = documentList.indexOf( editor->document() );
        if (!editor->document()->filePath().isEmpty()) {
            // if the session was new or currently restored, save current documents.
            Q_ASSERT(documentIndex >= 0);
            QVariantMap editorData;
            editorData.insert("documentIndex", documentIndex);
            editorData.insert("position", editor->textCursor().position());
            boxData.append( editorData );
        }
        else if (MainWindow::instance()->editor()->tempModeIsOn() && editor->document()->filePath().isEmpty() && documentIndex<MainWindow::instance()->editor()->originalDocumentList().count()) {
            // if some default documents are replacing missing files, save their index to mantain the right association with the split index.
            if ( (!(MainWindow::instance()->editor()->originalDocumentList().at(documentIndex).toString().isEmpty()))) {
                Q_ASSERT(documentIndex >= 0);
                QVariantMap editorData;
                editorData.insert("documentIndex", documentIndex);
                editorData.insert("position", editor->textCursor().position());
                boxData.append( editorData );
            }
        }
    }
    return boxData;
}

static QVariantMap saveSplitterState( QSplitter *splitter, const QList<Document*> & documentList )
{
    QVariantMap splitterData;

    splitterData.insert( "state", splitter->saveState().toBase64() );

    QVariantList childrenData;

    int childCount = splitter->count();
    for (int idx = 0; idx < childCount; idx++) {
        QWidget *child = splitter->widget(idx);

        CodeEditorBox *box = qobject_cast<CodeEditorBox*>(child);
        if (box) {
            QVariantList boxData = saveBoxState(box, documentList);
            childrenData.append( QVariant(boxData) );
            continue;           
        }

        QSplitter *childSplitter = qobject_cast<QSplitter*>(child);
        if (childSplitter) {
            QVariantMap childSplitterData = saveSplitterState(childSplitter, documentList);
            childrenData.append( QVariant(childSplitterData) );
        }
    }

    splitterData.insert( "elements", childrenData );

    return splitterData;
}

void MultiEditor::saveSession( Session *session )
{
    QList<Document*> documentList;

    QVariantList docsData;
    int docCount = mDocs.count();
    if (tempMode) {
        QString string = QString("Session saved in temporary-mode: untitled documents replaced with original paths. \n");
            Main::scProcess()->post(string);
    }
    for (int docIdx = 0; docIdx < docCount; ++docIdx) {
        Document *doc = documentForIndex(docIdx);
        // if the session has opened in temporary-mode
        if (tempMode) {
            // every new document is appended. So the docCount is bigger than originalDocumentsList: this statement makes sure not to crash the program.
            if (docIdx+1 <= originalDocumentsList.count()) {
                // there are 2 cases: the doc path is available or it isn't.
                if ( doc->filePath().isEmpty() ) {
                    // if the doc path is not available, save the original path.
                    docsData << originalDocumentsList.at(docIdx).toString();
                    documentList << doc;

                    if (originalDocumentsList.at(docIdx).toString().isEmpty())
                        return;
                    else {
                        QString string = QString("File path still missing: " + originalDocumentsList.at(docIdx).toString() + " \n");
                            Main::scProcess()->post(string);
                    }
                }
                if ( !(doc->filePath().isEmpty()) ) {
                    // if the doc path is available, save the path.
                    docsData << doc->filePath();
                    documentList << doc;
                }
            }
            else {
                if (doc->filePath().isEmpty()) {
                    documentList << doc;
                }
                else {
                    // for each new document save the filePath.
                    docsData << doc->filePath();
                    documentList << doc;

                    QString string = QString("New document added: " + doc->filePath() + " \n");
                        Main::scProcess()->post(string);
                }
            }
        }
        else {
            // any new document with no path is discharged
            if (doc->filePath().isEmpty()) {
                //docsData << doc->filePath();
                documentList << doc;
            }
            else {
                // the session had been opened with all the right doc paths. It works normally.
                docsData << doc->filePath();
                documentList << doc;
            }
        }
    }

    session->setValue( "documents", QVariant::fromValue(docsData) );

    session->remove( "editors" );

    QVariantList splitterState;
    foreach(MultiSplitter *editor, editorList) {
        // If the window has got one only editor and the document is untitled, discharge it.
        // If the window has more than one split (editor), save it. Particularly, save the Splitter if:
        if ( editor==editorList.at(0) /* it is the main Editor */
             || editor->splits().count() > 1  /* there is more than 1 spit in the window */
             || !editor->splits().at(0)->currentEditor()->document()->filePath().isEmpty() /* the document is saved */
             || indexForDocument(editor->splits().at(0)->currentEditor()->document())<originalDocumentList().count() /* the document is protected */ )
            splitterState << saveSplitterState(editor, documentList);
    }

    session->setValue( "editors", QVariant::fromValue(splitterState) );
}

void MultiEditor::loadBoxState( CodeEditorBox *box,
                                const QVariantList & data, const QList<Document*> & documentList )
{
    int docCount = documentList.count();
     // ensure working if the session has saved with one only untitled document
    if (docCount == 1 && documentForIndex(0)->filePath().isEmpty() ) {
        box->setDocument( documentList[0]);
    }
    else {
        foreach( QVariant docVar, data )
        {
            QVariantMap docData = docVar.value<QVariantMap>();
            int docIndex = docData.value("documentIndex").toInt();
            if (docIndex >= 0 && docIndex < docCount) {
                if (!(documentList[docIndex]->filePath().isEmpty())) {
                    int docPos = docData.value("position").toInt();
                    box->setDocument( documentList[docIndex], docPos );
                }
                // ensure working if the filePath is broken or lost or unsaved
                else {
                    box->setDocument( documentList[0] );
                }
            }
            // ensure working if there are more indexes than documents
            else {
                box->setDocument( documentList[0] );
            }
        }
    }
}

void MultiEditor::loadSplitterState( MultiSplitter *splitter,
                                     const QVariantMap & data, const QList<Document*> & documentList, MultiSplitter *splitterContainer )
{
    QByteArray state = QByteArray::fromBase64( data.value("state").value<QByteArray>() );

    QVariantList childrenData = data.value("elements").value<QVariantList>();
    foreach (const QVariant & childVar, childrenData) {
        if (childVar.type() == QVariant::List) {
            CodeEditorBox *childBox = newBox(splitterContainer);
            splitter->addWidget(childBox);
            QVariantList childBoxData = childVar.value<QVariantList>();
            loadBoxState( childBox, childBoxData, documentList );
        }
        else if (childVar.type() == QVariant::Map) {
            MultiSplitter *childSplitter = new MultiSplitter(splitter->returnWindow());
            splitter->addWidget(childSplitter);
            QVariantMap childSplitterData = childVar.value<QVariantMap>();
            loadSplitterState( childSplitter, childSplitterData, documentList, splitterContainer );
        }
    }

    if (!splitter->restoreState(state))
        qWarning("MultiEditor: could not restore splitter state!");
}

void MultiEditor::switchSession( Session *session )
{
    ///// Going offline...

    breakSignalConnections();

    DocumentManager *docManager = Main::documentManager();

    QList<Document*> documentList = docManager->documents();

    // close all docs
    foreach (Document *doc, documentList)
        docManager->close(doc);

    // remove all editors
    foreach (MultiSplitter *splitter, editorList) {
        if (splitter->returnWindow()) {
            delete splitter->returnWindow();
        } else {
            delete splitter;
        }
    }

    documentList.clear();
    editorList.clear();
    mDocs.clear();
    tempMode = false;
    originalDocumentsList.clear();
    sessionChanged = false;
    mCurrentEditorBox = 0;

    mSplitter = new MultiSplitter(0);
    editorList << mSplitter;

    defaultBox = 0;
    mEditorHistory.clear();

    if (session)
    {
        // open documents saved in the session
        QVariantList docDataList = session->value("documents").value<QVariantList>();

        if (docDataList.isEmpty()) {
            // If, for some reason, the session has been saved without document
            // Ensure working when a session has been saved without valid files
            Document * doc = Main::documentManager()->docReplace();
            documentList << doc;

            QString string = QString("No files have been saved in the last session! New Session! \n");
            Main::scProcess()->post(string);
        }
        else {
            foreach( const QVariant & docData, docDataList ) {

                QString filePath = docData.toString();
                // Ensure working if, for some reason, a file path has not been
                if (filePath.isEmpty()) {
                    originalDocumentsList << filePath;
                    Document * doc = Main::documentManager()->docReplace();
                    documentList << doc;
                }
                else {
                    Document * doc = docManager->open(filePath, -1, 0, false);
                    // If the file is available, open it.
                    if (doc) {
                        originalDocumentsList << filePath;
                        documentList << doc;
                    }
                    else {
                        // if a file is not found, warn the user about the prob
                        // 3 choices: replace the missing file with a new one (
                        // forget the file and delete it; ignore the warning. I
                        // make a copy of all the path, and replace the missing
                        // These blank documents will be protected and the orig

                        QString warnMissingFile = QString("file missing: " + filePath + "; ");
                        Main::scProcess()->post(warnMissingFile);

                        QMessageBox missingFile( QMessageBox::Warning,
                            tr("SuperCollider IDE"),
                            tr("The document '%1' has not been found. Do you want to replace it? \n\n"
                               "If you discharge it, the document will be removed. \n"
                               "If you ignore it, the document will be temporarily replaced by a blank window. "
                               "(Supercollider will try to re-open the file next time.)").arg(filePath),
                            QMessageBox::Open | QMessageBox::Close | QMessageBox::Cancel,
                            this
                        );

                        missingFile.setButtonText(QMessageBox::Open, "Replace");
                        missingFile.setButtonText(QMessageBox::Close, "Discharge");
                        missingFile.setButtonText(QMessageBox::Cancel, "Ignore");

                        switch (missingFile.exec()) {
                            case QMessageBox::Open:
                            {
                                QFileDialog dialog (this, Qt::Dialog);
                                dialog.setModal(true);
                                dialog.setWindowModality(Qt::ApplicationModal);

                                dialog.setFileMode( QFileDialog::ExistingFile );

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
                                        QStringList filename = dialog.selectedFiles();
                                        originalDocumentsList << filename[0];
                                        Document * doc = docManager->open(filename[0], -1, 0, false);
                                        documentList << doc;

                                        QString replaceMissingFile = QString( " replaced with: " + filename[0] + " \n");
                                        Main::scProcess()->post(replaceMissingFile);
                                    }

                                    // FIXME: workaround for Qt bug 25295
                                    // See SC issue #678
                                #ifdef Q_OS_MAC
                                    if (last_active_window)
                                        last_active_window->activateWindow();
                                #endif

                                sessionChanged = true;

                                break;
                            }
                            case QMessageBox::Cancel:
                            {
                                tempMode = true;
                                originalDocumentsList << filePath;
                                Document * doc = Main::documentManager()->docReplace();
                                documentList << doc;

                                QString replaceMissingFile = QString(" replaced with a protected blank window. \n");
                                Main::scProcess()->post(replaceMissingFile);

                                break;
                            }
                            case QMessageBox::Close:
                                sessionChanged = true;
                                QString deleteMissingFile = QString(" deleted. \n");
                                Main::scProcess()->post(deleteMissingFile);
                                break;
                            //default:;
                        }
                    }
                }
            }
        }

        if (tempMode) {
            QString warning = QString("ide: Session running in temporary-mode:  some file paths have not been found. Please replace files or re-save session. \n");
                Main::scProcess()->post(warning);
        }

        // restore docs
        foreach ( Document * doc, documentList )
            addDoc(doc);

        // restore editors
        if (session->contains("editors")) {
            // restore session of previous versions of Supercollider
            QVariant variantType = session->value("editors");
            if (variantType.type() == QVariant::Map) {
                QVariantMap splitterData = variantType.value<QVariantMap>();
                loadSplitterState( mSplitter, splitterData, documentList, mSplitter );

                if (mSplitter->count()) {
                    defaultBox = mSplitter->findChild<CodeEditorBox>();
                    if (!defaultBox) {
                        qWarning("Session seems to contain invalid editor split data!");
                        delete mSplitter;
                        editorList.clear();
                        mSplitter = new MultiSplitter(0);
                        editorList << mSplitter;
                    }
                }
            }
            if (variantType.type() == QVariant::List) {
                QVariantList splitterStateList = session->value("editors").value<QVariantList>();

                QVariant splitterState;
                for (int indexSplit = 0; indexSplit < splitterStateList.size(); indexSplit++)
                {
                    splitterState = splitterStateList.at(indexSplit);
                    QVariantMap splitterData = splitterState.toMap();
                    if (indexSplit == 0) {
                        loadSplitterState( mSplitter, splitterData, documentList, mSplitter );

                        if (mSplitter->count()) {
                            defaultBox = mSplitter->splits().first();
                            if (!defaultBox) {
                                qWarning("Session seems to contain invalid editor split data!");
                                delete mSplitter;
                                editorList.clear();
                                mSplitter = new MultiSplitter(0);
                                editorList << mSplitter;
                            }
                        }
                    }
                    else {
                        EditorWindow *window = makeNewWindow();

                        MultiSplitter *newSplitter = new MultiSplitter(window);
                        editorList << newSplitter;

                        loadSplitterState( newSplitter, splitterData, documentList, newSplitter );

                        if (newSplitter->count()) {
                            CodeEditorBox *newBox = newSplitter->findChild<CodeEditorBox>();
                            if (!newBox) {
                                qWarning("Session seems to contain invalid editor split data!");
                                delete newSplitter;
                                newSplitter = new MultiSplitter(window);
                            }
                        }

                        QVBoxLayout *l = setNewLayout(newSplitter);

                        window->setLayout(l);
                    }
                }
            }
        }
    }

    if (!defaultBox) {
        // Restoring the session didn't result in any editor box, so create one:
        defaultBox = newBox(mSplitter);
        mSplitter->addWidget( defaultBox );

        if (!mDocs.isEmpty())
            defaultBox->setDocument(mDocs.at(0));

        QString string = QString("New session \n");
        Main::scProcess()->post(string);
    }

    layout()->addWidget(mSplitter);
    layout()->addWidget(mSplitter->toolBox());

    connect(mSplitter->toolBox()->closeButton(), SIGNAL(clicked()), this, SLOT(hideToolBox()));

    makeSignalConnections();

    ///// Back online.

    mCurrentEditorBox = 0; // ensure complete update
    setCurrentBox( defaultBox );

    if (!session) {
        // create a document on new session
        docManager->create();
        docManager->sendActiveDocument();
    }

    defaultBox->setFocus(Qt::OtherFocusReason); // ensure focus

    if (sessionChanged) {
        MainWindow::instance()->saveSessionChanged();
    }
}

int MultiEditor::addDoc( Document * doc )
{
    if (!doc)
        return -1;

    int docIdx = indexForDocument(doc);
    if (docIdx != -1)
        return docIdx;

    QTextDocument *tdoc = doc->textDocument();

    QIcon icon;
    if(tdoc->isModified())
        icon = mDocModifiedIcon;

    mDocs << doc;

    mDocModifiedSigMap.setMapping(tdoc, doc);
    connect( tdoc, SIGNAL(modificationChanged(bool)), &mDocModifiedSigMap, SLOT(map()) );

    return docIdx;
}

void MultiEditor::setCurrent( Document *doc )
{
    CodeEditorBox *curBox = currentBox();
    curBox->setDocument(doc);
    curBox->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::showNextDocument()
{
    CodeEditorBox *curBox = currentBox();
    int currentIndex = mDocs.indexOf(curBox->currentDocument());
    if (currentIndex < mDocs.count()-1) {
        Document *doc = documentForIndex(currentIndex +1);
        if (!doc)
            return;

        curBox->setDocument(doc);
        curBox->setFocus(Qt::OtherFocusReason);
    }
    else {
        return;
    }
}

void MultiEditor::showPreviousDocument()
{
    CodeEditorBox *curBox = currentBox();
    int currentIndex = mDocs.indexOf(curBox->currentDocument());
    if (currentIndex > 0) {
        Document *doc = documentForIndex(currentIndex -1);
        if (!doc)
            return;

        curBox->setDocument(doc);
        curBox->setFocus(Qt::OtherFocusReason);
    }
    else {
        return;
    }
}

void MultiEditor::switchDocument()
{
    currentBox()->comboBox()->showPopup();
}

void MultiEditor::addSplitToHistory(CodeEditorBox *box) {
    mEditorHistory.prepend(box);
}

void MultiEditor::removeSplitFromHistory(CodeEditorBox *box) {
    mEditorHistory.removeOne(box);
}

void MultiEditor::setCurrentSplitInHistory(CodeEditorBox *box) {
    mEditorHistory.removeOne(box);
    mEditorHistory.prepend(box);
}

void MultiEditor::switchSplit()
{
    EditorSelectPopUp * popup = new EditorSelectPopUp(splitHistory(), this);

    QRect popupRect(0,0,300,200);
    popupRect.moveCenter(rect().center());
    popup->resize(popupRect.size());
    QPoint globalPosition = mapToGlobal(popupRect.topLeft());

    CodeEditorBox * selectedSplit = popup->exec(globalPosition);

    if (selectedSplit) {
        setCurrentBox(selectedSplit);
        currentBox()->raise();
        currentBox()->show();
        currentBox()->activateWindow();
    }
}

void MultiEditor::onOpen( Document *doc, int initialCursorPosition, int selectionLength )
{
    addDoc(doc);

    currentBox()->setDocument(doc, initialCursorPosition, selectionLength);
    currentBox()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::onClose( Document *doc )
{
    int docIdx = indexForDocument(doc);
    if (docIdx != -1) {
        mDocs.removeAt(docIdx);
        if (tempMode) {
            originalDocumentsList.removeAt(docIdx);
        }
    }
}

void MultiEditor::onDocModified( QObject *object )
{
    Document *doc = qobject_cast<Document*>(object);
    if (!doc) return;

    int docIdx = indexForDocument(doc);
    if (docIdx == -1) return;

    bool isModified = doc->textDocument()->isModified();
    QIcon icon;
    if(isModified)
        icon = mDocModifiedIcon;

    Main::evaluateCodeIfCompiled(QString("Document.findByQUuid(\'%1\').prSetEdited(%2)").arg(doc->id().constData()).arg(isModified), true);
}

void MultiEditor::show( Document *doc, int pos, int selectionLength )
{
    currentBox()->setDocument(doc, pos, selectionLength);
    currentBox()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::update( Document *doc )
{
    // update thisProcess.nowExecutingPath
    GenericCodeEditor *editor = currentEditor();
    if (editor->document() == doc)
        Main::documentManager()->setActiveDocument(doc);
}

void MultiEditor::onCurrentEditorChanged(GenericCodeEditor *editor)
{
    setCurrentEditor(editor);
//    currentFindReplacePanel()->setEditor( editor );
}

void MultiEditor::onBoxActivated(CodeEditorBox *box)
{
    setCurrentBox(box);
}

Document * MultiEditor::documentForIndex( int index )
{
    return mDocs.at(index);
}

int MultiEditor::indexForDocument( Document * doc )
{
    int docCount = mDocs.count();
    for (int idx = 0; idx < docCount; ++idx) {
        Document *currentDoc = documentForIndex(idx);
        if (currentDoc && currentDoc == doc)
            return idx;
    }
    return -1;
}

CodeEditorBox *MultiEditor::newBox( MultiSplitter * splitter)
{
    CodeEditorBox *box = new CodeEditorBox( splitter );

    connect(box, SIGNAL(activated(CodeEditorBox*)),
            this, SLOT(onBoxActivated(CodeEditorBox*)));

    addSplitToHistory(box);
    splitter->addSplit(box);

    return box;
}

void MultiEditor::setCurrentBox( CodeEditorBox * box )
{
    if (mCurrentEditorBox == box)
        return;

    // if a Window has been deleted, do not act over it because it would crash.
    if (mCurrentEditorBox && !checkIfWindowWasClosed())
        hideToolBox();

    mCurrentEditorBox = box;
    setCurrentSplitInHistory(box);
    mBoxSigMux->setCurrentObject(box);
    setCurrentEditor( box->currentEditor() );
    box->setFocus(Qt::OtherFocusReason);

    mCurrentEditorBox->setActive();
    // currentBox has been reset, so clear the gate.
    windowIsDeleted = false;
}

void MultiEditor::setCurrentEditor( GenericCodeEditor * editor )
{
    mEditorSigMux->setCurrentObject(editor);
    updateActions();

    Document *currentDocument = editor ? editor->document() : 0;
    Main::documentManager()->setActiveDocument(currentDocument);
    emit currentDocumentChanged(currentDocument);
}

GenericCodeEditor *MultiEditor::currentEditor()
{
    return currentBox()->currentEditor();
}

void MultiEditor::split( Qt::Orientation splitDirection )
{
    CodeEditorBox *box = newBox(currentBox()->multiSplitter());
    CodeEditorBox *curBox = currentBox();
    GenericCodeEditor *curEditor = curBox->currentEditor();

    if (curEditor)
        box->setDocument(curEditor->document(), curEditor->textCursor().position());

    currentBox()->multiSplitter()->insertWidget(box, curBox, splitDirection);
    box->setFocus( Qt::OtherFocusReason );
}

void MultiEditor::removeCurrentSplit()
{
    CodeEditorBox *box = currentBox();
    MultiSplitter *curSplit = box->multiSplitter();

    int boxCount = curSplit->findChildren<CodeEditorBox*>().count();

    if (boxCount < 2)
        // Do not allow removing the one and only box.
        return;

    // switch current box to first box found:
    CodeEditorBox *newbox = curSplit->findChild<CodeEditorBox>();
    Q_ASSERT(newbox);

    if (currentBox() == curSplit->splits().first()) {
        newbox = curSplit->splits().at(1);
        if (curSplit == mSplitter)
            defaultBox = curSplit->splits().at(1);
    }

    setCurrentBox(newbox);
    newbox->setFocus( Qt::OtherFocusReason );

    removeSplitFromHistory(box);
    curSplit->removeSplit(box);
    // delete box;
    curSplit->removeWidget(box);
}

void MultiEditor::removeAllSplits()
{
    CodeEditorBox *box = currentBox();
    Q_ASSERT(box);
    Q_ASSERT(currentBox()->multiSplitter()->count());

    if (currentBox()->multiSplitter() == mSplitter) {
        if (currentBox()->multiSplitter()->count() == 1 && currentBox()->multiSplitter()->widget(0) == box) {
            // Nothing to do.
            return;
        }
        else {
            MultiSplitter *newSplitter = new MultiSplitter(0);
            newSplitter->addWidget(box);

            delete mSplitter;
            mSplitter = newSplitter;
            layout()->addWidget(newSplitter);
            box->setFocus( Qt::OtherFocusReason );
        }
    }
    else {
        delete currentBox()->multiSplitter()->returnWindow();
    }

    mEditorHistory.clear();
    mEditorHistory.prepend(defaultBox);
}

void MultiEditor::setShowWhitespace(bool showWhitespace)
{
    Main::settings()->setValue("IDE/editor/showWhitespace", showWhitespace);
    Main::instance()->applySettings();
}

void MultiEditor::splitNewWindow()
{
    EditorWindow *window = makeNewWindow();

    MultiSplitter *newSplitter = new MultiSplitter(window);
    editorList << newSplitter;

    CodeEditorBox *box = newBox(newSplitter);
    CodeEditorBox *curBox = currentBox();
    GenericCodeEditor *curEditor = curBox->currentEditor();

    hideToolBox();

    if (curEditor)
        box->setDocument(curEditor->document(), curEditor->textCursor().position());

    newSplitter->addWidget(box);
    box->setFocus( Qt::OtherFocusReason );

    QVBoxLayout *l = setNewLayout(newSplitter);

    setCurrentBox( box ); // will updateActions();

    window->setLayout(l);
}

void MultiEditor::removeWindow() {
    // when closing a window, a signal is emitted and the function setCurrentBox() is called.
    // windowIsDeleted is done to remember not to act on the deleted window, to avoid crashing.
    windowIsDeleted = true;
    // removeWindow() does not work on the mainWindow.
    if (currentBox()->multiSplitter()->returnWindow() == 0)
        return;
    else {
        int index = editorList.indexOf(currentBox()->multiSplitter());
        editorList.removeAt(index);
        // Every Window can contain many Splits. When closing a window we have to remove all the
        // splits from the EditorSelectionPopUp.
        foreach (CodeEditorBox * box, currentBox()->multiSplitter()->splits()) {
            removeSplitFromHistory(box);
        }
        delete currentBox()->multiSplitter()->returnWindow();
    }
}

bool MultiEditor::checkIfWindowWasClosed() {
    return windowIsDeleted;
}

EditorWindow * MultiEditor::makeNewWindow() {
    EditorWindow *window = new EditorWindow();
    window->resize(640, 480);
    window->show();
    window->setWindowTitle(
        QApplication::translate("toplevel", "Code Editor")
    );
    return window;
}

QVBoxLayout * MultiEditor::setNewLayout(MultiSplitter *newSplitter) {
    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
#ifndef Q_OS_MAC
    QMenuBar *newMenu = MainWindow::instance()->createMenus();
    l->setMenuBar(newMenu);
#endif
    l->addWidget(newSplitter);
    l->addWidget(newSplitter->toolBox());
    connect(newSplitter->toolBox()->closeButton(), SIGNAL(clicked()), this, SLOT(hideToolBox()));
    return l;
}

void MultiEditor::showCmdLine()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->toolBox()->setCurrentWidget( curWindow->cmdLine() );
    curWindow->toolBox()->show();

    curWindow->cmdLine()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::showCmdLine( const QString & cmd)
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->cmdLine()->setText(cmd);
    showCmdLine();
}

void MultiEditor::showGoToLineTool()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    GenericCodeEditor *editor = currentEditor();
    curWindow->goToLineTool()->setValue( editor ? editor->textCursor().blockNumber() + 1 : 0 );

    curWindow->toolBox()->setCurrentWidget( curWindow->goToLineTool() );
    curWindow->toolBox()->show();

    curWindow->goToLineTool()->setFocus();
}

void MultiEditor::showFindTool()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->findReplaceTool()->setEditor( currentEditor() );
    curWindow->findReplaceTool()->setMode( TextFindReplacePanel::Find );
    curWindow->findReplaceTool()->initiate();

    curWindow->toolBox()->setCurrentWidget( curWindow->findReplaceTool() );
    curWindow->toolBox()->show();

    curWindow->findReplaceTool()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::showReplaceTool()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->findReplaceTool()->setEditor( currentEditor() );
    curWindow->findReplaceTool()->setMode( TextFindReplacePanel::Replace );
    curWindow->findReplaceTool()->initiate();

    curWindow->toolBox()->setCurrentWidget( curWindow->findReplaceTool() );
    curWindow->toolBox()->show();

    curWindow->findReplaceTool()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::hideToolBox()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->toolBox()->hide();
    GenericCodeEditor *editor = currentEditor();
    if (editor) {
        // This slot is mapped to Escape, so also clear highlighting
        // whenever invoked:
        editor->clearSearchHighlighting();
        if (!editor->hasFocus())
            editor->setFocus(Qt::OtherFocusReason);
    }
}

void MultiEditor::newDocument()
{
    mMain->documentManager()->create();
}

void MultiEditor::openDocument()
{
    QFileDialog dialog (this, Qt::Dialog);
    dialog.setModal(true);
    dialog.setWindowModality(Qt::ApplicationModal);

    dialog.setFileMode( QFileDialog::ExistingFiles );

    QString path = MainWindow::instance()->documentOpenPath();
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

void MultiEditor::closeDocument()
{
    GenericCodeEditor *editor = currentEditor();
    if(!editor) return;

    if (Main::documentManager()->documents().size() == 1 && !tempModeIsOn())
        newDocument();

    Q_ASSERT(editor->document());
    MainWindow::close( editor->document() );
}

void MultiEditor::openStartupFile()
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

    mMain->documentManager()->open( filePath );
}

void MultiEditor::openUserSupportDirectory()
{
    QUrl dirUrl = QUrl::fromLocalFile( standardDirectory(ScAppDataUserDir) );
    QDesktopServices::openUrl(dirUrl);
}

void MultiEditor::saveDocument()
{
    GenericCodeEditor *editor = currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    Q_ASSERT(doc);

    // when saving, check if the file exists. If it does not exist (its path is empty), it means that the file you are saving
    // is a temporary document, and its path is associated with a document that has not been found when opening the session.
    // To prevent damage to your session, warn the user that his action will modify the current session.
    int docIndex = indexForDocument( doc );
    if (tempModeIsOn() && docIndex<originalDocumentList().count()) {
        if (doc->filePath().isEmpty()) {
            QMessageBox::StandardButton sessionMessage;
            sessionMessage = QMessageBox::warning(
                currentBox()->multiSplitter()->returnWindow(),
                tr("SuperCollider IDE"),
                tr("This document is a temporary replacement, as the original file has not been found. \n"
                   "If you save it, the original file will be replaced."),
                QMessageBox::Save | QMessageBox::Cancel,
                QMessageBox::Cancel // the default
            );

            switch (sessionMessage) {
            case QMessageBox::Cancel:
                return;
            case QMessageBox::Save:
                if (!MainWindow::instance()->save(doc))
                    return;
            default:;
            }
        }
    }
    else {
        MainWindow::instance()->save(doc);
    }
}

void MultiEditor::saveDocumentAs()
{
    GenericCodeEditor *editor = currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    Q_ASSERT(doc);

    int docIndex = indexForDocument( doc );
    if (tempModeIsOn() && docIndex<originalDocumentList().count()) {
        if (doc->filePath().isEmpty()) {
            QMessageBox::StandardButton sessionMessage;
            sessionMessage = QMessageBox::warning(
                currentBox()->multiSplitter()->returnWindow(),
                tr("SuperCollider IDE"),
                tr("This document is a temporary replacement, as the original file has not been found. \n"
                   "If you save it, the original file will be replaced."),
                QMessageBox::Save | QMessageBox::Cancel,
                QMessageBox::Cancel // the default
            );

            switch (sessionMessage) {
            case QMessageBox::Cancel:
                return;
            default:;
            }
        }
    }

    MainWindow::instance()->save(doc, true);
}

void MultiEditor::saveAllDocuments()
{
    QList<Document*> docs = mMain->documentManager()->documents();
    foreach (Document *doc, docs)
        if (!MainWindow::instance()->save(doc))
            return;
}

void MultiEditor::reloadDocument()
{
    GenericCodeEditor *editor = currentEditor();
    if(!editor) return;

    Q_ASSERT(editor->document());
    MainWindow::instance()->reload(editor->document());
}

void MultiEditor::closeAllDocuments()
{
    if (MainWindow::instance()->promptSaveDocs()) {
        QList<Document*> docs = mMain->documentManager()->documents();
        foreach (Document *doc, docs)
            mMain->documentManager()->close(doc);
    }
}

void MultiEditor::newSession()
{
    mMain->sessionManager()->newSession();
}

void MultiEditor::saveCurrentSessionAs()
{
    QString name = QInputDialog::getText( this,
                                          tr("Save Current Session"),
                                          tr("Enter a name for the session:") );

    if (name.isEmpty()) return;

    mMain->sessionManager()->saveSessionAs(name);

    MainWindow::instance()->updateSessionsMenu();
}

void MultiEditor::openSessionsDialog()
{
    QPointer<MainWindow> mainwin(MainWindow::instance());
    SessionsDialog dialog(mMain->sessionManager(), this);
    dialog.exec();
    if (mainwin)
        mainwin->updateSessionsMenu();
}

void MultiEditor::showSwitchSessionDialog()
{
    SessionSwitchDialog * dialog = new SessionSwitchDialog(MainWindow::instance());
    int result = dialog->exec();

    if (result == QDialog::Accepted)
        MainWindow::instance()->openSession(dialog->activeElement());

    delete dialog;
}

void MultiEditor::findNext()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->findReplaceTool()->findNext();
}

void MultiEditor::findPrevious()
{
    MultiSplitter *curWindow = currentBox()->multiSplitter();
    curWindow->findReplaceTool()->findPrevious();
}
    
void EditorSelectPopUp::populateModel( MultiEditor::History & history )
{
    foreach (CodeEditorBox * split, history) {
        QString setLine;
        setLine.setNum(split->currentEditor()->lineNumber() + 1);
        QStandardItem * item = new QStandardItem(QString(split->currentEditor()->document()->title() + ": " + setLine  ));
        item->setData(QVariant::fromValue(split));
        mModel->appendRow(item);
    }
}

} // namespace ScIDE
