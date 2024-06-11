/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
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

#include "PyrMessage.h"
#include "PyrPrimitiveProto.h"
#include "PyrInterpreter.h"
#include "PyrPrimitive.h"
#include "PyrListPrim.h"
#include "PyrSymbol.h"
#include "GC.h"
#include <stdlib.h>
#include <assert.h>
#include <optional>
#include "PredefinedSymbols.h"
#include "PyrObjectProto.h"
#include "SCBase.h"
#include "PyrListPrim.h"

#define DEBUGMETHODS 0
#define METHODMETER 0

#ifdef DEBUGMETHODS
static constexpr bool DebugMethods = true;
#else
static constexpr bool DebugMethods = false;
#endif

#ifdef METHODMETER
static constexpr bool MethodMeter = true;
#else
static constexpr bool MethodMeter = false;
#endif

#ifdef GC_SANITYCHECK
static constexpr bool GCSanityCheckEnabled = true;
#else
static constexpr bool GCSanityCheckEnabled = false;
#endif

struct GCSanityChecker {
    GCSanityChecker(VMGlobals* g, const char* name): g(g), name(name) {
        if constexpr (GCSanityCheckEnabled) {
            g->gc->SanityCheck();
            CallStackSanity(g, name);
        }
    }
    ~GCSanityChecker() {
        if constexpr (GCSanityCheckEnabled) {
            g->gc->SanityCheck();
            CallStackSanity(g, name);
        }
    }
    GCSanityChecker(GCSanityChecker&&) = delete;
    GCSanityChecker(const GCSanityChecker&) = delete;

private:
    VMGlobals* g;
    const char* name;
};

// This MUST be called at the top of the function before the VMGlobals object is altered.
void prepForTailCall(VMGlobals* g) {
#if TAILCALLOPTIMIZE
    const int tailCall = g->tailCall;
    if (tailCall == 1)
        returnFromMethod(g);
    else if (tailCall > 0)
        returnFromBlock(g);
#endif
}

struct ResetTailCall {
    VMGlobals* g;
    ~ResetTailCall() {
#if TAILCALLOPTIMIZE
        g->tailCall = 0;
#endif
    }
};

PyrMethod** gRowTable;

PyrSlot temporaryKeywordStack[temporaryKeywordStackCapacity];
bool gKeywordError = true;
extern bool gTraceInterpreter;

long cvxUniqueMethods;
extern int ivxIdentDict_array;

void StoreToImmutableB(VMGlobals* g, PyrSlot*& sp, unsigned char*& ip);

void initUniqueMethods() {
    PyrClass* dummyclass;
    cvxUniqueMethods = classVarOffset("Object", "uniqueMethods", &dummyclass);
}

