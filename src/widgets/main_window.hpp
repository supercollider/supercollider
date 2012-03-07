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

#include <QMainWindow>

namespace ScIDE
{

class Main;
class MultiEditor;
class PostDock;

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

        // Language
        EvaluateCurrentFile,
        EvaluateSelectedRegion,
        StepForwardOnEvaluation,

        ActionCount
    };

    MainWindow(Main *);

    QAction *action( ActionRole );

public Q_SLOTS:
    void newDocument();
    void openDocument();
    void saveDocument();
    void saveDocumentAs();
    void closeDocument();
    void toggleComandLineFocus();

private Q_SLOTS:
    void evaluateCurrentRegion(); // either selection, or current line
    void evaluateCurrentFile();

private:
    void createMenus();
    QWidget *cmdLine();

    Main *mMain;

    QAction * mActions[ActionCount];

    MultiEditor *mEditors;

    PostDock * mPostDock;
};

} // namespace ScIDE

#endif
