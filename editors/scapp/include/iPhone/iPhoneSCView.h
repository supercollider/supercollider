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

#import "DrawBackground.h"
#include "PyrObject.h"

enum {
    layout_NoResize,
    layout_Resize
};

enum {
    // mHResize
    layout_FixedLeft = -1,
    layout_HElastic,
    layout_FixedRight,
};
enum {
    // mVResize
    layout_FixedTop = -1,
    layout_VElastic,
    layout_FixedBottom
};

enum {
    view_ResizeConstructionMode,
    view_PositionConstructionMode,
};

struct Layout
{
    Layout();

    // layout params for dynamic layout views
    float mMinWidth, mMaxWidth, mMinHeight, mMaxHeight;
    float mWeight;
    bool mShouldResize;
	SCRect bounds; //relative Bounds in SCCompositeView
    // layout params for composite views
    char mHResize, mVResize;
};



class SCView;
class SCContainerView;
class SCTopView;

typedef SCView* (*SCViewCtor)(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

struct SCViewMaker
{
	SCViewMaker(const char* inName, SCViewCtor inCtor);
	static SCView* MakeSCView(PyrObject* inObj, SCContainerView *inParent, SCRect inBounds, const char* classname);

	SCViewMaker *mNext;
	SCViewCtor mCtor;
	const char* mName;
};

extern SCViewMaker *gSCViewMakers;
extern SCView *gAnimatedViews;

class SCView
{
public:
	SCView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCView();

	virtual void draw(SCRect inDamage);
	virtual void drawFocus(SCRect inDamage);
	virtual void drawDisabled(SCRect inDamage);
	virtual void drawDragHilite(SCRect inDamage);
	virtual void drawIfNecessary(SCRect inDamage);
	virtual void touchBeginTrack(SCPoint where, UITouch *touch);
	virtual void touchTrack(SCPoint where, UITouch *touch);
	virtual void touchEndTrack(SCPoint where, UITouch *touch);
	virtual void touchOver(SCPoint where, UITouch *touch);
	virtual void draggingEntered (SCPoint where);
	virtual void draggingUpdated (SCPoint where);
	virtual void keyDown(int character, int modifiers, unsigned short keycode);
	virtual void keyUp(int character, int modifiers, unsigned short keycode);
	virtual void keyModifiersChanged(int modifiers);
	virtual void touchDownAction(SCPoint where, UITouch *touch);
	virtual void touchMoveAction(SCPoint where, UITouch *touch);
	virtual void touchUpAction(SCPoint where, UITouch *touch);
	void doConstructionMove(SCPoint where);
	void setConstructionModeFromPoint(SCPoint where);
	virtual bool shouldDim();
	void beginDrag(SCPoint where);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

	bool isFocus() const;
	bool hit(SCPoint p) const;
	void refresh();
	virtual void refreshInRect(SCRect b);
	void refreshFocus();
	void setDragHilite(bool inFlag);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual void setVisibleFromParent() { } // does nothing, needed for Cocoa views

	virtual bool isDragSource() const;
	virtual SCView* findView(SCPoint where);
	virtual SCView* findViewByID(int32 inID);
	virtual void makeFocus(bool focus);
	virtual SCView* nextFocus(bool *foundFocus, bool canFocus);
	virtual SCView* prevFocus(SCView **prevView, bool canFocus);
	virtual bool canFocus();
	//virtual NSView* focusResponder();

	void sendMessage(PyrSymbol *method, int numargs, PyrSlot *args, PyrSlot *result);

	virtual void setBounds(SCRect inBounds);
	virtual SCRect getBounds();
	virtual SCRect getDrawBounds(); //relative to ContainerView
	virtual Layout getLayout();
//	virtual void setLayout(Layout inLayout);


	SCView* next() { return mNext; }
	SCContainerView* parent() { return mParent; }

	//virtual NSMenu* contextMenu(SCPoint inPoint);

	virtual void setMenuItemChosen(int inItem) {}

	PyrObject* GetSCObj() { return mObj; }
	SCView* NextAnimatedView() const { return mNextAnimatedView; }

	void startAnimation();
	void stopAnimation();
	virtual void animate() { refresh(); }

	virtual bool isScroller() { return false; }
	virtual bool isSubViewScroller() { return false; }
	virtual bool isContainer() { return false; }
	virtual SCRect checkMinimumSize() { return mBounds; }
	virtual bool relativeOrigin() {return false;}

	bool isTopContainer(){ return (!mParent);};

protected:
	friend class SCContainerView;
	friend class SCScrollView;

	SCView *mNext;
	SCView *mNextAnimatedView;
	SCView *mPrevAnimatedView;
	SCContainerView *mParent;
	SCTopView *mTop;
	PyrObject* mObj;
	SCRect mBounds;
	Layout mLayout;
	DrawBackground* mBackground;
	DrawBackground* mBackgroundImage;
	bool mVisible;
	bool mEnabled;
	bool mCanFocus;
	bool mDragHilite;
	int32 mID;
	int mConstructionMode;
	SCColor mFocusColor;
	NSString *mDragLabel;

};


class SCContainerView : public SCView
{
public:
	SCContainerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCContainerView();

	virtual void drawIfNecessary(SCRect inDamage);

	virtual void add(SCView *inChild);
	virtual void remove(SCView *inChild);
	virtual SCView* findView(SCPoint where);
	virtual SCView* findViewByID(int32 inID);
	virtual void makeFocus(bool focus);
	virtual SCView* nextFocus(bool *foundFocus, bool canFocus);
	virtual SCView* prevFocus(SCView **prevView, bool canFocus);
	virtual bool canFocus();
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual SCRect checkMinimumSize();
	virtual void setVisibleFromParent();
	virtual bool isVisible() {return mVisible && mParent->isVisible(); }
	virtual bool relativeOrigin() {return mRelativeOrigin;}
	virtual bool isContainer() { return true; }

protected:
	SCView *mChildren;
	int mNumChildren;
	bool mRelativeOrigin;
};

class SCCompositeView : public SCContainerView
{
public:
	SCCompositeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCCompositeView();

	virtual void setBounds(SCRect inBounds);
protected:
};

class SCLayoutView : public SCContainerView
{
public:
	SCLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCLayoutView();

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void add(SCView *inChild);

protected:
    float mSpacing;
};

class SCHLayoutView : public SCLayoutView
{
public:
	SCHLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCHLayoutView();

	virtual void setBounds(SCRect inBounds);

protected:
};

class SCVLayoutView : public SCLayoutView
{
public:
	SCVLayoutView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCVLayoutView();

	virtual void setBounds(SCRect inBounds);

protected:
};

// tell host to draw stuff.
typedef void (*DamageCallback)(SCRect inRect, void *inData);
typedef void (*DragCallback)(SCPoint where, PyrSlot* inSlot, NSString* inString, NSString* label, void *inData);

class SCTopView : public SCCompositeView
{
public:
	SCTopView(PyrObject* inObj, SCRect inBounds);

	SCView *focusView() {
		if(isSubViewScroller()) {
			return mTop->focusView();
		} else {
			return mFocusView;
		}
	}

	void forgetView(SCView *view);

	void resetFocus();
	void addDamage(SCRect inRect);
	void beginDragCallback(SCPoint where, PyrSlot* slot, NSString* string, NSString* label);

	void setDamageCallback(DamageCallback inFunc, void *inHostData)
            { mDamageCallback = inFunc; mHostData = inHostData; }
	void setDragCallback(DragCallback inFunc)
            { mDragCallback = inFunc; }

	void tabNextFocus();
	void tabPrevFocus();
	void setDragView(SCView *inView);

	UIView* GetUIView() { return mUIView; }
	void SetUIView(UIView* inView) { mUIView = inView; }

	bool ConstructionMode() { return mConstructionMode; }
	void SetConstructionMode(bool inFlag) { mConstructionMode = inFlag; }

	virtual void drawFocus(SCRect inDamage);
	virtual bool canReceiveDrag();
	virtual void receiveDrag();
	virtual void setInternalBounds(SCRect internalBounds);
	virtual bool isVisible() {return mVisible; }
protected:
	friend class SCView;
	void focusIs(SCView *inView) {
		if(isSubViewScroller()) {
			mTop->focusIs(inView);
		} else {
			mFocusView = inView;
		}
	}

	DamageCallback mDamageCallback;
	DragCallback mDragCallback;
	void *mHostData;
	SCView *mFocusView;
	SCView *mDragView;
	UIView *mUIView;

	bool mConstructionMode;
};

/*
 SCScrollTopView and SCScrollView by Scott Wilson
 Development funded in part by the Arts and Humanites Research Council http://www.ahrc.ac.uk/
 */

class SCScrollTopView : public SCTopView
{
public:
	SCScrollTopView(PyrObject* inObj, SCRect inBounds);

	UIScrollView* GetScrollView() { return mUIScrollView; }
	void SetUIScrollView(UIScrollView* inView) { mUIScrollView = inView; }

	virtual bool isScroller() { return true; }

	virtual SCRect getDrawBounds();
	virtual void setInternalBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual SCRect checkMinimumSize();
	virtual void add(SCView *inChild);
	virtual void remove(SCView *inChild);
	bool isInSetClipViewOrigin() { return mInSetClipViewOrigin; }
	void setInSetClipViewOrigin(bool flag) { mInSetClipViewOrigin = flag; }

protected:
	UIScrollView *mUIScrollView;
	bool mInSetClipViewOrigin;
};

class SCScrollView : public SCScrollTopView
{
public:
	SCScrollView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCScrollView();
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void setVisibleFromParent();
	//virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual void setBounds(SCRect inBounds);
	virtual SCRect getBounds();
	virtual void add(SCView *inChild);
//	virtual void remove(SCView *inChild);

	virtual bool isSubViewScroller() { return true; }

	virtual void drawIfNecessary(SCRect inDamage);
	virtual void drawSubViewIfNecessary(SCRect inDamage);
	virtual bool isVisible() {return mVisible && mParent->isVisible(); }
	virtual bool relativeOrigin() {return false;}
};

inline bool SCView::isFocus() const { return mTop->focusView() == this; }

class SCSlider : public SCView
{
public:
	SCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

	virtual void draw(SCRect inDamage);
	virtual void touchTrack(SCPoint where, UITouch *touch);

	double value() { return mValue; }
	bool setValue(double inValue, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
	virtual void setValueFromPoint(SCPoint point);
	void calcThumbRect(SCRect bounds);

	SCRect mThumbRect;
	double mValue, mStepSize, mStepScale;
	DrawBackground* mKnob;
	float mThumbSize;
};
SCView* NewSCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


const int kLabelSize = 64;
struct SCButtonState
{
    char mLabel[kLabelSize];
    SCColor mLabelColor;
    SCColor mButtonColor;
};

const int kFontNameSize = 80;

class SCButton : public SCView
{
public:
	SCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCButton();

	virtual void draw(SCRect inDamage);
	virtual void touchTrack(SCPoint where, UITouch *touch);
    virtual void touchEndTrack(SCPoint where, UITouch *touch);

	bool setValue(int inValue, bool send, int modifiers);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:

	int mValue;
	char mFontName[kFontNameSize];
	float mFontSize;
	int mNumStates;
	SCButtonState *mStates;
	bool mPushed;
};
SCView* NewSCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

enum {
    kSCAlignLeft = -1,
    kSCAlignCenter,
    kSCAlignRight
};

class SCStaticText : public SCView
{
public:
	SCStaticText(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCStaticText();

	virtual void draw(SCRect inDamage);
	virtual bool shouldDim();

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

protected:
	virtual void drawString(SCRect bounds);

    char *mString;
    char mFontName[kFontNameSize];
    float mFontSize;
    SCColor mStringColor;
    int mAlignment;

};
SCView* NewSCStaticText(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

class SCNumberBox : public SCStaticText
{
public:
	SCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCNumberBox();

	virtual void draw(SCRect inDamage);
	virtual bool shouldDim();

	virtual void touchTrack(SCPoint where, UITouch *touch);
	//virtual void mouseEndTrack(SCPoint where, int modifiers);

	//virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	//virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();
};
SCView* NewSCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
