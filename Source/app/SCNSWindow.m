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

#import "SCNSWindow.h"
extern pthread_mutex_t gLangMutex;

@implementation SCNSWindow
- (BOOL) canBecomeKeyWindow { if(mHasBorders) return YES; else return [super canBecomeKeyWindow]; }
- (BOOL) hasBorders {return mHasBorders;}
- (void) setHasBorders: (BOOL) flag {mHasBorders = flag;}
- (void) setSCGraphView: (NSView*)view {mSCGraphView = view;}
- (NSView*) getSCGraphView {return mSCGraphView;}
- (IBAction) escape:(id)sender {
	pthread_mutex_lock(&gLangMutex);
    runLibrary(getsym("escapeWindow"));
    pthread_mutex_unlock(&gLangMutex);
}
@end
