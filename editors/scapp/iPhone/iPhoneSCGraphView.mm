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

#import "iPhoneSCGraphView.h"
#import "iSCLangController.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"
#include "PyrMessage.h"
#include "VMGlobals.h"
#include "PyrSched.h"
#include "SC_BoundsMacros.h"
#include "GC.h"
#import <UIKit/UIKit.h>

extern PyrSymbol *s_draw;
extern PyrSymbol *s_scview;
extern PyrSymbol *s_closed;
extern PyrSymbol *s_callDrawFunc;
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
/*
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
	if(modifiers & NSCommandKeyMask) // allow cmd-key only
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
*/

static CGRect SCtoCGRect(SCRect screct)
{
    CGRect nsrect;
    nsrect.origin.x = screct.x;
    nsrect.origin.y = screct.y;
    nsrect.size.width = screct.width;
    nsrect.size.height = screct.height;
    return nsrect;
}

static NSString *sSCObjType = @"SuperCollider object address";

- (id)initWithFrame: (CGRect) frame
{
    [super initWithFrame: frame];
	//[self registerForDraggedTypes: [NSArray arrayWithObjects: sSCObjType, NSStringPboardType, nil]];
	mDragStarted = NO;
	mMenuView = 0;
	mWindowObj = 0;
	mTopView = 0;
    windowShouldClose = YES;
	acceptsClickThrough = YES;
	autoScrolls = YES;
	[self setBackgroundColor:[UIColor whiteColor]];
    return self;
}

- (void) touch:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UITouch *touch in touches)
	{	
		CGPoint mouseLoc;
		if (!mTopView) return;
		mouseLoc = [touch locationInView:self];
		SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
		SCView *view = mTopView->findView(scpoint);
		if (view)
		{
			mDragStarted = NO;
			mMenuView = 0;
			view->makeFocus(true);
			bool constructionmode = mTopView->ConstructionMode();
			if(!constructionmode)
			{
				UITouchPhase phase = [touch phase];
				if (phase==UITouchPhaseBegan)
				{
					view->touchDownAction(scpoint, touch);
					view->touchBeginTrack(scpoint, touch);
				}
				else if (phase==UITouchPhaseMoved)
				{
					view->touchTrack(scpoint, touch);
					view->touchMoveAction(scpoint, touch);
				}
				else if (phase==UITouchPhaseEnded)
				{
					view->touchUpAction(scpoint, touch);
					view->touchEndTrack(scpoint, touch);			
				}
			}
		}
	}	
		
		
		
		
		//else view->setConstructionModeFromPoint(scpoint);

		//[self displayIfNeeded];
 /*       
        while (keepOn && !mDragStarted && !mMenuView) {
            theEvent = [[self window] nextEventMatchingMask: NSLeftMouseUpMask |NSRightMouseUp | NSOtherMouseUp |
                    NSLeftMouseDraggedMask | NSRightMouseDragged | NSOtherMouseDragged 
					| NSKeyDownMask | NSKeyUpMask
			];
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
               default:
                    post("evtype %d %4.4s\n", evtype, (char*)&evtype);
                        break;
            }
//			display:
            //[self displayIfNeeded];
            flushPostBuf();
        }
*/		
	mMenuView = 0;
    return;
}

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	[self touch:touches withEvent:event];
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
{
	[self touch:touches withEvent:event];
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
{
	[self touch:touches withEvent:event];
}

- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
{
	printf("touches cancelled\n");
}



/*
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
//	NSLog(@"SCGraphView MOUSEDOWN");
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
*/
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
    
    [view setNeedsDisplayInRect: SCtoCGRect(inRect)];
}

void dragFunc(SCPoint where, PyrSlot *inSlot, NSString* inString, NSString* label, void* inData)
{
    SCGraphView *view = (SCGraphView*)inData;
    CGPoint point = CGPointMake(where.x, where.y);
    [view beginDragFrom: point of: inSlot string: inString label: label];
}


/*
- (unsigned int)draggingSourceOperationMaskForLocal: (BOOL)flag
{
    return flag ? NSDragOperationEvery : NSDragOperationNone;
}
*/

- (void)setSCTopView: (SCTopView*)inView
{
    mTopView = inView;
    mTopView->setDamageCallback(damageFunc, (void*)self);
    mTopView->setDragCallback(dragFunc);
	mTopView->SetUIView(self);
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
	SCNSWindow *w = [self superview];
	[w close];
}

- (void)removeFromSuperview
{
	[[iSCLangController sharedInstance] removeDeferredOperationsFor: self];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super removeFromSuperview];
}

