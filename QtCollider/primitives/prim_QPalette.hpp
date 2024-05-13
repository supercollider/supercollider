/************************************************************************
 *
 * Copyright 2012 Jakob Leben (jakob.leben@gmail.com)
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

#include <QPalette>

struct PyrObject;
struct VMGlobals;

#define QPALETTE_FROM_OBJECT(OBJ) reinterpret_cast<QPalette*>(slotRawPtr(OBJ->slots));

namespace QtCollider {

void QPalette_Init(struct VMGlobals* g, struct PyrObject* obj, const QPalette& palette = QPalette());

}
