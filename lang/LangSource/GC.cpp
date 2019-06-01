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


#include "GC.h"
#include "PyrKernel.h"
#include "PyrObjectProto.h"
#include "PyrSymbol.h"
#include "InitAlloc.h"
#include <string.h>
#include <stdexcept>

#define PAUSETIMES 0


double pauseBeginTime = 0.;
double totalPauseTime = 0.;
double maxPauseTime = 0.;
double minPauseTime = 1e9;
int pauseCount = 0;
int numPausesGreaterThanOneMillisecond = 0;
int maxPauseStackScans = 0;
int maxPauseFlips = 0;
int maxPauseScans = 0;
int maxPausePartialScans = 0;
int maxPauseNumToScan = 0;
int maxPauseSlotsScanned = 0;
int checkStackScans = 0;
int checkFlips = 0;
int checkNumToScan = 0;
int checkScans = 0;
int checkPartialScans = 0;
int checkSlotsScanned = 0;

double elapsedTime();

inline void PyrGC::beginPause() {
    checkStackScans = mStackScans;
    checkFlips = mFlips;
    checkScans = mScans;
    checkNumToScan = mNumToScan;
    checkPartialScans = mNumPartialScans;
    checkSlotsScanned = mSlotsScanned;
    pauseBeginTime = elapsedTime();
}

inline void PyrGC::endPause() {
    double pauseTime = elapsedTime() - pauseBeginTime;
    if (pauseTime > 0.001)
        numPausesGreaterThanOneMillisecond++;
    if (pauseTime > maxPauseTime) {
        maxPauseTime = pauseTime;
        maxPauseStackScans = mStackScans - checkStackScans;
        maxPauseFlips = mFlips - checkFlips;
        maxPauseScans = mScans - checkScans;
        maxPauseNumToScan = checkNumToScan;
        maxPausePartialScans = mNumPartialScans - checkPartialScans;
        maxPauseSlotsScanned = mSlotsScanned - checkSlotsScanned;
    }
    if (pauseTime < minPauseTime)
        minPauseTime = pauseTime;
    totalPauseTime += pauseTime;
    pauseCount++;
}

void PyrGC::reportPause() {
    post("pauses %d\n", pauseCount);
    post("total pause time %g\n", totalPauseTime);
    post("num pauses > 1 ms  %d\n", numPausesGreaterThanOneMillisecond);
    post("avg pause time %g\n", totalPauseTime / pauseCount);
    post("min pause time %g\n", minPauseTime);
    post("max pause time %g\n", maxPauseTime);
    post("max pause scans %d\n", maxPauseScans);
    post("max pause partial obj scans %d\n", maxPausePartialScans);
    post("max pause num to scan %d\n", maxPauseNumToScan);
    post("max pause flips %d\n", maxPauseFlips);
    post("max pause stack scans %d\n", maxPauseStackScans);
    post("max pause slots scanned %d\n", maxPauseSlotsScanned);

    pauseBeginTime = 0.;
    totalPauseTime = 0.;
    maxPauseTime = 0.;
    minPauseTime = 1e9;
    pauseCount = 0;
    numPausesGreaterThanOneMillisecond = 0;
}

#if PAUSETIMES

#    define BEGINPAUSE beginPause();
#    define ENDPAUSE endPause();
#    define REPORTPAUSE reportPause();

#else

#    define BEGINPAUSE
#    define ENDPAUSE
#    define REPORTPAUSE

#endif

/*

---
list segments:
    black gray white free sweep

scan phase:
    clear list of new nonlocal reached objects.
        when a non local object is reached, mark it, and put it on the list if not retained.
sweep phase:
    send any new retained objects to other system
    send any no longer reatined objects to the other system.
    send this list to
    enqueue finalization messages
        finalize: call finalize method, move from sweep area to free area
    list of nonlocal objects.
    list of nonlocal reached objects.
*/

void fatalerror(const char* str);
void fatalerror(const char* str) {
    fputs(str, stderr);
    postfl(str);
    throw std::runtime_error(str);
    // exit(-1);
}

inline int ScanSize(PyrObjectHdr* obj) { return obj->obj_format <= obj_slot ? obj->size : 0; }

HOT void PyrGC::ScanSlots(PyrSlot* inSlots, long inNumToScan) {
    if (inNumToScan == 0)
        return;

    unsigned char whiteColor = mWhiteColor;
    unsigned char greyColor = mGreyColor;

    mSlotsScanned += inNumToScan;

    int foundGreyObjects = 0;
    PyrObjectHdr* grey = &mGrey;
    PyrObjectHdr* greyNext = grey->next;

    PyrSlot* slot = inSlots;
    PyrSlot* endslot = inSlots + inNumToScan;
    do {
        if (IsObj(slot)) {
            PyrObject* obj = slotRawObject(slot);
            if (obj->gc_color == whiteColor) {
                /* used to be ToGrey2(obj), but rearranged for slightly better performance
                 *
                 * move obj from white to grey */

                PyrObjectHdr* objPrev = obj->prev;
                PyrObjectHdr* objNext = obj->next;

                /* link in grey set */
                greyNext->prev = obj;
                grey->next = obj;
                obj->prev = grey;
                obj->next = greyNext;
                greyNext = obj;

                // remove from old set
                objNext->prev = objPrev;
                objPrev->next = objNext;

                obj->gc_color = greyColor;
                foundGreyObjects++;
            }
        }
        ++slot;
    } while (slot != endslot);
    mNumGrey += foundGreyObjects;
}

