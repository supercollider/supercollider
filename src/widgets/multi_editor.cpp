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

#include "multi_editor.hpp"
#include "code_edit.hpp"
#include "../doc_manager.hpp"
#include "../sig_mux.hpp"

#include <QDir>

namespace ScIDE {

MultiEditor::MultiEditor( DocumentManager *manager, QWidget * parent ) :
    QTabWidget(parent),
    mDocManager(manager),
    mSigMux(new SignalMultiplexer(this))
{
    setTabsClosable(true);

    connect(manager, SIGNAL(opened(Document*)),
            this, SLOT(onOpen(Document*)));
    connect(manager, SIGNAL(closed(Document*)),
            this, SLOT(onClose(Document*)));
    connect(manager, SIGNAL(saved(Document*)),
            this, SLOT(update(Document*)));

    connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentChanged(int)));
    connect(this, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onCloseRequest(int)));

    QAction * act;

    mActions[Undo] = act = new QAction(
        QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    act->setShortcuts(QKeySequence::Undo);
    act->setStatusTip(tr("Undo last editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(undo()));

    mActions[Redo] = act = new QAction(
        QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    act->setShortcuts(QKeySequence::Redo);
    act->setStatusTip(tr("Redo next editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(redo()));

    mActions[Cut] = act = new QAction(
        QIcon::fromTheme("edit-cut"), tr("&Cut"), this);
    act->setShortcuts(QKeySequence::Cut);
    act->setStatusTip(tr("Cut text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(cut()));

    mActions[Copy] = act = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    act->setShortcuts(QKeySequence::Copy);
    act->setStatusTip(tr("Copy text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copy()));

    mActions[Paste] = act = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    act->setShortcuts(QKeySequence::Paste);
    act->setStatusTip(tr("Paste text from clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(paste()));

    mActions[EnlargeFont] = act = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    act->setShortcut(QKeySequence(tr("CTRL++", "View|Enlarge Font")));
    act->setStatusTip(tr("Increase displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomIn()));

    mActions[ShrinkFont] = act = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    act->setShortcut(QKeySequence(tr("Ctrl+-", "View|Shrink Font")));
    act->setStatusTip(tr("Decrease displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomOut()));
}


void MultiEditor::onOpen( Document *doc )
{
    CodeEditor *editor = new CodeEditor();
    editor->setDocument(doc);

    addTab( editor, tabTitle(doc) );
    setCurrentIndex( count() - 1 );
}

void MultiEditor::onClose( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    delete editor;
}

void MultiEditor::update( Document *doc )
{
    int c = count();
    for(int i=0; i<c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if(!editor) continue;
        if(editor->document() == doc)
            setTabText(i, tabTitle(doc));
    }
}

void MultiEditor::onCloseRequest( int index )
{
    CodeEditor *editor = editorForTab( index );
    if(editor)
        mDocManager->close(editor->document());
}

void MultiEditor::onCurrentChanged( int index )
{
    if( index < 0 ) return;

    CodeEditor *editor = editorForTab(index);
    if(editor) {
        mSigMux->setCurrentObject(editor);
        editor->setFocus(Qt::OtherFocusReason);
    }
}

CodeEditor * MultiEditor::editorForTab( int index )
{
    CodeEditor *editor = qobject_cast<CodeEditor*>( widget(index) );
    if(!editor) {
        qWarning("MultiEditor: no editor at given index.");
        return NULL;
    } else
        return editor;
}

CodeEditor * MultiEditor::editorForDocument( Document *doc )
{
    int c = count();
    for(int i = 0; i < c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if( editor->document() == doc)
            return editor;
    }
    return 0;
}

QString MultiEditor::tabTitle( Document *doc )
{
    QString fname = doc->fileName();
    if(fname.isEmpty())
        return QString("<new>");
    else
        return QDir(fname).dirName();
}

} // namespace ScIDE
