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

@interface MyDocument : NSDocument
{
    IBOutlet NSTextView* initTextView;
    IBOutlet NSTextView* textView;
    IBOutlet NSScrollView* scrollView;
	Boolean isRichText;
}

- (NSTextView*)makeTextView;
- (NSTextView*) textView;

- (void)windowControllerDidLoadNib:(NSWindowController*) aController;

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

- (IBAction) executeSelection: (id) sender;
- (NSString*)currentlySelectedTextOrLine: (NSRange*) outRange;
-(void)selectRangeStart:(int)rangeStart size:(int)rangeSize;

- (IBAction) showHelp: (id) sender;
-(NSString*)pathOfHelpFileFor:(NSString*)selection;
-(void)showHelpFor:(NSString*)selection;

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

@end
