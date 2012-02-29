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

MainWindow::MainWindow(Main * main)
{
    _actions.resize(ActionCount);

    QAction *act;

    _actions[DocNew] = act = new QAction(tr("&New"), this);
    act->setShortcuts(QKeySequence::New);
    act->setStatusTip(tr("Create a new file"));
    connect(act, SIGNAL(triggered()), this, SLOT(newDocument()));

    _actions[DocOpen] = act = new QAction(tr("&Open..."), this);
    act->setShortcuts(QKeySequence::Open);
    act->setStatusTip(tr("Open an existing file"));
    connect(act, SIGNAL(triggered()), this, SLOT(openDocument()));

    _actions[DocSave] = act = new QAction(tr("&Save"), this);
    act->setShortcuts(QKeySequence::Save);
    act->setStatusTip(tr("Save the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocument()));

    _actions[DocSaveAs] = act = new QAction(tr("Save &As..."), this);
    act->setShortcuts(QKeySequence::SaveAs);
    act->setStatusTip(tr("Save the current document into a different file"));
    connect(act, SIGNAL(triggered()), this, SLOT(saveDocumentAs()));

    _actions[DocClose] = act = new QAction(tr("&Close"), this);
    act->setShortcuts(QKeySequence::SaveAs);
    act->setStatusTip(tr("Close the current document"));
    connect(act, SIGNAL(triggered()), this, SLOT(closeDocument()));

    _actions[Quit] = act = new QAction(tr("&Quit..."), this);
    act->setShortcuts(QKeySequence::Quit);
    act->setStatusTip(tr("Quit SuperCollider IDE"));

    QMenu *menu = new QMenu(tr("&File"), this);
    menu->addAction( _actions[DocNew] );
    menu->addAction( _actions[DocOpen] );
    menu->addAction( _actions[DocSave] );
    menu->addAction( _actions[DocSaveAs] );
    menu->addSeparator();
    menu->addAction( _actions[DocClose] );
    menu->addSeparator();
    menu->addAction( _actions[Quit] );

    menuBar()->addMenu(menu);

    _docTabs = new QTabWidget();
    _docTabs->setTabsClosable(true);
    setCentralWidget(_docTabs);

    connect(_docTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

    postDock = new PostDock(this);
    addDockWidget(Qt::BottomDockWidgetArea, postDock);

    connect(main->scProcess, SIGNAL( scPost(QString) ),
            postDock->postWindow, SLOT( append(QString) ) );

    main->startScLang();
}

QAction *MainWindow::action( ActionRole role )
{
    Q_ASSERT( role < ActionCount );
    return _actions[role];
}

void MainWindow::newDocument()
{
    CodeEditor *editor = new CodeEditor();
    _docs.append( editor->document() );
    _docTabs->addTab( editor, "<new>" );
}

void MainWindow::openDocument() {}
void MainWindow::saveDocument() {}
void MainWindow::saveDocumentAs() {}

void MainWindow::closeDocument()
{
    int index = _docTabs->currentIndex();

    if( index < 0 ) return;

    closeTab(index);
}

void MainWindow::closeTab(int index)
{
    QWidget *tabWidget = _docTabs->widget(index);

    if(!tabWidget) {
        qWarning("Request to close a tab at index out of range.");
        return;
    }

    _docTabs->removeTab( index );

    CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget);
    if(!editor) {
        qWarning("Tab widget is not a CodeEditor");
        delete tabWidget;
        return;
    }

    QTextDocument *doc = editor->document();
    if(doc)
        _docs.removeAll(doc);

    delete tabWidget;
}

} // namespace ScIDE
