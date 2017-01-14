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
    #ifndef Q_OS_MAC
        QDialog(parent, Qt::Popup  | Qt::FramelessWindowHint)
    #else
        QDialog(parent, Qt::Dialog | Qt::FramelessWindowHint)
    #endif
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
        foreach(GenericCodeEditor *editor, history)
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

EditorTabBar::EditorTabBar(QWidget *parent):
    QTabBar(parent)
{
    setDocumentMode(true);
    setTabsClosable(true);
    setMovable(true);
    setUsesScrollButtons(true);
    setDrawBase(true);
    setElideMode(Qt::ElideNone);
}

void EditorTabBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        showContextMenu(event);
        event->accept();
        return;
    }
    else if (event->button() == Qt::MiddleButton){
        mTabUnderCursor = tabAt(event->pos());
        onCloseTab();
        event->accept();
        return;
    }

    QTabBar::mousePressEvent(event);
}


void EditorTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (tabAt(event->pos()) == -1) { // no tab under cursor
            MainWindow::instance()->newDocument();
            event->accept();
            return;
        }
    }

    QTabBar::mouseDoubleClickEvent(event);
}

void EditorTabBar::showContextMenu(QMouseEvent * event)
{
    mTabUnderCursor = tabAt(event->pos());

    QMenu * menu = new QMenu(this);
    // Cannot have a close tab action if we are not over a tab
    if (mTabUnderCursor == -1){
        menu->addAction(tr("Close All Tabs"), this, SLOT(onCloseOtherTabs()));
    }
    else{
        menu->addAction(tr("Close"), this, SLOT(onCloseTab()));
        menu->addAction(tr("Close Other Tabs"), this, SLOT(onCloseOtherTabs()));
        menu->addAction(tr("Close Tabs to the Right"), this, SLOT(onCloseTabsToTheRight()));
    }

    menu->popup(event->pos());
}

void EditorTabBar::onCloseTab()
{
    Document* doc = tabData(mTabUnderCursor).value<Document*>();
    assert(doc);

    MainWindow::close(doc);
}

void EditorTabBar::onCloseOtherTabs()
{
    QVector<Document*> docsToClose;

    for(int currentTab = 0; currentTab != count(); ++currentTab) {
        if (currentTab != mTabUnderCursor)
            docsToClose.append(tabData(currentTab).value<Document*>());
    }

    for( Document * doc : docsToClose )
        MainWindow::close(doc);
}

void EditorTabBar::onCloseTabsToTheRight()
{
    QVector<Document*> docsToClose;

    for(int currentTab = mTabUnderCursor + 1; currentTab != count(); ++currentTab)
        docsToClose.append(tabData(currentTab).value<Document*>());

    for( Document * doc : docsToClose )
        MainWindow::close(doc);
}


MultiEditor::MultiEditor( Main *main, QWidget * parent ) :
    QWidget(parent),
    mEditorSigMux(new SignalMultiplexer(this)),
    mBoxSigMux(new SignalMultiplexer(this)),
#ifdef __APPLE__
    mDocModifiedIcon( QApplication::style()->standardIcon(QStyle::SP_DriveHDIcon) )
#else
    mDocModifiedIcon( QIcon::fromTheme("document-save") )
#endif
{
    mTabs = new EditorTabBar;

    mSplitter = new MultiSplitter(this);
    CodeEditorBox *defaultBox = newBox(mSplitter);

    mSplitter->addWidget(defaultBox);

    multiEditorLayout = new QVBoxLayout;
    multiEditorLayout->setContentsMargins(0,0,0,0);
    multiEditorLayout->setSpacing(0);
    multiEditorLayout->addWidget(mTabs);
    multiEditorLayout->addWidget(mSplitter);
    setLayout(multiEditorLayout);

    makeSignalConnections();

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

    connect(mTabs, SIGNAL(currentChanged(int)),
            this, SLOT(onCurrentTabChanged(int)));
    connect(mTabs, SIGNAL(tabCloseRequested(int)),
            this, SLOT(onCloseRequest(int)));
    connect(mTabs, SIGNAL(tabMoved(int, int)),
            this, SLOT(updateDocOrder(int, int)));

    mBoxSigMux->connect(SIGNAL(currentChanged(GenericCodeEditor*)),
            this, SLOT(onCurrentEditorChanged(GenericCodeEditor*)));
    
}

