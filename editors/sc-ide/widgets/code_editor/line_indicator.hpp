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

#pragma once

#include <QWidget>

namespace ScIDE {

class LineIndicator : public QWidget {
    Q_OBJECT

public:
    LineIndicator(class GenericCodeEditor* editor);
    void setHideLineIndicator(bool hide);
Q_SIGNALS:
    void widthChanged();
public Q_SLOTS:
    void setLineCount(int);

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent*);
    virtual void mouseDoubleClickEvent(QMouseEvent*);
    virtual void changeEvent(QEvent*);

private:
    int widthForLineCount(int lineCount);

    class GenericCodeEditor* mEditor;
    int mLineCount;
    int mLastCursorPos;
    bool hideLineIndicator;
};

}
