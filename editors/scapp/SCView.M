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
#include "SCGraphView.h"
#import "SCVirtualMachine.h"


#include "SCView.h"
#import "SCImage.h"


void QDDrawBevelRect(CGContextRef cgc, CGRect bounds, float width, bool inout);

SCViewMaker *gSCViewMakers = 0;
SCView *gAnimatedViews = 0;

extern PyrSymbol* s_color;
extern PyrSymbol* s_doaction;
extern PyrSymbol* s_draw;

PyrSymbol* s_x;
PyrSymbol* s_y;
PyrSymbol* s_lo;
PyrSymbol* s_hi;
PyrSymbol* s_range;
PyrSymbol* s_scview;
PyrSymbol* s_sccontview;
PyrSymbol* s_sctopview;
PyrSymbol* s_scscrollview;
PyrSymbol* s_beginDrag;
PyrSymbol* s_receiveDrag;
PyrSymbol* s_canReceiveDrag;
PyrSymbol* s_mouseDown;
PyrSymbol* s_mouseUp;
PyrSymbol* s_callDrawFunc;
PyrSymbol* s_toggleEditMode;

extern pthread_mutex_t gLangMutex;

int stringDrawCenteredInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);
int stringDrawLeftInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);
int stringDrawRightInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);

int nsStringDrawInRectAlign(NSString *nsstring, SCRect screct, char *cFontName, float fontSize, SCColor sccolor, int hAlign, int vAlign, NSSize *outSize);

RGBColor SCtoQDColor(SCColor sccolor)
{
	RGBColor qdcolor;

	qdcolor.red   = (unsigned short)(sccolor.red   * 65535.);
	qdcolor.green = (unsigned short)(sccolor.green * 65535.);
	qdcolor.blue  = (unsigned short)(sccolor.blue  * 65535.);
	return qdcolor;
}

// CoreGraphics coords get switched around in an NSQuickDrawView
Rect SCtoQDRect(SCRect screct)
{
	Rect qdrect;

	qdrect.left   = (int)screct.x;
	qdrect.top    = (int)screct.y;
	qdrect.right  = (int)(screct.x + screct.width);
	qdrect.bottom = (int)(screct.y + screct.height);
	return qdrect;
}

CGRect SCtoCGRect(SCRect screct)
{
	CGRect cgrect;

	cgrect.origin.x    = screct.x;
	cgrect.origin.y    = screct.y;
	cgrect.size.width  = screct.width;
	cgrect.size.height = screct.height;
	return cgrect;
}

int slotColorVal(PyrSlot *slot, SCColor *sccolor)
{
	if (!(isKindOfSlot(slot, s_color->u.classobj))) return errWrongType;

	PyrSlot *slots = slotRawObject(slot)->slots;

	int err;
	err = slotFloatVal(slots+0, &sccolor->red);
	if (err) return err;
	err = slotFloatVal(slots+1, &sccolor->green);
	if (err) return err;
	err = slotFloatVal(slots+2, &sccolor->blue);
	if (err) return err;
	err = slotFloatVal(slots+3, &sccolor->alpha);
	return err;
}

int setSlotColor(PyrSlot *slot, SCColor *sccolor)
{
	if (!(isKindOfSlot(slot, s_color->u.classobj))) return errWrongType;

	PyrSlot *slots = slotRawObject(slot)->slots;

	SetFloat(slots+0, sccolor->red);
	SetFloat(slots+1, sccolor->green);
	SetFloat(slots+2, sccolor->blue);
	SetFloat(slots+3, sccolor->alpha);
	return errNone;
}

int slotGetSCRect(PyrSlot* a, SCRect *r)
{
	if (!isKindOfSlot(a, s_rect->u.classobj)) return errWrongType; // arg check - br
	PyrSlot *slots = slotRawObject(a)->slots;
	int err;
	err = slotFloatVal(slots+0, &r->x);
	if (err) return err;
	err = slotFloatVal(slots+1, &r->y);
	if (err) return err;
	err = slotFloatVal(slots+2, &r->width);
	if (err) return err;
	err = slotFloatVal(slots+3, &r->height);
	if (err) return err;

	return errNone;
}


int getBackgroundVal(PyrSlot *slot, DrawBackground *inPtr);
int getBackgroundVal(PyrSlot *slot, DrawBackground *inPtr)
{
    SetNil(slot);
    //inPtr->GetSlot(slot);
    return errNone;
}

int slotBackgroundVal(PyrSlot *slot, DrawBackground **ioPtr);
int slotBackgroundVal(PyrSlot *slot, DrawBackground **ioPtr)
{
	int err, direction, steps;
	SCColor color1, color2;
	PyrClass *classobj = classOfSlot(slot);
	char *classname = slotRawSymbol(&classobj->name)->name;

	if (strcmp(classname, "Color")==0) {
		err = slotColorVal(slot, &color1);
		if (err) return err;

		delete *ioPtr;
		*ioPtr = new SolidColorBackground(color1);
	} else if (strcmp(classname, "Gradient") == 0) {
		PyrObject *obj = slotRawObject(slot);
		PyrSlot *slots = obj->slots;

		err = slotColorVal(slots+0, &color1);
		if (err) return err;
		err = slotColorVal(slots+1, &color2);
		if (err) return err;

		if (IsSym(slots+2)) {
			if (strncmp(slotRawSymbol(&slots[2])->name, "h", 1)==0) direction = grad_Horizontal;
			else if (strncmp(slotRawSymbol(&slots[2])->name, "v", 1)==0) direction = grad_Vertical;
			else if (strncmp(slotRawSymbol(&slots[2])->name, "n", 1)==0) direction = grad_Narrow;
			else if (strncmp(slotRawSymbol(&slots[2])->name, "w", 1)==0) direction = grad_Wide;
			else direction = grad_Vertical;
		} else {
			direction = grad_Horizontal;
		}

		err = slotIntVal(slots+3, &steps);
		if (err) return err;

		delete *ioPtr;
		*ioPtr = new GradientBackground(color1, color2, direction, steps);

	} else if (strcmp(classname, "HiliteGradient") == 0) {
		PyrObject *obj = slotRawObject(slot);
		PyrSlot *slots = obj->slots;

		err = slotColorVal(slots+0, &color1);
		if (err) return err;
		err = slotColorVal(slots+1, &color2);
		if (err) return err;

		if (IsSym(slots+2)) {
			if (strncmp(slotRawSymbol(&slots[2])->name, "h", 1)==0) direction = grad_Horizontal;
			else if (strncmp(slotRawSymbol(&slots[2])->name, "v", 1)==0) direction = grad_Vertical;
			else if (strncmp(slotRawSymbol(&slots[2])->name, "n", 1)==0) direction = grad_Narrow;
			else if (strncmp(slotRawSymbol(&slots[2])->name, "w", 1)==0) direction = grad_Wide;
			else direction = grad_Vertical;
		} else {
			direction = grad_Horizontal;
		}

		err = slotIntVal(slots+3, &steps);
		if (err) return err;

		float frac;
		err = slotFloatVal(slots+4, &frac);
		if (err) return err;

		delete *ioPtr;
		*ioPtr = new HiliteGradientBackground(color1, color2, direction, steps, frac);

	}
	return errNone;
}

int slotBackgroundImageVal(PyrSlot *slot, DrawBackground **ioPtr);
int slotBackgroundImageVal(PyrSlot *slot, DrawBackground **ioPtr)
{
	if (!isKindOfSlot(slot, class_array)) return errWrongType;

	PyrSlot *slots = slotRawObject(slot)->slots;
	PyrClass *classobj = classOfSlot(slots + 0);
	char *classname = slotRawSymbol(&classobj->name)->name;
	if( strcmp(classname, "SCImage") == 0 ) 
	{
		int err;
		SCImage *scImage;
		PyrObject *obj = slotRawObject(slots + 0);
		scImage = (SCImage *)slotRawPtr(obj->slots + 0); // Instance of Cocoa SCImage
		
		int tileMode;
		err = slotIntVal(slots+1, &tileMode);
		if (err) return err;

		float frac;
		err = slotFloatVal(slots+2, &frac);
		if (err) return err;

		NSRect fromRect;
		if (NotNil(slots+3)) {

			SCRect screct;
			err = slotGetSCRect(slots+3, &screct);
			if (err) return err;

			fromRect = NSMakeRect(screct.x, screct.y, screct.width, screct.height);

		} else {

			fromRect = NSMakeRect(0., 0., [scImage width], [scImage height]);
		}


		delete *ioPtr;
		*ioPtr = new SCImageBackground(scImage, fromRect, tileMode, frac);
	}
    return errNone;
}

// for any sc class that supports mouseDown and mouseUp methods
#import "TabletEvents.h"

#define TABLETTRACK(METHOD,X,Y) \
    if (mObj) { \
        pthread_mutex_lock (&gLangMutex); \
        VMGlobals *g = gMainVMGlobals; \
        g->canCallOS = true; \
        ++g->sp;  SetObject(g->sp, mObj); \
        SetFloat(++g->sp, X); \
        SetFloat(++g->sp, Y); \
        SetFloat(++g->sp, (float)[theEvent pressure]); \
        NSPoint tilt; \
        tilt = (NSPoint)[theEvent tilt]; \
        SetFloat(++g->sp, tilt.x); \
        SetFloat(++g->sp, tilt.y); \
        SetInt(++g->sp,[theEvent deviceID]); \
        SetInt(++g->sp,[theEvent buttonNumber]); \
        SetInt(++g->sp,[theEvent clickCount]); \
        SetInt(++g->sp,[theEvent absoluteZ]); \
        SetFloat(++g->sp,[theEvent rotationInDegrees]); \
        runInterpreter(g, METHOD, 11); \
        g->canCallOS = false; \
        pthread_mutex_unlock (&gLangMutex); \
    } \





Layout::Layout()
    : mMinWidth(0.), mMaxWidth(10000.), mMinHeight(0.), mMaxHeight(10000.), mWeight(1.),
    mShouldResize(true), mHResize(layout_FixedLeft), mVResize(layout_FixedTop) {
}

SCView::SCView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: mNext(0), mNextAnimatedView(0), mPrevAnimatedView(0),
		mParent(0), mTop(0), mObj(inObj), mBounds(inBounds),
		mBackground(0), mBackgroundImage(0), mVisible(true), mEnabled(true),
        mCanFocus(true), mDragHilite(false),
		mConstructionMode(-1), mDragLabel(0)
{
	mFocusColor = SCMakeColor(0.0,0.0,0.0, 0.5);
	mBounds = inBounds;
	if (inParent){
		inParent->add(this);
		if(!(inParent->isSubViewScroller())){
			SCRect pbounds = inParent->getLayout().bounds;
			mLayout.bounds.x = mBounds.x + pbounds.x;
			mLayout.bounds.y = mBounds.y + pbounds.y;
			mLayout.bounds.width = mBounds.width;
			mLayout.bounds.height = mBounds.height;
		} else {
			mLayout.bounds = mBounds;
       }
		
	}
	// store myself into sc object.
	if (mObj) SetPtr(mObj->slots+0, this);
}

SCView::~SCView()
{
	stopAnimation();
	makeFocus(false);
	mTop->forgetView(this);
	if (mParent) mParent->remove(this);
	if (mObj) SetNil(mObj->slots+0);

	delete mBackground;
	delete mBackgroundImage;

	mTop = 0;
	mParent = 0;
}

void SCView::startAnimation()
{
	mNextAnimatedView = gAnimatedViews;
	if (mNextAnimatedView) mNextAnimatedView->mPrevAnimatedView = this;
	mPrevAnimatedView = 0;
	gAnimatedViews = this;
}

void SCView::stopAnimation()
{
	SCView *nextAnim = mNextAnimatedView;
	SCView *prevAnim = mPrevAnimatedView;
	if (nextAnim) nextAnim->mPrevAnimatedView = prevAnim;
	if (prevAnim) prevAnim->mNextAnimatedView = nextAnim;
	else if (gAnimatedViews == this) gAnimatedViews = nextAnim;
	mPrevAnimatedView = mNextAnimatedView = 0;
}


bool SCView::hit(SCPoint where) const
{
	SCRect bounds = mLayout.bounds;
	return SCPointInRect(where, bounds);
}

void SCView::keyDown(int character, int modifiers, unsigned short keycode)
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("keyDown");
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetChar(g->sp, character);
		++g->sp;  SetInt(g->sp, modifiers);
		++g->sp;  SetInt(g->sp, character);
		++g->sp;  SetInt(g->sp, keycode);
		runInterpreter(g, method, 5);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

void SCView::keyUp(int character, int modifiers, unsigned short keycode)
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("keyUp");
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetChar(g->sp, character);
		++g->sp;  SetInt(g->sp, modifiers);
		++g->sp;  SetInt(g->sp, character);
		++g->sp;  SetInt(g->sp, keycode);
		runInterpreter(g, method, 5);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

void SCView::keyModifiersChanged(int modifiers)
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("keyModifiersChanged");
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetInt(g->sp, modifiers);
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);

}

void SCView::mouseDownAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("mouseDown");
	int clickCount = [theEvent clickCount];
	int buttonNum = [theEvent buttonNumber];
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		SCRect tbounds = getDrawBounds();
		where.x = where.x - tbounds.x;
		where.y = where.y - tbounds.y;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetFloat(g->sp, where.x);
		++g->sp;  SetFloat(g->sp, where.y);
		++g->sp;  SetInt(g->sp, modifiers);
		++g->sp;  SetInt(g->sp,buttonNum);
		++g->sp;  SetInt(g->sp,clickCount);
		runInterpreter(g, method, 6);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}
void SCView::mouseMoveAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("mouseMove");
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		SCRect tbounds = getDrawBounds();
		where.x = where.x - tbounds.x;
		where.y = where.y - tbounds.y;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetFloat(g->sp, where.x);
		++g->sp;  SetFloat(g->sp, where.y);
		++g->sp;  SetInt(g->sp, modifiers);
		runInterpreter(g, method, 4);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}
void SCView::mouseUpAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("mouseUp");
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		SCRect tbounds = getDrawBounds();
		where.x = where.x - tbounds.x;
		where.y = where.y - tbounds.y;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetFloat(g->sp, where.x);
		++g->sp;  SetFloat(g->sp, where.y);
		++g->sp;  SetInt(g->sp, modifiers);
		runInterpreter(g, method, 4);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

void SCView::setConstructionModeFromPoint(SCPoint where)
{
	SCRect bounds;
	bounds = SCMakeRect(mBounds.x, mBounds.y, mBounds.width*0.6, mBounds.height*0.6);
//	post("point: x: %f, y: %f, bounds: x: %f, y: %f\n", where.x, where.y, mBounds.x, mBounds.y);
	if( SCPointInRect(where, bounds)){
		mConstructionMode = view_PositionConstructionMode;
//		[[NSCursor openHandCursor] set];

	}else{
//		[[NSCursor crosshairCursor] set];
		mConstructionMode = view_ResizeConstructionMode;
	}
}

void SCView::doConstructionMove(SCPoint where)
{

	if( mConstructionMode == view_ResizeConstructionMode){
		mBounds.width = mBounds.width + (where.x - (mBounds.width + mBounds.x));
		mBounds.height = mBounds.height + (where.y - (mBounds.height + mBounds.y));
	}else if (mConstructionMode == view_PositionConstructionMode) {
		mBounds.x = where.x;
		mBounds.y = where.y;
	}
	refresh();
	mTop->refresh();
}

NSMenu* SCView::contextMenu(SCPoint inPoint)
{
	return 0;
}

void SCView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	mouseTrack(where, modifiers,theEvent);
}

void SCView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
}

void SCView::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	mouseTrack(where, modifiers,theEvent);
}

void SCView::mouseOver(SCPoint where, int modifiers, NSEvent *theEvent )
{
	pthread_mutex_lock (&gLangMutex);
	PyrSymbol *method = getsym("mouseOver");
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		SCRect tbounds = getDrawBounds();
		where.x = where.x - tbounds.x;
		where.y = where.y - tbounds.y;
		++g->sp;  SetObject(g->sp, mObj);
		++g->sp;  SetInt(g->sp, (int) where.x);
		++g->sp;  SetInt(g->sp, (int) where.y);
		++g->sp;  SetInt(g->sp, modifiers);
		runInterpreter(g, method, 4);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

bool SCView::canReceiveDrag()
{
    return false;
}

void SCView::receiveDrag()
{
}

void SCView::draggingEntered (SCPoint where)
{
	mouseOver(where, 0, NULL);
}
void SCView::draggingUpdated (SCPoint where)
{
	mouseOver(where, 0, NULL);

}

void SCView::setDragHilite(bool inFlag)
{
	bool prevFlag = mDragHilite;
	mDragHilite = inFlag;
	if (mDragHilite != prevFlag) refresh();
}

void hPaintGradient(CGContextRef cgc, CGRect bounds, SCColor startColor, SCColor endColor, int numSteps);

void SCView::draw(SCRect inDamage)
{
	CGContextRef cgc;
	CGRect rect;
	if ( mBackground || mBackgroundImage ) {
		SCRect bounds;
		bounds = getDrawBounds();
		// NSLog(@"back bounds: %f, %f, %f, %f", bounds.x, bounds.y, bounds.width, bounds.height);
		cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		rect = SCtoCGRect(bounds);
	}

	if (mBackground)
		mBackground->draw(cgc, rect);

	if (mBackgroundImage)
		mBackgroundImage->draw(cgc, rect);
}

void SCView::drawDisabled(SCRect inDamage)
{
	if (!mEnabled && shouldDim()) {
		SCRect bounds = getDrawBounds();
		CGRect rect = SCtoCGRect(bounds);
		CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(cgc);
		CGContextSetRGBFillColor(cgc, 1., 1., 1., 0.5);
		CGContextFillRect(cgc, rect);
		CGContextRestoreGState(cgc);
	}
}

void SCView::drawFocus(SCRect inDamage)
{
	if (isFocus()) {
		SCRect bounds = getDrawBounds();
		CGRect rect = SCtoCGRect(bounds);
		rect.origin.x -= 2;
		rect.origin.y -= 2;
		rect.size.width += 4;
		rect.size.height += 4;
		CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(cgc);
		CGContextSetLineWidth(cgc, 2);
//        CGContextSetRGBStrokeColor(cgc, 0., 0., 0., 0.5);
		CGContextSetRGBStrokeColor(cgc, mFocusColor.red, mFocusColor.green, mFocusColor.blue, mFocusColor.alpha);

		CGContextStrokeRect(cgc, rect);
		CGContextRestoreGState(cgc);
	}
}


void SCView::drawDragHilite(SCRect inDamage)
{
	if (mDragHilite) {
		SCRect bounds = getDrawBounds();

		CGRect rect = SCtoCGRect(bounds);
		rect.origin.x += 2;
		rect.origin.y += 2;
		rect.size.width -= 4;
		rect.size.height -= 4;
		CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(cgc);
		CGContextSetLineWidth(cgc, 4);
		CGContextSetRGBStrokeColor(cgc, 0., 0., 1., 0.4);
		CGContextStrokeRect(cgc, rect);
		CGContextRestoreGState(cgc);
	}
}

void SCView::drawIfNecessary(SCRect inDamage)
{
	SCRect bounds = getDrawBounds();
	if(isFocus()) {
		bounds.x -= 4; bounds.y -= 4;
		bounds.width += 8; bounds.height += 8;
	}
	if (SCRectsDoIntersect(inDamage, bounds) && mVisible) {
		draw(inDamage);
		drawDisabled(inDamage);
		drawDragHilite(inDamage);
		drawFocus(inDamage);
	}
}

SCView* SCView::findView(SCPoint where)
{
	if (hit(where) && mEnabled && mVisible) return this;
	else return 0;
}

SCView* SCView::findViewByID(int32 inID)
{
	if (inID == mID) return this;
	else return 0;
}

bool SCView::shouldDim()
{
	return true;
}

bool SCView::canFocus()
{
	bool flag = mEnabled && mVisible && mCanFocus;
	if (mParent) flag = flag && mParent->canFocus();
	return flag;
}

bool SCContainerView::canFocus()
{
    bool flag = mEnabled && mVisible;
    if (mParent) flag = flag && mParent->canFocus();
    return flag;
}

SCRect NStoSCRect(NSRect nsrect)
{
	SCRect screct;
    screct.x = nsrect.origin.x;
    screct.y = nsrect.origin.y;
    screct.width = nsrect.size.width;
    screct.height = nsrect.size.height;
    return screct;
}
NSRect SCtoNSRect(SCRect screct);

void SCView::setBounds(SCRect inBounds)
{
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

SCRect SCView::getBounds()
{
	return mBounds;
}


SCRect SCView::getDrawBounds() //relative to ContainerView
{
	return mLayout.bounds;
}

Layout SCView::getLayout()
{
    return mLayout;
}

void SCView::makeFocus(bool focus)
{
    if (focus) {
        if (canFocus() && !isFocus()) {
            SCView *prevFocus = mTop->focusView();
            if (prevFocus) prevFocus->makeFocus(false);
            mTop->focusIs(this);
            refreshFocus();
			NSView* newFirstResponder = focusResponder();
			[[newFirstResponder window] makeFirstResponder:newFirstResponder];
        }
    } else {
        if (isFocus()) {
            mTop->focusIs(0);
            refreshFocus();
        }
    }
}

NSView* SCView::focusResponder() { return mTop->GetNSView(); }

void SCContainerView::makeFocus(bool focus)
{
}

SCView* SCView::nextFocus(bool *foundFocus, bool canFocus)
{
    if (isFocus()) {
        *foundFocus = true;
        return 0;
    }
    canFocus = canFocus && mEnabled && mVisible && mCanFocus;
    if (canFocus && *foundFocus) return this;
    return 0;
}

SCView* SCView::prevFocus(SCView **prevView, bool canFocus)
{
    if (isFocus() && *prevView) return *prevView;
    canFocus = canFocus && mEnabled && mVisible && mCanFocus;
    if (canFocus) *prevView = this;
    return 0;
}


void SCView::refresh()
{
    mTop->addDamage(mLayout.bounds);
}

void SCView::refreshInRect(SCRect b)
{
	if(SCRectsDoIntersect(b, mLayout.bounds)) {
		SCRect sect;

		sect.x = sc_max(b.x, mLayout.bounds.x);
		sect.y = sc_max(b.y, mLayout.bounds.y);
		sect.width = sc_min(b.x + b.width, mLayout.bounds.x + mLayout.bounds.width);
		sect.height = sc_min(b.y + b.height, mLayout.bounds.y + mLayout.bounds.height);
		sect.width -= sect.x; sect.height -= sect.y;
		mTop->addDamage(sect);
	}
}

// cannot call from primitives. i.e. new view, or get/set property
void SCView::sendMessage(PyrSymbol *method, int numargs, PyrSlot *args, PyrSlot *result)
{
	//CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	//CGContextSaveGState(cgc);
	pthread_mutex_lock (&gLangMutex);
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj);
		for (int i=0; i<numargs; ++i) {
			++g->sp;
			slotCopy(g->sp, &args[i]);
		}
		runInterpreter(g, method, numargs+1);
		g->canCallOS = false;
		if (result) slotCopy(result, &g->result);
	}
	pthread_mutex_unlock (&gLangMutex);

	//CGContextRestoreGState(cgc);
}

bool SCView::isDragSource() const
{
	return false;
}

