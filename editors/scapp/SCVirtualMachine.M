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

#import <objc/objc-class.h>
#import "SCVirtualMachine.h"
#import "MyDocument.h"
#import "SCGraphView.h"
#import "SCNSWindow.h"
#include "ChangeCounter.h"

#import "RendezvousClient.h"
#import "SCService.h"

#include "SCBase.h"
#include "PyrSymbol.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include "UserPanel.h"
#include "SC_DirUtils.h"   // for gIdeName

#include <pthread.h>

#if (MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4)
#define class_getName(a)	((a)->name)
#endif

ChangeCounter gUIChangeCounter;

extern bool compiledOK;
extern pthread_mutex_t gLangMutex;
extern PyrSymbol *s_tick;

PyrString* newPyrStringN(class PyrGC *gc, long length, long flags, bool collect);
void dumpByteCodes(PyrBlock *theBlock);

void initPostBuffer();
void flushPostBuf();
void SetupHomeDirectory();
double elapsedTime();

extern char *gHomePath;

@implementation SCVirtualMachine

static id sharedSCVM = nil;

+ (id)sharedInstance {
    if (!sharedSCVM) {
        [[self allocWithZone:[NSApp zone]] init];
    }
    return sharedSCVM;
}

- (id)init
{
    if (sharedSCVM) {
        [super dealloc];
        return sharedSCVM;
    }

    if (!(self = [super init])) return nil;

    deferredOperations = [NSMutableArray arrayWithCapacity: 8];
    [deferredOperations retain];
    deferredTaskInterval = 1.f/60;

    guiWindows = [NSMutableArray arrayWithCapacity: 8];
    [guiWindows retain];
    [NSApp setDelegate: self]; //just to get a notification when sc quits. jan.t
    sharedSCVM = self;
    return self;
}

- (void)start
{
	gIdeName = "scapp"; // declare that code specific to SC.app IDE will be compiled in
	deferredTaskTimer =
			[NSTimer scheduledTimerWithTimeInterval: deferredTaskInterval target: self selector:
				@selector(doPeriodicTask:) userInfo: nil repeats: YES];

	appClockTimer =
			[NSTimer scheduledTimerWithTimeInterval: 0.020 target: self selector:
				@selector(doClockTask:) userInfo: nil repeats: YES];

	[deferredTaskTimer retain];
	[appClockTimer retain];

	[[NSRunLoop currentRunLoop] addTimer: deferredTaskTimer
		forMode: NSModalPanelRunLoopMode];

	[[NSRunLoop currentRunLoop] addTimer: deferredTaskTimer
		forMode: NSEventTrackingRunLoopMode];

	[[NSRunLoop currentRunLoop] addTimer: appClockTimer
		forMode: NSModalPanelRunLoopMode];

	[[NSRunLoop currentRunLoop] addTimer: appClockTimer
		forMode: NSEventTrackingRunLoopMode];

	SetupHomeDirectory();
	pyr_init_mem_pools( 2*1024*1024, 256*1024 );
	initPostBuffer();
	init_OSC(57120);
	schedInit();
	compileLibrary();

	// CR ADDED
	[[RendezvousClient sharedClient] findOSCServices];

	[self sendMain: "applicationStart"];

	// Sleep - Technical Q&A QA1340
	//These notifications are filed on NSWorkspace's notification center, not the default notification center.
	//  You will not receive sleep/wake notifications if you file with the default notification center.
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
			selector: @selector(receiveSleepNote:) name: NSWorkspaceWillSleepNotification object: NULL];
	[[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
			selector: @selector(receiveWakeNote:) name: NSWorkspaceDidWakeNotification object: NULL];

}

