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
#include "main_window.hpp"
#include "lookup_dialog.hpp"
#include "code_editor/editor.hpp"
#include "../core/doc_manager.hpp"
#include "../core/sig_mux.hpp"
#include "../core/main.hpp"
#include "../core/sc_process.hpp"

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <QApplication>
#include <QStyle>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QShortcut>
#include <QMenu>
#include <QDebug>

#include <QDialog>
#include <QFileInfo>
#include <QHeaderView>
#include <QListView>
#include <QTreeWidget>
#include <QStandardItemModel>


namespace ScIDE {

class DocumentSelectPopUp : public QDialog
{
public:
    explicit DocumentSelectPopUp(QWidget * parent):
        QDialog(parent, Qt::Popup)
    {
        mModel = new QStandardItemModel(this);
        populateModel();

        mListView = new QListView();
        mListView->setModel(mModel);
        mListView->setFrameShape(QFrame::NoFrame);

        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->addWidget(mListView);
        layout->setContentsMargins(1,1,1,1);

        connect(mListView, SIGNAL(activated(QModelIndex)), this, SLOT(accept()));

        mListView->setFocus(Qt::OtherFocusReason);

        QModelIndex nextIndex = mModel->index(1, 0);
        mListView->setCurrentIndex(nextIndex);

        mListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    Document * exec( const QPoint & pos )
    {
        move(pos);
        if (QDialog::exec())
            return currentDocument();
        else
            return 0;
    }

private:
    void keyReleaseEvent (QKeyEvent * ke)
    {
        // adapted from qtcreator
        if (ke->modifiers() == 0
            /*HACK this is to overcome some event inconsistencies between platforms*/
            || (ke->modifiers() == Qt::AltModifier
                && (ke->key() == Qt::Key_Alt || ke->key() == -1))) {
            ke->accept();
            accept();
        }
        QDialog::keyPressEvent(ke);
    }

    void keyPressEvent(QKeyEvent * ke)
    {
        switch (ke->key()) {
        case Qt::Key_Tab: {
            int row = mListView->currentIndex().row() + 1;
            if (!mModel->hasIndex(row, 0))
                row = 0;

            QModelIndex nextIndex = mModel->index(row, 0);
            mListView->setCurrentIndex(nextIndex);
            ke->accept();
            return;
        }

        case Qt::Key_Backtab: {
            int row = mListView->currentIndex().row() - 1;
            if (!mModel->hasIndex(row, 0))
                row = mModel->rowCount() - 1;

            QModelIndex nextIndex = mModel->index(row, 0);
            mListView->setCurrentIndex(nextIndex);
            ke->accept();
            return;
        }

        case Qt::Key_Escape:
            reject();
            return;

        default:
            ;
        }

        QDialog::keyPressEvent(ke);
    }

    Document * currentDocument()
    {
        QStandardItem * currentItem = mModel->itemFromIndex(mListView->currentIndex());

        return currentItem ? currentItem->data().value<Document*>()
                           : NULL;
    }

    void populateModel()
    {
        DocumentManager * manager = Main::instance()->documentManager();
        DocumentManager::DocumentList const & documentList = manager->recentActiveDocuments();

        foreach (Document * document, documentList) {
            QStandardItem * item = new QStandardItem(document->title());
            item->setData(QVariant::fromValue(document));
            mModel->appendRow(item);
        }
    }

    QListView *mListView;
    QStandardItemModel *mModel;
};

MultiEditor::MultiEditor( Main *main, QWidget * parent ) :
    QWidget(parent),
    mDocManager(main->documentManager()),
    mSigMux(new SignalMultiplexer(this)),
    mTabs(new QTabWidget),
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) )
{
    mTabs->setDocumentMode(true);
    mTabs->setTabsClosable(true);
    mTabs->setMovable(true);
    mTabs->setUsesScrollButtons(true);

    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->addWidget(mTabs);
    setLayout(l);

    connect(mDocManager, SIGNAL(opened(Document*, int)),
            this, SLOT(onOpen(Document*, int)));
    connect(mDocManager, SIGNAL(closed(Document*)),
            this, SLOT(onClose(Document*)));
    connect(mDocManager, SIGNAL(saved(Document*)),
            this, SLOT(update(Document*)));
    connect(mDocManager, SIGNAL(showRequest(Document*,int)),
            this, SLOT(show(Document*,int))),

    connect(mTabs, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentChanged(int)));
    connect(mTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onCloseRequest(int)));

    connect(&mModificationMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(onModificationChanged(QWidget*)));

    connect(main, SIGNAL(applySettingsRequest(Settings::Manager*)),
            this, SLOT(applySettings(Settings::Manager*)));

    connect(this, SIGNAL(currentChanged(Document*)), mDocManager, SLOT(activeDocumentChanged(Document*)));

    createActions();
    updateActions();
    applySettings(main->settings());
}

