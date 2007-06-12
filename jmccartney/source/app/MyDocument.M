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

#import <Cocoa/Cocoa.h>
#import "MyDocument.h"
#import "SCTextView.h"
#import "SCVirtualMachine.h"
#include "SCBase.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"

bool firstWindow = true;
bool needBlankWindow = true;
extern NSTextView *gPostView;
extern pthread_mutex_t gLangMutex;

@implementation MyDocument

- (NSTextView*)makeTextView
{
    NSTextView* aTextView = [[SCTextView alloc] initWithFrame: 
                    NSMakeRect(0,0,512,512)];
    [aTextView setAutoresizingMask: 63];
    [[aTextView textContainer] setWidthTracksTextView: YES];
    [aTextView setDelegate: self];
    [aTextView setAllowsUndo: YES];
    [aTextView setRichText: YES];
    [aTextView setSmartInsertDeleteEnabled: NO];
    [aTextView setFont: [NSFont fontWithName: @"Monaco" size: 9]];
    return aTextView;
}

- (void)windowControllerDidLoadNib:(NSWindowController*) aController
{
    [super windowControllerDidLoadNib:aController];

    NSSize contentSize;
    contentSize = [scrollView contentSize];
    if (initTextView) {
        textView = initTextView;
    } else {
        textView = [self makeTextView];
    }
    [textView setFrameSize: contentSize];
    [scrollView setDocumentView: textView];
    [textView release];
    [textView setSelectedRange: NSMakeRange(0,0)];

    if (firstWindow) {
        if (initTextView) {
            if (needBlankWindow) {
                [[NSDocumentController sharedDocumentController] newDocument: nil];
                needBlankWindow = false;
            }
        } else {
            firstWindow = false;
            needBlankWindow = false;
            gPostView = textView;
            
            [[SCVirtualMachine sharedInstance] start];
        }
    }
    NSWindow *window = [textView window];
    [window makeKeyWindow];
    [window makeFirstResponder: textView];
}


- (void)openCode:(id)sender
{
    [self sendSelection: "openCodeFile"];
}

- (void)methodTemplates: (id)sender
{
    [self sendSelection: "methodTemplates"];
}

- (void)methodReferences: (id)sender
{
    [self sendSelection: "methodReferences"];
}

#define GETTEXTCHAR(pos, text, textlen)	((pos<0) ? 0 : ((pos>=textlen) ? 0 : text[pos]))
#define MAXBRAX 128
unichar braks[MAXBRAX];
int brakptr = 0;

bool checkBraks(unsigned int startpos, unsigned int endpos, unichar *text, int length);
bool checkBraks(unsigned int startpos, unsigned int endpos, unichar *text, int length)
{
	int pos;
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

bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, bool ignoreImmediateParens);
bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, bool ignoreImmediateParens)
{
    int pos;
    unichar c, d;
    
    // check selection internally
    if (*endpos > *startpos && !checkBraks(*startpos, *endpos, text, length)) return false;	
    
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
    
    if (!((c == '(' && d == ')') || (c == '[' && d == ']') || (c == '{' && d == '}'))) {
            return false;
    }
    // success. shrink selection by one.
    ++(*startpos);
    --(*endpos);
    
    return true;
}


- (void)balanceParens: (id)sender
{
    NSRange selectedRange = [textView selectedRange];
    NSString *string = [textView string];

    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    unsigned int start, end;
    start = selectedRange.location;
    end = start + selectedRange.length;
    bool res = matchBraks(&start, &end, buffer, length, true);
    free(buffer);
    if (res) {
        NSRange newSelectedRange = NSMakeRange(start, end - start);
        [textView setSelectedRange: newSelectedRange];
    }
}

void SyntaxColorize(NSTextView* textView);

- (void)syntaxColorize: (id)sender
{
    SyntaxColorize(textView);
}

- (void) insertText: (char*) text length: (int)length
{
    NSRange selectedRange = [textView selectedRange];
    NSString *string = [[NSString alloc] initWithCString: text length: length];
    if ([textView shouldChangeTextInRange: selectedRange replacementString: string]) {
        [textView replaceCharactersInRange: selectedRange withString: string];
        [textView didChangeText];
    }
    [string release];
}


