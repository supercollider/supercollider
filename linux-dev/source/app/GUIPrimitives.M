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

#include <Cocoa/Cocoa.h>
#include <Carbon/Carbon.h>
#include "PyrPrimitive.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "VMGlobals.h"
#include "SC_RGen.h"
#import "MyDocument.h"
#import "SCGraphView.h"
#import "SCVirtualMachine.h"
#import "ChangeCounter.h"
#import "GC.h"

extern ChangeCounter gUIChangeCounter;
extern NSTextView* gPostView;
PyrSymbol *s_draw;
PyrSymbol *s_font;
PyrSymbol *s_closed;
PyrSymbol *s_tick;
PyrSymbol *s_doaction;

int prNewTextWindow(struct VMGlobals *g, int numArgsPushed);
int prNewTextWindow(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
        PyrString* string = a->uos;
                        
        NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) return errFailed;
        [docctl newDocument: nil];
        MyDocument* doc = [docctl currentDocument];
        if (!doc) return errFailed;
        
        [doc insertText: string->s length: string->size];
        NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
        if (!window) return errFailed;
        
        if (isKindOfSlot(b, class_string)) {
            PyrString* title = b->uos;
            NSString *nstitle = [NSString stringWithCString: title->s length: title->size];
            [window setTitle: nstitle];
        }
        [[doc undoManager] removeAllActions];
        
        if (IsTrue(c)) {
            gPostView = [doc textView];
        }
        
	return errNone;
}

int prOpenTextFile(struct VMGlobals *g, int numArgsPushed);
int prOpenTextFile(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
        PyrString* string = a->uos;
        int rangeStart, rangeSize;
        int err = slotIntVal(b, &rangeStart);
        if (err) return err;
        err = slotIntVal(c, &rangeSize);
        if (err) return err;
        
        NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) {
            post("No NSDocumentController\n");
            return errFailed;
        }
        
        NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
        NSURL *url = [NSURL fileURLWithPath: nsstring];
        NSString *nspath = [url path];
        
        MyDocument *doc = (MyDocument*)[docctl documentForFileName: nspath];
        if (!doc) {
            doc = [docctl openDocumentWithContentsOfFile: nspath display: true];
            if (!doc) {
                post("Can't open Document '%s'\n", [nspath cString]);
                return errFailed;
            }

        }
        NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
        if (!window) {
            post("window controller returns nil\n");
            return errFailed;
        }
        [window makeKeyAndOrderFront: nil];
        
        // select 
        [doc selectRangeStart: rangeStart size: rangeSize];
        /*
        NSTextView *textView = [doc textView];
        
        NSRange range = NSMakeRange(rangeStart, rangeSize);
        
        if (rangeSize < 0) {
            unsigned int lineStart, lineEnd;
            range.length = 0;
            NSString *nsstring = [textView string];
            if (!nsstring) {
                post("text view has no string\n");
                return errFailed;
            }
            [nsstring getLineStart: &lineStart end: &lineEnd contentsEnd: nil forRange: range];
            range = NSMakeRange(lineStart, lineEnd - lineStart);
        }
        [textView setSelectedRange: range];
        [textView scrollRangeToVisible: range];
        */
	return errNone;
}

int prGetStringFromUser(struct VMGlobals *g, int numArgsPushed);
int prGetStringFromUser(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	//PyrSlot *a = g->sp - 2;
	//PyrSlot *b = g->sp - 1;
	//PyrSlot *c = g->sp;
                                 
	return errNone;
}


