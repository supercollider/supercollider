//
//  DirBrowserView.h
//  iscsynth
//
//  Created by Axel Balley on 26/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class DirBrowserView;

@interface FileTransferController : UIViewController
{	
	DirBrowserView *browser;
	NSNetService *service;
	NSThread *thread;
	int sock;
	int run_thread;

	IBOutlet UILabel *label;
	IBOutlet UIProgressView *progress;
}
- (id) initWithNibName:(NSString *)name bundle:(NSBundle *)bundle browser:(DirBrowserView *)b;
- (void) close:(id)sender;
- (void) start;
- (void) stop;
- (void) threadProc:(id)arg;
- (void) updateLabel:(NSString *)string;
- (void) updateProgress:(NSNumber *)val;

@end

@interface DirBrowserViewController : UIViewController <UITableViewDataSource, UITableViewDelegate>
{
	FileTransferController *fileController;
	
	NSArray *array;
	NSString *root;
	NSString *path;
	
	IBOutlet UITableView *table;
	IBOutlet UINavigationItem *navItem;
	IBOutlet UIBarButtonItem *upItem;
	//UIToolbar *toolbar;
	//UIBarButtonItem *listenItem;

	id target;
	SEL selector;
}

- (id)init;
- (void) setRoot:(NSString *)p;
- (void) setPath:(NSString *)p;
- (NSString *) path;
- (void) setTarget:(id)t withSelector:(SEL)s;
- (void) refresh;
- (void) triggerUp:(id)sender;
- (void) triggerRefresh:(id)sender;
- (void) triggerListen:(id)sender;
- (void) closeListen;
- (void) flashScrollIndicators;

@end
