//
//  iscsynthAppDelegate.m
//  iscsynth
//
//  Created by Axel Balley on 20/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import "iscsynthAppDelegate.h"

@implementation iscsynthAppDelegate

@synthesize window;


- (void)applicationDidFinishLaunching:(UIApplication*)application {
    // Override point for customization after application launch
    [window addSubview:[tabBarController view]];

    [window makeKeyAndVisible];
}


- (void)dealloc {
    [window release];
    [super dealloc];
}


@end
