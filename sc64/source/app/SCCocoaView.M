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
#include "SC_InlineBinaryOp.h"


#include "SCCocoaView.h"
#include "QuickTime/Movies.h"

extern pthread_mutex_t gLangMutex;
extern bool compiledOK;
void SyntaxColorize(NSTextView* textView);

@implementation SCCocoaTextViewResponder

- (struct PyrObject*)getSCObject
{
	return mSCViewObject->GetSCObj();
}
- (void)textDidEndEditing:(NSNotification *)aNotification
{
//	post("endEditing");
}
- (void)textDidBeginEditing:(NSNotification *)aNotification
{
}

- (void)setSCView: (struct SCCocoaTextView*)inObject
{
	mSCViewObject = inObject;
}
	

- (IBAction) executeSelection: (id) sender
{
	if(enterExecutesSelection)
    [self sendSelection: "interpretPrintCmdLine" ];
}

- (void)sendSelection: (char*) methodName
{        
    if (!compiledOK) {
        return;
    }

	NSRange selectedRange;
	SCTextView * txtView = mSCViewObject->getTextView();
	NSString* selection = [txtView currentlySelectedTextOrLine: &selectedRange];
    const char *text = [selection UTF8String];
	int textlength = strlen(text);

    [[SCVirtualMachine sharedInstance] setCmdLine: text length: textlength];
    
    NSRange newSelectedRange = NSMakeRange(selectedRange.location + selectedRange.length, 0);
    [txtView setSelectedRange: newSelectedRange];
    
    pthread_mutex_lock(&gLangMutex);
    runLibrary(getsym(methodName));
    pthread_mutex_unlock(&gLangMutex);
    
}

- (void) keyDown: (NSEvent*) event
{
	// for some reason modifiers becomes 256 on my machine with no keys pressed. So need to mask against known keys.
		if(usesTabToFocusNextView){
		    NSString *characters = [event characters];
			unsigned int modifiers = [event modifierFlags];	
			uint32 allKnownModifiers = NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSCommandKeyMask 
				| NSAlternateKeyMask | NSHelpKeyMask | NSFunctionKeyMask;
			unichar character = 0;
			if([characters length] > 0) {
				character = [characters characterAtIndex: 0];
			}				
			if(character == 9 && ((modifiers & allKnownModifiers) == 0)) {
			   mSCViewObject->tabPrevFocus(); //this does not work as expected!
				return;
			} else if (character == 25 && ((modifiers & allKnownModifiers) == NSShiftKeyMask)) {
				mSCViewObject->tabNextFocus();
				return;
			} // other tab keys avail for user 
		}

}
- (void) keyUp: (NSEvent*) event
{

}
- (void) mouseDown: (NSEvent*) event
{
	mSCViewObject->makeFocus(true);
}
- (void) setUsesTabToFocusNextView: (BOOL) flag
{
	usesTabToFocusNextView = flag;
}

- (void) setEnterExecutesSelection: (BOOL) flag
{
	enterExecutesSelection = flag;
}

@end

NSRect SCtoNSRect(SCRect screct);

SCView* NewSCCocoaTextView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCCocoaTextView(inParent, inObj, inBounds);
}

SCCocoaTextView::SCCocoaTextView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds)
{
	//mEnabled = mVisible = mCanFocus = true;
	NSRect matrect = SCtoNSRect(mBounds);
	mTextView = [[SCTextView alloc] initWithFrame:matrect];
	NSView *view = mTop->GetNSView();		
	mCocoaToLangAction = [SCCocoaTextViewResponder alloc];
	[mCocoaToLangAction setSCView: this];
    [mTextView setAutoresizingMask: 63];
    [[mTextView textContainer] setWidthTracksTextView: YES];
    [mTextView setAllowsUndo: YES];
    [mTextView setRichText: YES];
    [mTextView setSmartInsertDeleteEnabled: NO];
    [mTextView setImportsGraphics: YES];
    [mTextView setFont: [NSFont fontWithName: @"Monaco" size: 9]];
	[mTextView setSelectedRange: NSMakeRange(0,0)];
	[mTextView setLangClassToCall:@"SCView" 
			withKeyDownActionIndex:1 withKeyUpActionIndex:2];
	[mTextView  setObjectKeyDownActionIndex:4 setObjectKeyUpActionIndex:5];	
	mScrollView = [[NSScrollView alloc] initWithFrame: matrect];
	[mScrollView setDocumentView: mTextView];
	[mTextView setDelegate: mCocoaToLangAction];
	[view addSubview: mScrollView];
	//This is a hack, otherwise the mTextView always has focus even if makeFirstResponder: view is called...
	[mTextView setAcceptsFirstResponder:NO];
	[mScrollView setDrawsBackground:YES];
	[mCocoaToLangAction setUsesTabToFocusNextView:YES];
	[mCocoaToLangAction setEnterExecutesSelection:YES];	
//	[mTextView autorelease];
//	[mScrollView autorelease];
}

