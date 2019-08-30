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

#include "prim_QPalette.hpp"
#include "primitives.h"
#include "../Common.h"
#include "../type_codec.hpp"

#include <PyrKernel.h>

#include <QPalette>

#include <cassert>

namespace QtCollider {

int QPalette_Finalize(struct VMGlobals* g, struct PyrObject* obj) {
    delete QPALETTE_FROM_OBJECT(obj);
    return errNone;
}

void QPalette_Init(struct VMGlobals* g, struct PyrObject* obj, const QPalette& palette) {
    assert(obj->classptr == SC_CLASS(QPalette));
    assert(IsNil(obj->slots) && IsNil(obj->slots + 1));

    QPalette* p = new QPalette(palette);
    SetPtr(obj->slots, p);
    InstallFinalizer(g, obj, 1, QPalette_Finalize);
}

QC_LANG_PRIMITIVE(QPalette_New, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPalette_Init(g, slotRawObject(r));
    return errNone;
}

QC_LANG_PRIMITIVE(QPalette_Auto, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QColor button = QtCollider::get(a);
    QColor window = QtCollider::get(a + 1);
    QPalette_Init(g, slotRawObject(r), QPalette(button, window));
    return errNone;
}

QC_LANG_PRIMITIVE(QPalette_System, 0, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPalette_Init(g, slotRawObject(r), QtCollider::systemPalette());
    return errNone;
}

QC_LANG_PRIMITIVE(QPalette_Color, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPalette* p = QPALETTE_FROM_OBJECT(slotRawObject(r));

    if (NotInt(a))
        return errWrongType;
    QPalette::ColorRole role = (QPalette::ColorRole)(slotRawInt(a));

    if (IsInt(a + 1)) {
        QPalette::ColorGroup group = (QPalette::ColorGroup)(slotRawInt(a + 1));
        QtCollider::set(r, p->color(group, role));
    } else {
        QtCollider::set(r, p->color(role));
    }

    return errNone;
}

QC_LANG_PRIMITIVE(QPalette_SetColor, 3, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPalette* p = QPALETTE_FROM_OBJECT(slotRawObject(r));

    QColor color = QtCollider::get(a);

    if (NotInt(a + 1))
        return errWrongType;
    QPalette::ColorRole role = (QPalette::ColorRole)(slotRawInt(a + 1));

    if (IsInt(a + 2)) {
        QPalette::ColorGroup group = (QPalette::ColorGroup)(slotRawInt(a + 2));
        p->setColor(group, role, color);
    } else {
        p->setColor(role, color);
    }

    return errNone;
}

QC_LANG_PRIMITIVE(QPalette_HasColor, 2, PyrSlot* r, PyrSlot* a, VMGlobals* g) {
    QPalette* p = QPALETTE_FROM_OBJECT(slotRawObject(r));

    if (NotInt(a))
        return errWrongType;
    QPalette::ColorRole role = (QPalette::ColorRole)(slotRawInt(a));

    QPalette::ColorGroup group;
    group = IsInt(a + 1) ? (QPalette::ColorGroup)(slotRawInt(a + 1)) : QPalette::Normal;

    SetBool(r, p->isBrushSet(group, role));

    return errNone;
}

void defineQPalettePrimitives() {
    LangPrimitiveDefiner definer;
    definer.define<QPalette_New>();
    definer.define<QPalette_Auto>();
    definer.define<QPalette_System>();
    definer.define<QPalette_Color>();
    definer.define<QPalette_SetColor>();
    definer.define<QPalette_HasColor>();
}

} // namespace QtCollider
