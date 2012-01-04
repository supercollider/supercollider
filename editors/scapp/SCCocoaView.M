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


#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include <pthread.h>
#import "SCBase.h"
#import "PyrSymbol.h"
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#include "SC_BoundsMacros.h"
#include "SC_InlineBinaryOp.h"
#include "PyrListPrim.h"


#include "SCCocoaView.h"
#include "QTKit/QTKit.h"
#import "HTMLRenderer.h"
#import "MyDocument.h"
#import "SCImage.h"


extern pthread_mutex_t gLangMutex;
extern bool compiledOK;

// SCImage support
extern PyrSymbol		*s_scimage; // class symbol
extern PyrObject*		newPyrSCImage(VMGlobals* g); // class creation func

void SyntaxColorize(NSTextView* textView);

static NSString *sSCObjType = @"SuperCollider object address";
extern PyrSymbol *s_scview;

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

//- (void) keyDown: (NSEvent*) event
//- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRanges:(NSArray *)affectedRanges replacementStrings:(NSArray *)characters
- (BOOL) handleKeyDown: (NSEvent*) event;
{

	// for some reason modifiers becomes 256 on my machine with no keys pressed. So need to mask against known keys.
	if(usesTabToFocusNextView && ([event type] == NSKeyDown)){
		NSString *characters = [event characters];
		unsigned int modifiers = [event modifierFlags];
		uint32 allKnownModifiers = NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSCommandKeyMask
			| NSAlternateKeyMask | NSHelpKeyMask | NSFunctionKeyMask;
		unichar character = 0;
		if([characters length] > 0) {
			character = [characters characterAtIndex: 0];
		}
		if(character == 9 && ((modifiers & allKnownModifiers) == 0)) {
			mSCViewObject->tabPrevFocus();
			return YES;
		} else if (character == 25 && ((modifiers & allKnownModifiers) == NSShiftKeyMask)) { // check above for tab
			//NSLog(@"backtab");
			/////[mSCViewObject->getTextView() resignFirstResponder];
			mSCViewObject->tabNextFocus();
			return YES;
		} // other tab keys avail for user
	}
	return NO;

}
//- (BOOL) handleKeyDown: (NSEvent*) event;
//{
//	//NSLog(@"keyDown");
//	// for some reason modifiers becomes 256 on my machine with no keys pressed. So need to mask against known keys.
//	if(usesTabToFocusNextView){
//		NSString *characters = [event characters];
//		unsigned int modifiers = [event modifierFlags];
//		uint32 allKnownModifiers = NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSCommandKeyMask
//		| NSAlternateKeyMask | NSHelpKeyMask | NSFunctionKeyMask;
//		unichar character = 0;
//		if([characters length] > 0) {
//			character = [characters characterAtIndex: 0];
//			//NSLog(@"char %i", character);
//		}
//		if (character == 25 && ((modifiers & allKnownModifiers) == NSShiftKeyMask)) { // check above for tab
//			//NSLog(@"backtab");
//			/////[mSCViewObject->getTextView() resignFirstResponder];
//			mSCViewObject->tabNextFocus();
//			return YES;
//		} // other tab keys avail for user
//	}
//	return NO;
//}
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

- (BOOL) textView: (NSTextView *) textView
    clickedOnLink: (id) link
		  atIndex: (unsigned) charIndex
{
	bool loadLinkInView = mSCViewObject->getLoadLinkInView();
	NSDocumentController* docctl = [NSDocumentController sharedDocumentController];
	if (!docctl && !loadLinkInView) return YES;

	NSURL *desiredURL;

	// is it a NSURL link or a NSString link?
	if ([link isKindOfClass: [NSString class]])
    {
		if([link hasPrefix:@"SC://"] || [link hasPrefix:@"sc://"]) { // this means search immediately
			NSString *helpPath = pathOfHelpFileFor([[[link substringFromIndex:5] stringByDeletingPathExtension] stringByDeletingPathExtension]);
			if(!helpPath) {
				post("WARNING:\nInvalid hyperlink: '%s' Please repair this.\n", [link cString]);
				return YES;
			}
			desiredURL = [NSURL URLWithString: helpPath];
		} else desiredURL = [NSURL URLWithString: [link stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding] relativeToURL: mSCViewObject->getLastURL()];

    } else if ([link isKindOfClass: [NSURL class]])
    {
		// check for schemes which we'll handle
		if([[link scheme] isEqualToString: @"SC"] || [[link scheme] isEqualToString:@"sc"]) { // this means search immediately
			NSString *helpPath = pathOfHelpFileFor([[[[link relativeString] substringFromIndex:5] stringByDeletingPathExtension] stringByDeletingPathExtension]);
			if(!helpPath) {
				post("WARNING:\nInvalid hyperlink: '%s' Please repair this.\n", [[link relativeString] cString]);
				return YES;
			}
			desiredURL = [NSURL fileURLWithPath: helpPath];
		} else if (![link scheme]) { // NULL could be a regular file link that's been edited
			if([[link relativePath] hasPrefix: @"/"]) {
				desiredURL = [NSURL fileURLWithPath: [link relativeString]];
			} else {
				desiredURL = [NSURL URLWithString: [[link relativeString] stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding] relativeToURL: mSCViewObject->getLastURL()];
			}
		} else if(![[link scheme] isEqualToString: @"file"]) {
			return NO; // it's http, etc. so pass it on to Safari or whatever
		} else {
			desiredURL = link; // it's a regular file:// URL
		}
    } else return NO;

	// try the link action first then use default
	if(mSCViewObject->linkAction([desiredURL absoluteString])) return YES;

	MyDocument *doc = nil;
	int result = 0;

	if([[NSFileManager defaultManager] fileExistsAtPath: [desiredURL path]]) {
		if(!loadLinkInView) doc = (MyDocument*)[docctl documentForURL: [desiredURL absoluteURL]];
	} else NSLog(@"file doesn't exist at path");
    if (!doc) {
		if(!loadLinkInView) {
			doc = [docctl openDocumentWithContentsOfURL: desiredURL display: true];
		} else {
			result = mSCViewObject->open([desiredURL path]);
		}
        if (!doc && result) {
			// it's a bad file:// URL, post a warning and search
			post("WARNING:\nInvalid hyperlink: '%s' Please repair this.\nSearching help directories for alternative.\n", [[desiredURL path] cString]);
			// delete extension twice in case something.help.rtf
			NSString *desiredHelpName = [[[[desiredURL path] lastPathComponent] stringByDeletingPathExtension] stringByDeletingPathExtension];
			NSString *helpPath = pathOfHelpFileFor(desiredHelpName);
			if(!helpPath) {
				post("Can't find Help File Document for: '%s'\n", [desiredHelpName cString]);
				return YES;
			}
			desiredURL = [NSURL fileURLWithPath: helpPath];
			if([[NSFileManager defaultManager] fileExistsAtPath: [desiredURL path]]) {
				if(!loadLinkInView) {
					doc = (MyDocument*)[docctl documentForURL: [desiredURL absoluteURL]];
				}
			} else post("file doesn't exist at path: '%s'\n", [[desiredURL path] cString]);
			if (!doc) {
				if(!loadLinkInView) {
					doc = [docctl openDocumentWithContentsOfURL: desiredURL display: true];
				} else {
					result = mSCViewObject->open(helpPath);
				}
				if(!doc && result) {
					post("Can't open Help File Document: '%s'\n", [[desiredURL path] cString]);
					return YES;
				}
			}
        }
    }
	if(!loadLinkInView) {
		NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
		if (!window) {
			post("!! window controller returns nil ? failed to open help file window\n");
			return YES;
		}
		[window makeKeyAndOrderFront: nil];
	}
    return YES;
}

- (void) loadHTMLToTextView:(NSURL *)url
{
	if (!url) return;
	SCTextView *textView = mSCViewObject->getTextView();
	NSTextStorage* text = [textView textStorage];
	[text beginEditing];	// Bracket with begin/end editing for efficiency

	NSAttributedString *htmlAttributedString = [HTMLRenderer attributedStringWithURL:url];

	if(htmlAttributedString)
	{
		[text setAttributedString:htmlAttributedString];
		[textView setDefaultTabsTo: 28.0f];
	}

	[text endEditing];
	[url release];
}

// for compatibility with Document
- (void)setActiveTextView:(SCTextView*)aTextView
{
}
@end


@implementation SCTextFieldResponder
//
//- (id)initWithFrame:(NSRect)frameRect
//{
//	backGroundColor = [[NSColor whiteColor] retain];
//	return [super initWithFrame:frameRect];
//}
//
//- (void)dealloc
//{
//	[backGroundColor release];
//	[super dealloc];
//}
//
//- (void)setBackgroundColor:(NSColor *)aColor
//{
//	[aColor retain];
//	[backGroundColor release];
//	backGroundColor = aColor;
//}

//- (void)drawRect:(NSRect)aRect
//{
//	// draw a background
//	CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
//	CGContextSaveGState(cgc);
//    CGContextSetRGBFillColor(cgc, 1.0, 0.0, 0.0, 0.5);
//    CGContextFillRect(cgc, (*(CGRect *)&(aRect)));
//    CGContextRestoreGState(cgc);
//
//	[super drawRect:aRect]; // call superclass here for everything else
//}

- (struct PyrObject*)getSCObject
{
	return mSCViewObject->GetSCObj();
}

//- (void)controlTextDidEndEditing:(NSNotification *)aNotification
//{
//	if(textReallyChanged){
//		pthread_mutex_lock (&gLangMutex);
//		PyrSymbol *method = getsym("doAction");
//		PyrObject *mObj;
//		if (mObj = mSCViewObject->GetSCObj()) {
//			VMGlobals *g = gMainVMGlobals;
//			g->canCallOS = true;
//			++g->sp;  SetObject(g->sp, mObj);
//			runInterpreter(g, method, 1);
//			g->canCallOS = false;
//		}
//		pthread_mutex_unlock (&gLangMutex);
//		textReallyChanged = false;
//	}
//}


//- (BOOL)textShouldBeginEditing:(NSText *)fieldEditor
//{
//	post("foo\n");
//	if(mSCViewObject->isFocus()) return YES;
//
//	return NO;
//}

//- (BOOL)control:(NSControl *)control textShouldBeginEditing:(NSText *)fieldEditor
//{
//	//[(SCFieldEditor*)fieldEditor setCurrentTextField:self];
//	post("ctsbe\n");
//	return YES;
//}
//
//- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor
//{
//	//[(SCFieldEditor*)fieldEditor setCurrentTextField:nil];
//	return YES;
//}


//- (void)mouseDown:(NSEvent *)theEvent
//{
//
//	mSCViewObject->makeFocus(true);
//	unsigned int modifiers = [theEvent modifierFlags];
//    //control tab/escape doesn't get passed here at all ?
//	if(modifiers & NSCommandKeyMask) {
//		[self selectText:self]; // exit editing and select all for a drag
//		[self display];
//	} else [super mouseDown:theEvent];
//}

//- (void)drawRect:(NSRect)aRect
//{
//	[backGroundColor setFill];
//	NSRectFill(aRect);
//	[super drawRect:aRect];
//}

