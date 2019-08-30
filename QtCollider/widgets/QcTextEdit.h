/************************************************************************
 *
 * Copyright 2010 Jakob Leben (jakob.leben@gmail.com)
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

#pragma once

#include "../QcHelper.h"

#include <QTextEdit>

class QcTextEdit : public QTextEdit, QcHelper {
    Q_OBJECT
    Q_PROPERTY(QString document READ documentFilename WRITE setDocument);
    Q_PROPERTY(int selectionStart READ selectionStart);
    Q_PROPERTY(int selectionSize READ selectionSize);
    Q_PROPERTY(QString selectedString READ selectedString WRITE replaceSelectedText);
    Q_PROPERTY(QString currentLine READ currentLine);
    Q_PROPERTY(QFont textFont READ dummyFont WRITE setTextFont);
    Q_PROPERTY(QColor textColor READ dummyColor WRITE setTextColor);
    Q_PROPERTY(QVariantList rangeColor READ dummyVariantList WRITE setRangeColor);
    Q_PROPERTY(QVariantList rangeFont READ dummyVariantList WRITE setRangeFont);
    Q_PROPERTY(QVariantList rangeText READ dummyVariantList WRITE setRangeText);
    Q_PROPERTY(bool enterInterpretsSelection READ interpretSelection WRITE setInterpretSelection);

public:
    QcTextEdit();
    QString documentFilename() const;
    void setDocument(const QString&);
    int selectionStart() const;
    int selectionSize() const;
    Q_INVOKABLE void select(int start, int size);
    QString selectedString() const;
    void replaceSelectedText(const QString&);
    QString currentLine() const;
    bool interpretSelection() const { return _interpretSelection; }
    void setInterpretSelection(bool b) { _interpretSelection = b; }
    void setTextFont(const QFont&);
    void setTextColor(const QColor&);
    void setRangeColor(const QVariantList&);
    void setRangeFont(const QVariantList&);
    void setRangeText(const QVariantList&);

Q_SIGNALS:
    void interpret(const QString& code);

protected:
    virtual void keyPressEvent(QKeyEvent*);
    virtual void insertFromMimeData(const QMimeData*);

private:
    QString& prepareText(QString& str) const;

    QString _document;
    bool _interpretSelection;
};
