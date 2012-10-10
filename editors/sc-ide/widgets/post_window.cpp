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
#include "util/gui_utilities.hpp"
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
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QRect availableScreenRect = qApp->desktop()->availableGeometry(this);
    mSizeHint = QSize( availableScreenRect.width() * 0.4, availableScreenRect.height() * 0.3 );

    QAction * action;

    QAction *copyAction = new QAction(tr("Copy"), this);
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    copyAction->setShortcut( Main::settings()->shortcut("IDE/shortcuts/editor-copy") );
    copyAction->setShortcutContext( Qt::WidgetShortcut );
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
    action->setShortcutContext( Qt::WidgetShortcut );
    action->setToolTip(tr("Shrink font"));
    connect(action, SIGNAL(triggered()), this, SLOT(zoomOut()));
    addAction(action);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mLineWrapAction = action = new QAction(tr("Wrap Text"), this);
    action->setCheckable(true);
    addAction(action);
    connect( action, SIGNAL(triggered(bool)), this, SLOT(setLineWrap(bool)) );

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
    int scrollback = settings->value("IDE/postWindow/scrollback").toInt();

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

    bool lineWrap = settings->value("IDE/postWindow/lineWrap").toBool();

    setMaximumBlockCount(scrollback);
    setFont(font);
    setPalette(palette);
    setLineWrap( lineWrap );

    QFontMetrics metrics (font);
    QString stringOfSpaces (settings->value("IDE/editor/indentWidth").toInt(), QChar(' '));
    setTabStopWidth(metrics.width(stringOfSpaces));
}

void PostWindow::storeSettings( Settings::Manager * settings )
{
    settings->setValue("IDE/postWindow/lineWrap", mLineWrapAction->isChecked() );
}

QString PostWindow::symbolUnderCursor()
{
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
        return cursor.selectedText();
    else
    {
        QString blockString = cursor.block().text();
        int position = cursor.positionInBlock();
        return wordInStringAt( position, blockString );
    }
}

void PostWindow::post(const QString &text)
{
    QScrollBar *scrollBar = verticalScrollBar();
    bool scroll = mAutoScrollAction->isChecked();

    QTextCursor c(document());
    c.movePosition(QTextCursor::End);
    c.insertText(text);

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

void PostWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    QPlainTextEdit::mouseDoubleClickEvent(e);
    extendSelectionForEnvVar(this, textCursor());

    cursor.endEditBlock();
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

void PostWindow::setLineWrap(bool lineWrapOn)
{
    setLineWrapMode( lineWrapOn ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap );
    mLineWrapAction->setChecked(lineWrapOn);
    Main::settings()->setValue( "IDE/postWindow/lineWrap", lineWrapOn );
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
