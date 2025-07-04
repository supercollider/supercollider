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
#include "PredefinedSymbols.h"
#include "PyrObjectProto.h"
#include "SCBase.h"

#include <optional>
#include <csetjmp>

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

std::int64_t cvxUniqueMethods;
extern int ivxIdentDict_array;

void StoreToImmutableB(VMGlobals* g, PyrSlot*& sp, unsigned char*& ip);

void initUniqueMethods() {
    PyrClass* dummyclass;
    cvxUniqueMethods = classVarOffset("Object", "uniqueMethods", &dummyclass);
}

HOT void sendMessageImpl(VMGlobals* g, PyrSymbol* selector, PyrSlot* recvrSlot, PyrClass* classobj,
                         std::int64_t numArgsPushed, std::int64_t numKeyArgsPushed) {
    ResetTailCall reset_tail_call { g };

    // General strategy here is to switch on the method type, which add different optimizations.
    // In cases of redirect and super calls the function to be recursively call, this is implemented with a goto.

lookup_again:
    // The variables classobj and selector are updated when the goto is triggered, causing these to change.
    if ((selector->flags & sym_Class) != 0) {
        // if selector is a class name, a lookup in gRowTable would be indexed out of bounds
        doesNotUnderstand(g, selector, numArgsPushed, numKeyArgsPushed);
        return;
    }
    auto method = gRowTable[slotRawInt(&classobj->classIndex) + selector->u.index];
    auto methodRaw = METHRAW(method);

    if (slotRawSymbol(&method->name) != selector) {
        doesNotUnderstand(g, selector, numArgsPushed, numKeyArgsPushed);
        return;
    }

    const auto pushDefaultArgsIfNotEnoughSupplied = [&]() {
        auto defaultArgs = slotRawObject(&method->prototypeFrame)->slots;
        std::copy(defaultArgs + numArgsPushed, defaultArgs + methodRaw->numargs, g->sp + 1);
        g->sp += methodRaw->numargs - numArgsPushed; // fix stack pointer to point to last arg pushed.
        numArgsPushed = methodRaw->numargs;
    };

    const auto applyKeywords = [&]() {
        numArgsPushed = keywordFixStack(g, method, methodRaw, numArgsPushed, numKeyArgsPushed);
        numKeyArgsPushed = 0;
    };

    // There are multiple types of methods.
    // The basic is methNormal which goes through executeMethod.
    // The rest are some form of optimization.
    switch (methodRaw->methType) {
    case methNormal:
        return executeMethod(g, method, numArgsPushed, numKeyArgsPushed);

    case methReturnSelf: {
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        return;
    }

    case methReturnLiteral: {
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        slotCopy(g->sp, &method->selectors); // Set the first slot to the literal.
        return;
    }

    case methReturnArg: {
        if (numKeyArgsPushed > 0)
            applyKeywords();
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        const auto classIndex = methodRaw->specialIndex; // Zero is index of the first argument.
        if (classIndex < numArgsPushed)
            slotCopy(g->sp, g->sp + classIndex);
        else
            slotCopy(g->sp, &slotRawObject(&method->prototypeFrame)->slots[classIndex]);
        return;
    }

    case methReturnInstVar: { // Return instance variable.
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        slotCopy(g->sp, &slotRawObject(recvrSlot)->slots[methodRaw->specialIndex]);
        return;
    }

    case methAssignInstVar: { // Assign to object instance variable
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        auto obj = slotRawObject(recvrSlot);
        if (obj->IsImmutable()) {
            StoreToImmutableB(g, g->sp, g->ip);
            return;
        }
        const auto classIndex = methodRaw->specialIndex;
        if (numArgsPushed >= 2) {
            slotCopy(&obj->slots[classIndex], g->sp + 1);
            g->gc->GCWrite(obj, g->sp + 1);
        } else {
            SetNil(&obj->slots[classIndex]);
        }
        slotCopy(g->sp, recvrSlot);
        return;
    }

    case methReturnClassVar: { // Return a classvar.
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        slotCopy(g->sp, &g->classvars->slots[methodRaw->specialIndex]);
        return;
    }

    case methAssignClassVar: { // Assign classvar.
        g->sp -= numArgsPushed - 1; // Remove all args but the receiver.
        if (numArgsPushed >= 2) {
            slotCopy(&g->classvars->slots[methodRaw->specialIndex], g->sp + 1);
            g->gc->GCWrite(g->classvars, g->sp + 1);
        } else {
            SetNil(&g->classvars->slots[methodRaw->specialIndex]);
        }
        slotCopy(g->sp, recvrSlot);
        return;
    }

    case methRedirect: { // Send a different selector to this, e.g. this.subclassResponsibility.
        if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgsIfNotEnoughSupplied();
        if (numKeyArgsPushed > 0)
            applyKeywords();
        selector = slotRawSymbol(&method->selectors);
        goto lookup_again;
    }

    case methRedirectSuper: { // Send a different selector to super, e.g. super.subclassResponsibility.
        if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgsIfNotEnoughSupplied();
        if (numKeyArgsPushed > 0)
            applyKeywords();
        selector = slotRawSymbol(&method->selectors);
        classobj = slotRawSymbol(&slotRawClass(&method->ownerclass)->superclass)->u.classobj;
        goto lookup_again;
    }

    case methForwardInstVar: { // Forward to an object instance variable, e.g., ^foo.bar.
        if (numKeyArgsPushed > 0)
            applyKeywords();
        else if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgsIfNotEnoughSupplied();
        selector = slotRawSymbol(&method->selectors);
        slotCopy(recvrSlot, &slotRawObject(recvrSlot)->slots[methodRaw->specialIndex]);
        classobj = classOfSlot(recvrSlot);
        goto lookup_again;
    }

    case methForwardClassVar: { // Forward to a class variable, e.g., ^Class.foo.bar.
        if (numKeyArgsPushed > 0)
            applyKeywords();
        else if (numArgsPushed < methodRaw->numargs)
            pushDefaultArgsIfNotEnoughSupplied();
        selector = slotRawSymbol(&method->selectors);
        slotCopy(recvrSlot, &g->classvars->slots[methodRaw->specialIndex]);
        classobj = classOfSlot(recvrSlot);
        goto lookup_again;
    }

    case methPrimitive: { // Some primitive, e.g., _SomeThing.
        if (numKeyArgsPushed > 0)
            doPrimitiveWithKeys(g, method, static_cast<int>(numArgsPushed), static_cast<int>(numKeyArgsPushed));
        else
            doPrimitive(g, method, static_cast<int>(numArgsPushed));
        return;
    }
    }
}

