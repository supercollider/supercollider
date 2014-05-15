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

#ifndef SCIDE_WIDGETS_MULTI_EDITOR_HPP_INCLUDED
#define SCIDE_WIDGETS_MULTI_EDITOR_HPP_INCLUDED

#include "../core/doc_manager.hpp"
#include "../core/main.hpp"
#include "editor_box.hpp"
#include "main_window.hpp"

#include <QWidget>
#include <QAction>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QTextDocument>
#include <QSplitter>
#include <QSignalMapper>
#include <QPointer>
#include <QStandardItemModel>
#include <QDialog>
#include <QListView>
#include <QPainter>

namespace ScIDE {

class CodeEditorBox;
class Document;
class DocumentManager;
class GenericCodeEditor;
class Main;
class MultiSplitter;
struct Session;
class SignalMultiplexer;
class ToolBox;
class TextFindReplacePanel;
class GoToLineTool;
class CmdLine;
class EditorWindow;

namespace Settings { class Manager; }

class MultiEditor : public QWidget
{
    Q_OBJECT

public:

    enum ActionRole
    {
        // Edit
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        IndentLineOrRegion,
        TriggerAutoCompletion,
        TriggerMethodCallAid,
        ToggleComment,
        ToggleOverwriteMode,

        CopyLineUp,
        CopyLineDown,
        MoveLineUp,
        MoveLineDown,

        GotoPreviousBlock,
        GotoNextBlock,
        SelectEnclosingBlock,

        GotoPreviousRegion,
        GotoNextRegion,
        GotoPreviousEmptyLine,
        GotoNextEmptyLine,

        SelectRegion,

        // View
        EnlargeFont,
        ShrinkFont,
        ResetFontSize,
        ShowWhitespace,
        IndentWithSpaces,

        NextDocument,
        PreviousDocument,
        SwitchDocument,
        SwitchSplit,

        SplitHorizontally,
        SplitVertically,
        SplitNewWindow,
        RemoveCurrentSplit,
        RemoveCurrentWindow,
        RemoveAllSplits,

        // Language
        EvaluateCurrentDocument,
        EvaluateRegion,
        EvaluateLine,

        // File
        Quit,
        DocNew,
        DocOpen,
        DocOpenStartup,
        DocOpenSupportDir,
        DocSave,
        DocSaveAs,
        DocSaveAll,
        DocClose,
        DocCloseAll,
        DocReload,

        // Session
        NewSession,
        SaveSessionAs,
        ManageSessions,
        OpenSessionSwitchDialog,

        // Edit
        Find,
        FindNext,
        FindPrevious,
        Replace,

        // View
        ShowCmdLine,
        CmdLineForCursor,
        ShowGoToLineTool,
        CloseToolBox,
        ShowFullScreen,
        FocusPostWindow,

        // Settings
        ShowSettings,

        // Language
        LookupImplementation,
        LookupImplementationForCursor,
        LookupReferences,

        // Help
        Help,
        LookupReferencesForCursor,
        LookupDocumentationForCursor,
        LookupDocumentation,        

        ActionRoleCount
    };

    typedef QList< CodeEditorBox * > History;

    MultiEditor( Main *, QWidget * parent = 0 );

    Document * documentForIndex( int index );
    int indexForDocument( Document * doc );

    GenericCodeEditor *currentEditor();
    CodeEditorBox *currentBox() { return mCurrentEditorBox; }
    CodeEditorBox *returnDefaultBox() { return defaultBox; }
    bool tempModeIsOn() { return tempMode; }
    QVariantList originalDocumentList() { return originalDocumentsList; }
    void split( Qt::Orientation direction );
    SignalMultiplexer * boxSigMux() { return mBoxSigMux; }
    History & splitHistory() { return mEditorHistory; }

    QAction * action( ActionRole role )
        { return mActions[role]; }

    void saveSession( Session * );
    void switchSession( Session * );

signals:
    void activated( MultiEditor *);
    void currentDocumentChanged( Document * );

public slots:
    void setCurrent( Document * );