int SCView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
    int err;
	char *name = symbol->name;
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
                if (err) return err;
                refreshFocus();
                //mBounds = screct;

				if(mTop->isScroller()) {
					((SCScrollTopView*)mTop)->setInSetClipViewOrigin(true);
					setBounds(screct);
					[(SCGraphView*)mTop->GetNSView() setFrameSizeToMinimum];
					((SCScrollTopView*)mTop)->setInSetClipViewOrigin(false);
				} else {setBounds(screct);}
                refreshFocus();
		return errNone;
	}
	if (strcmp(name, "visible")==0) {
                bool visible = IsTrue(slot);
                if (mVisible != visible) {
                    mVisible = visible;
                    if (!mVisible) mTop->resetFocus(); //
                    refresh();
                }
		return errNone;
	}
	if (strcmp(name, "enabled")==0) {
                bool enabled = IsTrue(slot);
                if (mEnabled != enabled) {
                    mEnabled = enabled;
                    if (!mEnabled) mTop->resetFocus();
                    refresh();
                }
		return errNone;
	}
	if (strcmp(name, "canFocus")==0) {
                bool canFocus = IsTrue(slot);
                if (mCanFocus != canFocus) {
                    mCanFocus = canFocus;
                    if (!mCanFocus) mTop->resetFocus();
                    refresh();
                }
		return errNone;
	}
	if (strcmp(name, "resize")==0) {
//  1  2  3
//  4  5  6
//  7  8  9
            int32 resize;
			err = slotIntVal(slot, &resize);
			if (err) return err;
            if (resize < 1 || resize > 9) return errIndexOutOfRange;
            mLayout.mHResize = ((resize - 1) % 3) - 1;
            mLayout.mVResize = ((resize - 1) / 3) - 1;
            mTop->refresh();
            return errNone;
	}
    if(strcmp(name,"id") ==0) {
		return slotIntVal(slot, &mID);
    }
    if(strcmp(name,"minWidth") ==0) {
		err = slotFloatVal(slot, &mLayout.mMinWidth);
        mTop->refresh();
        return err;
    }
    if(strcmp(name,"maxWidth") ==0) {
		err = slotFloatVal(slot, &mLayout.mMaxWidth);
        mTop->refresh();
        return err;
    }
    if(strcmp(name,"minHeight") ==0) {
 		err = slotFloatVal(slot, &mLayout.mMinHeight);
       mTop->refresh();
        return err;
    }
    if(strcmp(name,"maxHeight") ==0) {
 		err = slotFloatVal(slot, &mLayout.mMaxHeight);
        mTop->refresh();
        return err;
    }
	if (strcmp(name, "background")==0) {
            err = slotBackgroundVal(slot, &mBackground);
            if (err) return err;
            refresh();
            return errNone;
	}
	if (strcmp(name, "backgroundImage")==0) {
            err = slotBackgroundImageVal(slot, &mBackgroundImage);
            if (err) return err;
            refresh();
            return errNone;
	}
	if (strcmp(name, "focusColor")==0) {
			err = slotColorVal(slot, &mFocusColor);
            if (err) return err;
            refresh();
		return errNone;
	}
	if (strcmp(name, "dragLabel")==0) {
		if(isKindOfSlot(slot, class_string)) {
			PyrString* pstring = slotRawString(slot);
			if(!pstring) return errNone;
			if(mDragLabel) [mDragLabel release];
			mDragLabel = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
			return errNone;
		} else if (IsNil(slot)) {
			if(mDragLabel) [mDragLabel release];
			mDragLabel = 0;
			return errNone;
		} else return errWrongType;
	}
	return errPropertyNotFound;
}

int SCView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	if (strcmp(name, "bounds")==0) {
            if (!(isKindOfSlot(slot, s_rect->u.classobj))) {
                return errWrongType;
            }
            PyrSlot *slots = slotRawObject(slot)->slots;
            SetFloat(slots+0, mBounds.x);
            SetFloat(slots+1, mBounds.y);
            SetFloat(slots+2, mBounds.width);
            SetFloat(slots+3, mBounds.height);
            return errNone;
	}
	if (strcmp(name, "absoluteBounds")==0) {
			SCRect drawBounds;
            if (!(isKindOfSlot(slot, s_rect->u.classobj))) {
                return errWrongType;
            }
			drawBounds = mLayout.bounds;
            PyrSlot *slots = slotRawObject(slot)->slots;
            SetFloat(slots+0, drawBounds.x);
            SetFloat(slots+1, drawBounds.y);
            SetFloat(slots+2, drawBounds.width);
            SetFloat(slots+3, drawBounds.height);
			return errNone;
	}
	if (strcmp(name, "visible")==0) {
                SetBool(slot, mVisible);
		return errNone;
	}
	if (strcmp(name, "enabled")==0) {
                SetBool(slot, mEnabled);
		return errNone;
	}
	if (strcmp(name, "resize")==0) {
            int resize = mLayout.mVResize * 3 + mLayout.mHResize + 5;
            SetInt(slot, resize);
            return errNone;
        }
	if (strcmp(name, "id")==0) {
		SetInt(slot, mID);
		return errNone;
	}
	/*if (strcmp(name, "background")==0) {
            int err = getBackgroundVal(slot, mBackground);
            return err;
	}*/
	/*if (strcmp(name, "backColor")==0) {
            return setSlotColor(slot, &mBackColor);
	}*/

	if (strcmp(name, "focusColor")==0) {
		return setSlotColor(slot, &mFocusColor);;
	}
	return errPropertyNotFound;
}

void SCView::beginDrag(SCPoint where)
{
	sendMessage(s_beginDrag, 0, 0, 0);

	PyrSlot slot;
	PyrSlot stringSlot;
	NSString *string = 0;
	NSString *label = 0;
	pthread_mutex_lock (&gLangMutex);
	if (mObj) {
		VMGlobals *g = gMainVMGlobals;
		int classVarIndex = slotRawInt(&getsym("SCView")->u.classobj->classVarIndex);
		slotCopy(&slot, &g->classvars->slots[classVarIndex]);
		slotCopy(&stringSlot, &g->classvars->slots[classVarIndex+1]);
		if (isKindOfSlot(&stringSlot, class_string)) {
			string = [NSString stringWithCString: slotRawString(&stringSlot)->s length: slotRawString(&stringSlot)->size];
		}
		if(mDragLabel) label = mDragLabel;
	}
	pthread_mutex_unlock (&gLangMutex);

	mTop->beginDragCallback(where, &slot, string, label);
}

////////////////////////////////////////////////////////////////////////////////////////////////

SCContainerView::SCContainerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mChildren(0), mNumChildren(0)
{
	mLayout.bounds = mBounds;
	if(mParent ){
		SCRect pBounds = mParent->getDrawBounds(); // or jsut absolute frame bounds ?
		mLayout.bounds.x = inBounds.x + pBounds.x;
		mLayout.bounds.y = inBounds.y + pBounds.y;
	}
}

SCContainerView::~SCContainerView()
{
    SCView *child = mChildren;
    while (child) {
        SCView *next = child->mNext;
        child->mParent = 0;
        delete child;
        child = next;
    }

}

int SCContainerView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
    int err;
	char *name = symbol->name;

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		if (mVisible != visible) {
			mVisible = visible;
//			SCView *child = mChildren;
//			while (child) {
//				SCView *next = child->mNext;
//				child->setVisibleFromParent(visible && mParent->isVisible()); // needed for Cocoa views
//				child = next;
//			}
			setVisibleFromParent();
			if (!mVisible) mTop->resetFocus(); //
			refresh();
		}
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}

void SCContainerView::setVisibleFromParent()
{
	SCView *child = mChildren;
	while (child) {
		SCView *next = child->mNext;
		child->setVisibleFromParent(); // needed for Cocoa views
		child = next;
	}
}

void SCContainerView::add(SCView *inChild)
{
	inChild->mNext = mChildren;
	mChildren = inChild;
	mNumChildren++;
	inChild->mParent = this;
	inChild->mTop = mTop;
	inChild->refresh();
}

void SCContainerView::remove(SCView *inChild)
{
	SCView *child = mChildren;
	SCView *prev = 0;
    inChild->makeFocus(false);
	while (child) {
		SCView *next = child->mNext;
		if (child == inChild) {
			if (prev) prev->mNext = child->mNext;
			else mChildren = child->mNext;
			child->mParent = 0;
			mNumChildren--;
			return;
		}
		prev = child;
		child = next;
	}
}


void SCContainerView::drawIfNecessary(SCRect inDamage)
{
	SCRect drawBounds;
	drawBounds = getDrawBounds();
    if (SCRectsDoIntersect(inDamage, drawBounds) && mVisible) {
            draw(inDamage);
            SCView *child = mChildren;
			SCView *children[mNumChildren];
			int k = mNumChildren;
            while (child) {
//              child->drawIfNecessary(inDamage);
				children[--k] = child;
				child = child->next();
            }
			for(int i=0; i < mNumChildren; i++ ) {
				child = children[i];
				child->drawIfNecessary(inDamage);
			};
            drawDisabled(inDamage);
            drawDragHilite(inDamage);
            drawFocus(inDamage);
    }
}

SCView* SCContainerView::findView(SCPoint where)
{
        if (mEnabled && mVisible) {
            SCView *child = mChildren;
            while (child) {
				if(!(child->isScroller())) {
                    SCView *found = child->findView(where);
                    if (found) return found;
				}
				child = child->mNext;
            }
        }
	return 0;
}

SCView* SCContainerView::findViewByID(int32 inID)
{
	if (inID == mID) return this;
	SCView *child = mChildren;
	while (child) {
		SCView *found = child->findViewByID(inID);
		if (found) return found;
		child = child->mNext;
	}
	return 0;
}

SCView* SCContainerView::nextFocus(bool *foundFocus, bool canFocus)
{
    canFocus = canFocus && mEnabled && mVisible;
    SCView *child = mChildren;
    while (child) {
        SCView *view = child->nextFocus(foundFocus, canFocus);
        if (view) return view;
        child = child->mNext;
    }
    return 0;
}

SCView* SCContainerView::prevFocus(SCView **prevView, bool canFocus)
{
    canFocus = canFocus && mEnabled && mVisible;
    SCView *child = mChildren;
    while (child) {
        SCView *view = child->prevFocus(prevView, canFocus);
        if (view) return view;
        child = child->mNext;
    }
    return 0;
}

SCRect SCContainerView::checkMinimumSize() {
	SCView *child = mChildren;
	SCRect candidate = mBounds;
	// iterate through all the views and see if we need to be bigger (children may exceed the parent's bounds)
	while (child) {
        SCRect bounds = child->checkMinimumSize();
		candidate = SCRectUnion(bounds, candidate);
		child = child->next();
	}
	return candidate;
}


SCView* NewSCCompositeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCCompositeView(inParent, inObj, inBounds);
}

SCCompositeView::SCCompositeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCContainerView(inParent, inObj, inBounds)
{
}

SCCompositeView::~SCCompositeView()
{
}

void SCCompositeView::setBounds(SCRect inBounds)
{
    SCView *child = mChildren;
	SCView *myTop = mTop;
	SCRect topBounds = myTop->getBounds();
	SCRect topPBounds = myTop->getLayout().bounds;

//    float right = mBounds.x + mBounds.width;
//    float bottom = mBounds.y + mBounds.height;
	mLayout.bounds = inBounds;
//	if(isTopContainer()){
//		post("i'm my own parent ... \t");
//		post("in x:%f, y:%f\n", inBounds.x, inBounds.y);
//	}

	if(mParent){
		if(!(mParent->isSubViewScroller())){
			SCRect pBounds = mParent->getDrawBounds();
			mLayout.bounds.x = inBounds.x + pBounds.x;
			mLayout.bounds.y = inBounds.y + pBounds.y;
		} else {
			mLayout.bounds.x = inBounds.x;
			mLayout.bounds.y = inBounds.y;
		}
	}

    while (child) {
        SCRect bounds = child->getBounds();
        Layout layout = child->getLayout();
//        SCRect bounds = layout.bounds;

        float offset;
        switch (layout.mHResize) {
            case layout_FixedLeft :
                break;

            case layout_FixedRight :
				// if the parent is a top container, then relative or absolute are irrelevant.
				if(child->parent()->isTopContainer() || (inBounds.width != child->parent()->getBounds().width)){
					offset = (mBounds.x + mBounds.width) - (bounds.x + bounds.width);
					bounds.x = (inBounds.x + inBounds.width) - (bounds.width + offset);
				}

                break;

            case layout_HElastic :
                offset = (mBounds.x + mBounds.width) - (bounds.x + bounds.width);
               /* bounds.width = sc_clip(
                                    (inBounds.width) - (bounds.x + offset),
                                    layout.mMinWidth ,
                                    sc_min(layout.mMaxWidth,right - bounds.x)
                                );*/
                bounds.width = (inBounds.width) - ((bounds.x - mBounds.x) + offset);
//				}
        }
        switch (layout.mVResize) {
            case layout_FixedTop :
				break;

            case layout_FixedBottom :

				// if the parent is a top container, then relative or absolute are irrelevant.
				if(child->parent()->isTopContainer() || (inBounds.height != child->parent()->getBounds().height)){
							offset = (mBounds.y + mBounds.height) - (bounds.y + bounds.height);
							bounds.y = (inBounds.y + inBounds.height) - (bounds.height + offset);
				}

				break;

            case layout_VElastic :

                offset = (mBounds.y + mBounds.height) - (bounds.y + bounds.height);
                /*bounds.height = sc_clip(
                                    (inBounds.height) - (bounds.y + offset),
                                    layout.mMinHeight ,
                                    sc_min(layout.mMaxHeight,bottom - bounds.y)
                                );*/
                bounds.height = (inBounds.height) - ((bounds.y - mBounds.y) + offset);
//				}
        }

        child->setBounds(bounds);
        child = child->next();
    }
    // should be limited by the limitations of the contents
    mBounds = inBounds;
}

/////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCLayoutView(inParent, inObj, inBounds);
}

SCLayoutView::SCLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCContainerView(inParent, inObj, inBounds), mSpacing(4.)
{
}

SCLayoutView::~SCLayoutView()
{
}


int SCLayoutView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;
	if (strcmp(name, "spacing")==0) {
		err = slotFloatVal(slot, &mSpacing);
                if (err) return err;
                refresh();
		return errNone;
	}
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
                if (err) return err;
                refreshFocus();
                setBounds(screct);
                refreshFocus();
		return errNone;
	}
        return SCView::setProperty(symbol, slot);
}

int SCLayoutView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	if (strcmp(name, "spacing")==0) {
		SetFloat(slot, mSpacing);
		return errNone;
	}

	return SCView::getProperty(symbol, slot);
}

void SCLayoutView::add(SCView *inChild)
{
	SCContainerView::add(inChild);
	setBounds(mBounds); // re-layout
}

/////////////////////////////////////////////////////////////////////////////////////

SCView* NewSCHLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCHLayoutView(inParent, inObj, inBounds);
}

SCHLayoutView::SCHLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCLayoutView(inParent, inObj, inBounds)
{
}

SCHLayoutView::~SCHLayoutView()
{
}

void SCHLayoutView::setBounds(SCRect inBounds)
{
    SCView *child = mChildren;
    float totalWeight = 0.0;
	//if reativeOrigin is true, the inBounds need to be moved to absolute bounds
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
	SCRect absBounds = mLayout.bounds;
    while(child) {
        Layout layout = child->getLayout();
        // could store this when child added
        totalWeight += layout.mWeight;
        child = child->next();
    }
    // subtract the spacers we will use
    float totalWidth = absBounds.width - (mSpacing * (mNumChildren - 1));

    // find views who are constrained by a minimum or maximum size
    // and remove them from the set of weights.
    float scaleWeight = sc_max(totalWidth,0.0) * (1.0 / sc_max(totalWeight,0.01));
    child = mChildren;
    float widths[mNumChildren];
    SCView *children[mNumChildren];
    int ri = mNumChildren;
    while(child) {
        float width;
        //reverse the array
        children[--ri] = child;
        Layout layout = child->getLayout();
        float weightedWidth = scaleWeight * layout.mWeight;
        if(layout.mHResize == 0) {// okay to resize
            if (weightedWidth < layout.mMinWidth) {
                width = layout.mMinWidth;
                widths[ri]  = width;
                totalWidth -= width;
                totalWeight -= layout.mWeight;
            } else {
                if (weightedWidth > layout.mMaxWidth) {
                    width = layout.mMaxWidth;
                    widths[ri]  = width;
                    totalWidth -= width;
                    totalWeight -= layout.mWeight;
                } else {
                    widths[ri] = -1.0;
                }
            }
        }  else {
            SCRect rect = child->getBounds();
            widths[ri] = rect.width;
        }
        child = child->next();
    }
    //totalWidth is now the remaining flexible width

    // now layout the views
    float left = 0.f;  // subviews will automatically offset from window origin
    float top = 0.f;
    float height = absBounds.height;
	// NSLog(@"view bounds: %f, %f, %f, %f", absBounds.x, absBounds.y, absBounds.width, absBounds.height);
    scaleWeight = totalWidth * (1.0/totalWeight);
    child = mChildren;
    int i=0;
    for(; i < mNumChildren; i++ ) {
        child = children[i];
        Layout layout = child->getLayout();
        float width;
        if(widths[i] == -1.0) {
            width = scaleWeight * layout.mWeight;
        } else { // was constrained
            width = widths[i];
        }
        child->setBounds(SCMakeRect( left, top, width, height));
        left += (width + mSpacing);
        child = child->next();
    }

}

/////////////////////////////////////////////////////////////////////////////////////

SCView* NewSCVLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCVLayoutView(inParent, inObj, inBounds);
}

SCVLayoutView::SCVLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCLayoutView(inParent, inObj, inBounds)
{
}

SCVLayoutView::~SCVLayoutView()
{
}

void SCVLayoutView::setBounds(SCRect inBounds)
{
    SCView *child = mChildren;
    float totalWeight = 0.0;
    while(child) {
        Layout layout = child->getLayout();
        // could store this when child added
        totalWeight += layout.mWeight;
        child = child->next();
    }
	//if reativeOrigin is true, the inBounds need to be moved to absolute bounds
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
	SCRect absBounds = mLayout.bounds;
    // subtract the spacers we will use
    float totalHeight = absBounds.height - (mSpacing * (mNumChildren - 1));

    // find views who are constrained by a minimum or maximum size
    // and remove them from the set of weights.
    float scaleWeight = sc_max(totalHeight,0.0) * (1.0 / sc_max(totalWeight,0.01));
    child = mChildren;
    float heights[mNumChildren];
    SCView *children[mNumChildren];
    int ri = mNumChildren;
    while(child) {
        float height;
        //reverse the array
        children[--ri] = child;
        Layout layout = child->getLayout();
        float weightedHeight = scaleWeight * layout.mWeight;
        if(layout.mVResize == 0) {// okay to resize
            if (weightedHeight < layout.mMinHeight) {
                height = layout.mMinHeight;
                heights[ri]  = height;
                totalHeight -= height;
                totalWeight -= layout.mWeight;
            } else {
                if (weightedHeight > layout.mMaxHeight) {
                    height = layout.mMaxHeight;
                    heights[ri]  = height;
                    totalHeight -= height;
                    totalWeight -= layout.mWeight;
                } else {
                    heights[ri] = -1.0;
                }
            }
        }  else {
            SCRect rect = child->getBounds();
            heights[ri] = rect.height;
        }
        child = child->next();
    }
    //totalHeight is now the remaining flexible height

    // now layout the views
    float left = 0.f;  // subviews will automatically offset from window origin
    float top = 0.f;
    float width = absBounds.width;
    scaleWeight = totalHeight * (1.0/totalWeight);
    child = mChildren;
    int i=0;
    for(; i < mNumChildren; i++ ) {
        child = children[i];
        Layout layout = child->getLayout();
        float height;
        if(heights[i] == -1.0) {
            height = scaleWeight * layout.mWeight;
        } else { // was constrained
            height = heights[i];
        }
        child->setBounds(SCMakeRect( left, top, width, height));
        top += (height + mSpacing);
        child = child->next();
    }

}


/////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCTopView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCTopView(inObj, inBounds);
}

SCTopView::SCTopView(PyrObject* inObj, SCRect inBounds)
	: SCCompositeView(0, inObj, inBounds), mFocusView(0), mDragView(0), mConstructionMode(false)
{
    mTop = this;
    //float ltgry = 0.8;
    //float dkgry = 0.5;
    //mBackground = new SolidColorBackground(
    //                SCMakeColor(ltgry, ltgry, ltgry, 1.0));
}

void SCTopView::addDamage(SCRect inRect)
{
    (*mDamageCallback)(inRect, mHostData);
    //mDamage = SCRectUnion(mDamage, inRect);
}

void SCTopView::beginDragCallback(SCPoint where, PyrSlot* slot, NSString* string, NSString* label)
{
    (*mDragCallback)(where, slot, string, label, mHostData);
}

void SCView::refreshFocus()
{
    SCRect focusBounds = getDrawBounds();
    focusBounds.x -= 4;
    focusBounds.y -= 4;
    focusBounds.width += 8;
    focusBounds.height += 8;
    mTop->addDamage(focusBounds);
}

void SCTopView::resetFocus()
{
    SCView *view = focusView();
    if (view && !view->canFocus()) {
        focusIs(0);
        view->refreshFocus();
    }
}

void SCTopView::forgetView(SCView *view)
{
	if (view == mFocusView) mFocusView = 0;
	if (view == mDragView) mDragView = 0;
}

void SCTopView::tabNextFocus()
{
    bool foundFocus = mFocusView ? false : true;
    SCView *view = nextFocus(&foundFocus, true);
    if (!view && foundFocus) view = nextFocus(&foundFocus, true);
    if (view) view->makeFocus(true);
}

void SCTopView::tabPrevFocus()
{
    SCView *prevView = 0;
    SCView *view = prevFocus(&prevView, true);
    if (!view && prevView) view = prevView;
    if (view) view->makeFocus(true);
}

void SCTopView::setDragView(SCView *inView)
{
    if (inView != mDragView) {
        if (mDragView) mDragView->setDragHilite(false);
        mDragView = inView;
        if (mDragView) mDragView->setDragHilite(true);
    }
}

void SCTopView::drawFocus(SCRect inDamage)
{
    if (ConstructionMode()) {
        CGRect rect = SCtoCGRect(mBounds);
        rect.origin.x += 2;
        rect.origin.y += 2;
        rect.size.width -= 4;
        rect.size.height -= 4;
        CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
        CGContextSaveGState(cgc);
        CGContextSetLineWidth(cgc, 4);
        CGContextSetRGBStrokeColor(cgc, 1., 1., 0., 1.);
        CGContextStrokeRect(cgc, rect);
        CGContextRestoreGState(cgc);
    }
}

bool SCTopView::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCTopView::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

void SCTopView::setInternalBounds(SCRect internalBounds)
{
	setBounds(internalBounds);
}

/////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCScrollTopView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCScrollTopView(inObj, inBounds);
}

SCScrollTopView::SCScrollTopView(PyrObject* inObj, SCRect inBounds)
: SCTopView(inObj, inBounds), mInSetClipViewOrigin(false)
{

}

SCRect SCScrollTopView::checkMinimumSize()
{
	// size of SCGraphView must be at least large enough to hold any contained view for correct scrolling
	// so test against the NSClipView
    SCView *child = mChildren;

	SCRect candidate = SCMakeRect(0.f, 0.f, 1.f, 1.f); // smallest measureable at rect at origin

	// iterate through all the views and see if we need to be bigger
	while (child) {
        SCRect bounds = child->checkMinimumSize();
		candidate = SCRectUnion(bounds, candidate);
		child = child->next();
	}

	return candidate;
}

SCRect SCScrollTopView::getDrawBounds() //relative to ContainerView
{
	return SCRectUnion(mBounds, checkMinimumSize());
}

void SCScrollTopView::setInternalBounds(SCRect inBounds)
{
	// elasticity doesn't work with a scrolltopview
	mBounds = inBounds;
}

int slotGetPoint(PyrSlot* a, NSPoint *p);

