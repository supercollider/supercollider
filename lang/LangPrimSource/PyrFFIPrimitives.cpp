// Author Jordan Henderson - JordanHendersonMusic
#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "PyrObject.h"
#include "PyrSlot.h"
#include <cassert>


int openGluon(VMGlobals* g, int numArgsPushed) {
    assert(numArgsPushed >= 2);
    const auto numArgsToPass = numArgsPushed - 2;

    PyrSlot* rec = g->sp - numArgsPushed + 1;
    PyrSlot* path_slot = rec + 1;
    PyrSlot* first_arg = path_slot + 1;

    dumpPyrSlot(rec);
    dumpPyrSlot(path_slot);

    PyrString* s = slotRawString(path_slot);

    const auto [err, str] = slotStdStrVal(path_slot);
    if (err)
        return errFailed;
    const auto library_id = g->gluonManager.register_library(str.c_str(), first_arg, numArgsToPass);

    SetInt(rec, library_id);
    return errNone;
}

int closeGluon(VMGlobals* g, int numArgsPushed) {
    assert(numArgsPushed == 2);
    PyrSlot* rec = g->sp - 1;
    PyrSlot* id = g->sp;
    if (!IsInt(id))
        return errFailed;
    const auto library_id = slotRawInt(id);
    g->gluonManager.unregister_library(library_id);
    return errNone;
}

int callGluon(VMGlobals* g, int numArgsPushed) {
    assert(numArgsPushed >= 3);
    PyrSlot* return_slot = (g->sp - numArgsPushed) + 1;
    PyrSlot* id_slot = return_slot + 1;
    PyrSlot* function_name_slot = id_slot + 1;

    if (!IsInt(id_slot))
        return errFailed;
    if (!IsSym(function_name_slot))
        return errFailed;

    g->gluonManager.evaluate_function(slotRawInt(id_slot), slotRawSymbol(function_name_slot), g, nullptr, return_slot,
                                      function_name_slot + 1, numArgsPushed - 3);
    return errNone;
}

int callGluonWithCallback(VMGlobals* g, int numArgsPushed) {
    assert(numArgsPushed >= 4);
    PyrSlot* return_slot = (g->sp - numArgsPushed) + 1;
    PyrSlot* id_slot = return_slot + 1;
    PyrSlot* function_name_slot = id_slot + 1;
    PyrSlot* callback_slot = function_name_slot + 1;

    if (!IsInt(id_slot))
        return errFailed;
    if (!IsSym(function_name_slot))
        return errFailed;
    if (!IsObj(callback_slot))
        return errFailed;

    PyrObject* callback = slotRawObject(callback_slot);

    g->gluonManager.evaluate_function(slotRawInt(id_slot), slotRawSymbol(function_name_slot), g, callback, return_slot,
                                      callback_slot + 1, numArgsPushed - 4);
    return errNone;
}

int getInbuiltGluonID(VMGlobals* g, int numArgsPushed) {
    assert(numArgsPushed == 2);
    PyrSlot* return_slot = (g->sp - numArgsPushed) + 1;
    PyrSlot* name_slot = return_slot + 1;
    if (!IsSym(name_slot))
        return errFailed;

    const auto library_id = g->gluonManager.get_inbuilt_library_id(slotRawSymbol(name_slot));

    SetInt(return_slot, library_id);
    return errNone;
}

void initFFIPrimitives() {
    int base, index = 0;

    base = nextPrimitiveIndex();

    definePrimitive(base, index++, "_OpenGluon", openGluon, 2, 1);
    definePrimitive(base, index++, "_CloseGluon", closeGluon, 2, 0);
    definePrimitive(base, index++, "_CallGluon", callGluon, 3, 1);
    definePrimitive(base, index++, "_CallGluonWithCallback", callGluonWithCallback, 4, 1);
    definePrimitive(base, index++, "_GetInbuiltGluonID", getInbuiltGluonID, 2, 0);
}
