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
#import "SCView.h"

@interface SCGraphView : NSView
{
    struct PyrObject *mWindowObj;
    SCTopView *mTopView;
    bool mDragStarted;
    SCView* mMenuView;
    bool windowShouldClose;
	bool acceptsClickThrough;
	bool autoScrolls;
	NSRect oldBounds;
}

- (void)drawRect: (NSRect)bounds;
- (void) keyDown: (NSEvent*) event;
- (void) keyUp: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) mouseMoved:(NSEvent*) theEvent;  //jan t

- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (BOOL)isFlipped;
- (BOOL)mouseDownCanMoveWindow;

- (void)setSCTopView: (SCTopView*)inView;
//- (void)dealloc;
- (void)closeWindow;
- (void)willClose;
- (void)setWindowShouldClose:(BOOL)boo;
- (BOOL)windowShouldClose;
- (void)setAcceptsClickThrough:(BOOL)boo;
- (void)setAutoScrolls:(BOOL)boo;
- (void) beginDragFrom: (NSPoint)where of: (PyrSlot*)slot string:(NSString*) string label:(NSString*) label;

- (void) didResignKey;
- (void) didBecomeKey;


- (NSMenu*) menuForEvent:(NSEvent*)event;
- (void)startMenuTracking: (SCView*) inView;

- (IBAction) toggleUIEditMode: (id) sender;
- (void)scrollViewResized:(NSNotification *)notification;
- (void)userScrolled:(NSNotification *)notification;
- (void)setFrameSizeToMinimum;

@end
