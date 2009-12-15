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

#ifdef SC_WIN32
# include "SC_Win32Utils.h"
#else
# include <sys/time.h>
#endif

#include <float.h>
#define kBigBigFloat DBL_MAX
#define kSmallSmallFloat DBL_MIN


#include <new>
#include "InitAlloc.h"
#include "bullet.h"

//void tellPlugInsAboutToRun();
double timeNow();


int32 timeseed();
int32 timeseed()
{
	struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec ^ tv.tv_usec;
}

VMGlobals gVMGlobals;
VMGlobals *gMainVMGlobals = &gVMGlobals;

extern PyrObject *gSynth;

void debugf(char *fmt, ...) ;

#define SANITYCHECK 0
#define DEBUGINTERPRETER 0
#define METHODMETER 0
#define BCSTAT 0
#define CHECK_MAX_STACK_USE 0

#if SANITYCHECK
#define SANITYCHECKLITE 1
#else
#define SANITYCHECKLITE 0
#endif

#if CHECK_MAX_STACK_USE
int gMaxStackDepth = 0;
#endif

unsigned char* dumpOneByteCode(PyrBlock *theBlock, PyrClass* theClass, unsigned char *ip);
void dumpSlotOneWord(const char *tagstr, PyrSlot *slot);
//bool checkAllObjChecksum(PyrObject* obj);

bool gTraceInterpreter = false;
//bool gTraceInterpreter = true;


char* byteCodeString(int code);

extern int gNumClasses;
extern PyrClass *gClassList;

// runInterpreter has 7 call sites:
//	compileLibrary (4)
// 	runLibrary
// 	interpretCmdLine

void runInterpreter(VMGlobals *g, PyrSymbol *selector, int numArgsPushed)
{
		//postfl("->runInterpreter\n");
#if SANITYCHECKLITE
	g->gc->SanityCheck();
#endif
		//postfl(" >initInterpreter\n");

	if (initInterpreter(g, selector, numArgsPushed)) {
#if SANITYCHECKLITE
	g->gc->SanityCheck();
#endif
//        if (strcmp(selector->name, "tick") != 0) post("%s %d  execMethod %d\n", selector->name, numArgsPushed, g->execMethod);
	//post("->Interpret thread %08X\n", g->thread);
		if (g->execMethod) Interpret(g);
	//post("<-Interpret thread %08X\n", g->thread);
#if SANITYCHECKLITE
	g->gc->SanityCheck();
#endif

        }
        //postfl(" >endInterpreter\n");
        endInterpreter(g);
#if SANITYCHECKLITE
	g->gc->SanityCheck();
#endif
		//postfl("<-runInterpreter\n");
	//dumpGCStats(g->gc);

}

bool initAwakeMessage(VMGlobals *g);

void runAwakeMessage(VMGlobals *g)
{
	if (initAwakeMessage(g)) {
		if (g->execMethod) Interpret(g);
	}
	endInterpreter(g);
}

void initPyrThread(VMGlobals *g, PyrThread *thread, PyrSlot *func, int stacksize, PyrInt32Array* rgenArray,
	double beats, double seconds, PyrSlot* clock, bool collect);
int32 timeseed();