HOT void sendMessageImpl(VMGlobals* g, PyrSymbol* selector, PyrSlot* recvrSlot, PyrClass* classobj, long numArgsPushed,
                         long numKeyArgsPushed) {
    ResetTailCall reset_tail_call { g };
    PyrSlot* sp;

lookup_again:
    auto classIndex = slotRawInt(&classobj->classIndex) + selector->u.index;
    auto method = gRowTable[classIndex];
    auto methodRaw = METHRAW(method);

    if (slotRawSymbol(&method->name) != selector) {
        doesNotUnderstand(g, selector, numArgsPushed, numKeyArgsPushed);
        return;
    }

    const auto pushDefaultArgs = [&]() {
        PyrSlot *pslot, *qslot;
        long m, mmax;
        pslot = g->sp;
        qslot = slotRawObject(&method->prototypeFrame)->slots + numArgsPushed - 1;
        for (m = 0, mmax = methodRaw->numargs - numArgsPushed; m < mmax; ++m)
            slotCopy(++pslot, ++qslot);
        numArgsPushed = methodRaw->numargs;
        g->sp += mmax;
    };

    const auto applyKeywords = [&]() {
        numArgsPushed = keywordFixStack(g, method, methodRaw, numArgsPushed, numKeyArgsPushed);
        numKeyArgsPushed = 0;
    };


    // There are multiple types of methods.
    // The basic is methNormal which goes through executeMethod.
    // The rest are some form of optimization.
    switch (methodRaw->methType) {
    case methNormal: {
        executeMethod(g, method, numArgsPushed, numKeyArgsPushed);
        return;
    }

    case methReturnSelf: {
        g->sp -= numArgsPushed - 1;
        return;
    }

    case methReturnLiteral: {
        sp = g->sp -= numArgsPushed - 1;
        slotCopy(sp, &method->selectors); // in this case selectors is just a single value
        return;
    }

    case methReturnArg: {
        if (numKeyArgsPushed > 0)
            applyKeywords();
        sp = g->sp -= numArgsPushed - 1;
        classIndex = methodRaw->specialIndex; // zero is index of the first argument
        auto toCopy =
            classIndex < numArgsPushed ? sp + classIndex : &slotRawObject(&method->prototypeFrame)->slots[classIndex];
        slotCopy(sp, toCopy);
        return;
    }
        // return instance variable
    case methReturnInstVar: {
        sp = g->sp -= numArgsPushed - 1;
        classIndex = methodRaw->specialIndex;
        slotCopy(sp, &slotRawObject(recvrSlot)->slots[classIndex]);
        return;
    }

        // assign instance variable
    case methAssignInstVar: {
        sp = g->sp -= numArgsPushed - 1;
        classIndex = methodRaw->specialIndex;
        auto obj = slotRawObject(recvrSlot);
        if (obj->IsImmutable()) {
            StoreToImmutableB(g, sp, g->ip);
            return;
        }
        if (numArgsPushed >= 2) {
            slotCopy(&obj->slots[classIndex], sp + 1);
            g->gc->GCWrite(obj, sp + 1);
        } else {
            SetNil(&obj->slots[classIndex]);
        }
        slotCopy(sp, recvrSlot);
        return;
    }

    case methReturnClassVar: {
        sp = g->sp -= numArgsPushed - 1;
        slotCopy(sp, &g->classvars->slots[methodRaw->specialIndex]);
        return;
    }

    case methAssignClassVar: {
        sp = g->sp -= numArgsPushed - 1;
        if (numArgsPushed >= 2) {
            slotCopy(&g->classvars->slots[methodRaw->specialIndex], sp + 1);
            g->gc->GCWrite(g->classvars, sp + 1);
        } else {
            SetNil(&g->classvars->slots[methodRaw->specialIndex]);
        }
        slotCopy(sp, recvrSlot);
        return;
    }

    // send a different selector to self, e.g. this.subclassResponsibility
    case methRedirect: {
        if (numKeyArgsPushed > 0)
            applyKeywords();
        else if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgs();
        selector = slotRawSymbol(&method->selectors);
        goto lookup_again;
    }

    // send a different selector to self, e.g. super.subclassResponsibility
    case methRedirectSuper: {
        if (numKeyArgsPushed > 0)
            applyKeywords();
        else if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgs();
        selector = slotRawSymbol(&method->selectors);
        classobj = slotRawSymbol(&slotRawClass(&method->ownerclass)->superclass)->u.classobj;
        goto lookup_again;
    }
        // forward to an instance variable
    case methForwardInstVar: {
        if (numKeyArgsPushed > 0)
            applyKeywords();
        else if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgs();
        selector = slotRawSymbol(&method->selectors);
        classIndex = methodRaw->specialIndex;
        slotCopy(recvrSlot, &slotRawObject(recvrSlot)->slots[classIndex]);
        classobj = classOfSlot(recvrSlot);
        goto lookup_again;
    }

    case methForwardClassVar: {
        if (numKeyArgsPushed > 0)
            applyKeywords();
        else if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgs();
        selector = slotRawSymbol(&method->selectors);
        slotCopy(recvrSlot, &g->classvars->slots[methodRaw->specialIndex]);
        classobj = classOfSlot(recvrSlot);
        goto lookup_again;
    }

    case methPrimitive: {
        if (numKeyArgsPushed > 0)
            doPrimitiveWithKeys(g, method, numArgsPushed, numKeyArgsPushed);
        else
            doPrimitive(g, method, numArgsPushed);

        return;
    }
    }
}

HOT void sendMessage(VMGlobals* g, PyrSymbol* selector, long numArgsPushed, long numKeyArgsPushed) {
    const GCSanityChecker gc_sanity_checker(g, "sendMessage");

    auto recvrSlot = g->sp - numArgsPushed + 1;
    auto classobj = classOfSlot(recvrSlot);
    sendMessageImpl(g, selector, recvrSlot, classobj, numArgsPushed, numKeyArgsPushed);
}