-(void)rightMouseDown:(NSEvent*)theEvent { [self mouseDown:theEvent]; }
-(void)otherMouseDown:(NSEvent*)theEvent { [self mouseDown:theEvent]; }
- (void)mouseDown:(NSEvent *)theEvent
{
	//NSLog(@"SCGraphView MOUSEDOWN");
	//[[self window] makeFirstResponder:self]; // there may be an active field editor
    BOOL keepOn = YES;
    //BOOL isInside = YES;
    NSPoint mouseLoc;
	//NSLog(@"Click count: %i", [theEvent clickCount]);
	//if([theEvent clickCount] == 2) return;
	//if (!mTopView) return;
    unsigned int modifiers = [theEvent modifierFlags];
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
	SCTopView *mTopView = mSCViewObject->getTop();
    if (mSCViewObject) {
		mDragStarted = NO;
		bool mouseMoved = NO;
		//mMenuView = 0;
        mSCViewObject->makeFocus(true);
		// force focus ring into drawing even if we're already in focus
		SEL sel = @selector(setNeedsDisplay:);
		NSMethodSignature *sig = [NSView instanceMethodSignatureForSelector: sel];
		NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
		SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
		[anInvocation setTarget: [self superview]];
		[anInvocation setSelector: sel];
		BOOL flag = YES;
		[anInvocation setArgument: &flag atIndex: 2];
		[scvm defer: anInvocation];

		[self setEditingInactive:NO]; // in this way we know that editing was started by a mouse click rather than a key down

		bool constructionmode = mTopView->ConstructionMode();
		if(!constructionmode)
		{
			mSCViewObject->mouseDownAction(scpoint, modifiers,theEvent);
			mSCViewObject->mouseBeginTrack(scpoint, modifiers,theEvent);
		}else
			mSCViewObject->setConstructionModeFromPoint(scpoint);

        [self displayIfNeeded];

        while (keepOn && !mDragStarted) {
            theEvent = [[self window] nextEventMatchingMask: NSLeftMouseUpMask |NSRightMouseUp | NSOtherMouseUp |
						NSLeftMouseDraggedMask | NSRightMouseDragged | NSOtherMouseDragged
						| NSKeyDownMask | NSKeyUpMask
						];
			modifiers = [theEvent modifierFlags]; // added
            mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
            //isInside = [self mouse:mouseLoc inRect:[self bounds]];
            scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
            int evtype = [theEvent type];
            switch ([theEvent type]) {
				case NSLeftMouseDown:
				case NSRightMouseDown:
					if(constructionmode)
					{
						mSCViewObject->doConstructionMove(scpoint);
						mTopView->refresh();
					}else
						mSCViewObject->mouseDownAction(scpoint, modifiers,theEvent);
					//						post("down \n");
					break;
                case NSLeftMouseDragged:
                case NSRightMouseDragged:
                case NSOtherMouseDragged:
					if(constructionmode)
					{
						mSCViewObject->doConstructionMove(scpoint);
						mTopView->refresh();
					}else
						mSCViewObject->mouseTrack(scpoint, modifiers,theEvent);
					mSCViewObject->mouseMoveAction(scpoint, modifiers,theEvent);
					mouseMoved = YES;
					//							post("drag \n");
					break;
                case NSLeftMouseUp:
                case NSRightMouseUp:
                case NSOtherMouseUp:
					if(constructionmode)
					{
						//				view->doConstructionMove(scpoint);
						mTopView->refresh();
					}else
					{
						if(mouseMoved) { // check if this was just a mouse down or also a drag; if the latter end editing
							[[self window] endEditingFor:self];
							[self setEditingInactive:YES];
							//[[self window] makeFirstResponder:[self superview]];
							// trick focus ring into drawing
							SEL sel = @selector(setNeedsDisplay:);
							NSMethodSignature *sig = [NSView instanceMethodSignatureForSelector: sel];
							NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
							SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
							[anInvocation setTarget: [self superview]];
							[anInvocation setSelector: sel];
							BOOL flag = YES;
							[anInvocation setArgument: &flag atIndex: 2];
							[scvm defer: anInvocation];
						}
						//							if(!view.GetSCObj()) break;
						mSCViewObject->mouseUpAction(scpoint, modifiers,theEvent);
						mSCViewObject->mouseEndTrack(scpoint, modifiers,theEvent);
					}
					keepOn = NO;
					break;
				case NSKeyDown:
					if(!constructionmode)
					{
						[self keyDown:theEvent];
					}
					break;
				case NSKeyUp:
					if(!constructionmode)
					{
						[self keyUp:theEvent];
					}
					break;
                default:
                    post("evtype %d %4.4s\n", evtype, (char*)&evtype);
					/* Ignore any other kind of event. */
					break;
            }
			//			display:
            [self displayIfNeeded];
            flushPostBuf();
        }
    }
	//mMenuView = 0;
    return;
}

-(void)mouseMoved:(NSEvent*)theEvent
{
	NSPoint mouseLoc;
    unsigned int modifiers = [theEvent modifierFlags];
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    if (mSCViewObject) {
		mDragStarted = NO;
		//        view->makeFocus(true);
        mSCViewObject->mouseOver(scpoint, modifiers, theEvent);
	}
}

const int circDiam = 20;

- (NSImage*) makeDragImage: (PyrSlot*)slot label: (NSString*)label
{

    if (!slot) return 0;

    NSString *nsstring;
    if(label) {
		nsstring = label;
	} else if (slot) {
        PyrClass *classobj = classOfSlot(slot);
        nsstring = [NSString stringWithCString: slotRawSymbol(&classobj->name)->name];
        if (!nsstring) return 0;
    } else {
        nsstring = @"No Data!";
    }

    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    NSFont *font = [NSFont fontWithName: @"Helvetica" size: 12];
    if (!font) return 0;
    [dict setObject: font forKey: NSFontAttributeName ];

    NSSize strSize = [nsstring sizeWithAttributes: dict];
    NSRect strRect = NSMakeRect(circDiam, 0, circDiam + strSize.width, strSize.height);

    NSSize size = NSMakeSize(circDiam+strSize.width, sc_max(circDiam, strSize.height));

    NSImage *image = [[NSImage alloc] initWithSize: size];
    if (!image) return 0;

    [image autorelease];

    float alpha = 0.6;
    NSColor *colorClear = [NSColor colorWithCalibratedRed: 0
													green: 0
													 blue: 0
													alpha: 0];
    NSColor *colorTransBlack = [NSColor colorWithCalibratedRed: 0
														 green: 0
														  blue: 0
														 alpha: alpha];
    NSColor *colorTransBlue = [NSColor colorWithCalibratedRed: 0
														green: 0
														 blue: 1
														alpha: alpha];
    /*NSColor *colorTransLtBlue = [NSColor colorWithCalibratedRed: 0.8
	 green: 0.8
	 blue: 1
	 alpha: alpha];*/
    NSColor *colorTransWhite = [NSColor colorWithCalibratedRed: 1
														 green: 1
														  blue: 1
														 alpha: alpha];
    NSColor *colorCaptionBackgnd = [NSColor colorWithCalibratedRed: 0
															 green: 0
															  blue: 0
															 alpha: 0.4];
    NSColor *colorWhite = [NSColor colorWithCalibratedRed: 1
													green: 1
													 blue: 1
													alpha: 1];

    [dict setObject: colorWhite forKey: NSForegroundColorAttributeName ];

    [image lockFocus];
    [colorClear set];
    [NSBezierPath fillRect: NSMakeRect(0,0,size.width,size.height)];
    NSBezierPath *path = [NSBezierPath bezierPathWithOvalInRect: NSMakeRect(1,1,circDiam-2,circDiam-2)];

    [path setLineWidth: 1.5];
    [colorTransBlue set];
    [path fill];

    NSBezierPath *hilite = [NSBezierPath bezierPathWithOvalInRect:
							NSMakeRect(circDiam*0.3, circDiam*0.7, circDiam*0.4, circDiam*0.15)];

    [colorTransWhite set];
    [hilite fill];

    [colorTransBlack set];
    [path stroke];

    [colorCaptionBackgnd set];
    [NSBezierPath fillRect: strRect];

    [nsstring drawInRect: strRect withAttributes: dict];

    [image unlockFocus];

    return image;
}


- (void) beginDragFrom: (NSPoint)where of: (PyrSlot*)slot string:(NSString*) string label:(NSString*) label
{
    NSImage *image = [self makeDragImage: slot label: label];

    NSPasteboard *pboard = [NSPasteboard pasteboardWithName: NSDragPboard];
    [pboard declareTypes: [NSArray arrayWithObjects: sSCObjType, NSStringPboardType, nil] owner: self];

	int fakeData;
    NSData *data = [NSData dataWithBytes: &fakeData length: sizeof(int)];

    [pboard setData: data forType: sSCObjType];
    [pboard setString: string forType: NSStringPboardType];

    NSSize imageSize = [image size];
    where.x -= circDiam / 2;
    where.y += circDiam / 4;

    NSSize dragOffset = NSMakeSize(0.0, 0.0);
    mDragStarted = YES;
    [self dragImage: image at: where offset: dragOffset event: [NSApp currentEvent]
		 pasteboard: pboard source: self slideBack: YES];
}

//- (void)mouseDragged:(NSEvent *)theEvent
//{
//    NSSize dragOffset = NSMakeSize(0.0, 0.0);
//    NSPasteboard *pboard;
//
//    pboard = [NSPasteboard pasteboardWithName:NSDragPboard];
//    [pboard declareTypes:[NSArray arrayWithObjects:sSCObjType, NSStringPboardType, nil]  owner:self];
//
//	int fakeData;
//    NSData *data = [NSData dataWithBytes: &fakeData length: sizeof(int)];
//
//    [pboard setData: data forType: sSCObjType];
//    [pboard setString: [self stringValue] forType: NSStringPboardType];
//
//    [self dragImage:[[[self window] fieldEditor:NO forObject:self] dragImageForSelectionWithEvent:NSMakePoint(0.0, 0.0)] at:[self imageLocation] offset:dragOffset
//			  event:theEvent pasteboard:pboard source:self slideBack:YES];
//
//    return;
//}

- (void)setSCView: (struct SCTextField*)inObject
{
	mSCViewObject = inObject;
}

-(BOOL) acceptsFirstResponder
{
	return mAcceptsFirstResponder;
}

-(void) setAcceptsFirstResponder: (BOOL) flag
{
	mAcceptsFirstResponder = flag;
}

//
// TEST: For tab-index
//- (BOOL)becomeFirstResponder
//{
//	if(!mSCViewObject->isFocus()){
//		[self resignFirstResponder];
//		[mSCViewObject->getTop()->focusView()->focusResponder() becomeFirstResponder];
//		return NO;
//	} else return [super becomeFirstResponder];
//}
//-(BOOL) acceptsFirstResponder
//{
//	return YES;
//}
//
//- (BOOL)resignFirstResponder
//{
//	post("resignFirstResponder\n");
//	[[self window] endEditingFor:self];
//	return YES;
//}

// this for keyDowns. KeyUps come in keyUp below.
- (BOOL)textView:(NSTextView *)aTextView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString
{
	//post("sctir\n");
	NSEvent* event = [NSApp currentEvent];
	NSString *characters = [event characters];
    unsigned int modifiers = [event modifierFlags];
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }
	if ([event keyCode] == 53){ //escape key breaks from modal or fullscreen windows
		[[self window] keyDown:event];
	}
	mSCViewObject->keyDown(character, modifiers,[event keyCode]);
	return YES;
}

// handle tabs and enter
- (BOOL)textView:(NSTextView *)aTextView doCommandBySelector:(SEL)aSelector
{
	//post("dcbs\n");
	//NSLog(@"sel: %@", NSStringFromSelector(aSelector));
	if (aSelector == @selector(insertTab:)){
		mSCViewObject->tabPrevFocus();
		return YES;
	} else if (aSelector == @selector(insertBacktab:)){
		mSCViewObject->tabNextFocus();
		return YES;
	} else if (aSelector == @selector(insertNewline:)){
		[[self window] endEditingFor:self];
		// fire the action
		pthread_mutex_lock (&gLangMutex);
		PyrSymbol *method = getsym("doAction");
		PyrObject *mObj;
		if (mObj = mSCViewObject->GetSCObj()) {
			VMGlobals *g = gMainVMGlobals;
			g->canCallOS = true;
			++g->sp;  SetObject(g->sp, mObj);
			runInterpreter(g, method, 1);
			g->canCallOS = false;
		}
		pthread_mutex_unlock (&gLangMutex);
		//NSLog(@"sv: %@", [self superview]);
		[self setEditingInactive:YES];
		[[self window] makeFirstResponder:[self superview]];
		//NSLog(@"firstresp: %@", [[self window] firstResponder]);
		return NO; // this will let the field editor end editing
	} else if (aSelector == @selector(moveUp:) || aSelector == @selector(moveDown:)){ // stop editing and forward key event (for numbox)
		NSEvent* event = [NSApp currentEvent];
		NSString *characters = [event characters];
		unsigned int modifiers = [event modifierFlags];
		unichar character = 0;
		if([characters length] > 0) {
			character = [characters characterAtIndex: 0];
		}
		[[self window] endEditingFor:self];
		[self setEditingInactive:YES];
		[[self window] makeFirstResponder:[self superview]];
		mSCViewObject->keyDown(character, modifiers,[event keyCode]);
		return YES;
	}
	return NO;
}


- (NSDragOperation)draggingEntered:(id < NSDraggingInfo >)dragInfo {
	NSPasteboard* pboard = [dragInfo draggingPasteboard];
	if ([[pboard types] containsObject: sSCObjType]) {
		return mSCViewObject->draggingEntered();
	} else if ([[pboard types] containsObject: NSStringPboardType]) {
		NSString *nsstring = [pboard stringForType: NSStringPboardType];
		if (!nsstring) return NSDragOperationNone;

		pthread_mutex_lock (&gLangMutex);
		VMGlobals *g = gMainVMGlobals;
		PyrString* pstrobj = newPyrString(g->gc, [nsstring UTF8String], 0, true);
		int classVarIndex = slotRawInt(&getsym("SCView")->u.classobj->classVarIndex);
		SetObject(&g->classvars->slots[classVarIndex+0], pstrobj);
		g->gc->GCWrite(g->classvars, pstrobj);
		//PyrSymbol *method = getsym("importDrag");
		//g->canCallOS = true;
		++g->sp;  SetObject(g->sp, s_scview->u.classobj);
		//runInterpreter(g, method, 1);
		//g->canCallOS = false;

		pthread_mutex_unlock (&gLangMutex);

		int fakeData;
		NSData *data = [NSData dataWithBytes: &fakeData length: sizeof(int)];
		[pboard setData: data forType: sSCObjType];

	} else if ([[pboard types] containsObject: NSFilenamesPboardType]) {
		NSArray *files = [pboard propertyListForType: NSFilenamesPboardType];
		if (!files) return NSDragOperationNone;
		pthread_mutex_lock (&gLangMutex);
		VMGlobals *g = gMainVMGlobals;
		int size = [files count];
		PyrObject* array = newPyrArray(g->gc, size, 0, true);

		for (int i=0; i<size; ++i) {
			NSString *path = [files objectAtIndex: i];
			PyrString *string = newPyrString(g->gc, [path UTF8String], 0, true);
			SetObject(array->slots + array->size, string);
			array->size++;
			g->gc->GCWrite(array, string);
		}

		int classVarIndex = slotRawInt(&getsym("SCView")->u.classobj->classVarIndex);
		SetObject(&g->classvars->slots[classVarIndex+0], array);
		g->gc->GCWrite(g->classvars, array);

		pthread_mutex_unlock (&gLangMutex);

		int fakeData;
		NSData *data = [NSData dataWithBytes: &fakeData length: sizeof(int)];
		[pboard setData: data forType: sSCObjType];

	} else {
        return NSDragOperationNone;
	}

}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {

	return mSCViewObject->performDrag();
}

