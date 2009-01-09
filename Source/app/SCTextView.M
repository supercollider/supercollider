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

#import "SCTextView.h"
#import "MyDocument.h"
#include <pthread.h>

extern bool compiledOK;
//*)acceptableDragTypes
@implementation SCTextView
- (void) setLangClassToCall: (NSString*) stringin withKeyDownActionIndex:(int) downIndex withKeyUpActionIndex:(int) upIndex 
{
	langClassToCall = stringin;
	keyDownActionIndex = downIndex;
	keyUpActionIndex = upIndex;
	[langClassToCall autorelease];
}

- (void) setObjectKeyDownActionIndex:(int) upindex setObjectKeyUpActionIndex:(int) downindex
{
	objectKeyDownActionIndex = downindex;
	objectKeyUpActionIndex = upindex;
}

-(BOOL) acceptsFirstResponder
{
	return mAcceptsFirstResponder;
}

-(void) setAcceptsFirstResponder: (BOOL) flag
{
	 mAcceptsFirstResponder = flag;
}

- (void) autoIndent: (NSEvent*) event
{
	unichar c;
	unichar spaces[128];
	int nspaces = 0;
	NSRange range = [self selectedRange];
        
	NSString *string = [self string];
	int pos = range.location;
        
	for (; pos > 0;) {
		c = [string characterAtIndex: --pos];
		if (c == '\r' || c == '\n') break;
		if ((c == '\t' || c == ' ') && nspaces < 126) spaces[nspaces++] = c;
		else nspaces = 0;
	}
 
	[super keyDown: event];
	range = [self selectedRange];
	pos = range.location;
	string = [self string];
	c = [string characterAtIndex: pos-1]; // did a newline actually get inserted? (maybe not if using foreign language input mode)
	
	if (nspaces && (c == '\r' || c == '\n')) {
		spaces[nspaces] = 0;
	
        // reverse the string
        for (int i=0; i<nspaces/2; ++i) {
                c = spaces[i];
                spaces[i] = spaces[nspaces-1-i];
                spaces[nspaces-1-i] = c;
        }
        NSString *newString = [NSString stringWithCharacters: spaces length: nspaces];
        if ([self shouldChangeTextInRange: range replacementString: newString]) {
            [self replaceCharactersInRange: range withString: newString];
            [self didChangeText];
        }
	}
}

- (NSString*)currentlySelectedTextOrLine: (NSRange*) outRange
{
    NSString* string = [self string];
    NSRange selectedRange = [self selectedRange];
    if (selectedRange.length <= 0) {
        unsigned int lineStart, lineEnd;
        [string getLineStart: &lineStart end: &lineEnd 
            contentsEnd: nil forRange:selectedRange];
        selectedRange = NSMakeRange(lineStart, lineEnd - lineStart);
    }
	if (outRange) *outRange = selectedRange;
    return [string substringWithRange: selectedRange];
}

- (NSString*)currentlySelectedText: (NSRange*) outRange
{
    NSString* string = [self string];
    NSRange selectedRange = [self selectedRange];
	if (outRange) *outRange = selectedRange;
    return [string substringWithRange: selectedRange];
}

- (void) keyUp: (NSEvent*) event
{
    NSString *characters = [event characters];
	if(compiledOK){
		unsigned int modifiers = [event modifierFlags];
		unichar character = 0;
		if([characters length] > 0) {
			character = [characters characterAtIndex: 0];
		}
		unsigned int keycode = [event keyCode];
			
		PyrObject * pobj = [[self delegate] getSCObject];
		if (pobj) {
			pthread_mutex_lock (&gLangMutex);
			PyrSymbol *documentclass = getsym([langClassToCall cString]);
			PyrObject *classobj = (PyrObject*) documentclass->u.classobj;		
			if(NotNil(pobj->slots+objectKeyUpActionIndex) || NotNil(classobj->slots+keyUpActionIndex)){
				if(compiledOK){
					PyrSymbol *method = getsym("keyUp");
					VMGlobals *g = gMainVMGlobals;
					g->canCallOS = true;
					++g->sp;  SetObject(g->sp, pobj); 
					++g->sp;  SetChar(g->sp, character); 
					++g->sp;  SetInt(g->sp, modifiers); 
					++g->sp;  SetInt(g->sp, character); 
					++g->sp;  SetInt(g->sp, keycode); 
					runInterpreter(g, method, 5);
					g->canCallOS = false;
				}	
			}
			pthread_mutex_unlock (&gLangMutex);
		} 
	}
    if ([characters isEqual: @"\03"]) {
    } else if (([characters isEqual: @"\n"] || [characters isEqual: @"\r"]) && !([event modifierFlags] & NSAlternateKeyMask)) {
    } else {
		[super keyUp: event];
    }
}

