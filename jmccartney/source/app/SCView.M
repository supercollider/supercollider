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


#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include <pthread.h>
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include "SC_BoundsMacros.h"

#include "SCView.h"

void QDDrawBevelRect(CGContextRef cgc, CGRect bounds, float width, bool inout);

SCViewMaker *gSCViewMakers = 0;

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
PyrSymbol* s_beginDrag;
PyrSymbol* s_receiveDrag;
PyrSymbol* s_canReceiveDrag;

extern pthread_mutex_t gLangMutex;

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

    PyrSlot *slots = slot->uo->slots;
        
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

    PyrSlot *slots = slot->uo->slots;
    
    SetFloat(slots+0, sccolor->red);
    SetFloat(slots+1, sccolor->green);
    SetFloat(slots+2, sccolor->blue);
    SetFloat(slots+3, sccolor->alpha);
    return errNone;
}

int slotGetSCRect(PyrSlot* a, SCRect *r)
{
	PyrSlot *slots = a->uo->slots;
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
    char *classname = classobj->name.us->name;
        
    if (strcmp(classname, "Color")==0) {
        err = slotColorVal(slot, &color1);
        if (err) return err;
        
        delete *ioPtr;
        *ioPtr = new SolidColorBackground(color1);
    } else if (strcmp(classname, "Gradient") == 0) {
        PyrObject *obj = slot->uo;
        PyrSlot *slots = obj->slots;
        
        err = slotColorVal(slots+0, &color1);
        if (err) return err;
        err = slotColorVal(slots+1, &color2);
        if (err) return err;
        
        if (IsSym(slots+2)) {
            if (strncmp(slots[2].us->name, "h", 1)==0) direction = grad_Horizontal;
            else if (strncmp(slots[2].us->name, "v", 1)==0) direction = grad_Vertical;
            else if (strncmp(slots[2].us->name, "n", 1)==0) direction = grad_Narrow;
            else if (strncmp(slots[2].us->name, "w", 1)==0) direction = grad_Wide;
            else direction = grad_Vertical;
        } else {
            direction = grad_Horizontal;
        }
        
        err = slotIntVal(slots+3, &steps);
        if (err) return err;

        delete *ioPtr;
        *ioPtr = new GradientBackground(color1, color2, direction, steps);
        
    } else if (strcmp(classname, "HiliteGradient") == 0) {
        PyrObject *obj = slot->uo;
        PyrSlot *slots = obj->slots;
        
        err = slotColorVal(slots+0, &color1);
        if (err) return err;
        err = slotColorVal(slots+1, &color2);
        if (err) return err;
        
        if (IsSym(slots+2)) {
            if (strncmp(slots[2].us->name, "h", 1)==0) direction = grad_Horizontal;
            else if (strncmp(slots[2].us->name, "v", 1)==0) direction = grad_Vertical;
            else if (strncmp(slots[2].us->name, "n", 1)==0) direction = grad_Narrow;
            else if (strncmp(slots[2].us->name, "w", 1)==0) direction = grad_Wide;
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
    mShouldResize(true), mHResize(layout_FixedLeft), mVResize(layout_FixedTop)
{
}

SCView::SCView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: mNext(0), mParent(0), mTop(0), mObj(inObj), mBounds(inBounds), 
	mVisible(true), mEnabled(true), mCanFocus(true), mDragHilite(false),
        mBackground(0)
{
	if (inParent) inParent->add(this);
	// store myself into sc object.
	if (mObj) SetPtr(mObj->slots+0, this);
        
}

SCView::~SCView()
{
    if (mParent) mParent->remove(this);
    if (mObj) SetNil(mObj->slots+0);
    delete mBackground;
}

bool SCView::hit(SCPoint where) const
{
	return SCPointInRect(where, mBounds);
}

void SCView::keyDown(int character, int modifiers)
{
    PyrSymbol *method = getsym("keyDown");
    pthread_mutex_lock (&gLangMutex);
    if (mObj) {
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, mObj); 
        ++g->sp;  SetChar(g->sp, character); 
        ++g->sp;  SetInt(g->sp, modifiers); 
        runInterpreter(g, method, 3);
        g->canCallOS = false;
    }
    pthread_mutex_unlock (&gLangMutex);
}

void SCView::mouseBeginTrack(SCPoint where, int modifiers)
{
	mouseTrack(where, modifiers);
}

void SCView::mouseTrack(SCPoint where, int modifiers)
{
}

void SCView::mouseEndTrack(SCPoint where, int modifiers)
{
	mouseTrack(where, modifiers);
}

void SCView::mouseOver(SCPoint where)
{
}

bool SCView::canReceiveDrag()
{
    return false;
}

void SCView::receiveDrag()
{
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
    if (mBackground) {
        CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
        CGRect rect = SCtoCGRect(mBounds);
        mBackground->draw(cgc, rect);
    }
}

void SCView::drawDisabled(SCRect inDamage)
{
    if (!mEnabled && shouldDim()) {
        CGRect rect = SCtoCGRect(mBounds);
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
        CGRect rect = SCtoCGRect(mBounds);
        rect.origin.x -= 2;
        rect.origin.y -= 2;
        rect.size.width += 4;
        rect.size.height += 4;
        CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
        CGContextSaveGState(cgc);
        CGContextSetLineWidth(cgc, 2);
        CGContextSetRGBStrokeColor(cgc, 0., 0., 0., 0.5);
        CGContextStrokeRect(cgc, rect);
        CGContextRestoreGState(cgc);
    }
}

void SCView::drawDragHilite(SCRect inDamage)
{
    if (mDragHilite) {
        CGRect rect = SCtoCGRect(mBounds);
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
	if (SCRectsDoIntersect(inDamage, mBounds) && mVisible) {
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

void SCView::setBounds(SCRect inBounds)
{
    mBounds = inBounds;
}

SCRect SCView::getBounds()
{
    return mBounds;
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
        }
    } else {
        if (isFocus()) {
            mTop->focusIs(0);
            refreshFocus();
        }
    }
}

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
    mTop->addDamage(mBounds);
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
            ++g->sp;  g->sp->ucopy = args[i].ucopy; 
        }
        runInterpreter(g, method, numargs+1);
        g->canCallOS = false;
        if (result) result->ucopy = g->result.ucopy;
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
                mBounds = screct;
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
            int resize = slot->ui;
            if (resize < 1 || resize > 9) return errIndexOutOfRange;
            mLayout.mHResize = ((resize - 1) % 3) - 1;
            mLayout.mVResize = ((resize - 1) / 3) - 1;
            mTop->refresh();
            return errNone;
	}
	/*if (strcmp(name, "backColor")==0) {
            return setSlotColor(slot, &mBackColor);
	}*/
	if (strcmp(name, "background")==0) {
            err = slotBackgroundVal(slot, &mBackground);
            if (err) return err;
            refresh();
            return errNone;
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
        
            PyrSlot *slots = slot->uo->slots;
            SetFloat(slots+0, mBounds.x);
            SetFloat(slots+1, mBounds.y);
            SetFloat(slots+2, mBounds.width);
            SetFloat(slots+3, mBounds.height);
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
	/*if (strcmp(name, "background")==0) {
            int err = getBackgroundVal(slot, mBackground);
            return err;
	}*/
	/*if (strcmp(name, "backColor")==0) {
            return setSlotColor(slot, &mBackColor);
	}*/
	return errPropertyNotFound;
}

void SCView::beginDrag(SCPoint where)
{
    sendMessage(s_beginDrag, 0, 0, 0);
    
    PyrSlot slot;
    pthread_mutex_lock (&gLangMutex);
    if (mObj) {
        VMGlobals *g = gMainVMGlobals;
        int classIndex = getsym("SCView")->u.classobj->classIndex.ui;
        PyrObject *classvars = g->classvars[classIndex].uo;
        slot.ucopy = classvars->slots[0].ucopy;
    }
    pthread_mutex_unlock (&gLangMutex);
    
    mTop->beginDragCallback(where, &slot);
}

////////////////////////////////////////////////////////////////////////////////////////////////

SCContainerView::SCContainerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mChildren(0), mNumChildren(0)
{
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
    if (SCRectsDoIntersect(inDamage, mBounds) && mVisible) {
            draw(inDamage);
            SCView *child = mChildren;
            while (child) {
                child->drawIfNecessary(inDamage);
                child = child->mNext;
            }
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
                    SCView *found = child->findView(where);
                    if (found) return found;
                    child = child->mNext;
            }
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
    while (child) {
        SCRect bounds = child->getBounds();
        Layout layout = child->getLayout();
        float offset;
        switch (layout.mHResize) {
            case layout_FixedLeft : 
                break;
            case layout_FixedRight :
                offset = (mBounds.x + mBounds.width) - (bounds.x + bounds.width);
                bounds.x = (inBounds.x + inBounds.width) - (bounds.width + offset);
                break;
            case layout_HElastic :
                offset = (mBounds.x + mBounds.width) - (bounds.x + bounds.width);
                bounds.width = (inBounds.width) - (bounds.x + offset);
        }
        switch (layout.mVResize) {
            case layout_FixedTop : 
                break;
            case layout_FixedBottom :
                offset = (mBounds.y + mBounds.height) - (bounds.y + bounds.height);
                bounds.y = (inBounds.y + inBounds.height) - (bounds.height + offset);
                break;
            case layout_VElastic :
                offset = (mBounds.y + mBounds.height) - (bounds.y + bounds.height);
                bounds.height = (inBounds.height) - (bounds.y + offset);
        }
        child->setBounds(bounds);
        child = child->next();
    }
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
    float totalWidth = mSpacing * (mNumChildren + 1);
    SCView *child = mChildren;
    while (child) {
        SCRect bounds = child->getBounds();
        Layout layout = child->getLayout();
        
        child->setBounds(bounds);
        child = child->next();
    }
    mBounds = inBounds;
}


/////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCTopView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCTopView(inObj, inBounds);
}