- (void)keyUp:(NSEvent*)event
{

	//post("doKeyUpAction\n");
	NSString *characters = [event characters];
    unsigned int modifiers = [event modifierFlags];
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }
	mSCViewObject->keyUp(character, modifiers,[event keyCode]);
}


void QDDrawBevelRect(CGContextRef cgc, CGRect bounds, float width, bool inout);
- (void)drawRect:(NSRect)aRect
{
	[super drawRect:aRect];

	CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	CGContextSaveGState(cgc);
#if (MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)
	QDDrawBevelRect(cgc, (*(CGRect *)&([self bounds])), 1, true);
#else
	QDDrawBevelRect(cgc, NSRectToCGRect([self bounds]), 1, true);
#endif
	CGContextRestoreGState(cgc);

}

// doing this here keeps C++ classes much simpler
- (void) addNumberFormatter
{
	[NSNumberFormatter setDefaultFormatterBehavior:NSNumberFormatterBehavior10_4];
	mFormatter = [[[NSNumberFormatter alloc] init] autorelease];
	[mFormatter setNumberStyle: NSNumberFormatterDecimalStyle];
	[mFormatter setAlwaysShowsDecimalSeparator:NO];
	[mFormatter setExponentSymbol:@"e"];
	[mFormatter setMaximumFractionDigits:20];
	[mFormatter setDecimalSeparator:@"."];
	[mFormatter setUsesGroupingSeparator:NO];
	[[self cell] setFormatter:mFormatter];
}

// with these two methods we can check in C++ keyDown methods to see if editing is active
// if not, we start it and replace the string in the field editor
- (void) setEditingInactive: (BOOL)flag
{
	mEditingInactive = flag;
}

- (BOOL) editingInactive
{
	return mEditingInactive;
}
@end

@implementation SCNSMenuItem

- (void)setSCObject: (struct PyrObject*)inObject
{
    mMenuItemObj = inObject;
}

- (struct PyrObject*)getSCObject
{
    return mMenuItemObj;
}

- (void)doAction: (id)sender
{
	//	post("doAction \n");
    pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [self getSCObject];
	if(compiledOK && pobj){
		PyrSymbol *method = getsym("doAction");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj);
		++g->sp;  SetInt(g->sp, 1);
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
    pthread_mutex_unlock (&gLangMutex);
}

@end

@implementation SCNSLevelIndicator

- (void)drawRect:(NSRect)aRect
{
	if(drawPeak) { // make warning and critical show for peak not value if peak is drawn
		if(criticalAboveWarning) {
			if(peakValue >= critical) {
				[self setCriticalValue: value];
				[self setWarningValue: value - 0.1];
			} else if (peakValue >= warning) {
				[self setWarningValue: value];
				[self setCriticalValue: value + 0.1];
			} else {
				[self setWarningValue: 1.1];
				[self setCriticalValue: 1.1];
			}
		} else {
			if(peakValue <= critical) {
				[self setCriticalValue: value];
				[self setWarningValue: value + 0.1];
			} else if (peakValue <= warning) {
				[self setWarningValue: value];
				[self setCriticalValue: value - 0.1];
			} else {
				[self setWarningValue: -0.1];
				[self setCriticalValue: -0.1];
			}
		}

		[super drawRect:aRect];

		CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(cgc);
		CGContextSetRGBFillColor(cgc, 1.0, 1.0, 0.0, 0.6);
		CGRect peakRect;
		//peakRect = CGRectMake(peakLevel, aRect.origin.y + peakSubtract, 3.f, aRect.size.height - peakSubtract);
		peakRect = CGRectMake(peakLevel, peakY, 3.f, peakHeight);
		CGContextFillRect(cgc, peakRect);
		CGContextRestoreGState(cgc);
	} else  {
		[super drawRect:aRect];
	}
}

- (id)initWithFrame:(NSRect)frameRect
{
	if (![super initWithFrame:frameRect])
		return nil;
	drawPeak = NO;
	isVertical = NO;
	peakSubtract = 0.f;
	peakLevel = 0.f;
	value = peakValue = 0.0;
	critical = warning = 1.1;
	criticalAboveWarning = YES;
	[self prepPeakBounds];
	return self;
}

- (void)setFrame:(NSRect)frameRect
{
	[super setFrame:frameRect];
	[self prepPeakBounds];
	[self setNeedsDisplay:YES];
}

- (void)setDrawPeak:(BOOL)flag
{
	drawPeak = flag;
	if(!flag) {
		[self setWarningValue: warning];
		[self setCriticalValue: critical];
	}
	[self prepPeakBounds];
	[self setNeedsDisplay:YES];
}

- (void)setDoubleValue:(double)val
{
	[super setDoubleValue:val];
	value = val;
}

- (void)setMaxValue:(double)maxVal
{
	peakValue = (peakValue / [self maxValue]) * maxVal;
	[super setMaxValue:maxVal];
}

- (void)setIsVertical:(BOOL)flag
{
	isVertical = flag;
	[self prepPeakBounds];
	[self setNeedsDisplay:YES];
}

- (void)setPeakSubtract:(float)val
{
	peakSubtract = val;
	[self prepPeakBounds];
	[self setNeedsDisplay:YES];
}

- (void)setPeakLevel:(float)val
{
	peakValue = val * [self maxValue];
	peakLevel = val * ([self bounds].size.width - 3);
	//NSLog(@"peakLevel %f", peakLevel);
	[self setNeedsDisplay:YES];
}

- (void)prepPeakBounds
{
	NSRect bounds = [self bounds];
	peakY = bounds.origin.y + peakSubtract;
	peakHeight = bounds.size.height - peakSubtract;
}

- (void)setUpWarning:(double)val
{
	warning = nextafter(val, val + 1.0);
	if(critical > warning) {
		criticalAboveWarning = YES;
	} else {
		criticalAboveWarning = NO;
	}
	[self setWarningValue:warning];
}

- (void)setUpCritical:(double)val
{
	critical = nextafter(val, val + 1.0);
	if(critical > warning) {
		criticalAboveWarning = YES;
	} else {
		criticalAboveWarning = NO;
	}
	[self setCriticalValue:critical];
}

@end

@implementation SCNSFlippedView

- (BOOL)isFlipped { return YES; }

@end


@implementation SCNSWebView

-(void)initVars {
	loadCount=0; 
	handleLinks = true;
	enterExecutesSelection = YES;
}

- (void)setSCObject: (struct SCWebView*)inObject
{
    mSCWebView = inObject;
}

- (struct SCWebView*)getSCObject
{
    return mSCWebView;
}

- (void)setHandleLinks: (bool)handle
{
	handleLinks = handle;
}

- (void)resetLoadCount
{
	loadCount = 0;
}

- (BOOL)shouldCloseWithWindow {
	mSCWebView = nil;
	[[SCVirtualMachine sharedInstance] removeDeferredOperationsFor:self];
	return YES;
}

// delegate methods
- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame
{
	loadCount = loadCount - 1;
	if (loadCount <= 0 && mSCWebView) {
		loadCount = 0;
		SEL selector = @selector(doLoadAction);
		NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
									[sender methodSignatureForSelector: selector]];
		[invocation setTarget:sender];
		[invocation setSelector: selector];
		
		[[SCVirtualMachine sharedInstance] defer: invocation];
	}
}

- (void)webView:(WebView *)sender didStartProvisionalLoadForFrame:(WebFrame *)frame
{
	loadCount = loadCount + 1;
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame
{
	NSLog(@"didFailProvisionalLoadWithError: %@",[error localizedDescription]);
	if ([error code] == NSURLErrorCancelled) return; // this is Error -999
	if (mSCWebView) {
		post("SCWebView load request failed: '%s'\n", [[error localizedDescription] cString]);
		SEL selector = @selector(doFailAction);
		NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
									[sender methodSignatureForSelector: selector]];
		[invocation setTarget:sender];
		[invocation setSelector: selector];
		
		[[SCVirtualMachine sharedInstance] defer: invocation];
	}
}

- (void)doFailAction
{
	if (mSCWebView) {
		mSCWebView->doLoadFailedAction();
	}
}

- (void)doLoadAction
{
	if (mSCWebView) {
		mSCWebView->doOnLoadAction();
	}
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error
{
	NSLog(@"didFailLoadWithError: %@",[error localizedDescription]);
	if ([error code] == NSURLErrorCancelled) return; // this is Error -999
	if (mSCWebView) {
		post("SCWebView load request failed: '%s'\n", [[error localizedDescription] cString]);
		SEL selector = @selector(doFailAction);
		NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
									[sender methodSignatureForSelector: selector]];
		[invocation setTarget:sender];
		[invocation setSelector: selector];
		
		[[SCVirtualMachine sharedInstance] defer: invocation];
	}
}

// call link action if set, otherwise proceed
- (void)webView:(WebView *)webView decidePolicyForNavigationAction:(NSDictionary *)actionInformation request:(NSURLRequest *)request frame:(WebFrame *)frame decisionListener:(id )listener
{	
	//NSLog(@"decide policy");
	int actionKey = [[actionInformation objectForKey:WebActionNavigationTypeKey] intValue];
	if(actionKey == WebNavigationTypeLinkClicked) {
		//NSLog(@"link clicked");
		if(handleLinks) {
			loadCount = 0;
			//NSLog(@"handle link");
			[listener use];
		} else {
			//NSLog(@"fire action");
			[listener ignore];
			NSString *urlString = [[[request URL] absoluteString] retain];

			SEL selector = @selector(doLinkAction:);
			NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
										[webView methodSignatureForSelector: selector]];
			[invocation setTarget:webView];
			[invocation setSelector: selector];
			[invocation setArgument:&urlString atIndex:2];
			
			[[SCVirtualMachine sharedInstance] defer: invocation];
		}
	} else { 
		[listener use];
		//NSLog(@"link not clicked");
	}

}

// handle error
- (void)webView:(WebView *)webView unableToImplementPolicyWithError:(NSError *)error frame:(WebFrame *)frame
{
	NSLog(@"unableToImplementPolicyWithError: %@",[error localizedDescription]);
}

- (void)doLinkAction:(NSString *)urlString
{
	VMGlobals *g = gMainVMGlobals;
	const char * cstr = [urlString UTF8String];
	PyrString *string = newPyrString(g->gc, cstr, 0, true);
	mSCWebView->doLinkClickedAction(string);
	[urlString release];
}

- (BOOL)webView:(WebView *)webView shouldInsertText:(NSString *)insertText replacingDOMRange:(DOMRange *)range givenAction:(WebViewInsertAction)action
{
	//post("sctir\n");
	NSEvent* event = [NSApp currentEvent];
	NSString *characters = [event characters];
    unsigned int modifiers = [event modifierFlags];
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }
	if ([event keyCode] == 53){ //escape key breaks from modal or fullscreen windows
		[[self window] keyDown:event];
	}
	mSCWebView->keyDown(character, modifiers,[event keyCode]);
	if ([characters isEqual: @"\03"] ||
		(([characters isEqual: @"\n"] || [characters isEqual: @"\r"]) && ([event modifierFlags] & (NSControlKeyMask | NSShiftKeyMask)))) {
		[self executeSelection: self];
		return NO;
	}
	
	return YES;
}

- (void)keyDown:(NSEvent *)event
{
	NSString *characters = [event characters];
    unsigned int modifiers = [event modifierFlags];
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }
	if ([event keyCode] == 53){ //escape key breaks from modal or fullscreen windows
		[[self window] keyDown:event];
	}
	mSCWebView->keyDown(character, modifiers,[event keyCode]);
	if ([characters isEqual: @"\03"] ||
		(([characters isEqual: @"\n"] || [characters isEqual: @"\r"]) && ([event modifierFlags] & (NSControlKeyMask | NSShiftKeyMask)))) {
		[self executeSelection: self];
	} else {
		[super keyDown:event];
	}
	
}

- (void) setEnterExecutesSelection: (BOOL) flag
{
	enterExecutesSelection = flag;
}

- (void)sendSelection: (char*) methodName
{
    if (!compiledOK) {
        return;
    }
	
	[self setSelection];
	
    pthread_mutex_lock(&gLangMutex);
    runLibrary(getsym(methodName));
    pthread_mutex_unlock(&gLangMutex);
	
}

