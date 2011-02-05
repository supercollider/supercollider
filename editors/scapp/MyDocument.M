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

#import <Cocoa/Cocoa.h>
#import "HTMLRenderer.h"
#import "MyDocument.h"
#import "SCTextView.h"
#import "SCVirtualMachine.h"
#import "GoToPanel.h"
#import "GetStringFromUser.h"
#include "SCBase.h"
#include "SC_DirUtils.h"
#import "SMLAdvancedFindController.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#ifdef SC_WIN32
//# include <wx/wx.h>
# include <direct.h>
#else
# include <sys/param.h>
#endif

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
bool defaultDocumentUseAutoInOutDent = true;

NSFont *defaultFont = nil;

@implementation MyDocument

- (id)init
{
	if(!defaultFont) {
		defaultFont = [NSFont fontWithName: @"Monaco" size: 9];
		[defaultFont retain];
	}
	mWindowObj = nil;
    initTextView = nil;
    textView = nil;
    scrollView = nil;
    isRichText = YES;
	promptToSave = YES;
	pathToSaveAttachments = nil;
	split = NO;
	return [super init];
}

// the first one, not the split
- (SCTextView*)makeTextView
{
    SCTextView* aTextView = [[SCTextView alloc] initWithFrame: NSMakeRect(0,0,612,512)];
	isRichText = YES;
	[self initialiseTextViewParams: aTextView];
	[aTextView setFont: defaultFont];
	activeTextView = aTextView;
	return aTextView;
}
	
