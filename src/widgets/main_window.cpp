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

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

namespace ScIDE {

MainWindow::MainWindow(Main * main) :
    mMain(main)
{
    createMenus();

    mDocTabs = new QTabWidget();
    mDocTabs->setTabsClosable(true);
    setCentralWidget(mDocTabs);

    mPostDock = new PostDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, mPostDock);

    connect(main->scProcess(), SIGNAL( scPost(QString) ),
            mPostDock->mPostWindow, SLOT( append(QString) ) );
    connect(mMain->documentManager(), SIGNAL(opened(Document*)),
            this, SLOT(createTab(Document*)));
    connect(mMain->documentManager(), SIGNAL(saved(Document*)),
            this, SLOT(updateTab(Document*)));
    connect(mDocTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

void MainWindow::createMenus()
{
    QAction *act;

    mActions[DocNew] = act = new QAction(tr("&New"), this);
    act->setShortcuts(QKeySequence::New);
    act->setStatusTip(tr("Create a new file"));
    connect(act, SIGNAL(triggered()), this, SLOT(newDocument()));

    mActions[DocOpen] = act = new QAction(tr("&Open..."), this);
    act->setShortcuts(QKeySequence::Open);
    act->setStatusTip(tr("Open an existing file"));
    connect(act, SIGNAL(triggered()), this, SLOT(openDocument()));

    mActions[DocSave] = act = new QAction(tr("&Save"), this);
    act->setShortcuts(QKeySequence::Save);
    act->setStatusTip(tr("Save the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocument()));

    mActions[DocSaveAs] = act = new QAction(tr("Save &As..."), this);
    act->setShortcuts(QKeySequence::SaveAs);
    act->setStatusTip(tr("Save the current document into a different file"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));

    mActions[DocClose] = act = new QAction(tr("&Close"), this);
    act->setShortcuts(QKeySequence::SaveAs);
    act->setStatusTip(tr("Close the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(closeDocument()));

    mActions[Quit] = act = new QAction(tr("&Quit..."), this);
    act->setShortcuts(QKeySequence::Quit);
    act->setStatusTip(tr("Quit SuperCollider IDE"));

    mActions[EvaluateCurrentFile] = act = new QAction(tr("Evaluate File"), this);
    act->setStatusTip(tr("Evaluate current File"));
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateCurrentFile()));

    mActions[EvaluateSelectedRegion] = act = new QAction(tr("Evaluate Region"), this);
    act->setStatusTip(tr("Evaluate selected Region"));
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateSelectedRegion()));

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

    menu = new QMenu(tr("&Language"), this);
    menu->addAction( mMain->scProcess()->action(SCProcess::StartSCLang) );
    menu->addAction( mMain->scProcess()->action(SCProcess::RecompileClassLibrary) );
    menu->addAction( mMain->scProcess()->action(SCProcess::StopSCLang) );
    menu->addSeparator();
    menu->addAction( mActions[EvaluateCurrentFile]);
    menu->addAction( mActions[EvaluateSelectedRegion]);
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

void MainWindow::evaluateSelectedRegion()
{
    CodeEditor *editor = currentCodeEditor();
    if (!editor)
        return;

    QString selectedText = editor->textCursor().selectedText();
    mPostDock->mPostWindow->append(selectedText);

    if (!selectedText.size())
        return; // no selection

    Main::instance()->scProcess()->evaluateCode(selectedText, false);
}

void MainWindow::evaluateCurrentFile()
{
    CodeEditor *editor = currentCodeEditor();
    if (!editor)
        return;

    QString documentText = editor->document()->textDocument()->toPlainText();
    Main::instance()->scProcess()->evaluateCode(documentText, false);
}

} // namespace ScIDE
