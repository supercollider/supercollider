/************************************************************************
 *
 * Copyright 2012 Jakob Leben (jakob.leben@gmail.com)
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (qt-info@nokia.com)
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

#include <QLayout>
#include <QList>

namespace QtCollider {

class StackLayout : public QLayout {
    Q_OBJECT
    Q_ENUMS(StackingMode)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_PROPERTY(StackingMode stackingMode READ stackingMode WRITE setStackingMode)
    Q_PROPERTY(int count READ count)

public:
    enum StackingMode { StackOne, StackAll };

    StackLayout();
    ~StackLayout();

    int addWidget(QWidget* w);
    int insertWidget(int index, QWidget* w);

    QWidget* currentWidget() const;
    void setCurrentWidget(QWidget* widget);
    int currentIndex() const;
    void setCurrentIndex(int index);

    inline QWidget* widget() { return QLayout::widget(); }

    QWidget* widget(int) const;
    int count() const;

    StackingMode stackingMode() const;
    void setStackingMode(StackingMode stackingMode);

    // abstract virtual functions:
    void addItem(QLayoutItem* item);
    QLayoutItem* itemAt(int) const;
    QLayoutItem* takeAt(int);
    QSize sizeHint() const;
    QSize minimumSize() const;
    Qt::Orientations expandingDirections() const;
    void setGeometry(const QRect& rect);

    virtual void invalidate();

private:
    QList<QLayoutItem*> _list;
    int _index;
    StackingMode _mode;
    bool _gotParent;
};

} // namespace QtCollider