void GCSet::Init(int inGCSet) {
    mBlack.classptr = NULL;
    mBlack.obj_sizeclass = inGCSet;
    mBlack.size = 0;
    mBlack.gc_color = obj_gcmarker;

    mWhite.classptr = NULL;
    mWhite.obj_sizeclass = inGCSet;
    mWhite.size = 0;
    mWhite.gc_color = obj_gcmarker;

    mFree = &mBlack;

    mBlack.next = &mWhite;
    mWhite.next = &mBlack;

    mBlack.prev = &mWhite;
    mWhite.prev = &mBlack;
}

void GCSet::MajorFlip() {
    // move all white items to beginning of free list
    mFree = mWhite.next;
    if (!PyrGC::IsMarker(mBlack.next)) {
        // move all black items to white list
        mWhite.next = mBlack.next;
        mFree->prev = mWhite.prev;
        mBlack.next->prev = &mWhite;
        mWhite.prev->next = mFree;

        // black list empty
        mBlack.next = &mWhite;
        mWhite.prev = &mBlack;
    }
}

void GCSet::MinorFlip() {
    // move all white items to beginning of free list
    mFree = mWhite.next;
}

PyrProcess* newPyrProcess(VMGlobals* g, PyrClass* procclassobj);

PyrGC::PyrGC(VMGlobals* g, AllocPool* inPool, PyrClass* mainProcessClass, long poolSize) {
    mVMGlobals = g;
    mPool = inPool;
    // mCurSet = 0;
    mNumToScan = 0;

    mFlips = 0;
    mCollects = 0;
    mAllocTotal = 0;
    mNumAllocs = 0;
    mScans = 0;
    mStackScans = 0;
    mNumPartialScans = 0;
    mSlotsScanned = 0;

    mGreyColor = 3 << 2;
    mBlackColor = 2 << 2;
    mWhiteColor = 1 << 2;
    mFreeColor = 0;

    mRunning = false;

    mCanSweep = false;
    mPartialScanObj = NULL;
    mPartialScanSlot = 0;
    mUncollectedAllocations = 0;

    mGrey.classptr = NULL;
    mGrey.obj_sizeclass = 0;
    mGrey.size = 0;
    mGrey.gc_color = obj_gcmarker;

    mGrey.prev = &mGrey;
    mGrey.next = &mGrey;

    mNumGrey = 0;

    mNewPool.Init(mPool, poolSize, poolSize, 9000);

    // initialize treadmills
    for (int i = 0; i < kNumGCSets; ++i) {
        mSets[i].Init(i);
    }
    g->process = NULL; // initPyrThread checks to see if process has been started
    mProcess = newPyrProcess(g, mainProcessClass);

    mStack = slotRawObject(&slotRawThread(&mProcess->mainThread)->stack);
    ToBlack(mStack);
    SetNil(&slotRawThread(&mProcess->mainThread)->stack);

    mNumGrey = 0;
    ToGrey2(mProcess);
    g->sp = mStack->slots - 1;
    g->process = mProcess;
    mRunning = true;

    SanityCheck();
    // assert(SanityCheck());
}

PyrObject* PyrGC::NewPermanent(size_t inNumBytes, long inFlags, long inFormat) {
    // obtain size info
    int32 alignedSize = (inNumBytes + kAlignMask) & ~kAlignMask; // 16 byte align
    int32 numSlots = alignedSize / sizeof(PyrSlot);
    numSlots = numSlots < 1 ? 1 : numSlots;
    int32 sizeclass = LOG2CEIL(numSlots);
    sizeclass = sc_min(sizeclass, kNumGCSizeClasses - 1);

    int32 allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);

    // allocate permanent objects
    PyrObject* obj = (PyrObject*)pyr_pool_runtime->Alloc(allocSize);

    obj->gc_color = obj_permanent;
    obj->next = obj->prev = NULL;
    obj->obj_sizeclass = sizeclass;
    obj->obj_format = inFormat;
    obj->obj_flags = inFlags;
    obj->size = 0;
    obj->classptr = class_object;
    return obj;
}

void PyrGC::BecomePermanent(PyrObject* inObject) {
    if (IsGrey(inObject))
        mNumGrey--;
    DLRemove(inObject);
    inObject->gc_color = obj_permanent;
    inObject->obj_flags |= obj_immutable;
    inObject->next = inObject->prev = inObject;
}

void PyrGC::BecomeImmutable(PyrObject* inObject) { inObject->obj_flags |= obj_immutable; }