HOT void sendSuperMessage(VMGlobals* g, PyrSymbol* selector, long numArgsPushed, long numKeyArgsPushed) {
    const GCSanityChecker gc_sanity_checker(g, "sendSuperMessage");

    auto recvrSlot = g->sp - numArgsPushed + 1;
    auto classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
    sendMessageImpl(g, selector, recvrSlot, classobj, numArgsPushed, numKeyArgsPushed);
}


extern PyrClass* class_identdict;

// An optimization for unique methods.
// Returns false if it isn't a unique method.
bool tryUniqueMethod(VMGlobals* g, PyrMethod* meth, PyrSlot* receiverSlot, PyrSlot* selectorSlot, long numArgsPushed,
                     long numKeyArgsPushed) {
    if (slotRawClass(&meth->ownerclass) != class_object)
        return false;
    auto uniqueMethodSlot = &g->classvars->slots[cvxUniqueMethods];
    if (!isKindOfSlot(uniqueMethodSlot, class_identdict))
        return false;
    auto arraySlot = slotRawObject(uniqueMethodSlot)->slots + ivxIdentDict_array;
    if (!IsObj(arraySlot))
        return false;
    auto array = slotRawObject(arraySlot);
    if (array->classptr != class_array)
        return false;
    auto i = arrayAtIdentityHashInPairs(array, receiverSlot);
    if (i < 0)
        return false;
    auto slot = array->slots + i;
    if (IsNil(slot))
        return false;
    ++slot;
    if (!isKindOfSlot(slot, class_identdict))
        return false;
    auto arraySlot2 = slotRawObject(slot)->slots + ivxIdentDict_array;
    if (!IsObj(arraySlot2))
        return false;
    auto array2 = slotRawObject(arraySlot2);
    if (array2->classptr != class_array)
        return false;
    auto i2 = arrayAtIdentityHashInPairs(array2, selectorSlot);
    if (i2 < 0)
        return false;
    auto slot2 = array2->slots + i;
    if (IsNil(slot2))
        return false;
    ++slot2;
    slotCopy(selectorSlot, receiverSlot);
    slotCopy(receiverSlot, slot2);
    blockValueWithKeys(g, numArgsPushed + 1, numKeyArgsPushed);
    return true;
}

void doesNotUnderstand(VMGlobals* g, PyrSymbol* selector, long numArgsPushed, long numKeyArgsPushed) {
    const GCSanityChecker check(g, "doesNotUnderstand");

    auto receiverSlot = g->sp - numArgsPushed + 1;
    auto classObject = classOfSlot(receiverSlot);

    // Push selector so it is after the receiver but before any arguments.
    SetSymbol(++g->sp, selector);
    if (numArgsPushed > 1) {
        // right rotate by one (skipping receiver)
        // moving args up and placing selector next to the receiver
        auto rargBegin = std::reverse_iterator<PyrSlot*>(g->sp + 1);
        auto rargEnd = std::reverse_iterator<PyrSlot*>(g->sp - numArgsPushed + 1);
        std::rotate(rargBegin, rargBegin + 1, rargEnd);
    }
    auto selectorSlot = g->sp - numArgsPushed + 1;

    const auto callIndex = slotRawInt(&classObject->classIndex) + s_doesNotUnderstand->u.index;
    auto method = gRowTable[callIndex];

    if (tryUniqueMethod(g, method, receiverSlot, selectorSlot, numArgsPushed, numKeyArgsPushed))
        return;

    executeMethod(g, method, numArgsPushed + 1, numKeyArgsPushed);
}


inline PyrFrame* createFrameToCall(VMGlobals* g, PyrBlock* block) {
    const PyrMethodRaw* methraw = METHRAW(block);
    const PyrObject* proto = slotRawObject(&block->prototypeFrame);
    auto frame =
        reinterpret_cast<PyrFrame*>(g->gc->NewFrame(methraw->frameSize, 0, obj_slot, methraw->needsHeapContext));
    frame->classptr = class_frame;
    frame->size = FRAMESIZE + proto->size;
    SetObject(&frame->method, block);
    SetObject(&frame->homeContext, frame);
    SetObject(&frame->context, frame);
    if (PyrFrame* caller = g->frame; caller != nullptr) {
        SetPtr(&caller->ip, g->ip);
        SetObject(&frame->caller, caller);
    } else {
        SetInt(&frame->caller, 0);
    }
    SetPtr(&frame->ip, nullptr);
    return frame;
}


