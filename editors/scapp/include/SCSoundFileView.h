/*
 *  SCSoundFileView.h
 *  xSC3lang
 *
 *  Created by falkenst on Thu Nov 18 2004.
 *  Copyright (c) 2004 jan truetzschler. All rights reserved.

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
 *
 */

//#import "DrawBackground.h"
//#include "PyrObject.h"
#include "SCView.h"
#import "SCTextView.h"
#import "SCVirtualMachine.h"

#ifndef NO_LIBSNDFILE
#include <sndfile.h>
#else
#include "SC_sndfile_stub.h"
#endif

//jan t
struct SCSoundFileSelection {
		bool startIsEditable;
		bool sizeIsEditable;
		int start;
		int size;
		SCColor color;
};
typedef struct SCSoundFileSelection SCSoundFileSelection;
//struct SndMinMax {
//		float min;
//		float max;
//};
//typedef struct SndMinMax SndMinMax;
struct SndMinMaxBuf {
		float *min;
		float *max;
		int samples;
		bool isUsable;
};
typedef struct SndMinMaxBuf SndMinMaxBuf;
const int kMaxSndSelections = 64;
const int kMaxSndChannels = 128;
class SCSoundFileView : public SCView
{
public:
	SCSoundFileView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCSoundFileView();

	virtual void draw(SCRect inDamage);
	virtual void draw0(SCRect inDamage, CGContextRef cgc);
	virtual void draw1(SCRect inDamage, CGContextRef cgc);
	virtual void draw2(SCRect inDamage, CGContextRef cgc);
	virtual void draw0Buf(SCRect inDamage, CGContextRef cgc);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	//virtual int readSndFile(const char * filename, int startFrame, int frames, int resampleFactor, bool closeFile);
	virtual int readSndFile(SNDFILE *file, int startFrame, int frames, int resampleFactor, SF_INFO info);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent);
	virtual void mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent);
//	virtual void mouseOver(SCPoint where);
	virtual int findSelection(int frame);
	virtual void setBounds(SCRect inbounds);

	SCPoint pixelToUnits(SCPoint p, SCRect r)
            {
                return SCMakePoint(
                    (p.x - r.x) * mZoom.x + mScroll.x,
                    (p.y - r.y) * mZoom.y + mScroll.y);
            }
	SCPoint unitsToPixel(SCPoint u, SCRect r)
            {
                return SCMakePoint(
                    (u.x - mScroll.x) * mInvZoom.x + r.x,
                    (u.y - mScroll.y) * mInvZoom.y + r.y);
            }

protected:

	int mBufNum;
	SNDFILE *mSndFile;
	SndMinMaxBuf mSndMinMax;
	SndBuf mSndBuf;
	SCPoint mZoom, mInvZoom, mScroll;
	int mStyle; // 0 = separate, 1 = overlay, 2 = x,y.
	SCColor mWaveColors[kMaxSndChannels];
	SCColor mGridColor;
	bool mGridOn;
	float mGridResolution;
 	int mGridOffset;
	SCPoint mAbsolutePosition;
	bool mIsReadingSoundFile;
	int mCurrentSelection;
	bool mMoveSelection;
	int mLastFrame;
	SCSoundFileSelection mSelections[kMaxSndSelections];
	bool mDrawsWaveForm;
	float mResampleFactor;
	int mElasticMode;
	bool mShowTimeCursor;
	int mTimeCursorPosition;
	SCColor mTimeCursorColor;
	int mFramesInBounds;
};


SCView* NewSCSoundFileView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
