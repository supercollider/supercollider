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

#import "SCGraphView.h"
#import "SCVirtualMachine.h"
#include "PyrKernel.h"
#include "PyrMessage.h"
#include "VMGlobals.h"
#include "PyrSched.h"
#include "SC_BoundsMacros.h"
#include "GC.h"
#include <Carbon/Carbon.h>

extern PyrSymbol *s_draw;
extern PyrSymbol *s_scview;

@implementation SCGraphView

- (BOOL)isFlipped
{
	return YES;
}

- (BOOL)mouseDownCanMoveWindow
{
	return NO;
}

- (void) keyDown: (NSEvent*) event
{
    NSString *characters = [event characters];
    unsigned int modifiers = [event modifierFlags];
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }
    //control tab/escape doesn't get passed here at all ?
    //NSLog(@"unicode %d  length:%d clength:%d",character,[characters length],[characters cStringLength]);
    
    if (mTopView) {
        if(character == 9 && (modifiers == 0)) {
            mTopView->tabPrevFocus();
            return;
        } else if (character == 25 && (modifiers == NSShiftKeyMask)) {
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
    unichar character = 0;
    if([characters length] > 0) {
        character = [characters characterAtIndex: 0];
    }

    if (mTopView) {
        SCView *view = mTopView->focusView();
        if (view) {
            view->keyUp(character, [event modifierFlags],[event keyCode]);
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
    [self registerForDraggedTypes: [NSArray arrayWithObjects: sSCObjType, NSStringPboardType, nil]];
	mDragStarted = NO;
	mMenuView = 0;
	mWindowObj = 0;
	mTopView = 0;
    windowShouldClose = YES;
    return self;
}

- (NSMenu*) menuForEvent:(NSEvent*)theEvent;
{
    NSPoint mouseLoc;
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
    if (!view) return 0;
	return view->contextMenu(scpoint);
}

- (void)mouseDown:(NSEvent *)theEvent
{
    BOOL keepOn = YES;
    //BOOL isInside = YES;
    NSPoint mouseLoc;
    
	if (!mTopView) return;
    unsigned int modifiers = [theEvent modifierFlags];
    mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
    if (view) {
		mDragStarted = NO;
		mMenuView = 0;
        view->makeFocus(true);
        view->mouseBeginTrack(scpoint, modifiers);
        [self displayIfNeeded];
        
        while (keepOn && !mDragStarted && !mMenuView) {
            theEvent = [[self window] nextEventMatchingMask: NSLeftMouseUpMask |
                    NSLeftMouseDraggedMask];
            mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
            //isInside = [self mouse:mouseLoc inRect:[self bounds]];
            scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    
            int evtype = [theEvent type];
            switch ([theEvent type]) {
                case NSLeftMouseDragged:
                        view->mouseTrack(scpoint, modifiers);
                        break;
                case NSLeftMouseUp:
                        view->mouseEndTrack(scpoint, modifiers);
                        keepOn = NO;
                        break;
                default:
                    post("evtype %d %4.4s\n", evtype, (char*)&evtype);
                        /* Ignore any other kind of event. */
                        break;
            }
            [self displayIfNeeded];
            flushPostBuf();
        }
    }
	mMenuView = 0;
    return;
}

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

void dragFunc(SCPoint where, PyrSlot *inSlot, void* inData)
{
    SCGraphView *view = (SCGraphView*)inData;
    NSPoint point = NSMakePoint(where.x, where.y);
    [view beginDragFrom: point of: inSlot ];
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
}

/*- (void)dealloc
{
    //post("******graph view dealloc**********\n");
}
*/

- (void)closeWindow
{
    delete mTopView;
	mTopView = 0;
	[[self window] close];
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

- (NSImage*) makeDragImage: (PyrSlot*)slot
{

    if (!slot) return 0;
    
    NSString *nsstring;
    if (slot) {
        PyrClass *classobj = classOfSlot(slot);
        nsstring = [NSString stringWithCString: classobj->name.us->name];
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
    NSColor *colorTransLtBlue = [NSColor colorWithCalibratedRed: 0.8
                            green: 0.8 
                            blue: 1 
                            alpha: alpha];
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


- (void) beginDragFrom: (NSPoint)where of: (PyrSlot*)slot
{
    NSImage *image = [self makeDragImage: slot];
    
    NSPasteboard *pboard = [NSPasteboard pasteboardWithName: NSDragPboard];
    [pboard declareTypes: [NSArray arrayWithObject: sSCObjType] owner: self];
    
	int fakeData;
    NSData *data = [NSData dataWithBytes: &fakeData length: sizeof(int)];
    
    [pboard setData: data forType: sSCObjType];
    
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
			int classIndex = getsym("SCView")->u.classobj->classIndex.ui;
			PyrObject *classvars = g->classvars[classIndex].uo;
			SetObject(classvars->slots+0, pstrobj);
			g->gc->GCWrite(classvars, pstrobj);
			
			PyrSymbol *method = getsym("importDrag");
			//g->canCallOS = true;
			++g->sp;  SetObject(g->sp, s_scview->u.classobj); 
			runInterpreter(g, method, 1);
			//g->canCallOS = false;
			
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
    if (view) {
        bool flag = view->canReceiveDrag();
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
    if (view) {
        bool flag = view->canReceiveDrag();
        mTopView->setDragView(flag ? view : 0);
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
    //post("performDragOperation %08X\n", self);
    NSPoint mouseLoc = [self convertPoint:[dragInfo draggingLocation] fromView:nil];
    SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
    SCView *view = mTopView->findView(scpoint);
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
        NSRect bounds = [self bounds];
        SCRect screct;
        
        screct.x = bounds.origin.x;
        screct.y = bounds.origin.y;
        screct.width = bounds.size.width;
        screct.height = bounds.size.height;
        mTopView->setBounds(screct);
        
        screct.x = drawBounds.origin.x;
        screct.y = drawBounds.origin.y;
        screct.width = drawBounds.size.width;
        screct.height = drawBounds.size.height;
        
        /*CGRect cgrect = *(CGRect*)&drawBounds;
        CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
        CGContextSaveGState(cgc);
        CGContextClipToRect(cgc, cgrect);*/
        
        Rect qdrect;
        SetRect(&qdrect, (int)screct.x, (int)screct.y, 
            (int)(screct.x + screct.width), (int)(screct.y + screct.height));
        ClipRect(&qdrect);

        mTopView->drawIfNecessary(screct);

        //CGContextRestoreGState(cgc);
    }
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

int stringDrawInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;
    NSString *nsstring = [NSString stringWithCString: cString];
    
    [nsstring drawInRect: SCtoNSRect(screct) withAttributes: dict];

    return errNone;
}

int stringDrawCenteredInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;

    NSString *nsstring = [NSString stringWithCString: cString];
    
    NSSize size = [nsstring sizeWithAttributes: dict];
    
    NSRect drawBounds = SCtoNSRect(screct);

    float hdiff = drawBounds.size.width - size.width;
    float vdiff = drawBounds.size.height - size.height;
    
    drawBounds.origin.x += hdiff * .5;
    drawBounds.origin.y += vdiff * .5;
    drawBounds.size.width -= hdiff;
    drawBounds.size.height -= vdiff;
    
    CGRect cgrect = *(CGRect*)&screct;
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGContextClipToRect(cgc, cgrect);
    
    [nsstring drawInRect: drawBounds withAttributes: dict];
    
    CGContextRestoreGState(cgc);
    return errNone;
}

int stringDrawLeftInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;

    NSString *nsstring = [NSString stringWithCString: cString];
    
    NSSize size = [nsstring sizeWithAttributes: dict];
    
    NSRect drawBounds = SCtoNSRect(screct);

    //float hdiff = drawBounds.size.width - size.width;
    float vdiff = drawBounds.size.height - size.height;
    
    drawBounds.origin.y += vdiff * .5;
    drawBounds.size.width = size.width;
    drawBounds.size.height -= vdiff;
    
    CGRect cgrect = *(CGRect*)&screct;
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGContextClipToRect(cgc, cgrect);
    
    [nsstring drawInRect: drawBounds withAttributes: dict];
        
    CGContextRestoreGState(cgc);
    return errNone;
}

int stringDrawRightInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;

    NSString *nsstring = [NSString stringWithCString: cString];
    
    NSSize size = [nsstring sizeWithAttributes: dict];
    
    NSRect drawBounds = SCtoNSRect(screct);

    float hdiff = drawBounds.size.width - size.width;
    float vdiff = drawBounds.size.height - size.height;
    
    drawBounds.origin.x += hdiff;
    drawBounds.origin.y += vdiff * .5;
    drawBounds.size.width = size.width;
    drawBounds.size.height -= vdiff;
    
    CGRect cgrect = *(CGRect*)&screct;
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSaveGState(cgc);
    CGContextClipToRect(cgc, cgrect);
    
    [nsstring drawInRect: drawBounds withAttributes: dict];
    
    CGContextRestoreGState(cgc);
    return errNone;
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
	mTopView->SetConstructionMode(!mTopView->ConstructionMode());
	[self setNeedsDisplay: YES];
}

@end