-(void)setDeferredTaskInterval: (float) interval
{

	[deferredTaskTimer invalidate];
	[deferredTaskTimer release];
	deferredTaskInterval = sc_max(interval, 1.f/600);
	deferredTaskTimer =
	[NSTimer scheduledTimerWithTimeInterval: deferredTaskInterval target: self selector:
	 @selector(doPeriodicTask:) userInfo: nil repeats: YES];

	[deferredTaskTimer retain];

	[[NSRunLoop currentRunLoop] addTimer: deferredTaskTimer
								 forMode: NSModalPanelRunLoopMode];

	[[NSRunLoop currentRunLoop] addTimer: deferredTaskTimer
								 forMode: NSEventTrackingRunLoopMode];
}

- (void)doPeriodicTask: (NSTimer*) timer
{
    [self performDeferredOperations];
    [self doAnimatedViews];
    flushPostBuf();
}

// Sleep
//  Technical Q&A QA1340
//  Registering and unregistering for sleep and wake notifications
- (void) receiveSleepNote: (NSNotification*) note
{
//	NSLog(@"receiveSleepNote: %@", [note name]);
	[self sendMain: "sleep"];
}

- (void) receiveWakeNote: (NSNotification*) note
{
//    NSLog(@"receiveSleepNote: %@", [note name]);
	[self sendMain: "wake"];
}

- (void)doClockTask: (NSTimer*) timer
{
   if (pthread_mutex_trylock(&gLangMutex) == 0) {
        if (compiledOK) runLibrary(s_tick);
        pthread_mutex_unlock(&gLangMutex);
    }
    flushPostBuf();
}


- (void)setCmdLine: (const char*) text length: (int)length
{
	if (!compiledOK) {
		postfl("The library has not been compiled successfully.\n");
		return;
	}
	pthread_mutex_lock(&gLangMutex);
	if (compiledOK) {
		VMGlobals *g = gMainVMGlobals;

		int textlen = length;
		PyrString* strobj = newPyrStringN(g->gc, textlen, 0, true);
		memcpy(strobj->s, (char*)text, textlen);

		SetObject(&slotRawInterpreter(&g->process->interpreter)->cmdLine, strobj);
		g->gc->GCWrite(slotRawObject(&g->process->interpreter), strobj);
	}
	pthread_mutex_unlock(&gLangMutex);
}

- (void)sendMain: (const char*) methodName
{
	pthread_mutex_lock(&gLangMutex);
	runLibrary(getsym(methodName));
	pthread_mutex_unlock(&gLangMutex);
}

- (void)compileLibrary:(id)sender
{
// if not and user pref, open first error file to that line
if(! compileLibrary() ) [self postWindowToFront: NULL];
}

- (void)runMain:(id)sender
{
	[self sendMain: "run"];
}

- (void)stopMain:(id)sender
{
	[self sendMain: "stop"];
}

- (void)hardStopMain:(id)sender
{
	[self sendMain: "hardStop"];
}

- (IBAction) preferences: (id) sender
{
	[self sendMain: "preferences"];
}

- (IBAction) newSCWindow: (id) sender
{
	[self sendMain: "newSCWindow"];
}
/* i am the delegate for each SCGraphView window */
-(BOOL)windowShouldClose:(id)sender
{
	 return [(SCGraphView*)([(SCNSWindow*)sender getSCGraphView]) windowShouldClose];
}

extern NSTextView *gPostView;

- (void)postWindowToFront:(id)sender
{
	if (gPostView) [[gPostView window] makeKeyAndOrderFront: sender];
}
- (void)clearPostWindow:(id)sender
{
	if(gPostView) {
		NSTextStorage *textStorage = [gPostView textStorage];
		NSRange range = NSMakeRange(0, [textStorage length]);
		if ([gPostView shouldChangeTextInRange: range replacementString: @""]) {
			[gPostView replaceCharactersInRange: range withString: @""];
			[gPostView didChangeText];
		}
	}
}
- (void)defer: (NSInvocation*) action
{
	[deferredOperations addObject: action];
}


