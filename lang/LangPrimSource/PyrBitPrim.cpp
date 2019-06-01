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

Primitives for some bit operations.

*/

#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "clz.h"


int prNumBits(VMGlobals* g, int numArgsPushed);
int prNumBits(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetRaw(a, NUMBITS(slotRawInt(a)));
    return errNone;
}

int prLog2Ceil(VMGlobals* g, int numArgsPushed);
int prLog2Ceil(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetRaw(a, LOG2CEIL(slotRawInt(a)));
    return errNone;
}

int prCLZ(VMGlobals* g, int numArgsPushed);
int prCLZ(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetRaw(a, CLZ(slotRawInt(a)));
    return errNone;
}

int prCTZ(VMGlobals* g, int numArgsPushed);
int prCTZ(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetRaw(a, CTZ(slotRawInt(a)));
    return errNone;
}

int prNextPowerOfTwo(VMGlobals* g, int numArgsPushed);
int prNextPowerOfTwo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;

    SetRaw(a, NEXTPOWEROFTWO(slotRawInt(a)));
    return errNone;
}

int prIsPowerOfTwo(VMGlobals* g, int numArgsPushed);
int prIsPowerOfTwo(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    SetBool(a, ISPOWEROFTWO(slotRawInt(a)));
    return errNone;
}

int prBinaryGrayCode(VMGlobals* g, int numArgsPushed);
int prBinaryGrayCode(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a;

    a = g->sp;
    SetRaw(a, GRAYCODE(slotRawInt(a)));
    return errNone;
}

int prSetBit(VMGlobals* g, int numArgsPushed);
int prSetBit(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    int32 bit, mask;
    int err = slotIntVal(b, &bit);
    if (err)
        return err;
    bit = bit & 31;
    mask = 1L << bit;
    if (IsFalse(c)) {
        SetRaw(a, slotRawInt(a) & ~mask);
    } else {
        SetRaw(a, slotRawInt(a) | mask);
    }
    return errNone;
}

int prHammingDistance(VMGlobals* g, int numArgsPushed);
int prHammingDistance(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    if (NotInt(a) || NotInt(b))
        return errWrongType;

    int aInt = slotRawInt(a);
    int bInt = slotRawInt(b);

    int count = 0, mask = 1;
    for (int i = 0; i < 32; i++) {
        if ((aInt & mask) != (bInt & mask))
            count = count + 1;
        mask = mask << 1;
    }
    SetRaw(a, count);

    return errNone;
}

void initBitPrimitives();
void initBitPrimitives() {
    int base, index = 0;

    base = nextPrimitiveIndex();

    definePrimitive(base, index++, "_NextPowerOfTwo", prNextPowerOfTwo, 1, 0);
    definePrimitive(base, index++, "_IsPowerOfTwo", prIsPowerOfTwo, 1, 0);
    definePrimitive(base, index++, "_CLZ", prCLZ, 1, 0);
    definePrimitive(base, index++, "_CTZ", prCTZ, 1, 0);
    definePrimitive(base, index++, "_NumBits", prNumBits, 1, 0);
    definePrimitive(base, index++, "_Log2Ceil", prLog2Ceil, 1, 0);
    definePrimitive(base, index++, "_SetBit", prSetBit, 3, 0);
    definePrimitive(base, index++, "_BinaryGrayCode", prBinaryGrayCode, 1, 0);
    definePrimitive(base, index++, "_HammingDistance", prHammingDistance, 2, 0);
}


#if _SC_PLUGINS_

#    include "SCPlugin.h"

// export the function that SC will call to load the plug in.
#    pragma export on
extern "C" {
SCPlugIn* loadPlugIn(void);
}
#    pragma export off


// define plug in object
class APlugIn : public SCPlugIn {
public:
    APlugIn();
    virtual ~APlugIn();

    virtual void AboutToCompile();
};

APlugIn::APlugIn() {
    // constructor for plug in
}

APlugIn::~APlugIn() {
    // destructor for plug in
}

void APlugIn::AboutToCompile() {
    // this is called each time the class library is compiled.
    initBitPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn() { return new APlugIn(); }

#endif
