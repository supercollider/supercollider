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

#include "main_window.hpp"
#include "post_window.hpp"
#include "../core/main.hpp"
#include "../core/settings/manager.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QScrollBar>
#include <QShortcut>
#include <QToolBar>

namespace ScIDE {

PostWindow::PostWindow(QWidget* parent):
    QPlainTextEdit(parent),
    mNewlinePending(false)
{
    setReadOnly(true);
    setLineWrapMode(QPlainTextEdit::NoWrap);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QRect availableScreenRect = qApp->desktop()->availableGeometry(this);
    mSizeHint = QSize( availableScreenRect.width() / 2.7, 300 );

    QAction * action;

    QAction *copyAction = new QAction(tr("Copy"), this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    copyAction->setShortcut( Main::settings()->shortcut("IDE/shortcuts/copy") );
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
    action->setToolTip(tr("Enlarge font"));
    connect(action, SIGNAL(triggered()), this, SLOT(zoomIn()));
    addAction(action);

    action = new QAction(tr("Shrink Post Font"), this);
    action->setIconText("-");
    action->setShortcut(tr("Ctrl+-", "Shrink Font"));
    action->setToolTip(tr("Shrink font"));
    action->setShortcutContext( Qt::WidgetShortcut );
    connect(action, SIGNAL(triggered()), this, SLOT(zoomOut()));
    addAction(action);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mAutoScrollAction = new QAction(tr("Auto Scroll"), this);
    mAutoScrollAction->setToolTip(tr("Scroll to bottom on new posts"));
    mAutoScrollAction->setCheckable(true);
    mAutoScrollAction->setChecked(true);
    addAction(mAutoScrollAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(this, SIGNAL(scrollToBottomRequest()),
            this, SLOT(scrollToBottom()), Qt::QueuedConnection);
    connect(mAutoScrollAction, SIGNAL(triggered(bool)),
            this, SLOT(onAutoScrollTriggered(bool)));

    applySettings( Main::settings() );
}

void PostWindow::applySettings(Settings::Manager * settings)
{
    int scrollback = settings->value("IDE/editor/postWindowScrollback").toInt();

    QFont font = settings->codeFont();

    QPalette palette;
    settings->beginGroup("IDE/editor/colors");
    if (settings->contains("text")) {
        QTextCharFormat format = settings->value("text").value<QTextCharFormat>();
        QBrush bg = format.background();
        QBrush fg = format.foreground();
        if (bg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Base, bg);
        if (fg.style() != Qt::NoBrush)
            palette.setBrush(QPalette::Text, fg);
    }
    settings->endGroup(); // colors

    setMaximumBlockCount(scrollback);
    setFont(font);
    setPalette(palette);
}

QString PostWindow::symbolUnderCursor()
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}

void PostWindow::post(const QString &text)
{
    QScrollBar *scrollBar = verticalScrollBar();
    bool scroll = mAutoScrollAction->isChecked();

    QTextCursor c(document());
    c.movePosition(QTextCursor::End);

    if (mNewlinePending) {
        c.insertBlock();
        mNewlinePending = false;
        c.movePosition(QTextCursor::End);
    }

    if ( text.endsWith("\n") ) {
        QString textToInsert (text);
        textToInsert.chop(1);
        mNewlinePending = true;
        c.insertText(textToInsert);
    } else {
        c.insertText(text);
    }

    if (scroll)
        emit(scrollToBottomRequest());
}

void PostWindow::scrollToBottom()
{
    verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void PostWindow::onAutoScrollTriggered(bool on)
{
    if (on)
        scrollToBottom();
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
    const int newSize = currentFont.pointSize() + steps;
    if (newSize <= 0)
        return;
    currentFont.setPointSize(newSize);
    setFont(currentFont);
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

void PostWindow::focusOutEvent( QFocusEvent * event )
{
    MainWindow::instance()->focusCodeEditor();
    event->accept();
}

bool PostWindow::openDocumentation()
{
    return Main::openDocumentation(symbolUnderCursor());
}

void PostWindow::openDefinition()
{
    Main::openDefinition(symbolUnderCursor(), MainWindow::instance());
}

void PostWindow::findReferences()
{
    Main::findReferences(symbolUnderCursor(), MainWindow::instance());
}


PostDock::PostDock(QWidget* parent):
    QDockWidget(tr("Post window"), parent)
{
    setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    setFeatures(DockWidgetFloatable | DockWidgetMovable | DockWidgetClosable);

    mPostWindow = new PostWindow(this);
    setWidget(mPostWindow);

    QToolBar *toolBar = new QToolBar();
    toolBar->addAction(mPostWindow->mAutoScrollAction);

    QWidget *titleBar = new QWidget();
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
