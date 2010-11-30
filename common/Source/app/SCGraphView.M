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

#import "SCGraphView.h"
#import "SCVirtualMachine.h"
#include "PyrKernel.h"
#include "PyrMessage.h"
#include "VMGlobals.h"
#include "PyrSched.h"
#include "SC_BoundsMacros.h"
#include "GC.h"
#include <Carbon/Carbon.h>

#if __LP64__
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

CGRect SCtoCGRect(SCRect screct);

extern PyrSymbol *s_draw;
extern PyrSymbol *s_scview;
extern PyrSymbol *s_closed;
extern PyrSymbol *s_callDrawHook;
extern PyrSymbol *s_toggleEditMode;

@implementation SCGraphView

- (void)setAcceptsClickThrough:(BOOL)boo
{
	acceptsClickThrough = boo;
}

- (void)setAutoScrolls:(BOOL)boo;
{
	autoScrolls = boo;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)theEvent
{
	return acceptsClickThrough;
}

- (BOOL)isFlipped
{
	return YES;
}

- (BOOL)mouseDownCanMoveWindow
{
	return NO;
}

//eat all key Events might add this
- (BOOL)performKeyEquivalent:(NSEvent *)event
{
//	NSString *characters = [event characters];
	unsigned int modifiers = [event modifierFlags];
    //control tab/escape doesn't get passed here at all ?
	if((modifiers & NSCommandKeyMask) && !(modifiers & NSFunctionKeyMask)) // allow cmd-key only
		[self keyDown: event];
	return NO; //for now pass on the event
}

- (void)flagsChanged:(NSEvent *)event
{
    unsigned int modifiers = [event modifierFlags];
//    NSLog(@" modifiers %d %08X",modifiers, modifiers);
    if (mTopView) {
	     SCView *view = mTopView->focusView();
        if (view) {
            view->keyModifiersChanged(modifiers);
        } else {
            mTopView->keyModifiersChanged(modifiers);
        }
	}
}
- (void) keyDown: (NSEvent*) event
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
    //control tab/escape doesn't get passed here at all ?
 //   NSLog(@"unicode %d  length:%d clength:%d mTopView %08X  modifiers %d %08X",
//		character,[characters length],[characters cStringLength], mTopView, modifiers, modifiers);

    if (mTopView) {
		// for some reason modifiers becomes 256 on my machine with no keys pressed. So need to mask against known keys.
		uint32 allKnownModifiers = NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSCommandKeyMask
			| NSAlternateKeyMask | NSHelpKeyMask | NSFunctionKeyMask | NSCommandKeyMask;
        if(character == 9 && ((modifiers & allKnownModifiers) == 0)) {
            mTopView->tabPrevFocus();
            return;
        } else if (character == 25 && ((modifiers & allKnownModifiers) == NSShiftKeyMask)) {
			mTopView->tabNextFocus();
			return;
        } // other tab keys avail for user
        SCView *view = mTopView->focusView();
        if (view) {
            view->keyDown(character, modifiers,[event keyCode]);
        } else {
            mTopView->keyDown(character,modifiers,[event keyCode]);
        }
    }
/*
    if ([characters isEqual: @"w"] && (modifiers & NSCommandKeyMask)) {
        [[SCVirtualMachine sharedInstance] endFullScreen: [self window]];
    }
    if ([characters isEqual: @"z"]) {
        [[SCVirtualMachine sharedInstance] endFullScreen: [self window]];
    }
*/
}


