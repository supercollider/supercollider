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
    [aTextView setImportsGraphics: YES];
    [aTextView setFont: [NSFont fontWithName: @"Monaco" size: 9]];
	isRichText = YES;
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

bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, bool ignoreImmediateParens);
bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, bool ignoreImmediateParens)
{
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
    [textView didChangeText];
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
		[textView didChangeText];
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
		[textView didChangeText];
    }
    
}

- (IBAction) executeSelection: (id) sender
{
    [self sendSelection: "interpretPrintCmdLine" ];
}

#if 0

- (IBAction) showHelp: (id) sender
{
    [self sendSelection: "showHelp" ];
}

#else

NSString* helpFileWithName(NSFileManager* fileManager, NSString* desiredHelpFile, NSString* extension)
{
    // The place to start looking
    NSString* helpFolderPath = [NSString stringWithFormat: @"%@/Help/",
		    [fileManager currentDirectoryPath]];

    // the name of the help file we are looking for
    NSString* helpFileName = [NSString stringWithFormat: @"%@%@", desiredHelpFile, extension];

    // recurse through this directory until we find it
    NSString* helpFilePath = nil;
    NSDirectoryEnumerator* dirEnumerator = [fileManager enumeratorAtPath:helpFolderPath];
    NSString* candidate;
    while (candidate = [dirEnumerator nextObject]) {
        if([[candidate lastPathComponent] isEqualToString:helpFileName]) {
            helpFilePath = [helpFolderPath stringByAppendingString: candidate];
            break;
        }
    }

    return helpFilePath;
}

- (NSString*)currentlySelectedTextOrLine: (NSRange*) outRange
{
    NSString* string = [textView string];
    NSRange selectedRange = [textView selectedRange];
    if (selectedRange.length <= 0) {
        unsigned int lineStart, lineEnd;
        [string getLineStart: &lineStart end: &lineEnd 
            contentsEnd: nil forRange:selectedRange];
        selectedRange = NSMakeRange(lineStart, lineEnd - lineStart);
    }
	if (outRange) *outRange = selectedRange;
    return [string substringWithRange: selectedRange];
}
-(void)selectRangeStart:(int)rangeStart size:(int)rangeSize
{
    NSTextView *localTextView = [self textView];
    
    NSRange range = NSMakeRange(rangeStart, rangeSize);
    
    if (rangeSize < 0) {
        unsigned int lineStart, lineEnd;
        range.length = 0;
        NSString *nsstring = [localTextView string];
        if (!nsstring) {
            post("text view has no string\n");
            return;
        }
        [nsstring getLineStart: &lineStart end: &lineEnd contentsEnd: nil forRange: range];
        range = NSMakeRange(lineStart, lineEnd - lineStart);
    }
    [localTextView setSelectedRange: range];
    [localTextView scrollRangeToVisible: range];
}
/*
-(void)selectLine:(int)linenum
{
    NSTextView *localTextView = [self textView];
    
    unsigned int lineStart, lineEnd;
    //count the lines ??
    // what separator ?
    
    range.length = 0;
    NSString *nsstring = [localTextView string];
    if (!nsstring) {
        post("text view has no string\n");
        return errFailed;
    }
    [nsstring getLineStart: &lineStart end: &lineEnd contentsEnd: nil forRange: range];
    range = NSMakeRange(lineStart, lineEnd - lineStart);

    [localTextView setSelectedRange: range];
    [localTextView scrollRangeToVisible: range];   
}
*/

- (IBAction) showHelp: (id) sender;
{
    [self showHelpFor:[self currentlySelectedTextOrLine: NULL]];
}

-(NSString*)pathOfHelpFileFor:(NSString*)selection 
{

    NSString* helpFilePath = nil;

    NSFileManager* fileManager = [NSFileManager defaultManager];
    if (!fileManager) return helpFilePath; // == NULL

    helpFilePath = helpFileWithName(fileManager, selection, @".help.rtf");
    if (!helpFilePath) {
		helpFilePath = helpFileWithName(fileManager, selection, @".rtf");
		if (!helpFilePath) {
			helpFilePath = helpFileWithName(fileManager, selection, @".help");
			if (!helpFilePath) {
				helpFilePath = helpFileWithName(fileManager, selection, @".sc");
				//if (!helpFilePath) {
					//helpFilePath = helpFileWithName(fileManager, @"Help", @".help.rtf");
                    // possiblly == NULL
				//}
			}
		}
    }
    return helpFilePath; // possibly == NULL
}