- (void)initialiseTextViewParams: (SCTextView*)aTextView
{
    [aTextView setAutoresizingMask: 63];
    [[aTextView textContainer] setWidthTracksTextView: YES];
    [aTextView setDelegate: self];
    [aTextView setAllowsUndo: YES];
    [aTextView setRichText: isRichText];
    [aTextView setSmartInsertDeleteEnabled: NO];
    [aTextView setImportsGraphics: YES];
	[aTextView setLangClassToCall:@"CocoaDocument" 
			withKeyDownActionIndex:4 withKeyUpActionIndex:5];
	[aTextView setObjectKeyDownActionIndex:2 setObjectKeyUpActionIndex:1];
	[aTextView setAcceptsFirstResponder:YES]; 
	[aTextView setUsesAutoInOutdent:defaultDocumentUseAutoInOutDent];
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
	NSWindow *window = [scrollView window];
	
	
	mySplitView = [[NSSplitView alloc] initWithFrame:[scrollView frame]];
	[window setContentView: mySplitView];
	
	[mySplitView addSubview:scrollView];
	
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

- (void)balanceParens: (id)sender
{
    [activeTextView balanceParens: nil];
}

- (BOOL)textView:(NSTextView *)theTextView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString
{
	if (replacementString && [replacementString length] > 0) {
		unichar c = [replacementString characterAtIndex: 0];
		if (strchr(")]}", c)) {
			NSString *string = [theTextView string];

			int length = [string length];
			unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
			[string getCharacters: buffer];
			
			unsigned int start, end;
			start = affectedCharRange.location;
			end = start;
						
			bool doMatchBraks = !blankUnparsedChars(buffer, end, true);
			
			if(doMatchBraks)
			{
				bool res = matchBraks(&start, &end, buffer, length, c, false);
				if (res) {
					[self flashHighlight: YES atIndex: start wait: 0.33 timesLeft: 1];
				} else {
					//NSBeep();
					[self flashHighlight: NO atIndex: affectedCharRange.location wait: 0.07 timesLeft: 6];
				}
			}
			free(buffer);
		}
	}
	return YES;
}

- (void)flashHighlight: (BOOL)onoff atIndex: (int)index wait: (NSTimeInterval)interval timesLeft: (int)timesLeft
{
    NSLayoutManager *layoutManager = [activeTextView layoutManager];
	NSRange highlightRange = NSMakeRange(index, 1);

	if (onoff) {
		[layoutManager addTemporaryAttributes: 
			[NSDictionary dictionaryWithObjectsAndKeys: [NSColor lightGrayColor], NSBackgroundColorAttributeName, nil] 
							forCharacterRange: highlightRange];	
	} else {
		//highlightRange.length++;
		[layoutManager removeTemporaryAttribute: NSBackgroundColorAttributeName
							forCharacterRange: highlightRange];
	}
	if (timesLeft) {
		timesLeft--;
		onoff = !onoff;
		SEL sel = @selector(flashHighlight:atIndex:wait:timesLeft:);
		NSMethodSignature *sig = [MyDocument instanceMethodSignatureForSelector: sel];
		NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
		[anInvocation setTarget: self];
		[anInvocation setSelector: sel];
		[anInvocation setArgument: &onoff atIndex: 2];  
		[anInvocation setArgument: &index atIndex: 3];  
		[anInvocation setArgument: &interval atIndex: 4];  
		[anInvocation setArgument: &timesLeft atIndex: 5];  
		[NSTimer scheduledTimerWithTimeInterval: interval invocation: anInvocation repeats: NO];
	}
}


void SyntaxColorize(NSTextView* textView);

- (void)syntaxColorize: (id)sender
{
    SyntaxColorize(activeTextView);
    [activeTextView didChangeText];
}

- (void) insertText: (char*) text length: (int)length
{
    NSRange selectedRange = [activeTextView selectedRange];
    NSString *string = [[NSString alloc] initWithCString: text length: length];
    if ([activeTextView shouldChangeTextInRange: selectedRange replacementString: string]) {
        [activeTextView replaceCharactersInRange: selectedRange withString: string];
        [activeTextView didChangeText];
    }
    [string release];
}


- (void)wrapParensBegin:(NSString*)begStr end:(NSString*)endStr
{
	NSTextStorage *textStorage = [activeTextView textStorage];
    NSRange selectedRange = [activeTextView selectedRange];
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];
    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
	[newString autorelease];
	
	[newString replaceCharactersInRange: NSMakeRange([newString length],0) withString: endStr];
	[newString replaceCharactersInRange: NSMakeRange(0,0) withString: begStr];
    
    if ([activeTextView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
		NSRange newSelectedRange = selectedRange;		
		if(selectedRange.length > 0) newSelectedRange.length += [begStr length] + [endStr length];
		else 
		{
			newSelectedRange.length = 0;
			newSelectedRange.location = newSelectedRange.location + [begStr length];
		}	
        [activeTextView setSelectedRange: newSelectedRange];
		SyntaxColorize(activeTextView);
        [activeTextView didChangeText];
    }
}

- (void)wrapParens: (int)sender
{
	[self wrapParensBegin: @"(" end: @")"];
}

- (void)wrapSquareBrackets: (int)sender
{
	[self wrapParensBegin: @"[" end: @"]"];
}

- (void)wrapCurlyBrackets: (int)sender
{
	[self wrapParensBegin: @"{" end: @"}"];
}

- (void)wrapComments: (int)sender
{
	[self wrapParensBegin: @"/*" end: @"*/"];
}


- (void)shiftLeft: (id)sender
{
   NSTextStorage *textStorage = [activeTextView textStorage];
    NSRange selectedRange = [activeTextView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
    
    int j = 1;
    if (buffer[0] == NSTabCharacter) {
        [newString deleteCharactersInRange: NSMakeRange(0,1)];
		j -= 1;
    }
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
    
    if ([activeTextView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [activeTextView setSelectedRange: newSelectedRange];
        [activeTextView didChangeText];
    }
}

- (void)shiftRight: (id)sender
{
   NSTextStorage *textStorage = [activeTextView textStorage];
    NSRange selectedRange = [activeTextView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSString *tabStr = [[NSString alloc] initWithString: @"\t"];
    [tabStr autorelease];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
	[newString replaceCharactersInRange: NSMakeRange(0,0) withString: tabStr];
    int j = 1;
    for (int i=0; i<length-1; ++i, ++j) {
        unichar c = buffer[i];
        if (c == NSNewlineCharacter || c == NSCarriageReturnCharacter) {
			[newString replaceCharactersInRange: NSMakeRange(j+1,0) withString: tabStr];
            j += 1;
        }
    }    
    free(buffer);
    NSRange newSelectedRange = NSMakeRange(selectedRange.location, j+1);
    
    if ([activeTextView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [activeTextView setSelectedRange: newSelectedRange];
        [activeTextView didChangeText];
    }
}

- (void)commentCode: (id)sender
{
    NSTextStorage *textStorage = [activeTextView textStorage];
    NSRange selectedRange = [activeTextView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

    NSString *commentChars = [[NSString alloc] initWithString: @"//"];
    [commentChars autorelease];

    NSMutableAttributedString *newString = [[NSMutableAttributedString alloc] initWithAttributedString: originalSelection];
    [newString autorelease];

    NSString *string = [originalSelection string];
    
    int length = [string length];
    unichar* buffer = (unichar*)malloc((length+1) * sizeof(unichar));
    [string getCharacters: buffer];
    
	[newString replaceCharactersInRange: NSMakeRange(0,0) withString: commentChars];
    int j = 2;
    for (int i=0; i<length-1; ++i, ++j) {
        unichar c = buffer[i];
        if (c == NSNewlineCharacter || c == NSCarriageReturnCharacter) {
			[newString replaceCharactersInRange: NSMakeRange(j+1,0) withString: commentChars];
            j += 2;
        }
    }    
    free(buffer);
    NSRange newSelectedRange = NSMakeRange(selectedRange.location, j+1);
   
    if ([activeTextView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [activeTextView didChangeText];
        [activeTextView setSelectedRange: newSelectedRange];
        SyntaxColorize(activeTextView);
		[activeTextView didChangeText];
    }
}

- (void)uncommentCode:(id)sender
{
   NSTextStorage *textStorage = [activeTextView textStorage];
    NSRange selectedRange = [activeTextView selectedRange];
    if (selectedRange.length <= 0) return;
    NSAttributedString *originalSelection = [textStorage attributedSubstringFromRange: selectedRange];

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
    
    if ([activeTextView shouldChangeTextInRange: selectedRange replacementString: [newString string]]) {
        [textStorage replaceCharactersInRange: selectedRange withAttributedString: newString];
        [activeTextView didChangeText];
        [activeTextView setSelectedRange: newSelectedRange];
        SyntaxColorize(activeTextView);
		[activeTextView didChangeText];
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

NSString* helpFileWithName(NSFileManager* fileManager, NSString* desiredHelpFile, NSString* helpFolderPath)
{
    // catch the common case of wanting main help
	if([desiredHelpFile isEqual: @""]) {
		NSString* mainHelpPath = @"Help/Help.html";
		if([fileManager fileExistsAtPath: mainHelpPath]) {
			return mainHelpPath;
		} else {
			// maybe it's moved or changed format
			return helpFileWithName(fileManager, @"Help", helpFolderPath);
		}
	}
	
	// If called with no folder check in the bundle (if OS X), SC distribution Help
    // directory, and the System and User Extensions directories.
    if(!helpFolderPath) { 
		NSString* helpFilePath;
		NSString* searchPath;
		 
		char resourceDirPath[MAXPATHLEN - 32];
		sc_GetResourceDirectory(resourceDirPath, MAXPATHLEN - 32);
		sc_AppendToPath(resourceDirPath, "Help/");
		searchPath = [NSString stringWithCString:(const char *) resourceDirPath];
		helpFilePath = helpFileWithName(fileManager, desiredHelpFile, searchPath);
		if (helpFilePath) return helpFilePath;

		if(!sc_IsStandAlone()) {
			searchPath = [NSString stringWithFormat: @"%@/Help/",
						 [fileManager currentDirectoryPath]];
			helpFilePath = helpFileWithName(fileManager, desiredHelpFile, searchPath);
			if (helpFilePath) return helpFilePath;
			
			searchPath = @"/Library/Application Support/SuperCollider/Extensions/";
			helpFilePath = helpFileWithName(fileManager, desiredHelpFile, searchPath);
			if (helpFilePath) return helpFilePath;
			
			searchPath = [NSString stringWithFormat: @"%@/Library/Application Support/SuperCollider/Extensions/",  
						NSHomeDirectory()];
			helpFilePath = helpFileWithName(fileManager, desiredHelpFile, searchPath);
			if (helpFilePath) return helpFilePath;
		}
		return NULL;
    }

    // the name of the help file we are looking for 
    NSString* helpFileName;
       	
	// substitute for these symbols, as they are not valid filenames on one or more platforms
    if([desiredHelpFile isEqual: @"-"]) {
    	helpFileName = @"subtraction";
    } else if([desiredHelpFile isEqual: @"/"]) {
        helpFileName = @"division";
	} else if([desiredHelpFile isEqual: @"**"]) {
        helpFileName = @"exponentiation";
	} else if([desiredHelpFile isEqual: @"%"]) {
        helpFileName = @"modulo";
	} else if([desiredHelpFile isEqual: @"*"]) {
        helpFileName = @"multiplication";
	} else if([desiredHelpFile isEqual: @"+"]) {
        helpFileName = @"addition";
	} else if([desiredHelpFile isEqual: @"<"]) {
        helpFileName = @"lessthan";	
	} else if([desiredHelpFile isEqual: @"<="]) {
        helpFileName = @"lessorequalthan";
	} else if([desiredHelpFile isEqual: @">"]) {
        helpFileName = @"greaterthan";
	} else if([desiredHelpFile isEqual: @">="]) {
        helpFileName = @"greaterorequalthan";
    } else {
        helpFileName = desiredHelpFile;
    }

    // recurse through this directory until we find it
    NSString* helpFilePath = nil;
    NSDirectoryEnumerator* dirEnumerator = [fileManager enumeratorAtPath:helpFolderPath];
    NSString* candidate;
	NSString* fullName;
    while (candidate = [dirEnumerator nextObject]) {
		if([[candidate lastPathComponent] isEqualToString: @".svn"]
			|| [[candidate lastPathComponent] isEqualToString: @"ignore"]
			|| [[candidate lastPathComponent] isEqualToString: @".git"]
			|| [[candidate lastPathComponent] isEqualToString: @"_darcs"]
			) {
			[dirEnumerator skipDescendents];
		} else {
			NSString* trialName = [helpFolderPath stringByAppendingString: candidate];

			char name[MAXPATHLEN];
			char resolvedName[MAXPATHLEN];
			[trialName getCString:name];
			bool isAlias = false;
			sc_ResolveIfAlias(name, resolvedName, isAlias, MAXPATHLEN);
			fullName = [NSString stringWithCString:resolvedName];
			// compare with valid extensions
			if([[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".html"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".rtf"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".rtfd"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".scd"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".help.scd"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".schelp"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".help.rtf"]]
				|| [[fullName lastPathComponent] isEqualToString: [helpFileName stringByAppendingString: @".htm"]]
				) {
				helpFilePath = fullName;
				break;
			} else if // if that didn't work, check if this is a directory symlink (allow directories to have extensions)
			((([[fileManager fileAttributesAtPath: fullName traverseLink: NO] objectForKey:@"NSFileType"] == NSFileTypeSymbolicLink) || isAlias)){
			  NSString* linkName = [[fullName stringByResolvingSymlinksInPath] stringByAppendingString: @"/"];
			  if(([[fileManager fileAttributesAtPath: linkName traverseLink: NO] objectForKey:@"NSFileType"] == NSFileTypeDirectory)) {
				// if so traverse the link and call helpFileWithName() recursively
				helpFilePath = helpFileWithName(fileManager, desiredHelpFile, linkName);
				if(helpFilePath) break;
			  }
			}
		}
    }

    return helpFilePath;
}



-(void)selectRangeStart:(int)rangeStart size:(int)rangeSize
{

    NSRange range = NSMakeRange(rangeStart, rangeSize);
    NSString *nsstring = [activeTextView string];
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
			[nsstring getLineStart: (NSUInteger*)&lineStart end: (NSUInteger*)&lineEnd contentsEnd: nil forRange: range];
			range = NSMakeRange(lineStart, lineEnd - lineStart);
		}
		if(rangeStart < 0) range.location = 0;
		if(rangeStart >= maxlength) range.location = maxlength;
		if(rangeSize + rangeStart >=  maxlength) {
			range.length = maxlength - range.location;
		}
	}
    [activeTextView setSelectedRange: range];
    [activeTextView scrollRangeToVisible: range];
}

-(void)selectLine:(int)linenum
{
	NSString *nsstring = [activeTextView string];
	int length = [nsstring length];
	if(length < 1) return;
    NSLayoutManager *layoutManager = [activeTextView layoutManager];
    unsigned numberOfLines, index, numberOfGlyphs = [layoutManager numberOfGlyphs];
    NSRange lineRange;
	if(linenum <= 0) linenum = 1;
    for (numberOfLines = 0, index = 0; index < numberOfGlyphs && (int)numberOfLines < linenum; numberOfLines++) {
    
        (void) [layoutManager lineFragmentRectForGlyphAtIndex:index effectiveRange:&lineRange];
            index = NSMaxRange(lineRange);
    }

    [activeTextView setSelectedRange: lineRange];
    [activeTextView scrollRangeToVisible: lineRange];
}


//call from menu
- (IBAction)selectLineWindow: (id) sender
{
	[[GoToPanel sharedInstance] orderFrontGotoLinePanel:nil];
	
}

- (IBAction) advancedFindReplaceAction:(id)sender
{
	[[SMLAdvancedFindController sharedInstance] showAdvancedFindWindow];
}

- (IBAction) showHelp: (id) sender;
{
	showHelpFor([activeTextView currentlySelectedText: NULL]);
}

- (IBAction)showClassBrowser:(id)sender {
	[self sendSelection: "showClassBrowser"];
}

#endif

// LINK SUPPORT
//	Handle a click in a link.
- (BOOL) textView: (NSTextView *) textView
    clickedOnLink: (id) link
    atIndex: (unsigned) charIndex
{
    NSDocumentController* docctl = [NSDocumentController sharedDocumentController];
	if (!docctl) return YES;
	
	NSURL *desiredURL;
	
	// is it a NSURL link or a NSString link?
	if ([link isKindOfClass: [NSString class]])
    {
		if([link hasPrefix:@"SC://"] || [link hasPrefix:@"sc://"]) { // this means search immediately
			NSString *helpPath = pathOfHelpFileFor([[[link substringFromIndex:5] stringByDeletingPathExtension] stringByDeletingPathExtension]);
			if(!helpPath) {
				post("WARNING:\nInvalid hyperlink: '%s' Please repair this.\n", [link cString]);
				return YES;
			}
			desiredURL = [NSURL URLWithString: helpPath];
		} else desiredURL = [NSURL URLWithString: [link stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding] relativeToURL: [self fileURL]];
        
    } else if ([link isKindOfClass: [NSURL class]])
    {

		// check for schemes which we'll handle
		if([[link scheme] isEqualToString: @"SC"] || [[link scheme] isEqualToString:@"sc"]) { // this means search immediately

			NSString *helpPath = pathOfHelpFileFor([[[[link relativeString] substringFromIndex:5] stringByDeletingPathExtension] stringByDeletingPathExtension]);
			if(!helpPath) {
				post("WARNING:\nInvalid hyperlink: '%s' Please repair this.\n", [[link relativeString] cString]);
				return YES;
			}
			desiredURL = [NSURL fileURLWithPath: helpPath];
		} else if (![link scheme]) { // NULL could be a regular file link that's been edited
			if([[link relativePath] hasPrefix: @"/"]) {
				desiredURL = [NSURL fileURLWithPath: [link relativeString]];
			} else {
				desiredURL = [NSURL URLWithString: [[link relativeString] stringByAddingPercentEscapesUsingEncoding: NSUTF8StringEncoding] relativeToURL: [self fileURL]];
			}
			//desiredURL = [NSURL fileURLWithPath: [link relativeString]];
		} else if(![[link scheme] isEqualToString: @"file"]) {
			return NO; // it's http, etc. so pass it on to Safari or whatever
		} else desiredURL = link; // it's a regular file:// URL
    } else return NO;
    
	MyDocument *doc = nil;
	if([[NSFileManager defaultManager] fileExistsAtPath: [desiredURL path]]) {

		doc = (MyDocument*)[docctl documentForURL: [desiredURL absoluteURL]];
	} else NSLog(@"file doesn't exist at path");
    if (!doc) {
        doc = [docctl openDocumentWithContentsOfURL: desiredURL display: true];
        if (!doc) {
			// it's a bad file:// URL, post a warning and search
			post("WARNING:\nInvalid hyperlink: '%s' Please repair this.\nSearching help directories for alternative.\n", [[desiredURL path] cString]);
			// delete extension twice in case something.help.rtf
			NSString *desiredHelpName = [[[[desiredURL path] lastPathComponent] stringByDeletingPathExtension] stringByDeletingPathExtension];
			NSString *helpPath = pathOfHelpFileFor(desiredHelpName); 
			if(!helpPath) {
				post("Can't find Help File Document for: '%s'\n", [desiredHelpName cString]);
				return YES;
			}
			desiredURL = [NSURL fileURLWithPath: helpPath];
			if([[NSFileManager defaultManager] fileExistsAtPath: [desiredURL path]]) {
				doc = (MyDocument*)[docctl documentForURL: [desiredURL absoluteURL]];
			} else post("file doesn't exist at path: '%s'\n", [[desiredURL path] cString]);
			if (!doc) {
				doc = [docctl openDocumentWithContentsOfURL: desiredURL display: true];
				if(!doc) {
					post("Can't open Help File Document: '%s'\n", [[desiredURL path] cString]);
					return YES;
				}
			}
        }
    }
    NSWindow *window = [[[doc windowControllers] objectAtIndex: 0] window];
    if (!window) {
        post("!! window controller returns nil ? failed to open help file window\n");
        return YES;
    }
    [window makeKeyAndOrderFront: nil];
    return YES;
}
	
// Create a link to a helpfile

- (IBAction) createLink: (id) sender
{
    NSRange			selection;
    NSString			*link;
    NSMutableDictionary		*linkAttributes;

    selection = [activeTextView selectedRange];
	// check to see if we can make a link
    if (selection.length == 0)
    {
        NSRunAlertPanel (@"Link to Help file", @"Please select some text before creating a link.", nil, nil, nil);
        return;
    } else if(![self fileURL]) {
		NSRunAlertPanel (@"Link to Help file", @"You must save this Document before creating a link.", nil, nil, nil);
        return;
	}

    //	Get the text which will go into the link. 
	
	link = pathOfHelpFileFor([activeTextView currentlySelectedTextOrLine: NULL]);
	if(!link) {
		NSRunAlertPanel (@"Link to Help file", [NSString stringWithFormat: @"No help named %@ exists.", [activeTextView currentlySelectedTextOrLine: NULL]], nil, nil, nil);
		return;
	}
	BOOL targetInMainHelp = [[[self fileURL] path] rangeOfString: [NSString stringWithFormat: @"%@/Help/", [[NSFileManager defaultManager] currentDirectoryPath]]].location != NSNotFound;
	BOOL linkInMainHelp = [link rangeOfString: [NSString stringWithFormat: @"%@/Help/", [[NSFileManager defaultManager] currentDirectoryPath]]].location != NSNotFound;
	// if both are in main help, or both are not, produce a relative link, otherwise an SC:// URL, which will call the search system.
	if((targetInMainHelp && linkInMainHelp) || (!targetInMainHelp && !linkInMainHelp)) {
		link = pathOfFileRelativeToBaseDir(link, [[[self fileURL] path] stringByDeletingLastPathComponent]);
	} else link = [NSURL URLWithString: [@"SC://" stringByAppendingString: [activeTextView currentlySelectedTextOrLine: NULL]]];
	
    //	Start to build attributes
    linkAttributes = [NSMutableDictionary dictionaryWithObject: link forKey: NSLinkAttributeName];

    // Currently always blue
    [linkAttributes setObject: [NSColor blueColor]  forKey: NSForegroundColorAttributeName];

    //	Add the attributes. This adds the link attributes to the selected range.
    [[activeTextView textStorage] addAttributes: linkAttributes  range: selection];
    
    // Make sure it saves as HTML as default
    [self setFileType: @"HTML"]; // need to change name to html
    [self updateChangeCount: NSChangeDone];
}


- (void)sendSelection: (char*) methodName
{        
    if (!compiledOK) {
        return;
    }

	NSRange selectedRange;
	NSString* selection = [activeTextView currentlySelectedTextOrLine: &selectedRange];
    const char *text = [selection UTF8String];
	int textlength = strlen(text);

    [[SCVirtualMachine sharedInstance] setCmdLine: text length: textlength];
    
    NSRange newSelectedRange = NSMakeRange(selectedRange.location + selectedRange.length, 0);
    [activeTextView setSelectedRange: newSelectedRange];
    
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

- (BOOL)prepareSavePanel:(NSSavePanel *)savePanel {
	[savePanel setDelegate: self];
	return YES;
}

- (void)setpathToSaveAttachments:(NSString *)newPathToSaveAttachments {
	[newPathToSaveAttachments retain];
	[pathToSaveAttachments release];
	pathToSaveAttachments = newPathToSaveAttachments;
}

- (NSString *)panel:(id)sender userEnteredFilename:(NSString *)filename confirmed:(BOOL)okFlag {
	if(okFlag) {
		if([[self fileTypeFromLastRunSavePanel] isEqualToString: @"SC Help (HTML)"] && [[textView textStorage] containsAttachments]) {
			//.../attachments/filename/
			[self setpathToSaveAttachments: 
				[[[[sender filename] stringByDeletingLastPathComponent] stringByAppendingPathComponent: @"attachments"] stringByAppendingPathComponent: [[[sender filename] lastPathComponent] stringByDeletingPathExtension]]];
			return filename;
		} else { 
			return filename; 
		}
	}
	return nil;
}

- (BOOL)writeToFile:(NSString*) path ofType:(NSString *)aType
{
    BOOL success;
	NSString* extension = [path pathExtension];
	if ([extension isEqualToString: @"sc"] || [extension isEqualToString: @"txt"] || [extension isEqualToString: @"scd"] || [extension isEqualToString: @"quark"]
			|| [aType isEqualToString: @"NSStringPboardType"]) 
	{
		NSString *text = [textView string];
		success = [text writeToFile: path atomically: YES];
	} else if ([extension isEqualToString: @"html"] || [extension isEqualToString: @"htm"]) {
		
		[textView setDefaultTabsTo: 28.0f]; // set default tabs to preserve layout
		
		//[self replaceOldStyleHelpLinksWithHyperLinks];
		NSTextStorage *textStorage = [textView textStorage];
		[self convertFileLinksToRelative: textStorage]; // clean up any manually edited file:// links
		NSRange range = NSMakeRange(0, [textStorage length]);
		NSDictionary *attributes = [NSDictionary dictionaryWithObject: NSHTMLTextDocumentType forKey:NSDocumentTypeDocumentAttribute];
		
		// Check for images etc.
		if ([textStorage containsAttachments]) {
			// this has wrappers for html and all attachments
			NSFileWrapper *wrapper = [textStorage fileWrapperFromRange: range documentAttributes: attributes error: NULL];
			success = wrapper ? YES : NO;
			// if no wrapper bail out...
			if(!success) return success;
			
			// now get the html component so we can clean it up and save it under the requested name
			NSFileWrapper *indexWrapper = [[wrapper fileWrappers] objectForKey: @"index.html"];
			[indexWrapper retain];
			[wrapper removeFileWrapper: indexWrapper];
			NSMutableString *html = [[NSMutableString alloc] initWithData: [indexWrapper regularFileContents] encoding: NSUTF8StringEncoding]; // maybe should check here?
			// strip out "file:" so as to have nice portable relative urls
			[html replaceOccurrencesOfString: @"file:///" withString: @"" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])];
			
			// replace "%20" with " " as psycollider doesn't like the former
			[html replaceOccurrencesOfString: @"%20" withString: @" " options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])];
			
			// fix faded autosyntax Colours
			[html replaceOccurrencesOfString: @"color: #0016bd" withString: @"color: #0000bf" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])]; // blue
			[html replaceOccurrencesOfString: @"color: #ae1a19" withString: @"color: #bf0000" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])]; // red
			[html replaceOccurrencesOfString: @"color: #2b7000" withString: @"color: #007300" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])]; // green
			// grey doesn't fade
			
			// NSTextView doesn't like <b> tags within spans, and unfortunately the export outputs bold underlines that way.
			// The following figures out what the span name is for underlining, and fixes this
			fixHTMLBoldUnderline(html);

			// now save attachments to the final directory
			NSFileManager *fm = [NSFileManager defaultManager];
			// check attachments/filename/ exists
			if(![fm fileExistsAtPath: [pathToSaveAttachments stringByDeletingLastPathComponent]]) {
				success = [fm createDirectoryAtPath: [pathToSaveAttachments stringByDeletingLastPathComponent] attributes: nil];
				if(!success) return success;
			}
			if(![fm fileExistsAtPath: pathToSaveAttachments]) {
				success = [fm createDirectoryAtPath: pathToSaveAttachments attributes: nil];
				if(!success) return success;
			}

			NSEnumerator *files = [[wrapper fileWrappers] objectEnumerator];
			NSFileWrapper *thisFile;
			while(thisFile = [files nextObject]) {
				// save relative to pathToSaveAttachments (which we stashed earlier) as the path arg passed in
				// may not be the requested path, but rather a temp save location
				NSString *thisPath = [pathToSaveAttachments stringByAppendingPathComponent: [thisFile preferredFilename]];
				NSString *thisFileName = [thisPath lastPathComponent];
				
				// now check if this is an image and convert to png.
				NSBitmapImageRep *imageRep = [NSBitmapImageRep imageRepWithData: [thisFile regularFileContents]];
				
				if(imageRep) { 
					NSData *pngData = [imageRep representationUsingType: NSPNGFileType properties: nil];
					NSString *pngURL = [[NSString stringWithFormat:@"attachments/%@/", [[path lastPathComponent] stringByDeletingPathExtension]] 
						stringByAppendingString: [[thisFileName stringByDeletingPathExtension] stringByAppendingPathExtension: @"png"]];
					[html replaceOccurrencesOfString: [NSString stringWithFormat:@"<img src=\"%@\" alt=\"%@\">", thisFileName, thisFileName] 
										  withString: [NSString stringWithFormat:@"<img src=\"%@\" alt=\"%@\">", pngURL, pngURL] 
											 options: NSCaseInsensitiveSearch 
											   range: NSMakeRange(0, [html length])
						];
					success = success && [pngData writeToFile: [pathToSaveAttachments stringByAppendingPathComponent: [pngURL lastPathComponent]] atomically:YES];
				} else if([[thisFileName pathExtension] isEqualToString:@"pdf"]) {
					// it's a pdf
					NSString *correctURL = [[NSString stringWithFormat:@"attachments/%@/", [[path lastPathComponent] stringByDeletingPathExtension]] stringByAppendingString: thisFileName];
					[html replaceOccurrencesOfString: [NSString stringWithFormat:@"<img src=\"%@\"", thisFileName] 
										  withString: [NSString stringWithFormat:@"<img src=\"%@\"", correctURL]
											 options: NSCaseInsensitiveSearch
											   range: NSMakeRange(0, [html length])
						];
					success = success && [thisFile writeToFile: thisPath atomically:YES updateFilenames: YES];
				} else {
					// not an image
					NSString *correctURL = [[NSString stringWithFormat:@"attachments/%@/", [[path lastPathComponent] stringByDeletingPathExtension]] stringByAppendingString: thisFileName];
					[html replaceOccurrencesOfString: [NSString stringWithFormat:@"<object data=\"%@\">%@</object>", thisFileName, thisFileName] 
										  withString: [NSString stringWithFormat:@"<object data=\"%@\">%@</object>", correctURL, correctURL]
											 options: NSCaseInsensitiveSearch
											   range: NSMakeRange(0, [html length])
						];
					success = success && [thisFile writeToFile: thisPath atomically:YES updateFilenames: YES];
				}
			}
			
			// write the html
			NSFileWrapper *htmlWrapper = [[NSFileWrapper alloc] initRegularFileWithContents: [html dataUsingEncoding: NSUTF8StringEncoding]];
			if(!htmlWrapper) { NSLog(@"HTML Wrapper init failed."); }
			success = success && [htmlWrapper writeToFile: path atomically:YES updateFilenames: YES];
			if(!success) return success;
			[indexWrapper release];
			[htmlWrapper release];
			[html release];
		} else {
			// html with no attachments, way more simpler
			NSData *data = [textStorage dataFromRange:range documentAttributes:attributes error:NULL];
			NSMutableString *html = [[NSMutableString alloc] initWithData: data encoding: NSUTF8StringEncoding];
			if(html) {
				fixHTMLBoldUnderline(html);
				// fix faded autosyntax Colours
				[html replaceOccurrencesOfString: @"color: #0016bd" withString: @"color: #0000bf" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])]; // blue
				[html replaceOccurrencesOfString: @"color: #ae1a19" withString: @"color: #bf0000" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])]; // red
				[html replaceOccurrencesOfString: @"color: #2b7000" withString: @"color: #007300" options: NSCaseInsensitiveSearch range: NSMakeRange(0, [html length])]; // green
				// grey doesn't fade
			}
			data = [html dataUsingEncoding: NSUTF8StringEncoding];
			success = data ? [data writeToFile: path atomically: YES] : NO; 
			[html release];
		};
        
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
	[textView breakUndoCoalescing];
	return success;
}