SCCocoaTextView::~SCCocoaTextView()
{
	[mScrollView removeFromSuperview];
	[mCocoaToLangAction release];
	[mTextView release];
	[mScrollView release];
}

void SCCocoaTextView::tabPrevFocus()
{
	mTop->tabPrevFocus();
}
void SCCocoaTextView::tabNextFocus()
{
	//post("next focus\n");
	mTop->tabNextFocus();
}
void SCCocoaTextView::makeFocus(bool focus)
{
    if (focus) {
        if (canFocus() && !isFocus()) {
			[mTextView setAcceptsFirstResponder:YES];
			[[mTextView window] makeFirstResponder: mTextView];	
        }
    } else {
        if (isFocus()) {
			if([[mTextView window] firstResponder] == mTextView)
				[[mTextView window] makeFirstResponder: mTop->GetNSView()];	
			[mTextView setAcceptsFirstResponder:NO];
        }
    }
	SCView::makeFocus(focus);
}


int slotGetSCRect(PyrSlot* a, SCRect *r);
extern PyrSymbol *s_font;
int slotBackgroundVal(PyrSlot *slot, DrawBackground **ioPtr);

void SCCocoaTextView::setBounds(SCRect inBounds)
{
		mBounds = inBounds;
		[[mScrollView superview] setNeedsDisplayInRect:[mScrollView frame]];

		[mScrollView setFrame: SCtoNSRect(mBounds)];
		[mTextView setFrame: SCtoNSRect(mBounds)];

	//	[mScrollView setBounds: SCtoNSRect(mBounds)];
	//	[mTextView setBounds: SCtoNSRect(mBounds)];
		[mScrollView setNeedsDisplay: YES];
		[mTextView setNeedsDisplay: YES];	
}

int slotColorVal(PyrSlot *slot, SCColor *sccolor);

