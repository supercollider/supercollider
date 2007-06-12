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


#include "GC.h"
#include "PyrKernel.h"
#include "PyrObjectProto.h"
#include "InitAlloc.h"
#include <string.h>
#include <stdexcept>

#define SANITYCHECK 0

const int kScanThreshold =  400;
 
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

void fatalerror(char*str);
void fatalerror(char*str)
{
	fprintf(stderr, str);
	postfl(str);
	throw std::runtime_error(str);
	//exit(-1);
}

inline int ScanSize(PyrObject *obj) { return obj->obj_format <= obj_slot ? obj->size : 0; }

void GC::ScanSlots(PyrSlot *inSlots, long inNumToScan)
{
	PyrObject *obj;
	int32 rtagHFrame = tagHFrame; 
	int32 rtagObj = tagObj; 
	
	for (int i=0; i<inNumToScan; ++i) {
		PyrSlot *slot = inSlots + i;
		int32 tag = slot->utag;
		if (tag == rtagHFrame) {
			if (slot->uof) {
				obj = slot->uof->myself.uo;
				if (IsWhite(obj)) {
					ToGrey2(obj);
				}
			}
		} else if (tag == rtagObj && slot->uo) {
			obj = slot->uo;
			if (IsWhite(obj)) {
				ToGrey2(obj);
			}
		}
	}
}

void GCSet::Init(int inGCSet)
{
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
	mWhite.next  = &mBlack;
	
	mBlack.prev = &mWhite;
	mWhite.prev = &mBlack;
	
}

