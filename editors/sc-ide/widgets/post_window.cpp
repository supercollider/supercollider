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
#include "../core/settings/theme.hpp"
#include "../core/util/overriding_action.hpp"
#include "editor.hpp"

#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QPointer>
#include <QScrollBar>
#include <QShortcut>
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <QMimeData>
#include <qtextcursor.h>
#include <qtextformat.h>
#include <qurl.h>

namespace ScIDE {

PostWindow::PostWindow(QWidget* parent): QPlainTextEdit(parent) {
    setReadOnly(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);
    previousChar = QChar('\n');

    QRect availableScreenRect = this->screen()->availableGeometry();
    mSizeHint = QSize(availableScreenRect.width() * 0.4, availableScreenRect.height() * 0.3);

    createActions(Main::settings());

    setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(this, &PostWindow::scrollToBottomRequest, this, &PostWindow::scrollToBottom, Qt::QueuedConnection);

    grabGesture(Qt::PinchGesture);
    setAttribute(Qt::WA_AcceptTouchEvents);

    applySettings(Main::settings());
}

void PostWindow::createActions(Settings::Manager* settings) {
    QAction* action;
    OverridingAction* ovrAction;

    QString postCategory(tr("Post Window"));

    mActions[Copy] = action = new QAction(tr("Copy"), this);
    action->setShortcut(QKeySequence::Copy);
    action->setShortcutContext(Qt::WidgetShortcut);
    action->setEnabled(false);
    connect(action, &QAction::triggered, this, &PostWindow::copy);
    connect(this, &PostWindow::copyAvailable, action, &QAction::setEnabled);
    addAction(action);

    mActions[Clear] = action = new QAction(tr("Clear"), this);
    action->setStatusTip(tr("Clear post window"));
    action->setShortcutContext(Qt::ApplicationShortcut);
    action->setShortcut(tr("Ctrl+Shift+P", "Clear post window"));
    settings->addAction(action, "post-clear", postCategory);
    connect(action, &QAction::triggered, this, &PostWindow::clear);
    addAction(action);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mActions[DocClose] = ovrAction = new OverridingAction(tr("Close"), this);
    action->setStatusTip(tr("Close the current document"));
    connect(ovrAction, &QAction::triggered, this, &PostWindow::closeDocument);
    ovrAction->addToWidget(this);

    mActions[ZoomIn] = ovrAction = new OverridingAction(tr("Enlarge Font"), this);
    ovrAction->setIconText("+");
    ovrAction->setStatusTip(tr("Enlarge post window font"));
    // Use lambda for zoomIn as the default parameter is not preserved when using pointer to member function
    connect(ovrAction, &QAction::triggered, this, [this]() { zoomIn(); });
    ovrAction->addToWidget(this);

    mActions[ZoomOut] = ovrAction = new OverridingAction(tr("Shrink Font"), this);
    ovrAction->setIconText("-");
    ovrAction->setStatusTip(tr("Shrink post window font"));
    // Use lambda for zoomOut as the default parameter is not preserved when using pointer to member function
    connect(ovrAction, &QAction::triggered, this, [this]() { zoomOut(); });
    ovrAction->addToWidget(this);

    mActions[ResetZoom] = ovrAction = new OverridingAction(tr("Reset Font Size"), this);
    connect(ovrAction, &QAction::triggered, this, &PostWindow::resetZoom);
    ovrAction->addToWidget(this);

    action = new QAction(this);
    action->setSeparator(true);
    addAction(action);

    mActions[LineWrap] = action = new QAction(tr("Wrap Text"), this);
    action->setStatusTip(tr("Wrap lines wider than the post window"));
    action->setCheckable(true);
    addAction(action);
    connect(action, &QAction::triggered, this, &PostWindow::setLineWrap);
    settings->addAction(action, "post-line-wrap", postCategory);

    mActions[AutoScroll] = action = new QAction(tr("Auto Scroll"), this);
    action->setStatusTip(tr("Scroll to bottom on new posts"));
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, &QAction::triggered, this, &PostWindow::onAutoScrollTriggered);
    addAction(action);
    settings->addAction(action, "post-auto-scroll", postCategory);
}

void PostWindow::updateActionShortcuts(Settings::Manager* settings) {
    settings->beginGroup("IDE/shortcuts");
    mActions[DocClose]->setShortcut(settings->shortcut("ide-document-close"));
    mActions[ZoomIn]->setShortcut(settings->shortcut("editor-enlarge-font"));
    mActions[ZoomOut]->setShortcut(settings->shortcut("editor-shrink-font"));
    mActions[ResetZoom]->setShortcut(settings->shortcut("editor-reset-font-size"));
    settings->endGroup();
}

void PostWindow::closeDocument() { MainWindow::instance()->postDocklet()->close(); }

