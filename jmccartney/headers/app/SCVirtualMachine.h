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


@interface SCVirtualMachine : NSObject {
    NSMutableArray *deferredOperations;
    NSMutableArray *guiWindows;
}

+ (id)sharedInstance;

- (id)init;
- (void)start;
- (void)doPeriodicTask: (NSTimer*) timer;
- (void)setCmdLine: (const char*) text length: (int)length;
- (void)sendMain: (char*) methodName;

- (void)defer: (NSInvocation*) action;
- (void)performDeferredOperations;

- (void)addWindow: (NSWindow*)window;
- (void)closeAllGUIWindows;

- (IBAction) runMain: (id) sender;
- (IBAction) stopMain: (id) sender;
- (IBAction) compileLibrary: (id) sender;

// deferred primitives

- (void)newGuiWindowTitle:(NSString*)title bounds:(NSRect)bounds 
            SCObject: (void*)obj styleMask: (unsigned int) mask;
- (void)becomeFullScreen: (NSWindow*)window;
- (void)endFullScreen: (NSWindow*)window;
- (void)loadUserPanel:(NSString*)filename SCObject: (void*)scobj;


@end