class AllocArrayIfWrittenTo {
public:
    AllocArrayIfWrittenTo() = delete;
    AllocArrayIfWrittenTo(PyrObject* default_resource, long size, VMGlobals* g):
        array(default_resource),
        size(size),
        g(g) {}
    AllocArrayIfWrittenTo(AllocArrayIfWrittenTo&&) = default;
    AllocArrayIfWrittenTo(const AllocArrayIfWrittenTo&) = default;
    AllocArrayIfWrittenTo& operator=(AllocArrayIfWrittenTo&&) = default;
    AllocArrayIfWrittenTo& operator=(const AllocArrayIfWrittenTo&) = default;
    ~AllocArrayIfWrittenTo() = default;

    void append(const PyrSlot* obj) {
        if (!hasBeenWrittenTo) {
            array = newPyrArray(g->gc, static_cast<int>(size), 0, false);
            arrayWritePtr = array->slots;
            hasBeenWrittenTo = true;
        }
        slotCopy(arrayWritePtr, obj);
        ++arrayWritePtr;
    }
    void copyInto(const PyrSlot* first, const PyrSlot* last) {
        if (!hasBeenWrittenTo) {
            array = newPyrArray(g->gc, static_cast<int>(size), 0, false);
            arrayWritePtr = array->slots;
            hasBeenWrittenTo = true;
        }
        std::copy(first, last, array->slots);
    }

    PyrObject* getArray() {
        if (!hasBeenWrittenTo)
            return array;
        array->size = static_cast<int>(std::distance(array->slots, arrayWritePtr));
        return array;
    }

private:
    bool hasBeenWrittenTo = false;
    PyrObject* array;
    PyrSlot* arrayWritePtr = nullptr;
    long size;
    VMGlobals* g;
};


void kwArgMismatchErrorMethod(const char* argName, void* ptr) {
    auto meth = reinterpret_cast<PyrMethod*>(ptr);
    post("WARNING: keyword arg '%s' not found in call to %s:%s\n", argName,
         slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);
}
void kwArgMismatchErrorBlock(const char* argName, void* ptr) {
    post("WARNING: keyword arg '%s' not found in call to function.\n", argName);
}

