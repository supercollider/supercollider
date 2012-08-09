/*
    SCCocoaView.M


    Created by falkenst on Tue Feb 08 2005.
    Copyright (c) 2005 jan truetzschler. All rights reserved.

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

#include "SCView.h"
#import "SCCocoaView.h"
#import "SCTextView.h"
#import "SCVirtualMachine.h"
#include "QTKit/QTKit.h"
#include <Quartz/Quartz.h>
#include <WebKit/WebView.h>
#import <WebKit/WebEditingDelegate.h>

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1060
@interface SCCocoaTextViewResponder : NSResponder <NSTextViewDelegate>
#else 
@interface SCCocoaTextViewResponder : NSResponder
#endif

{
    //struct PyrObject *mSCObject;
	class SCCocoaTextView *mSCViewObject;
	BOOL usesTabToFocusNextView;
	BOOL enterExecutesSelection;
}

//- (void) keyDown;
//- (void) keyUp;
//- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (void)textDidEndEditing:(NSNotification *)aNotification;
- (void)textDidBeginEditing:(NSNotification *)aNotification;
- (void)setSCView: (class SCCocoaTextView*)inObject;
- (IBAction) executeSelection: (id) sender;
- (void)sendSelection: (NSString*) nsmethodName;
- (void) keyUp: (NSEvent*) event;
- (BOOL) handleKeyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) setUsesTabToFocusNextView: (BOOL) flag;
- (void) setEnterExecutesSelection: (BOOL) flag;
- (BOOL) textView: (NSTextView *) textView
    clickedOnLink: (id) link
		  atIndex: (unsigned) charIndex;
- (void) loadHTMLToTextView:(NSURL *)url;
- (void)setActiveTextView:(SCTextView*)aTextView;
@end

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 1060
@interface SCTextFieldResponder : NSTextField <NSTextFieldDelegate>
#else 
@interface SCTextFieldResponder : NSTextField
#endif
{
	class SCTextField *mSCViewObject;
	BOOL mDragStarted;
	BOOL mAcceptsFirstResponder;
	BOOL mEditingInactive; // we use this to determine if editing is not active when keyDown is received
	NSNumberFormatter *mFormatter; // for numbox
}

- (struct PyrObject*)getSCObject;
- (void)setSCView: (class SCTextField*)inObject;
- (void) setAcceptsFirstResponder: (BOOL) flag;
- (void) addNumberFormatter;
- (void) setEditingInactive: (BOOL)flag;
- (BOOL) editingInactive;
@end

@interface SCNSMenuItem : NSMenuItem {
	struct PyrObject *mMenuItemObj;
}

- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (void)doAction: (id)sender;

@end

@interface SCNSLevelIndicator : NSLevelIndicator {
	BOOL drawPeak, isVertical, criticalAboveWarning;
	float peakSubtract, peakLevel, peakY, peakHeight;
	double value, peakValue;
	double warning, critical;
}

- (void)setDrawPeak:(BOOL)flag;
- (void)setIsVertical:(BOOL)flag;
- (void)setPeakSubtract:(float)val;
- (void)setPeakLevel:(float)val;
- (void)prepPeakBounds;
- (void)setUpWarning:(double)val;
- (void)setUpCritical:(double)val;

@end

@interface SCNSFlippedView : NSView {

}

@end

@interface SCNSWebView : WebView {
	class SCWebView *mSCWebView;
	int loadCount;
	bool handleLinks;
	BOOL enterExecutesSelection;
}

- (void)initVars;
- (void) setHandleLinks: (bool)handle;
- (void)resetLoadCount;
- (void)setSCObject: (class SCWebView*)inObject;
- (class SCWebView*)getSCObject;
- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame;
- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame;
- (void)doFailAction;
- (void)doLoadAction;
- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error;
- (BOOL)webView:(WebView *)webView shouldInsertText:(NSString *)text replacingDOMRange:(DOMRange *)range givenAction:(WebViewInsertAction)action;
- (void)webView:(WebView *)webView decidePolicyForNavigationAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id )listener;
- (void)webView:(WebView *)webView unableToImplementPolicyWithError:(NSError *)error frame:(WebFrame *)frame;
- (void)doLinkAction:(NSString *)urlString;
- (void) setEnterExecutesSelection: (BOOL) flag;
- (void)sendSelection: (NSString*) nsmethodName;
- (void)setSelection;
- (IBAction)openCode:(id)sender;
- (IBAction) showHelpFor: (id) sender;
- (IBAction)showHelpSearch:(id)sender;

- (IBAction)methodTemplates: (id)sender;
- (IBAction)methodReferences: (id)sender;
- (IBAction)executeSelection: (id) sender;
- (void)cmdF:(id)sender;

@end

class SCCocoaTextView : public SCView
{
public:
	SCCocoaTextView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCCocoaTextView();
	virtual void makeFocus(bool focus);
	virtual void setBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void keyDown(int character, int modifiers, unsigned short keycode);
	virtual void keyUp(int character, int modifiers, unsigned short keycode);

	SCTextView* getTextView(){return mTextView;};
	void tabPrevFocus();
	void tabNextFocus();
	virtual void setVisibleFromParent();
	void setLoadLinkInView(bool flag){mLoadLinkInView = flag;};
	bool getLoadLinkInView(){return mLoadLinkInView;};

	int open(NSString *path);
	NSURL* getLastURL() {return mLastURL;};
	bool linkAction(NSString *path);

	virtual NSView* focusResponder() { return mTextView; }

protected:
	SCTextView *mTextView;
	NSScrollView *mScrollView;
	SCCocoaTextViewResponder *mCocoaToLangAction;
	bool mLoadLinkInView;
	NSURL *mLastURL;
};

class SCMovieView : public SCView
{
public:
	SCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCMovieView();
	virtual void setBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void setVisibleFromParent();
	virtual NSView* focusResponder() { return mMovieView; }

protected:
	QTMovieView *mMovieView;
	QTTime mTime;
	QTMovie *mMovie;
};

class SCWebView : public SCView
{
public:
	SCWebView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCWebView();
	virtual void setBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void setVisibleFromParent();
	//virtual void keyDown(int character, int modifiers, unsigned short keycode);
	void tabPrevFocus();
	void tabNextFocus();
	virtual void doOnLoadAction();
	virtual void doLoadFailedAction();
	virtual void doLinkClickedAction(PyrString* pstring);
	virtual NSView* focusResponder() { return mWebView; }
	SCTopView* getTop() { return mTop; }
	virtual void mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent);
	
protected:
	SCNSWebView *mWebView;
	SCNSFlippedView *flipView;
};

//class SCTextField : public SCStaticText
class SCTextField : public SCView
{
public:
	SCTextField(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCTextField();
	virtual void setBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void setVisibleFromParent();
	virtual void makeFocus(bool focus);
	virtual void keyDown(int character, int modifiers, unsigned short keycode);
	void tabPrevFocus();
	void tabNextFocus();
	virtual NSView* focusResponder() { return mTextField; }
	BOOL performDrag();
	NSDragOperation draggingEntered();
	virtual bool canReceiveDrag();
	SCTopView* getTop() { return mTop; }
	virtual void mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent);
	virtual void beginDrag(SCPoint where);

protected:
	SCTextFieldResponder *mTextField;
    SCColor mBoxColor;
	//SCTextFieldResponder *mCocoaToLangAction;

	SCColor mStringColor;
};

class SCNumberBox : public SCTextField
{
public:
	SCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCNumberBox();
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual void keyDown(int character, int modifiers, unsigned short keycode);
};

//////////////////////////////////////
/*
 SCQuartzComposerView by Scott Wilson
 Copyright (c) 2007 Scott Wilson. All rights reserved.
 Development funded in part by the Arts and Humanites Research Council http://www.ahrc.ac.uk/
 */