int SCScrollTopView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;
	if (strcmp(name, "setHasHorizontalScroller")==0) {
		bool hasScroller = IsTrue(slot);
		setInSetClipViewOrigin(true);
		[mNSScrollView setHasHorizontalScroller:hasScroller];
		[(SCGraphView*)mNSView setFrameSizeToMinimum];
		setInSetClipViewOrigin(false);
		refresh();
		return errNone;
	}

	if (strcmp(name, "setHasVerticalScroller")==0) {
		bool hasScroller = IsTrue(slot);
		setInSetClipViewOrigin(true);
		[mNSScrollView setHasVerticalScroller:hasScroller];
		[(SCGraphView*)mNSView setFrameSizeToMinimum];
		setInSetClipViewOrigin(false);
		refresh();
		return errNone;
	}

	if (strcmp(name, "setAutohidesScrollers")==0) {
		bool autoHides = IsTrue(slot);
		setInSetClipViewOrigin(true);
		[mNSScrollView setAutohidesScrollers:autoHides];
		[(SCGraphView*)mNSView setFrameSizeToMinimum];
		[mNSScrollView setNeedsDisplay: YES];

		// trick scrollview into redrawing
		NSRect frame = [mNSScrollView frame];
		[mNSScrollView setFrame: NSInsetRect(frame,1,1)];
		[mNSScrollView setFrame: frame];
		setInSetClipViewOrigin(false);
		refresh();
		return errNone;
	}

	if (strcmp(name, "clipViewOrigin")==0) {
		NSPoint origin;
		err = slotGetPoint(slot, &origin);
		if (err) return err;
		setInSetClipViewOrigin(true);
		[[mNSScrollView documentView] scrollPoint:origin];
		setInSetClipViewOrigin(false);
		refresh();
		return errNone;
	}

	if (strcmp(name, "setAutoScrolls")==0) {
		bool autoScrolls = IsTrue(slot);
		[(SCGraphView*)mNSView setAutoScrolls:autoScrolls];
		return errNone;
	}

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		if(visible)
		{
			[mNSScrollView setHidden:NO];
		}
		else
		{
			[mNSScrollView setHidden:YES];
		}
		mVisible = visible;
		setVisibleFromParent(); //SCContainerView
		resetFocus();
		return errNone;
	}

	return SCCompositeView::setProperty(symbol, slot);
}

int SCScrollTopView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	if (strcmp(name, "absoluteBounds")==0) {
		if (!(isKindOfSlot(slot, s_rect->u.classobj))) {
			return errWrongType;
		}
		SCRect bounds = NStoSCRect([mNSScrollView frame]);
		PyrSlot *slots = slotRawObject(slot)->slots;
		SetFloat(slots+0, bounds.x);
		SetFloat(slots+1, bounds.y);
		SetFloat(slots+2, bounds.width);
		SetFloat(slots+3, bounds.height);
		return errNone;
	}

	if (strcmp(name, "clipViewOrigin")==0) {
		if (!(isKindOfSlot(slot, s_point->u.classobj))) {
			return errWrongType;
		}
		NSPoint origin = [mNSScrollView documentVisibleRect].origin;
		PyrSlot *slots = slotRawObject(slot)->slots;
		SetFloat(slots+0, origin.x);
		SetFloat(slots+1, origin.y);
		return errNone;
	}

	if (strcmp(name, "innerBounds")==0) {
		if (!(isKindOfSlot(slot, s_rect->u.classobj))) {
			return errWrongType;
		}
		SCRect bounds = NStoSCRect([mNSView frame]);
		PyrSlot *slots = slotRawObject(slot)->slots;
		SetFloat(slots+0, bounds.x);
		SetFloat(slots+1, bounds.y);
		SetFloat(slots+2, bounds.width);
		SetFloat(slots+3, bounds.height);
		return errNone;
	}

	return SCCompositeView::getProperty(symbol, slot);
}

void SCScrollTopView::add(SCView *inChild)
{
	setInSetClipViewOrigin(true);
	SCContainerView::add(inChild);
	[(SCGraphView*)GetNSView() setFrameSizeToMinimum];
	setInSetClipViewOrigin(false);
}

void SCScrollTopView::remove(SCView *inChild)
{
	setInSetClipViewOrigin(true);
	SCContainerView::remove(inChild);
	[(SCGraphView*)GetNSView() setFrameSizeToMinimum];
	setInSetClipViewOrigin(false);
}

/////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCScrollView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCScrollView(inParent, inObj, inBounds);
}

SCScrollView::SCScrollView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
: SCScrollTopView(inObj, inBounds)
{

	NSRect bounds = SCtoNSRect(inBounds);

	if (inParent){
		inParent->add(this);
		if(!(inParent->isSubViewScroller())){
			SCRect pbounds = inParent->getLayout().bounds;
			mLayout.bounds.x = mBounds.x + pbounds.x;
			mLayout.bounds.y = mBounds.y + pbounds.y;
			mLayout.bounds.width = mBounds.width;
			mLayout.bounds.height = mBounds.height;
		} else {
			mLayout.bounds = mBounds;
		}
	}


	NSView *topGraphview = mTop->GetNSView();
	SCGraphView* view = [[SCGraphView alloc] initWithFrame: NSMakeRect(0, 0, bounds.size.width, bounds.size.height)];
	[view setSCObject: [(SCGraphView*)topGraphview getSCObject]];

	[view setSCTopView: this];
	NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame: SCtoNSRect(mLayout.bounds)];
	[scrollView setHasVerticalScroller:YES];
	[scrollView setHasHorizontalScroller:YES];
	[scrollView setAutohidesScrollers:YES];
	[[scrollView horizontalScroller] setControlSize:NSSmallControlSize];
	[[scrollView verticalScroller] setControlSize:NSSmallControlSize];
	[[scrollView horizontalScroller] setControlTint:NSGraphiteControlTint];
	[[scrollView verticalScroller] setControlTint:NSGraphiteControlTint];

	[scrollView setBackgroundColor:[NSColor clearColor]];
	[scrollView setDrawsBackground:NO];
	// configure the scroller to have no visible border
	[scrollView setBorderType:NSNoBorder];
	[scrollView setAutoresizingMask:NSViewNotSizable];
	[scrollView setDocumentView:view];

	[scrollView setPostsFrameChangedNotifications: YES]; // we need this to resize the SCGraphView if the scroll view exceeds its bounds
	[[NSNotificationCenter defaultCenter] addObserver:view
											 selector:@selector(scrollViewResized:)
												 name:@"NSViewFrameDidChangeNotification"
											   object:scrollView];

	NSClipView *contentView = [scrollView contentView];
	[contentView setPostsBoundsChangedNotifications:YES];
	[[NSNotificationCenter defaultCenter] addObserver:view
											 selector:@selector(userScrolled:)
												 name:@"NSViewBoundsDidChangeNotification"
											   object:contentView];

	SetNSScrollView(scrollView);
	[topGraphview addSubview: scrollView];
	[view setFrameSizeToMinimum];
	setBounds(mLayout.bounds);
	setVisibleFromParent();

}

void SCScrollView::add(SCView *inChild)
{
	inChild->mNext = mChildren;
	mChildren = inChild;
	mNumChildren++;
	inChild->mParent = this;
	inChild->mTop = this;
	inChild->refresh();
	[(SCGraphView*)GetNSView() setFrameSizeToMinimum];
}

void SCScrollView::drawIfNecessary(SCRect inDamage)
{
	// only draw when our own SCGraphView wants it
	// not for the top level SCGraphView
	// so this returns nothing
}

void SCScrollView::drawSubViewIfNecessary(SCRect inDamage)
{
	SCRect maxContentBounds;
	// its not my mBounds nor my drawBounds that we want to test against
	// but rather my contents' total bounds
	maxContentBounds = checkMinimumSize();
	// there might be a short cut to this

    if (SCRectsDoIntersect(inDamage, maxContentBounds) && mVisible) {
		draw(inDamage);
		SCView *child = mChildren;
		SCView *children[mNumChildren];
		int k = mNumChildren;
		while (child) {
			children[--k] = child;
			child = child->next();
		}
		for(int i=0; i < mNumChildren; i++ ) {
			child = children[i];
			child->drawIfNecessary(inDamage);
		};

		drawDisabled(inDamage);
		drawDragHilite(inDamage);
		drawFocus(inDamage);
    }
}

void SCScrollView::setBounds(SCRect inBounds)
{
	[mNSScrollView setFrame: SCtoNSRect(inBounds)];
}

SCRect SCScrollView::getBounds()
{
	return NStoSCRect([mNSScrollView frame]);
}

int SCScrollView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		refreshFocus();
		SCRect pbounds = mParent->getLayout().bounds; 	// relative origin fix
		screct.x += pbounds.x;
		screct.y += pbounds.y;

		setBounds(screct);
		refreshFocus();
		return errNone;
	}

	if (strcmp(name, "border")==0) {
		bool hasBorder = IsTrue(slot);
		if(hasBorder) {
			[mNSScrollView setBorderType:NSLineBorder];
		} else {
			[mNSScrollView setBorderType:NSNoBorder];
		}
		[(SCGraphView*)mNSView setFrameSizeToMinimum];
		refresh();
		return errNone;
	}

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		mVisible = visible;
		setVisibleFromParent();
		return errNone;
	}

	return SCScrollTopView::setProperty(symbol, slot);
}

void SCScrollView::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mNSScrollView setHidden:NO];
	} else {
		[mNSScrollView setHidden:YES];
	}
	SCContainerView::setVisibleFromParent();
	mTop->resetFocus();
}

SCScrollView::~SCScrollView()
{
	[mNSScrollView removeFromSuperview];
	[mNSScrollView release];
}

///////////////////////////////////////////////////////////////////////////////////////

SCView* NewSCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCSlider(inParent, inObj, inBounds);
}

SCSlider::SCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mStepSize(0.), mStepScale(0.), mKnob(0), mThumbSize(12.)
{
        mValue = 1.;
	setValue(0.0, false);
}

//int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);

void SCSlider::draw(SCRect inDamage)
{
    SCRect bounds = getDrawBounds();
    calcThumbRect(bounds);

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, rect);

    if (mBackgroundImage)
        mBackgroundImage->draw(cgc, rect);

#if 1
    QDDrawBevelRect(cgc, rect, 1, true);
#endif

    CGRect cgThumbRect = SCtoCGRect(mThumbRect);
    if (mKnob) mKnob->draw(cgc, cgThumbRect);
    QDDrawBevelRect(cgc, cgThumbRect, 2, false);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
    //drawBevelRect(SCtoQDRect(mThumbRect), 2, 0, SCtoQDColor(mKnobColor), 2);
}

bool SCSlider::setValue(double inValue, bool send)
{
    inValue = sc_clip(inValue, 0., 1.);
    if (mStepSize > 0.) {
        inValue = floor(inValue * mStepScale + 0.5) * mStepSize;
    }
    bool changed = inValue != mValue;
    if (changed) {
        mValue = inValue;
        refresh();

        if (send) sendMessage(s_doaction, 0, 0, 0);
    }
    return changed;
}

void SCSlider::setValueFromPoint(SCPoint point)
{
    double moveableRange, value;
	SCRect bounds = getDrawBounds();
    if (bounds.width > bounds.height) {
	moveableRange = bounds.width - mThumbSize - 2;
	value = (point.x - bounds.x - 1 - mThumbSize/2) / moveableRange;
    } else {
	moveableRange = bounds.height - mThumbSize - 2;
	value = 1. - (point.y - bounds.y - 1 - mThumbSize/2) / moveableRange;
    }
    setValue(value, true);
}

void SCSlider::calcThumbRect(SCRect bounds)
{
    double moveableRange;

    moveableRange = (bounds.width > bounds.height)
                  ?  bounds.width : bounds.height;
    moveableRange -= mThumbSize + 2;

    double offset = mValue * moveableRange;

    if (bounds.width > bounds.height) {
	mThumbRect = SCMakeRect(bounds.x + offset + 1, bounds.y + 1,
				mThumbSize, bounds.height - 2);
    } else {
	mThumbRect = SCMakeRect(bounds.x + 1, bounds.y + bounds.height - offset - 1 - mThumbSize,
				bounds.width - 2, mThumbSize);
    }
}

void SCSlider::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
        setValueFromPoint(where);
    }
}

int SCSlider::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	if (symbol == s_value) {
		double value;
		err = slotDoubleVal(slot, &value);
                if (err) return err;
		bool changed = setValue(value, false);
                SetBool(slot, changed);
		return errNone;
	}
	char *name = symbol->name;
	if (strcmp(name, "knobColor")==0) {
		err = slotBackgroundVal(slot, &mKnob);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "step")==0) {
		err = slotDoubleVal(slot, &mStepSize);
		if (!err) {
			mStepScale = 1. / mStepSize;
			bool changed = setValue(mValue, false);
			SetBool(slot, changed);
		}
		return errNone;
	}
	if (strcmp(name, "thumbSize")==0) {
		err = slotFloatVal(slot, &mThumbSize);
		if (err) return err;
		refresh();
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}

int SCSlider::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_value) {
        SetFloat(slot, mValue);
           return errNone;
	}
	char *name = symbol->name;
        if (strcmp(name, "step")==0) {
            SetFloat(slot, mStepSize);
            return errNone;
        }
        if (strcmp(name, "thumbSize")==0) {
	    SetFloat(slot, mThumbSize);
	    return errNone;
        }

        return SCView::getProperty(symbol, slot);
}



bool SCSlider::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCSlider::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////

SCView* NewSCRangeSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCRangeSlider(inParent, inObj, inBounds);
}

SCRangeSlider::SCRangeSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mStepSize(0.), mStepScale(0.)
{
	mKnob = new HiliteGradientBackground(SCMakeColor(0,0,0.5,1), SCMakeColor(0.5,0.5,1,1), grad_Narrow, 24);
        mLo = -1.;
        mHi = -1.;
	setValue(0.0, 0.0, false);
}

//int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);

void SCRangeSlider::draw(SCRect inDamage)
{
    calcRangeRect();
    SCRect bounds = getDrawBounds();

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);

    CGRect cgBounds = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, cgBounds);

	if (mBackgroundImage)
        mBackgroundImage->draw(cgc, cgBounds);

    QDDrawBevelRect(cgc, cgBounds, 1, true);

    CGRect cgRangeRect = SCtoCGRect(mRangeRect);
    if (mKnob) mKnob->draw(cgc, cgRangeRect);
    QDDrawBevelRect(cgc, cgRangeRect, 1, false);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 2, 1, SCtoQDColor(mBackColor), 2);
    //drawBevelRect(SCtoQDRect(mRangeRect), 1, 0, SCtoQDColor(mKnobColor), 2);
}

bool SCRangeSlider::setValue(double inLo, double inHi, bool send)
{
    inLo = sc_clip(inLo, 0., 1.);
    inHi = sc_clip(inHi, 0., 1.);

    if (mStepSize > 0.) {
        inLo = floor(inLo * mStepScale + 0.5) * mStepSize;
        inHi = floor(inHi * mStepScale + 0.5) * mStepSize;
    }
    bool changed = inLo != mLo || inHi != mHi;
    if (changed) {
        mLo = inLo;
        mHi = inHi;
        refresh();

        if (send) sendMessage(s_doaction, 0, 0, 0);
    }
    return changed;
}

void SCRangeSlider::setValueFromPoint(SCPoint where)
{
    double moveableRange, lo, hi;
    SCRect bounds = getDrawBounds();

    if (bounds.width > bounds.height) {
        moveableRange = bounds.width - 5;
        lo = (sc_min(mAnchor.x, where.x) - bounds.x - 2) / moveableRange;
        hi = (sc_max(mAnchor.x, where.x) - bounds.x - 2) / moveableRange;
    } else {
        moveableRange = bounds.height - 5;
        lo = 1. - (sc_max(mAnchor.y, where.y) - bounds.y - 2) / moveableRange;
        hi = 1. - (sc_min(mAnchor.y, where.y) - bounds.y - 2) / moveableRange;
    }
    setValue(lo, hi, true);
}

void SCRangeSlider::calcRangeRect()
{
    double moveableRange;
    SCRect bounds = getDrawBounds();
    moveableRange = (bounds.width > bounds.height)
                  ?  bounds.width : bounds.height;
    moveableRange -= 5;

    double lo = mLo * moveableRange;
    double hi = mHi * moveableRange + 1;

    if (bounds.width > bounds.height) {
       mRangeRect = SCMakeRect(bounds.x + lo + 2, bounds.y + 1,
                     hi - lo, bounds.height - 2);
    } else {
       mRangeRect = SCMakeRect(bounds.x + 1, bounds.y + bounds.height - hi - 2,
                     bounds.width - 2, hi - lo);
    }
}

void SCRangeSlider::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    mAnchor = where;
    // sc.solar: allow drag
    //setValueFromPoint(where);
}

void SCRangeSlider::moveRangeFromPoint(SCPoint where)
{
    double moveableRange, lo, hi, pos, range;
    SCRect bounds = getDrawBounds();
    if (bounds.width > bounds.height) {
        moveableRange = bounds.width - 5;
        pos = (where.x - bounds.x - 2) / moveableRange;
        range = mHi - mLo;
        if (pos-(range/2) < 0.0) {
            lo = 0;
            hi = range;
        } else if (pos+(range/2) > 1.0) {
            hi = 1;
            lo = 1 - range;
        } else {
            lo = pos-(range/2);
            hi = pos+(range/2);
        }
    } else {
        moveableRange = bounds.height - 5;
        pos = 1.0 - (where.y - bounds.y - 2) / moveableRange;
        range = mHi - mLo;
        if (pos-(range/2) < 0.0) {
            lo = 0;
            hi = range;
        } else if (pos+(range/2) > 1.0) {
            hi = 1;
            lo = 1 - range;
        } else {
            lo = pos-(range/2);
            hi = pos+(range/2);
        }
    }
    setValue(lo, hi, true);
}

void SCRangeSlider::adjustLoFromPoint(SCPoint where)
{
    double moveableRange, lo, hi, pos;
    SCRect bounds = getDrawBounds();
    if (bounds.width > bounds.height) {
        moveableRange = bounds.width - 5;
        pos = (where.x - bounds.x - 2) / moveableRange;
        lo = pos;
        hi = mHi;
    } else {
        moveableRange = bounds.height - 5;
        pos = (where.y - bounds.y - 2) / moveableRange;
        lo = pos;
        hi = mHi;
    }
    setValue(lo, hi, true);
}
void SCRangeSlider::adjustHiFromPoint(SCPoint where)
{
    double moveableRange, lo, hi, pos;
    SCRect bounds = getDrawBounds();
    if (bounds.width > bounds.height) {
        moveableRange = bounds.width - 5;
        pos = (where.x - bounds.x - 2) / moveableRange;
        lo = mLo;
        hi = pos;
    } else {
        moveableRange = bounds.height - 5;
        pos = (where.y - bounds.y - 2) / moveableRange;
        lo = mLo;
        hi = pos;
    }
    setValue(lo, hi, true);
}

void SCRangeSlider::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else if  (modifiers & NSControlKeyMask) {
        moveRangeFromPoint(where);
    } else if (modifiers & NSShiftKeyMask) {
        adjustLoFromPoint(where);
    } else if (modifiers & NSAlternateKeyMask) {
        adjustHiFromPoint(where);
    } else {
        setValueFromPoint(where);
    }
}

int SCRangeSlider::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	if (symbol == s_lo || symbol == s_value) {
		double lo;
		err = slotDoubleVal(slot, &lo);
                if (err) return err;
		bool changed = setValue(lo, mHi, false);
                SetBool(slot, changed);
		return errNone;
	}
	if (symbol == s_hi) {
		double hi;
		err = slotDoubleVal(slot, &hi);
                if (err) return err;
		bool changed = setValue(mLo, hi, false);
                SetBool(slot, changed);
		return errNone;
	}
	if (symbol == s_range) {
		double range;
		err = slotDoubleVal(slot, &range);
                if (err) return err;
		bool changed = setValue(mLo, mLo + range, false);
                SetBool(slot, changed);
		return errNone;
	}
	char *name = symbol->name;
	if (strcmp(name, "step")==0) {
		err = slotDoubleVal(slot, &mStepSize);
		if (!err) {
			mStepScale = 1. / mStepSize;
			bool changed = setValue(mLo, mHi, false);
			SetBool(slot, changed);
		}
		return errNone;
	}
	if (strcmp(name, "knobColor")==0) {
		err = slotBackgroundVal(slot, &mKnob);
		if (err) return err;
		refresh();
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}

int SCRangeSlider::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_lo || symbol == s_value) {
            SetFloat(slot, mLo);
           return errNone;
	}
	if (symbol == s_hi) {
            SetFloat(slot, mHi);
           return errNone;
	}
	if (symbol == s_range) {
            SetFloat(slot, mHi - mLo);
           return errNone;
	}
	char *name = symbol->name;
        if (strcmp(name, "step")==0) {
            SetFloat(slot, mStepSize);
            return errNone;
        }

        return SCView::getProperty(symbol, slot);
}


bool SCRangeSlider::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCRangeSlider::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////

SCView* NewSC2DSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SC2DSlider(inParent, inObj, inBounds);
}

SC2DSlider::SC2DSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mStepSize(0.), mStepScale(0.), mKnob(0)
{
        mX = -1.;
        mY = -1.;
	setValue(0.0, 0.0, false);
}

//int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);

void SC2DSlider::draw(SCRect inDamage)
{
    calcThumbRect();

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(getDrawBounds());
    if (mBackground) mBackground->draw(cgc, rect);

    if (mBackgroundImage)
        mBackgroundImage->draw(cgc, rect);

    QDDrawBevelRect(cgc, rect, 1, true);

    CGRect cgThumbRect = SCtoCGRect(mThumbRect);
    if (mKnob) mKnob->draw(cgc, cgThumbRect);
    QDDrawBevelRect(cgc, cgThumbRect, 2, false);
    CGContextRestoreGState(cgc);

//    drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
//    drawBevelRect(SCtoQDRect(mThumbRect), 2, 0, SCtoQDColor(mKnobColor), 2);
}

bool SC2DSlider::setValue(double inX, double inY, bool send)
{
    inX = sc_clip(inX, 0., 1.);
    inY = sc_clip(inY, 0., 1.);
    if (mStepSize > 0.) {
        inX = floor(inX * mStepScale + 0.5) * mStepSize;
        inY = floor(inY * mStepScale + 0.5) * mStepSize;
    }
    bool changed = inX != mX || inY != mY;
    if (changed) {
        mX = inX;
        mY = inY;
        refresh();

        if (send) sendMessage(s_doaction, 0, 0, 0);
    }
    return changed;
}

const int THUMBSIZE = 12;

void SC2DSlider::setValueFromPoint(SCPoint where)
{
    SCRect bounds = getDrawBounds();
    double x = (where.x - bounds.x - 1 - THUMBSIZE/2) / (bounds.width - THUMBSIZE - 2);
    double y = 1. - (where.y - bounds.y - 1 - THUMBSIZE/2) / (bounds.height - THUMBSIZE - 2);
    setValue(x, y, true);
}

void SC2DSlider::calcThumbRect()
{
    SCRect bounds = getDrawBounds();
    double x = mX * (bounds.width - THUMBSIZE - 2);
    double y = mY * (bounds.height - THUMBSIZE - 2);

    mThumbRect = SCMakeRect(bounds.x + x + 1, bounds.y + bounds.height - y - 1 - THUMBSIZE, THUMBSIZE, THUMBSIZE);
}

void SC2DSlider::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
        setValueFromPoint(where);
    }
}

int SC2DSlider::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	if (symbol == s_x) {
		double x;
		err = slotDoubleVal(slot, &x);
                if (err) return err;
		bool changed = setValue(x, mY, false);
                SetBool(slot, changed);
		return errNone;
	}
	if (symbol == s_y) {
		double y;
		err = slotDoubleVal(slot, &y);
                if (err) return err;
		bool changed = setValue(mX, y, false);
                SetBool(slot, changed);
		return errNone;
	}
	char *name = symbol->name;
	if (strcmp(name, "knobColor")==0) {
		err = slotBackgroundVal(slot, &mKnob);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "step")==0) {
		err = slotDoubleVal(slot, &mStepSize);
		if (!err) {
			mStepScale = 1. / mStepSize;
			bool changed = setValue(mX, mY, false);
			SetBool(slot, changed);
		}
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}

int SC2DSlider::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_x) {
            SetFloat(slot, mX);
           return errNone;
	}
	if (symbol == s_y) {
            SetFloat(slot, mY);
           return errNone;
	}
	char *name = symbol->name;
        if (strcmp(name, "step")==0) {
            SetFloat(slot, mStepSize);
            return errNone;
        }

        return SCView::getProperty(symbol, slot);
}

bool SC2DSlider::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SC2DSlider::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

SCView* NewSCScope(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCScope(inParent, inObj, inBounds);
}