HOT void sendMessage(VMGlobals* g, PyrSymbol* selector, std::int64_t numArgsPushed, std::int64_t numKeyArgsPushed) {
    const GCSanityChecker gc_sanity_checker(g, "sendMessage");

    auto recvrSlot = g->sp - numArgsPushed + 1;
    auto classobj = classOfSlot(recvrSlot);
    sendMessageImpl(g, selector, recvrSlot, classobj, numArgsPushed, numKeyArgsPushed);
}


HOT void sendSuperMessage(VMGlobals* g, PyrSymbol* selector, std::int64_t numArgsPushed,
                          std::int64_t numKeyArgsPushed) {
    const GCSanityChecker gc_sanity_checker(g, "sendSuperMessage");

    auto recvrSlot = g->sp - numArgsPushed + 1;
    auto classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
    sendMessageImpl(g, selector, recvrSlot, classobj, numArgsPushed, numKeyArgsPushed);
}


extern PyrClass* class_identdict;

// An optimization for unique methods.
// Returns false if it isn't a unique method.
bool tryUniqueMethod(VMGlobals* g, PyrMethod* meth, PyrSlot* receiverSlot, PyrSlot* selectorSlot,
                     std::int64_t numArgsPushed, std::int64_t numKeyArgsPushed) {
    if (slotRawClass(&meth->ownerclass) != class_object)
        return false;
    auto uniqueMethodSlot = &g->classvars->slots[cvxUniqueMethods];
    if (!isKindOfSlot(uniqueMethodSlot, class_identdict))
        return false;
    auto uniqueMethodDictArraySlot = slotRawObject(uniqueMethodSlot)->slots + ivxIdentDict_array;
    if (!IsObj(uniqueMethodDictArraySlot))
        return false;
    auto uniqueMethodDictArray = slotRawObject(uniqueMethodDictArraySlot);
    if (uniqueMethodDictArray->classptr != class_array)
        return false;
    auto uniqueMethodOfReceiverIndex = arrayAtIdentityHashInPairs(uniqueMethodDictArray, receiverSlot);
    if (uniqueMethodOfReceiverIndex < 0)
        return false;
    auto receiverUniqueMethodsKey = uniqueMethodDictArray->slots + uniqueMethodOfReceiverIndex;
    if (IsNil(receiverUniqueMethodsKey))
        return false;
    auto receiverUniqueMethodsValue = receiverUniqueMethodsKey + 1;
    if (!isKindOfSlot(receiverUniqueMethodsValue, class_identdict))
        return false;
    auto receiverUniqueMethodsArraySlot = slotRawObject(receiverUniqueMethodsValue)->slots + ivxIdentDict_array;
    if (!IsObj(receiverUniqueMethodsArraySlot))
        return false;
    auto receiverUniqueMethodsArray = slotRawObject(receiverUniqueMethodsArraySlot);
    if (receiverUniqueMethodsArray->classptr != class_array)
        return false;
    auto selectorIndex = arrayAtIdentityHashInPairs(receiverUniqueMethodsArray, selectorSlot);
    if (selectorIndex < 0)
        return false;
    auto selectorMethodKey = receiverUniqueMethodsArray->slots + selectorIndex;
    if (IsNil(selectorMethodKey))
        return false;
    auto selectorMethodValue = selectorMethodKey + 1;
    slotCopy(selectorSlot, receiverSlot);
    slotCopy(receiverSlot, selectorMethodValue);
    blockValueWithKeys(g, static_cast<int>(numArgsPushed + 1), static_cast<int>(numKeyArgsPushed));
    return true;
}

