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

class QPainter;
class QWidget;
class QObject;

namespace QtCollider {
// WARNING these can be called only from Qt thread (no locking needed):
void announcePainting();
bool paintingAnnounced();

// WARNING language must be locked to call these:
bool beginPainting(QPainter*, QObject* obj = 0);
void endPainting();
bool isPaintingObject(QObject* obj);

QPainter* globalPainter();
}