void DumpBackTrace(VMGlobals* g);

HOT PyrObject* PyrGC::New(size_t inNumBytes, long inFlags, long inFormat, bool inRunCollection) {
    PyrObject* obj = NULL;

    if (inFlags & obj_permanent) {
        return NewPermanent(inNumBytes, inFlags, inFormat);
    }

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif

    // obtain size info

    int32 alignedSize = (inNumBytes + kAlignMask) & ~kAlignMask; // 16 byte align
    int32 numSlots = alignedSize / sizeof(PyrSlot);
    numSlots = numSlots < 1 ? 1 : numSlots;
    int32 sizeclass = LOG2CEIL(numSlots);
    sizeclass = sc_min(sizeclass, kNumGCSizeClasses - 1);

    int32 credit = 1L << sizeclass;
    mAllocTotal += credit;
    mNumAllocs++;

    mNumToScan += credit;
    obj = Allocate(inNumBytes, sizeclass, inRunCollection);

    obj->obj_format = inFormat;
    obj->obj_flags = inFlags & 255;
    obj->size = 0;
    obj->classptr = class_object;
    obj->gc_color = mWhiteColor;

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif
    return obj;
}


HOT PyrObject* PyrGC::NewFrame(size_t inNumBytes, long inFlags, long inFormat, bool inAccount) {
    PyrObject* obj = NULL;

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif

    // obtain size info

    int32 alignedSize = (inNumBytes + kAlignMask) & ~kAlignMask; // 16 byte align
    int32 numSlots = alignedSize / sizeof(PyrSlot);
    numSlots = numSlots < 1 ? 1 : numSlots;
    int32 sizeclass = LOG2CEIL(numSlots);
    sizeclass = sc_min(sizeclass, kNumGCSizeClasses - 1);

    int32 credit = 1L << sizeclass;
    mAllocTotal += credit;
    mNumAllocs++;
    mNumToScan += credit;

    obj = Allocate(inNumBytes, sizeclass, inAccount);

    obj->obj_format = inFormat;
    obj->obj_flags = inFlags;
    obj->size = 0;
    obj->classptr = class_frame;
    obj->gc_color = mWhiteColor;

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif
    return obj;
}

PyrObject* PyrGC::NewFinalizer(ObjFuncPtr finalizeFunc, PyrObject* inObject, bool inRunCollection) {
    PyrObject* obj = NULL;

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif

    // obtain size info

    int32 sizeclass = 1;

    int32 credit = 1L << sizeclass;
    mNumToScan += credit;
    mAllocTotal += credit;
    mNumAllocs++;

    if (inRunCollection && mNumToScan >= kScanThreshold) {
        Collect();
    }

    GCSet* gcs = mSets + kFinalizerSet;

    obj = (PyrObject*)gcs->mFree;
    if (!IsMarker(obj)) {
        // from free list
        gcs->mFree = obj->next;
    } else {
        if (sizeclass > kMaxPoolSet) {
            SweepBigObjects();
            int32 allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
            obj = (PyrObject*)mPool->Alloc(allocSize);
        } else {
            int32 allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
            obj = (PyrObject*)mNewPool.Alloc(allocSize);
        }
        if (!obj) {
            post("Finalizer alloc failed.\n");
            MEMFAILED;
        }
        DLInsertAfter(&gcs->mWhite, obj);
    }


    obj->obj_sizeclass = sizeclass;
    obj->obj_format = obj_slot;
    obj->obj_flags = 0;
    obj->size = 2;
    obj->classptr = class_finalizer;
    obj->gc_color = mWhiteColor;

    SetPtr(obj->slots + 0, (void*)finalizeFunc);
    SetObject(obj->slots + 1, inObject);

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif
    return obj;
}


void PyrGC::SweepBigObjects() {
    if (!mCanSweep)
        return;

    for (int i = kMaxPoolSet + 1; i < kNumGCSizeClasses; ++i) {
        GCSet* gcs = mSets + i;
        PyrObjectHdr* obj = gcs->mFree;

        if (!IsMarker(obj)) {
            // unlink chain of free objects
            gcs->mFree = obj->prev->next = &gcs->mBlack;
            gcs->mBlack.prev = obj->prev;

            do {
                PyrObjectHdr* nextobj = obj->next;
                void* ptr = (void*)obj;
                mPool->Free(ptr);
                obj = nextobj;
            } while (!IsMarker(obj));
        }
    }
    mCanSweep = false;
}

void PyrGC::CompletePartialScan(PyrObject* obj) {
    if (mPartialScanObj == obj) {
        int32 remain = obj->size - mPartialScanSlot;
        ScanSlots(mPartialScanObj->slots + mPartialScanSlot, remain);
    }
}

