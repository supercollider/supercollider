/*
 *  SCSoundFileView.h
 *  xSC3lang
 *
 *  Created by jan truetzschler on 4/12/06.
 *  Copyright (c) 2006 jan truetzschler. All rights reserved.

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
 *
 */

#import "iPhoneSCNSWindow.h"
#import "iPhoneSCGraphView.h"

@interface SCNSWindowViewController : UIViewController
{
}
@end

@implementation SCNSWindowViewController

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return YES;
}

@end

@implementation SCNSWindow
- (id) initWithFrame:(CGRect)bounds
{
	if (self=[super initWithFrame:bounds])
	{
		title = 0;
		viewController = [[SCNSWindowViewController alloc] init];
		viewController.view = self;
		viewController.tabBarItem.image = nil;
		[self setBackgroundColor:[UIColor grayColor]];
		self.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
	}
	return self;
}

- (BOOL) canBecomeKeyWindow { return YES; }
- (BOOL) hasBorders {return mHasBorders;}
- (void) setHasBorders: (BOOL) flag {mHasBorders = flag;}
- (void) setSCGraphView: (UIView*)view {mSCGraphView = view;}
- (UIView*) getSCGraphView {return mSCGraphView;}
- (void) setTitle:(NSString *)s
{
	if (title) [title release];
	title = [[NSString stringWithString:s] retain];
	viewController.title = title;
}
- (NSString *) title {return title;}
- (UIViewController *) controller { return viewController; }
- (void) close
{
    [(SCGraphView *) mSCGraphView willClose];

	[viewController release];
	viewController = 0;
	[self release];
}
- (void) dealloc
{
	if (title) [title release];
	if (viewController) [viewController release];
	[super dealloc];
}
@end