- (void) keyUp: (NSEvent*) event
{
    NSString *characters = [event characters];
    unsigned int modifiers = [event modifierFlags];
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }
//   NSLog(@"keyUp: unicode %d  length:%d clength:%d mTopView %08X  modifiers %d %08X",
//		character,[characters length],[characters cStringLength], mTopView, modifiers, modifiers);
    if (mTopView) {

		uint32 allKnownModifiers = NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSCommandKeyMask
			| NSAlternateKeyMask | NSHelpKeyMask | NSFunctionKeyMask;
        if(character == 9 && ((modifiers & allKnownModifiers) == 0)) {
            return;
        } else if (character == 25 && ((modifiers & allKnownModifiers) == NSShiftKeyMask)) {
			return;
        } // other tab keys avail for user

        SCView *view = mTopView->focusView();
        if (view) {
            view->keyUp(character, modifiers,[event keyCode]);
        } else {
            mTopView->keyUp(character,modifiers,[event keyCode]);
        }
    }
}

NSRect SCtoNSRect(SCRect screct)
{
    NSRect nsrect;
    nsrect.origin.x = screct.x;
    nsrect.origin.y = screct.y;
    nsrect.size.width = screct.width;
    nsrect.size.height = screct.height;
    return nsrect;
}

static NSString *sSCObjType = @"SuperCollider object address";

- (id)initWithFrame: (NSRect) frame
{
    [super initWithFrame: frame];
    [self registerForDraggedTypes: [NSArray arrayWithObjects: sSCObjType, NSStringPboardType, NSFilenamesPboardType, nil]];
	mDragStarted = NO;
	mMenuView = 0;
	mWindowObj = 0;
	mTopView = 0;
    windowShouldClose = YES;
	acceptsClickThrough = YES;
	autoScrolls = YES;
    return self;
}

- (NSMenu*) menuForEvent:(NSEvent*)theEvent;
{
    NSPoint mouseLoc;
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
	if (!mTopView) return 0;
    SCView *view = mTopView->findView(scpoint);
    if (!view) return 0;
	return view->contextMenu(scpoint);
}

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
	if (!mTopView) return;
    unsigned int modifiers = [theEvent modifierFlags];
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
    if (view) {
		[[self window] makeFirstResponder:self]; // there may be an active field editor
		mDragStarted = NO;
		mMenuView = 0;
        view->makeFocus(true);
		bool constructionmode = mTopView->ConstructionMode();
		if(!constructionmode)
		{
			view->mouseDownAction(scpoint, modifiers,theEvent);
			view->mouseBeginTrack(scpoint, modifiers,theEvent);
		}else
		view->setConstructionModeFromPoint(scpoint);

		[self displayIfNeeded];

        while (keepOn && !mDragStarted && !mMenuView) {
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
						view->doConstructionMove(scpoint);
						mTopView->refresh();
					}else
						view->mouseDownAction(scpoint, modifiers,theEvent);
//						post("down \n");
						break;
                case NSLeftMouseDragged:
					if(autoScrolls) [self autoscroll:theEvent];
                case NSRightMouseDragged:
                case NSOtherMouseDragged:
						if(constructionmode)
						{
							view->doConstructionMove(scpoint);
							mTopView->refresh();
						}else
							view->mouseTrack(scpoint, modifiers,theEvent);
							view->mouseMoveAction(scpoint, modifiers,theEvent);
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
//							if(!view.GetSCObj()) break;
							view->mouseUpAction(scpoint, modifiers,theEvent);
							view->mouseEndTrack(scpoint, modifiers,theEvent);
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
	mMenuView = 0;
    return;
}

-(void)mouseMoved:(NSEvent*)theEvent
{
	NSPoint mouseLoc;
	if (!mTopView) return;
    unsigned int modifiers = [theEvent modifierFlags];
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
    if (view) {
		mDragStarted = NO;
		mMenuView = 0;
//        view->makeFocus(true);
        view->mouseOver(scpoint, modifiers, theEvent);
	}
}

//scrollWheel:(NSEvent*)theEvent;

- (void)setSCObject: (struct PyrObject*)inObject;
{
    mWindowObj = inObject;
}

- (struct PyrObject*)getSCObject
{
    return mWindowObj;
}

void damageFunc(SCRect inRect, void* inData)
{
    SCGraphView *view = (SCGraphView*)inData;

    [view setNeedsDisplayInRect: SCtoNSRect(inRect)];
}