HOT void PyrGC::DoPartialScan(int32 inObjSize) {
    int32 remain = inObjSize - mPartialScanSlot;
    mNumPartialScans++;
    if (remain <= 0) {
        mPartialScanObj = NULL;
        mNumToScan -= 4;
        if (mNumToScan < 0)
            mNumToScan = 0;
        return;
    }
    int32 numtoscan = sc_min(remain, mNumToScan);
    ScanSlots(mPartialScanObj->slots + mPartialScanSlot, numtoscan);

    if (numtoscan == remain) {
        mPartialScanObj = NULL;
        mNumToScan -= numtoscan + 4;
    } else {
        mPartialScanSlot += numtoscan;
        mNumToScan -= numtoscan;
    }
    if (mNumToScan < 0)
        mNumToScan = 0;
    // post("partial %5d xx %4d %2d %s\n", mScans, mNumToScan, mNumGrey);
    // post("partial %5d %2d %4d %2d %s\n", mScans, i, mNumToScan, mNumGrey, slotRawSymbol(&obj->classptr->name)->name);
}

HOT bool PyrGC::ScanOneObj() {
    // Find a set that has a grey object
    PyrObject* obj;
    obj = (PyrObject*)mGrey.next;
    if (IsMarker(obj)) {
        if (mNumGrey)
            fatalerror("grey count error\n");
        return false;
    }

    /*if (!IsGrey(obj)) {
        postfl("Object on grey list not grey  %d %d\n", obj->gc_color, mGreyColor);
        fatalerror("C1");
    }*/

    mScans++;

    // post("-> scan %d %d %d\n", mNumGrey, IsGrey(obj), mNumToScan);
    // Found a grey object
    // move obj from grey to black

    ToBlack(obj);

    int32 size = ScanSize(obj);
    // post("<- scan %d %d %d %d\n", mNumGrey, IsGrey(obj), mNumToScan, size);
    if (size > mNumToScan + 32) {
        mPartialScanObj = obj;
        mPartialScanSlot = 0;
        DoPartialScan(size);
    } else if (size > 0) {
        ScanSlots(obj->slots, size);
        mNumToScan -= 1L << obj->obj_sizeclass;
        if (mNumToScan < 0)
            mNumToScan = 0;
    } else {
        mNumToScan -= 1L << obj->obj_sizeclass;
        if (mNumToScan < 0)
            mNumToScan = 0;
    }
    return true;
}

void PyrGC::ScanStack() {
    // scan the stack
    PyrObject* obj = mStack;

    VMGlobals* g = mVMGlobals;

    PyrSlot* slot = obj->slots;
    int32 size = obj->size = g->sp - slot + 1;

    ScanSlots(slot, size);
}

void PyrGC::ScanFrames() {
    VMGlobals* g = mVMGlobals;
    PyrFrame* frame = g->frame;
    while (frame) {
#if 1
        // this is more incremental
        if (IsWhite(frame)) {
            ToGrey2(frame);
        }
#else
        // this is more efficient
        if (!IsBlack(frame)) {
            ToBlack(frame);
            int32 size = ScanSize(frame);
            PyrSlot* slots = ((PyrObject*)frame)->slots;
            ScanSlots(slots, size);
        }
#endif
        frame = slotRawFrame(&frame->caller);
    }
}

void PyrGC::Flip() {
#ifdef GC_SANITYCHECK
    SanityCheck();
#endif

    ScanFinalizers();

    GCSet* gcs = mSets;
    if ((mFlips & 3) == 0) {
        // major flip
        for (int i = 0; i < kNumGCSets; ++i, ++gcs) {
            gcs->MajorFlip();
        }

        // advance colors
        mBlackColor += 4;
        mWhiteColor += 4;
        mGreyColor += 4;
        mFreeColor += 4;
    } else {
        // minor flip
        for (int i = 0; i < kNumGCSets; ++i, ++gcs) {
            gcs->MinorFlip();
        }
    }
    // move root to grey area
    mNumGrey = 0;
    ToGrey2(mProcess);

    ToBlack(mStack);

    // reset counts
    mNumToScan = 0;
    mCanSweep = true;

    mFlips++;
    // post("flips %d  collects %d   nalloc %d   alloc %d   grey %d\n", mFlips, mCollects, mNumAllocs, mAllocTotal,
    // mNumGrey);

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif
}


void PyrGC::FullCollection() {
    Collect(100000000); // collect space
    SweepBigObjects();
}

void PyrGC::Collect(int32 inNumToScan) {
    mNumToScan = sc_max(mNumToScan, inNumToScan);
    Collect(); // collect space
}

