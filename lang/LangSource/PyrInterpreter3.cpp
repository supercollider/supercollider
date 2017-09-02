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
# include <signal.h>

#include <boost/chrono.hpp>

#include <float.h>
#define kBigBigFloat DBL_MAX
#define kSmallSmallFloat DBL_MIN


#include <new>
#include "InitAlloc.h"
#include "function_attributes.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Warray-bounds"
#endif


//void tellPlugInsAboutToRun();
double timeNow();


int32 timeseed()
{
	using namespace std::chrono;

	high_resolution_clock::time_point now = high_resolution_clock::now();
	high_resolution_clock::duration since_epoch = now.time_since_epoch();

	seconds     secs     = duration_cast<seconds>(since_epoch);
	nanoseconds nanosecs = since_epoch - secs;

	int_least64_t seed = secs.count() ^ nanosecs.count();

	return (int32)seed;
}

VMGlobals gVMGlobals;
VMGlobals *gMainVMGlobals = &gVMGlobals;

void debugf(char *fmt, ...) ;

#define DEBUGINTERPRETER 0
#define METHODMETER 0
#define BCSTAT 0
#define CHECK_MAX_STACK_USE 0


#if CHECK_MAX_STACK_USE
int gMaxStackDepth = 0;
#endif

unsigned char* dumpOneByteCode(PyrBlock *theBlock, PyrClass* theClass, unsigned char *ip);
void dumpSlotOneWord(const char *tagstr, PyrSlot *slot);
//bool checkAllObjChecksum(PyrObject* obj);

bool gTraceInterpreter = false;
//bool gTraceInterpreter = true;


const char* byteCodeString(int code);

extern int gNumClasses;
extern PyrClass *gClassList;

// runInterpreter has 7 call sites:
//	compileLibrary (4)
// 	runLibrary
// 	interpretCmdLine

static void endInterpreter(VMGlobals *g);


SCLANG_DLLEXPORT_C void runInterpreter(VMGlobals *g, PyrSymbol *selector, int numArgsPushed)
{
		//postfl("->runInterpreter\n");
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
		//postfl(" >initInterpreter\n");

	if (initInterpreter(g, selector, numArgsPushed)) {
#ifdef GC_SANITYCHECK
		g->gc->SanityCheck();
#endif
//        if (strcmp(selector->name, "tick") != 0) post("%s %d  execMethod %d\n", selector->name, numArgsPushed, g->execMethod);
	//post("->Interpret thread %p\n", g->thread);
		if (g->execMethod) Interpret(g);
	//post("<-Interpret thread %p\n", g->thread);
#ifdef GC_SANITYCHECK
		g->gc->SanityCheck();
#endif
	}

	//postfl(" >endInterpreter\n");
	endInterpreter(g);
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
		//postfl("<-runInterpreter\n");
	//dumpGCStats(g->gc);
}

static bool initAwakeMessage(VMGlobals *g);

void runAwakeMessage(VMGlobals *g)
{
	if (initAwakeMessage(g)) {
		if (g->execMethod) Interpret(g);
	}
	endInterpreter(g);
}

void initPyrThread(VMGlobals *g, PyrThread *thread, PyrSlot *func, int stacksize, PyrInt32Array* rgenArray,
	double beats, double seconds, PyrSlot* clock, bool runGC);
int32 timeseed();

