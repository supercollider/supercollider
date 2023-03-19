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

#include "QcCanvas.h"
#include "../QcHelper.h"

#include <QScrollArea>

class QObjectProxy;

class QcScrollWidget : public QcCanvas {
    Q_OBJECT
public:
    QcScrollWidget(QWidget* parent = 0);
    QSize sizeHint() const;

protected:
    virtual bool event(QEvent*);
    virtual bool eventFilter(QObject*, QEvent*);
};

class QcScrollArea : public QScrollArea, public QcHelper {
    Q_OBJECT
    Q_PROPERTY(bool hasBorder READ dummyBool WRITE setHasBorder);
    Q_PROPERTY(QRectF innerBounds READ innerBounds);
    Q_PROPERTY(QPointF visibleOrigin READ visibleOrigin WRITE setVisibleOrigin);

Q_SIGNALS:
    void scrolled();

public:
    QcScrollArea();
    Q_INVOKABLE void setWidget(QWidget*);
    Q_INVOKABLE void addChild(QWidget* w);
    void setHasBorder(bool b);
    QRectF innerBounds() const;
    QPointF visibleOrigin() const;
    void setVisibleOrigin(const QPointF&);
    QSize sizeHint() const { return QSize(300, 200); }
    QSize minimumSizeHint() const { return QSize(50, 50); }
};
