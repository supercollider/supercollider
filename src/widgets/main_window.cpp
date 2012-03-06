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

#include "../main.hpp"
#include "main_window.hpp"
#include "post_window.hpp"
#include "code_edit.hpp"
#include "cmd_line.hpp"

#include <QAction>
#include <QShortcut>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QVBoxLayout>

namespace ScIDE {

MainWindow::MainWindow(Main * main) :
    mMain(main),
    mDocSigMux(new SignalMultiplexer(this))
{
    createMenus();

    mDocTabs = new QTabWidget();
    mDocTabs->setTabsClosable(true);

    // use a layout for tool widgets, to provide for separate margin control
    QVBoxLayout *tool_box = new QVBoxLayout;
    tool_box->addWidget(cmdLine());
    tool_box->setContentsMargins(5,2,5,2);

    QVBoxLayout *center_box = new QVBoxLayout;
    center_box->setContentsMargins(0,0,0,0);
    center_box->setSpacing(0);
    center_box->addWidget(mDocTabs);
    center_box->addLayout(tool_box);
    QWidget *central = new QWidget;
    central->setLayout(center_box);
    setCentralWidget(central);

    mPostDock = new PostDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, mPostDock);

    connect(main->scProcess(), SIGNAL( scPost(QString) ),
            mPostDock->mPostWindow, SLOT( append(QString) ) );
    connect(mMain->documentManager(), SIGNAL(opened(Document*)),
            this, SLOT(createTab(Document*)));
    connect(mMain->documentManager(), SIGNAL(saved(Document*)),
            this, SLOT(updateTab(Document*)));

    connect(mDocTabs, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentEditorChanged(int)));
    connect(mDocTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(closeTab(int)));
}

