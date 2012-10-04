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
#include <QPlainTextEdit>

namespace ScIDE {

namespace Settings { class Manager; }

class PostDock;

class PostWindow:
    public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit PostWindow(QWidget* parent = 0);

    void applySettings( Settings::Manager * );
    void storeSettings( Settings::Manager * );

    QSize sizeHint() const { return mSizeHint; }
    QSize minimumSizeHint() const { return QSize(50,50); }
    QString symbolUnderCursor();

signals:
    void scrollToBottomRequest();

public slots:
    void post(const QString &text);
    void scrollToBottom();
    void zoomIn(int steps = 1);
    void zoomOut(int steps = 1);

    bool openDocumentation();
    void openDefinition();
    void findReferences();

private slots:
    void onAutoScrollTriggered(bool);
    void setLineWrap(bool on);

private:
    friend class PostDock;

    void zoomFont(int steps);
    void wheelEvent( QWheelEvent * );
    void focusOutEvent (QFocusEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

    QAction * mAutoScrollAction;
    QAction * mClearAction;
    QAction * mLineWrapAction;
    QSize mSizeHint;
};


class PostDock:
    public QDockWidget
{
    Q_OBJECT

public:
    PostDock(QWidget* parent = 0);

private slots:
    void onFloatingChanged(bool floating);

public:
    PostWindow * mPostWindow;
};

} // namespace ScIDE

#endif