class SCQuartzComposerView : public SCView
{
public:
	SCQuartzComposerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
	virtual ~SCQuartzComposerView();
	virtual void setBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
	virtual id getNSObjectForSCObject(PyrSlot *scobject, int *returnErr);
	virtual int getSCObjectForNSObject(PyrSlot *slot, id nsObject, NSString *type);
	virtual void setVisibleFromParent();
	virtual NSView* focusResponder() { return mQCView; }

protected:
	QCView *mQCView;
};

class SCLevelIndicator : public SCView
	{
	public:
		SCLevelIndicator(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
		virtual ~SCLevelIndicator();
		virtual void setBounds(SCRect inBounds);
		void setImage();
		void resetParams();
		virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);
		virtual int getProperty(PyrSymbol *symbol, PyrSlot *slot);
		virtual void setVisibleFromParent();
		void tabPrevFocus();
		void tabNextFocus();
		virtual NSView* focusResponder() { return mLevelIndicator; }

	protected:
		SCNSLevelIndicator *mLevelIndicator;
		NSImage *mImage;
		int mStyle;
		double mNumSteps, mWarning, mCritical;
		float mTickHeight;
		bool mIsVertical;
	};

SCView* NewSCCocoaTextView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
SCView* NewSCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
SCView* NewSCQuartzComposerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
SCView* NewSCTextField(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
SCView* NewSCLevelIndicator(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