-(void)showHelpFor:(NSString*)selection
{
    NSDocumentController* docctl = [NSDocumentController sharedDocumentController];
    if (!docctl)
	return;
	
    NSString *helpFilePath = [self pathOfHelpFileFor: selection];
    if(!helpFilePath) { // none found
        helpFilePath = helpFileWithName([NSFileManager defaultManager], @"Help", @".help.rtf");
        if(!helpFilePath) { // not even Help.help ?
            post("\nCan't find help for '%s'\n", [selection cString]);
            return;
        }
    }
    
    MyDocument *doc = (MyDocument*)[docctl documentForFileName: helpFilePath];
    if (!doc) {
        doc = [docctl openDocumentWithContentsOfFile: helpFilePath display: true];
        if (!doc) {
            post("Can't open Help File Document '%s'\n", [helpFilePath cString]);
            return;
        }
    }
    NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
    if (!window) {
        post("!! window controller returns nil ? failed to open help file window\n");
        return;
    }
    [window makeKeyAndOrderFront: nil];
}

#endif

extern bool compiledOK;

- (void)sendSelection: (char*) methodName
{        
    if (!compiledOK) {
        return;
    }

	NSRange selectedRange;
	NSString* selection = [self currentlySelectedTextOrLine: &selectedRange];
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
	//post("type '%s'\n", [aType cString]);
    // Insert code here to write your document from the given data.  You can also choose to override -fileWrapperRepresentationOfType: or -writeToFile:ofType: instead.
    //NSString* string = [textView string];
    
    //return [textView RTFDFromRange: NSMakeRange(0, [string length]);
/*    if ([[textView textStorage] containsAttachments]) {
        return [textView writeRTFDToFile: path atomically: YES];
    } else {
        return [textView writeRTFToFile: path atomically: YES];
    }*/
	
    BOOL success;

	NSString* extension = [path pathExtension];
	if ([extension isEqualToString: @"sc"] || [extension isEqualToString: @"txt"] 
			|| [aType isEqualToString: @"NSStringPboardType"]) 
	{
		NSString *text = [textView string];
		success = [text writeToFile: path atomically: YES];
	} else {
		NSTextStorage *textStorage = [textView textStorage];
	
		NSRange range = NSMakeRange(0, [textStorage length]);
		NSMutableDictionary *dict = [NSMutableDictionary dictionary];
	//post(">path '%s'\n", [path cString]);
		if ([textStorage containsAttachments]) {
			if (![extension isEqualToString: @"rtfd"]) {
				path = [[path stringByDeletingPathExtension] stringByAppendingPathExtension: @"rtfd"];
			}
	//post("<path '%s'\n", [path cString]);
			NSFileWrapper *wrapper = [textStorage RTFDFileWrapperFromRange: range documentAttributes: dict];
			success = wrapper ? [wrapper writeToFile: path atomically:YES updateFilenames: YES] : NO;
		} else {
			if ([extension isEqualToString: @""]) {
				path = [path stringByAppendingPathExtension: @"rtf"];
			}
			NSData *data = [textStorage RTFFromRange: range documentAttributes: dict];
			success = data ? [data writeToFile: path atomically: YES] : NO;
		}
	}
	//post("success %d\n", success);
	if (!success) return NO;
    
	NSFileManager *fmgr = [NSFileManager defaultManager];
    
    NSMutableDictionary *attr = [NSMutableDictionary dictionary];
    NSNumber *creator = [NSNumber numberWithInt: 'SCjm'];
    [attr setObject: creator forKey: NSFileHFSCreatorCode];
    success = [fmgr changeFileAttributes: attr atPath: path];
    //printf("write success %d\n", success);

    //NSMutableDictionary *attr2 = [fmgr fileAttributesAtPath: path traverseLink: NO];
    //OSType creatorval = [attr2 fileHFSCreatorCode];
    //printf("creator is %d   SCjm %d\n", creatorval, 'SCjm');
	
    return success;
}

- (BOOL)readFromFile:(NSString *)path ofType:(NSString *)aType
{

    //NSFileManager *fmgr = [NSFileManager defaultManager];
    //NSMutableDictionary *attr2 = [fmgr fileAttributesAtPath: path traverseLink: NO];
    //OSType creatorval = [attr2 fileHFSCreatorCode];
	
    //printf("r creator is %d   SCjm %d\n", creatorval, 'SCjm');

    NSMutableDictionary *options = [NSMutableDictionary dictionary];
    NSDictionary *docAttrs;
    NSURL *url = [NSURL fileURLWithPath: path];
	//NSString *docType;
	//id val, viewSizeVal, paperSizeVal;
    BOOL success;
    
    if (!initTextView) initTextView = [self makeTextView];
    NSTextStorage* text = [initTextView textStorage];
    
    [options setObject:url forKey:@"BaseURL"];
    //[options setObject:[NSNumber numberWithUnsignedInt:NoStringEncoding] forKey:@"CharacterEncoding"];
    
    // Insert code here to read your document from the given data.  You can also choose to override -loadFileWrapperRepresentation:ofType: or -readFromFile:ofType: instead.

    [text beginEditing];	// Bracket with begin/end editing for efficiency
    [[text mutableString] setString:@""];	// Empty the document
    success = [text readFromURL:url options:options documentAttributes: &docAttrs];	// Read!

	NSString* extension = [path pathExtension];
    [text endEditing];
 	if ([extension isEqualToString: @"sc"]) {
		[initTextView setFont: [NSFont fontWithName: @"Monaco" size: 9]];
		SyntaxColorize(initTextView);
		//[textView didChangeText];
	}

    return success;
}

