//
//  LiveCodingViewController.m
//  isclang
//
//  Created by Axel Balley on 30/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "LiveCodingViewController.h"

extern void rtf2txt(char *txt);

@implementation LiveCodingViewController

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil])
	{
		target = 0;
		selector = 0;
	}
	return self;
}

- (void) viewDidLoad
{
	[textView setAutocapitalizationType:UITextAutocapitalizationTypeNone];
	[textView setAutocorrectionType:UITextAutocorrectionTypeNo];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardDidShow:) name:UIKeyboardDidShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
	[self showButtons:NO];
}

- (void) keyboardDidShow:(NSNotification *) notif
{
	NSDictionary *info = [notif userInfo];
	NSValue *val = [info objectForKey:UIKeyboardFrameEndUserInfoKey];
	CGRect r = [val CGRectValue];
	
	CGRect done_frame = doneButton.frame;
	CGRect exec_frame = execButton.frame;
	
	r = [self.view convertRect:r fromView:nil];
				
	done_frame.origin.y = r.origin.y - done_frame.size.height - 10;
	exec_frame.origin.y = done_frame.origin.y;
	
	[doneButton setFrame:done_frame];
	[execButton setFrame:exec_frame];
	
	[self showButtons:YES];
}

- (void) keyboardWillHide:(NSNotification *) notif
{
	[self showButtons:NO];
}

- (BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return YES;
}

- (void) setTarget:(id)t withSelector:(SEL)s
{
	target = t;
	selector = s;
}

- (void) loadFile:(NSString *)file
{
	NSString *contents = [NSString stringWithContentsOfFile:file encoding:NSASCIIStringEncoding error:nil];
	int length = [contents length];
	char *buf = (char *) malloc(length+1);
	[contents getCString:buf maxLength:length+1 encoding:NSASCIIStringEncoding];
	rtf2txt(buf);
	while (*buf=='\n') buf++;
	
	[textView setText:[NSString stringWithCString:buf encoding:NSASCIIStringEncoding]];
}

- (void) showButtons: (BOOL)state
{
	[doneButton setHidden:!state];
	[execButton setHidden:!state];
}

- (void) textViewDidBeginEditing: (UITextView *)theView
{
	//[self showButtons:YES];
}

- (IBAction) triggerDone: (id)sender
{
	[textView resignFirstResponder];
	[self showButtons:NO];
}

- (IBAction) triggerExecute: (id)sender
{
	NSRange range = [textView selectedRange];
	NSString *text = [textView text];
	NSUInteger start, end;
	[text getLineStart:&start end:&end contentsEnd:nil forRange:range];
	NSString *line = [text substringWithRange:NSMakeRange(start, end-start)];
	
	if (target && [target respondsToSelector:selector]) [target performSelector:selector withObject:line];
	
	[textView resignFirstResponder];
	[self showButtons:NO];
}

- (IBAction) triggerExecuteFile: (id)sender
{
	if (target && [target respondsToSelector:selector]) [target performSelector:selector withObject:[textView text]];
}

- (void) dealloc
{
    [super dealloc];
}


@end
