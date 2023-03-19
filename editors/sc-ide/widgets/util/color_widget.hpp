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

#include <QLabel>
#include <QColorDialog>
#include <QPainter>

namespace ScIDE {

class ColorWidget : public QLabel {
    Q_OBJECT

public:
    ColorWidget(QWidget* parent = 0): QLabel(parent) { setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); }

    void setColor(const QColor& color) {
        mBrush.setColor(color);
        mBrush.setStyle(Qt::SolidPattern);
        update();
        emit colorChanged(mBrush.color());
        emit brushChanged(mBrush);
    }

    void setBrush(const QBrush& brush) {
        mBrush = brush;
        update();
        emit colorChanged(mBrush.color());
        emit brushChanged(mBrush);
    }

    QColor color() const { return mBrush.color(); }

    QBrush brush() const { return mBrush; }

public slots:

    void clear() { setBrush(QBrush()); }

signals:
    void colorChanged(const QColor&);
    void brushChanged(const QBrush&);
    void colorPicked(const QColor&);

protected:
    virtual void mouseReleaseEvent(QMouseEvent*) {
        QColor c = QColorDialog::getColor(color(), this);
        if (c.isValid()) {
            setColor(c);
            emit colorPicked(c);
        }
    }

    virtual void paintEvent(QPaintEvent*) {
        QBrush brush = mBrush;
        if (brush.style() == Qt::NoBrush) {
            brush.setColor(palette().color(QPalette::WindowText));
            brush.setStyle(Qt::BDiagPattern);
        }

        QPainter painter(this);
        painter.setBrush(brush);
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }

private:
    QBrush mBrush;
};

} // namespace ScIDE
