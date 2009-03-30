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

@interface SCTextView : NSTextView
{
	NSString *langClassToCall;
	int keyDownActionIndex, keyUpActionIndex;
	int objectKeyDownActionIndex, objectKeyUpActionIndex;
	BOOL mAcceptsFirstResponder;
}
- (void) setAcceptsFirstResponder: (BOOL) flag;
- (void) keyDown: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) mouseUpAction: (NSEvent*) theEvent index: (int) index;

- (void) autoIndent: (NSEvent*) event;
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
@end

bool matchBraks(unsigned int *startpos, unsigned int *endpos, unichar *text, int length, unichar rightBrak, bool ignoreImmediateParens);