// This function puts the arguments into the passed in callFrame ready to be called.
// It will also lookup arguments in the environment if one is passed in.
void prepareArgsForExecute(VMGlobals* g, PyrBlock* block, PyrFrame* callFrame, long totalSuppliedArgs,
                           long numKwArgsSupplied, bool isMethod, PyrObject* optionalEnvirLookup) {
    // There are some instances where the proto is a nullptr.
    // There is nothing this function can do in that case.
    if (!IsObj(&block->prototypeFrame))
        return;
    PyrObject* proto = slotRawObject(&block->prototypeFrame);

    const PyrMethodRaw* methodRaw = METHRAW(block);
    const auto methNumActualArgs = methodRaw->posargs;
    const auto numNormalSuppliedArgs = totalSuppliedArgs - (numKwArgsSupplied * 2);
    const bool methHasVarArg = methodRaw->varargs > 0;
    const bool methHasKwArg = methodRaw->varargs > 1;
    const auto methNumNormArgs = methodRaw->numargs;
    const auto methNumVariables = methodRaw->numvars;
    const auto methArgNames = slotRawSymbolArray(&block->argNames)->symbols;

    PyrSlot* resultingCallFrame = callFrame->vars;

    PyrSlot* pushedArgs = g->sp - (totalSuppliedArgs - 1);
    PyrSlot* pushedKeywords = g->sp - (numKwArgsSupplied * 2 - 1);

    if (methNumActualArgs == 0) {
        std::copy(proto->slots, proto->slots + methNumVariables, resultingCallFrame);
        return;
    }

    // Optional arrays for variable arguments and keyword arguments.
    PyrObject* variableArgumentsArray = nullptr;
    PyrObject* keywordArgumentsArray = nullptr;
    int keywordArgumentSize = 0;

    const auto findArgNameCollision = [=](const PyrSymbol* argName) -> std::optional<uint32_t> {
        for (size_t namei = 0; namei < methNumNormArgs; ++namei) {
            const auto indexOffsetFromThis = namei + (isMethod ? 1 : 0);
            if (methArgNames[indexOffsetFromThis] == argName)
                return indexOffsetFromThis;
        }
        return std::nullopt;
    };

    // Number that will actually be pushed to resulting call frame.
    const auto numNormalArgsAdded = std::min<uint32>(numNormalSuppliedArgs, methNumNormArgs);
    std::copy(pushedArgs, pushedArgs + numNormalArgsAdded, resultingCallFrame);

    if (numNormalSuppliedArgs > methNumNormArgs) {
        if (methHasVarArg) {
            // Put extra in variable array if method supports it.
            const auto size = static_cast<int>(numNormalSuppliedArgs) - methNumNormArgs;
            variableArgumentsArray = newPyrArray(g->gc, size, 0, false);
            variableArgumentsArray->size = size;
            std::copy(pushedArgs + numNormalArgsAdded, pushedArgs + numNormalSuppliedArgs,
                      variableArgumentsArray->slots);
        }
    } else if (numNormalSuppliedArgs < methNumNormArgs) {
        // Push default if not enough given.
        std::copy(proto->slots + numNormalArgsAdded, proto->slots + methNumNormArgs,
                  resultingCallFrame + numNormalArgsAdded);
        if (optionalEnvirLookup) {
            // Lookup in envir if present.
            for (auto i = numNormalArgsAdded; i < methNumNormArgs; ++i) {
                PyrSlot keyslot;
                SetSymbol(&keyslot, methArgNames[i]);
                identDict_lookupNonNil(optionalEnvirLookup, &keyslot, calcHash(&keyslot), &resultingCallFrame[i]);
            }
        }
    }

    // Deal with the keywords.
    for (size_t i = 0; i < numKwArgsSupplied * 2; i += 2) {
        PyrSymbol* argKeyword = slotRawSymbol(pushedKeywords + i);
        PyrSlot* argValue = pushedKeywords + i + 1;
        // If found in method.
        if (const auto argCollisionIndex = findArgNameCollision(argKeyword); argCollisionIndex.has_value()) {
            if (*argCollisionIndex < numNormalArgsAdded) { // already pushed
                if (!isMethod)
                    post("Ignoring duplicate keyword '%s' at position %d found in function call\n", argKeyword->name,
                         *argCollisionIndex);
                else {
                    auto* method = reinterpret_cast<PyrMethod*>(block);
                    post("Ignoring duplicate keyword '%s' at position %d found in call to %s:%s\n", argKeyword->name,
                         *argCollisionIndex, slotRawSymbol(&slotRawClass(&method->ownerclass)->name)->name,
                         slotRawSymbol(&method->name)->name);
                }
            } else {
                resultingCallFrame[*argCollisionIndex] = *argValue;
            }

        } else if (methHasKwArg) {
            if (keywordArgumentsArray == nullptr)
                keywordArgumentsArray = newPyrArray(g->gc, static_cast<int>(numKwArgsSupplied) * 2, 0, false);
            SetSymbol(keywordArgumentsArray->slots + keywordArgumentSize, argKeyword);
            ++keywordArgumentSize;
            *(keywordArgumentsArray->slots + keywordArgumentSize) = *argValue;
            ++keywordArgumentSize;
        } else if (gKeywordError) {
            if (isMethod)
                kwArgMismatchErrorMethod(argKeyword->name, block);
            else
                kwArgMismatchErrorBlock(argKeyword->name, block);
        }
    }

    // Put var arg array or default on stack.
    if (methHasVarArg)
        SetObject(resultingCallFrame + methNumNormArgs,
                  variableArgumentsArray ? variableArgumentsArray : slotRawObject(&proto->slots[methNumNormArgs]));

    // Put var kwarg array or default on stack.
    if (methHasKwArg) {
        if (keywordArgumentsArray) {
            keywordArgumentsArray->size = keywordArgumentSize;
            SetObject(resultingCallFrame + methNumNormArgs + 1, keywordArgumentsArray);
        } else {
            SetObject(resultingCallFrame + methNumNormArgs + 1, slotRawObject(&proto->slots[methNumNormArgs + 1]));
        }
    }

    // Push default variable values to call_frame.
    std::copy(proto->slots + methNumActualArgs, proto->slots + methNumActualArgs + methNumVariables,
              resultingCallFrame + methNumActualArgs);
}