int slotGetNSRect(PyrSlot* a, NSRect *r)
{
	PyrSlot *slots = a->uo->slots;
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


int slotGetCGRect(PyrSlot* a, CGRect *r)
{
	PyrSlot *slots = a->uo->slots;
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
	PyrSlot *slots = a->uo->slots;
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

int slotGetPoint(PyrSlot* a, NSPoint *p)
{
	PyrSlot *slots = a->uo->slots;
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
	
        PyrSlot *args = g->sp - 5;
	PyrSlot *a = args + 0;
	PyrSlot *b = args + 1;
	PyrSlot *c = args + 2;
	PyrSlot *d = args + 3;
	PyrSlot *e = args + 4;
	PyrSlot *f = args + 5;

	if (!(isKindOfSlot(b, class_string))) return errWrongType;
	if (!(isKindOfSlot(c, s_rect->u.classobj))) return errWrongType;

        unsigned int mask = IsTrue(d) && IsTrue(e) ? NSResizableWindowMask : 0;
        mask |= IsTrue(e) ? 
            NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask 
			| NSTexturedBackgroundWindowMask
            : NSBorderlessWindowMask;
            
	NSRect bounds;
	int err = slotGetNSRect(c, &bounds);		
	if (err) return err;
              
	PyrString *string = b->uos;
        NSString *title = [NSString stringWithCString: string->s length: string->size];

        NSWindow *window = [[NSWindow alloc] 
            initWithContentRect: bounds
            styleMask: mask
            backing: NSBackingStoreBuffered
            defer: NO];
        [window setTitle: title];
    
        SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
        SCGraphView* view = [[SCGraphView alloc] initWithFrame: bounds];
        [view setSCObject: a->uo];
        [view setSCTopView: (SCTopView*)(f->uo->slots[0].ui)];
        SetPtr(a->uo->slots + 0, view);
        
        [window setDelegate: scvm];
        [window setContentView: view];
        [view autorelease];
/*
        NSRect matrect = NSMakeRect(40,40,400,400);
        NSSize cellsize = NSMakeSize(200,100);
        id mx = [[NSMatrix alloc] 
                    initWithFrame: matrect 
                    mode: NSTrackModeMatrix 
                    cellClass: [NSSliderCell class]
                    numberOfRows: 4 
                    numberOfColumns: 2 ];
        [mx setCellSize: cellsize];
        [view addSubview: mx];

*/
        [window makeFirstResponder: view];
        [window setFrameOrigin: bounds.origin];
        [scvm addWindow: window];

/*    
    id sender = nil;

    SEL sel = @selector(makeKeyAndOrderFront:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &sender atIndex: 2];       
    [scvm defer: anInvocation];
*/
/*
        SEL sel = @selector(newGuiWindowTitle:bounds:SCObject:styleMask:);
        NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];
        
        NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
        SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
        [anInvocation setTarget: scvm];
        [anInvocation setSelector: sel];
        [anInvocation setArgument: &title atIndex: 2];       
        [anInvocation setArgument: &bounds atIndex: 3];       
        [anInvocation setArgument: &a->uo atIndex: 4];       
        [anInvocation setArgument: &flags atIndex: 5];       
        [scvm defer: anInvocation];
*/                
	return errNone;
}

int prSCWindow_Refresh(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Refresh(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;
    
    SEL sel = @selector(setNeedsDisplay:);
    NSMethodSignature *sig = [NSView instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: view];
    [anInvocation setSelector: sel];
    BOOL flag = YES;
    [anInvocation setArgument: &flag atIndex: 2];       
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_Close(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Close(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;
    //[[view window] close];

   // NSWindow *window = [view window];
    
    SEL sel = @selector(closeWindow);
    NSMethodSignature *sig = [SCGraphView instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: view];
    [anInvocation setSelector: sel];
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_Minimize(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Minimize(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;
    //[[view window] miniaturize: nil];

    NSWindow *window = [view window];
    id sender = nil;
    
    SEL sel = @selector(miniaturize:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &sender atIndex: 2];       
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_ToFront(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_ToFront(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;
    //[[view window] makeKeyAndOrderFront: nil];

    NSWindow *window = [view window];
    id sender = nil;
    
    SEL sel = @selector(makeKeyAndOrderFront:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &sender atIndex: 2];
    [scvm defer: anInvocation];

    return errNone;
}


int prSCWindow_FullScreen(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_FullScreen(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    NSWindow *window = [view window];
    
    SEL sel = @selector(becomeFullScreen:);
    NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: scvm];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &window atIndex: 2];       
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_EndFullScreen(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_EndFullScreen(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    NSWindow *window = [view window];
    
    SEL sel = @selector(endFullScreen:);
    NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: scvm];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &window atIndex: 2];       
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_SetShouldClose(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_SetShouldClose(struct VMGlobals *g, int numArgsPushed)
{
    PyrSlot *a = g->sp - 1;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;
    PyrSlot *boo = g->sp;

    // no need to defer, right ?
    [view setWindowShouldClose: IsTrue(boo)];

    return errNone;
}

int prSCWindow_SetName(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_SetName(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, class_string))) return errWrongType;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;
    PyrString *string = b->uos;
    NSString *title = [NSString stringWithCString: string->s length: string->size];
    [[view window] setTitle: title];
    return errNone;
}

int prSCWindow_SetBounds(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_SetBounds(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect bounds;
    int err = slotGetNSRect(b, &bounds);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    NSWindow *window = [view window];
    //[window setFrame: bounds display: YES];

    SEL sel = @selector(setFrame:display:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    BOOL flag = YES;
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &bounds atIndex: 2];       
    [anInvocation setArgument: &flag atIndex: 3];       
    [scvm defer: anInvocation];
   
    return errNone;
}

int prSCWindow_GetBounds(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_GetBounds(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    NSRect bounds = [[view window] frame];
    PyrSlot *slots = b->uo->slots;
    SetFloat(slots+0, bounds.origin.x);
    SetFloat(slots+1, bounds.origin.y);
    SetFloat(slots+2, bounds.size.width);
    SetFloat(slots+3, bounds.size.height);
    a->ucopy = b->ucopy;
    
    return errNone;
}

int prSCWindow_GetScreenBounds(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_GetScreenBounds(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect bounds = [[NSScreen mainScreen] frame];
    PyrSlot *slots = b->uo->slots;
    SetFloat(slots+0, bounds.origin.x);
    SetFloat(slots+1, bounds.origin.y);
    SetFloat(slots+2, bounds.size.width);
    SetFloat(slots+3, bounds.size.height);
    a->ucopy = b->ucopy;
    
    return errNone;
}




//doesn't work as intended
int prSCWindow_Show(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Show(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;

    NSWindow *window = [view window];
    NSRect frame = [window frame];

    SEL sel = @selector(setFrame:display:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    BOOL flag = YES;
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &frame atIndex: 2];       
    [anInvocation setArgument: &flag atIndex: 3];       
    [scvm defer: anInvocation];

    return errNone;
}
//doesn't work as intended
int prSCWindow_Hide(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Hide(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    NSWindow *window = [view window];
    NSRect frame = [window frame];

    SEL sel = @selector(setFrame:display:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    BOOL flag = NO;
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &frame atIndex: 2];       
    [anInvocation setArgument: &flag atIndex: 3];       
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_SetAlpha(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_SetAlpha(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    float alpha;
    int err = slotFloatVal(b, &alpha);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    //[[view window] setAlphaValue: alpha];
    NSWindow *window = [view window];

    SEL sel = @selector(setAlphaValue:);
    NSMethodSignature *sig = [NSWindow instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: window];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &alpha atIndex: 2];       
    [scvm defer: anInvocation];
    
    return errNone;
}

int addFontAttribute(NSMutableDictionary *dict, PyrSlot *fontSlot);
int addFontAttribute(NSMutableDictionary *dict, PyrSlot *fontSlot)
{
    if (IsNil(fontSlot)) return errNone; // use default font
    if (!(isKindOfSlot(fontSlot, s_font->u.classobj))) return errWrongType;

    PyrSlot *nameSlot = fontSlot->uo->slots+0;
    PyrSlot *sizeSlot = fontSlot->uo->slots+1;
    
    if (!(isKindOfSlot(nameSlot, class_string))) return errWrongType;
    
    float size;
    int err = slotFloatVal(sizeSlot, &size);
    if (err) return err;

    PyrString *string = nameSlot->uos;
    NSString *fontName = [NSString stringWithCString: string->s length: string->size];
    if (!fontName) return errFailed;
    NSFont *font = [NSFont fontWithName: fontName size: size];
    if (!font) return errFailed;
    [dict setObject: font forKey: NSFontAttributeName ];
    
    return errNone;
}

int slotColorVal(PyrSlot *slot, SCColor *sccolor);


int slotRGBColorVal(PyrSlot *slot, RGBColor *rgbcolor)
{
    if (!(isKindOfSlot(slot, s_color->u.classobj))) return errWrongType;

    PyrSlot *slots = slot->uo->slots;
        
    int err;
    float red, green, blue, alpha;
    err = slotFloatVal(slots+0, &red);
    if (err) return err;
    err = slotFloatVal(slots+1, &green);
    if (err) return err;
    err = slotFloatVal(slots+2, &blue);
    if (err) return err;
    err = slotFloatVal(slots+3, &alpha);
    if (err) return err;
    
    rgbcolor->red = (unsigned short)(red * 65535.);
    rgbcolor->green = (unsigned short)(green * 65535.);
    rgbcolor->blue = (unsigned short)(blue * 65535.);
    //rgbcolor->alpha = (unsigned short)(alpha * 65535.);
    
    return errNone;
}

int addFontColorAttribute(NSMutableDictionary *dict, PyrSlot *colorSlot);
int addFontColorAttribute(NSMutableDictionary *dict, PyrSlot *colorSlot)
{
    if (IsNil(colorSlot)) return errNone; // use default color
    SCColor rgb;
    int err = slotColorVal(colorSlot, &rgb);
    if (err) return err;
    NSColor *color = [NSColor colorWithCalibratedRed: rgb.red
                            green: rgb.green 
                            blue: rgb.blue 
                            alpha: rgb.alpha];    
    [dict setObject: color forKey: NSForegroundColorAttributeName ];
    
    return errNone;
}

int prFont_AvailableFonts(struct VMGlobals *g, int numArgsPushed);
int prFont_AvailableFonts(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp;
	
	NSFontManager *fontManager = [NSFontManager sharedFontManager];
	NSArray *fonts = [fontManager availableFonts];

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


int prString_GetBounds(struct VMGlobals *g, int numArgsPushed);
int prString_GetBounds(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *a = g->sp - 2;
    PyrSlot *b = g->sp - 1;
    PyrSlot *c = g->sp;
    
    if (!isKindOfSlot(b, s_rect->u.classobj)) return errWrongType;

    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    int err = addFontAttribute(dict, c);
    if (err) return err;
    
    PyrString* string = a->uos;
    NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
    if (!nsstring) return errFailed;
    NSSize size = [nsstring sizeWithAttributes: dict];
    
    PyrSlot* rectSlots = b->uo->slots;
    SetFloat(rectSlots+0, 0.);
    SetFloat(rectSlots+1, 0.);
    SetFloat(rectSlots+2, size.width);
    SetFloat(rectSlots+3, size.height);
    a->ucopy = b->ucopy;
    
    return errNone;
}

int prString_DrawAtPoint(struct VMGlobals *g, int numArgsPushed);
int prString_DrawAtPoint(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *a = g->sp - 3;
    PyrSlot *b = g->sp - 2;
    PyrSlot *c = g->sp - 1;
    PyrSlot *d = g->sp;

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

    NSPoint p;
    int err = slotGetPoint(b, &p);		
    if (err) return err;

    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    err = addFontAttribute(dict, c);
    if (err) return err;
    err = addFontColorAttribute(dict, d);
    if (err) return err;
    
    PyrString* string = a->uos;
    NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
    if (!nsstring) return errFailed;

    [nsstring drawAtPoint: p withAttributes: dict];

    return errNone;
}

int prString_DrawInRect(struct VMGlobals *g, int numArgsPushed);
int prString_DrawInRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *a = g->sp - 3;
    PyrSlot *b = g->sp - 2;
    PyrSlot *c = g->sp - 1;
    PyrSlot *d = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect rect;
    int err = slotGetNSRect(b, &rect);		
    if (err) return err;

    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    err = addFontAttribute(dict, c);
    if (err) return err;
    err = addFontColorAttribute(dict, d);
    if (err) return err;

    PyrString* string = a->uos;
    NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
    if (!nsstring) return errFailed;

    [nsstring drawInRect: rect withAttributes: dict];

    return errNone;
}

int prColor_SetStroke(struct VMGlobals *g, int numArgsPushed);
int prColor_SetStroke(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *a = g->sp;
    PyrSlot *slots = a->uo->slots;
    int err;
    float red, green, blue, alpha;

    err = slotFloatVal(slots+0, &red);
    if (err) return err;
    err = slotFloatVal(slots+1, &green);
    if (err) return err;
    err = slotFloatVal(slots+2, &blue);
    if (err) return err;
    err = slotFloatVal(slots+3, &alpha);
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetRGBStrokeColor(cgc, red, green, blue, alpha);
    CGContextSetRGBFillColor(cgc, red, green, blue, alpha);
   /* NSColor *color = [NSColor colorWithCalibratedRed: red
                            green: green 
                            blue: blue 
                            alpha: alpha];
    if (color) {
        [color set];
    }*/
    
    return errNone;
}

int prColor_SetFill(struct VMGlobals *g, int numArgsPushed);
int prColor_SetFill(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *a = g->sp;
    PyrSlot *slots = a->uo->slots;
    int err;
    float red, green, blue, alpha;

    err = slotFloatVal(slots+0, &red);
    if (err) return err;
    err = slotFloatVal(slots+1, &green);
    if (err) return err;
    err = slotFloatVal(slots+2, &blue);
    if (err) return err;
    err = slotFloatVal(slots+3, &alpha);
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetRGBFillColor(cgc, red, green, blue, alpha);
   /* NSColor *color = [NSColor colorWithCalibratedRed: red
                            green: green 
                            blue: blue 
                            alpha: alpha];
    if (color) {
        [color set];
    }*/
    
    return errNone;
}

int prPen_Push(struct VMGlobals *g, int numArgsPushed);
int prPen_Push(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    [NSGraphicsContext saveGraphicsState];
    return errNone;
}

int prPen_Pop(struct VMGlobals *g, int numArgsPushed);
int prPen_Pop(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    [NSGraphicsContext restoreGraphicsState];
    return errNone;
}

int prPen_StrokeRect(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokeRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect rect;
    int err = slotGetNSRect(b, &rect);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    [NSBezierPath strokeRect: rect];
//    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    
    return errNone;
}

int prPen_FillRect(struct VMGlobals *g, int numArgsPushed);
int prPen_FillRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect rect;
    int err = slotGetNSRect(b, &rect);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    [NSBezierPath fillRect: rect];
    
    return errNone;
}

int prPen_StrokeOval(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokeOval(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect rect;
    int err = slotGetNSRect(b, &rect);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    [[NSBezierPath bezierPathWithOvalInRect: rect] stroke];
    
    return errNone;
}

int prPen_FillOval(struct VMGlobals *g, int numArgsPushed);
int prPen_FillOval(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect rect;
    int err = slotGetNSRect(b, &rect);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    [[NSBezierPath bezierPathWithOvalInRect: rect] fill];
    
    return errNone;
}

int prPen_StrokeLine(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokeLine(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 2;
    PyrSlot *b = g->sp - 1;
    PyrSlot *c = g->sp;

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;
    if (!(isKindOfSlot(c, s_point->u.classobj))) return errWrongType;

    NSPoint p1, p2;
    int err = slotGetPoint(b, &p1);		
    if (err) return err;
    err = slotGetPoint(c, &p2);		
    if (err) return err;
    
    SCGraphView* view = (SCGraphView*)a->uo->slots[0].ui;
    if (!view) return errNone;

    [NSBezierPath strokeLineFromPoint: p1 toPoint: p2];
    
    return errNone;
}

int prPen_MoveTo(struct VMGlobals *g, int numArgsPushed);
int prPen_MoveTo(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

    NSPoint p1;
    int err = slotGetPoint(b, &p1);		
    if (err) return err;
 
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextMoveToPoint(cgc, p1.x, p1.y);
    
    return errNone;
}

int prPen_LineTo(struct VMGlobals *g, int numArgsPushed);
int prPen_LineTo(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

    NSPoint p1;
    int err = slotGetPoint(b, &p1);		
    if (err) return err;
 
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextAddLineToPoint(cgc, p1.x, p1.y);
    
    return errNone;
}

int prPen_BeginPath(struct VMGlobals *g, int numArgsPushed);
int prPen_BeginPath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;
 
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextBeginPath(cgc);
    
    return errNone;
}

int prPen_StrokePath(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokePath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;
 
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawPath(cgc, kCGPathStroke);
    
    return errNone;
}

int prPen_FillPath(struct VMGlobals *g, int numArgsPushed);
int prPen_FillPath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;
 
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextFillPath(cgc);
    
    return errNone;
}



int prPen_Translate(struct VMGlobals *g, int numArgsPushed);
int prPen_Translate(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    //PyrSlot *a = g->sp - 2;
    PyrSlot *b = g->sp - 1;
    PyrSlot *c = g->sp;

    float x, y;
    int err = slotFloatVal(b, &x);		
    if (err) return err;
    err = slotFloatVal(c, &y);		
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextTranslateCTM(cgc, x, y);
    
    return errNone;
}

int prPen_Scale(struct VMGlobals *g, int numArgsPushed);
int prPen_Scale(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    //PyrSlot *a = g->sp - 2;
    PyrSlot *b = g->sp - 1;
    PyrSlot *c = g->sp;

    float x, y;
    int err = slotFloatVal(b, &x);		
    if (err) return err;
    err = slotFloatVal(c, &y);		
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextScaleCTM(cgc, x, y);
    
    return errNone;
}
    
int prPen_Rotate(struct VMGlobals *g, int numArgsPushed);
int prPen_Rotate(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    //PyrSlot *a = g->sp - 3;
    PyrSlot *b = g->sp - 2;
    PyrSlot *c = g->sp - 1;
    PyrSlot *d = g->sp;

    float angle, x, y;
    int err = slotFloatVal(b, &angle);		
    if (err) return err;
    err = slotFloatVal(c, &x);		
    if (err) return err;
    err = slotFloatVal(d, &y);		
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    if (x == 0. && y == 0.) {
        CGContextRotateCTM(cgc, angle);
    } else {
        CGContextTranslateCTM(cgc, x, y);
        CGContextRotateCTM(cgc, angle);
        CGContextTranslateCTM(cgc, -x, -y);
    }
    return errNone;
}

int prPen_SetWidth(struct VMGlobals *g, int numArgsPushed);
int prPen_SetWidth(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    float width;
    int err = slotFloatVal(b, &width);		
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetLineWidth(cgc, width);
    
    return errNone;
}

int prPen_SetMatrix(struct VMGlobals *g, int numArgsPushed);
int prPen_SetMatrix(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;
    
    if (!isKindOfSlot(b, class_arrayed_collection)) return errWrongType;
    
    if (b->uo->size < 6) return errFailed;
    PyrObject *mobj = b->uo;
    
    CGAffineTransform afx;
    
    int err = getIndexedFloat(mobj, 0, &afx.a);
    if (err) return err;
    err = getIndexedFloat(mobj, 1, &afx.b);
    if (err) return err;
    err = getIndexedFloat(mobj, 2, &afx.c);
    if (err) return err;
    err = getIndexedFloat(mobj, 3, &afx.d);
    if (err) return err;
    err = getIndexedFloat(mobj, 4, &afx.tx);
    if (err) return err;
    err = getIndexedFloat(mobj, 5, &afx.ty);
    if (err) return err;

    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextConcatCTM(cgc, afx);

    return errNone;
}

int prPen_DrawAquaButton(struct VMGlobals *g, int numArgsPushed);
int prPen_DrawAquaButton(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *args = g->sp - 4;
    
    if (!(isKindOfSlot(args+1, s_rect->u.classobj))) return errWrongType;

    Rect qdrect;
    int err = slotGetQDRect(args+1, &qdrect);		
    if (err) return err;
    
    ThemeButtonDrawInfo tinfo;
    tinfo.state = IsTrue(args+3) ? kThemeStatePressedDown : kThemeStatePressedUp ;
    tinfo.value = IsTrue(args+4) ? kThemeButtonOn : kThemeButtonOff ;
    tinfo.adornment = kThemeAdornmentDefault;
    
    DrawThemeButton(&qdrect, args[2].ui, &tinfo, nil, nil, nil, 0);
    
    return errNone;
}

int prLoadUserPanel(struct VMGlobals *g, int numArgsPushed);
int prLoadUserPanel(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;
    
    if (!isKindOfSlot(b, class_string)) return errWrongType;
    PyrString* string = b->uos;
    
    NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
    [nsstring retain];
    
    void *scobj = a->uo;    
    SEL sel = @selector(loadUserPanel:SCObject:);
    NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];
    
    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: scvm];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &nsstring atIndex: 2];       
    [anInvocation setArgument: &scobj atIndex: 3];       
    [scvm defer: anInvocation];
    
    return errNone;
}

int prHasDeferred(struct VMGlobals *g, int numArgsPushed);
int prHasDeferred(struct VMGlobals *g, int numArgsPushed)
{
    gUIChangeCounter.Change();
    return errNone;
}

int prDrawButtonState(struct VMGlobals *g, int numArgsPushed);
int prDrawButtonState(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
    
    PyrSlot *slots = g->sp - 5;
    
    PyrSlot *rectSlot = slots + 0;
    PyrSlot *strokeColorSlot = slots + 1;
    PyrSlot *fillColorSlot = slots + 2;
    PyrSlot *stringColorSlot = slots + 3;
    PyrSlot *fontSlot = slots + 4;
    PyrSlot *stringSlot = slots + 5;

    int err;

    CGRect rect;
    err = slotGetCGRect(rectSlot, &rect);		
    if (err) return err;
    
    CGContextRef cgc = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

    CGContextSaveGState(cgc);
    
    SCColor fillColor;
    err = slotColorVal(fillColorSlot, &fillColor);
    if (!err) {
        CGContextSetRGBFillColor(cgc, fillColor.red, fillColor.green, fillColor.blue, fillColor.alpha);
        CGContextFillRect(cgc, rect);
    }
    
    SCColor strokeColor;
    err = slotColorVal(strokeColorSlot, &strokeColor);
    if (!err) {
        CGContextSetRGBStrokeColor(cgc, 
            strokeColor.red, strokeColor.green, strokeColor.blue, strokeColor.alpha);
        CGContextStrokeRect(cgc, rect);
    }
    
    if (isKindOfSlot(stringSlot, class_string)) {
        NSMutableDictionary *dict = [NSMutableDictionary dictionary];
        err = addFontAttribute(dict, fontSlot);
        if (err) return err;
        err = addFontColorAttribute(dict, stringColorSlot);
        if (err) return err;
        
        PyrString *scstring = stringSlot->uos;
        NSString *nsstring = [NSString stringWithCString: scstring->s length: scstring->size];
        if (!nsstring) return errFailed;
        NSSize strsize = [nsstring sizeWithAttributes: dict];
            
        NSPoint p;
        p.x = rect.origin.x + rect.size.width/2 - strsize.width/2;
        p.y = rect.origin.y + rect.size.height/2 - strsize.height/2;
        
        [nsstring drawAtPoint: p withAttributes: dict];
    }
    
    CGContextRestoreGState(cgc);
    
    return errNone;
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////


int slotGetRoundRect(PyrSlot* a, Rect *r, int *radius);
int slotGetRoundRect(PyrSlot* a, Rect *r, int *radius)
{
	PyrSlot *slots = a->uo->slots;
        int err;
	err = slotGetQDRect(a, r);
	if (err) return err;

	err = slotIntVal(slots+4, radius);
	if (err) return err;
	
	return errNone;
}

int slotBevelRect(PyrSlot* a, Rect *r, int *width, bool *inout);
int slotBevelRect(PyrSlot* a, Rect *r, int *width, bool *inout)
{
	PyrSlot *slots = a->uo->slots;
        int err;
	err = slotGetQDRect(a, r);
	if (err) return err;

	err = slotIntVal(slots+4, width);
	if (err) return err;
	
	if (IsTrue(slots+5)) *inout = true;
	else *inout = false;
	
	return errNone;
}

int slotGetArc(PyrSlot* a, Rect *r, int *startAngle, int *arcAngle);
int slotGetArc(PyrSlot* a, Rect *r, int *startAngle, int *arcAngle)
{
	PyrSlot *slots = a->uo->slots;
        int err;
	err = slotGetQDRect(a, r);
	if (err) return err;

	err = slotIntVal(slots+4, startAngle);
	if (err) return err;
	err = slotIntVal(slots+5, arcAngle);
	if (err) return err;
	
	return errNone;
}
enum {
    drawop_Invert = -1,
    drawop_Erase,
    drawop_Stroke,
    drawop_Fill
};

int prRect_Draw(struct VMGlobals *g, int numArgsPushed);
int prRect_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
	Rect r;
	int err = slotGetQDRect(a, &r);
	if (err) return err;	
        
        RGBColor color;
        err = slotRGBColorVal(b, &color);
	if (err) return err;	
        RGBForeColor(&color);
        
        switch (c->ui) {
            case drawop_Invert : InvertRect(&r); break;
            case drawop_Erase : EraseRect(&r); break;
            case drawop_Stroke : FrameRect(&r); break;
            case drawop_Fill : PaintRect(&r); break;
        }
	return errNone;
}

int prOval_Draw(struct VMGlobals *g, int numArgsPushed);
int prOval_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	Rect r;
	int err = slotGetQDRect(a, &r);
	if (err) return err;	
        
        RGBColor color;
        err = slotRGBColorVal(b, &color);
	if (err) return err;	
        RGBForeColor(&color);
        
        switch (c->ui) {
            case drawop_Invert : InvertOval(&r); break;
            case drawop_Erase : EraseOval(&r); break;
            case drawop_Stroke : FrameOval(&r); break;
            case drawop_Fill : PaintOval(&r); break;
        }
	return errNone;
}

int prRoundRect_Draw(struct VMGlobals *g, int numArgsPushed);
int prRoundRect_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	Rect r;
	int radius;
	int err = slotGetRoundRect(a, &r, &radius);
	if (err) return err;	
		
        RGBColor color;
        err = slotRGBColorVal(b, &color);
	if (err) return err;	
        RGBForeColor(&color);
        
        switch (c->ui) {
            case drawop_Invert : InvertRoundRect(&r, radius, radius); break;
            case drawop_Erase : EraseRoundRect(&r, radius, radius); break;
            case drawop_Stroke : FrameRoundRect(&r, radius, radius); break;
            case drawop_Fill : PaintRoundRect(&r, radius, radius); break;
        }

	return errNone;
}

int prArc_Draw(struct VMGlobals *g, int numArgsPushed);
int prArc_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
	Rect r;
	int startAngle, arcAngle;
	int err = slotGetArc(a, &r, &startAngle, &arcAngle);
	if (err) return err;	
        
        RGBColor color;
        err = slotRGBColorVal(b, &color);
	if (err) return err;	
        RGBForeColor(&color);
        
        switch (c->ui) {
            case drawop_Invert : InvertArc(&r, startAngle, arcAngle); break;
            case drawop_Erase : EraseArc(&r, startAngle, arcAngle); break;
            case drawop_Stroke : FrameArc(&r, startAngle, arcAngle); break;
            case drawop_Fill : PaintArc(&r, startAngle, arcAngle); break;
        }
	return errNone;
}

void hilite_color(RGBColor *rgb);
void hilite_color(RGBColor *rgb)
{
	rgb->red   = 65535L - ((65535L - rgb->red  ) >> 2);
	rgb->green = 65535L - ((65535L - rgb->green) >> 2);
	rgb->blue  = 65535L - ((65535L - rgb->blue ) >> 2);
}

void shadow_color(RGBColor *rgb);
void shadow_color(RGBColor *rgb)
{
	rgb->red   >>= 1;
	rgb->green >>= 1;
	rgb->blue  >>= 1;
}

void dim_color(RGBColor *rgb);
void dim_color(RGBColor *rgb)
{
	rgb->red   = (32768L + rgb->red  ) >> 1;
	rgb->green = (32768L + rgb->green) >> 1;
	rgb->blue  = (32768L + rgb->blue ) >> 1;
}

void bevelRect(Rect *r0, int width,
	RGBColor *color1, RGBColor *color2, RGBColor *color3, bool fill);
void bevelRect(Rect *r0, int width,
	RGBColor *color1, RGBColor *color2, RGBColor *color3, bool fill) 
{
	Rect r;
	PenState ps;
	RGBColor saveForeColor;
	
	GetPenState(&ps);
	GetForeColor(&saveForeColor);
	r = *r0;
	PenNormal();
	PenSize(width, width);
	RGBForeColor(color1);
	MoveTo(r.left, r.top);
	LineTo(r.right-width, r.top);
	MoveTo(r.left, r.top);
	LineTo(r.left, r.bottom-width);
	PenSize(1, 1);
	RGBForeColor(color2);
	while (width--) {
		MoveTo(r.right-1, r.top);
		LineTo(r.right-1, r.bottom-1);
		MoveTo(r.left, r.bottom-1);
		LineTo(r.right-1, r.bottom-1);
		InsetRect(&r,1,1);
	}
	RGBForeColor(color3);
	if (fill) {
		PaintRect(&r);
	}
	SetPenState(&ps);
	RGBForeColor(&saveForeColor);	
}

void bevelInRect(Rect *r, int width, RGBColor colorFill, bool fill);
void bevelInRect(Rect *r, int width, RGBColor colorFill, bool fill) 
{
	RGBColor hilite, shadow;

	hilite = shadow = colorFill;

	hilite_color(&hilite);
	shadow_color(&shadow);

	bevelRect(r, width, &shadow, &hilite, &colorFill, fill);
}

void bevelOutRect(Rect *r, int width, RGBColor colorFill, bool fill);
void bevelOutRect(Rect *r, int width, RGBColor colorFill, bool fill) 
{
	RGBColor hilite, shadow;

	hilite = shadow = colorFill;

	hilite_color(&hilite);
	shadow_color(&shadow);

	bevelRect(r, width, &hilite, &shadow, &colorFill, fill);
}

int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop);
int drawBevelRect(Rect r, int width, int inout, RGBColor color, int drawop)
{
	if (inout) {
            switch (drawop) {
                case drawop_Invert : InvertRect(&r); break;
                case drawop_Erase : EraseRect(&r); break;
                case drawop_Stroke : bevelInRect(&r, width, color, false); break;
                case drawop_Fill : bevelInRect(&r, width, color, true); break;
            }
	} else {
            switch (drawop) {
                case drawop_Invert : InvertRect(&r); break;
                case drawop_Erase : EraseRect(&r); break;
                case drawop_Stroke : bevelOutRect(&r, width, color, false); break;
                case drawop_Fill : bevelOutRect(&r, width, color, true); break;
            }
	}
	return errNone;
}


inline Point origin(Rect a)
{
	Point p;
	p.h = a.left;
	p.v = a.top;
	return p;
}	

inline Point extent(Rect a)
{
	Point p;
	p.h = a.right - a.left;
	p.v = a.bottom - a.top;
	return p;
}

inline Point operator+(Point a, Point b)
{
	Point p;
	p.h = a.h + b.h;
	p.v = a.v + b.v;
	return p;
}

inline Point operator-(Point a, Point b)
{
	Point p;
	p.h = a.h - b.h;
	p.v = a.v - b.v;
	return p;
}

inline Point operator/(Point a, int z)
{
	Point p;
	p.h = a.h / z;
	p.v = a.v / z;
	return p;
}

Point centerRectIn(Rect &a, Rect &b);
Point centerRectIn(Rect &a, Rect &b)
{
	Point spacing = (extent(b) - extent(a)) / 2;
	Point newOrigin = origin(b) - origin(a) + spacing;
	return newOrigin;
}

void stringBounds(char *s, int len, Rect &r);
void stringBounds(char *s, int len, Rect &r)
{
	FontInfo finfo;
	GetFontInfo(&finfo);

	int width = TextWidth(s, 0, len);
	
	r.left = 0;
	r.top = -finfo.ascent;
	r.right = width;
	r.bottom = finfo.descent;
}

int prButton_Draw(struct VMGlobals *g, int numArgsPushed);
int prButton_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	//PyrSlot *a = g->sp - 5; // button
	PyrSlot *b = g->sp - 4;	// rect
	PyrSlot *c = g->sp - 3;	// backColor
	PyrSlot *d = g->sp - 2;	// forecolor
	PyrSlot *e = g->sp - 1;	// string
	PyrSlot *f = g->sp;  	// down
	
	Rect r, sr;
	int width = 2;
	bool down = IsTrue(f);

	int err = slotGetQDRect(b, &r);
	if (err) return err;

	RGBColor backColor, foreColor;
	err = slotRGBColorVal(c, &backColor);
	if (err) return err;
	err = slotRGBColorVal(d, &foreColor);
	if (err) return err;

	FrameRoundRect(&r, 4, 4);
	InsetRect(&r, 1, 1);

	if (down) {
		bevelInRect(&r, width, backColor, true);
	} else {
		bevelOutRect(&r, width, backColor, true);
	}
	if (err) return err;
	
	if (!(isKindOfSlot(e, class_string))) return errWrongType;
	PyrString* string = e->uos;
	
	RGBForeColor(&foreColor);
	stringBounds(string->s, string->size, sr);
	Point p = centerRectIn(sr, r);
	
	MoveTo(p.h, p.v);
	MacDrawText(string->s, 0, string->size);
	
	
	return errNone;
}

int prBevelRect_Draw(struct VMGlobals *g, int numArgsPushed);
int prBevelRect_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	Rect r;
	int width;
	bool inout;
	int err = slotBevelRect(a, &r, &width, &inout);
	if (err) return err;	
        
        RGBColor backColor;
        err = slotRGBColorVal(b, &backColor);
	if (err) return err;	
        
	return drawBevelRect(r, width, inout, backColor, c->ui);
}

int prTextBox_Draw(struct VMGlobals *g, int numArgsPushed);
int prTextBox_Draw(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp;
        PyrSlot* slots = a->uo->slots;
	PyrSlot *boundsSlot = slots + 1;
	PyrSlot *stringSlot = slots + 2;
	PyrSlot *justifySlot = slots + 3;
	PyrSlot *colorSlot = slots + 4;
	
	if (!(isKindOfSlot(boundsSlot, s_rect->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;
	if (!IsSym(justifySlot)) return errWrongType;

	RGBColor foreColor;
	int err = slotRGBColorVal(colorSlot, &foreColor);
	if (err) return err;
	
	Rect bounds;
	err = slotGetQDRect(boundsSlot, &bounds);
	if (err) return err;	
	RGBForeColor(&foreColor);
	
	int just;
	char *justName = justifySlot->us->name;
	if (strlen(justName) < 4 || justName[0] == 'd') just = teFlushDefault;
	else if (justName[0] == 'l') just = teFlushLeft;
	else if (justName[0] == 'c') just = teCenter;
	else if (justName[0] == 'r') just = teFlushRight;
	TETextBox(stringSlot->uos->s, stringSlot->uo->size, &bounds, just);
	
	return errNone;
}


void initGUIPrimitives()
{
	int base, index;
	
        s_draw = getsym("draw");
        s_font = getsym("Font");
        s_closed = getsym("closed");
        s_tick = getsym("tick");
        s_doaction = getsym("doAction");
        
	base = nextPrimitiveIndex();
	index = 0;
	
	definePrimitive(base, index++, "_NewTextWindow", prNewTextWindow, 3, 0);	
	definePrimitive(base, index++, "_OpenTextFile", prOpenTextFile, 3, 0);	
	definePrimitive(base, index++, "_GetStringFromUser", prGetStringFromUser, 3, 0);	
	definePrimitive(base, index++, "_SCWindow_New", prSCWindow_New, 6, 0);	
	definePrimitive(base, index++, "_SCWindow_Refresh", prSCWindow_Refresh, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_Close", prSCWindow_Close, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_Minimize", prSCWindow_Minimize, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_ToFront", prSCWindow_ToFront, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_BeginFullScreen", prSCWindow_FullScreen, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_EndFullScreen", prSCWindow_EndFullScreen, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_SetShouldClose", prSCWindow_SetShouldClose, 2, 0);	
    
	definePrimitive(base, index++, "_SCWindow_Show", prSCWindow_Show, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_Hide", prSCWindow_Hide, 1, 0);	
	definePrimitive(base, index++, "_SCWindow_SetName", prSCWindow_SetName, 2, 0);	
	definePrimitive(base, index++, "_SCWindow_SetAlpha", prSCWindow_SetAlpha, 2, 0);	
	definePrimitive(base, index++, "_SCWindow_SetBounds", prSCWindow_SetBounds, 2, 0);	
	definePrimitive(base, index++, "_SCWindow_GetBounds", prSCWindow_GetBounds, 2, 0);	
	definePrimitive(base, index++, "_SCWindow_GetScreenBounds", prSCWindow_GetScreenBounds, 2, 0);	

	definePrimitive(base, index++, "_String_GetBounds", prString_GetBounds, 3, 0);	
	definePrimitive(base, index++, "_String_DrawAtPoint", prString_DrawAtPoint, 4, 0);	
	definePrimitive(base, index++, "_String_DrawInRect", prString_DrawInRect, 4, 0);	
	definePrimitive(base, index++, "_Color_SetStroke", prColor_SetStroke, 1, 0);	
	definePrimitive(base, index++, "_Color_SetFill", prColor_SetFill, 1, 0);	

	definePrimitive(base, index++, "_Pen_StrokeRect", prPen_StrokeRect, 2, 0);	
	definePrimitive(base, index++, "_Pen_FillRect", prPen_FillRect, 2, 0);	
	definePrimitive(base, index++, "_Pen_StrokeOval", prPen_StrokeOval, 2, 0);	
	definePrimitive(base, index++, "_Pen_FillOval", prPen_FillOval, 2, 0);	
	definePrimitive(base, index++, "_Pen_StrokeLine", prPen_StrokeLine, 3, 0);	
	definePrimitive(base, index++, "_Pen_Push", prPen_Push, 1, 0);	
	definePrimitive(base, index++, "_Pen_Pop", prPen_Pop, 1, 0);	
	definePrimitive(base, index++, "_Pen_Translate", prPen_Translate, 3, 0);	
	definePrimitive(base, index++, "_Pen_Scale", prPen_Scale, 3, 0);	
	definePrimitive(base, index++, "_Pen_Rotate", prPen_Rotate, 4, 0);	
	definePrimitive(base, index++, "_Pen_SetWidth", prPen_SetWidth, 2, 0);	
	definePrimitive(base, index++, "_Pen_SetMatrix", prPen_SetMatrix, 2, 0);	

	definePrimitive(base, index++, "_Pen_BeginPath", prPen_BeginPath, 1, 0);	
	definePrimitive(base, index++, "_Pen_MoveTo", prPen_MoveTo, 2, 0);	
	definePrimitive(base, index++, "_Pen_LineTo", prPen_LineTo, 2, 0);	
	definePrimitive(base, index++, "_Pen_StrokePath", prPen_StrokePath, 1, 0);	
	definePrimitive(base, index++, "_Pen_FillPath", prPen_FillPath, 1, 0);	
        
	definePrimitive(base, index++, "_Pen_DrawAquaButton", prPen_DrawAquaButton, 5, 0);	
        
	definePrimitive(base, index++, "_HasDeferred", prHasDeferred, 1, 0);	
	definePrimitive(base, index++, "_LoadUserPanel", prLoadUserPanel, 2, 0);	

	definePrimitive(base, index++, "_Rect_Draw", prRect_Draw, 3, 0);
	definePrimitive(base, index++, "_Oval_Draw", prOval_Draw, 3, 0);
	//definePrimitive(base, index++, "_Lines_Draw", prLines_Draw, 1, 0);
	//definePrimitive(base, index++, "_ZigZag_Draw", prZigZag_Draw, 1, 0);
	//definePrimitive(base, index++, "_Polygon_Draw", prPolygon_Draw, 1, 0);
	//definePrimitive(base, index++, "_String_GetBounds", prString_GetBounds, 2, 0);
	//definePrimitive(base, index++, "_String_Draw", prString_Draw, 1, 0);
	//definePrimitive(base, index++, "_Symbol_Draw", prSymbol_Draw, 1, 0);
	//definePrimitive(base, index++, "_Char_Draw", prChar_Draw, 1, 0);
	definePrimitive(base, index++, "_TextBox_Draw", prTextBox_Draw, 5, 0);
	definePrimitive(base, index++, "_RoundRect_Draw", prRoundRect_Draw, 3, 0);
	definePrimitive(base, index++, "_BevelRect_Draw", prBevelRect_Draw, 3, 0);
	definePrimitive(base, index++, "_Button_Draw", prButton_Draw, 6, 0);
	definePrimitive(base, index++, "_Arc_Draw", prArc_Draw, 3, 0);
	definePrimitive(base, index++, "_Font_AvailableFonts", prFont_AvailableFonts, 1, 0);
}

