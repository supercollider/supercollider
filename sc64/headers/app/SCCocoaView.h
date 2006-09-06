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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "SCView.h"
#import "SCCocoaView.h"
#import "SCTextView.h"
#import "SCVirtualMachine.h"
#include "QuickTime/Movies.h"

@interface SCCocoaTextViewResponder : NSResponder
{
    //struct PyrObject *mSCObject;
	struct SCCocoaTextView *mSCViewObject;
	BOOL usesTabToFocusNextView;
	BOOL enterExecutesSelection;
}

//- (void) keyDown;
//- (void) keyUp;
//- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (void)textDidEndEditing:(NSNotification *)aNotification;
- (void)textDidBeginEditing:(NSNotification *)aNotification;
- (void)setSCView: (struct SCCocoaTextView*)inObject;
- (IBAction) executeSelection: (id) sender;
- (void)sendSelection: (char*) methodName;
- (void) keyUp: (NSEvent*) event;
- (void) keyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) setUsesTabToFocusNextView: (BOOL) flag;
- (void) setEnterExecutesSelection: (BOOL) flag;


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
	SCTextView* getTextView(){return mTextView;};
	void tabPrevFocus();
	void tabNextFocus();

protected:
	SCTextView *mTextView;
	NSScrollView *mScrollView;
	SCCocoaTextViewResponder *mCocoaToLangAction;
};

class SCMovieView : public SCView
{
public:	
	SCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds); 
	virtual ~SCMovieView();
	virtual void setBounds(SCRect inBounds);
	virtual int setProperty(PyrSymbol *symbol, PyrSlot *slot);

protected:
	NSMovieView *mMovieView;
	/* QT */
	TimeBase mTimeBase;
	TimeRecord mTimeRecord;
	Movie mMovie;	
};

SCView* NewSCCocoaTextView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);
SCView* NewSCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds);

