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


#include <UIKit/UIKit.h>
#include <pthread.h>
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include "SC_BoundsMacros.h"
#include "SC_InlineBinaryOp.h"
#include "iPhoneSCGraphView.h"


#include "iPhoneSCView.h"


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
PyrSymbol* s_callDrawHook;
PyrSymbol* s_toggleEditMode;

extern pthread_mutex_t gLangMutex;

int stringDrawCenteredInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);
int stringDrawLeftInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);
int stringDrawRightInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);

int nsStringDrawInRectAlign(NSString *nsstring, SCRect screct, char *cFontName, float fontSize, SCColor sccolor, int hAlign, int vAlign, CGSize *outSize);


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
		if(inParent->relativeOrigin()){
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

void SCView::touchDownAction(SCPoint where, UITouch *touch)
{
    pthread_mutex_lock (&gLangMutex);
    PyrSymbol *method = getsym("mouseDown");
	int clickCount = [touch tapCount];
	int buttonNum = 0;
    if (mObj) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
		if(mParent->relativeOrigin()){
			SCRect tbounds = mParent->getDrawBounds();
			where.x = where.x - tbounds.x;
			where.y = where.y - tbounds.y;
		}
		++g->sp;  SetObject(g->sp, mObj);
        ++g->sp;  SetFloat(g->sp, where.x);
        ++g->sp;  SetFloat(g->sp, where.y);
        ++g->sp;  SetInt(g->sp, 0);
		++g->sp;  SetInt(g->sp,0);
		++g->sp;  SetInt(g->sp,clickCount);
        runInterpreter(g, method, 6);
        g->canCallOS = false;
    }
    pthread_mutex_unlock (&gLangMutex);
}

void SCView::touchMoveAction(SCPoint where, UITouch *touch)
{
    pthread_mutex_lock (&gLangMutex);
    PyrSymbol *method = getsym("mouseMove");
    if (mObj) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
		if(mParent->relativeOrigin()){
			SCRect tbounds = mParent->getDrawBounds();
			where.x = where.x - tbounds.x;
			where.y = where.y - tbounds.y;
		}
        ++g->sp;  SetObject(g->sp, mObj);
        ++g->sp;  SetFloat(g->sp, where.x);
        ++g->sp;  SetFloat(g->sp, where.y);
        ++g->sp;  SetInt(g->sp, 0);
        runInterpreter(g, method, 4);
        g->canCallOS = false;
    }
    pthread_mutex_unlock (&gLangMutex);
}
void SCView::touchUpAction(SCPoint where, UITouch *touch)
{
    pthread_mutex_lock (&gLangMutex);
    PyrSymbol *method = getsym("mouseUp");
    if (mObj) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
		if(mParent->relativeOrigin()){
			SCRect tbounds = mParent->getDrawBounds();
			where.x = where.x - tbounds.x;
			where.y = where.y - tbounds.y;
		}
        ++g->sp;  SetObject(g->sp, mObj);
        ++g->sp;  SetFloat(g->sp, where.x);
        ++g->sp;  SetFloat(g->sp, where.y);
        ++g->sp;  SetInt(g->sp, 0);
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

void SCView::touchBeginTrack(SCPoint where, UITouch *touch)
{
	touchTrack(where, touch);
}

void SCView::touchTrack(SCPoint where, UITouch *touch)
{
}

void SCView::touchEndTrack(SCPoint where, UITouch *touch)
{
	touchTrack(where, touch);
}

void SCView::touchOver(SCPoint where, UITouch *touch)
{
    pthread_mutex_lock (&gLangMutex);
    PyrSymbol *method = getsym("mouseOver");
    if (mObj) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
		if(mParent->relativeOrigin()){
			SCRect tbounds = mParent->getDrawBounds();
			where.x = where.x - tbounds.x;
			where.y = where.y - tbounds.y;
		}
        ++g->sp;  SetObject(g->sp, mObj);
        ++g->sp;  SetInt(g->sp, (int) where.x);
        ++g->sp;  SetInt(g->sp, (int) where.y);
        ++g->sp;  SetInt(g->sp, 0);
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
	touchOver(where, 0);
}
void SCView::draggingUpdated (SCPoint where)
{
	touchOver(where, 0);

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
        cgc = (CGContextRef) UIGraphicsGetCurrentContext();
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
        CGContextRef cgc = (CGContextRef) UIGraphicsGetCurrentContext();
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
        CGContextRef cgc = (CGContextRef) UIGraphicsGetCurrentContext();
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
        CGContextRef cgc = (CGContextRef) UIGraphicsGetCurrentContext();
        CGContextSaveGState(cgc);
        CGContextSetLineWidth(cgc, 4);
        CGContextSetRGBStrokeColor(cgc, 0., 0., 1., 0.4);
        CGContextStrokeRect(cgc, rect);
        CGContextRestoreGState(cgc);
    }
}