void dragFunc(SCPoint where, PyrSlot *inSlot, NSString* inString, NSString* label, void* inData)
{
    SCGraphView *view = (SCGraphView*)inData;
    NSPoint point = NSMakePoint(where.x, where.y);
    [view beginDragFrom: point of: inSlot string: inString label: label];
}

- (unsigned int)draggingSourceOperationMaskForLocal: (BOOL)flag
{
    return flag ? NSDragOperationEvery : NSDragOperationNone;
}

- (void)setSCTopView: (SCTopView*)inView
{
    mTopView = inView;
    mTopView->setDamageCallback(damageFunc, (void*)self);
    mTopView->setDragCallback(dragFunc);
	mTopView->SetNSView(self);
	oldBounds = NSMakeRect(0, 0, 0, 0);
}

- (void)dealloc
{
	//printf("dealloc %08X mTopView %08X\n", self, mTopView);
    delete mTopView;
	mTopView = 0;
	[super dealloc];
}


- (void)closeWindow
{
	[[self window] close];
}

- (void)removeFromSuperview
{
	[[SCVirtualMachine sharedInstance] removeDeferredOperationsFor: self];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super removeFromSuperview];
}

- (void)willClose
{
	[[SCVirtualMachine sharedInstance] removeDeferredOperationsFor: self];
	[[SCVirtualMachine sharedInstance] removeDeferredOperationsFor: [self window]];
	[[NSNotificationCenter defaultCenter] removeObserver:self];

    pthread_mutex_lock (&gLangMutex);
    if (mWindowObj) {
        SetPtr(mWindowObj->slots + 0, self);
        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, mWindowObj); // push window obj
        runInterpreter(g, s_closed, 1);
        g->canCallOS = false;
        mWindowObj = 0;
    }
    pthread_mutex_unlock (&gLangMutex);

    delete mTopView;
	mTopView = 0;
}
extern PyrSymbol * s_didBecomeKey;
extern PyrSymbol * s_didResignKey;
- (void) didResignKey{
	if (mWindowObj) {
		if(NotNil(mWindowObj->slots + 11)){ // check whether endFrontAction is nil
			pthread_mutex_lock (&gLangMutex);
			SetPtr(mWindowObj->slots + 0, self);
			VMGlobals *g = gMainVMGlobals;
			g->canCallOS = true;

			++g->sp;  SetObject(g->sp, mWindowObj); // push window obj
			runInterpreter(g, s_didResignKey, 1);
			g->canCallOS = false;
			pthread_mutex_unlock (&gLangMutex);
		}
	}
}

- (void) didBecomeKey{
    if (mWindowObj) {
		if(NotNil(mWindowObj->slots + 10)){

			pthread_mutex_lock (&gLangMutex);
			SetPtr(mWindowObj->slots + 0, self);
			VMGlobals *g = gMainVMGlobals;
			g->canCallOS = true;
			++g->sp;  SetObject(g->sp, mWindowObj); // push window obj
			runInterpreter(g, s_didBecomeKey, 1);
			g->canCallOS = false;
			pthread_mutex_unlock (&gLangMutex);
		}
	}
}
/*  from command-w, scvm is the delegate */
- (void)setWindowShouldClose:(BOOL)boo
{
    windowShouldClose = boo;
}

