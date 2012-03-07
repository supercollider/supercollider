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

#include <QFileDialog>

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

    createActions();
    updateActions();
}

void MultiEditor::createActions()
{
    QAction * act;

    // File

    mActions[DocNew] = act = new QAction(
        QIcon::fromTheme("document-new"), tr("&New"), this);
    act->setShortcuts(QKeySequence::New);
    act->setStatusTip(tr("Create a new document"));
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
    mSigMux->connect(SIGNAL(modificationChanged(bool)), act, SLOT(setEnabled(bool)));

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

    // Edit

    mActions[Undo] = act = new QAction(
        QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    act->setShortcuts(QKeySequence::Undo);
    act->setStatusTip(tr("Undo last editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(undo()));
    mSigMux->connect(SIGNAL(undoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Redo] = act = new QAction(
        QIcon::fromTheme("edit-redo"), tr("&Redo"), this);
    act->setShortcuts(QKeySequence::Redo);
    act->setStatusTip(tr("Redo next editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(redo()));
    mSigMux->connect(SIGNAL(redoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Cut] = act = new QAction(
        QIcon::fromTheme("edit-cut"), tr("&Cut"), this);
    act->setShortcuts(QKeySequence::Cut);
    act->setStatusTip(tr("Cut text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(cut()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Copy] = act = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    act->setShortcuts(QKeySequence::Copy);
    act->setStatusTip(tr("Copy text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copy()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Paste] = act = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    act->setShortcuts(QKeySequence::Paste);
    act->setStatusTip(tr("Paste text from clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(paste()));

    // View

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

void MultiEditor::updateActions()
{
    CodeEditor *editor = currentEditor();
    QTextDocument *doc = editor ? editor->document()->textDocument() : 0;

    mActions[DocSave]->setEnabled( doc && doc->isModified() );
    mActions[DocSaveAs]->setEnabled( doc );
    mActions[DocClose]->setEnabled( doc );
    mActions[Undo]->setEnabled( doc && doc->isUndoAvailable() );
    mActions[Redo]->setEnabled( doc && doc->isRedoAvailable() );
    mActions[Copy]->setEnabled( editor && editor->textCursor().hasSelection() );
    mActions[Cut]->setEnabled( mActions[Copy]->isEnabled() );
    mActions[Paste]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
}

void MultiEditor::newDocument()
{
    mDocManager->create();
}

void MultiEditor::openDocument()
{
    QString filename = QFileDialog::getOpenFileName( this, "Open File" );
    if(filename.isEmpty()) return;

    mDocManager->open(filename);
}

void MultiEditor::saveDocument()
{
    CodeEditor *editor = currentEditor();
    if(!editor) return;

    Document *doc = editor->document();
    if(doc->fileName().isEmpty())
        saveDocumentAs();
    else
        mDocManager->save(doc);
}

void MultiEditor::saveDocumentAs()
{
    CodeEditor *editor = currentEditor();
    if(!editor) return;

    QString filename = QFileDialog::getSaveFileName( this, "Save File" );
    if(filename.isEmpty()) return;

    mDocManager->saveAs( editor->document(), filename );
}

void MultiEditor::closeDocument()
{
    CodeEditor *editor = currentEditor();
    if(!editor) return;

    mDocManager->close(editor->document());
}

void MultiEditor::onOpen( Document *doc )
{
    CodeEditor *editor = new CodeEditor();
    editor->setDocument(doc);

    addTab( editor, doc->title() );
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
        if(editor && editor->document() == doc)
            setTabText(i, doc->title());
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
    CodeEditor *editor = editorForTab(index);
    if(editor) {
        mSigMux->setCurrentObject(editor);
        editor->setFocus(Qt::OtherFocusReason);
    }
    updateActions();
}

CodeEditor * MultiEditor::editorForTab( int index )
{
    return qobject_cast<CodeEditor*>( widget(index) );
}

CodeEditor * MultiEditor::editorForDocument( Document *doc )
{
    int c = count();
    for(int i = 0; i < c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if( editor && editor->document() == doc)
            return editor;
    }
    return 0;
}

} // namespace ScIDE
