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


struct Layout
{
    Layout();

    // layout params for dynamic layout views
    float mMinWidth, mMaxWidth, mMinHeight, mMaxHeight;
    float mWeight;
    bool mShouldResize;
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
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseOver(SCPoint where);
	virtual void keyDown(int character, int modifiers, unsigned short keycode);
	virtual void keyUp(int character, int modifiers, unsigned short keycode);
	virtual bool shouldDim();
	void beginDrag(SCPoint where);
	
	virtual bool canReceiveDrag();
	virtual void receiveDrag();
	
	bool isFocus() const;
	bool hit(SCPoint p) const;
	void refresh();
	void refreshFocus();
	void setDragHilite(bool inFlag);
	
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	
	virtual bool isDragSource() const;
	virtual SCView* findView(SCPoint where);
	virtual SCView* findViewByID(int32 inID);
	virtual void makeFocus(bool focus);
	virtual SCView* nextFocus(bool *foundFocus, bool canFocus);
	virtual SCView* prevFocus(SCView **prevView, bool canFocus);
	virtual bool canFocus();

	void sendMessage(PyrSymbol *method, int numargs, PyrSlot *args, PyrSlot *result);

	virtual void setBounds(SCRect inBounds);
	virtual SCRect getBounds();
	virtual Layout getLayout();
	
	SCView* next() { return mNext; }
	SCContainerView* parent() { return mParent; }
	
	virtual NSMenu* contextMenu(SCPoint inPoint);
	
	virtual void setMenuItemChosen(int inItem) {}
	
	PyrObject* GetSCObj() { return mObj; }
	SCView* NextAnimatedView() const { return mNextAnimatedView; }
	
	void startAnimation();
	void stopAnimation();
	virtual void animate() { refresh(); }
	
protected:
	friend class SCContainerView;

	SCView *mNext;
	SCView *mNextAnimatedView;
	SCView *mPrevAnimatedView;
	SCContainerView *mParent;
	SCTopView *mTop;
	PyrObject* mObj;
	SCRect mBounds;
	Layout mLayout;
	DrawBackground* mBackground;
	bool mVisible;
	bool mEnabled;
	bool mCanFocus;
	bool mDragHilite;
	int32 mID;
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
	
protected:
	SCView *mChildren;
	int mNumChildren;
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
typedef void (*DragCallback)(SCPoint where, PyrSlot* inSlot, void *inData);

class SCTopView : public SCCompositeView
{
public:
	SCTopView(PyrObject* inObj, SCRect inBounds);
	
	SCView *focusView() { return mFocusView; }

	void resetFocus();
	void addDamage(SCRect inRect);
	void beginDragCallback(SCPoint where, PyrSlot* slot);
	
	void setDamageCallback(DamageCallback inFunc, void *inHostData)
            { mDamageCallback = inFunc; mHostData = inHostData; }
	void setDragCallback(DragCallback inFunc)
            { mDragCallback = inFunc; }
	
	void tabNextFocus();
	void tabPrevFocus();
	void setDragView(SCView *inView);
		
		NSView* GetNSView() { return mNSView; }
		void SetNSView(NSView* inView) { mNSView = inView; }
	
		bool ConstructionMode() { return mConstructionMode; }
		void SetConstructionMode(bool inFlag) { mConstructionMode = inFlag; }
		
	virtual void drawFocus(SCRect inDamage);
		
protected:
	friend class SCView;
	void focusIs(SCView *inView) { mFocusView = inView; }

	DamageCallback mDamageCallback;
	DragCallback mDragCallback;
	void *mHostData;
	SCView *mFocusView;
	SCView *mDragView;
		NSView *mNSView;
		