- (void)willClose
{    
	[[iSCLangController sharedInstance] removeDeferredOperationsFor: self];
	[[iSCLangController sharedInstance] removeDeferredOperationsFor: [self window]];
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

//static int ivxGUIScreen_frameNumber = 11;

- (void)drawRect: (CGRect)drawBounds
{
	if (mTopView) {
        
        SCRect screct;
        
		CGRect bounds = [self bounds];
		screct.x = bounds.origin.x;
		screct.y = bounds.origin.y;
		screct.width = bounds.size.width;
		screct.height = bounds.size.height;
		mTopView->setInternalBounds(screct);
        
        screct.x = drawBounds.origin.x;
        screct.y = drawBounds.origin.y;
        screct.width = drawBounds.size.width;
        screct.height = drawBounds.size.height;

/*                
        Rect qdrect = {(int)screct.x, (int)screct.y, 
            (int)(screct.x + screct.width), (int)(screct.y + screct.height)};
        ClipRect(&qdrect);
*/		
		if(mTopView->isSubViewScroller()) {
			((SCScrollView*)mTopView)->drawSubViewIfNecessary(screct);
		} else {
			mTopView->drawIfNecessary(screct);
		} 
	   }
    pthread_mutex_lock (&gLangMutex);
    if (mWindowObj && NotNil(mWindowObj->slots+6)) {
        CGRect cgrect = *(CGRect*)&drawBounds;
        CGContextRef cgc = (CGContextRef)UIGraphicsGetCurrentContext();
        CGContextSaveGState(cgc);
        CGContextClipToRect(cgc, cgrect);

        VMGlobals *g = gMainVMGlobals;
        g->canCallOS = true;
        ++g->sp;  SetObject(g->sp, mWindowObj); // push window obj
        runInterpreter(g, s_callDrawFunc, 1);
        g->canCallOS = false;

        CGContextRestoreGState(cgc);
    }
    pthread_mutex_unlock (&gLangMutex);
}

/*
NSDictionary *makeFontAttrDict(char *cFontName, float fontSize, SCColor sccolor)
{
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];

    NSString *fontName = [NSString stringWithCString: cFontName];

    UIFont *font = [UIFont fontWithName: fontName size: fontSize];
    if (!font) return 0;


    UIColor *nscolor = [UIColor colorWithRed: sccolor.red
                            green: sccolor.green 
                            blue: sccolor.blue 
                            alpha: sccolor.alpha];    
	[dict setObject: font forKey: NSFontAttributeName ];
	[dict setObject: nscolor forKey: NSForegroundColorAttributeName ];
    return dict;
}
*/

int nsStringDrawInRect(NSString *nsstring, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
/* 
	NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;
    
    [nsstring drawInRect: NSRectToCGRect(SCtoNSRect(screct)) withAttributes: dict];
*/
	int len = [nsstring length];
	char *buf = (char *) malloc(len+1);
	[nsstring getCString:buf maxLength:len+1 encoding:NSASCIIStringEncoding];
	
    CGRect drawBounds = SCtoCGRect(screct);

    CGRect cgrect = *(CGRect*)&screct;
    CGContextRef cgc = (CGContextRef)UIGraphicsGetCurrentContext();
    CGContextSaveGState(cgc);
    CGContextClipToRect(cgc, cgrect);
 
	CGContextSelectFont(cgc, cFontName, fontSize, kCGEncodingMacRoman);
	CGContextSetTextDrawingMode(cgc, kCGTextFill);
	CGContextSetRGBFillColor(cgc, sccolor.red, sccolor.green, sccolor.blue, sccolor.alpha);
	CGContextSetTextMatrix(cgc, CGAffineTransformMakeScale(1, -1));
	CGContextShowTextAtPoint(cgc, drawBounds.origin.x, drawBounds.origin.y+drawBounds.size.height, buf, len);
 
    CGContextRestoreGState(cgc);

	free(buf);
    return errNone;
}

CGSize nsStringSize(NSString *nsstring, char *cFontName, float fontSize, SCColor sccolor)
{
/*
    NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
	return [nsstring sizeWithAttributes: dict];
*/
	return CGSizeMake(0,0);
}

int nsStringDrawInRectAlign(NSString *nsstring, SCRect screct, char *cFontName, float fontSize, SCColor sccolor, 
	int hAlign, int vAlign, CGSize *outSize)
{
	return nsStringDrawInRect(nsstring, screct, cFontName, fontSize, sccolor);
/*
	NSDictionary* dict = makeFontAttrDict(cFontName, fontSize, sccolor);
    if (!dict) return errFailed;
    
    NSSize size = [nsstring sizeWithAttributes: dict];
    if (outSize) *outSize = size;


    UIFont *font = [UIFont fontWithName: fontName size: fontSize];
	
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
    
    CGRect cgrect = *(CGRect*)&screct;
    CGContextRef cgc = (CGContextRef)UIGraphicsGetCurrentContext();
    CGContextSaveGState(cgc);
    CGContextClipToRect(cgc, cgrect);
    
    //[nsstring drawInRect: drawBounds withAttributes: dict];
    
    CGContextRestoreGState(cgc);

    return errNone;
*/
}


int stringDrawInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRect(nsstring, screct, cFontName, fontSize, sccolor);
}

int stringDrawCenteredInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRectAlign(nsstring, screct, cFontName, fontSize, sccolor, 0, 0, NULL);
}

int stringDrawLeftInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
{
	NSString *nsstring = [NSString stringWithCString: cString];
	return nsStringDrawInRectAlign(nsstring, screct, cFontName, fontSize, sccolor, -1, 0, NULL);
}

int stringDrawRightInRect(char *cString, SCRect screct, char *cFontName, float fontSize, SCColor sccolor)
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
/*
	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
	SEL sel = @selector(display);
    NSMethodSignature *sig = [NSView instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    [anInvocation setTarget: [[self window] contentView]];
    [anInvocation setSelector: sel];     
    [scvm defer: anInvocation];
*/
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
/*
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
*/
@end