SCTopView::SCTopView(PyrObject* inObj, SCRect inBounds)
	: SCCompositeView(0, inObj, inBounds), mFocusView(0), mDragView(0)
{
    mTop = this;
    float ltgry = 0.8;
    float dkgry = 0.5;
    mBackground = new SolidColorBackground(
                    SCMakeColor(ltgry, ltgry, ltgry, 1.0));
}

void SCTopView::addDamage(SCRect inRect)
{
    (*mDamageCallback)(inRect, mHostData);
    //mDamage = SCRectUnion(mDamage, inRect);
}

void SCTopView::beginDragCallback(SCPoint where, PyrSlot* slot)
{
    (*mDragCallback)(where, slot, mHostData);
}

void SCView::refreshFocus()
{
    SCRect focusBounds = mBounds;
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

///////////////////////////////////////////////////////////////////////////////////////

SCView* NewSCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCSlider(inParent, inObj, inBounds);
}

SCSlider::SCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mStepSize(0.), mStepScale(0.), mKnob(0)
{
        mValue = 1.;
	setValue(0.0, false);
}

int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);

void SCSlider::draw(SCRect inDamage)
{
    
    calcThumbRect();
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);
    QDDrawBevelRect(cgc, SCtoCGRect(mThumbRect), 2, false);
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

