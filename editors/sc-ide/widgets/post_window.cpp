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

#include "post_window.hpp"
#include "../core/main.hpp"

#include <QPointer>
#include <QToolBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QShortcut>

namespace ScIDE {

PostWindow::PostWindow(QWidget* parent):
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setMaximumBlockCount(500);

    QFont f( font() );
    f.setFamily("monospace");
    f.setStyleHint(QFont::TypeWriter);
    setFont(f);

    QAction * action;

    QAction *copyAction = new QAction(tr("Copy"), this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    copyAction->setShortcut( Main::instance()->settings()->shortcut("IDE/shortcuts/copy") );
    addAction(copyAction);

    mClearAction = new QAction(tr("Clear"), this);
    connect(mClearAction, SIGNAL(triggered()), this, SLOT(clear()));
    addAction(mClearAction);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    action = new QAction(tr("Enlarge Post Font"), this);
    action->setIconText("+");
    action->setShortcut(tr("Ctrl++", "Enlarge Font"));
    action->setShortcutContext( Qt::WidgetShortcut );
    connect(action, SIGNAL(triggered()), this, SLOT(zoomIn()));
    addAction(action);

    action = new QAction(tr("Shink Post Font"), this);
    action->setIconText("-");
    action->setShortcut(tr("Ctrl+-", "Shrink Font"));
    action->setShortcutContext( Qt::WidgetShortcut );
    connect(action, SIGNAL(triggered()), this, SLOT(zoomOut()));
    addAction(action);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mAutoScrollAction = new QAction(tr("Auto Scroll"), this);
    mAutoScrollAction->setCheckable(true);
    addAction(mAutoScrollAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(this, SIGNAL(scrollToBottomRequest()),
            this, SLOT(scrollToBottom()), Qt::QueuedConnection);
    connect(mAutoScrollAction, SIGNAL(triggered()), this, SLOT(scrollToBottom()));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(onScrollChange()));
    connect(verticalScrollBar(), SIGNAL(rangeChanged(int, int)),
            this, SLOT(onScrollChange()));
}


void PostWindow::post(const QString &text)
{
    QScrollBar *scrollBar = verticalScrollBar();
    bool scroll = scrollBar->value() == scrollBar->maximum();

    QTextCursor c(document());
    c.movePosition(QTextCursor::End);
    c.insertBlock();
    c.insertText(text);

    if(scroll)
        emit(scrollToBottomRequest());
}

void PostWindow::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void PostWindow::onScrollChange()
{
    QScrollBar *scrollBar = verticalScrollBar();
    bool isAtBottom = scrollBar->value() == scrollBar->maximum();
    mAutoScrollAction->setChecked(isAtBottom);
    mAutoScrollAction->setEnabled(!isAtBottom);
}

void PostWindow::zoomIn(int steps)
{
    zoomFont(steps);
}

void PostWindow::zoomOut(int steps)
{
    zoomFont(-steps);
}

void PostWindow::zoomFont(int steps)
{
    QFont currentFont = font();
    qreal size = currentFont.pointSizeF();
    if( size != -1 )
        currentFont.setPointSizeF( qMax(1.0, size + steps) );
    else
        currentFont.setPixelSize( qMax(1, currentFont.pixelSize() + steps) );

    setFont( currentFont );
}

void PostWindow::wheelEvent( QWheelEvent * e )
{
    if (e->modifiers() == Qt::ControlModifier) {
        if (e->delta() > 0)
            zoomIn();
        else
            zoomOut();
        return;
    }

    QPlainTextEdit::wheelEvent(e);
}

PostDock::PostDock(QWidget* parent):
    QDockWidget(tr("Post Window"), parent)
{
    setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(DockWidgetFloatable | DockWidgetMovable);

    mPostWindow = new PostWindow(this);
    setWidget(mPostWindow);

    QWidget *titleBar = new QWidget();
    QToolBar *toolBar = new QToolBar();
    foreach(QAction *a, mPostWindow->actions())
        toolBar->addAction(a);
    QHBoxLayout *l = new QHBoxLayout();
    l->setContentsMargins(5,2,5,0);
    l->addWidget(new QLabel(windowTitle()), 1);
    l->addWidget(toolBar);
    titleBar->setLayout(l);
    setTitleBarWidget(titleBar);

    connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(onFloatingChanged(bool)));
}

void PostDock::onFloatingChanged(bool floating)
{
    // HACK: After undocking when main window maximized, the dock widget can not be
    // resized anymore. Apparently it has to do something with the fact that the dock
    // widget spans from edge to edge of the screen.
    // The issue is avoided by slightly shrinking the dock widget.
    if (floating)
        resize(size() - QSize(1,1));
}

} // namespace ScIDE
