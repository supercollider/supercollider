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


#include "Opcodes.h"
#include "PyrInterpreter.h"
#include "PyrPrimitive.h"
#include "PyrPrimitiveProto.h"
#include "PyrMathPrim.h"
#include "PyrListPrim.h"
#include "PyrKernel.h"
#include "PyrMessage.h"
#include "PyrParseNode.h"
#include "PyrSignal.h"
#include "PyrSched.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrKernelProto.h"
#include "PyrSymbolTable.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "SpecialSelectorsOperatorsAndClasses.h"

#include <boost/chrono.hpp>

#include <float.h>
#define kBigBigFloat DBL_MAX
#define kSmallSmallFloat DBL_MIN


#include <new>
#include "InitAlloc.h"
#include "function_attributes.h"

using namespace Opcode;

#ifdef __clang__
#    pragma clang diagnostic ignored "-Warray-bounds"
#endif


// void tellPlugInsAboutToRun();
double timeNow();


int32 timeseed() {
    using namespace std::chrono;

    high_resolution_clock::time_point now = high_resolution_clock::now();
    high_resolution_clock::duration since_epoch = now.time_since_epoch();

    seconds secs = duration_cast<seconds>(since_epoch);
    nanoseconds nanosecs = since_epoch - secs;

    int_least64_t seed = secs.count() ^ nanosecs.count();

    return (int32)seed;
}

VMGlobals gVMGlobals;
VMGlobals* gMainVMGlobals = &gVMGlobals;

void debugf(char* fmt, ...);

#define DEBUGINTERPRETER 0
#define METHODMETER 0
#define BCSTAT 0
#define CHECK_MAX_STACK_USE 0


#if CHECK_MAX_STACK_USE
int gMaxStackDepth = 0;
#endif

unsigned char* dumpOneByteCode(PyrBlock* theBlock, PyrClass* theClass, unsigned char* ip);
void dumpSlotOneWord(const char* tagstr, PyrSlot* slot);
// bool checkAllObjChecksum(PyrObject* obj);

bool gTraceInterpreter = false;
// bool gTraceInterpreter = true;


const char* byteCodeString(int code);

extern int gNumClasses;
extern PyrClass* gClassList;

// runInterpreter has 7 call sites:
//	compileLibrary (4)
// 	runLibrary
// 	interpretCmdLine

static void endInterpreter(VMGlobals* g);


SCLANG_DLLEXPORT_C void runInterpreter(VMGlobals* g, PyrSymbol* selector, int numArgsPushed) {
    // postfl("->runInterpreter\n");
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
#endif
    // postfl(" >initInterpreter\n");

    if (initInterpreter(g, selector, numArgsPushed)) {
#ifdef GC_SANITYCHECK
        g->gc->SanityCheck();
#endif
        //        if (strcmp(selector->name, "tick") != 0) post("%s %d  execMethod %d\n", selector->name, numArgsPushed,
        //        g->execMethod);
        // post("->Interpret thread %p\n", g->thread);
        if (g->execMethod)
            Interpret(g);
            // post("<-Interpret thread %p\n", g->thread);
#ifdef GC_SANITYCHECK
        g->gc->SanityCheck();
#endif
    }

    // postfl(" >endInterpreter\n");
    endInterpreter(g);
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
#endif
    // postfl("<-runInterpreter\n");
    // dumpGCStats(g->gc);
}

static bool initAwakeMessage(VMGlobals* g);

void runAwakeMessage(VMGlobals* g) {
    if (initAwakeMessage(g)) {
        if (g->execMethod)
            Interpret(g);
    }
    endInterpreter(g);
}

void initPyrThread(VMGlobals* g, PyrThread* thread, PyrSlot* func, int stacksize, PyrInt32Array* rgenArray,
                   double beats, double seconds, PyrSlot* clock, bool runGC);
int32 timeseed();

PyrProcess* newPyrProcess(VMGlobals* g, PyrClass* procclassobj) {
    PyrGC* gc = g->gc;
    PyrProcess* proc = (PyrProcess*)instantiateObject(gc, procclassobj, 0, true, false);

    PyrObject* sysSchedulerQueue = newPyrArray(gc, 4096, 0, false);
    sysSchedulerQueue->size = 1;
    SetInt(sysSchedulerQueue->slots + 0, 0); // stability count
    SetObject(&proc->sysSchedulerQueue, sysSchedulerQueue);

    PyrObject* classVars = newPyrArray(gc, gNumClassVars, 0, false);
    classVars->size = gNumClassVars;
    nilSlots(classVars->slots, gNumClassVars);
    SetObject(&proc->classVars, classVars);
    g->classvars = classVars;

    // fill class vars from prototypes:
    PyrClass* classobj = gClassList;
    while (classobj) {
        if (IsObj(&classobj->cprototype)) {
            int numClassVars = slotRawObject(&classobj->cprototype)->size;
            if (numClassVars > 0) {
                memcpy(g->classvars->slots + slotRawInt(&classobj->classVarIndex),
                       slotRawObject(&classobj->cprototype)->slots, numClassVars * sizeof(PyrSlot));
            }
        }
        classobj = slotRawClass(&classobj->nextclass);
    }

    SetNil(&proc->nowExecutingPath);

    class_thread = getsym("Thread")->u.classobj;
    if (class_thread) {
        SetNil(&proc->curThread);
        PyrThread* thread = (PyrThread*)instantiateObject(gc, class_thread, 0, true, false);
        // SetObject(&threadsArray->slots[0], thread);
        SetObject(&proc->mainThread, thread);
        PyrInt32Array* rgenArray = newPyrInt32Array(gc, 4, 0, false);
        rgenArray->size = 4;
        rgenArray->i[0] = 0;
        rgenArray->i[1] = 0;
        rgenArray->i[2] = 0;
        rgenArray->i[3] = 0;
        ((RGen*)(rgenArray->i))->init(timeseed());

        PyrSlot clockSlot;
        SetObject(&clockSlot, s_systemclock->u.classobj);
        initPyrThread(g, thread, &o_nil, EVALSTACKDEPTH, rgenArray, 0., 0., &clockSlot, false);
        // postfl("elapsedTime %.6f\n", elapsedTime());
    } else {
        error("Class Thread not found.\n");
    }

    PyrSymbol* contextsym = getsym("functionCompileContext");
    size_t index = slotRawInt(&class_interpreter->classIndex) + contextsym->u.index;
    PyrMethod* meth = gRowTable[index];
    if (!meth || slotRawSymbol(&meth->name) != contextsym) {
        error("compile context method 'functionCompileContext' not found.\n");
        // SetNil(&proc->interpreter);
    } else {
        PyrObject* proto;
        PyrFrame* frame;
        PyrMethodRaw* methraw;

        PyrInterpreter* interpreter = (PyrInterpreter*)instantiateObject(gc, class_interpreter, 0, true, false);
        SetObject(&proc->interpreter, interpreter);
        proto = slotRawObject(&meth->prototypeFrame);

        methraw = METHRAW(meth);
        frame = (PyrFrame*)gc->New(methraw->frameSize, 0, obj_slot, false);
        frame->classptr = class_frame;
        frame->size = FRAMESIZE + proto->size; /// <- IS THIS WRONG ??
        SetObject(&frame->method, meth);
        SetObject(&frame->homeContext, frame);
        SetInt(&frame->caller, 0);
        SetNil(&frame->context);
        SetPtr(&frame->ip, nullptr);
        SetObject(&frame->vars[0], interpreter);

        SetObject(&interpreter->context, frame);
    }

    return proc;
}


#if BCSTAT
int prevop = 0;
int bcstat[256];
int bcpair[256][256];

void clearbcstat();
void clearbcstat() {
    int i, j;
    for (i = 0; i < 256; ++i) {
        bcstat[i] = 0;
        for (j = 0; j < 256; ++j) {
            bcpair[i][j] = 0;
        }
    }
}

void dumpbcstat();
void dumpbcstat() {
    FILE* file;
    int i, j, k, total;

    file = fopen("bcstat", "w");
    if (file) {
        fprintf(file, "----------\n");
        total = 0;
        for (i = 0; i < 256; ++i) {
            total += bcstat[i];
            if (bcstat[i])
                fprintf(file, "%3d %8d  %-32s\n", i, bcstat[i], byteCodeString(i));
        }
        fprintf(file, "\ntotal %d\n", total);
        fprintf(file, "-----cur,next-----\n");
        for (i = 0, k = 0; i < 256; ++i) {
            for (j = 0; j < 256; j++) {
                if (bcpair[i][j] > 0) {
                    fprintf(file, "%4d %3d %3d %8d %-32s %-32s\n", k++, i, j, bcpair[i][j], byteCodeString(i),
                            byteCodeString(j));
                }
            }
        }
        fprintf(file, "-----cur,prev-----\n");
        for (i = 0, k = 0; i < 256; ++i) {
            for (j = 0; j < 256; j++) {
                if (bcpair[j][i] > 0) {
                    fprintf(file, "%4d %3d %3d %8d %-32s %-32s\n", k++, i, j, bcpair[j][i], byteCodeString(i),
                            byteCodeString(j));
                }
            }
        }
    }
    fclose(file);
}
#endif

void initPatterns();
void initThreads();
void initGUI();

#ifndef _WIN32
bool running = true;
static void handleSigUsr1(int param) {
    printf("handleSigUsr1()...\n");
    running = false;
}
#endif

template <typename F> void ifTailCallOptimise(F&& f) {
#if TAILCALLOPTIMIZE
    std::forward<F>(f)();
#endif
}

bool initRuntime(VMGlobals* g, int poolSize, AllocPool* inPool) {
    /*
        create a GC environment
        create a vmachine instance of main
        initialize VMGlobals
    */

    PyrClass* class_main = s_main->u.classobj;

    if (!class_main) {
        error("Class 'Main' not defined.\n");
        return false;
    }
    if (!isSubclassOf(class_main, class_process)) {
        error("Class Main is not a subclass of Process.\n");
        return false;
    }

    // create GC environment, process
    g->allocPool = inPool;
    g->gc = (PyrGC*)g->allocPool->Alloc(sizeof(PyrGC));
    MEMFAIL(g->gc);
    new (g->gc) PyrGC(g, g->allocPool, class_main, poolSize);
    g->thread = slotRawThread(&g->process->mainThread);
    SetObject(&g->receiver, g->process);

    // these will be set up when the run method is called
    g->method = nullptr;
    g->block = nullptr;
    g->frame = nullptr;
    g->ip = nullptr;

    // initialize process random number generator
    g->rgen = (RGen*)(slotRawObject(&g->thread->randData)->slots);

    initThreads();
    initPatterns();
    initUniqueMethods();
    initGUI();
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
#endif
    // tellPlugInsAboutToRun();

#ifndef _WIN32
    signal(SIGUSR1, handleSigUsr1);
#endif

    assert((g->gc->SanityCheck()));
#ifdef GC_SANITYCHECK
    g->gc->SanityCheck();
#endif

    return true;
}

