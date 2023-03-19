/************************************************************************
 *
 * Copyright 2010-2012 Jakob Leben (jakob.leben@gmail.com)
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

#include "primitives.h"
#include "../Common.h"
#include "QtCollider.h"

#include <SCBase.h>

namespace QtCollider {

#define QC_DO_SYMBOL(SYM) PyrSymbol* sym_##SYM
QC_DO_SYMBOLS
#undef QC_DO_SYMBOL

void defineQObjectPrimitives();
void defineQPenPrimitives();
void defineMiscPrimitives();
void defineQWidgetPrimitives();
void defineQPalettePrimitives();
void defineQImagePrimitives();

#ifdef __APPLE__
void defineQcQuartzComposerPrimitives();
#endif

void initPrimitives() {
    QtCollider::init();

    qcDebugMsg(1, "initializing QtGUI primitives");

    defineQObjectPrimitives();
    defineQWidgetPrimitives();
    defineQPenPrimitives();
    defineMiscPrimitives();
    defineQPalettePrimitives();
    defineQImagePrimitives();

#ifdef __APPLE__
    defineQcQuartzComposerPrimitives();
#endif

#define QC_DO_SYMBOL(SYM) sym_##SYM = getsym(#SYM);
    QC_DO_SYMBOLS
#undef QC_DO_SYMBOL
}

} // namespace QtCollider
