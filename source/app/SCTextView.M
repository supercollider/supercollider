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

@implementation SCTextView

- (void) autoIndent
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
        spaces[nspaces++] = '\n';
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

- (void) keyDown: (NSEvent*) event
{
    NSString *characters = [event characters];
    if ([characters isEqual: @"\03"]) {
        [[self delegate] executeSelection: self];
    } else if (([characters isEqual: @"\n"] || [characters isEqual: @"\r"]) && !([event modifierFlags] & NSAlternateKeyMask)) {
        [self autoIndent];
    } else {
        [super keyDown: event];
    }
}

bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, bool ignoreImmediateParens);

- (void) mouseDown: (NSEvent*) event
{
    if ([event clickCount] == 2) {
        NSWindow *window = [self window];
        NSPoint p = [window convertBaseToScreen: [event locationInWindow]];
        int index = [self characterIndexForPoint: p];
        NSString *string = [self string];
        int length = [string length];
        if (index < 0 || index > length) { goto below; }
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
            bool res = matchBraks(&start, &end, buffer, length, false);
            free(buffer);
            if (res) {
                NSRange newSelectedRange = NSMakeRange(start, end - start);
                [self setSelectedRange: newSelectedRange];
            }
        } else goto below; 
    } else {
below:
        [super mouseDown: event];
    }
}


@end
