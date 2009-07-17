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

#import <UIKit/UIKit.h>
#import "iPhoneSCView.h"

@interface SCGraphView : UIView
{
    struct PyrObject *mWindowObj;
    SCTopView *mTopView;
    bool mDragStarted;
    SCView* mMenuView;
    bool windowShouldClose;
	bool acceptsClickThrough;
	bool autoScrolls;
}

- (void)drawRect: (CGRect)bounds;
/*
- (void) keyDown: (NSEvent*) event;
- (void) keyUp: (NSEvent*) event;
- (void) mouseDown: (NSEvent*) event;
- (void) mouseMoved:(NSEvent*) theEvent;  //jan t
*/
- (void)setSCObject: (struct PyrObject*)inObject;
- (struct PyrObject*)getSCObject;
- (BOOL)isFlipped;
- (BOOL)mouseDownCanMoveWindow;

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)setSCTopView: (SCTopView*)inView;
//- (void)dealloc;
- (void)closeWindow;
- (void)willClose;
- (void)setWindowShouldClose:(BOOL)boo;
- (BOOL)windowShouldClose;
- (void)setAcceptsClickThrough:(BOOL)boo;
- (void)setAutoScrolls:(BOOL)boo;
- (void) beginDragFrom: (CGPoint)where of: (PyrSlot*)slot string:(NSString*) string;

//- (NSMenu*) menuForEvent:(NSEvent*)event;
- (void)startMenuTracking: (SCView*) inView;

- (IBAction) toggleUIEditMode: (id) sender;
- (void)scrollViewResized:(NSNotification *)notification;
- (void)userScrolled:(NSNotification *)notification;
- (void)setFrameSizeToMinimum;

@end
