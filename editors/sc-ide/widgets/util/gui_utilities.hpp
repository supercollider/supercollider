/*
    SuperCollider Qt IDE
    Copyright (c) 2012-2013 Jakob Leben & Tim Blechmann
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

#pragma once

#include <QPlainTextEdit>
#include <QRegExp>
#include <QTextBlock>

namespace ScIDE {

// match words, environment variable and symbols
inline QString tokenInStringAt(int position, const QString& source) {
    const QRegExp wordRegexp("(~?|\\\\?)\\w+");

    int offset = 0;
    while (offset <= position) {
        int index = wordRegexp.indexIn(source, offset);
        if (index == -1)
            break;
        int len = wordRegexp.matchedLength();
        if (index <= position && index + len >= position)
            return wordRegexp.cap(0);
        offset = index + len;
    }

    return QString();
}

inline void extendSelectionForEnvVar(QPlainTextEdit* textEdit, QTextCursor selection) {
    if (selection.hasSelection()) {
        if (selection.selectedText() == QStringLiteral("~")) {
            QTextCursor wordAfter(selection);
            wordAfter.movePosition(QTextCursor::NextCharacter);
            wordAfter.select(QTextCursor::WordUnderCursor);
            if (wordAfter.hasSelection() && (selection.block() == wordAfter.block())) {
                selection.setPosition(selection.selectionStart());
                selection.setPosition(wordAfter.selectionEnd(), QTextCursor::KeepAnchor);
                textEdit->setTextCursor(selection);
            }
        } else {
            int positionBeforeSelection = selection.selectionStart() - 1;
            if (positionBeforeSelection >= 0) {
                QChar charBeforeSelection = textEdit->document()->characterAt(positionBeforeSelection);
                if (charBeforeSelection == QChar('~')) {
                    QTextCursor extendedSelection = textEdit->textCursor();
                    extendedSelection.setPosition(positionBeforeSelection);
                    extendedSelection.setPosition(selection.selectionEnd(), QTextCursor::KeepAnchor);
                    textEdit->setTextCursor(extendedSelection);
                }
            }
        }
    }
}

QRect fittedToScreen(const QRect& rect, QWidget* referenceWidget);

} // namespace ScIDE
