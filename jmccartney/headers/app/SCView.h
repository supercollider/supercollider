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
	static SCView* MakeSCView(PyrObject* inObj, SCContainerView *inParent, SCRect inBounds);
	
	SCViewMaker *mNext;
	SCViewCtor mCtor;
	const char* mName;
};

extern SCViewMaker *gSCViewMakers;

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
	virtual void mouseBeginTrack(SCPoint where, int modifiers);
	virtual void mouseTrack(SCPoint where, int modifiers);
	virtual void mouseEndTrack(SCPoint where, int modifiers);
	virtual void mouseOver(SCPoint where);
	virtual void keyDown(int character, int modifiers);
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
        
protected:
	friend class SCContainerView;

	SCView *mNext;
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
        
protected:
        friend class SCView;
        void focusIs(SCView *inView) { mFocusView = inView; }

        DamageCallback mDamageCallback;
        DragCallback mDragCallback;
        void *mHostData;
        SCView *mFocusView;
        SCView *mDragView;
};

inline bool SCView::isFocus() const { return mTop->focusView() == this; }

class SCSlider : public SCView
{
public:	
	SCSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseTrack(SCPoint where, int modifiers);
	
	double value() { return mValue; }
	bool setValue(double inValue, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

        virtual bool canReceiveDrag();
        virtual void receiveDrag();

protected:
	void setValueFromPoint(SCPoint point);
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
	virtual void mouseBeginTrack(SCPoint where, int modifiers);
	virtual void mouseTrack(SCPoint where, int modifiers);
	
	bool setValue(double inLo, double inHi, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

        virtual bool canReceiveDrag();
        virtual void receiveDrag();

protected:
	void setValueFromPoint(SCPoint point);
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
	virtual void mouseTrack(SCPoint where, int modifiers);
	
	bool setValue(double inLo, double inHi, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

        virtual bool canReceiveDrag();
        virtual void receiveDrag();

protected:
	void setValueFromPoint(SCPoint point);
	void calcThumbRect();
	
	SCRect mThumbRect;
	double mX, mY;
        double mStepSize, mStepScale;
        DrawBackground* mKnob;
};
SCView* NewSC2DSlider(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


/*
class SCCartesianView : public SCView
{
public:	
	SCCartesianView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseTrack(SCPoint where, int modifiers);
	
	bool setValue(double inLo, double inHi, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

        SCPoint pixelToUnits(SCPoint p) 
            { 
                return SCMakePoint(
                    (p.x - mBounds.x) * mZoom.x + mScroll.x,
                    (p.y - mBounds.y) * mZoom.y + mScroll.y);
            }
        SCPoint unitsToPixel(SCPoint u) 
            {
                return SCMakePoint(
                    (u.x - mScroll.x) * mInvZoom.x + mBounds.x,
                    (u.y - mScroll.y) * mInvZoom.y + mBounds.y);
            }
            
protected:
	
        SCRect mLimits;
	SCPoint mZoom, mInvZoom, mScroll;
	SCColor mGridColor;
        bool mGridOn;
};
SCView* NewSCCartesianView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


*/

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
	virtual void mouseTrack(SCPoint where, int modifiers);
	virtual void mouseEndTrack(SCPoint where, int modifiers);
	
	bool setValue(int inValue, bool send);

	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);

protected:
	
	int mValue;
        char mFontName[kFontNameSize];
        float mFontSize;
        int mNumStates;
        SCButtonState *mStates;
        bool mPushed;
};
SCView* NewSCButton(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

class SCUserView : public SCView
{
public:	
	SCUserView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 

	virtual void draw(SCRect inDamage);
	virtual void mouseBeginTrack(SCPoint where, int modifiers);
	virtual void mouseTrack(SCPoint where, int modifiers);
	virtual void mouseEndTrack(SCPoint where, int modifiers);
	virtual void keyDown(int character, int modifiers);
        
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

	virtual void mouseTrack(SCPoint where, int modifiers);
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
	virtual void mouseBeginTrack(SCPoint where, int modifiers);
        
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

	virtual void mouseBeginTrack(SCPoint where, int modifiers);

protected:
};
SCView* NewSCDragBoth(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);