const int THUMBSIZE = 12;

void SCSlider::setValueFromPoint(SCPoint point)
{
    double moveableRange, value;
    
    if (mBounds.width > mBounds.height) {
        moveableRange = mBounds.width - THUMBSIZE - 2;
        value = (point.x - mBounds.x - 1 - THUMBSIZE/2) / moveableRange;
    } else {
        moveableRange = mBounds.height - THUMBSIZE - 2;
        value = (point.y - mBounds.y - 1 - THUMBSIZE/2) / moveableRange;
    }
    setValue(value, true);
}

void SCSlider::calcThumbRect()
{
    double moveableRange;
    
    moveableRange = (mBounds.width > mBounds.height) 
                  ?  mBounds.width : mBounds.height;
    moveableRange -= THUMBSIZE + 2;
        
    double offset = mValue * moveableRange;

    if (mBounds.width > mBounds.height) {
       mThumbRect = SCMakeRect(mBounds.x + offset + 1, mBounds.y + 1,
                     THUMBSIZE, mBounds.height - 2);   
    } else {
       mThumbRect = SCMakeRect(mBounds.x + 1, mBounds.y + offset + 1,
                     mBounds.width - 2, THUMBSIZE);   
    }
}

void SCSlider::mouseTrack(SCPoint where, int modifiers)
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
	SCColor sccolor;
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

