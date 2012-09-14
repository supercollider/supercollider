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

#include "yaml-cpp/node.h"
#include "yaml-cpp/parser.h"

#include <QApplication>
#include <QDebug>
#include <QDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QListView>
#include <QMenu>
#include <QPainter>
#include <QStandardItemModel>
#include <QShortcut>
#include <QStyle>
#include <QTreeWidget>
#include <QVBoxLayout>


namespace ScIDE {

class DocumentSelectPopUp : public QDialog
{
public:
    DocumentSelectPopUp(const CodeEditorBox::History & history, QWidget * parent):
        QDialog(parent, Qt::Popup)
    {
        mModel = new QStandardItemModel(this);
        populateModel(history);

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
    bool event(QEvent * event)
    {
        if (event->type() == QEvent::ShortcutOverride) {
            event->accept();
            return true;
        }
        return QWidget::event(event);
    }

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
        QDialog::keyReleaseEvent(ke);
    }

    void keyPressEvent(QKeyEvent * ke)
    {
        switch (ke->key()) {
        case Qt::Key_Down:
        case Qt::Key_Tab:
            cycleDown();
            ke->accept();
            return;

        case Qt::Key_Up:
        case Qt::Key_Backtab:
            cycleUp();
            ke->accept();
            return;

        case Qt::Key_Escape:
            reject();
            return;

        default:
            ;
        }

        QDialog::keyPressEvent(ke);
    }

    void paintEvent( QPaintEvent * )
    {
        QPainter painter(this);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(palette().color(QPalette::Dark));
        painter.drawRect(rect().adjusted(0,0,-1,-1));
    }

    void cycleDown()
    {
        int row = mListView->currentIndex().row() + 1;
        if (!mModel->hasIndex(row, 0))
            row = 0;

        QModelIndex nextIndex = mModel->index(row, 0);
        mListView->setCurrentIndex(nextIndex);
    }

    void cycleUp()
    {
        int row = mListView->currentIndex().row() - 1;
        if (!mModel->hasIndex(row, 0))
            row = mModel->rowCount() - 1;

        QModelIndex nextIndex = mModel->index(row, 0);
        mListView->setCurrentIndex(nextIndex);
    }

    Document * currentDocument()
    {
        QStandardItem * currentItem = mModel->itemFromIndex(mListView->currentIndex());
        return currentItem ? currentItem->data().value<Document*>()
                           : NULL;
    }

    void populateModel( const CodeEditorBox::History & history )
    {
        QList<Document*> displayDocuments;
        foreach(ScCodeEditor *editor, history)
            displayDocuments << editor->document();

        QList<Document*> managerDocuments =  Main::documentManager()->documents();
        foreach(Document *document, managerDocuments)
            if (!displayDocuments.contains(document))
                displayDocuments << document;

        foreach (Document * document, displayDocuments) {
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
    mSigMux(new SignalMultiplexer(this)),
    mBoxSigMux(new SignalMultiplexer(this)),
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton) )
{
    mTabs = new QTabBar;
    mTabs->setDocumentMode(true);
    mTabs->setTabsClosable(true);
    mTabs->setMovable(true);
    mTabs->setUsesScrollButtons(true);
    mTabs->setDrawBase(false);

    CodeEditorBox *defaultBox = newBox();

    mSplitter = new MultiSplitter();
    mSplitter->addWidget(defaultBox);

    QVBoxLayout *l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    l->addWidget(mTabs);
    l->addWidget(mSplitter);
    setLayout(l);

    makeSignalConnections();

    mSigMux->connect(SIGNAL(modificationChanged(bool)),
                     this, SLOT(onModificationChanged(bool)));

    mBoxSigMux->connect(SIGNAL(currentChanged(ScCodeEditor*)),
                        this, SLOT(onCurrentEditorChanged(ScCodeEditor*)));

    createActions();

    setCurrentBox( defaultBox ); // will updateActions();

    applySettings(main->settings());
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

    connect(mTabs, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentTabChanged(int)));
    connect(mTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onCloseRequest(int)));
}

