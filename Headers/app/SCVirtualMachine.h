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


@interface SCVirtualMachine : NSObject {
    NSMutableArray *deferredOperations;
    NSMutableArray *guiWindows;
	NSTimer *deferredTaskTimer;
	NSTimer *appClockTimer;
	float deferredTaskInterval;
}

+ (id)sharedInstance;

- (id)init;
- (void)start;
- (void)setDeferredTaskInterval: (float) interval;
- (void)doPeriodicTask: (NSTimer*) timer;
- (void)doClockTask: (NSTimer*) timer;
- (void)setCmdLine: (const char*) text length: (int)length;
- (void)sendMain: (char*) methodName;

- (void)defer: (NSInvocation*) action;
- (void)performDeferredOperations;
- (void)removeDeferredOperationsFor:(id) object;
- (void)doAnimatedViews;

- (void)addWindow: (NSWindow*)window;
- (void)closeAllGUIWindows;

- (IBAction) runMain: (id) sender;
- (IBAction) stopMain: (id) sender;
- (IBAction) hardStopMain: (id) sender;
- (IBAction) preferences: (id) sender;
- (IBAction) compileLibrary: (id) sender;
- (IBAction) newSCWindow: (id) sender;
- (IBAction)clearPostWindow:(id)sender;
- (void)postWindowToFront:(id)sender;

// deferred primitives


- (void)becomeFullScreen: (NSWindow*)window;
- (void)endFullScreen: (NSWindow*)window;
- (void)loadUserPanel:(NSString*)filename SCObject: (void*)scobj;
- (void) windowDidBecomeKey:(NSNotification *)aNotification;
- (void) runAsModal:(NSWindow*)window;
- (void) runAsModalSheet: (NSWindow*)sheet forWindow:(NSWindow*)window;
- (void) sheetDidEnd:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo;
// Sleep
- (void) receiveSleepNote: (NSNotification*) note;
- (void) receiveWakeNote: (NSNotification*) note;

-(IBAction)showHelp:(id)sender;
-(IBAction)showHelpBrowser:(id)sender;
-(IBAction)showHelpSearch:(id)sender;
-(IBAction)showClassBrowser:(id)sender;

@end