int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);

void SCRangeSlider::draw(SCRect inDamage)
{
    calcRangeRect();
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    
    CGRect cgBounds = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, cgBounds);
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
    if (mBounds.width > mBounds.height) {
        moveableRange = mBounds.width - 5;
        lo = (sc_min(mAnchor.x, where.x) - mBounds.x - 2) / moveableRange;
        hi = (sc_max(mAnchor.x, where.x) - mBounds.x - 2) / moveableRange;
    } else {
        moveableRange = mBounds.height - 5;
        lo = (sc_min(mAnchor.y, where.y) - mBounds.y - 2) / moveableRange;
        hi = (sc_max(mAnchor.y, where.y) - mBounds.y - 2) / moveableRange;
    }
    setValue(lo, hi, true);
}

void SCRangeSlider::calcRangeRect()
{
    double moveableRange;
    
    moveableRange = (mBounds.width > mBounds.height) 
                  ?  mBounds.width : mBounds.height;
    moveableRange -= 5;
        
    double lo = mLo * moveableRange;
    double hi = mHi * moveableRange + 1;

    if (mBounds.width > mBounds.height) {
       mRangeRect = SCMakeRect(mBounds.x + lo + 2, mBounds.y + 1,
                     hi - lo, mBounds.height - 2);   
    } else {
       mRangeRect = SCMakeRect(mBounds.x + 1, mBounds.y + lo + 2,
                     mBounds.width - 2, hi - lo);   
    }
}

void SCRangeSlider::mouseBeginTrack(SCPoint where, int modifiers)
{
    mAnchor = where;
    setValueFromPoint(where);
}

void SCRangeSlider::mouseTrack(SCPoint where, int modifiers)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } else {
        setValueFromPoint(where);
    }
}

int SCRangeSlider::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{	
	int err;
	SCColor sccolor;
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

int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);

void SC2DSlider::draw(SCRect inDamage)
{
    calcThumbRect();

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);
    QDDrawBevelRect(cgc, SCtoCGRect(mThumbRect), 2, false);
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

void SC2DSlider::setValueFromPoint(SCPoint where)
{
    double x = (where.x - mBounds.x - 1 - THUMBSIZE/2) / (mBounds.width - THUMBSIZE - 2);
    double y = (where.y - mBounds.y - 1 - THUMBSIZE/2) / (mBounds.height - THUMBSIZE - 2);
    setValue(x, y, true);
}

void SC2DSlider::calcThumbRect()
{        
    double x = mX * (mBounds.width - THUMBSIZE - 2);
    double y = mY * (mBounds.height - THUMBSIZE - 2);

    mThumbRect = SCMakeRect(mBounds.x + x + 1, mBounds.y + y + 1, THUMBSIZE, THUMBSIZE);   
}

void SC2DSlider::mouseTrack(SCPoint where, int modifiers)
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
	SCColor sccolor;
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

///////////////////////////////////////////////////////////////////////////////////////////////


SCView* NewSCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCButton(inParent, inObj, inBounds);
}

SCButton::SCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds), mNumStates(0), mStates(0), mPushed(false)
{
	setValue(0, false);
        strcpy(mFontName, "Helvetica");
        mFontSize = 12.;
}

SCButton::~SCButton()
{
    delete [] mStates;
}

int stringDrawCenteredInRect(char *cString, SCRect rect, char *cFontName, float size, SCColor sccolor);