- (void)shiftLeft: (id)sender
{
   NSTextStorage *textStorage = [textView textStorage];
    NSRange selectedRange = [textView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSAttributedString *tabStr = [[NSAttributedString alloc] initWithString: @"\t"];
    [tabStr autorelease];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    
    if (buffer[0] == NSTabCharacter) {
        [newString deleteCharactersInRange: NSMakeRange(0,1)];
    }
    int j = 0;
    for (int i=0; i<length-1; ++i, ++j) {
        unichar c = buffer[i];
        unichar d = buffer[i+1];
        if (d == NSTabCharacter && (c == NSNewlineCharacter || c == NSCarriageReturnCharacter)) {
            [newString deleteCharactersInRange: NSMakeRange(j,1)];
            j -= 1;
        }
    }    
    free(buffer);
    NSRange newSelectedRange = NSMakeRange(selectedRange.location, j);
    
    if ([textView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [textView setSelectedRange: newSelectedRange];
        [textView didChangeText];
    }
}

- (void)shiftRight: (id)sender
{
   NSTextStorage *textStorage = [textView textStorage];
    NSRange selectedRange = [textView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSAttributedString *tabStr = [[NSAttributedString alloc] initWithString: @"\t"];
    [tabStr autorelease];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    [newString insertAttributedString: tabStr atIndex: 0];
    int j = 1;
    for (int i=0; i<length-1; ++i, ++j) {
        unichar c = buffer[i];
        if (c == NSNewlineCharacter || c == NSCarriageReturnCharacter) {
            [newString insertAttributedString: tabStr atIndex: j+1];
            j += 1;
        }
    }    
    free(buffer);
    NSRange newSelectedRange = NSMakeRange(selectedRange.location, j+1);
    
    if ([textView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [textView setSelectedRange: newSelectedRange];
        [textView didChangeText];
    }
}

- (void)commentCode: (id)sender
{
    NSTextStorage *textStorage = [textView textStorage];
    NSRange selectedRange = [textView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSAttributedString *commentChars = [[NSAttributedString alloc] initWithString: @"//"];
    [commentChars autorelease];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    [newString insertAttributedString: commentChars atIndex: 0];
    int j = 2;
    for (int i=0; i<length-1; ++i, ++j) {
        unichar c = buffer[i];
        if (c == NSNewlineCharacter || c == NSCarriageReturnCharacter) {
            [newString insertAttributedString: commentChars atIndex: j+1];
            j += 2;
        }
    }    
    free(buffer);
    NSRange newSelectedRange = NSMakeRange(selectedRange.location, j+1);
   
    if ([textView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [textView didChangeText];
        [textView setSelectedRange: newSelectedRange];
        SyntaxColorize(textView);
    }
}

- (void)uncommentCode:(id)sender
{
   NSTextStorage *textStorage = [textView textStorage];
    NSRange selectedRange = [textView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSAttributedString *tabStr = [[NSAttributedString alloc] initWithString: @"\t"];
    [tabStr autorelease];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    if (length < 2) return;
    
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    
    int i = 0;
    int j = 0;
    if (buffer[0] == '/' && buffer[1] == '/') {
        [newString deleteCharactersInRange: NSMakeRange(0,2)];
        i += 2;
    }
    for (; i<length-2; ++i, ++j) {
        unichar c = buffer[i];
        unichar d = buffer[i+1];
        unichar e = buffer[i+2];
        if (d == '/' && e == '/' && (c == NSNewlineCharacter || c == NSCarriageReturnCharacter)) {
            [newString deleteCharactersInRange: NSMakeRange(j+1,2)];
            j -= 2;
        }
    }    
    free(buffer);
    NSRange newSelectedRange = NSMakeRange(selectedRange.location, j+2);
    
    if ([textView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [textView didChangeText];
        [textView setSelectedRange: newSelectedRange];
        SyntaxColorize(textView);
    }
    
}

- (IBAction) executeSelection: (id) sender;
{
    [self sendSelection: "interpretPrintCmdLine" ];
}

- (IBAction) showHelp: (id) sender;
{
    [self sendSelection: "showHelp" ];
}

extern bool compiledOK;

- (void)sendSelection: (char*) methodName
{        
    if (!compiledOK) {
        return;
    }

    NSString* string = [textView string];
    NSRange selectedRange = [textView selectedRange];
    NSString* selection;

    if (selectedRange.length <= 0) {
        unsigned int lineStart, lineEnd;
        [string getLineStart: &lineStart end: &lineEnd 
            contentsEnd: nil forRange:selectedRange];
        selectedRange = NSMakeRange(lineStart, lineEnd - lineStart);
    }
    selection = [string substringWithRange: selectedRange];
    const char *text = [selection UTF8String];
    
    [[SCVirtualMachine sharedInstance] setCmdLine: text length: selectedRange.length];
    
    NSRange newSelectedRange = NSMakeRange(selectedRange.location + selectedRange.length, 0);
    [textView setSelectedRange: newSelectedRange];
    
    pthread_mutex_lock(&gLangMutex);
    runLibrary(getsym(methodName));
    pthread_mutex_unlock(&gLangMutex);
}

- (NSString *)windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (BOOL)writeToFile:(NSString*) path ofType:(NSString *)aType
{
    // Insert code here to write your document from the given data.  You can also choose to override -fileWrapperRepresentationOfType: or -writeToFile:ofType: instead.
    //NSString* string = [textView string];
    
    //return [textView RTFDFromRange: NSMakeRange(0, [string length]);
/*    if ([[textView textStorage] containsAttachments]) {
        return [textView writeRTFDToFile: path atomically: YES];
    } else {
        return [textView writeRTFToFile: path atomically: YES];
    }*/

    BOOL success;
    NSTextStorage *textStorage = [textView textStorage];

    NSRange range = NSMakeRange(0, [textStorage length]);
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];

    if ([textStorage containsAttachments]) {
        NSFileWrapper *wrapper = [textStorage RTFDFileWrapperFromRange: range documentAttributes: dict];
        success = wrapper ? [wrapper writeToFile: path atomically:YES updateFilenames: YES] : NO;
    } else {
        NSData *data = [textStorage RTFFromRange: range documentAttributes: dict];
        success = data ? [data writeToFile: path atomically: YES] : NO;
    }
    if (!success) return NO;
/*    
    NSFileManager *fmgr = [NSFileManager defaultManager];

    {
    NSMutableDictionary *attr2 = [fmgr fileAttributesAtPath: path traverseLink: NO];
    NSNumber* creator2 = [attr2 objectForKey: NSFileHFSCreatorCode];
    int creatorval = [creator2 intValue];
    printf("creator was %d   SCjm %d\n", creatorval, 'SCjm');
    }
    
    NSMutableDictionary *attr = [NSMutableDictionary dictionary];
    NSNumber *creator = [NSNumber numberWithInt: 'SCjm'];
    [attr setObject: creator forKey: NSFileHFSCreatorCode];
    success = [fmgr changeFileAttributes: attr atPath: path];
    printf("write success %d\n", success);

    NSMutableDictionary *attr2 = [fmgr fileAttributesAtPath: path traverseLink: NO];
    NSNumber* creator2 = [attr2 objectForKey: NSFileHFSCreatorCode];
    int creatorval = [creator2 intValue];
    printf("creator is %d   SCjm %d\n", creatorval, 'SCjm');
*/
    return success;
}

- (BOOL)readFromFile:(NSString *)path ofType:(NSString *)aType
{
/*
    NSFileManager *fmgr = [NSFileManager defaultManager];
    NSMutableDictionary *attr2 = [fmgr fileAttributesAtPath: path traverseLink: NO];
    NSNumber* creator2 = [attr2 objectForKey: NSFileHFSCreatorCode];
    int creatorval = [creator2 intValue];
    printf("creator is %d   SCjm %d\n", creatorval, 'SCjm');
*/
    NSMutableDictionary *options = [NSMutableDictionary dictionary];
    NSDictionary *docAttrs;
    NSURL *url = [NSURL fileURLWithPath: path];
    NSString *docType;
    id val, viewSizeVal, paperSizeVal;
    BOOL success;
    
    if (!initTextView) initTextView = [self makeTextView];
    NSTextStorage* text = [initTextView textStorage];
    
    [options setObject:url forKey:@"BaseURL"];
    //[options setObject:[NSNumber numberWithUnsignedInt:UnknownStringEncoding] forKey:@"CharacterEncoding"];
    
    // Insert code here to read your document from the given data.  You can also choose to override -loadFileWrapperRepresentation:ofType: or -readFromFile:ofType: instead.

    [text beginEditing];	// Bracket with begin/end editing for efficiency
    [[text mutableString] setString:@""];	// Empty the document
    success = [text readFromURL:url options:options documentAttributes: &docAttrs];	// Read!
    [text endEditing];
 
    return success;
}

- (BOOL) shouldRunSavePanelWithAccessoryView
{
    return NO;
}

- (NSTextView*) textView;
{
    return textView;
}

- (BOOL)windowShouldClose:(id)sender
{
    return (textView != gPostView);
}

- (void)windowWillClose:(NSNotification *)aNotification
{
    if (textView == gPostView) gPostView = nil;
}

- (IBAction) becomePostWindow: (id) sender
{
    gPostView = textView;
}

@end