void MultiEditor::breakSignalConnections()
{
    DocumentManager *docManager = Main::documentManager();
    docManager->disconnect(this);
    mTabs->disconnect(this);
}

void MultiEditor::createActions()
{
    Settings::Manager *settings = Main::settings();
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
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Cut text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(cut()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Copy] = act = new QAction(
        QIcon::fromTheme("edit-copy"), tr("&Copy"), this);
    act->setShortcut(tr("Ctrl+C", "Copy"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Copy text to clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(copy()));
    mSigMux->connect(SIGNAL(copyAvailable(bool)), act, SLOT(setEnabled(bool)));

    mActions[Paste] = act = new QAction(
        QIcon::fromTheme("edit-paste"), tr("&Paste"), this);
    act->setShortcut(tr("Ctrl+V", "Paste"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    act->setStatusTip(tr("Paste text from clipboard"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(paste()));

    mActions[IndentLineOrRegion] = act = new QAction(
        QIcon::fromTheme("format-indent-line"), tr("Indent Line or Region"), this);
    act->setStatusTip(tr("Indent Line or Region"));
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(indent()));

    mActions[TriggerAutoCompletion] = act = new QAction(tr("Trigger Autocompletion"), this);
    act->setStatusTip(tr("Suggest possible completions of text at cursor"));
    act->setShortcut(tr("Ctrl+Space", "Trigger Autocompletion"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(triggerAutoCompletion()));

    mActions[TriggerMethodCallAid] = act = new QAction(tr("Trigger Method Call Aid"), this);
    act->setStatusTip(tr("Show arguments for currently typed method call"));
    act->setShortcut(tr("Ctrl+Shift+Space", "Trigger Method Call Aid"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
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

    mActions[GotoPreviousRegion] = act = new QAction(
        QIcon::fromTheme("edit-gotopreviousregion"), tr("Go to Previous Region"), this);
    act->setShortcut(tr("Alt+[", "Go to Previous Region"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(gotoPreviousRegion()));

    mActions[GotoNextRegion] = act = new QAction(
        QIcon::fromTheme("edit-gotonextregion"), tr("Go to Next Region"), this);
    act->setShortcut(tr("Alt+]", "Go to Next Region"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(gotoNextRegion()));

    mActions[GotoPreviousEmptyLine] = act = new QAction( tr("Go to Previous Empty Line"), this);
    act->setShortcut(tr("Ctrl+Up", "Go to Previous Empty Line"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(gotoPreviousEmptyLine()));

    mActions[GotoNextEmptyLine] = act = new QAction( tr("Go to Next Empty Line"), this);
    act->setShortcut(tr("Ctrl+Down", "Go to Next Empty Line"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(gotoNextEmptyLine()));

    mActions[SelectRegion] = act = new QAction( tr("Select Region"), this);
    act->setShortcut(tr("Ctrl+Shift+R", "Select Region"));
    act->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mSigMux->connect(act, SIGNAL(triggered()), SLOT(selectCurrentRegion()));

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

    mActions[SplitHorizontally] = act = new QAction(tr("Split To Right"), this);
    act->setShortcut( tr("Ctrl+P, 3", "Split To Right"));
    connect(act, SIGNAL(triggered()), this, SLOT(splitHorizontally()));

    mActions[SplitVertically] = act = new QAction(tr("Split To Bottom"), this);
    act->setShortcut( tr("Ctrl+P, 2", "Split To Bottom"));
    connect(act, SIGNAL(triggered()), this, SLOT(splitVertically()));

    mActions[RemoveCurrentSplit] = act = new QAction(tr("Remove Current Split"), this);
    act->setShortcut( tr("Ctrl+P, 1", "Remove Current Split"));
    connect(act, SIGNAL(triggered()), this, SLOT(removeCurrentSplit()));

    mActions[RemoveAllSplits] = act = new QAction(tr("Remove All Splits"), this);
    act->setShortcut( tr("Ctrl+P, 0", "Remove All Splits"));
    connect(act, SIGNAL(triggered()), this, SLOT(removeAllSplits()));

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
    addAction(mActions[Cut]);
    addAction(mActions[Copy]);
    addAction(mActions[Paste]);
    addAction(mActions[EnlargeFont]);
    addAction(mActions[ShrinkFont]);
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
    addAction(mActions[GotoPreviousRegion]);
    addAction(mActions[GotoNextRegion]);
    addAction(mActions[GotoPreviousEmptyLine]);
    addAction(mActions[GotoNextEmptyLine]);
    addAction(mActions[SelectRegion]);
}

void MultiEditor::updateActions()
{
    ScCodeEditor *editor = currentEditor();
    QTextDocument *doc = editor ? editor->textDocument() : 0;

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
    mActions[GotoPreviousRegion]->setEnabled( editor );
    mActions[GotoNextRegion]->setEnabled( editor );
    mActions[GotoPreviousEmptyLine]->setEnabled( editor );
    mActions[GotoNextEmptyLine]->setEnabled( editor );
    mActions[SelectRegion]->setEnabled( editor );

    mActions[IndentLineOrRegion]->setEnabled( editor );
    mActions[EnlargeFont]->setEnabled( editor );
    mActions[ShrinkFont]->setEnabled( editor );
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
}

static QVariantList saveBoxState( CodeEditorBox *box, const QList<Document*> & documentList )
{
    // save editors in reverse order - first one is last shown.
    QVariantList boxData;
    int idx = box->history().count();
    while(idx--) {
        ScCodeEditor *editor = box->history()[idx];
        if (!editor->document()->filePath().isEmpty()) {
            int documentIndex = documentList.indexOf( editor->document() );
            Q_ASSERT(documentIndex >= 0);
            QVariantMap editorData;
            editorData.insert("documentIndex", documentIndex);
            editorData.insert("position", editor->textCursor().position());
            boxData.append( editorData );
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

    QVariantList tabsData;
    int tabCount = mTabs->count();
    for (int tabIdx = 0; tabIdx < tabCount; ++tabIdx) {
        Document *doc = documentForTab(tabIdx);
        documentList << doc;
        tabsData << doc->filePath();
    }

    session->setValue( "documents", QVariant::fromValue(tabsData) );

    session->remove( "editors" );
    session->setValue( "editors", saveSplitterState(mSplitter, documentList) );
}

void MultiEditor::loadBoxState( CodeEditorBox *box,
                                const QVariantList & data, const QList<Document*> & documentList )
{
    int docCount = documentList.count();
    foreach( QVariant docVar, data )
    {
        QVariantMap docData = docVar.value<QVariantMap>();
        int docIndex = docData.value("documentIndex").toInt();
        int docPos = docData.value("position").toInt();
        if (docIndex >= 0 && docIndex < docCount)
            box->setDocument( documentList[docIndex], docPos );
    }
}

void MultiEditor::loadSplitterState( QSplitter *splitter,
                                     const QVariantMap & data, const QList<Document*> & documentList )
{
    QByteArray state = QByteArray::fromBase64( data.value("state").value<QByteArray>() );

    QVariantList childrenData = data.value("elements").value<QVariantList>();
    foreach (const QVariant & childVar, childrenData) {
        if (childVar.type() == QVariant::List) {
            CodeEditorBox *childBox = newBox();
            splitter->addWidget(childBox);
            QVariantList childBoxData = childVar.value<QVariantList>();
            loadBoxState( childBox, childBoxData, documentList );
        }
        else if (childVar.type() == QVariant::Map) {
            QSplitter *childSplitter = new QSplitter;
            splitter->addWidget(childSplitter);
            QVariantMap childSplitterData = childVar.value<QVariantMap>();
            loadSplitterState( childSplitter, childSplitterData, documentList );
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

    // remove all tabs
    while (mTabs->count())
        mTabs->removeTab(0);

    // remove all editors
    delete mSplitter;

    documentList.clear();

    mSplitter = new MultiSplitter();

    CodeEditorBox *firstBox = 0;

    if (session)
    {
        // open documents saved in the session
        QVariantList docDataList = session->value("documents").value<QVariantList>();
        foreach( const QVariant & docData, docDataList ) {
            QString filePath = docData.toString();
            Document * doc = docManager->open(filePath, -1, 0, false);
            documentList << doc;
        }

        // restore tabs
        foreach ( Document * doc, documentList ) {
            if (!doc)
                continue;
            int newTabIndex = mTabs->addTab( doc->title() );
            mTabs->setTabData( newTabIndex, QVariant::fromValue<Document*>(doc) );
        }

        // restore editors
        if (session->contains("editors")) {
            QVariantMap splitterData = session->value("editors").value<QVariantMap>();
            loadSplitterState( mSplitter, splitterData, documentList );

            if (mSplitter->count()) {
                firstBox = mSplitter->findChild<CodeEditorBox>();
                if (!firstBox) {
                    qWarning("Session seems to contain invalid editor split data!");
                    delete mSplitter;
                    mSplitter = new MultiSplitter();
                }
            }
        }
    }

    if (!firstBox) {
        // Restoring the session didn't result in any editor box, so create one:
        firstBox = newBox();
        mSplitter->addWidget( firstBox );
    }

    layout()->addWidget(mSplitter);

    makeSignalConnections();

    ///// Back online.

    mCurrentEditorBox = 0; // ensure complete update
    setCurrentBox( firstBox );

    if (!session)
        // create a document on new session
        docManager->create();

    firstBox->setFocus(Qt::OtherFocusReason); // ensure focus
}

void MultiEditor::setCurrent( Document *doc )
{
    int tabIdx = tabForDocument(doc);
    if (tabIdx != -1)
        mTabs->setCurrentIndex(tabIdx);
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
    CodeEditorBox *box = currentBox();

    DocumentSelectPopUp * popup = new DocumentSelectPopUp(box->history(), this);

    QRect popupRect(0,0,300,200);
    popupRect.moveCenter(rect().center());
    popup->resize(popupRect.size());
    QPoint globalPosition = mapToGlobal(popupRect.topLeft());

    Document * selectedDocument = popup->exec(globalPosition);

    if (selectedDocument)
        box->setDocument(selectedDocument);
}

void MultiEditor::onOpen( Document *doc, int initialCursorPosition, int selectionLength )
{
    QTextDocument *tdoc = doc->textDocument();

    QIcon icon;
    if(tdoc->isModified())
        icon = mDocModifiedIcon;

    int newTabIndex = mTabs->addTab( icon, doc->title() );
    mTabs->setTabData( newTabIndex, QVariant::fromValue<Document*>(doc) );

    currentBox()->setDocument(doc, initialCursorPosition, selectionLength);
    currentBox()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::onClose( Document *doc )
{
    int tabIdx = tabForDocument(doc);
    if (tabIdx != -1)
        mTabs->removeTab(tabIdx);
    // TODO: each box should switch document according to their own history
}

void MultiEditor::show( Document *doc, int pos, int selectionLength )
{
    currentBox()->setDocument(doc, pos, selectionLength);
    currentBox()->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::update( Document *doc )
{
    int tabIdx = tabForDocument(doc);
    if (tabIdx != -1)
        mTabs->setTabText(tabIdx, doc->title());
}

void MultiEditor::onCloseRequest( int index )
{
    Document *doc = documentForTab(index);
    if (doc)
        MainWindow::close(doc);
}

void MultiEditor::onCurrentTabChanged( int index )
{
    if (index == -1)
        return;

    Document *doc = documentForTab(index);
    if (!doc)
        return;

    CodeEditorBox *curBox = currentBox();
    curBox->setDocument(doc);
    curBox->setFocus(Qt::OtherFocusReason);
}

void MultiEditor::onCurrentEditorChanged(ScCodeEditor *editor)
{
    setCurrentEditor(editor);
}

void MultiEditor::onBoxActivated(CodeEditorBox *box)
{
    setCurrentBox(box);
}

void MultiEditor::onModificationChanged( bool modified )
{
    Q_ASSERT(currentEditor());

    int tabIdx = tabForDocument( currentEditor()->document() );
    if (tabIdx == -1)
        return;

    QIcon icon;
    if(modified)
        icon = mDocModifiedIcon;
    mTabs->setTabIcon( tabIdx, icon );
}

void MultiEditor::evaluateRegion()
{
    ScCodeEditor * editor = currentEditor();
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
            text = cursor.selectedText();
        } else {
            // If no current region, try current line
            cursor = editor->textCursor();
            text = cursor.block().text();
            if( mStepForwardEvaluation ) {
                QTextCursor newCursor = cursor;
                newCursor.movePosition(QTextCursor::NextBlock);
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
    ScCodeEditor * editor = currentEditor();
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
    ScCodeEditor * editor = currentEditor();
    if (!editor)
        return;

    QString documentText = editor->textDocument()->toPlainText();
    Main::evaluateCode(documentText);
}

Document * MultiEditor::documentForTab( int index )
{
    return mTabs->tabData(index).value<Document*>();
}

int MultiEditor::tabForDocument( Document * doc )
{
    int tabCount = mTabs->count();
    for (int idx = 0; idx < tabCount; ++idx) {
        Document *tabDoc = documentForTab(idx);
        if (tabDoc && tabDoc == doc)
            return idx;
    }
    return -1;
}

CodeEditorBox *MultiEditor::newBox()
{
    CodeEditorBox *box = new CodeEditorBox();

    connect(box, SIGNAL(activated(CodeEditorBox*)),
            this, SLOT(onBoxActivated(CodeEditorBox*)));

    return box;
}

void MultiEditor::setCurrentBox( CodeEditorBox * box )
{
    if (mCurrentEditorBox == box)
        return;

    mCurrentEditorBox = box;
    mBoxSigMux->setCurrentObject(box);
    setCurrentEditor( box->currentEditor() );

    mCurrentEditorBox->setActive();
}

void MultiEditor::setCurrentEditor( ScCodeEditor * editor )
{
    if (editor) {
        int tabIndex = tabForDocument(editor->document());
        if (tabIndex != -1)
            mTabs->setCurrentIndex(tabIndex);
    }

    mSigMux->setCurrentObject(editor);
    updateActions();

    Document *currentDocument = editor ? editor->document() : 0;
    Main::scProcess()->setActiveDocument(currentDocument);
    emit currentDocumentChanged(currentDocument);
}

ScCodeEditor *MultiEditor::currentEditor()
{
    return currentBox()->currentEditor();
}

void MultiEditor::split( Qt::Orientation splitDirection )
{
    CodeEditorBox *box = newBox();
    CodeEditorBox *curBox = currentBox();
    ScCodeEditor *curEditor = curBox->currentEditor();

    if (curEditor)
        box->setDocument(curEditor->document(), curEditor->textCursor().position());

    mSplitter->insertWidget(box, curBox, splitDirection);
    box->setFocus( Qt::OtherFocusReason );
}

void MultiEditor::removeCurrentSplit()
{
    int boxCount = mSplitter->findChildren<CodeEditorBox*>().count();
    if (boxCount < 2)
        // Do not allow removing the one and only box.
        return;

    CodeEditorBox *box = currentBox();
    mSplitter->removeWidget(box);

    // switch current box to first box found:
    box = mSplitter->findChild<CodeEditorBox>();
    Q_ASSERT(box);
    setCurrentBox(box);
    box->setFocus( Qt::OtherFocusReason );
}

void MultiEditor::removeAllSplits()
{
    CodeEditorBox *box = currentBox();
    Q_ASSERT(box);
    Q_ASSERT(mSplitter->count());
    if (mSplitter->count() == 1 && mSplitter->widget(0) == box)
        // Nothing to do.
        return;

    MultiSplitter *newSplitter = new MultiSplitter;
    newSplitter->addWidget(box);

    delete mSplitter;
    mSplitter = newSplitter;
    layout()->addWidget(newSplitter);

    box->setFocus( Qt::OtherFocusReason );
}

} // namespace ScIDE