- (void) keyDown: (NSEvent*) event
{
    NSString *characters = [event characters];
	BOOL ignoreControlKeys = NO;
	if(compiledOK){
		unsigned int modifiers = [event modifierFlags];
		unichar character = 0;
		if([characters length] > 0) {
			character = [characters characterAtIndex: 0];
		}
		unsigned int keycode = [event keyCode];
			
		PyrObject * pobj = [[self delegate] getSCObject];

		if (pobj) {
			pthread_mutex_lock (&gLangMutex);
			PyrSymbol *documentclass = getsym([langClassToCall cString]);
			PyrObject *classobj = (PyrObject*) documentclass->u.classobj;			
			if(NotNil(pobj->slots+objectKeyDownActionIndex) || NotNil(classobj->slots+keyDownActionIndex)){
				if(compiledOK){
					PyrSymbol *method = getsym("keyDown");
					VMGlobals *g = gMainVMGlobals;
					g->canCallOS = true;
					++g->sp;  SetObject(g->sp, pobj); 
					++g->sp;  SetChar(g->sp, character); 
					++g->sp;  SetInt(g->sp, modifiers); 
					++g->sp;  SetInt(g->sp, character); 
					++g->sp;  SetInt(g->sp, keycode); 
					runInterpreter(g, method, 5);
					g->canCallOS = false;
				}
			}
			pthread_mutex_unlock (&gLangMutex);
		}
	}
    if ([characters isEqual: @"\03"]) {
        [[self delegate] executeSelection: self];
    } else if (([characters isEqual: @"\n"] || [characters isEqual: @"\r"]) && !([event modifierFlags] & NSAlternateKeyMask)) {
        [self autoIndent: event];
    } else {
        //call lang
		if([[self delegate] handleKeyDown: event]) return;
		if(ignoreControlKeys && ([event modifierFlags] & NSCommandKeyMask)) return;
		[super keyDown: event];
    }
}

bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, unichar rightBrak, bool ignoreImmediateParens);
//-(void)rightMouseDown:(NSEvent*)theEvent { [[self delegate] mouseDown:theEvent]; [super rightMouseDown: theEvent]; }
//-(void)otherMouseDown:(NSEvent*)theEvent {[[self delegate] mouseDown:theEvent]; [super otherMouseDown: theEvent];  }
//-(void) mouseDragged:(NSEvent*)theEvent {[[self delegate] mouseDown:theEvent]; [super mouseDragged: theEvent]; }
-(BOOL) dragSelectionWithEvent:(NSEvent *)event offset:(NSSize)mouseOffset slideBack:(BOOL)slideBack
{
[[self delegate] mouseDown:event];
	[super dragSelectionWithEvent:event offset:mouseOffset slideBack:slideBack];
}
- (void) mouseDown: (NSEvent*) event
{
	NSWindow *window = [self window];
	NSPoint p = [window convertBaseToScreen: [event locationInWindow]];
	int index = [self characterIndexForPoint: p];
    if ([event clickCount] == 2) {
        NSString *string = [self string];
        int length = [string length];
        if (index < 0 || index >= length) { goto below; }
        unichar c = [string characterAtIndex: index];
        if (index > 0 && (c == '\n' || c == '\r')) {
            c = [string characterAtIndex: --index];
        }
        if (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}') {
            unsigned int start, end;
            unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
            [string getCharacters: buffer];
            if (c == '[' || c == '(' || c == '{') {
                    start = end = index + 1;
            } else if (c == ']' || c == ')' || c == '}') {
                    start = end = index;
            }            
            bool res = matchBraks(&start, &end, buffer, length, 0, false);
            free(buffer);
            if (res) {
                NSRange newSelectedRange = NSMakeRange(start, end - start);
                [self setSelectedRange: newSelectedRange];
            }
        } else goto below; 
    } else {
below:

        [super mouseDown: event];
        [self mouseUpAction: event index: index];
		[[self delegate] mouseDown: event];

    }
}