- (NSDictionary *)fileAttributesToWriteToFile:(NSString *)fullDocumentPath ofType:(NSString *)type saveOperation: (NSSaveOperationType) saveOperationType
{
    NSMutableDictionary *attr = [NSMutableDictionary dictionary];
    NSNumber *creator = [NSNumber numberWithInt: 'SCjm'];
    [attr setObject: creator forKey: NSFileHFSCreatorCode];
	return attr;
}

- (void) loadHTML:(NSURL *)url
{
	if (!url) return;
	
	if (!initTextView) initTextView = [self makeTextView];
	NSTextStorage* text = [initTextView textStorage];

	[text beginEditing];	// Bracket with begin/end editing for efficiency
	[[text mutableString] setString:@""];	// Empty the document

	NSAttributedString *htmlAttributedString = [HTMLRenderer attributedStringWithURL:url];

	if(htmlAttributedString)
	{
		[text setAttributedString:htmlAttributedString];
		[initTextView setDefaultTabsTo: 28.0f];
	}

	[text endEditing];
	
	[url release];
}

- (BOOL)readFromFile:(NSString *)path ofType:(NSString *)aType
{
	NSURL *url = [NSURL fileURLWithPath: path];
	BOOL success = NO;

	NSString* extension = [path pathExtension];

	if (!initTextView) initTextView = [self makeTextView];
	NSTextStorage* text = [initTextView textStorage];

	if ([extension isEqualToString: @"html"] || [extension isEqualToString: @"htm"])
	{
		[url retain];
		if (docCreatedFromLang)
		{
			SEL sel = @selector(loadHTML:);
			NSMethodSignature *sig = [MyDocument instanceMethodSignatureForSelector: sel];
			SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
			
			NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
			[anInvocation setTarget: self];
			[anInvocation setSelector: sel];
			[anInvocation setArgument:&url atIndex:2];
			[scvm defer: anInvocation];
		}
		else
		{
			[self loadHTML:url];
		}
		[self setpathToSaveAttachments:[[[path stringByDeletingLastPathComponent] stringByAppendingPathComponent: @"attachments"] stringByAppendingPathComponent: [[path lastPathComponent] stringByDeletingPathExtension]]];
		
		return YES;
	}

	[text beginEditing];	// Bracket with begin/end editing for efficiency
	[[text mutableString] setString:@""];	// Empty the document

	
	NSError *error;
	success = [text readFromURL:url options:nil documentAttributes:nil error:&error];
	if(!success) NSLog(@"Error opening file: %@", error);

	if ([extension isEqualToString: @"sc"] || [extension isEqualToString: @"scd"]) {
		[initTextView setFont: defaultFont];
		SyntaxColorize(initTextView);
	}
	[text endEditing];

	// in case of html with images, etc.
	[self setpathToSaveAttachments:[[[path stringByDeletingLastPathComponent] stringByAppendingPathComponent: @"attachments"] stringByAppendingPathComponent: [[path lastPathComponent] stringByDeletingPathExtension]]];
	return success;
}

