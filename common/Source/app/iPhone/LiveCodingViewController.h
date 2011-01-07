//
//  LiveCodingViewController.h
//  isclang
//
//  Created by Axel Balley on 30/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LiveCodingViewController : UIViewController <UITextViewDelegate>
{
	IBOutlet UITextView *textView;
	IBOutlet UIButton *doneButton;
	IBOutlet UIButton *execButton;

	id target;
	SEL selector;
}
- (void) setTarget:(id)t withSelector:(SEL)s;
- (void) loadFile:(NSString *)file;
- (void) showButtons: (BOOL)state;
- (IBAction) triggerDone: (id)sender;
- (IBAction) triggerExecute: (id)sender;
- (IBAction) triggerExecuteFile: (id)sender;

@end
