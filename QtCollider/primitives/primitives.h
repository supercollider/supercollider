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

#include <PyrPrimitive.h>
#include <VMGlobals.h>
#include <PyrSlot.h>

#include <QList>

#include <cstring>

namespace QtCollider {

template <typename T> struct LangPrimitive {};

class LangPrimitiveDefiner {
public:
    LangPrimitiveDefiner(): _base(nextPrimitiveIndex()), _index(0) {}

    template <typename T> void define() { LangPrimitive<T>::define(_base, _index++); }

private:
    int _base;
    int _index;
};


} // namespace

#define QC_LANG_PRIMITIVE(NAME, ARGC, RECEIVER, ARGS, GLOBAL)                                                          \
    struct NAME {};                                                                                                    \
    template <> struct LangPrimitive<NAME> {                                                                           \
        static int implementation(RECEIVER, ARGS, GLOBAL);                                                             \
        static int mediate(VMGlobals* g, int i) {                                                                      \
            PyrSlot* stack = g->sp - i + 1;                                                                            \
            return implementation(stack, i > 1 ? stack + 1 : 0, g);                                                    \
        }                                                                                                              \
        static void define(int base, int index) { definePrimitive(base, index, "_" #NAME, &mediate, ARGC + 1, 0); }    \
    };                                                                                                                 \
    int LangPrimitive<NAME>::implementation(RECEIVER, ARGS, GLOBAL)

#if 0
#    define QC_LANG_PRIMITIVE(name, argc, receiver, args, global)                                                      \
        int name(receiver, args, global);                                                                              \
        static QtCollider::LangPrimitive<&name> p_##name("_" #name, argc);                                             \
        int name(receiver, args, global)
#endif