- (void)setSelection;
{
	NSString* selection = [self stringByEvaluatingJavaScriptFromString:@"(function (){selectLine(); selObj = window.getSelection(); string = selObj.toString(); selObj.collapseToEnd(); return string })();"];
	const char *text = [selection UTF8String];
	int textlength = strlen(text);
	
	[[SCVirtualMachine sharedInstance] setCmdLine: text length: textlength];
}

- (IBAction)openCode:(id)sender
{
    [self sendSelection: "openCodeFile"];
}

- (IBAction) showHelpFor: (id) sender
{
	[self sendSelection: "showHelp"];
}

- (IBAction)showHelpSearch:(id)sender {
	[self sendSelection: "showHelpSearch"];
}

- (IBAction)methodTemplates: (id)sender
{
    [self sendSelection: "methodTemplates"];
}

- (IBAction)methodReferences: (id)sender
{
    [self sendSelection: "methodReferences"];
}

- (IBAction)executeSelection: (id) sender
{
	if(enterExecutesSelection)
		[self sendSelection: "interpretPrintCmdLine" ];
}

// workaround for plaintext copy
- (BOOL)webView:(WebView *)webView doCommandBySelector:(SEL)command
{
	if (command == @selector(copy:)) {
		NSString *markup = [[self selectedDOMRange] markupString];
		NSData *data = [markup dataUsingEncoding: NSUTF8StringEncoding];
		NSNumber *n = [NSNumber numberWithUnsignedInteger: NSUTF8StringEncoding];
		NSDictionary *options = [NSDictionary dictionaryWithObject:n forKey: NSCharacterEncodingDocumentOption];
		NSAttributedString *as = [[NSAttributedString alloc] initWithHTML:data options:options documentAttributes: NULL];
		NSString *selectedString = [as string];
		[as autorelease];
		
		NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
		[pasteboard clearContents];
		NSArray *objectsToCopy = [NSArray arrayWithObject: selectedString];
		[pasteboard writeObjects:objectsToCopy];
		return YES;
	}
	return NO;
}

- (void)cmdF:(id)sender {
	[self keyDown:[NSApp currentEvent]];
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

	mLoadLinkInView = true;
	mLastURL = nil;
	NSRect matrect = SCtoNSRect(getDrawBounds());
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
	[[mTextView textContainer] setContainerSize:NSMakeSize([mScrollView contentSize].width, FLT_MAX)];
	//This is a hack, otherwise the mTextView always has focus even if makeFirstResponder: view is called...
	[mTextView setAcceptsFirstResponder:NO];
	[mScrollView setDrawsBackground:YES];
	[mCocoaToLangAction setUsesTabToFocusNextView:YES];
	[mCocoaToLangAction setEnterExecutesSelection:YES];
//	[mTextView autorelease];
//	[mScrollView autorelease];

	setVisibleFromParent();
}

SCCocoaTextView::~SCCocoaTextView()
{
	[mScrollView removeFromSuperview];
	[mCocoaToLangAction release];
	[mTextView release];
	[mScrollView release];
	[mLastURL release];
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
			//[[mTextView window] makeFirstResponder: mTextView];
        }
    } else {
        if (isFocus()) {
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
		if(!(mParent->isSubViewScroller())){
			SCRect pbounds = mParent->getLayout().bounds;
			mLayout.bounds.x = mBounds.x + pbounds.x;
			mLayout.bounds.y = mBounds.y + pbounds.y;
			mLayout.bounds.width = mBounds.width;
			mLayout.bounds.height = mBounds.height;
		} else {
			mLayout.bounds = mBounds;
		}
		[mScrollView setFrame: SCtoNSRect(mLayout.bounds)];
		//[mTextView setFrame: SCtoNSRect(mLayout.bounds)]; // not needed - br

	//	[mScrollView setBounds: SCtoNSRect(mBounds)];
	//	[mTextView setBounds: SCtoNSRect(mBounds)];
	//	[mScrollView setNeedsDisplay: YES]; // not needed - br
	//	[mTextView setNeedsDisplay: YES];	// not needed - br
}

void SCCocoaTextView::keyDown(int character, int modifiers, unsigned short keycode)
{
}

void SCCocoaTextView::keyUp(int character, int modifiers, unsigned short keycode)
{
}


int slotColorVal(PyrSlot *slot, SCColor *sccolor);
int setSlotColor(PyrSlot *slot, SCColor *sccolor);

int SCCocoaTextView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		mVisible = visible;
		setVisibleFromParent();
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

		setBounds(screct);

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
		PyrString* pstring = slotRawString(slot);
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

	if (strcmp(name, "open")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(slot);
		if(!pstring) return errNone;
		NSRange selectedRange = [mTextView selectedRange];
		NSString *path = [[NSString alloc] initWithCString: pstring->s length: pstring->size];

		int result = open(path);

		[path release];
		return result;
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
		PyrSlot *slots = slotRawObject(slot)->slots;
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
		PyrSlot *slots =slotRawObject(slot)->slots;
		PyrSlot *fontSlot = slots+0;
		if (IsNil(fontSlot)) return errNone; // use default font
		if (!(isKindOfSlot(fontSlot, s_font->u.classobj))) return errWrongType;
		PyrSlot *nameSlot = slotRawObject(fontSlot)->slots+0;
		PyrSlot *sizeSlot = slotRawObject(fontSlot)->slots+1;
		float size;
		int err = slotFloatVal(sizeSlot, &size);
		if (err) return err;

		PyrString *pstring = slotRawString(nameSlot);
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
//        PyrString* string = slotRawString(slot);
////        [doc insertText: string->s length: string->size];
//        return errNone;
//	}
	if (strcmp(name, "insertStringInRange")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots =slotRawObject(slot)->slots;
		PyrSlot *stringSlot = slots+0;
		if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;

		int rangeStart, rangeSize;
		int err = slotIntVal(slots+1, &rangeStart); //if -1 do not use range
        if (err) return err;
		err = slotIntVal(slots+2, &rangeSize);
        if (err) return err;

		PyrString* pstring = slotRawString(stringSlot);
		NSRange selectedRange;
		int length = [[mTextView string] length];

		if(rangeSize < 0) rangeSize = length - 1;
		if(rangeStart >= length) rangeStart = length - 1 ;
		if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;

		if(rangeStart<0) selectedRange =	NSMakeRange(0, length);
		else selectedRange =	NSMakeRange(rangeStart, rangeSize);

		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		BOOL editable = [mTextView isEditable];
		if(!editable) [mTextView setEditable:YES]; //always allow programmatic editing

		if ([mTextView  shouldChangeTextInRange: selectedRange replacementString: string]) {
			[mTextView  replaceCharactersInRange: selectedRange withString: string];
			[mTextView  didChangeText];
		}

		if(!editable) [mTextView setEditable:NO];

		[string release];

        return errNone;
	}

	if (strcmp(name, "setSyntaxColors")==0) {
		SyntaxColorize(mTextView);
		return errNone;
	}

	if (strcmp(name, "setUsesAutoInOutdent")==0) {
		bool uses = IsTrue(slot);
		[mTextView setUsesAutoInOutdent: uses];
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}

int SCCocoaTextView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	VMGlobals *g = gMainVMGlobals;

	if (strcmp(name, "string")==0) {
		NSString* str = [mTextView string];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}

	if (strcmp(name, "selectedString")==0) {
		NSString* str = [mTextView currentlySelectedTextOrLine:NULL];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
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

	if (strcmp(name, "path")==0) {
		if(mLastURL) {
			const char * cstr = [[mLastURL path] UTF8String];
			PyrString *string = newPyrString(g->gc, cstr, 0, true);
			SetObject(slot, string);
		} else SetNil(slot);
		return errNone;
	}

	return SCView::getProperty(symbol, slot);
}

int SCCocoaTextView::open(NSString *path)
{
	NSURL *url = [[NSURL alloc] initWithString: path];
	if(!url) return errFailed;

	NSTextStorage* text = [mTextView textStorage];

	NSString* extension = [path pathExtension];

	if ([extension isEqualToString: @"html"] || [extension isEqualToString: @"htm"]) {

		SEL sel = @selector(loadHTMLToTextView:);
		NSMethodSignature *sig = [SCCocoaTextViewResponder instanceMethodSignatureForSelector: sel];
		SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];

		NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
		[anInvocation setTarget: mCocoaToLangAction];
		[anInvocation setSelector: sel];
		[anInvocation setArgument:&url atIndex:2];
		[scvm defer: anInvocation];
		return errNone;
	}
	else {
		[text beginEditing];	// Bracket with begin/end editing for efficiency
		[[text mutableString] setString:@""];	// Empty the document

		NSError *error;
		BOOL success = [text readFromURL:url options:nil documentAttributes:nil error:&error];
		if(!success) {
			NSLog(@"Error opening file: %@", error);
			[text endEditing];
			[url release];
			return errFailed;
		}
	}

	if ([extension isEqualToString: @"sc"] || [extension isEqualToString: @"scd"]) {
		[mTextView setFont: [NSFont fontWithName: @"Monaco" size: 9]];
		SyntaxColorize(mTextView);
	}
	[mTextView scrollPoint:NSMakePoint(0, 0)];
	[text endEditing];
	[mLastURL release];
	mLastURL = url;
	return errNone;
}

void SCCocoaTextView::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mScrollView setHidden:NO];
		[mTextView setHidden:NO];
		return;
	} else {
		[mScrollView setHidden:YES];
		[mTextView setHidden:YES];
	}

}

extern int ivxSCTextView_linkAction;

bool SCCocoaTextView::linkAction(NSString *url)
{
	if(NotNil(mObj->slots + ivxSCTextView_linkAction)){
		pthread_mutex_lock (&gLangMutex);
		PyrSymbol *method = getsym("doLinkAction");
		if (mObj) {
			VMGlobals *g = gMainVMGlobals;
			g->canCallOS = true;
			const char * cstr = [url UTF8String];
			PyrString *string = newPyrString(g->gc, cstr, 0, true);
			++g->sp;  SetObject(g->sp, mObj);
			++g->sp;  SetObject(g->sp, string);
			++g->sp;  SetObject(g->sp, string);
			runInterpreter(g, method, 3);
			g->canCallOS = false;
		}
		pthread_mutex_unlock (&gLangMutex);
		return true;
	} else return false; // handle the link in the responder
}




////////////////////
SCView* NewSCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCMovieView(inParent, inObj, inBounds);
}

SCMovieView::SCMovieView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds)
{
		NSRect matrect = SCtoNSRect(getDrawBounds());
		mMovieView = [[QTMovieView alloc] initWithFrame:matrect];
		[mMovieView setStepButtonsVisible:YES];
		[mMovieView setTranslateButtonVisible:YES];
		NSView *view = mTop->GetNSView();
		[view addSubview: mMovieView];

		setVisibleFromParent();
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
	if(!(mParent->isSubViewScroller())){
		SCRect pbounds = mParent->getLayout().bounds;
		mLayout.bounds.x = mBounds.x + pbounds.x;
		mLayout.bounds.y = mBounds.y + pbounds.y;
		mLayout.bounds.width = mBounds.width;
		mLayout.bounds.height = mBounds.height;
	} else {
		mLayout.bounds = mBounds;
	}

	[mMovieView setFrame: SCtoNSRect(mLayout.bounds)];
	[mMovieView setBounds: SCtoNSRect(mBounds)]; //?
	[mMovieView setNeedsDisplay: YES];
}

int SCMovieView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		mVisible = visible;
		setVisibleFromParent();
		return errNone;
	}

	if (strcmp(name, "stop")==0) {
		[mMovie stop];
		return errNone;
	}

	if (strcmp(name, "start")==0) {
		[mMovie play];
		return errNone;
	}
	if (strcmp(name, "stepForward")==0) {
		[mMovie stepForward];
		return errNone;
	}
	if (strcmp(name, "stepBack")==0) {
		[mMovie stepBackward];
		return errNone;
	}
