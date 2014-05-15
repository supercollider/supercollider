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

#ifndef SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED
#define SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED

#include <QSplitter>
#include <QWidget>

#include "../editor_box.hpp"
#include "../cmd_line.hpp"
#include "../find_replace_tool.hpp"
#include "../goto_line_tool.hpp"
#include "../tool_box.hpp"
#include "../main_window.hpp"
#include "../multi_editor.hpp"

namespace ScIDE {

class CodeEditorBox;
class EditorWindow;
class CmdLine;
class ToolBox;
class CmdLine;
class TextFindReplacePanel;
class GoToLineTool;
class MainWindow;

class MultiSplitter : public QSplitter
{
    Q_OBJECT
    
public:
    explicit MultiSplitter(EditorWindow *window, QWidget *parent = 0);

    EditorWindow *returnWindow() { return mWindow; }

    ToolBox *toolBox();
    CmdLine *cmdLine();
    TextFindReplacePanel *findReplaceTool();
    GoToLineTool *goToLineTool();

    void addSplit(CodeEditorBox *split);

    void removeSplit(CodeEditorBox *split);

    QList<CodeEditorBox*> splits();

    void insertWidget( QWidget *widget, QWidget *neighbour, Qt::Orientation direction );

    void removeWidget( QWidget *widget );

    template<typename T> T* findChild()
    {
        return MultiSplitter::findChild<T>(this);
    }
    
private slots:
    void hideToolBox();

private:
    template<typename T> static T* findChild( QSplitter * splitter ) {
        int childCount = splitter->count();
        for (int idx = 0; idx < childCount; ++idx) {
            QWidget *child = splitter->widget(idx);
            T* typedChild = qobject_cast<T*>(child);
            if (typedChild)
                return typedChild;
            QSplitter *childSplitter = qobject_cast<QSplitter*>(child);
            if (childSplitter) {
                typedChild = findChild<T>( childSplitter );
                if (typedChild)
                    return typedChild;
            }
        }
        return 0;
    }

    QSplitter *parentSplitterOf( QWidget *widget );

    void setEqualSizes(QSplitter *splitter);

    bool widgetIsChild( QWidget *widget );

    EditorWindow *mWindow;
    QList<CodeEditorBox *> splitList;
    ToolBox * mToolBox;
    CmdLine * mCmdLine;
    TextFindReplacePanel * mReplaceTool;
    GoToLineTool * mGoToLineTool;
};

} // namespace ScIDE

#endif // SCIDE_WIDGETS_UTIL_MULTI_SPLITTER_HPP_INCLUDED