int SCCocoaTextView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{	
	int err;
	char *name = symbol->name;

	if (strcmp(name, "visible")==0) {
		//only 10.3
		if(IsTrue(slot)) 
		{
			[mScrollView setHidden:NO];
			[mTextView setHidden:NO];

		}
		else
		{
			[mScrollView setHidden:YES];
			[mTextView setHidden:YES];
		}
	
		return errNone;
	}
	
	if (strcmp(name, "usesTabToFocusNextView")==0) {
		if(IsTrue(slot))[mCocoaToLangAction setUsesTabToFocusNextView:YES];
		else [mCocoaToLangAction setUsesTabToFocusNextView:NO];
		return errNone;		
	}
	if (strcmp(name, "enterExecutesSelection")==0) {
		if(IsTrue(slot))[mCocoaToLangAction setEnterExecutesSelection:YES];
		else [mCocoaToLangAction setEnterExecutesSelection:NO];
		return errNone;		
	}	
	if (strcmp(name, "setScrollersSize")==0) {
		if(IsTrue(slot)) [[mScrollView verticalScroller] setControlSize: NSMiniControlSize];
		else  [mScrollView setAutohidesScrollers:NO];
		[mScrollView setNeedsDisplay: YES];
		return errNone;

	}
	
	if (strcmp(name, "setAutohidesScrollers")==0) {
		if(IsTrue(slot)) [mScrollView setAutohidesScrollers:YES];
		else  [mScrollView setAutohidesScrollers:NO];
		[mScrollView setNeedsDisplay: YES];		
		return errNone;

	}
	
	if (strcmp(name, "setHasHorizontalScroller")==0) {
		if(IsTrue(slot)) [mScrollView setHasHorizontalScroller:YES];
		else  [mScrollView setHasHorizontalScroller:NO];
		[mScrollView setNeedsDisplay: YES];
		return errNone;
		
	}
	
	if (strcmp(name, "setHasVerticalScroller")==0) {
		if(IsTrue(slot)) [mScrollView setHasVerticalScroller:YES];
		else  [mScrollView setHasVerticalScroller:NO];
		[mScrollView setNeedsDisplay: YES];
		return errNone;		
	}	
	if (strcmp(name, "setEditable")==0) {
		if(IsTrue(slot)) [mTextView setEditable:YES];
		else  [mTextView setEditable:NO];
		return errNone;		
	}		
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		//add avariable to choos if this should resize the textview too
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		[[mScrollView superview] setNeedsDisplayInRect:[mScrollView frame]];
		mBounds = screct;

		[mScrollView setFrame: SCtoNSRect(mBounds)];
		[mTextView setFrame: SCtoNSRect(mBounds)];

	//	[mScrollView setBounds: SCtoNSRect(mBounds)];
	//	[mTextView setBounds: SCtoNSRect(mBounds)];
		[mScrollView setNeedsDisplay: YES];
		[mTextView setNeedsDisplay: YES];
	
		return errNone;
	}
	
	if (strcmp(name, "textBounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		[[mScrollView superview] setNeedsDisplayInRect:[mScrollView frame]];
		//mBounds = screct;

		[mTextView setFrame: SCtoNSRect(screct)];

	//	[mScrollView setBounds: SCtoNSRect(mBounds)];
	//	[mTextView setBounds: SCtoNSRect(mBounds)];
		[mScrollView setNeedsDisplay: YES];
		[mTextView setNeedsDisplay: YES];
	
		return errNone;
	}
	if (strcmp(name, "selectedString")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slot->uos;
		if(!pstring) return errNone;
		NSRange selectedRange = [mTextView selectedRange];
		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		if ([mTextView shouldChangeTextInRange: selectedRange replacementString: string]) {
			[mTextView replaceCharactersInRange: selectedRange withString: string];
			[mTextView didChangeText];
		}
		[string release];
		return errNone;
	}
	
	if (strcmp(name, "background")==0) {
		err = slotBackgroundVal(slot, &mBackground);
		if (err) return err;
		SCColor rgb;
		err = slotColorVal(slot, &rgb);
		if (err) return err;
		NSColor *color = [NSColor colorWithCalibratedRed: rgb.red
                            green: rgb.green 
                            blue: rgb.blue 
                            alpha: rgb.alpha];
		[mTextView setBackgroundColor: color];
		[mScrollView setBackgroundColor: color];
		return errNone;
	}	

	if (strcmp(name, "setTextColor")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slot->uo->slots;	
		SCColor rgb;
		int rangeStart, rangeSize;
		err = slotColorVal(slots+0, &rgb);
		if (err) return err;
		err = slotIntVal(slots+1, &rangeStart);
		if (err) return err;		
		err = slotIntVal(slots+2, &rangeSize);
		if (err) return err;


		NSColor *color = [NSColor colorWithCalibratedRed: rgb.red
                            green: rgb.green 
                            blue: rgb.blue 
                            alpha: rgb.alpha];
		
		//[[doc textView] setBackgroundColor: color];
		
		if(rangeStart < 0){
			[mTextView setTextColor: color];
			[mTextView didChangeText];
			return errNone;
		}
		int length = [[mTextView string] length];
		if(rangeStart >= length) rangeStart = length - 1 ;
		if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;
		NSRange selectedRange =	NSMakeRange(rangeStart, rangeSize);
		
		
		[mTextView setTextColor: color range: selectedRange];
		[mTextView didChangeText];
		return errNone;
	}
	
	if (strcmp(name, "setFont")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;	
		PyrSlot *slots =slot->uo->slots;
		PyrSlot *fontSlot = slots+0;
		if (IsNil(fontSlot)) return errNone; // use default font
		if (!(isKindOfSlot(fontSlot, s_font->u.classobj))) return errWrongType;
		PyrSlot *nameSlot = fontSlot->uo->slots+0;
		PyrSlot *sizeSlot = fontSlot->uo->slots+1;
		float size;
		int err = slotFloatVal(sizeSlot, &size);
		if (err) return err;

		PyrString *pstring = nameSlot->uos;
		NSString *fontName = [NSString stringWithCString: pstring->s length: pstring->size];
		if (!fontName) return errFailed;
		NSFont *font = [NSFont fontWithName: fontName size: size];
		if (!font) return errFailed;
		
		int rangeStart, rangeSize;
		err = slotIntVal(slots+1, &rangeStart); //if -1 do not use range
        if (err) return err;
		 err = slotIntVal(slots+2, &rangeSize);
        if (err) return err;

		if(rangeStart < 0){
			[mTextView setFont: font];
			return errNone;
		}
		NSString* string = [mTextView string];
		int length = [string length];
		if(length < 1) return errFailed;		
		if(rangeStart >= length) rangeStart = length - 1 ;
		if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;
		NSRange selectedRange =	NSMakeRange(rangeStart, rangeSize);
		
        [mTextView setFont: font range: selectedRange];
		return errNone;
	}
	
//	if (strcmp(name, "insertString")==0) {
//		if (!(isKindOfSlot(slot, class_string))) return errWrongType;
//        PyrString* string = slot->uos;
////        [doc insertText: string->s length: string->size];
//        return errNone;
//	}
	if (strcmp(name, "insertStringInRange")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;	
		PyrSlot *slots =slot->uo->slots;	
		PyrSlot *stringSlot = slots+0;	
		if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;

		int rangeStart, rangeSize;
		int err = slotIntVal(slots+1, &rangeStart); //if -1 do not use range
        if (err) return err;
		err = slotIntVal(slots+2, &rangeSize);
        if (err) return err;

		PyrString* pstring = stringSlot->uos;
		NSRange selectedRange;
		int length = [[mTextView string] length];
		
		if(rangeSize < 0) rangeSize = length - 1;
		if(rangeStart >= length) rangeStart = length - 1 ;
		if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;
		
		if(rangeStart<0) selectedRange =	NSMakeRange(0, length);
		else selectedRange =	NSMakeRange(rangeStart, rangeSize);
		
		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		if ([mTextView  shouldChangeTextInRange: selectedRange replacementString: string]) {
			[mTextView  replaceCharactersInRange: selectedRange withString: string];
			[mTextView  didChangeText];
		}
		[string release];
	
        return errNone;
	}
	
	return SCView::setProperty(symbol, slot);
}