- (BOOL)windowShouldClose
{
    return windowShouldClose;
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

- (unsigned int)draggingEntered:(id)dragInfo {
	if (!mTopView) return NSDragOperationNone;
	NSPasteboard* pboard = [dragInfo draggingPasteboard];
	if ([[pboard types] containsObject: sSCObjType]) {

	} else if ([[pboard types] containsObject: NSStringPboardType]) {
		NSString *nsstring = [pboard stringForType: NSStringPboardType];
		if (!nsstring) return NSDragOperationNone;
		pthread_mutex_lock (&gLangMutex);
			VMGlobals *g = gMainVMGlobals;
			PyrString* pstrobj = newPyrString(g->gc, [nsstring UTF8String], 0, true);
			int classVarIndex = slotRawInt(&getsym("SCView")->u.classobj->classVarIndex);
			SetObject(&g->classvars->slots[classVarIndex+0], pstrobj); // currentDrag
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

    NSPoint mouseLoc = [self convertPoint:[dragInfo draggingLocation] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
	if(!view && mTopView->ConstructionMode())
		view = mTopView;
    if (view) {
        bool flag = view->canReceiveDrag();
		view->draggingEntered(scpoint );
        mTopView->setDragView(flag ? view : 0);
        [self displayIfNeeded];
        return flag ? NSDragOperationEvery : NSDragOperationNone;
    } else {
        mTopView->setDragView(0);
        [self displayIfNeeded];
        return NSDragOperationNone;
    }
}

- (unsigned int)draggingUpdated:(id)dragInfo {
	if (!mTopView) return NSDragOperationNone;
    NSPoint mouseLoc = [self convertPoint:[dragInfo draggingLocation] fromView:nil];

    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
	if(!view && mTopView->ConstructionMode())
		view = mTopView;
    if (view) {
        bool flag = view->canReceiveDrag();
        mTopView->setDragView(flag ? view : 0);
		view->draggingUpdated(scpoint);
        [self displayIfNeeded];
        return flag ? NSDragOperationEvery : NSDragOperationNone;
    } else {
        mTopView->setDragView(0);
        [self displayIfNeeded];
        return NSDragOperationNone;
    }
}

- (void)draggingExited:(id)dragInfo {
	if (!mTopView) return;
    //NSPoint mouseLoc = [self convertPoint:[dragInfo draggingLocation] fromView:nil];
    //SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    mTopView->setDragView(0);
    [self displayIfNeeded];
}

- (BOOL)prepareForDragOperation:(id)dragInfo {
    //post("prepareForDragOperation %08X\n", self);
	if (!mTopView) return FALSE;
    NSPoint mouseLoc = [self convertPoint:[dragInfo draggingLocation] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
	if(!view && mTopView->ConstructionMode())
		view = mTopView;
    if (view) {
        bool flag = view->canReceiveDrag();
        mTopView->setDragView(flag ? view : 0);
        [self displayIfNeeded];
        return flag ? YES : NO;
    } else {
        mTopView->setDragView(0);
        [self displayIfNeeded];
        return NO;
    }
}

- (BOOL)performDragOperation:(id)dragInfo {
	if (!mTopView) return NO;
//    post("performDragOperation %08X\n", self);
    NSPoint mouseLoc = [self convertPoint:[dragInfo draggingLocation] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
	if(!view && mTopView->ConstructionMode())
		view = mTopView;
    if (view) {
        bool flag = view->canReceiveDrag();
        if (flag) {
            mTopView->setDragView(view);
            view->receiveDrag();
        } else {
            mTopView->setDragView(0);
        }
        [self displayIfNeeded];
        return flag ? YES : NO;
    } else {
        mTopView->setDragView(0);
        [self displayIfNeeded];
        return NO;
    }
}

- (void)concludeDragOperation:(id)sender {
    if (mTopView) mTopView->setDragView(0);
    [self displayIfNeeded];

    //post("concludeDragOperation %08X\n", self);
}

//static int ivxGUIScreen_frameNumber = 11;

- (void)drawRect: (NSRect)drawBounds
{
	if (mTopView) {

        SCRect screct;

		NSRect bounds = [self bounds];
		screct.x = bounds.origin.x;
		screct.y = bounds.origin.y;
		screct.width = bounds.size.width;
		screct.height = bounds.size.height;

		if(!NSEqualRects(bounds, oldBounds)){
			mTopView->setInternalBounds(screct);
			oldBounds = bounds;
		}


        screct.x = drawBounds.origin.x;
        screct.y = drawBounds.origin.y;
        screct.width = drawBounds.size.width;
        screct.height = drawBounds.size.height;

		CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
		CGContextSaveGState(cgc);
		//CGContextClipToRect(cgc, CGRectMake(screct.x, screct.y, screct.width, screct.height));
		CGContextClipToRect(cgc, *(CGRect*)&bounds);


        /*
		 Rect qdrect;
		 SetRect(&qdrect, (int)screct.x, (int)screct.y,
		 (int)(screct.x + screct.width), (int)(screct.y + screct.height));
		 ClipRect(&qdrect);
		 */

		const CGRect *dirtyRects;
		CGRect	curRect;
		NSInteger		numDirtyRects;

		[self getRectsBeingDrawn:(const NSRect**)&dirtyRects count:(NSInteger*)&numDirtyRects];

		int i=0;
		//post("numrects: %i\n", numDirtyRects);
		for(; i < numDirtyRects; ++i) {
			curRect = *(dirtyRects+i);

			screct.x = curRect.origin.x;
			screct.y = curRect.origin.y;
			screct.width = curRect.size.width;
			screct.height = curRect.size.height;
			//post("(%2.2f, %2.2f, %2.2f, %2.2f)\n", screct.x, screct.y, screct.width, screct.height);

			CGContextSaveGState(cgc);
			CGContextClipToRect(cgc, curRect);
			if(mTopView->isSubViewScroller())
			{
				((SCScrollView*)mTopView)->drawSubViewIfNecessary(screct);
			}
			else
			{
				mTopView->drawIfNecessary(screct);
			}
			CGContextRestoreGState(cgc);
		}

		CGContextRestoreGState(cgc);
    }

    pthread_mutex_lock (&gLangMutex);
    if (mWindowObj && NotNil(mWindowObj->slots+6)) {
        CGRect cgrect = *(CGRect*)&drawBounds;
        CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
        CGContextSaveGState(cgc);
        CGContextClipToRect(cgc, cgrect);

        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, mWindowObj); // push window obj
        runInterpreter(g, s_callDrawHook, 1);
        g->canCallOS = false;

        CGContextRestoreGState(cgc);
    }
    pthread_mutex_unlock (&gLangMutex);
}


NSDictionary *makeFontAttrDict(char *cFontName, float fontSize, SCColor sccolor)
{
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];

    NSString *fontName = [NSString stringWithCString: cFontName];

    NSFont *font = [NSFont fontWithName: fontName size: fontSize];
    if (!font) return 0;


    NSColor *nscolor = [NSColor colorWithCalibratedRed: sccolor.red
                            green: sccolor.green
                            blue: sccolor.blue
                            alpha: sccolor.alpha];
    [dict setObject: font forKey: NSFontAttributeName ];
    [dict setObject: nscolor forKey: NSForegroundColorAttributeName ];
    return dict;
}

int nsStringDrawInRect(NSString *nsstring, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;

    [nsstring drawInRect: SCtoNSRect(screct) withAttributes: dict];

    return errNone;
}

NSSize nsStringSize(NSString *nsstring, char *cFontName, float fontSize, SCColor sccolor)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
	return [nsstring sizeWithAttributes: dict];
}

int nsStringDrawInRectAlign(NSString *nsstring, SCRect screct, char *cFontName, float fontSize, SCColor sccolor,
	int hAlign, int vAlign, NSSize *outSize)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;

    NSSize size = [nsstring sizeWithAttributes: dict];
    if (outSize) *outSize = size;

    NSRect drawBounds = SCtoNSRect(screct);

    float hdiff = drawBounds.size.width - size.width;
    float vdiff = drawBounds.size.height - size.height;

	if (hAlign == 0) {
	    drawBounds.origin.x += hdiff * .5;
	} else if (hAlign > 0) {
 	   drawBounds.origin.x += hdiff;
	}

	if (vAlign == 0) {
	    drawBounds.origin.y += vdiff * .5;
	} else if (vAlign > 0) {
 	   drawBounds.origin.y += vdiff;
	}

    CGRect cgrect = SCtoCGRect(screct);
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGContextClipToRect(cgc, cgrect);

    [nsstring drawInRect: drawBounds withAttributes: dict];

    CGContextRestoreGState(cgc);
    return errNone;
}


int stringDrawInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRect(nsstring, screct, cFontName, fontSize, sccolor);
}

int stringDrawCenteredInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRectAlign(nsstring, screct, cFontName, fontSize, sccolor, 0, 0, NULL);
}

int stringDrawLeftInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRectAlign(nsstring, screct, cFontName, fontSize, sccolor, -1, 0, NULL);
}

int stringDrawRightInRect(const char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRectAlign(nsstring, screct, cFontName, fontSize, sccolor, 1, 0, NULL);
}

SCColor blendColor(float blend, SCColor a, SCColor b)
{
   SCColor c;
   c.red = a.red + blend * (b.red - a.red);
   c.green = a.green + blend * (b.green - a.green);
   c.blue = a.blue + blend * (b.blue - a.blue);
   c.alpha = a.alpha + blend * (b.alpha - a.alpha);
   return c;
}

void vPaintGradient(CGContextRef cgc, CGRect bounds, SCColor startColor, SCColor endColor, int numSteps)
{
    numSteps = (int)sc_min(numSteps, floor(bounds.size.height));
    float rNumSteps1 = 1. / (numSteps - 1.);

    CGRect rect;
    rect.origin.x    = bounds.origin.x;
    rect.size.width  = bounds.size.width;
    float step = bounds.size.height / numSteps;
    rect.size.height = ceil(step);

    for (int i=0; i<numSteps; ++i) {
        float blend = i * rNumSteps1;
        SCColor color = blendColor(blend, startColor, endColor);
        CGContextSetRGBFillColor(cgc, color.red, color.green, color.blue, color.alpha);

        rect.origin.y = bounds.origin.y + floor(i * step);
        rect.size.height = ceil(bounds.origin.y + (i + 1) * step) - rect.origin.y;

        CGContextFillRect(cgc, rect);
    }
}