- (BOOL) shouldRunSavePanelWithAccessoryView
{
    return YES;
}

- (SCTextView*) textView;
{
    return textView;
}

- (SCTextView*) textView2;
{
    return textView2;
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
	if (textView == gPostView) gPostView = nil;
 	[self callSCLangWithMethod: s_closed];
	[self setSCObject: nil];
}
- (void)dealloc{
	//for some reason some Documents created with open do not call windowWillClose
	//so that its action is called here: jan.t
	[pathToSaveAttachments release];
	[super dealloc];
}
- (IBAction) becomePostWindow: (id) sender
{
    gPostView = textView;
}


- (BOOL) isDocumentEdited
{
	if ([self textView] == gPostView || !promptToSave) return false;
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
		[textAttributes setObject: defaultFont forKey: NSFontAttributeName ];
	
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
    } else if (action == @selector(toggleSplitWindow:))
	{
		[aCell setState:(split ? NSOnState : NSOffState)];
		return YES;
	} else {
		return [super validateMenuItem: aCell];
	}
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
- (SCTextView*) initTextView;
{
    return initTextView;
}

- (SCTextView*) activeTextView
{
	return activeTextView;
}

- (void)setBackgroundColor:(NSColor *)color
{
	if ([color brightnessComponent] < 0.5) {
		[textView setInsertionPointColor: [NSColor whiteColor]];
		[textView2 setInsertionPointColor: [NSColor whiteColor]];
	} else {
		[textView setInsertionPointColor: [NSColor blackColor]];
		[textView2 setInsertionPointColor: [NSColor blackColor]];
	}
	
	[textView setBackgroundColor: color];
	[[self initTextView] setBackgroundColor: color];
	[textView2 setBackgroundColor: color];
	[[self scrollView] setBackgroundColor: color];
	[scrollView2 setBackgroundColor: color];
	[[textView window] setAlphaValue: [color alphaComponent]];
}

