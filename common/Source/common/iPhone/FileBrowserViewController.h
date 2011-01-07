//
//  FileBrowserViewController.h
//  iscsynth
//
//  Created by Axel Balley on 26/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
/*
@class FileBrowserViewController;

@interface FileTransferController : UIViewController
{
	FileBrowserViewController *browser;
	NSNetService *service;
	NSThread *thread;
	int sock;
	int run_thread;

	IBOutlet UILabel *label;
	IBOutlet UIProgressView *progress;
}
- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle browser:(FileBrowserViewController *)b;
- (void) close:(id)sender;
- (void) start;
- (void) stop;
- (void) threadProc:(id)arg;
- (void) updateLabel:(NSString *)string;
- (void) updateProgress:(NSNumber *)val;

@end
*/

@interface FileBrowserViewController : UINavigationController
{
	id target;
	SEL selector;
}
@end

@interface FileBrowserPageViewController : UIViewController <UITableViewDataSource, UITableViewDelegate>
{
	//FileTransferController *fileController;

	NSArray *array;
	NSString *path;

	IBOutlet UITableView *table;
	IBOutlet UIBarButtonItem *refreshButton;
}

- (id)init;
- (void) setPath:(NSString *)p;
- (void) refresh;
- (void) triggerRefresh:(id)sender;
- (void) flashScrollIndicators;

@end