void hPaintGradient(CGContextRef cgc, CGRect bounds, SCColor startColor, SCColor endColor, int numSteps)
{
    numSteps = (int)sc_min(numSteps, floor(bounds.size.width));
    float rNumSteps1 = 1. / (numSteps - 1.);

    CGRect rect;
    rect.origin.y    = bounds.origin.y;
    rect.size.height = bounds.size.height;
    float step = bounds.size.width / numSteps;
    rect.size.width = ceil(step);

    for (int i=0; i<numSteps; ++i) {
        float blend = i * rNumSteps1;
        SCColor color = blendColor(blend, startColor, endColor);
        CGContextSetRGBFillColor(cgc, color.red, color.green, color.blue, color.alpha);

        rect.origin.x = bounds.origin.x + floor(i * step);
        rect.size.width = ceil(bounds.origin.x + (i + 1) * step) - rect.origin.x;

        CGContextFillRect(cgc, rect);
    }
}

void QDDrawBevelRect(CGContextRef cgc, CGRect bounds, float width, bool inout)
{
    if (inout) {
        CGContextSetRGBFillColor(cgc, 0, 0, 0, 0.5);
    } else {
        CGContextSetRGBFillColor(cgc, 1, 1, 1, 0.5);
    }
    CGContextMoveToPoint(cgc, bounds.origin.x, bounds.origin.y);
    CGContextAddLineToPoint(cgc, bounds.origin.x + bounds.size.width, bounds.origin.y);
    CGContextAddLineToPoint(cgc, bounds.origin.x + bounds.size.width - width, bounds.origin.y + width);
    CGContextAddLineToPoint(cgc, bounds.origin.x + width, bounds.origin.y + width);
    CGContextAddLineToPoint(cgc, bounds.origin.x + width, bounds.origin.y + bounds.size.height - width);
    CGContextAddLineToPoint(cgc, bounds.origin.x, bounds.origin.y + bounds.size.height);
    CGContextAddLineToPoint(cgc, bounds.origin.x, bounds.origin.y);
    CGContextFillPath(cgc);

    if (inout) {
        CGContextSetRGBFillColor(cgc, 1, 1, 1, 0.5);
    } else {
        CGContextSetRGBFillColor(cgc, 0, 0, 0, 0.5);
    }
    CGContextMoveToPoint(cgc, bounds.origin.x + bounds.size.width, bounds.origin.y + bounds.size.height);
    CGContextAddLineToPoint(cgc, bounds.origin.x, bounds.origin.y + bounds.size.height);
    CGContextAddLineToPoint(cgc, bounds.origin.x + width, bounds.origin.y + bounds.size.height - width);
    CGContextAddLineToPoint(cgc,
        bounds.origin.x + bounds.size.width - width, bounds.origin.y + bounds.size.height - width);
    CGContextAddLineToPoint(cgc, bounds.origin.x + bounds.size.width - width, bounds.origin.y + width);
    CGContextAddLineToPoint(cgc, bounds.origin.x + bounds.size.width, bounds.origin.y);
    CGContextAddLineToPoint(cgc, bounds.origin.x + bounds.size.width, bounds.origin.y + bounds.size.height);
    CGContextFillPath(cgc);
}