void GCSet::MoveWhiteToFree()
{
	// move all white items to beginning of free list
	mFree = mWhite.next;
	if (!IsMarker(mBlack.next)) {
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

PyrProcess* newPyrProcess(class GC *gc, PyrClass *procclassobj);

GC::GC(VMGlobals *g, AllocPool *inPool, PyrClass *mainProcessClass, long poolSize)
{
	mVMGlobals = g;
	mPool = inPool;
	//mCurSet = 0;
	mNumToScan = 0;
	
	mFlips = 0;
	mCollects = 0;
	mAllocTotal = 0;
	mNumAllocs = 0;
	mScans = 0;
	
	mGreyColor = 3<<2;
	mBlackColor = 2<<2;
	mWhiteColor = 1<<2;
	
	mProcessID = g->processID;
	mRunning = false;

	mCanSweep = false;
	mPartialScanObj = NULL;
	mPartialScanSet = NULL;
	mPartialScanSlot = 0;
	
	mGrey.classptr = NULL;
	mGrey.obj_sizeclass = 0;
	mGrey.size = 0;
	mGrey.gc_color = obj_gcmarker;
	
	mGrey.prev = &mGrey;
	mGrey.next = &mGrey;

	mNumGrey = 0;
	
	mNewPool.Init(mPool, poolSize, poolSize, 9000);
	
	// initialize treadmills
	for (int i=0; i<kNumGCSets; ++i) {
		mSets[i].Init(i);
	}
	
	mProcess = newPyrProcess(this, mainProcessClass);
	
	mStack = mProcess->mainThread.uot->stack.uo;
	ToBlack(mStack);
	SetNil(&mProcess->mainThread.uot->stack);
	
	mNumGrey = 0;
	ToGrey2(mProcess);
	g->sp = mStack->slots - 1;
	g->process = mProcess;
	mRunning = true;
	
	SanityCheck();
	//assert(SanityCheck());
	
}

PyrObject *GC::NewPermanent(size_t inNumBytes, long inFlags, long inFormat) 
{
	// obtain size info
	int32 alignedSize = (inNumBytes + kAlignMask) & ~kAlignMask; // 16 byte align
	int32 numSlots = alignedSize / sizeof(PyrSlot);
	numSlots = numSlots < 1 ? 1 : numSlots;
	int32 sizeclass = LOG2CEIL(numSlots);
	sizeclass = sc_min(sizeclass, kNumGCSizeClasses-1);
	
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

void GC::BecomePermanent(PyrObject *inObject)
{

	if (mGrey.next == inObject) {
		mGrey.next = inObject->next;
		mNumGrey--;
	} else {
		if (IsGrey(inObject)) mNumGrey--;
	}
	inObject->gc_color = obj_permanent;
	inObject->obj_flags |= obj_immutable;
	inObject->next->prev = inObject->prev;
	inObject->prev->next = inObject->next;
	inObject->next = inObject->prev = inObject;
}

void GC::BecomeImmutable(PyrObject *inObject)
{
	inObject->obj_flags |= obj_immutable;
}

//void DumpBackTrace(VMGlobals *g);

PyrObject *GC::New(size_t inNumBytes, long inFlags, long inFormat, bool inCollect) 
{
	PyrObject *obj = NULL;

	if (inFlags & obj_permanent) {
		return NewPermanent(inNumBytes, inFlags, inFormat);
	}
	
#if SANITYCHECK
	SanityCheck();
#endif

	// obtain size info

	int32 alignedSize = (inNumBytes + kAlignMask) & ~kAlignMask; // 16 byte align
	int32 numSlots = alignedSize / sizeof(PyrSlot);
	numSlots = numSlots < 1 ? 1 : numSlots;
	int32 sizeclass = LOG2CEIL(numSlots);
	sizeclass = sc_min(sizeclass, kNumGCSizeClasses-1);

	int32 credit = (1L << sizeclass) + 4;
	mNumToScan += credit;
	mAllocTotal += credit;
	mNumAllocs++;
	
	if (inCollect && mNumToScan > kScanThreshold) {
		Collect();
	}
	
	GCSet *gcs = mSets + sizeclass;
	for (int tries=0; tries<2; ++tries) {
		obj = (PyrObject*)gcs->mFree;
		if (!IsMarker(obj)) {
			// from free list
			gcs->mFree = gcs->mFree->next;
			break;
		} else if (tries == 0) {
			if (sizeclass > kMaxPoolSet) {
				SweepBigObjects();
				int32 allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
				obj = (PyrObject*)mPool->Alloc(allocSize);
			} else {
				int32 allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
				obj = (PyrObject*)mNewPool.Alloc(allocSize);
			}
			if (obj) {
				DLInsertAfter(&gcs->mWhite, obj);
				break;
			} else {
				FullCollection();
				// try again
			}
		} else {
			post("alloc failed. size = %d\n", inNumBytes);
			MEMFAILED; 
		}
	}
			
	obj->obj_sizeclass = sizeclass;
	obj->obj_format = inFormat;
	obj->obj_flags = inFlags & 255;
	obj->size = 0;
	obj->classptr = class_object;
	obj->gc_color = mWhiteColor;
	
#if SANITYCHECK
	SanityCheck();
#endif
	return obj;
}	

PyrObject *GC::NewFinalizer(ObjFuncPtr finalizeFunc, PyrObject *inObject, bool inCollect) 
{
	PyrObject *obj = NULL;
	
#if SANITYCHECK
	SanityCheck();
#endif

	// obtain size info

	int32 sizeclass = 1;

	int32 credit = (1L << sizeclass) + 4;
	mNumToScan += credit;
	mAllocTotal += credit;
	mNumAllocs++;
	
	if (inCollect && mNumToScan > kScanThreshold) {
		Collect();
	}
	
	GCSet *gcs = mSets + kFinalizerSet;
	for (int tries=0; tries<2; ++tries) {
		obj = (PyrObject*)gcs->mFree;
		if (!IsMarker(obj)) {
			// from free list
			gcs->mFree = gcs->mFree->next;
			break;
		} else if (tries == 0) {
			int32 allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
			obj = (PyrObject*)mNewPool.Alloc(allocSize);
			if (obj) {
				DLInsertAfter(&gcs->mWhite, obj);
				break;
			} else {
				FullCollection();
				// try again
			}
		} else {
			post("alloc failed for Finalizer\n");
			MEMFAILED; 
		}
	}
			
	obj->obj_sizeclass = sizeclass;
	obj->obj_format = obj_slot;
	obj->obj_flags = 0;
	obj->size = 2;
	obj->classptr = class_finalizer;
	obj->gc_color = mWhiteColor;
	
	SetPtr(obj->slots+0, finalizeFunc);
	SetObject(obj->slots+1, inObject);
		
#if SANITYCHECK
	SanityCheck();
#endif
	return obj;
}	


void GC::SweepBigObjects()
{	
	if (!mCanSweep) return;
	
	for (int i=kMaxPoolSet+1; i<kNumGCSizeClasses; ++i) {
		GCSet *gcs = mSets + i;
		PyrObjectHdr *obj = gcs->mFree;
		
		if (!IsMarker(obj)) {
		
			// unlink chain of free objects
			gcs->mFree = obj->prev->next = &gcs->mBlack;
			gcs->mBlack.prev = obj->prev;
			
			do {
				PyrObjectHdr *nextobj = obj->next;
				//void* ptr = (void*)RAWPART(obj);
				void* ptr = (void*)obj;
				mPool->Free(ptr);
				obj = nextobj;
			} while (!IsMarker(obj));
		}
	}
	mCanSweep = false;
}

void GC::DoPartialScan(int32 inObjSize)
{
	int32 remain = inObjSize - mPartialScanSlot;

	if (remain <= 0) {
		mPartialScanObj = NULL;
		mNumToScan -= 4; 
		if (mNumToScan<0) mNumToScan = 0;
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
	if (mNumToScan<0) mNumToScan = 0;
	//post("partial %5d xx %4d %2d %s\n", mScans, mNumToScan, mNumGrey);
	//post("partial %5d %2d %4d %2d %s\n", mScans, i, mNumToScan, mNumGrey, obj->classptr->name.us->name);
}

bool GC::ScanOneObj()
{
	// Find a set that has a grey object
	PyrObject* obj;
	obj = (PyrObject*)mGrey.next;
	if (IsMarker(obj)) {
		if (mNumGrey) fatalerror("grey count error\n");
		return false;
	}
	
	/*if (!IsGrey(obj)) {
		postfl("Object on grey list not grey  %d %d\n", obj->gc_color, mGreyColor);
		fatalerror("C1");
	}*/

	mScans++;
	
	//post("-> scan %d %d %d\n", mNumGrey, IsGrey(obj), mNumToScan);
	// Found a grey object
	// move obj from grey to black
	
	ToBlack(obj);

	int32 size = ScanSize(obj);
	//post("<- scan %d %d %d %d\n", mNumGrey, IsGrey(obj), mNumToScan, size);
	if (size > mNumToScan + 32) {
		mPartialScanObj = obj;
		mPartialScanSlot = 0;
		DoPartialScan(size);
	} else if (size > 0) {
		ScanSlots(obj->slots, size);
		mNumToScan -= (1L << obj->obj_sizeclass) + 4;
		if (mNumToScan<0) mNumToScan = 0;
	}
	return true;
}

void GC::ScanStack()
{
	// scan the stack
	PyrObject* obj = mStack;

	// put current frame on stack to have it scanned
	VMGlobals *g = mVMGlobals;
	if (g->frame) {
		g->sp++; SetFrame(g->sp, g->frame);
	}
	
	PyrSlot* slot = obj->slots;
	int32 size = obj->size = g->sp - slot + 1;

	ScanSlots(slot, size);
	if (g->frame) {
		g->sp--; // pop frame
	}
}

void GC::Flip()
{
	//post("GC::Flip %d\n", mProcessID);
		
#if SANITYCHECK
	SanityCheck();
#endif
	
	ScanFinalizers();

	GCSet *gcs = mSets;
	for (int i=0; i<kNumGCSets; ++i, ++gcs) {
		gcs->MoveWhiteToFree();
	}

	// advance colors
	mBlackColor += 4;
	mWhiteColor += 4;
	mGreyColor += 4;
	mFreeColor += 4;
					
	// move root to grey area
	mNumGrey = 0; 
	ToGrey2(mProcess);

	ToBlack(mStack);
	
	// reset counts
	mNumToScan = 0;
	mCanSweep = true;
		
	mFlips++;
	//post("flips %d  collects %d   nalloc %d   alloc %d   grey %d\n", mFlips, mCollects, mNumAllocs, mAllocTotal, mNumGrey);
	
#if SANITYCHECK
	SanityCheck();
#endif
}


void GC::FullCollection()
{
	Collect(100000000);	// collect space
	SweepBigObjects();
}

void GC::Collect(int32 inNumToScan) 
{
	mNumToScan = sc_max(mNumToScan, inNumToScan); 
	Collect();	// collect space
}

void GC::Collect() 
{
	bool stackScanned = false;
	mCollects++;
	
#if SANITYCHECK
	SanityCheck();
#endif

	if (mNumToScan > 0) {
		//post("->Collect pid %d  ns %d  ng %d  s %d\n", mProcessID, mNumToScan, mNumGrey, mScans);
		//DumpInfo();
		mNumToScan += mNumToScan >> 1;
		//post("->Collect2 pid %d  ns %d  ng %d  s %d\n", mProcessID, mNumToScan, mNumGrey, mScans);
		//mCurSet = 0;
		while (mNumToScan > 0) {
			while (mNumToScan > 0 && (mNumGrey > 0 || mPartialScanObj)) {
				if (mPartialScanObj) {
					DoPartialScan(ScanSize(mPartialScanObj));
				} else {
					if (!ScanOneObj()) break; 
				}
			}
			if (mNumGrey == 0 && mPartialScanObj == NULL) {
				if (!stackScanned) {
					stackScanned = true;
					ScanStack();
				}
				if (mNumGrey == 0 && mPartialScanObj == NULL && stackScanned) {
					Flip();
					break;
				}
			}
		}
		//post("<-Collect pid %d  ns %d  ng %d  s %d\n", mProcessID, mNumToScan, mNumGrey, mScans);
		//DumpInfo();
		//post("size9:\n");
		//TraceAnyPathToObjsOfSize(9);
		//post("greys:\n");
		//TraceAnyPathToAllGrey();
	}
	//post("mNumToScan %d\n", mNumToScan);
#if SANITYCHECK
	SanityCheck();
#endif
}



void GC::Finalize(PyrObject *finalizer)
{
	if (!IsPtr(finalizer->slots+0)) return;
	if (!IsObj(finalizer->slots+1)) return;
	
	ObjFuncPtr func = (ObjFuncPtr)finalizer->slots[0].ui;
	PyrObject *obj = finalizer->slots[1].uo;
	post("FINALIZE %s %08X\n", obj->classptr->name.us->name, obj);
	(func)(mVMGlobals, obj);
	
	SetNil(obj->slots+0);
	SetNil(obj->slots+1);
}

void GC::ScanFinalizers()
{
	GCSet *gcs = &mSets[kFinalizerSet];
	PyrObjectHdr *obj = gcs->mWhite.next;
	PyrObjectHdr *firstFreeObj = gcs->mFree;
	
	while (obj != firstFreeObj) {
		Finalize((PyrObject*)obj);
		obj = obj->next;
	}
}

//#define GCTHROW(str)
//#define GCTHROW(str) throw std::runtime_error(str)
#define GCTHROW(str) fatalerror(str)

bool GC::SanityCheck2()
{
	int numgrey = 0;
	PyrObjectHdr *grey = mGrey.next;
	while (!IsMarker(grey)) {
		numgrey++;
		if (!IsGrey(grey)) {
			postfl("sc Object on grey list not grey  %d %d   %d\n", grey->gc_color, mGreyColor, numgrey);
			return false;
		}
		grey = grey->next;
	}
	//postfl("sc %d %d\n", mNumGrey, numgrey);
	return mNumGrey == numgrey;
}

bool GC::SanityCheck()
{
	if (!mRunning) return true;
	
	
	//postfl("GC::SanityCheck\n");
	bool res = LinkSanity() && ListSanity();
	//&& SanityMarkObj((PyrObject*)mProcess,NULL,0) && SanityMarkObj(mStack,NULL,0) 
	//&& SanityClearObj((PyrObject*)mProcess,0) && SanityClearObj(mStack,0);
	//if (!res) DumpInfo();
	return res;
}

bool GC::ListSanity()
{
	bool found;
	char str[256];
	
	if (StackDepth() < 0) {
		sprintf(str, "stack underflow %ld\n", StackDepth());
		postfl(str);
		GCTHROW(str);
		return false;
	}

	//postfl("GC::ListSanity\n");
	for (int i=0; i<kNumGCSets; ++i) {
		PyrObjectHdr *obj;
		GCSet* set = mSets + i;
		
		// check black marker
		obj = &set->mBlack;
		if (!IsMarker(obj)) {
			//debugf("set %d black marker color wrong %d %0X\n", i, obj->gc_color, obj);
			sprintf(str, "set %d black marker color wrong %d %0X\n", i, obj->gc_color, obj);
			postfl(str);
			GCTHROW(str);
			//dumpObject((PyrObject*)obj);
			return false;
		}
		
		// check white marker
		obj = &set->mWhite;
		if (!IsMarker(obj)) {
			//debugf("set %d white marker color wrong %d %0X\n", i, obj->gc_color, obj);
			sprintf(str, "set %d white marker color wrong %d %0X\n", i, obj->gc_color, obj);
			postfl(str);
			GCTHROW(str);
			//dumpObject((PyrObject*)obj);
			return false;
		}
				
		// check free pointer between white and black marker
		if (set->mFree != &set->mBlack) {
			obj = set->mWhite.next;
			found = false;
			while (!IsMarker(obj)) {
				if (obj == set->mFree) { found = true; break; }
				obj = obj->next;
			}
			if (!found) {
				//debugf("set %d free pointer not between white and black\n", i);
				sprintf(str, "set %d free pointer not between white and black\n", i);
				postfl(str);
				GCTHROW(str);
				return false;
			}
		}
		
		// scan black list
		obj = set->mBlack.next;
		while (!IsMarker(obj)) {
			if (obj->gc_color != mBlackColor) {
				//debugf("set %d black list obj color wrong %d (%d, %d, %d) %0X\n", 
				//	i, obj->gc_color, mBlackColor, mGreyColor, mWhiteColor, obj);
				sprintf(str, "set %d black list obj color wrong %d (%d, %d, %d) %0X\n", 
					i, obj->gc_color, mBlackColor, mGreyColor, mWhiteColor, obj);
				postfl(str);
				dumpObject((PyrObject*)obj);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			if (GetGCSet(obj) != set) {
				//debugf("set %d black obj gcset wrong %d %0X\n", i, obj->obj_sizeclass, obj);
				sprintf(str, "set %d black obj gcset wrong %d %0X\n", i, obj->obj_sizeclass, obj);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			if (obj->next->prev != obj) {
				sprintf(str, "set %d black obj->next->prev != obj\n", i);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
			}
			obj = obj->next;
		}
				
		// scan white list
		obj = set->mWhite.next;
		while (obj != set->mFree) {
			if (obj->gc_color != mWhiteColor) {
				//debugf("set %d white list obj color wrong %d (%d, %d, %d) %0X\n", 
				//	i, obj->gc_color, mBlackColor, mGreyColor, mWhiteColor, obj);
				//debugf("hmmm free %0X  black %0X\n", set->mFree, set->black);
				sprintf(str, "set %d white list obj color wrong %d (%d, %d, %d) %0X\n", 
					i, obj->gc_color, mBlackColor, mGreyColor, mWhiteColor, obj);
				postfl("hmmm free %0X  black %0X\n", set->mFree, &set->mBlack);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			if (GetGCSet(obj) != set) {
				//debugf("set %d white obj gcset wrong %d %0X\n", i, obj->obj_sizeclass, obj);
				sprintf(str, "set %d white obj gcset wrong %d %0X\n", i, obj->obj_sizeclass, obj);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			if (obj->next->prev != obj) {
				sprintf(str, "set %d white obj->next->prev != obj\n", i);
				//dumpObject((PyrObject*)obj);
				postfl(str);
				GCTHROW(str);
			}
			obj = obj->next;
		}
		
		// mark all free list items free
		obj = set->mFree;
		while (!IsMarker(obj)) {
			/*if (obj->gc_color == mGreyColor) {
				//debugf("grey obj on free list\n");
				sprintf(str, "grey obj on free list\n");
				postfl(str);
				GCTHROW(str);
				return false;
			}*/
			//post("FREE\n");
			//dumpObject((PyrObject*)(PyrObject*)obj);
			obj->gc_color = mFreeColor;
			if (GetGCSet(obj) != set) {
				//debugf("set %d free obj gcset wrong %d %0X\n", i, obj->obj_sizeclass, obj);
				sprintf(str, "set %d free obj gcset wrong %d %0X\n", i, obj->obj_sizeclass, obj);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			if (obj->next->prev != obj) {
				sprintf(str, "set %d free obj->next->prev != obj\n", i);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
			}
			obj = obj->next;
		}
	}
	
	int numgrey = 0;
	PyrObjectHdr *grey = mGrey.next;
	while (!IsMarker(grey)) {
		numgrey++;
		if (!IsGrey(grey)) {
			sprintf(str, "sc Object on grey list not grey  %d %d   %d\n", grey->gc_color, mGreyColor, numgrey);
			postfl(str);
			postfl("%08X <- %08X -> %08X grey %08X process %08X\n", mGrey.prev, &mGrey, mGrey.next, grey, mProcess);
			GCTHROW(str);
			return false;
		}
		grey = grey->next;
	}
	
	if (numgrey != mNumGrey) {
		sprintf(str, "grey count off %d %d\n", numgrey, mNumGrey);
		postfl(str);
		GCTHROW(str);
		//gcDumpInfo(gc);
		postfl(".");
		return false;
	}
	return true;
}

bool GC::LinkSanity()
{
	char str[256];
	//postfl("GC::LinkSanity\n");
	for (int i=0; i<kNumGCSets; ++i) {
		GCSet* set = mSets + i;
		
		// scan entire loop
		PyrObjectHdr* obj = &set->mBlack;
		do {
			if (obj->next->prev != obj) {
				sprintf(str, "set %d black obj->next->prev != obj\n", i);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			if (obj->prev->next != obj) {
				sprintf(str, "set %d black obj->prev->next != obj\n", i);
				postfl(str);
				GCTHROW(str);
				//dumpObject((PyrObject*)obj);
				return false;
			}
			obj = obj->next;
		} while (obj != &set->mBlack);
	}
	return true;	
}

#define DUMPINSANITY 1

bool GC::SanityMarkObj(PyrObject *objA, PyrObject *fromObj, int level)
{
	int j, size, tag;
	PyrSlot *slot;
	PyrObject *objB;
	char str[256];
	
	if (objA->IsPermanent()) return true;
	if (objA->IsMarked()) return true;
	if (objA->size > MAXINDEXSIZE(objA)) {
		sprintf(str, "obj indexed size larger than max: %d > %d\n", objA->size, MAXINDEXSIZE(objA));
		postfl(str);
		GCTHROW(str);
		//dumpObject((PyrObject*)objA);
		return false;
	}
	objA->SetMark(); // mark it
	
	// scan it
	size = objA->size;
	if (size > 0) {
		slot = objA->slots;
		for (j=size; j--; ++slot) {
			objB = NULL;
			tag = slot->utag;
			if (tag == tagHFrame && slot->uof) {
				objB = slot->uof->myself.uo;
			} else if (tag == tagObj && slot->uo) {	
				objB = slot->uo;
			}
			if (objB && (long)objB < 100) {
				sprintf(str, "weird obj ptr\n");
				postfl(str);
				GCTHROW(str);
				return false;
			}
			if (objB) {
				if (objA == mStack) {
				} else if (objA->gc_color == mBlackColor && objA != mPartialScanObj) {
					if (objB->gc_color == mWhiteColor) { 
						
						//debugf("black to white ref %0X %0X\n", objA, objB);
						//debugf("sizeclass %d %d\n",  objA->obj_sizeclass, objB->obj_sizeclass);
						//debugf("class %s %s\n",  objA->classptr->name.us->name, objB->classptr->name.us->name);
						
						sprintf(str, "black to white ref %0X %0X\n", objA, objB);
						postfl(str);
						GCTHROW(str);
#if DUMPINSANITY
						dumpBadObject(objA);
						dumpBadObject(objB);
						postfl("\n");
#endif
						return false;
					}
				}
				/*if (level > 40) {
					postfl("40 levels deep!\n");
					dumpBadObject(objA);
					dumpBadObject(objB);
					return false;
				}*/
				bool err = SanityMarkObj(objB, objA, level + 1);
				if (!err) return false;
			}
		}
	}
	return true;
}

bool GC::SanityClearObj(PyrObject *objA, int level)
{
	int size;
	
	if (!(objA->IsMarked())) return true;
	if (objA->IsPermanent()) return true;
	objA->ClearMark(); // unmark it
	
	// scan it
	size = objA->size;
	if (size > 0) {
		PyrSlot *slot = objA->slots;
		for (int j=size; j--; ++slot) {
			PyrObject *objB = NULL;
			int tag = slot->utag;
			if (tag == tagHFrame && slot->uof) {
				objB = slot->uof->myself.uo;
			} else if (tag == tagObj && slot->uo) {	
				objB = slot->uo;
			}
			if (objB) {
				/*if (level > 40) {
					postfl("40 levels deep!\n");
					dumpBadObject(objA);
					//dumpObject((PyrObject*)objB);  //newPyrFrame
					return errFailed;
				}*/
				bool err = SanityClearObj(objB, level+1);
				if (!err) return false;
			}
		}
	}
	return true;
}

void GC::DumpInfo()
{
	int i;
	PyrObjectHdr *obj;
	int numblack, numgrey, numwhite, numfree, numref, settotal, setsiztotal;
	int totblack, totgrey, totwhite, totfree, totref, total, siztotal;
	
	post("flips %d  collects %d   nalloc %d   alloc %d   grey %d\n", mFlips, mCollects, mNumAllocs, mAllocTotal, mNumGrey);
	
	totblack = 0;
	totgrey = 0;
	totwhite = 0;
	totfree = 0;
	totref = 0;
	total = 0;
	siztotal = 0;
	for (i=0; i<kNumGCSizeClasses; ++i) {
		GCSet *set = mSets + i;
		
		// scan black list
		numblack = 0;
		obj = set->mBlack.next;
		while (!IsMarker(obj)) {
			numblack++;
			obj = obj->next;
		}
		
		// scan grey list
		numgrey = 0;
		while (!IsMarker(obj)) {
			numgrey++;
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
		numref = 0;
		obj = set->mFree;
		while (!IsMarker(obj)) {
			numfree++;
			obj = obj->next;
		}
		settotal = numblack + numgrey + numwhite + numfree;
		setsiztotal = settotal << (i + 3);
		siztotal += setsiztotal;
		totblack += numblack;
		totgrey += numgrey;
		totwhite += numwhite;
		totfree += numfree;
		totref += numref;
		total += settotal;
		if (settotal) {
#if SETGREY
			post("%2d  bgwfr t sz: %3d %3d %3d %3d  %3d   %3d   %8d   (%3d)\n", i, 
				numblack, numgrey, numwhite, numfree, numref, settotal, setsiztotal, set->numGrey);
#else
			post("%2d  bgwfr t sz: %3d %3d %3d %3d  %3d   %3d   %8d\n", i, 
				numblack, numgrey, numwhite, numfree, numref, settotal, setsiztotal);
#endif
			//flushPostBuf();
		}
	}
	post("tot bgwfr t sz: %3d %3d %3d %3d  %3d   %3d   %8d\n", 
		totblack, totgrey, totwhite, totfree, totref, total, siztotal);
	//flushPostBuf();
}

void GC::DumpEverything()
{
	for (int i=0; i<kNumGCSizeClasses; ++i) {
		GCSet *set = mSets + i;
		
		// scan black list
		PyrObjectHdr *obj = set->mBlack.next;
		while (!IsMarker(obj)) {
			post("black %08X %s %d %d\n", obj, obj->classptr->name.us->name, obj->obj_sizeclass, obj->size);
			obj = obj->next;
		}
		
		// scan grey list
		obj = mGrey.next;
		while (!IsMarker(obj)) {
			post("grey %08X %s %d %d\n", obj, obj->classptr->name.us->name, obj->obj_sizeclass, obj->size);
			obj = obj->next;
		}
		
		// scan white list
		obj = set->mWhite.next;
		while (obj != set->mFree) {
			post("white %08X %s %d %d\n", obj, obj->classptr->name.us->name, obj->obj_sizeclass, obj->size);
			obj = obj->next;
		}
		
		// scan free list
		obj = set->mFree;
		while (!IsMarker(obj)) {
			post("free %08X %s %d %d\n", obj, obj->classptr->name.us->name, obj->obj_sizeclass, obj->size);
			obj = obj->next;
		}
	}
	//flushPostBuf();
}

void GC::ClearMarks()
{
	for (int i=0; i<kNumGCSets; ++i) {
		GCSet *set = mSets + i;
		
		// scan black list
		PyrObjectHdr *obj = set->mBlack.next;
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
	//flushPostBuf();
}


void DumpBackTrace(VMGlobals *g);

void GC::ToGrey(PyrObjectHdr* obj)
{	
	/* move obj from white to grey */
	/* link around object */
	DLRemove(obj);		
									
	/* link in new place */			
	DLInsertBefore(&mGrey, obj);				
									
	/* set grey list pointer to obj */
	obj->gc_color = mGreyColor;	
	mNumGrey ++ ;	
	mNumToScan += (1L << obj->obj_sizeclass) + 4;
}

void GC::ToGrey2(PyrObjectHdr* obj)
{	
	/* move obj from white to grey */
	/* link around object */
	DLRemove(obj);		
									
	/* link in new place */			
	DLInsertBefore(&mGrey, obj);				
									
	/* set grey list pointer to obj */
	obj->gc_color = mGreyColor;	
	mNumGrey ++ ;	
}