		bool mConstructionMode;
};

inline bool SCView::isFocus() const { return mTop->focusView() == this; }

class SCSlider : public SCView
{
public:	
	SCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	
	double value() { return mValue; }
	bool setValue(double inValue, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
	virtual void setValueFromPoint(SCPoint point);
	void calcThumbRect();
	
	SCRect mThumbRect;
	double mValue, mStepSize, mStepScale;
	DrawBackground* mKnob;
};
SCView* NewSCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


class SCRangeSlider : public SCView
{
public:	
	SCRangeSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	
	bool setValue(double inLo, double inHi, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
	virtual void setValueFromPoint(SCPoint point);
    // sc.solar addition
	void moveRangeFromPoint(SCPoint point);
	void adjustLoFromPoint(SCPoint point);
	void adjustHiFromPoint(SCPoint point);
    // sc.solar addition end
	void calcRangeRect();
	
	SCRect mRangeRect;
	double mLo, mHi, mStepSize, mStepScale;
	SCPoint mAnchor;
	DrawBackground* mKnob;
};
SCView* NewSCRangeSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);



class SC2DSlider : public SCView
{
public:	
	SC2DSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	
	virtual bool setValue(double inLo, double inHi, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
	virtual void setValueFromPoint(SCPoint point);
	void calcThumbRect();
	
	SCRect mThumbRect;
	double mX, mY;
	double mStepSize, mStepScale;
	DrawBackground* mKnob;
};
SCView* NewSC2DSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


class SC2DTabletSlider : public SC2DSlider
{
public:	
	SC2DTabletSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);
    
    virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);

    virtual bool setValue(double inX, double inY,bool send);

protected:	
    int mClipInBounds;

};
SCView* NewSC2DTabletSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


#include "SC_SndBuf.h"

const int kMaxScopeChannels = 16;
class SCScope : public SCView
{
public:	
	SCScope(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCScope();
	
	virtual void draw(SCRect inDamage);
	virtual void draw0(CGContextRef cgc);
	virtual void draw1(CGContextRef cgc);
	virtual void draw2(CGContextRef cgc);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void animate();
	
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

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
	SndBuf mSndBuf;
	SCPoint mZoom, mInvZoom, mScroll;
	int mStyle; // 0 = separate, 1 = overlay, 2 = x,y.
	SCColor mWaveColors[kMaxScopeChannels];
	SCColor mGridColor;
	bool mGridOn;
};
SCView* NewSCScope(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);



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
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
    virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);