SCScope::SCScope(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mBufNum(0), mStyle(0), mGridOn(true)
{
	mZoom = mInvZoom = SCMakePoint(1.,1.);
	mScroll = SCMakePoint(0.,0.);
	memset(&mSndBuf, 0, sizeof(SndBuf));
	mGridColor = SCMakeColor(0.2,0.2,1.0, 1.0);
    mBackground = new SolidColorBackground(
                    SCMakeColor(0.,0.,0., 1.0));
	SCColor waveColor = SCMakeColor(1.0,1.0,0.0, 1.0);
	for (int i=0; i<kMaxScopeChannels; ++i) mWaveColors[i] = waveColor;
	startAnimation();
}

SCScope::~SCScope()
{
	stopAnimation();
	free(mSndBuf.data);
}

int getScopeBuf(uint32 index, SndBuf *buf, bool& didChange);

void SCScope::animate()
{
	bool didChange;
	/*int err =*/ getScopeBuf(mBufNum, &mSndBuf, didChange);
	if (didChange) refresh();
}

void SCScope::draw0(CGContextRef cgc)
{
	float *data = mSndBuf.data;
	if (!data) return;

	int samples = mSndBuf.samples;
	SCRect bounds = getDrawBounds();
	// draw scope.
    CGRect rect = SCtoCGRect(bounds);
	int width = (int)bounds.width - 2;
	int channels = sc_min(mSndBuf.channels, kMaxScopeChannels);
	//post("channels %d\n", channels);
	float chanHeight = (bounds.height - 2.) / channels;
	float chanHeight2 = 0.5 * chanHeight;
	float yScale = mZoom.y * chanHeight2;

	for (int j=0; j<channels; ++j)
	{
		CGContextSetRGBFillColor(cgc, mWaveColors[j].red, mWaveColors[j].green, mWaveColors[j].blue, mWaveColors[j].alpha);
		float fframe = mScroll.x;
		float hzoom = mZoom.x;
		int iframe = (int)floor(fframe);
		int isample = iframe * channels;
		float val = -data[isample + j];

		CGRect chanRect;
		chanRect.origin.x = rect.origin.x + 1.;
		chanRect.origin.y = rect.origin.y + 1. + chanHeight * j + chanHeight2;
		chanRect.size.width = width;
		chanRect.size.height = chanHeight;

	//post("width %d\n", width);
		for (int i = 0; i < width && isample < samples; i++)
		{
			float ymin, ymax;
			ymin = ymax = val;
			float nextfframe = fframe + hzoom;
			int nextiframe = (int)floor(nextfframe);
			int nscan = nextiframe - iframe;
			for (int k=0; k<nscan && isample < samples; ++k)
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

void SCScope::draw1(CGContextRef cgc)
{
	float *data = mSndBuf.data;
	if (!data) return;

	int samples = mSndBuf.samples;
	SCRect bounds = getDrawBounds();

	// draw scope.
    CGRect rect = SCtoCGRect(bounds);
	int width = (int)bounds.width - 2;
	int channels = sc_min(mSndBuf.channels, kMaxScopeChannels);
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

void SCScope::draw2(CGContextRef cgc)
{
	float *data = mSndBuf.data;
	if (!data) return;

	int samples = mSndBuf.samples;
	SCRect bounds = getDrawBounds();

	// draw scope.
    CGRect rect = SCtoCGRect(bounds);
	int channels = sc_min(mSndBuf.channels, kMaxScopeChannels);
	float height = rect.size.height - 2.;
	float height2 = 0.5 * height;
	float width = mBounds.width - 2.;
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



void SCScope::draw(SCRect inDamage)
{
	bool didChange;
	int err = getScopeBuf(mBufNum, &mSndBuf, didChange);
	if (err) return;
	SCRect bounds = getDrawBounds();
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, rect);

    if (mBackgroundImage)
        mBackgroundImage->draw(cgc, rect);

    QDDrawBevelRect(cgc, rect, 1, true);

	// draw grid.

	switch (mStyle) {
		case 0 : draw0(cgc); break;
		case 1 : draw1(cgc); break;
		case 2 : draw2(cgc); break;
	}

    CGContextRestoreGState(cgc);
}


void SCScope::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
}

int SCScope::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	if (symbol == s_x) {
		double x;
		err = slotDoubleVal(slot, &x);
		if (err) return err;
		mScroll.x = x;
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
	if (strcmp(name, "xZoom")==0) {
		double x;
		err = slotDoubleVal(slot, &x);
		if (err) return err;
		mZoom.x = x;
		mInvZoom.x = 1./x;
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
	if (strcmp(name, "bufnum")==0) {
		err = slotIntVal(slot, &mBufNum);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "gridColor")==0) {
		err = slotColorVal(slot, &mGridColor);
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

	return SCView::setProperty(symbol, slot);
}

int SCScope::getProperty(PyrSymbol *symbol, PyrSlot *slot)
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
	if (strcmp(name, "bufnum")==0) {
		SetInt(slot, mBufNum);
		return errNone;
	}
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
		refresh();
		return errNone;
	}

	return SCView::getProperty(symbol, slot);
}

///////////////////////////////////////////////////////////////////////////////////////

SCView* NewSC2DTabletSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SC2DTabletSlider(inParent, inObj, inBounds);
}
SC2DTabletSlider::SC2DTabletSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SC2DSlider(inParent, inObj, inBounds), mClipInBounds(1)
{
}

int SC2DTabletSlider::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
    char *name = symbol->name;
    if (strcmp(name, "clipInBounds")==0) {
        slotIntVal(slot, &mClipInBounds);
        return errNone;
    }
    return SC2DSlider::setProperty(symbol, slot);
}

bool SC2DTabletSlider::setValue(double inX, double inY,bool )
{
    if(mClipInBounds) {
        inX = sc_clip(inX, 0., 1.);
        inY = sc_clip(inY, 0., 1.);
    }
    if (mStepSize > 0.) {
        inX = floor(inX * mStepScale + 0.5) * mStepSize;
        inY = floor(inY * mStepScale + 0.5) * mStepSize;
    }
    bool changed = inX != mX || inY != mY;
    if (changed) {
        mX = inX;
        mY = inY;
        refresh();
    }
    return changed;
}

void SC2DTabletSlider::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
        setValueFromPoint(where);
        TABLETTRACK(s_mouseDown,mX,mY)
    }
}

void SC2DTabletSlider::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
   if (! (modifiers & NSCommandKeyMask)) {
        setValueFromPoint(where);
        TABLETTRACK(s_doaction,mX,mY)
    }
}

void SC2DTabletSlider::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        receiveDrag();
    } else {
        setValueFromPoint(where);
        TABLETTRACK(s_mouseUp,mX,mY)
    }
}
void SC2DTabletSlider::mouseDownAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
}
void SC2DTabletSlider::mouseMoveAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
}
void SC2DTabletSlider::mouseUpAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCButton(inParent, inObj, inBounds);
}

SCButton::SCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mNumStates(0), mStates(0), mPushed(false)
{
	setValue(0, false, 0);
        strcpy(mFontName, "Helvetica");
        mFontSize = 12.;
}

SCButton::~SCButton()
{
    delete [] mStates;
}

void SCButton::draw(SCRect inDamage)
{
    SCColor buttonColor;
	SCRect bounds = getDrawBounds();

    if (mStates) {
        SCButtonState *state = mStates + mValue;
        buttonColor = state->mButtonColor;
        //drawBevelRect(SCtoQDRect(mBounds), 2, mPushed ? 1 : 0, SCtoQDColor(buttonColor), 2);

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);

    CGRect cgrect = SCtoCGRect(bounds);
    if (buttonColor.alpha > 0.0) {
        CGContextSetRGBFillColor(cgc, buttonColor.red, buttonColor.green, buttonColor.blue, buttonColor.alpha);
        CGContextFillRect(cgc, cgrect);
    }
    QDDrawBevelRect(cgc, cgrect, 2, mPushed);
    CGContextRestoreGState(cgc);

        SCRect innerBounds = bounds;
        int inset = 2;
        int pushOffset = mPushed ? 2 : 0;
        innerBounds.x += inset + pushOffset;
        innerBounds.y += inset + pushOffset;
        innerBounds.width -= inset * 2 + pushOffset;
        innerBounds.height -= inset * 2 + pushOffset;
        stringDrawCenteredInRect(state->mLabel, innerBounds, mFontName, mFontSize, state->mLabelColor);
    }
}

bool SCButton::setValue(int inValue, bool send, int modifiers)
{
    bool changed = inValue != mValue;
    PyrSlot args[1];
    if (inValue < 0 || inValue >= mNumStates) inValue = 0;
    if (inValue != mValue || mNumStates < 2) {
        mValue = inValue;
        refresh();
        SetInt(args, modifiers);
        if (send) sendMessage(s_doaction, 1, args, 0);
    }
    return changed;
}


void SCButton::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
		bool inside = hit(where);
		if (inside != mPushed) {
			mPushed = inside;
			refresh();
		}
	}
}

void SCButton::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    bool inside = hit(where);
    if (inside) setValue(mValue+1, true, modifiers);
    mPushed = false;
}

int SCMakeButtonState(SCButton* view, SCButtonState *inState, PyrSlot *slot)
{
    int err;
    if (!isKindOfSlot(slot, class_array)) return errWrongType;
    PyrSlot *slots = slotRawObject(slot)->slots;

    inState->mLabel[0] = 0;
    inState->mLabelColor = SCMakeColor(0,0,0,1); // black
    inState->mButtonColor = SCMakeColor(0.7,0.7,0.7,1);

    if (slotRawObject(slot)->size < 1) return errNone;
    err = slotStrVal(slots+0, inState->mLabel, kLabelSize);
    if (err) return err;

    if (slotRawObject(slot)->size < 2) return errNone;
    err = slotColorVal(slots+1, &inState->mLabelColor);
    if (err) {
        inState->mLabelColor = SCMakeColor(0,0,0,1); // black
    }

    if (slotRawObject(slot)->size < 3) return errNone;
    err = slotColorVal(slots+2, &inState->mButtonColor);
    if (err) {
        //inState->mButtonColor = view->getBackColor();
    }
    return errNone;
}

int SCButton::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	if (symbol == s_value) {
            int value;
            err = slotIntVal(slot, &value);
            if (err) return err;
            bool changed = setValue(value, false, 0);
            SetBool(slot, changed);
            return errNone;
	}
	char *name = symbol->name;
	if (strcmp(name, "font")==0) {
            if (!isKindOfSlot(slot, getsym("SCFont")->u.classobj)) return errWrongType;
            PyrSlot *slots = slotRawObject(slot)->slots;

            float fontSize;
            err = slotFloatVal(slots+1, &fontSize);
            if (err) return err;

            err = slotStrVal(slots+0, mFontName, kFontNameSize);
            if (err) return err;

            mFontSize = fontSize;
            return errNone;
        }
	if (strcmp(name, "states")==0) {
            if (!isKindOfSlot(slot, class_array)) return errWrongType;

            // wipe out old
            delete [] mStates;
            mStates = 0;
            mNumStates = 0;

            PyrObject *array = slotRawObject(slot);
            int numStates = array->size;
            SCButtonState* states = new SCButtonState[numStates];
            if (!states) return errFailed;
            for (int i=0; i<numStates; ++i) {
                SCButtonState *state = states + i;
                PyrSlot *slot = array->slots + i;
                err = SCMakeButtonState(this, state, slot);
                if (err) {
                    delete [] states;
                    return err;
                }
            }
            mStates = states;
            mNumStates = numStates;
            return errNone;
	}

        return SCView::setProperty(symbol, slot);
}

int SCButton::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_value) {
            SetInt(slot, mValue);
            return errNone;
	}
	//char *name = symbol->name;

        return SCView::getProperty(symbol, slot);
}

bool SCButton::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCButton::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////

@implementation SCNSMenu
- (id) init
{
	if ([super init])
	{
		current = 0;
	}

	return self;
}

- (void) selectedItem:(id)item
{
	NSMenuItem *s = (NSMenuItem *) item;
	current = [s tag];
}

- (int) current
{
	return current;
}
@end

SCView* NewSCPopUpMenu(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCPopUpMenu(inParent, inObj, inBounds);
}

SCPopUpMenu::SCPopUpMenu(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mValue(0), mMenu(0)
{
	mStringColor = SCMakeColor(0,0,0,1);
	//mArrowColor = SCMakeColor(0,0,0,0);
	strcpy(mFontName, "Helvetica");
	mFontSize = 12.;
}

SCPopUpMenu::~SCPopUpMenu()
{
    if (mMenu) [mMenu release];
}

void SCPopUpMenu::draw(SCRect inDamage)
{
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
	SCRect dbounds = getDrawBounds();

    CGRect rect = SCtoCGRect(dbounds);
    if (mBackground) mBackground->draw(cgc, rect);

    if (mBackgroundImage)
		mBackgroundImage->draw(cgc, rect);

    CGRect bounds = SCtoCGRect(dbounds);
    QDDrawBevelRect(cgc, bounds, 1, false);

	CGContextSetRGBFillColor(cgc, mStringColor.red, mStringColor.green, mStringColor.blue, mStringColor.alpha);

	int vc = (int)(bounds.origin.y + bounds.size.height/2);
	int hc = (int)(bounds.origin.x + 7);
    CGContextMoveToPoint(cgc, hc-3, vc-3);

    CGContextAddLineToPoint(cgc, hc+3, vc-3);
    CGContextAddLineToPoint(cgc, hc, vc+3);
    CGContextAddLineToPoint(cgc, hc-3, vc-3);
    CGContextFillPath(cgc);

    CGContextRestoreGState(cgc);

    if (mMenu) {
		int numberOfItems = [mMenu numberOfItems];
		if (numberOfItems) {
			char *cstring = NULL;
			NSMenuItem *item = [mMenu itemAtIndex:mValue];
			if (item)
			{
				NSString *title = [item title];
				if (title)
				{
					int length = [title lengthOfBytesUsingEncoding:NSMacOSRomanStringEncoding];
					cstring = (char *) malloc(length+1);
					[title getCString:cstring maxLength:length+1 encoding:NSMacOSRomanStringEncoding];
				}
			}
			SCRect innerBounds = dbounds;
			int inset = 2;
			innerBounds.x += inset + 10;
			innerBounds.y += inset;
			innerBounds.width -= inset * 2 + 10;
			innerBounds.height -= inset * 2;
			stringDrawCenteredInRect((const char *) (cstring ? cstring : "[text not retrieved]"), innerBounds, mFontName, mFontSize, mStringColor);
			if (cstring) free(cstring);
		}
    }
}

bool SCPopUpMenu::setValue(int inValue, bool send)
{
	if (!mMenu) return false;

    bool changed = inValue != mValue;
	int numberOfItems = [mMenu numberOfItems];
	inValue = sc_mod(inValue, numberOfItems);
    if (inValue != mValue || numberOfItems < 2) {
        mValue = inValue;
        refresh();
        if (send) sendMessage(s_doaction, 0, 0, 0);
    }
    return changed;
}


void SCPopUpMenu::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
		if (!mMenu) return;
		SCRect bounds = getDrawBounds();

		[(SCGraphView*)mTop->GetNSView() startMenuTracking: this];

		int numberOfItems = [mMenu numberOfItems];
		for (int i=0; i<numberOfItems; ++i) {
			[[mMenu itemAtIndex:i] setState:mValue==i];
		}

		NSPoint p = NSMakePoint(bounds.x, bounds.y);
		NSView *view = mTop->GetNSView();
		[NSMenu popUpContextMenu:mMenu withEvent:theEvent forView:view];
		setValue([mMenu current], true);
	}
}

int SCPopUpMenu::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	if (symbol == s_value) {
            int value;
            err = slotIntVal(slot, &value);
            if (err) return err;
            bool changed = setValue(value, false);
            SetBool(slot, changed);
            return errNone;
	}
	char *name = symbol->name;
	if (strcmp(name, "font")==0) {
            if (!isKindOfSlot(slot, getsym("SCFont")->u.classobj)) return errWrongType;
            PyrSlot *slots = slotRawObject(slot)->slots;

            float fontSize;
            err = slotFloatVal(slots+1, &fontSize);
            if (err) return err;

            err = slotStrVal(slots+0, mFontName, kFontNameSize);
            if (err) return err;

            mFontSize = fontSize;
            return errNone;
        }
	if (strcmp(name, "items")==0) {
            if (!isKindOfSlot(slot, class_array)) return errWrongType;

            // wipe out old
            if (mMenu) [mMenu release];

            PyrObject *array = slotRawObject(slot);
            int numItems = array->size;

            mMenu = [[SCNSMenu alloc] init];

            for (int i=0; i<numItems; ++i) {
                PyrSlot *slot = array->slots + i;

				char title[256];
				int err = slotStrVal(slot, title, 255);
				if (err) return err;

				NSString *t = [NSString stringWithCString:(const char *)title encoding:NSMacOSRomanStringEncoding];
				NSMenuItem *item = [mMenu addItemWithTitle:t action:@selector(selectedItem:) keyEquivalent:@""];
				[item setTarget:mMenu];
				[item setTag:i];
            }
			refresh();
            return errNone;
	}
	if (strcmp(name, "stringColor")==0) {
		err = slotColorVal(slot, &mStringColor);
                if (err) return err;
                refresh();
		return errNone;
	}

        return SCView::setProperty(symbol, slot);
}

int SCPopUpMenu::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_value) {
            SetInt(slot, mValue);
            return errNone;
	}
	char* name = symbol->name;
	if (strcmp(name, "stringColor")==0) {
            return setSlotColor(slot, &mStringColor);
	}

        return SCView::getProperty(symbol, slot);
}


bool SCPopUpMenu::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCPopUpMenu::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

/////////////////////////////////////////////////////////////////
//SCMultiSliderView by jan trutzschler jan.21.03 //jan.23.03 changed draw added more prop.//jan.28.03 debug
//jan.29.03 value 1.0 up // draw . //feb.03.02 ..

int allocSlotDoubleArrayVal(PyrSlot *slot, double **arr);
int allocSlotDoubleArrayVal(PyrSlot *slot, double **arr)
{
    int len;
	int err;
    if (*arr) {
		delete [] *arr;
		*arr = 0;
	}
    if (isKindOfSlot(slot, class_array)) {
            len = slotRawObject(slot)->size;
            *arr = new double[len];
           // memcpy(*arr, slotRawObject(slot)->slots, len * sizeof(double));
			for(int i =0; i<len; i++){
				err = slotDoubleVal(slotRawObject(slot)->slots+i, *arr+i);
				if (err) *(*arr+i) = 0.0;
			}
            return errNone;
    }
    return errWrongType;
}
SCView* NewSCMultiSliderView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCMultiSliderView(inParent, inObj, inBounds);
}

SCMultiSliderView::SCMultiSliderView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mThumbSize(12.), mStepSize(0.),
		mStepScale(0.), mKnob(0), mXOffset(1)
{
        mTabSize = (int) (inBounds.width / mThumbSize);
        mCurrentIndex = mStartIndex = 0;
        mThumbSizeY = mThumbSize;
        mSelectionSize = 1;
        mCurrentY = 0.0;
        mCurrentX = 0.0;
        mYValues = new double [mTabSize];
        memset(mYValues, 0, mTabSize * sizeof(double));
        mReadOnly = mShowIndex = false;
        mFillColor = SCMakeColor(0,0,0,1); // black
        mStrokeColor = SCMakeColor(0,0,0,1); // black
        mIsFilled = mDrawLinesActive = false;
        mDrawRectsActive = true;
        mSecYValues = NULL;
        mIsHorizontal = true;
		mElasticMode = 0;
        //setVisibleSize();
}
SCMultiSliderView::~SCMultiSliderView()
{
    delete mYValues;
    if(mSecYValues) delete mSecYValues;

}

void SCMultiSliderView::draw(SCRect inDamage)
{
	SCRect bounds = getDrawBounds();

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(bounds);
    CGContextClipToRect(cgc, rect);
    double * vals = mYValues;
	int xpixels = mTabSize;
    int xstart = mStartIndex;
	if(mElasticMode) {
		if(mIsHorizontal) mElasticIndexStep =  (bounds.width - 2)/(mTabSize-mStartIndex);
		else mElasticIndexStep = (bounds.height - 2)/(mTabSize-mStartIndex);
	} else mElasticIndexStep = 0.0;
	double step = mElasticIndexStep;

    if (mBackground) mBackground->draw(cgc, rect);

    if (mBackgroundImage)
        mBackgroundImage->draw(cgc, rect);

    QDDrawBevelRect(cgc, rect, 1, true);

    CGContextSetRGBFillColor(cgc, mFillColor.red, mFillColor.green, mFillColor.blue, mFillColor.alpha);
    CGContextSetRGBStrokeColor(cgc, mStrokeColor.red, mStrokeColor.green, mStrokeColor.blue, mStrokeColor.alpha);

    CGRect drawRect;
    float yval = 0;

     //draw dots for multislider
    if(mDrawRectsActive)
    {
		int j = 0;
		for(int i = xstart; i < xpixels; i++, j++){
            drawRect = SCtoCGRect(calcThumbRect(j, vals[i], step));
            CGContextFillRect (cgc, drawRect);
            if (mThumbSize > 1.)	CGContextStrokeRect(cgc,drawRect);
			if(mSecYValues){ //draw the upper part of a wave view
				drawRect = SCtoCGRect(calcThumbRect(j, mSecYValues[i], step));
                CGContextFillRect (cgc, drawRect);
                if (mThumbSize > 1.)	CGContextStrokeRect(cgc,drawRect);
			}
        }
    }
    //draw lines
    if (mDrawLinesActive) {
        float xadd, gcx, gcy;
		if(mElasticMode){
			xadd = (step > mThumbSize) ? (mThumbSize * 0.5) : (step>1.0) ? (step* 0.5) : 1;
		}else{
			if(mThumbSize > 1) xadd =  mThumbSize * 0.5;
			else xadd = 0;
		}
		int j = 0;
		for(int i = xstart; i < xpixels; i++, j++){
            yval = vals[i];
            drawRect = SCtoCGRect(calcThumbRect(j, yval, step));
            gcx =  drawRect.origin.x + xadd;
            gcy =  drawRect.origin.y;
			if( i == xstart)
                CGContextMoveToPoint(cgc,  gcx, gcy);
			else
                CGContextAddLineToPoint(cgc, gcx, gcy);
        }
		//these are fixed values: used for the max of soundfiles, need to go backwards to fill
        if (mSecYValues != NULL) {
            int j = xpixels - 1 - xstart;
            for(int i = xpixels - 1; i >= xstart; i--, j--){
                yval = mSecYValues[i];
                drawRect = SCtoCGRect(calcThumbRect(j, yval, step));
                gcx =  drawRect.origin.x + xadd;
                gcy =  drawRect.origin.y + xadd;
                CGContextAddLineToPoint(cgc, gcx, gcy);
            }
        }
        if(mIsFilled) CGContextEOFillPath(cgc);
            else CGContextStrokePath(cgc);
    }
    //draw selection:
    if(mShowIndex) {
        if(mCurrentIndex + mSelectionSize >= xstart){
            int currentI, selsize;
            if(mCurrentIndex < xstart) {
                currentI = 0;
                selsize = sc_clip(mSelectionSize, xstart, mTabSize);
            } else {
                currentI = mCurrentIndex - xstart;
                selsize = mSelectionSize;
            }
            if(mIsHorizontal){
				if(mElasticMode)
				drawRect = CGRectMake(bounds.x + 1 + (currentI * mElasticIndexStep),
							bounds.y + 1,
							(selsize * mElasticIndexStep),
							bounds.height);
				else
                drawRect = CGRectMake(bounds.x + 1 + (currentI * ( mXOffset + mThumbSize)),
							bounds.y + 1,
							(selsize * ( mXOffset + mThumbSize)) - mXOffset,
							bounds.height);
            } else {
				if(mElasticMode)
				drawRect = CGRectMake( bounds.x + 1,
							bounds.y + 1 + (currentI * mElasticIndexStep),
							bounds.width,
							(selsize * mElasticIndexStep));
				else
                drawRect = CGRectMake( bounds.x + 1,
							bounds.y + 1 + (currentI * ( mXOffset + mThumbSize)),
							bounds.width,
							(selsize * ( mXOffset + mThumbSize))  - mXOffset);
            }
            CGContextSetRGBFillColor(cgc, mFillColor.red, mFillColor.green, mFillColor.blue, 0.4);
            CGContextFillRect (cgc, drawRect);
        }
    }
    CGContextRestoreGState(cgc);

}

