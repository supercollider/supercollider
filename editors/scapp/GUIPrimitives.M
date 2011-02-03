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
#import "SCTextView.h"
#import "SCNSWindow.h"
#import "SCCocoaView.h"

#if (MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)
typedef float ColorData;
#else
typedef CGFloat ColorData;
#endif


extern ChangeCounter gUIChangeCounter;
extern NSTextView* gPostView;
PyrSymbol *s_draw;
PyrSymbol *s_font;
PyrSymbol *s_closed;
PyrSymbol *s_tick;
PyrSymbol *s_doaction;
PyrSymbol *s_didBecomeKey;
PyrSymbol *s_didResignKey;

extern bool docCreatedFromLang;
extern bool defaultDocumentUseAutoInOutDent;
int slotColorVal(PyrSlot *slot, SCColor *sccolor);

int prNumberOfOpenTextWindows(struct VMGlobals *g, int numArgsPushed);
int prNumberOfOpenTextWindows(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

		NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) return errFailed;
		int num = [[docctl documents] count];
        if (!num) return errFailed;
		SetInt(a, num);
		return errNone;
}


int prTextWindow_IsEdited(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_IsEdited(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

		MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		bool edited = (bool) [doc isDocumentEdited];
		if(edited) SetTrue(a);
		else SetFalse(a);
		return errNone;
}

int prTextWindow_SetEditable(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetEditable(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	if(IsFalse(b)){
		[[doc textView] setEditable: NO];
		[[doc textView2] setEditable: NO];
	} else {
		[[doc textView] setEditable: YES];
		[[doc textView2] setEditable: YES];
	}
	return errNone;
}

int slotGetNSRect(PyrSlot* a, NSRect *r);

int prTextWindow_SetBounds(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetBounds(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    NSRect bounds;
    int err = slotGetNSRect(b, &bounds);
    if (err) return err;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;

    NSWindow *window = [[doc textView] window];
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

int prTextWindow_GetBounds(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetBounds(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;

	NSWindow *window = [[doc textView] window];

	NSRect bounds = [window frame];
	PyrSlot *slots = slotRawObject(b)->slots;
	SetFloat(slots+0, bounds.origin.x);
	SetFloat(slots+1, bounds.origin.y);
	SetFloat(slots+2, bounds.size.width);
	SetFloat(slots+3, bounds.size.height);
	slotCopy(a, b);

	return errNone;
}

int prTextWindow_SetName(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetName(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	if (!(isKindOfSlot(b, class_string))) return errWrongType;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;

	NSWindow *window = [[doc textView] window];
	PyrString *string = slotRawString(b);
	NSString *title = [NSString stringWithCString: string->s length: string->size];
	[window setTitle: title];
	return errNone;
}

int prTextWindow_SetBackgroundColor(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetBackgroundColor(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
	if (IsNil(b)) return errNone;
	SCColor rgb;
	int err = slotColorVal(b, &rgb);
	if (err) return err;
	NSColor *color = [NSColor colorWithCalibratedRed: rgb.red
						green: rgb.green
						blue: rgb.blue
						alpha: rgb.alpha];

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	[doc setBackgroundColor: color];
	//[[doc textView] didChangeText];

	return errNone;
}

int prTextWindow_GetBackgroundColor(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetBackgroundColor(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	// let the lang allocate the Color
	// rtf seems not to save the backgroundcolor. set it in the lang first!
	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
	if (IsNil(b)) return errNone;
	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	NSTextView * view = [doc initTextView];
	if(!view) view = [doc textView];
	if(!view) return errNone;
	NSColor * color = [view backgroundColor];
	if(!color) return errNone;
	ColorData red, green, blue, alpha;
	[color getRed:&red green:&green blue:&blue alpha:&alpha];
	PyrSlot *slots = slotRawObject(b)->slots;
	SetFloat(slots+0, red);
	SetFloat(slots+1, green);
	SetFloat(slots+2, blue);
	SetFloat(slots+3, alpha);
	return errNone;
}

extern SCColor synColText, synColClass, synColString, synColSymbol, synColComment, synColNumber;

int prSetSyntaxColorTheme(struct VMGlobals *g, int numArgsPushed);
int prSetSyntaxColorTheme(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *colText;
	PyrSlot *colClass;
	PyrSlot *colString;
	PyrSlot *colSymbol;
	PyrSlot *colComment;
	PyrSlot *colNumber;

	colText = g->sp - numArgsPushed + 2;
	colClass = colText + 1;
	colString = colClass + 1;
	colSymbol = colString + 1;
	colComment = colSymbol + 1;
	colNumber = colComment + 1;

	if (isKindOfSlot(colText, s_color->u.classobj)) {
		int err = slotColorVal(colText, &synColText);
		if(err) return err;
	}

	if (isKindOfSlot(colClass, s_color->u.classobj)) {
		int err = slotColorVal(colClass, &synColClass);
		if(err) return err;
	};

	if (isKindOfSlot(colString, s_color->u.classobj)) {
		int err = slotColorVal(colString, &synColString);
		if(err) return err;
	};

	if (isKindOfSlot(colSymbol, s_color->u.classobj)) {
		int err = slotColorVal(colSymbol, &synColSymbol);
		if(err) return err;
	};

	if (isKindOfSlot(colComment, s_color->u.classobj)) {
		int err = slotColorVal(colComment, &synColComment);
		if(err) return err;
	};

	if (isKindOfSlot(colNumber, s_color->u.classobj)) {
		int err = slotColorVal(colNumber, &synColNumber);
		if(err) return err;
	};

	return errNone;

}

int prTextWindow_SetSelectedBackgroundColor(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetSelectedBackgroundColor(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
	if (IsNil(b)) return errNone;
	SCColor rgb;
	int err = slotColorVal(b, &rgb);
	if (err) return err;
	NSColor *color = [NSColor colorWithCalibratedRed: rgb.red
											   green: rgb.green
												blue: rgb.blue
											   alpha: rgb.alpha];
	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	[doc setSelectedBackgroundColor: color];
	return errNone;
}

int prTextWindow_GetSelectedBackgroundColor(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetSelectedBackgroundColor(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
	if (IsNil(b)) return errNone;
	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	NSTextView * view = [doc initTextView];
	if(!view) view = [doc textView];
	if(!view) return errNone;
	NSColor * color = [[[view selectedTextAttributes] objectForKey: NSBackgroundColorAttributeName] colorUsingColorSpaceName: NSCalibratedRGBColorSpace];
	if(!color) return errNone;
	ColorData red, green, blue, alpha;
	[color getRed:&red green:&green blue:&blue alpha:&alpha];
	PyrSlot *slots = slotRawObject(b)->slots;
	SetFloat(slots+0, red);
	SetFloat(slots+1, green);
	SetFloat(slots+2, blue);
	SetFloat(slots+3, alpha);
	return errNone;
}


int prTextWindow_SetTextColor(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetTextColor(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp-3;
		PyrSlot *b = g->sp-2;
		PyrSlot *c = g->sp-1;
		PyrSlot *d = g->sp;

		int rangeStart, rangeSize;
		if (IsNil(b)) return errNone;

		SCColor rgb;
		int err = slotColorVal(b, &rgb);
		if (err) return err;
		err = slotIntVal(c, &rangeStart); //if -1 do not use range
        if (err) return err;
		err = slotIntVal(d, &rangeSize);
        if (err) return err;


		NSColor *color = [NSColor colorWithCalibratedRed: rgb.red
                            green: rgb.green
                            blue: rgb.blue
                            alpha: rgb.alpha];

		MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		//[[doc textView] setBackgroundColor: color];

		if(rangeStart < 0){
			[[doc activeTextView] setTextColor: color];
			[[doc activeTextView] didChangeText];
			return errNone;
		}
		int length = [[[doc activeTextView] string] length];
		if(rangeStart >= length) rangeStart = length - 1 ;
		if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;
		NSRange selectedRange =	NSMakeRange(rangeStart, rangeSize);


		[[doc activeTextView] setTextColor: color range: selectedRange];
		[[doc activeTextView] didChangeText];




		return errNone;
}

int prTextWindow_SetFont(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetFont(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *a = g->sp - 3;
	PyrSlot *fontSlot = g->sp - 2; //fontsize

	PyrSlot *d = g->sp - 1; //rangestart
	PyrSlot *e = g->sp; //rangesize
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
	err = slotIntVal(d, &rangeStart); //if -1 do not use range
	if (err) return err;
		err = slotIntVal(e, &rangeSize);
	if (err) return err;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	if(rangeStart < 0){
		[[doc activeTextView] setFont: font];
		return errNone;
	}
	NSString* string = [[doc activeTextView] string];
	int length = [string length];
	if(length < 1) return errFailed;
	if(rangeStart >= length) rangeStart = length - 1 ;
	if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;
	NSRange selectedRange =	NSMakeRange(rangeStart, rangeSize);

	[[doc activeTextView] setFont: font range: selectedRange];
	return errNone;
}
int prTextWindow_UnderlineSelection(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_UnderlineSelection(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	[[doc activeTextView] underline: NULL];
	return errNone;

}

int prTextWindow_Close(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_Close(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	SEL sel = @selector(closeWindow);
	NSMethodSignature *sig = [MyDocument instanceMethodSignatureForSelector: sel];

	NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
	[anInvocation setTarget: doc];
	[anInvocation setSelector: sel];
	[scvm defer: anInvocation];


	return errNone;
}

int prTextWindow_SelectLine(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SelectLine(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp; //the win number

	int err, linenum;
	err = slotIntVal(b, &linenum);
	if (err) return errWrongType;
	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	//[doc selectLine: linenum];
	SEL sel = @selector(selectLine:);
	NSMethodSignature *sig = [MyDocument instanceMethodSignatureForSelector: sel];

	NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
	[anInvocation setTarget: doc];
	[anInvocation setSelector: sel];
	[anInvocation setArgument: &linenum atIndex: 2];
	[scvm defer: anInvocation];

	return errNone;
}

int prTextWindow_SelectRange(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SelectRange(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1; //the win number
	PyrSlot *c = g->sp;

	int rangeStart, rangeSize;
	int err = slotIntVal(b, &rangeStart);
	if (err) return err;
	err = slotIntVal(c, &rangeSize);
	if (err) return err;
	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	[doc selectRangeStart: rangeStart size: rangeSize];
	return errNone;
}

int prTextWindow_GetByIndex(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetByIndex(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp - 1;
		PyrSlot *b = g->sp; //the win number

        int err, inputIndex;
		err = slotIntVal(b, &inputIndex);
		if (err) return errWrongType;

        NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) return errFailed;
        MyDocument* doc = [[docctl documents] objectAtIndex: inputIndex];
        if (!doc) return errFailed;
		PyrObject * obj = [doc getSCObject];
		//if there is a scobject already don't store i
		if(obj){
			//post("docuemnt has scObject already\n");
			SetNil(a);
			return errNone;
		}
		[doc setSCObject: slotRawObject(a)];
		SetPtr(slotRawObject(a)->slots + 0, doc);
//		if([doc getSCKeyDownAction])
//		SetObject(slotRawObject(a)->slots + 1, [doc getSCKeyDownAction]);
		return errNone;
}

int prTextWindow_GetLastIndex(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetLastIndex(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;
        NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) return errFailed;
        MyDocument* doc = [[docctl documents] lastObject];
        if (!doc) return errFailed;
		PyrObject * obj = [doc getSCObject];
		if(obj){
			//post("docuemnt has scObject already\n");
			SetNil(a);
			return errNone;
		}
		[doc setSCObject: slotRawObject(a)];
		SetPtr(slotRawObject(a)->slots + 0, doc);
		return errNone;
}

int prTextWindow_ToFront(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_ToFront(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *b = g->sp; //the win number

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(b)->slots);
	if (!doc) return errFailed;
	docCreatedFromLang = true;
	[[[doc textView] window] makeKeyAndOrderFront: nil];
	docCreatedFromLang = false;
	return errNone;
}

int prTextWindow_SyntaxColorize(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SyntaxColorize(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *b = g->sp; //the win number

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(b)->slots);
	if (!doc) return errFailed;
	[doc syntaxColorize: nil];
	return errNone;
}

int prTextWindow_InsertTextInRange(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_InsertTextInRange(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *a = g->sp - 3;
	PyrSlot *b = g->sp - 2; //the text
	PyrSlot *c = g->sp - 1;
	PyrSlot *d = g->sp;

	if (!(isKindOfSlot(b, class_string))) return errWrongType;

	int rangeStart, rangeSize;
	int err = slotIntVal(c, &rangeStart); //if -1 do not use range
	if (err) return err;
	err = slotIntVal(d, &rangeSize);
	if (err) return err;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	PyrString* pstring = slotRawString(b);
	NSRange selectedRange;
	int length = [[[doc activeTextView] string] length];

	if(rangeSize < 0) rangeSize = length - 1;
	if(rangeStart >= length) rangeStart = length - 1 ;
	if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;

	if(rangeStart<0) selectedRange =	NSMakeRange(0, length);
	else selectedRange =	NSMakeRange(rangeStart, rangeSize);


	NSString *string = [[NSString alloc] initWithCString: pstring->s length: pstring->size];
	if ([[doc activeTextView]  shouldChangeTextInRange: selectedRange replacementString: string]) {
		[[doc activeTextView]  replaceCharactersInRange: selectedRange withString: string];
		[[doc activeTextView]  didChangeText];
	}
	[string release];

	return errNone;
}

//insert in current position
int prTextWindow_InsertText(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_InsertText(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *b = g->sp - 1;
        PyrSlot *c = g->sp; //the text
		if (!(isKindOfSlot(c, class_string))) return errWrongType;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(b)->slots);
        if (!doc) return errFailed;
        PyrString* string = slotRawString(c);
        [doc insertText: string->s length: string->size];
        return errNone;
}

int prTextWindow_GetName(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetName(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
       // const char * cstr = [[doc displayName] UTF8String];
		const char * cstr = [[[[doc textView] window] title] UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(a, string);
        return errNone;
}

int prTextWindow_GetFileName(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetFileName(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
        const char * cstr = [[doc fileName] UTF8String];
		if(!cstr){
			SetNil(a);
			return errNone;
		}
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(a, string);
        return errNone;
}
int prTextWindow_SetFileName(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetFileName(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp-1;
		PyrSlot *b = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		PyrString* filenmae = slotRawString(b);
		NSString *nsname = [NSString stringWithCString: filenmae->s length: filenmae->size];
		if(!nsname) return errFailed;
        [doc setFileName: nsname];
        return errNone;
}
int prTextWindow_GetSelectedRangeLocation(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetSelectedRangeLocation(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		NSRange range = [[doc activeTextView] selectedRange];
		SetInt(a, range.location);
        return errNone;
}

#if 0
int prTextWindow_SelectToken(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SelectToken(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;

		NSString* string = [[doc activeTextView] string];
		NSRange range = [[doc activeTextView] selectedRange];
		int clickLoc = range.location;
		range.location = range.location < 64 ? 0 : range.location - 64;
		range.length = 64;
		NSRange wordRange = range;
		for (int i=clickLoc; i>=(int)range.location; --i) {
			if (isspace([string characterAtIndex: i])) {
				wordRange.location = i;
				wordRange.length = range.location + range.length - i;
				break;
			}
		}
		for (int i=clickLoc; i<(int)(range.location + range.length); ++i) {
			if (isspace([string characterAtIndex: i])) {
				wordRange.length = i - wordRange.location;
				break;
			}
		}

        [doc selectRangeStart: wordRange.location size: wordRange.length];

        return errNone;
}
#endif

int prTextWindow_SelectUnderlinedText(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SelectUnderlinedText(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;

	NSTextView *textView = [doc activeTextView];

	int clickLoc;
	int err = slotIntVal(b, &clickLoc);
	if (err) {
		clickLoc = [textView selectedRange].location;
	}
	NSRange range = NSMakeRange(clickLoc, 0);

	NSTextStorage *textStorage = [textView textStorage];
	NSRange limitRange = NSMakeRange(0, [textStorage length]);
	NSRange underlineRange;
	if (range.location < limitRange.length) {

		id isUnderlined = [textStorage attribute: NSUnderlineStyleAttributeName
							atIndex: range.location
							longestEffectiveRange: &underlineRange
							inRange: limitRange];

		if (isUnderlined) {

			NSString* string = [[doc activeTextView] string];

		   // underlined range should not cross newlines.
			for (int i=clickLoc; i>=(int)underlineRange.location; --i) {
				int c = [string characterAtIndex: i];
				if (c == '\n') {
					underlineRange.length -= i + 1 - underlineRange.location;
					underlineRange.location = i + 1;
					break;
				}
			}
			for (int i=clickLoc; i<(int)(underlineRange.location + underlineRange.length); ++i) {
				int c = [string characterAtIndex: i];
				if (c == '\n') {
					underlineRange.length = i - underlineRange.location;
					break;
				}
			}

			// trim whitespace
			int start = underlineRange.location;
			int end = underlineRange.location + underlineRange.length;
			for (int i=start; i<end; ++i) {
				int c = [string characterAtIndex: i];
				if (!isspace(c)) break;
				underlineRange.location ++;
				underlineRange.length --;
			}

			start = underlineRange.location + underlineRange.length - 1;
			end = underlineRange.location;
			for (int i=start; i>=end; --i) {
				int c = [string characterAtIndex: i];
				if (!isspace(c)) break;
				underlineRange.length --;
			}

			// include square brackets in selection
			start = underlineRange.location - 1;
			end = underlineRange.location + underlineRange.length;
			if (start >= 0 && [string characterAtIndex: start] == '['
				&& end < (int)limitRange.length && [string characterAtIndex: end] == ']') {
				underlineRange.location --;
				underlineRange.length += 2;
			}
			// include parentheses in selection
			if (start >= 0 && [string characterAtIndex: start] == '('
				&& end < (int)limitRange.length && [string characterAtIndex: end] == ')') {
				underlineRange.location --;
				underlineRange.length += 2;
			}

			if ((int)underlineRange.length > 0 && (int)underlineRange.location <= clickLoc && clickLoc < (int)(underlineRange.location + underlineRange.length)) {
				[doc selectRangeStart: underlineRange.location size: underlineRange.length];
				SetTrue(a);
			} else {
				SetFalse(a);
			}
		} else {
			SetFalse(a);
		}
	} else {
		SetFalse(a);
	}
	return errNone;
}

int prTextWindow_LinkAtClickPos(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_LinkAtClickPos(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;

	NSTextView *textView = [doc activeTextView];

	int clickLoc;
	int err = slotIntVal(b, &clickLoc);
	if (err) {
		clickLoc = [textView selectedRange].location;
	}
	NSRange range = NSMakeRange(clickLoc, 0);

	NSTextStorage *textStorage = [textView textStorage];
	NSRange limitRange = NSMakeRange(0, [textStorage length]);
	if (range.location < limitRange.length) {

		id isLink = [textStorage attribute: NSLinkAttributeName atIndex: range.location effectiveRange: NULL];

		if (isLink) { SetTrue(a);} else { SetFalse(a);}

	} else {
		SetFalse(a);
	}
	return errNone;
}


int prTextWindow_GetSelectedRangeLength(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetSelectedRangeLength(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		NSRange range = [[doc activeTextView] selectedRange];
		SetInt(a, range.length);
		return errNone;
}

int prTextWindow_SelectedText(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SelectedText(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;

        NSString* str = [[doc activeTextView] currentlySelectedTextOrLine:NULL];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(a, string);
        return errNone;
}

int prTextWindow_TextWithRange(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_TextWithRange(struct VMGlobals *g, int numArgsPushed)
{

		 if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp - 2;
		PyrSlot *b = g->sp - 1; //from
		PyrSlot *c = g->sp;//length

		int rangeStart, rangeSize;
        int err = slotIntVal(b, &rangeStart);
        if (err) return err;
        err = slotIntVal(c, &rangeSize);
        if (err) return err;
		MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		NSString* string = [[doc activeTextView] string];
		int length = [string length];
		if(length < 1)
		{
			SetNil(a);
			return errNone;
		}
		if(rangeStart >= length) rangeStart = length - 1 ;
		if(rangeStart + rangeSize >= length) rangeSize = length - rangeStart;

		NSRange selectedRange =	NSMakeRange(rangeStart, rangeSize);
		NSString* str = [string substringWithRange: selectedRange];
        const char * cstr = [str UTF8String];
        PyrString *pstring = newPyrString(g->gc, cstr, 0, true);
        SetObject(a, pstring);
		return errNone;
}

int prTextWindow_Text(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_Text(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;

        NSString* str = [[doc activeTextView] string];
        const char * cstr = [str UTF8String];
        PyrString *string = newPyrString(g->gc, cstr, 0, true);
        SetObject(a, string);
        return errNone;
}
int prTextWindow_RemoveUndo(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_RemoveUndo(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		[doc updateChangeCount: NSChangeCleared];
		return errNone;
}

int prTextWindow_UnfocusedFront(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_UnfocusedFront(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
        [[[doc textView] window] orderFrontRegardless];
        return errNone;
}

int prTextWindow_AlwaysOnTop(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_AlwaysOnTop(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp-1;
		PyrSlot *b = g->sp;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		if(IsTrue(b))
			[[[doc textView] window] setLevel:NSFloatingWindowLevel];
		else
			[[[doc textView] window] setLevel:NSNormalWindowLevel];
        return errNone;
}

int prTextWindow_IsAlwaysOnTop(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_IsAlwaysOnTop(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		int level = [[[doc textView] window] level];
		if(level>0) SetTrue(a);
		else SetFalse(a);
        return errNone;
}

int prTextWindow_SetPromptToSave(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetPromptToSave(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp-1;
		PyrSlot *b = g->sp;

        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		if(IsTrue(b))
			[doc setPromptToSave:YES];
		else
			[doc setPromptToSave:NO];
        return errNone;
}

int prTextWindow_PromptToSave(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_PromptToSave(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *a = g->sp;
        MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
        if (!doc) return errFailed;
		bool flag = [doc promptToSave];
		if(flag) SetTrue(a);
		else SetFalse(a);
        return errNone;
}

extern NSTextView *gPostView;

int prTextWindow_GetIndexOfListener(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_GetIndexOfListener(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

		PyrSlot *a = g->sp;
		NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) return errFailed;
		MyDocument* doc = [gPostView delegate];
        if (!doc) return errFailed;
		int index = [[docctl documents] indexOfObject: doc];
		SetInt(a, index);
        return errNone;
}

int prTextWindow_UsesAutoInOutdent(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_UsesAutoInOutdent(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp; //bool
	
	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	[doc setUsesAutoInOutdent:IsTrue(b)];
	return errNone;
}

int prTextWindow_DefaultUsesAutoInOutdent(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_DefaultUsesAutoInOutdent(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	
	//PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp; //bool
	
	defaultDocumentUseAutoInOutDent = IsTrue(b);
	return errNone;
}


///////


int prNewTextWindow(struct VMGlobals *g, int numArgsPushed);
int prNewTextWindow(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;

        PyrSlot *d = g->sp - 3;
		PyrSlot *titleSlot = g->sp - 2;
        PyrSlot *stringSlot = g->sp - 1;
		PyrSlot *c = g->sp;

		if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;

        PyrString* string = slotRawString(stringSlot);
        NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
        if (!docctl) return errFailed;
		docCreatedFromLang = true;
        NSError * outError;
		MyDocument* doc = [docctl openUntitledDocumentAndDisplay:YES  error: &outError];
//      MyDocument* doc = [docctl currentDocument];
        if (!doc) {
			docCreatedFromLang = false;
			return errFailed;
		}

        [doc insertText: string->s length: string->size];
        NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
        if (!window) {
			docCreatedFromLang = false;
			return errFailed;
		}
		if (isKindOfSlot(titleSlot, class_string)) {
			PyrString* title = slotRawString(titleSlot);
			NSString *nstitle = [NSString stringWithCString: title->s length: title->size];
			[window setTitle: nstitle];
		}
		[[doc undoManager] removeAllActions];

		if (IsTrue(c)) {
			gPostView = [doc textView];
		}
		PyrObject * obj = [doc getSCObject];
		if(obj){
			//post("docuemnt has scObject already\n");
			docCreatedFromLang = false;
			SetNil(d);
			return errNone;
		}
		[doc setSCObject: slotRawObject(d)];
        SetPtr(slotRawObject(d)->slots + 0, doc);
		docCreatedFromLang = false;
	return errNone;
}


int prOpenTextFile(struct VMGlobals *g, int numArgsPushed);
int prOpenTextFile(struct VMGlobals *g, int numArgsPushed)
{
        if (!g->canCallOS) return errCantCallOS;
		PyrSlot *d = g->sp - 3;

		PyrSlot *a = g->sp - 2;
		PyrSlot *b = g->sp - 1;
		PyrSlot *c = g->sp;

		if (!(isKindOfSlot(a, class_string))) return errWrongType;
		PyrString* string = slotRawString(a);
        if(string->size == 0) return errFailed;
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

		docCreatedFromLang = true;
		MyDocument *doc = [docctl openDocumentWithContentsOfFile: nspath display: true];
		if (!doc) {
			post("Can't open Document '%s'\n", [nspath cString]);
			docCreatedFromLang = false;
			return errFailed;
		}
		NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
		if (!window) {
				post("window controller returns nil\n");
				docCreatedFromLang = false;
				return errFailed;
		}
        [window makeKeyAndOrderFront: nil];

        // select
        [doc selectRangeStart: rangeStart size: rangeSize];

		PyrObject * obj = [doc getSCObject];
		//if there is a scobject already don't store i
		if(obj){
			//post("docuemnt has scObject already\n");
			SetNil(d);
			docCreatedFromLang = false;
			return errNone;
		}
		[doc setSCObject: slotRawObject(d)];
		SetPtr(slotRawObject(d)->slots + 0, doc);
		docCreatedFromLang = false;
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

extern NSFont *defaultFont;

int prTextWindow_SetDefaultFont(struct VMGlobals *g, int numArgsPushed);
int prTextWindow_SetDefaultFont(struct VMGlobals *g, int numArgsPushed)
{
	NSFont *newDefault, *oldDefault;
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *a = g->sp - 1;
	PyrSlot *fontSlot = g->sp; //font
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
	newDefault = [NSFont fontWithName: fontName size: size];
	if (!newDefault) {
		return errFailed;
	} else {
		oldDefault = defaultFont;
		[newDefault retain];
		defaultFont = newDefault;
		[oldDefault release];
	}
	return errNone;

}

extern SCColor postTextColor;

int prSetPostTextColor(struct VMGlobals *g, int numArgsPushed);
int prSetPostTextColor(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a;
	PyrSlot *c;

	a = g->sp - 1;
	c = a + 1;

	if(!(isKindOfSlot(c, s_color->u.classobj))) return errWrongType;

	int err = slotColorVal(c, &postTextColor);
	if(err) return err;

	return errNone;
}


int prBalanceParens(struct VMGlobals *g, int numArgsPushed);
int prBalanceParens(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp;

	MyDocument* doc = (MyDocument*) slotRawPtr(slotRawObject(a)->slots);
	if (!doc) return errFailed;
	[[doc activeTextView] balanceParens: nil];

	return errNone;
}


int slotGetNSRect(PyrSlot* a, NSRect *r)
{
	PyrSlot *slots = slotRawObject(a)->slots;
	int err;
	err = slotVal(slots+0, &r->origin.x);
	if (err) return err;
	err = slotVal(slots+1, &r->origin.y);
	if (err) return err;
	err = slotVal(slots+2, &r->size.width);
	if (err) return err;
	err = slotVal(slots+3, &r->size.height);
	if (err) return err;

	return errNone;
}


int slotGetCGRect(PyrSlot* a, CGRect *r)
{
	PyrSlot *slots = slotRawObject(a)->slots;
	int err;
	err = slotVal(slots+0, &r->origin.x);
	if (err) return err;
	err = slotVal(slots+1, &r->origin.y);
	if (err) return err;
	err = slotVal(slots+2, &r->size.width);
	if (err) return err;
	err = slotVal(slots+3, &r->size.height);
	if (err) return err;

	return errNone;
}


int slotGetPoint(PyrSlot* a, NSPoint *p)
{
	PyrSlot *slots = slotRawObject(a)->slots;
	int err;
	err = slotVal(slots+0, &p->x);
	if (err) return err;
	err = slotVal(slots+1, &p->y);
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

	bool eIsTrue = IsTrue(e);
	unsigned int mask = IsTrue(d) && eIsTrue ? NSResizableWindowMask : 0;
	if (IsFalse(j) && eIsTrue){
		mask |= NSMiniaturizableWindowMask | NSClosableWindowMask;
	}
	mask |= eIsTrue ? NSTitledWindowMask | NSTexturedBackgroundWindowMask : NSBorderlessWindowMask;

	NSRect bounds;
	int err = slotGetNSRect(c, &bounds);
	if (err) return err;

	PyrString *string = slotRawString(b);
	NSString *title = [NSString stringWithCString: string->s length: string->size];

	SCNSWindow *window = [[SCNSWindow alloc]
		initWithContentRect: bounds
		styleMask: mask
		backing: NSBackingStoreBuffered
		defer: NO];
	[window setTitle: title];
	[window setHasBorders: YES];

	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
	SCGraphView* view = [[SCGraphView alloc] initWithFrame: bounds];
	[view setSCObject: slotRawObject(a)];
	SetPtr(slotRawObject(a)->slots + 0, view);
	[window setDelegate: scvm];
	[window setSCGraphView: view];

	if(IsTrue(h)) {
		SCScrollTopView* scrollTopView = (SCScrollTopView*)slotRawPtr(slotRawObject(f)->slots);
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

	} else {
		[view setSCTopView: (SCTopView*)slotRawPtr(slotRawObject(f)->slots)];
		[window setContentView: view];
		[view autorelease];
	}
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
        [anInvocation setArgument: &slotRawObject(a) atIndex: 4];
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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;
   // [[view window] close];

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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;
    PyrSlot *boo = g->sp;

    // no need to defer, right ?
    [view setWindowShouldClose: IsTrue(boo)];

    return errNone;
}

int prSCWindow_AlwaysOnTop(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_AlwaysOnTop(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;
    NSWindow *window = [view window];
	if(IsTrue(b))
		[window setLevel:NSFloatingWindowLevel];
	else
		[window setLevel:NSNormalWindowLevel];
	return errNone;
}

int prSCWindow_AcceptsClickThrough(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_AcceptsClickThrough(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;
	if(IsTrue(b))
		[view setAcceptsClickThrough:YES];
	else
		[view setAcceptsClickThrough:NO];
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

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    NSWindow *window = [view window];

	bounds = [NSWindow frameRectForContentRect: bounds styleMask: [window styleMask]];

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

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

	NSWindow *window = [view window];
    NSRect bounds = [window frame];
	bounds = [NSWindow contentRectForFrameRect: bounds styleMask: [window styleMask]];

    PyrSlot *slots = slotRawObject(b)->slots;
    SetFloat(slots+0, bounds.origin.x);
    SetFloat(slots+1, bounds.origin.y);
    SetFloat(slots+2, bounds.size.width);
    SetFloat(slots+3, bounds.size.height);
    slotCopy(a, b);

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
    PyrSlot *slots = slotRawObject(b)->slots;
    SetFloat(slots+0, bounds.origin.x);
    SetFloat(slots+1, bounds.origin.y);
    SetFloat(slots+2, bounds.size.width);
    SetFloat(slots+3, bounds.size.height);
    slotCopy(a, b);

    return errNone;
}




//doesn't work as intended
int prSCWindow_Show(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_Show(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);

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
    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
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

int prSCWindow_SetAcceptMouseOver(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_SetAcceptMouseOver(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    BOOL accept;
    accept = (BOOL) IsTrue(b);

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    NSWindow *window = [view window];
	[window setAcceptsMouseMovedEvents:accept];
    return errNone;
}

int prSCWindow_RunModal(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_RunModal(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp;

    SCGraphView* view = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!view) return errNone;

    NSWindow *window = [view window];

	SEL sel = @selector(runAsModal:);
    NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: scvm];
    [anInvocation setSelector: sel];
    [anInvocation setArgument: &window atIndex: 2];
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_StopModal(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_StopModal(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	[NSApp abortModal];

    return errNone;
}

int prSCWindow_RunModalSheet(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_RunModalSheet(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    SCGraphView* sheetView = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!sheetView) return errNone;

    SCNSWindow *sheet = (SCNSWindow*)[sheetView window];

	SCGraphView* windowView = (SCGraphView*)slotRawPtr(slotRawObject(b)->slots);
    if (!windowView) return errNone;

    NSWindow *window = [windowView window];

	SEL sel = @selector(runAsModalSheet:forWindow:);
    NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: scvm];
    [anInvocation setSelector: sel];
	[anInvocation setArgument: &sheet atIndex: 2];
	[anInvocation setArgument: &window atIndex: 3];
    [scvm defer: anInvocation];

    return errNone;
}

int prSCWindow_StopModalSheet(struct VMGlobals *g, int numArgsPushed);
int prSCWindow_StopModalSheet(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp;

    SCGraphView* sheetView = (SCGraphView*)slotRawPtr(slotRawObject(a)->slots);
    if (!sheetView) return errNone;

    NSWindow *sheet = [sheetView window];

	[NSApp endSheet:sheet];

    return errNone;
}

int prShowCursor(struct VMGlobals *g, int numArgsPushed);
int prShowCursor(struct VMGlobals *g, int numArgsPushed)
{
   if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    BOOL show;
    show = (BOOL) IsTrue(b);
    if(show) [NSCursor unhide];
	else [NSCursor hide];
    return errNone;
}

int addFontAttribute(NSMutableDictionary *dict, PyrSlot *fontSlot);
int addFontAttribute(NSMutableDictionary *dict, PyrSlot *fontSlot)
{
    if (IsNil(fontSlot)) return errNone; // use default font
    if (!(isKindOfSlot(fontSlot, s_font->u.classobj))) return errWrongType;

    PyrSlot *nameSlot = slotRawObject(fontSlot)->slots+0;
    PyrSlot *sizeSlot = slotRawObject(fontSlot)->slots+1;

    if (!(isKindOfSlot(nameSlot, class_string))) return errWrongType;

    float size;
    int err = slotFloatVal(sizeSlot, &size);
    if (err) return err;

    PyrString *string = slotRawString(nameSlot);
    NSString *fontName = [NSString stringWithCString: string->s length: string->size];
    if (!fontName) return errFailed;
    NSFont *font = [NSFont fontWithName: fontName size: size];
    if (!font) return errFailed;
    [dict setObject: font forKey: NSFontAttributeName ];

    return errNone;
}



int slotRGBColorVal(PyrSlot *slot, RGBColor *rgbcolor)
{
    if (!(isKindOfSlot(slot, s_color->u.classobj))) return errWrongType;

    PyrSlot *slots = slotRawObject(slot)->slots;

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

int prFont_SetAntiAliasing(struct VMGlobals *g, int numArgsPushed);
int prFont_SetAntiAliasing(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
	CGContextSetShouldAntialias(cgc, IsTrue(b));

	return errNone;
}

int prFont_SetSmoothing(struct VMGlobals *g, int numArgsPushed);
int prFont_SetSmoothing(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
	CGContextSetShouldSmoothFonts(cgc, IsTrue(b));

	return errNone;
}

int prPen_SetSmoothing(struct VMGlobals *g, int numArgsPushed);
int prPen_SetSmoothing(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
	CGContextSetShouldAntialias(cgc, IsTrue(b));

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

    PyrString* string = slotRawString(a);
    NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
    if (!nsstring) return errFailed;
    NSSize size = [nsstring sizeWithAttributes: dict];

    PyrSlot* rectSlots = slotRawObject(b)->slots;
    SetFloat(rectSlots+0, 0.);
    SetFloat(rectSlots+1, 0.);
    SetFloat(rectSlots+2, size.width);
    SetFloat(rectSlots+3, size.height);
    slotCopy(a, b);

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

    PyrString* string = slotRawString(a);
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

    PyrString* string = slotRawString(a);
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
    PyrSlot *slots = slotRawObject(a)->slots;
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

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextSetRGBStrokeColor(cgc, red, green, blue, alpha);
//    CGContextSetRGBFillColor(cgc, red, green, blue, alpha);
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
    PyrSlot *slots = slotRawObject(a)->slots;
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

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextSetRGBFillColor(cgc, red, green, blue, alpha);
//	NSColor *color = [NSColor colorWithCalibratedRed: red
//                            green: green
//                            blue: blue
//                            alpha: alpha];
//    if (color) {
//        [color setFill];
//    }

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

int prPen_AddRect(struct VMGlobals *g, int numArgsPushed);
int prPen_AddRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    CGRect rect;
    int err = slotGetCGRect(b, &rect);
	if (err) return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];

	CGContextAddRect(cgc, rect);
    return errNone;
}

int prPen_StrokeRect(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokeRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    CGRect rect;
    int err = slotGetCGRect(b, &rect);
    if (err) 
		return err;
	
	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) 
		return errFailed;
	
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextBeginPath(cgc);
	CGContextAddRect(cgc, rect);
    CGContextDrawPath(cgc, kCGPathStroke);

    return errNone;
}

int prPen_FillRect(struct VMGlobals *g, int numArgsPushed);
int prPen_FillRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    CGRect rect;
    int err = slotGetCGRect(b, &rect);
    if (err) 
		return err;

    CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	if( !ctx )
		return errFailed;
	
	CGContextFillRect( ctx, rect );

    return errNone;
}

int prPen_StrokeOval(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokeOval(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	if( !ctx )
		return errFailed;
	
    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

	CGRect rect;
	int err = slotGetCGRect(b, &rect);
    if (err) 
		return err;

    CGContextAddEllipseInRect( ctx, rect );
	CGContextStrokePath( ctx );
	
    return errNone;
}

int prPen_FillOval(struct VMGlobals *g, int numArgsPushed);
int prPen_FillOval(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	if( !ctx )
		return errFailed;
	
    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) 
		return errWrongType;

    NSRect rect;
    int err = slotGetNSRect(b, &rect);
    if (err) return err;

	CGContextAddEllipseInRect( ctx, *(CGRect*)&rect );
	CGContextFillPath( ctx );

    return errNone;
}

int prPen_StrokeLine(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokeLine(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

	CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	if( !ctx )
		return errFailed;
	
    //PyrSlot *a = g->sp - 2;
    PyrSlot *b = g->sp - 1;
    PyrSlot *c = g->sp;

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;
    if (!(isKindOfSlot(c, s_point->u.classobj))) return errWrongType;

    NSPoint p1, p2;
    int err = slotGetPoint(b, &p1);
    if (err) return err;
    err = slotGetPoint(c, &p2);
    if (err) return err;
		
	CGContextMoveToPoint( ctx, p1.x, p2.x );
	CGContextAddLineToPoint( ctx, p1.x, p2.x );
	CGContextStrokePath( ctx );
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
	if( !cgc )
		return errFailed;
	
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
	if( !cgc ) 
		return errFailed;
	
    CGContextAddLineToPoint(cgc, p1.x, p1.y);

    return errNone;
}

int prPen_AddArc(struct VMGlobals *g, int numArgsPushed);
int prPen_AddArc(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 4;
    PyrSlot *b = g->sp - 3;
    PyrSlot *c = g->sp - 2;	// radius
    PyrSlot *d = g->sp - 1; // start angle
    PyrSlot *e = g->sp; // angle

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

    NSPoint p1;
    int err = slotGetPoint(b, &p1);
    if (err) return err;

	float radius;
	err = slotFloatVal(c, &radius);
    if (err) return err;

	float startAngle;
	err = slotFloatVal(d, &startAngle);
    if (err) return err;

	float angle;
	err = slotFloatVal(e, &angle);
    if (err) return err;

	int clockwise = angle > 0. ? 0 : 1;
	float endAngle = startAngle + angle;

	NSPoint p2;
	p2.x = p1.x + radius * cos(startAngle);
	p2.y = p1.y + radius * sin(startAngle);

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
	//post("start %g %g  end %g %g  arc %g %g\n", startAngle, startAngle * 180./3.14156, endAngle, endAngle * 180./3.14156, angle, angle * 180./3.14156);
	CGContextMoveToPoint(cgc, p2.x, p2.y);
	CGContextAddArc(cgc, p1.x, p1.y, radius, startAngle, endAngle, clockwise);

    return errNone;
}

int prPen_AddWedge(struct VMGlobals *g, int numArgsPushed);
int prPen_AddWedge(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 4;
    PyrSlot *b = g->sp - 3;
    PyrSlot *c = g->sp - 2;	// radius
    PyrSlot *d = g->sp - 1; // start angle
    PyrSlot *e = g->sp; // angle

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

    NSPoint p1;
    int err = slotGetPoint(b, &p1);
    if (err) return err;

	float radius;
	err = slotFloatVal(c, &radius);
    if (err) return err;

	float startAngle;
	err = slotFloatVal(d, &startAngle);
    if (err) return err;

	float angle;
	err = slotFloatVal(e, &angle);
    if (err) return err;

	int clockwise = angle > 0. ? 0 : 1;
	float endAngle = startAngle + angle;

    NSPoint p2;
	p2.x = p1.x + radius * cos(startAngle);
	p2.y = p1.y + radius * sin(startAngle);

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextMoveToPoint(cgc, p1.x, p1.y);
    CGContextAddLineToPoint(cgc, p2.x, p2.y);
	CGContextAddArc(cgc, p1.x, p1.y, radius, startAngle, endAngle, clockwise);
    CGContextAddLineToPoint(cgc, p1.x, p1.y);

    return errNone;
}

int prPen_AddAnnularWedge(struct VMGlobals *g, int numArgsPushed);
int prPen_AddAnnularWedge(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 5;
    PyrSlot *b = g->sp - 4;
    PyrSlot *c = g->sp - 3;	// innerRadius
    PyrSlot *d = g->sp - 2;	// outerRadius
    PyrSlot *e = g->sp - 1; // start angle
    PyrSlot *f = g->sp; // angle

    if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

    NSPoint p1;
    int err = slotGetPoint(b, &p1);
    if (err) return err;

	float innerRadius;
	err = slotFloatVal(c, &innerRadius);
    if (err) return err;

	float outerRadius;
	err = slotFloatVal(d, &outerRadius);
    if (err) return err;

	float startAngle;
	err = slotFloatVal(e, &startAngle);
    if (err) return err;

	float angle;
	err = slotFloatVal(f, &angle);
    if (err) return err;

	int clockwise = angle > 0. ? 0 : 1;
	float endAngle = startAngle + angle;

    NSPoint p2, p3, p4;
	p2.x = p1.x + innerRadius * cos(startAngle);
	p2.y = p1.y + innerRadius * sin(startAngle);
	p3.x = p1.x + outerRadius * cos(startAngle);
	p3.y = p1.y + outerRadius * sin(startAngle);
	p4.x = p1.x + innerRadius * cos(endAngle);
	p4.y = p1.y + innerRadius * sin(endAngle);

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextMoveToPoint(cgc, p2.x, p2.y);
    CGContextAddLineToPoint(cgc, p3.x, p3.y);
	CGContextAddArc(cgc, p1.x, p1.y, outerRadius, startAngle, endAngle, clockwise);
    CGContextAddLineToPoint(cgc, p4.x, p4.y);
	CGContextAddArc(cgc, p1.x, p1.y, innerRadius, endAngle, startAngle, 1-clockwise);

    return errNone;
}

int prPen_BeginPath(struct VMGlobals *g, int numArgsPushed);
int prPen_BeginPath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextBeginPath(cgc);

    return errNone;
}

int prPen_ClosePath(struct VMGlobals *g, int numArgsPushed);
int prPen_ClosePath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextClosePath(cgc);

    return errNone;
}

int prPen_StrokePath(struct VMGlobals *g, int numArgsPushed);
int prPen_StrokePath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextDrawPath(cgc, kCGPathStroke);

    return errNone;
}

int prPen_FillPath(struct VMGlobals *g, int numArgsPushed);
int prPen_FillPath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextFillPath(cgc);

    return errNone;
}


int prPen_ClipPath(struct VMGlobals *g, int numArgsPushed);
int prPen_ClipPath(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextClip(cgc);

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

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
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

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
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

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
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

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
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

    if (slotRawObject(b)->size < 6) return errFailed;
    PyrObject *mobj = slotRawObject(b);

    CGAffineTransform afx;

	int err = getIndexedVal(mobj, 0, &afx.a);
	if (err) return err;
	err = getIndexedVal(mobj, 1, &afx.b);
	if (err) return err;
	err = getIndexedVal(mobj, 2, &afx.c);
	if (err) return err;
	err = getIndexedVal(mobj, 3, &afx.d);
	if (err) return err;
	err = getIndexedVal(mobj, 4, &afx.tx);
	if (err) return err;
	err = getIndexedVal(mobj, 5, &afx.ty);
	if (err) return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextConcatCTM(cgc, afx);

    return errNone;
}

int prPen_Skew(struct VMGlobals *g, int numArgsPushed);
int prPen_Skew(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp - 1;
    PyrSlot *c = g->sp;

    float x;
    int err = slotFloatVal(b, &x);
    if (err) return err;

    float y;
    err = slotFloatVal(c, &y);
    if (err) return err;

    CGAffineTransform afx;
    afx.a = 1.0;
	afx.b = y;
	afx.c = x;
	afx.d = 1.0;
	afx.tx = 0.0;
	afx.ty = 0.0;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx) return errFailed;
    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
    CGContextConcatCTM(cgc, afx);

    return errNone;
}


int prPen_SetBlend(struct VMGlobals *g, int numArgsPushed);
int prPen_SetBlend(struct VMGlobals *g, int numArgsPushed)
{
	#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *c = g->sp; // blendmode
	int mode = 0, err = errFailed;

	err = slotIntVal(c, &mode);
	if( err != errNone )
		return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
	CGContextSetBlendMode(cgc, (CGBlendMode)mode);
	#endif
	return errNone;
}

int prPen_SetShadow(struct VMGlobals *g, int numArgsPushed);
int prPen_SetShadow(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	int err;

	PyrSlot *a = g->sp - 2; // offset
	PyrSlot *b = g->sp - 1; // blur
    PyrSlot *c = g->sp; // color

	NSPoint offset;
	float	blur;
	SCColor scColor;

    err = slotColorVal(c, &scColor);
	if( err != errNone )
		return err;

	err = slotGetPoint(a, &offset);
	if( err != errNone )
		return err;

	err = slotFloatVal(b, &blur);
	if( err != errNone )
		return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	CGContextRef _ctx = (CGContextRef)[gctx graphicsPort];
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	ColorData colors[4];
	colors[0] = scColor.red;
	colors[1] = scColor.green;
	colors[2] = scColor.blue;
	colors[3] = scColor.alpha;
	CGColorRef col = CGColorCreate(colorSpace, colors);
	if(!col){
		CGColorSpaceRelease(colorSpace);
		return errFailed;
	}

	CGContextSetShadowWithColor(_ctx, (CGSize){offset.x, offset.y}, blur, col);

	CGColorSpaceRelease(colorSpace);
	CGColorRelease(col);

	return errNone;
}

int prPen_AddOval(struct VMGlobals *g, int numArgsPushed);
int prPen_AddOval(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    //PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    CGRect rect;
    int err = slotGetCGRect(b, &rect);
    if (err) return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

    CGContextAddEllipseInRect((CGContextRef)[gctx graphicsPort], rect);

    return errNone;
}

int prPen_AddBezierQuadCurve(struct VMGlobals *g, int numArgsPushed);
int prPen_AddBezierQuadCurve(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	int err;
	NSPoint points[2];

	PyrSlot *a = g->sp - 1; // point
	PyrSlot *b = g->sp; // control point 1

	if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

	err = slotGetPoint(a, &points[0]);
	if( err != errNone )
		return err;

	err = slotGetPoint(b, &points[1]);
	if( err != errNone )
		return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	CGContextAddQuadCurveToPoint((CGContextRef)[gctx graphicsPort],
		points[1].x, points[1].y,
		points[0].x, points[0].y
	);

	return errNone;
}

int prPen_AddBezierCubicCurve(struct VMGlobals *g, int numArgsPushed);
int prPen_AddBezierCubicCurve(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	int err;
	NSPoint points[3];

	PyrSlot *a = g->sp - 2; // point
	PyrSlot *b = g->sp - 1; // control point 1
    PyrSlot *c = g->sp; // control point 2

	if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(c, s_point->u.classobj))) return errWrongType;

	err = slotGetPoint(a, &points[0]);
	if( err != errNone )
		return err;

	err = slotGetPoint(b, &points[1]);
	if( err != errNone )
		return err;

	err = slotGetPoint(c, &points[2]);
	if( err != errNone )
		return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if ( !gctx )
		return errFailed;

	CGContextAddCurveToPoint((CGContextRef)[gctx graphicsPort],
		points[1].x, points[1].y,
		points[2].x, points[2].y,
		points[0].x, points[0].y
	);

	return errNone;
}

int prPen_AddArcToPoint(struct VMGlobals *g, int numArgsPushed);
int prPen_AddArcToPoint(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *a = g->sp - 2; // point1
	PyrSlot *b = g->sp - 1; // point2
	PyrSlot *c = g->sp; // radius

	if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;

	NSPoint p1;
	NSPoint p2;

	int err = slotGetPoint(a, &p1);
	if (err) return err;

	err = slotGetPoint(b, &p2);
	if (err) return err;

	float radius;
	err = slotFloatVal(c, &radius);
	if (err) return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if ( !gctx ) 
		return errFailed;

	CGContextAddArcToPoint((CGContextRef)[gctx graphicsPort], p1.x, p1.y, p2.x, p2.y, radius);

	return errNone;
}

int prPen_ClearRect(struct VMGlobals *g, int numArgsPushed);
int prPen_ClearRect(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *b = g->sp;

    if (!(isKindOfSlot(b, s_rect->u.classobj))) return errWrongType;

    CGRect rect;
    int err = slotGetCGRect(b, &rect);
    if (err) return err;


	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if ( !gctx )
		return errFailed;

    CGContextClearRect((CGContextRef)[gctx graphicsPort], rect);

    return errNone;
}

int prPen_DrawPath(struct VMGlobals *g, int numArgsPushed);
int prPen_DrawPath(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

    PyrSlot *b = g->sp;
	int err, mode=0;

	if( (err = slotIntVal(b, &mode)) != noErr )
		return err;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if ( !gctx )
		return errFailed;

    CGContextRef cgc = (CGContextRef)[gctx graphicsPort];
	//kCGPathFill=0, kCGPathEOFill=1, kCGPathStroke=2, kCGPathFillStroke=3, or kCGPathEOFillStroke=4
	CGContextDrawPath( 
					  (CGContextRef)[gctx graphicsPort], 
					  (CGPathDrawingMode)sc_clip( mode, kCGPathFill, kCGPathEOFillStroke ) 
					  );
	return errNone;
}

int prPen_BeginTransparencyLayer(struct VMGlobals *g, int numArgsPushed);
int prPen_BeginTransparencyLayer(struct VMGlobals *g, int numArgsPushed)
{

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3

	if (!g->canCallOS) return errCantCallOS;
	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;


	#if 1/*MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5*/

	CGContextBeginTransparencyLayer((CGContextRef)[gctx graphicsPort], NULL);

	#else /*MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5*/

	PyrSlot *b = g->sp;
	if (!(isKindOfSlot(b, s_rect->u.classobj)))
		return errWrongType;

    CGRect rect;
    int err = slotGetCGRect(b, &rect);
    if (err)
		return err;

	if(rect.size.width == 0 || rect.size.height == 0) // NULL in either side should take all current clipped region
		CGContextBeginTransparencyLayer((CGContextRef)[gctx graphicsPort], NULL);
	else
		CGContextBeginTransparencyLayerWithRect((CGContextRef)[gctx graphicsPort], rect, NULL);

	#endif

#endif
	return errNone;
}

int prPen_EndTransparencyLayer(struct VMGlobals *g, int numArgsPushed);
int prPen_EndTransparencyLayer(struct VMGlobals *g, int numArgsPushed)
{

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3

	if (!g->canCallOS)
		return errCantCallOS;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	CGContextEndTransparencyLayer((CGContextRef)[gctx graphicsPort]);

#endif

	return errNone;
}

int prPen_LineJoin(struct VMGlobals *g, int numArgsPushed);
int prPen_LineJoin(struct VMGlobals *g, int numArgsPushed)
{
	int mode=0, err;

	if (!g->canCallOS)
		return errCantCallOS;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	PyrSlot *b  = g->sp; // mode
	if((err = slotIntVal(b, &mode)) != noErr)
		return err;

	//kCGLineJoinMiter=0, kCGLineJoinRound=1,kCGLineJoinBevel=2
	CGContextSetLineJoin(
						 (CGContextRef)[gctx graphicsPort], 
						 (CGLineJoin)sc_clip( mode, kCGLineJoinMiter, kCGLineJoinBevel )
						 );

	return errNone;
}

int prPen_LineDash(struct VMGlobals *g, int numArgsPushed);
int prPen_LineDash(struct VMGlobals *g, int numArgsPushed)
{
	if ( !g->canCallOS )
		return errCantCallOS;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if ( !gctx )
		return errFailed;

	PyrSlot *a  = g->sp; // array of floats
	PyrObject*	pyrarray = slotRawObject(a);
	PyrClass*	pyrclass = pyrarray->classptr;

	if( pyrclass == class_floatarray )  // float array only
	{
		ColorData *colors = (ColorData *) malloc(pyrarray->size*sizeof(ColorData));
		int i;
		float *src = ((PyrFloatArray *)pyrarray)->f;
		for (i=0; i<pyrarray->size; i++)
		{
			colors[i] = (ColorData) *(src++);
		}
		CGContextSetLineDash(
			(CGContextRef)[gctx graphicsPort],
			0,
			colors,
			pyrarray->size
		);
		free(colors);
		return errNone;
	}
	else
	{
		post("Pen line dash needs a FloatArray as argument !\n");
	}

	return errFailed;
}

int prPen_LineCap(struct VMGlobals *g, int numArgsPushed);
int prPen_LineCap(struct VMGlobals *g, int numArgsPushed)
{
	if ( !g->canCallOS )
		return errCantCallOS;

	int err=noErr, mode = 0;
	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if ( !gctx )
		return errFailed;

	PyrSlot *b  = g->sp; // mode
	if( (err = slotIntVal(b, &mode)) != noErr )
		return err;

	//kCGLineCapButt (default - 0), kCGLineCapRound - 1, or kCGLineCapSquare - 2
	CGContextSetLineCap( 
						(CGContextRef)[gctx graphicsPort], 
						(CGLineCap)sc_clip(mode, kCGLineCapButt, kCGLineCapSquare)
						);
	return errNone;
}

int prPen_SetAlpha(struct VMGlobals *g, int numArgsPushed);
int prPen_SetAlpha(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS)
		return errCantCallOS;

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	int err	= errNone; float value;
	PyrSlot *a  = g->sp; // float

	err = slotFloatVal(a, &value);
	if(err != errNone)
		return err;

	CGContextSetAlpha((CGContextRef)[gctx graphicsPort], value);

	return errNone;
}


/*
*	Gradient Functions
*	Valid only for MAC OS X 10.2 -) use the CGShading object
*/

#if MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2

typedef struct GradientInfo {
	SCColor	color0;
	SCColor color1;
	int		scCallback; // future plan for custom gradient calc functions
} GradientInfo, GradientInfoRef;

static GradientInfo gGradientInfo;

static void					GradientCalcLinear(void *info, const ColorData *in, ColorData *out)
{
    size_t k;
	/*
	if(gGradientInfo.scCallback) {
		for (k = 0; k < 4; k++) {
			*out++ = // result of Pen.gradientCalcFunc call;
		}
	}else{
	*/
		for (k = 0; k < 4; k++) {
			*out++ = (ColorData)(((float *)(&gGradientInfo.color1))[k]) * (*in) + (ColorData)(((float *)(&gGradientInfo.color0))[k]) * (1-(*in));
		}
	//}
}

static CGFunctionRef		CreateGradientFunctionRef()
{
	size_t components;
    static const ColorData input_value_range [2] = { 0, 1 };
    static const ColorData output_value_ranges [8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
    static const CGFunctionCallbacks callbacks = { 0,
									&GradientCalcLinear,
									NULL};


	/*
	Note: (colorspace = RGBA == 4)
	-------------------------
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    components = 1 + CGColorSpaceGetNumberOfComponents (colorSpace);
	CGColorSpaceRelease(colorSpace);
	*/

	// initialize gradient info
	gGradientInfo.color0 = SCMakeColor(1.f,1.f,1.f,1.f);
	gGradientInfo.color1 = SCMakeColor(0.f,0.f,0.f,1.f);
	gGradientInfo.scCallback = false;

    return CGFunctionCreate ((void *) &gGradientInfo,
                                1,
                                input_value_range,
                                4, /* RGBA */
                                output_value_ranges,
                                &callbacks);
}

/*
static CGFunctionRef		gGradientFunctionRef = NULL;
static CGFunctionRef		GetGradientFunction()
{
	if(!gGradientFunctionRef) {
		post("creating gradient function !\n");
		gGradientFunctionRef = CreateGradientFunctionRef();
	}
	return gGradientFunctionRef;
}
*/

int prPen_DrawAxialGradient(struct VMGlobals *g, int numArgsPushed);
int prPen_DrawAxialGradient(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	int err;
	NSPoint points[2];

	PyrSlot *a = g->sp - 3; //	point 1
    PyrSlot *b = g->sp - 2; //	point 2
	PyrSlot *c0= g->sp - 1; //	color 1
	PyrSlot *c1= g->sp; //	color 2

	if (!(isKindOfSlot(a, s_point->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(c0, s_color->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(c1, s_color->u.classobj))) return errWrongType;

	CGFunctionRef gradFunc = CreateGradientFunctionRef(); // we need to create a new function ref since Quartz cache it
	if(!gradFunc) {
		post("Error: Failed creating gradient function !\n");
		return errFailed;
	}

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	err = slotColorVal(c0, &(gGradientInfo.color0));
	if( err != errNone )
		return err;

	err = slotColorVal(c1, &(gGradientInfo.color1));
	if( err != errNone )
		return err;

	err = slotGetPoint(a, &points[0]);
	if( err != errNone )
		return err;

	err = slotGetPoint(b, &points[1]);
	if( err != errNone )
		return err;

	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGShadingRef shadingRef = CGShadingCreateAxial(
		colorSpace,
		*(CGPoint*)(&points[0]), *(CGPoint*)(&points[1]),
		gradFunc, true, true
	);

	CGContextSaveGState((CGContextRef)[gctx graphicsPort]);
	CGContextClip((CGContextRef)[gctx graphicsPort]);
	CGContextDrawShading(
		(CGContextRef)[gctx graphicsPort], shadingRef
	);
	CGContextRestoreGState((CGContextRef)[gctx graphicsPort]);

	CGColorSpaceRelease(colorSpace);
	CGShadingRelease(shadingRef);
	CGFunctionRelease(gradFunc);

	return errNone;
}

int prPen_DrawRadialGradient(struct VMGlobals *g, int numArgsPushed);
int prPen_DrawRadialGradient(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	int		err;
	NSPoint points[2];
	float	radius[2] = {0.f, 0.f};

	PyrSlot *a = g->sp - 5; //	point 1
    PyrSlot *b = g->sp - 4; //	point 2
	PyrSlot *srad = g->sp - 3; //	start radius
	PyrSlot *erad = g->sp - 2; //	end radius
	PyrSlot *c0= g->sp - 1; //	color 1
	PyrSlot *c1= g->sp; //	color 2


	if (!(isKindOfSlot(a, s_point->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(b, s_point->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(c0, s_color->u.classobj))) return errWrongType;
	if (!(isKindOfSlot(c1, s_color->u.classobj))) return errWrongType;

	CGFunctionRef gradFunc = CreateGradientFunctionRef();
	if(!gradFunc) {
		NSLog(@"Error: prPen_DrawRadialGradient Failed creating gradient function !\n");
		return errFailed;
	}

	NSGraphicsContext *gctx = [NSGraphicsContext currentContext];
	if (!gctx)
		return errFailed;

	err = slotColorVal(c0, &(gGradientInfo.color0));
	if( err != errNone )
		return err;

	err = slotColorVal(c1, &(gGradientInfo.color1));
	if( err != errNone )
		return err;

	err = slotGetPoint(a, &points[0]);
	if( err != errNone )
		return err;

	err = slotGetPoint(b, &points[1]);
	if( err != errNone )
		return err;

	err = slotFloatVal(srad, &radius[0]);
	if( err != errNone )
		return err;

	err = slotFloatVal(erad, &radius[1]);
	if( err != errNone )
		return err;

	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
	CGShadingRef shadingRef = CGShadingCreateRadial(
		colorSpace,
		*(CGPoint*)(&points[0]), radius[0],
		*(CGPoint*)(&points[1]), radius[1],
		gradFunc, true, true
	);

	CGContextSaveGState( (CGContextRef)[gctx graphicsPort] );
	CGContextClip( (CGContextRef)[gctx graphicsPort] );
	CGContextDrawShading(
		(CGContextRef)[gctx graphicsPort], shadingRef
	);
	CGContextRestoreGState( (CGContextRef)[gctx graphicsPort] );

	CGColorSpaceRelease(colorSpace);
	CGShadingRelease(shadingRef);
	CGFunctionRelease(gradFunc);
	return errNone;
}
#else
// Dummy primitives if mac os x < 10.2 // should never be the case now... but
int prPen_DrawAxialGradient(struct VMGlobals *g, int numArgsPushed);
int prPen_DrawAxialGradient(struct VMGlobals *g, int numArgsPushed) {return errNone;}
int prPen_DrawRadialGradient(struct VMGlobals *g, int numArgsPushed);
int prPen_DrawRadialGradient(struct VMGlobals *g, int numArgsPushed) {return errNone;}
#endif

/*
*	End Of Gradient Functions
*/

int prLoadUserPanel(struct VMGlobals *g, int numArgsPushed);
int prLoadUserPanel(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;

    PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;

    if (!isKindOfSlot(b, class_string)) return errWrongType;
    PyrString* string = slotRawString(b);

    NSString *nsstring = [NSString stringWithCString: string->s length: string->size];
    [nsstring retain];

    void *scobj = slotRawObject(a);
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


int prNewMenuItem(struct VMGlobals *g, int numArgsPushed);
int prNewMenuItem(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *caller = g->sp - 4;		// the SC Object
	PyrSlot *parentObj = g->sp - 3;	// the parent menu or nil for app menu
	PyrSlot *ind = g->sp - 2;			// the index
	PyrSlot *stringSlot = g->sp - 1;	// name of the menu item
	PyrSlot *submenbool = g->sp;		// submenu bool

	bool nilIndex;
	int menIndex, err;

	nilIndex = IsNil(ind);
	if(!nilIndex){
		err = slotIntVal(ind, &menIndex);
		if (err) return err;
	}

	//NSLog(@"menIndex: %i", menIndex);

	if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;

	PyrString* string = slotRawString(stringSlot);

	NSString *label = [[NSString alloc] initWithCString: string->s length: string->size];

	//NSLog(@"Label: %@", label);
	// make a menu item
	SCNSMenuItem *menuItem = [[SCNSMenuItem alloc] initWithTitle:label action:@selector(doAction:) keyEquivalent:@""];
	[menuItem setTarget:menuItem];
	[menuItem setSCObject: slotRawObject(caller)];
	SetPtr(slotRawObject(caller)->slots + 0, menuItem); // set the dataptr

	if (IsTrue(submenbool)) {
		// make a submenu if needed
		//NSLog(@"Is SubMenu");
		[menuItem setSubmenu:[[NSMenu alloc] initWithTitle:label]];
		[[menuItem submenu] setAutoenablesItems:NO];
	}

	NSMenu *parentMenu;
	if (IsNil(parentObj)) {
		// make a submenu if needed
		//NSLog(@"Top Level");
		parentMenu = [NSApp mainMenu];
	} else if (IsSym(parentObj)) {
		PyrSymbol *parentSymbol;
		int err = slotSymbolVal(parentObj, &parentSymbol);
		if (err) return err;
		if (strcmp(parentSymbol->name, "Help")==0) {
			parentMenu = [[[NSApp mainMenu] itemWithTitle:NSLocalizedString(@"Help", @"Help Menu Name")] submenu];
		}
	} else if (isKindOfSlot(parentObj, getsym("CocoaMenuItem")->u.classobj)){
		SCNSMenuItem *parentItem = (SCNSMenuItem*)(slotRawPtr(slotRawObject(parentObj)->slots));
		if([parentItem hasSubmenu]) {
			//NSLog(@"Parent has submenu");
			parentMenu = [parentItem submenu];
		}
	}

	if(!parentMenu) {
		[label release];
		return errWrongType;
	}

	// protect against bad indices
	int parentSize = [parentMenu numberOfItems];
	//NSLog(@"parentSize: %i", parentSize);
	if(menIndex > parentSize || nilIndex) {
		//NSLog(@"Index > parentSize");
		menIndex = parentSize;
	}
	if(menIndex < 0) {
		//NSLog(@"Index < 0");
		menIndex = 0;
	}
	//NSLog(@"Final Index: %i", menIndex);

	[parentMenu insertItem:menuItem atIndex:menIndex];
	[menuItem retain];
	if([menuItem hasSubmenu]) { [[menuItem submenu] release]; } // this has been retained
	[label release];
	return errNone;
}

int prNewMenuSeparator(struct VMGlobals *g, int numArgsPushed);
int prNewMenuSeparator(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *caller = g->sp - 2;		// the SC Object
	PyrSlot *parentObj = g->sp - 1;	// the parent menu or nil for app menu
	PyrSlot *ind = g->sp;			// the index

	bool nilIndex;
	int menIndex, err;

	nilIndex = IsNil(ind);
	if(!nilIndex){
		err = slotIntVal(ind, &menIndex);
		if (err) return err;
	}

	// make a menu item
	NSMenuItem *menuItem = [NSMenuItem separatorItem];
	SetPtr(slotRawObject(caller)->slots + 0, menuItem); // set the dataptr

	NSMenu *parentMenu;
	if (IsNil(parentObj)) {
		parentMenu = [NSApp mainMenu];
	} else if (IsSym(parentObj)) {
		PyrSymbol *parentSymbol;
		int err = slotSymbolVal(parentObj, &parentSymbol);
		if (err) return err;
		if (strcmp(parentSymbol->name, "Help")==0) {
			parentMenu = [[[NSApp mainMenu] itemWithTitle:NSLocalizedString(@"Help", @"Help Menu Name")] submenu];
		}
	} else if (isKindOfSlot(parentObj, getsym("CocoaMenuItem")->u.classobj)){
		SCNSMenuItem *parentItem = (SCNSMenuItem*)(slotRawPtr(slotRawObject(parentObj)->slots));
		if([parentItem hasSubmenu]) {
			//NSLog(@"Parent has submenu");
			parentMenu = [parentItem submenu];
		}
	}

	if(!parentMenu) {
		return errWrongType;
	}

	// protect against bad indices
	int parentSize = [parentMenu numberOfItems];
	//NSLog(@"parentSize: %i", parentSize);
	if(menIndex > parentSize || nilIndex) {
		//NSLog(@"Index > parentSize");
		menIndex = parentSize;
	}
	if(menIndex < 0) {
		//NSLog(@"Index < 0");
		menIndex = 0;
	}
	//NSLog(@"Final Index: %i", menIndex);

	[parentMenu insertItem:menuItem atIndex:menIndex];
	[menuItem retain];
	return errNone;
}


int prRemoveMenuItem(struct VMGlobals *g, int numArgsPushed);
int prRemoveMenuItem(struct VMGlobals *g, int numArgsPushed)
{

	PyrSlot *caller = g->sp;

	NSMenuItem *menuItem = (NSMenuItem*)(slotRawPtr(slotRawObject(caller)->slots));
	if(!menuItem) return errFailed;
	[[menuItem menu] removeItem:menuItem];
	SetPtr(slotRawObject(caller)->slots, NULL); // we're now a lame duck
	//if([menuItem hasSubmenu]) { [[menuItem submenu] release] };
	[menuItem release];
	return errNone;
}

int prEnableMenuItem(struct VMGlobals *g, int numArgsPushed);
int prEnableMenuItem(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *caller = g->sp - 1;
	PyrSlot *boolean = g->sp;

	SCNSMenuItem *menuItem = (SCNSMenuItem*)(slotRawPtr(slotRawObject(caller)->slots));
	if(!menuItem) return errFailed;
	if(IsTrue(boolean)) {
		[menuItem setEnabled:YES];
	} else {
		[menuItem setEnabled:NO];
	}
	return errNone;
}

int prSetMenuItemState(struct VMGlobals *g, int numArgsPushed);
int prSetMenuItemState(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *caller = g->sp - 1;
	PyrSlot *boolean = g->sp;

	SCNSMenuItem *menuItem = (SCNSMenuItem*)(slotRawPtr(slotRawObject(caller)->slots));
	if(!menuItem) return errFailed;
	if(IsTrue(boolean)) {
		[menuItem setState:NSOnState];
	} else {
		[menuItem setState:NSOffState];
	}
	return errNone;
}

int prSetMenuItemKeyboardEquivalent(struct VMGlobals *g, int numArgsPushed);
int prSetMenuItemKeyboardEquivalent(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;
	PyrSlot *caller = g->sp - 3;
	PyrSlot *stringSlot = g->sp - 2;
	PyrSlot *alt = g->sp - 1;
	PyrSlot *ctrl = g->sp;

	SCNSMenuItem *menuItem = (SCNSMenuItem*)(slotRawPtr(slotRawObject(caller)->slots));
	if(!menuItem) return errFailed;

	if (!(isKindOfSlot(stringSlot, class_string))) return errWrongType;

	PyrString* string = slotRawString(stringSlot);

	NSString *kbEquiv = [[NSString alloc] initWithCString: string->s length: string->size];

	[menuItem setKeyEquivalent:kbEquiv];

	int mod = NSCommandKeyMask;
	if(IsTrue(alt)) {
		mod = mod | NSAlternateKeyMask;
	}

	if(IsTrue(ctrl)) {
		mod = mod | NSControlKeyMask;
	}

	[menuItem setKeyEquivalentModifierMask:mod];

	[kbEquiv release];
	return errNone;
}

int prMouse_getCoords(struct VMGlobals *g, int numArgsPushed);
int prMouse_getCoords(struct VMGlobals *g, int numArgsPushed)
{
    if (!g->canCallOS) return errCantCallOS;
	
	PyrSlot *a = g->sp - 1;
    PyrSlot *b = g->sp;
	
	NSPoint mouse_location = [NSEvent mouseLocation];
    PyrSlot *slots = slotRawObject(b)->slots;
    SetFloat( slots+0, mouse_location.x );
    SetFloat( slots+1, mouse_location.y );
	slotCopy(a, b);


    return errNone;
}

int prRecompile(struct VMGlobals *g, int numArgsPushed);
int prRecompile(struct VMGlobals *g, int numArgsPushed)
{
	SEL sel = @selector(compileLibrary:);
    NSMethodSignature *sig = [SCVirtualMachine instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: scvm];
    [anInvocation setSelector: sel];
    [scvm defer: anInvocation];

	return errNone;
}

int prSetDeferredTaskInterval(struct VMGlobals *g, int numArgsPushed);
int prSetDeferredTaskInterval(struct VMGlobals *g, int numArgsPushed)
{
	if (!g->canCallOS) return errCantCallOS;

	PyrSlot *intv = g->sp;

	int err;
	float interval;

	err = slotFloatVal(intv, &interval);
	if (err) return err;
	SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
	[scvm setDeferredTaskInterval: interval];

	return errNone;
}

void initSCImagePrimitives();

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


	//reset scobjects in mydocument
	//_TextWindow_ primitves by jan.t
	NSDocumentController *docctl = [NSDocumentController sharedDocumentController];
	if (docctl) {
		int num = [[docctl documents] count];
        for(int i=0; i<num; i++){
			MyDocument * doc = [[docctl documents] objectAtIndex: i];
			[doc setSCObject: nil];
		}
	}

	definePrimitive(base, index++, "_TextWindow_ToFront", prTextWindow_ToFront, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SelectedText", prTextWindow_SelectedText, 1, 0);
	definePrimitive(base, index++, "_TextWindow_InsertText", prTextWindow_InsertText, 2, 0);
	definePrimitive(base, index++, "_TextWindow_InsertTextInRange", prTextWindow_InsertTextInRange, 4, 0);
	definePrimitive(base, index++, "_TextWindow_GetByIndex", prTextWindow_GetByIndex, 2, 0);
	definePrimitive(base, index++, "_NumberOfOpenTextWindows", prNumberOfOpenTextWindows, 1, 0);
	definePrimitive(base, index++, "_TextWindow_GetLastIndex", prTextWindow_GetLastIndex, 1, 0);
	definePrimitive(base, index++, "_TextWindow_GetName", prTextWindow_GetName, 1, 0);
	definePrimitive(base, index++, "_TextWindow_IsEdited", prTextWindow_IsEdited, 1, 0);
	definePrimitive(base, index++, "_TextWindow_Close", prTextWindow_Close, 1, 0);
	definePrimitive(base, index++, "_TextWindow_Text", prTextWindow_Text, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SetBackgroundColor", prTextWindow_SetBackgroundColor, 2, 0);
	definePrimitive(base, index++, "_TextWindow_GetBackgroundColor", prTextWindow_GetBackgroundColor, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SetSelectedBackgroundColor", prTextWindow_SetSelectedBackgroundColor, 2, 0);
	definePrimitive(base, index++, "_TextWindow_GetSelectedBackgroundColor", prTextWindow_GetSelectedBackgroundColor, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SetTextColor", prTextWindow_SetTextColor, 4, 0);
	definePrimitive(base, index++, "_TextWindow_SyntaxColorize", prTextWindow_SyntaxColorize, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SelectLine", prTextWindow_SelectLine, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SelectRange", prTextWindow_SelectRange, 3, 0);
	definePrimitive(base, index++, "_TextWindow_TextWithRange", prTextWindow_TextWithRange, 3, 0);
	definePrimitive(base, index++, "_TextWindow_SetFont", prTextWindow_SetFont, 4, 0);
	definePrimitive(base, index++, "_TextWindow_GetIndexOfListener", prTextWindow_GetIndexOfListener, 1, 0);
	definePrimitive(base, index++, "_TextWindow_GetSelectedRangeLocation", prTextWindow_GetSelectedRangeLocation, 1, 0);
	definePrimitive(base, index++, "_TextWindow_GetSelectedRangeLength", prTextWindow_GetSelectedRangeLength, 1, 0);
	definePrimitive(base, index++, "_TextWindow_UnfocusedFront", prTextWindow_UnfocusedFront, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SetBounds", prTextWindow_SetBounds, 2, 0);
	definePrimitive(base, index++, "_TextWindow_GetBounds", prTextWindow_GetBounds, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SetName", prTextWindow_SetName, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SetEditable", prTextWindow_SetEditable, 2, 0);
	definePrimitive(base, index++, "_TextWindow_RemoveUndo", prTextWindow_RemoveUndo, 1, 0);
//	definePrimitive(base, index++, "_TextWindow_SelectToken", prTextWindow_SelectToken, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SelectUnderlinedText", prTextWindow_SelectUnderlinedText, 2, 0);
	definePrimitive(base, index++, "_TextWindow_GetFileName", prTextWindow_GetFileName, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SetFileName", prTextWindow_SetFileName, 2, 0);

	definePrimitive(base, index++, "_TextWindow_UnderlineSelection", prTextWindow_UnderlineSelection, 1, 0);
	definePrimitive(base, index++, "_TextWindow_AlwaysOnTop", prTextWindow_AlwaysOnTop, 2, 0);
	definePrimitive(base, index++, "_TextWindow_IsAlwaysOnTop", prTextWindow_IsAlwaysOnTop, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SetPromptToSave", prTextWindow_SetPromptToSave, 2, 0);
	definePrimitive(base, index++, "_TextWindow_PromptToSave", prTextWindow_PromptToSave, 1, 0);
	definePrimitive(base, index++, "_TextWindow_SetDefaultFont", prTextWindow_SetDefaultFont, 2, 0);
	definePrimitive(base, index++, "_PostWindow_SetTextColor", prSetPostTextColor, 2, 0);
	definePrimitive(base, index++, "_TextWindow_SetSyntaxColorTheme", prSetSyntaxColorTheme, 6, 0);
	definePrimitive(base, index++, "_TextWindow_BalanceParens", prBalanceParens, 1, 0);
	definePrimitive(base, index++, "_TextWindow_UsesAutoInOutdent", prTextWindow_UsesAutoInOutdent, 2, 0);
	definePrimitive(base, index++, "_TextWindow_DefaultUsesAutoInOutdent", prTextWindow_DefaultUsesAutoInOutdent, 2, 0);


///
	definePrimitive(base, index++, "_NewTextWindow", prNewTextWindow, 4, 0);
	definePrimitive(base, index++, "_OpenTextFile", prOpenTextFile, 4, 0);

	definePrimitive(base, index++, "_GetStringFromUser", prGetStringFromUser, 3, 0);
	definePrimitive(base, index++, "_SCWindow_New", prSCWindow_New, 7, 0);
	definePrimitive(base, index++, "_SCWindow_Refresh", prSCWindow_Refresh, 1, 0);
	definePrimitive(base, index++, "_SCWindow_Close", prSCWindow_Close, 1, 0);
	definePrimitive(base, index++, "_SCWindow_Minimize", prSCWindow_Minimize, 1, 0);
	definePrimitive(base, index++, "_SCWindow_ToFront", prSCWindow_ToFront, 1, 0);
	definePrimitive(base, index++, "_SCWindow_BeginFullScreen", prSCWindow_FullScreen, 1, 0);
	definePrimitive(base, index++, "_SCWindow_EndFullScreen", prSCWindow_EndFullScreen, 1, 0);
	definePrimitive(base, index++, "_SCWindow_SetShouldClose", prSCWindow_SetShouldClose, 2, 0);

	definePrimitive(base, index++, "_SCWindow_Show", prSCWindow_Show, 1, 0);
	definePrimitive(base, index++, "_SCWindow_Hide", prSCWindow_Hide, 1, 0);
	definePrimitive(base, index++, "_SCWindow_AlwaysOnTop", prSCWindow_AlwaysOnTop, 2, 0);
	definePrimitive(base, index++, "_SCWindow_AcceptsClickThrough", prSCWindow_AcceptsClickThrough, 2, 0);

	definePrimitive(base, index++, "_SCWindow_SetName", prSCWindow_SetName, 2, 0);
	definePrimitive(base, index++, "_SCWindow_SetAlpha", prSCWindow_SetAlpha, 2, 0);
	definePrimitive(base, index++, "_SCWindow_SetBounds", prSCWindow_SetBounds, 2, 0);
	definePrimitive(base, index++, "_SCWindow_GetBounds", prSCWindow_GetBounds, 2, 0);
	definePrimitive(base, index++, "_SCWindow_GetScreenBounds", prSCWindow_GetScreenBounds, 2, 0);
	definePrimitive(base, index++, "_SCWindow_SetAcceptMouseOver", prSCWindow_SetAcceptMouseOver, 2, 0);
	definePrimitive(base, index++, "_SCWindow_RunModal", prSCWindow_RunModal, 1, 0);
	definePrimitive(base, index++, "_SCWindow_StopModal", prSCWindow_StopModal, 1, 0);
	definePrimitive(base, index++, "_SCWindow_RunModalSheet", prSCWindow_RunModalSheet, 2, 0);
	definePrimitive(base, index++, "_SCWindow_StopModalSheet", prSCWindow_StopModalSheet, 1, 0);
	definePrimitive(base, index++, "_ShowCursor", prShowCursor, 2, 0);

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
	definePrimitive(base, index++, "_Pen_Skew", prPen_Skew, 3, 0);
	definePrimitive(base, index++, "_Pen_Scale", prPen_Scale, 3, 0);
	definePrimitive(base, index++, "_Pen_Rotate", prPen_Rotate, 4, 0);
	definePrimitive(base, index++, "_Pen_SetWidth", prPen_SetWidth, 2, 0);
	definePrimitive(base, index++, "_Pen_SetMatrix", prPen_SetMatrix, 2, 0);
	definePrimitive(base, index++, "_Pen_AddRect", prPen_AddRect, 2, 0);
	definePrimitive(base, index++, "_Pen_AddArc", prPen_AddArc, 5, 0);
	definePrimitive(base, index++, "_Pen_AddWedge", prPen_AddWedge, 5, 0);
	definePrimitive(base, index++, "_Pen_AddAnnularWedge", prPen_AddAnnularWedge, 6, 0);

	definePrimitive(base, index++, "_Pen_BeginPath", prPen_BeginPath, 1, 0);
	definePrimitive(base, index++, "_Pen_ClosePath", prPen_ClosePath, 1, 0);
	definePrimitive(base, index++, "_Pen_MoveTo", prPen_MoveTo, 2, 0);
	definePrimitive(base, index++, "_Pen_LineTo", prPen_LineTo, 2, 0);
	definePrimitive(base, index++, "_Pen_StrokePath", prPen_StrokePath, 1, 0);
	definePrimitive(base, index++, "_Pen_FillPath", prPen_FillPath, 1, 0);
	definePrimitive(base, index++, "_Pen_ClipPath", prPen_ClipPath, 1, 0);

	definePrimitive(base, index++, "_Pen_AddQuadCurve", prPen_AddBezierQuadCurve, 3, 0);
	definePrimitive(base, index++, "_Pen_AddCubicCurve", prPen_AddBezierCubicCurve, 4, 0);
	definePrimitive(base, index++, "_Pen_AddArcToPoint", prPen_AddArcToPoint, 4, 0);
	definePrimitive(base, index++, "_Pen_ClearRect", prPen_ClearRect, 2, 0);
	definePrimitive(base, index++, "_Pen_SetBlendMode", prPen_SetBlend, 2, 0);
	definePrimitive(base, index++, "_Pen_SetShadow", prPen_SetShadow, 4, 0);
	definePrimitive(base, index++, "_Pen_SetAlpha", prPen_SetAlpha, 2, 0);
	definePrimitive(base, index++, "_Pen_BeginTLayer", prPen_BeginTransparencyLayer, 1, 0);
	definePrimitive(base, index++, "_Pen_EndTLayer", prPen_EndTransparencyLayer, 1, 0);
	definePrimitive(base, index++, "_Pen_DrawPath", prPen_DrawPath, 2, 0);
	definePrimitive(base, index++, "_Pen_LineJoin", prPen_LineJoin, 2, 0);
	definePrimitive(base, index++, "_Pen_LineDash", prPen_LineDash, 2, 0);
	definePrimitive(base, index++, "_Pen_LineCap", prPen_LineCap, 2, 0);
	definePrimitive(base, index++, "_Pen_AddOval", prPen_AddOval, 2, 0);
	definePrimitive(base, index++, "_Pen_DrawAxialGradient", prPen_DrawAxialGradient, 5, 0);
	definePrimitive(base, index++, "_Pen_DrawRadialGradient", prPen_DrawRadialGradient, 7, 0);

	definePrimitive(base, index++, "_HasDeferred", prHasDeferred, 1, 0);
	definePrimitive(base, index++, "_LoadUserPanel", prLoadUserPanel, 2, 0);

	definePrimitive(base, index++, "_Font_AvailableFonts", prFont_AvailableFonts, 1, 0);
	definePrimitive(base, index++, "_Font_SetAntiAliasing", prFont_SetAntiAliasing, 2, 0);
	definePrimitive(base, index++, "_Font_SetSmoothing", prFont_SetSmoothing, 2, 0);

	definePrimitive(base, index++, "_TextWindow_LinkAtClickPos", prTextWindow_LinkAtClickPos, 2, 0);
	definePrimitive(base, index++, "_Pen_SetSmoothing", prPen_SetSmoothing, 2, 0);

	definePrimitive(base, index++, "_NewMenuItem", prNewMenuItem, 5, 0);
	definePrimitive(base, index++, "_NewMenuSeparator", prNewMenuSeparator, 3, 0);
	definePrimitive(base, index++, "_RemoveMenuItem", prRemoveMenuItem, 1, 0);
	definePrimitive(base, index++, "_EnableMenuItem", prEnableMenuItem, 2, 0);
	definePrimitive(base, index++, "_SetMenuItemState", prSetMenuItemState, 2, 0);
	definePrimitive(base, index++, "_SetMenuItemKeyboardEquivalent", prSetMenuItemKeyboardEquivalent, 4, 0);
	definePrimitive(base, index++, "_Mouse_getCoords", prMouse_getCoords, 2, 0);

	definePrimitive(base, index++, "_Recompile", prRecompile, 1, 0);
	definePrimitive(base, index++, "_SetDeferredTaskInterval", prSetDeferredTaskInterval, 2, 0);

	initSCImagePrimitives();
}

