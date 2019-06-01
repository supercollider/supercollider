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

An object archiving system for SuperCollider.

*/

#include "PyrArchiverT.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "VMGlobals.h"
#include "GC.h"
#include "ReadWriteMacros.h"

int prAsArchive(struct VMGlobals* g, int numArgsPushed);
int prAsArchive(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    PyrArchiver<char*> arch(g);

    int err = arch.prepareToWriteArchive(a);
    if (err)
        return err;

    int32 size = arch.calcArchiveSize();

    PyrInt8Array* obj = newPyrInt8Array(g->gc, size, 0, true);
    obj->size = size;
    arch.setStream((char*)obj->b);
    err = arch.writeArchive();

    if (err == errNone)
        SetObject(a, obj);
    else
        SetNil(a);

    return err;
}

int prUnarchive(struct VMGlobals* g, int numArgsPushed);
int prUnarchive(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    if (!isKindOfSlot(a, class_int8array))
        return errWrongType;

    PyrArchiver<char*> arch(g);

    arch.setStream((char*)slotRawObject(a)->slots);
    int err = arch.readArchive(a);
    return err;
}

int prWriteArchive(struct VMGlobals* g, int numArgsPushed);
int prWriteArchive(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    if (!isKindOfSlot(b, class_string))
        return errWrongType;

    char pathname[PATH_MAX];
    memcpy(pathname, slotRawString(b)->s, slotRawObject(b)->size);
    pathname[slotRawString(b)->size] = 0;

    PyrArchiver<FILE*> arch(g);
    FILE* file = fopen(pathname, "wb");
    int err = errNone;
    if (file) {
        err = arch.prepareToWriteArchive(a);
        if (!err) {
            arch.setStream(file);
            err = arch.writeArchive();
        }
        fclose(file);
    } else {
        error("file open failed\n");
        err = errFailed;
    }
    return err;
}

int prReadArchive(struct VMGlobals* g, int numArgsPushed);
int prReadArchive(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    if (!isKindOfSlot(b, class_string))
        return errWrongType;

    char pathname[PATH_MAX];
    memcpy(pathname, slotRawString(b)->s, slotRawObject(b)->size);
    pathname[slotRawString(b)->size] = 0;

    PyrArchiver<FILE*> arch(g);
    FILE* file = fopen(pathname, "rb");

    int err;
    if (file) {
        arch.setStream(file);
        err = arch.readArchive(a);
        fclose(file);
    } else {
        error("file open failed\n");
        err = errFailed;
    }
    return err;
}

void initArchiverPrimitives();
void initArchiverPrimitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_AsArchive", prAsArchive, 1, 0);
    definePrimitive(base, index++, "_Unarchive", prUnarchive, 1, 0);
    definePrimitive(base, index++, "_WriteArchive", prWriteArchive, 2, 0);
    definePrimitive(base, index++, "_ReadArchive", prReadArchive, 2, 0);
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
    initArchiverPrimitives();
}

// This function is called when the plug in is loaded into SC.
// It returns an instance of APlugIn.
SCPlugIn* loadPlugIn() { return new APlugIn(); }

#endif
