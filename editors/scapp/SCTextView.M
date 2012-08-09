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

#import "SCTextView.h"
#import "MyDocument.h"
#import "SCCocoaView.h"
#import "SC_DirUtils.h"
#include "SCBase.h"
#include "PyrSymbol.h"
#include <pthread.h>

extern bool compiledOK;
//*)acceptableDragTypes
@implementation SCTextView

- (id)initWithFrame:(NSRect)frameRect {
	useAutoInOutdent = false;
	return [super initWithFrame:frameRect];
}
			
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

- (BOOL)becomeFirstResponder
{
	BOOL accept = [super becomeFirstResponder];
	if(accept) [[self delegate] setActiveTextView: self];
	return accept;
}

- (void) setUsesAutoInOutdent: (bool) flag
{
	useAutoInOutdent = flag;
}

- (bool) usesAutoInOutdent
{
	return useAutoInOutdent;
}

- (void) autoIndent: (NSEvent*) event
{
	unichar c, d;
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
	d = [string characterAtIndex: --pos]; // did a newline actually get inserted? (maybe not if using foreign language input mode)
	c = d;
	
	// autoindent
	if(useAutoInOutdent) {
		for (; pos > 0;) {
			d = [string characterAtIndex: --pos];
			if(d == '{') {
				spaces[nspaces++] = '\t';
				spaces[nspaces] = 0;
				break;
			}
			if (d == '\r' || d == '\n' || d == '}') break;
		}
	}
			
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
        unsigned long lineStart, lineEnd;
        [string getLineStart: (NSUInteger*)&lineStart end: (NSUInteger*)&lineEnd 
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
		PyrObject * pobj = (PyrObject*)[[self delegate] getSCObject];
		if (pobj) {
			pthread_mutex_lock (&gLangMutex);
			PyrSymbol *documentclass = getsym([langClassToCall cStringUsingEncoding:[NSString defaultCStringEncoding]]);
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
			
		PyrObject * pobj = (PyrObject*)[[self delegate] getSCObject];

		if (pobj) {
			pthread_mutex_lock (&gLangMutex);
			PyrSymbol *documentclass = getsym([langClassToCall cStringUsingEncoding:[NSString defaultCStringEncoding]]);
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
		// shift- or control-return added by hjh
    if ([characters isEqual: @"\03"] ||
			(([characters isEqual: @"\n"] || [characters isEqual: @"\r"])
				&& ([event modifierFlags] & (NSControlKeyMask | NSShiftKeyMask)))) {
        [[self delegate] executeSelection: self];
    } else if (([characters isEqual: @"\n"] || [characters isEqual: @"\r"]) && !([event modifierFlags] & NSAlternateKeyMask)) {
        [self autoIndent: event];
	} else { 
        //call lang
		if([[self delegate] handleKeyDown: event]) return;
		if(ignoreControlKeys && ([event modifierFlags] & NSCommandKeyMask)) return;
		[super keyDown: event];
		
		if ([characters isEqual: @"}"] && useAutoInOutdent) {
			[self outdentBrack];
		}
    }
}

- (void)outdentBrack
{
	NSString *string = [self string];
	NSRange range = [self selectedRange];
	int pos = range.location - 1;
	int nspaces = 0;
	unichar c;
	
	// make sure I have only whitespace before me
	for (; pos > 0;) {
		c = [string characterAtIndex: --pos];
		if (c == '\r' || c == '\n') break;
		if ((c == '\t' || c == ' ') && nspaces < 126) nspaces++;
		else return;
	}
	if(nspaces == 0) return; // bail
	
	// okay now see if I have a matching bracket
	unsigned int start, end;
	start = end = range.location - 1;
	int length = [string length];
	unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
	[string getCharacters: buffer];
    
    if(!blankUnparsedChars(buffer, end, false))
    	blankUnparsedChars(buffer, length, true);
    			
    bool res = matchBraks(&start, &end, buffer, length, '}', false);
	
	if(!res) return; // bail
	unichar spaces[128];
	nspaces = 0;
	
	for (; start > 0;) {
		c = [string characterAtIndex: --start];
		if (c == '\r' || c == '\n') break;
		if ((c == '\t' || c == ' ') && nspaces < 126) spaces[nspaces++] = c;
		else nspaces = 0;
	}
	
	c = [string characterAtIndex: pos-1]; 
	range = NSMakeRange(pos + 1, range.location - (pos + 2));
	
//	if (nspaces && (c == '\r' || c == '\n')) {
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
//	}
}

- (IBAction) executeSelection: (id) sender {
	[[self delegate] executeSelection: sender];
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
			
            if(!blankUnparsedChars(buffer, end, false))
           		blankUnparsedChars(buffer, length, true);
           
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
	PyrObject * pobj = (PyrObject*)[[self delegate] getSCObject];
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

#define GETTEXTCHAR(pos, text, textlen)	(((int)pos<0) ? 0 : (((int)pos>=(int)textlen) ? 0 : text[pos]))
#define MAXBRAX 256
unichar braks[MAXBRAX];
int brakptr = 0;

bool checkBraks(unsigned int startpos, unsigned int endpos, unichar *text, int length);
bool checkBraks(unsigned int startpos, unsigned int endpos, unichar *text, int length)
{
	unsigned int pos;
	unichar c;
	
	brakptr = 0;
	pos = startpos;
	for (; pos < endpos; ++pos) {
		c = GETTEXTCHAR(pos, text, length);
		if (c == 0) return false;
		
		if (c == '(') {
			if (brakptr+1 < MAXBRAX) {
				braks[brakptr++] = ')';
			} else return false;
		} else if (c == '[') {
			if (brakptr+1 < MAXBRAX) {
				braks[brakptr++] = ']';
			} else return false;
		} else if (c == '{') {
			if (brakptr+1 < MAXBRAX) {
				braks[brakptr++] = '}';
			} else return false;
		} else if (c == ')' || c == ']' || c == '}') {
			if (brakptr > 0) {
				if (braks[--brakptr] != c) return false;
			}
		} 
	}
	return brakptr == 0;
}

//bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, unichar rightBrak, bool ignoreImmediateParens);
bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, unichar rightBrak, bool ignoreImmediateParens)
{
    unichar c, d;
    
    // check selection internally
    if (!rightBrak && *endpos > *startpos && !checkBraks(*startpos, *endpos, text, length)) return false;	
    
    c = GETTEXTCHAR(((*startpos)-1), text, length);
    d = GETTEXTCHAR(*endpos, text, length);
    
    if (ignoreImmediateParens) {
            if ((c == '(' || c == '[' || c == '{') && (d == ')' || d == ']' || d == '}')) {
                    // if selection is bounded by brackets but they do not match then fail
                    if (!((c == '(' && d == ')') || (c == '[' && d == ']') || (c == '{' && d == '}'))) {
                            return false;
                    } else {
                            // else expand selection by one before searching for next outer pair
                            --(*startpos);
                            ++(*endpos);
                    }
            }
    }
    
    brakptr = 0;
	if (rightBrak) {
		d = rightBrak;
	}

	do {
			--(*startpos);
			c = GETTEXTCHAR(*startpos, text, length);
			if (c == ')') {
					if (brakptr+1 < MAXBRAX) {
							braks[brakptr++] = '(';
					} else return false;
			} else if (c == ']') {
					if (brakptr+1 < MAXBRAX) {
							braks[brakptr++] = '[';
					} else return false;
			} else if (c == '}') {
					if (brakptr+1 < MAXBRAX) {
							braks[brakptr++] = '{';
					} else return false;
			} else if (c == '(' || c == '[' || c == '{') {
					if (brakptr > 0) {
							if (braks[--brakptr] != c) return false;
					} else break;
			} 
	} while (c);
	if (c == 0) return false;
	
	if (!rightBrak) {
		do {
				d = GETTEXTCHAR(*endpos, text, length);
				(*endpos)++;
				if (d == '(') {
						if (brakptr+1 < MAXBRAX) {
								braks[brakptr++] = ')';
						} else return false;
				} else if (d == '[') {
						if (brakptr+1 < MAXBRAX) {
								braks[brakptr++] = ']';
						} else return false;
				} else if (d == '{') {
						if (brakptr+1 < MAXBRAX) {
								braks[brakptr++] = '}';
						} else return false;
				} else if (d == ')' || d == ']' || d == '}') {
						if (brakptr > 0) {
								if (braks[--brakptr] != d) return false;
						} else break;
				} 
		} while (d);
		if (d == 0) return false;
	}
	
    if (!((c == '(' && d == ')') || (c == '[' && d == ']') || (c == '{' && d == '}'))) {
            return false;
    }
	
	if (!rightBrak) {
		// success. shrink selection by one.
		++(*startpos);
		--(*endpos);
	}
    
    return true;
}

bool blankUnparsedChars(unichar* buffer, int length, bool process)
{
	unsigned int i;
	unichar c;
	
	bool blankNext = false;
	bool blankThis = false;
	
	bool isString = false;
	bool isSingleLineComment = false;
	bool isMultiLineComment = false;
	bool isSymbol = false;
	
	for(i = 0;i<length;i++) {
		
		c = GETTEXTCHAR(i, buffer, length);
		
		if(blankNext) {
			blankThis = true;
			blankNext = false;
		}
		
		if(!blankThis) {
			
			if(c == '/' && !isString && !isSymbol && !isSingleLineComment && !isMultiLineComment) {
				unichar d = GETTEXTCHAR(i+1,buffer,length);
				if(d == '/')
					isSingleLineComment = true;
				
				if(d == '*')
					isMultiLineComment = true;
			}
			
			if(isSingleLineComment && c == '\n')
				isSingleLineComment = false;
			
			if(isMultiLineComment && c == '*')
				if(GETTEXTCHAR(i+1,buffer,length) == '/')
					isMultiLineComment = false;

			if(c == '\"' && !isSingleLineComment && !isMultiLineComment && !isSymbol)
				isString = !isString;
			
			if(c == '\'' && !isSingleLineComment && !isMultiLineComment && !isString)
				isSymbol = !isSymbol;
				
			if(c == '$')
				blankNext = true;
		}
		
		if(c == '\\')
			blankNext = true;
		
		
		if(process && (isString || isSingleLineComment || isMultiLineComment || isSymbol || blankThis))
		{
			buffer[i] = ' ';
		}	
		
		blankThis = false;	
	}
	
	return blankNext || isString || isSingleLineComment || isMultiLineComment || isSymbol;
}

- (void)balanceParens: (id)sender
{
    NSRange selectedRange = [self selectedRange];
    NSString *string = [self string];

    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    unsigned int start, end;
    start = selectedRange.location;
    end = start + selectedRange.length;
    
    if(!blankUnparsedChars(buffer, end, false))
    	blankUnparsedChars(buffer, length, true);
    
    bool res = matchBraks(&start, &end, buffer, length, 0, true);
    free(buffer);
    if (res) {
        NSRange newSelectedRange = NSMakeRange(start, end - start);
        [self setSelectedRange: newSelectedRange];
    }
}

- (NSArray*)completionsForPartialWordRange:(NSRange)charRange indexOfSelectedItem:(NSInteger*)index
{
	if([self loadCompletionDict]){
		NSString* queryStr = [(NSAttributedString *)[self attributedSubstringFromRange:charRange] string];
		// create an NSArray containing all object names which match the query
		NSMutableArray* completions;
		completions = [[NSMutableArray alloc] init];
		NSEnumerator * enumerator = [completionDict objectEnumerator];
		NSString* element;
		while(element = (NSString*)[enumerator nextObject]){
			//post("%s hasPrefix: %s\n", element, theStr);
			if([element hasPrefix: queryStr]){
				[completions addObject: element];
			}
		}
		//post("completionDict filtered from %u to %u entries\n", [completionDict count], [completions count]);
		// Also append natural-language possibilities:
		[completions addObjectsFromArray: [super completionsForPartialWordRange: charRange indexOfSelectedItem: index]];
		return completions;
	}{
		// no dict file, or failure - pass through to standard non-SC suggestions.
		return [super completionsForPartialWordRange: charRange indexOfSelectedItem: index];
	}
}

- (bool) loadCompletionDict
{
	// if nsarray looks already ready, then return true
	if(completionDict != NULL){
		return true;
	}
	
	char* fpath = (char*)malloc(PATH_MAX);
	if(fpath==NULL) return false;
	
	sc_GetUserAppSupportDirectory(fpath, PATH_MAX);
	strncat(fpath, "/sclang_completion_dict", PATH_MAX);
	
	// if file not exists or not openable, return false
	FILE* fp = fopen(fpath, "r");
	if(fp==NULL){
		free(fpath);
		//post("couldn't open dict file :(\n");
		return false;
	}
	
	// each line in the file becomes an entry in our array
	char* line = (char*)malloc(100);
	completionDict = [[NSMutableArray alloc] init];
	while( fgets(line, 100, fp) != NULL){
		if((line[0] != '\0') && (line[strlen(line) - 1] == '\n')){
			line[strlen(line) - 1] = '\0'; // rm trailing newline
		}
		//post(line);
		[completionDict addObject: [NSString stringWithCString: line encoding: [NSString defaultCStringEncoding]]];
	}
	
	fclose(fp);
	free(fpath);
	free(line);
	//post("completionDict contains %u entries\n", [completionDict count]);
	return true;
}


- (IBAction)openCode:(id)sender
{
    [[self delegate] sendSelection: "openCodeFile"];
}

- (IBAction) showHelpFor: (id) sender
{
	[[self delegate] sendSelection: "showHelp"];
}

- (IBAction)showHelpSearch:(id)sender {
	[[self delegate] sendSelection: "showHelpSearch"];
}

- (IBAction)methodTemplates: (id)sender
{
    [[self delegate] sendSelection: "methodTemplates"];
}

- (IBAction)methodReferences: (id)sender
{
    [[self delegate] sendSelection: "methodReferences"];
}

- (void)print:(id)sender
{
	NSPrintInfo *printInfo = [[[NSPrintInfo sharedPrintInfo] copy] autorelease];
	if( ! printInfo)
		return;
	
	[printInfo setHorizontalPagination: NSFitPagination];
	[printInfo setHorizontallyCentered: YES];
	[printInfo setVerticallyCentered: NO];
	[[NSPrintOperation printOperationWithView:self printInfo:printInfo] runOperation];
		
}

// redirect here to allow SCViews to get Cmd-F
- (void)cmdF:(id)sender {
	[[TextFinder sharedInstance] orderFrontFindPanel:sender];
}

@end