bool SCMultiSliderView::setValue(int xIn, double yIn, bool send)
{
    bool changed;
    yIn = sc_clip(yIn, 0., 1.);
    xIn = sc_clip(xIn, 0, mTabSize - 1);

    if (mStepSize > 0.) {
        yIn = floor(yIn * mStepScale + 0.5) * mStepSize;
    }
    if(!mReadOnly){
        mCurrentIndex = xIn;
        changed = mYValues[xIn] != yIn;
        if (changed) {
            mYValues[xIn] = yIn;
            mCurrentY = yIn;
            if (send) sendMessage(s_doaction, 0, 0, 0);
            // post("new val: %f  x: %d \n", (float) yIn, xIn);
            refresh();
        }
    } else {
        int xindx, lastindx, maxSize;
        lastindx = mCurrentIndex;
        changed = false;
        xindx = (int) xIn ;
        xindx = sc_clip(xindx, 0, mTabSize - 1);
        mCurrentIndex = xindx;
        mCurrentY = mYValues[mCurrentIndex];
        if(mCurrentIndex > (lastindx + mSelectionSize)) mSelectionSize = 1;
        maxSize = mTabSize - mCurrentIndex;
        if(mSelectionSize > maxSize) mSelectionSize = maxSize;

        if (send) sendMessage(s_doaction, 0, 0, 0);
        //post("readOnly: x: %d \n", mCurrentIndex);
        if(mShowIndex)	refresh();
    }
    return changed;

}

int SCMultiSliderView::indexFromPoint(SCPoint where)
{
	SCRect bounds = getDrawBounds();

    if(mIsHorizontal){
		if(mElasticMode)
			return (int) ((where.x - bounds.x -1)/mElasticIndexStep);
		else
			return (int) ((where.x - bounds.x -1)/(mThumbSize + mXOffset)) + mStartIndex;
    } else {
        return (int) ((where.y - bounds.y -1)/(mThumbSize + mXOffset)) + mStartIndex;
    }
}

double SCMultiSliderView::valueFromPoint(SCPoint where)
{
	SCRect bounds = getDrawBounds();

    if(mIsHorizontal){
        return 1.0 - (where.y - bounds.y - 1 - mThumbSizeY/2) / (bounds.height - mThumbSizeY - 2);
    } else {
        return (where.x - bounds.x - 1 - mThumbSizeY/2) / (bounds.width - mThumbSizeY - 2);
    }
}

void SCMultiSliderView::setValueFromPoint(SCPoint where)
{
    double y = valueFromPoint(where);
    int xIndx = indexFromPoint(where);
    setValue(xIndx, y, true);
}

SCRect SCMultiSliderView::calcThumbRect(int indexIn, double valIn, double step)
{
    double x,y, thumbx, thumby;
	float xoffset = mXOffset;
	SCRect bounds = getDrawBounds();

	//post("step is: %f ", step);

    if(mIsHorizontal){
        thumby = mThumbSizeY;
		if(mElasticMode){
			thumbx = (step > mThumbSize) ? mThumbSize : (step>1.0) ? step : 1;
			x = indexIn * step ;
		}else{
			x = (double) indexIn * ( xoffset + mThumbSize);
			thumbx = mThumbSize;
		}
        y = valIn * (bounds.height - thumby - 2);
        y = bounds.y + bounds.height - y - 1 - thumby;
        x = bounds.x + x + 1;
        if(mIsFilled) thumby = bounds.height;
    } else {
        thumbx = mThumbSizeY;
		if(mElasticMode){
			thumbx = (step > mThumbSize) ? mThumbSize : step;
			y = indexIn * step ;
		}else{
			y = (double) indexIn * ( xoffset + mThumbSize);
		}
		x = valIn * (bounds.width - thumbx - 2);
        //x = bounds.x + bounds.width - x - 1 - thumby;
		x = bounds.x + x + 1;
		y = bounds.y + y + 1;
        if(mIsFilled){
            thumbx += x -  bounds.x;
            x = bounds.x;
        }
        thumby = mThumbSize;
    }
    return SCMakeRect( x, y, thumbx, thumby);
}

void SCMultiSliderView::setSelection(SCPoint where)
{
    int wx;
    int visiIn = mCurrentIndex - mStartIndex;
    int maxSize = mTabSize - mCurrentIndex;
	wx = indexFromPoint(where);
    if(wx > visiIn) mSelectionSize = (int) wx - visiIn;
        else {
            setValueFromPoint(where);
            mSelectionSize = (int) mCurrentIndex - wx;
        }
    if(mSelectionSize > maxSize) mSelectionSize = maxSize;
    refresh();
}

void SCMultiSliderView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	mPrevPoint = where;
	mouseTrack(where, modifiers,theEvent);
}
void SCMultiSliderView::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent){
//    sendMessage(getsym("mouseEndTrack"), 0, 0, 0);
    mouseTrack(where, modifiers,theEvent);
}
void SCMultiSliderView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else if (modifiers & NSShiftKeyMask) {
        if(mShowIndex) {
            setSelection(where);
            sendMessage(getsym("doAction"), 0, 0, 0);
        }
    } else if (modifiers & NSControlKeyMask) {
        if(mIsHorizontal) mCurrentX = where.x; //absolute y mouse position
        else mCurrentX = where.y;
        sendMessage(getsym("doMetaAction"), 0, 0, 0);
    } else {
		int prevIndex = indexFromPoint(mPrevPoint);
		int index = indexFromPoint(where);
		double prevValue = valueFromPoint(mPrevPoint);
		double value = valueFromPoint(where);
		if (prevIndex == index) {
			setValue(index, value, true);
		} else if (prevIndex < index) {
			double val = prevValue;
			double delta = (value - prevValue) / (index - prevIndex);
			for (int i=prevIndex; i<=index; ++i) {
				setValue(i, val, true);
				val += delta;
			}
		} else {
			double val = value;
			double delta = (prevValue - value) / (prevIndex - index);
			for (int i=index; i<=prevIndex; ++i) {
				setValue(i, val, true);
				val += delta;
			}
		}
		mPrevPoint = where;
    }
}

int SCMultiSliderView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
    char *name = symbol->name;
    if (strcmp(name, "step")==0) {
		err = slotDoubleVal(slot, &mStepSize);
		if (!err) {
			mStepScale = 1. / mStepSize;
			bool changed = false;
			bool changedout = false;
			for(int i=0; i< mTabSize; ++i){
				changed = setValue(i, mYValues[i], false);
				if(changed) changedout = changed;
			}
			SetBool(slot, changedout);
		}
		return errNone;
	}
    if (strcmp(name, "thumbSize")==0) {
        err = slotFloatVal(slot, &mThumbSize);
        //setVisibleSize();
        refresh();
        return errNone;
    }
    if (strcmp(name, "thumbWidth")==0) {
        err = slotFloatVal(slot, &mThumbSizeY);
        //setVisibleSize();
        refresh();
        return errNone;
    }
    if (strcmp(name, "indexThumbSize")==0) {
        err = slotFloatVal(slot, &mThumbSize);
        //setVisibleSize();
        refresh();
        return errNone;
    }
    if (strcmp(name, "valueThumbSize")==0) {
        err = slotFloatVal(slot, &mThumbSizeY);
        //setVisibleSize();
        refresh();
        return errNone;
    }
    if (strcmp(name, "xOffset")==0) {
        err = slotFloatVal(slot, &mXOffset);
        if(mXOffset < 1.0) mXOffset = 0.0; //mResamp = (int) 1.0 / mXOffset;
        //setVisibleSize();
        refresh();
        return errNone;
    }
    if (strcmp(name, "fillColor")==0) {
        err = slotColorVal(slot, &mFillColor);
        refresh();
        return errNone;
    }
    if (strcmp(name, "strokeColor")==0) {
        err = slotColorVal(slot, &mStrokeColor);
        refresh();
        return errNone;
    }
    if (strcmp(name, "startIndex")==0) {
        err = slotIntVal(slot, &mStartIndex);
	 //setVisibleSize();
        refresh();
        return errNone;
    }
    if (strcmp(name, "readOnly")==0) {
        mReadOnly = IsTrue(slot);
        refresh();
        return errNone;
    }
    if (strcmp(name, "isHorizontal")==0) {
        mIsHorizontal = IsTrue(slot);
        refresh();
        return errNone;
    }
    if (strcmp(name, "drawLines")==0) {
        mDrawLinesActive = IsTrue(slot);
        refresh();
        return errNone;
    }
    if (strcmp(name, "drawRects")==0) {
        mDrawRectsActive = IsTrue(slot);
        refresh();
        return errNone;
    }
    if (strcmp(name, "isFilled")==0) {
        mIsFilled = IsTrue(slot);
        refresh();
        return errNone;
    }
    if (strcmp(name, "showIndex")==0) {
        mShowIndex = IsTrue(slot);
        refresh();
        return errNone;
    }
    if (symbol == s_x) {
        slotIntVal(slot, &mCurrentIndex);
        mCurrentIndex  = sc_clip(mCurrentIndex, 0, mTabSize - 1);
        int maxSize = mTabSize - mCurrentIndex;
        if(mSelectionSize > maxSize) mSelectionSize = maxSize;
        refresh();
        return errNone;
	}
	if (symbol == s_y) {
        slotDoubleVal(slot, &mCurrentY);
        mCurrentY = sc_clip(mCurrentY, 0.0, 1.0);
        mYValues[mCurrentIndex] = mCurrentY;
        refresh();
        return errNone;
	}
    if (symbol == s_value) {
        if (!isKindOfSlot(slot, class_array)) return errWrongType;
        if(slotRawObject(slot)->size != mTabSize) {
            err = allocSlotDoubleArrayVal(slot, &mYValues);
            mTabSize = slotRawObject(slot)->size;
			SetBool(slot, true);

        } else {
            int len = slotRawObject(slot)->size;
			double val;
			bool changed = false;
           // memcpy(mYValues, slotRawObject(slot)->slots, len * sizeof(double));
			for(int i =0; i<len; i++){
				err = slotDoubleVal(slotRawObject(slot)->slots+i, &val);
				if(mYValues[i] != val) changed = true;
				if (err) mYValues[i] = 0.0; else mYValues[i] = val;
			}
			SetBool(slot, changed);

        }
        //setVisibleSize();
        refresh();
        return errNone;
	}
    if (strcmp(name, "referenceValues")==0) {
        if (!isKindOfSlot(slot, class_array)) return errWrongType;
        if(slotRawObject(slot)->size == mTabSize && mSecYValues != NULL) {
            err = allocSlotDoubleArrayVal(slot, &mSecYValues);
        } else {
			err = allocSlotDoubleArrayVal(slot, &mSecYValues);
        }
        refresh();
        return errNone;
	}
    if (strcmp(name, "selectionSize")==0) {
        slotIntVal(slot, &mSelectionSize);
        refresh();
        return errNone;
    }

    if (strcmp(name, "elasticResizeMode")==0) {
        slotIntVal(slot, &mElasticMode);
        refresh();
        return errNone;
    }



    return SCView::setProperty(symbol, slot);
}

int SCMultiSliderView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{

	char *name = symbol->name;
        if (strcmp(name, "step")==0) {
            SetFloat(slot, mStepSize);
            return errNone;
        }
        if (symbol == s_x) {
            SetInt(slot, mCurrentIndex);
           return errNone;
	}
	if (symbol == s_y) {
            SetFloat(slot, mCurrentY);
           return errNone;
	}
        if (symbol == s_value) {
              int len;
            if (isKindOfSlot(slot, class_array)) {
                len = slotRawObject(slot)->size;
                if(len > mTabSize)  return errWrongType;
                memcpy(slotRawObject(slot)->slots, mYValues, len * sizeof(double));
                }
            return errNone;
	}
        if (strcmp(name, "referenceValues")==0) {
              int len;
            if (isKindOfSlot(slot, class_array)) {
                len = slotRawObject(slot)->size;
                if(len > mTabSize)  return errWrongType;
                if (mSecYValues != NULL)
                memcpy(slotRawObject(slot)->slots, mSecYValues, len * sizeof(double));
                else SetNil(slot);
                }
            return errNone;
	}
        if (strcmp(name, "selectionSize")==0) {
            SetInt(slot, mSelectionSize);
            return errNone;
        }
        if (strcmp(name, "absoluteX")==0) {
            SetInt(slot, (int) mCurrentX);
            return errNone;
        }


        return SCView::getProperty(symbol, slot);
}

bool SCMultiSliderView::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCMultiSliderView::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}
//////////////////////////////////////////////////////////////////////////////////


SCView* NewSCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCUserView(inParent, inObj, inBounds);
}

SCUserView::SCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds)
{
	mClearOnRefresh = true;
	mDrawingEnabled = true;
	mNSImageForLayering =  nil;
	for(char i = 0; i<kFrameLastTimes; i++) {
		mFrameLastTimes[i] = (float)i;
	}
	mFrameRate = 0.f;
	mFrameCounter = 0;
}

SCUserView::~SCUserView()
{
	if( mNSImageForLayering ) 
		[ mNSImageForLayering release ];
}


extern NSBitmapImageRep* CreateBitmapContextRGBA(const int width, const int height);
extern NSBitmapImageRep* BitmapContextRGBAWithImage(const int width, const int height, id image);

void SCUserView::draw(SCRect inDamage)
{
	
#define USE_BITMAPREP		0
	
	NSGraphicsContext	*lGCtx = NULL;
	CGContextRef		mainGC = NULL;
	SCRect				bounds = getDrawBounds();
	CGRect				cgBounds = SCtoCGRect( bounds );
	
	SCView::draw(bounds); // draw background
	
	lGCtx = [NSGraphicsContext currentContext];
	mainGC = (CGContextRef)[lGCtx graphicsPort];
	
	if( !mDrawingEnabled || !mainGC )
		return;
	
	if( bounds.width <= 0 || bounds.height <= 0) 
		return;
	
	mFrameRate = 0.f;
	for( uint8_t i = 1; i < kFrameLastTimes; ++i ) 
	{
		mFrameRate = mFrameRate + ( mFrameLastTimes[i] - mFrameLastTimes[i-1] );
		mFrameLastTimes[i-1] = mFrameLastTimes[i];
	}
	
	mFrameRate = 1.f / ( mFrameRate / (kFrameLastTimes-1) );
	++mFrameCounter;
	
	struct timeval tv;
	gettimeofday(&tv, 0);
	mFrameLastTimes[kFrameLastTimes-1] = (double)tv.tv_sec + 1.0e-6 * (double)tv.tv_usec;
	
	CGContextSaveGState( mainGC );
	if( !mClearOnRefresh )
	{
		if(
		   !mNSImageForLayering ||
		   [ mNSImageForLayering size ].width != bounds.width ||
		   [ mNSImageForLayering size ].height != bounds.height		   
		) 
		{
			if( mNSImageForLayering )
				[ mNSImageForLayering release ];
			
			mNSImageForLayering = [[NSImage alloc]initWithSize: *(NSSize*)&cgBounds.size];
			[mNSImageForLayering setFlipped:YES]; // important to provide compatibility with SC flipped view
			if( !mNSImageForLayering )
			{
				post( "Error: Failed creating valid NSBitmapImageRep for SCUserView" );
				return;
			}
			
#if USE_BITMAPREP
			mImageRep = CreateBitmapContextRGBA((int)bounds.width, (int)bounds.height);
			if( !mImageRep )
			{
				post( "Error: Failed creating valid NSBitmapImageRep for SCUserView" );
				return;
			}
			[mNSImageForLayering addRepresentation: mImageRep];
			[mImageRep release];
#else
			[mNSImageForLayering setCacheMode: NSImageCacheBySize]; // should be also bitmap based
#endif
		}
		[mNSImageForLayering lockFocus];
		
	}
	else
	{
		CGContextClipToRect( mainGC, cgBounds );
		CGContextTranslateCTM( mainGC, cgBounds.origin.x, cgBounds.origin.y );
	}
	
    sendMessage(s_draw, 0, 0, 0);
	
	if( !mClearOnRefresh )
	{	
		[mNSImageForLayering unlockFocus];
		NSAffineTransform* trs = [NSAffineTransform transform];
		[trs translateXBy:cgBounds.origin.x yBy:cgBounds.origin.y + cgBounds.size.height];
		[trs scaleXBy:1.0 yBy:-1.0];
		[trs concat];
		[mNSImageForLayering drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.f];
	}
	CGContextRestoreGState( mainGC );
}

void SCUserView::clearDrawing()
{
	if( mClearOnRefresh ) 
		return;

	//SCRect bounds = getDrawBounds();
	//CGContextClearRect ( mNSImageForLayering ,  CGRectMake( 0, 0, bounds.width, bounds.height));
	[ mNSImageForLayering lockFocus ];
	CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext]graphicsPort];
	CGContextClearRect( cgc, CGRectMake( 0.f, 0.f, [mNSImageForLayering size].width, [mNSImageForLayering size].height ) );
	[ mNSImageForLayering unlockFocus ];
	
}

void SCUserView::mouseAction(PyrSymbol *method, SCPoint where, int modifiers)
{
//	SCRect bounds = getDrawBounds();
//
//	mRealtiveMousePoint.x = where.x - bounds.x;
//	mRealtiveMousePoint.y = where.y - bounds.y;

//    PyrSlot args[3];
//    SetFloat(args+0, where.x);
//    SetFloat(args+1, where.y);
//    SetInt(args+2, modifiers);
//    sendMessage(method, 3, args, 0);
}

void SCUserView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	SCRect bounds = getDrawBounds();

	mRealtiveMousePoint.x = where.x - bounds.x;
	mRealtiveMousePoint.y = where.y - bounds.y;
//    mouseAction(getsym("mouseBeginTrack"), where, modifiers);
}

  void SCUserView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
  {
//  	SCRect bounds = getDrawBounds();
//	mRealtiveMousePoint.x = where.x - bounds.x;
//	mRealtiveMousePoint.y = where.y - bounds.y;
//	if (modifiers & NSCommandKeyMask) {
//		beginDrag(where);
//	}

//	else {
//	mouseAction(getsym("mouseTrack"), where, modifiers);
//	}
  }

void SCUserView::mouseDownAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	SCRect bounds = getDrawBounds();

	if(parent()->isTopContainer()) {
		mRealtiveMousePoint.x = where.x - bounds.x;
		mRealtiveMousePoint.y = where.y - bounds.y;
	}else{
		mRealtiveMousePoint.x = where.x - mBounds.x;
		mRealtiveMousePoint.y = where.y - mBounds.y;
	}

	SCView::mouseDownAction(where, modifiers, theEvent);

	if (modifiers & NSCommandKeyMask) {
		beginDrag(where);
	}
}

void SCUserView::mouseMoveAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	SCRect bounds = getDrawBounds();

	if(parent()->isTopContainer()) {
		mRealtiveMousePoint.x = where.x - bounds.x;
		mRealtiveMousePoint.y = where.y - bounds.y;
	}else{
		mRealtiveMousePoint.x = where.x - mBounds.x;
		mRealtiveMousePoint.y = where.y - mBounds.y;
	}

	SCView::mouseMoveAction( where, modifiers, theEvent);

}

void SCUserView::mouseUpAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	SCRect bounds = getDrawBounds();

	if(parent()->isTopContainer()) {
		mRealtiveMousePoint.x = where.x - bounds.x;
		mRealtiveMousePoint.y = where.y - bounds.y;
	}else{
		mRealtiveMousePoint.x = where.x - mBounds.x;
		mRealtiveMousePoint.y = where.y - mBounds.y;
	}

	SCView::mouseUpAction( where, modifiers, theEvent);

}

void SCUserView::mouseOver(SCPoint where, int modifiers, NSEvent *theEvent )
{
	SCRect bounds = getDrawBounds();

	if(parent()->isTopContainer()) {
		mRealtiveMousePoint.x = where.x - bounds.x;
		mRealtiveMousePoint.y = where.y - bounds.y;
	}else{
		mRealtiveMousePoint.x = where.x - mBounds.x;
		mRealtiveMousePoint.y = where.y - mBounds.y;
	}

	SCView::mouseOver( where, modifiers, theEvent);
}

void SCUserView::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
//    mouseAction(getsym("mouseEndTrack"), where, modifiers);
}

void SCUserView::keyDown(int character, int modifiers)
{
}

void SCUserView::keyUp(int character, int modifiers)
{
}

bool SCUserView::canReceiveDrag()
{
	PyrSlot result;
	sendMessage(s_canReceiveDrag, 0, 0, &result);
	return IsTrue(&result);
}

void SCUserView::receiveDrag()
{
	PyrSlot args[2];
	SetFloat(args+0, mRealtiveMousePoint.x);
	SetFloat(args+1, mRealtiveMousePoint.y); 
	sendMessage(s_receiveDrag, 2, args, 0);
}

int SCUserView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

    if (strcmp(name, "clearOnRefresh")==0) {
		mClearOnRefresh = IsTrue(slot);
		return errNone;
    }
	if (strcmp(name, "clearDrawing")==0) {
		clearDrawing();
		return errNone;
	}
	if (strcmp(name, "drawingEnabled")==0) {
		mDrawingEnabled = IsTrue(slot);
		return errNone;
	}
	if (strcmp(name, "animate")==0) {
		if (IsTrue(slot)) {
			startAnimation();
		} else {
			stopAnimation();
		}
		return errNone;
	}
	
    return SCView::setProperty(symbol, slot);
}

int SCUserView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

    if (strcmp(name, "mousePosition")==0) {
		if(isKindOfSlot(slot, s_point->u.classobj)){

			PyrSlot *slots = slotRawObject(slot)->slots;
			SetFloat(slots+0, mRealtiveMousePoint.x);
			SetFloat(slots+1, mRealtiveMousePoint.y);
		}
		return errNone;
    }
	if (strcmp(name, "frame")==0) {
		SetInt(slot, mFrameCounter);
		return errNone;
	}
	if (strcmp(name, "frameRate")==0) {
		SetFloat(slot, mFrameRate);
		return errNone;
	}

    return SCView::getProperty(symbol, slot);
}

void SCUserView::refreshInRect(SCRect b)
{
	b.x += mLayout.bounds.x;
	b.y += mLayout.bounds.y;
	SCView::refreshInRect(b);
}


////////////////////////////////////////////////

SCView* NewSCStaticText(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCStaticText(inParent, inObj, inBounds);
}

SCStaticText::SCStaticText(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mString(0), mAlignment(kSCAlignLeft)
{
        strcpy(mFontName, "Helvetica");
        mFontSize = 12.;
        mStringColor = SCMakeColor(0,0,0,1);
        mEnabled = false;
        mCanFocus = false;
}

SCStaticText::~SCStaticText()
{
    delete [] mString;
}

bool SCStaticText::shouldDim()
{
    return false;
}

void SCStaticText::draw(SCRect inDamage)
{
	SCView::draw(inDamage);
	SCRect bounds = getDrawBounds();

	drawString(bounds);
}

void SCStaticText::drawString(SCRect bounds)
{
    if (mString) {
        if (mAlignment < 0) {
            stringDrawLeftInRect(mString, bounds, mFontName, mFontSize, mStringColor);
        } else if (mAlignment > 0) {
            stringDrawRightInRect(mString, bounds, mFontName, mFontSize, mStringColor);
        } else {
            stringDrawCenteredInRect(mString, bounds, mFontName, mFontSize, mStringColor);
        }
    }
}

int allocSlotStrVal(PyrSlot *slot, char **str)
{
    int len;
	if (*str) {
		delete [] *str;
		*str = 0;
	}
    if (IsSym(slot)) {
            len = strlen(slotRawSymbol(slot)->name);
            *str = new char[len+1];
            strcpy(*str, slotRawSymbol(slot)->name);
            return errNone;
    } else if (isKindOfSlot(slot, class_string)) {
            len = slotRawObject(slot)->size;
            *str = new char[len+1];
            memcpy(*str, slotRawString(slot)->s, len);
            (*str)[len] = 0;
            return errNone;
    }
    return errWrongType;
}

int SCStaticText::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;
	if (strcmp(name, "string")==0) {
            err = allocSlotStrVal(slot, &mString);
            if (err) return err;
            refresh();
            return errNone;
        }
	if (strcmp(name, "font")==0) {
            if (!isKindOfSlot(slot, getsym("SCFont")->u.classobj)) return errWrongType;
            PyrSlot *slots = slotRawObject(slot)->slots;

            float fontSize;
            err = slotFloatVal(slots+1, &fontSize);
            if (err) return err;

            err = slotStrVal(slots+0, mFontName, kFontNameSize);
            if (err) return err;

            mFontSize = fontSize;
            refresh();
            return errNone;
        }
	if (strcmp(name, "stringColor")==0) {
		err = slotColorVal(slot, &mStringColor);
                if (err) return err;
                refresh();
		return errNone;
	}
	if (strcmp(name, "align")==0) {
                int align;
                if (IsSym(slot)) {
                    if (slotRawSymbol(slot)->name[0] == 'l') mAlignment = -1;
                    else if (slotRawSymbol(slot)->name[0] == 'r') mAlignment = 1;
                    else if (slotRawSymbol(slot)->name[0] == 'c') mAlignment = 0;
                    else return errFailed;
                } else {
                    err = slotIntVal(slot, &align);
                    if (err) return err;
                    mAlignment = align;
                }
                refresh();
		return errNone;
        }
        return SCView::setProperty(symbol, slot);
}