- (void)removeDeferredOperationsFor:(id) object
{
	NSMutableArray *newArray = [NSMutableArray arrayWithCapacity: 8];
	[newArray retain];
	for (unsigned int i=0; i<[deferredOperations count]; ++i) {
		NSInvocation* action = (NSInvocation*)[deferredOperations objectAtIndex: i];
		if ([action target] != object) {
			[newArray addObject: action];
		}
	}
	[deferredOperations release];
	deferredOperations = newArray;
}

- (void)performDeferredOperations
{
	while ([deferredOperations count]) {
		NSInvocation* action = (NSInvocation*)[deferredOperations objectAtIndex: 0];
		[action retain];
		[deferredOperations removeObjectAtIndex: 0];
		///NSLog(@"%d %@ %08X\n", [deferredOperations count], action, [action target]);
		[action invoke];
		[action release];
	}
}

- (void)doAnimatedViews
{
    SCView* view = gAnimatedViews;
	while (view)
	{
		view->animate();
		view = view->NextAnimatedView();
	}
}

- (void)closeAllGUIWindows
{
	[UserPanel closeAll];

	if ([guiWindows count]) {
		NSMutableArray *copy = [guiWindows copy];
		[guiWindows removeAllObjects];
		[copy makeObjectsPerformSelector: @selector(close)];
		[copy release];
	}

	[deferredOperations removeAllObjects];
}

- (void)addWindow: (NSWindow*)window;
{
	[guiWindows addObject: window];
}

void setupUserPaneViews(id target, id view, int tagsum)
{

	int newTagSum = tagsum;
	if ([view respondsToSelector: @selector(setTag:)]) {
		int thisTag = [view tag];
		if (thisTag > 0) newTagSum += thisTag;
	}
	post("  view '%s' %d %d\n", class_getName(object_getClass(view)), tagsum, newTagSum);

	if ([view respondsToSelector: @selector(setTarget:)]) {
		[view setTarget: target];
	}
	if ([view respondsToSelector: @selector(setAction:)]) {
		[view setAction: @selector(panelAction:)];
	}
	if ([view respondsToSelector: @selector(setTag:)]) {
		[view setTag: newTagSum];
	}

	if ([view respondsToSelector: @selector(subviews)]) {
		NSArray *subviews = [view subviews];
		int numSubviews = [subviews count];
		for (int i=0; i<numSubviews; ++i) {
			id subview = [subviews objectAtIndex: i];
			setupUserPaneViews(target, subview, newTagSum);
		}
	}
	if ([view respondsToSelector: @selector(cells)]) {
		NSArray *cells = [view cells];
		if (cells) {
			int numCells = [cells count];
			for (int i=0; i<numCells; ++i) {
				id cell = [cells objectAtIndex: i];
				setupUserPaneViews(target, cell, newTagSum);
			}
		}
	}
}

- (void)loadUserPanel:(NSString*)filename SCObject: (void*)scobj
{
	UserPanel* panel = [[UserPanel alloc] init];
	[panel retain];

	NSURL *url = [NSURL fileURLWithPath: filename];
	[filename release];
	NSString *nspath = [url path];
	NSMutableDictionary *dict = [NSMutableDictionary dictionary];
	[dict setObject: panel forKey: @"NSOwner"];
	NSZone *zone = [NSApp zone];
	post("path '%s'\n", [nspath cStringUsingEncoding:[NSString defaultCStringEncoding]]);
	BOOL result = [NSBundle loadNibFile: nspath externalNameTable: dict withZone: zone];
	if (!result) {
		post("loadNibFile failed\n");
		return;
	}

	NSWindow *window = [panel window];
	if (!window) {
		post("UserPanel window outlet was not bound.\n");
		return;
	}

	NSView *view = [window contentView];
	[panel setSCObject: (PyrObject*)scobj];
	setupUserPaneViews(panel, view, 0);

	[window makeKeyAndOrderFront: nil];

	// set all targets
};


bool gIsFullScreen = false;

