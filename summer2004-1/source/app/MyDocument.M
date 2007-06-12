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
#import "GoToPanel.h"
#import "AIHTMLDecoder.h"
#import "GetStringFromUser.h"
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
extern bool compiledOK;


bool docCreatedFromLang = false; //if this is true addDocument is not called

// these were in MyDocument, but that made it impossible to call them if a GUI window was in front 
// so that there was no current document.



@implementation MyDocument

- (NSTextView*)makeTextView
{
    NSTextView* aTextView = [[SCTextView alloc] initWithFrame: 
                    NSMakeRect(0,0,612,512)];
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

- (void)addDocument
{
    [self sendSelection: "addDocument"];
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
	//jt: call lang..
	if(!docCreatedFromLang)[self addDocument];
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
    NSString* helpFileName;
       
    // substitute for - or /
    if([desiredHelpFile isEqual: @"-"]) {
    	helpFileName = [NSString stringWithFormat: @"%@%@", @"subtraction", extension];
    } else if([desiredHelpFile isEqual: @"/"]) {
        helpFileName = [NSString stringWithFormat: @"%@%@", @"division", extension];
    } else {
        helpFileName = [NSString stringWithFormat: @"%@%@", desiredHelpFile, extension];
    }

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
    NSString *nsstring = [localTextView string];
	if (!nsstring) {
		post("text view has no string\n");
		return;
	}
	int maxlength  = [nsstring length];
	if(maxlength < 1) range = NSMakeRange(0, 0);
	else{
		if (rangeSize < 0) {
			unsigned int lineStart, lineEnd;
			range.length = 0;
			[nsstring getLineStart: &lineStart end: &lineEnd contentsEnd: nil forRange: range];
			range = NSMakeRange(lineStart, lineEnd - lineStart);
		}
		if(rangeStart < 0) range.location = 0;
		if(rangeStart >= maxlength) range.location = maxlength;
		if(rangeSize + rangeStart >=  maxlength) {
			range.length = maxlength - range.location;
		}
	}
    [localTextView setSelectedRange: range];
    [localTextView scrollRangeToVisible: range];
}
-(void)selectLine:(int)linenum
{
    NSTextView *localTextView = [self textView];
	NSString *nsstring = [localTextView string];
	int length = [nsstring length];
	if(length < 1) return;
    NSLayoutManager *layoutManager = [localTextView layoutManager];
    unsigned numberOfLines, index, numberOfGlyphs = [layoutManager numberOfGlyphs];
    NSRange lineRange;
	if(linenum <= 0) linenum = 1;
    for (numberOfLines = 0, index = 0; index < numberOfGlyphs && numberOfLines<linenum; numberOfLines++){
    
        (void) [layoutManager lineFragmentRectForGlyphAtIndex:index effectiveRange:&lineRange];
            index = NSMaxRange(lineRange);
    }

    [localTextView setSelectedRange: lineRange];
    [localTextView scrollRangeToVisible: lineRange];
}


//call from menu
- (IBAction)selectLineWindow: (id) sender
{
	[[GoToPanel sharedInstance] orderFrontGotoLinePanel:nil];
	
}


- (IBAction) showHelp: (id) sender;
{
	showHelpFor([self currentlySelectedTextOrLine: NULL]);
}

#endif

// LINK SUPPORT
//	Handle a click in a link.
- (BOOL) textView: (NSTextView *) textView
    clickedOnLink: (id) link
    atIndex: (unsigned) charIndex
{
    if ([link isKindOfClass: [NSString class]])
    {
        showHelpFor(link);
        return YES;
        
    } else if ([link isKindOfClass: [NSURL class]])
    {
        showHelpFor( [link path]);
        return YES;
    }
    
    return NO;
}

// Create a link

- (IBAction) createLink: (id) sender
{
    NSRange			selection;
    NSString			*link;
    NSMutableDictionary		*linkAttributes;

    selection = [textView selectedRange];
    if (selection.length == 0)
    {
        NSRunAlertPanel (@"Create Link", @"Please select some text before creating a link.", nil, nil, nil);
        return;
    }

    //	Get the text which will go into the link. 
    // 	Currently this looks for a helpfile with the name of the selection and any of the standard extensions 

    link = [pathOfHelpFileFor([self currentlySelectedTextOrLine: NULL]) lastPathComponent];
    if ([link length] ==0)
    {
        return;
    }
    
    //	Start to build attributes
    linkAttributes = [NSMutableDictionary dictionaryWithObject: link
        forKey: NSLinkAttributeName];

    // Currently always blue and underlined
    [linkAttributes setObject: [NSColor blueColor]  forKey: NSForegroundColorAttributeName];
    [linkAttributes setObject: [NSNumber numberWithBool: YES]  forKey: NSUnderlineStyleAttributeName];
    

    //	Add the attributes. This adds the link attributes to the selected range.
    [[textView textStorage] addAttributes: linkAttributes  range: selection];
    
    // Make sure it saves as HTML as default
    [self setFileType: @"SC Help (HTML)"];
    [self updateChangeCount: NSChangeDone];
}


- (void)sendSelection: (char*) methodName
{        
    if (!compiledOK) {
        return;
    }

	NSRange selectedRange;
	NSString* selection = [self currentlySelectedTextOrLine: &selectedRange];
    const char *text = [selection UTF8String];
	int textlength = strlen(text);

    [[SCVirtualMachine sharedInstance] setCmdLine: text length: textlength];
    
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
    BOOL success;

	NSString* extension = [path pathExtension];
	if ([extension isEqualToString: @"sc"] || [extension isEqualToString: @"txt"] 
			|| [aType isEqualToString: @"NSStringPboardType"]) 
	{
		NSString *text = [textView string];
		success = [text writeToFile: path atomically: YES];
	} else if ([extension isEqualToString: @"html"] || [extension isEqualToString: @"htm"])
        {
                NSTextStorage *textStorage = [textView textStorage];
		//NSString *html = [AIHTMLDecoder encodeHTML: textStorage encodeFullString: YES];
                NSString *html = [AIHTMLDecoder encodeHTML:textStorage headers:YES fontTags:YES closeFontTags:YES styleTags:YES closeStyleTagsOnFontChange:YES encodeNonASCII:YES];
                success = html ? [html writeToFile: path atomically: YES] : NO;
        
        } else {
		NSTextStorage *textStorage = [textView textStorage];
	
		NSRange range = NSMakeRange(0, [textStorage length]);
		NSMutableDictionary *dict = [NSMutableDictionary dictionary];
		if ([textStorage containsAttachments]) {
			if (![extension isEqualToString: @"rtfd"]) {
				path = [[path stringByDeletingPathExtension] stringByAppendingPathExtension: @"rtfd"];
			}
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
// CR MODIFIED
	return success;
}

// CR ADDED
- (NSDictionary *)fileAttributesToWriteToFile:(NSString *)fullDocumentPath ofType:(NSString *)type saveOperation: (NSSaveOperationType) saveOperationType
{
    NSMutableDictionary *attr = [NSMutableDictionary dictionary];
    NSNumber *creator = [NSNumber numberWithInt: 'SCjm'];
    [attr setObject: creator forKey: NSFileHFSCreatorCode];
	return attr;
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
extern PyrSymbol * s_didBecomeKey;
- (void) windowDidBecomeKey:(NSNotification *)aNotification
{
	
	if(!docCreatedFromLang){
		[self callSCLangWithMethod: s_didBecomeKey];
	}

}

extern PyrSymbol * s_didResignKey;
- (void) windowDidResignKey:(NSNotification *)aNotification
{
	
	if(!docCreatedFromLang){
		[self callSCLangWithMethod: s_didResignKey];
	}
	
}
- (void) callSCLangWithMethod: (PyrSymbol*) method {
	pthread_mutex_lock (&gLangMutex);
	if(compiledOK) {
		struct PyrObject *scobj = [self getSCObject];
		if (scobj) {
			SetPtr(scobj->slots + 0, self);
			VMGlobals *g = gMainVMGlobals;
			g->canCallOS = true;
			++g->sp;  SetObject(g->sp, scobj); // push window obj
			runInterpreter(g, method, 1);
			g->canCallOS = false;
		}
	}
    pthread_mutex_unlock (&gLangMutex);
}
- (void)windowWillClose:(NSNotification *)aNotification
{
    //for some reason some Documents created with open do not call windowWillClose
    /*
   
	[self callSCLangWithMethod: s_closed];
	[self setSCObject: nil];
    */
}
- (void)dealloc{
	//for some reason some Documents created with open do not call windowWillClose
	//so that its action is called here: jan.t
	if (textView == gPostView) gPostView = nil;
 	[self callSCLangWithMethod: s_closed];
	[self setSCObject: nil];
	[super dealloc];
}
- (IBAction) becomePostWindow: (id) sender
{
    gPostView = textView;
}


- (BOOL) isDocumentEdited
{
	if ([self textView] == gPostView) return false;
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

- (void)setSCObject: (struct PyrObject*)inObject
{
    mWindowObj = inObject;
}
    
- (struct PyrObject*)getSCObject
{
    return mWindowObj;
}

- (void) closeWindow
{
	[self close];
}

- (NSScrollView*) scrollView;
{
    return scrollView;
}
- (NSTextView*) initTextView;
{
    return initTextView;
}

- (void)setBackgroundColor:(NSColor *)color
{
	[[self textView] setBackgroundColor: color];
	[[self initTextView] setBackgroundColor: color];
	[[self scrollView] setBackgroundColor: color];
	[[[self textView] window] setAlphaValue: [color alphaComponent]];
}

@end


NSString* pathOfHelpFileFor(NSString* selection) 
{

    NSString* helpFilePath = nil;

    NSFileManager* fileManager = [NSFileManager defaultManager];
    if (!fileManager) return helpFilePath; // == NULL

    helpFilePath = helpFileWithName(fileManager, selection, @".help.rtf");
	if (helpFilePath) return helpFilePath;
	
	helpFilePath = helpFileWithName(fileManager, selection, @".rtf");
	if (helpFilePath) return helpFilePath;

	helpFilePath = helpFileWithName(fileManager, selection, @".help");
	if (helpFilePath) return helpFilePath;
	
	helpFilePath = helpFileWithName(fileManager, selection, @".sc");
	if (helpFilePath) return helpFilePath;

	helpFilePath = helpFileWithName(fileManager, selection, @".rtfd");
	if (helpFilePath) return helpFilePath;

        
        // support for html help files
        helpFilePath = helpFileWithName(fileManager, selection, @".htm");
	if (helpFilePath) return helpFilePath;
        
        helpFilePath = helpFileWithName(fileManager, selection, @".html");
	if (helpFilePath) return helpFilePath;
                                // special case for html files where link contains file suffix
	helpFilePath = helpFileWithName(fileManager, selection, @"");
	if (helpFilePath) return helpFilePath;

    return helpFilePath; // possibly == NULL
}

void showHelpFor(NSString* selection)
{
    NSDocumentController* docctl = [NSDocumentController sharedDocumentController];
    if (!docctl)
	return;
	
    NSString *helpFilePath = pathOfHelpFileFor(selection);
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