static bool initAwakeMessage(VMGlobals* g) {
    // post("initAwakeMessage %p %p\n", g->thread, slotRawThread(&g->process->mainThread));
    slotCopy(&g->process->curThread, &g->process->mainThread); //??
    g->thread = slotRawThread(&g->process->mainThread); //??

    // these will be set up when the run method is called
    g->method = nullptr;
    g->block = nullptr;
    g->frame = nullptr;
    g->ip = nullptr;
    g->execMethod = 0;

    // set process as the receiver
    PyrSlot* slot = g->sp - 3;
    slotCopy(&g->receiver, slot);

    SetFloat(&g->thread->beats, slotRawFloat(&slot[1]));
    SetFloat(&g->thread->seconds, slotRawFloat(&slot[2]));
    slotCopy(&g->thread->clock, &slot[3]);
    g->gc->GCWrite(g->thread, slot + 3);

    // start it
    sendMessage(g, s_awake, 4, 0);

    return g->method != nullptr;
}

bool initInterpreter(VMGlobals* g, PyrSymbol* selector, int numArgsPushed) {
    slotCopy(&g->process->curThread, &g->process->mainThread);
    g->thread = slotRawThread(&g->process->mainThread);

    // these will be set up when the run method is called
    ifTailCallOptimise([&]() { g->tailCall = 0; });
    g->method = nullptr;
    g->block = nullptr;
    g->frame = nullptr;
    g->ip = nullptr;
    g->execMethod = 0;
    double elapsed = elapsedTime();
    SetFloat(&g->thread->beats, elapsed);
    SetFloat(&g->thread->seconds, elapsed);
    SetObject(&g->thread->clock, s_systemclock->u.classobj);
    g->gc->GCWrite(g->thread, s_systemclock->u.classobj);

    // set process as the receiver
    PyrSlot* slot = g->sp - numArgsPushed + 1;
    slotCopy(&g->receiver, slot);

    // start it
    sendMessage(g, selector, numArgsPushed, 0);

    return g->method != nullptr;
}


static void endInterpreter(VMGlobals* g) {
    slotCopy(&g->result, g->sp);
    //	dumpObjectSlot(&g->result);
    g->gc->Stack()->size = 0;
    g->sp = g->gc->Stack()->slots - 1;
    g->gc->LazyCollect();
}


static void StoreToImmutableA(VMGlobals* g, PyrSlot*& sp, unsigned char*& ip) {
    // only the value is on the stack
    slotCopy(sp + 1, sp); // copy value up one
    slotCopy(sp, &g->receiver); // put receiver in place
    sp++;
    g->sp = sp;
    g->ip = ip;
    post("StoreToImmutableA\n");
    dumpObjectSlot(sp - 1);
    dumpObjectSlot(sp);
    sendMessage(g, getsym("immutableError"), 2, 0);
    sp = g->sp;
    ip = g->ip;
}

void StoreToImmutableB(VMGlobals* g, PyrSlot*& sp, unsigned char*& ip) {
    // receiver and value are on the stack.
    sp++;
    g->sp = sp;
    g->ip = ip;
    post("StoreToImmutableB\n");
    dumpObjectSlot(sp - 1);
    dumpObjectSlot(sp);
    PyrSymbol* selector = getsym("immutableError");
    sendMessage(g, selector, 2, 0);
    sp = g->sp;
    ip = g->ip;
}


void dumpByteCodes(PyrBlock* theBlock);

static inline bool checkStackOverflow(VMGlobals* g, PyrSlot* sp) {
    PyrObject* stack = g->gc->Stack();
    int depth = sp - stack->slots;
    return depth < slotRawInt(&g->thread->stackSize);
}

static inline void checkStackDepth(VMGlobals* g, PyrSlot* sp) {
#if CHECK_MAX_STACK_USE
    int stackDepth = sp - g->sp + 1;
    if (stackDepth > gMaxStackDepth) {
        gMaxStackDepth = stackDepth;
        printf("gMaxStackDepth %d\n", gMaxStackDepth);
    }
#endif
}

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#    define LABELS_AS_VALUES
#endif

#ifdef LABELS_AS_VALUES
#    define dispatch_opcode                                                                                            \
        op1 = ip[1];                                                                                                   \
        ++ip;                                                                                                          \
        checkStackDepth(g, sp);                                                                                        \
        assert(checkStackOverflow(g, sp));                                                                             \
        goto* opcode_labels[op1]
#else
#    define dispatch_opcode break
#endif

#if defined(__GNUC__)
#    define LIKELY(x) __builtin_expect((x), 1)
#    define UNLIKELY(x) __builtin_expect((x), 0)
#else
#    define LIKELY(x) x
#    define UNLIKELY(x) x
#endif

#if defined(__GNUC__) && !defined(__clang__)
// gcc manual:
// Note: When compiling a program using computed gotos, a GCC extension, you may get better run-time performance if
//       you disable the global common subexpression elimination pass by adding -fno-gcse to the command line.
#    pragma GCC push_options
#    pragma GCC optimize("-fno-gcse")
#endif


/// Pass the name of an Opcode struct, e.g. InterpretOpcode(SendMsgX)
/// Setups of a switch case and a label for the computed gotos.
#define InterpretOpcode(NAME)                                                                                          \
    case NAME.code:                                                                                                    \
        opcode_label_##NAME:

#define InterpretExtendedOpcode(NAME) case Extended::NAME.code:

/// Pass the name of an Opcode struct that has 16 variants in the second nibble, e.g. InterpretOpcode16(PushInstVar)
/// Setups of a switch case and a label for the computed gotos.
#define InterpretOpcode16(NAME)                                                                                        \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
    case NAME.codeOffset<9>():                                                                                         \
    case NAME.codeOffset<10>():                                                                                        \
    case NAME.codeOffset<11>():                                                                                        \
    case NAME.codeOffset<12>():                                                                                        \
    case NAME.codeOffset<13>():                                                                                        \
    case NAME.codeOffset<14>():                                                                                        \
    case NAME.codeOffset<15>():                                                                                        \
        opcode_label_##NAME:

#define InterpretOpcode15(NAME)                                                                                        \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
    case NAME.codeOffset<9>():                                                                                         \
    case NAME.codeOffset<10>():                                                                                        \
    case NAME.codeOffset<11>():                                                                                        \
    case NAME.codeOffset<12>():                                                                                        \
    case NAME.codeOffset<13>():                                                                                        \
    case NAME.codeOffset<14>():                                                                                        \
        opcode_label_##NAME:

#define InterpretOpcode14(NAME)                                                                                        \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
    case NAME.codeOffset<9>():                                                                                         \
    case NAME.codeOffset<10>():                                                                                        \
    case NAME.codeOffset<11>():                                                                                        \
    case NAME.codeOffset<12>():                                                                                        \
    case NAME.codeOffset<13>():                                                                                        \
        opcode_label_##NAME:

#define InterpretOpcode7(NAME)                                                                                         \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
        opcode_label_##NAME:

#define InterpretOpcode8(NAME)                                                                                         \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
        opcode_label_##NAME:

#define InterpretOpcode9(NAME)                                                                                         \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
    case NAME.codeOffset<4>():                                                                                         \
    case NAME.codeOffset<5>():                                                                                         \
    case NAME.codeOffset<6>():                                                                                         \
    case NAME.codeOffset<7>():                                                                                         \
    case NAME.codeOffset<8>():                                                                                         \
        opcode_label_##NAME:

#define InterpretOpcode4(NAME)                                                                                         \
    case NAME.codeOffset<0>():                                                                                         \
    case NAME.codeOffset<1>():                                                                                         \
    case NAME.codeOffset<2>():                                                                                         \
    case NAME.codeOffset<3>():                                                                                         \
        opcode_label_##NAME:

HOT void Interpret(VMGlobals* g) {
    // byte code values
    Byte* ip;
    Byte op1;

    // interpreter globals
    int numArgsPushed, numKeyArgsPushed;
    PyrSymbol* selector;
    PyrClass* classobj;

    // temporary variables used in the interpreter
    PyrSlot* slot;
    PyrSlot* sp;

#ifdef LABELS_AS_VALUES
    static void* opcode_labels[] = { &&opcode_label_PushClassX,
                                     &&opcode_label_PushInstVarX,
                                     &&opcode_label_PushTempVarX,
                                     &&opcode_label_PushTempZeroVarX,
                                     &&opcode_label_PushLiteralX,
                                     &&opcode_label_PushClassVarX,
                                     &&opcode_label_PushSpecialClass,
                                     &&opcode_label_StoreInstVarX,
                                     &&opcode_label_StoreTempVarX,
                                     &&opcode_label_StoreClassVarX,
                                     &&opcode_label_SendMsgX,
                                     &&opcode_label_SendSuperMsgX,
                                     &&opcode_label_SendSpecialMsgX,
                                     &&opcode_label_SendSpecialUnaryArithMsgX,
                                     &&opcode_label_SendSpecialBinaryArithMsgX,
                                     &&opcode_label_SpecialOpcode,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_PushInstVar,
                                     &&opcode_label_JumpIfTrue,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushTempVar,
                                     &&opcode_label_PushConstant8,
                                     &&opcode_label_PushConstant16,
                                     &&opcode_label_PushConstant24,
                                     &&opcode_label_PushConstant32,
                                     &&opcode_label_PushInteger8,
                                     &&opcode_label_PushInteger16,
                                     &&opcode_label_PushInteger24,
                                     &&opcode_label_PushInteger32,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushTempZeroVar,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushLiteral,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushClassVar,
                                     &&opcode_label_PushSpecialValueThis,
                                     &&opcode_label_PushOneAndSubtract,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushSpecialNumber,
                                     &&opcode_label_PushOneAndAddOne,
                                     &&opcode_label_PushSpecialValue,
                                     &&opcode_label_PushSpecialValue,
                                     &&opcode_label_PushSpecialValue,
                                     &&opcode_label_PushSpecialValue,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreInstVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_StoreTempVar,
                                     &&opcode_label_PushInstVarAndSendSpecialMsg,
                                     &&opcode_label_PushAllArgsAndSendMsg,
                                     &&opcode_label_PushAllButFirstArgAndSendMsg,
                                     &&opcode_label_PushAllArgsAndSendSpecialMsg,
                                     &&opcode_label_PushAllButFirstArgAndSendSpecialMsg,
                                     &&opcode_label_PushAllButFirstTwoArgsAndSendMsg,
                                     &&opcode_label_PushAllButFirstTwoArgsAndSendSpecialMsg,
                                     &&opcode_label_ExtendedCodes,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_StoreClassVar,
                                     &&opcode_label_SendMsgThisOpt,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_SendMsg,
                                     &&opcode_label_TailCallReturnFromFunction,
                                     &&opcode_label_SendSuperMsgThisOpt,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSuperMsg,
                                     &&opcode_label_SendSpecialMsgThisOpt,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialUnaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_SendSpecialBinaryArithMsg,
                                     &&opcode_label_Drop,
                                     &&opcode_label_Dup,
                                     &&opcode_label_BlockReturn,
                                     &&opcode_label_Return,
                                     &&opcode_label_ReturnSelf,
                                     &&opcode_label_ReturnTrue,
                                     &&opcode_label_ReturnFalse,
                                     &&opcode_label_ReturnNil,
                                     &&opcode_label_JumpIfFalse,
                                     &&opcode_label_JumpIfFalsePushNil,
                                     &&opcode_label_JumpIfFalsePushFalse,
                                     &&opcode_label_JumpIfTruePushTrue,
                                     &&opcode_label_JumpFwd,
                                     &&opcode_label_JumpBack,
                                     &&opcode_label_SpecialBinaryOpWithAdverb,
                                     &&opcode_label_TailCallReturnFromMethod };

#endif

#if 0
	unsigned char *bzero;
	PyrSlot *szero;
	char str[80];
#endif


#if BCSTAT
    // clearbcstat();
    op1 = 0;
    prevop = 0;
#endif

#if 0
	bzero = g->ip;
	szero = g->sp;
	//SetSymbol(g->sp, getsym("STACK TOP")); // just for debugging
	//g->sp++; // just for debugging
#endif

    // Codewarrior puts them in registers. take advantage..
    sp = g->sp;
    ip = g->ip;

    numKeyArgsPushed = 0;

    if (setjmp(g->escapeInterpreter) != 0) {
        return;
    }


#ifndef _WIN32
    while (running) {
#else
    while (true) {
#endif

        checkStackDepth(g, sp);

#if BCSTAT
        prevop = op1;
#endif

        op1 = ip[1];
        ++ip;

        assert(checkStackOverflow(g, sp));

#if BCSTAT
        ++bcstat[op1];
        ++bcpair[prevop][op1];
        prevop = op1;
#endif
        // printf("op1 %d\n", op1);
        // postfl("sp %p   frame %p  caller %p  ip %p\n", sp, g->frame, g->frame->caller.uof,
        // slotRawInt(&g->frame->caller.uof->ip)); postfl("sp %p   frame %p   diff %d    caller %p\n", sp, g->frame,
        // ((int)sp - (int)g->frame)>>3, g->frame->caller.uof);
#if DEBUGINTERPRETER
        if (gTraceInterpreter) {
            // DumpStack(g, sp);
            if (FrameSanity(g->frame, "dbgint")) {
                // Debugger();
            }
            // g->gc->SanityCheck();
            // assert(g->gc->SanityCheck());
            g->sp = sp;
            g->ip = ip;
            g->gc->FullCollection();
            sp = g->sp;
            ip = g->ip;
            postfl("[%3d] %20s-%-16s  ", (sp - g->gc->Stack()->slots) + 1,
                   slotRawSymbol(&slotRawClass(&g->method->ownerclass)->name)->name,
                   slotRawSymbol(&g->method->name)->name);
            dumpOneByteCode(g->block, NULL, ip);
        }
#endif
#ifdef GC_SANITYCHECK
        //	gcLinkSanity(g->gc);
        g->gc->SanityCheck();
//	do_check_pool(pyr_pool_runtime);
//	do_check_pool(pyr_pool_compile);
#endif
#if METHODMETER
        if (gTraceInterpreter) {
            slotRawInt(&g->method->byteMeter)++;
        }
#endif

        const auto storeLoadSpAndIp = [&](auto action) {
            g->sp = sp;
            g->ip = ip;
            action();
            sp = g->sp;
            ip = g->ip;
        };


        switch (op1) {
            InterpretOpcode(PushClassX) {
                const auto [classOperand] = PushClassX.pullOperandsFromInstructions(ip);
                classobj = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[classOperand])->u.classobj;
                if (classobj) {
                    ++sp;
                    SetObject(sp, classobj);
                } else {
                    postfl("Execution warning: Class '%s' not found\n",
                           slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[classOperand])->name);
                    slotCopy(++sp, &gSpecialValues.Nil_);
                }
                dispatch_opcode;
            }

            InterpretOpcode(PushInstVarX) {
                const auto [instVarIndex] = PushInstVarX.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->receiver)->slots[instVarIndex]);
                dispatch_opcode;
            }

            InterpretOpcode(PushTempVarX) {
                const auto [frameOffset, varIndex] = PushTempVarX.pullOperandsFromInstructions(ip);
                PyrFrame* tframe = g->frame;
                for (Byte count = frameOffset; count > 0; count -= 1) {
                    tframe = slotRawFrame(&tframe->context);
                }
                slotCopy(++sp, &tframe->vars[varIndex]);
                dispatch_opcode;
            }

            InterpretOpcode(PushTempZeroVarX) {
                const auto [varindex] = PushTempZeroVarX.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->frame->vars[varindex]);
                dispatch_opcode;
            }

            InterpretOpcode(PushLiteralX) {
                const auto [literalIndex] = PushLiteralX.pullOperandsFromInstructions(ip);
                // push a block as a closure if it is one
                slot = slotRawObject(&g->block->selectors)->slots + literalIndex;
                if (IsObj(slot) && slotRawObject(slot)->classptr == gSpecialClasses[op_class_fundef]->u.classobj) {
                    // push a closure
                    g->sp = sp; // gc may push the stack
                    PyrClosure* closure = (PyrClosure*)g->gc->New(2 * sizeof(PyrSlot), 0, obj_notindexed, true);
                    sp = g->sp;
                    closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
                    closure->size = 2;
                    slotCopy(&closure->block, slot);
                    if (IsNil(&slotRawBlock(slot)->contextDef)) {
                        slotCopy(&closure->context, &slotRawInterpreter(&g->process->interpreter)->context);
                    } else {
                        SetObject(&closure->context, g->frame);
                    }
                    ++sp;
                    SetObject(sp, closure);
                } else {
                    slotCopy(++sp, slot);
                }
                dispatch_opcode;
            }

            InterpretOpcode(PushClassVarX) {
                const auto [i1, i0] = PushClassVarX.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->classvars->slots[i1.asInt(i0)]);
                dispatch_opcode;
            }

            InterpretOpcode(PushSpecialClass) {
                const auto [specialClassIndex] = PushSpecialClass.pullOperandsFromInstructions(ip);
                classobj = gSpecialClasses[(Byte)specialClassIndex]->u.classobj;
                if (classobj) {
                    ++sp;
                    SetObject(sp, classobj);
                } else
                    slotCopy(++sp, &gSpecialValues.Nil_);
                dispatch_opcode;
            }

            InterpretOpcode(StoreInstVarX) {
                const auto [instVarIndex] = StoreInstVarX.pullOperandsFromInstructions(ip);
                PyrObject* obj = slotRawObject(&g->receiver);
                if (obj->IsImmutable()) {
                    StoreToImmutableA(g, sp, ip);
                } else {
                    slot = obj->slots + instVarIndex;
                    slotCopy(slot, sp);
                    g->gc->GCWrite(obj, slot);
                }
                dispatch_opcode;
            }

            InterpretOpcode(StoreTempVarX) {
                const auto [frameOffset, index] = StoreTempVarX.pullOperandsFromInstructions(ip);
                PyrFrame* tframe = g->frame;
                for (Byte count = frameOffset; count > 0; count -= 1) {
                    tframe = slotRawFrame(&tframe->context);
                }
                slot = tframe->vars + index;
                slotCopy(slot, sp);
                g->gc->GCWrite(tframe, slot);
                dispatch_opcode;
            }

            InterpretOpcode(StoreClassVarX) {
                const auto [i1, i0] = StoreClassVarX.pullOperandsFromInstructions(ip);
                slotCopy(&g->classvars->slots[i1.asInt(i0)], sp);
                g->gc->GCWrite(g->classvars, sp);
                dispatch_opcode;
            }

            InterpretOpcode(SendMsgX) {
                const auto [numArgs, numKwArgs, selectorIndex] = SendMsgX.pullOperandsFromInstructions(ip);
                numArgsPushed = numArgs;
                numKeyArgsPushed = numKwArgs;
                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = sp - numArgsPushed + 1;
                if (numKeyArgsPushed)
                    goto key_class_lookup_then_key_msg_lookup;
                else
                    goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(SendSuperMsgX) {
                const auto [numArgs, numKwArgs, selectorIndex] = SendSuperMsgX.pullOperandsFromInstructions(ip);
                numArgsPushed = numArgs;
                numKeyArgsPushed = numKwArgs;
                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = g->sp - numArgsPushed + 1;
                classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
                if (numKeyArgsPushed)
                    goto key_msg_lookup;
                else
                    goto msg_lookup;
            }

            InterpretOpcode(SendSpecialMsgX) {
                const auto [numArgs, numKwArgs, selectorIndex] = SendSpecialMsgX.pullOperandsFromInstructions(ip);
                numArgsPushed = numArgs;
                numKeyArgsPushed = numKwArgs;
                selector = gSpecialSelectors[selectorIndex];
                slot = sp - numArgsPushed + 1;
                if (numKeyArgsPushed)
                    goto key_class_lookup_then_key_msg_lookup;
                else
                    goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(SendSpecialUnaryArithMsgX) {
                const auto [unaryMathOp] = SendSpecialUnaryArithMsgX.pullOperandsFromInstructions(ip);
                storeLoadSpAndIp([&, unaryMathOp = unaryMathOp]() {
                    g->primitiveIndex = (Byte)unaryMathOp;
                    doSpecialUnaryArithMsg(g, -1);
                });
                ifTailCallOptimise([&]() { g->tailCall = 0; });
                dispatch_opcode;
            }

            InterpretOpcode(SendSpecialBinaryArithMsgX) {
                const auto [binaryMathOp] = SendSpecialBinaryArithMsgX.pullOperandsFromInstructions(ip);
                storeLoadSpAndIp([&, binaryMathOp = binaryMathOp]() {
                    g->primitiveIndex = (Byte)binaryMathOp;
                    doSpecialBinaryArithMsg(g, 2, false);
                });
                dispatch_opcode;
            }

            InterpretOpcode(SpecialOpcode) {
                const auto [psuedoVar] = SpecialOpcode.pullOperandsFromInstructions(ip);
                switch (psuedoVar) {
                case OpPseudoVarEnum::Process:
                    ++sp;
                    SetObject(sp, g->process);
                    break;
                case OpPseudoVarEnum::Thread:
                    ++sp;
                    SetObject(sp, g->thread);
                    break;
                case OpPseudoVarEnum::Method:
                    ++sp;
                    SetObject(sp, g->method);
                    break;
                case OpPseudoVarEnum::FunctionDef:
                    ++sp;
                    SetObject(sp, g->block);
                    break;
                case OpPseudoVarEnum::Function: {
                    // push a closure
                    g->sp = sp; // gc may push the stack
                    PyrClosure* closure = (PyrClosure*)g->gc->New(2 * sizeof(PyrSlot), 0, obj_notindexed, true);
                    sp = g->sp;
                    closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
                    closure->size = 2;
                    SetObject(&closure->block, g->block);
                    SetObject(&closure->context, slotRawFrame(&g->frame->context));
                    ++sp;
                    SetObject(sp, closure);
                    break;
                }
                default:
                    slotCopy(++sp, &gSpecialValues.Nil_);
                    break;
                }
                dispatch_opcode;
            }

            InterpretOpcode16(PushInstVar) {
                const auto [nibble] = PushInstVar.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->receiver)->slots[nibble]);
                dispatch_opcode;
            }

            InterpretOpcode(JumpIfTrue) {
                if (IsTrue(sp)) {
                    const auto [high16, low16] = JumpIfTrue.pullOperandsFromInstructions(ip);
                    ip += high16.asInt(low16);
                } else if (IsFalse(sp)) {
                    const auto [i1, i0] = JumpIfTrue.pullOperandsFromInstructions(ip);
                } else {
                    // Note: we don't increment the instruction pointer in this case!
                    numArgsPushed = 1;
                    selector = gSpecialSelectors[opmNonBooleanError];
                    slot = sp;
                    goto class_lookup_then_msg_lookup;
                }
                --sp;
                dispatch_opcode;
            }

            InterpretOpcode7(PushTempVar) {
                const auto [frameOffset, varIndex] = PushTempVar.pullOperandsFromInstructions(ip);
                PyrFrame* start = g->frame;
                for (int i = frameOffset; i > 0; --i)
                    start = slotRawFrame(&start->context);
                slotCopy(++sp, &start->vars[varIndex]);
                dispatch_opcode;
            }

            InterpretOpcode(PushConstant8) {
                const auto [slotIndex] = PushConstant8.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[slotIndex.asInt()]);
                dispatch_opcode;
            }

            InterpretOpcode(PushConstant16) {
                const auto [v1, v0] = PushConstant16.pullOperandsFromInstructions(ip);
                const int ival = v1.asInt(v0);
                slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
                dispatch_opcode;
            }

            InterpretOpcode(PushConstant24) {
                const auto [v2, v1, v0] = PushConstant24.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[v2.asInt(v1, v0)]);
                dispatch_opcode;
            }

            InterpretOpcode(PushConstant32) {
                const auto [v3, v2, v1, v0] = PushConstant32.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[v3.asInt(v2, v1, v0)]);
                dispatch_opcode;
            }

            InterpretOpcode(PushInteger8) {
                const auto [i0] = PushInteger8.pullOperandsFromInstructions(ip);
                SetInt(++sp, i0.asInt());
                dispatch_opcode;
            }

            InterpretOpcode(PushInteger16) {
                const auto [i1, i0] = PushInteger16.pullOperandsFromInstructions(ip);
                SetInt(++sp, i1.asInt(i0));
                dispatch_opcode;
            }

            InterpretOpcode(PushInteger24) {
                const auto [i2, i1, i0] = PushInteger24.pullOperandsFromInstructions(ip);
                SetInt(++sp, i2.asInt(i1, i0));
                dispatch_opcode;
            }

            InterpretOpcode(PushInteger32) {
                const auto [i3, i2, i1, i0] = PushInteger32.pullOperandsFromInstructions(ip);
                SetInt(++sp, i3.asInt(i2, i1, i0));
                dispatch_opcode;
            }

            InterpretOpcode16(PushTempZeroVar) {
                const auto [var_i] = PushTempZeroVar.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->frame->vars[var_i]);
                dispatch_opcode;
            }

            InterpretOpcode16(PushLiteral) {
                const auto [slot_i] = PushLiteral.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[slot_i]);
                dispatch_opcode;
            }

            InterpretOpcode16(PushClassVar) {
                const auto [slots_i] = PushClassVar.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->classvars->slots[slots_i]);
                dispatch_opcode;
            }

            InterpretOpcode(PushSpecialValueThis) {
                slotCopy(++sp, &g->receiver);
                dispatch_opcode;
            }

            InterpretOpcode(PushOneAndSubtract) {
                if (IsInt(sp)) {
                    SetRaw(sp, slotRawInt(sp) - 1);
                    ifTailCallOptimise([&]() { g->tailCall = 0; });
                } else {
                    slotCopy(++sp, &gSpecialNumbers.One);
                    storeLoadSpAndIp([&]() {
                        g->primitiveIndex = opSub;
                        prSubNum(g, -1);
                    });
                }
                dispatch_opcode;
            }

            InterpretOpcode9(PushSpecialNumber) {
                const auto [specialNumber] = PushSpecialNumber.pullOperandsFromInstructions(ip);
                slotCopy(++sp, gSpecialNumbers[specialNumber]);
                dispatch_opcode;
            }

            InterpretOpcode(PushOneAndAddOne) {
                if (IsInt(sp)) {
                    SetRaw(sp, slotRawInt(sp) + 1);
                    ifTailCallOptimise([&]() { g->tailCall = 0; });
                } else {
                    slotCopy(++sp, &gSpecialNumbers.One);
                    storeLoadSpAndIp([&]() {
                        g->primitiveIndex = opAdd;
                        prAddNum(g, -1);
                    });
                }
                dispatch_opcode;
            }

            InterpretOpcode4(PushSpecialValue) {
                const auto [value] = PushSpecialValue.pullOperandsFromInstructions(ip);
                slotCopy(++sp, gSpecialValues[value]);
                dispatch_opcode;
            }

            InterpretOpcode16(StoreInstVar) {
                const auto [index] = StoreInstVar.pullOperandsFromInstructions(ip);
                PyrObject* obj = slotRawObject(&g->receiver);
                if (obj->IsImmutable())
                    StoreToImmutableA(g, sp, ip);
                else {
                    PyrSlot* slot = obj->slots + index;
                    slotCopy(slot, sp--);
                    g->gc->GCWrite(obj, slot);
                }
                dispatch_opcode;
            }

            InterpretOpcode8(StoreTempVar) {
                const auto [frameOffset, varIndex] = StoreTempVar.pullOperandsFromInstructions(ip);
                PyrFrame* tframe = g->frame;
                for (Byte i = frameOffset; i > 0; --i) {
                    tframe = slotRawFrame(&tframe->context);
                }
                slot = tframe->vars + varIndex;
                slotCopy(slot, sp--);
                g->gc->GCWrite(tframe, slot);
                dispatch_opcode;
            }

            InterpretOpcode(PushInstVarAndSendSpecialMsg) {
                const auto [instVarIndex, selectorIndex] =
                    PushInstVarAndSendSpecialMsg.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &slotRawObject(&g->receiver)->slots[instVarIndex]);
                numArgsPushed = 1;
                selector = gSpecialSelectors[selectorIndex];
                slot = sp;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(PushAllArgsAndSendMsg) {
                const auto [selectorIndex] = PushAllArgsAndSendMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = METHRAW(g->block)->numargs;
                PyrSlot* pslot = g->frame->vars - 1;
                for (int m = 0; m < numArgsPushed; ++m)
                    *++sp = *++pslot;

                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(PushAllButFirstArgAndSendMsg) {
                const auto [selectorIndex] = PushAllButFirstArgAndSendMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = METHRAW(g->block)->numargs;
                PyrSlot* pslot = g->frame->vars;
                for (int m = 0; m < numArgsPushed - 1; ++m)
                    *++sp = *++pslot;

                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(PushAllArgsAndSendSpecialMsg) {
                const auto [selectorIndex] = PushAllArgsAndSendSpecialMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = METHRAW(g->block)->numargs;
                PyrSlot* pslot = g->frame->vars - 1;
                for (int m = 0; m < numArgsPushed; ++m)
                    *++sp = *++pslot;

                selector = gSpecialSelectors[selectorIndex];
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(PushAllButFirstArgAndSendSpecialMsg) {
                const auto [specialIndex] = PushAllButFirstArgAndSendSpecialMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = METHRAW(g->block)->numargs;
                PyrSlot* pslot = g->frame->vars;
                for (int m = 0; m < numArgsPushed - 1; ++m)
                    *++sp = *++pslot;

                selector = gSpecialSelectors[specialIndex];
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(PushAllButFirstTwoArgsAndSendMsg) {
                const auto [index] = PushAllButFirstTwoArgsAndSendMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = METHRAW(g->block)->numargs + 1;
                PyrSlot* pslot = g->frame->vars;
                for (int m = 0; m < numArgsPushed - 2; ++m)
                    *++sp = *++pslot;

                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[index]);
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(PushAllButFirstTwoArgsAndSendSpecialMsg) {
                const auto [index] = PushAllButFirstTwoArgsAndSendSpecialMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = METHRAW(g->block)->numargs + 1;
                PyrSlot* pslot = g->frame->vars;
                for (int m = 0; m < numArgsPushed - 2; ++m)
                    *++sp = *++pslot;

                selector = gSpecialSelectors[index];
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            /// ExtendedCodes are often used to implement looping methods.
            /// They work by manipulating the instruction pointer, therefore it is imperative the opcodes are emitted
            /// in the expected sequence.
            InterpretOpcode(ExtendedCodes) {
                // Helpers
                const auto pushAndPrepForCall = [&](auto selectorIndex, auto&&... args) {
                    static_assert(sizeof...(args) != 0);
                    (slotCopy(++sp, args), ...);
                    numArgsPushed = sizeof...(args);
                    selector = gSpecialSelectors[selectorIndex];
                    slot = sp - (sizeof...(args) - 1);
                };

                const auto [extendedCode] = ExtendedCodes.pullOperandsFromInstructions(ip);
                switch (extendedCode) {
                case Extended::IntegerDo.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (slotRawInt(&vars[2]) < slotRawInt(&g->receiver)) {
                        // Push: function, i, and i (i is duplicated because it is its own index).
                        pushAndPrepForCall(opmValue, &vars[1], &vars[2], &vars[2]);
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver); // Push self.
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                // NOTE: this is also used by ArrayedCollectionDo.
                // TODO: rework byte codes to avoid this dual use of the opcodes.
                case Extended::IntegerDo.DropAndJumpBackToLoop.code: {
                    --sp;
                    SetRaw(&g->frame->vars[2], slotRawInt(&g->frame->vars[2]) + 1); // inc i
                    ip -= Extended::IntegerDo.jumpSize();
                    dispatch_opcode;
                }


                case Extended::IntegerReverseDo.Init.code: {
                    SetRaw(&g->frame->vars[2], slotRawInt(&g->receiver) - 1);
                    dispatch_opcode;
                }
                case Extended::IntegerReverseDo.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (slotRawInt(&vars[2]) >= 0) {
                        // Push: function, i, and j
                        pushAndPrepForCall(opmValue, &vars[1], &vars[2], &vars[3]);
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                case Extended::IntegerReverseDo.DropAndJumpBackToLoop.code: {
                    --sp; // Drop
                    PyrSlot* vars = g->frame->vars;
                    SetRaw(&vars[2], slotRawInt(&vars[2]) - 1); // dec i
                    SetRaw(&vars[3], slotRawInt(&vars[3]) + 1); // inc j
                    ip -= Extended::IntegerReverseDo.jumpSize();
                    dispatch_opcode;
                }

                case Extended::IntegerFor.Init.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (GetTag(&vars[1]) != tagInt) {
                        if (IsFloat(&vars[1])) {
                            SetInt(&vars[1], (int32)(slotRawFloat(&vars[1])));
                        } else {
                            error("Integer-for : endval not a SimpleNumber.\n");
                            pushAndPrepForCall(opmPrimitiveFailed, &g->receiver);
                            goto class_lookup_then_msg_lookup;
                        }
                    }

                    if (slotRawInt(&g->receiver) <= slotRawInt(&vars[1]))
                        SetRaw(&vars[5], 1);
                    else
                        SetRaw(&vars[5], -1);
                    slotCopy(&vars[3], &g->receiver);
                    dispatch_opcode;
                }
                case Extended::IntegerFor.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if ((slotRawInt(&vars[5]) > 0 && slotRawInt(&vars[3]) <= slotRawInt(&vars[1]))
                        || (slotRawInt(&vars[5]) < 0 && slotRawInt(&vars[3]) >= slotRawInt(&vars[1]))) {
                        // Push: function, i, j
                        pushAndPrepForCall(opmValue, &vars[2], &vars[3], &vars[4]);
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                case Extended::IntegerFor.DropAndJumpBackToLoop.code: {
                    --sp;
                    PyrSlot* vars = g->frame->vars;
                    SetRaw(&vars[3], slotRawInt(&vars[3]) + slotRawInt(&vars[5])); // inc i by stepval
                    SetRaw(&vars[4], slotRawInt(&vars[4]) + 1); // inc j
                    ip -= Extended::IntegerFor.jumpSize();
                    dispatch_opcode;
                }


                case Extended::IntegerForBy.Init.code: {
                    PyrSlot* vars = g->frame->vars;
                    // Detect case of a zero-valued stepval argument, which
                    // would result in an infinite loop.
                    if ((IsFloat(vars + 2) && slotRawFloat(&vars[2]) == 0.0)
                        || (IsInt(vars + 2) && slotRawInt(&vars[2]) == 0)) {
                        error("Integer-forBy: zero-valued stepval argument.\n");
                        pushAndPrepForCall(opmPrimitiveFailed, &g->receiver);
                        goto class_lookup_then_msg_lookup;
                    }
                    // If any argument is floating point we cast all arguments
                    // to floating point, including the accumulator. This avoids
                    // potential infinite loops due to integer truncation.
                    if (IsFloat(vars + 1) || IsFloat(vars + 2)) {
                        if (IsInt(vars + 1))
                            SetFloat(&vars[1], (double)(slotRawInt(&vars[1])));
                        if (IsInt(vars + 2))
                            SetFloat(&vars[2], (double)(slotRawInt(&vars[2])));
                        SetFloat(&vars[4], (double)(slotRawInt(&g->receiver)));
                    } else {
                        if ((GetTag(&vars[1]) != tagInt) || NotInt(&vars[2])) {
                            error("Integer-forBy : endval or stepval not an Integer or Float.\n");
                            pushAndPrepForCall(opmPrimitiveFailed, &g->receiver);
                            goto class_lookup_then_msg_lookup;
                        }
                        slotCopy(&vars[4], &g->receiver);
                    }
                    dispatch_opcode;
                }
                case Extended::IntegerForBy.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    bool continueForBy = false;
                    if (IsFloat(vars + 4)) {
                        continueForBy =
                            (slotRawFloat(&vars[2]) >= 0.0 && slotRawFloat(&vars[4]) <= slotRawFloat(&vars[1]))
                            || (slotRawFloat(&vars[2]) < 0.0 && slotRawFloat(&vars[4]) >= slotRawFloat(&vars[1]));
                    } else {
                        continueForBy = (slotRawInt(&vars[2]) >= 0 && slotRawInt(&vars[4]) <= slotRawInt(&vars[1]))
                            || (slotRawInt(&vars[2]) < 0 && slotRawInt(&vars[4]) >= slotRawInt(&vars[1]));
                    }
                    if (continueForBy) {
                        // Push: function, i, j
                        pushAndPrepForCall(opmValue, &vars[3], &vars[4], &vars[5]);
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                case Extended::IntegerForBy.DropAndJumpBackToLoop.code: {
                    --sp;
                    PyrSlot* vars = g->frame->vars;
                    if (IsFloat(vars + 4)) {
                        SetRaw(&vars[4], slotRawFloat(&vars[4]) + slotRawFloat(&vars[2]));
                    } else {
                        SetRaw(&vars[4], slotRawInt(&vars[4]) + slotRawInt(&vars[2])); // inc i
                    }
                    SetRaw(&vars[5], slotRawInt(&vars[5]) + 1); // inc j
                    ip -= Extended::IntegerForBy.jumpSize();
                    dispatch_opcode;
                }

                case Extended::ArrayedCollectionDo.LoopOrReturn.code: {
                    // 0 this, 1 func, 2 i
                    PyrSlot* vars = g->frame->vars;
                    if (slotRawInt(&vars[2]) < slotRawObject(&g->receiver)->size) {
                        slotCopy(++sp, &vars[1]); // push function
                        getIndexedSlot(slotRawObject(&g->receiver), ++sp, slotRawInt(&vars[2])); // push this.at(i)
                        slotCopy(++sp, &vars[2]); // push i
                        numArgsPushed = 3;
                        selector = gSpecialSelectors[opmValue];
                        slot = sp - 2;
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                    // TODO: ArrayedCollectionDo also uses the IntegerDo's DropAndJumpBackToLoop. Fix this.

                case Extended::ArrayedCollectionReversedDo.Init.code:
                    SetRaw(&g->frame->vars[2], slotRawObject(&g->receiver)->size - 1);
                    dispatch_opcode;
                case Extended::ArrayedCollectionReversedDo.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (slotRawInt(&vars[2]) >= 0) {
                        slotCopy(++sp, &vars[1]); // push function
                        getIndexedSlot(slotRawObject(&g->receiver), ++sp, slotRawInt(&vars[2])); // push this.at(i)
                        slotCopy(++sp, &vars[3]); // push j
                        numArgsPushed = 3;
                        selector = gSpecialSelectors[opmValue];
                        slot = sp - 2;
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                    // TODO: ArrayedCollectionReverseDo also uses the IntegerReverseDo's DropAndJumpBackToLoop. Fix
                    // this.

                case Extended::DictionaryKeyValuesArrayDo.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (IsNil(&vars[1])) {
                        error("Dictionary-keysValuesArrayDo: first argument should not be nil.\n");
                        // Note: this changed the numArgsPushed variable from 0 to 1, was this a bug?
                        pushAndPrepForCall(opmPrimitiveFailed, &g->receiver);
                        goto class_lookup_then_msg_lookup;
                    }

                    int m = slotRawInt(&vars[3]);
                    PyrObject* obj = slotRawObject(&vars[1]);
                    if (m < obj->size) {
                        slot = obj->slots + m; // key
                        while (IsNil(slot)) {
                            m += 2;
                            if (m >= obj->size) {
                                SetRaw(&vars[3], m);
                                goto keysValuesArrayDo_return;
                            }
                            slot = obj->slots + m; // key
                        }
                        SetRaw(&vars[3], m);
                        // Push: function, key, value, j
                        pushAndPrepForCall(opmValue, &vars[2], &slot[0], &slot[1], &vars[4]);
                        SetRaw(&vars[4], slotRawInt(&vars[4]) + 1);
                        goto class_lookup_then_msg_lookup;
                    } else {
                    keysValuesArrayDo_return:
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                case Extended::DictionaryKeyValuesArrayDo.DropAndJumpBackToLoop.code: {
                    --sp;
                    SetRaw(&g->frame->vars[3], slotRawInt(&g->frame->vars[3]) + 2); // inc i
                    ip -= Extended::DictionaryKeyValuesArrayDo.jumpSize();
                    dispatch_opcode;
                }

                case Extended::FloatDo.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (slotRawFloat(&vars[2]) + 0.5 < slotRawFloat(&g->receiver)) {
                        // Push: function, i, i (is is also the index).
                        pushAndPrepForCall(opmValue, &vars[1], &vars[2], &vars[2]);
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                case Extended::FloatDo.DropAndJumpBackToLoop.code: {
                    --sp; // Drop
                    SetRaw(&g->frame->vars[2], slotRawFloat(&g->frame->vars[2]) + 1.0); // inc i
                    ip -= Extended::FloatDo.jumpSize();
                    dispatch_opcode;
                }

                case Extended::FloatDoReverse.Init.code: {
                    SetFloat(&g->frame->vars[2], slotRawFloat(&g->receiver) - 1.0);
                    dispatch_opcode;
                }
                case Extended::FloatDoReverse.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (slotRawFloat(&vars[2]) + 0.5 >= 0.0) {
                        // Push: function, i, j
                        pushAndPrepForCall(opmValue, &vars[1], &vars[2], &vars[3]);
                        goto class_lookup_then_msg_lookup;
                    } else {
                        slotCopy(++sp, &g->receiver);
                        storeLoadSpAndIp([&]() { returnFromMethod(g); });
                    }
                    dispatch_opcode;
                }
                case Extended::FloatDoReverse.DropAndJumpBackToLoop.code: {
                    --sp;
                    PyrSlot* vars = g->frame->vars;
                    SetRaw(&vars[2], slotRawFloat(&vars[2]) - 1.0); // dec i
                    SetRaw(&vars[3], slotRawFloat(&vars[3]) - 1.0); // inc j
                    ip -= Extended::FloatDoReverse.jumpSize();
                    dispatch_opcode;
                }


                case Extended::QuestionMark.IsNil.code: {
                    --sp;
                    if (IsNil(sp))
                        *sp = *(sp + 1);
                    dispatch_opcode;
                }

                case Extended::DoubleQuestionMark.Jump.code: {
                    const auto [i1, i0] = Extended::DoubleQuestionMark.Jump.pullOperandsFromInstructions(ip);
                    if (NotNil(sp))
                        ip += i1.asInt(i0);
                    else
                        --sp;
                    dispatch_opcode;
                }

                case Extended::IfNotNilJump.Jump.code: {
                    const auto [i1, i0] = Extended::IfNotNilJump.Jump.pullOperandsFromInstructions(ip);
                    if (NotNil(sp))
                        ip += i1.asInt(i0);
                    --sp; // remove not nil from stack
                    dispatch_opcode;
                }

                case Extended::IfNilJump.Jump.code: {
                    const auto [i1, i0] = Extended::IfNilJump.Jump.pullOperandsFromInstructions(ip);
                    if (IsNil(sp))
                        ip += i1.asInt(i0);
                    --sp; // remove nil from stack
                    dispatch_opcode;
                }

                case Extended::IfNotNilJumpPushNilElsePop.Jump.code: {
                    const auto [i1, i0] = Extended::IfNotNilJumpPushNilElsePop.Jump.pullOperandsFromInstructions(ip);
                    if (NotNil(sp)) {
                        ip += i1.asInt(i0);
                        slotCopy(sp, &gSpecialValues.Nil_);
                    } else
                        --sp; // Remove nil from stack, evaluate function on stack (??)
                    dispatch_opcode;
                }

                case Extended::IfNilThenJumpElsePopNil.Jump.code: {
                    const auto [i1, i0] = Extended::IfNilThenJumpElsePopNil.Jump.pullOperandsFromInstructions(ip);
                    if (IsNil(sp))
                        ip += i1.asInt(i0); // Jump
                    else
                        --sp; // Remove non-nil from the stack, evaluate function on stack (!?)
                    dispatch_opcode;
                }

                case Extended::Switch.LookupAndJump.code: {
                    PyrObject* obj = slotRawObject(sp);
                    const int slotIndex = 1 + arrayAtIdentityHashInPairs(obj, (sp - 1));
                    sp -= 2;
                    ip += slotRawInt(&obj->slots[slotIndex]);
                    dispatch_opcode;
                }

                case Extended::NumberForSeries.Init.code: {
                    PyrSlot* vars = g->frame->vars;
                    // 0 receiver, 1 step, 2 last, 3 function, 4 i, 5 j
                    if (IsInt(vars + 0) && (IsInt(vars + 1) || IsNil(vars + 1))
                        && (IsInt(vars + 2) || IsNil(vars + 2))) {
                        if (IsNil(vars + 1)) {
                            if (IsNil(vars + 2))
                                SetInt(vars + 2, 0x7FFFFFFF);
                            if (slotRawInt(&vars[0]) < slotRawInt(&vars[2]))
                                SetInt(vars + 1, 1);
                            else
                                SetInt(vars + 1, -1);
                        } else {
                            if (IsNil(vars + 2)) {
                                if (slotRawInt(&vars[1]) < slotRawInt(&vars[0]))
                                    SetInt(vars + 2, 0x80000000);
                                else
                                    SetInt(vars + 2, 0x7FFFFFFF);
                            }
                            SetInt(vars + 1, slotRawInt(&vars[1]) - slotRawInt(&vars[0]));
                        }
                        slotCopy(&vars[4], &vars[0]);
                    } else {
                        if (IsInt(vars + 0)) {
                            SetFloat(&vars[4], slotRawInt(&vars[0]));
                        } else if (IsFloat(vars + 0)) {
                            SetFloat(&vars[4], slotRawFloat(&vars[0]));
                        } else {
                        bailFromNumberSeries:
                            error("Number-forSeries : first, second or last not an Integer or Float.\n");
                            pushAndPrepForCall(opmPrimitiveFailed, &g->receiver);
                            goto class_lookup_then_msg_lookup;
                        }

                        if (IsNil(vars + 1)) {
                            if (IsNil(vars + 2))
                                SetFloat(vars + 2, kBigBigFloat);
                            else if (IsInt(vars + 2))
                                SetFloat(&vars[2], slotRawInt(&vars[2]));
                            else if (!IsFloat(vars + 2))
                                goto bailFromNumberSeries;

                            if (slotRawFloat(&vars[4]) < slotRawFloat(&vars[2]))
                                SetFloat(vars + 1, 1.);
                            else
                                SetFloat(vars + 1, -1.);
                        } else {
                            if (IsInt(vars + 1))
                                SetFloat(&vars[1], slotRawInt(&vars[1]));
                            else if (!IsFloat(vars + 1))
                                goto bailFromNumberSeries;

                            if (IsNil(vars + 2)) {
                                if (slotRawFloat(&vars[1]) < slotRawFloat(&vars[4]))
                                    SetFloat(vars + 2, kSmallSmallFloat);
                                else
                                    SetFloat(vars + 2, kBigBigFloat);
                            } else if (IsInt(vars + 2))
                                SetFloat(&vars[2], slotRawInt(&vars[2]));
                            else if (!IsFloat(vars + 2))
                                goto bailFromNumberSeries;
                            SetFloat(vars + 1, slotRawFloat(&vars[1]) - slotRawFloat(&vars[4]));
                        }
                    }
                    dispatch_opcode;
                }
                case Extended::NumberForSeries.LoopOrReturn.code: {
                    PyrSlot* vars = g->frame->vars;
                    if (GetTag(&vars[1]) == tagInt) {
                        if ((slotRawInt(&vars[1]) >= 0 && slotRawInt(&vars[4]) <= slotRawInt(&vars[2]))
                            || (slotRawInt(&vars[1]) < 0 && slotRawInt(&vars[4]) >= slotRawInt(&vars[2]))) {
                            // Push: function, i, j
                            pushAndPrepForCall(opmValue, &vars[3], &vars[4], &vars[5]);
                            goto class_lookup_then_msg_lookup;
                        } else {
                            slotCopy(++sp, &g->receiver);
                            storeLoadSpAndIp([&]() { returnFromMethod(g); });
                        }
                    } else {
                        if ((slotRawFloat(&vars[1]) >= 0. && slotRawFloat(&vars[4]) <= slotRawFloat(&vars[2]))
                            || (slotRawFloat(&vars[1]) < 0. && slotRawFloat(&vars[4]) >= slotRawFloat(&vars[2]))) {
                            // Push: function, i, j
                            pushAndPrepForCall(opmValue, &vars[3], &vars[4], &vars[5]);
                            goto class_lookup_then_msg_lookup;
                        } else {
                            slotCopy(++sp, &g->receiver);
                            storeLoadSpAndIp([&]() { returnFromMethod(g); });
                        }
                    }
                    dispatch_opcode;
                }
                case Extended::NumberForSeries.DropAndJumpBackToLoop.code: {
                    --sp;
                    PyrSlot* vars = g->frame->vars;
                    if (GetTag(&vars[1]) == tagInt)
                        SetRaw(&vars[4], slotRawInt(&vars[4]) + slotRawInt(&vars[1])); // inc i
                    else
                        SetRaw(&vars[4], slotRawFloat(&vars[4]) + slotRawFloat(&vars[1])); // inc i
                    SetRaw(&vars[5], slotRawInt(&vars[5]) + 1); // inc j
                    ip -= Extended::NumberForSeries.jumpSize();
                    dispatch_opcode;
                }

                default:
                    break;
                }
                dispatch_opcode;
            }

            InterpretOpcode16(StoreClassVar) {
                const auto [index] = StoreClassVar.pullOperandsFromInstructions(ip);
                slotCopy(&g->classvars->slots[index], sp--);
                g->gc->GCWrite(g->classvars, (sp + 1));
                dispatch_opcode;
            }

            InterpretOpcode(SendMsgThisOpt) {
                const auto [index] = SendMsgThisOpt.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->receiver);
                numArgsPushed = 1;
                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[index]);
                slot = sp;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode15(SendMsg) {
                const auto [numArgs, selectorIndex] = SendMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = numArgs;
                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode(TailCallReturnFromFunction) {
                ifTailCallOptimise([&]() { g->tailCall = 2; });
                dispatch_opcode;
            }

            InterpretOpcode(SendSuperMsgThisOpt) {
                const auto [selectorIndex] = SendSuperMsgThisOpt.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->receiver);
                numArgsPushed = 1;
                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = sp;
                classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
                goto msg_lookup;
            }

            InterpretOpcode14(SendSuperMsg) {
                const auto [numArgs, selectorIndex] = SendSuperMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = numArgs;
                selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[selectorIndex]);
                slot = sp - numArgsPushed + 1;
                classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;
                goto msg_lookup;
            }

            InterpretOpcode(SendSpecialMsgThisOpt) {
                const auto [specialSelector] = SendSpecialMsgThisOpt.pullOperandsFromInstructions(ip);
                slotCopy(++sp, &g->receiver);
                numArgsPushed = 1;
                selector = gSpecialSelectors[specialSelector];
                slot = sp;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode15(SendSpecialMsg) {
                const auto [numArgs, specialSelector] = SendSpecialMsg.pullOperandsFromInstructions(ip);
                numArgsPushed = numArgs;
                selector = gSpecialSelectors[(Byte)specialSelector];
                slot = sp - numArgsPushed + 1;
                goto class_lookup_then_msg_lookup;
            }

            InterpretOpcode16(SendSpecialUnaryArithMsg) {
                const auto [unaryMathNibble] = SendSpecialUnaryArithMsg.pullOperandsFromInstructions(ip);
                switch (unaryMathNibble) {
                case OpUnaryMathNibble::Neg: {
                    if (IsFloat(sp)) {
                        SetFloat(sp, -slotRawFloat(sp));
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else if (IsInt(sp)) {
                        SetRaw(sp, -slotRawInt(sp));
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else
                        goto default_unary_math_nibble_case;
                } break;

                case OpUnaryMathNibble::Not: {
                    if (IsTrue(sp)) {
                        SetTagRaw(sp, tagFalse);
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else if (IsFalse(sp)) {
                        SetTagRaw(sp, tagTrue);
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else
                        goto default_unary_math_nibble_case;
                } break;

                case OpUnaryMathNibble::IsNil: {
                    if (IsNil(sp))
                        SetTagRaw(sp, tagTrue);
                    else
                        slotCopy(sp, &gSpecialValues.False);
                    ifTailCallOptimise([&]() { g->tailCall = 0; });
                } break;

                case OpUnaryMathNibble::NotNil: {
                    if (NotNil(sp))
                        slotCopy(sp, &gSpecialValues.True);
                    else
                        SetTagRaw(sp, tagFalse);
                    ifTailCallOptimise([&]() { g->tailCall = 0; });
                } break;

                default: {
                default_unary_math_nibble_case:
                    g->sp = sp;
                    g->ip = ip;
                    g->primitiveIndex = (Byte)unaryMathNibble;
                    doSpecialUnaryArithMsg(g, -1);
                    sp = g->sp;
                    ip = g->ip;
                } break;
                }
                dispatch_opcode;
            }

            InterpretOpcode16(SendSpecialBinaryArithMsg) {
                const auto [binaryMathNibble] = SendSpecialBinaryArithMsg.pullOperandsFromInstructions(ip);
                switch (binaryMathNibble) {
                case OpBinaryMathNibble::Add: {
                    if (IsInt(sp - 1) && IsInt(sp)) {
                        --sp;
                        SetRaw(sp, slotRawInt(sp) + slotRawInt(sp + 1));
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else
                        storeLoadSpAndIp([&]() {
                            g->primitiveIndex = (Byte)OpBinaryMathNibble::Add;
                            IsInt(sp - 1) ? prAddInt(g, -1) : prAddNum(g, -1);
                        });
                } break;

                case OpBinaryMathNibble::Sub: {
                    if (IsInt(sp - 1) && IsInt(sp)) {
                        --sp;
                        SetRaw(sp, slotRawInt(sp) - slotRawInt(sp + 1));
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else
                        storeLoadSpAndIp([&]() {
                            g->primitiveIndex = (Byte)OpBinaryMathNibble::Sub;
                            IsInt(sp - 1) ? prSubInt(g, -1) : prSubNum(g, -1);
                        });
                } break;

                case OpBinaryMathNibble::Mul: {
                    if (IsInt(sp - 1) && IsInt(sp)) {
                        --sp;
                        SetRaw(sp, slotRawInt(sp) * slotRawInt(sp + 1));
                        ifTailCallOptimise([&]() { g->tailCall = 0; });
                    } else
                        storeLoadSpAndIp([&]() {
                            g->primitiveIndex = opMul;
                            IsInt(sp - 1) ? prMulInt(g, -1) : prMulNum(g, -1);
                        });
                } break;

                default: {
                default_binary_math_nibble_case:
                    storeLoadSpAndIp([&, binaryMathNibble = binaryMathNibble]() {
                        g->primitiveIndex = (Byte)binaryMathNibble;
                        doSpecialBinaryArithMsg(g, 2, false);
                    });
                } break;
                }
                dispatch_opcode;
            }

            InterpretOpcode(Drop) {
                --sp;
                dispatch_opcode;
            }

            InterpretOpcode(Dup) {
                ++sp;
                *sp = sp[-1];
                dispatch_opcode;
            }

            InterpretOpcode(BlockReturn) {
                storeLoadSpAndIp([&]() { returnFromBlock(g); });
                dispatch_opcode;
            }

            InterpretOpcode(Return) {
                storeLoadSpAndIp([&]() { returnFromMethod(g); });
                dispatch_opcode;
            }

            InterpretOpcode(ReturnSelf) {
                slotCopy(++sp, &g->receiver);
                storeLoadSpAndIp([&]() { returnFromMethod(g); });
                dispatch_opcode;
            }

            InterpretOpcode(ReturnTrue) {
                slotCopy(++sp, &gSpecialValues.True);
                storeLoadSpAndIp([&]() { returnFromMethod(g); });
                dispatch_opcode;
            }

            InterpretOpcode(ReturnFalse) {
                slotCopy(++sp, &gSpecialValues.False);
                storeLoadSpAndIp([&]() { returnFromMethod(g); });
                dispatch_opcode;
            }

            InterpretOpcode(ReturnNil) {
                slotCopy(++sp, &gSpecialValues.Nil_);
                storeLoadSpAndIp([&]() { returnFromMethod(g); });
                dispatch_opcode;
            }

            InterpretOpcode(JumpIfFalse) {
                // cannot compare with o_false because it is NaN
                if (IsFalse(sp)) {
                    const auto [i1, i0] = JumpIfFalse.pullOperandsFromInstructions(ip);
                    ip += i1.asInt(i0);
                    --sp;
                } else if (IsTrue(sp)) {
                    const auto [i1, i0] = JumpIfFalse.pullOperandsFromInstructions(ip);
                    --sp;
                } else {
                    numArgsPushed = 1;
                    selector = gSpecialSelectors[opmNonBooleanError];
                    slot = sp;
                    goto class_lookup_then_msg_lookup;
                }
                dispatch_opcode;
            }

            InterpretOpcode(JumpIfFalsePushNil) {
                if (IsFalse(sp)) {
                    const auto [i1, i0] = JumpIfFalsePushNil.pullOperandsFromInstructions(ip);
                    ip += i1.asInt(i0);
                    slotCopy(sp, &gSpecialValues.Nil_);
                } else if (IsTrue(sp)) {
                    const auto [i1, i0] = JumpIfFalsePushNil.pullOperandsFromInstructions(ip);
                    --sp;
                } else {
                    numArgsPushed = 1;
                    selector = gSpecialSelectors[opmNonBooleanError];
                    slot = sp;
                    goto class_lookup_then_msg_lookup;
                }
                dispatch_opcode;
            }

            InterpretOpcode(JumpIfFalsePushFalse) {
                if (IsFalse(sp)) {
                    const auto [i1, i0] = JumpIfFalsePushFalse.pullOperandsFromInstructions(ip);
                    ip += i1.asInt(i0);
                } else if (IsTrue(sp)) {
                    const auto [i1, i0] = JumpIfFalsePushFalse.pullOperandsFromInstructions(ip);
                    --sp;
                } else {
                    numArgsPushed = 1;
                    selector = gSpecialSelectors[opmNonBooleanError];
                    slot = sp;
                    goto class_lookup_then_msg_lookup;
                }
                dispatch_opcode;
            }

            InterpretOpcode(JumpIfTruePushTrue) {
                if (IsFalse(sp)) {
                    const auto [i1, i0] = JumpIfTruePushTrue.pullOperandsFromInstructions(ip);
                    --sp;
                } else if (IsTrue(sp)) {
                    const auto [i1, i0] = JumpIfTruePushTrue.pullOperandsFromInstructions(ip);
                    ip += i1.asInt(i0);
                    slotCopy(sp, &gSpecialValues.True);
                } else {
                    numArgsPushed = 1;
                    selector = gSpecialSelectors[opmNonBooleanError];
                    slot = sp;
                    goto class_lookup_then_msg_lookup;
                }
                dispatch_opcode;
            }

            InterpretOpcode(JumpFwd) {
                const auto [i1, i0] = JumpFwd.pullOperandsFromInstructions(ip);
                ip += i1.asInt(i0);
                dispatch_opcode;
            }

            InterpretOpcode(JumpBack) {
                const auto [i1, i0] = JumpBack.pullOperandsFromInstructions(ip);
                ip -= i1.asInt(i0) + (JumpBack.byteSize - 1); // Remove the jump back operands.
                --sp; // Also drops the stack. This saves an opcode in the while loop
                      // which is the only place this opcode is used.
                dispatch_opcode;
            }

            InterpretOpcode(SpecialBinaryOpWithAdverb) {
                const auto [trinaryMath] = SpecialBinaryOpWithAdverb.pullOperandsFromInstructions(ip);
                storeLoadSpAndIp([&, trinaryMath = trinaryMath]() {
                    g->primitiveIndex = (Byte)trinaryMath;
                    doSpecialBinaryArithMsg(g, 3, false);
                });
                dispatch_opcode;
            }

            InterpretOpcode(TailCallReturnFromMethod) {
                ifTailCallOptimise([&]() { g->tailCall = 1; });
                dispatch_opcode;
            }


            ////////////////////////////////////

        class_lookup_then_msg_lookup:
            classobj = classOfSlot(slot);

        msg_lookup : {
            size_t index = slotRawInt(&classobj->classIndex) + selector->u.index;
            PyrMethod* meth = nullptr;
            if (UNLIKELY((selector->flags & sym_Class) != 0)) {
                // You have sent a message which is a class name. This is a bad thing.
                // There are two cases. It is either an illegitimate classname like
                // 1 FooBaz: 2
                // in which case selector->u.index == 0 and you get a message or it is a real one like
                // 1 Object: 2
                // in which case selector->u.index isn't pointing to a method and you get a segfault. So...
                meth = nullptr;
            } else {
                meth = gRowTable[index];
            }

            // and now if meth is null, bail out just like if I don't understand it
            if (UNLIKELY(meth == nullptr || (slotRawSymbol(&meth->name) != selector))) {
                storeLoadSpAndIp([&]() { doesNotUnderstand(g, selector, numArgsPushed, 0); });
                ifTailCallOptimise([&]() { g->tailCall = 0; });
                dispatch_opcode;
            }

            PyrMethodRaw* methraw = METHRAW(meth);
            const auto pushDefaultArgsAndChopUnused = [&]() {
                if (numArgsPushed < methraw->numargs) {
                    // Not enough args pushed.
                    PyrSlot* qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
                    for (int m = 0, mmax = methraw->numargs - numArgsPushed; m < mmax; ++m)
                        slotCopy(++sp, ++qslot);
                    numArgsPushed = methraw->numargs;
                } else if (methraw->varargs == 0 && numArgsPushed > methraw->numargs) {
                    // Too many args.
                    const auto num_slots_to_chop = numArgsPushed - methraw->numargs;
                    sp -= num_slots_to_chop;
                    numArgsPushed = methraw->numargs;
                }
            };

            switch (methraw->methType) {
            case methNormal:
                storeLoadSpAndIp([&]() { executeMethod(g, meth, numArgsPushed, 0); });
                break;

            case methReturnSelf:
                sp -= numArgsPushed - 1;
                break;

            case methReturnLiteral:
                sp -= numArgsPushed - 1;
                slotCopy(sp, &meth->selectors); // in this case selectors is just a single value
                break;

            case methReturnArg:
                sp -= numArgsPushed - 1;
                index = methraw->specialIndex; // zero is index of the first argument
                if (index < numArgsPushed)
                    slotCopy(sp, sp + index);
                else
                    slotCopy(sp, &slotRawObject(&meth->prototypeFrame)->slots[index]);
                break;

            case methReturnInstVar:
                sp -= numArgsPushed - 1;
                index = methraw->specialIndex;
                slotCopy(sp, &slotRawObject(slot)->slots[index]);
                break;

            case methAssignInstVar: {
                sp -= numArgsPushed - 1;
                index = methraw->specialIndex;
                PyrObject* obj = slotRawObject(slot);
                if (obj->IsImmutable()) {
                    StoreToImmutableB(g, sp, ip);
                } else {
                    if (numArgsPushed >= 2) {
                        slotCopy(&obj->slots[index], sp + 1);
                        g->gc->GCWrite(obj, sp + 1);
                    } else
                        slotCopy(&obj->slots[index], &gSpecialValues.Nil_);
                    slotCopy(sp, slot);
                }
                break;
            }

            case methReturnClassVar:
                sp -= numArgsPushed - 1;
                slotCopy(sp, &g->classvars->slots[methraw->specialIndex]);
                break;

            case methAssignClassVar:
                sp -= numArgsPushed - 1;
                if (numArgsPushed >= 2) {
                    slotCopy(&g->classvars->slots[methraw->specialIndex], sp + 1);
                    g->gc->GCWrite(g->classvars, sp + 1);
                } else
                    slotCopy(&g->classvars->slots[methraw->specialIndex], &gSpecialValues.Nil_);
                slotCopy(sp, slot);
                break;

            case methRedirect: /* send a different selector to self */
                pushDefaultArgsAndChopUnused();
                selector = slotRawSymbol(&meth->selectors);
                goto msg_lookup; // Note the recurrsion here!

            case methRedirectSuper: /* send a different selector to self */
                pushDefaultArgsAndChopUnused();
                selector = slotRawSymbol(&meth->selectors);
                classobj = slotRawSymbol(&slotRawClass(&meth->ownerclass)->superclass)->u.classobj;
                goto msg_lookup;

            case methForwardInstVar:
                pushDefaultArgsAndChopUnused();
                selector = slotRawSymbol(&meth->selectors);
                index = methraw->specialIndex;
                slotCopy(slot, &slotRawObject(slot)->slots[index]);
                classobj = classOfSlot(slot);
                goto msg_lookup; // Note the recursion.

            case methForwardClassVar:
                pushDefaultArgsAndChopUnused();
                selector = slotRawSymbol(&meth->selectors);
                slotCopy(slot, &g->classvars->slots[methraw->specialIndex]);
                classobj = classOfSlot(slot);
                goto msg_lookup; // Note the recursion.

            case methPrimitive:
                storeLoadSpAndIp([&]() { doPrimitive(g, meth, numArgsPushed); });
                break;
            }

            ifTailCallOptimise([&]() { g->tailCall = 0; });
            dispatch_opcode;
        }
            ////////////////////////////////////

        key_class_lookup_then_key_msg_lookup:
            classobj = classOfSlot(slot);

        key_msg_lookup : {
            size_t index = slotRawInt(&classobj->classIndex) + selector->u.index;
            PyrMethod* meth = gRowTable[index];

            if (UNLIKELY(slotRawSymbol(&meth->name) != selector)) {
                storeLoadSpAndIp([&]() { doesNotUnderstand(g, selector, numArgsPushed, numKeyArgsPushed); });
                numKeyArgsPushed = 0;
                ifTailCallOptimise([&]() { g->tailCall = 0; });
                dispatch_opcode;
            }
            PyrMethodRaw* methraw;
            methraw = METHRAW(meth);

            switch (methraw->methType) {
            case methNormal:
                storeLoadSpAndIp([&]() { executeMethod(g, meth, numArgsPushed, numKeyArgsPushed); });
                break;

            case methReturnSelf:
                sp -= numArgsPushed - 1;
                break;

            case methReturnLiteral:
                sp -= numArgsPushed - 1;
                slotCopy(sp, &meth->selectors); /* in this case selectors is just a single value */
                break;

            case methReturnArg:
                g->sp = sp;
                numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
                numKeyArgsPushed = 0;
                sp = g->sp;
                sp -= numArgsPushed - 1;
                index = methraw->specialIndex; // zero is index of the first argument
                if (index < numArgsPushed) {
                    slotCopy(sp, sp + index);
                } else {
                    slotCopy(sp, &slotRawObject(&meth->prototypeFrame)->slots[index]);
                }
                break;

            case methReturnInstVar:
                sp -= numArgsPushed - 1;
                index = methraw->specialIndex;
                slotCopy(sp, &slotRawObject(slot)->slots[index]);
                break;

            case methAssignInstVar: {
                sp -= numArgsPushed - 1;
                numArgsPushed -= numKeyArgsPushed << 1;
                index = methraw->specialIndex;
                PyrObject* obj = slotRawObject(slot);
                if (obj->IsImmutable()) {
                    StoreToImmutableB(g, sp, ip);
                } else {
                    if (numArgsPushed >= 2) {
                        slotCopy(&obj->slots[index], sp + 1);
                        g->gc->GCWrite(obj, sp + 1);
                    } else
                        slotCopy(&obj->slots[index], &gSpecialValues.Nil_);
                    slotCopy(sp, slot);
                }
                break;
            }

            case methReturnClassVar:
                sp -= numArgsPushed - 1;
                slotCopy(sp, &g->classvars->slots[methraw->specialIndex]);
                break;

            case methAssignClassVar:
                sp -= numArgsPushed - 1;
                if (numArgsPushed >= 2) {
                    slotCopy(&g->classvars->slots[methraw->specialIndex], sp + 1);
                    g->gc->GCWrite(g->classvars, sp + 1);
                } else
                    slotCopy(&g->classvars->slots[methraw->specialIndex], &gSpecialValues.Nil_);
                slotCopy(sp, slot);
                break;

            case methRedirect: /* send a different selector to self, e.g. this.subclassResponsibility */
                g->sp = sp;
                numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
                numKeyArgsPushed = 0;
                sp = g->sp;
                selector = slotRawSymbol(&meth->selectors);
                goto msg_lookup;

            case methRedirectSuper: /* send a different selector to super */
                g->sp = sp;
                numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
                numKeyArgsPushed = 0;
                sp = g->sp;
                selector = slotRawSymbol(&meth->selectors);
                classobj = slotRawSymbol(&slotRawClass(&meth->ownerclass)->superclass)->u.classobj;
                classobj = slotRawSymbol(&slotRawClass(&meth->ownerclass)->superclass)->u.classobj;

                classobj = slotRawSymbol(&slotRawClass(&meth->ownerclass)->superclass)->u.classobj;

                goto msg_lookup;

            case methForwardInstVar:
                g->sp = sp;
                numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
                numKeyArgsPushed = 0;
                sp = g->sp;
                selector = slotRawSymbol(&meth->selectors);
                index = methraw->specialIndex;
                slotCopy(slot, &slotRawObject(slot)->slots[index]);
                classobj = classOfSlot(slot);
                classobj = classOfSlot(slot);

                classobj = classOfSlot(slot);

                goto msg_lookup;

            case methForwardClassVar:
                g->sp = sp;
                numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
                numKeyArgsPushed = 0;
                sp = g->sp;
                selector = slotRawSymbol(&meth->selectors);
                slotCopy(slot, &g->classvars->slots[methraw->specialIndex]);
                classobj = classOfSlot(slot);
                classobj = classOfSlot(slot);

                classobj = classOfSlot(slot);

                goto msg_lookup;

            case methPrimitive:
                storeLoadSpAndIp([&]() { doPrimitiveWithKeys(g, meth, numArgsPushed, numKeyArgsPushed); });
                break;
            }

            numKeyArgsPushed = 0;
            ifTailCallOptimise([&]() { g->tailCall = 0; });
            dispatch_opcode;
        }


            // End of Intrepreting Opcodes
        } // switch(op1)
    } // end while(running)
#ifndef _WIN32
    running = true; // reset the signal
#endif
    g->sp = sp;
    g->ip = ip;
}


#if defined(__GNUC__) && !defined(__clang__)
#    pragma GCC pop_options
#endif


void DumpSimpleBackTrace(VMGlobals* g);
void DumpSimpleBackTrace(VMGlobals* g) {
    int i;
    PyrFrame* frame;

    post("CALL STACK:\n");
    // print the variables and arguments for the most recent frames in the
    // call graph
    frame = g->frame;

    for (i = 0; i < 16; ++i) {
        char str[256];
        slotOneWord(&frame->method, str);
        post("%s ip %d\n", str,
             (char*)slotRawPtr(&frame->ip) - (char*)slotRawObject(&slotRawMethod(&frame->method)->code)->slots);
        frame = slotRawFrame(&frame->caller);
        if (!frame)
            break;
    }
    if (frame) {
        post("...\n");
    }
    // DumpStack(g, g->sp);
}

void DumpBackTrace(VMGlobals* g) {
    int i;
    PyrFrame* frame;

    post("CALL STACK:\n");
    // print the variables and arguments for the most recent frames in the
    // call graph
    frame = g->frame;

    for (i = 0; i < 16; ++i) {
        if (FrameSanity(frame, "DumpBackTrace")) {
            post("FRAME CORRUPTED\n");
            return;
        }
        DumpFrame(frame);
        frame = slotRawFrame(&frame->caller);
        if (!frame)
            break;
    }
    if (frame) {
        post("...\n");
    }
    // DumpStack(g, g->sp);
}

void DumpDetailedFrame(PyrFrame* frame);
void DumpDetailedBackTrace(VMGlobals* g);
void DumpDetailedBackTrace(VMGlobals* g) {
    int i;
    PyrFrame* frame;

    post("CALL STACK:\n");
    // print the variables and arguments for the most recent frames in the
    // call graph
    frame = g->frame;

    for (i = 0; i < 16; ++i) {
        if (FrameSanity(frame, "DumpDetailedBackTrace")) {
            post("FRAME CORRUPTED\n");
            return;
        }
        DumpDetailedFrame(frame);
        frame = slotRawFrame(&frame->caller);
        if (!frame)
            break;
    }
    if (frame) {
        post("...\n");
    }
    // DumpStack(g, g->sp);
}

void DumpStack(VMGlobals* g, PyrSlot* sp) {
    PyrSlot* slot;
    char str[128];
#if BCSTAT
    dumpbcstat();
#endif
    postfl("STACK:\n");
    slot = sp - 64;
    if (slot < g->gc->Stack()->slots)
        slot = g->gc->Stack()->slots;
    for (size_t i = slot - g->gc->Stack()->slots; slot <= sp; slot++, ++i) {
        slotString(slot, str);
        post("   %2d  %s\n", i, str);
    }
}
