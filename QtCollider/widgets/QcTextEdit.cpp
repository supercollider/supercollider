/************************************************************************
 *
 * Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * This file is part of SuperCollider Qt GUI.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

#include "QcTextEdit.h"
#include "../QcWidgetFactory.h"
#include "../hacks/hacks_qt.hpp"

#include <QFile>
#include <QUrl>
#include <QKeyEvent>
#include <QApplication>
#include <QTextBlock>

class QcTextEditFactory : public QcWidgetFactory<QcTextEdit> {
    void initialize(QWidgetProxy* p, QcTextEdit* w) { p->setMouseEventWidget(w->viewport()); }
};

QC_DECLARE_FACTORY(QcTextEdit, QcTextEditFactory);

QcTextEdit::QcTextEdit(): _interpretSelection(true) {
    setAttribute(Qt::WA_AcceptTouchEvents);

    connect(this, SIGNAL(interpret(QString)), qApp, SLOT(interpret(QString)), Qt::QueuedConnection);
}


QString QcTextEdit::documentFilename() const { return _document; }

void QcTextEdit::setDocument(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QByteArray data = file.readAll();
    setText(data);
    _document = filename;
}

int QcTextEdit::selectionStart() const { return textCursor().selectionStart(); }

int QcTextEdit::selectionSize() const {
    QTextCursor cursor = textCursor();
    return cursor.selectionEnd() - cursor.selectionStart();
}

void QcTextEdit::select(int start, int size) {
    if (start < 0)
        start = 0;

    QTextCursor cursor(document());

    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, start);
    cursor.movePosition(size > 0 ? QTextCursor::Right : QTextCursor::Left, QTextCursor::KeepAnchor, qAbs(size));

    setTextCursor(cursor);
}

QString QcTextEdit::selectedString() const {
    QString selection(textCursor().selectedText());
    return prepareText(selection);
}

void QcTextEdit::replaceSelectedText(const QString& string) {
    QTextCursor cursor(textCursor());
    if (cursor.hasSelection()) {
        cursor.removeSelectedText();
    };
    cursor.insertText(string);
}

QString QcTextEdit::currentLine() const { return textCursor().block().text(); }

void QcTextEdit::setTextFont(const QFont& f) {
    QTextCharFormat format;
    format.setFont(f);

    QTextCursor cursor(document());
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);

    QTextEdit::setFont(f);
}

void QcTextEdit::setTextColor(const QColor& color) {
    QTextCharFormat format;
    format.setForeground(color);

    QTextCursor cursor(document());
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);
}

void QcTextEdit::setRangeColor(const QVariantList& list) {
    if (list.count() < 3)
        return;
    QColor c = list[0].value<QColor>();
    int start = list[1].toInt();
    int size = list[2].toInt();

    QTextCharFormat format;
    format.setForeground(c);

    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, start);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, size);
    cursor.mergeCharFormat(format);
}

void QcTextEdit::setRangeFont(const QVariantList& list) {
    if (list.count() < 3)
        return;
    QFont f = list[0].value<QFont>();
    int start = list[1].toInt();
    int size = list[2].toInt();

    QTextCharFormat format;
    format.setFont(f);

    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, start);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, size);
    cursor.mergeCharFormat(format);
}

void QcTextEdit::setRangeText(const QVariantList& list) {
    if (list.count() < 3)
        return;
    QString text = list[0].value<QString>();
    int start = list[1].toInt();
    int size = list[2].toInt();

    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, start);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, size);
    cursor.insertText(text);
}

void QcTextEdit::keyPressEvent(QKeyEvent* e) {
    if (_interpretSelection && e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier)
        && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)) {
        QTextCursor c(textCursor());
        QString code;

        if (c.hasSelection()) {
            code = c.selectedText();
        }
        { code = c.block().text(); }

        Q_EMIT(interpret(prepareText(code)));

        return;
    }

    QTextEdit::keyPressEvent(e);
}

void QcTextEdit::insertFromMimeData(const QMimeData* data) {
    if (data->hasUrls()) {
        QTextCursor c(textCursor());
        QList<QUrl> urls = data->urls();
        int n = urls.count();
        for (int i = 0; i < n; ++i) {
            QUrl& url = urls[i];
            QString text = QURL_IS_LOCAL_FILE(url) ? url.toLocalFile() : url.toString();
            c.insertText(text);
            if (n > 1)
                c.insertText("\n");
        }
    } else
        QTextEdit::insertFromMimeData(data);
}

QString& QcTextEdit::prepareText(QString& text) const {
    // NOTE: QTextDocument contains unicode paragraph separators U+2029
    // instead of newline \n characters
    return text.replace(QChar(0x2029), QChar('\n'));
}