void PostWindow::applySettings(Settings::Manager* settings) {
    int scrollback = settings->value("IDE/postWindow/scrollback").toInt();

    QFont font = settings->codeFont();

    QPalette palette;
    QTextCharFormat format;
    format.merge(settings->getThemeVal("text"));
    format.merge(settings->getThemeVal("postwindowtext"));

    QBrush bg = format.background();
    QBrush fg = format.foreground();
    if (bg.style() != Qt::NoBrush)
        palette.setBrush(QPalette::Base, bg);
    if (fg.style() != Qt::NoBrush)
        palette.setBrush(QPalette::Text, fg);

    bool lineWrap = settings->value("IDE/postWindow/lineWrap").toBool();

    setMaximumBlockCount(scrollback);
    setFont(font);
    setPalette(palette);
    setLineWrap(lineWrap);

    QFontMetrics metrics(font);
    QString stringOfSpaces(settings->value("IDE/editor/indentWidth").toInt(), QChar(' '));

    setTabStopDistance(metrics.horizontalAdvance(stringOfSpaces));

    updateActionShortcuts(settings);
}

void PostWindow::storeSettings(Settings::Manager* settings) {
    settings->setValue("IDE/postWindow/lineWrap", mActions[LineWrap]->isChecked());
}

QString PostWindow::symbolUnderCursor() {
    QTextCursor cursor = textCursor();
    if (cursor.hasSelection())
        return cursor.selectedText();
    else {
        QString blockString = cursor.block().text();
        int position = cursor.positionInBlock();
        return tokenInStringAt(position, blockString);
    }
}

void PostWindow::post(const QString& text) {
    const bool scroll = mActions[AutoScroll]->isChecked();
    QTextCursor cursor(document());

    if (text == "\n") {
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(text, currentFormat);

        if (scroll)
            emit(scrollToBottomRequest());
        return;
    }

    const auto ends_with_new_line = text.back() == '\n';
    const auto lines = text.split("\n");
    const auto line_count = lines.size();
    for (size_t i { 0 }; i < line_count; ++i) {
        const auto line = lines[i];
        const auto line_format = formatForPostLine(line);
        cursor.movePosition(QTextCursor::End);

        // If there is some text that looks like a URI and contains '://' then turn it into a html anchor so we can
        // click it.
        if (!line.contains("://")) {
            cursor.insertText(line, line_format);
        } else {
            // words are just text separated by spaces.
            const auto words = line.split(" ");
            const auto words_count = words.size();
            for (size_t w { 0 }; w < words_count; ++w) {
                const auto& word = words[w];
                cursor.movePosition(QTextCursor::End);

                if (const auto maybe_url = QUrl(word, QUrl::ParsingMode::StrictMode);
                    maybe_url.isValid() && word.contains("://")) {
                    cursor.insertHtml(QString("<a href='") + word + QString("'>") + word + QString("</a>"));
                } else {
                    cursor.insertText(word, line_format);
                }

                // Put space back in, if not last word.
                if (w + 1 != words_count)
                    cursor.insertText(" ", line_format);
            }
        }

        // Don't write a new line in the final case.
        if (i + 1 != line_count)
            cursor.insertText("\n", line_format);
    }

    if (scroll)
        emit(scrollToBottomRequest());
}

QTextCharFormat PostWindow::formatForPostLine(QString line) {
    Settings::Manager* settings = Main::settings();
    QTextCharFormat postWindowError = settings->getThemeVal("postwindowerror");
    QTextCharFormat postWindowWarning = settings->getThemeVal("postwindowwarning");
    QTextCharFormat postWindowSuccess = settings->getThemeVal("postwindowsuccess");
    QTextCharFormat postWindowEmphasis = settings->getThemeVal("postwindowemphasis");

    QTextCharFormat format;

    if (line.startsWith("ERROR:", Qt::CaseInsensitive) || line.startsWith("!"))
        format.merge(postWindowError);
    else if (line.startsWith("WARNING:", Qt::CaseInsensitive) || line.startsWith("?"))
        format.merge(postWindowWarning);
    else if (line.startsWith("->"))
        format.merge(postWindowSuccess);
    else if (line.startsWith("***"))
        format.merge(postWindowEmphasis);
    // else no format

    return format;
}

void PostWindow::scrollToBottom() { verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum); }

void PostWindow::onAutoScrollTriggered(bool on) {
    if (on)
        scrollToBottom();
}

void PostWindow::zoomIn(int steps) { zoomFont(steps); }

void PostWindow::zoomOut(int steps) { zoomFont(-steps); }

void PostWindow::zoomFont(int steps) {
    QFont currentFont = font();
    const float newSize = GenericCodeEditor::clampFontSize(currentFont.pointSizeF() + steps);
    currentFont.setPointSizeF(newSize);
    setFont(currentFont);
}

void PostWindow::zoomFont(float scaler) {
    QFont currentFont = font();
    const float newSize = GenericCodeEditor::clampFontSize(currentFont.pointSizeF() * scaler);
    currentFont.setPointSizeF(newSize);
    setFont(currentFont);
}