int SCCocoaTextView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

	if (strcmp(name, "string")==0) {
		NSString* str = [mTextView string];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(NULL, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}

	if (strcmp(name, "selectedString")==0) {        
		NSString* str = [mTextView currentlySelectedTextOrLine:NULL];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(NULL, cstr, 0, true);
        SetObject(slot, string);
		return errNone;		
	}

	if (strcmp(name, "selectedRange")==0) {        
		NSRange range = [mTextView selectedRange];
		SetInt(slot, range.length);
		return errNone;
	}

	if (strcmp(name, "selectedRangeLocation")==0) {        	
		NSRange range = [mTextView selectedRange];
		SetInt(slot, range.location);
		return errNone;
	}
	
	return SCView::getProperty(symbol, slot);
}

////////////////////
SCView* NewSCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCMovieView(inParent, inObj, inBounds);
}

SCMovieView::SCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds)
{
		NSRect matrect = SCtoNSRect(mBounds);
		mMovieView = [[NSMovieView alloc] initWithFrame:matrect];
		NSView *view = mTop->GetNSView();	
		[view addSubview: mMovieView];
		
		//need to call EnterMovies for other qt c calls
		EnterMovies();
}

SCMovieView::~SCMovieView()
{
	[mMovieView removeFromSuperview];
	[mMovieView release];
}

void SCMovieView::setBounds(SCRect screct)
{
	[[mMovieView superview] setNeedsDisplayInRect:[mMovieView frame]];
	mBounds = screct;
	[mMovieView setFrame: SCtoNSRect(mBounds)];
//	[mMovieView setBounds: SCtoNSRect(mBounds)];
	[mMovieView setNeedsDisplay: YES];
}