- (void)setSelectedBackgroundColor:(NSColor *)color
{
	[textView setSelectedTextAttributes: [NSDictionary dictionaryWithObject: color forKey: NSBackgroundColorAttributeName]];
	[textView2 setSelectedTextAttributes: [NSDictionary dictionaryWithObject: color forKey: NSBackgroundColorAttributeName]];
}

- (BOOL)promptToSave
{
	return promptToSave;
}

-(void)setPromptToSave:(BOOL)flag
{
	promptToSave = flag;
}
- (void) keyDown: (NSEvent*) event
{
}

- (BOOL) handleKeyDown: (NSEvent*) event;
{
	return NO;
}

- (void) keyUp: (NSEvent*) event
{
}
- (void) mouseDown: (NSEvent*) event
{
	
}

- (IBAction)convertHelpLinksToHyperlinks:(id)sender
{
	[self replaceOldStyleHelpLinksWithHyperLinks];
}
- (void)replaceOldStyleHelpLinksWithHyperLinks
{
	NSTextStorage *textStorage = [textView textStorage];
	NSRange limitRange = NSMakeRange(0, [textStorage length]);
	NSRange underlineRange;
	NSMutableArray *bracketIndices = [NSMutableArray arrayWithCapacity: 10];
	while (limitRange.length > 0) {
		id isUnderlined = [textStorage attribute: NSUnderlineStyleAttributeName 
										 atIndex: limitRange.location 
						   longestEffectiveRange: &underlineRange 
										 inRange: limitRange];
		
		if(underlineRange.length<=0) break;
		if (isUnderlined) {
			int openBracketIndex = -1, closeBracketIndex = -1;
			if([[textStorage string] characterAtIndex: underlineRange.location] == '[') {
				openBracketIndex = underlineRange.location;
			} else if(underlineRange.location > 0) {
				if([[textStorage string] characterAtIndex: underlineRange.location - 1] == '[') openBracketIndex = underlineRange.location - 1;
			}
			
			if([[textStorage string] characterAtIndex: NSMaxRange(underlineRange) - 1] == ']') {
				closeBracketIndex = NSMaxRange(underlineRange) - 1;
			} else if(NSMaxRange(underlineRange) < [textStorage length]) {
				if([[textStorage string] characterAtIndex: NSMaxRange(underlineRange)] == ']') closeBracketIndex = NSMaxRange(underlineRange);
			}
			if(openBracketIndex >= 0 && closeBracketIndex > 0) {	
				[bracketIndices addObject: [NSNumber numberWithInt: openBracketIndex]];
				[bracketIndices addObject: [NSNumber numberWithInt: closeBracketIndex]];
				//[textStorage beginEditing];
				// remove the underline, and bold if there
				[textStorage removeAttribute: NSUnderlineStyleAttributeName range: underlineRange];
				NSFont *font = [textStorage attribute:NSFontAttributeName atIndex: underlineRange.location effectiveRange:NULL];
				font = [[NSFontManager sharedFontManager] convertFont: font toNotHaveTrait: NSBoldFontMask];
				if(font) [textStorage addAttribute: NSFontAttributeName value: font range: underlineRange];
				// now make the link
				[textView setSelectedRange: NSMakeRange(openBracketIndex + 1, closeBracketIndex - openBracketIndex - 1)];
				[self createLink:NULL];
				//[textStorage endEditing];
				
			} 			
			
		} 
		limitRange = NSMakeRange(NSMaxRange(underlineRange), [textStorage length] - NSMaxRange(underlineRange));
		
		
	}
	
	NSEnumerator *bracketEnum = [bracketIndices reverseObjectEnumerator];
	NSNumber *bracketIndex;
	while ((bracketIndex = [bracketEnum nextObject])) {
		[textStorage deleteCharactersInRange: NSMakeRange([bracketIndex intValue], 1)];
	}
}
	
