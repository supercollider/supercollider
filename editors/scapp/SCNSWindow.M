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
 *
 */

#import "SCNSWindow.h"
#include <pthread.h>
#import "PyrSymbolTable.h"
#import "PyrLexer.h"
#include "../../lang/LangSource/PyrSched.h"

@implementation SCNSWindow
- (BOOL) canBecomeKeyWindow { if(mHasBorders) return YES; else return [super canBecomeKeyWindow]; }
- (BOOL) hasBorders {return mHasBorders;}
- (void) setHasBorders: (BOOL) flag {mHasBorders = flag;}
- (void) setSCGraphView: (NSView*)view {mSCGraphView = view;}
- (NSView*) getSCGraphView {return mSCGraphView;}
- (IBAction) escape:(id)sender {
	gLangMutex.lock();
    runLibrary(getsym("escapeWindow"));
    gLangMutex.unlock();
}
- (void)keyDown:(NSEvent *)event
{
	if ([event keyCode] == 53)
	{
		[self escape:self];
	} else [super keyDown:event];
}
@end