extern PyrSymbol * s_mouseUp;
- (void) mouseUpAction: (NSEvent*) theEvent index: (int) index
{
	if(!compiledOK) {
		return;
	}	
	NSPoint mouseLoc;	
	unsigned int modifiers = [theEvent modifierFlags];
	mouseLoc = [self convertPoint:[theEvent locationInWindow] fromView:nil];
	//	SCPoint scpoint = SCMakePoint(mouseLoc.x, mouseLoc.y);
	int clickCount = [theEvent clickCount];
	int buttonNum = [theEvent buttonNumber];	
	pthread_mutex_lock (&gLangMutex);
	PyrObject * pobj = [[self delegate] getSCObject];
	if (pobj) {
		VMGlobals *g = gMainVMGlobals;
		g->canCallOS = true;
		++g->sp;  SetObject(g->sp, pobj); 
		++g->sp;  SetInt(g->sp, mouseLoc.x); 
		++g->sp;  SetInt(g->sp, mouseLoc.y); 
		++g->sp;  SetInt(g->sp, modifiers); 
		++g->sp;  SetInt(g->sp,buttonNum);
		++g->sp;  SetInt(g->sp,clickCount);
		++g->sp;  SetInt(g->sp,index);
		runInterpreter(g, s_mouseUp, 7);
		g->canCallOS = false;
	}
	pthread_mutex_unlock (&gLangMutex);
}
- (void)setDefaultTabsTo:(float)value {
	NSTextStorage *text = [self textStorage];
	int length = [text length], i = 0;
	NSRange range;
	while(i < length) {
		NSMutableParagraphStyle *style = [[text attribute: NSParagraphStyleAttributeName atIndex: i longestEffectiveRange: &range inRange: NSMakeRange(0, [text length])] mutableCopy];
		if(style) {
			[style setDefaultTabInterval: value];
			[text addAttribute: NSParagraphStyleAttributeName value: style range: range];
			[style release];
		}
		i = i + range.length;
	}
}

// we need to override this because RTFfromRange converts relative links to http scheme links
// this makes a copy of selected text and converts links to absolute file scheme links
// when the target instance of MyDocument is saved it will convert any file scheme links back to relative
- (BOOL)writeSelectionToPasteboard:(NSPasteboard *)pboard type:(NSString *)type {
	BOOL res;
	MyDocument *doc = [[NSDocumentController sharedDocumentController] documentForWindow: [self window]];
	if(([type isEqualToString:NSRTFPboardType] || [type isEqualToString:NSRTFDPboardType]) && doc){
		NSRange range = [self selectedRange];
		NSMutableAttributedString *selectedText = [[[self textStorage] attributedSubstringFromRange: range] mutableCopy];
		range.location = 0;
		NSRange linkRange;
		while (range.length > 0) {
			id link = [selectedText attribute: NSLinkAttributeName 
									 atIndex: range.location 
					   longestEffectiveRange: &linkRange 
									 inRange: range];
			if(linkRange.length<=0) break;
			if (link && [link isKindOfClass: [NSString class]] && (![link hasPrefix:@"SC://"] || ![link hasPrefix:@"sc://"]) && ![link isAbsolutePath]) {
				// convert to a file:// URL
				NSURL *newLink = [NSURL URLWithString: [link stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding] relativeToURL: [doc fileURL]];
				newLink = [newLink absoluteURL];
				[selectedText addAttribute: NSLinkAttributeName value: newLink range: linkRange];
			}
			range = NSMakeRange(NSMaxRange(linkRange), NSMaxRange(range) - NSMaxRange(linkRange));
		}
		if([type isEqualToString:NSRTFPboardType]){
			res = [pboard setData:[selectedText RTFFromRange:NSMakeRange(0, [selectedText length]) documentAttributes:nil] forType:NSRTFPboardType];
		} else if([type isEqualToString:NSRTFDPboardType]) {
			res = [pboard setData:[selectedText RTFDFromRange:NSMakeRange(0, [selectedText length]) documentAttributes:nil] forType:NSRTFDPboardType];
		} else {
			res = NO;
		}
		[selectedText release];
	} else {
		res = [super writeSelectionToPasteboard:pboard type:type];
	}
	return res;
}

