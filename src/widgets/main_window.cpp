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

    connect(main->scProcess, SIGNAL( scPost(QString) ),
            mPostDock->mPostWindow, SLOT( append(QString) ) );
    connect(mMain->documentManager(), SIGNAL(opened(QTextDocument*)),
            this, SLOT(createTab(QTextDocument*)));
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
    menu->addAction( mMain->mStartSCLang );
    menu->addAction( mMain->mRecompileClassLibrary );
    menu->addAction( mMain->mStopSCLang );

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

void MainWindow::openDocument() {}
void MainWindow::saveDocument() {}
void MainWindow::saveDocumentAs() {}

void MainWindow::closeDocument()
{
    int tabIndex = mDocTabs->currentIndex();

    if( tabIndex < 0 ) return;

    closeTab(tabIndex);
}

void MainWindow::createTab( QTextDocument *doc )
{
    CodeEditor *editor = new CodeEditor(doc);
    mDocTabs->addTab( editor, "<new>" );
}

void MainWindow::closeTab( int tabIndex )
{
    QWidget *tabWidget = mDocTabs->widget(tabIndex);

    if(!tabWidget) {
        qWarning("Trying to close a tab with no widget.");
        return;
    }

    CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget);
    if(!editor) {
        qWarning("Tab widget is not a CodeEditor");
        return;
    }

    QTextDocument *doc = editor->document();
    mMain->documentManager()->close(doc);
    delete tabWidget;
}

} // namespace ScIDE
