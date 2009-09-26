//
//  isclangAppDelegate.h
//  isclang
//
//  Created by Axel Balley on 25/10/08.
//  Copyright __MyCompanyName__ 2008. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface isclangAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;

	IBOutlet UITabBarController *tabBarController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end

