/*
 *  SCSoundFileView.cpp
 *  xSC3lang
 *
 *  Created by falkenst on Thu Nov 18 2004.
 *  Copyright (c) 2004 jan truetzschler. All rights reserved.
 *
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

#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include <pthread.h>
#include "SCBase.h"
#include "PyrSymbol.h"
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include "SC_BoundsMacros.h"
#include "SC_InlineBinaryOp.h"


#include "SCSoundFileView.h"

int slotColorVal(PyrSlot *slot, SCColor *sccolor);
int setSlotColor(PyrSlot *slot, SCColor *sccolor);
CGRect SCtoCGRect(SCRect screct);
extern PyrSymbol* s_x;
extern PyrSymbol* s_y;
void QDDrawBevelRect(CGContextRef cgc, CGRect bounds, float width, bool inout);
int allocSlotStrVal(PyrSlot *slot, char **str);
int slotGetSCRect(PyrSlot* a, SCRect *r);



// replacement for calloc.
// calloc lazily zeroes memory on first touch. This is good for most purposes, but bad for realtime audio.
void *zalloc(size_t n, size_t size)
{
	size *= n;
	if (size) {
		void* ptr = malloc(size);
		if (ptr) {
			memset(ptr, 0, size);
			return ptr;
		}
	}
	return 0;
}

inline int32 BUFMASK(int32 x)
{
	return (1 << (31 - CLZ(x))) - 1;
}

int bufAlloc(SndBuf* buf, int numChannels, int numFrames, double sampleRate);
int bufAlloc(SndBuf* buf, int numChannels, int numFrames, double sampleRate)
{
	long numSamples = numFrames * numChannels;
	if(numSamples < 1) return errFailed;
	if(buf->data) free(buf->data);
	buf->data = (float*)zalloc(numSamples, sizeof(float));
	if (!buf->data) return errFailed;

	buf->channels = numChannels;
	buf->frames   = numFrames;
	buf->samples  = numSamples;
	buf->mask     = BUFMASK(numSamples); // for delay lines
	buf->mask1    = buf->mask - 1;	// for oscillators
	buf->samplerate = sampleRate;
	buf->sampledur = 1. / sampleRate;

	return errNone;
}

int bufAllocMinMax(SndMinMaxBuf* buf, int numChannels, int numFrames);
int bufAllocMinMax(SndMinMaxBuf* buf, int numChannels, int numFrames)
{
	long numSamples = numFrames * numChannels;
	if(numSamples < 1) return errFailed;
	buf->isUsable = false;
	if(buf->min) free(buf->min);
	if(buf->max) free(buf->max);
	buf->min = (float*)zalloc(numSamples, sizeof(float));
	buf->max = (float*)zalloc(numSamples, sizeof(float));
	if (!buf->min || !buf->max) return errFailed;
	buf->samples = numSamples;

	return errNone;
}

SCView* NewSCSoundFileView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCSoundFileView(inParent, inObj, inBounds);
}

SCSoundFileView::SCSoundFileView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mBufNum(0), mStyle(0), mGridOn(true), mGridResolution(1.f),  mGridOffset(0), mIsReadingSoundFile(false),
	mCurrentSelection(0), mMoveSelection(false), mLastFrame(0), mDrawsWaveForm(true), mShowTimeCursor (false), mFramesInBounds(0)
{
	mZoom = mInvZoom = SCMakePoint(1.,1.);
	mScroll = SCMakePoint(0.,0.);
	memset(&mSndFile, 0, sizeof(SndBuf));
	memset(&mSndMinMax, 0, sizeof(SndMinMaxBuf));
	mSndMinMax.isUsable = false;
	mSndBuf.frames = 0;
	mSndBuf.data = 0;
	mSndBuf.channels = 0;
	mSndBuf.samplerate = 44100;
	mResampleFactor = 64;
	mGridColor = SCMakeColor(0.2,0.2,1.0, 1.0);
    mBackground = new SolidColorBackground(
                    SCMakeColor(0.,0.,0., 1.0));
	SCColor waveColor = SCMakeColor(1.0,1.0,0.0, 1.0);
	mTimeCursorPosition = 0;
	mTimeCursorColor = SCMakeColor(0.1,0.2,1.0, 1.0);
	mElasticMode = 0;

	for (int i=0; i<kMaxSndSelections; ++i)
	{
		mSelections[i].startIsEditable = true;
		mSelections[i].sizeIsEditable = true;
		mSelections[i].size = 0;
		mSelections[i].start = 0;
		mSelections[i].color = SCMakeColor(0.2,0.2,1.0, 0.4);
	}
	for (int i=0; i<kMaxSndChannels; ++i) mWaveColors[i] = waveColor;
}

SCSoundFileView::~SCSoundFileView()
{
	free(mSndBuf.data);
	free(mSndMinMax.min);
	free(mSndMinMax.max);
}



int SCSoundFileView::readSndFile(SNDFILE *file, int startFrame, int frames, int resampleFactor, SF_INFO info)
{

	if (!file || mIsReadingSoundFile) return errFailed;
	mIsReadingSoundFile = true;

	//check for
	startFrame = sc_min(startFrame, info.frames - 1);
	int maxframes = info.frames - startFrame;
	if(!frames || frames > maxframes) frames = maxframes;
	//would be good to find a way to calculate an optimal factor ...
	if(!resampleFactor) resampleFactor=64;
	mResampleFactor = (float) resampleFactor;
	mResampleFactor = sc_clip(mResampleFactor, 1.f, maxframes);
	mScroll.x = 0.;
	int readsize = (int) mResampleFactor;

//	post("zoom.x: %f, frames: %d, resampleFactor: %f \n", mZoom.x, (int) frames, mResampleFactor);

	int err = bufAlloc(&mSndBuf, info.channels, (int) (frames/mResampleFactor), info.samplerate);
	mZoom.x =  ((frames/mResampleFactor)/(mBounds.width-2.f));

	if(err) return err;
	int channels = sc_min(mSndBuf.channels, kMaxSndChannels);

	if(mSndMinMax.isUsable) {
		mSndMinMax.isUsable=false;
		free(mSndMinMax.min);
		free(mSndMinMax.max);
	}
	bufAllocMinMax(&mSndMinMax, channels, (int)(mBounds.width ));


	if(startFrame>0) sf_seek(file, startFrame, SEEK_SET);
	if(mResampleFactor>1){

		float data[readsize*channels];
		int count=0;
		for(int i=0; i<frames; i+=readsize){
			sf_read_float(file, data, readsize*channels);
			// find minmax: ...
			for (int j=0; j<channels; ++j)
			{
				float ymin, ymax, val;
				val =  data[j];
				ymin = ymax = val;
				//read through all samples
				for(int k=0; k<(readsize*channels); k+=channels)
				{
					val = data[k+j];
					if(val<ymin) ymin = val;
					else if(val>ymax) ymax = val;
				}

				if(sc_abs(ymin) > sc_abs(ymax)) mSndBuf.data[count++] = ymin;
				else  mSndBuf.data[count++] = ymax;
			}
		}

	} else {
		sf_read_float(file, mSndBuf.data, frames*channels);
	}
	mIsReadingSoundFile = false;
	return errNone;
}

int SCSoundFileView::findSelection(int frame)
{
	for (int i=0; i<kMaxSndSelections; ++i)
	{
		if(frame >= mSelections[i].start && frame < mSelections[i].start+mSelections[i].size)
		return i;
	}
	return -1;
}

void SCSoundFileView::setBounds(SCRect inBounds)
{
	if(inBounds.width != mBounds.width){
		if(mElasticMode) mZoom.x =  mSndBuf.frames/(inBounds.width-2.f);
 		mSndMinMax.isUsable = false;
		if(mSndBuf.channels)
		bufAllocMinMax(&mSndMinMax, mSndBuf.channels, (int)(inBounds.width ));
	}
    mBounds = inBounds;
	if(!(mParent->isSubViewScroller())){
		SCRect pbounds = mParent->getLayout().bounds;
		mLayout.bounds.x = mBounds.x + pbounds.x;
		mLayout.bounds.y = mBounds.y + pbounds.y;
		mLayout.bounds.width = mBounds.width;
		mLayout.bounds.height = mBounds.height;
	} else {
		mLayout.bounds = mBounds;
	}
}

void SCSoundFileView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	//mPrevPoint = where;
	//mouseTrack(where, modifiers,theEvent);
	int frame;
	[[NSCursor resizeLeftCursor] set];
	SCRect bounds = getDrawBounds();
	frame =  (int) (mScroll.x + ((where.x - bounds.x - 1) * mZoom.x * mResampleFactor ));
	//post("current frame is %d, bounds.x is %d\n", frame, mBounds.x);
	if (modifiers & NSAlternateKeyMask) {
//		refresh();
	} else {
	//check if there is a selection:
		int selection = findSelection(frame);
		if(selection >= 0){
			mMoveSelection = true;
			mCurrentSelection = selection;
			[[NSCursor openHandCursor] set];

		} else {
			mMoveSelection = false;
			if(mSelections[mCurrentSelection].startIsEditable) mSelections[mCurrentSelection].start = frame;
		}

		mTimeCursorPosition = frame;
		mLastFrame = frame;
	}
}

void SCSoundFileView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	SCRect bounds = getDrawBounds();
	mAbsolutePosition.x = (int) where.x; //absolute mouse position
	mAbsolutePosition.y = (int) where.y;
	int frame;
		frame = (int)  (mScroll.x + (where.x - bounds.x - 1) * mZoom.x * mResampleFactor );
	if (modifiers & NSAlternateKeyMask) {
		SCSoundFileSelection * currentSelection = &mSelections[mCurrentSelection];
		int endOfSelection = currentSelection->start + currentSelection->size;
			if(currentSelection->sizeIsEditable && frame > endOfSelection)
				currentSelection->size = frame - currentSelection->start;
			if(currentSelection->sizeIsEditable && currentSelection->startIsEditable && frame < endOfSelection)
			{
				currentSelection->start = frame;
				currentSelection->size = endOfSelection - currentSelection->start;
			}
		refresh();
	} else
	if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else if (modifiers & NSShiftKeyMask) {
		sendMessage(getsym("doAction"), 0, 0, 0);
    } else if (modifiers & NSControlKeyMask) {
        sendMessage(getsym("doMetaAction"), 0, 0, 0);
    } else {

	//	post("current frame is %d, bounds.x is %d, where is %d \n", frame, mBounds.x, where.x);
		if(mMoveSelection)
		{
			int moveframes = frame - mLastFrame;
			mLastFrame = frame;
	//		post(" frame:%i, moveframes:%i, mLastFrame:%i \n", frame, moveframes, mLastFrame);
			if(mSelections[mCurrentSelection].startIsEditable) mSelections[mCurrentSelection].start = mSelections[mCurrentSelection].start + moveframes;
		} else {

			[[NSCursor resizeRightCursor] set];
			//[[NSCursor currentCursor] pop];
			if(frame < mSelections[mCurrentSelection].start)
			{
				int endframe;
				endframe = mSelections[mCurrentSelection].start;
				if(mSelections[mCurrentSelection].startIsEditable) mSelections[mCurrentSelection].start = frame;
				if(mSelections[mCurrentSelection].sizeIsEditable) mSelections[mCurrentSelection].size = endframe - mSelections[mCurrentSelection].start;
	//			post("current frame is %d, mSelectionStart is %d, where is %f \n", frame, mSelections[mCurrentSelection].start, where.x);
			} else if(mSelections[mCurrentSelection].sizeIsEditable) mSelections[mCurrentSelection].size = frame - mSelections[mCurrentSelection].start;
		}
		sendMessage(getsym("doAction"), 0, 0, 0);
		refresh();
	//	post("size: %d \n", mSelections[mCurrentSelection].size);
	}
}

void SCSoundFileView::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{

//	mSelections[mCurrentSelection].start = sc_clip(mSelections[mCurrentSelection].start, 0, mSndBuf.frames*mResampleFactor);
//	mSelections[mCurrentSelection].size = sc_clip(mSelections[mCurrentSelection].size, 0,
//													(mSndBuf.frames*mResampleFactor) - mSelections[mCurrentSelection].start);
//mMoveSelection = false;
    mouseTrack(where, modifiers,theEvent);
    sendMessage(getsym("mouseEndTrack"), 0, 0, 0);

	[[NSCursor arrowCursor] set];

}

void SCSoundFileView::draw0(SCRect inDamage, CGContextRef cgc)
{
	float *data = mSndBuf.data;
	if (!data || mIsReadingSoundFile || mScroll.x >= (mSndBuf.frames * mResampleFactor)) return;
	int samples = mSndBuf.samples;
	// draw scope.
	SCRect bounds = getDrawBounds();
    CGRect rect = SCtoCGRect(bounds);
	int width = (int) mBounds.width - 2;
	int channels = sc_min(mSndBuf.channels, kMaxSndChannels);
	float chanHeight = (mBounds.height - 2.) / channels;
	float chanHeight2 = 0.5 * chanHeight;
	float yScale = mZoom.y * chanHeight2;
	//iterate over channels:
	float xscroll = mScroll.x/mResampleFactor;
	for (int j=0; j<channels; ++j)
	{
		CGContextSetRGBFillColor(cgc, mWaveColors[j].red, mWaveColors[j].green, mWaveColors[j].blue, mWaveColors[j].alpha);
		float fframe = xscroll;
		float hzoom = mZoom.x;
		int iframe = (int)floor(fframe);
		int isample = iframe * channels;
		float val = -data[isample + j];
		int msample = 0;

		CGRect chanRect;
		chanRect.origin.x = rect.origin.x + 1.;
		chanRect.origin.y = rect.origin.y + 1. + chanHeight * j + chanHeight2;
		chanRect.size.width = width;
		chanRect.size.height = chanHeight;
		CGRect viewRect = CGRectMake(bounds.x + 1.f, (bounds.y + 1.f) + (j * chanHeight), bounds.width - 2.f, chanHeight);
		//iterate over frames:
		for (int i = 0; i < width && isample < samples; i++)
		{
			float ymin, ymax;
			ymin = ymax = val;
			float nextfframe = fframe + hzoom;
			int nextiframe = (int)floor(nextfframe);
			int nscan = nextiframe - iframe;
			//nscan = nscan * channels;
			for (int k=0; k<nscan && isample < samples; ++k)
			{
				val = -data[isample + j];
				if (val < ymin) ymin = val;
				else if (val > ymax) ymax = val;
				isample += channels;

			}
			iframe = nextiframe;
			fframe = nextfframe;

			if(mSndMinMax.min && mSndMinMax.max && j+msample < mSndMinMax.samples){
				mSndMinMax.min[j+msample] = ymin;
				mSndMinMax.max[j+msample] = ymax;
				msample += channels;
			}

			CGRect wrect;
			wrect.origin.x = rect.origin.x + 1. + i;
			wrect.size.width = 1.;
			wrect.origin.y = chanRect.origin.y + ymin * yScale;
			wrect.size.height = (ymax - ymin) * yScale + 1.;

//			post("%g %g    %d   %g %g\n", ymin, ymax, isample, wrect.origin.x, wrect.size.height);

			wrect = CGRectIntersection(wrect, viewRect);
			if ( !CGRectIsNull(wrect) ) {
			    CGContextFillRect(cgc, wrect);
			}
		}
		mFramesInBounds = msample;
	}
	if(mSndMinMax.min && mSndMinMax.max) mSndMinMax.isUsable = true;
	else mSndMinMax.isUsable = false;
}

// this is a bit optimized version of draw0, when dragging selections the min max needn't be sorted out
void SCSoundFileView::draw0Buf(SCRect inDamage, CGContextRef cgc)
{
	if (!mSndMinMax.isUsable) return;
	// draw scope.
//	post("draw0Buf. \n");
	SCRect bounds = getDrawBounds();
    CGRect rect = SCtoCGRect(bounds);
	int width = (int) bounds.width - 2;
	int channels = sc_min(mSndBuf.channels, kMaxSndChannels);
	int widthdisp;
	int startx = 0;
	if(inDamage.x > 0)
		widthdisp = (int) inDamage.width;
	else {
		startx = sc_abs(startx);
		widthdisp = (int) bounds.width -2;
	}
	int numsamples = (int) (mFramesInBounds *mSndBuf.channels);
	float chanHeight = (bounds.height - 2.) / channels;
	float chanHeight2 = 0.5 * chanHeight;
	float yScale = mZoom.y * chanHeight2;
	//iterate over channels:
	for (int j=0; j<channels; ++j)
	{
		CGContextSetRGBFillColor(cgc, mWaveColors[j].red, mWaveColors[j].green, mWaveColors[j].blue, mWaveColors[j].alpha);
//		float fframe = mScroll.x;
		int msample=startx*channels;

		CGRect viewRect, chanRect;
		viewRect = CGRectMake(bounds.x + 1.f, (bounds.y + 1.f) + (j * chanHeight), bounds.width - 2.f, chanHeight);
		chanRect.origin.x = rect.origin.x + 1.;
		chanRect.origin.y = rect.origin.y + 1. + chanHeight * j + chanHeight2;
		chanRect.size.width = width;
		chanRect.size.height = chanHeight;
		for (int i = 0; i < widthdisp && msample<numsamples; i++)
		//no x-zoom, offset?
		{
			float ymin, ymax;
			ymin = mSndMinMax.min[j+msample];
			ymax = mSndMinMax.max[j+msample];
			msample += channels;

			CGRect wrect;
			wrect.origin.x = rect.origin.x + 1. + i;
			wrect.size.width = 1.;
			wrect.origin.y = chanRect.origin.y + ymin * yScale;
			wrect.size.height = (ymax - ymin) * yScale + 1.;

//			post("%g %g    %d   %g %g\n", ymin, ymax, isample, wrect.origin.x, wrect.size.height);

			wrect = CGRectIntersection(wrect, viewRect);
			if ( !CGRectIsNull(wrect) ) {
			    CGContextFillRect(cgc, wrect);
			}
		}
//				post("msample %d\n", msample);

	}
}

void SCSoundFileView::draw1(SCRect inDamage, CGContextRef cgc)
{
	float *data = mSndBuf.data;
	if (!data) return;

	int samples = mSndBuf.samples;
	SCRect bounds = getDrawBounds();
	// draw scope.
    CGRect rect = SCtoCGRect(bounds);
	int width = (int) bounds.width - 2;
	int channels = sc_min(mSndBuf.channels, kMaxSndChannels);
	//post("channels %d\n", channels);
	float chanHeight = bounds.height - 2.;
	float chanHeight2 = 0.5 * chanHeight;
	float yScale = mZoom.y * chanHeight2;

	for (int j=0; j < channels; ++j)
	{
		CGContextSetRGBFillColor(cgc, mWaveColors[j].red, mWaveColors[j].green, mWaveColors[j].blue, mWaveColors[j].alpha);
		float fframe = mScroll.x;
		float hzoom = mZoom.x;
		int iframe = (int)floor(fframe);
		int isample = iframe * channels;
		float val = -data[isample + j];

		CGRect chanRect;
		chanRect.origin.x = rect.origin.x + 1.;
		chanRect.origin.y = rect.origin.y + 1. + chanHeight2;
		chanRect.size.width = rect.size.width - 2.;
		chanRect.size.height = chanHeight;

	//post("width %d\n", width);
		for (int i = 0; i < width && isample < samples; i++)
		{
			float ymin, ymax;
			ymin = ymax = val;
			float nextfframe = fframe + hzoom;
			int nextiframe = (int)floor(nextfframe);
			int nscan = nextiframe - iframe;
			for (int k=0; k<nscan; ++k)
			{
				val = -data[isample + j];
				if (val < ymin) ymin = val;
				else if (val > ymax) ymax = val;
				isample += channels;
			}
			iframe = nextiframe;
			fframe = nextfframe;

			CGRect wrect;
			wrect.origin.x = rect.origin.x + 1. + i;
			wrect.size.width = 1.;
			wrect.origin.y = chanRect.origin.y + ymin * yScale;
			wrect.size.height = (ymax - ymin) * yScale + 1.;

			//if (i == 64) post("%g %g    %g   %g %g\n", ymin, ymin, wrect.origin.x, wrect.origin.y, wrect.size.height);

			CGContextFillRect(cgc, wrect);
		}
	}
}

void SCSoundFileView::draw2(SCRect inDamage, CGContextRef cgc)
{
	float *data = mSndBuf.data;
	if (!data) return;

	int samples = mSndBuf.samples;
	SCRect bounds = getDrawBounds();
	// draw scope.
    CGRect rect = SCtoCGRect(bounds);
	int channels = sc_min(mSndBuf.channels, kMaxSndChannels);
	float height = rect.size.height - 2.;
	float height2 = 0.5 * height;
	float width = rect.size.width - 2.;
	float width2 = 0.5 * width;
	float yScale = mZoom.y * height2;
	float xScale = mZoom.x * width2;
	float xoff = rect.origin.x + width2 + 1.;
	float yoff = rect.origin.y + height2 + 1.;


	for (int k=0, j=0; j<channels; k++, j+=2)
	{
		CGContextSetRGBStrokeColor(cgc, mWaveColors[k].red, mWaveColors[k].green, mWaveColors[k].blue, mWaveColors[k].alpha);
		float x = xoff + data[j] * xScale;
		float y = yoff - data[j+1] * yScale;
		CGContextMoveToPoint(cgc, x, y);

		for (int i=channels; i<samples; i+=channels) {
			x = xoff + data[i+j] * xScale;
			y = yoff - data[i+j+1] * yScale;
			CGContextAddLineToPoint(cgc, x, y);
		}
		CGContextStrokePath(cgc);
	}
}

void SCSoundFileView::draw(SCRect inDamage)
{
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
	SCRect bounds = getDrawBounds();
    CGRect rect = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);

	float rzreciprocal = 1.f/ ( mResampleFactor * mZoom.x);
//	mStartFrameResampled = (int)(mScroll.x / mResampleFactor);
	CGRect viewRect, drawRect;
	viewRect = CGRectMake(bounds.x + 1.f, bounds.y + 1.f, bounds.width - 2.f, bounds.height - 2.f);
	for (int i=0; i<kMaxSndSelections; ++i)
	{
		if ( mSelections[i].size > 0 ) {
			float selectionstart = ((mSelections[i].start-mScroll.x) * rzreciprocal ) + 1.f + bounds.x;
			float selectionsize =  mSelections[i].size * rzreciprocal;
			drawRect = CGRectMake(selectionstart,
									bounds.y + 1.f,
									selectionsize,
									bounds.height - 2.f);
			drawRect = CGRectIntersection(drawRect, viewRect);
			if ( !CGRectIsNull(drawRect) ) {
				CGContextSetRGBFillColor(cgc, mSelections[i].color.red, mSelections[i].color.green, mSelections[i].color.blue, mSelections[i].color.alpha);
				CGContextFillRect (cgc, drawRect);
			}
		}
	}

	if(!mDrawsWaveForm) goto restore;
	// draw grid.
	if(mGridOn && (mGridResolution > 0.0)){
		float gridsize =  ((mGridResolution * mSndBuf.samplerate) / ( mResampleFactor * mZoom.x));
		CGContextSetRGBFillColor(cgc, mGridColor.red, mGridColor.green, mGridColor.blue, mGridColor.alpha);
		//float xgridpos =  mBounds.x + 1.;
		float xgridpos = bounds.x + 1. - ((mScroll.x - mGridOffset) * rzreciprocal);
		int maxwidth = (int) (bounds.width + bounds.x) - 2;
		for (int i=0; xgridpos<maxwidth; ++i)
		{
			if (xgridpos > bounds.x) {
				drawRect = CGRectMake(xgridpos,
										bounds.y + 1.f,
										1.f,
										bounds.height-2.f);
				CGContextFillRect (cgc, drawRect);
			}
			xgridpos += gridsize;

		}
	}
	switch (mStyle) {
		case 0 : if(mSndMinMax.isUsable && !(mTop->isScroller())) draw0Buf(inDamage, cgc); else draw0(inDamage, cgc);  break;
		case 1 : draw1(inDamage, cgc); break;
		case 2 : draw2(inDamage, cgc); break;
	}


	if(mShowTimeCursor){
		// mTimeCursorPosition is in frames
		float xcursorpos = ((mTimeCursorPosition - mScroll.x)  * rzreciprocal) + bounds.x - 1.f;
		if ((xcursorpos > bounds.x) && (xcursorpos < bounds.x + bounds.width)) {
			CGContextSetRGBFillColor(cgc, mTimeCursorColor.red, mTimeCursorColor.green, mTimeCursorColor.blue, mTimeCursorColor.alpha);

				drawRect = CGRectMake(xcursorpos,
										bounds.y + 1.f,
										1.5f,
										bounds.height-2.f);
				CGContextFillRect (cgc, drawRect);
		}
//			post("xcursorpos %f, resampleFactor %f, mTimeCursorPosition %i \n", xcursorpos, mResampleFactor, mTimeCursorPosition);
	}

	restore:
    CGContextRestoreGState(cgc);
}


int SCSoundFileView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;

	if (symbol == s_x) {
		double x;
		err = slotDoubleVal(slot, &x);
		if (err) return err;
		mScroll.x = sc_clip(x, 0.0, mSndBuf.frames * mResampleFactor) ;
//		mStartFrame = (int)(mScroll.x * mResampleFactor);
		mSndMinMax.isUsable = false;		//this is a straight forward solution, might be better to work this out in drawBuf
		return errNone;
	}
	if (symbol == s_y) {
		double y;
		err = slotDoubleVal(slot, &y);
		if (err) return err;
		mScroll.y = y;
		return errNone;
	}

	char *name = symbol->name;
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		if(screct.width != mBounds.width){
			if(mElasticMode) mZoom.x =  mSndBuf.frames/(screct.width-2.f);
			mSndMinMax.isUsable = false;
			bufAllocMinMax(&mSndMinMax, mSndBuf.channels, (int)(screct.width ));
		}
		setBounds(screct);

		//pass on to SCView ...
	}

	if (strcmp(name,"elasticResizeMode")==0){
        slotIntVal(slot, &mElasticMode);
        refresh();
		return errNone;

	}
	if (strcmp(name, "xZoom")==0) {
		double x;
		err = slotDoubleVal(slot, &x);
		if (err) return err;
		mZoom.x = x;
		mInvZoom.x = 1./x;
		mSndMinMax.isUsable = false;
		return errNone;
	}
	if (strcmp(name, "yZoom")==0) {
		double y;
		err = slotDoubleVal(slot, &y);
		if (err) return err;
		mZoom.y = y;
		mInvZoom.y = 1./y;
		refresh();
		return errNone;
	}

	if (strcmp(name, "gridResolution")==0) {
		err = slotFloatVal(slot, &mGridResolution);
		if (err) return err;
		refresh();
		return errNone;
	}

	if (strcmp(name, "gridOffset")==0) {
 		int gridOffset;
 		err = slotIntVal(slot, &gridOffset);
 		if (err) return err;
 		mGridOffset = gridOffset;
  		refresh();
  		return errNone;
	}

	if (strcmp(name, "gridColor")==0) {
		err = slotColorVal(slot, &mGridColor);
		if (err) return err;
		refresh();
		return errNone;
	}

	if (strcmp(name, "gridOn")==0) {
		mGridOn = IsTrue(slot);
		refresh();
		return errNone;
	}

	if (strcmp(name, "selectionColor")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		int index;
		err = slotIntVal(slots+0, &index);
		if (err) return err;
		err = slotColorVal(slots+1, &(mSelections+index)->color);
		if (err) return err;
		refresh();
		return errNone;

	}
	if (strcmp(name, "waveColors")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		for (int i=0; i<slotRawObject(slot)->size; ++i)
		{
			err = slotColorVal(slots+i, mWaveColors+i);
			if (err) return err;
		}
		refresh();
		return errNone;
	}
	if (strcmp(name, "style")==0) {
		err = slotIntVal(slot, &mStyle);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "drawsWaveForm")==0) {
		mDrawsWaveForm = IsTrue(slot);
		refresh();
		return errNone;
	}
	if(strcmp(name, "readSndFile")==0){
		int resample, startframe, frames, numframes;
		//float samplerate;
		SF_INFO info;
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		SNDFILE * file = (SNDFILE*) slotRawPtr(slotRawObject(slots+0)->slots);
		err = slotIntVal(slots+1, &startframe);
		if (err) return err;
		err = slotIntVal(slots+2, &frames);
		if (err) return err;
		err = slotIntVal(slots+3, &resample);
		if (err) return err;
		err = slotIntVal(slots+4, &info.samplerate);
		if (err) return err;
		err = slotIntVal(slots+5, &numframes);
		if (err) return err;
		info.frames = (sf_count_t) numframes;
		err = slotIntVal(slots+6, &info.channels);
		if (err) return err;
		mSndMinMax.isUsable = false;
		err = readSndFile(file, startframe, frames, resample, info);
		if (err) return err;
		//refresh();
		return errNone;
	}
	if (strcmp(name, "currentSelection")==0) {
		err = slotIntVal(slot, &mCurrentSelection);
		if (err) return err;
		return errNone;
	}
	if (strcmp(name, "selectionStart")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		int index;
		err = slotIntVal(slots+0, &index);
		if (err) return err;
		err = slotIntVal(slots+1, &(mSelections+index)->start);
		if (err) return err;
		refresh();
		return errNone;
	}

	if (strcmp(name, "selectionSize")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		int index;
		err = slotIntVal(slots+0, &index);
		if (err) return err;
		err = slotIntVal(slots+1, &(mSelections+index)->size);
		if (err) return err;
		refresh();
		return errNone;
	}

	if (strcmp(name, "selectionStartIsEditable")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		int index;
		err = slotIntVal(slots+0, &index);
		if (err) return err;
		(mSelections+index)->startIsEditable = IsTrue(slots+1);
		return errNone;
	}

	if (strcmp(name, "selectionSizeIsEditable")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		int index;
		err = slotIntVal(slots+0, &index);
		if (err) return err;
		(mSelections+index)->sizeIsEditable = IsTrue(slots+1);
		return errNone;
	}

	if (strcmp(name, "setViewData")==0) {
		int startFrame;
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		err = slotFloatVal(slots+1, &mResampleFactor);
		if(err) return err;
		err = slotIntVal(slots+2, &startFrame);
		if(err) return err;
		int channels, samplerate;
		err = slotIntVal(slots+3, &channels);
		if(err) return err;
		err = slotIntVal(slots+4, &samplerate);
		if(err) return err;
		if (!isKindOfSlot(slots+0, class_array)) return errWrongType;
		int size = slotRawObject(slots+0)->size;
		mScroll.x = (float) startFrame;
		if(size ==  mSndBuf.samples && mSndBuf.data && mSndBuf.channels==channels && mSndBuf.samplerate == samplerate){
			PyrDoubleArray * arr;
			arr = (PyrDoubleArray*) slotRawObject(slots+0);
			for(int i=0; i<size; i++){
			   mSndBuf.data[i] = arr->d[i];
			}
		} else {
			int frames = (int) ceil(size/channels);
			int err = bufAlloc(&mSndBuf, channels, frames, samplerate);
			if(err) return err;
			PyrDoubleArray * arr;
//			printf("frames:%i, size:%i, mSndBuf.samples:%i\n",frames, size, mSndBuf.samples);
			arr = (PyrDoubleArray*) slotRawObject(slots+0);
			size = mSndBuf.samples;
			for(int i=0; i<size; i++){
			   mSndBuf.data[i] = arr->d[i];
			}
		}
		if(mElasticMode){
//			mZoom.x =  ((mSndBuf.frames/mResampleFactor)/(mBounds.width-2.f));
			mZoom.x =  mSndBuf.frames/(mBounds.width-2.f);

		}
		mSndMinMax.isUsable = false;
		err = bufAllocMinMax(&mSndMinMax, mSndBuf.channels, (int)(mBounds.width ));
		refresh();
		return errNone;
	}
	if (strcmp(name, "showTimeCursor")==0) {
		mShowTimeCursor = IsTrue(slot);
		refresh();
		return errNone;
	}
	if (strcmp(name, "timeCursorPosition")==0) {
		err = slotIntVal(slot, &mTimeCursorPosition);
		if(err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "timeCursorColor")==0) {
		err = slotColorVal(slot, &mTimeCursorColor);
		if (err) return err;
		refresh();
		return errNone;
	}
	return SCView::setProperty(symbol, slot);
}

int SCSoundFileView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_x) {
		SetFloat(slot, mScroll.x);
		return errNone;
	}
	if (symbol == s_y) {
		SetFloat(slot, mScroll.y);
		return errNone;
	}

	char *name = symbol->name;
	if (strcmp(name, "xZoom")==0) {
		SetFloat(slot, mZoom.x);
		return errNone;
	}
	if (strcmp(name, "yZoom")==0) {
		SetFloat(slot, mZoom.y);
		return errNone;
	}
//	if (strcmp(name, "bufnum")==0) {
//		SetInt(slot, mBufNum);
//		return errNone;
//	}
	if (strcmp(name, "gridColor")==0) {
		return setSlotColor(slot, &mGridColor);
	}
	if (strcmp(name, "waveColors")==0) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		for (int i=0; i<slotRawObject(slot)->size; ++i)
		{
			int err = setSlotColor(slots+i, mWaveColors+i);
			if (err) return err;
		}
		return errNone;
	}
//	if (strcmp(name, "selections")==0)
//	{
//		if (!isKindOfSlot(slot, class_array)) return errWrongType;
//            int size = (slotRawObject(slot)->slots[0]).uo->size;
//			size = sc_clip(size, 0, kMaxSndSelections);
//            PyrDoubleArray * startarr;
//            PyrDoubleArray * rangearr;
//            startarr = ((PyrDoubleArray*)(slotRawObject(slot)->slots[0].uo));
//            rangearr = ((PyrDoubleArray*)(slotRawObject(slot)->slots[1].uo));
//
//            for(int i=0; i<size; i++){
//                startarr->d[i] = mSelections[i].start;
//                rangearr->d[i] = mSelections[i].size;
//            }
//            return errNone;
//	}

	if (strcmp(name, "selections")==0)
	{
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		int size = slotRawObject(slot)->size;
		size = sc_clip(size, 0, kMaxSndSelections);
		PyrObject *array =  slotRawObject(slot);

		for(int i=0; i<size; i++){
			PyrSlot  *slots= slotRawObject(array->slots+i)->slots;
			SetInt(slots+0, mSelections[i].start);
			SetInt(slots+1, mSelections[i].size);
		}
		return errNone;
	}

	if (strcmp(name, "absoluteX")==0) {
		SetInt(slot, (int) mAbsolutePosition.x);
		return errNone;
	}
	if (strcmp(name, "absoluteY")==0) {
		SetInt(slot, (int) mAbsolutePosition.y);
		return errNone;
	}
	if (strcmp(name, "currentSelection")==0) {
		SetInt(slot, (int) mCurrentSelection);
		return errNone;
	}
	if (strcmp(name, "selectionStart")==0) {
		int index;
		int err = slotIntVal(slot, &index);
		if (err) return err;
		SetInt(slot, (int) mSelections[index].start);
		return errNone;
	}

	if (strcmp(name, "selectionSize")==0) {
		int index;
		int err = slotIntVal(slot, &index);
		if (err) return err;
		SetInt(slot, (int) mSelections[index].size);
		return errNone;
	}

	if (strcmp(name, "selectionStartTime")==0) {
		int index;
		int err = slotIntVal(slot, &index);
		if (err) return err;
		SetFloat(slot, (float) mSelections[index].start/mSndBuf.samplerate);
		return errNone;
	}

	if (strcmp(name, "selectionDuration")==0) {
		int index;
		int err = slotIntVal(slot, &index);
		if (err) return err;
		SetFloat(slot, (float) mSelections[index].size/mSndBuf.samplerate);
		return errNone;
	}

	if (strcmp(name, "getViewData")==0) {

		if (!isKindOfSlot(slot, class_array)) return errWrongType;
            int size = slotRawObject(slot)->size;
			size = sc_clip(size, 0, mSndBuf.samples);
            PyrDoubleArray * arr;
            arr = (PyrDoubleArray*) slotRawObject(slot);
            for(int i=0; i<size; i++){
                arr->d[i] = mSndBuf.data[i];
            }
            return errNone;
	}
	if (strcmp(name, "getViewNumSamples")==0) {

		SetInt(slot, (int) mSndBuf.samples);
		return errNone;
	}
	if (strcmp(name, "timeCursorPosition")==0) {

		SetInt(slot, mTimeCursorPosition);
		return errNone;
	}
	if (strcmp(name, "gridOn")==0) {

		SetBool(slot, mGridOn);
		return errNone;
	}
	if (strcmp(name, "gridResolution")==0) {

		SetFloat(slot, mGridResolution);
		return errNone;
	}
	if (strcmp(name, "timeCursorOn")==0) {

		SetBool(slot, mShowTimeCursor);
		return errNone;
	}
	if (strcmp(name, "timeCursorColor")==0) {

		return setSlotColor(slot, &mTimeCursorColor);
	}
	return SCView::getProperty(symbol, slot);
}
