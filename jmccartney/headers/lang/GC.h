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

The garbage collector for SuperCollider.
Based on Wilson and Johnstone's real time collector and the Baker treadmill.

*/

#ifndef _GC_
#define _GC_

#include "PyrObject.h"
#include "VMGlobals.h"
#include "AdvancingAllocPool.h"
#if GCDEBUG
#include <vector>
#endif

void DumpSimpleBackTrace(VMGlobals *g);

const int kMaxPoolSet = 7;
const int kNumGCSizeClasses = 28;
const int kFinalizerSet = kNumGCSizeClasses;
const int kNumGCSets = kNumGCSizeClasses + 1;

class GCSet
{
public:
	GCSet() {}
	void Init(int inSizeClass);
	
	bool HasFree() { return mFree != &mBlack; }
	
private:
	friend class GC;
	
	void MoveWhiteToFree();
	
	PyrObjectHdr mBlack;
	PyrObjectHdr mWhite;
	PyrObjectHdr* mFree;
};

struct SlotRef {
	SlotRef(PyrObject* inObj, int32 inIndex) : obj(inObj), slotIndex(inIndex) {}
	
	PyrObject *obj;
	int32 slotIndex;
};

class GC
{
public:
	GC(VMGlobals *g, AllocPool *inPool, PyrClass *mainProcessClass, long poolSize);
	
	PyrObject* New(size_t inNumBytes, long inFlags, long inFormat, bool inCollect);
						
	static PyrObject* NewPermanent(size_t inNumBytes, 
						long inFlags, long inFormat);

	PyrObject* NewFinalizer(ObjFuncPtr finalizeFunc, PyrObject *inObject, bool inCollect);
	
	int32 ProcessID() { return mProcessID; }
		
#if 0	
// Codewarrior is not inlining these.. why?
	bool IsBlack(PyrObjectHdr* inObj) { return inObj->gc_color == mBlackColor; }
	bool IsWhite(PyrObjectHdr* inObj) { return inObj->gc_color == mWhiteColor; }
	bool IsGrey(PyrObjectHdr* inObj) { return inObj->gc_color == mGreyColor; }
	bool IsMarker(PyrObjectHdr* inObj) { return inObj->gc_color == obj_gcmarker; }
#else 

#define IsBlack(inObj) ((inObj)->gc_color == mBlackColor)
#define IsWhite(inObj) ((inObj)->gc_color == mWhiteColor)
#define IsGrey(inObj) ((inObj)->gc_color == mGreyColor)
#define IsFree(inObj) (!(IsMarker(inObj) || inObj->IsPermanent() || \
					IsBlack(inObj) || IsWhite(inObj) || IsGrey(inObj)))
#define IsMarker(inObj) ((inObj)->gc_color == obj_gcmarker)
#endif

	bool ObjIsBlack(PyrObjectHdr* inObj) { return IsBlack(inObj); }
	bool ObjIsGrey(PyrObjectHdr* inObj) { return IsGrey(inObj); }
	bool ObjIsFree(PyrObjectHdr* inObj) { return IsFree(inObj); }


	// general purpose write barriers:
	void GCWrite(PyrObjectHdr* inParent, PyrSlot* inSlot) 
		{ 
			if (IsBlack(inParent) && IsObj(inSlot) && IsWhite(inSlot->uo)) {
				ToGrey(inSlot->uo);
			}
			assert(SanityCheck2());
		}
	void GCWrite(PyrObjectHdr* inParent, PyrObjectHdr* inChild) 
		{ 
			if (IsBlack(inParent) && IsWhite(inChild)) {
				ToGrey(inChild);
			}
			assert(SanityCheck2());
		}
	// when you know the parent is black:
	void GCWriteBlack(PyrSlot* inSlot) 
		{ 
			if (IsObj(inSlot)) {
				if (IsWhite(inSlot->uo)) {
					ToGrey(inSlot->uo);
				}
			}
			assert(SanityCheck());
		}
	void GCWriteBlack(PyrObjectHdr* inChild) 
		{ 
			if (IsWhite(inChild)) {
				ToGrey(inChild);
			}
			assert(SanityCheck());
		}
	// when you know the child is white
	void GCWriteNew(PyrObjectHdr* inParent, PyrObjectHdr* inChild) 
		{ 
			if (IsBlack(inParent)) {
				ToGrey(inChild);
			}
			assert(SanityCheck());
		}

