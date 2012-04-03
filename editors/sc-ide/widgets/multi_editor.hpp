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

namespace ScIDE {

class Main;
class Document;
class DocumentManager;
class CodeEditor;
class SignalMultiplexer;

namespace Settings { class Manager; }

class TextFindReplacePanel;

class MultiEditor : public QWidget
{
    Q_OBJECT

public:

    enum ActionRole {
        // File
        DocNew = 0,
        DocOpen,
        DocSave,
        DocSaveAs,
        DocClose,

        // Edit
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,
        Find,
        Replace,
        IndentMore,
        IndentLess,

        // View
        EnlargeFont,
        ShrinkFont,
        ShowWhitespace,

        ActionRoleCount
    };

    MultiEditor( Main *, QWidget * parent = 0 );

    int editorCount() { return mTabs->count(); }

    CodeEditor *currentEditor()
        { return editorForTab( mTabs->currentIndex() ); }

    QAction * action( ActionRole role )
        { return mActions[role]; }

    bool stepForwardEvaluation() { return mStepForwardEvaluation; }

Q_SIGNALS:
    void currentChanged( Document * );

public Q_SLOTS:

    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void closeDocument();
    void setCurrent( Document * );

    void showFindPanel();
    void showReplacePanel();
    void hideToolPanel();

    void applySettings( Settings::Manager * );

private Q_SLOTS:

    void onOpen( Document * );
    void onClose( Document * );
    void update( Document * );
    void onCloseRequest( int index );
    void onCurrentChanged( int index );
    void onModificationChanged( QWidget * );

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
    TextFindReplacePanel *mFindReplacePanel;

    // settings
    bool mStepForwardEvaluation;
};

class TextFindReplacePanel : public QWidget
{
    Q_OBJECT

public:
    enum Mode
    {
        Find = 1,
        Replace
    };

public:
    TextFindReplacePanel( QWidget * parent = 0 );

    void setEditor( CodeEditor *editor ) { mEditor = editor; }

    Mode mode () const { return mMode; }
    void setMode( Mode );

    QString findString() const { return mFindField->text(); }
    QString replaceString() const { return mReplaceField->text(); }
    bool matchCase() const { return mMatchCaseOpt->isChecked(); }

public Q_SLOTS:
    void findNext();
    void findPrevious();
    void findAll();
    void replace();
    void replaceAll();

Q_SIGNALS:
    void close();

private Q_SLOTS:
    void onFindFieldReturn();

private:
    void find (bool backwards);

    QToolButton *mCloseBtn;
    QLineEdit *mFindField;
    QLabel *mFindLabel;
    QLineEdit *mReplaceField;
    QLabel *mReplaceLabel;
    QPushButton *mNextBtn;
    QPushButton *mPrevBtn;
    QPushButton *mFindAllBtn;
    QPushButton *mReplaceBtn;
    QPushButton *mReplaceAllBtn;
    QCheckBox *mMatchCaseOpt;

    QGridLayout *mGrid;

    Mode mMode;

    CodeEditor *mEditor;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_MULTI_EDITOR_HPP_INCLUDED
