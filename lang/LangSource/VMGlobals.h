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
/*

Each virtual machine has a copy of VMGlobals, which contains the state of the virtual machine.

*/

#pragma once

#include "PyrSlot.h"
#include "SC_AllocPool.h"
#include "SC_RGen.h"
#include <setjmp.h>
#include <map>
#include <cstdint>
#include "GluonFFI.hpp"


#define TAILCALLOPTIMIZE 1

typedef void (*FifoMsgFunc)(struct VMGlobals*, struct FifoMsg*);

struct FifoMsg {
    FifoMsg(): func(0), dataPtr(0) { dataWord[0] = dataWord[1] = 0; }
    void Perform(struct VMGlobals* g);
    void Free(struct VMGlobals* g);

    FifoMsgFunc func;
    void* dataPtr;
    std::int64_t dataWord[2];
};


// WARNING: This object is created in the global scope, meaning it is susceptible to the global destructor bug.
// Additionally, it isn't possible to call functions like `getsym' in the constructor of this object as they require
// a specific extern global variable to be set.
// This means all members must also obey these constraints.
// VMGlobals is actually initialised in `initRuntime'.
// TODO: It would be nice to refact `initRuntime' so proper c++ constructors can be used.
struct VMGlobals {
    VMGlobals();
    VMGlobals(VMGlobals&&) = delete;
    VMGlobals(const VMGlobals&) = delete;
    VMGlobals& operator=(VMGlobals&&) = delete;
    VMGlobals& operator=(const VMGlobals&) = delete;

    // global context
    class AllocPool* allocPool { nullptr };
    struct PyrProcess* process { nullptr };
    class SymbolTable* symbolTable { nullptr };
    class PyrGC* gc { nullptr }; // garbage collector for this process
    PyrObject* classvars { nullptr };
    sc_gluon::GluonManager gluonManager {};
#if TAILCALLOPTIMIZE
    int tailCall {}; // next byte code is a tail call.
#endif
    bool canCallOS { false };

    // thread context
    struct PyrThread* thread { nullptr };
    struct PyrMethod* method { nullptr };
    struct PyrBlock* block { nullptr };
    struct PyrFrame* frame { nullptr };
    struct PyrMethod* primitiveMethod { nullptr };
    unsigned char* ip { nullptr }; // current instruction pointer
    PyrSlot* sp { nullptr }; // current stack ptr
    PyrSlot* args { nullptr };
    PyrSlot receiver; // init to nil in cpp
    PyrSlot result; // init to nil in cpp
    int numpop { 0 }; // number of args to pop for primitive
    std::int64_t primitiveIndex { 0 };
    RGen* rgen { nullptr };
    jmp_buf escapeInterpreter;

    // scratch context
    std::int64_t execMethod { 0 };

    // primitive exceptions
    std::map<PyrThread*, std::pair<std::exception_ptr, PyrMethod*>> lastExceptions {};
};

inline void FifoMsg::Perform(struct VMGlobals* g) { (func)(g, this); }

inline void FifoMsg::Free(struct VMGlobals* g) { g->allocPool->Free(dataPtr); }

extern VMGlobals gVMGlobals;
extern VMGlobals* gMainVMGlobals;
extern VMGlobals* gCompilingVMGlobals;
