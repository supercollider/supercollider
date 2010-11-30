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

#if __LP64__
typedef long NSInteger;
typedef unsigned long NSUInteger;
#else
typedef int NSInteger;
typedef unsigned int NSUInteger;
#endif

@interface SCTextView : NSTextView
{
	NSString *langClassToCall;
	int keyDownActionIndex, keyUpActionIndex;
	int objectKeyDownActionIndex, objectKeyUpActionIndex;
	BOOL mAcceptsFirstResponder;
	BOOL useAutoInOutdent; // defaults false for SCTextViews, true for docs
	NSMutableArray *completionDict;
}
- (void) setAcceptsFirstResponder: (BOOL) flag;
- (void) keyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) mouseUpAction: (NSEvent*) theEvent index: (int) index;

- (void) setUsesAutoInOutdent: (bool)flag;
- (bool) usesAutoInOutdent;

- (IBAction) executeSelection: (id) sender;
- (void) autoIndent: (NSEvent*) event;
- (void)outdentBrack;
- (NSString*)currentlySelectedTextOrLine: (NSRange*) outRange;
- (NSString*)currentlySelectedText: (NSRange*) outRange;
- (void) setLangClassToCall: (NSString*) stringin withKeyDownActionIndex:(int) downIndex withKeyUpActionIndex:(int) upIndex ;
- (void) setObjectKeyDownActionIndex:(int) mindex setObjectKeyUpActionIndex:(int) downIndex;
- (void)setDefaultTabsTo:(float)value;
- (IBAction)openCode:(id)sender;
- (IBAction) showHelp: (id) sender;

- (IBAction)methodTemplates: (id)sender;
- (IBAction)methodReferences: (id)sender;
- (void) balanceParens: (id) sender;
- (NSArray*)completionsForPartialWordRange:(NSRange)charRange indexOfSelectedItem:(NSInteger*)index;

// If completions textfile exists, ensures it's loaded. Returns false if fail or textfile not found
- (bool) loadCompletionDict;
@end

bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, unichar rightBrak, bool ignoreImmediateParens);
bool blankUnparsedChars(unichar* buffer, int length, bool process);