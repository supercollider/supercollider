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

The garbage collector for SuperCollider.
Based on Wilson and Johnstone's real time collector and the Baker treadmill.

*/

#ifndef _GC_
#define _GC_

#include "PyrObject.h"
#include "VMGlobals.h"
#include "AdvancingAllocPool.h"
#include "function_attributes.h"

void DumpSimpleBackTrace(VMGlobals *g);

const int kMaxPoolSet = 7;
const int kNumGCSizeClasses = 28;
const int kFinalizerSet = kNumGCSizeClasses;
const int kNumGCSets = kNumGCSizeClasses + 1;
const int kScanThreshold =  256;


class GCSet
{
public:
	GCSet() {}
	void Init(int inSizeClass);

	bool HasFree() { return mFree != &mBlack; }

private:
	friend class PyrGC;

	void MajorFlip();
	void MinorFlip();

	PyrObjectHdr mBlack;
	PyrObjectHdr mWhite;
	PyrObjectHdr* mFree;
};

struct SlotRef {
	SlotRef(PyrObject* inObj, int32 inIndex) : obj(inObj), slotIndex(inIndex) {}

	PyrObject *obj;
	int32 slotIndex;
};

class PyrGC
{
	static const int kLazyCollectThreshold = 1024;

public:
	PyrGC(VMGlobals *g, AllocPool *inPool, PyrClass *mainProcessClass, long poolSize);

	MALLOC PyrObject* New(size_t inNumBytes, long inFlags, long inFormat, bool inCollect);
	MALLOC PyrObject* NewFrame(size_t inNumBytes, long inFlags, long inFormat, bool inAccount);

	MALLOC static PyrObject* NewPermanent(size_t inNumBytes,
						long inFlags, long inFormat);

	MALLOC PyrObject* NewFinalizer(ObjFuncPtr finalizeFunc, PyrObject *inObject, bool inCollect);

	bool IsBlack(PyrObjectHdr* inObj)  { return inObj->gc_color == mBlackColor; }
	bool IsWhite(PyrObjectHdr* inObj)  { return inObj->gc_color == mWhiteColor; }
	bool IsGrey(PyrObjectHdr* inObj)   { return inObj->gc_color == mGreyColor; }
	static bool IsMarker(PyrObjectHdr* inObj) { return inObj->gc_color == obj_gcmarker; }
	bool IsFree(PyrObjectHdr* inObj)   { return (!(IsMarker(inObj) ||
													inObj->IsPermanent() ||
													IsBlack(inObj) ||
													IsWhite(inObj) ||
													IsGrey(inObj))); }

	bool ObjIsBlack(PyrObjectHdr* inObj) { return IsBlack(inObj); }
	bool ObjIsGrey(PyrObjectHdr* inObj) { return IsGrey(inObj); }
	bool ObjIsFree(PyrObjectHdr* inObj) { return IsFree(inObj); }


	// general purpose write barriers:
	void GCWrite(PyrObjectHdr* inParent, PyrSlot* inSlot)
		{
			if (IsBlack(inParent) && IsObj(inSlot) && IsWhite(slotRawObject(inSlot))) {
				ToGrey(slotRawObject(inSlot));
			}
		}
	void GCWrite(PyrObjectHdr* inParent, PyrObjectHdr* inChild)
		{
			if (IsBlack(inParent) && IsWhite(inChild)) {
				ToGrey(inChild);
			}
		}
	// when you know the parent is black:
	void GCWriteBlack(PyrSlot* inSlot)
		{
			if (IsObj(inSlot)) {
				if (IsWhite(slotRawObject(inSlot))) {
					ToGrey(slotRawObject(inSlot));
				}
			}
		}
	void GCWriteBlack(PyrObjectHdr* inChild)
		{
			if (IsWhite(inChild)) {
				ToGrey(inChild);
			}
		}
	// when you know the child is white
	void GCWriteNew(PyrObjectHdr* inParent, PyrObjectHdr* inChild)
		{
			assert(IsWhite(inChild));
			if (IsBlack(inParent)) {
				ToGrey(inChild);
			}
		}

// users should not call anything below.

	void Collect();
	void Collect(int32 inNumToScan);
	void LazyCollect()
	{
		if (mUncollectedAllocations > kLazyCollectThreshold)
			Collect();
	}
	void FullCollection();
	void ScanFinalizers();
	void RunAllFinalizers();
	GCSet* GetGCSet(PyrObjectHdr* inObj);
	void CompletePartialScan(PyrObject *obj);

	inline void ToGrey(PyrObjectHdr* inObj);
	inline void ToGrey2(PyrObjectHdr* inObj);
	void ToBlack(PyrObjectHdr* inObj);
	void ToWhite(PyrObjectHdr *inObj);
	void Free(PyrObjectHdr* inObj);


	long StackDepth() { return mVMGlobals->sp - mStack->slots + 1; }
	PyrObject* Stack() { return mStack; }
	void SetStack(PyrObject* inStack) { mStack = inStack; }

	bool SanityCheck();
	bool SanityCheck2();
	bool LinkSanity();
	bool ListSanity();
	bool BlackToWhiteCheck(PyrObject *objA);
	bool SanityMarkObj(PyrObject *objA, PyrObject *fromObj, int level);
	bool SanityClearObj(PyrObject *objA, int level);
	void DumpInfo();
	void DumpGrey();
	void DumpSet(int set);

