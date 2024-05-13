/************************************************************************
 *
 * Copyright 2011 Jakob Leben (jakob.leben@gmail.com)
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

#include "BasicWidgets.h"
#include "QcScrollArea.h"

class QcWindow : public QcCustomPainted {
    Q_OBJECT
public:
    QcWindow() {}
    Q_INVOKABLE QcWindow(const QString& title, const QRectF& geom, bool resizable, bool frame);
};

class QcScrollWindow : public QcScrollArea {
    Q_OBJECT
public:
    QcScrollWindow() {}
    Q_INVOKABLE QcScrollWindow(const QString& title, const QRectF& geom, bool resizable, bool frame);
};