void PostWindow::resetZoom() {
    QFont font = this->font();
    QFont defaultFont = Main::settings()->codeFont();
    font.setPointSize(defaultFont.pointSize());
    setFont(font);
}

bool PostWindow::event(QEvent* event) {
    switch (event->type()) {
    case QEvent::ShortcutOverride: {
        QKeyEvent* kevent = static_cast<QKeyEvent*>(event);
        if (kevent == QKeySequence::Copy) {
            event->accept();
            return true;
        }
        break;
    }
    case QEvent::Gesture: {
        return gestureEvent(static_cast<QGestureEvent*>(event));
        break;
    }
    default:
        break;
    }

    return QPlainTextEdit::event(event);
}

void PostWindow::wheelEvent(QWheelEvent* e) {
    // FIXME: Disable zooming for now, to avoid nasty effect when Ctrl
    // is unintentionally pressed while inertial scrolling is going on.

    // Moreover, Ctrl|Shift + Wheel scrolls by pages, which is also
    // rather annoying.

    // So rather just forward the event without modifiers.

    e->setModifiers(Qt::NoModifier);

    QPlainTextEdit::wheelEvent(e);
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

void PostWindow::mousePressEvent(QMouseEvent* e) {
    clickedAnchor = (e->button() & Qt::LeftButton) ? anchorAt(e->pos()) : QString();
    QPlainTextEdit::mousePressEvent(e);
}

void PostWindow::mouseReleaseEvent(QMouseEvent* e) {
    // Make sure that the release event was over the same anchor it started on.
    if (e->button() & Qt::LeftButton && !clickedAnchor.isEmpty() && anchorAt(e->pos()) == clickedAnchor) {
        QUrl url { clickedAnchor };
        auto command =
            QString("PostWindowURLHandler(\"") + url.scheme() + QString("\", \"") + clickedAnchor + QString("\");");
        emit handleClickedURL(command, true);
    }
    QPlainTextEdit::mouseReleaseEvent(e);
}

void PostWindow::focusOutEvent(QFocusEvent* event) {
    if (event->reason() == Qt::TabFocusReason)
        MainWindow::instance()->focusCodeEditor();
    else
        QPlainTextEdit::focusOutEvent(event);
}

void PostWindow::mouseDoubleClickEvent(QMouseEvent* e) {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    QPlainTextEdit::mouseDoubleClickEvent(e);
    extendSelectionForEnvVar(this, textCursor());

    cursor.endEditBlock();
}

QMimeData* PostWindow::createMimeDataFromSelection() const {
    // Here, we bundle up just the plaintext (not HTML, as is the default) of
    // the post window's selected contents.
    QMimeData* data = new QMimeData;
    data->setText(textCursor().selection().toPlainText());
    return data;
}


bool PostWindow::gestureEvent(QGestureEvent* event) {
    if (QGesture* pinch = event->gesture(Qt::PinchGesture)) {
        auto* pinchGesture = static_cast<QPinchGesture*>(pinch);
        if (pinchGesture->state() == Qt::GestureUpdated) {
            float scaleFactor = pinchGesture->scaleFactor();
            zoomFont(scaleFactor);
        }
        return true;
    }
    return false;
}

bool PostWindow::openDocumentation() { return Main::openDocumentation(symbolUnderCursor()); }

void PostWindow::openDefinition() { Main::openDefinition(symbolUnderCursor(), window()); }

void PostWindow::openCommandLine() { Main::openCommandLine(symbolUnderCursor()); }

void PostWindow::findReferences() { Main::findReferences(symbolUnderCursor(), window()); }

void PostWindow::setLineWrap(bool lineWrapOn) {
    setLineWrapMode(lineWrapOn ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
    mActions[LineWrap]->setChecked(lineWrapOn);
    Main::settings()->setValue("IDE/postWindow/lineWrap", lineWrapOn);
}

PostDocklet::PostDocklet(QWidget* parent): Docklet(tr("Post window"), parent) {
    setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

    mPostWindow = new PostWindow;
    setWidget(mPostWindow);

    // This adds the QAction defined in PostWindow::createActions to the toolbar attached to the post window.
    toolBar()->addAction(mPostWindow->mActions[PostWindow::AutoScroll]);
    toolBar()->addAction(mPostWindow->mActions[PostWindow::Clear]);

    // connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(onFloatingChanged(bool)));
}

void PostDocklet::onFloatingChanged(bool floating) {
    // HACK: After undocking when main window maximized, the dock widget can not be
    // resized anymore. Apparently it has to do something with the fact that the dock
    // widget spans from edge to edge of the screen.
    // The issue is avoided by slightly shrinking the dock widget.
    if (floating)
        dockWidget()->resize(dockWidget()->size() - QSize(1, 1));
}


} // namespace ScIDE
