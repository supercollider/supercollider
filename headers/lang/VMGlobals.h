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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*

Each virtual machine has a copy of VMGlobals, which contains the state of the virtual machine.

*/

#ifndef _VMGLOBALS_H_
#define _VMGLOBALS_H_

#include "PyrSlot.h"
#include "SC_AllocPool.h"
#include "SC_RGen.h"

const int kNumProcesses = 1;
const int kMainProcessID = 0;

typedef void (*FifoMsgFunc)(struct VMGlobals*, struct FifoMsg*);

struct FifoMsg {
	FifoMsg() : func(0), dataPtr(0) { dataWord[0] = dataWord[1] = 0; }
	void Perform(struct VMGlobals* g);
	void Free(struct VMGlobals* g);
	
	FifoMsgFunc func;
	void* dataPtr;
	long dataWord[2];
};

struct VMGlobals {
	VMGlobals();
	
	// global context
	class AllocPool *allocPool;
	struct PyrProcess *process;
	class SymbolTable *symbolTable;
	class PyrGC *gc;		// garbage collector for this process
	PyrSlot *classvars;
	bool canCallOS;

	// thread context
	struct PyrThread *thread;
	struct PyrMethod *method;
	struct PyrBlock *block;
	struct PyrFrame *frame;
	struct PyrMethod *primitiveMethod;
	unsigned char *ip;		// current instruction pointer
	PyrSlot *sp;	// current stack ptr
	PyrSlot *args;
	PyrSlot receiver;	// the receiver
	PyrSlot result;
	int numpop; // number of args to pop for primitive
	long returnLevels;
	long processID;
	long primitiveIndex;
	RGen *rgen;
	
	// scratch context
	long execMethod;
} ;

inline void FifoMsg::Perform(struct VMGlobals* g)
	{
		(func)(g, this);
	}
	
inline void FifoMsg::Free(struct VMGlobals* g)
	{
		g->allocPool->Free(dataPtr);
	}

extern VMGlobals gVMGlobals[kNumProcesses];
extern VMGlobals *gMainVMGlobals;
extern VMGlobals *gCompilingVMGlobals;
#endif