- (void)startMenuTracking: (SCView*) inView
{
	mMenuView = inView;
}

- (IBAction) toggleUIEditMode: (id) sender;
{
//	if (!mTopView) return;
//	mTopView->SetConstructionMode(!mTopView->GetConstructionMode());
//	[self setNeedsDisplay: YES];

    VMGlobals *g = gMainVMGlobals;
	g->canCallOS = true;
	++g->sp;  SetObject(g->sp, mWindowObj); // push window obj
	runInterpreter(g, s_toggleEditMode, 1);
	g->canCallOS = false;
}

- (void)scrollViewResized:(NSNotification *)notification
{
	[self setFrameSizeToMinimum];

	// this seems to be needed for correct resize behaivour
	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
	SEL sel = @selector(display);
    NSMethodSignature *sig = [NSView instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    [anInvocation setTarget: [[self window] contentView]];
    [anInvocation setSelector: sel];
    [scvm defer: anInvocation];
}

extern PyrSymbol* s_doaction;

- (void)userScrolled:(NSNotification *)notification
{
	// if this happens from a visibleOrigin method we can't use sendMessage, so the action gets called from the lang
	// similarly, this blocks the action from being fired due to scrolling because of incidental resize (i.e. remove a child)
	if(!((SCScrollTopView*)mTopView)->isInSetClipViewOrigin()) {
		mTopView->sendMessage(s_doaction, 0, 0, 0); // this must be a scroll view
	}
}

- (void)setFrameSizeToMinimum
{
	NSScrollView* sv;
	if (sv = [self enclosingScrollView]){

		NSSize visSize = [[sv contentView] documentVisibleRect].size;

		NSSize candidate = SCtoNSRect(((SCScrollTopView*)mTopView)->checkMinimumSize()).size;
		if((candidate.width > visSize.width) || (candidate.height > visSize.height)){
			[self setFrameSize: candidate]; // set then check visible rect again to account for scroll bars that may have appeared or vanished
			visSize = [[sv contentView] visibleRect].size;
			[self setFrameSize: NSMakeSize(sc_max(candidate.width, visSize.width), sc_max(candidate.height, visSize.height))];
		} else {
			[self setFrameSize: visSize]; // always at least this big
		}

		[self setNeedsDisplay: YES];
		[sv setNeedsDisplay: YES];
	}

}

@end
