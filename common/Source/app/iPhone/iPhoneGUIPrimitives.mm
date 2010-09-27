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

#import <UIKit/UIKit.h>
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "SC_RGen.h"

#import "iPhoneSCGraphView.h"
#import "ChangeCounter.h"
#import "GC.h"
#import "iPhoneSCNSWindow.h"
#import "iSCLangController.h"
//#import "SCTextView.h"
//#import "SCCocoaView.h"

extern ChangeCounter gUIChangeCounter;
//extern NSTextView* gPostView;
PyrSymbol *s_draw;
PyrSymbol *s_font;
PyrSymbol *s_closed;
PyrSymbol *s_tick;
PyrSymbol *s_doaction;
PyrSymbol *s_didBecomeKey;
PyrSymbol *s_didResignKey;

extern bool docCreatedFromLang;
int slotColorVal(PyrSlot *slot, SCColor *sccolor);


int slotGetCGRect(PyrSlot* a, CGRect *r)
{
	PyrSlot *slots = slotRawObject(a)->slots;
        int err;
	err = slotFloatVal(slots+0, &r->origin.x);
	if (err) return err;
	err = slotFloatVal(slots+1, &r->origin.y);
	if (err) return err;
	err = slotFloatVal(slots+2, &r->size.width);
	if (err) return err;
	err = slotFloatVal(slots+3, &r->size.height);
	if (err) return err;

        return errNone;
}

int slotGetQDRect(PyrSlot* a, Rect *r)
{
	PyrSlot *slots = slotRawObject(a)->slots;
	int err;
        float x, y, width, height;

	err = slotFloatVal(slots+0, &x);
	if (err) return err;
	err = slotFloatVal(slots+1, &y);
	if (err) return err;
	err = slotFloatVal(slots+2, &width);
	if (err) return err;
	err = slotFloatVal(slots+3, &height);
	if (err) return err;

	r->left   = (int)x;
	r->right  = (int)(x + width);
	r->top    = (int)(y - height);
	r->bottom = (int)y;

	return errNone;
}

int slotGetPoint(PyrSlot* a, CGPoint *p)
{
	PyrSlot *slots = slotRawObject(a)->slots;
        int err;
	err = slotFloatVal(slots+0, &p->x);
	if (err) return err;
	err = slotFloatVal(slots+1, &p->y);
	if (err) return err;

        return errNone;
}