void MultiEditor::createActions()
{
    Settings::Manager *settings = Main::instance()->settings();
    settings->beginGroup("IDE/shortcuts");

    QAction * act;

    // Edit

    mActions[Undo] = act = new QAction(
        QIcon::fromTheme("edit-undo"), tr("&Undo"), this);
    act->setShortcut(tr("Ctrl+Z", "Undo"));
    act->setStatusTip(tr("Undo last editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(undo()));
    mSigMux->connect(SIGNAL(undoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Redo] = act = new QAction(
        QIcon::fromTheme("edit-redo"), tr("Re&do"), this);
    act->setShortcut(tr("Ctrl+Shift+Z", "Redo"));
    act->setStatusTip(tr("Redo next editing action"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(redo()));
    mSigMux->connect(SIGNAL(redoAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Cut] = act = new QAction(
        QIcon::fromTheme("edit-cut"), tr("Cu&t"), this);
    act->setShortcut(tr("Ctrl+X", "Cut"));
    act->setStatusTip(tr("Cut text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(cut()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Copy] = act = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    act->setShortcut(tr("Ctrl+C", "Copy"));
    act->setStatusTip(tr("Copy text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copy()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Paste] = act = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    act->setShortcut(tr("Ctrl+V", "Paste"));
    act->setStatusTip(tr("Paste text from clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(paste()));

    mActions[IndentLineOrRegion] = act = new QAction(
        QIcon::fromTheme("format-indent-line"), tr("Indent Line or Region"), this);
    act->setStatusTip(tr("Indent Line or Region"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indent()));

    mActions[TriggerAutoCompletion] = act = new QAction(tr("Trigger Autocompletion"), this);
    act->setStatusTip(tr("Suggest possible completions of text at cursor"));
    act->setShortcut(tr("Ctrl+Space", "Trigger Autocompletion"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(triggerAutoCompletion()));

    mActions[TriggerMethodCallAid] = act = new QAction(tr("Trigger Method Call Aid"), this);
    act->setStatusTip(tr("Show arguments for currently typed method call"));
    act->setShortcut(tr("Alt+Space", "Trigger Method Call Aid"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(triggerMethodCallAid()));

    mActions[ToggleComment] = act = new QAction(
        QIcon::fromTheme("edit-comment"), tr("Toggle &Comment"), this);
    act->setShortcut(tr("Ctrl+/", "Toggle Comment"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Toggle Comment"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(toggleComment()));

    mActions[ToggleOverwriteMode] = act = new QAction(
        QIcon::fromTheme("edit-overwrite"), tr("Toggle &Overwrite Mode"), this);
    act->setShortcut(tr("Insert", "Toggle Overwrite Mode"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(toggleOverwriteMode()));

    mActions[CopyLineUp] = act = new QAction(
        QIcon::fromTheme("edit-copylineup"), tr("Copy Line Up"), this);
    act->setShortcut(tr("Ctrl+Alt+Up", "Copy Line Up"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copyLineUp()));

    mActions[CopyLineDown] = act = new QAction(
        QIcon::fromTheme("edit-copylinedown"), tr("Copy Line Down"), this);
    act->setShortcut(tr("Ctrl+Alt+Down", "Copy Line Up"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copyLineDown()));

    mActions[MoveLineUp] = act = new QAction(
        QIcon::fromTheme("edit-movelineup"), tr("move Line Up"), this);
    act->setShortcut(tr("Ctrl+Shift+Up", "Move Line Up"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(moveLineUp()));

    mActions[MoveLineDown] = act = new QAction(
        QIcon::fromTheme("edit-movelinedown"), tr("Move Line Down"), this);
    act->setShortcut(tr("Ctrl+Shift+Down", "Move Line Up"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(moveLineDown()));

    mActions[GotoPreviousBlock] = act = new QAction(
        QIcon::fromTheme("edit-gotopreviousblock"), tr("Go to Previous Block"), this);
    act->setShortcut(tr("Ctrl+[", "Go to Previous Block"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(gotoPreviousBlock()));

    mActions[GotoNextBlock] = act = new QAction(
        QIcon::fromTheme("edit-gotonextblock"), tr("Go to Next Block"), this);
    act->setShortcut(tr("Ctrl+]", "Go to Next Block"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(gotoNextBlock()));

    // View

    mActions[EnlargeFont] = act = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    act->setShortcut(tr("Ctrl++", "Enlarge font"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Increase displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomIn()));

    mActions[ShrinkFont] = act = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    act->setShortcut( tr("Ctrl+-", "Shrink font"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Decrease displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(zoomOut()));

    mActions[ResetFontSize] = act = new QAction(
        QIcon::fromTheme("zoom-reset"), tr("&Reset Font Size"), this);
    act->setShortcut( tr("Ctrl+0", "Reset font"));
    act->setStatusTip(tr("Reset displayed font size"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(resetFontSize()));

    mActions[ShowWhitespace] = act = new QAction(tr("Show Spaces and Tabs"), this);
    act->setCheckable(true);
    mSigMux->connect(act, SIGNAL(triggered(bool)), SLOT(setShowWhitespace(bool)));

    mActions[NextDocument] = act = new QAction(tr("Next Document"), this);
    act->setShortcut( tr("Alt+Right", "Next Document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showNextDocument()));

    mActions[PreviousDocument] = act = new QAction(tr("Previous Document"), this);
    act->setShortcut( tr("Alt+Left", "Next Document"));
    connect(act, SIGNAL(triggered()), this, SLOT(showPreviousDocument()));

    mActions[SwitchDocument] = act = new QAction(tr("Switch Document"), this);
    act->setShortcut( tr("Ctrl+Tab", "Switch Document"));
    connect(act, SIGNAL(triggered()), this, SLOT(switchDocument()));

    // Language

    mActions[EvaluateCurrentDocument] = act = new QAction(
        QIcon::fromTheme("media-playback-start"), tr("Evaluate &File"), this);
    act->setStatusTip(tr("Evaluate current File"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateDocument()));

    mActions[EvaluateRegion] = act = new QAction(
    QIcon::fromTheme("media-playback-start"), tr("&Evaluate Selection, Line or Region"), this);
    act->setShortcut(tr("Ctrl+Return", "Evaluate region"));
    act->setStatusTip(tr("Evaluate current region"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateRegion()));

    mActions[EvaluateLine] = act = new QAction(
    QIcon::fromTheme("media-playback-startline"), tr("&Evaluate Line"), this);
    act->setShortcut(tr("Shift+Ctrl+Return", "Evaluate line"));
    act->setStatusTip(tr("Evaluate current line"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act, SIGNAL(triggered()), this, SLOT(evaluateLine()));

    mActions[OpenDefinition] = act = new QAction(tr("Open Class/Method Definition"), this);
    act->setShortcut(tr("Ctrl+I", "Open definition of selected class or method"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(openDefinition()));

    settings->endGroup(); // IDE/shortcuts

    for (int i = 0; i < ActionRoleCount; ++i)
        settings->addAction( mActions[i] );

    // These actions are not added to any menu, so they have to be added
    // at least to this widget, in order for the shortcuts to always respond:
    addAction(mActions[TriggerAutoCompletion]);
    addAction(mActions[TriggerMethodCallAid]);
    addAction(mActions[SwitchDocument]);

    // These actions have to be added because to the widget because they have
    // Qt::WidgetWithChildrenShortcut context:
    addAction(mActions[EnlargeFont]);
    addAction(mActions[ShrinkFont]);
    addAction(mActions[OpenDefinition]);
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
}

void MultiEditor::updateActions()
{
    CodeEditor *editor = currentEditor();
    QTextDocument *doc = editor ? editor->document()->textDocument() : 0;

    mActions[Undo]->setEnabled( doc && doc->isUndoAvailable() );
    mActions[Redo]->setEnabled( doc && doc->isRedoAvailable() );
    mActions[Copy]->setEnabled( editor && editor->textCursor().hasSelection() );
    mActions[Cut]->setEnabled( mActions[Copy]->isEnabled() );
    mActions[Paste]->setEnabled( editor );
    mActions[ToggleComment]->setEnabled( editor );
    mActions[ToggleOverwriteMode]->setEnabled( editor );
    mActions[CopyLineUp]->setEnabled( editor );
    mActions[CopyLineDown]->setEnabled( editor );
    mActions[MoveLineUp]->setEnabled( editor );
    mActions[MoveLineDown]->setEnabled( editor );
    mActions[GotoPreviousBlock]->setEnabled( editor );
    mActions[GotoNextBlock]->setEnabled( editor );

    mActions[IndentLineOrRegion]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
    mActions[OpenDefinition]->setEnabled( editor );
    mActions[EvaluateCurrentDocument]->setEnabled( editor );
    mActions[EvaluateRegion]->setEnabled( editor );
    mActions[EvaluateLine]->setEnabled( editor );
    mActions[ResetFontSize]->setEnabled( editor );
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

void MultiEditor::setCurrent( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    if(editor)
        mTabs->setCurrentWidget(editor);
}

void MultiEditor::showNextDocument()
{
    int currentIndex = mTabs->currentIndex();
    mTabs->setCurrentIndex( qMin(currentIndex + 1, mTabs->count() - 1) );
}

void MultiEditor::showPreviousDocument()
{
    int currentIndex = mTabs->currentIndex();
    mTabs->setCurrentIndex( qMax(0, currentIndex - 1) );
}

void MultiEditor::switchDocument()
{
    DocumentSelectPopUp * popup = new DocumentSelectPopUp(this);

    QRect popupRect(0,0,300,200);
    popupRect.moveCenter(rect().center());
    popup->resize(popupRect.size());
    QPoint globalPosition = mapToGlobal(popupRect.topLeft());

    Document * selectedDocument = popup->exec(globalPosition);

    if (selectedDocument)
        mTabs->setCurrentWidget( editorForDocument(selectedDocument) );
}

void MultiEditor::onOpen( Document *doc, int pos )
{
    CodeEditor *editor = new CodeEditor();
    editor->setDocument(doc);
    editor->applySettings(Main::instance()->settings());

    QTextDocument *tdoc = doc->textDocument();

    QIcon icon;
    if(tdoc->isModified())
        icon = mDocModifiedIcon;

    mTabs->addTab( editor, icon, doc->title() );
    mTabs->setCurrentIndex( mTabs->count() - 1 );

    mModificationMapper.setMapping(tdoc, editor);
    connect(tdoc, SIGNAL(modificationChanged(bool)),
            &mModificationMapper, SLOT(map()));

    if (pos > 0)
        editor->showPosition(pos);
}

void MultiEditor::onClose( Document *doc )
{
    CodeEditor *editor = editorForDocument(doc);
    delete editor;
}

void MultiEditor::show( Document *doc, int pos )
{
    CodeEditor *editor = editorForDocument( doc );
    mTabs->setCurrentWidget(editor);
    editor->showPosition(pos);
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
        MainWindow::close(editor->document());
}

void MultiEditor::onCurrentChanged( int index )
{
    CodeEditor *editor = editorForTab(index);

    if(editor) {
        mSigMux->setCurrentObject(editor);
        editor->setFocus(Qt::OtherFocusReason);
    }

    updateActions();

    Document * currentDocument = editor ? editor->document() : NULL;

    Main::scProcess()->setActiveDocument(currentDocument);
    Q_EMIT( currentChanged( currentDocument ) );
}

void MultiEditor::onModificationChanged( QWidget *w )
{
    CodeEditor *editor = qobject_cast<CodeEditor*>(w);
    if(!editor) return;

    int i = mTabs->indexOf(editor);
    if( i == -1 ) return;

    QIcon icon;
    if(editor->document()->textDocument()->isModified())
        icon = mDocModifiedIcon;
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


void MultiEditor::evaluateRegion()
{
    CodeEditor * editor = currentEditor();
    if (!editor)
        return;

    QString text;

    // Try current selection
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection())
        text = cursor.selectedText();
    else {
        // If no selection, try current region
        cursor = editor->currentRegion();
        if (!cursor.isNull()) {
            // if region is in a single line, evaluate complete line

            QTextCursor selectionStart (cursor);
            selectionStart.setPosition(cursor.selectionStart());

            QTextCursor selectionEnd (cursor);
            selectionEnd.setPosition(cursor.selectionEnd());

            if (selectionStart.block() == selectionEnd.block())
                cursor.select(QTextCursor::LineUnderCursor);

            text = cursor.selectedText();
        } else {
            // If no current region, try current line
            cursor = editor->textCursor();
            text = cursor.block().text();
            if( mStepForwardEvaluation ) {
                QTextCursor newCursor = cursor;
                newCursor.movePosition(QTextCursor::NextBlock);
                newCursor.movePosition(QTextCursor::EndOfBlock);
                editor->setTextCursor(newCursor);
            }
            // Adjust cursor for code blinking:
            cursor.movePosition(QTextCursor::StartOfBlock);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        }
    }

    if (text.isEmpty())
        return;

    text.replace( QChar( 0x2029 ), QChar( '\n' ) );

    Main::evaluateCode(text);

    editor->blinkCode( cursor );
}

void MultiEditor::evaluateLine()
{
    CodeEditor * editor = currentEditor();
    if (!editor)
        return;

    QString text;

    // Try current selection
    QTextCursor cursor = editor->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    text = cursor.selectedText();

    if( mStepForwardEvaluation ) {
        QTextCursor newCursor = cursor;
        newCursor.movePosition(QTextCursor::NextBlock);
        newCursor.movePosition(QTextCursor::EndOfBlock);
        editor->setTextCursor(newCursor);
    }

    if (text.isEmpty())
        return;

    // Adjust cursor for code blinking:
    cursor.movePosition(QTextCursor::StartOfBlock);
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    text.replace( QChar( 0x2029 ), QChar( '\n' ) );

    Main::evaluateCode(text);

    editor->blinkCode( cursor );
}

void MultiEditor::evaluateDocument()
{
    CodeEditor * editor = currentEditor();
    if (!editor)
        return;

    QString documentText = editor->document()->textDocument()->toPlainText();
    Main::evaluateCode(documentText);
}

void MultiEditor::openDefinition(const QString &string)
{
    QString definitionString = string.trimmed();
    if (definitionString.isEmpty())
        return;

    LookupDialog dialog(this);
    dialog.query(definitionString);
    dialog.exec();
}

void MultiEditor::openDefinition()
{
    CodeEditor * editor = currentEditor();
    QTextCursor textCursor = editor->textCursor();

    if (!textCursor.hasSelection())
        textCursor.select(QTextCursor::WordUnderCursor);

    openDefinition(textCursor.selectedText());
}

bool MultiEditor::openDocumentation(const QString &string)
{
    QString symbol = string.trimmed();
    if (symbol.isEmpty())
        return false;

    QString code = QString("HelpBrowser.openHelpFor(\"%1\")").arg(symbol);
    Main::evaluateCode(code, true);
    return true;
}

bool MultiEditor::openDocumentation()
{
    CodeEditor * editor = currentEditor();
    if (!editor)
        return false;

    QTextCursor textCursor = editor->textCursor();

    if (!textCursor.hasSelection())
        textCursor.select(QTextCursor::WordUnderCursor);

    return openDocumentation(textCursor.selectedText());
}


} // namespace ScIDE