void SCButton::draw(SCRect inDamage)
{
    SCColor buttonColor;
    if (mStates) {
        SCButtonState *state = mStates + mValue;
        buttonColor = state->mButtonColor;
        //drawBevelRect(SCtoQDRect(mBounds), 2, mPushed ? 1 : 0, SCtoQDColor(buttonColor), 2);
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    
    CGRect cgrect = SCtoCGRect(mBounds);
    if (buttonColor.alpha > 0.0) {
        CGContextSetRGBFillColor(cgc, buttonColor.red, buttonColor.green, buttonColor.blue, buttonColor.alpha);
        CGContextFillRect(cgc, cgrect);
    }
    QDDrawBevelRect(cgc, cgrect, 2, mPushed);
    CGContextRestoreGState(cgc);

        SCRect innerBounds = mBounds;
        int inset = 2;
        int pushOffset = mPushed ? 2 : 0;
        innerBounds.x += inset + pushOffset;
        innerBounds.y += inset + pushOffset;
        innerBounds.width -= inset * 2 + pushOffset;
        innerBounds.height -= inset * 2 + pushOffset;
        stringDrawCenteredInRect(state->mLabel, innerBounds, mFontName, mFontSize, state->mLabelColor);
    }
}

bool SCButton::setValue(int inValue, bool send)
{
    bool changed = inValue != mValue;
    if (inValue < 0 || inValue >= mNumStates) inValue = 0;
    if (inValue != mValue || mNumStates < 2) {
        mValue = inValue;
        refresh();
        if (send) sendMessage(s_doaction, 0, 0, 0);
    }
    return changed;
}


void SCButton::mouseTrack(SCPoint where, int modifiers)
{
    bool inside = hit(where);
    if (inside != mPushed) {
        mPushed = inside;
        refresh();
    }
}

void SCButton::mouseEndTrack(SCPoint where, int modifiers)
{
    bool inside = hit(where);
    if (inside) setValue(mValue+1, true);
    mPushed = false;
}

int SCMakeButtonState(SCButton* view, SCButtonState *inState, PyrSlot *slot)
{
    int err;
    if (!isKindOfSlot(slot, class_array)) return errWrongType;
    PyrSlot *slots = slot->uo->slots;
    
    inState->mLabel[0] = 0;
    inState->mLabelColor = SCMakeColor(0,0,0,1); // black
    inState->mButtonColor = SCMakeColor(0.7,0.7,0.7,1);
    
    if (slot->uo->size < 1) return errNone;
    err = slotStrVal(slots+0, inState->mLabel, kLabelSize);
    if (err) return err;
    
    if (slot->uo->size < 2) return errNone;
    err = slotColorVal(slots+1, &inState->mLabelColor);
    if (err) {
        inState->mLabelColor = SCMakeColor(0,0,0,1); // black
    }

    if (slot->uo->size < 3) return errNone;
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
            bool changed = setValue(value, false);
            SetBool(slot, changed);
            return errNone;
	}
	char *name = symbol->name;
	if (strcmp(name, "font")==0) {
            if (!isKindOfSlot(slot, getsym("Font")->u.classobj)) return errWrongType;
            PyrSlot *slots = slot->uo->slots;
        
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
            
            PyrObject *array = slot->uo;
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
	char *name = symbol->name;

        return SCView::getProperty(symbol, slot);
}



SCView* NewSCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCUserView(inParent, inObj, inBounds);
}

SCUserView::SCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds)
{
}

void SCUserView::draw(SCRect inDamage)
{
    sendMessage(s_draw, 0, 0, 0);
}


void SCUserView::mouseAction(PyrSymbol *method, SCPoint where, int modifiers)
{
    PyrSlot args[3];
    SetFloat(args+0, where.x); 
    SetFloat(args+1, where.y); 
    SetInt(args+2, modifiers); 
    sendMessage(method, 3, args, 0);
}

void SCUserView::mouseBeginTrack(SCPoint where, int modifiers)
{
    mouseAction(getsym("mouseBeginTrack"), where, modifiers);
}

