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
#import "UserPanel.h"
#import "SCTextView.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"

extern pthread_mutex_t gLangMutex;
extern PyrSymbol *s_closed;

@interface MyDocument : NSDocument
{
    IBOutlet SCTextView* initTextView;
    IBOutlet SCTextView* textView;
    IBOutlet NSScrollView* scrollView;
    Boolean isRichText;
    struct PyrObject *mWindowObj;
	Boolean promptToSave;
	NSString *pathToSaveAttachments;
}


- (SCTextView*)makeTextView;
- (SCTextView*) textView;

- (void)windowControllerDidLoadNib:(NSWindowController*) aController;

- (void)addDocument;

- (IBAction)openCode:(id)sender;
- (IBAction)methodTemplates: (id)sender;
- (IBAction)methodReferences: (id)sender;

- (IBAction)balanceParens: (id)sender;

- (IBAction)syntaxColorize: (id)sender;
- (void) insertText: (char*) text length: (int)length;
- (IBAction)shiftLeft: (id)sender;
- (IBAction)shiftRight: (id)sender;
- (IBAction)commentCode: (id)sender;
- (IBAction)uncommentCode:(id)sender;
- (IBAction)wrapParens: (int)sender;
- (IBAction)wrapSquareBrackets: (int)sender;
- (IBAction)wrapCurlyBrackets: (int)sender;

- (IBAction) executeSelection: (id) sender;
//- (NSString*)currentlySelectedTextOrLine: (NSRange*) outRange;
-(void)selectRangeStart:(int)rangeStart size:(int)rangeSize;

- (IBAction) showHelp: (id) sender;

- (BOOL) textView: (NSTextView *) textView
    clickedOnLink: (id) link
    atIndex: (unsigned) charIndex;
- (IBAction) createLink: (id) sender;

- (void)sendSelection: (char*) methodName;

- (NSString *)windowNibName;

- (BOOL)writeToFile:(NSString*) path ofType:(NSString *)aType;
- (BOOL)readFromFile:(NSString *)path ofType:(NSString *)aType;

- (BOOL) shouldRunSavePanelWithAccessoryView;

- (BOOL)windowShouldClose:(id)sender;
- (void)windowWillClose:(NSNotification *)aNotification;
- (IBAction) becomePostWindow: (id) sender;
- (BOOL) isDocumentEdited;

- (void)doToggleRich;
// toggleRich: puts up an alert before ultimately calling doToggleRich
- (IBAction)toggleRich:(id)sender;
- (void)setRichText:(BOOL)flag;

- (BOOL)validateMenuItem:(NSMenuItem *)aCell;

- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (void) closeWindow;
- (void)setBackgroundColor:(NSColor *)color;
- (NSScrollView*) scrollView;
- (NSTextView*) initTextView;
- (void)selectLine:(int)linenum;
- (IBAction)selectLineWindow: (id) sender;
- (IBAction)advancedFindReplaceAction:(id)sender;
- (void) callSCLangWithMethod: (PyrSymbol*) method;
- (BOOL) promptToSave;
- (void) setPromptToSave: (BOOL) flag; 
- (void) keyUp: (NSEvent*) event;
- (void) keyDown: (NSEvent*) event;
- (BOOL) handleKeyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;

- (BOOL)textView:(NSTextView *)textView shouldChangeTextInRange:(NSRange)affectedCharRange replacementString:(NSString *)replacementString;
- (void)flashHighlight: (BOOL)onoff atIndex: (int)index wait: (NSTimeInterval)interval timesLeft: (int)timesLeft;
- (void)replaceOldStyleHelpLinksWithHyperLinks;
- (IBAction)convertHelpLinksToHyperlinks:(id)sender;
- (void)convertFileLinksToRelative:(NSTextStorage *)textStorage;

@end

NSString* pathOfHelpFileFor(NSString* selection);
void showHelpFor(NSString* selection);
void fixHTMLBoldUnderline(NSMutableString* html);
NSString* pathOfFileRelativeToBaseDir(NSString *filepath, NSString *baseDir);
