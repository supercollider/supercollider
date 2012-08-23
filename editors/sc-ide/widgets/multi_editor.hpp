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

#include <QWidget>
#include <QTabWidget>
#include <QAction>
#include <QSignalMapper>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QTextDocument>

namespace ScIDE {

class Main;
class Document;
class DocumentManager;
class CodeEditor;
class SignalMultiplexer;
class ScRequest;

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

        // View
        EnlargeFont,
        ShrinkFont,
        ResetFontSize,
        ShowWhitespace,

        NextDocument,
        PreviousDocument,
        SwitchDocument,

        // Language
        EvaluateCurrentDocument,
        EvaluateRegion,
        EvaluateLine,

        OpenDefinition,

        ActionRoleCount
    };

    MultiEditor( Main *, QWidget * parent = 0 );

    int editorCount() { return mTabs->count(); }

    CodeEditor *editor( int index ) { return editorForTab(index); }

    CodeEditor *currentEditor()
        { return editorForTab( mTabs->currentIndex() ); }

    QAction * action( ActionRole role )
        { return mActions[role]; }

    bool stepForwardEvaluation() { return mStepForwardEvaluation; }

    void openDefinition(QString const & string);
    bool openDocumentation(QString const & string);

Q_SIGNALS:
    void currentChanged( Document * );

public Q_SLOTS:

    void setCurrent( Document * );

    void applySettings( Settings::Manager * );

    void showNextDocument();
    void showPreviousDocument();
    void switchDocument();

    bool openDocumentation();

private Q_SLOTS:

    void onOpen( Document *, int initialCursorPosition );
    void onClose( Document * );
    void show( Document *, int cursorPosition = -1 );
    void update( Document * );
    void onCloseRequest( int index );
    void onCurrentChanged( int index );
    void onModificationChanged( QWidget * );
    void openDefinition();
    void evaluateRegion();
    void evaluateLine();
    void evaluateDocument();

private:
    void createActions();
    void updateActions();
    CodeEditor * editorForTab( int index );
    CodeEditor * editorForDocument( Document * );

    DocumentManager * mDocManager;
    SignalMultiplexer * mSigMux;
    QSignalMapper mModificationMapper;
    QAction *mActions[ActionRoleCount];

    // gui
    QTabWidget *mTabs;
    QIcon mDocModifiedIcon;

    // settings
    bool mStepForwardEvaluation;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_MULTI_EDITOR_HPP_INCLUDED