int SCStaticText::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

	if (strcmp(name, "stringColor")==0) {
            return setSlotColor(slot, &mStringColor);
	}

        return SCView::getProperty(symbol, slot);
}



////////////////////////////////////////////////

SCView* NewSCListView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCListView(inParent, inObj, inBounds);
}

SCListView::SCListView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mArray(0), mScroll(0), mAlignment(kSCAlignLeft)
{
        strcpy(mFontName, "Helvetica");
        mFontSize = 12.;
        mStringColor = SCMakeColor(0,0,0,1);
		mSelectedStringColor = SCMakeColor(1,1,1,1);
        mHiliteColor = SCMakeColor(0,0,0,1);
		mValue = 0;
		mItemBackgroundColor = NULL;
}

SCListView::~SCListView()
{
    if (mArray) CFRelease(mArray);
}

NSSize nsStringSize(NSString *nsstring, char *cFontName, float fontSize, SCColor sccolor);
NSRect SCtoNSRect(SCRect screct);

void SCListView::draw(SCRect inDamage)
{
	SCView::draw(inDamage);
	SCRect bounds = getDrawBounds();
	CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	CGContextSaveGState(cgc);

	CGRect bevelRect = SCtoCGRect(bounds);
	QDDrawBevelRect(cgc, bevelRect, 1, true);

	int numItems = mArray ? CFArrayGetCount(mArray) : 0;

	mStrSize = nsStringSize((NSString*)CFSTR("XWYjpgy"), mFontName, mFontSize, mStringColor);

	float itemHeight = mStrSize.height + 3.f;
	//float totalHeight = itemHeight * numItems;
	float visibleItems = bounds.height / itemHeight;
	float fraction = visibleItems / (float)numItems;

	float totalHeight = itemHeight * numItems;

	float maxScroll = totalHeight - bounds.height;
	if (maxScroll < 0) maxScroll = 0;
	mScroll = sc_clip(mScroll, 0, maxScroll);

	SCRect itemBounds = bounds;
	itemBounds.x += 4;
	itemBounds.y += 2 - mScroll;
	itemBounds.width -= 6; itemBounds.height -= 4;

	itemBounds.height = itemHeight;

	if (fraction < 1.f)
	{

		itemBounds.width -= 16;

		// draw slider
		SCRect sliderBounds = bounds;
		sliderBounds.x = bounds.x + bounds.width - 16;
		sliderBounds.width = 15;
		sliderBounds.y += 1;
		sliderBounds.height -= 2;
		bevelRect = SCtoCGRect(sliderBounds);
		QDDrawBevelRect(cgc, bevelRect, 1, true);

		float travel = (1.f - fraction) * (bounds.height - 4);
		SCRect thumbBounds = sliderBounds;
		thumbBounds.y = 1.f + sliderBounds.y + travel * (mScroll / maxScroll);
		thumbBounds.height = (sliderBounds.height - 2) * fraction;
		thumbBounds.x += 1;
		thumbBounds.width -= 2;
		bevelRect = SCtoCGRect(thumbBounds);
		QDDrawBevelRect(cgc, bevelRect, 2, false);
	}

	if (mArray) {
		CGRect clipRect = SCtoCGRect(bounds);
		clipRect.origin.y += 1;
		clipRect.size.height -= 2;
		CGContextClipToRect(cgc, clipRect);

		int minIndex = (int)((mScroll - itemHeight + 1) / itemHeight);
		int maxIndex = (int)(minIndex + 1 + (bounds.height + itemHeight - 1) / itemHeight);
		if (maxIndex > numItems - 1) maxIndex = numItems - 1;
		itemBounds.y += minIndex * itemHeight;


		for (int i = minIndex; i <= maxIndex; ++i)
		{
			NSString* nsstring = (NSString*)CFArrayGetValueAtIndex(mArray, i);
			NSSize size;
			if(mItemBackgroundColor){
				SCColor color = mItemBackgroundColor[i];
				CGContextSetRGBFillColor(cgc, color.red, color.green, color.blue, color.alpha);
				CGRect drawRect = SCtoCGRect(itemBounds);
				CGContextFillRect (cgc, drawRect);
			}
			if (i == mValue) {
				CGContextSetRGBFillColor(cgc, mHiliteColor.red, mHiliteColor.green, mHiliteColor.blue, mHiliteColor.alpha);
				CGRect drawRect = SCtoCGRect(itemBounds);
				CGContextFillRect (cgc, drawRect);
				nsStringDrawInRectAlign(nsstring, itemBounds, mFontName, mFontSize, mSelectedStringColor, mAlignment, -1, &size);
			} else {
				nsStringDrawInRectAlign(nsstring, itemBounds, mFontName, mFontSize, mStringColor, mAlignment, -1, &size);
			}

			itemBounds.y += itemBounds.height;
		}
		CGContextRestoreGState(cgc);
    }
}

void SCListView::scrollToValue()
{
	int numItems = mArray ? CFArrayGetCount(mArray) : 0;
	SCRect bounds = getDrawBounds();

	float itemHeight = mStrSize.height + 3.f;
	//float visibleItems = mBounds.height / itemHeight;
	//float fraction = visibleItems / (float)numItems;
	float totalHeight = itemHeight * numItems;

	float maxScroll = totalHeight - bounds.height;
	if (maxScroll < 0) maxScroll = 0;

	SCRect itemBounds = bounds;
	itemBounds.x += 4;
	itemBounds.y += 2 - mScroll;
	itemBounds.width -= 6; itemBounds.height -= 4;
	itemBounds.height = itemHeight;

	itemBounds.y += mValue * itemHeight;

	if (itemBounds.y < bounds.y) mScroll = mValue * itemHeight;
	else if (itemBounds.y + itemHeight > bounds.y + bounds.height) mScroll = (mValue + 1) * itemHeight - bounds.height;
}

bool SCListView::setValue(int inValue, bool send)
{
    bool changed = inValue != mValue;
	int numItems = mArray ? CFArrayGetCount(mArray) : 0;
	if (inValue < 0) inValue = numItems - 1;
    else if (inValue >= numItems) inValue = 0;
    if (inValue != mValue || numItems < 2) {
        mValue = inValue;

		scrollToValue();

        refresh();
        if (send) sendMessage(s_doaction, 0, 0, 0);
    }
    return changed;
}

int SCListView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;

	if (symbol == s_value) {
		int value;
		err = slotIntVal(slot, &value);
		if (err) return err;
		bool changed = setValue(value, false);
		SetBool(slot, changed);
		return errNone;
	}

	char *name = symbol->name;
	if (strcmp(name, "items")==0) {
		if (IsNil(slot)) {
			if (mArray) {
				CFRelease(mArray);
				mArray = NULL;
			}
			refresh();
			return errNone;
		}
		if (!isKindOfSlot(slot, class_array)) return errWrongType;

		// wipe out old
		if (mArray) {
			CFRelease(mArray);
			mArray = NULL;
		}

		PyrObject *array = slotRawObject(slot);
		int numItems = array->size;

		mArray = (CFMutableArrayRef)CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);

		for (int i=0; i<numItems; ++i) {
			PyrSlot *slot = array->slots + i;

			char cstr[1024];
			CFStringRef cfstr;
			int err = slotStrVal(slot, cstr, 1023);
			if (err) return err;

			cfstr = CFStringCreateWithCString(kCFAllocatorDefault, cstr, kCFStringEncodingUTF8);

			CFArrayAppendValue(mArray, cfstr);
			CFRelease(cfstr);
		}
		refresh();
		return errNone;
	}
	if (strcmp(name, "font")==0) {
		if (!isKindOfSlot(slot, getsym("SCFont")->u.classobj)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;

		float fontSize;
		err = slotFloatVal(slots+1, &fontSize);
		if (err) return err;

		err = slotStrVal(slots+0, mFontName, kFontNameSize);
		if (err) return err;

		mFontSize = fontSize;
		refresh();
		return errNone;
	}
	if (strcmp(name, "stringColor")==0) {
		err = slotColorVal(slot, &mStringColor);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "itemColors")==0) {
		if (IsNil(slot)) {
			if (mItemBackgroundColor) {
//				CFRelease(mItemBackgroundColor);
				delete(mItemBackgroundColor);
				mItemBackgroundColor = NULL;
			}
			refresh();
			return errNone;
		}
		if (!isKindOfSlot(slot, class_array)) return errWrongType;

		// wipe out old
		if (mItemBackgroundColor) {
//			CFRelease(mItemBackgroundColor);
			delete(mItemBackgroundColor);
			mItemBackgroundColor = NULL;
		}

		PyrObject *array = slotRawObject(slot);
		int numItems = array->size;

//		mItemBackgroundColor = (CFMutableArrayRef)CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
		mItemBackgroundColor = (SCColor*) malloc(numItems * sizeof(SCColor));
		for (int i=0; i<numItems; ++i) {
			PyrSlot *slot = array->slots + i;

			SCColor color;
			int err = slotColorVal(slot, &color);
			if (err) return err;
			mItemBackgroundColor[i] =  color;
		}
		refresh();
		return errNone;
	}

	if (strcmp(name, "selectedStringColor")==0) {
		err = slotColorVal(slot, &mSelectedStringColor);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "hiliteColor")==0) {
		err = slotColorVal(slot, &mHiliteColor);
		if (err) return err;
		refresh();
		return errNone;
	}
	if (strcmp(name, "align")==0) {
		int align;
		if (IsSym(slot)) {
			if (slotRawSymbol(slot)->name[0] == 'l') mAlignment = -1;
			else if (slotRawSymbol(slot)->name[0] == 'r') mAlignment = 1;
			else if (slotRawSymbol(slot)->name[0] == 'c') mAlignment = 0;
			else return errFailed;
		} else {
			err = slotIntVal(slot, &align);
			if (err) return err;
			mAlignment = align;
		}
		refresh();
		return errNone;
	}
	return SCView::setProperty(symbol, slot);
}

int SCListView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	if (symbol == s_value) {
            SetInt(slot, mValue);
            return errNone;
	}
	char *name = symbol->name;

	if (strcmp(name, "stringColor")==0) {
            return setSlotColor(slot, &mStringColor);
	}

	if (strcmp(name, "itemColors")==0) {
		int size, err;
		PyrSlot * slots;
		slots = slotRawObject(slot)->slots;
		size = slotRawObject(slot)->size;
		for(int i=0; i<size; i++)
		{
			err = setSlotColor(slots+i, &mItemBackgroundColor[i]);
			if(err) return err;
		}
		 return errNone;
	}

	return SCView::getProperty(symbol, slot);
}


bool SCListView::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCListView::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

void SCListView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{

	mAnchor = where;
	mAnchorScroll = mScroll;
	SCRect bounds = getDrawBounds();

	mStrSize = nsStringSize((NSString*)CFSTR("XWYjpgy"), mFontName, mFontSize, mStringColor);
	int numItems = mArray ? CFArrayGetCount(mArray) : 0;

	float itemHeight = mStrSize.height + 3.f;
	float visibleItems = bounds.height / itemHeight;
	float fraction = visibleItems / (float)numItems;

	mScrolling = fraction < 1.f && where.x >= bounds.x + bounds.width - 16;
	if (!mScrolling) {
		int value = (int)((where.y - bounds.y - 2 + mScroll) / itemHeight);
		setValue(value, true);
	}
}

void SCListView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
	} else {
		if (mScrolling) {
			SCRect bounds = getDrawBounds();

			int numItems = mArray ? CFArrayGetCount(mArray) : 0;

			float itemHeight = mStrSize.height + 3.f;
			float visibleItems = bounds.height / itemHeight;
			float fraction = visibleItems / (float)numItems;
			float totalHeight = itemHeight * numItems;
			float travel = (1.f - fraction) * (bounds.height - 4);

			float maxScroll = totalHeight - bounds.height;
			if (maxScroll < 0) maxScroll = 0;
			float sliderOffset = travel * (mAnchorScroll / maxScroll);

			float change = where.y - mAnchor.y;
			float newSliderOffset = sliderOffset + change;
			mScroll = maxScroll * newSliderOffset / travel;

			mScroll = sc_clip(mScroll, 0, maxScroll);

			refresh();
		}
    }
}

////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//SCEnvelopeView by jan trutzschler feb.02.03 // udated feb.22.03
int allocSlot2DDoubleArrayPyrSlot (PyrSlot *slot, double **arr, int position);
int allocSlot2DDoubleArrayPyrSlot(PyrSlot *slot, double **arr, int position)
{
	int len;
	if (*arr) {
		delete [] *arr;
		*arr = 0;
	}

	if (isKindOfSlot(slot, class_array)) {
			len = slotRawObject(slotRawObject(slot)->slots + position)->size;
			*arr = new double[len];
			memcpy(*arr, ((PyrDoubleArray*)slotRawObject(slotRawObject(slot)->slots+position))->d, len * sizeof(double));
			return errNone;
	}
	return errWrongType;
}
int allocEnvArray (int size, SCEnvObject **arr);
int allocEnvArray (int size, SCEnvObject **arr){

	* arr = new SCEnvObject[size];

	return errNone;
}
//int allocSlotEnvObjArray(PyrSlot *slot, SCEnvObject **arr);


SCView* NewSCEnvelopeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCEnvelopeView(inParent, inObj, inBounds);
}

SCEnvelopeView::SCEnvelopeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mThumbSize(5), mStepSize(0.),mStepScale(0.)
{

	mTabSize = 0;
	mActiveSize = mTabSize;
	mCurrentIndex = mStartIndex = 0;
	mThumbSizeY = mThumbSize;
	mSelectionSize = 1;
	mCurrentY = 0.0;
	mCurrentX = 0.0;
	mLastIndex = 0;
	mIsEnvView = true;
	mIsTracking = false;

	mEnvObj = 0;
	drawOrder = 0;

	mFillColor = mStrokeColor = SCMakeColor(0,0,0,1); // black
	mSelectedColor = SCMakeColor(1.f,0,0,1);
	mIsFilled = mDrawLinesActive = true;
	mDrawRectsActive = true;
//	setVisibleSize();
	mSelectedIndex = -1;
	mAbsoluteX = 0.0;
	mIsFixedSelection = false;

	strcpy(mFontName, "Helvetica");
	mFontSize = 12.;
	mDrawCenteredConnection = true;
	mGridOn = false;
	mGridColor = SCMakeColor(0, 0, 0.8, 0.3);
	mGrid = SCMakePoint(0.1, 0.1);
}
SCEnvelopeView::~SCEnvelopeView()
{

	for(int i = 0; i < mTabSize; i++){
		if( (mEnvObj+i)->mString)
			delete [] (mEnvObj+i)->mString;
		if((mEnvObj+i)->mConnections)
			delete [] (mEnvObj+i)->mConnections;
		/*
		if((mEnvObj+i)->mConnectionOutputs)
			delete [] (mEnvObj+i)->mConnectionOutputs;
		if((mEnvObj+i)->mConnectionInputs)
			delete [] (mEnvObj+i)->mConnectionInputs;
			*/

	 }

    delete [] mEnvObj;

	if(drawOrder) delete [] drawOrder;

}

int SCEnvelopeView::allocSlotEnvObjArray(PyrSlot *slot, SCEnvObject **arr)
{
	int size = slotRawObject(&(slotRawObject(slot)->slots[0]))->size;

	SCEnvObject * scenv = 0;
	int err = allocEnvArray(size, &scenv);
	if(err) return err;
    if (*arr) {
		if(size > mTabSize){
			memcpy(scenv, *arr, mTabSize * sizeof(SCEnvObject));
			for(int i=mTabSize; i< size; i++){
				(scenv+i)->mObjectColor = mFillColor;
				(scenv+i)->mString = 0;
				(scenv+i)->mConnections = 0;
				(scenv+i)->mColor = mFillColor;
				(scenv+i)->mEditable = true;
				(scenv+i)->mIsSelected = false;
				(scenv+i)->mNumConnection = 0;
				(scenv+i)->mRect.height = mThumbSizeY;
				(scenv+i)->mRect.width = mThumbSize;
				(scenv+i)->shape = 1;
				(scenv+i)->curve = 1.0;

			}
		} else { //meaning size < mTabSize
			memcpy(scenv, *arr, size * sizeof(SCEnvObject));
		}
		delete [] *arr;
		*arr = 0;

	} else {
		for(int i=0; i< size; i++){
			(scenv+i)->mObjectColor = mFillColor;
			(scenv+i)->mString = 0;
			(scenv+i)->mConnections = 0;
			(scenv+i)->mColor = mFillColor;
			(scenv+i)->mEditable = true;
			(scenv+i)->mIsSelected = false;
			(scenv+i)->mNumConnection = 0;
			(scenv+i)->mRect.height = mThumbSizeY;
			(scenv+i)->mRect.width = mThumbSize;
			(scenv+i)->shape = 1;
			(scenv+i)->curve = 1.0;
		}
	}

	err = allocEnvArray(size, arr);
	if(err) return err;
	memcpy(*arr, scenv, size * sizeof(SCEnvObject));
	delete [] scenv;
	return errNone;
}

// helper constants for shape calculation
#define		EXPM1	(float) exp(-1.f)
#define		EXPM1R	(float) (1.f - exp(-1.f))

inline double linlinNoClip(double x, double a, double b, double c, double d)
{
//    if (x <= a) return c;
//    if (x >= b) return d;
    return (x-a)/(b-a) * (d-c) + c;
}

