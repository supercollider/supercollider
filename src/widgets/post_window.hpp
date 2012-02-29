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

#ifndef SCIDE_WIDGETS_POST_WINDOW_HPP_INCLUDED
#define SCIDE_WIDGETS_POST_WINDOW_HPP_INCLUDED

#include <QAction>
#include <QDockWidget>
#include <QPointer>
#include <QTextBrowser>

namespace ScIDE {

class PostWindow:
    public QTextBrowser
{
    Q_OBJECT

public:
    explicit PostWindow(QWidget* parent = 0):
        QTextBrowser(parent)
    {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        mClearAction = new QAction(tr("Clear Post Window"), this);
        connect(mClearAction, SIGNAL(triggered()), this, SLOT(clear()));
        addAction(mClearAction);
        setContextMenuPolicy(Qt::ActionsContextMenu);
    }

    QAction * mClearAction;
};


/* FIXME: undocking the widget when maximized, the size of the dock window cannot be changed
 *
 * */
class PostDock:
    public QDockWidget
{
    Q_OBJECT

public:
    explicit PostDock(QWidget* parent = 0):
        QDockWidget(tr("Post Window"), parent)
    {
        setAllowedAreas(Qt::BottomDockWidgetArea);
        mPostWindow = new PostWindow(this);
        setWidget(mPostWindow);

        setFeatures(DockWidgetFloatable | DockWidgetMovable);
    }

    PostWindow * mPostWindow;
};

}

#endif
