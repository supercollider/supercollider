//
//  iSCLangController.h
//  isclang
//
//  Created by Axel Balley on 26/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import "FileBrowserViewController.h"
#import "LiveCodingViewController.h"
#import "iPhoneSCNSWindow.h"

@interface iSCLangController : NSObject {
	UInt32 routeOverride;
	NSTimer *appClockTimer;
	NSTimer *deferredTaskTimer;
    NSMutableArray *deferredOperations;

	MPMoviePlayerController *recordingPlayer;

	IBOutlet UITabBarController *tabBarController;
	IBOutlet UITextView *logView;
	IBOutlet FileBrowserViewController *browserViewController;
	IBOutlet LiveCodingViewController *liveViewController;
	IBOutlet UIBarButtonItem *speakersButton;
}

@property UInt32 routeOverride;

+ (iSCLangController *) sharedInstance;
- (void) start:(id)arg;
- (void) selectFile:(NSString *)string;
- (void) selectPatch:(NSString *)string;
- (void) selectRecording:(NSString *)string;
- (void) interpret:(NSString *)string;
- (void) doClockTask:(NSTimer*) timer;
- (IBAction) triggerStop:(id)sender;
- (UITextView *) logView;
- (UIBarButtonItem *) speakersButton;
- (IBAction) toggleSpeakers:(id)sender;

- (void) insertWindow:(SCNSWindow *)window;
- (void) makeWindowFront:(SCNSWindow *)window;
- (void) closeWindow:(SCNSWindow *)window;
- (void)defer: (NSInvocation*) action;
- (void)performDeferredOperations;
- (void)removeDeferredOperationsFor:(id) object;

@end