void SCEnvelopeView::draw(SCRect inDamage)
{
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
	SCRect bounds = getDrawBounds();

    CGRect rect = SCtoCGRect(bounds);
    CGContextClipToRect(cgc, rect);

    int xpixels = mActiveSize;
    int xstart = 0;

    if (mBackground) mBackground->draw(cgc, rect);

    if (mBackgroundImage)
		mBackgroundImage->draw(cgc, rect);

    QDDrawBevelRect(cgc, rect, 1, true);



    CGRect drawRect;
    SCColor fillcolor;
    SCEnvObject * envobj;

	//draw grid
	if(mGridOn){
		float xgrid, ygrid, xgridpos, ygridpos;
		float maxwidth =  (bounds.width + bounds.x) - 2.f;
		float maxheight =  (bounds.height + bounds.y) - 2.f;

		xgrid = mGrid.x;
		ygrid = mGrid.y;

		CGContextSetRGBFillColor(cgc, mGridColor.red, mGridColor.green, mGridColor.blue, mGridColor.alpha);
		xgrid = xgrid*bounds.width;
		xgridpos = bounds.x+1.f + xgrid;
		ygrid = ygrid*bounds.height;
		ygridpos = bounds.y+1.f + ygrid;
		while(xgridpos < maxwidth && (xgrid > 0.0))
		{
			drawRect = CGRectMake(xgridpos,
								  bounds.y + 1.f,
								  1.f,
								  bounds.height-2.f);
			CGContextFillRect (cgc, drawRect);
			xgridpos += xgrid;
		}

		while(ygridpos < maxheight && (ygrid > 0.0))
		{
			drawRect = CGRectMake(bounds.x+1.f,
								  ygridpos,
								  bounds.width-2.f,
								  1.f);
			CGContextFillRect (cgc, drawRect);
			ygridpos += ygrid;
		}
	}

    CGContextSetRGBFillColor(cgc, mFillColor.red, mFillColor.green, mFillColor.blue, mFillColor.alpha);
    CGContextSetRGBStrokeColor(cgc, mStrokeColor.red, mStrokeColor.green, mStrokeColor.blue, mStrokeColor.alpha);

	//draw lines
    if (mDrawLinesActive) {

		int numConnection, num;

        for(int i = xstart; i < xpixels; i++){
			//	float gcoDx;
            envobj = &mEnvObj[i];
            setEnvRect(envobj->x, envobj->y, envobj);

			if(mIsEnvView){	//straight connection
				if(i == xstart)
                    CGContextMoveToPoint(cgc, envobj->mDrawPoint.x,  envobj->mDrawPoint.y);
                else {
					float  gcx, gcy, gcox, gcoy;
					SCEnvObject * conObj = (mEnvObj + (i-1));
					float curve;
					float dur, numpointsY, yLevel, pos;
					float begTime, endTime, begLevel, endLevel, stepsize, ar, br, yrange;
					int numsteps;
					float startPointX, endPointX, startPointY, endPointY;
					curve = conObj->curve;
					gcox = envobj->mDrawPoint.x;
					gcoy = envobj->mDrawPoint.y;
					gcx = conObj->mDrawPoint.x;
					gcy =  conObj->mDrawPoint.y;
//					post("shape at i: %i, is: %i\n", i, conObj->shape);
					switch(conObj->shape){
						case shape_Step:
							CGContextMoveToPoint(cgc, gcox, gcoy);
							CGContextAddLineToPoint(cgc, gcox, gcy);
							CGContextAddLineToPoint(cgc, gcx, gcy);
							break;
						case shape_Linear:
							CGContextMoveToPoint(cgc, gcox, gcoy);
							CGContextAddLineToPoint(cgc, gcx, gcy);
							break;

						case shape_Exponential:

							if(conObj->x <= envobj->x)
							{
								startPointX = gcx;
								endPointX = gcox;

								startPointY = gcy;
								endPointY= gcoy;

								begTime = conObj->x;
								begLevel = sc_clip(conObj->y, 0.000001f, 1.f);

								endTime = envobj->x;
								endLevel = sc_clip(envobj->y, 0.000001f, 1.f);

							} else {
								startPointX = gcox;
								endPointX = gcx;

								startPointY = gcoy;
								endPointY= gcy;

								begTime = envobj->x;
								begLevel = sc_clip(envobj->y, 0.000001f, 1.f);

								endTime = conObj->x;
								endLevel = sc_clip(conObj->y, 0.000001f, 1.f);
							}

							numsteps = (int) endPointX - startPointX;
							dur = endTime - begTime;
							stepsize = dur/numsteps;
							CGContextMoveToPoint(cgc, startPointX, startPointY);

							for(int i=1; i<=numsteps+1; i++){
								double step = sc_clip((i*stepsize), 0.0, dur); // need to clip when here when clipping endLevel or begLevel
								double pos = step  / dur;
								yLevel =  begLevel * pow(endLevel / begLevel, pos);
								CGContextAddLineToPoint(cgc, linlinNoClip((i*stepsize), 0.f, dur, startPointX, endPointX), linlinNoClip(yLevel, begLevel, endLevel, startPointY, endPointY));
							}
							CGContextAddLineToPoint(cgc, endPointX, endPointY);


							break;

						case shape_Sine:
							CGContextMoveToPoint(cgc, gcox, gcoy);
							CGContextAddCurveToPoint(cgc, gcox * EXPM1R + gcx * EXPM1, gcoy, gcox * EXPM1 + gcx * EXPM1R, gcy, gcx, gcy );
							break;

						case shape_Welch:
							CGContextMoveToPoint(cgc, gcox, gcoy);
							if( gcoy < gcy ) {
								CGContextAddCurveToPoint(cgc, gcox * 0.556f + gcx * 0.444f, gcoy * 0.278f + gcy * 0.722f, gcox * 0.29f + gcx * 0.71f, gcy, gcx, gcy );
							} else {
								CGContextAddCurveToPoint(cgc, gcx * 0.29f + gcox * 0.71f, gcoy, gcx * 0.556f + gcox * 0.444f, gcy * 0.278f + gcoy * 0.722f, gcx, gcy );
							}
							break;
						case shape_Curve:

							if(conObj->x <= envobj->x)
							{
								startPointX = gcx;
								endPointX = gcox;

								startPointY = gcy;
								endPointY= gcoy;

								begTime = conObj->x;
								begLevel = sc_clip(conObj->y, 0.000001f, 1.f);

								endTime = envobj->x;
								endLevel = sc_clip(envobj->y, 0.000001f, 1.f);

							} else {
								startPointX = gcox;
								endPointX = gcx;

								startPointY = gcoy;
								endPointY= gcy;

								begTime = envobj->x;
								begLevel = sc_clip(envobj->y, 0.000001f, 1.f);

								endTime = conObj->x;
								endLevel = sc_clip(conObj->y, 0.000001f, 1.f);
							}

							numsteps = (int) endPointX - startPointX;
							dur = endTime - begTime;
							stepsize = dur/numsteps;
							CGContextMoveToPoint(cgc, startPointX, startPointY);

							for(int i=1; i<=numsteps+1; i++){
								double step = sc_clip((i*stepsize), 0.0, dur); // need to clip when here when clipping endLevel or begLevel
								double pos = step  / dur;
								if (fabs(curve) < 0.0001) {
									yLevel = pos * (endLevel - begLevel) + begLevel;
								} else {
									double denom = 1. - exp(curve);
									double numer = 1. - exp(pos * curve);
									yLevel = begLevel + (endLevel - begLevel) * (numer/denom);
								}

								CGContextAddLineToPoint(cgc, linlinNoClip((i*stepsize), 0.f, dur, startPointX, endPointX), linlinNoClip(yLevel, begLevel, endLevel, startPointY, endPointY));
							}
							CGContextAddLineToPoint(cgc, endPointX, endPointY);


							break;

						case shape_Squared:
							CGContextMoveToPoint(cgc, gcox, gcoy);
							if( gcoy < gcy ) {
								CGContextAddCurveToPoint(cgc, gcox * 0.9f + gcx * 0.1f, gcoy, gcox * 0.5f + gcx * 0.5f, gcoy, gcx, gcy );
							} else {
								CGContextAddCurveToPoint(cgc,  gcx * 0.5f + gcox * 0.5f, gcy, gcx * 0.9f + gcox * 0.1f, gcy, gcx, gcy );
							}
							break;

						case shape_Cubed:
							CGContextMoveToPoint(cgc, gcox, gcoy);
							if( gcoy < gcy ) {
								CGContextAddCurveToPoint(cgc, gcox * 0.6667f + gcx * 0.3333f, gcoy, gcox * 0.3333f + gcx * 0.6667f, gcoy, gcx, gcy );
							} else {
								CGContextAddCurveToPoint(cgc, gcx * 0.3333f + gcox * 0.6667f, gcy, gcx * 0.6667f + gcox * 0.3333f, gcy, gcx, gcy );
							}
							break;

					}
				}
            } else {

				numConnection = envobj->mNumConnection;

				for(int j = 0; j < numConnection; j++){
					float  gcx, gcy, gcox, gcoy, curve;
					num = (int) (envobj->mConnections[j]);
					SCEnvObject * conObj = (mEnvObj + num);
					curve = conObj->curve;
//					post("shape at num: %i, is: %i\n", num, conObj->shape);
					gcox = envobj->mDrawPoint.x;
					gcoy = envobj->mDrawPoint.y;
					gcx = conObj->mDrawPoint.x;
					gcy =  conObj->mDrawPoint.y;
					if (num >=0 ){
						switch(conObj->shape){
							case shape_Step:
								CGContextMoveToPoint(cgc, gcox, gcoy);
								CGContextAddLineToPoint(cgc, gcox, gcy);
								CGContextAddLineToPoint(cgc, gcx, gcy);
								break;
							case shape_Linear:
								CGContextMoveToPoint(cgc, gcox, gcoy);
								CGContextAddLineToPoint(cgc, gcx, gcy);
								break;
							case shape_Sine:
								CGContextMoveToPoint(cgc, gcox, gcoy);
								CGContextAddCurveToPoint(cgc, gcox * EXPM1R + gcx * EXPM1, gcox, gcox * EXPM1 + gcx * EXPM1R, gcy, gcx, gcy );
								break;

							case shape_Exponential:
								float dur, numpointsY, yLevel, pos;
								float begTime, endTime, begLevel, endLevel, stepsize, ar, br, yrange;
								int numsteps;
								float startPointX, endPointX, startPointY, endPointY;

								if(conObj->x <= envobj->x)
								{
									startPointX = gcx;
									endPointX = gcox;

									startPointY = gcy;
									endPointY= gcoy;

									begTime = conObj->x;
									begLevel = sc_clip(conObj->y, 0.000001f, 1.f);

									endTime = envobj->x;
									endLevel = sc_clip(envobj->y, 0.000001f, 1.f);

								} else {
									startPointX = gcox;
									endPointX = gcx;

									startPointY = gcoy;
									endPointY= gcy;

									begTime = envobj->x;
									begLevel = sc_clip(envobj->y, 0.000001f, 1.f);

									endTime = conObj->x;
									endLevel = sc_clip(conObj->y, 0.000001f, 1.f);
								}

								numsteps = (int) endPointX - startPointX;
								dur = endTime - begTime;
								stepsize = dur/numsteps;
								CGContextMoveToPoint(cgc, startPointX, startPointY);

								for(int i=1; i<=numsteps+1; i++){
									double step = sc_clip((i*stepsize), 0.0, dur); // need to clip when here when clipping endLevel or begLevel
									double pos = step  / dur;
									yLevel =  begLevel * pow(endLevel / begLevel, pos);
									CGContextAddLineToPoint(cgc, linlinNoClip((i*stepsize), 0.f, dur, startPointX, endPointX), linlinNoClip(yLevel, begLevel, endLevel, startPointY, endPointY));
								}
								CGContextAddLineToPoint(cgc, endPointX, endPointY);


								break;

							case shape_Welch:
								CGContextMoveToPoint(cgc, gcox, gcoy);
								if( gcoy < gcy ) {
									CGContextAddCurveToPoint(cgc, gcox * 0.556f + gcx * 0.444f, gcoy * 0.278f + gcy * 0.722f, gcox * 0.29f + gcx * 0.71f, gcy, gcx, gcy );
								} else {
									CGContextAddCurveToPoint(cgc, gcx * 0.29f + gcox * 0.71f, gcoy, gcx * 0.556f + gcox * 0.444f, gcy * 0.278f + gcoy * 0.722f, gcx, gcy );
								}
								break;
							case shape_Curve:

								if(conObj->x <= envobj->x)
								{
									startPointX = gcx;
									endPointX = gcox;

									startPointY = gcy;
									endPointY= gcoy;

									begTime = conObj->x;
									begLevel = sc_clip(conObj->y, 0.000001f, 1.f);

									endTime = envobj->x;
									endLevel = sc_clip(envobj->y, 0.000001f, 1.f);

								} else {
									startPointX = gcox;
									endPointX = gcx;

									startPointY = gcoy;
									endPointY= gcy;

									begTime = envobj->x;
									begLevel = sc_clip(envobj->y, 0.000001f, 1.f);

									endTime = conObj->x;
									endLevel = sc_clip(conObj->y, 0.000001f, 1.f);
								}

								numsteps = (int) endPointX - startPointX;
								dur = endTime - begTime;
								stepsize = dur/numsteps;
								CGContextMoveToPoint(cgc, startPointX, startPointY);

								for(int i=1; i<=numsteps+1; i++){
									double step = sc_clip((i*stepsize), 0.0, dur); // need to clip when here when clipping endLevel or begLevel
									double pos = step  / dur;
									if (fabs(curve) < 0.0001) {
										yLevel = pos * (endLevel - begLevel) + begLevel;
									} else {
										double denom = 1. - exp(curve);
										double numer = 1. - exp(pos * curve);
										yLevel = begLevel + (endLevel - begLevel) * (numer/denom);
									}

									CGContextAddLineToPoint(cgc, linlinNoClip((i*stepsize), 0.f, dur, startPointX, endPointX), linlinNoClip(yLevel, begLevel, endLevel, startPointY, endPointY));
								}
								CGContextAddLineToPoint(cgc, endPointX, endPointY);


								break;

							case shape_Squared:
								CGContextMoveToPoint(cgc, gcox, gcoy);
								if( gcoy < gcy ) {
									CGContextAddCurveToPoint(cgc, gcox * 0.9f + gcx * 0.1f, gcoy, gcox * 0.5f + gcx * 0.5f, gcoy, gcx, gcy );
								} else {
									CGContextAddCurveToPoint(cgc,  gcx * 0.5f + gcox * 0.5f, gcy, gcx * 0.9f + gcox * 0.1f, gcy, gcx, gcy );
								}
								break;
							case shape_Cubed:
								CGContextMoveToPoint(cgc, gcox, gcoy);
								if( gcoy < gcy ) {
									CGContextAddCurveToPoint(cgc, gcox * 0.6667f + gcx * 0.3333f, gcoy, gcox * 0.3333f + gcx * 0.6667f, gcoy, gcx, gcy );
								} else {
									CGContextAddCurveToPoint(cgc, gcx * 0.3333f + gcox * 0.6667f, gcy, gcx * 0.6667f + gcox * 0.3333f, gcy, gcx, gcy );
								}
								break;

						}

					}
				}

			}
		}

		CGContextStrokePath(cgc);
    }
	///////////////////////////draw rects
	if(mDrawRectsActive)
    {
        for(int i = xstart; i < xpixels; i++){
			envobj = &mEnvObj[drawOrder[i]];
			drawRect = SCtoCGRect(envobj->mRect);
			setEnvRect(envobj->x, envobj->y, envobj);

			fillcolor = envobj->mColor;
			CGContextSetRGBFillColor(cgc, fillcolor.red, fillcolor.green, fillcolor.blue, fillcolor.alpha);
			CGContextFillRect (cgc, drawRect);
			if(envobj->mRect.width > 1.0 || envobj->mRect.height > 1.0)
				CGContextStrokeRect(cgc,drawRect);
			if(envobj->mString)
				stringDrawCenteredInRect(envobj->mString, envobj->mRect, mFontName, mFontSize, mStrokeColor);
			//draw outputs
			CGContextSetRGBFillColor(cgc, mStrokeColor.red, mStrokeColor.green, mStrokeColor.blue, mStrokeColor.alpha);

        }
    }

    //draw selection:
    CGContextRestoreGState(cgc);

}

bool SCEnvelopeView::setValue(SCEnvObject * envob,  double xIn, double yIn, bool send)
{
    bool changed = true;
   // SCEnvObject * envob;
    //indx = sc_clip(indx, 0, mTabSize - 1);
    xIn = sc_clip(xIn, 0., 1.);
    yIn = sc_clip(yIn, 0., 1.);

    if(!envob->mEditable) return false;
    if (mStepSize > 0.) {
        xIn = floor(xIn * mStepScale + 0.5) * mStepSize;
        yIn = floor(yIn * mStepScale + 0.5) * mStepSize;
    }
    //mCurrentIndex = indx;
       // if (changed) {
    //envob = &mEnvObj[indx];
        //changed = valXIn != envob->x || valYIn != envob->y;
    setEnvRect(xIn, yIn, envob);
    mCurrentY = yIn;
    mCurrentX = xIn;
    if (send) sendMessage(s_doaction, 0, 0, 0);
    //refresh();
    return changed;
       // }
}
void SCEnvelopeView::setValueFromPoint(SCPoint where)
{
    int indx = mSelectedIndex;
	float width, height;
	SCRect bounds = getDrawBounds();

	width = (mEnvObj+ mSelectedIndex)->mRect.width;
	height = (mEnvObj+ mSelectedIndex)->mRect.height;
    double x = (double) (where.x - bounds.x - 1 - width/2) / (bounds.width - width - 2);
    double y = (double) 1.0 - (where.y - bounds.y - 1 - height/2) / (bounds.height - height - 2);
    if(indx >= 0){
		setValue(mEnvObj+ mSelectedIndex, x, y, true);
		refresh();
	}
}


bool SCEnvelopeView::setEnvRect(double valXIn, double valYIn, SCEnvObject * envobIn)
{
    double x,y, thumbx, thumby;
	SCEnvObject * envob = envobIn;
	envob->x = valXIn;
	envob->y = valYIn;
	thumby = envob->mRect.height;
	thumbx = envob->mRect.width;
	SCRect bounds = getDrawBounds();

	x = valXIn * (bounds.width - thumbx - 2);
	y = valYIn * (bounds.height - thumby - 2);
	y = bounds.y + bounds.height - y - 1 - thumby;
	x = bounds.x + x + 1;

	envob->mRect = SCMakeRect( x, y, thumbx, thumby);
	envob->mDrawPoint.x = x + (envob->mRect.width * 0.5);
	envob->mDrawPoint.y = y + (envob->mRect.height * 0.5);
	//envob->mIsVisible = true;
//			if(! envob->mObjectColor == NULL) envob->mObjectColor = mFillColor;
//			envob->mColor = envob->mObjectColor;
	//}
  //  return SCMakeRect( x, y, thumbx, thumby);
  return true;
}
void SCEnvelopeView::setSelection(SCPoint where, bool fixed, bool checkForConnection)
{
    SCEnvObject * envo;
    bool sel = false;
    //single selection
	mMousePoint = where;
    if(mSelectedIndex >= 0 && !mIsFixedSelection){
        (mEnvObj + mSelectedIndex)->mIsSelected = false;
        (mEnvObj + mSelectedIndex)->mColor =(&mEnvObj[mSelectedIndex])->mObjectColor;
        mSelectedIndex = -1;
        if(fixed) mIsFixedSelection = false;

    }
    //

    for(int count=mActiveSize - 1; count>=0; count--) {
		int i = drawOrder[count];
        envo = &mEnvObj[i]; // check in reverse drawOrder so that 'on top' gets selected first
        sel = SCPointInRect(where, envo->mRect);
//		post("in SCEnvelopeView::setSelection: where.x %f, where.y %f, sel %i\n", where.x, where.y, (int) sel);
        if(sel){
			// toggle fixed selection
			if(envo->mIsSelected && mIsFixedSelection){
//				post("toggle\n");

				envo->mIsSelected = false;
				envo->mColor = envo->mObjectColor;
				mSelectedIndex = -1;
				mIsFixedSelection = false;
				return;
			} else {
				envo->mIsSelected = true;
				envo->mColor = mSelectedColor;
				mSelectedIndex = i;
				mLastIndex = i;
				if(fixed) mIsFixedSelection = true;
				int newdrawOrder[mActiveSize];
				// iterate over
				newdrawOrder[mActiveSize - 1] = i;
				int j = 0, k = 0;
				while (j < mActiveSize) {
					if(drawOrder[j] != i) {
						newdrawOrder[k] = drawOrder[j];
						k++;
					}
					j++;
				}
				memcpy(drawOrder, newdrawOrder, mActiveSize * sizeof(int));
				return;
			}
        }
    }
   /*
   multiple selection
     if(!sel) {
        (&mEnvObj[mSelectedIndex])->mIsSelected = false;
        (&mEnvObj[mSelectedIndex])->mColor = mFillColor;
        mSelectedIndex = -1;
    }
    */
    refresh();
}

void SCEnvelopeView::mouseDownAction(SCPoint where, int modifiers, NSEvent *theEvent)
{
	mouseTrack(where, modifiers,theEvent);
	SCView::mouseDownAction(where, modifiers, theEvent);
}

void SCEnvelopeView::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent){
//    sendMessage(getsym("mouseEndTrack"), 0, 0, 0);
//    if (modifiers & NSShiftKeyMask) {
//        mouseTrack(where, modifiers,theEvent);
//    }
    if(!mIsFixedSelection){
     if(mSelectedIndex >= 0){
        (&mEnvObj[mSelectedIndex])->mIsSelected = false;
        (&mEnvObj[mSelectedIndex])->mColor = (&mEnvObj[mSelectedIndex])->mObjectColor;
        mSelectedIndex = -1;
        refresh();

    }
    }
	if(mSelectedIndex >= 0)
		setValueFromPoint(where);

	mIsTracking = false;
   // mouseTrack(where, modifiers);
}
void SCEnvelopeView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
	//mPrevPoint = where;
	//mouseTrack(where, modifiers,theEvent);
//	NSLog(@"foo");
}

void SCEnvelopeView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else if (modifiers & NSShiftKeyMask) {

            setSelection(where, true, false);
           /* mActiveSize++;
            if(mActiveSize < mTabSize) {
                mSelectedIndex = mActiveSize - 1;
                mEnvObj[mSelectedIndex].mIsSelected = true;
                mEnvObj[mSelectedIndex].mIsStatic = false;
            }else {
            mActiveSize = mTabSize;
            }
            */
            refresh();
            //setVisibleSize();
           // setValueFromPoint(where);
            sendMessage(getsym("doAction"), 0, 0, 0);
    } else if (modifiers & NSControlKeyMask) {
        mAbsoluteX = where.x; //absolute y mouse position
	    setSelection(where, false, true);
        sendMessage(getsym("doMetaAction"), 0, 0, 0);
		refresh();
    } else {
    if(!mIsFixedSelection && mSelectedIndex < 0)
        setSelection(where, false, false);

		if(mSelectedIndex >= 0) {
			if(!mIsTracking){
				mIsTracking = true;
				SCEnvObject selected = mEnvObj[mSelectedIndex];
				trackOffset = SCMakePoint(selected.mDrawPoint.x - where.x, selected.mDrawPoint.y - where.y);
			}
			SCPoint location = SCMakePoint(trackOffset.x + where.x, trackOffset.y + where.y);
			setValueFromPoint(location);
		}
    }
}


/*
void SCEnvelopeView::setVisibleSize()
{


    float offset;
    float scaledsize;
    if(mThumbSize > 0.0) {
        if(mXOffset < 1.0) offset = 0;
        else offset = mXOffset;
        scaledsize = (float) (mTabSize - mStartIndex) * mThumbSize;
        if (scaledsize <= mBounds.width) mVisibleSize = mTabSize;
        else mVisibleSize = (int) mBounds.width / mThumbSize;
    } else {
        mVisibleSize = 0;
    }

    mVisibleSize = mActiveSize;
}
*/

int SCEnvelopeView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
		int err;
        char *name = symbol->name;
        if (strcmp(name, "step")==0) {
		err = slotDoubleVal(slot, &mStepSize);
		if (!err) {
			mStepScale = 1. / mStepSize;
			bool changed = false;
			bool changedout = false;

			SCEnvObject * envob;
            for(int i=0; i<mTabSize; ++i){
                envob = mEnvObj+i;
				changed = setValue(envob,envob->x,envob->y, false);
				if(changed) changedout = changed;
            }
			if(changedout) refresh();
			SetBool(slot, changedout);
		}
			return errNone;
		}

		if (strcmp(name, "thumbSize")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
		    PyrSlot *slots = slotRawObject(slot)->slots;
			int err, idx;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			err = slotIntVal(slots+1, &mThumbSize);
			mThumbSizeY = mThumbSize;
			if (err) return err;
			if(idx >= mTabSize) return errNone;
            if(idx < 0) {
				for(int i=0; i<mTabSize; i++){
					(mEnvObj+i)->mRect.width = mThumbSize;
					(mEnvObj+i)->mRect.height = mThumbSize;
				}
			} else {
				(mEnvObj+idx)->mRect.width = mThumbSize;
				(mEnvObj+idx)->mRect.height = mThumbSize;
			}
            refresh();
            return errNone;
        }

        if (strcmp(name, "thumbWidth")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
		    PyrSlot *slots = slotRawObject(slot)->slots;
			int err, idx;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			err = slotIntVal(slots+1, &mThumbSize);
			if (err) return err;
			if(idx >= mTabSize) return errNone;
            if(idx < 0) {
				for(int i=0; i<mTabSize; i++){
					(mEnvObj+i)->mRect.width = mThumbSize;
				}
			}
			else
				(&mEnvObj[idx])->mRect.width = mThumbSize;
            refresh();
            return errNone;
        }

        if (strcmp(name, "thumbHeight")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
		    PyrSlot *slots = slotRawObject(slot)->slots;
			int err, idx;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			err = slotIntVal(slots+1, &mThumbSizeY);
			if (err) return err;
			if(idx >= mTabSize) return errNone;
            if(idx < 0) {
				for(int i=0; i<mTabSize; i++){
					(mEnvObj+i)->mRect.height = mThumbSizeY;
				}
			}
			else
				(&mEnvObj[idx])->mRect.height = mThumbSizeY;
            refresh();
            return errNone;
        }

        if (strcmp(name, "selectedIndex")==0) {
			int  newSelection;
			SCEnvObject *obj;
			obj = mEnvObj+mSelectedIndex;
            err = slotIntVal(slot, &newSelection);
			if(err) return err;
			if(mSelectedIndex == newSelection) return errNone;

			if(mSelectedIndex >= 0) obj->mColor = obj->mObjectColor;
			mSelectedIndex = sc_clip(newSelection, -1, mTabSize-1);
			if(mSelectedIndex >= 0)
			{
				obj = mEnvObj+mSelectedIndex;
				obj->mColor = mSelectedColor;
				mCurrentX = obj->x;
				mCurrentY = obj->y;

			}
            refresh();
            return errNone;
        }

        if (strcmp(name, "setFixedSelection")==0) {
			mIsFixedSelection = IsTrue(slot);
            return errNone;
        }

		if (strcmp(name, "setIndex")==0) { //index selection without refresh() and color change.
            err = slotIntVal(slot, &mSelectedIndex);
			if(mSelectedIndex >= 0)
			mSelectedIndex = sc_clip(mSelectedIndex, 0, mTabSize-1);
            return errNone;
        }
		if (strcmp(name, "connect")==0) { //if this is set all connection start in the middle of the rect

			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			PyrSlot *slots = slotRawObject(slot)->slots;

			int err, idx;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			if(idx >= mTabSize || idx < 0) return errIndexOutOfRange;
			SCEnvObject * envobj;
		    PyrSlot *connections;
			connections = slots+1;
			if (!isKindOfSlot(connections, class_array)) return errWrongType;

			envobj = (mEnvObj + idx);
			err = allocSlotDoubleArrayVal(connections, &envobj->mConnections);
			//err = allocSlotIntArrayVal(slot, &(&mEnvObj[mSelectedIndex])->mConnections); //int array crashes sc
			envobj->mNumConnection = slotRawObject(connections)->size;
		//	envobj->deltax = 0;
		//	envobj->mConnectionInputs = 0;
		//	envobj->mConnectionOutputs = 0;
			mDrawCenteredConnection = true;
			mIsEnvView = false;
			refresh();
            return errNone;
        }

        if (strcmp(name, "fillColor")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			PyrSlot *slots = slotRawObject(slot)->slots;

			int err, idx;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			if(idx >= mTabSize) return errIndexOutOfRange;
            err = slotColorVal(slot+1, &mFillColor);
			if (err) return err;

			if(idx < 0) {
				for(int i=0; i<mTabSize; i++){
					(mEnvObj+i)->mColor = (mEnvObj+i)->mObjectColor = mFillColor;
				}
			}
			else
				(mEnvObj+idx)->mColor = (mEnvObj+idx)->mObjectColor = mFillColor;

            refresh();
            return errNone;
        }
        if (strcmp(name, "strokeColor")==0) {
            err = slotColorVal(slot, &mStrokeColor);
            refresh();
            return errNone;
        }
        if (strcmp(name, "selectionColor")==0) {
            err = slotColorVal(slot, &mSelectedColor);
            refresh();
            return errNone;
        }

        if (strcmp(name, "startIndex")==0) {
            err = slotIntVal(slot, &mStartIndex);
            refresh();
            return errNone;
        }
        if (strcmp(name, "editable")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			PyrSlot *slots = slotRawObject(slot)->slots;

			int err, idx, editable;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			if(idx >= mTabSize) return errIndexOutOfRange;
			editable = IsTrue(slots+1);
			if(idx < 0) {
				for(int i=0; i<mTabSize; i++){
					(mEnvObj + i)->mEditable = editable;
				}
			}
			else
				(mEnvObj + idx)->mEditable = editable;
            return errNone;
        }
        if (strcmp(name, "drawLines")==0) {
            mDrawLinesActive = IsTrue(slot);
            refresh();
            return errNone;
        }
        if (strcmp(name, "drawRects")==0) {
            mDrawRectsActive = IsTrue(slot);
            refresh();
            return errNone;
        }
		if (strcmp(name, "string")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			PyrSlot *slots = slotRawObject(slot)->slots;

			int err, idx;
			err = slotIntVal(slots+0, &idx);
			if (err) return err;
			if(idx >= mTabSize || idx < 0) return errIndexOutOfRange;
			PyrSlot *string = slots+1;

            err = allocSlotStrVal(string,  &(&mEnvObj[idx])->mString);
            if (err) return err;
            refresh();
            return errNone;
        }

        if (symbol == s_x) {
            double y;
            slotDoubleVal(slot, &mCurrentX);
            mCurrentX  = sc_clip(mCurrentX, 0.0, 1.0);
            if(mSelectedIndex >= 0){
            y =  (&mEnvObj[mSelectedIndex])->y;
            setEnvRect(mCurrentX, y,&mEnvObj[mSelectedIndex]);
            refresh();
            }
            return errNone;
	}
	if (symbol == s_y) {
            double x;
            slotDoubleVal(slot, &mCurrentY);
            mCurrentY = sc_clip(mCurrentY, 0.0, 1.0);
            if(mSelectedIndex >= 0){
            x =  (&mEnvObj[mSelectedIndex])->x;
            setEnvRect(x, mCurrentY, &mEnvObj[mSelectedIndex]);
            refresh();
            }
            return errNone;
	}
	if (symbol == s_value) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		if (!isKindOfSlot(slotRawObject(slot)->slots+0, class_array)) return errWrongType;
		if (!isKindOfSlot(slotRawObject(slot)->slots+1, class_array)) return errWrongType;

		int size = slotRawObject(&(slotRawObject(slot)->slots[0]))->size;
		if(size != mTabSize) {
			// initialise drawing order
			if(drawOrder) {
				delete [] drawOrder;
				drawOrder = 0;
			}
			if(size > 0){
				drawOrder = new int[size];
				for(int i=0; i<size; i++){
					drawOrder[i] = i;
				}
			}
		//this should keep the settings of the old ones ... maybe make a  separate add action ...
			err = allocSlotEnvObjArray(slot, &mEnvObj);
			if(err) return err;
			mTabSize = size;
		}
		//move this to allocslotEnvobjArray:
		double x, y;
		PyrDoubleArray * xarr;
		PyrDoubleArray * yarr;
		xarr = (PyrDoubleArray*)(slotRawObject(&slotRawObject(slot)->slots[0]));
		yarr = (PyrDoubleArray*)(slotRawObject(&slotRawObject(slot)->slots[1]));
		SCEnvObject * envob;
		for(int i=0; i<size; i++){
			envob = mEnvObj+i;
			x = xarr->d[i];
			y = yarr->d[i];
			setValue(envob,x,y, false);
			//post("set x: %f ,y:  %f \n", x, y);
		}
		mActiveSize = size;
		//setVisibleSize();


		refresh();

		return errNone;
	}
	/*
	if (strcmp(name, "connectFrom")==0) {
			err = slotIntVal(slot, &mConnectFrom);
            return errNone;
        }
	if (strcmp(name, "connectTo")==0) {
           err = slotIntVal(slot, &mConnectTo);
            return errNone;
        }
	*/
		if (strcmp(name, "setGraphView")==0) {
			mDrawCenteredConnection = false;
			mIsEnvView = false;
			return errNone;
		}
		// set curve for one segment or one curve type for all segments
		if (strcmp(name, "setCurve")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			PyrSlot *slots = slotRawObject(slot)->slots;
			int err, idx, curve, shape;

			err = slotIntVal(slots+0, &idx);
			if (err) return err;

			err = slotIntVal(slots+1, &shape);
			if (err) return err;

			err = slotIntVal(slots+2, &curve);
			if (err) return err;

			if(idx >= mTabSize) return errNone;
			if(idx < 0) {
				for(int i=0; i<mTabSize; i++){
					(mEnvObj+i)->shape = shape;
					(mEnvObj+i)->curve = curve;
				}
			} else {
				(mEnvObj+idx)->shape = shape;
				(mEnvObj+idx)->curve = curve;

			}
			refresh();
			return errNone;
		}
		// set curves for all segments
		if (strcmp(name, "setCurves")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			int size = slotRawObject(slot)->size;
			size = sc_clip(size, 0, mTabSize);
			for(int i=0; i<size; i++)
			{
				PyrSlot * shapeCurveSlot = slotRawObject(slot)->slots+i;
				int shape;
				float curve;
				if(isKindOfSlot(shapeCurveSlot, class_array))
				{
					int err = slotIntVal(slotRawObject(shapeCurveSlot)->slots+0, &shape);
					if (err) return err;
					err = slotFloatVal(slotRawObject(shapeCurveSlot)->slots+1, &curve);
					if (err) return err;
					(mEnvObj+i)->shape = shape;
					(mEnvObj+i)->curve = curve;
				}
			}
			refresh();
			return errNone;
		}

		if (strcmp(name, "showGrid")==0) {
			mGridOn = IsTrue(slot);
			refresh();
			return errNone;
		}

		if (strcmp(name, "setGrid")==0) {
			if (!isKindOfSlot(slot, class_array)) return errWrongType;
			err = slotFloatVal(slotRawObject(slot)->slots+0, &mGrid.x);
			if (err) return err;
			err = slotFloatVal(slotRawObject(slot)->slots+1, &mGrid.y);
			if(mGridOn) refresh();
 			return errNone;
		}

		if (strcmp(name, "setGridColor")==0) {
			err = slotColorVal(slot, &mGridColor);
			if(mGridOn) refresh();
			return errNone;
		}
		/*
        if (strcmp(name, "selectionSize")==0) {
            slotIntVal(slot, &mSelectionSize);
            refresh();
            return errNone;
        }
        */

        return SCView::setProperty(symbol, slot);
}