/*
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
*/
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		setBounds(screct);
		return errNone;
	}

	if (strcmp(name, "setMovie")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(slot);
		if(!pstring) return errNone;
		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		NSURL * url = [[NSURL alloc] initFileURLWithPath: string];
		QTMovie *movie = [[QTMovie alloc] initWithURL: url error:nil];
		if(!movie) return errFailed;
		//check for current movie:
		QTMovie *old_movie = [mMovieView movie];
		[mMovieView setMovie: movie];
		if(old_movie){
			[old_movie release];
		}
		[string release];
		[url release];
		/* QT: */
		mMovie = movie;
		mTime = [movie currentTime];

		return errNone;
	}

	if (strcmp(name, "setMuted")==0) {
		if(IsTrue(slot))[mMovie setMuted: YES];
		else [mMovie setMuted: NO];
		return errNone;
	}

	if (strcmp(name, "setEditable")==0) {
		if(IsTrue(slot))[mMovieView setEditable: YES];
		else [mMovieView setEditable: NO];
		return errNone;
	}

	if (strcmp(name, "setPlaysSelectionOnly")==0) {
		[mMovie setAttribute:[NSNumber numberWithBool:IsTrue(slot)] forKey:QTMoviePlaysSelectionOnlyAttribute];
		return errNone;
	}

	if (strcmp(name, "setRate")==0) {
		float rate;
		err = slotFloatVal(slot, &rate);
		if(err) return err;
		[mMovie setRate:rate];
		return errNone;
	}
	if (strcmp(name, "setVolume")==0) {
		float vol;
		err = slotFloatVal(slot, &vol);
		if(err) return err;
		[mMovie setVolume:vol];
		return errNone;
	}
	if (strcmp(name, "setLoopMode")==0) {
		int mode;
		err = slotIntVal(slot, &mode);
		if(err) return err;
		NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithBool:NO], QTMovieLoopsBackAndForthAttribute, [NSNumber numberWithBool:NO], QTMovieLoopsAttribute, nil];
		switch(mode)
		{
				case 0: [dict setObject:[NSNumber numberWithBool:YES] forKey:QTMovieLoopsBackAndForthAttribute]; break;
				case 1: [dict setObject:[NSNumber numberWithBool:YES] forKey:QTMovieLoopsAttribute]; break;
				case 2:
				default: break;
		}
		[mMovie setMovieAttributes:dict];
		return errNone;
	}
	if (strcmp(name, "gotoEnd")==0) {
		[mMovie gotoEnd];
		return errNone;
	}

	if (strcmp(name, "gotoBeginning")==0) {
		[mMovie gotoBeginning];
		return errNone;
	}
/*
	if (strcmp(name, "showControllerAndAdjustSize")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		BOOL showC, adjust;
		if(IsTrue(slots+0)) showC=YES;
		else showC = NO;
		if(IsTrue(slots+1)) adjust=YES;
		else adjust = NO;
		[mMovieView setControllerVisible:showC];
		//[mMovieView showController: showC adjustingSize: adjust];
		return errNone;
	}
*/
	if (strcmp(name, "copy")==0) {
		[mMovieView copy: NULL];
		return errNone;
	}
	if (strcmp(name, "clear")==0) {
		[mMovieView delete: NULL];
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
		float time;
		err = slotFloatVal(slot, &time);
		if(err) return err;
		QTTime qttime = mTime;
		qttime.timeValue = time * qttime.timeScale;
		[mMovie setCurrentTime:qttime];
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}

int SCMovieView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	//GetMovieDuration([[mMovieView movie] QTMovie]);
	if (strcmp(name, "getCurrentTime")==0) {
		float time;
		//post("timescale: %d  \n",  mTimeRecord.scale);
		QTTime qt_time = [mMovie currentTime];
		time = (float) ((float)qt_time.timeValue / qt_time.timeScale);
		SetFloat(slot, time);
		return errNone;
	}
	return SCView::getProperty(symbol, slot);
}

void SCMovieView::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mMovieView setHidden:NO];
	} else {
		[mMovieView setHidden:YES];
	}
	[mMovieView setNeedsDisplay:YES];
	NSRect frame = [mMovieView frame];
	[mMovieView setFrame: NSInsetRect(frame,1,1)];
	[mMovieView setFrame: frame];
	mTop->resetFocus();
	refresh();
}

extern PyrSymbol *s_proto, *s_parent;
extern int ivxIdentDict_array, ivxIdentDict_size, ivxIdentDict_parent, ivxIdentDict_proto, ivxIdentDict_know;
int identDictPut(struct VMGlobals *g, PyrObject *dict, PyrSlot *key, PyrSlot *value);
extern PyrClass *class_identdict;
///////////
/*
SCQuartzComposerView by Scott Wilson
Copyright (c) 2007 Scott Wilson. All rights reserved.
Development funded in part by the Arts and Humanites Research Council http://www.ahrc.ac.uk/
*/

SCView* NewSCQuartzComposerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCQuartzComposerView(inParent, inObj, inBounds);
}

SCQuartzComposerView::SCQuartzComposerView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
: SCView(inParent, inObj, inBounds)
{
	NSRect matrect = SCtoNSRect(getDrawBounds());
	mQCView = [[QCView alloc] initWithFrame:matrect];
	[mQCView setEventForwardingMask: NSAnyEventMask];
	NSView *view = mTop->GetNSView();
	[view addSubview: mQCView];

	setVisibleFromParent();
}

SCQuartzComposerView::~SCQuartzComposerView()
{
	[mQCView removeFromSuperview];
	[mQCView release];
}


void SCQuartzComposerView::setBounds(SCRect screct)
{
	[[mQCView superview] setNeedsDisplayInRect:[mQCView frame]];
	mBounds = screct;
	if(!(mParent->isSubViewScroller())){
		SCRect pbounds = mParent->getLayout().bounds;
		mLayout.bounds.x = mBounds.x + pbounds.x;
		mLayout.bounds.y = mBounds.y + pbounds.y;
		mLayout.bounds.width = mBounds.width;
		mLayout.bounds.height = mBounds.height;
	} else {
		mLayout.bounds = mBounds;
	}
	[mQCView setFrame: SCtoNSRect(mLayout.bounds)];
	[mQCView setNeedsDisplay: YES];

}


int SCQuartzComposerView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
//		if(visible  && mParent->isVisible())
//		{
//			[mQCView setHidden:NO];
//		}
//		else
//		{
//			[mQCView setHidden:YES];
//			mTop->resetFocus();
//		}
		mVisible = visible;
		setVisibleFromParent();
		return errNone;
	}

	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		int err = slotGetSCRect(slot, &screct);
		if (err) return err;
		mBounds = screct;
		setBounds(screct);
		return errNone;
	}

	if (strcmp(name, "loadCompositionFromFile")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(slot);
		if(!pstring) return errNone;
		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		BOOL success = [mQCView loadCompositionFromFile: string];
		if(!success) return errFailed;
		[string release];
		return errNone;
	}

	if (strcmp(name, "stop")==0) {
		[mQCView stopRendering];
		return errNone;
	}

	if (strcmp(name, "start")==0) {
		BOOL success = [mQCView startRendering];
		if(!success) return errFailed;
		return errNone;
	}

	//// doesn't seem to work ; fix later
	//	if (strcmp(name, "erase")==0) {
	//		[mQCView erase];
	//		return errNone;
	//	}
	//
//	if (strcmp(name, "eraseColor")==0) {
//		SCColor rgb;
//		int err = slotColorVal(slot, &rgb);
//		if (err) return err;
//		NSColor *color = [NSColor colorWithCalibratedRed: rgb.red green: rgb.green blue: rgb.blue alpha: rgb.alpha];
//		//NSLog(@"color: %@", color);
//		//NSColor *color = [NSColor blueColor];
//		[mQCView setEraseColor: color];
//		[mQCView erase];
//		return errNone;
//	}

	if (strcmp(name, "setMaxRenderingFrameRate")==0) {
		float rate;
		int err = slotFloatVal(slot, &rate);
		if (err) return err;
		[mQCView setMaxRenderingFrameRate: rate];
		return errNone;
	}

	if (strcmp(name, "setInputValue")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		PyrSymbol *keysymbol;
		int err = slotSymbolVal(slots + 0, &keysymbol);
		if (err) return err;

		NSString *key = [[NSString alloc] initWithCString: keysymbol->name encoding: NSASCIIStringEncoding];
		if(![[mQCView inputKeys] containsObject: key]) {
			[key release];
			//post("There is no port with key \"%s\".\n\n", [key cString]);
			return errFailed;
		}

		id nsObject = getNSObjectForSCObject(slots + 1, &err);
		if(!nsObject) {[key release]; return err;}
		BOOL success = [mQCView setValue: nsObject forInputKey: key];
		[key release];
		if(!success) return errFailed;
		return errNone;

	}

	return SCView::setProperty(symbol, slot);
}

id SCQuartzComposerView::getNSObjectForSCObject(PyrSlot *scobject, int *returnErr) {

	int err;
	// find the value type and set appropriately
	if(IsFloat(scobject)) { // it's a float
		float val;
		err = slotFloatVal(scobject, &val);
		if (err) {returnErr = &err; return NULL;}
		NSNumber *returnObject = [NSNumber numberWithFloat: val];
		if(!returnObject) { err = errFailed; returnErr = &err; return NULL;}
		return returnObject;
	} else if(IsInt(scobject)) { // it's an int
		int val;
		err = slotIntVal(scobject, &val);
		if (err) {returnErr = &err; return NULL;}
		NSNumber *returnObject = [NSNumber numberWithInt: val];
		if(!returnObject) { err = errFailed; returnErr = &err; return NULL;}
		return returnObject;
	} else if(IsTrue(scobject)) { // it's bool true
		NSNumber *returnObject = [NSNumber numberWithBool: YES];
		if(!returnObject) { err = errFailed; returnErr = &err; return NULL;}
		return returnObject;
	} else if(IsFalse(scobject)) { // it's bool false
		NSNumber *returnObject = [NSNumber numberWithBool: NO];
		if(!returnObject) { err = errFailed; returnErr = &err; return NULL;}
		return returnObject;
	} else if(isKindOfSlot(scobject, s_string->u.classobj)) { // it's a string
		PyrString *string = slotRawString(scobject);
		if(string->size == 0) { err = errFailed; returnErr = &err; return NULL;}
		NSString *returnObject =  [NSString stringWithCString: string->s length: string->size];
		if(!returnObject) { err = errFailed; returnErr = &err; return NULL;}
		return returnObject;
	} else if(isKindOfSlot(scobject, s_color->u.classobj)) { // it's a color
		SCColor rgb;
		err = slotColorVal(scobject, &rgb);
		if (err) {returnErr = &err; return NULL;}
		NSColor *returnObject = [NSColor colorWithCalibratedRed: rgb.red green: rgb.green blue: rgb.blue alpha: rgb.alpha];
		if(!returnObject) { err = errFailed; returnErr = &err; return NULL;}
		return returnObject;
	} else if(isKindOfSlot(scobject, s_identitydictionary->u.classobj)) { // it's a structure (dict)
		PyrObject *array;
		array = slotRawObject(&(slotRawObject(scobject)->slots[ivxIdentDict_array]));
		if (!isKindOf((PyrObject*)array, class_array)) { err = errFailed; returnErr = &err; return NULL;}
		NSMutableDictionary *structure = [NSMutableDictionary dictionary];
		int len = array->size;

		for(int i=0; i<len; i=i+2){
			PyrSlot *element = array->slots+i;
			if(!IsNil(element)) {
				PyrSymbol *keysymbol;
				err = slotSymbolVal(element, &keysymbol);
				if (err) {returnErr = &err; return NULL;}
				NSString *key = [[NSString alloc] initWithCString: keysymbol->name encoding: NSASCIIStringEncoding];
				int innerErr;
				id innerSCObject = getNSObjectForSCObject(element + 1, &innerErr);
				if(!innerSCObject) { returnErr = &innerErr; return NULL;}
				[structure setObject: innerSCObject forKey: key];
			}
		}
		err = errNone;
		returnErr = &err;
		return structure;
	} else if(isKindOfSlot(scobject, class_array)) { // it's a structure (array)
		PyrSlot *array = scobject;
		int len = slotRawObject(array)->size;
		NSMutableArray *structure = [NSMutableArray arrayWithCapacity: (unsigned)len];

		for(int i =0; i<len; i++){
			PyrSlot *element = slotRawObject(array)->slots+i;
			int innerErr;
			id innerSCObject = getNSObjectForSCObject(element, &innerErr);
			if(!innerSCObject) { returnErr = &innerErr; return NULL;}
			[structure addObject: innerSCObject];
		}

		err = errNone;
		returnErr = &err;
		return structure;
	} else if(isKindOfSlot(scobject, s_scimage->u.classobj)) { // it's an SCImage : )
		SCImage *scimage = (SCImage *)slotRawPtr(slotRawObject(scobject)->slots);
		if(scimage) {
			if([scimage isAccelerated])
				return [scimage ciimage];
			else
				return [scimage nsimage];
		}
		else {
			post("SCQuartzComposerView: invalid SCImage as input port !");
			err = errWrongType;
			returnErr = &err;
			return NULL;
		}
	} else {
		err = errWrongType; // it's something else...
		returnErr = &err;
		return NULL;
	}

}

