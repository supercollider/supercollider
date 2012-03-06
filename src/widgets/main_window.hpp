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

#ifndef SCIDE_WIDGETS_MAIN_WINDOW_HPP_INCLUDED
#define SCIDE_WIDGETS_MAIN_WINDOW_HPP_INCLUDED

#include "post_window.hpp"
#include "../main.hpp"
#include "../doc_manager.hpp"
#include "../sig_mux.hpp"

#include <QMainWindow>
#include <QTabWidget>
#include <QTabWidget>
#include <QVector>

namespace ScIDE
{

class MainWindow : public QMainWindow
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
        Quit,

        // Edit
        Undo,
        Redo,
        Cut,
        Copy,
        Paste,

        // View
        IncreaseFontSize,
        DecreaseFontSize,

        // Language
        EvaluateCurrentFile,
        EvaluateSelectedRegion,
        StepForwardOnEvaluation,

        ActionCount
    };

    MainWindow(struct Main *);

    QAction *action( ActionRole );

public Q_SLOTS:
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void closeDocument();

private Q_SLOTS:
    void createTab( Document * );
    void updateTab( Document * );
    void closeTab(int index);

    void onCurrentEditorChanged(int);

    void evaluateCurrentRegion(); // either selection, or current line
    void evaluateCurrentFile();

private:
    void createMenus();
    class CodeEditor * codeEditorForTab( int index );
    class CodeEditor * currentCodeEditor()
        { return codeEditorForTab( mDocTabs->currentIndex() ); }
    QString tabTitle( Document * );
    QWidget *cmdLine();

    Main *mMain;

    QAction * mActions[ActionCount];

    QTabWidget *mDocTabs;
    SignalMultiplexer * mDocSigMux;

    PostDock * mPostDock;
};

} // namespace ScIDE

#endif