	void BecomePermanent(PyrObject *inObject);
	void BecomeImmutable(PyrObject *inObject);

	bool IsPartialScanObject(PyrObject* inObject) const { return inObject == mPartialScanObj; }
	int32 GetPartialScanIndex() const { return mPartialScanSlot; }

private:
	inline PyrObject * Allocate(size_t inNumBytes, int32 sizeclass, bool inCollect);
	static void throwMemfailed(size_t inNumBytes);

	void ScanSlots(PyrSlot *inSlots, long inNumToScan);
	void SweepBigObjects();
	void DoPartialScan(int32 inObjSize);
	bool ScanOneObj();
	void Flip();
	void ScanStack();
	void ScanFrames();
	void DLRemove(PyrObjectHdr *obj);
	void DLInsertAfter(PyrObjectHdr *after, PyrObjectHdr *obj);
	void DLInsertBefore(PyrObjectHdr *before, PyrObjectHdr *obj);

	void ClearMarks();
	void Finalize(PyrObject *obj);

	void beginPause();
	void endPause();
	void reportPause();

	VMGlobals *mVMGlobals;
	AllocPool *mPool;
	AdvancingAllocPool mNewPool;
	GCSet mSets[kNumGCSets];
	PyrProcess *mProcess; // the root is the pyrprocess which contains this struct
	PyrObject *mStack;
	PyrObject *mPartialScanObj;
	PyrObjectHdr mGrey;

	int32 mPartialScanSlot;
	int32 mNumToScan;
	int32 mNumGrey;

	int32 mFlips, mCollects, mAllocTotal, mScans, mNumAllocs, mStackScans, mNumPartialScans, mSlotsScanned, mUncollectedAllocations;

	unsigned char mBlackColor, mGreyColor, mWhiteColor, mFreeColor;
	bool mCanSweep;
	bool mRunning;
};

inline void PyrGC::DLRemove(PyrObjectHdr *obj)
{
	obj->next->prev = obj->prev;
	obj->prev->next = obj->next;
}

inline void PyrGC::DLInsertAfter(PyrObjectHdr *after, PyrObjectHdr *obj)
{
	obj->next = after->next;
	obj->prev = after;
	after->next->prev = obj;
	after->next = obj;
}

inline void PyrGC::DLInsertBefore(PyrObjectHdr *before, PyrObjectHdr *obj)
{
	obj->prev = before->prev;
	obj->next = before;
	before->prev->next = obj;
	before->prev = obj;
}

inline GCSet* PyrGC::GetGCSet(PyrObjectHdr* inObj)
{
	return mSets + (inObj->classptr == class_finalizer ? kFinalizerSet : inObj->obj_sizeclass);
}

inline void PyrGC::ToBlack(PyrObjectHdr *obj)
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

inline void PyrGC::ToWhite(PyrObjectHdr *obj)
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

inline void PyrGC::Free(PyrObjectHdr* obj)
{
	if (IsGrey(obj)) {
		mNumGrey--;
		//post("ToWhite %d\n", mNumGrey);
	}

	DLRemove(obj);

	GCSet *gcs = GetGCSet(obj);
	DLInsertBefore(gcs->mFree, obj);
	gcs->mFree = obj;

	obj->gc_color = mFreeColor;
	obj->size = 0;
}

inline void PyrGC::ToGrey(PyrObjectHdr* obj)
{
	/* move obj from white to grey */
	/* link around object */
	DLRemove(obj);

	/* link in new place */
	DLInsertAfter(&mGrey, obj);

	/* set grey list pointer to obj */
	obj->gc_color = mGreyColor;
	mNumGrey ++ ;
	mNumToScan += 1L << obj->obj_sizeclass;
}

inline void PyrGC::ToGrey2(PyrObjectHdr* obj)
{
	/* move obj from white to grey */
	/* link around object */
	DLRemove(obj);

	/* link in new place */
	DLInsertAfter(&mGrey, obj);

	/* set grey list pointer to obj */
	obj->gc_color = mGreyColor;
	mNumGrey ++ ;
}

inline PyrObject * PyrGC::Allocate(size_t inNumBytes, int32 sizeclass, bool inRunCollection)
{
	if (inRunCollection && mNumToScan >= kScanThreshold)
		Collect();
	else {
		if (inRunCollection)
			mUncollectedAllocations = 0;
		else
			++mUncollectedAllocations;
	}

	GCSet *gcs = mSets + sizeclass;

	PyrObject * obj = (PyrObject*)gcs->mFree;
	if (!IsMarker(obj)) {
		// from free list
		gcs->mFree = obj->next;
		assert(obj->obj_sizeclass == sizeclass);
	} else {
		if (sizeclass > kMaxPoolSet) {
			SweepBigObjects();
			size_t allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
			obj = (PyrObject*)mPool->Alloc(allocSize);
		} else {
			size_t allocSize = sizeof(PyrObjectHdr) + (sizeof(PyrSlot) << sizeclass);
			obj = (PyrObject*)mNewPool.Alloc(allocSize);
		}
		if (!obj)
			throwMemfailed(inNumBytes);
		DLInsertAfter(&gcs->mWhite, obj);
		obj->obj_sizeclass = sizeclass;
	}
	return obj;
}

#endif
