/*
    SuperCollider QT IDE
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

#ifndef WIDGETS_POSTWINDOW_HPP_INCLUDED
#define WIDGETS_POSTWINDOW_HPP_INCLUDED

#include <QDockWidget>
#include <QTextBrowser>
#include <QPointer>

namespace ScIDE {

class PostWindow:
    public QTextBrowser
{
    Q_OBJECT

public:
    explicit PostWindow(QWidget* parent = 0):
        QTextBrowser(parent)
    {}

private:

};

class PostDock:
    public QDockWidget
{
Q_OBJECT

public:
    explicit PostDock(QWidget* parent = 0):
        QDockWidget(tr("Post Window"), parent)
    {
        setAllowedAreas(Qt::BottomDockWidgetArea);
        postWindow = new PostWindow(this);
        setWidget(postWindow);

        setFeatures(DockWidgetFloatable | DockWidgetMovable);
    }

private:
    PostWindow * postWindow;
};

}

#endif
