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
#include "../core/util/overriding_action.hpp"

#include <QApplication>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QScrollBar>
#include <QShortcut>
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QMimeData>

namespace ScIDE {

PostWindow::PostWindow(QWidget* parent):
    QPlainTextEdit(parent)
{
    setReadOnly(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape( QFrame::NoFrame );

    viewport()->setAttribute( Qt::WA_MacNoClickThrough, true );

    QRect availableScreenRect = qApp->desktop()->availableGeometry(this);
    mSizeHint = QSize( availableScreenRect.width() * 0.4, availableScreenRect.height() * 0.3 );

    createActions( Main::settings() );

    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(this, SIGNAL(scrollToBottomRequest()),
            this, SLOT(scrollToBottom()), Qt::QueuedConnection);

    applySettings( Main::settings() );
}

void PostWindow::createActions( Settings::Manager * settings )
{
    QAction * action;
    OverridingAction * ovrAction;

    QString postCategory(tr("Post Window"));

    mActions[Copy] = action = new QAction(tr("Copy"), this);
    action->setShortcut( QKeySequence::Copy );
    action->setShortcutContext( Qt::WidgetShortcut );
    action->setEnabled( false );
    connect(action, SIGNAL(triggered()), this, SLOT(copy()));
    connect( this, SIGNAL(copyAvailable(bool)), action, SLOT(setEnabled(bool)) );
    addAction(action);

    mActions[Clear] = action = new QAction(tr("Clear"), this);
    action->setStatusTip(tr("Clear post window"));
    action->setShortcutContext(Qt::ApplicationShortcut);
    action->setShortcut(tr("Ctrl+Shift+P", "Clear post window"));
    settings->addAction( action, "post-clear", postCategory );
    connect(action, SIGNAL(triggered()), this, SLOT(clear()));
    addAction(action);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mActions[ZoomIn] = ovrAction = new OverridingAction(tr("Enlarge Font"), this);
    ovrAction->setIconText("+");
    ovrAction->setStatusTip(tr("Enlarge post window font"));
    connect(ovrAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
    ovrAction->addToWidget(this);

    mActions[ZoomOut] = ovrAction = new OverridingAction(tr("Shrink Font"), this);
    ovrAction->setIconText("-");
    ovrAction->setStatusTip(tr("Shrink post window font"));
    connect(ovrAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
    ovrAction->addToWidget(this);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mActions[LineWrap] = action = new QAction(tr("Wrap Text"), this);
    action->setStatusTip(tr("Wrap lines wider than the post window"));
    action->setCheckable(true);
    addAction(action);
    connect( action, SIGNAL(triggered(bool)), this, SLOT(setLineWrap(bool)) );
    settings->addAction( action, "post-line-wrap", postCategory );

    mActions[AutoScroll] = action = new QAction(tr("Auto Scroll"), this);
    action->setStatusTip(tr("Scroll to bottom on new posts"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(onAutoScrollTriggered(bool)));
    addAction(action);
    settings->addAction( action, "post-auto-scroll", postCategory );
}

void PostWindow::updateActionShortcuts( Settings::Manager * settings )
{
    settings->beginGroup("IDE/shortcuts");
    mActions[ZoomIn]->setShortcut( settings->shortcut("editor-enlarge-font") );
    mActions[ZoomOut]->setShortcut( settings->shortcut("editor-shrink-font") );
    settings->endGroup();
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

    updateActionShortcuts(settings);
}

void PostWindow::storeSettings( Settings::Manager * settings )
{
    settings->setValue("IDE/postWindow/lineWrap", mActions[LineWrap]->isChecked() );
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
    bool scroll = mActions[AutoScroll]->isChecked();

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

bool PostWindow::event( QEvent * event )
{
    switch (event->type()) {
    case QEvent::ShortcutOverride: {
        QKeyEvent *kevent = static_cast<QKeyEvent*>(event);
        if (kevent == QKeySequence::Copy) {
            event->accept();
            return true;
        }
        break;
    }
    default:
        break;
    }

    return QPlainTextEdit::event(event);
}

void PostWindow::wheelEvent( QWheelEvent * e )
{
    // FIXME: Disable zooming for now, to avoid nasty effect when Ctrl
    // is unintentionally pressed while inertial scrolling is going on.

    // Moreover, Ctrl|Shift + Wheel scrolls by pages, which is also
    // rather annoying.

    // So rather just forward the event without modifiers.

    QWheelEvent modifiedEvent( e->pos(), e->globalPos(), e->delta(),
                               e->buttons(), 0, e->orientation() );
    QPlainTextEdit::wheelEvent( &modifiedEvent );
    return;

#if 0
    if (e->modifiers() == Qt::ControlModifier) {
        if (e->delta() > 0)
            zoomIn();
        else
            zoomOut();
        return;
    }

    QPlainTextEdit::wheelEvent(e);
#endif
}

void PostWindow::focusOutEvent( QFocusEvent * event )
{
    if (event->reason() == Qt::TabFocusReason)
        MainWindow::instance()->focusCodeEditor();
    else
        QPlainTextEdit::focusOutEvent(event);
}

void PostWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    QPlainTextEdit::mouseDoubleClickEvent(e);
    extendSelectionForEnvVar(this, textCursor());

    cursor.endEditBlock();
}

QMimeData *PostWindow::createMimeDataFromSelection() const
{
    // Here, we bundle up just the plaintext (not HTML, as is the default) of
    // the post window's selected contents.
    QMimeData *data = new QMimeData;
    data->setText(textCursor().selection().toPlainText());
    return data;
}

bool PostWindow::openDocumentation()
{
    return Main::openDocumentation(symbolUnderCursor());
}

void PostWindow::openDefinition()
{
    Main::openDefinition(symbolUnderCursor(), window());
}

void PostWindow::findReferences()
{
    Main::findReferences(symbolUnderCursor(), window());
}

void PostWindow::setLineWrap(bool lineWrapOn)
{
    setLineWrapMode( lineWrapOn ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap );
    mActions[LineWrap]->setChecked(lineWrapOn);
    Main::settings()->setValue( "IDE/postWindow/lineWrap", lineWrapOn );
}

PostDocklet::PostDocklet(QWidget* parent):
    Docklet(tr("Post window"), parent)
{
    setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

    mPostWindow = new PostWindow;
    setWidget(mPostWindow);

    toolBar()->addAction( mPostWindow->mActions[PostWindow::AutoScroll] );

    //connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(onFloatingChanged(bool)));
}

void PostDocklet::onFloatingChanged(bool floating)
{
    // HACK: After undocking when main window maximized, the dock widget can not be
    // resized anymore. Apparently it has to do something with the fact that the dock
    // widget spans from edge to edge of the screen.
    // The issue is avoided by slightly shrinking the dock widget.
    if (floating)
        dockWidget()->resize(dockWidget()->size() - QSize(1,1));
}

} // namespace ScIDE