// the text storage converts relative URLs to file:// URLs on open so this converts them back, and cleans up any manually edited links before saving.
- (void)convertFileLinksToRelative:(NSTextStorage *)textStorage
{
	//NSTextStorage *textStorage = [textView textStorage];
	NSRange limitRange = NSMakeRange(0, [textStorage length]);
	NSRange linkRange;
	while (limitRange.length > 0) {
		id link = [textStorage attribute: NSLinkAttributeName 
										 atIndex: limitRange.location 
						   longestEffectiveRange: &linkRange 
										 inRange: limitRange];
		if(linkRange.length<=0) break;
		if (link && [link isKindOfClass: [NSURL class]] && ([[link scheme] isEqualToString: @"file"])) {
			NSString *newLink = pathOfFileRelativeToBaseDir([link path], [[[self fileURL] path] stringByDeletingLastPathComponent]);
			[textStorage addAttribute: NSLinkAttributeName value: newLink range: linkRange];
		}
		limitRange = NSMakeRange(NSMaxRange(linkRange), NSMaxRange(limitRange) - NSMaxRange(linkRange));
	}
	
}

- (IBAction)toggleSplitWindow: (id)sender
{
	if(!split){
		textView2 = [[SCTextView alloc] initWithFrame: [scrollView frame]];
		[self initialiseTextViewParams: textView2];
		NSLog(@"delegate: %@", [textView2 delegate]);
		[textView2 setBackgroundColor: [textView backgroundColor]];
		[textView2 setSelectedTextAttributes: [textView selectedTextAttributes]];
		[textView2 setUsesAutoInOutdent:[textView usesAutoInOutdent]];
		scrollView2 = [[NSScrollView alloc] initWithFrame:[scrollView frame]];
		[scrollView2 setHasVerticalScroller:YES];
		
		// a bit easier to see
		[scrollView setBorderType:NSBezelBorder];
		[scrollView2 setBorderType:NSBezelBorder];
		
		[mySplitView addSubview:scrollView2];
		[scrollView2 setDocumentView: textView2];
		
		[[textView2 layoutManager] replaceTextStorage:[textView textStorage]];
		
		[textView2 release];
		[scrollView2 release];
		
		[mySplitView adjustSubviews];
		split = YES;
		[(NSMenuItem*)sender setState:NSOnState];
	} else {
		[scrollView2 removeFromSuperview];
		[scrollView setBorderType:NSNoBorder];
		scrollView2 = nil;
		textView2 = nil;
		split = NO;
		[(NSMenuItem*)sender setState:NSOffState];
	}
}