int prSCWindow_New(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_New(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *args = g->sp - 6;
	PyrSlot *a = args + 0;
	PyrSlot *b = args + 1; // name
	PyrSlot *c = args + 2; // bounds
	PyrSlot *d = args + 3; // resizable
	PyrSlot *e = args + 4; // border
	PyrSlot *f = args + 6; // view
	PyrSlot *h = args + 5; // scroll
	PyrSlot *j = args + 7; // is this app modal? (Doesn't matter for sheets as they have no close button)

	if (!(isKindOfSlot(b, class_string))) return errWrongType;
	if (!(isKindOfSlot(c, s_rect->u.classobj))) return errWrongType;

	CGRect bounds;
	int err = slotGetCGRect(c, &bounds);
	if (err) return err;

	PyrString *string = slotRawString(b);
	NSString *title = [NSString stringWithCString: string->s length: string->size];

	SCNSWindow *window = [[SCNSWindow alloc] initWithFrame: bounds];
	[window setTitle: title];
	[window setHasBorders: YES];

	iSCLangController *controller = [iSCLangController sharedInstance];
	SCGraphView* view = [[SCGraphView alloc] initWithFrame: bounds];
	[view setSCObject: slotRawObject(a)];
	SetPtr(slotRawObject(a)->slots + 0, view);
	[controller insertWindow:window];
	[window setSCGraphView: view];

	if(IsTrue(h)) {
/*
		SCScrollTopView* scrollTopView = (SCScrollTopView*)slotRawInt(slotRawObject(f)->slots);
		[view setSCTopView: scrollTopView];

		NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame: bounds];
		[scrollView setHasVerticalScroller:YES];
		[scrollView setHasHorizontalScroller:YES];
		[scrollView setAutohidesScrollers:YES];
		[[scrollView horizontalScroller] setControlSize:NSSmallControlSize];
		[[scrollView verticalScroller] setControlSize:NSSmallControlSize];
		[[scrollView horizontalScroller] setControlTint:NSGraphiteControlTint];
		[[scrollView verticalScroller] setControlTint:NSGraphiteControlTint];

		[scrollView setBackgroundColor:[NSColor clearColor]];
		[scrollView setDrawsBackground:NO];
		// configure the scroller to have no visible border
		[scrollView setBorderType:NSNoBorder];
		[scrollView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
		[scrollView setDocumentView:view];

		[scrollView setPostsFrameChangedNotifications: YES]; // we need this to resize the SCGraphView if the scroll view exceeds its bounds
		[[NSNotificationCenter defaultCenter] addObserver:view
												 selector:@selector(scrollViewResized:)
													 name:@"NSViewFrameDidChangeNotification"
												   object:scrollView];

		NSClipView *contentView = [scrollView contentView];
		[contentView setPostsBoundsChangedNotifications:YES];
		[[NSNotificationCenter defaultCenter] addObserver:view
												 selector:@selector(userScrolled:)
													 name:@"NSViewBoundsDidChangeNotification"
												   object:contentView];

		scrollTopView->SetNSScrollView(scrollView);
		[view autorelease];

		[window setContentView: scrollView];
		[scrollView autorelease];
*/
	} else {
		[view setSCTopView: (SCTopView*)slotRawPtr(slotRawObject(f)->slots)];
		[window addSubview: view];
		[view autorelease];
	}
	//[window makeFirstResponder: view];
	//[window setFrameOrigin: bounds.origin];


	return errNone;
}


int prSCWindow_Refresh(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Refresh(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    SEL sel = @selector(setNeedsDisplay:);
    NSMethodSignature *sig = [UIView instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    iSCLangController* controller = [iSCLangController sharedInstance];
    [anInvocation setTarget: view];
    [anInvocation setSelector: sel];
    BOOL flag = YES;
    [anInvocation setArgument: &flag atIndex: 2];
    [controller defer: anInvocation];

    return errNone;
}

int prSCWindow_Close(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Close(struct VMGlobals *g, int numArgsPushed)
{
   if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    SCNSWindow *window = (SCNSWindow *) [view superview];

    SEL sel = @selector(closeWindow:);
    NSMethodSignature *sig = [iSCLangController instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    iSCLangController* controller = [iSCLangController sharedInstance];
    [anInvocation setTarget: controller];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &window atIndex: 2];
    [controller defer: anInvocation];

    return errNone;
/*
    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    SEL sel = @selector(closeWindow);
    NSMethodSignature *sig = [SCGraphView instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    iSCLangController* controller = [iSCLangController sharedInstance];
    [anInvocation setTarget: view];
    [anInvocation setSelector: sel];
    [controller defer: anInvocation];

    return errNone;
*/
}


int prSCWindow_ToFront(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_ToFront(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    SCNSWindow *window = (SCNSWindow *) [view superview];

    SEL sel = @selector(makeWindowFront:);
    NSMethodSignature *sig = [iSCLangController instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    iSCLangController* controller = [iSCLangController sharedInstance];
    [anInvocation setTarget: controller];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &window atIndex: 2];
    [controller defer: anInvocation];

    return errNone;
}

int prSCWindow_SetName(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_SetName(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, class_string))) return errWrongType;

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;
    PyrString *string = slotRawString(b);
    NSString *title = [NSString stringWithCString: string->s length: string->size];
    [[view window] setTitle: title];
    return errNone;
}


int prFont_AvailableFonts(struct VMGlobals *g, int numArgsPushed);
int prFont_AvailableFonts(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp;

	NSArray *fonts = [UIFont familyNames];

	int size = [fonts count];
	PyrObject* array = newPyrArray(g->gc, size, 0, true);
	SetObject(a, array);

	for (int i=0; i<size; ++i) {
		NSString *name = [fonts objectAtIndex: i];
		//if (!name) continue;
		PyrString *string = newPyrString(g->gc, [name UTF8String], 0, true);
		SetObject(array->slots + array->size, string);
		array->size++;
		g->gc->GCWrite(array, string);
	}

    return errNone;
}


void initGUIPrimitives()
{
	int base, index;

	s_draw = getsym("draw");
	s_font = getsym("SCFont");
	s_closed = getsym("closed");
	s_tick = getsym("tick");
	s_doaction = getsym("doAction");
	s_didBecomeKey = getsym("didBecomeKey");
	s_didResignKey = getsym("didResignKey");

	base = nextPrimitiveIndex();
	index = 0;

	definePrimitive(base, index++, "_SCWindow_New", prSCWindow_New, 7, 0);
	definePrimitive(base, index++, "_SCWindow_Refresh", prSCWindow_Refresh, 1, 0);
	definePrimitive(base, index++, "_SCWindow_Close", prSCWindow_Close, 1, 0);
	definePrimitive(base, index++, "_SCWindow_ToFront", prSCWindow_ToFront, 1, 0);
	definePrimitive(base, index++, "_SCWindow_SetName", prSCWindow_SetName, 2, 0);

	definePrimitive(base, index++, "_Font_AvailableFonts", prFont_AvailableFonts, 1, 0);
}