- (void)becomeFullScreen: (NSWindow*)window
{
    int err = CGDisplayCapture(kCGDirectMainDisplay);
    if (err != kCGErrorSuccess) {
        post("Can't capture screen.\n");
        return;
    }

    int windowLevel = CGShieldingWindowLevel();
    [window setLevel: windowLevel];
    gIsFullScreen = true;

    NSRect bounds = [[NSScreen mainScreen] frame];
    [window setFrame: bounds display: YES];
    [window setShowsResizeIndicator: NO];

    //can't set NSBorderlessWindowMask after created
    // but if you create it with that,
    //  subclass NSWindow,override canBecomeKeyWindow { return YES; }
    // or you can't get key events

    // then you can't move it
}

- (void)endFullScreen: (NSWindow*)window
{
    if (gIsFullScreen) {
        CGDisplayRelease(kCGDirectMainDisplay);
        [window setShowsResizeIndicator: YES];
        //[window close];
        gIsFullScreen = false;
    }
}

- (void)windowWillClose:(NSNotification *)aNotification
{
    SCNSWindow* window = [aNotification object];
    [self endFullScreen: window];
    [guiWindows removeObject: window];

    [(SCGraphView*)[window getSCGraphView] willClose];
}

- (void)windowWillMiniaturize:(NSNotification *)notification
{
    if(gIsFullScreen){
		[self sendMain:"exitFullScreen"];
	}
}

- (void) windowDidBecomeKey:(NSNotification *)aNotification
{
    SCNSWindow* window = [aNotification object];
    [(SCGraphView*)[window getSCGraphView] didBecomeKey];
}

- (void) windowDidResignKey:(NSNotification *)aNotification
{
    SCNSWindow* window = [aNotification object];
    [(SCGraphView*)[window getSCGraphView] didResignKey];
}

- (void) runAsModal:(NSWindow*)window {
	[NSApp runModalForWindow: window];

	// after abort:
	[window close];
}

- (void) runAsModalSheet: (NSWindow*)sheet forWindow:(NSWindow*)window {
	SEL sel = @selector(sheetDidEnd:returnCode:contextInfo:);
	[NSApp beginSheet: sheet modalForWindow: window modalDelegate:self didEndSelector:sel contextInfo:NULL];
}

- (void)sheetDidEnd:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo {
	SEL sel = @selector(close);
    NSMethodSignature *sig = [SCNSWindow instanceMethodSignatureForSelector: sel];

    NSInvocation *anInvocation = [NSInvocation invocationWithMethodSignature: sig];
    SCVirtualMachine* scvm = [SCVirtualMachine sharedInstance];
    [anInvocation setTarget: sheet];
    [anInvocation setSelector: sel];
    [scvm defer: anInvocation];
}

- (IBAction)showHelpFor:(id)sender {
	[self sendMain: "showHelp"];
}
- (IBAction)showHelpBrowser:(id)sender {
	[self sendMain: "showHelpBrowser"];
}
- (IBAction)showHelpSearch:(id)sender {
	[self sendMain: "showHelpSearch"];
}
- (IBAction)showClassBrowser:(id)sender {
	[self sendMain: "showClassBrowser"];
}

- (IBAction) executeSelection: (id)sender
{
    [self sendMain: "interpretPrintSelectedText" ];
}
- (IBAction) openCode: (id)sender
{
    [self sendMain: "openCodeFile"];
}
- (IBAction) methodTemplates: (id)sender
{
    [self sendMain: "methodTemplates"];
}
- (IBAction) methodReferences: (id)sender
{
    [self sendMain: "methodReferences"];
}


- (void)applicationWillTerminate:(NSNotification *)notification
{
		[self sendMain: "shutdown"];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification;
{
	SCService* serviceObject = [[SCService alloc] init];
	[NSApp setServicesProvider: serviceObject];
}


@end

void closeAllGUIScreens();
void closeAllGUIScreens()
{
    [[SCVirtualMachine sharedInstance] closeAllGUIWindows];
}


