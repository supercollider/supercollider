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

#include "Common.h"
#include "metatype.hpp"

#include <PyrKernel.h>
#include <VMGlobals.h>
#include <PyrLexer.h>

// WARNING: QtCollider::lockLang() must be called before
void QtCollider::runLang(PyrObjectHdr* receiver, PyrSymbol* method, const QList<QVariant>& args, PyrSlot* result) {
    VMGlobals* g = gMainVMGlobals;
    g->canCallOS = true;
    ++g->sp;
    SetObject(g->sp, receiver);
    Q_FOREACH (QVariant var, args) {
        ++g->sp;
        if (!QtCollider::set(g->sp, var)) {
            qcErrorMsg("Failed to write a slot when trying to run interpreter!");
            SetNil(g->sp);
        }
    }
    runInterpreter(g, method, args.size() + 1);
    g->canCallOS = false;
    if (result)
        slotCopy(result, &g->result);
}

int QtCollider::wrongThreadError() {
    qcErrorMsg("You can not use this Qt functionality in the current thread. "
               "Try scheduling on AppClock instead.");
    return errFailed;
}