void SCView::drawIfNecessary(SCRect inDamage)
{
	if (SCRectsDoIntersect(inDamage, getDrawBounds()) && mVisible) {
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

SCRect CGtoSCRect(CGRect nsrect)
{
	SCRect screct;
    screct.x = nsrect.origin.x;
    screct.y = nsrect.origin.y;
    screct.width = nsrect.size.width;
    screct.height = nsrect.size.height;
    return screct;
}

void SCView::setBounds(SCRect inBounds)
{
    mBounds = inBounds;
	if(mParent->relativeOrigin()){
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
//	if(mParent->relativeOrigin()){
//		SCRect pbounds = mParent->getBounds();
//		mLayout.bounds.x = mBounds.x + pbounds.x;
//		mLayout.bounds.y = mBounds.y + pbounds.y;
//		mLayout.bounds.width = mBounds.width;
//		mLayout.bounds.height = mBounds.height;
//	} else {
//		mLayout.bounds = mBounds;
//	}
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
			//NSView* newFirstResponder = focusResponder();
			//[[newFirstResponder window] makeFirstResponder:newFirstResponder];
        }
    } else {
        if (isFocus()) {
            mTop->focusIs(0);
            refreshFocus();
        }
    }
}

//NSView* SCView::focusResponder() { return mTop->GetNSView(); }

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
            ++g->sp;  slotCopy(g->sp, &args[i]);
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
					[(SCGraphView*)mTop->GetUIView() setFrameSizeToMinimum];
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
/*
	if (strcmp(name, "backgroundImage")==0) {
            err = slotBackgroundImageVal(slot, &mBackgroundImage);
            if (err) return err;
            refresh();
            return errNone;
	}
*/
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
	: SCView(inParent, inObj, inBounds), mChildren(0), mNumChildren(0), mRelativeOrigin(false)
{
	mLayout.bounds = mBounds;
	if(mParent ){
		if(mParent->relativeOrigin()){
			SCRect pBounds = mParent->getDrawBounds(); // or jsut absolute frame bounds ?
			mLayout.bounds.x = inBounds.x + pBounds.x;
			mLayout.bounds.y = inBounds.y + pBounds.y;
		}
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

	if (strcmp(name, "relativeOrigin")==0) {
		mRelativeOrigin = IsTrue(slot);
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
//    float right = mBounds.x + mBounds.width;
//    float bottom = mBounds.y + mBounds.height;
	mLayout.bounds = inBounds;
//	if(isTopContainer()){
//		post("i'm my own parent ... \t");
//		post("in x:%f, y:%f\n", inBounds.x, inBounds.y);
//	}

	if(mParent ){
		if(mParent->relativeOrigin()){
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
					if(!child->relativeOrigin()){
							offset = (mBounds.x + mBounds.width) - (bounds.x + bounds.width);
							bounds.x = (inBounds.x + inBounds.width) - (bounds.width + offset); //absolute
					} else {
						if(child->isContainer()){
							if(child->parent()->isTopContainer() || (inBounds.width != child->parent()->getBounds().width)){
								offset = (mBounds.x + mBounds.width) - (bounds.x + bounds.width);
								bounds.x = (inBounds.x + inBounds.width) - (bounds.width + offset); //absolute
							}
						}
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
        }
        switch (layout.mVResize) {
            case layout_FixedTop :
                break;
            case layout_FixedBottom :
					if(!child->relativeOrigin()){
							offset = (mBounds.y + mBounds.height) - (bounds.y + bounds.height);
							bounds.y = (inBounds.y + inBounds.height) - (bounds.height + offset);
					} else {
						if(child->isContainer()){
							if(child->parent()->isTopContainer() || (inBounds.height != child->parent()->getBounds().height)){
								offset = (mBounds.y + mBounds.height) - (bounds.y + bounds.height);
								bounds.y = (inBounds.y + inBounds.height) - (bounds.height + offset);
							}
						}
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
	if(mParent->relativeOrigin()){
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
    float left = absBounds.x;
    float top = absBounds.y;
    float height = absBounds.height;
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
	if(mParent->relativeOrigin()){
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
    float left = absBounds.x;
    float top = absBounds.y;
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
        CGContextRef cgc = (CGContextRef) UIGraphicsGetCurrentContext();
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

    CGContextRef cgc = (CGContextRef)UIGraphicsGetCurrentContext();;
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

void SCSlider::touchTrack(SCPoint where, UITouch *touch)
{
    if (/*modifiers & NSCommandKeyMask*/0) {
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

    CGContextRef cgc = (CGContextRef)UIGraphicsGetCurrentContext();
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


void SCButton::touchTrack(SCPoint where, UITouch *touch)
{
	if (/*modifiers & NSCommandKeyMask*/0) {
        beginDrag(where);
    } else {
		bool inside = hit(where);
		if (inside != mPushed) {
			mPushed = inside;
			refresh();
		}
	}
}

void SCButton::touchEndTrack(SCPoint where, UITouch *touch)
{
    bool inside = hit(where);
    if (inside) setValue(mValue+1, true, 0);
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

SCView* NewSCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCNumberBox(inParent, inObj, inBounds);
}

SCNumberBox::SCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCStaticText(inParent, inObj, inBounds)
{
        mBackground = new SolidColorBackground(SCMakeColor(1.0,1.0,1.0, 1.0)); // default is white
        mEnabled = true;
        mCanFocus = true;
}

SCNumberBox::~SCNumberBox()
{
}

bool SCNumberBox::shouldDim()
{
    return true;
}

void SCNumberBox::draw(SCRect inDamage)
{
	SCView::draw(inDamage);
   	SCRect bounds = getDrawBounds();

	CGContextRef cgc = (CGContextRef)UIGraphicsGetCurrentContext();
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

//int SCNumberBox::setProperty(PyrSymbol *symbol, PyrSlot *slot)
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

void SCNumberBox::touchTrack(SCPoint where, UITouch *touch)
{
/*
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    }
*/
}

//int SCNumberBox::getProperty(PyrSymbol *symbol, PyrSlot *slot)
//{
//	char *name = symbol->name;
// 	if (strcmp(name, "boxColor")==0) {
//             return setSlotColor(slot, &mBoxColor);
//       }
//        return SCStaticText::getProperty(symbol, slot);
//}

bool SCNumberBox::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

void SCNumberBox::receiveDrag()
{
    sendMessage(s_receiveDrag, 0, 0, 0);
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

void registerSCViewClasses()
{
	if (sRegisteredSCViewClasses) return;
	sRegisteredSCViewClasses = true;

	new SCViewMaker("SCTopView", NewSCTopView);
	new SCViewMaker("SCCompositeView", NewSCCompositeView);
	new SCViewMaker("SCHLayoutView", NewSCHLayoutView);
	new SCViewMaker("SCVLayoutView", NewSCVLayoutView);
	new SCViewMaker("SCSlider", NewSCSlider);
	new SCViewMaker("SCButton", NewSCButton);
	new SCViewMaker("SCStaticText", NewSCStaticText);
	new SCViewMaker("SCNumberBox", NewSCNumberBox);
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

	SCView *view = (SCView*)slotRawPtr(slotRawObject(g->sp)->slots);
	if(!view) 	return errNone;
	view->refresh();
	return errNone;
}

int prSCView_RefreshInRect(struct VMGlobals *g, int numArgsPushed);
int prSCView_RefreshInRect(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	SCRect r;
	SCView *view = (SCView*)slotRawPtr(slotRawObject(&(g->sp - 1)[0])->slots);

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
	SCView *view = (SCView*)slotRawPtr(slotRawObject(&viewObjSlot[0])->slots);
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
	s_callDrawHook = getsym("callDrawHook");
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
/*
	ivxSCDragSource_object = instVarOffset("SCDragSource", "object");
	ivxSCDragBoth_object = instVarOffset("SCDragBoth", "object");
	ivxSCTextView_linkAction = instVarOffset("SCTextView", "linkAction");
*/
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


