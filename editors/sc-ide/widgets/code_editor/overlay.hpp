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

#include <QGraphicsPixmapItem>
#include <QPropertyAnimation>

namespace ScIDE {

class GenericCodeEditor;

class OverlayAnimator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor);
    QColor backgroundColor() const;
    void setBackgroundColor(const QColor& color);

public:
    OverlayAnimator(GenericCodeEditor* editor, QObject* parent = 0);
    void setActiveAppearance(bool active);

private:
    GenericCodeEditor* mEditor;
    QPropertyAnimation mBackgroundAnimation;
};

class CodeFragmentOverlay : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
};

} // namespace ScIDE