void doesNotUnderstand(VMGlobals* g, PyrSymbol* selector, std::int64_t numArgsPushed, std::int64_t numKeyArgsPushed) {
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


void kwArgMismatchErrorMethod(const char* argName, void* ptr) {
    auto meth = reinterpret_cast<PyrMethod*>(ptr);
    post("WARNING: keyword arg '%s' not found in call to %s:%s\n", argName,
         slotRawSymbol(&slotRawClass(&meth->ownerclass)->name)->name, slotRawSymbol(&meth->name)->name);
}
void kwArgMismatchErrorBlock(const char* argName, void*) {
    post("WARNING: keyword arg '%s' not found in call to function.\n", argName);
}

// This function puts the arguments into the passed in callFrame ready to be called.
// It will also lookup arguments in the environment if one is passed in.
void prepareArgsForExecute(VMGlobals* g, PyrBlock* block, PyrFrame* callFrame, std::int64_t totalSuppliedArgs,
                           std::int64_t numKwArgsSupplied, bool isMethod, PyrObject* optionalEnvirLookup) {
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

    PyrSlot* outCallFrameStack = callFrame->vars;

    PyrSlot* pushedArgs = g->sp - (totalSuppliedArgs - 1);
    PyrSlot* pushedKeywords = g->sp - (numKwArgsSupplied * 2 - 1);

    if (methNumActualArgs == 0) {
        std::copy(proto->slots, proto->slots + methNumVariables, outCallFrameStack);
        return;
    }

    // Optional arrays for variable arguments and keyword arguments.
    PyrObject* variableArgumentsArray = nullptr;
    PyrObject* keywordArgumentsArray = nullptr;
    int keywordArgumentSize = 0;

    const auto findKeywordArgIndex = [=](const PyrSymbol* argName) -> std::optional<uint32_t> {
        for (size_t namei = 0; namei < methNumNormArgs; ++namei) {
            const auto indexOffsetFromThis = namei + (isMethod ? 1 : 0);
            if (methArgNames[indexOffsetFromThis] == argName)
                return indexOffsetFromThis;
        }
        return std::nullopt;
    };

    // Number that will actually be pushed to resulting call frame.
    const auto numNormalArgsAdded = std::min<uint32>(numNormalSuppliedArgs, methNumNormArgs);
    std::copy(pushedArgs, pushedArgs + numNormalArgsAdded, outCallFrameStack);

    if (numNormalSuppliedArgs > methNumNormArgs && methHasVarArg) {
        // Too many args.
        // Put extra in variable array if method supports it, otherwise drop them.
        const auto size = static_cast<int>(numNormalSuppliedArgs) - methNumNormArgs;
        variableArgumentsArray = newPyrArray(g->gc, size, 0, false);
        variableArgumentsArray->size = size;
        std::copy(pushedArgs + numNormalArgsAdded, pushedArgs + numNormalSuppliedArgs, variableArgumentsArray->slots);

    } else if (numNormalSuppliedArgs < methNumNormArgs) {
        // Too few args, push defaults.
        std::copy(proto->slots + numNormalArgsAdded, proto->slots + methNumNormArgs,
                  outCallFrameStack + numNormalArgsAdded);

        // This is the only case where the optional environment is used to lookup arguments.
        if (optionalEnvirLookup)
            for (auto i = numNormalArgsAdded; i < methNumNormArgs; ++i) {
                PyrSlot keyslot;
                SetSymbol(&keyslot, methArgNames[i]);
                identDict_lookupNonNil(optionalEnvirLookup, &keyslot, calcHash(&keyslot), &outCallFrameStack[i]);
            }
    }

    // Deal with the keywords.
    for (size_t i = 0; i < numKwArgsSupplied * 2; i += 2) {
        PyrSymbol* argKeyword = slotRawSymbol(pushedKeywords + i);
        PyrSlot* argValue = pushedKeywords + i + 1;

        // If found in method.
        if (const auto argIndex = findKeywordArgIndex(argKeyword); argIndex.has_value()) {
            outCallFrameStack[*argIndex] = *argValue;

            // SC docs show that in the case of colliding args, the last one added should always be the result.
            if (*argIndex < numNormalArgsAdded) {
                // Already pushed, duplicate keyword found, update value, post warning.
                if (!isMethod) {
                    post("Duplicate keyword '%s' at position %d found in function call\n", argKeyword->name, *argIndex);
                } else {
                    auto* method = reinterpret_cast<PyrMethod*>(block);
                    post("Duplicate keyword '%s' at position %d found in call to %s:%s\n", argKeyword->name, *argIndex,
                         slotRawSymbol(&slotRawClass(&method->ownerclass)->name)->name,
                         slotRawSymbol(&method->name)->name);
                }
            }
        } else if (methHasKwArg) {
            // Allocate if nullptr.
            keywordArgumentsArray = keywordArgumentsArray
                ? keywordArgumentsArray
                : newPyrArray(g->gc, static_cast<int>(numKwArgsSupplied) * 2, 0, false);
            // Add keyword and value to keywordArgumentsArray.
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
    if (methHasVarArg) {
        if (variableArgumentsArray)
            SetObject(outCallFrameStack + methNumNormArgs, variableArgumentsArray);
        else
            slotCopy(outCallFrameStack + methNumNormArgs, proto->slots + methNumNormArgs);
    }

    // Put var kwarg array or default on stack.
    if (methHasKwArg) {
        if (keywordArgumentsArray) {
            keywordArgumentsArray->size = keywordArgumentSize;
            SetObject(outCallFrameStack + methNumNormArgs + 1, keywordArgumentsArray);
        } else {
            slotCopy(outCallFrameStack + methNumNormArgs + 1, proto->slots + methNumNormArgs + 1);
        }
    }

    // Push default variable values to call_frame.
    std::copy(proto->slots + methNumActualArgs, proto->slots + methNumActualArgs + methNumVariables,
              outCallFrameStack + methNumActualArgs);
}

inline PyrFrame* createFrameForExecuteMethod(VMGlobals* g, PyrBlock* block) {
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

HOT void executeMethod(VMGlobals* g, PyrBlock* meth, std::int64_t totalNumArgsPushed, std::int64_t numKwArgsPushed) {
    prepForTailCall(g);
    const GCSanityChecker gc_sanity_checker(g, "executeMethod");

    auto callFrame = createFrameForExecuteMethod(g, meth);
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


int keywordFixStack(VMGlobals* g, PyrMethod* meth, PyrMethodRaw* methraw, std::int64_t allArgsPushed,
                    std::int64_t numKeyArgsPushed) {
    PyrSlot *pslot, *qslot;
    std::int64_t i, j, m, diff, numArgsPushed, numArgsNeeded;

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
    return static_cast<int>(numArgsPushed);
}
