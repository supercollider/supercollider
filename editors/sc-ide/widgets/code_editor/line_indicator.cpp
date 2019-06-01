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

#include "editor.hpp"
#include "line_indicator.hpp"

using namespace ScIDE;

LineIndicator::LineIndicator(GenericCodeEditor* editor): QWidget(editor), mEditor(editor) { setLineCount(1); }

void LineIndicator::setLineCount(int count) {
    mLineCount = count;
    setFixedWidth(widthForLineCount(count));
    Q_EMIT(widthChanged());
}

void LineIndicator::changeEvent(QEvent* e) {
    if (e->type() == QEvent::FontChange) {
        setFixedWidth(widthForLineCount(mLineCount));
        Q_EMIT(widthChanged());
    } else
        QWidget::changeEvent(e);
}

void LineIndicator::paintEvent(QPaintEvent* e) { mEditor->paintLineIndicator(e); }

void LineIndicator::mousePressEvent(QMouseEvent* e) {
    QTextCursor cursor = mEditor->cursorForPosition(QPoint(0, e->pos().y()));
    if (cursor.isNull()) {
        mLastCursorPos = -1;
        return;
    }
    mEditor->setTextCursor(cursor);
    mLastCursorPos = cursor.position();
}

void LineIndicator::mouseMoveEvent(QMouseEvent* e) {
    QTextCursor cursor = mEditor->cursorForPosition(QPoint(0, e->pos().y()));
    if (cursor.isNull() || cursor.position() == mLastCursorPos)
        return;
    QTextCursor origCursor = mEditor->textCursor();
    origCursor.setPosition(cursor.position(), QTextCursor::KeepAnchor);
    mEditor->setTextCursor(origCursor);
    mLastCursorPos = cursor.position();
    // The selectionChanged() signal of the editor does not always fire here!
    // Qt bug?
    update();
}

void LineIndicator::mouseDoubleClickEvent(QMouseEvent* e) {
    QTextCursor cursor = mEditor->cursorForPosition(QPoint(0, e->pos().y()));
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
    mEditor->setTextCursor(cursor);
}

int LineIndicator::widthForLineCount(int lineCount) {
    int digits = 2;

    if (hideLineIndicator)
        return 0;

    while (lineCount >= 100) {
        lineCount /= 10;
        ++digits;
    }

    return 6 + fontMetrics().width('9') * digits;
}

void LineIndicator::setHideLineIndicator(bool hide) { hideLineIndicator = hide; }
