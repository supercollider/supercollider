//
//  iSCSynthController.h
//  iscsynth
//
//  Created by Axel Balley on 21/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FileBrowserViewController.h"

#include "SC_World.h"
#include "SC_HiddenWorld.h"
#include "SC_CoreAudio.h"
#include "SC_WorldOptions.h"

@interface iSCSynthController : NSObject <UITabBarControllerDelegate> {
    WorldOptions options;
    struct World* world;
    NSTimer* timer;

    int lastNodeID;

    IBOutlet UILabel* avgCPULabel;
    IBOutlet UILabel* peakCPULabel;
    IBOutlet UILabel* synthsLabel;
    IBOutlet UILabel* ugensLabel;
    IBOutlet UISwitch* speakerSwitch;
    IBOutlet UIButton* freeAllButton;

    IBOutlet UITextView* logView;

    IBOutlet UIViewController* logViewController;
    IBOutlet FileBrowserViewController* synthdefsViewController;
}

- (IBAction)toggleState:(id)sender;
- (IBAction)toggleSpeaker:(id)sender;
- (IBAction)triggerFreeAll:(id)sender;
- (void)start;
- (void)stop;
- (void)freeAllNodes;
- (void)update:(NSTimer*)timer;
- (void)log:(NSString*)string;
- (void)selectSynthdef:(NSString*)string;

@end
