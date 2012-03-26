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

#include <QTabWidget>
#include <QAction>
#include <QSignalMapper>
#include <QSettings>

namespace ScIDE {

class Main;
class Document;
class DocumentManager;
class CodeEditor;
class SignalMultiplexer;

class MultiEditor : public QTabWidget
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
        IndentMore,
        IndentLess,

        // View
        EnlargeFont,
        ShrinkFont,
        ShowWhitespace,

        ActionRoleCount
    };

    MultiEditor( Main *, QWidget * parent = 0 );

    CodeEditor *currentEditor()
        { return editorForTab( currentIndex() ); }

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
    void applySettings( QSettings * );

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

    Main *mMain;
    DocumentManager * mDocManager;
    SignalMultiplexer * mSigMux;
    QSignalMapper mModificationMapper;
    QAction *mActions[ActionRoleCount];

    // settings
    bool mStepForwardEvaluation;
};


} // namespace ScIDE

#endif // SCIDE_WIDGETS_MULTI_EDITOR_HPP_INCLUDED