int SCMovieView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{	
	int err;
	char *name = symbol->name;

	if (strcmp(name, "stop")==0) {
		[mMovieView stop: nil];
		return errNone;
	}

	if (strcmp(name, "start")==0) {
		[mMovieView start: nil];
		return errNone;	
	}
	if (strcmp(name, "stepForward")==0) {
		[mMovieView stepForward: nil];
		return errNone;		
	}
	if (strcmp(name, "stepBack")==0) {
		[mMovieView stepBack: nil];
		return errNone;		
	}		
	if (strcmp(name, "resizeWithMagnification")==0) {
		float mag;
		err = slotFloatVal(slot, &mag);
		if(err) return err;
		[mMovieView resizeWithMagnification: mag];
		NSSize size = [mMovieView sizeForMagnification: mag];
		mBounds.width = size.width;
		mBounds.height = size.height;
		return errNone;
	}	
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		mBounds = screct;
		[mMovieView setFrame: SCtoNSRect(mBounds)];
		[mMovieView setBounds: SCtoNSRect(mBounds)];
		[mMovieView setNeedsDisplay: YES];
		return errNone;
	}
	
	if (strcmp(name, "setMovie")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slot->uos;
		if(!pstring) return errNone;
		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];		
		NSURL * url = [[NSURL alloc] initFileURLWithPath: string];		
		NSMovie *movie = [[NSMovie alloc] initWithURL: url byReference: YES];
		if(!movie) return errFailed;
		//check for current movie:
		NSMovie *nsmovie = [mMovieView movie];
		[mMovieView setMovie: movie];
		if(nsmovie){
			[nsmovie release];
		}
		[string release];
		[url release];
		/* QT: */
		mMovie = (Movie) [[mMovieView movie] QTMovie];
		mTimeBase = GetMovieTimeBase (mMovie); 
		mTimeRecord.base = mTimeBase;
		GetMovieTime(mMovie, &mTimeRecord);
		
		return errNone;
	}

	if (strcmp(name, "setMuted")==0) {
		if(IsTrue(slot))[mMovieView setMuted: YES];
		else [mMovieView setMuted: NO];
		return errNone;		
	}
	
	if (strcmp(name, "setEditable")==0) {
		if(IsTrue(slot))[mMovieView setEditable: YES];
		else [mMovieView setEditable: NO];
		return errNone;		
	}

	if (strcmp(name, "setPlaysSelectionOnly")==0) {
		if(IsTrue(slot))[mMovieView setPlaysSelectionOnly: YES];
		else [mMovieView setPlaysSelectionOnly: NO];
		return errNone;		
	}

	if (strcmp(name, "setRate")==0) {
		float rate;
		err = slotFloatVal(slot, &rate);
		if(err) return err;
		[mMovieView setRate:rate];
		return errNone;		
	}
	if (strcmp(name, "setVolume")==0) {
		float vol;
		err = slotFloatVal(slot, &vol);
		if(err) return err;
		[mMovieView setVolume:vol];
		return errNone;		
	}
	if (strcmp(name, "setLoopMode")==0) {
		int mode;
		err = slotIntVal(slot, &mode);
		if(err) return err;
		switch(mode){
				case 0: [mMovieView setLoopMode:NSQTMovieLoopingBackAndForthPlayback]; break;
				case 1: [mMovieView setLoopMode:NSQTMovieLoopingPlayback]; break;
				case 2: [mMovieView setLoopMode:NSQTMovieNormalPlayback]; break;
		}
		return errNone;		
	}
	
	if (strcmp(name, "gotoEnd")==0) {
		[mMovieView gotoEnd: NULL];
		return errNone;		
	}

	if (strcmp(name, "gotoBeginning")==0) {
		[mMovieView gotoBeginning: NULL];
		return errNone;		
	}	
	
	if (strcmp(name, "showControllerAndAdjustSize")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slot->uo->slots;
		BOOL showC, adjust;
		if(IsTrue(slots+0)) showC=YES;
		else showC = NO;
		if(IsTrue(slots+1)) adjust=YES;
		else adjust = NO;
		[mMovieView showController: showC adjustingSize: adjust];
		return errNone;		
	}
	if (strcmp(name, "copy")==0) {
		[mMovieView copy: NULL];
		return errNone;		
	}
	if (strcmp(name, "clear")==0) {
		[mMovieView clear: NULL];
		return errNone;		
	}
	if (strcmp(name, "cut")==0) {
		[mMovieView cut: NULL];
		return errNone;		
	}
	if (strcmp(name, "paste")==0) {
		[mMovieView paste: NULL];
		return errNone;		
	}					
	
	if (strcmp(name, "setCurrentTime")==0) {
		int time;
		err = slotIntVal(slot, &time);
		if(err) return err;		
		SetMovieTimeValue(mMovie, (TimeValue) time *  mTimeRecord.scale);
		return errNone;		
	}
		
	return SCView::setProperty(symbol, slot);
}
//
//int SCCocoaTextView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
//{
//	char *name = symbol->name;
//GetMovieDuration([[mMovieView movie] QTMovie]);
//
//	return SCView::getProperty(symbol, slot);
//}