int SCQuartzComposerView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	if (strcmp(name, "getOutputValue")==0) {
		PyrSymbol *keysymbol;
		int err = slotSymbolVal(slot, &keysymbol);
		if (err) return err;

		NSString *key = [[NSString alloc] initWithCString: keysymbol->name encoding: NSASCIIStringEncoding];
		if(![[mQCView outputKeys] containsObject: key]) {
			[key release];
			//post("There is no port with key \"%s\".\n\n", [key cString]);
			return errFailed;
		}

		NSDictionary *outputAttributes = [[mQCView attributes] objectForKey: key];
		NSString *type = [outputAttributes objectForKey:QCPortAttributeTypeKey];

		id nsObject = [mQCView valueForOutputKey: key];
		[key release];
		err = getSCObjectForNSObject(slot, nsObject, type);
		if (err) return err;
		return errNone;

	} else if(strcmp(name, "getInputValue")==0) {
		PyrSymbol *keysymbol;
		int err = slotSymbolVal(slot, &keysymbol);
		if (err) return err;

		NSString *key = [[NSString alloc] initWithCString: keysymbol->name encoding: NSASCIIStringEncoding];
		if(![[mQCView inputKeys] containsObject: key]) {
			[key release];
			//post("There is no port with key \"%s\".\n\n", [key cString]);
			return errFailed;
		}

		NSDictionary *inputAttributes = [[mQCView attributes] objectForKey: key];
		NSString *type = [inputAttributes objectForKey:QCPortAttributeTypeKey];

		id nsObject = [mQCView valueForInputKey: key];
		[key release];
		err = getSCObjectForNSObject(slot, nsObject, type);
		if (err) return err;
		return errNone;

	} else if(strcmp(name, "getInputKeys")==0) {
		NSArray* inputKeys = [mQCView inputKeys];
		int size = [inputKeys count];
		VMGlobals *g = gMainVMGlobals;
		PyrObject* array = newPyrArray(g->gc, size, 0, true);
		SetObject(slot, array);

		for (int i=0; i<size; ++i) {
			NSString *name = [inputKeys objectAtIndex: i];
			PyrString *string = newPyrString(g->gc, [name UTF8String], 0, true);
			SetObject(array->slots + array->size, string);
			array->size++;
			g->gc->GCWrite(array, string);
		}

		return errNone;

	} else if(strcmp(name, "getOutputKeys")==0) {
		NSArray* inputKeys = [mQCView outputKeys];
		int size = [inputKeys count];
		VMGlobals *g = gMainVMGlobals;
		PyrObject* array = newPyrArray(g->gc, size, 0, true);
		SetObject(slot, array);

		for (int i=0; i<size; ++i) {
			NSString *name = [inputKeys objectAtIndex: i];
			PyrString *string = newPyrString(g->gc, [name UTF8String], 0, true);
			SetObject(array->slots + array->size, string);
			array->size++;
			g->gc->GCWrite(array, string);
		}

		return errNone;
	}

	return SCView::getProperty(symbol, slot);
}

int SCQuartzComposerView::getSCObjectForNSObject(PyrSlot *slot, id nsObject, NSString *type)
{
	if([type isEqualToString:QCPortTypeBoolean]) {
		SetBool(slot, [nsObject boolValue]);
		return errNone;
	} else if([type isEqualToString:QCPortTypeIndex]) {
		SetInt(slot, [nsObject intValue]);
		return errNone;
	} else if([type isEqualToString:QCPortTypeNumber]) {
		SetFloat(slot, [nsObject floatValue]);
		return errNone;
	} else if([type isEqualToString:QCPortTypeString]) {
		const char * cstr = [nsObject UTF8String];
		VMGlobals *g = gMainVMGlobals;
		PyrString *string = newPyrString(g->gc, cstr, 0, true);
		SetObject(slot, string);
		return errNone;
	} else if([type isEqualToString:QCPortTypeColor]) {

		VMGlobals *g = gMainVMGlobals;
		PyrObject* colorObj = instantiateObject(g->gc, s_color->u.classobj, 0, false, true);
		SCColor rgb = SCMakeColor([nsObject redComponent], [nsObject greenComponent], [nsObject blueComponent], [nsObject alphaComponent]);
		SetObject(slot, colorObj);
		int err = setSlotColor(slot, &rgb);
		if (err) { return err;}
		return errNone;
	} else if([type isEqualToString:QCPortTypeStructure]) {
		//NSLog(@"QCPortTypeStructure");
		//NSLog(@"class: %@", [nsObject class]);

		// for the moment QC seems to deal with all internal structures as NSCFDictionary
		// but check here to be safe
		if([nsObject isKindOfClass: [NSDictionary class]]){
			//NSLog(@"it's a dict");
			PyrObject *dict, *array;
			VMGlobals *g = gMainVMGlobals;

			dict = instantiateObject(g->gc, class_identdict, 5, true, false);
			array = newPyrArray(g->gc, 4, 0, false);
			array->size = 4;
			nilSlots(array->slots, array->size);
			SetObject(dict->slots + ivxIdentDict_array, array);
			g->gc->GCWrite(dict, array);
			SetObject(slot, dict);

			NSEnumerator *enumerator = [nsObject keyEnumerator];
			id key;

			while ((key = [enumerator nextObject])) {
				//NSLog(@"key class: %@", [key class]);
				id innerNSObject = [nsObject objectForKey: key];
				//NSLog(@"innerNSObject: %@", innerNSObject);
				PyrSlot innerSlot;
				NSString *innerType;

				if([innerNSObject isKindOfClass: [NSNumber class]]){
					//NSLog(@"objCType: %s", [innerNSObject objCType]);
					if(!strcmp([innerNSObject objCType], @encode(BOOL))) {
						//NSLog(@"Number");
						innerType = QCPortTypeBoolean;
					} else if(!strcmp([innerNSObject objCType], @encode(int))) {
						innerType = QCPortTypeIndex;
					} else innerType = QCPortTypeNumber;
				} else if([innerNSObject isKindOfClass: [NSColor class]]){
					//NSLog(@"Color");
					innerType = QCPortTypeColor;
				} else if([innerNSObject isKindOfClass: [NSString class]]){
					//NSLog(@"String");
					innerType = QCPortTypeString;
				} else if([innerNSObject isKindOfClass: [NSArray class]] || [innerNSObject isKindOfClass: [NSDictionary class]]){
					//NSLog(@"Structure");
					innerType = QCPortTypeStructure;
				} else return errWrongType; // it's something else

				//NSLog(@"innerObject Class: %@", [innerNSObject class]);
				int err = getSCObjectForNSObject(&innerSlot, innerNSObject, innerType);
				if(err) return err;

				PyrSlot outKey;
				SetSymbol(&outKey, getsym([key cString]));
				err = identDictPut(g, dict, &outKey, &innerSlot);
				if(err) return err;
			}


			return errNone;
		}

	}
	else if([type isEqualToString:QCPortTypeImage]) { // SCImage
		NSImage *nsimage = (NSImage*)nsObject;
		if(!nsimage)
			post("SCQuartzComposerView: bad return value as QCPortTypeImage");

		SCImage *scimage = [[SCImage alloc]initWithNSImage:nsimage];
		if(scimage) {
			VMGlobals *g		= gMainVMGlobals;
			PyrObject *object	= newPyrSCImage(g); // should be garbage collected
			if(object) {
				PyrSlot		*slots	= object->slots;
				SetObject(slot, object);
				SetPtr(slots + 0, scimage);
				SetFloat(slots + 1, (float)[scimage width]);
				SetFloat(slots + 2, (float)[scimage height]);
				return errNone;
			}
		}
		else {
			post("SCQuartzComposerView: failed NSImage to SCImage conversion !");
		}
	}
	return errWrongType; // it's something else


}

void SCQuartzComposerView::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mQCView setHidden:NO];
		return;
	} else {
		[mQCView setHidden:YES];
	}
	mTop->resetFocus();
}


////////////////////
SCView* NewSCWebView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCWebView(inParent, inObj, inBounds);
}