HOT void executeMethod(VMGlobals* g, PyrBlock* meth, long totalNumArgsPushed, long numKwArgsPushed) {
    prepForTailCall(g);
    const GCSanityChecker gc_sanity_checker(g, "executeMethod");

    auto callFrame = createFrameToCall(g, meth);
    prepareArgsForExecute(g, meth, callFrame, totalNumArgsPushed, numKwArgsPushed, true);

    g->execMethod = numKwArgsPushed == 0 ? 20 : 10;
    g->method = (PyrMethod*)meth;
    g->ip = slotRawInt8Array(&meth->code)->b - 1;
    g->frame = callFrame;
    g->block = (PyrBlock*)meth;
    g->sp -= totalNumArgsPushed;
    slotCopy(&g->receiver, callFrame->vars);
}

void switchToThread(VMGlobals* g, PyrThread* newthread, int oldstate, int* numArgsPushed);

HOT void returnFromBlock(VMGlobals* g) {
    const GCSanityChecker gc_sanity_checker(g, "returnFromBlock");
    PyrFrame* curframe;
    PyrFrame* returnFrame;
    PyrFrame* homeContext;
    PyrBlock* block;
    PyrMethod* meth;
    PyrMethodRaw* methraw;
    PyrMethodRaw* blockraw;

    // if (gTraceInterpreter) postfl("->returnFromBlock\n");
    // printf("->returnFromBlock\n");
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
    CallStackSanity(g, "returnFromBlock");
#endif
    curframe = g->frame;

    // again:
    returnFrame = slotRawFrame(&curframe->caller);
    if (returnFrame) {
        block = slotRawBlock(&curframe->method);
        blockraw = METHRAW(block);

        g->frame = returnFrame;
        g->ip = (unsigned char*)slotRawPtr(&returnFrame->ip);
        g->block = slotRawBlock(&returnFrame->method);
        homeContext = slotRawFrame(&returnFrame->homeContext);
        meth = slotRawMethod(&homeContext->method);
        methraw = METHRAW(meth);
        slotCopy(&g->receiver, &homeContext->vars[0]); //??
        g->method = meth;

        meth = slotRawMethod(&curframe->method);
        methraw = METHRAW(meth);
        if (!methraw->needsHeapContext) {
            g->gc->Free(curframe);
        } else {
            SetInt(&curframe->caller, 0);
        }

    } else {
        ////// this should never happen .
        error("return from Function at top of call stack.\n");
        g->method = nullptr;
        g->block = nullptr;
        g->frame = nullptr;
        g->sp = g->gc->Stack()->slots - 1;
        longjmp(g->escapeInterpreter, 1);
    }
    // if (gTraceInterpreter) postfl("<-returnFromBlock\n");
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
    CallStackSanity(g, "returnFromBlock");
#endif
}


