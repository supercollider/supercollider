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

#include "gui_utilities.hpp"

#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>

namespace ScIDE {

QRect fittedToScreen(const QRect& rect, QWidget* referenceWidget) {
    QRect fittedRect = rect;
    QRect screen = QApplication::desktop()->availableGeometry(referenceWidget);
    if (!screen.contains(fittedRect)) {
        if (fittedRect.right() > screen.right())
            fittedRect.moveRight(screen.right());
        if (fittedRect.left() < screen.left())
            fittedRect.moveLeft(screen.left());
        if (fittedRect.bottom() > screen.bottom())
            fittedRect.moveBottom(screen.bottom());
        if (fittedRect.top() < screen.top())
            fittedRect.moveTop(screen.top());
    }
    return fittedRect;
}

}