HOT void PyrGC::Collect() {
    BEGINPAUSE
    bool stackScanned = false;
    mCollects++;

#ifdef GC_SANITYCHECK
    SanityCheck();
#endif

    if (mNumToScan > 0) {
        // post("->Collect  ns %d  ng %d  s %d\n", mNumToScan, mNumGrey, mScans);
        // DumpInfo();
        mNumToScan += mNumToScan >> 3;

        // post("->Collect2  ns %d  ng %d  s %d\n", mNumToScan, mNumGrey, mScans);
        // mCurSet = 0;
        while (mNumToScan > 0) {
            while (mNumToScan > 0 && (mNumGrey > 0 || mPartialScanObj)) {
                if (mPartialScanObj) {
                    DoPartialScan(ScanSize(mPartialScanObj));
                } else {
                    if (!ScanOneObj())
                        break;
                }
            }
            if (mNumGrey == 0 && mPartialScanObj == NULL) {
                if (!stackScanned) {
                    stackScanned = true;
                    mStackScans++;
                    ScanStack();
                    ScanFrames();
                }
                if (mNumGrey == 0 && mPartialScanObj == NULL && stackScanned) {
                    Flip();
                    break;
                }
            }
        }
        // post("<-Collect  ns %d  ng %d  s %d\n", mNumToScan, mNumGrey, mScans);
        // DumpInfo();
        // post("size9:\n");
        // TraceAnyPathToObjsOfSize(9);
        // post("greys:\n");
        // TraceAnyPathToAllGrey();
    }
    // post("mNumToScan %d\n", mNumToScan);

    mUncollectedAllocations = 0;
#ifdef GC_SANITYCHECK
    SanityCheck();
#endif
    ENDPAUSE
}


void PyrGC::Finalize(PyrObject* finalizer) {
    if (!IsPtr(finalizer->slots + 0))
        return;
    if (!IsObj(finalizer->slots + 1))
        return;

    ObjFuncPtr func = (ObjFuncPtr)slotRawPtr(&finalizer->slots[0]);
    PyrObject* obj = slotRawObject(&finalizer->slots[1]);
    // post("FINALIZE %s %p\n", slotRawSymbol(&obj->classptr->name)->name, obj);
    (func)(mVMGlobals, obj);

    SetNil(obj->slots + 0);
    SetNil(obj->slots + 1);
}

void PyrGC::ScanFinalizers() {
    GCSet* gcs = &mSets[kFinalizerSet];
    PyrObjectHdr* obj = gcs->mWhite.next;
    PyrObjectHdr* firstFreeObj = gcs->mFree;

    while (obj != firstFreeObj) {
        Finalize((PyrObject*)obj);
        obj = obj->next;
    }
}

void PyrGC::RunAllFinalizers() {
    GCSet* gcs = &mSets[kFinalizerSet];

    PyrObjectHdr* obj = gcs->mBlack.next;
    while (!IsMarker(obj)) {
        Finalize((PyrObject*)obj);
        obj = obj->next;
    }

    obj = gcs->mWhite.next;
    PyrObjectHdr* firstFreeObj = gcs->mFree;
    while (obj != firstFreeObj) {
        Finalize((PyrObject*)obj);
        obj = obj->next;
    }

    obj = mGrey.next;
    while (!IsMarker(obj)) {
        if (obj->classptr == class_finalizer)
            Finalize((PyrObject*)obj);
        obj = obj->next;
    }
}

bool PyrGC::SanityCheck2() {
    int numgrey = 0;
    PyrObjectHdr* grey = mGrey.next;
    while (!IsMarker(grey)) {
        numgrey++;
        if (!IsGrey(grey)) {
            postfl("sc Object on grey list not grey  %d %d   %d\n", grey->gc_color, mGreyColor, numgrey);
            return false;
        }
        grey = grey->next;
    }
    // postfl("sc %d %d\n", mNumGrey, numgrey);
    return mNumGrey == numgrey;
}

bool PyrGC::SanityCheck() {
    if (!mRunning)
        return true;

    // postfl("PyrGC::SanityCheck\n");
    bool res = LinkSanity()
        && ListSanity()
        // 			&& SanityMarkObj((PyrObject*)mProcess,NULL,0) && SanityMarkObj(mStack,NULL,0)
        // 			&& SanityClearObj((PyrObject*)mProcess,0) && SanityClearObj(mStack,0)
        && SanityCheck2();
    // if (!res) DumpInfo();
    // if (!res) Debugger();
    return res;
}

