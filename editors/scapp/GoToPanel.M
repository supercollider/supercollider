/*
 *  GoToPanel.M
 *  SC3lang
 *
 *  Created by j. trutzschler on 02 sept 2003.
	derived from TextFinder.m by Ali Ozer

	a panel that searches and selects a line 

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




#import "GoToPanel.h"
#import "SCBase.h"
#import "PyrSymbol.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"

@implementation GoToPanel

static id sharedSelectLineObject = nil;

+ (id)sharedInstance 
{
    if (!sharedSelectLineObject) {
        [[self allocWithZone:[[NSApplication sharedApplication] zone]] init];
    }
    return sharedSelectLineObject;
}

- (id)init 
{
    if (sharedSelectLineObject) {
        [super dealloc];
        return sharedSelectLineObject;
    }

    if (!(self = [super init])) return nil;

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appDidActivate:) name:NSApplicationDidBecomeActiveNotification object:[NSApplication sharedApplication]];

    sharedSelectLineObject = self;
    return self;
}

- (void)appDidActivate:(NSNotification *)notification 
{
   // [self loadFindStringFromPasteboard];
}

- (void) loadUI
{
if (![NSBundle loadNibNamed:@"GoToLinePanel" owner:self]) {
    post("could not load GoToPanel.nib");
    }
    if (self == sharedSelectLineObject) [[findTextField window] setFrameAutosaveName:@"Find"];

}

- (NSPanel *)gotoLinePanel 
{
    if (!findTextField) [self loadUI];
    return (NSPanel *)[findTextField window];
}

- (void)dealloc 
{
    if (self != sharedSelectLineObject) {
        [[NSNotificationCenter defaultCenter] removeObserver:self];
        [super dealloc];
    }
}

- (NSTextView *)textObjectToSelectIn 
{
    id obj = [[NSApp mainWindow] firstResponder];
    return (obj && [obj isKindOfClass:[NSTextView class]]) ? obj : nil;
}

- (void)orderFrontGotoLinePanel:(id)sender 
{
    NSPanel *panel = [self gotoLinePanel];
    [findTextField selectText:nil];
    [panel makeKeyAndOrderFront:nil];
}

- (void)selectLine:(id)sender
{
    int linenum = [findTextField intValue];
    linenum = linenum;
    if(!linenum)[self getAndDisplayCurrentLine];
    else
    [self prselectLine: linenum];
}

- (void) getAndDisplayCurrentLine 
{
    NSTextView *localTextView = [self textObjectToSelectIn];
	NSRange range = [localTextView selectedRange];
	unsigned select = range.location + 1;
	if(!select) select = 0;
    NSLayoutManager *layoutManager = [localTextView layoutManager];
    unsigned numberOfLines, index, numberOfGlyphs = [layoutManager numberOfGlyphs];
    NSRange lineRange;
    for (numberOfLines = 0, index = 0; index < numberOfGlyphs && index<select; numberOfLines++){
    
        (void) [layoutManager lineFragmentRectForGlyphAtIndex:index effectiveRange:&lineRange];
            index = NSMaxRange(lineRange);
    }
    [findTextField setIntValue: numberOfLines];
	[localTextView setSelectedRange: lineRange];
    [localTextView scrollRangeToVisible: lineRange];
}

- (void)prselectLine:(int)linenum
{
          	
    NSTextView *localTextView = [self textObjectToSelectIn];
    NSLayoutManager *layoutManager = [localTextView layoutManager];
    unsigned numberOfLines, index, numberOfGlyphs = [layoutManager numberOfGlyphs];
    NSRange lineRange;
    for (numberOfLines = 0, index = 0; index < numberOfGlyphs && (int)numberOfLines < linenum; numberOfLines++) {
    
        (void) [layoutManager lineFragmentRectForGlyphAtIndex:index effectiveRange:&lineRange];
            index = NSMaxRange(lineRange);
    }

    [localTextView setSelectedRange: lineRange];
    [localTextView scrollRangeToVisible: lineRange];
}

@end