// users should not call anything below.
				
	void Collect();
	void Collect(int32 inNumToScan);
	void FullCollection();
	void ScanFinalizers();
	GCSet* GetGCSet(PyrObjectHdr* inObj);
		
	void ToGrey(PyrObjectHdr* inObj);
	void ToGrey2(PyrObjectHdr* inObj);
	void ToBlack(PyrObjectHdr* inObj);
	void ToWhite(PyrObjectHdr *obj);

	int32 StackDepth() { return mVMGlobals->sp - mStack->slots + 1; }
	PyrObject* Stack() { return mStack; }
	void SetStack(PyrObject* inStack) { mStack = inStack; }

	bool SanityCheck();
	bool SanityCheck2();
	bool LinkSanity();
	bool ListSanity();
	bool SanityMarkObj(PyrObject *objA, PyrObject *fromObj, int level);
	bool SanityClearObj(PyrObject *objA, int level);
	void DumpInfo();
	void DumpEverything();
	
	void BecomePermanent(PyrObject *inObject);
	void BecomeImmutable(PyrObject *inObject);
	
private:
	void Free(PyrObject* inObj);
	void ScanSlots(PyrSlot *inSlots, long inNumToScan);
	void SweepBigObjects();
	void DoPartialScan(int32 inObjSize);
	bool ScanOneObj();
	void Flip();
	void ScanStack();
	void DLRemove(PyrObjectHdr *obj);
	void DLInsertAfter(PyrObjectHdr *after, PyrObjectHdr *obj);
	void DLInsertBefore(PyrObjectHdr *before, PyrObjectHdr *obj);

	void ClearMarks();
	void Finalize(PyrObject *obj);
	
	VMGlobals *mVMGlobals;
	AllocPool *mPool;
	AdvancingAllocPool mNewPool;
	GCSet mSets[kNumGCSets]; 
	PyrProcess *mProcess; // the root is the pyrprocess which contains this struct
	PyrObject *mStack;
	PyrObject *mPartialScanObj;
	PyrObjectHdr mGrey;

	GCSet *mPartialScanSet;
	int32 mPartialScanSlot;
	int32 mNumToScan;
	int32 mNumGrey;
	int32 mCurSet;
	
	int32 mFlips, mCollects, mAllocTotal, mScans, mNumAllocs;
	
	unsigned char mBlackColor, mGreyColor, mWhiteColor, mFreeColor;
	int8 mProcessID;
	bool mCanSweep;
	bool mRunning;
};

inline void GC::DLRemove(PyrObjectHdr *obj) 
{
	obj->next->prev = obj->prev;
	obj->prev->next = obj->next;
}

inline void GC::DLInsertAfter(PyrObjectHdr *after, PyrObjectHdr *obj) 
{
	obj->next = after->next;
	obj->prev = after;
	after->next->prev = obj;
	after->next = obj;
}

inline void GC::DLInsertBefore(PyrObjectHdr *before, PyrObjectHdr *obj) 
{
	obj->prev = before->prev;
	obj->next = before;
	before->prev->next = obj;
	before->prev = obj;
}

inline GCSet* GC::GetGCSet(PyrObjectHdr* inObj) 
{
	return mSets + (inObj->classptr == class_finalizer ? kFinalizerSet : inObj->obj_sizeclass);
}

inline void GC::ToBlack(PyrObjectHdr *obj) 
{		
	if (IsGrey(obj)) {
		mNumGrey--;
		//post("ToBlack %d\n", mNumGrey);
	}

	DLRemove(obj);		
	
	GCSet *gcs = GetGCSet(obj);
	DLInsertAfter(&gcs->mBlack, obj);

	obj->gc_color = mBlackColor;
}

inline void GC::ToWhite(PyrObjectHdr *obj) 
{	
	if (IsGrey(obj)) {
		mNumGrey--;
		//post("ToWhite %d\n", mNumGrey);
	}

	DLRemove(obj);	
		
	GCSet *gcs = GetGCSet(obj);
	DLInsertAfter(&gcs->mWhite, obj);

	obj->gc_color = mWhiteColor;
}

#endif