bool PyrGC::ListSanity() {
    bool found;

    if (StackDepth() < 0) {
        fprintf(stderr, "stack underflow %d\n", (int)StackDepth());
        return false;
    }

    // postfl("PyrGC::ListSanity\n");
    for (int i = 0; i < kNumGCSets; ++i) {
        PyrObjectHdr* obj;
        GCSet* set = mSets + i;

        // check black marker
        obj = &set->mBlack;
        if (!IsMarker(obj)) {
            // debugf("set %d black marker color wrong %d %p\n", i, obj->gc_color, obj);
            fprintf(stderr, "set %d black marker color wrong %d %p\n", i, obj->gc_color, obj);
            setPostFile(stderr);
            DumpBackTrace(mVMGlobals);
            dumpBadObject((PyrObject*)obj);
            return false;
        }

        // check white marker
        obj = &set->mWhite;
        if (!IsMarker(obj)) {
            // debugf("set %d white marker color wrong %d %p\n", i, obj->gc_color, obj);
            fprintf(stderr, "set %d white marker color wrong %d %p\n", i, obj->gc_color, obj);
            setPostFile(stderr);
            DumpBackTrace(mVMGlobals);
            dumpBadObject((PyrObject*)obj);
            return false;
        }

        // check free pointer between white and black marker
        if (set->mFree != &set->mBlack) {
            obj = set->mWhite.next;
            found = false;
            while (!IsMarker(obj)) {
                if (obj == set->mFree) {
                    found = true;
                    break;
                }
                obj = obj->next;
            }
            if (!found) {
                // debugf("set %d free pointer not between white and black\n", i);
                fprintf(stderr, "set %d free pointer not between white and black\n", i);
                fprintf(stderr, "set->mFree %p\n", set->mFree);
                fprintf(stderr, "set->mWhite %p\n", &set->mWhite);
                fprintf(stderr, "set->mBlack %p\n", &set->mBlack);
                setPostFile(stderr);
                DumpBackTrace(mVMGlobals);
                dumpBadObject((PyrObject*)set->mFree);

                fprintf(stderr, "black %d white %d grey %d\n", mBlackColor, mWhiteColor, mGreyColor);

                obj = &set->mWhite;
                int count = 0;
                do {
                    if (obj == set->mFree)
                        fprintf(stderr, "%4d %p %3d %d FREE\n", count, obj, obj->gc_color, obj->obj_sizeclass);
                    else if (obj == &set->mWhite)
                        fprintf(stderr, "%4d %p %3d %d WHITE\n", count, obj, obj->gc_color, obj->obj_sizeclass);
                    else if (obj == &set->mBlack)
                        fprintf(stderr, "%4d %p %3d %d BLACK\n", count, obj, obj->gc_color, obj->obj_sizeclass);
                    else
                        fprintf(stderr, "%4d %p %3d %d\n", count, obj, obj->gc_color, obj->obj_sizeclass);
                    obj = obj->next;
                    count++;
                } while (obj != &set->mWhite);

                return false;
            }
        }

        // scan black list
        obj = set->mBlack.next;
        while (!IsMarker(obj)) {
            if (obj->gc_color != mBlackColor) {
                // debugf("set %d black list obj color wrong %d (%d, %d, %d) %p\n",
                //	i, obj->gc_color, mBlackColor, mGreyColor, mWhiteColor, obj);
                fprintf(stderr, "set %d black list obj color wrong %d (%d, %d, %d) %p\n", i, obj->gc_color, mBlackColor,
                        mGreyColor, mWhiteColor, obj);
                setPostFile(stderr);
                DumpBackTrace(mVMGlobals);
                dumpBadObject((PyrObject*)obj);
                return false;
            }
            if (GetGCSet(obj) != set) {
                // debugf("set %d black obj gcset wrong %d %p\n", i, obj->obj_sizeclass, obj);
                fprintf(stderr, "set %d black obj gcset wrong %d %p\n", i, obj->obj_sizeclass, obj);
                setPostFile(stderr);
                dumpBadObject((PyrObject*)obj);
                return false;
            }
            if (obj->next->prev != obj) {
                fprintf(stderr, "set %d black obj->next->prev != obj\n", i);
                setPostFile(stderr);
                DumpBackTrace(mVMGlobals);
                dumpBadObject((PyrObject*)obj);
            }

            // scan for refs to white.
            if (!BlackToWhiteCheck((PyrObject*)obj))
                return false;

            obj = obj->next;
        }

        // scan white list
        obj = set->mWhite.next;
        while (obj != set->mFree) {
            if (obj->gc_color != mWhiteColor) {
                // debugf("set %d white list obj color wrong %d (%d, %d, %d) %p\n",
                //	i, obj->gc_color, mBlackColor, mGreyColor, mWhiteColor, obj);
                // debugf("hmmm free %p  black %p\n", set->mFree, set->black);
                fprintf(stderr, "set %d white list obj color wrong %d (%d, %d, %d) %p\n", i, obj->gc_color, mBlackColor,
                        mGreyColor, mWhiteColor, obj);
                fprintf(stderr, "hmmm free %p  black %p\n", set->mFree, &set->mBlack);
                setPostFile(stderr);
                DumpBackTrace(mVMGlobals);
                dumpBadObject((PyrObject*)obj);
                return false;
            }
            if (GetGCSet(obj) != set) {
                // debugf("set %d white obj gcset wrong %d %p\n", i, obj->obj_sizeclass, obj);
                fprintf(stderr, "set %d white obj gcset wrong %d %p\n", i, obj->obj_sizeclass, obj);
                setPostFile(stderr);
                DumpBackTrace(mVMGlobals);
                dumpBadObject((PyrObject*)obj);
                return false;
            }
            if (obj->next->prev != obj) {
                fprintf(stderr, "set %d white obj->next->prev != obj\n", i);
                setPostFile(stderr);
                DumpBackTrace(mVMGlobals);
                dumpBadObject((PyrObject*)obj);
            }
            obj = obj->next;
        }

        // mark all free list items free
        obj = set->mFree;
        while (!IsMarker(obj)) {
            /*if (obj->gc_color == mGreyColor) {
                //debugf("grey obj on free list\n");
                fprintf(stderr, "grey obj on free list\n");
                return false;
            }*/
            // post("FREE\n");
            // dumpObject((PyrObject*)(PyrObject*)obj);
            obj->gc_color = mFreeColor;
            if (GetGCSet(obj) != set) {
                // debugf("set %d free obj gcset wrong %d %p\n", i, obj->obj_sizeclass, obj);
                fprintf(stderr, "set %d free obj gcset wrong %d %p\n", i, obj->obj_sizeclass, obj);
                // dumpObject((PyrObject*)obj);
                return false;
            }
            if (obj->next->prev != obj) {
                fprintf(stderr, "set %d free obj->next->prev != obj\n", i);
                // dumpObject((PyrObject*)obj);
            }
            obj = obj->next;
        }
    }

    int numgrey = 0;
    PyrObjectHdr* grey = mGrey.next;
    while (!IsMarker(grey)) {
        numgrey++;
        if (!IsGrey(grey)) {
            fprintf(stderr, "sc Object on grey list not grey  %d %d   %d\n", grey->gc_color, mGreyColor, numgrey);
            fprintf(stderr, "%p <- %p -> %p grey %p process %p\n", mGrey.prev, &mGrey, mGrey.next, grey, mProcess);
            return false;
        }
        grey = grey->next;
    }

    if (numgrey != mNumGrey) {
        fprintf(stderr, "grey count off %d %d\n", numgrey, mNumGrey);
        DumpInfo();
        fprintf(stderr, ".");
        return false;
    }
    return true;
}