void SCUserView::mouseTrack(SCPoint where, int modifiers)
{
    mouseAction(getsym("mouseTrack"), where, modifiers);
}

void SCUserView::mouseEndTrack(SCPoint where, int modifiers)
{
    mouseAction(getsym("mouseEndTrack"), where, modifiers);
}

void SCUserView::keyDown(int character, int modifiers)
{
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

int stringDrawCenteredInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);
int stringDrawLeftInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);
int stringDrawRightInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor);

void SCStaticText::draw(SCRect inDamage)
{
    if (mString) {
        SCView::draw(inDamage);
        SCRect bounds = mBounds;

        bounds.x += 4; bounds.y += 4; bounds.width -= 8; bounds.height -= 8;
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
    if (IsSym(slot)) {
            len = strlen(slot->us->name);
            *str = new char[len+1];
            strcpy(*str, slot->us->name);
            return errNone;
    } else if (isKindOfSlot(slot, class_string)) {
            len = slot->uo->size;
            *str = new char[len+1];
            memcpy(*str, slot->uos->s, len);
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
            if (!isKindOfSlot(slot, getsym("Font")->u.classobj)) return errWrongType;
            PyrSlot *slots = slot->uo->slots;
        
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
                    if (slot->us->name[0] == 'l') mAlignment = -1;
                    else if (slot->us->name[0] == 'r') mAlignment = 1;
                    else if (slot->us->name[0] == 'c') mAlignment = 0;
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
////////////////////////////////////////////////

SCView* NewSCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCNumberBox(inParent, inObj, inBounds);
}

SCNumberBox::SCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCStaticText(inParent, inObj, inBounds)
{
        mBoxColor = SCMakeColor(1,1,1,1);
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
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, rect);

    if (mBoxColor.alpha > 0.0) {
        CGContextSetRGBFillColor(cgc, mBoxColor.red, mBoxColor.green, mBoxColor.blue, mBoxColor.alpha);
        CGContextFillRect(cgc, rect);
    }

    QDDrawBevelRect(cgc, rect, 1, true);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
    SCStaticText::draw(inDamage);
}

int SCNumberBox::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{	
	int err;
	char *name = symbol->name;
 	if (strcmp(name, "boxColor")==0) {
		err = slotColorVal(slot, &mBoxColor);
                if (err) return err;
                refresh();
		return errNone;
	}
       return SCStaticText::setProperty(symbol, slot);
}

void SCNumberBox::mouseTrack(SCPoint where, int modifiers)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    } 
}

int SCNumberBox::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
 	if (strcmp(name, "boxColor")==0) {
             return setSlotColor(slot, &mBoxColor);
       }
        return SCStaticText::getProperty(symbol, slot);
}

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
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, false);
    CGContextRestoreGState(cgc);
        
    //drawBevelRect(SCtoQDRect(mBounds), 1, 0, SCtoQDColor(mBackColor), 2);
    SCStaticText::draw(inDamage);
}

bool SCDragSource::shouldDim()
{
    return true;
}

int ivxSCDragSource_object;

void SCDragSource::mouseBeginTrack(SCPoint where, int modifiers)
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
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
    SCStaticText::draw(inDamage);
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
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGRect rect = SCtoCGRect(mBounds);
    if (mBackground) mBackground->draw(cgc, rect);
    QDDrawBevelRect(cgc, rect, 1, true);
    SCRect bounds = mBounds;
    bounds.x += 2; bounds.y += 2; bounds.width -= 4; bounds.height -= 4;
    QDDrawBevelRect(cgc, SCtoCGRect(bounds), 1, false);
    CGContextRestoreGState(cgc);

    //drawBevelRect(SCtoQDRect(mBounds), 1, 1, SCtoQDColor(mBackColor), 2);
    //SCRect bounds = mBounds;
    //bounds.x += 2; bounds.y += 2; bounds.width -= 4; bounds.height -= 4;
    //drawBevelRect(SCtoQDRect(bounds), 1, 0, SCtoQDColor(mBackColor), 2);
    SCStaticText::draw(inDamage);
}