PyrProcess* newPyrProcess(VMGlobals *g, PyrClass *procclassobj)
{
	PyrProcess *proc;
	PyrInterpreter *interpreter;
	PyrClass *classobj;
	PyrObject *classVars;
	PyrThread *thread;
	//PyrDSP *dsp;
        PyrGC* gc = g->gc;

	int numClassVars;

	proc = (PyrProcess*)instantiateObject(gc, procclassobj, 0, true, false);

	PyrObject *sysSchedulerQueue = newPyrArray(gc, 1024, 0, false);
	SetObject(&proc->sysSchedulerQueue, sysSchedulerQueue);

	classVars = newPyrArray(gc, gNumClassVars, 0, false);
	classVars->size = gNumClassVars;
	nilSlots(classVars->slots, gNumClassVars);
	SetObject(&proc->classVars, classVars);
	g->classvars = classVars;

	// fill class vars from prototypes:
	classobj = gClassList;
	while (classobj) {
		if (slotRawObject(&classobj->cprototype)) {
			numClassVars = slotRawObject(&classobj->cprototype)->size;
			if (numClassVars > 0) {
				memcpy(g->classvars->slots + slotRawInt(&classobj->classVarIndex), slotRawObject(&classobj->cprototype)->slots, numClassVars * sizeof(PyrSlot));
			}
		}
		classobj = slotRawClass(&classobj->nextclass);
	}

	class_thread = getsym("Thread")->u.classobj;
	if (class_thread) {
		SetNil(&proc->curThread);
		thread = (PyrThread*)instantiateObject(gc, class_thread, 0, true, false);
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

	PyrSymbol *contextsym;
	int index;
	PyrMethod *meth;

	contextsym = getsym("functionCompileContext");
	index = slotRawInt(&class_interpreter->classIndex) + contextsym->u.index;
	meth = gRowTable[index];
	if (!meth || slotRawSymbol(&meth->name) != contextsym) {
		error("compile context method 'functionCompileContext' not found.\n");
		//SetNil(&proc->interpreter);
	} else {
		PyrObject *proto;
		PyrFrame *frame;
		PyrMethodRaw *methraw;

		interpreter = (PyrInterpreter*)instantiateObject(gc, class_interpreter, 0, true, false);
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

#ifndef SC_WIN32
bool running = true;
void handleSigUsr1(int param)
{
  printf("handleSigUsr1()...\n");
  running = false;
}
#endif

bool initRuntime(VMGlobals *g, int poolSize, AllocPool *inPool)
{
	PyrClass *class_main;
	/*
		create a GC environment
		create a vmachine instance of main
		initialize VMGlobals
	*/

	class_main = s_main->u.classobj;

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

	//initUGenFuncs();
	signal_init_globs();
	initThreads();
	initPatterns();
	initUniqueMethods();
	initGUI();
#if SANITYCHECKLITE
	g->gc->SanityCheck();
#endif
	//tellPlugInsAboutToRun();

#ifndef SC_WIN32
	signal(SIGUSR1,handleSigUsr1);
#endif

	assert((g->gc->SanityCheck()));
#if SANITYCHECKLITE
	g->gc->SanityCheck();
#endif

	return true;
}

bool initAwakeMessage(VMGlobals *g)
{
	//post("initAwakeMessage %08X %08X\n", g->thread, slotRawThread(&g->process->mainThread));
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

	g->thread->beats.uf = slot[1].uf;
	g->thread->seconds.uf = slot[2].uf;
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
	g->thread->beats.uf = g->thread->seconds.uf = elapsedTime();
	SetObject(&g->thread->clock, s_systemclock->u.classobj);
	g->gc->GCWrite(g->thread, s_systemclock->u.classobj);

	// set process as the receiver
	PyrSlot *slot = g->sp - numArgsPushed + 1;
	slotCopy(&g->receiver, slot);

	// start it
	sendMessage(g, selector, numArgsPushed);

	return g->method != NULL;
}


void endInterpreter(VMGlobals *g)
{
	slotCopy(&g->result, g->sp);
//	dumpObjectSlot(&g->result);
	g->gc->Stack()->size = 0;
	g->sp = g->gc->Stack()->slots - 1;
}


void StoreToImmutableA(VMGlobals *g, PyrSlot *& sp, unsigned char *& ip);
void StoreToImmutableA(VMGlobals *g, PyrSlot *& sp, unsigned char *& ip)
{
	// only the value is on the stack
	slotCopy(&sp[1], &sp[0]); // copy value up one
	slotCopy(&sp[0], &g->receiver); // put receiver in place
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

void StoreToImmutableB(VMGlobals *g, PyrSlot *& sp, unsigned char *& ip);
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

void Interpret(VMGlobals *g)
{
	// byte code values
	unsigned char *ip;
	unsigned char op1;
	int op2, op3, index, tag;
	// interpreter globals

	// temporary variables used in the interpreter
	int ival, jmplen, numArgsPushed, numKeyArgsPushed;
	PyrFrame *tframe;
	PyrSymbol *selector;
	PyrClass *classobj;
	PyrSlot *slot, *vars;
	PyrSlot *sp, *pslot;
	PyrObject *obj;
	PyrClosure *closure;
	PyrMethod *meth;
	int m,mmax;

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
#ifndef SC_WIN32
	while (running) {  // not going to indent body to save line space
#else
	while (true) {
#endif

#if CHECK_MAX_STACK_USE
	int stackDepth = sp - g->sp + 1;
	if (stackDepth > gMaxStackDepth) {
		gMaxStackDepth = stackDepth;
		printf("gMaxStackDepth %d\n", gMaxStackDepth);
	}
#endif

#if BCSTAT
	prevop = op1;
#endif

	op1 = ip[1];
	++ip;
#if BCSTAT
	++bcstat[op1];
	++bcpair[prevop][op1];
	prevop = op1;
#endif
	//printf("op1 %d\n", op1);
	//postfl("sp %08X   frame %08X  caller %08X  ip %08X\n", sp, g->frame, g->frame->caller.uof, slotRawInt(&g->frame->caller.uof->ip));
	//postfl("sp %08X   frame %08X   diff %d    caller %08X\n", sp, g->frame, ((int)sp - (int)g->frame)>>3, g->frame->caller.uof);
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
			g->method->ownerclass.uoc->name.us->name, g->slotRawSymbol(&method->name)->name);
		dumpOneByteCode(g->block, NULL, ip);
	}
#endif
#if SANITYCHECK
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
			op2 = ip[1]; ++ip; // get literal index
			classobj = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2])->u.classobj;
			if (classobj) {
				++sp; SetObject(sp, classobj);
			} else {
				postfl("Execution warning: Class '%s' not found\n", slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2])->name);
				slotCopy(++sp, (PyrSlot*)&gSpecialValues[svNil]);
			}
			break;
		case 1 : // opExtended, opPushInstVar
			op2 = ip[1]; ++ip; // get inst var index
			slotCopy(++sp, &slotRawObject(&g->receiver)->slots[op2]);
			break;
		case 2 : // opExtended, opPushTempVar
			op2 = ip[1]; // get temp var level
			op3 = ip[2]; // get temp var index
			ip += 2;
			for (tframe = g->frame; --op2; tframe = slotRawFrame(&tframe->context)) { /* noop */ }
			slotCopy(++sp, &tframe->vars[op3]);
			break;
		case 3 : // opExtended, opPushTempZeroVar
			op2 = ip[1]; ++ip; // get temp var index
			slotCopy(++sp, &g->frame->vars[op2]);
			break;
		case 4 : // opExtended, opPushLiteral
			op2 = ip[1]; ++ip; // get literal index
			// push a block as a closure if it is one
			slot = slotRawObject(&g->block->selectors)->slots + op2;
			if (IsObj(slot) && slotRawObject(slot)->classptr == gSpecialClasses[op_class_fundef]->u.classobj) {
				// push a closure
				g->sp = sp; // gc may push the stack
				closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, true);
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
			break;
		case 5 : // opExtended, opPushClassVar
			op2 = ip[1]; // get class
			op3 = ip[2]; // get class var index
			ip += 2;
			slotCopy(++sp, &g->classvars->slots[(op2<<8)|op3]);
			break;
		case 6 :  // opExtended, opPushSpecialValue == push a special class
			op2 = ip[1]; ++ip; // get class name index
			classobj = gSpecialClasses[op2]->u.classobj;
			if (classobj) {
				++sp; SetObject(sp, classobj);
			} else {
				slotCopy(++sp, (PyrSlot*)&gSpecialValues[svNil]);
			}
			break;
		case 7 : // opExtended, opStoreInstVar
			op2 = ip[1]; ++ip; // get inst var index
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + op2;
				slotCopy(slot, sp);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 8 : // opExtended, opStoreTempVar
			op2 = ip[1]; // get temp var level
			op3 = ip[2]; // get temp var index
			ip += 2;
			for (tframe = g->frame; op2--; tframe = slotRawFrame(&tframe->context)) { /* noop */ }
			slot = tframe->vars + op3;
			slotCopy(slot, sp);
			g->gc->GCWrite(tframe, slot);
			break;
		case 9 : // opExtended, opStoreClassVar
			op2 = ip[1]; // get index of class name literal
			op3 = ip[2]; // get class var index
			ip += 2;
			slotCopy(&g->classvars->slots[(op2<<8)|op3], sp);
			g->gc->GCWrite(g->classvars, sp);
			break;
		case 10 : // opExtended, opSendMsg
			numArgsPushed = ip[1]; // get num args
			numKeyArgsPushed = ip[2]; // get num keyword args
			op3 = ip[3]; // get selector index
			ip += 3;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op3]);

			slot = sp - numArgsPushed + 1;

			if (numKeyArgsPushed) goto key_class_lookup;
			else goto class_lookup;

		case 11 : // opExtended, opSendSuper
			numArgsPushed = ip[1]; // get num args
			numKeyArgsPushed = ip[2]; // get num keyword args
			op3 = ip[3]; // get selector index
			ip += 3;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op3]);

			slot = g->sp - numArgsPushed + 1;
			classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;

			if (numKeyArgsPushed) goto key_msg_lookup;
			else goto msg_lookup;

		case 12 :  // opExtended, opSendSpecialMsg
			numArgsPushed = ip[1]; // get num args
			numKeyArgsPushed = ip[2]; // get num keyword args
			op3 = ip[3]; // get selector index
			ip += 3;

			selector = gSpecialSelectors[op3];
			slot = sp - numArgsPushed + 1;

			if (numKeyArgsPushed) goto key_class_lookup;
			else goto class_lookup;

		case 13 :  // opExtended, opSendSpecialUnaryArithMsg
			op2 = ip[1]; ++ip; // get selector index
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op2;
			doSpecialUnaryArithMsg(g, -1);
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			sp = g->sp; ip = g->ip;
			break;
		case 14 :  // opExtended, opSendSpecialBinaryArithMsg
			op2 = ip[1]; ++ip; // get selector index
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op2;
			doSpecialBinaryArithMsg(g, 2, false);
			sp = g->sp; ip = g->ip;
			break;
		case 15 : // opExtended, opSpecialOpcode (none yet)
			op2 = ip[1]; ++ip; // get extended special opcode
			switch (op2) {
				case opgProcess : // push thisProcess
					++sp; SetObject(sp, g->process); break;
				case opgThread : // push thisProcess
					++sp; SetObject(sp, g->thread); break;
				case opgMethod : // push thisMethod
					++sp; SetObject(sp, g->method); break;
				case opgFunctionDef : // push thisFunctionDef
					++sp; SetObject(sp, g->block); break;
				case opgFunction : // push thisFunc
					// push a closure
					g->sp = sp; // gc may push the stack
					closure = (PyrClosure*)g->gc->New(2*sizeof(PyrSlot), 0, obj_notindexed, true);
					sp = g->sp;
					closure->classptr = gSpecialClasses[op_class_func]->u.classobj;
					closure->size = 2;
					SetObject(&closure->block, g->block);
					SetObject(&closure->context, slotRawFrame(&g->frame->context));
					++sp; SetObject(sp, closure);
					break;
				default :
					slotCopy(++sp, (PyrSlot*)&gSpecialValues[svNil]); break;
			}
			break;
		// opPushInstVar, 0..15
		case 16 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 0]); break;
		case 17 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 1]); break;
		case 18 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 2]); break;
		case 19 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 3]); break;
		case 20 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 4]); break;
		case 21 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 5]); break;
		case 22 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 6]); break;
		case 23 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 7]); break;
		case 24 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 8]); break;
		case 25 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[ 9]); break;
		case 26 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[10]); break;
		case 27 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[11]); break;
		case 28 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[12]); break;
		case 29 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[13]); break;
		case 30 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[14]); break;
		case 31 : slotCopy(++sp, &slotRawObject(&g->receiver)->slots[15]); break;

		case 32 : // JumpIfTrue
			// cannot compare with o_false because it is NaN
			if ( IsTrue(sp) ) {
				jmplen = (ip[1]<<8) | ip[2];
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
			break;

		// opPushTempVar, levels 1..7
		case 33 : slotCopy(++sp, &slotRawFrame(&g->frame->context)->vars[ip[1]]); ++ip; break;
		case 34 : slotCopy(++sp, &slotRawFrame(&slotRawFrame(&g->frame->context)->context)->vars[ip[1]]); ++ip; break;
		case 35 : slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->vars[ip[1]]); ++ip; break;
		case 36 : slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->
					context)->vars[ip[1]]); ++ip; break;
		case 37 : slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->
					context)->context)->vars[ip[1]]); ++ip; break;
		case 38 : slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->
					context)->context)->context)->vars[ip[1]]); ++ip; break;
		case 39 : slotCopy(++sp, &slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->
					context)->context)->context)->context)->vars[ip[1]]); ++ip; break;

		// push literal constants.
		case 40 :
			ival = ip[1];
			ip+=1;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			break;
		case 41 :
			ival = (ip[1] << 8) | ip[2];
			ip+=2;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			break;
		case 42 :
			ival = (ip[1] << 16) | (ip[2] << 8) | ip[3];
			ip+=3;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			break;
		case 43 :
			ival = (ip[1] << 24) | (ip[2] << 16) | (ip[3] << 8) | ip[4];
			ip+=4;
			slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ival]);
			break;

		// push integers.
		case 44 :
			ival = (int32)(ip[1] << 24) >> 24;
			ip+=1;
			++sp; SetInt(sp, ival);
			break;
		case 45 :
			ival = (int32)((ip[1] << 24) | (ip[2] << 16)) >> 16;
			ip+=2;
			++sp; SetInt(sp, ival);
			break;
		case 46 :
			ival = (int32)((ip[1] << 24) | (ip[2] << 16) | (ip[3] << 8)) >> 8;
			ip+=3;
			++sp; SetInt(sp, ival);
			break;
		case 47 :
			ival = (int32)((ip[1] << 24) | (ip[2] << 16) | (ip[3] << 8) | ip[4]);
			ip+=4;
			++sp; SetInt(sp, ival);
			break;


		// opPushTempZeroVar
		case 48 : slotCopy(++sp, &g->frame->vars[ 0]); break;
		case 49 : slotCopy(++sp, &g->frame->vars[ 1]); break;
		case 50 : slotCopy(++sp, &g->frame->vars[ 2]); break;
		case 51 : slotCopy(++sp, &g->frame->vars[ 3]); break;
		case 52 : slotCopy(++sp, &g->frame->vars[ 4]); break;
		case 53 : slotCopy(++sp, &g->frame->vars[ 5]); break;
		case 54 : slotCopy(++sp, &g->frame->vars[ 6]); break;
		case 55 : slotCopy(++sp, &g->frame->vars[ 7]); break;
		case 56 : slotCopy(++sp, &g->frame->vars[ 8]); break;
		case 57 : slotCopy(++sp, &g->frame->vars[ 9]); break;
		case 58 : slotCopy(++sp, &g->frame->vars[10]); break;
		case 59 : slotCopy(++sp, &g->frame->vars[11]); break;
		case 60 : slotCopy(++sp, &g->frame->vars[12]); break;
		case 61 : slotCopy(++sp, &g->frame->vars[13]); break;
		case 62 : slotCopy(++sp, &g->frame->vars[14]); break;
		case 63 : slotCopy(++sp, &g->frame->vars[15]); break;

		// case opPushLiteral
		case 64 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 0]); break;
		case 65 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 1]); break;
		case 66 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 2]); break;
		case 67 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 3]); break;
		case 68 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 4]); break;
		case 69 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 5]); break;
		case 70 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 6]); break;
		case 71 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 7]); break;
		case 72 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 8]); break;
		case 73 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[ 9]); break;
		case 74 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[10]); break;
		case 75 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[11]); break;
		case 76 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[12]); break;
		case 77 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[13]); break;
		case 78 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[14]); break;
		case 79 : slotCopy(++sp, &slotRawObject(&g->block->constants)->slots[15]); break;

		//	opPushClassVar
		case 80 :  case 81 :  case 82 :  case 83 :
		case 84 :  case 85 :  case 86 :  case 87 :
		case 88 :  case 89 :  case 90 :  case 91 :
		case 92 :  case 93 :  case 94 :  case 95 :
			op2 = op1 & 15;
			op3 = ip[1]; ++ip; // get class var index
			slotCopy(++sp, &g->classvars->slots[(op2<<8)|op3]);
			break;

		// opPushSpecialValue
		case  96 : slotCopy(++sp, &g->receiver); break;
		case  97 : // push one and subtract
			if (IsInt(sp)) {
				SetRaw(sp, slotRawInt(sp) - 1);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else {
				slotCopy(++sp, (PyrSlot*)&gSpecialValues[svOne]);
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opSub;
				prSubNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			break;
		case  98 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svNegOne]); break;
		case  99 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svZero]); break;
		case 100 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svOne]); break;
		case 101 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svTwo]); break;
		case 102 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFHalf]); break;
		case 103 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFNegOne]); break;
		case 104 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFZero]); break;
		case 105 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFOne]); break;
		case 106 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFTwo]); break;
		case 107 : // push one and add
			if (IsInt(sp)) {
				SetRaw(sp, slotRawInt(sp) + 1);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else {
				slotCopy(++sp, (PyrSlot*)&gSpecialValues[svOne]);
				g->sp = sp; g->ip = ip;
				g->primitiveIndex = opAdd;
				prAddNum(g, -1);
				sp = g->sp; ip = g->ip;
			}
			break;
		case 108 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svTrue]); break;
		case 109 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFalse]); break;
		case 110 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svNil]); break;
		case 111 : slotCopy(++sp, (PyrSlot*)&gSpecialValues[svInf]); break;

		// opStoreInstVar, 0..15