int SCEnvelopeView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{

	char *name = symbol->name;
	if (strcmp(name, "step")==0) {
		SetFloat(slot, mStepSize);
		return errNone;
	}

	if (symbol == s_x) {
		if(mSelectedIndex >= 0){
			SetFloat(slot, mCurrentX);
		} else {
			SetNil(slot);
		}
		return errNone;
	}
	if (symbol == s_y) {
		if(mSelectedIndex >= 0){
			SetFloat(slot, mCurrentY);
		} else {
			SetNil(slot);
		}
		return errNone;
	}
	if (strcmp(name, "selectedIndex")==0) {
		SetInt(slot, (int) mSelectedIndex);
		return errNone;
	}
		if (strcmp(name, "lastIndex")==0) {
		SetInt(slot, (int) mLastIndex);
		return errNone;
	}

	if (symbol == s_value) {
		if (!isKindOfSlot(slot, class_array)) return errWrongType;
		int size = slotRawObject(&(slotRawObject(slot)->slots[0]))->size;
		SCEnvObject * envo;
		if(size > mTabSize)  size = mTabSize;

		PyrDoubleArray * xarr;
		PyrDoubleArray * yarr;
		xarr = (PyrDoubleArray*)(slotRawObject(&slotRawObject(slot)->slots[0]));
		yarr = (PyrDoubleArray*)(slotRawObject(&slotRawObject(slot)->slots[1]));

		for(int i=0; i<size; i++){
			envo = &mEnvObj[i];
			xarr->d[i] = envo->x;
			yarr->d[i] = envo->y;
		}
		return errNone;
	}
	/*
	if (strcmp(name, "selectionSize")==0) {
		SetInt(slot, mSelectionSize);
		return errNone;
	}
	*/
	if (strcmp(name, "absoluteX")==0) {
		SetInt(slot, (int) mAbsoluteX);
		return errNone;
	}
	/*
	if (strcmp(name, "connectFrom")==0) {
		SetInt(slot, (int) mConnectFrom);
		return errNone;
	}
	if (strcmp(name, "connectTo")==0) {
		SetInt(slot, (int) mConnectTo);
		return errNone;
	}
	*/
	return SCView::getProperty(symbol, slot);
}

bool SCEnvelopeView::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCEnvelopeView::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}


////////////////////////////////////////////////

SCView* NewSCNumberBoxOld(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCNumberBoxOld(inParent, inObj, inBounds);
}

SCNumberBoxOld::SCNumberBoxOld(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCStaticText(inParent, inObj, inBounds)
{
        mBackground = new SolidColorBackground(SCMakeColor(1.0,1.0,1.0, 1.0)); // default is white
        mEnabled = true;
        mCanFocus = true;
}

SCNumberBoxOld::~SCNumberBoxOld()
{
}

bool SCNumberBoxOld::shouldDim()
{
    return true;
}

void SCNumberBoxOld::draw(SCRect inDamage)
{
	SCView::draw(inDamage);
   	SCRect bounds = getDrawBounds();

	CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, rect);

    QDDrawBevelRect(cgc, rect, 1, true);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
	SCRect sbounds = bounds;

	sbounds.x += 2;
	sbounds.width -= 4;
    drawString(sbounds);
}

//int SCNumberBoxOld::setProperty(PyrSymbol *symbol, PyrSlot *slot)
//{
//	int err;
//	char *name = symbol->name;
// 	if (strcmp(name, "boxColor")==0) {
//		err = slotColorVal(slot, &mBoxColor);
//                if (err) return err;
//                refresh();
//		return errNone;
//	}
//       return SCStaticText::setProperty(symbol, slot);
//}

void SCNumberBoxOld::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    }
}

//int SCNumberBoxOld::getProperty(PyrSymbol *symbol, PyrSlot *slot)
//{
//	char *name = symbol->name;
// 	if (strcmp(name, "boxColor")==0) {
//             return setSlotColor(slot, &mBoxColor);
//       }
//        return SCStaticText::getProperty(symbol, slot);
//}

bool SCNumberBoxOld::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCNumberBoxOld::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

////////////////////////////////////////////////


SCView* NewSCDragSource(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCDragSource(inParent, inObj, inBounds);
}

SCDragSource::SCDragSource(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
    : SCStaticText(inParent, inObj, inBounds)
{
        mEnabled = true;
}

SCDragSource::~SCDragSource()
{
}

void SCDragSource::draw(SCRect inDamage)
{
	SCView::draw(inDamage);
	SCRect bounds = getDrawBounds();

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, false);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 0, SCtoQDColor(mBackColor), 2);
    drawString(bounds);
}

bool SCDragSource::shouldDim()
{
    return true;
}

int ivxSCDragSource_object;

void SCDragSource::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    beginDrag(where);
}

////////////////////////////////////////////////


SCView* NewSCDragSink(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCDragSink(inParent, inObj, inBounds);
}

SCDragSink::SCDragSink(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
    : SCStaticText(inParent, inObj, inBounds)
{
        mEnabled = true;
}

SCDragSink::~SCDragSink()
{
}

void SCDragSink::draw(SCRect inDamage)
{
	SCView::draw(inDamage);
	SCRect bounds = getDrawBounds();

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(bounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
    drawString(bounds);
}

bool SCDragSink::shouldDim()
{
    return true;
}

bool SCDragSink::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCDragSink::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
}

////////////////////////////////////////////////
////////////////////////////////////////////////


SCView* NewSCDragBoth(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCDragBoth(inParent, inObj, inBounds);
}

SCDragBoth::SCDragBoth(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
    : SCDragSink(inParent, inObj, inBounds)
{
}

void SCDragBoth::draw(SCRect inDamage)
{
	SCView::draw(inDamage);

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
	SCRect dbounds = getDrawBounds();

    CGRect rect = SCtoCGRect(dbounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);
    SCRect bounds = dbounds;
    bounds.x += 2; bounds.y += 2; bounds.width -= 4; bounds.height -= 4;
    QDDrawBevelRect(cgc, SCtoCGRect(bounds), 1, false);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(bounds), 1, 1, SCtoQDColor(mBackColor), 2);
    //SCRect bounds = bounds;
    //bounds.x += 2; bounds.y += 2; bounds.width -= 4; bounds.height -= 4;
    //drawBevelRect(SCtoQDRect(bounds), 1, 0, SCtoQDColor(mBackColor), 2);
    drawString(bounds);
}

int ivxSCDragBoth_object;

void SCDragBoth::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    beginDrag(where);
}


////////////////////////////////////////////////
#import "TabletEvents.h"

SCView* NewSCTabletView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCTabletView(inParent, inObj, inBounds);
}

SCTabletView::SCTabletView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mClipToBounds(1)
{
}

SCTabletView::~SCTabletView()
{
}

int SCTabletView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
    char *name = symbol->name;
    if (strcmp(name, "clipToBounds")==0) {
        slotIntVal(slot, &mClipToBounds);
        return errNone;
    }
    return SCView::setProperty(symbol, slot);
}

void SCTabletView::mouseBeginTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    float x,y;
	SCRect bounds = getDrawBounds();

    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
        x = where.x - bounds.x;
        y = where.y - bounds.y;
        if(mClipToBounds) {
            x = sc_clip(x,0.,bounds.width);
            y = sc_clip(y,0.,bounds.height);
        }
        TABLETTRACK(s_mouseDown,x,y)
    }
}

void SCTabletView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    float x,y;
	SCRect bounds = getDrawBounds();

    if (! (modifiers & NSCommandKeyMask)) {
        x = where.x - bounds.x;
        y = where.y - bounds.y;
        if(mClipToBounds) {
            x = sc_clip(x,0.,bounds.width);
            y = sc_clip(y,0.,bounds.height);
        }
        TABLETTRACK(s_doaction,x,y)
    }
}

void SCTabletView::mouseEndTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    float x,y;
    if (modifiers & NSCommandKeyMask) {
        this->receiveDrag();
    } else {
		SCRect bounds = getDrawBounds();

        x = where.x - bounds.x;
        y = where.y - bounds.y;
        if(mClipToBounds) {
            x = sc_clip(x,0.,bounds.width);
            y = sc_clip(y,0.,bounds.height);
        }
        TABLETTRACK(s_mouseUp,x,y)
    }
}

void SCTabletView::mouseDownAction(SCPoint where, int modifiers, NSEvent *theEvent)
{

}
void SCTabletView::mouseMoveAction(SCPoint where, int modifiers, NSEvent *theEvent)
{

}
void SCTabletView::mouseUpAction(SCPoint where, int modifiers, NSEvent *theEvent)
{

}

////////////////////////////////////////////////

SCViewMaker::SCViewMaker(const char* inName, SCViewCtor inCtor)
    : mNext(gSCViewMakers), mCtor(inCtor), mName(inName)
{
	gSCViewMakers = this;
}

SCView* MakeSCView(PyrObject* inObj, SCContainerView *inParent, SCRect inBounds,const char *classname)
{
    SCViewMaker* maker = gSCViewMakers;
	while (maker) {
        if (strcmp(classname, maker->mName) == 0) {
            return (maker->mCtor)(inParent, inObj, inBounds);
        }
        maker = maker->mNext;
	}
	return 0;
}

static bool sRegisteredSCViewClasses = false;
extern SCView* NewSCSoundFileView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCCocoaTextView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCQuartzComposerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCWebView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCTextField(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
extern SCView* NewSCLevelIndicator(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

void registerSCViewClasses()
{
    if (sRegisteredSCViewClasses) return;
    sRegisteredSCViewClasses = true;

    new SCViewMaker("SCTopView", NewSCTopView);
    new SCViewMaker("SCCompositeView", NewSCCompositeView);
    new SCViewMaker("SCHLayoutView", NewSCHLayoutView);
    new SCViewMaker("SCVLayoutView", NewSCVLayoutView);
    new SCViewMaker("SCSlider", NewSCSlider);
    new SCViewMaker("SCRangeSlider", NewSCRangeSlider);
    new SCViewMaker("SC2DSlider", NewSC2DSlider);
    new SCViewMaker("SC2DTabletSlider", NewSC2DTabletSlider);
    new SCViewMaker("SCButton", NewSCButton);
    new SCViewMaker("SCStaticText", NewSCStaticText);
    new SCViewMaker("SCNumberBoxOld", NewSCNumberBoxOld);
    new SCViewMaker("SCDragSource", NewSCDragSource);
    new SCViewMaker("SCDragSink", NewSCDragSink);
    new SCViewMaker("SCDragBoth", NewSCDragBoth);
    new SCViewMaker("SCUserView", NewSCUserView);
    new SCViewMaker("SCPopUpMenu", NewSCPopUpMenu);
    new SCViewMaker("SCMultiSliderView", NewSCMultiSliderView);
    new SCViewMaker("SCEnvelopeView", NewSCEnvelopeView);
    new SCViewMaker("SCTabletView", NewSCTabletView);
    new SCViewMaker("SCScope", NewSCScope);
    new SCViewMaker("SCListView", NewSCListView);
    new SCViewMaker("SCSoundFileView", NewSCSoundFileView);
	//these can be found in SCCocoaView.M
    new SCViewMaker("SCTextView", NewSCCocoaTextView);
    new SCViewMaker("SCMovieView", NewSCMovieView);
	new SCViewMaker("SCQuartzComposerView", NewSCQuartzComposerView);
    new SCViewMaker("SCWebView", NewSCWebView);
	new SCViewMaker("SCTextField", NewSCTextField);
	new SCViewMaker("SCNumberBox", NewSCNumberBox);
	new SCViewMaker("SCScrollTopView", NewSCScrollTopView);
	new SCViewMaker("SCScrollView", NewSCScrollView);
	new SCViewMaker("SCLevelIndicator", NewSCLevelIndicator);
}



int prSCView_New(struct VMGlobals *g, int numArgsPushed);
int prSCView_New(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 3;
	// view, parent, bounds, viewclass
	PyrSlot parentSlot;
	SCContainerView *parent;
	if (isKindOfSlot(args, s_sctopview->u.classobj) && !isKindOfSlot(args, s_scscrollview->u.classobj)) {
		parent = 0;
	} else {
		if (!isKindOfSlot(args+1, s_sccontview->u.classobj)) return errWrongType;
		// check if it still has a dataptr
		parentSlot = slotRawObject(&args[1])->slots[0];
		if(IsNil(&parentSlot)) return errFailed;
		parent = (SCContainerView*)slotRawPtr(&parentSlot);
	}
	if (!(isKindOfSlot(args+2, s_rect->u.classobj))) return errWrongType;

	SCRect bounds;
	int err = slotGetSCRect(args+2, &bounds);
	if (err) return err;
	SCView *view = MakeSCView(slotRawObject(&args[0]), parent, bounds, slotRawSymbol(&slotRawClass(&args[3])->name)->name);
	if (!view) return errFailed;

	return errNone;
}

int prSCView_SetProperty(struct VMGlobals *g, int numArgsPushed);
int prSCView_SetProperty(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 2;

	if (!IsSym(args+1)) return errWrongType;

	SCView *view = (SCView*)slotRawPtr(slotRawObject(&args[0])->slots);
	if (!view) return errFailed;

	int err = view->setProperty(slotRawSymbol(&args[1]), args+2);
	if (err) SetNil(args+2);

	slotCopy(&args[0], &args[2]);

	return err;
}

int prSCView_GetProperty(struct VMGlobals *g, int numArgsPushed);
int prSCView_GetProperty(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 2;

	if (!IsSym(args+1)) return errWrongType;

	SCView *view = (SCView*)slotRawPtr(slotRawObject(&args[0])->slots);
	if (!view) return errFailed;

	int err = view->getProperty(slotRawSymbol(&args[1]), args+2);
	if (err) SetNil(args+2);

	slotCopy(&args[0], &args[2]);

	return errNone;
}

int prSCView_FindByID(struct VMGlobals *g, int numArgsPushed);
int prSCView_FindByID(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	SCView *view = (SCView*)slotRawPtr(slotRawObject(a)->slots);
	if (!view) return errFailed;

	int32 tag;
	int err = slotIntVal(b, &tag);
	if (err) return err;

	view = view->findViewByID(tag);
	if (!view) {
		SetNil(a);
	} else {
		SetObjectOrNil(a, view->GetSCObj());
	}

	return errNone;
}

int prSCView_Focus(struct VMGlobals *g, int numArgsPushed);
int prSCView_Focus(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *viewObjSlot = g->sp - 1;
	SCView *view = (SCView*)slotRawPtr(slotRawObject(&viewObjSlot[0])->slots);
	PyrSlot *boo = g->sp;

	if (!view) return errFailed;
	view->makeFocus(IsTrue(boo));
	return errNone;
}

int prSCView_HasFocus(struct VMGlobals *g, int numArgsPushed);
int prSCView_HasFocus(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *viewObjSlot = g->sp;
	SCView *view = (SCView*)slotRawPtr(slotRawObject(&viewObjSlot[0])->slots);

	if (!view) return errFailed;
	SetBool(viewObjSlot, view->isFocus());
	return errNone;
}

int prSCView_Refresh(struct VMGlobals *g, int numArgsPushed);
int prSCView_Refresh(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	if(::IsNil( slotRawObject(g->sp)->slots+0 ))
		return errFailed;

	SCView *view = (SCView*)slotRawPtr(slotRawObject(g->sp)->slots);
	if(!view) return errNone;
	view->refresh();
	return errNone;
}

int prSCView_RefreshInRect(struct VMGlobals *g, int numArgsPushed);
int prSCView_RefreshInRect(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	if(::IsNil( slotRawObject(g->sp - 1)->slots+0 ))
		return errFailed;

	SCRect r;
	SCView *view = (SCView*)slotRawPtr(slotRawObject(g->sp - 1)->slots);

	if(slotGetSCRect(g->sp, &r) != noErr)
		return errFailed;

	view->refreshInRect(r);
	return errNone;
}

int prSCView_Remove(struct VMGlobals *g, int numArgsPushed);
int prSCView_Remove(struct VMGlobals *g, int numArgsPushed)
{
	//if (!g->canCallOS) return errCantCallOS;

	PyrSlot *viewObjSlot = g->sp;
	SCView *view = (SCView*)slotRawPtr(slotRawObject(viewObjSlot)->slots);
	if (!view) return errFailed;

	// removes from parent, set mObj to nil
	delete view;
	return errNone;
}

void initSCViewPrimitives()
{

	registerSCViewClasses();

	int base, index;

	s_x = getsym("x");
	s_y = getsym("y");
	s_lo = getsym("lo");
	s_hi = getsym("hi");
	s_range = getsym("range");
	s_scview = getsym("SCView");
	s_sccontview = getsym("SCContainerView");
	s_sctopview = getsym("SCTopView");
	s_scscrollview = getsym("SCScrollView");
	s_beginDrag = getsym("beginDrag");
	s_receiveDrag = getsym("receiveDrag");
	s_canReceiveDrag = getsym("canReceiveDrag");
	s_mouseDown = getsym("mouseDown");
	s_mouseUp = getsym("mouseUp");
	s_callDrawFunc = getsym("callDrawFunc");
	s_toggleEditMode = getsym("toggleEditMode");

	base = nextPrimitiveIndex();
	index = 0;

	definePrimitive(base, index++, "_SCView_New", prSCView_New, 4, 0);
	definePrimitive(base, index++, "_SCView_SetProperty", prSCView_SetProperty, 3, 0);
	definePrimitive(base, index++, "_SCView_GetProperty", prSCView_GetProperty, 3, 0);
	definePrimitive(base, index++, "_SCView_FindByID", prSCView_FindByID, 2, 0);
	definePrimitive(base, index++, "_SCView_Focus", prSCView_Focus, 2, 0);
	definePrimitive(base, index++, "_SCView_HasFocus", prSCView_HasFocus, 1, 0);
	definePrimitive(base, index++, "_SCView_Refresh", prSCView_Refresh, 1, 0);
	definePrimitive(base, index++, "_SCView_Remove", prSCView_Remove, 1, 0);
	definePrimitive(base, index++, "_SCView_RefreshInRect", prSCView_RefreshInRect, 2, 0);
}

int ivxSCTextView_linkAction;

void initGUI();
void initGUI()
{
	ivxSCDragSource_object = instVarOffset("SCDragSource", "object");
	ivxSCDragBoth_object = instVarOffset("SCDragBoth", "object");
	ivxSCTextView_linkAction = instVarOffset("SCTextView", "linkAction");
}



/*
loose ends
drag/drop views
background color / pic

new views
    larger text view
    scope view
    plot view
        graph limits
        grid on/off
        data 1d, 2d
            color
    hdivider
    vdivider
    overlay view

*/