bool PyrGC::LinkSanity() {
    // postfl("PyrGC::LinkSanity\n");
    for (int i = 0; i < kNumGCSets; ++i) {
        GCSet* set = mSets + i;

        // scan entire loop
        PyrObjectHdr* obj = &set->mBlack;
        do {
            if (obj->next->prev != obj) {
                fprintf(stderr, "set %d black obj->next->prev != obj\n", i);
                // dumpObject((PyrObject*)obj);
                return false;
            }
            if (obj->prev->next != obj) {
                fprintf(stderr, "set %d black obj->prev->next != obj\n", i);
                // dumpObject((PyrObject*)obj);
                return false;
            }
            obj = obj->next;
        } while (obj != &set->mBlack);
    }
    return true;
}

#define DUMPINSANITY 1

bool PyrGC::BlackToWhiteCheck(PyrObject* objA) {
    if (objA->obj_format > obj_slot)
        return true;
    // scan it
    int size = objA->size;
    if (size > 0) {
        PyrSlot* slot = objA->slots;
        for (int j = size; j--; ++slot) {
            PyrObject* objB = NULL;
            if (IsObj(slot) && slotRawObject(slot)) {
                objB = slotRawObject(slot);
            }
            if (objB && (uintptr_t)objB < 100) {
                fprintf(stderr, "weird obj ptr\n");
                return false;
            }
            if (objB) {
                if (objA == mStack)
                    continue;

                if (objA->gc_color == mBlackColor && objA != mPartialScanObj) {
                    if (objB->gc_color == mWhiteColor) {
                        if (objA->classptr == class_frame) {
                            // jmc: black stack frames pointing to white nodes can be ignore
                            PyrFrame* frameA = (PyrFrame*)objA;
                            PyrMethod* meth = slotRawMethod(&frameA->method);
                            PyrMethodRaw* methraw = METHRAW(meth);
                            if (methraw->needsHeapContext)
                                continue;
                        }
#if DUMPINSANITY
                        fprintf(stderr, "black frame to white ref %p %p\n", objA, objB);
                        dumpBadObject(objA);
                        dumpBadObject(objB);
                        fprintf(stderr, "\n");
#endif
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool PyrGC::SanityMarkObj(PyrObject* objA, PyrObject* fromObj, int level) {
    if (objA->IsPermanent())
        return true;
    if (objA->IsMarked())
        return true;
    if (objA->size > MAXINDEXSIZE(objA)) {
        fprintf(stderr, "obj indexed size larger than max: %d > %ld\n", objA->size, MAXINDEXSIZE(objA));
        // dumpObject((PyrObject*)objA);
        return false;
    }

    objA->SetMark(); // mark it
    if (!BlackToWhiteCheck(objA))
        return false;

    if (objA->obj_format <= obj_slot) {
        // scan it
        int size = objA->size;
        if (size > 0) {
            PyrSlot* slot = objA->slots;
            for (int j = size; j--; ++slot) {
                PyrObject* objB = NULL;
                int tag = GetTag(slot);
                if (tag == tagObj && slotRawObject(slot))
                    objB = slotRawObject(slot);

                if (objB) {
                    /*
                    if (level > 40) {
                        fprintf(stderr, "40 levels deep!\n");
                        dumpBadObject(objA);
                        dumpBadObject(objB);
                        return false;
                    } */
                    bool err = SanityMarkObj(objB, objA, level + 1);
                    if (!err)
                        return false;
                }
            }
        }
    }
    return true;
}

bool PyrGC::SanityClearObj(PyrObject* objA, int level) {
    if (!(objA->IsMarked()))
        return true;
    if (objA->IsPermanent())
        return true;
    objA->ClearMark(); // unmark it

    if (objA->obj_format <= obj_slot) {
        // scan it
        int size = objA->size;
        if (size > 0) {
            PyrSlot* slot = objA->slots;
            for (int j = size; j--; ++slot) {
                PyrObject* objB = NULL;
                if (IsObj(slot) && slotRawObject(slot)) {
                    objB = slotRawObject(slot);
                }
                if (objB) {
                    /*if (level > 40) {
                        fprintf(stderr, "40 levels deep!\n");
                        dumpBadObject(objA);
                        //dumpObject((PyrObject*)objB);  //newPyrFrame
                        return errFailed;
                    }*/
                    bool err = SanityClearObj(objB, level + 1);
                    if (!err)
                        return false;
                }
            }
        }
    }
    return true;
}

void PyrGC::DumpInfo() {
    int i;
    PyrObjectHdr* obj;
    int numblack, numwhite, numfree, settotal, setsiztotal;
    int totblack, totgrey, totwhite, totfree, totref, total, siztotal;

    REPORTPAUSE
    post("flips %d  collects %d   nalloc %d   alloc %d   grey %d\n", mFlips, mCollects, mNumAllocs, mAllocTotal,
         mNumGrey);

    totblack = 0;
    totgrey = 0;
    totwhite = 0;
    totfree = 0;
    totref = 0;
    total = 0;
    siztotal = 0;
    for (i = 0; i < kNumGCSizeClasses; ++i) {
        GCSet* set = mSets + i;

        // scan black list
        numblack = 0;
        obj = set->mBlack.next;
        while (!IsMarker(obj)) {
            numblack++;
            obj = obj->next;
        }

        // scan white list
        numwhite = 0;
        obj = set->mWhite.next;
        while (obj != set->mFree) {
            numwhite++;
            obj = obj->next;
        }

        // scan free list
        numfree = 0;
        obj = set->mFree;
        while (!IsMarker(obj)) {
            numfree++;
            obj = obj->next;
        }
        settotal = numblack + numwhite + numfree;
        setsiztotal = settotal << (i + 3);
        siztotal += setsiztotal;
        totblack += numblack;
        totwhite += numwhite;
        totfree += numfree;
        total += settotal;
        if (settotal) {
            post("%2d  bwf t sz: %6d %6d %6d   %6d   %8d\n", i, numblack, numwhite, numfree, settotal, setsiztotal);
        }
    }
    post("tot bwf t sz: %6d %6d %6d   %6d   %8d\n", totblack, totwhite, totfree, total, siztotal);
}

void PyrGC::DumpGrey() {
    // scan grey list
    PyrObjectHdr* obj = mGrey.next;
    while (!IsMarker(obj)) {
        post("grey %s %d %d\n", slotRawSymbol(&obj->classptr->name)->name, obj->obj_sizeclass, obj->size);
        obj = obj->next;
    }
}

void PyrGC::DumpSet(int i) {
    GCSet* set = mSets + i;

    // scan black list
    PyrObjectHdr* obj = set->mBlack.next;
    while (!IsMarker(obj)) {
        post("black %s %d %d\n", slotRawSymbol(&obj->classptr->name)->name, obj->obj_sizeclass, obj->size);
        obj = obj->next;
    }

    // scan white list
    obj = set->mWhite.next;
    while (obj != set->mFree) {
        post("white %s %d %d\n", slotRawSymbol(&obj->classptr->name)->name, obj->obj_sizeclass, obj->size);
        obj = obj->next;
    }

    // scan free list
    obj = set->mFree;
    while (!IsMarker(obj)) {
        post("free %s %d %d\n", slotRawSymbol(&obj->classptr->name)->name, obj->obj_sizeclass, obj->size);
        obj = obj->next;
    }
}

void PyrGC::ClearMarks() {
    for (int i = 0; i < kNumGCSets; ++i) {
        GCSet* set = mSets + i;

        // scan black list
        PyrObjectHdr* obj = set->mBlack.next;
        while (!IsMarker(obj)) {
            obj->ClearMark(); // unmark it
            obj = obj->next;
        }

        // scan grey list
        obj = mGrey.next;
        while (!IsMarker(obj)) {
            obj->ClearMark(); // unmark it
            obj = obj->next;
        }

        // scan white list
        obj = set->mWhite.next;
        while (obj != set->mFree) {
            obj->ClearMark(); // unmark it
            obj = obj->next;
        }

        // scan free list
        obj = set->mFree;
        while (!IsMarker(obj)) {
            obj->ClearMark(); // unmark it
            obj = obj->next;
        }
    }
}

void PyrGC::throwMemfailed(size_t inNumBytes) {
    post("alloc failed. size = %d\n", inNumBytes);
    MEMFAILED;
}