SCWebView::SCWebView(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
: SCView(inParent, inObj, inBounds)
{
	NSRect matrect = SCtoNSRect(getDrawBounds());
	//NSLog(@"bounds %@", NSStringFromRect(matrect));
	mWebView = [[SCNSWebView alloc] initWithFrame:matrect frameName:nil groupName: nil];
	[mWebView initVars];
	NSView *view = mTop->GetNSView();
	flipView = [[view window] contentView];
	if (![flipView isKindOfClass: [SCNSFlippedView class]] ) {
		flipView = [[SCNSFlippedView alloc] initWithFrame:[view frame]]; // a wrapper view hack to get coords right
		[view retain];
		[view setAutoresizingMask: 63];
		[flipView setAutoresizesSubviews:YES];
		[[view window] setContentView:flipView];
		[flipView addSubview:view];
	} else {
		[flipView retain]; // increment the retain count
	}
	[view addSubview:mWebView];
	
	[mWebView setEditingDelegate:mWebView];
	[mWebView setFrameLoadDelegate:mWebView];
	[mWebView setPolicyDelegate:mWebView];
	//[mWebView setUIDelegate:mWebView];
	
	[mWebView setSCObject: this];
	
	[mWebView setFocusRingType:NSFocusRingTypeNone];
	[mWebView setEnterExecutesSelection:YES];
	[mWebView setPreferencesIdentifier:@"SuperCollider"];
	
	setVisibleFromParent();
}

SCWebView::~SCWebView()
{
	[mWebView removeFromSuperview];
	[mWebView close];
	[[SCVirtualMachine sharedInstance] removeDeferredOperationsFor:mWebView];
	[mWebView release];
	if ([flipView retainCount] == 1) { // if nobody else needs this wrapperView, clean it up
		NSView *view = mTop->GetNSView();
		[[view window] setContentView:view];
		[view release];
		[flipView removeFromSuperview];
		[flipView release];
	} else {
		[flipView release]; // otherwise just decrement the retain count
	}

}

void SCWebView::setBounds(SCRect screct)
{
	if(
	   mBounds.x == screct.x &&
	   mBounds.x == screct.y &&
	   mBounds.width == screct.width &&
	   mBounds.height == screct.height
	   ) { // in case - prevent a refresh if the bounds are the same
		return;
	}
	
	mBounds = screct;
	if(!(mParent->isSubViewScroller())){
		SCRect pbounds = mParent->getLayout().bounds;
		mLayout.bounds.x = mBounds.x + pbounds.x;
		mLayout.bounds.y = mBounds.y + pbounds.y;
		mLayout.bounds.width = mBounds.width;
		mLayout.bounds.height = mBounds.height;
	} else {
		mLayout.bounds = mBounds;
	}
	
	[mWebView setFrame: SCtoNSRect(mLayout.bounds)];
}

void SCWebView::doOnLoadAction()
{
	pthread_mutex_lock (&gLangMutex);			
	if(compiledOK){
		PyrSymbol *method = getsym("didLoad");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj); 
		runInterpreter(g, method, 1);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

void SCWebView::doLoadFailedAction()
{
	pthread_mutex_lock (&gLangMutex);			
	if(compiledOK){
		PyrSymbol *method = getsym("didFail");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj); 
		runInterpreter(g, method, 1);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

void SCWebView::doLinkClickedAction(PyrString* pstring)
{
	pthread_mutex_lock (&gLangMutex);			
	if(compiledOK){
		PyrSymbol *method = getsym("linkActivated");
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, mObj); 
		++g->sp;  SetObject(g->sp, pstring); 
		runInterpreter(g, method, 2);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}

int SCWebView::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;
	
	if (strcmp(name, "url")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(slot);
		if(!pstring) return errFailed;
		[mWebView resetLoadCount];
		NSString *path = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		[[mWebView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:path]]];
		return errNone;
	}
	
	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		mVisible = visible;
		setVisibleFromParent();
		return errNone;
	}
	
	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		setBounds(screct);
		return errNone;
	}
	
	if (strcmp(name, "back")==0) {
		if([mWebView canGoBack]) {
			[mWebView resetLoadCount];
			SEL selector = @selector(goBack);
			NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
										[mWebView methodSignatureForSelector: selector]];
			[invocation setTarget:mWebView];
			[invocation setSelector: selector];
			
			[[SCVirtualMachine sharedInstance] defer: invocation];
		}
		return errNone;
	}
	
	if (strcmp(name, "forward")==0) {
		if([mWebView canGoForward]) {
			[mWebView resetLoadCount];
			SEL selector = @selector(goForward);
			NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
										[mWebView methodSignatureForSelector: selector]];
			[invocation setTarget:mWebView];
			[invocation setSelector: selector];
			
			[[SCVirtualMachine sharedInstance] defer: invocation];
		}
		return errNone;
	}
	
	if (strcmp(name, "reload")==0) {
		[mWebView resetLoadCount];
		SEL selector = @selector(reload:);
		NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
									[mWebView methodSignatureForSelector: selector]];
		[invocation setTarget:mWebView];
		[invocation setSelector: selector];
		[invocation setArgument:&mWebView atIndex:2];
		
		[[SCVirtualMachine sharedInstance] defer: invocation];
		return errNone;
	}
	
	if (strcmp(name, "html")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(slot);
		if(!pstring) return errFailed;
		[mWebView resetLoadCount];
		NSString *html = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		[[mWebView mainFrame] loadHTMLString:html baseURL:nil];
		return errNone;
	}
	
	if (strcmp(name, "handleLinks")==0) {
		bool handleLinks = IsTrue(slot);
		[mWebView setHandleLinks: handleLinks];
		return errNone;
	}
	
	if (strcmp(name, "editable")==0) {
		bool editable = IsTrue(slot);
		[mWebView setEditable: editable];
		return errNone;
	}
	
	if (strcmp(name, "findText")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots =slotRawObject(slot)->slots;
		PyrSlot *stringSlot = slots+0;
		if(!isKindOfSlot(stringSlot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(stringSlot);
		if(!pstring) return errFailed;
		NSString *searchText = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		
		PyrSlot *dir = slots+1;
		bool goesForward = IsFalse(dir);

		[mWebView searchFor:searchText direction:goesForward caseSensitive:NO wrap:YES];
		return errNone;
	}
	
	if (strcmp(name, "enterExecutesSelection")==0) {
		if(IsTrue(slot))[mWebView setEnterExecutesSelection:YES];
		else [mWebView setEnterExecutesSelection:NO];
		return errNone;
	}

	if (strcmp(name, "fontFamily")==0) {
		if(!isKindOfSlot(slot, class_array)) return errWrongType;
		PyrSlot *slots = slotRawObject(slot)->slots;
		if(!IsSym(slots+0)) return errWrongType;
		PyrString* pstring = slotRawString(slots+1);
		if(!pstring) return errFailed;

		PyrSymbol *genericFont = slotRawSymbol(slots+0);
		NSString *specificFont = [[NSString alloc] initWithCString: pstring->s length: pstring->size];

		if (genericFont == getsym("standard"))
			[[mWebView preferences] setStandardFontFamily: specificFont];
		else if (genericFont == getsym("fixed"))
			[[mWebView preferences] setFixedFontFamily: specificFont];
		else if (genericFont == getsym("serif"))
			[[mWebView preferences] setSerifFontFamily: specificFont];
		else if (genericFont == getsym("sansSerif"))
			[[mWebView preferences] setSansSerifFontFamily: specificFont];
		else if (genericFont == getsym("cursive"))
			[[mWebView preferences] setCursiveFontFamily: specificFont];
		else if (genericFont == getsym("fantasy"))
			[[mWebView preferences] setFantasyFontFamily: specificFont];

		[specificFont release];

		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}


int SCWebView::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	VMGlobals *g = gMainVMGlobals;
	
	if (strcmp(name, "url")==0) {
		NSString *rawLocationString = [mWebView stringByEvaluatingJavaScriptFromString:@"location.href;"];
		const char * cstr = [rawLocationString UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}
	
	if (strcmp(name, "html")==0) {
		NSString *html = [mWebView stringByEvaluatingJavaScriptFromString:@"document.documentElement.outerHTML"];
		const char * cstr = [html UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}
	
	if (strcmp(name, "plainText")==0) {
		NSString *plainText = [mWebView stringByEvaluatingJavaScriptFromString:@"document.body.innerText"];
		const char * cstr = [plainText UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}
	
	if (strcmp(name, "selectedText")==0) {
		NSString *selectedText = [mWebView stringByEvaluatingJavaScriptFromString:@"(function (){return window.getSelection().toString();})();"];
		const char * cstr = [selectedText UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}
	
	if (strcmp(name, "title")==0) {
		NSString *rawLocationString = [mWebView mainFrameTitle];
		const char * cstr = [rawLocationString UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}
	
	return SCView::getProperty(symbol, slot);
}


void SCWebView::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mWebView setHidden:NO];
	} else {
		[mWebView setHidden:YES];
	}
	[mWebView setNeedsDisplay:YES];
	NSRect frame = [mWebView frame];
	[mWebView setFrame: frame];
	mTop->resetFocus();
	refresh();
}


void SCWebView::tabPrevFocus()
{
	mTop->tabPrevFocus();
}
void SCWebView::tabNextFocus()
{
	//post("next focus\n");
	mTop->tabNextFocus();
}

void SCWebView::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    }
}


////////////////////

////////////////////
SCView* NewSCTextField(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCTextField(inParent, inObj, inBounds);
}

SCTextField::SCTextField(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
	: SCView(inParent, inObj, inBounds)
//: SCStaticText(inParent, inObj, inBounds)
{
	NSRect matrect = SCtoNSRect(getDrawBounds());
	mTextField = [[SCTextFieldResponder alloc] initWithFrame:matrect];
	NSView *view = mTop->GetNSView();
	[view addSubview: mTextField];

	[mTextField setFocusRingType:NSFocusRingTypeNone];
	//NSLog(@"SCTextField init\n");

	//mCocoaToLangAction = [SCTextFieldResponder alloc];
	[mTextField setSCView: this];
	[mTextField setDelegate: mTextField];
	[mTextField setEditingInactive:NO];

	[mTextField setBordered:NO]; // for some reason, if we don't set this we can't have transparency
	[mTextField setDrawsBackground:NO]; // SCView will draw for us. This also allows 0 < alpha < 1

	[[mTextField cell] setScrollable:YES];

	mBackground = new SolidColorBackground(SCMakeColor(1.0,1.0,1.0, 1.0)); // default is white

	[mTextField registerForDraggedTypes: [NSArray arrayWithObjects: sSCObjType, NSStringPboardType, NSFilenamesPboardType, nil]];
	//This is a hack, otherwise the mTextField always has focus even if makeFirstResponder: view is called...
	[mTextField setAcceptsFirstResponder:NO];
	setVisibleFromParent();
}

SCTextField::~SCTextField()
{
	[mTextField removeFromSuperview];
	[mTextField release];
}

void SCTextField::setBounds(SCRect screct)
{
	if(
		mBounds.x == screct.x &&
		mBounds.x == screct.y &&
		mBounds.width == screct.width &&
		mBounds.height == screct.height
	) { // in case - prevent a refresh if the bounds are the same
		return;
	}

	//[[mTextField superview] setNeedsDisplayInRect:[mTextField frame]];
	mBounds = screct;
	if(!(mParent->isSubViewScroller())){
		SCRect pbounds = mParent->getLayout().bounds;
		mLayout.bounds.x = mBounds.x + pbounds.x;
		mLayout.bounds.y = mBounds.y + pbounds.y;
		mLayout.bounds.width = mBounds.width;
		mLayout.bounds.height = mBounds.height;
	} else {
		mLayout.bounds = mBounds;
	}

	[mTextField setFrame: SCtoNSRect(mLayout.bounds)];
	//[mTextField setNeedsDisplay: YES];
}

int SCTextField::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;

	if (strcmp(name, "string")==0) {
		if(!isKindOfSlot(slot, class_string)) return errWrongType;
		PyrString* pstring = slotRawString(slot);
		if(!pstring) return errNone;
		NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
		[mTextField setStringValue: string];
		[string release];
		return errNone;
	}

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		mVisible = visible;
		setVisibleFromParent();
		return errNone;
	}

	if (strcmp(name, "font")==0) {
		NSFont *font;
		if (IsNil(slot)){

			font = [NSFont controlContentFontOfSize: 0.0 ];

		}else{

			if (!(isKindOfSlot(slot, s_font->u.classobj))) return errWrongType;
			PyrSlot *nameSlot = slotRawObject(slot)->slots+0;
			PyrSlot *sizeSlot = slotRawObject(slot)->slots+1;
			float size;
			if (IsNil(sizeSlot)){
				size = [[mTextField font] pointSize];
			}else{
				int err = slotFloatVal(sizeSlot, &size);
				if (err) return err;
			}

			PyrString *pstring = slotRawString(nameSlot);
			NSString *fontName = [NSString stringWithCString: pstring->s length: pstring->size];
			if (!fontName) return errFailed;
			font = [NSFont fontWithName: fontName size: size];
			if (!font) return errFailed;
		}

		[mTextField setFont: font];
		return errNone;
	}

	if (strcmp(name, "stringColor")==0) {
		err = slotColorVal(slot, &mStringColor);
		if (err) return err;
		NSColor *color = [NSColor colorWithCalibratedRed: mStringColor.red
                            green: mStringColor.green
                            blue: mStringColor.blue
                            alpha: mStringColor.alpha];
		[mTextField setTextColor: color];
		refresh();
		return errNone;
	}

	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		setBounds(screct);
		return errNone;
	}

	if (strcmp(name, "align")==0) {
		int align;
		if (IsSym(slot)) {
			[[mTextField window] endEditingFor:mTextField];
			if (slotRawSymbol(slot)->name[0] == 'l')      [mTextField setAlignment: NSLeftTextAlignment];
			else if (slotRawSymbol(slot)->name[0] == 'r') [mTextField setAlignment: NSRightTextAlignment];
			else if (slotRawSymbol(slot)->name[0] == 'c') [mTextField setAlignment: NSCenterTextAlignment];
			else return errFailed;

			//[mTextField display];
		}/* else {
			err = slotIntVal(slot, &align);
			if (err) return err;
			mAlignment = align;
		} */
		refresh();
		return errNone;
	}

	if (strcmp(name, "enabled")==0) {
		bool enabled = IsTrue(slot);
		if (mEnabled != enabled) {
			mEnabled = enabled;
			[mTextField setEnabled:(BOOL)enabled];
			if (!mEnabled) mTop->resetFocus();
			refresh();
		}
		return errNone;
	}

	//return SCStaticText::setProperty(symbol, slot);
	return SCView::setProperty(symbol, slot);
}


int SCTextField::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;
	VMGlobals *g = gMainVMGlobals;

	if (strcmp(name, "string")==0) {
		NSString* str = [mTextField stringValue];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(slot, string);
		return errNone;
	}
	if (strcmp(name, "boxColor")==0) {
		return setSlotColor(slot, &mBoxColor);
	}
	if (strcmp(name, "stringColor")==0) {
		return setSlotColor(slot, &mStringColor);
	}

	//return SCStaticText::getProperty(symbol, slot);
	return SCView::getProperty(symbol, slot);
}


void SCTextField::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mTextField setHidden:NO];
	} else {
		[mTextField setHidden:YES];
	}
	[mTextField setNeedsDisplay:YES];
	NSRect frame = [mTextField frame];
	[mTextField setFrame: NSInsetRect(frame,1,1)];
	[mTextField setFrame: frame];
	mTop->resetFocus();
	refresh();
}

void SCTextField::makeFocus(bool focus)
{
    if (focus) {
		if (canFocus() && !isFocus()) {
			[mTextField setAcceptsFirstResponder:YES];

			// trick focus ring into drawing
			SEL sel = @selector(setNeedsDisplay:);
			NSMethodSignature *sig = [NSView instanceMethodSignatureForSelector: sel];
			NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
			SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
			[anInvocation setTarget: [mTextField superview]];
			[anInvocation setSelector: sel];
			BOOL flag = YES;
			[anInvocation setArgument: &flag atIndex: 2];
			[scvm defer: anInvocation];
        }
    } else {
        if (isFocus()) {
			[mTextField setAcceptsFirstResponder:NO];
        }
    }
	SCView::makeFocus(focus);
}

void SCTextField::tabPrevFocus()
{
	mTop->tabPrevFocus();
}
void SCTextField::tabNextFocus()
{
	//post("next focus\n");
	mTop->tabNextFocus();
}

void SCTextField::keyDown(int character, int modifiers, unsigned short keycode)
{
	// when enter was pressed firstResponder was passed to the parent SCGraphView
	// this checks if keyDown was passed from there and if so starts to edit using the key pressed
	// We access the field editor as that's the receiving object while editing is active
	if([mTextField editingInactive] && (character != 13 || character !=3)) {
		//post("keydownEP\n");
		[[mTextField window] makeFirstResponder:mTextField];
		unichar charVal = (unichar)character;
		NSString *charstring = [[NSString alloc] initWithCharacters: &charVal length: 1];
		NSText *fieldEditor = [[mTextField window] fieldEditor:YES forObject:mTextField];
		[fieldEditor setString:charstring];
		[charstring release];
		[mTextField setEditingInactive:NO];
	}
	SCView::keyDown(character, modifiers, keycode);
}

extern PyrSymbol* s_canReceiveDrag;

bool SCTextField::canReceiveDrag()
{
    PyrSlot result;
    sendMessage(s_canReceiveDrag, 0, 0, &result);
    return IsTrue(&result);
}

NSDragOperation SCTextField::draggingEntered()
{
	bool flag = canReceiveDrag();
	mTop->setDragView(flag ? this : 0);
	[mTextField displayIfNeeded];
	return flag ? NSDragOperationEvery : NSDragOperationNone;
}

extern PyrSymbol* s_receiveDrag;

BOOL SCTextField::performDrag()
{
        bool flag = canReceiveDrag();
        if (flag) {
            mTop->setDragView(this);
            sendMessage(s_receiveDrag, 0, 0, 0);
			mTop->setDragView(0);
        } else {
            mTop->setDragView(0);
        }
        [mTextField displayIfNeeded];
        return flag ? YES : NO;
}

void SCTextField::mouseTrack(SCPoint where, int modifiers, NSEvent *theEvent)
{
    if (modifiers & NSCommandKeyMask) {
        beginDrag(where);
    }
}

extern PyrSymbol *s_beginDrag;

void SCTextField::beginDrag(SCPoint where)
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

	//mTop->beginDragCallback(where, &slot, string, label);
	NSPoint point = NSMakePoint(where.x, where.y);
	[mTextField beginDragFrom: point of: &slot string: string label: label];
}

////////////////////
SCView* NewSCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCNumberBox(inParent, inObj, inBounds);
}