HOT void returnFromMethod(VMGlobals* g) {
    GCSanityChecker gc_sanity_checker(g, "returnFromMethod");
    PyrFrame *returnFrame, *curframe, *homeContext;
    PyrMethod* meth;
    PyrMethodRaw* methraw;
    curframe = g->frame;

    homeContext = slotRawFrame(&slotRawFrame(&curframe->context)->homeContext);
    if (homeContext == nullptr) {
    null_return:
#if TAILCALLOPTIMIZE
        if (g->tailCall)
            return; // do nothing.
#endif

        /*
            static bool once = true;
            if (once || gTraceInterpreter)
            {
                once = false;
                post("return all the way out. sd %d\n", g->sp - g->gc->Stack()->slots);
                postfl("%s:%s\n",
                    slotRawClass(&g->method->ownerclass)->name.us->name, g->slotRawSymbol(&method->name)->name
                );
                post("tailcall %d\n", g->tailCall);
                post("homeContext %p\n", homeContext);
                post("returnFrame %p\n", returnFrame);
                dumpObjectSlot(&homeContext->caller);
                DumpStack(g, g->sp);
                DumpBackTrace(g);
            }
            gTraceInterpreter = false;
        */
        // if (IsNil(&homeContext->caller)) return; // do nothing.

        // return all the way out.
        PyrSlot* bottom = g->gc->Stack()->slots;
        slotCopy(bottom, g->sp);
        g->sp = bottom; // ??!! pop everybody
        g->method = nullptr;
        g->block = nullptr;
        g->frame = nullptr;
        longjmp(g->escapeInterpreter, 2);
    } else {
        returnFrame = slotRawFrame(&homeContext->caller);

        if (returnFrame == nullptr)
            goto null_return;
        // make sure returnFrame is a caller and find earliest stack frame
        {
            PyrFrame* tempFrame = curframe;
            while (tempFrame != returnFrame) {
                tempFrame = slotRawFrame(&tempFrame->caller);
                if (!tempFrame) {
                    if (isKindOf((PyrObject*)g->thread, class_routine) && NotNil(&g->thread->parent)) {
                        // not found, so yield to parent thread and continue searching.
                        PyrSlot value;
                        slotCopy(&value, g->sp);

                        int numArgsPushed = 1;
                        switchToThread(g, slotRawThread(&g->thread->parent), tSuspended, &numArgsPushed);

                        // on the other side of the looking glass, put the yielded value on the stack as the result..
                        g->sp -= numArgsPushed - 1;
                        slotCopy(g->sp, &value);

                        curframe = tempFrame = g->frame;
                    } else {
                        slotCopy(&g->sp[2], &g->sp[0]);
                        slotCopy(g->sp, &g->receiver);
                        g->sp++;
                        SetObject(g->sp, g->method);
                        g->sp++;
                        sendMessage(g, getsym("outOfContextReturn"), 3, 0);
                        return;
                    }
                }
            }
        }

        {
            PyrFrame* tempFrame = curframe;
            while (tempFrame != returnFrame) {
                meth = slotRawMethod(&tempFrame->method);
                methraw = METHRAW(meth);
                PyrFrame* nextFrame = slotRawFrame(&tempFrame->caller);
                if (!methraw->needsHeapContext) {
                    SetInt(&tempFrame->caller, 0);
                } else {
                    if (tempFrame != homeContext)
                        SetInt(&tempFrame->caller, 0);
                }
                tempFrame = nextFrame;
            }
        }

        // return to it
        g->ip = (unsigned char*)slotRawPtr(&returnFrame->ip);
        g->frame = returnFrame;
        g->block = slotRawBlock(&returnFrame->method);

        homeContext = slotRawFrame(&returnFrame->homeContext);
        meth = slotRawMethod(&homeContext->method);
        methraw = METHRAW(meth);

#if DEBUGMETHODS
        if (gTraceInterpreter) {
            postfl("%s:%s <- %s:%s\n", slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name,
                   slotRawSymbol(&meth->name)->name, slotRawSymbol(&slotRawClass(&g->method->ownerclass)->name)->name,
                   slotRawSymbol(&g->method->name)->name);
        }
#endif

        g->method = meth;
        slotCopy(&g->receiver, &homeContext->vars[0]);
    }
}


int keywordFixStack(VMGlobals* g, PyrMethod* meth, PyrMethodRaw* methraw, long allArgsPushed, long numKeyArgsPushed) {
    PyrSlot *pslot, *qslot;
    long i, j, m, diff, numArgsPushed, numArgsNeeded;

    if (numKeyArgsPushed) {
        // evacuate keyword args to separate area
        pslot = temporaryKeywordStack + (numKeyArgsPushed << 1);
        qslot = g->sp + 1;
        for (m = 0; m < numKeyArgsPushed; ++m) {
            *--pslot = *--qslot;
            *--pslot = *--qslot;
        }
    }

    PyrSlot* vars = g->sp - allArgsPushed + 1;

    numArgsPushed = allArgsPushed - (numKeyArgsPushed << 1);
    numArgsNeeded = methraw->numargs;
    diff = numArgsNeeded - numArgsPushed;
    if (diff > 0) { // not enough args
        pslot = vars + numArgsPushed - 1;
        qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
        for (m = 0; m < diff; ++m)
            slotCopy(++pslot, ++qslot);
        numArgsPushed = numArgsNeeded;
    }

    // do keyword lookup:
    if (numKeyArgsPushed && methraw->posargs) {
        PyrSymbol** name0 = slotRawSymbolArray(&meth->argNames)->symbols + 1;
        PyrSlot* key = temporaryKeywordStack;
        for (i = 0; i < numKeyArgsPushed; ++i, key += 2) {
            PyrSymbol** name = name0;
            for (j = 1; j < methraw->posargs; ++j, ++name) {
                if (*name == slotRawSymbol(key)) {
                    slotCopy(&vars[j], &key[1]);
                    goto found;
                }
            }
            if (gKeywordError)
                kwArgMismatchErrorMethod(slotRawSymbol(key)->name, meth);
        found:;
        }
    }

    g->sp += numArgsPushed - allArgsPushed;
    return numArgsPushed;
}