    void showNextDocument();
    void showPreviousDocument();
    void switchDocument();
    void switchSplit();
    void splitHorizontally() { split(Qt::Horizontal); }
    void splitVertically() { split(Qt::Vertical); }
    void removeCurrentSplit();
    void removeAllSplits();
    void splitNewWindow();
    void removeWindow();

    void setShowWhitespace(bool on);

    void showCmdLine( const QString & );

    void saveDocument();
    void saveDocumentAs();
    void saveAllDocuments();
    void closeDocument();
    void reloadDocument();
    void closeAllDocuments();
    void hideToolBox();

private slots:
    void newDocument();
    void openDocument();
    void openStartupFile();
    void openUserSupportDirectory();
    void newSession();
    void saveCurrentSessionAs();
    void openSessionsDialog();
    void showSwitchSessionDialog();
    void showCmdLine();
    void showGoToLineTool();
    void showFindTool();
    void showReplaceTool();
    void findNext();
    void findPrevious();
    void applySettings( Settings::Manager * );
    void onOpen( Document *, int initialCursorPosition, int selectionLength );
    void onClose( Document * );
    void show( Document *, int cursorPosition = -1, int selectionLenght = 0 );
    void update( Document * );
    void onCurrentEditorChanged( GenericCodeEditor * );
    void onBoxActivated( CodeEditorBox * );
    void onDocModified( QObject * );

private:
    void makeSignalConnections();
    void breakSignalConnections();
    void createActions();
    void updateActions();
    int addDoc( Document * );
    CodeEditorBox *newBox(MultiSplitter *);
    void setCurrentBox( CodeEditorBox * );
    void setCurrentEditor( GenericCodeEditor * );
    void loadBoxState( CodeEditorBox *box, const QVariantList & data, const QList<Document *> & documentList );
    void loadSplitterState( MultiSplitter *, const QVariantMap & data, const QList<Document *> & documentList, MultiSplitter *);
    void addSplitToHistory(CodeEditorBox *box);
    void removeSplitFromHistory(CodeEditorBox *box);
    void setCurrentSplitInHistory(CodeEditorBox *box);
    bool checkIfWindowWasClosed();
    EditorWindow *makeNewWindow();
    QVBoxLayout *setNewLayout(MultiSplitter *);

    QAction *mActions[ActionRoleCount];

    SignalMultiplexer * mEditorSigMux;
    SignalMultiplexer * mBoxSigMux;
    QSignalMapper mDocModifiedSigMap;

    // gui
    QList<Document *> mDocs;
    CodeEditorBox *mCurrentEditorBox;
    MultiSplitter *mSplitter;
    QIcon mDocModifiedIcon;
    CodeEditorBox *defaultBox;

    Main *mMain;
    QList<MultiSplitter *> editorList;
    QList<ToolBox *> toolBoxList;
    QList<CmdLine *> cmdLineList;
    QList<TextFindReplacePanel *> replaceToolList;
    QList<GoToLineTool *> GoToLineToolList;
    bool tempMode;
    bool sessionChanged;
    QVariantList originalDocumentsList;
    bool windowIsDeleted = false;

    History mEditorHistory;
};
    
    class EditorWindow : public QWidget
    {
    public:
        EditorWindow(QWidget * parent = 0):
        QWidget(parent)
        { }
        
        void closeEvent(QCloseEvent *event)
        {
            MultiEditor *editor = MainWindow::instance()->editor();
            editor->removeWindow();
        }
    };
    
    class EditorSelectPopUp : public QDialog
    {
    public:
        EditorSelectPopUp(MultiEditor::History & history, QWidget * parent):
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
        
        CodeEditorBox * exec( const QPoint & pos )
        {
            move(pos);
            if (QDialog::exec())
                return currentSplit();
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
                case Qt::Key_E:
                    cycleDown();
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
        
        CodeEditorBox * currentSplit()
        {
            QStandardItem * currentItem = mModel->itemFromIndex(mListView->currentIndex());
            return currentItem ? currentItem->data().value<CodeEditorBox*>()
            : NULL;
        }
        
        void populateModel( MultiEditor::History & history );
        
        QListView *mListView;
        QStandardItemModel *mModel;
    };

} // namespace ScIDE

#endif // SCIDE_WIDGETS_MULTI_EDITOR_HPP_INCLUDED