SCNumberBox::SCNumberBox(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
: SCTextField(inParent, inObj, inBounds)
//: SCStaticText(inParent, inObj, inBounds)
{
	[mTextField addNumberFormatter];
}

SCNumberBox::~SCNumberBox()
{
}

int SCNumberBox::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;

	if (strcmp(name, "value")==0) {
		double value;
		err = slotDoubleVal(slot, &value);
		if (err) return err;
		[mTextField setDoubleValue:value];
		return errNone;
	}

//	if (strcmp(name, "clipLo")==0) {
//		double value;
//		err = slotDoubleVal(slot, &value);
//		if (err) return err;
//		[[mTextField formatter] setMinimum:[NSNumber numberWithDouble:value]];
//		return errNone;
//	}
//
//	if (strcmp(name, "clipHi")==0) {
//		double value;
//		err = slotDoubleVal(slot, &value);
//		if (err) return err;
//		[[mTextField formatter] setMaximum:[NSNumber numberWithDouble:value]];
//		return errNone;
//	}

	return SCTextField::setProperty(symbol, slot);
}

int SCNumberBox::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

	if (strcmp(name, "value")==0) {
		double val = [mTextField doubleValue];
        SetFloat(slot, val);
		return errNone;
	}

	return SCTextField::getProperty(symbol, slot);
}

void SCNumberBox::keyDown(int character, int modifiers, unsigned short keycode)
{
	// test for arrows enter and return
	if([mTextField editingInactive] && (character == 63232 || character == 63233 || character == 63234 || character == 63235 || character == 3 || character == 13)){
		SCView::keyDown(character, modifiers, keycode);
	} else {
		SCTextField::keyDown(character, modifiers, keycode);
	}
}

////////////////////
SCView* NewSCLevelIndicator(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
{
	return new SCLevelIndicator(inParent, inObj, inBounds);
}

SCLevelIndicator::SCLevelIndicator(SCContainerView *inParent, PyrObject* inObj, SCRect inBounds)
: SCView(inParent, inObj, inBounds)
{
	NSRect matrect = SCtoNSRect(getDrawBounds());
	mLevelIndicator = [[SCNSLevelIndicator alloc] initWithFrame:matrect];
	NSView *view = mTop->GetNSView();
	[view addSubview: mLevelIndicator];

	[[mLevelIndicator cell] setLevelIndicatorStyle:NSContinuousCapacityLevelIndicatorStyle];
	[mLevelIndicator setMinValue:0.0];
	[mLevelIndicator setMaxValue:1.0];
	mStyle = 1;
	mNumSteps = 1;
	if(matrect.size.height > matrect.size.width) {
		[mLevelIndicator setBoundsRotation: 90.0]; // vertical
		[mLevelIndicator setIsVertical:YES];
		[mLevelIndicator setNeedsDisplay: YES];
		mIsVertical = true;
	} else mIsVertical = false;

	mWarning = 0.0;
	mCritical = 0.0;
	mTickHeight = 0.f;

	mImage = [[NSImage imageNamed:@"NSApplicationIcon"] retain];
	setImage();

	setVisibleFromParent();
}

SCLevelIndicator::~SCLevelIndicator()
{
	[mLevelIndicator removeFromSuperview];
	[mLevelIndicator release];
	[mImage release];
}

void SCLevelIndicator::setBounds(SCRect screct)
{
	mBounds = screct;
	if(!(mParent->isSubViewScroller())){
		SCRect pbounds = mParent->getLayout().bounds;
		mLayout.bounds.x = mBounds.x + pbounds.x;
		mLayout.bounds.y = mBounds.y + pbounds.y;
		mLayout.bounds.width = mBounds.width;
		mLayout.bounds.height = mBounds.height;
	} else {
		mLayout.bounds = mBounds;
	}

	if(mBounds.height > mBounds.width) {
		[mLevelIndicator setBoundsRotation: 90.0]; // vertical
		mIsVertical = true;
		[mLevelIndicator setIsVertical:YES];
	} else {
		[mLevelIndicator setBoundsRotation: 0.0]; // horizontal
		mIsVertical = false;
		[mLevelIndicator setIsVertical:NO];
	}

	if([mLevelIndicator numberOfTickMarks] > 0)
		mTickHeight = [mLevelIndicator rectOfTickMarkAtIndex:0].size.height; // 0 will be major if there are any
	else
		mTickHeight = 0.f;

	[mLevelIndicator setPeakSubtract:mTickHeight];

	setImage();
	[mLevelIndicator setFrame: SCtoNSRect(mLayout.bounds)];
	[mLevelIndicator setNeedsDisplay: YES];
}

void SCLevelIndicator::setImage()
{
	NSImage* newImage = nil;

	NSSize imageSize = [mImage size];
	float width = imageSize.width;
	float height = imageSize.height;

	SCRect bounds = getDrawBounds();
	NSSize targetSize;
	if(mIsVertical) {
		targetSize = NSMakeSize(bounds.height / mNumSteps, bounds.width - mTickHeight);
	} else {
		targetSize = NSMakeSize(bounds.width / mNumSteps, bounds.height - mTickHeight);
	}

	float targetWidth = targetSize.width;
	float targetHeight = targetSize.height;

	float scaleFactor = 0.0;
	float scaledWidth = targetWidth;
	float scaledHeight = targetHeight;

	NSPoint newPoint = NSMakePoint(0,0);

	if ( NSEqualSizes( imageSize, targetSize ) == NO )
	{
		float widthFactor, heightFactor;
		if(mIsVertical){
			widthFactor = targetHeight / width;
			heightFactor = targetWidth / height;
		} else {
			widthFactor = targetWidth / width;
			heightFactor = targetHeight / height;
		}

		if ( widthFactor < heightFactor)
			scaleFactor = widthFactor;
		else
			scaleFactor = heightFactor;

		scaledWidth = width * scaleFactor;
		scaledHeight = height * scaleFactor;

		if(mIsVertical) {
			if ( widthFactor < heightFactor)
				newPoint.y = (targetWidth - scaledHeight) * 0.5;
			else if ( widthFactor > heightFactor )
				newPoint.x = (targetHeight - scaledWidth) * 0.5;
		} else {
			if ( widthFactor < heightFactor)
				newPoint.y = (targetHeight - scaledHeight) * 0.5;
			else if ( widthFactor > heightFactor )
				newPoint.x = (targetWidth - scaledWidth) * 0.5;
		}
	}

	//NSLog(@"targetSize: %@", NSStringFromSize(targetSize));

	// create a new image to draw into
	newImage = [[NSImage alloc] initWithSize:targetSize];

	[newImage lockFocus];

	NSRect newRect;
	newRect.origin = newPoint;
	newRect.size.width = scaledWidth;
	newRect.size.height = scaledHeight;

	//NSLog(@"newPoint: %@", NSStringFromRect(newRect));

	if(mIsVertical) {

		NSAffineTransform *rotateTF = [NSAffineTransform transform];
		NSPoint centerPoint = NSMakePoint(targetSize.width / 2, targetSize.height / 2);

		[rotateTF translateXBy: centerPoint.x yBy: centerPoint.y];
		[rotateTF rotateByDegrees: - 90];
		[rotateTF translateXBy: -centerPoint.y yBy: -centerPoint.x];
		[rotateTF concat];
	}

	[mImage drawInRect: newRect fromRect: NSZeroRect operation: NSCompositeSourceOver fraction: 1.0];

	[newImage unlockFocus];

	[[mLevelIndicator cell] setImage:newImage];

	[newImage autorelease];
}
void SCLevelIndicator::resetParams()
{
	if(mStyle >= 2) {
		[mLevelIndicator setUpWarning:mWarning * mNumSteps];
		[mLevelIndicator setUpCritical:mCritical * mNumSteps];
		[mLevelIndicator setMaxValue: mNumSteps];
	} else {
		[mLevelIndicator setUpWarning:mWarning];
		[mLevelIndicator setUpCritical:mCritical];
		[mLevelIndicator setMaxValue: 1.0];
	}
	[mLevelIndicator setNeedsDisplay:YES];
}

int SCLevelIndicator::setProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	int err;
	char *name = symbol->name;

	if (strcmp(name, "value")==0) {
		double value;
		err = slotDoubleVal(slot, &value);
		if (err) return err;
		if(mStyle >= 2) value = value * mNumSteps;
		[mLevelIndicator setDoubleValue:value];
		return errNone;
	}

	if (strcmp(name, "warning")==0) {
		double value;
		err = slotDoubleVal(slot, &value);
		if (err) return err;
		mWarning = value;
		resetParams();
		return errNone;
	}

	if (strcmp(name, "critical")==0) {
		double value;
		err = slotDoubleVal(slot, &value);
		if (err) return err;
		mCritical = value;
		resetParams();
		return errNone;
	}

	if (strcmp(name, "visible")==0) {
		bool visible = IsTrue(slot);
		mVisible = visible;
		setVisibleFromParent();
		return errNone;
	}

	if (strcmp(name, "bounds")==0) {
		SCRect screct;
		err = slotGetSCRect(slot, &screct);
		if (err) return err;
		setBounds(screct);
		return errNone;
	}

	if (strcmp(name, "enabled")==0) {
		bool enabled = IsTrue(slot);
		if (mEnabled != enabled) {
			mEnabled = enabled;
			[mLevelIndicator setEnabled:(BOOL)enabled];
			if (!mEnabled) mTop->resetFocus();
			refresh();
		}
		return errNone;
	}

	if (strcmp(name, "style")==0) {
		int style;
		err = slotIntVal(slot, &style);
		if (err) return err;
		if(style <= 0) {
			[[mLevelIndicator cell] setLevelIndicatorStyle:NSContinuousCapacityLevelIndicatorStyle];
			mStyle = 0;
		} else if(style == 1) {
			[[mLevelIndicator cell] setLevelIndicatorStyle:NSRelevancyLevelIndicatorStyle];
			mStyle = 1;
		} else if(style == 2) {
			[[mLevelIndicator cell] setLevelIndicatorStyle:NSDiscreteCapacityLevelIndicatorStyle];
			mStyle = 2;
		} else {
			[[mLevelIndicator cell] setLevelIndicatorStyle:NSRatingLevelIndicatorStyle];
			mStyle = 3;
			setImage();
		}
		resetParams();
		return errNone;
	}

	if (strcmp(name, "numSteps")==0) {
		int numSteps;
		err = slotIntVal(slot, &numSteps);
		if (err) return err;
		mNumSteps = numSteps >= 1 ? (double)numSteps : 1.0;
		resetParams();
		setImage();
		return errNone;
	}

	if (strcmp(name, "numTicks")==0) {
		int numTicks;
		err = slotIntVal(slot, &numTicks);
		if (err) return err;
		[mLevelIndicator setNumberOfTickMarks:numTicks];
		if(numTicks > 0)
			mTickHeight = [mLevelIndicator rectOfTickMarkAtIndex:0].size.height; // 0 will be major if there are any
		else
			mTickHeight = 0.f;
		[mLevelIndicator setPeakSubtract:mTickHeight];
		setImage();
		return errNone;
	}

	if (strcmp(name, "numMajorTicks")==0) {
		int numTicks;
		err = slotIntVal(slot, &numTicks);
		if (err) return err;
		[mLevelIndicator setNumberOfMajorTickMarks:numTicks];
		if([mLevelIndicator numberOfTickMarks] > 0)
			mTickHeight = [mLevelIndicator rectOfTickMarkAtIndex:0].size.height; // 0 will be major if there are any
		else
			mTickHeight = 0.f;
		[mLevelIndicator setPeakSubtract:mTickHeight];
		setImage();
		return errNone;
	}

	if (strcmp(name, "image")==0) {
		if(isKindOfSlot(slot, s_scimage->u.classobj)) { // it's an SCImage : )
			SCImage *scimage = (SCImage *)slotRawPtr(slotRawObject(slot)->slots);
			if(scimage) {
				NSImage *oldImage = mImage;
				mImage = [[scimage nsimage] retain];
				setImage();
				[oldImage release];
				return errNone;
			}
			else {
				post("Invalid Image");
				return errWrongType;
			}
		} else {
			return errWrongType;
		}
	}

	if (strcmp(name, "drawsPeak")==0) {
		bool drawsPeak = IsTrue(slot);
		[mLevelIndicator setDrawPeak:drawsPeak];
		return errNone;
	}

	if (strcmp(name, "peakLevel")==0) {
		float value;
		err = slotFloatVal(slot, &value);
		if (err) return err;
		[mLevelIndicator setPeakLevel:sc_clip(value, 0.f, 1.f)];
		return errNone;
	}

	return SCView::setProperty(symbol, slot);
}


int SCLevelIndicator::getProperty(PyrSymbol *symbol, PyrSlot *slot)
{
	char *name = symbol->name;

	if (strcmp(name, "value")==0) {
		double value = [mLevelIndicator doubleValue];
		SetFloat(slot, value);
		return errNone;
	}

	return SCView::getProperty(symbol, slot);
}


void SCLevelIndicator::setVisibleFromParent()
{
	if(mVisible && mParent->isVisible()) {
		[mLevelIndicator setHidden:NO];
	} else {
		[mLevelIndicator setHidden:YES];
	}
	[mLevelIndicator setNeedsDisplay:YES];
	mTop->resetFocus();
	refresh();
}

void SCLevelIndicator::tabPrevFocus()
{
	mTop->tabPrevFocus();
}
void SCLevelIndicator::tabNextFocus()
{
	//post("next focus\n");
	mTop->tabNextFocus();
}