	bool setValue(int inValue, bool send);

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


class SCPopUpMenu : public SCView
{
public:	
	SCPopUpMenu(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCPopUpMenu();

	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	
	bool setValue(int inValue, bool send);
	virtual void setMenuItemChosen(int inItem) { setValue(inItem, true); }

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
	
	int mValue;
	MenuHandle mMenuH;
	char mFontName[kFontNameSize];
	float mFontSize;
	SCColor mStringColor;
};
SCView* NewSCPopUpMenu(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


class SCListView : public SCView
{
public:	
	SCListView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCListView();

	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent);
	
	bool setValue(int inValue, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();
	
	void scrollToValue();

protected:
	
	int mValue;
	CFMutableArrayRef mArray;
	char mFontName[kFontNameSize];
	float mFontSize;
	float mScroll;
	float mAnchorScroll;
	SCPoint mAnchor;
	SCColor mStringColor;
	SCColor mSelectedStringColor;
	SCColor mHiliteColor;
    int mAlignment;
	NSSize mStrSize;
	bool mScrolling;
};
SCView* NewSCListView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

//by jan trutzschler (jt@kandos.de)
class SCMultiSliderView : public SCView
{
public:	
	SCMultiSliderView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCMultiSliderView();

	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
    virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	
    void setSelection(SCPoint where);
	bool setValue(int inX, double inY, bool send);
	//virtual void setPoint(int x, double y, bool send);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	void setVisibleSize();
	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
	int indexFromPoint(SCPoint where);
//	int getVisibleMax();

	double valueFromPoint(SCPoint where);
	void setValueFromPoint(SCPoint point);
	SCRect calcThumbRect(int xIn, double valIn, double step);
	int mThumbSize, mThumbSizeY; // size of the rect
	int mTabSize, mVisibleSize; // size of the table
	SCColor mFillColor;
	SCColor mStrokeColor;
	SCRect mThumbRect;
	double mCurrentY, mCurrentX;
	int mCurrentIndex, mStartIndex, mSelectionSize;
	double mStepSize, mStepScale;
	double * mYValues;
	double * mSecYValues;
	DrawBackground* mKnob;
	float mXOffset ; //space between points
	bool mReadOnly, mDrawLinesActive, mShowIndex, mDrawRectsActive, mIsHorizontal, mIsFilled;
	SCPoint mPrevPoint;
	int mElasticMode;
	double mElasticIndexStep;
	
};
SCView* NewSCMultiSliderView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
////
//by jan truetzschler jt@kandos.de
struct SCEnvObject {
    SCColor mColor; //changes between selected and mObjectColor
	SCColor mObjectColor; //if its not selected
    SCRect mRect;
	int mNumConnection;
    SCPoint mDrawPoint;
	double * mConnections; //tells to where it is connected
	int mNumInputs, mNumOutputs; 
	double x, y;
    bool mIsSelected, mIsVisible, mIsStatic;
	char *mString;	
};
typedef struct SCEnvObject SCEnvObject;

class SCEnvelopeView : public SCView
{
public:	
	SCEnvelopeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCEnvelopeView();
	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
    
	void setSelection(SCPoint where, bool fixed, bool checkForConnection);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:

	void setValueFromPoint(SCPoint point);
	bool setValue(SCEnvObject * envob, double x, double y, bool send);
	int allocSlotEnvObjArray(PyrSlot *slot, SCEnvObject **arr);
	bool setEnvRect(double valX, double valY, SCEnvObject * envobIn);

	int mThumbSize, mThumbSizeY; // size of the rect
	int mTabSize, mVisibleSize, mActiveSize; // size of the table
	SCColor mFillColor, mSelectedColor, mStrokeColor;
	SCRect mThumbRect;
	double mCurrentY, mCurrentX, mAbsoluteX;
	int mCurrentIndex, mStartIndex, mSelectionSize, mLastIndex;
	double mStepSize, mStepScale;
	SCEnvObject * mEnvObj;
	//DrawBackground* mKnob;
	bool  mDrawLinesActive, mShowIndex, mDrawRectsActive, mIsFilled, mIsFixedSelection, mIsEnvView;
	int mSelectedIndex;
	SCPoint mMousePoint;

	double xGridMultiplier;

	//draw string in box
    char mFontName[kFontNameSize];
    float mFontSize;
    SCColor mStringColor;
    int mAlignment;
	bool mDrawCenteredConnection;
	
    
};
SCView* NewSCEnvelopeView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


//

class SCUserView : public SCView
{
public:	
	SCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);	
    virtual void keyDown(int character, int modifiers);
	virtual void keyUp(int character, int modifiers);
	
protected:
	void mouseAction(PyrSymbol *method, SCPoint where, int modifiers);
};
SCView* NewSCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


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

	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	//virtual void mouseEndTrack(SCPoint where, int modifiers);
	
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual bool canReceiveDrag();
	virtual void receiveDrag();

protected:
    SCColor mBoxColor;
};
SCView* NewSCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

class SCDragSource : public SCStaticText
{
public:	
	SCDragSource(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCDragSource();
	
	virtual void draw(SCRect inDamage);
	virtual bool shouldDim();
	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	
protected:
};
SCView* NewSCDragSource(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


class SCDragSink : public SCStaticText
{
public:	
	SCDragSink(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCDragSink();
	
	virtual void draw(SCRect inDamage);
	virtual bool shouldDim();
	
	virtual bool canReceiveDrag();
	virtual void receiveDrag();
	
protected:
};
SCView* NewSCDragSink(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


class SCDragBoth : public SCDragSink
{
public:	
	SCDragBoth(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	
	virtual void draw(SCRect inDamage);

	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);

protected:
};
SCView* NewSCDragBoth(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


//felix
class SCTabletView : public SCView
{
public:	
	SCTabletView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCTabletView();
    
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);

	virtual void mouseBeginTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseTrack(SCPoint where, int modifiers,NSEvent *theEvent);
	virtual void mouseEndTrack(SCPoint where, int modifiers,NSEvent *theEvent);
protected:
    int mClipToBounds;
};
SCView* NewSCTabletView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