int ivxSCDragBoth_object;

void SCDragBoth::mouseBeginTrack(SCPoint where, int modifiers)
{
    beginDrag(where);
}

////////////////////////////////////////////////

SCViewMaker::SCViewMaker(const char* inName, SCViewCtor inCtor)
    : mNext(gSCViewMakers), mCtor(inCtor), mName(inName)
{
	gSCViewMakers = this;
}

SCView* MakeSCView(PyrObject* inObj, SCContainerView *inParent, SCRect inBounds)
{
        const char *classname = inObj->classptr->name.us->name;
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
    new SCViewMaker("SCSlider", NewSCSlider);
    new SCViewMaker("SCRangeSlider", NewSCRangeSlider);
    new SCViewMaker("SC2DSlider", NewSC2DSlider);
    new SCViewMaker("SCButton", NewSCButton);
    new SCViewMaker("SCStaticText", NewSCStaticText);
    new SCViewMaker("SCNumberBox", NewSCNumberBox);
    new SCViewMaker("SCDragSource", NewSCDragSource);
    new SCViewMaker("SCDragSink", NewSCDragSink);
    new SCViewMaker("SCDragBoth", NewSCDragBoth);
    new SCViewMaker("SCUserView", NewSCUserView);
}



int prSCView_New(struct VMGlobals *g, int numArgsPushed);
int prSCView_New(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 2;
        // view, parent, bounds
    
        SCContainerView* parent;
        if (isKindOfSlot(args+0, s_sctopview->u.classobj)) {
            parent = 0;
        } else {
            if (!isKindOfSlot(args+1, s_sccontview->u.classobj)) return errWrongType;
            parent = (SCContainerView*)args[1].uo->slots[0].ui;
        }
	
	SCRect bounds;
	int err = slotGetSCRect(args+2, &bounds);
	if (err) return err;
	
	SCView *view = MakeSCView(args[0].uo, parent, bounds);
	if (!view) return errFailed;

	return errNone;
}

int prSCView_SetProperty(struct VMGlobals *g, int numArgsPushed);
int prSCView_SetProperty(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 2;
	
	if (!IsSym(args+1)) return errWrongType;
	
	SCView *view = (SCView*)args[0].uo->slots[0].ui;
	if (!view) return errFailed;

	int err = view->setProperty(args[1].us, args+2);
        if (err) SetNil(args+2);
        
        args[0].ucopy = args[2].ucopy;
        
	return err;
}

int prSCView_GetProperty(struct VMGlobals *g, int numArgsPushed);
int prSCView_GetProperty(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 2;
	
	if (!IsSym(args+1)) return errWrongType;
	
	SCView *view = (SCView*)args[0].uo->slots[0].ui;
	if (!view) return errFailed;

	int err = view->getProperty(args[1].us, args+2);
        if (err) SetNil(args+2);
        
        args[0].ucopy = args[2].ucopy;
        
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
        s_beginDrag = getsym("beginDrag");
        s_receiveDrag = getsym("receiveDrag");
        s_canReceiveDrag = getsym("canReceiveDrag");
        
	base = nextPrimitiveIndex();
	index = 0;
	
	definePrimitive(base, index++, "_SCView_New", prSCView_New, 3, 0);
	definePrimitive(base, index++, "_SCView_SetProperty", prSCView_SetProperty, 3, 0);
	definePrimitive(base, index++, "_SCView_GetProperty", prSCView_GetProperty, 3, 0);	
}

void initGUI();
void initGUI()
{
	ivxSCDragSource_object = instVarOffset("SCDragSource", "object");
	ivxSCDragBoth_object = instVarOffset("SCDragBoth", "object");
}


/*
loose ends
drag/drop views
background color / pic

new views
    text view
    button view
    scope view
    envelope view
    popup menu
    plot view
        graph limits
        grid on/off
        data 1d, 2d
            color
    hlayoutview
    vlayoutview
    hdivider
    vdivider
    overlay view

*/