- (NSDragOperation)draggingEntered:(id < NSDraggingInfo >)sender {
	NSDragOperation sourceDragMask;
	sourceDragMask = [sender draggingSourceOperationMask];
	
	if(sourceDragMask == NSDragOperationCopy) { // we're holding the alt key
		// block if we haven't been saved or aren't in a document
		if(![[[NSDocumentController sharedDocumentController] documentForWindow: [self window]] fileURL]) {
			return NSDragOperationNone;
		} else { return NSDragOperationCopy; }
	}
	
	return [super draggingEntered:sender]; // pass on to NSTextView
}

NSString* pathOfFileRelativeToBaseDir(NSString *filepath, NSString *baseDir); // from MyDocument.M

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender {
    NSPasteboard *pboard;
    NSDragOperation sourceDragMask;
	
    sourceDragMask = [sender draggingSourceOperationMask];
    pboard = [sender draggingPasteboard];
	
	NSPoint mouseLoc = [[self window] convertBaseToScreen:[sender draggingLocation]];
	unsigned int charIndex = [self characterIndexForPoint:mouseLoc];
	
    if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
		NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
		NSEnumerator *enumerator = [files objectEnumerator];
		id anObject;
		NSString *filesString = @"", *docDir;
		BOOL commaSpace = NO;
		BOOL alt = (sourceDragMask == NSDragOperationCopy);
		// we already checked in draggingEntered if this is in a document
		if(alt) docDir = [[[[[NSDocumentController sharedDocumentController] documentForWindow: [self window]] fileURL] path] stringByDeletingLastPathComponent];
		while (anObject = [enumerator nextObject]) {
			if(commaSpace) filesString = [filesString stringByAppendingString:@", "];
			filesString = [filesString stringByAppendingString:@"\""];
			if(alt) anObject = pathOfFileRelativeToBaseDir(anObject, docDir); // convert to relative
			filesString = [filesString stringByAppendingString:anObject];
			filesString = [filesString stringByAppendingString:@"\""];
			if(alt) filesString = [filesString stringByAppendingString:@".resolveRelative"];
			commaSpace = YES;
		}
		if([files count] > 1) filesString = [[@"[" stringByAppendingString:filesString] stringByAppendingString:@"]"];
		
		if ([self shouldChangeTextInRange:NSMakeRange(charIndex, 0) replacementString:filesString]) {
			[[self textStorage] replaceCharactersInRange:NSMakeRange(charIndex, 0) withString:filesString];
			[self setSelectedRange:NSMakeRange(charIndex, [filesString length])];
			[self didChangeText];
			return YES;
		}
    }
    return [super performDragOperation:sender];
}


- (IBAction)openCode:(id)sender
{
    [[self delegate] sendSelection: "openCodeFile"];
}

- (IBAction)methodTemplates: (id)sender
{
    [[self delegate] sendSelection: "methodTemplates"];
}

- (IBAction)methodReferences: (id)sender
{
    [[self delegate] sendSelection: "methodReferences"];
}

@end