#if 1
		case 112 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 113 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 1;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 114 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 2;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 115 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 3;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 116 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 4;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 117 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 5;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 118 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 6;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 119 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 7;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 120 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 8;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 121 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 9;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 122 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 10;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 123 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 11;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 124 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 12;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 125 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 13;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 126 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 14;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
		case 127 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + 15;
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
#else
		case 112 :  case 113 :  case 114 :  case 115 :
		case 116 :  case 117 :  case 118 :  case 119 :
		case 120 :  case 121 :  case 122 :  case 123 :
		case 124 :  case 125 :  case 126 :  case 127 :
			obj = slotRawObject(&g->receiver);
			if (obj->obj_flags & obj_immutable) { StoreToImmutableA(g, (PyrSlot*&)sp, ip); }
			else {
				slot = obj->slots + (op1 & 15);
				slotCopy(slot, sp--);
				g->gc->GCWrite(obj, slot);
			}
			break;
#endif

		// opStoreTempVar
		case 128 :
			op3 = ip[1]; ++ip;  // get temp var index
			tframe = g->frame; // zero level
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			break;

		case 129 :
			op3 = ip[1]; ++ip;  // get temp var index
			tframe = slotRawFrame(&g->frame->context); // one level
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			break;

		case 130 :
			op3 = ip[1]; ++ip;  // get temp var index
			tframe = slotRawFrame(&slotRawFrame(&g->frame->context)->context); // two levels
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			break;

		case 131 :
			op3 = ip[1]; ++ip;  // get temp var index
			tframe = slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context); // three levels
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			break;

		case 132 :
			op3 = ip[1]; ++ip;  // get temp var index
			tframe = slotRawFrame(&slotRawFrame(&slotRawFrame(&slotRawFrame(&g->frame->context)->context)->context)->context); // four levels
			slot = tframe->vars + op3;
			slotCopy(slot, sp--);
			g->gc->GCWrite(tframe, slot);
			break;

		case 133 : case 134 : case 135 :
			op2 = op1 & 15;
			op3 = ip[1]; ++ip; // get temp var index
			for (tframe = g->frame; op2--; tframe = slotRawFrame(&tframe->context)) { /* noop */ }
			slot = tframe->vars + op3;
			slotCopy(slot, sp);
			g->gc->GCWrite(tframe, slot);
			break;

		case 136 :  // push inst var, send special selector
			op2 = ip[1]; // get inst var index
			op3 = ip[2]; // get selector
			ip+=2;

			slotCopy(++sp, &slotRawObject(&g->receiver)->slots[op2]);

			numArgsPushed = 1;
			selector = gSpecialSelectors[op3];
			slot = sp;

			goto class_lookup;

		case 137 :  // push all args, send msg
			numArgsPushed = METHRAW(g->block)->numargs;
			pslot = g->frame->vars - 1;
			for (m=0,mmax=numArgsPushed; m<mmax; ++m) *++sp = *++pslot;

			op2 = ip[1]; ++ip; // get selector index
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 138 :  // push all but first arg, send msg
			numArgsPushed = METHRAW(g->block)->numargs;
			pslot = g->frame->vars;
			for (m=0,mmax=numArgsPushed-1; m<mmax; ++m) *++sp = *++pslot;

			op2 = ip[1]; ++ip; // get selector index
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 139 :  // push all args, send special
			numArgsPushed = METHRAW(g->block)->numargs;
			pslot = g->frame->vars - 1;
			for (m=0,mmax=numArgsPushed; m<mmax; ++m) *++sp = *++pslot;

			op2 = ip[1]; ++ip; // get selector
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 140 :  // push all but first arg, send special
			numArgsPushed = METHRAW(g->block)->numargs;
			pslot = g->frame->vars;
			for (m=0,mmax=numArgsPushed-1; m<mmax; ++m) *++sp = *++pslot;

			op2 = ip[1]; ++ip; // get selector
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 141 :  // one arg pushed, push all but first arg, send msg
			numArgsPushed = METHRAW(g->block)->numargs + 1;
			pslot = g->frame->vars;
			for (m=0,mmax=numArgsPushed-2; m<mmax; ++m) *++sp = *++pslot;

			op2 = ip[1]; ++ip; // get selector index
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 142 :  // one arg pushed, push all but first arg, send special
			numArgsPushed = METHRAW(g->block)->numargs + 1;
			pslot = g->frame->vars;
			for (m=0,mmax=numArgsPushed-2; m<mmax; ++m) *++sp = *++pslot;

			op2 = ip[1]; ++ip; // get selector
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 143 : // loop byte codes
			// this is major cheating to speed up often used looping methods
			// these byte codes are specific to their method and should only be used there.
			op2 = ip[1]; ++ip; // get which one
			switch (op2) {
				// Integer-do : 143 0, 143 1
				case 0 :
					vars = g->frame->vars;
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
				break;
				case 1 :
					-- sp ; // Drop
					SetRaw(&g->frame->vars[2], slotRawInt(&g->frame->vars[2]) + 1); // inc i
					ip -= 4;
					break;

				// Integer-reverseDo : 143 2, 143 3, 143 4
				case 2 :
					SetRaw(&g->frame->vars[2], slotRawInt(&g->receiver) - 1);
					break;
				case 3 :
					vars = g->frame->vars;
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
					break;
				case 4 :
					-- sp ; // Drop
					vars = g->frame->vars;
					SetRaw(&vars[2], slotRawInt(&vars[2]) - 1); // dec i
					SetRaw(&vars[3], slotRawInt(&vars[3]) + 1); // inc j
					ip -= 4;
					break;

				// Integer-for : 143 5, 143 6, 143 16
				case 5 :
					vars = g->frame->vars;
					tag = GetTag(&vars[1]);

					if (tag != tagInt) {
						if (IsFloat(&vars[1])) {
							// coerce to int
							SetInt(&vars[1], (int32)(vars[1].uf));
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

					break;
				case 6 :
					vars = g->frame->vars;
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
					break;

				// Integer-forBy : 143 7, 143 8, 143 9
				case 7 :
					vars = g->frame->vars;
					if (IsFloat(vars+1)) {
						SetInt(&vars[1], (int32)(vars[1].uf));
					}
					if (IsFloat(vars+2)) {
						SetInt(&vars[2], (int32)(vars[2].uf));
					}
					tag = GetTag(&vars[1]);
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
					break;
				case 8 :
					vars = g->frame->vars;
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
					break;
				case 9 :
					--sp ; // Drop
					vars = g->frame->vars;
					SetRaw(&vars[4], slotRawInt(&vars[4]) + slotRawInt(&vars[2])); // inc i
					SetRaw(&vars[5], slotRawInt(&vars[5]) + 1); // inc j
					ip -= 4;
					break;

				// ArrayedCollection-do : 143 10, 143 1
				case 10 :
					// 0 this, 1 func, 2 i
					vars = g->frame->vars;

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
					break;

				// ArrayedCollection-reverseDo : 143 11, 143 12, 143 4
				case 11 :
					SetRaw(&g->frame->vars[2], slotRawObject(&g->receiver)->size - 1);
					break;
				case 12 :
					vars = g->frame->vars;
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
					break;

				// Dictionary-keysValuesArrayDo
				case 13 :
					vars = g->frame->vars;
					m = slotRawInt(&vars[3]);
					obj = slotRawObject(&vars[1]);
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
					break;
				case 14 :
					-- sp; // Drop
					SetRaw(&g->frame->vars[3], slotRawInt(&g->frame->vars[3]) + 2); // inc i
					ip -= 4;
					break;
				case 15 :
					// unused opcode.
					break;

				case 16 :
					-- sp ; // Drop
					vars = g->frame->vars;
					SetRaw(&vars[3], slotRawInt(&vars[3]) + slotRawInt(&vars[5])); // inc i by stepval
					SetRaw(&vars[4], slotRawInt(&vars[4]) + 1); // inc j
					ip -= 4;
					break;

				// Float-do : 143 17, 143 18
				case 17 :
					vars = g->frame->vars;
					if (vars[2].uf + 0.5 < g->receiver.uf) {
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
					break;
				case 18 :
					-- sp ; // Drop
					g->frame->vars[2].uf += 1.0; // inc i
					ip -= 4;
					break;

				// Float-reverseDo : 143 19, 143 20, 143 21
				case 19 :
					g->frame->vars[2].uf = g->receiver.uf - 1.0;
					break;
				case 20 :
					vars = g->frame->vars;
					if (vars[2].uf + 0.5 >= 0.0) {
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
					break;
				case 21 :
					-- sp ; // Drop
					vars = g->frame->vars;
					vars[2].uf -= 1.0; // dec i
					vars[3].uf += 1.0; // inc j
					ip -= 4;
					break;
				case 22 : // ? question mark method
					--sp;
					if (IsNil(sp)) {
						*sp = *(sp+1);
					}
					break;
				case 23 : // if not nil push this and jump. used to implement ??
					if (NotNil(sp)) {
						jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						--sp;
						ip+=2;
					}
					break;
				case 24 : // ifNil
					if ( NotNil(sp) ) {
						jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						ip+=2;
					}
					--sp;
					break;
				case 25 : // ifNotNil
					if ( IsNil(sp) ) {
						jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						ip+=2;
					}
					--sp;
					break;
				case 26 : // ifNotNilPushNil
					if ( NotNil(sp) ) {
						jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
						slotCopy(sp, (PyrSlot*)&gSpecialValues[svNil]);
					} else {
						ip+=2;
						--sp;
					}
					break;
				case 27 : // ifNilPushNil
					if ( IsNil(sp) ) {
						jmplen = (ip[1]<<8) | ip[2];
						ip += jmplen + 2;
					} else {
						ip+=2;
						--sp;
					}
					break;
				case 28 : // switch
					obj = slotRawObject(sp);
					op2 = 1 + arrayAtIdentityHashInPairs(obj, (sp-1));
					sp-=2;
					ip += slotRawInt(&obj->slots[op2]);
					break;

				// Number-forSeries : 143 29, 143 30, 143 31
				case 29 :
					vars = g->frame->vars;
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
							vars[4].uf = slotRawInt(&vars[0]);
						} else if (IsFloat(vars+0)) {
							vars[4].uf = vars[0].uf;
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
							else if (IsInt(vars+2)) vars[2].uf = slotRawInt(&vars[2]);
							else if (!IsFloat(vars+2)) goto bailFromNumberSeries;

							if (vars[4].uf < vars[2].uf) SetFloat(vars+1, 1.);
							else SetFloat(vars+1, -1.);
						} else {
							if (IsInt(vars+1)) vars[1].uf = slotRawInt(&vars[1]);
							else if (!IsFloat(vars+1)) goto bailFromNumberSeries;

							if (IsNil(vars+2)) {
								if (vars[1].uf < vars[4].uf) SetFloat(vars+2, kSmallSmallFloat);
								else SetFloat(vars+2, kBigBigFloat);
							}
							else if (IsInt(vars+2)) vars[2].uf = slotRawInt(&vars[2]);
							else if (!IsFloat(vars+2)) goto bailFromNumberSeries;
							SetFloat(vars+1, vars[1].uf - vars[4].uf);
						}
					}
					break;
				case 30 :
					vars = g->frame->vars;
					tag = GetTag(&vars[1]);
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
						if ((vars[1].uf >= 0. && vars[4].uf <= vars[2].uf)
								|| (vars[1].uf < 0. && vars[4].uf >= vars[2].uf)) {
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
					break;
				case 31 :
					-- sp ; // Drop
					vars = g->frame->vars;

					tag = GetTag(&vars[1]);
					if (tag == tagInt) {
						SetRaw(&vars[4], slotRawInt(&vars[4]) + slotRawInt(&vars[1])); // inc i
					} else {
						vars[4].uf += vars[1].uf; // inc i
					}
					SetRaw(&vars[5], slotRawInt(&vars[5]) + 1); // inc j
					ip -= 4;
					break;

			}
			break;


		//	opStoreClassVar
		case 144 :  case 145 :  case 146 :  case 147 :
		case 148 :  case 149 :  case 150 :  case 151 :
		case 152 :  case 153 :  case 154 :  case 155 :
		case 156 :  case 157 :  case 158 :  case 159 :
			op2 = op1 & 15;
			op3 = ip[1]; ++ip; // get class var index
			slotCopy(&g->classvars->slots[(op2<<8)|op3], sp--);
			g->gc->GCWrite(g->classvars, (sp+1));
			break;

		// opSendMsg
		case 160 :
			// special case for this as only arg
			op2 = ip[1]; ++ip; // get selector index
			slotCopy(++sp, &g->receiver);
			numArgsPushed = 1;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp;

			goto class_lookup;

		case 161 :  case 162 :  case 163 :
		case 164 :  case 165 :  case 166 :  case 167 :
		case 168 :  case 169 :  case 170 :  case 171 :
		case 172 :  case 173 :  case 174 :  case 175 :

			op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = op1 & 15;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		case 176 : // opcTailCallReturnFromFunction
#if TAILCALLOPTIMIZE
			g->tailCall = 2;
#endif
			break;
		// opSuperMsg
		case 177 :
			// special case for this as only arg
			op2 = ip[1]; ++ip; // get selector index
			slotCopy(++sp, &g->receiver);
			numArgsPushed = 1;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp;
			classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;

			goto msg_lookup;

		case 178 :  case 179 :
		case 180 :  case 181 :  case 182 :  case 183 :
		case 184 :  case 185 :  case 186 :  case 187 :
		case 188 :  case 189 :  case 190 :  case 191 :

			op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = op1 & 15;
			selector = slotRawSymbol(&slotRawObject(&g->block->selectors)->slots[op2]);
			slot = sp - numArgsPushed + 1;
			classobj = slotRawSymbol(&slotRawClass(&g->method->ownerclass)->superclass)->u.classobj;

			goto msg_lookup;

		// opSendSpecialMsg
		case 192 :

			slotCopy(++sp, &g->receiver);
			op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = 1;
			selector = gSpecialSelectors[op2];
			slot = sp;

			goto class_lookup;

		case 193 :  case 194 :  case 195 :
		case 196 :  case 197 :  case 198 :  case 199 :
		case 200 :  case 201 :  case 202 :  case 203 :
		case 204 :  case 205 :  case 206 :  case 207 :

			op2 = ip[1]; ++ip; // get selector index
			numArgsPushed = op1 & 15;
			selector = gSpecialSelectors[op2];
			slot = sp - numArgsPushed + 1;

			goto class_lookup;

		// opSendSpecialUnaryArithMsg
		case 208 :  // opNeg
			if (IsFloat(sp)) {
				SetFloat(sp, -sp->uf);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else if (IsInt(&sp[0])) {
				SetRaw(&sp[0], -slotRawInt(&sp[0]));
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else goto unary_send;
			break;
		case 209 : // opNot
			if (IsTrue(&sp[0])) {
				SetTagRaw(&sp[0], tagFalse);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else if (IsFalse(&sp[0])) {
				SetTagRaw(&sp[0], tagTrue);
#if TAILCALLOPTIMIZE
				g->tailCall = 0;
#endif
			} else goto unary_send;
			break;
		case 210 : // opIsNil
			if (IsNil(&sp[0])) {
				SetTagRaw(&sp[0], tagTrue);
			} else {
				slotCopy(sp, (PyrSlot*)&gSpecialValues[svFalse]);
			}
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			break;
		case 211 : // opNotNil
			if (NotNil(&sp[0])) {
				slotCopy(sp, (PyrSlot*)&gSpecialValues[svTrue]);
			} else {
				SetTagRaw(&sp[0], tagFalse);
			}
#if TAILCALLOPTIMIZE
			g->tailCall = 0;
#endif
			break;

		case 212 :  case 213 :  case 214 :  case 215 :
		case 216 :  case 217 :  case 218 :  case 219 :
		case 220 :  case 221 :  case 222 :  case 223 :
			unary_send:
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op1 & 15;
			doSpecialUnaryArithMsg(g, -1);
			sp = g->sp; ip = g->ip;
			break;

		// opSendSpecialBinaryArithMsg
		case 224 : // add
			if (IsInt(&sp[-1])) {
				if (IsInt(&sp[0])) {
					--sp; SetRaw(&sp[0], slotRawInt(&sp[0]) + slotRawInt(&sp[1]));
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
			break;
		case 225 : // subtract
			if (IsInt(&sp[-1])) {
				if (IsInt(&sp[0])) {
					--sp; SetRaw(&sp[0], slotRawInt(&sp[0]) - slotRawInt(&sp[1]));
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
			break;
		case 226 :  // multiply
			if (IsInt(&sp[-1])) {
				if (IsInt(&sp[0])) {
					--sp; SetRaw(&sp[0], slotRawInt(&sp[0]) * slotRawInt(&sp[1]));
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
			break;

		case 227 :
		case 228 :  case 229 :  case 230 :  case 231 :
		case 232 :  case 233 :  case 234 :  case 235 :
		case 236 :  case 237 :  case 238 :  case 239 :
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op1 & 15;
			doSpecialBinaryArithMsg(g, 2, false);
			sp = g->sp; ip = g->ip;
			break;

		// opSpecialOpcodes
		case 240 : --sp; break; // opDrop
		case 241 : ++sp; *sp = sp[-1]; break;	// opDup

		case 242 : // opcFunctionReturn
			g->sp = sp; g->ip = ip;
			returnFromBlock(g);
			sp = g->sp; ip = g->ip;
			break;
		case 243 : // opcReturn
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			break;
		case 244 : // opcReturnSelf
		slotCopy(++sp, &g->receiver);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			break;
		case 245 : // opcReturnTrue
			slotCopy(++sp, (PyrSlot*)&gSpecialValues[svTrue]);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			break;
		case 246 : // opcReturnFalse
			slotCopy(++sp, (PyrSlot*)&gSpecialValues[svFalse]);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			break;
		case 247 : // opcReturnNil
			slotCopy(++sp, (PyrSlot*)&gSpecialValues[svNil]);
			g->sp = sp; g->ip = ip;
			returnFromMethod(g);
			sp = g->sp; ip = g->ip;
			break;

		case 248 : // opcJumpIfFalse
			// cannot compare with o_false because it is NaN
			if ( IsFalse(sp) ) {
				jmplen = (ip[1]<<8) | ip[2];
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
			break;
		case 249 : // opcJumpIfFalsePushNil
			if ( IsFalse(sp)) {
				jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
				slotCopy(sp, (PyrSlot*)&gSpecialValues[svNil]);
			} else if ( IsTrue(sp)) {
				--sp;
				ip+=2;
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			break;
		case 250 : // opcJumpIfFalsePushFalse
			if (IsFalse(sp)) {
				jmplen = (ip[1]<<8) | ip[2];
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
			break;
		case 251 : // opcJumpIfTruePushTrue
			if (IsFalse(sp)) {
				--sp;
				ip+=2;
			} else if (IsTrue(sp)) {
				jmplen = (ip[1]<<8) | ip[2];
				ip += jmplen + 2;
				slotCopy(sp, (PyrSlot*)&gSpecialValues[svTrue]);
			} else {
				numArgsPushed = 1;
				selector = gSpecialSelectors[opmNonBooleanError];
				slot = sp;

				goto class_lookup;
			}
			break;
		case 252 : // opcJumpFwd
			jmplen = (ip[1]<<8) | ip[2];
			ip += jmplen + 2;
			break;
		case 253 : // opcJumpBak
			--sp; // also drops the stack. This saves an opcode in the while loop
					// which is the only place this opcode is used.
			jmplen = (ip[1]<<8) | ip[2];
			ip -= jmplen;

			//assert(g->gc->SanityCheck());
			break;
		case 254 : // opcSpecialBinaryOpWithAdverb
			op2 = ip[1]; ++ip; // get selector index
			g->sp = sp; g->ip = ip;
			g->primitiveIndex = op2;
			doSpecialBinaryArithMsg(g, 3, false);
			sp = g->sp; ip = g->ip;
			break;
		case 255 : // opcTailCallReturnFromMethod
#if TAILCALLOPTIMIZE
			g->tailCall = 1;
#endif
			break;

			////////////////////////////////////

			class_lookup:
			// normal class lookup
			classobj = classOfSlot(slot);

			// message sends handled here:
			msg_lookup:
			index = slotRawInt(&classobj->classIndex) + selector->u.index;
			meth = gRowTable[index];

			if (slotRawSymbol(&meth->name) != selector) {
				g->sp = sp; g->ip = ip;
				doesNotUnderstand(g, selector, numArgsPushed);
				sp = g->sp; ip = g->ip;
			} else {
				PyrMethodRaw *methraw;
				methraw = METHRAW(meth);
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
							slotCopy(sp, &sp[index]);
						} else {
							slotCopy(sp, &slotRawObject(&meth->prototypeFrame)->slots[index]);
						}
						break;
					case methReturnInstVar : /* return inst var */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex;
						slotCopy(sp, &slotRawObject(slot)->slots[index]);
						break;
					case methAssignInstVar : /* assign inst var */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex;
						obj = slotRawObject(slot);
						if (obj->obj_flags & obj_immutable) { StoreToImmutableB(g, (PyrSlot*&)sp, ip); }
						else {
							if (numArgsPushed >= 2) {
								slotCopy(&obj->slots[index], &sp[1]);
								g->gc->GCWrite(obj, sp + 1);
							} else {
								slotCopy(&obj->slots[index], (PyrSlot*)&gSpecialValues[svNil]);
							}
							slotCopy(sp, slot);
						}
						break;
					case methReturnClassVar : /* return class var */
						sp -= numArgsPushed - 1;
						slotCopy(sp, &g->classvars->slots[methraw->specialIndex]);
						break;
					case methAssignClassVar : /* assign class var */
						sp -= numArgsPushed - 1;
						if (numArgsPushed >= 2) {
							slotCopy(&g->classvars->slots[methraw->specialIndex], &sp[1]);
							g->gc->GCWrite(g->classvars, sp + 1);
						} else {
							g->classvars->slots[methraw->specialIndex].uf = gSpecialValues[svNil];
						}
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
			continue;

			////////////////////////////////////

			key_class_lookup:
			// normal class lookup
			classobj = classOfSlot(slot);

			// message sends handled here:
			key_msg_lookup:
			index = slotRawInt(&classobj->classIndex) + selector->u.index;
			meth = gRowTable[index];

			if (slotRawSymbol(&meth->name) != selector) {
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
							slotCopy(sp, &sp[index]);
						} else {
							slotCopy(sp, &slotRawObject(&meth->prototypeFrame)->slots[index]);
						}
						break;
					case methReturnInstVar : /* return inst var */
						sp -= numArgsPushed - 1;
						index = methraw->specialIndex;
						slotCopy(sp, &slotRawObject(slot)->slots[index]);
						break;
					case methAssignInstVar : /* assign inst var */
						sp -= numArgsPushed - 1;
						numArgsPushed -= numKeyArgsPushed << 1;
						index = methraw->specialIndex;
						obj = slotRawObject(slot);
						if (obj->obj_flags & obj_immutable) { StoreToImmutableB(g, (PyrSlot*&)sp, ip); }
						else {
							if (numArgsPushed >= 2) {
								slotCopy(&obj->slots[index], &sp[1]);
								g->gc->GCWrite(obj, sp + 1);
							} else {
								obj->slots[index].uf = gSpecialValues[svNil];
							}
							slotCopy(sp, slot);
						}
						break;
					case methReturnClassVar : /* return class var */
						sp -= numArgsPushed - 1;
						slotCopy(sp, &g->classvars->slots[methraw->specialIndex]);
						break;
					case methAssignClassVar : /* assign class var */
						sp -= numArgsPushed - 1;
						if (numArgsPushed >= 2) {
							slotCopy(&g->classvars->slots[methraw->specialIndex], &sp[1]);
							g->gc->GCWrite(g->classvars, sp + 1);
						} else {
							g->classvars->slots[methraw->specialIndex].uf = gSpecialValues[svNil];
						}
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
	} // switch(op1)
	} // end while(running)
#ifndef SC_WIN32
	running = true; // reset the signal
#endif
	g->sp = sp; g->ip = ip;
}

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
		post("%s ip %d\n", str, (char*)slotRawInt(&frame->ip) - (char*)slotRawObject(&slotRawMethod(&frame->method)->code)->slots);
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
	int i;
	PyrSlot *slot;
	char str[128];
#if BCSTAT
	dumpbcstat();
#endif
	postfl("STACK:\n");
	slot = sp - 64;
	if (slot < g->gc->Stack()->slots) slot = g->gc->Stack()->slots;
	for (i=slot - g->gc->Stack()->slots; slot<=sp; slot++, ++i) {
		slotString(slot, str);
		post("   %2d  %s\n", i, str);
	}
}