void MainWindow::createMenus()
{
    QAction *act;

    // File

    mActions[DocNew] = act = new QAction(
        QIcon::fromTheme("document-new"), tr("&New"), this);
    act->setShortcuts(QKeySequence::New);
    act->setStatusTip(tr("Create a new file"));
    connect(act, SIGNAL(triggered()), this, SLOT(newDocument()));

    mActions[DocOpen] = act = new QAction(
        QIcon::fromTheme("document-open"), tr("&Open..."), this);
    act->setShortcuts(QKeySequence::Open);
    act->setStatusTip(tr("Open an existing file"));
    connect(act, SIGNAL(triggered()), this, SLOT(openDocument()));

    mActions[DocSave] = act = new QAction(
        QIcon::fromTheme("document-save"), tr("&Save"), this);
    act->setShortcuts(QKeySequence::Save);
    act->setStatusTip(tr("Save the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocument()));

    mActions[DocSaveAs] = act = new QAction(
        QIcon::fromTheme("document-save-as"), tr("Save &As..."), this);
    act->setShortcuts(QKeySequence::SaveAs);
    act->setStatusTip(tr("Save the current document into a different file"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));

    mActions[DocClose] = act = new QAction(
        QIcon::fromTheme("window-close"), tr("&Close"), this);
    act->setShortcuts(QKeySequence::Close);
    act->setStatusTip(tr("Close the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(closeDocument()));

    mActions[Quit] = act = new QAction(
        QIcon::fromTheme("application-exit"), tr("&Quit..."), this);
    act->setShortcuts(QKeySequence::Quit);
    act->setStatusTip(tr("Quit SuperCollider IDE"));

    // Edit

    mActions[Undo] = act = new QAction(
        QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    act->setShortcuts(QKeySequence::Undo);
    act->setStatusTip(tr("Undo last editing action"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(undo()));

    mActions[Redo] = act = new QAction(
        QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    act->setShortcuts(QKeySequence::Redo);
    act->setStatusTip(tr("Redo next editing action"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(redo()));

    mActions[Cut] = act = new QAction(
        QIcon::fromTheme("edit-cut"), tr("&Cut"), this);
    act->setShortcuts(QKeySequence::Cut);
    act->setStatusTip(tr("Cut text to clipboard"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(cut()));

    mActions[Copy] = act = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    act->setShortcuts(QKeySequence::Copy);
    act->setStatusTip(tr("Copy text to clipboard"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(copy()));

    mActions[Paste] = act = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    act->setShortcuts(QKeySequence::Paste);
    act->setStatusTip(tr("Paste text from clipboard"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(paste()));

    // View

    mActions[IncreaseFontSize] = act = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    act->setShortcut(QKeySequence(tr("CTRL++", "View|Enlarge Font")));
    act->setStatusTip(tr("Increase displayed font size"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomIn()));

    mActions[DecreaseFontSize] = act = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    act->setShortcut(QKeySequence(tr("Ctrl+-", "View|Shrink Font")));
    act->setStatusTip(tr("Decrease displayed font size"));
    mDocSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomOut()));

    new QShortcut( QKeySequence("Ctrl+Tab"), this, SLOT(toggleComandLineFocus()) );

    // Language

    mActions[EvaluateCurrentFile] = act = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("Evaluate &File"), this);
    act->setStatusTip(tr("Evaluate current File"));
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateCurrentFile()));

    mActions[EvaluateSelectedRegion] = act = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("&Evaluate Region"), this);
    act->setShortcut(QKeySequence(tr("Ctrl+Return", "Language|Evaluate Region")));
    act->setStatusTip(tr("Evaluate selection or current line"));
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateCurrentRegion()));

    mActions[StepForwardOnEvaluation] = act = new QAction(tr("&Step Forward"), this);
    act->setStatusTip(tr("Step to the next line on evaluation"));
    act->setCheckable(true);

    mMain->scProcess()->action(ScIDE::SCProcess::StopMain)
        ->setShortcut(QKeySequence(tr("Ctrl+.", "Language|Stop Main")));

    QMenu *menu = new QMenu(tr("&File"), this);
    menu->addAction( mActions[DocNew] );
    menu->addAction( mActions[DocOpen] );
    menu->addAction( mActions[DocSave] );
    menu->addAction( mActions[DocSaveAs] );
    menu->addSeparator();
    menu->addAction( mActions[DocClose] );
    menu->addSeparator();
    menu->addAction( mActions[Quit] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Edit"), this);
    menu->addAction( mActions[Undo] );
    menu->addAction( mActions[Redo] );
    menu->addSeparator();
    menu->addAction( mActions[Cut] );
    menu->addAction( mActions[Copy] );
    menu->addAction( mActions[Paste] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&View"), this);
    menu->addAction( mActions[IncreaseFontSize] );
    menu->addAction( mActions[DecreaseFontSize] );

    menuBar()->addMenu(menu);

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(SCProcess::StartSCLang) );
    menu->addAction( mMain->scProcess()->action(SCProcess::RecompileClassLibrary) );
    menu->addAction( mMain->scProcess()->action(SCProcess::StopSCLang) );
    menu->addSeparator();
    menu->addAction( mActions[EvaluateCurrentFile] );
    menu->addAction( mActions[EvaluateSelectedRegion] );
    menu->addAction( mActions[StepForwardOnEvaluation] );
    menu->addSeparator();
    menu->addAction( mMain->scProcess()->action(ScIDE::SCProcess::RunMain) );
    menu->addAction( mMain->scProcess()->action(ScIDE::SCProcess::StopMain) );

    menuBar()->addMenu(menu);
}


QAction *MainWindow::action( ActionRole role )
{
    Q_ASSERT( role < ActionCount );
    return mActions[role];
}

void MainWindow::newDocument()
{
    mMain->documentManager()->create();
}

void MainWindow::openDocument()
{
    QString filename = QFileDialog::getOpenFileName( this, "Open File" );
    if(filename.isEmpty()) return;

    mMain->documentManager()->open(filename);
}

void MainWindow::saveDocument()
{
    CodeEditor *editor = currentCodeEditor();
    if(!editor) return;

    Document *doc = editor->document();
    if(doc->fileName().isEmpty())
        saveDocumentAs();
    else
        mMain->documentManager()->save(doc);
}

void MainWindow::saveDocumentAs()
{
    CodeEditor *editor = currentCodeEditor();
    if(!editor) return;

    QString filename = QFileDialog::getSaveFileName( this, "Save File" );
    if(filename.isEmpty()) return;

    mMain->documentManager()->saveAs( editor->document(), filename );
}

void MainWindow::closeDocument()
{
    int tabIndex = mDocTabs->currentIndex();

    if( tabIndex < 0 ) return;

    closeTab(tabIndex);
}

void MainWindow::toggleComandLineFocus()
{
    QWidget *cmd = cmdLine();
    if(cmd->hasFocus()) {
        QWidget *editor = currentCodeEditor();
        if(editor) editor->setFocus(Qt::OtherFocusReason);
    }
    else
        cmd->setFocus(Qt::OtherFocusReason);
}

void MainWindow::createTab( Document *doc )
{
    CodeEditor *editor = new CodeEditor();
    editor->setDocument(doc);

    mDocTabs->addTab( editor, tabTitle(doc) );
    mDocTabs->setCurrentIndex(mDocTabs->count() - 1);
}

void MainWindow::updateTab( Document *doc )
{
    int c = mDocTabs->count();
    for(int i=0; i<c; ++i) {
        CodeEditor *editor = codeEditorForTab(i);
        if(!editor) continue;
        if(editor->document() == doc)
            mDocTabs->setTabText(i, tabTitle(doc));
    }
}

void MainWindow::closeTab( int tabIndex )
{
    CodeEditor *editor = codeEditorForTab( tabIndex );
    if(!editor) return;

    Document *doc = editor->document();
    delete editor;
    mMain->documentManager()->close(doc);
}

void MainWindow::onCurrentEditorChanged(int index)
{
    if( index < 0 ) return;

    CodeEditor *editor = codeEditorForTab(index);
    if(editor) {
        mDocSigMux->setCurrentObject(editor);
        editor->setFocus(Qt::OtherFocusReason);
    }
}

CodeEditor* MainWindow::codeEditorForTab( int index )
{
    CodeEditor *editor = qobject_cast<CodeEditor*>( mDocTabs->widget(index) );
    if(!editor) {
        qWarning("MainWindow: no code editor at given tab index.");
        return NULL;
    } else
        return editor;
}

QString MainWindow::tabTitle( Document *doc )
{
    QString fname = doc->fileName();
    if(fname.isEmpty())
        return QString("<new>");
    else
        return QDir(fname).dirName();
}

QWidget *MainWindow::cmdLine()
{
    static QWidget *widget = 0;
    if(!widget) {
        CmdLine *w = new CmdLine("Command line:");
        connect(w, SIGNAL(invoked(QString, bool)),
                mMain->scProcess(), SLOT(evaluateCode(QString, bool)));
        widget = w;
    }
    return widget;
}

void MainWindow::evaluateCurrentRegion()
{
    // Evaluate selection, if any, otherwise current line

    CodeEditor *editor = currentCodeEditor();
    if (!editor)
        return;

    QString text;
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection())
        text = cursor.selectedText();
    else {
        text = cursor.block().text();
        if( mActions[StepForwardOnEvaluation]->isChecked() ) {
            cursor.movePosition(QTextCursor::NextBlock);
            cursor.movePosition(QTextCursor::EndOfBlock);
            editor->setTextCursor(cursor);
        }
    }

    if (text.isEmpty()) return;

    Main::instance()->scProcess()->evaluateCode(text);
}

void MainWindow::evaluateCurrentFile()
{
    CodeEditor *editor = currentCodeEditor();
    if (!editor)
        return;

    QString documentText = editor->document()->textDocument()->toPlainText();
    Main::instance()->scProcess()->evaluateCode(documentText);
}

} // namespace ScIDE
