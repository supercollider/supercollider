//
//  iSCSynthController.mm
//  iscsynth
//
//  Created by Axel Balley on 21/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//  Copyright (C) 2017 Brian Heim. All rights reserved.
//

#import <AudioToolbox/AudioToolbox.h>
#import "iSCSynthAppDelegate.h"
#import "iSCSynthController.h"

#include "SC_Graph.h"
#include "SC_GraphDef.h"
#include "SC_Prototypes.h"
#include "SC_Node.h"
#include "SC_Filesystem.hpp"

#include <cstring>

static iSCSynthController* theController = 0;

int vpost(const char* fmt, va_list ap) {
    char buf[512];
    vsnprintf(buf, sizeof(buf), fmt, ap);

    if (theController)
        [theController log:[NSString stringWithCString:buf encoding:NSASCIIStringEncoding]];
    return 0;
}

@implementation iSCSynthController

- (id)init {
    if (!theController && (self = [super init])) {
        options.mBufLength = 1024;
        timer = 0;
        world = 0;
        lastNodeID = 1000;

        unsigned long route = kAudioSessionOverrideAudioRoute_None;
        AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(route), &route);

        SetPrintFunc(vpost);
        theController = self;
    }

    return self;
}

- (void)awakeFromNib {
    NSFileManager* manager = [NSFileManager defaultManager];
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef url = CFBundleCopyBundleURL(bundle);
    CFStringRef s = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
    CFRelease(url);

    NSError* error;
    char supportpath[256];
    strncpy(supportpath, SC_Filesystem::instance().getDirectory(SC_Filesystem::DirName::UserAppSupport).c_str(), 256);
    NSString* support = [NSString stringWithCString:supportpath encoding:NSASCIIStringEncoding];
    NSString* dir = [support stringByAppendingString:@"/synthdefs"];
    if (![manager fileExistsAtPath:dir]) {
        NSString* from = [s stringByAppendingString:@"/synthdefs"];
        if ([manager fileExistsAtPath:from]) {
            [manager copyItemAtPath:from toPath:dir error:&error];
        }
    }
    CFRelease(s);


    [logView setFont:[[logView font] fontWithSize:9.0f]];
    [logView setTextColor:[UIColor blueColor]];

    [synthdefsViewController setRoot:dir];
    [synthdefsViewController setTarget:self withSelector:@selector(selectSynthdef:)];

    [speakerSwitch setOn:NO];
    [freeAllButton setHidden:YES];

#if !TARGET_IPHONE_SIMULATOR
    [self start];
#endif
}

- (void)start {
    if (world)
        World_Cleanup(world);
    world = World_New(&options);
    if (!world || !World_OpenUDP(world, 57110))
        return;

    timer = [NSTimer scheduledTimerWithTimeInterval:0.5f
                                             target:self
                                           selector:@selector(update:)
                                           userInfo:nil
                                            repeats:YES];
    // if (!toggle.on) [toggle setOn:YES animated:YES];
    //[toggle setTitle:@"Stop" forState:UIControlStateNormal|UIControlStateHighlighted|UIControlStateSelected];
}

- (void)stop {
    if (world)
        World_Cleanup(world);
    world = 0;
    if (timer)
        [timer invalidate];
    timer = 0;
    //[toggle setTitle:@"Start" forState:UIControlStateNormal|UIControlStateHighlighted|UIControlStateSelected];
    // if (toggle.on) [toggle setOn:NO animated:YES];
    [avgCPULabel setText:@"0.0"];
    [peakCPULabel setText:@"0.0"];
}

- (void)freeAllNodes {
    // World_FreeAllGraphDefs(world);
    Group* group = World_GetGroup(world, 0);
    Group_DeleteAll(group);
}

- (void)update:(NSTimer*)timer {
    double avgCPU = world->hw->mAudioDriver->GetAvgCPU();
    [avgCPULabel setText:[NSString stringWithFormat:@"%.1f", avgCPU]];
    double peakCPU = world->hw->mAudioDriver->GetPeakCPU();
    [peakCPULabel setText:[NSString stringWithFormat:@"%.1f", peakCPU]];
    [synthsLabel setText:[NSString stringWithFormat:@"%d", world->mNumGraphs]];
    [ugensLabel setText:[NSString stringWithFormat:@"%d", world->mNumUnits]];

    [freeAllButton setHidden:(BOOL)(!world->mNumGraphs)];
}

- (IBAction)toggleState:(id)sender {
    /*
    if (world) [self stop];
    else [self start];
    */
}

- (IBAction)toggleSpeaker:(id)sender {
    UISwitch* s = (UISwitch*)sender;
    UInt32 route;
    if (s.on)
        route = kAudioSessionOverrideAudioRoute_Speaker;
    else
        route = kAudioSessionOverrideAudioRoute_None;
    AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(route), &route);
}

- (IBAction)triggerFreeAll:(id)sender {
    [self freeAllNodes];
}

- (void)selectSynthdef:(NSString*)string {
    char defpath[256];
    [string getCString:defpath maxLength:256 encoding:NSASCIIStringEncoding];

    GraphDef* def = GraphDef_Load(world, defpath, 0);
    if (!def)
        return;
    Group* group = World_GetGroup(world, 0);
    Graph* graph = 0;
    int data = 0;
    sc_msg_iter msg(0, (char*)&data);
    Graph_New(world, def, lastNodeID++, &msg, &graph);
    if (graph && group) {
        Group_AddTail(group, &graph->mNode);
        Node_StateMsg(&graph->mNode, kNode_Go);
    }
}

- (void)tabBarController:(UITabBarController*)tabBarController
    didSelectViewController:(UIViewController*)viewController {
    if (viewController == logViewController) {
        [logView flashScrollIndicators];
    } else if (viewController == synthdefsViewController) {
        [synthdefsViewController flashScrollIndicators];
    }
}

- (void)log:(NSString*)string {
    NSString* s = [logView text];
    NSString* s2 = [s stringByAppendingString:string];
    [logView setText:s2];
    /*
        int len = [s2 length];
        if (len) [logView scrollRangeToVisible:NSMakeRange(len-1, 0)];
    */
    int offset = [logView contentSize].height - [logView bounds].size.height;
    if (offset >= 0)
        [logView setContentOffset:CGPointMake(0, offset) animated:NO];
}

- (void)dealloc {
    if (world)
        [self stop];

    [super dealloc];
}

@end
