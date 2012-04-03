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
#include "../core/doc_manager.hpp"
#include "../core/sig_mux.hpp"
#include "../core/main.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QApplication>

namespace ScIDE {

MultiEditor::MultiEditor( Main *main, QWidget * parent ) :
    QWidget(parent),
    mDocManager(main->documentManager()),
    mSigMux(new SignalMultiplexer(this)),
    mTabs(new QTabWidget)
{
    mTabs->setDocumentMode(true);
    mTabs->setTabsClosable(true);
    mTabs->setMovable(true);

    mFindReplacePanel = new TextFindReplacePanel;
    mFindReplacePanel->setVisible(false);

    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->addWidget(mTabs);
    l->addWidget(mFindReplacePanel);
    setLayout(l);

    connect(mDocManager, SIGNAL(opened(Document*)),
            this, SLOT(onOpen(Document*)));
    connect(mDocManager, SIGNAL(closed(Document*)),
            this, SLOT(onClose(Document*)));
    connect(mDocManager, SIGNAL(saved(Document*)),
            this, SLOT(update(Document*)));

    connect(mTabs, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentChanged(int)));
    connect(mTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onCloseRequest(int)));

    connect(&mModificationMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(onModificationChanged(QWidget*)));

    connect(mFindReplacePanel, SIGNAL(close()), this, SLOT(hideToolPanel()));

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));

    createActions();
    updateActions();
    applySettings(main->settings());
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
        QIcon::fromTheme("edit-redo"), tr("Re&do"), this);
    act->setShortcuts(QKeySequence::Redo);
    act->setStatusTip(tr("Redo next editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(redo()));
    mSigMux->connect(SIGNAL(redoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Cut] = act = new QAction(
        QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
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

    mActions[Find] = act = new QAction(
        QIcon::fromTheme("edit-find"), tr("&Find..."), this);
    act->setShortcuts(QKeySequence::Find);
    act->setStatusTip(tr("Find text in document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showFindPanel()));

    mActions[Replace] = act = new QAction(
        QIcon::fromTheme("edit-replace"), tr("&Replace..."), this);
    act->setShortcuts(QKeySequence::Replace);
    act->setStatusTip(tr("Find and replace text in document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showReplacePanel()));

    new QShortcut( Qt::Key_Escape, this, SLOT(hideToolPanel()) );

    mActions[IndentMore] = act = new QAction(
        QIcon::fromTheme("format-indent-more"), tr("Indent &More"), this);
    act->setStatusTip(tr("Increase indentation of selected lines"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indentMore()));

    mActions[IndentLess] = act = new QAction(
        QIcon::fromTheme("format-indent-less"), tr("Indent &Less"), this);
    act->setStatusTip(tr("Decrease indentation of selected lines"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indentLess()));

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

    mActions[ShowWhitespace] = act = new QAction(tr("Show Spaces and Tabs"), this);
    act->setCheckable(true);
    mSigMux->connect(act, SIGNAL(triggered(bool)), SLOT(setShowWhitespace(bool)));
}

void MultiEditor::updateActions()
{
    CodeEditor *editor = currentEditor();
    QTextDocument *doc = editor ? editor->document()->textDocument() : 0;

    mActions[DocSave]->setEnabled( doc );
    mActions[DocSaveAs]->setEnabled( doc );
    mActions[DocClose]->setEnabled( doc );
    mActions[Undo]->setEnabled( doc && doc->isUndoAvailable() );
    mActions[Redo]->setEnabled( doc && doc->isRedoAvailable() );
    mActions[Copy]->setEnabled( editor && editor->textCursor().hasSelection() );
    mActions[Cut]->setEnabled( mActions[Copy]->isEnabled() );
    mActions[Paste]->setEnabled( editor );
    mActions[IndentMore]->setEnabled( editor );
    mActions[IndentLess]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
    mActions[ShowWhitespace]->setEnabled( editor );
    mActions[ShowWhitespace]->setChecked( editor && editor->showWhitespace() );
}

void MultiEditor::applySettings( Settings::Manager *s )
{
    s->beginGroup("IDE/editor");
    mStepForwardEvaluation = s->value("stepForwardEvaluation").toBool();
    s->endGroup();

    int c = editorCount();
    for( int i = 0; i < c; ++i )
    {
        CodeEditor *editor = editorForTab(i);
        if(!editor) continue;
        editor->applySettings(s);
    }
}

void MultiEditor::newDocument()
{
    mDocManager->create();
}

void MultiEditor::openDocument()
{
    mDocManager->open();
}

void MultiEditor::saveDocument()
{
    CodeEditor *editor = currentEditor();
    if(!editor) return;

    mDocManager->save( editor->document() );
}

void MultiEditor::saveDocumentAs()
{
    CodeEditor *editor = currentEditor();
    if(!editor) return;

    mDocManager->saveAs( editor->document() );
}

void MultiEditor::closeDocument()
{
    CodeEditor *editor = currentEditor();
    if(!editor) return;

    mDocManager->close(editor->document());
}

void MultiEditor::setCurrent( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    if(editor)
        mTabs->setCurrentWidget(editor);
}

void MultiEditor::showFindPanel()
{
    mFindReplacePanel->setMode( TextFindReplacePanel::Find );
    mFindReplacePanel->initiate();
    mFindReplacePanel->show();
    mFindReplacePanel->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::showReplacePanel()
{
    mFindReplacePanel->setMode( TextFindReplacePanel::Replace );
    mFindReplacePanel->initiate();
    mFindReplacePanel->show();
    mFindReplacePanel->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::hideToolPanel()
{
    mFindReplacePanel->hide();
    CodeEditor *editor = currentEditor();
    if (editor && !editor->hasFocus())
        editor->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::onOpen( Document *doc )
{
    CodeEditor *editor = new CodeEditor();
    editor->setDocument(doc);
    editor->applySettings(Main::instance()->settings());

    QTextDocument *tdoc = doc->textDocument();

    QIcon icon;
    if(tdoc->isModified())
        icon = QIcon::fromTheme("document-save");

    mTabs->addTab( editor, icon, doc->title() );
    mTabs->setCurrentIndex( mTabs->count() - 1 );

    mModificationMapper.setMapping(tdoc, editor);
    connect(tdoc, SIGNAL(modificationChanged(bool)),
            &mModificationMapper, SLOT(map()));
}

void MultiEditor::onClose( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    delete editor;
}

void MultiEditor::update( Document *doc )
{
    int c = editorCount();
    for(int i=0; i<c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if(editor && editor->document() == doc)
            mTabs->setTabText(i, doc->title());
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

    mFindReplacePanel->setEditor(editor);

    updateActions();

    if(editor)
        Q_EMIT( currentChanged(editor->document()) );
}

void MultiEditor::onModificationChanged( QWidget *w )
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(w);
    if(!editor) return;

    int i = mTabs->indexOf(editor);
    if( i == -1 ) return;

    QIcon icon;
    if(editor->document()->textDocument()->isModified())
        icon = QIcon::fromTheme("document-save");
    mTabs->setTabIcon( i, icon );
}

CodeEditor * MultiEditor::editorForTab( int index )
{
    return qobject_cast<CodeEditor*>( mTabs->widget(index) );
}

CodeEditor * MultiEditor::editorForDocument( Document *doc )
{
    int c = editorCount();
    for(int i = 0; i < c; ++i) {
        CodeEditor *editor = editorForTab(i);
        if( editor && editor->document() == doc)
            return editor;
    }
    return 0;
}

////////////////////// TextFindReplacePanel //////////////////////////

TextFindReplacePanel::TextFindReplacePanel( QWidget * parent ):
    QWidget(parent),
    mMode((Mode) 0), // a hack so that first setMode() works
    mEditor(0)
{
    mCloseBtn = new QToolButton;
    mCloseBtn->setIcon(QIcon::fromTheme("window-close"));
    mCloseBtn->setAutoRaise(true);

    mFindField = new QLineEdit;
    mReplaceField = new QLineEdit;

    mNextBtn = new QPushButton(tr("Next"));
    mPrevBtn = new QPushButton(tr("Previous"));
    mFindAllBtn = new QPushButton(tr("Find All"));
    mReplaceBtn = new QPushButton(tr("Replace"));
    mReplaceAllBtn = new QPushButton(tr("Replace All"));

    mMatchCaseOpt = new QCheckBox(tr("Match Case"));

    mFindLabel = new QLabel(tr("Find:"));
    mFindLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mReplaceLabel = new QLabel(tr("Replace:"));
    mReplaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    mGrid = new QGridLayout();
    mGrid->setContentsMargins(2,2,2,2);
    mGrid->addWidget(mCloseBtn, 0, 0);
    mGrid->addWidget(mFindLabel, 0, 1);
    mGrid->addWidget(mFindField, 0, 2);
    mGrid->addWidget(mNextBtn, 0, 3);
    mGrid->addWidget(mPrevBtn, 0, 4);
    mGrid->addWidget(mFindAllBtn, 0, 5);
    mGrid->addWidget(mMatchCaseOpt, 0, 6);
    mGrid->addWidget(mReplaceLabel, 1, 1);
    mGrid->addWidget(mReplaceField, 1, 2);
    mGrid->addWidget(mReplaceBtn, 1, 3);
    mGrid->addWidget(mReplaceAllBtn, 1, 4);
    setLayout(mGrid);

    setMode(Find);

    setFocusProxy(mFindField);
    QWidget::setTabOrder(mFindField, mReplaceField);

    connect(mCloseBtn, SIGNAL(clicked()), this, SIGNAL(close()));
    connect(mNextBtn, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(mPrevBtn, SIGNAL(clicked()), this, SLOT(findPrevious()));
    connect(mFindAllBtn, SIGNAL(clicked()), this, SLOT(findAll()));
    connect(mReplaceBtn, SIGNAL(clicked()), this, SLOT(replace()));
    connect(mReplaceAllBtn, SIGNAL(clicked()), this, SLOT(replaceAll()));
    connect(mFindField, SIGNAL(returnPressed()), this, SLOT(onFindFieldReturn()));
    connect(mReplaceField, SIGNAL(returnPressed()), this, SLOT(replace()));

    // FIXME: disabling non-functional buttons for now:
    mFindAllBtn->setEnabled(false);
    mReplaceAllBtn->setEnabled(false);
}

void TextFindReplacePanel::setMode( Mode mode )
{
    if (mode == mMode) return;

    mMode = mode;

    bool visible = mMode == Replace;
    mReplaceLabel->setVisible(visible);
    mReplaceField->setVisible(visible);
    mReplaceBtn->setVisible(visible);
    mReplaceAllBtn->setVisible(visible);
}

void TextFindReplacePanel::initiate()
{
    if(mEditor)
    {
        QTextCursor c( mEditor->textCursor() );
        if(c.hasSelection() &&
           c.document()->findBlock(c.selectionStart()) ==
           c.document()->findBlock(c.selectionEnd()))
        {
            mFindField->setText(c.selectedText());
            mReplaceField->clear();
        }
    }

    mFindField->selectAll();
}

void TextFindReplacePanel::findNext()
{
    find(false);
}

void TextFindReplacePanel::findPrevious()
{
    find(true);
}

void TextFindReplacePanel::onFindFieldReturn()
{
    find (QApplication::keyboardModifiers() & Qt::ShiftModifier);
}

void TextFindReplacePanel::find (bool backwards)
{
    if (!mEditor) return;

    QString str(findString());
    if (str.isEmpty()) return;

    QTextDocument::FindFlags flags;
    if (backwards)
        flags |= QTextDocument::FindBackward;
    if (matchCase())
        flags |= QTextDocument::FindCaseSensitively;

    mEditor->find(str, flags);
}

void TextFindReplacePanel::findAll()
{
    //TODO
}

void TextFindReplacePanel::replace()
{
    if (!mEditor) return;

    QString fstr(findString());
    if (fstr.isEmpty()) return;

    QString rstr(replaceString());

    QTextCursor c( mEditor->textCursor() );
    if (c.hasSelection() && c.selectedText().compare(
            fstr, matchCase() ? Qt::CaseSensitive : Qt::CaseInsensitive) == 0)
    {
        c.insertText(rstr);
    }

    findNext();
}

void TextFindReplacePanel::replaceAll()
{
    //TODO
}


} // namespace ScIDE
