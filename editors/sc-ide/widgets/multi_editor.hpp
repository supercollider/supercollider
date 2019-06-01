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

#pragma once

#include <QWidget>
#include <QTabBar>
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

namespace ScIDE {

class CodeEditorBox;
class Document;
class DocumentManager;
class GenericCodeEditor;
class Main;
class MultiSplitter;
struct Session;
class SignalMultiplexer;

namespace Settings {
class Manager;
}

class EditorTabBar : public QTabBar {
    Q_OBJECT

public:
    explicit EditorTabBar(QWidget* parent = nullptr);

private slots:
    void onCloseTab();
    void onCloseOtherTabs();
    void onCloseTabsToTheRight();

private:
    void mousePressEvent(QMouseEvent* event) override final;
    void mouseDoubleClickEvent(QMouseEvent* event) override final;

    void showContextMenu(QMouseEvent* event);

    int mTabUnderCursor;
};

class MultiEditor : public QWidget {
    Q_OBJECT

public:
    enum ActionRole {
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
        DeleteWord,

        GotoPreviousBlock,
        GotoNextBlock,
        SelectEnclosingBlock,

        GotoPreviousRegion,
        GotoNextRegion,
        GotoPreviousEmptyLine,
        GotoNextEmptyLine,

        SelectRegion,

        // View
        DocClose,
        EnlargeFont,
        ShrinkFont,
        ResetFontSize,
        ShowWhitespace,
        ShowLinenumber,
        IndentWithSpaces,
        ShowAutocompleteHelp,

        NextDocument,
        PreviousDocument,
        SwitchDocument,

        SplitHorizontally,
        SplitVertically,
        RemoveCurrentSplit,
        RemoveAllSplits,

        // Language
        EvaluateCurrentDocument,
        EvaluateRegion,
        EvaluateLine,

        ActionRoleCount
    };

    MultiEditor(Main*, QWidget* parent = 0);

    int tabCount() { return mTabs->count(); }
    Document* documentForTab(int index);
    int tabForDocument(Document* doc);

    GenericCodeEditor* currentEditor();
    CodeEditorBox* currentBox() { return mCurrentEditorBox; }
    void split(Qt::Orientation direction);

    QAction* action(ActionRole role) { return mActions[role]; }

    void saveSession(Session*);
    void switchSession(Session*);

signals:
    void currentDocumentChanged(Document*);
    void splitViewActivated();
    void splitViewDeactivated();
    void updateDockletOrder(int, int);

public slots:

    void setCurrent(Document*);

    void showNextDocument();
    void showPreviousDocument();
    void switchDocument();

    void splitHorizontally() { split(Qt::Horizontal); }
    void splitVertically() { split(Qt::Vertical); }
    void removeCurrentSplit();
    void removeAllSplits();

    void setShowWhitespace(bool on);
    void setShowLinenumber(bool on);

    void setShowAutocompleteHelp(bool on);

    void updateTabsOrder(QList<Document*>);

private slots:
    void applySettings(Settings::Manager*);
    void onOpen(Document*, int initialCursorPosition, int selectionLength);
    void onClose(Document*);
    void show(Document*, int cursorPosition = -1, int selectionLenght = 0);
    void update(Document*);
    void onCloseRequest(int index);
    void onCurrentTabChanged(int index);
    void onCurrentEditorChanged(GenericCodeEditor*);
    void onBoxActivated(CodeEditorBox*);
    void onDocModified(QObject*);
    void updateDocOrder(int, int);

private:
    void makeSignalConnections();
    void breakSignalConnections();
    void createActions();
    void updateActions();

    /**
     * Put a new doc into the editor. If `doc` is already opened, this simply returns its current index.
     *
     * @param doc The doc to insert.
     * @param insertIndex Index at which to insert. If negative, the doc is inserted after the current tab.
     * @return The index of the tab.
     */
    int insertTab(Document* doc, int insertIndex = -1);
    CodeEditorBox* newBox(MultiSplitter*);
    void setCurrentBox(CodeEditorBox*);
    void setCurrentEditor(GenericCodeEditor*);
    void loadBoxState(CodeEditorBox* box, const QVariantList& data, const QList<Document*>& documentList);
    void loadSplitterState(MultiSplitter*, const QVariantMap& data, const QList<Document*>& documentList);
    void showEditorTabs(bool);
    void activateComboBoxWhenSplitting();
    void setMainComboBoxOption();

    QAction* mActions[ActionRoleCount];

    SignalMultiplexer* mEditorSigMux;
    SignalMultiplexer* mBoxSigMux;
    QSignalMapper mDocModifiedSigMap;

    // gui
    QTabBar* mTabs;
    CodeEditorBox* mCurrentEditorBox;
    MultiSplitter* mSplitter;
    QIcon mDocModifiedIcon;
    QVBoxLayout* multiEditorLayout;
};

} // namespace ScIDE