- (void)setActiveTextView:(SCTextView*)aTextView
{
	activeTextView = aTextView;
}

- (void) setUsesAutoInOutdent: (bool) flag;
{
	[textView setUsesAutoInOutdent:flag];
	[textView2 setUsesAutoInOutdent:flag];
}
@end


NSString* pathOfHelpFileFor(NSString* selection) 
{

    NSString* helpFilePath = nil;

    NSFileManager* fileManager = [NSFileManager defaultManager];
    if (!fileManager) return helpFilePath; // == NULL

    helpFilePath = helpFileWithName(fileManager, selection, nil);
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
        helpFilePath = helpFileWithName([NSFileManager defaultManager], @"Help", nil);
        if(!helpFilePath) { // not even Help.help ?
            post("\nCan't find help for '%s'\n", [selection cString]);
            return;
        }
    }
    
    MyDocument *doc = (MyDocument*)[docctl documentForFileName: helpFilePath];
    if (!doc) {
        doc = [docctl openDocumentWithContentsOfURL:[NSURL fileURLWithPath:helpFilePath] display:true error:nil];
		  
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

void fixHTMLBoldUnderline(NSMutableString* html) {
	// NSTextView doesn't like <b> tags within spans, and unfortunately the export outputs bold underlines that way.
	// The following figures out what the span name is for underlining, and fixes this
	NSRange underlineSpan = [html rangeOfString: @"{text-decoration: underline}"];
	
	if(underlineSpan.location != NSNotFound) {
		
		unsigned lineStart, nameStart;
		// goto beginning of line
		[html getLineStart: (NSUInteger*)&lineStart end: NULL contentsEnd: NULL forRange: underlineSpan];
		nameStart = lineStart + 5; // offset for "span."
		NSString *underlineSpanClass = [html substringWithRange: NSMakeRange(nameStart, underlineSpan.location - 1 - nameStart)];
		//NSLog(@"span class: %@", underlineSpanClass);
		NSString *underlineBoldSpanTag = [NSString stringWithFormat: @"<span class=\"%@\"><b>", underlineSpanClass];
		NSString *replaceTag = [NSString stringWithFormat: @"<b><span class=\"%@\">", underlineSpanClass];
		//NSLog(@"span tag: %@", underlineBoldSpanTag);
		
		NSRange openTagRange = [html rangeOfString: underlineBoldSpanTag options: NSCaseInsensitiveSearch]; // finds the first one
		while (openTagRange.location != NSNotFound) {
			[html replaceCharactersInRange: openTagRange withString: replaceTag];
			// now replace the close tag
			unsigned int end = NSMaxRange(openTagRange);
			NSRange closeTagRange = [html rangeOfString: @"</b></span>" options: NSCaseInsensitiveSearch range: NSMakeRange(end,  [html length] - end)];
			[html replaceCharactersInRange: closeTagRange withString: @"</span></b>"];
			// see if there's another one
			end = NSMaxRange(closeTagRange);
			openTagRange = [html rangeOfString: underlineBoldSpanTag options: NSCaseInsensitiveSearch range: NSMakeRange(end,  [html length] - end)];
		}
	}
}

// base is a directory, filepath is a file
NSString* pathOfFileRelativeToBaseDir(NSString *filepath, NSString *baseDir) {
	
	if (![filepath isAbsolutePath]) {
		return filepath;
	} else if(![baseDir isAbsolutePath]) {
		return nil;
	}
	NSArray *pathComponents = [filepath pathComponents];
	NSArray *baseComponents = [baseDir pathComponents];
	NSEnumerator *pathEnumerator = [pathComponents objectEnumerator];
	NSEnumerator *baseEnumerator = [baseComponents objectEnumerator];
	
	int i = 0, pathCount, baseCount, shortest;
	pathCount = [pathComponents count];
	baseCount = [baseComponents count];
	shortest = pathCount <= baseCount ? pathCount : baseCount;
	
	while((i < shortest) && [[pathEnumerator nextObject] isEqualToString: [baseEnumerator nextObject]]) {
		i++;
	}
	
	NSString *result = @"";
	for(int j =0; j < (baseCount - i); j++) {
		result = [result stringByAppendingString: @"../"];
	}
	result = [result stringByAppendingString: [NSString pathWithComponents: [pathComponents subarrayWithRange: NSMakeRange(i, pathCount - i) ]]];	
	return result;
}