void MultiEditor::updateDocOrder(int from, int to)
{
    Q_EMIT( updateDockletOrder(from, to) );
}

void MultiEditor::breakSignalConnections()
{
    DocumentManager *docManager = Main::documentManager();
    docManager->disconnect(this);
    mTabs->disconnect(this);
    mBoxSigMux->disconnect(this);
}

void MultiEditor::createActions()
{
    Settings::Manager *settings = Main::settings();

    QAction * action;
    const QString editorCategory(tr("Text Editor"));

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

    mActions[DeleteWord] = action = new QAction(
        QIcon::fromTheme("edit-deleteword"), tr("Delete Word"), this);
#ifdef Q_OS_MAC
    action->setShortcut(tr("Meta+W", "Delete Word"));
#endif
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(deleteWord()));
    settings->addAction(action, "delete-word", editorCategory);

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

    mActions[DocClose] = action = new QAction(
        QIcon::fromTheme("window-close"), tr("&Close"), this);
    action->setShortcut(tr("Ctrl+W", "Close document"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Close the current document"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(closeDocument()));
    settings->addAction( action, "ide-document-close", editorCategory);

    mActions[EnlargeFont] = action = new QAction(
        QIcon::fromTheme("zoom-in"), tr("&Enlarge Font"), this);
    action->setShortcut(tr("Ctrl++", "Enlarge font"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Increase displayed font size"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(zoomIn()));
    settings->addAction( action, "editor-enlarge-font", editorCategory);

    mActions[ShrinkFont] = action = new QAction(
        QIcon::fromTheme("zoom-out"), tr("&Shrink Font"), this);
    action->setShortcut( tr("Ctrl+-", "Shrink font"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    action->setStatusTip(tr("Decrease displayed font size"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(zoomOut()));
    settings->addAction( action, "editor-shrink-font", editorCategory);

    mActions[ResetFontSize] = action = new QAction(
        QIcon::fromTheme("zoom-reset"), tr("&Reset Font Size"), this);
    action->setShortcut( tr("Ctrl+0", "Reset font"));
    action->setStatusTip(tr("Reset displayed font size"));
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(resetFontSize()));
    settings->addAction( action, "editor-reset-font-size", editorCategory);

    mActions[ShowWhitespace] = action = new QAction(tr("Show Spaces and Tabs"), this);
    action->setCheckable(true);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
#ifdef Q_OS_MAC
    action->setShortcut( QKeySequence( Qt::META | Qt::Key_E, Qt::META | Qt::Key_V ) );
#else
    action->setShortcut( QKeySequence( Qt::ALT | Qt::Key_E, Qt::ALT | Qt::Key_V ) );
#endif
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setShowWhitespace(bool)));
    settings->addAction( action, "editor-toggle-show-whitespace", editorCategory);

    mActions[ShowLinenumber] = action = new QAction(tr("Show Line Number"), this);
    action->setCheckable(true);
    action->setShortcut( tr("Ctrl+Alt+#", "Show Line Number"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setShowLinenumber(bool)));
    settings->addAction( action, "editor-toggle-show-line-number", editorCategory);

    mActions[ShowAutocompleteHelp] = action = new QAction(tr("Show Autocomplete Help"), this);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(setShowAutocompleteHelp(bool)));
    settings->addAction( action, "editor-toggle-show-autocomplete-help", editorCategory);

    mActions[IndentWithSpaces] = action = new QAction(tr("Use Spaces for Indentation"), this);
    action->setCheckable(true);
    action->setStatusTip( tr("Indent with spaces instead of tabs") );
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered(bool)), SLOT(setSpaceIndent(bool)),
                           SignalMultiplexer::ConnectionOptional );
    settings->addAction( action, "editor-toggle-space-indent", editorCategory );

    mActions[NextDocument] = action = new QAction(tr("Next Document"), this);
#ifndef Q_OS_MAC
    action->setShortcut( tr("Alt+Right", "Next Document"));
#else
    action->setShortcut( tr("Ctrl+Alt+Right", "Next Document"));
#endif
    connect(action, SIGNAL(triggered()), this, SLOT(showNextDocument()));
    settings->addAction( action, "editor-document-next", editorCategory);

    mActions[PreviousDocument] = action = new QAction(tr("Previous Document"), this);
#ifndef Q_OS_MAC
    action->setShortcut( tr("Alt+Left", "Previous Document"));
#else
    action->setShortcut( tr("Ctrl+Alt+Left", "Previous Document"));
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

    mActions[SplitHorizontally] = action = new QAction(tr("Split To Right"), this);
    //action->setShortcut( tr("Ctrl+P, 3", "Split To Right"));
    connect(action, SIGNAL(triggered()), this, SLOT(splitHorizontally()));
    settings->addAction( action, "editor-split-right", editorCategory);

    mActions[SplitVertically] = action = new QAction(tr("Split To Bottom"), this);
    //action->setShortcut( tr("Ctrl+P, 2", "Split To Bottom"));
    connect(action, SIGNAL(triggered()), this, SLOT(splitVertically()));
    settings->addAction( action, "editor-split-bottom", editorCategory);

    mActions[RemoveCurrentSplit] = action = new QAction(tr("Remove Current Split"), this);
    //action->setShortcut( tr("Ctrl+P, 1", "Remove Current Split"));
    connect(action, SIGNAL(triggered()), this, SLOT(removeCurrentSplit()));
    settings->addAction( action, "editor-split-remove", editorCategory);

    mActions[RemoveAllSplits] = action = new QAction(tr("Remove All Splits"), this);
    //action->setShortcut( tr("Ctrl+P, 0", "Remove All Splits"));
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
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(evaluateRegion()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-eval-smart", editorCategory);

    mActions[EvaluateLine] = action = new QAction(
    QIcon::fromTheme("media-playback-start"), tr("&Evaluate Selection or Line"), this);
    action->setShortcut(tr("Shift+Return", "Evaluate selection/line"));
    action->setStatusTip(tr("Evaluate current selection/line"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    mEditorSigMux->connect(action, SIGNAL(triggered()), SLOT(evaluateLine()),
                           SignalMultiplexer::ConnectionOptional);
    settings->addAction( action, "editor-eval-line", editorCategory);

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
    addAction(mActions[DocClose]);
    addAction(mActions[EnlargeFont]);
    addAction(mActions[ShrinkFont]);
    addAction(mActions[ShowWhitespace]);
    addAction(mActions[ShowLinenumber]);
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
    addAction(mActions[DeleteWord]);
    addAction(mActions[GotoPreviousBlock]);
    addAction(mActions[GotoNextBlock]);
    addAction(mActions[SelectEnclosingBlock]);
    addAction(mActions[GotoPreviousRegion]);
    addAction(mActions[GotoNextRegion]);
    addAction(mActions[GotoPreviousEmptyLine]);
    addAction(mActions[GotoNextEmptyLine]);
    addAction(mActions[SelectRegion]);
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
    mActions[DeleteWord]->setEnabled( editor );
    mActions[GotoPreviousEmptyLine]->setEnabled( editor );
    mActions[GotoNextEmptyLine]->setEnabled( editor );
    mActions[DocClose]->setEnabled( editor );
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
    bool show_linenumber = settings->value("IDE/editor/showLinenumber").toBool();
    bool show_autocompletehelp = settings->value("IDE/editor/showAutocompleteHelp").toBool();
    mActions[ShowWhitespace]->setChecked( show_whitespace );
    mActions[ShowLinenumber]->setChecked( show_linenumber );
    mActions[ShowAutocompleteHelp]->setChecked(show_autocompletehelp);

    setMainComboBoxOption();

    int boxCount = mSplitter->findChildren<CodeEditorBox*>().count();
    if (boxCount > 1) {
        activateComboBoxWhenSplitting();  
    }
}

void MultiEditor::activateComboBoxWhenSplitting() {
    emit splitViewActivated();
    bool comboBoxInUse = Main::settings()->value("IDE/editor/useComboBox").toBool();
    if (!comboBoxInUse) {
        bool comboBoxWhenSplitting = Main::settings()->value("IDE/editor/useComboBoxWhenSplitting").toBool();
        showEditorTabs(comboBoxWhenSplitting);
    }
}

void MultiEditor::setMainComboBoxOption() {
    bool comboBoxInUse = Main::settings()->value("IDE/editor/useComboBox").toBool();
    showEditorTabs( comboBoxInUse );
}

void MultiEditor::showEditorTabs( bool condition ) 
{
    if (condition)
        mTabs->hide();
    else
        mTabs->show();
}

static QVariantList saveBoxState( CodeEditorBox *box, const QList<Document*> & documentList )
{
    // save editors in reverse order - first one is last shown.
    QVariantList boxData;
    int idx = box->history().count();
    while(idx--) {
        GenericCodeEditor *editor = box->history()[idx];
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
        if (doc) {
            documentList << doc;
            tabsData << doc->filePath();
        }
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

void MultiEditor::loadSplitterState( MultiSplitter *splitter,
                                     const QVariantMap & data, const QList<Document*> & documentList )
{
    QByteArray state = QByteArray::fromBase64( data.value("state").value<QByteArray>() );

    QVariantList childrenData = data.value("elements").value<QVariantList>();
    foreach (const QVariant & childVar, childrenData) {
        if (childVar.type() == QVariant::List) {
            CodeEditorBox *childBox = newBox(splitter);
            splitter->addWidget(childBox);
            QVariantList childBoxData = childVar.value<QVariantList>();
            loadBoxState( childBox, childBoxData, documentList );
        }
        else if (childVar.type() == QVariant::Map) {
            MultiSplitter *childSplitter = new MultiSplitter(this);
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

    mSplitter = new MultiSplitter(this);

    CodeEditorBox *firstBox = 0;

    if (session)
    {
        // open documents saved in the session
        QVariantList docDataList = session->value("documents").value<QVariantList>();
        foreach( const QVariant & docData, docDataList ) {
            QString filePath = docData.toString();
            Document * doc = docManager->open(filePath, -1, 0, true);
            documentList << doc;
        }

        // restore tabs
        foreach ( Document * doc, documentList )
            addTab(doc);

        // restore editors
        if (session->contains("editors")) {
            QVariantMap splitterData = session->value("editors").value<QVariantMap>();
            loadSplitterState( mSplitter, splitterData, documentList );

            if (mSplitter->count()) {
                firstBox = mSplitter->findChild<CodeEditorBox>();
                if (!firstBox) {
                    qWarning("Session seems to contain invalid editor split data!");
                    delete mSplitter;
                    mSplitter = new MultiSplitter(this);
                }
            }
        }
    }

    if (!firstBox) {
        // Restoring the session didn't result in any editor box, so create one:
        firstBox = newBox(mSplitter);
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

    setMainComboBoxOption();
    if (mSplitter->count()>1)
        activateComboBoxWhenSplitting();
    else
        emit splitViewDeactivated();
}

int MultiEditor::addTab( Document * doc )
{
    if (!doc)
        return -1;

    int tabIdx = tabForDocument(doc);
    if (tabIdx != -1)
        return tabIdx;

    QTextDocument *tdoc = doc->textDocument();

    QIcon icon;
    if(tdoc->isModified())
        icon = mDocModifiedIcon;

    tabIdx = mTabs->addTab( icon, doc->title() );
    mTabs->setTabData( tabIdx, QVariant::fromValue<Document*>(doc) );

    mDocModifiedSigMap.setMapping(tdoc, doc);
    connect( tdoc, SIGNAL(modificationChanged(bool)), &mDocModifiedSigMap, SLOT(map()) );

    return tabIdx;
}

void MultiEditor::setCurrent( Document *doc )
{
    int tabIdx = tabForDocument(doc);
    if (tabIdx != -1)
        mTabs->setCurrentIndex(tabIdx);
}

void MultiEditor::updateTabsOrder( QList<Document*> docOrder ) {
    mTabs->blockSignals(true);
    for ( int idx = 0; idx < docOrder.count(); idx++ ) {
        if ( docOrder.at(idx) != documentForTab(idx) ) {
            Document *doc = docOrder.at(idx);
            int tabIdx = tabForDocument(doc);
            mTabs->moveTab(tabIdx, idx);
        }
    }
    mTabs->blockSignals(false);
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
    addTab(doc);

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

void MultiEditor::onDocModified( QObject *object )
{
    Document *doc = qobject_cast<Document*>(object);
    if (!doc) return;

    int tabIdx = tabForDocument(doc);
    if (tabIdx == -1) return;

    bool isModified = doc->textDocument()->isModified();
    QIcon icon;
    if(isModified)
        icon = mDocModifiedIcon;

    Main::evaluateCodeIfCompiled(QStringLiteral("Document.findByQUuid(\'%1\').prSetEdited(%2)").arg(doc->id().constData()).arg(isModified), true);
    
    mTabs->setTabIcon( tabIdx, icon );
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

    // update thisProcess.nowExecutingPath
    GenericCodeEditor *editor = currentEditor();
    if (editor->document() == doc)
        Main::documentManager()->setActiveDocument(doc);
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

void MultiEditor::onCurrentEditorChanged(GenericCodeEditor *editor)
{
    setCurrentEditor(editor);
}

void MultiEditor::onBoxActivated(CodeEditorBox *box)
{
    setCurrentBox(box);
}

Document * MultiEditor::documentForTab( int index )
{
    QVariant doc = mTabs->tabData(index);
    if (doc.isValid() && !doc.isNull())
        return doc.value<Document*>();
    else
        return NULL;
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

CodeEditorBox *MultiEditor::newBox( MultiSplitter * currSplitter )
{
    CodeEditorBox *box = new CodeEditorBox( currSplitter );

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

void MultiEditor::setCurrentEditor( GenericCodeEditor * editor )
{
    if (editor) {
        int tabIndex = tabForDocument(editor->document());
        if (tabIndex != -1)
            mTabs->setCurrentIndex(tabIndex);
    }

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
    CodeEditorBox *box = newBox(mSplitter);
    CodeEditorBox *curBox = currentBox();
    GenericCodeEditor *curEditor = curBox->currentEditor();

    if (curEditor)
        box->setDocument(curEditor->document(), curEditor->textCursor().position());

    mSplitter->insertWidget(box, curBox, splitDirection);
    box->setFocus( Qt::OtherFocusReason );

    activateComboBoxWhenSplitting();
}

void MultiEditor::removeCurrentSplit()
{
    int boxCount = mSplitter->findChildren<CodeEditorBox*>().count();
    if (boxCount < 2) {
        // Do not allow removing the one and only box.
        return;
    }

    CodeEditorBox *box = currentBox();
    mSplitter->removeWidget(box);

    // switch current box to first box found:
    box = mSplitter->findChild<CodeEditorBox>();
    Q_ASSERT(box);
    setCurrentBox(box);
    box->setFocus( Qt::OtherFocusReason );

    if (boxCount == 2) {
        emit splitViewDeactivated();
        setMainComboBoxOption();
    }
    else if (boxCount > 2) {
        activateComboBoxWhenSplitting();
    }
}

void MultiEditor::removeAllSplits()
{
    CodeEditorBox *box = currentBox();
    Q_ASSERT(box);
    Q_ASSERT(mSplitter->count());
    if (mSplitter->count() == 1 && mSplitter->widget(0) == box)
        // Nothing to do.
        return;

    breakSignalConnections();

    MultiSplitter *newSplitter = new MultiSplitter(this);

    CodeEditorBox *nBox = newBox(newSplitter);
    newSplitter->addWidget(nBox);

    GenericCodeEditor *curEditor = box->currentEditor();
    if (curEditor)
        nBox->setDocument(curEditor->document(), curEditor->textCursor().position());

    delete mSplitter;
    mSplitter = newSplitter;
    layout()->addWidget(mSplitter);

    emit splitViewDeactivated();
    setMainComboBoxOption();

    mCurrentEditorBox = 0; // ensure complete update
    setCurrentBox(nBox);   

    makeSignalConnections(); // ensure signal connections

    nBox->setFocus( Qt::OtherFocusReason );
}

void MultiEditor::setShowWhitespace(bool showWhitespace)
{
    Main::settings()->setValue("IDE/editor/showWhitespace", showWhitespace);
    Main::instance()->applySettings();
}

void MultiEditor::setShowLinenumber(bool showLinenumber)
{
    Main::settings()->setValue("IDE/editor/showLinenumber", showLinenumber);
    Main::instance()->applySettings();
}

void MultiEditor::setShowAutocompleteHelp(bool showAutocompleteHelp)
{
    Main::settings()->setValue("IDE/editor/showAutocompleteHelp", showAutocompleteHelp);
}

} // namespace ScIDE