PyrProcess* newPyrProcess(VMGlobals *g, PyrClass *procclassobj)
{
	PyrGC* gc = g->gc;
	PyrProcess * proc = (PyrProcess*)instantiateObject(gc, procclassobj, 0, true, false);

	PyrObject *sysSchedulerQueue = newPyrArray(gc, 4096, 0, false);
	sysSchedulerQueue->size = 1;
	SetInt(sysSchedulerQueue->slots + 0, 0); // stability count
	SetObject(&proc->sysSchedulerQueue, sysSchedulerQueue);

	PyrObject *classVars = newPyrArray(gc, gNumClassVars, 0, false);
	classVars->size = gNumClassVars;
	nilSlots(classVars->slots, gNumClassVars);
	SetObject(&proc->classVars, classVars);
	g->classvars = classVars;

	// fill class vars from prototypes:
	PyrClass * classobj = gClassList;
	while (classobj) {
		if (IsObj(&classobj->cprototype)) {
			int numClassVars = slotRawObject(&classobj->cprototype)->size;
			if (numClassVars > 0) {
				memcpy(g->classvars->slots + slotRawInt(&classobj->classVarIndex), slotRawObject(&classobj->cprototype)->slots, numClassVars * sizeof(PyrSlot));
			}
		}
		classobj = slotRawClass(&classobj->nextclass);
	}

	SetNil(&proc->nowExecutingPath);

	class_thread = getsym("Thread")->u.classobj;
	if (class_thread) {
		SetNil(&proc->curThread);
		PyrThread * thread = (PyrThread*)instantiateObject(gc, class_thread, 0, true, false);
		//SetObject(&threadsArray->slots[0], thread);
		SetObject(&proc->mainThread, thread);
		PyrInt32Array *rgenArray = newPyrInt32Array(gc, 4, 0, false);
		rgenArray->size = 4;
		rgenArray->i[0] = 0;
		rgenArray->i[1] = 0;
		rgenArray->i[2] = 0;
		rgenArray->i[3] = 0;
		((RGen*)(rgenArray->i))->init(timeseed());

		PyrSlot clockSlot;
		SetObject(&clockSlot, s_systemclock->u.classobj);
		initPyrThread(g, thread, &o_nil, EVALSTACKDEPTH, rgenArray, 0., 0., &clockSlot, false);
		//postfl("elapsedTime %.6f\n", elapsedTime());
	} else {
		error("Class Thread not found.\n");
	}

	PyrSymbol * contextsym = getsym("functionCompileContext");
	size_t index = slotRawInt(&class_interpreter->classIndex) + contextsym->u.index;
	PyrMethod * meth = gRowTable[index];
	if (!meth || slotRawSymbol(&meth->name) != contextsym) {
		error("compile context method 'functionCompileContext' not found.\n");
		//SetNil(&proc->interpreter);
	} else {
		PyrObject *proto;
		PyrFrame *frame;
		PyrMethodRaw *methraw;

		PyrInterpreter * interpreter = (PyrInterpreter*)instantiateObject(gc, class_interpreter, 0, true, false);
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
		SetPtr(&frame->ip,  0);
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
void clearbcstat()
{
	int i, j;
	for (i=0; i<256; ++i) {
		bcstat[i] = 0;
		for (j=0; j<256; ++j) {
			bcpair[i][j] = 0;
		}
	}
}

void dumpbcstat();
void dumpbcstat()
{
	FILE *file;
	int i, j, k, total;

	file = fopen("bcstat", "w");
	if (file) {
		fprintf(file, "----------\n");
		total = 0;
		for (i=0; i<256; ++i) {
			total +=  bcstat[i];
			if (bcstat[i]) fprintf(file, "%3d %8d  %-32s\n", i, bcstat[i], byteCodeString(i));
		}
		fprintf(file, "\ntotal %d\n", total);
		fprintf(file, "-----cur,next-----\n");
		for (i=0, k=0; i<256; ++i) {
			for (j=0; j<256; j++) {
				if (bcpair[i][j] > 0) {
					fprintf(file, "%4d %3d %3d %8d %-32s %-32s\n", k++, i, j, bcpair[i][j], byteCodeString(i), byteCodeString(j));
				}
			}
		}
		fprintf(file, "-----cur,prev-----\n");
		for (i=0, k=0; i<256; ++i) {
			for (j=0; j<256; j++) {
				if (bcpair[j][i] > 0) {
					fprintf(file, "%4d %3d %3d %8d %-32s %-32s\n", k++, i, j, bcpair[j][i], byteCodeString(i), byteCodeString(j));
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
static void handleSigUsr1(int param)
{
  printf("handleSigUsr1()...\n");
  running = false;
}
#endif

bool initRuntime(VMGlobals *g, int poolSize, AllocPool *inPool)
{
	/*
		create a GC environment
		create a vmachine instance of main
		initialize VMGlobals
	*/

	PyrClass * class_main = s_main->u.classobj;

	if (!class_main) { error("Class 'Main' not defined.\n"); return false; }
	if (!isSubclassOf(class_main, class_process)) {
		error("Class Main is not a subclass of Process.\n");
		return false;
	}

	// create GC environment, process
	g->allocPool = inPool;
	g->gc = (PyrGC*)g->allocPool->Alloc(sizeof(PyrGC));
	new (g->gc) PyrGC(g, g->allocPool, class_main, poolSize);
	g->thread = slotRawThread(&g->process->mainThread);
	SetObject(&g->receiver, g->process);

	// these will be set up when the run method is called
	g->method = NULL;
	g->block = NULL;
	g->frame = NULL;
	g->ip = NULL;

	// initialize process random number generator
	g->rgen = (RGen*)(slotRawObject(&g->thread->randData)->slots);

	initThreads();
	initPatterns();
	initUniqueMethods();
	initGUI();
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif
	//tellPlugInsAboutToRun();

#ifndef _WIN32
	signal(SIGUSR1,handleSigUsr1);
#endif

	assert((g->gc->SanityCheck()));
#ifdef GC_SANITYCHECK
	g->gc->SanityCheck();
#endif

	return true;
}

static bool initAwakeMessage(VMGlobals *g)
{
	//post("initAwakeMessage %p %p\n", g->thread, slotRawThread(&g->process->mainThread));
	slotCopy(&g->process->curThread, &g->process->mainThread); //??
	g->thread = slotRawThread(&g->process->mainThread); //??

	// these will be set up when the run method is called
	g->method = NULL;
	g->block = NULL;
	g->frame = NULL;
	g->ip = NULL;
	g->execMethod = 0;

	// set process as the receiver
	PyrSlot *slot = g->sp - 3;
	slotCopy(&g->receiver, slot);

	SetFloat(&g->thread->beats, slotRawFloat(&slot[1]));
	SetFloat(&g->thread->seconds, slotRawFloat(&slot[2]));
	slotCopy(&g->thread->clock, &slot[3]);
	g->gc->GCWrite(g->thread, slot+3);

	// start it
	sendMessage(g, s_awake, 4);

	return g->method != NULL;
}

bool initInterpreter(VMGlobals *g, PyrSymbol *selector, int numArgsPushed)
{
	slotCopy(&g->process->curThread, &g->process->mainThread);
	g->thread = slotRawThread(&g->process->mainThread);

	// these will be set up when the run method is called
#if TAILCALLOPTIMIZE
	g->tailCall = 0;
#endif
	g->method = NULL;
	g->block = NULL;
	g->frame = NULL;
	g->ip = NULL;
	g->execMethod = 0;
	double elapsed = elapsedTime();
	SetFloat(&g->thread->beats, elapsed);
	SetFloat(&g->thread->seconds, elapsed);
	SetObject(&g->thread->clock, s_systemclock->u.classobj);
	g->gc->GCWrite(g->thread, s_systemclock->u.classobj);

	// set process as the receiver
	PyrSlot *slot = g->sp - numArgsPushed + 1;
	slotCopy(&g->receiver, slot);

	// start it
	sendMessage(g, selector, numArgsPushed);

	return g->method != NULL;
}


static void endInterpreter(VMGlobals *g)
{
	slotCopy(&g->result, g->sp);
//	dumpObjectSlot(&g->result);
	g->gc->Stack()->size = 0;
	g->sp = g->gc->Stack()->slots - 1;
	g->gc->LazyCollect();
}


static void StoreToImmutableA(VMGlobals *g, PyrSlot *& sp, unsigned char *& ip)
{
	// only the value is on the stack
	slotCopy(sp + 1, sp); // copy value up one
	slotCopy(sp, &g->receiver); // put receiver in place
	sp++;
	g->sp = sp;
	g->ip = ip;
	post("StoreToImmutableA\n");
	dumpObjectSlot(sp-1);
	dumpObjectSlot(sp);
	sendMessage(g, getsym("immutableError"), 2);
	sp = g->sp;
	ip = g->ip;
}

void StoreToImmutableB(VMGlobals *g, PyrSlot *& sp, unsigned char *& ip)
{
	// receiver and value are on the stack.
	sp++;
	g->sp = sp;
	g->ip = ip;
	post("StoreToImmutableB\n");
	dumpObjectSlot(sp-1);
	dumpObjectSlot(sp);
	PyrSymbol *selector = getsym("immutableError");
	sendMessage(g, selector, 2);
	sp = g->sp;
	ip = g->ip;
}


void dumpByteCodes(PyrBlock *theBlock);

static inline void handlePushClassVar(VMGlobals* g, PyrSlot *& sp, unsigned char *& ip, unsigned char op2)
{
	unsigned char op3 = ip[1]; ++ip; // get class var index
	slotCopy(++sp, &g->classvars->slots[(op2<<8)|op3]);
}

static inline void handleStoreInstVar(VMGlobals* g, PyrSlot *& sp, unsigned char *& ip, unsigned int index)
{
	PyrObject* obj = slotRawObject(&g->receiver);
	if (obj->IsImmutable())
		StoreToImmutableA(g, sp, ip);
	else {
		PyrSlot * slot = obj->slots + index;
		slotCopy(slot, sp--);
		g->gc->GCWrite(obj, slot);
	}
}

static inline void handleSendSpecialUnaryArithMsg(VMGlobals* g, PyrSlot *& sp, unsigned char *& ip, unsigned char op1)
{
	g->sp = sp; g->ip = ip;
	g->primitiveIndex = op1 & 15;
	doSpecialUnaryArithMsg(g, -1);
	sp = g->sp; ip = g->ip;
}

static inline void handleSendSpecialBinaryArithMsg(VMGlobals* g, PyrSlot *& sp, unsigned char *& ip, unsigned char op1)
{
	g->sp = sp; g->ip = ip;
	g->primitiveIndex = op1 & 15;
	doSpecialBinaryArithMsg(g, 2, false);
	sp = g->sp; ip = g->ip;
}

static inline bool checkStackOverflow(VMGlobals* g, PyrSlot * sp)
{
	PyrObject * stack = g->gc->Stack();
	int depth = sp - stack->slots;
	return depth < slotRawInt(&g->thread->stackSize);
}

static inline void checkStackDepth(VMGlobals* g, PyrSlot * sp)
{
#if CHECK_MAX_STACK_USE
	int stackDepth = sp - g->sp + 1;
	if (stackDepth > gMaxStackDepth) {
		gMaxStackDepth = stackDepth;
		printf("gMaxStackDepth %d\n", gMaxStackDepth);
	}
#endif
}

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#define LABELS_AS_VALUES
#endif

#ifdef LABELS_AS_VALUES
#define dispatch_opcode \
	op1 = ip[1];		\
	++ip;				\
	checkStackDepth(g, sp); \
	assert(checkStackOverflow(g, sp)); \
	goto *opcode_labels[op1]
#else
#define dispatch_opcode break
#endif

#if defined(__GNUC__)
#define LIKELY(x)       __builtin_expect((x),1)
#define UNLIKELY(x)     __builtin_expect((x),0)
#else
#define LIKELY(x)   x
#define UNLIKELY(x) x
#endif

#if defined(__GNUC__) && !defined(__clang__)
// gcc manual:
// Note: When compiling a program using computed gotos, a GCC extension, you may get better run-time performance if
//       you disable the global common subexpression elimination pass by adding -fno-gcse to the command line.
#pragma GCC push_options
#pragma GCC optimize("-fno-gcse")
#endif

HOT void Interpret(VMGlobals *g)
{
	// byte code values
	unsigned char *ip;
	unsigned char op1;

	// interpreter globals
	int numArgsPushed, numKeyArgsPushed;
	PyrSymbol *selector;
	PyrClass *classobj;

	// temporary variables used in the interpreter
	PyrSlot *slot;
	PyrSlot *sp;

#ifdef LABELS_AS_VALUES
	static void * opcode_labels[] = {
		&&handle_op_0,
		&&handle_op_1,
		&&handle_op_2,
		&&handle_op_3,
		&&handle_op_4,
		&&handle_op_5,
		&&handle_op_6,
		&&handle_op_7,
		&&handle_op_8,
		&&handle_op_9,
		&&handle_op_10,
		&&handle_op_11,
		&&handle_op_12,
		&&handle_op_13,
		&&handle_op_14,
		&&handle_op_15,
		&&handle_op_16,
		&&handle_op_17,
		&&handle_op_18,
		&&handle_op_19,
		&&handle_op_20,
		&&handle_op_21,
		&&handle_op_22,
		&&handle_op_23,
		&&handle_op_24,
		&&handle_op_25,
		&&handle_op_26,
		&&handle_op_27,
		&&handle_op_28,
		&&handle_op_29,
		&&handle_op_30,
		&&handle_op_31,
		&&handle_op_32,
		&&handle_op_33,
		&&handle_op_34,
		&&handle_op_35,
		&&handle_op_36,
		&&handle_op_37,
		&&handle_op_38,
		&&handle_op_39,
		&&handle_op_40,
		&&handle_op_41,
		&&handle_op_42,
		&&handle_op_43,
		&&handle_op_44,
		&&handle_op_45,
		&&handle_op_46,
		&&handle_op_47,
		&&handle_op_48,
		&&handle_op_49,
		&&handle_op_50,
		&&handle_op_51,
		&&handle_op_52,
		&&handle_op_53,
		&&handle_op_54,
		&&handle_op_55,
		&&handle_op_56,
		&&handle_op_57,
		&&handle_op_58,
		&&handle_op_59,
		&&handle_op_60,
		&&handle_op_61,
		&&handle_op_62,
		&&handle_op_63,
		&&handle_op_64,
		&&handle_op_65,
		&&handle_op_66,
		&&handle_op_67,
		&&handle_op_68,
		&&handle_op_69,
		&&handle_op_70,
		&&handle_op_71,
		&&handle_op_72,
		&&handle_op_73,
		&&handle_op_74,
		&&handle_op_75,
		&&handle_op_76,
		&&handle_op_77,
		&&handle_op_78,
		&&handle_op_79,
		&&handle_op_80,
		&&handle_op_81,
		&&handle_op_82,
		&&handle_op_83,
		&&handle_op_84,
		&&handle_op_85,
		&&handle_op_86,
		&&handle_op_87,
		&&handle_op_88,
		&&handle_op_89,
		&&handle_op_90,
		&&handle_op_91,
		&&handle_op_92,
		&&handle_op_93,
		&&handle_op_94,
		&&handle_op_95,
		&&handle_op_96,
		&&handle_op_97,
		&&handle_op_98,
		&&handle_op_99,
		&&handle_op_100,
		&&handle_op_101,
		&&handle_op_102,
		&&handle_op_103,
		&&handle_op_104,
		&&handle_op_105,
		&&handle_op_106,
		&&handle_op_107,
		&&handle_op_108,
		&&handle_op_109,
		&&handle_op_110,
		&&handle_op_111,
		&&handle_op_112,
		&&handle_op_113,
		&&handle_op_114,
		&&handle_op_115,
		&&handle_op_116,
		&&handle_op_117,
		&&handle_op_118,
		&&handle_op_119,
		&&handle_op_120,
		&&handle_op_121,
		&&handle_op_122,
		&&handle_op_123,
		&&handle_op_124,
		&&handle_op_125,
		&&handle_op_126,
		&&handle_op_127,
		&&handle_op_128,
		&&handle_op_129,
		&&handle_op_130,
		&&handle_op_131,
		&&handle_op_132,
		&&handle_op_133,
		&&handle_op_134,
		&&handle_op_135,
		&&handle_op_136,
		&&handle_op_137,
		&&handle_op_138,
		&&handle_op_139,
		&&handle_op_140,
		&&handle_op_141,
		&&handle_op_142,
		&&handle_op_143,
		&&handle_op_144,
		&&handle_op_145,
		&&handle_op_146,
		&&handle_op_147,
		&&handle_op_148,
		&&handle_op_149,
		&&handle_op_150,
		&&handle_op_151,
		&&handle_op_152,
		&&handle_op_153,
		&&handle_op_154,
		&&handle_op_155,
		&&handle_op_156,
		&&handle_op_157,
		&&handle_op_158,
		&&handle_op_159,
		&&handle_op_160,
		&&handle_op_161,
		&&handle_op_162,
		&&handle_op_163,
		&&handle_op_164,
		&&handle_op_165,
		&&handle_op_166,
		&&handle_op_167,
		&&handle_op_168,
		&&handle_op_169,
		&&handle_op_170,
		&&handle_op_171,
		&&handle_op_172,
		&&handle_op_173,
		&&handle_op_174,
		&&handle_op_175,
		&&handle_op_176,
		&&handle_op_177,
		&&handle_op_178,
		&&handle_op_179,
		&&handle_op_180,
		&&handle_op_181,
		&&handle_op_182,
		&&handle_op_183,
		&&handle_op_184,
		&&handle_op_185,
		&&handle_op_186,
		&&handle_op_187,
		&&handle_op_188,
		&&handle_op_189,
		&&handle_op_190,
		&&handle_op_191,
		&&handle_op_192,
		&&handle_op_193,
		&&handle_op_194,
		&&handle_op_195,
		&&handle_op_196,
		&&handle_op_197,
		&&handle_op_198,
		&&handle_op_199,
		&&handle_op_200,
		&&handle_op_201,
		&&handle_op_202,
		&&handle_op_203,
		&&handle_op_204,
		&&handle_op_205,
		&&handle_op_206,
		&&handle_op_207,
		&&handle_op_208,
		&&handle_op_209,
		&&handle_op_210,
		&&handle_op_211,
		&&handle_op_212,
		&&handle_op_213,
		&&handle_op_214,
		&&handle_op_215,
		&&handle_op_216,
		&&handle_op_217,
		&&handle_op_218,
		&&handle_op_219,
		&&handle_op_220,
		&&handle_op_221,
		&&handle_op_222,
		&&handle_op_223,
		&&handle_op_224,
		&&handle_op_225,
		&&handle_op_226,
		&&handle_op_227,
		&&handle_op_228,
		&&handle_op_229,
		&&handle_op_230,
		&&handle_op_231,
		&&handle_op_232,
		&&handle_op_233,
		&&handle_op_234,
		&&handle_op_235,
		&&handle_op_236,
		&&handle_op_237,
		&&handle_op_238,
		&&handle_op_239,
		&&handle_op_240,
		&&handle_op_241,
		&&handle_op_242,
		&&handle_op_243,
		&&handle_op_244,
		&&handle_op_245,
		&&handle_op_246,
		&&handle_op_247,
		&&handle_op_248,
		&&handle_op_249,
		&&handle_op_250,
		&&handle_op_251,
		&&handle_op_252,
		&&handle_op_253,
		&&handle_op_254,
		&&handle_op_255
	};

#endif

#if 0
	unsigned char *bzero;
	PyrSlot *szero;
	char str[80];
#endif


#if BCSTAT
	//clearbcstat();
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
	while (running) {  // not going to indent body to save line space
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
	//printf("op1 %d\n", op1);
	//postfl("sp %p   frame %p  caller %p  ip %p\n", sp, g->frame, g->frame->caller.uof, slotRawInt(&g->frame->caller.uof->ip));
	//postfl("sp %p   frame %p   diff %d    caller %p\n", sp, g->frame, ((int)sp - (int)g->frame)>>3, g->frame->caller.uof);
#if DEBUGINTERPRETER
	if (gTraceInterpreter) {
		//DumpStack(g, sp);
		if (FrameSanity(g->frame, "dbgint")) {
			//Debugger();
		}
		//g->gc->SanityCheck();
		//assert(g->gc->SanityCheck());
			g->sp = sp; g->ip = ip;
		g->gc->FullCollection();
			sp = g->sp; ip = g->ip;
		postfl("[%3d] %20s-%-16s  ",
			(sp - g->gc->Stack()->slots) + 1,
			slotRawSymbol(&slotRawClass(&g->method->ownerclass)->name)->name, slotRawSymbol(&g->method->name)->name);
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

	switch (op1) {
		case 0 : //	push class
		handle_op_0: {
			int op2 = ip[1]; ++ip; // get literal index
			classobj = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2])->u.classobj;
			if (classobj) {
				++sp; SetObject(sp, classobj);
			} else {
				postfl("Execution warning: Class '%s' not found\n", slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2])->name);
				slotCopy(++sp, &gSpecialValues[svNil]);
			}
			dispatch_opcode;
		}
		case 1 : // opExtended, opPushInstVar
		handle_op_1: {
			int op2 = ip[1]; ++ip; // get inst var index
			slotCopy(++sp, &slotRawObject(&g->receiver)->slots[op2]);
			dispatch_opcode;
		}
		case 2 : // opExtended, opPushTempVar
		handle_op_2: {
			int op2 = ip[1]; // get temp var level
			int op3 = ip[2]; // get temp var index
			ip += 2;
			PyrFrame *tframe = g->frame;
			for (; op2--; tframe = slotRawFrame(&tframe->context)) { /* noop */ }
			slotCopy(++sp, &tframe->vars[op3]);
			dispatch_opcode;
		}
		case 3 : // opExtended, opPushTempZeroVar
		handle_op_3: {
			int op2 = ip[1]; ++ip; // get temp var index
			slotCopy(++sp, &g->frame->vars[op2]);
			dispatch_opcode;
		}
		case 4 : // opExtended, opPushLiteral
		handle_op_4: {
			int op2 = ip[1]; ++ip; // get literal index
			// push a block as a closure if it is one
			slot = slotRawObject(&g->block->selectors)->slots + op2;
			if (IsObj(slot) && slotRawObject(slot)->classptr == gSpecialClasses[op_class_fundef]->u.classobj) {
				// push a closure
				g->sp = sp; // gc may push the stack
				PyrClosure * closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, true);
				sp = g->sp;
				closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
				closure->size = 2;
				slotCopy(&closure->block, slot);
				if (IsNil(&slotRawBlock(slot)->contextDef)) {
					slotCopy(&closure->context, &slotRawInterpreter(&g->process->interpreter)->context);
				} else {
					SetObject(&closure->context, g->frame);
				}
				++sp; SetObject(sp, closure);
			} else {
				slotCopy(++sp, slot);
			}
			dispatch_opcode;
		}
		case 5 : // opExtended, opPushClassVar
		handle_op_5: {
			int op2 = ip[1]; // get class
			int op3 = ip[2]; // get class var index
			ip += 2;
			slotCopy(++sp, &g->classvars->slots[(op2<<8)|op3]);
			dispatch_opcode;
		}
		case 6 :  // opExtended, opPushSpecialValue == push a special class
		handle_op_6: {
			int op2 = ip[1]; ++ip; // get class name index
			classobj = gSpecialClasses[op2]->u.classobj;
			if (classobj) {
				++sp; SetObject(sp, classobj);
			} else {
				slotCopy(++sp, &gSpecialValues[svNil]);
			}
			dispatch_opcode;
		}
		case 7 : // opExtended, opStoreInstVar
		handle_op_7: {
			int op2 = ip[1]; ++ip; // get inst var index
			PyrObject * obj = slotRawObject(&g->receiver);
			if (obj->IsImmutable()) { StoreToImmutableA(g, sp, ip); }
			else {
				slot = obj->slots + op2;
				slotCopy(slot, sp);
				g->gc->GCWrite(obj, slot);
			}
			dispatch_opcode;
		}
		case 8 : // opExtended, opStoreTempVar
		handle_op_8: {
			int op2 = ip[1]; // get temp var level
			int op3 = ip[2]; // get temp var index
			ip += 2;
			PyrFrame * tframe = g->frame;
			for (; op2--; tframe = slotRawFrame(&tframe->context)) { /* noop */ }
			slot = tframe->vars + op3;
			slotCopy(slot, sp);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}
		case 9 : // opExtended, opStoreClassVar
		handle_op_9: {
			int op2 = ip[1]; // get index of class name literal
			int op3 = ip[2]; // get class var index
			ip += 2;
			slotCopy(&g->classvars->slots[(op2<<8)|op3], sp);
			g->gc->GCWrite(g->classvars, sp);
			dispatch_opcode;
		}
		case 10 : // opExtended, opSendMsg
		handle_op_10: {
			numArgsPushed = ip[1]; // get num args
			numKeyArgsPushed = ip[2]; // get num keyword args
			int op3 = ip[3]; // get selector index
			ip += 3;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op3]);

			slot = sp - numArgsPushed + 1;

			if (numKeyArgsPushed) goto key_class_lookup;
			else goto class_lookup;

		}
		case 11 : // opExtended, opSendSuper
		handle_op_11: {
			numArgsPushed = ip[1]; // get num args
			numKeyArgsPushed = ip[2]; // get num keyword args
			int op3 = ip[3]; // get selector index
			ip += 3;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op3]);

			slot = g->sp - numArgsPushed + 1;
			classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;

			if (numKeyArgsPushed) goto key_msg_lookup;
			else goto msg_lookup;
		}

		case 12 :  // opExtended, opSendSpecialMsg
		handle_op_12: {
			numArgsPushed = ip[1]; // get num args
			numKeyArgsPushed = ip[2]; // get num keyword args
			int op3 = ip[3]; // get selector index
			ip += 3;

			selector = gSpecialSelectors[op3];
			slot = sp - numArgsPushed + 1;

			if (numKeyArgsPushed) goto key_class_lookup;
			else goto class_lookup;
		}

		case 13 :  // opExtended, opSendSpecialUnaryArithMsg
		handle_op_13: {
			int op2 = ip[1]; ++ip; // get selector index
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op2;
			doSpecialUnaryArithMsg(g, -1);
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		}
		case 14 :  // opExtended, opSendSpecialBinaryArithMsg
		handle_op_14: {
			int op2 = ip[1]; ++ip; // get selector index
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op2;
			doSpecialBinaryArithMsg(g, 2, false);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		}
		case 15 : // opExtended, opSpecialOpcode (none yet)
		handle_op_15: {
			int op2 = ip[1]; ++ip; // get extended special opcode
			switch (op2) {
				case opgProcess : // push thisProcess
					++sp; SetObject(sp, g->process); break;
				case opgThread : // push thisProcess
					++sp; SetObject(sp, g->thread); break;
				case opgMethod : // push thisMethod
					++sp; SetObject(sp, g->method); break;
				case opgFunctionDef : // push thisFunctionDef
					++sp; SetObject(sp, g->block); break;
				case opgFunction : { // push thisFunc
					// push a closure
					g->sp = sp; // gc may push the stack
					PyrClosure * closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, true);
					sp = g->sp;
					closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
					closure->size = 2;
					SetObject(&closure->block, g->block);
					SetObject(&closure->context, slotRawFrame(&g->frame->context));
					++sp; SetObject(sp, closure);
					break;
				}
				default :
					slotCopy(++sp, &gSpecialValues[svNil]); break;
			}
			dispatch_opcode;
		}
		// opPushInstVar, 0..15
		case 16 : handle_op_16: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 0]); dispatch_opcode;
		case 17 : handle_op_17: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 1]); dispatch_opcode;
		case 18 : handle_op_18: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 2]); dispatch_opcode;
		case 19 : handle_op_19: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 3]); dispatch_opcode;
		case 20 : handle_op_20: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 4]); dispatch_opcode;
		case 21 : handle_op_21: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 5]); dispatch_opcode;
		case 22 : handle_op_22: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 6]); dispatch_opcode;
		case 23 : handle_op_23: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 7]); dispatch_opcode;
		case 24 : handle_op_24: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 8]); dispatch_opcode;
		case 25 : handle_op_25: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 9]); dispatch_opcode;
		case 26 : handle_op_26: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[10]); dispatch_opcode;
		case 27 : handle_op_27: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[11]); dispatch_opcode;
		case 28 : handle_op_28: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[12]); dispatch_opcode;
		case 29 : handle_op_29: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[13]); dispatch_opcode;
		case 30 : handle_op_30: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[14]); dispatch_opcode;
		case 31 : handle_op_31: slotCopy(++sp, &slotRawObject(&g->receiver)->slots[15]); dispatch_opcode;

		case 32 : // JumpIfTrue
		handle_op_32:
			// cannot compare with o_false because it is NaN
			if ( IsTrue(sp) ) {
				int jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
			} else if ( IsFalse(sp)) {
				ip+=2;
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			--sp;
			dispatch_opcode;

		// opPushTempVar, levels 1..7
		case 33 : handle_op_33:
			slotCopy(++sp, &slotRawFrame(&g->frame->context)->vars[ip[1]]); ++ip; dispatch_opcode;
		case 34 : handle_op_34:
			slotCopy(++sp, &slotRawFrame(&slotRawFrame(&g->frame->context)->context)->vars[ip[1]]); ++ip; dispatch_opcode;
		case 35 : handle_op_35:
			slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->vars[ip[1]]);
			++ip; dispatch_opcode;
		case 36 : handle_op_36:
			slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->
					context)->vars[ip[1]]); ++ip; dispatch_opcode;
		case 37 : handle_op_37:
			slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->
					context)->context)->context)->context)->vars[ip[1]]); ++ip; dispatch_opcode;
		case 38 : handle_op_38:
			slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->
					context)->context)->context)->context)->context)->vars[ip[1]]); ++ip; dispatch_opcode;
		case 39 : handle_op_39:
			slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->
					context)->context)->context)->context)->context)->context)->vars[ip[1]]); ++ip; dispatch_opcode;

		// push literal constants.
		case 40 :
		handle_op_40: {
			int ival = ip[1];
			ip+=1;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			dispatch_opcode;
		}
		case 41 :
		handle_op_41: {
			int ival = (ip[1] << 8) | ip[2];
			ip+=2;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			dispatch_opcode;
		}
		case 42 :
		handle_op_42: {
			int ival = (ip[1] << 16) | (ip[2] << 8) | ip[3];
			ip+=3;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			dispatch_opcode;
		}
		case 43 :
		handle_op_43: {
			int ival = (ip[1] << 24) | (ip[2] << 16) | (ip[3] << 8) | ip[4];
			ip+=4;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			dispatch_opcode;
		}
		// push integers.
		case 44 :
		handle_op_44: {
			int ival = (int32)(ip[1] << 24) >> 24;
			ip+=1;
			++sp; SetInt(sp, ival);
			dispatch_opcode;
		}
		case 45 :
		handle_op_45: {
			int ival = (int32)((ip[1] << 24) | (ip[2] << 16)) >> 16;
			ip+=2;
			++sp; SetInt(sp, ival);
			dispatch_opcode;
		}
		case 46 :
		handle_op_46: {
			int ival = (int32)((ip[1] << 24) | (ip[2] << 16) | (ip[3] << 8)) >> 8;
			ip+=3;
			++sp; SetInt(sp, ival);
			dispatch_opcode;
		}
		case 47 :
		handle_op_47: {
			int ival = (int32)((ip[1] << 24) | (ip[2] << 16) | (ip[3] << 8) | ip[4]);
			ip+=4;
			++sp; SetInt(sp, ival);
			dispatch_opcode;
		}

		// opPushTempZeroVar
		case 48 : handle_op_48: slotCopy(++sp, &g->frame->vars[ 0]); dispatch_opcode;
		case 49 : handle_op_49: slotCopy(++sp, &g->frame->vars[ 1]); dispatch_opcode;
		case 50 : handle_op_50: slotCopy(++sp, &g->frame->vars[ 2]); dispatch_opcode;
		case 51 : handle_op_51: slotCopy(++sp, &g->frame->vars[ 3]); dispatch_opcode;
		case 52 : handle_op_52: slotCopy(++sp, &g->frame->vars[ 4]); dispatch_opcode;
		case 53 : handle_op_53: slotCopy(++sp, &g->frame->vars[ 5]); dispatch_opcode;
		case 54 : handle_op_54: slotCopy(++sp, &g->frame->vars[ 6]); dispatch_opcode;
		case 55 : handle_op_55: slotCopy(++sp, &g->frame->vars[ 7]); dispatch_opcode;
		case 56 : handle_op_56: slotCopy(++sp, &g->frame->vars[ 8]); dispatch_opcode;
		case 57 : handle_op_57: slotCopy(++sp, &g->frame->vars[ 9]); dispatch_opcode;
		case 58 : handle_op_58: slotCopy(++sp, &g->frame->vars[10]); dispatch_opcode;
		case 59 : handle_op_59: slotCopy(++sp, &g->frame->vars[11]); dispatch_opcode;
		case 60 : handle_op_60: slotCopy(++sp, &g->frame->vars[12]); dispatch_opcode;
		case 61 : handle_op_61: slotCopy(++sp, &g->frame->vars[13]); dispatch_opcode;
		case 62 : handle_op_62: slotCopy(++sp, &g->frame->vars[14]); dispatch_opcode;
		case 63 : handle_op_63: slotCopy(++sp, &g->frame->vars[15]); dispatch_opcode;

		// case opPushLiteral
		case 64 : handle_op_64: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 0]); dispatch_opcode;
		case 65 : handle_op_65: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 1]); dispatch_opcode;
		case 66 : handle_op_66: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 2]); dispatch_opcode;
		case 67 : handle_op_67: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 3]); dispatch_opcode;
		case 68 : handle_op_68: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 4]); dispatch_opcode;
		case 69 : handle_op_69: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 5]); dispatch_opcode;
		case 70 : handle_op_70: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 6]); dispatch_opcode;
		case 71 : handle_op_71: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 7]); dispatch_opcode;
		case 72 : handle_op_72: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 8]); dispatch_opcode;
		case 73 : handle_op_73: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 9]); dispatch_opcode;
		case 74 : handle_op_74: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[10]); dispatch_opcode;
		case 75 : handle_op_75: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[11]); dispatch_opcode;
		case 76 : handle_op_76: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[12]); dispatch_opcode;
		case 77 : handle_op_77: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[13]); dispatch_opcode;
		case 78 : handle_op_78: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[14]); dispatch_opcode;
		case 79 : handle_op_79: slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[15]); dispatch_opcode;

		// opPushClassVar
		case 80: handle_op_80:
			handlePushClassVar(g, sp, ip, 0); dispatch_opcode;
		case 81: handle_op_81:
			handlePushClassVar(g, sp, ip, 1); dispatch_opcode;
		case 82: handle_op_82:
			handlePushClassVar(g, sp, ip, 2); dispatch_opcode;
		case 83: handle_op_83:
			handlePushClassVar(g, sp, ip, 3); dispatch_opcode;
		case 84: handle_op_84:
			handlePushClassVar(g, sp, ip, 4); dispatch_opcode;
		case 85: handle_op_85:
			handlePushClassVar(g, sp, ip, 5); dispatch_opcode;
		case 86: handle_op_86:
			handlePushClassVar(g, sp, ip, 6); dispatch_opcode;
		case 87: handle_op_87:
			handlePushClassVar(g, sp, ip, 7); dispatch_opcode;
		case 88: handle_op_88:
			handlePushClassVar(g, sp, ip, 8); dispatch_opcode;
		case 89: handle_op_89:
			handlePushClassVar(g, sp, ip, 9); dispatch_opcode;
		case 90: handle_op_90:
			handlePushClassVar(g, sp, ip, 10); dispatch_opcode;
		case 91: handle_op_91:
			handlePushClassVar(g, sp, ip, 11); dispatch_opcode;
		case 92: handle_op_92:
			handlePushClassVar(g, sp, ip, 12); dispatch_opcode;
		case 93: handle_op_93:
			handlePushClassVar(g, sp, ip, 13); dispatch_opcode;
		case 94: handle_op_94:
			handlePushClassVar(g, sp, ip, 14); dispatch_opcode;
		case 95: handle_op_95:
			handlePushClassVar(g, sp, ip, 15); dispatch_opcode;

		// opPushSpecialValue
		case  96 : handle_op_96: slotCopy(++sp, &g->receiver); dispatch_opcode;
		case  97 : // push one and subtract
		handle_op_97:
			if (IsInt(sp)) {
				SetRaw(sp, slotRawInt(sp) - 1);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else {
				slotCopy(++sp, &gSpecialValues[svOne]);
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opSub;
				prSubNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			dispatch_opcode;
		case  98 : handle_op_98: slotCopy(++sp, &gSpecialValues[svNegOne]); dispatch_opcode;
		case  99 : handle_op_99: slotCopy(++sp, &gSpecialValues[svZero]); dispatch_opcode;
		case 100 : handle_op_100: slotCopy(++sp, &gSpecialValues[svOne]); dispatch_opcode;
		case 101 : handle_op_101: slotCopy(++sp, &gSpecialValues[svTwo]); dispatch_opcode;
		case 102 : handle_op_102: slotCopy(++sp, &gSpecialValues[svFHalf]); dispatch_opcode;
		case 103 : handle_op_103: slotCopy(++sp, &gSpecialValues[svFNegOne]); dispatch_opcode;
		case 104 : handle_op_104: slotCopy(++sp, &gSpecialValues[svFZero]); dispatch_opcode;
		case 105 : handle_op_105: slotCopy(++sp, &gSpecialValues[svFOne]); dispatch_opcode;
		case 106 : handle_op_106: slotCopy(++sp, &gSpecialValues[svFTwo]); dispatch_opcode;
		case 107 : // push one and add
		handle_op_107:
			if (IsInt(sp)) {
				SetRaw(sp, slotRawInt(sp) + 1);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else {
				slotCopy(++sp, &gSpecialValues[svOne]);
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opAdd;
				prAddNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			dispatch_opcode;
		case 108 : handle_op_108: slotCopy(++sp, &gSpecialValues[svTrue]); dispatch_opcode;
		case 109 : handle_op_109: slotCopy(++sp, &gSpecialValues[svFalse]); dispatch_opcode;
		case 110 : handle_op_110: slotCopy(++sp, &gSpecialValues[svNil]); dispatch_opcode;
		case 111 : handle_op_111: slotCopy(++sp, &gSpecialValues[svInf]); dispatch_opcode;

		// opStoreInstVar, 0..15
		case 112 : handle_op_112:
			handleStoreInstVar(g, sp, ip, 0); dispatch_opcode;
		case 113 : handle_op_113:
			handleStoreInstVar(g, sp, ip, 1); dispatch_opcode;
		case 114 : handle_op_114:
			handleStoreInstVar(g, sp, ip, 2); dispatch_opcode;
		case 115 : handle_op_115:
			handleStoreInstVar(g, sp, ip, 3); dispatch_opcode;
		case 116 : handle_op_116:
			handleStoreInstVar(g, sp, ip, 4); dispatch_opcode;
		case 117 : handle_op_117:
			handleStoreInstVar(g, sp, ip, 5); dispatch_opcode;
		case 118 : handle_op_118:
			handleStoreInstVar(g, sp, ip, 6); dispatch_opcode;
		case 119 : handle_op_119:
			handleStoreInstVar(g, sp, ip, 7); dispatch_opcode;
		case 120 : handle_op_120:
			handleStoreInstVar(g, sp, ip, 8); dispatch_opcode;
		case 121 : handle_op_121:
			handleStoreInstVar(g, sp, ip, 9); dispatch_opcode;
		case 122 : handle_op_122:
			handleStoreInstVar(g, sp, ip, 10); dispatch_opcode;
		case 123 : handle_op_123:
			handleStoreInstVar(g, sp, ip, 11); dispatch_opcode;
		case 124 : handle_op_124:
			handleStoreInstVar(g, sp, ip, 12); dispatch_opcode;
		case 125 : handle_op_125:
			handleStoreInstVar(g, sp, ip, 13); dispatch_opcode;
		case 126 : handle_op_126:
			handleStoreInstVar(g, sp, ip, 14); dispatch_opcode;
		case 127 : handle_op_127:
			handleStoreInstVar(g, sp, ip, 15); dispatch_opcode;

		// opStoreTempVar
		case 128 :
		handle_op_128: {
			int op3 = ip[1]; ++ip;  // get temp var index
			PyrFrame * tframe = g->frame; // zero level
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}

		case 129 :
		handle_op_129: {
			int op3 = ip[1]; ++ip;  // get temp var index
			PyrFrame * tframe = slotRawFrame(&g->frame->context); // one level
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}

		case 130 :
		handle_op_130: {
			int op3 = ip[1]; ++ip;  // get temp var index
			PyrFrame * tframe = slotRawFrame(&slotRawFrame(&g->frame->context)->context); // two levels
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}

		case 131 :
		handle_op_131: {
			int op3 = ip[1]; ++ip;  // get temp var index
			PyrFrame * tframe = slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context); // three levels
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}

		case 132 :
		handle_op_132: {
			int op3 = ip[1]; ++ip;  // get temp var index
			PyrFrame * tframe = slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->context); // four levels
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}

		case 133 : case 134 : case 135 :
		handle_op_133:
		handle_op_134:
		handle_op_135: {
			int op2 = op1 & 15;
			int op3 = ip[1]; ++ip; // get temp var index
			PyrFrame * tframe = g->frame;
			for (;op2--; tframe = slotRawFrame(&tframe->context)) { /* noop */ }
			slot = tframe->vars + op3;
			slotCopy(slot, sp);
			g->gc->GCWrite(tframe, slot);
			dispatch_opcode;
		}

		case 136 :  // push inst var, send special selector
		handle_op_136: {
			int op2 = ip[1]; // get inst var index
			int op3 = ip[2]; // get selector
			ip+=2;

			slotCopy(++sp, &slotRawObject(&g->receiver)->slots[op2]);

			numArgsPushed = 1;
			selector = gSpecialSelectors[op3];
			slot = sp;

			goto class_lookup;
		}

		case 137 :  // push all args, send msg
		handle_op_137: {
			numArgsPushed = METHRAW(g->block)->numargs;
			PyrSlot * pslot = g->frame->vars - 1;
			for (int m=0; m<numArgsPushed; ++m) *++sp = *++pslot;

			int op2 = ip[1]; ++ip; // get selector index
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}
		case 138 :  // push all but first arg, send msg
		handle_op_138: {
			numArgsPushed = METHRAW(g->block)->numargs;
			PyrSlot * pslot = g->frame->vars;
			for (int m=0;  m<numArgsPushed-1; ++m) *++sp = *++pslot;

			int op2 = ip[1]; ++ip; // get selector index
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		case 139 :  // push all args, send special
		handle_op_139: {
			numArgsPushed = METHRAW(g->block)->numargs;
			PyrSlot * pslot = g->frame->vars - 1;
			for (int m=0 ; m<numArgsPushed; ++m) *++sp = *++pslot;

			int op2 = ip[1]; ++ip; // get selector
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		case 140 :  // push all but first arg, send special
		handle_op_140: {
			numArgsPushed = METHRAW(g->block)->numargs;
			PyrSlot * pslot = g->frame->vars;
			for (int m=0; m<numArgsPushed-1; ++m) *++sp = *++pslot;

			int op2 = ip[1]; ++ip; // get selector
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		case 141 :  // one arg pushed, push all but first arg, send msg
		handle_op_141: {
			numArgsPushed = METHRAW(g->block)->numargs + 1;
			PyrSlot * pslot = g->frame->vars;
			for (int m=0; m<numArgsPushed-2; ++m) *++sp = *++pslot;

			int op2 = ip[1]; ++ip; // get selector index
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		case 142 :  // one arg pushed, push all but first arg, send special
		handle_op_142: {
			numArgsPushed = METHRAW(g->block)->numargs + 1;
			PyrSlot * pslot = g->frame->vars;
			for (int m=0; m<numArgsPushed-2; ++m) *++sp = *++pslot;

			int op2 = ip[1]; ++ip; // get selector
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		case 143 : // loop byte codes
		handle_op_143:{
			// this is major cheating to speed up often used looping methods
			// these byte codes are specific to their method and should only be used there.
			int op2 = ip[1]; ++ip; // get which one
			switch (op2) {
				// Integer-do : 143 0, 143 1
				case 0 : {
					PyrSlot *vars = g->frame->vars;
					if (slotRawInt(&vars[2]) < slotRawInt(&g->receiver)) {
						slotCopy(++sp, &vars[1]); // push function
						slotCopy(++sp, &vars[2]); // push i
						slotCopy(++sp, &vars[2]); // push i
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}
				case 1 :
					-- sp ; // Drop
					SetRaw(&g->frame->vars[2], slotRawInt(&g->frame->vars[2]) + 1); // inc i
					ip -= 4;
					dispatch_opcode;

				// Integer-reverseDo : 143 2, 143 3, 143 4
				case 2 :
					SetRaw(&g->frame->vars[2], slotRawInt(&g->receiver) - 1);
					dispatch_opcode;
				case 3 : {
					PyrSlot *vars = g->frame->vars;
					if (slotRawInt(&vars[2]) >= 0) {
						slotCopy(++sp, &vars[1]); // push function
						slotCopy(++sp, &vars[2]); // push i
						slotCopy(++sp, &vars[3]); // push j
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}
				case 4 : {
					-- sp ; // Drop
					PyrSlot *vars = g->frame->vars;
					SetRaw(&vars[2], slotRawInt(&vars[2]) - 1); // dec i
					SetRaw(&vars[3], slotRawInt(&vars[3]) + 1); // inc j
					ip -= 4;
					dispatch_opcode;
				}
				// Integer-for : 143 5, 143 6, 143 16
				case 5 : {
					PyrSlot * vars = g->frame->vars;
					int tag = GetTag(&vars[1]);

					if (tag != tagInt) {
						if (IsFloat(&vars[1])) {
							// coerce to int
							SetInt(&vars[1], (int32)(slotRawFloat(&vars[1])));
						} else {
							error("Integer-for : endval not a SimpleNumber.\n");

							slotCopy(++sp, &g->receiver);
							numArgsPushed = 1;
							selector = gSpecialSelectors[opmPrimitiveFailed];
							slot = sp;

							goto class_lookup;
						}
					}

					if (slotRawInt(&g->receiver) <= slotRawInt(&vars[1])) {
						SetRaw(&vars[5], 1);
					} else {
						SetRaw(&vars[5], -1);
					}
					slotCopy(&vars[3], &g->receiver);

					dispatch_opcode;
				}
				case 6 : {
					PyrSlot * vars = g->frame->vars;
					if ((slotRawInt(&vars[5]) > 0 && slotRawInt(&vars[3]) <= slotRawInt(&vars[1]))
							|| (slotRawInt(&vars[5]) < 0 && slotRawInt(&vars[3]) >= slotRawInt(&vars[1])))
					{
						slotCopy(++sp, &vars[2]); // push function
						slotCopy(++sp, &vars[3]); // push i
						slotCopy(++sp, &vars[4]); // push j
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}

				// Integer-forBy : 143 7, 143 8, 143 9
				case 7 : {
					PyrSlot * vars = g->frame->vars;
					if (IsFloat(vars+1)) {
						SetInt(&vars[1], (int32)(slotRawFloat(&vars[1])));
					}
					if (IsFloat(vars+2)) {
						SetInt(&vars[2], (int32)(slotRawFloat(&vars[2])));
					}
					int tag = GetTag(&vars[1]);
					if ((tag != tagInt)
							|| NotInt(&vars[2])) {
						error("Integer-forBy : endval or stepval not an Integer.\n");

						slotCopy(++sp, &g->receiver);
						numArgsPushed = 1;
						selector = gSpecialSelectors[opmPrimitiveFailed];
						slot = sp;

						goto class_lookup;
					}
					slotCopy(&vars[4], &g->receiver);
					dispatch_opcode;
				}
				case 8 : {
					PyrSlot * vars = g->frame->vars;
					if ((slotRawInt(&vars[2]) >= 0 && slotRawInt(&vars[4]) <= slotRawInt(&vars[1]))
							|| (slotRawInt(&vars[2]) < 0 && slotRawInt(&vars[4]) >= slotRawInt(&vars[1]))) {
						slotCopy(++sp, &vars[3]); // push function
						slotCopy(++sp, &vars[4]); // push i
						slotCopy(++sp, &vars[5]); // push j
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}
				case 9 : {
					--sp ; // Drop
					PyrSlot * vars = g->frame->vars;
					SetRaw(&vars[4], slotRawInt(&vars[4]) + slotRawInt(&vars[2])); // inc i
					SetRaw(&vars[5], slotRawInt(&vars[5]) + 1); // inc j
					ip -= 4;
					dispatch_opcode;
				}
				// ArrayedCollection-do : 143 10, 143 1
				case 10 : {
					// 0 this, 1 func, 2 i
					PyrSlot * vars = g->frame->vars;

					if (slotRawInt(&vars[2]) < slotRawObject(&g->receiver)->size) {
						slotCopy(++sp, &vars[1]); // push function
						getIndexedSlot(slotRawObject(&g->receiver), ++sp, slotRawInt(&vars[2])); // push this.at(i)
						slotCopy(++sp, &vars[2]); // push i
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}

				// ArrayedCollection-reverseDo : 143 11, 143 12, 143 4
				case 11 :
					SetRaw(&g->frame->vars[2], slotRawObject(&g->receiver)->size - 1);
					dispatch_opcode;
				case 12 : {
					PyrSlot * vars = g->frame->vars;
					if (slotRawInt(&vars[2]) >= 0) {
						slotCopy(++sp, &vars[1]); // push function
						getIndexedSlot(slotRawObject(&g->receiver), ++sp, slotRawInt(&vars[2])); // push this.at(i)
						slotCopy(++sp, &vars[3]); // push j
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup; // class_lookup:
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}

				// Dictionary-keysValuesArrayDo
				case 13 : {
					PyrSlot * vars = g->frame->vars;
					
					if (IsNil(&vars[1])) {
						error("Dictionary-keysValuesArrayDo: first argument should not be nil.\n");
						
						slotCopy(++sp, &g->receiver);
						numArgsPushed = 0;
						selector = gSpecialSelectors[opmPrimitiveFailed];
						slot = sp;
						
						goto class_lookup;
					}

					int m = slotRawInt(&vars[3]);
					PyrObject * obj = slotRawObject(&vars[1]);
					
					
					
					if ( m < obj->size ) {
						slot = obj->slots + m;	// key
						while (IsNil(slot)) {
							m += 2;
							if ( m >= obj->size ) {
								SetRaw(&vars[3], m);
								goto keysValuesArrayDo_return;
							}
							slot = obj->slots + m;	// key
						}
						SetRaw(&vars[3], m);
						slotCopy(++sp, &vars[2]); // function
						slotCopy(++sp, &slot[0]); // key
						slotCopy(++sp, &slot[1]); // val
						slotCopy(++sp, &vars[4]); // j
						SetRaw(&vars[4], slotRawInt(&vars[4]) + 1);

						// SendSpecialMsg value
						numArgsPushed = 4;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 3;

						goto class_lookup; // class_lookup:
					} else {
						keysValuesArrayDo_return:
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}
				case 14 :
					-- sp; // Drop
					SetRaw(&g->frame->vars[3], slotRawInt(&g->frame->vars[3]) + 2); // inc i
					ip -= 4;
					dispatch_opcode;
				case 15 :
					// unused opcode.
					break;

				case 16 : {
					-- sp ; // Drop
					PyrSlot * vars = g->frame->vars;
					SetRaw(&vars[3], slotRawInt(&vars[3]) + slotRawInt(&vars[5])); // inc i by stepval
					SetRaw(&vars[4], slotRawInt(&vars[4]) + 1); // inc j
					ip -= 4;
					dispatch_opcode;
				}

				// Float-do : 143 17, 143 18
				case 17 : {
					PyrSlot * vars = g->frame->vars;
					if (slotRawFloat(&vars[2]) + 0.5 < slotRawFloat(&g->receiver)) {
						slotCopy(++sp, &vars[1]); // push function
						slotCopy(++sp, &vars[2]); // push i
						slotCopy(++sp, &vars[2]); // push i
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}
				case 18 : {
					-- sp ; // Drop
					SetRaw(&g->frame->vars[2], slotRawFloat(&g->frame->vars[2]) + 1.0); // inc i
					ip -= 4;
					dispatch_opcode;
				}

				// Float-reverseDo : 143 19, 143 20, 143 21
				case 19 : {
					SetFloat(&g->frame->vars[2], slotRawFloat(&g->receiver) - 1.0);
					dispatch_opcode;
				}
				case 20 : {
					PyrSlot * vars = g->frame->vars;
					if (slotRawFloat(&vars[2]) + 0.5 >= 0.0) {
						slotCopy(++sp, &vars[1]); // push function
						slotCopy(++sp, &vars[2]); // push i
						slotCopy(++sp, &vars[3]); // push j
						// SendSpecialMsg value
						numArgsPushed = 3;
						selector = gSpecialSelectors[opmValue];
						slot = sp - 2;

						goto class_lookup;
					} else {
						slotCopy(++sp, &g->receiver);
						g->sp = sp; g->ip = ip;
						returnFromMethod(g);
						sp = g->sp; ip = g->ip;
					}
					dispatch_opcode;
				}
				case 21 : {
					-- sp ; // Drop
					PyrSlot * vars = g->frame->vars;
					SetRaw(&vars[2], slotRawFloat(&vars[2]) - 1.0); // dec i
					SetRaw(&vars[3], slotRawFloat(&vars[3]) - 1.0); // inc j
					ip -= 4;
					dispatch_opcode;
				}
				case 22 : // ? question mark method
					--sp;
					if (IsNil(sp)) {
						*sp = *(sp+1);
					}
					dispatch_opcode;
				case 23 : // if not nil push this and jump. used to implement ??
					if (NotNil(sp)) {
						int jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						--sp;
						ip+=2;
					}
					dispatch_opcode;
				case 24 : // ifNil
					if ( NotNil(sp) ) {
						int jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						ip+=2;
					}
					--sp;
					dispatch_opcode;
				case 25 : // ifNotNil
					if ( IsNil(sp) ) {
						int jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						ip+=2;
					}
					--sp;
					dispatch_opcode;
				case 26 : // ifNotNilPushNil
					if ( NotNil(sp) ) {
						int jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
						slotCopy(sp, &gSpecialValues[svNil]);
					} else {
						ip+=2;
						--sp;
					}
					dispatch_opcode;
				case 27 : // ifNilPushNil
					if ( IsNil(sp) ) {
						int jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						ip+=2;
						--sp;
					}
					dispatch_opcode;
				case 28 :{ // switch
					PyrObject * obj = slotRawObject(sp);
					op2 = 1 + arrayAtIdentityHashInPairs(obj, (sp-1));
					sp-=2;
					ip += slotRawInt(&obj->slots[op2]);
					dispatch_opcode;
				}
				// Number-forSeries : 143 29, 143 30, 143 31
				case 29 : {
					PyrSlot * vars = g->frame->vars;
					// 0 receiver, 1 step, 2 last, 3 function, 4 i, 5 j
					if (IsInt(vars+0) && (IsInt(vars+1) || IsNil(vars+1)) && (IsInt(vars+2) || IsNil(vars+2))) {
						if (IsNil(vars+1)) {
							if (IsNil(vars+2)) SetInt(vars+2, 0x7FFFFFFF);
							if (slotRawInt(&vars[0]) < slotRawInt(&vars[2])) SetInt(vars+1, 1);
							else SetInt(vars+1, -1);
						} else {
							if (IsNil(vars+2)) {
								if (slotRawInt(&vars[1]) < slotRawInt(&vars[0])) SetInt(vars+2, 0x80000000);
								else SetInt(vars+2, 0x7FFFFFFF);
							}
							SetInt(vars+1, slotRawInt(&vars[1]) - slotRawInt(&vars[0]));
						}
						slotCopy(&vars[4], &vars[0]);
					} else {
						if (IsInt(vars+0)) {
							SetFloat(&vars[4], slotRawInt(&vars[0]));
						} else if (IsFloat(vars+0)) {
							SetFloat(&vars[4], slotRawFloat(&vars[0]));
						} else {
							bailFromNumberSeries:
							error("Number-forSeries : first, second or last not an Integer or Float.\n");

							slotCopy(++sp, &g->receiver);
							numArgsPushed = 1;
							selector = gSpecialSelectors[opmPrimitiveFailed];
							slot = sp;

							goto class_lookup;
						}

						if (IsNil(vars+1)) {
							if (IsNil(vars+2)) SetFloat(vars+2, kBigBigFloat);
							else if (IsInt(vars+2)) SetFloat(&vars[2], slotRawInt(&vars[2]));
							else if (!IsFloat(vars+2)) goto bailFromNumberSeries;

							if (slotRawFloat(&vars[4]) < slotRawFloat(&vars[2])) SetFloat(vars+1, 1.);
							else SetFloat(vars+1, -1.);
						} else {
							if (IsInt(vars+1)) SetFloat(&vars[1], slotRawInt(&vars[1]));
							else if (!IsFloat(vars+1)) goto bailFromNumberSeries;

							if (IsNil(vars+2)) {
								if (slotRawFloat(&vars[1]) < slotRawFloat(&vars[4])) SetFloat(vars+2, kSmallSmallFloat);
								else SetFloat(vars+2, kBigBigFloat);
							}
							else if (IsInt(vars+2)) SetFloat(&vars[2], slotRawInt(&vars[2]));
							else if (!IsFloat(vars+2)) goto bailFromNumberSeries;
							SetFloat(vars+1, slotRawFloat(&vars[1]) - slotRawFloat(&vars[4]));
						}
					}
					dispatch_opcode;
				}
				case 30 : {
					PyrSlot * vars = g->frame->vars;
					int tag = GetTag(&vars[1]);
					if (tag == tagInt) {
						if ((slotRawInt(&vars[1]) >= 0 && slotRawInt(&vars[4]) <= slotRawInt(&vars[2]))
								|| (slotRawInt(&vars[1]) < 0 && slotRawInt(&vars[4]) >= slotRawInt(&vars[2]))) {
							slotCopy(++sp, &vars[3]); // push function
							slotCopy(++sp, &vars[4]); // push i
							slotCopy(++sp, &vars[5]); // push j
							// SendSpecialMsg value
							numArgsPushed = 3;
							selector = gSpecialSelectors[opmValue];
							slot = sp - 2;

							goto class_lookup;
						} else {
							slotCopy(++sp, &g->receiver);
							g->sp = sp; g->ip = ip;
							returnFromMethod(g);
							sp = g->sp; ip = g->ip;
						}
					} else {
						if ((slotRawFloat(&vars[1]) >= 0. && slotRawFloat(&vars[4]) <= slotRawFloat(&vars[2]))
								|| (slotRawFloat(&vars[1]) < 0. && slotRawFloat(&vars[4]) >= slotRawFloat(&vars[2]))) {
							slotCopy(++sp, &vars[3]); // push function
							slotCopy(++sp, &vars[4]); // push i
							slotCopy(++sp, &vars[5]); // push j
							// SendSpecialMsg value
							numArgsPushed = 3;
							selector = gSpecialSelectors[opmValue];
							slot = sp - 2;

							goto class_lookup;
						} else {
							slotCopy(++sp, &g->receiver);
							g->sp = sp; g->ip = ip;
							returnFromMethod(g);
							sp = g->sp; ip = g->ip;
						}
					}
					dispatch_opcode;
				}
				case 31 : {
					-- sp ; // Drop
					PyrSlot * vars = g->frame->vars;

					int tag = GetTag(&vars[1]);
					if (tag == tagInt) {
						SetRaw(&vars[4], slotRawInt(&vars[4]) + slotRawInt(&vars[1])); // inc i
					} else {
						SetRaw(&vars[4], slotRawFloat(&vars[4]) + slotRawFloat(&vars[1])); // inc i
					}
					SetRaw(&vars[5], slotRawInt(&vars[5]) + 1); // inc j
					ip -= 4;
					dispatch_opcode;
				}
			}
			dispatch_opcode;
		}


		// opStoreClassVar
		case 144 :  case 145 :  case 146 :  case 147 :
		case 148 :  case 149 :  case 150 :  case 151 :
		case 152 :  case 153 :  case 154 :  case 155 :
		case 156 :  case 157 :  case 158 :  case 159 :
		handle_op_144: handle_op_145: handle_op_146: handle_op_147:
		handle_op_148: handle_op_149: handle_op_150: handle_op_151:
		handle_op_152: handle_op_153: handle_op_154: handle_op_155:
		handle_op_156: handle_op_157: handle_op_158: handle_op_159: {

			int op2 = op1 & 15;
			int op3 = ip[1]; ++ip; // get class var index
			slotCopy(&g->classvars->slots[(op2<<8)|op3], sp--);
			g->gc->GCWrite(g->classvars, (sp+1));
			dispatch_opcode;
		}

		// opSendMsg
		case 160 :
		handle_op_160: {
			// special case for this as only arg
			int op2 = ip[1]; ++ip; // get selector index
			slotCopy(++sp, &g->receiver);
			numArgsPushed = 1;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp;

			goto class_lookup;
		}

		case 161 :  case 162 :  case 163 :
		case 164 :  case 165 :  case 166 :  case 167 :
		case 168 :  case 169 :  case 170 :  case 171 :
		case 172 :  case 173 :  case 174 :  case 175 :
		handle_op_161: handle_op_162: handle_op_163:
		handle_op_164: handle_op_165: handle_op_166: handle_op_167:
		handle_op_168: handle_op_169: handle_op_170: handle_op_171:
		handle_op_172: handle_op_173: handle_op_174: handle_op_175: {

			int op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = op1 & 15;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		case 176 : // opcTailCallReturnFromFunction
		handle_op_176:
#if TAILCALLOPTIMIZE
			g->tailCall = 2;
#endif
			dispatch_opcode;
		// opSuperMsg
		case 177 :
		handle_op_177: {
			// special case for this as only arg
			int op2 = ip[1]; ++ip; // get selector index
			slotCopy(++sp, &g->receiver);
			numArgsPushed = 1;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp;
			classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;

			goto msg_lookup;
		}

		case 178 :  case 179 :
		case 180 :  case 181 :  case 182 :  case 183 :
		case 184 :  case 185 :  case 186 :  case 187 :
		case 188 :  case 189 :  case 190 :  case 191 :

		handle_op_178: handle_op_179:
		handle_op_180: handle_op_181: handle_op_182: handle_op_183:
		handle_op_184: handle_op_185: handle_op_186: handle_op_187:
		handle_op_188: handle_op_189: handle_op_190: handle_op_191: {

			int op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = op1 & 15;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;
			classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;

			goto msg_lookup;
		}

		// opSendSpecialMsg
		case 192 :
		handle_op_192: {
			slotCopy(++sp, &g->receiver);
			int op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = 1;
			selector = gSpecialSelectors[op2];
			slot = sp;

			goto class_lookup;
		}

		case 193 :  case 194 :  case 195 :
		case 196 :  case 197 :  case 198 :  case 199 :
		case 200 :  case 201 :  case 202 :  case 203 :
		case 204 :  case 205 :  case 206 :  case 207 :

		handle_op_193: handle_op_194: handle_op_195:
		handle_op_196: handle_op_197: handle_op_198: handle_op_199:
		handle_op_200: handle_op_201: handle_op_202: handle_op_203:
		handle_op_204: handle_op_205: handle_op_206: handle_op_207: {

			int op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = op1 & 15;
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;
		}

		// opSendSpecialUnaryArithMsg
		case 208 :  // opNeg
		handle_op_208:
		if (IsFloat(sp)) {
				SetFloat(sp, -slotRawFloat(sp));
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else if (IsInt(sp)) {
				SetRaw(sp, -slotRawInt(sp));
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else
				handleSendSpecialUnaryArithMsg(g, sp, ip, op1);
			dispatch_opcode;
		case 209 : // opNot
		handle_op_209:
			if (IsTrue(sp)) {
				SetTagRaw(sp, tagFalse);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else if (IsFalse(sp)) {
				SetTagRaw(sp, tagTrue);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else
				handleSendSpecialUnaryArithMsg(g, sp, ip, op1);
			dispatch_opcode;
		case 210 : // opIsNil
		handle_op_210:
			if (IsNil(sp)) {
				SetTagRaw(sp, tagTrue);
			} else {
				slotCopy(sp, &gSpecialValues[svFalse]);
			}
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			dispatch_opcode;
		case 211 : // opNotNil
		handle_op_211:
			if (NotNil(sp)) {
				slotCopy(sp, &gSpecialValues[svTrue]);
			} else {
				SetTagRaw(sp, tagFalse);
			}
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			dispatch_opcode;

		case 212 : handle_op_212:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 213 : handle_op_213:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 214 : handle_op_214:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 215 : handle_op_215:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 216 : handle_op_216:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 217 : handle_op_217:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 218 : handle_op_218:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 219 : handle_op_219:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 220 : handle_op_220:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 221 : handle_op_221:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 222 : handle_op_222:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 223 : handle_op_223:
			handleSendSpecialUnaryArithMsg(g, sp, ip, op1); dispatch_opcode;


		// opSendSpecialBinaryArithMsg
		case 224 : // add
		handle_op_224:
			if (IsInt(sp - 1)) {
				if (IsInt(sp)) {
					--sp; SetRaw(sp, slotRawInt(sp) + slotRawInt(sp + 1));
#if TAILCALLOPTIMIZE
					g->tailCall = 0;
#endif
				} else {
					g->sp = sp; g->ip = ip;
					g->primitiveIndex = opAdd;
					prAddInt(g, -1);
					sp = g->sp; ip = g->ip;
				}
			} else {
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opAdd;
				prAddNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			dispatch_opcode;
		case 225 : // subtract
		handle_op_225:
			if (IsInt(sp - 1)) {
				if (IsInt(sp)) {
					--sp; SetRaw(sp, slotRawInt(sp) - slotRawInt(sp + 1));
#if TAILCALLOPTIMIZE
					g->tailCall = 0;
#endif
				} else {
					g->sp = sp; g->ip = ip;
					g->primitiveIndex = opSub;
					prSubInt(g, -1);
					sp = g->sp; ip = g->ip;
				}
			} else {
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opSub;
				prSubNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			dispatch_opcode;
		case 226 :  // multiply
		handle_op_226:
			if (IsInt(sp - 1)) {
				if (IsInt(sp)) {
					--sp; SetRaw(sp, slotRawInt(sp) * slotRawInt(sp + 1));
#if TAILCALLOPTIMIZE
					g->tailCall = 0;
#endif
				} else {
					g->sp = sp; g->ip = ip;
					g->primitiveIndex = opMul;
					prMulInt(g, -1);
					sp = g->sp; ip = g->ip;
				}
			} else {
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opMul;
				prMulNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			dispatch_opcode;

		case 227 : handle_op_227:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 228 : handle_op_228:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 229 : handle_op_229:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 230 : handle_op_230:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 231 : handle_op_231:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 232 : handle_op_232:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 233 : handle_op_233:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 234 : handle_op_234:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 235 : handle_op_235:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 236 : handle_op_236:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 237 : handle_op_237:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 238 : handle_op_238:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;
		case 239 : handle_op_239:
			handleSendSpecialBinaryArithMsg(g, sp, ip, op1); dispatch_opcode;

		// opSpecialOpcodes
		case 240 :
		handle_op_240:
			--sp; dispatch_opcode; // opDrop
		case 241 :
		handle_op_241:
			++sp; *sp = sp[-1]; dispatch_opcode;	// opDup
		case 242 : // opcFunctionReturn
		handle_op_242:
			g->sp = sp; g->ip = ip;
			returnFromBlock(g);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		case 243 : // opcReturn
		handle_op_243:
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		case 244 : // opcReturnSelf
		handle_op_244:
			slotCopy(++sp, &g->receiver);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		case 245 : // opcReturnTrue
		handle_op_245:
			slotCopy(++sp, &gSpecialValues[svTrue]);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		case 246 : // opcReturnFalse
		handle_op_246:
			slotCopy(++sp, &gSpecialValues[svFalse]);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		case 247 : // opcReturnNil
		handle_op_247:
			slotCopy(++sp, &gSpecialValues[svNil]);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;

		case 248 : // opcJumpIfFalse
		handle_op_248:
			// cannot compare with o_false because it is NaN
			if ( IsFalse(sp) ) {
				int jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
			} else if ( IsTrue(sp)) {
				ip+=2;
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			--sp;
			dispatch_opcode;
		case 249 : // opcJumpIfFalsePushNil
		handle_op_249:
			if ( IsFalse(sp)) {
				int jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
				slotCopy(sp, &gSpecialValues[svNil]);
			} else if ( IsTrue(sp)) {
				--sp;
				ip+=2;
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			dispatch_opcode;
		case 250 : // opcJumpIfFalsePushFalse
		handle_op_250:
			if (IsFalse(sp)) {
				int jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
				//*sp = r_false;
			} else if (IsTrue(sp)) {
				--sp;
				ip+=2;
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			dispatch_opcode;
		case 251 : // opcJumpIfTruePushTrue
		handle_op_251:
			if (IsFalse(sp)) {
				--sp;
				ip+=2;
			} else if (IsTrue(sp)) {
				int jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
				slotCopy(sp, &gSpecialValues[svTrue]);
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			dispatch_opcode;
		case 252 : // opcJumpFwd
		handle_op_252: {
			int jmplen = (ip[1]<<8) | ip[2];
			ip += jmplen + 2;
			dispatch_opcode;
		}
		case 253 : // opcJumpBak
		handle_op_253: {
			--sp; // also drops the stack. This saves an opcode in the while loop
					// which is the only place this opcode is used.
			int jmplen = (ip[1]<<8) | ip[2];
			ip -= jmplen;

			//assert(g->gc->SanityCheck());
			dispatch_opcode;
		}
		case 254 : // opcSpecialBinaryOpWithAdverb
		handle_op_254: {
			int op2 = ip[1]; ++ip; // get selector index
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op2;
			doSpecialBinaryArithMsg(g, 3, false);
			sp = g->sp; ip = g->ip;
			dispatch_opcode;
		}
		case 255 : // opcTailCallReturnFromMethod
		handle_op_255:
#if TAILCALLOPTIMIZE
			g->tailCall = 1;
#endif
			dispatch_opcode;

			////////////////////////////////////

		class_lookup:
			// normal class lookup
			classobj = classOfSlot(slot);

			// message sends handled here:
		msg_lookup: {
			size_t index = slotRawInt(&classobj->classIndex) + selector->u.index;
			PyrMethod *meth = NULL;
			if( UNLIKELY( ( selector->flags & sym_Class ) != 0 ) )
			  {
			    // You have sent a message which is a class name. This is a bad thing.
			    // There are two cases. It is either an illegitimate classname like
			    // 1 FooBaz: 2
			    // in which case selector->u.index == 0 and you get a message or it is a real one like
			    // 1 Object: 2
			    // in which case selector->u.index isn't pointing to a method and you get a segfault. So...
			    meth = NULL;
			  }
			else
			  {
			    meth = gRowTable[index];
			  }

			// and now if meth is null, bail out just like if I don't understand it
			if (UNLIKELY(meth == NULL || ( slotRawSymbol(&meth->name) != selector))) {
				g->sp = sp; g->ip = ip;
				doesNotUnderstand(g, selector, numArgsPushed);
				sp = g->sp; ip = g->ip;
			} else {
				PyrMethodRaw *methraw = METHRAW(meth);
				switch (methraw->methType) {
					case methNormal : /* normal msg send */
						g->sp = sp; g->ip = ip;
						executeMethod(g, meth, numArgsPushed);
						sp = g->sp; ip = g->ip;
						break;
					case methReturnSelf : /* return self */
						sp -= numArgsPushed - 1;
						break;
					case methReturnLiteral : /* return literal */
						sp -= numArgsPushed - 1;
						slotCopy(sp, &meth->selectors); /* in this case selectors is just a single value */
						break;
					case methReturnArg : /* return an argument */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex; // zero is index of the first argument
						if (index < numArgsPushed) {
							slotCopy(sp, sp + index);
						} else {
							slotCopy(sp, &slotRawObject(&meth->prototypeFrame)->slots[index]);
						}
						break;
					case methReturnInstVar : /* return inst var */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex;
						slotCopy(sp, &slotRawObject(slot)->slots[index]);
						break;
					case methAssignInstVar : { /* assign inst var */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex;
						PyrObject * obj = slotRawObject(slot);
						if (obj->IsImmutable()) { StoreToImmutableB(g, sp, ip); }
						else {
							if (numArgsPushed >= 2) {
								slotCopy(&obj->slots[index], sp + 1);
								g->gc->GCWrite(obj, sp + 1);
							} else {
								slotCopy(&obj->slots[index], &gSpecialValues[svNil]);
							}
							slotCopy(sp, slot);
						}
						break;
					}
					case methReturnClassVar : /* return class var */
						sp -= numArgsPushed - 1;
						slotCopy(sp, &g->classvars->slots[methraw->specialIndex]);
						break;
					case methAssignClassVar : /* assign class var */
						sp -= numArgsPushed - 1;
						if (numArgsPushed >= 2) {
							slotCopy(&g->classvars->slots[methraw->specialIndex], sp + 1);
							g->gc->GCWrite(g->classvars, sp + 1);
						} else
							slotCopy(&g->classvars->slots[methraw->specialIndex], &gSpecialValues[svNil]);
						slotCopy(sp, slot);
						break;
					case methRedirect : /* send a different selector to self */
						if (numArgsPushed < methraw->numargs) { // not enough args pushed
							/* push default arg values */
							PyrSlot *qslot;
							int m, mmax;
							qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
							for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) slotCopy(++sp, ++qslot);
							numArgsPushed = methraw->numargs;
						}
						selector = slotRawSymbol(&meth->selectors);
						goto msg_lookup;
					case methRedirectSuper : /* send a different selector to self */
						if (numArgsPushed < methraw->numargs) { // not enough args pushed
							/* push default arg values */
							PyrSlot *qslot;
							int m, mmax;
							qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
							for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) slotCopy(++sp, ++qslot);
							numArgsPushed = methraw->numargs;
						}
						selector = slotRawSymbol(&meth->selectors);
						classobj = slotRawSymbol(&slotRawClass(&meth->ownerclass)->superclass)->u.classobj;
						goto msg_lookup;
					case methForwardInstVar : /* forward to an instance variable */
						if (numArgsPushed < methraw->numargs) { // not enough args pushed
							/* push default arg values */
							PyrSlot *qslot;
							int m, mmax;
							qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
							for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) slotCopy(++sp, ++qslot);
							numArgsPushed = methraw->numargs;
						}
						selector = slotRawSymbol(&meth->selectors);
						index = methraw->specialIndex;
						slotCopy(slot, &slotRawObject(slot)->slots[index]);

						classobj = classOfSlot(slot);

						goto msg_lookup;
					case methForwardClassVar : /* forward to an instance variable */
						if (numArgsPushed < methraw->numargs) { // not enough args pushed
							/* push default arg values */
							PyrSlot *qslot;
							int m, mmax;
							qslot = slotRawObject(&meth->prototypeFrame)->slots + numArgsPushed - 1;
							for (m=0, mmax=methraw->numargs - numArgsPushed; m<mmax; ++m) slotCopy(++sp, ++qslot);
							numArgsPushed = methraw->numargs;
						}
						selector = slotRawSymbol(&meth->selectors);
						slotCopy(slot, &g->classvars->slots[methraw->specialIndex]);

						classobj = classOfSlot(slot);

						goto msg_lookup;
					case methPrimitive : /* primitive */
						g->sp = sp; g->ip = ip;
						doPrimitive(g, meth, numArgsPushed);
						sp = g->sp; ip = g->ip;
						break;
				} // switch (meth->methType)
			} // end handle message
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			dispatch_opcode;

			}
			////////////////////////////////////

		key_class_lookup:
			// normal class lookup
			classobj = classOfSlot(slot);

			// message sends handled here:
		key_msg_lookup: {
			size_t index = slotRawInt(&classobj->classIndex) + selector->u.index;
			PyrMethod *meth = gRowTable[index];

			if (UNLIKELY(slotRawSymbol(&meth->name) != selector)) {
				g->sp = sp; g->ip = ip;
				doesNotUnderstandWithKeys(g, selector, numArgsPushed, numKeyArgsPushed);
				sp = g->sp; ip = g->ip;
			} else {
				PyrMethodRaw *methraw;
				methraw = METHRAW(meth);
				switch (methraw->methType) {
					case methNormal : /* normal msg send */
						g->sp = sp; g->ip = ip;
						executeMethodWithKeys(g, meth, numArgsPushed, numKeyArgsPushed);
						sp = g->sp; ip = g->ip;
						break;
					case methReturnSelf : /* return self */
						sp -= numArgsPushed - 1;
						break;
					case methReturnLiteral : /* return literal */
						sp -= numArgsPushed - 1;
						slotCopy(sp, &meth->selectors); /* in this case selectors is just a single value */
						break;
					case methReturnArg : /* return an argument */
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
					case methReturnInstVar : /* return inst var */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex;
						slotCopy(sp, &slotRawObject(slot)->slots[index]);
						break;
					case methAssignInstVar : { /* assign inst var */
						sp -= numArgsPushed - 1;
						numArgsPushed -= numKeyArgsPushed << 1;
						index = methraw->specialIndex;
						PyrObject * obj = slotRawObject(slot);
						if (obj->IsImmutable()) { StoreToImmutableB(g, sp, ip); }
						else {
							if (numArgsPushed >= 2) {
								slotCopy(&obj->slots[index], sp + 1);
								g->gc->GCWrite(obj, sp + 1);
							} else
								slotCopy(&obj->slots[index], &gSpecialValues[svNil]);
							slotCopy(sp, slot);
						}
						break;
					}
					case methReturnClassVar : /* return class var */
						sp -= numArgsPushed - 1;
						slotCopy(sp, &g->classvars->slots[methraw->specialIndex]);
						break;
					case methAssignClassVar : /* assign class var */
						sp -= numArgsPushed - 1;
						if (numArgsPushed >= 2) {
							slotCopy(&g->classvars->slots[methraw->specialIndex], sp + 1);
							g->gc->GCWrite(g->classvars, sp + 1);
						} else
							slotCopy(&g->classvars->slots[methraw->specialIndex], &gSpecialValues[svNil]);
						slotCopy(sp, slot);
						break;
					case methRedirect : /* send a different selector to self, e.g. this.subclassResponsibility */
						g->sp = sp;
						numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
						numKeyArgsPushed = 0;
						sp = g->sp;
						selector = slotRawSymbol(&meth->selectors);

						goto msg_lookup;
					case methRedirectSuper : /* send a different selector to super */
						g->sp = sp;
						numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
						numKeyArgsPushed = 0;
						sp = g->sp;
						selector = slotRawSymbol(&meth->selectors);

						classobj = slotRawSymbol(&slotRawClass(&meth->ownerclass)->superclass)->u.classobj;

						goto msg_lookup;
					case methForwardInstVar : /* forward to an instance variable */
						g->sp = sp;
						numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
						numKeyArgsPushed = 0;
						sp = g->sp;
						selector = slotRawSymbol(&meth->selectors);
						index = methraw->specialIndex;
						slotCopy(slot, &slotRawObject(slot)->slots[index]);

						classobj = classOfSlot(slot);

						goto msg_lookup;
					case methForwardClassVar : /* forward to an instance variable */
						g->sp = sp;
						numArgsPushed = keywordFixStack(g, meth, methraw, numArgsPushed, numKeyArgsPushed);
						numKeyArgsPushed = 0;
						sp = g->sp;
						selector = slotRawSymbol(&meth->selectors);
						slotCopy(slot, &g->classvars->slots[methraw->specialIndex]);

						classobj = classOfSlot(slot);

						goto msg_lookup;
					case methPrimitive : /* primitive */
						g->sp = sp; g->ip = ip;
						doPrimitiveWithKeys(g, meth, numArgsPushed, numKeyArgsPushed);
						sp = g->sp; ip = g->ip;
						break;
				} // switch (meth->methType)
			} // end handle message
			numKeyArgsPushed = 0;
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			dispatch_opcode;
 			}
	} // switch(op1)
	} // end while(running)
#ifndef _WIN32
	running = true; // reset the signal
#endif
	g->sp = sp; g->ip = ip;
}


#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC pop_options
#endif


void DumpSimpleBackTrace(VMGlobals *g);
void DumpSimpleBackTrace(VMGlobals *g)
{
	int i;
	PyrFrame *frame;

	post("CALL STACK:\n");
	// print the variables and arguments for the most recent frames in the
	// call graph
	frame = g->frame;

	for (i=0; i<16; ++i) {
		char str[256];
		slotOneWord(&frame->method, str);
		post("%s ip %d\n", str, (char*)slotRawPtr(&frame->ip) - (char*)slotRawObject(&slotRawMethod(&frame->method)->code)->slots);
		frame = slotRawFrame(&frame->caller);
		if (!frame) break;
	}
	if (frame) { post("...\n"); }
	//DumpStack(g, g->sp);
}

void DumpBackTrace(VMGlobals *g)
{
	int i;
	PyrFrame *frame;

	post("CALL STACK:\n");
	// print the variables and arguments for the most recent frames in the
	// call graph
	frame = g->frame;

	for (i=0; i<16; ++i) {
		if (FrameSanity(frame, "DumpBackTrace")) {
			post("FRAME CORRUPTED\n");
			return;
		}
		DumpFrame(frame);
		frame = slotRawFrame(&frame->caller);
		if (!frame) break;
	}
	if (frame) { post("...\n"); }
	//DumpStack(g, g->sp);
}

void DumpDetailedFrame(PyrFrame *frame);
void DumpDetailedBackTrace(VMGlobals *g);
void DumpDetailedBackTrace(VMGlobals *g)
{
	int i;
	PyrFrame *frame;

	post("CALL STACK:\n");
	// print the variables and arguments for the most recent frames in the
	// call graph
	frame = g->frame;

	for (i=0; i<16; ++i) {
		if (FrameSanity(frame, "DumpDetailedBackTrace")) {
			post("FRAME CORRUPTED\n");
			return;
		}
		DumpDetailedFrame(frame);
		frame = slotRawFrame(&frame->caller);
		if (!frame) break;
	}
	if (frame) { post("...\n"); }
	//DumpStack(g, g->sp);
}

void DumpStack(VMGlobals *g, PyrSlot *sp)
{
	PyrSlot *slot;
	char str[128];
#if BCSTAT
	dumpbcstat();
#endif
	postfl("STACK:\n");
	slot = sp - 64;
	if (slot < g->gc->Stack()->slots) slot = g->gc->Stack()->slots;
	for (size_t i=slot - g->gc->Stack()->slots; slot<=sp; slot++, ++i) {
		slotString(slot, str);
		post("   %2d  %s\n", i, str);
	}
}