- (BOOL) shouldRunSavePanelWithAccessoryView
{
    return YES;
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

- (BOOL) isDocumentEdited
{
	if (textView == gPostView) return false;
	return [super isDocumentEdited];
}

//////////////////////////////////////

- (void)doToggleRich {
    [self setRichText:!isRichText];
    //[self setEncoding:NoStringEncoding];
    //[self setConverted:NO];
    if ([[textView textStorage] length] > 0) [[textView window] setDocumentEdited: YES];
    //[self setDocumentName:nil];
}

/* toggleRich: puts up an alert before ultimately calling doToggleRich
*/
- (void)toggleRich:(id)sender {
    //int length = [[textView textStorage] length];
    //NSRange range;
    //NSDictionary *attrs;
	[self doToggleRich];
/*
    // If we are rich and any ofthe text attrs have been changed from the default, then put up an alert first...
    if (isRichText && (length > 0) && (attrs = [[textView textStorage] attributesAtIndex:0 effectiveRange:&range]) && ((attrs == nil) || (range.length < length) || ![[self defaultTextAttributes:YES] isEqual:attrs])) {
        NSBeginAlertSheet(NSLocalizedString(@"Convert document to plain text?", @"Title of alert confirming Make Plain Text"),
                        NSLocalizedString(@"OK", @"OK"), NSLocalizedString(@"Cancel", @"Button choice allowing user to cancel."), nil, [textView window], 
                        self, NULL, @selector(didEndToggleRichSheet:returnCode:contextInfo:), NULL,
                        NSLocalizedString(@"Converting will lose fonts, colors, and other text styles in the document.", @"Subtitle of alert confirming Make Plain Text"));
    } else {
        [self doToggleRich];
    }
*/
}

/*
- (void)didEndToggleRichSheet:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo {
    if (returnCode == NSAlertDefaultReturn) [self doToggleRich];
}
*/

/* Doesn't check to see if the prev value is the same --- Otherwise the first time doesn't work...
*/
- (void)setRichText:(BOOL)flag {
    NSTextView *view = textView;

    isRichText = flag;

    //if (!isRichText) [self removeAttachments];
    
    [view setRichText:isRichText];
    [view setUsesRuler:isRichText];	/* If NO, this correctly gets rid of the ruler if it was up */
    //if (isRichText && [[Preferences objectForKey:ShowRuler] boolValue]) [view setRulerVisible:YES];	/* Show ruler if rich, and desired */
    [view setImportsGraphics:isRichText];

	if (!isRichText) {
		NSMutableDictionary *textAttributes = [NSMutableDictionary dictionary];
		[textAttributes setObject: [NSFont fontWithName: @"Monaco" size: 9] forKey: NSFontAttributeName ];
	
		if ([[textView textStorage] length]) {
			[[textView textStorage] setAttributes:textAttributes range: NSMakeRange(0, [[textView textStorage] length])];
		}

		[view setTypingAttributes:textAttributes];
	}
}


/* Menu validation: Arbitrary numbers to determine the state of the menu items whose titles change. Speeds up the validation... Not zero. */   
#define TagForFirst 42
#define TagForSecond 43

static void validateToggleItem(NSMenuItem *aCell, BOOL useFirst, NSString *first, NSString *second) {
    if (useFirst) {
        if ([aCell tag] != TagForFirst) {
            [aCell setTitleWithMnemonic:first];
            [aCell setTag:TagForFirst];
        }
    } else {
        if ([aCell tag] != TagForSecond) {
            [aCell setTitleWithMnemonic:second];
            [aCell setTag:TagForSecond];
        }
    }
}

- (BOOL)validateMenuItem:(NSMenuItem *)aCell {
    SEL action = [aCell action];
    if (action == @selector(toggleRich:)) {
	validateToggleItem(aCell, isRichText, NSLocalizedString(@"&Make Plain Text", @"Menu item to make the current document plain text"), NSLocalizedString(@"&Make Rich Text", @"Menu item to make the current document rich text"));
        //if (![textView isEditable] || [self hasSheet]) return NO;
    } else {
		return [super validateMenuItem: aCell];
	}
	
	return YES;
}

@end
