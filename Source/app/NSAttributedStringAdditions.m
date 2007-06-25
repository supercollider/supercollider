// This code has its origins in the Colloquy project, which is licensed under the GPLv2. It has been heavily modified from the original. -rb

#import <WebKit/WebKit.h>
#import "NSAttributedStringAdditions.h"

//#define DEBUG

#ifdef DEBUG
	#define Log(fmt, ...) NSLog(fmt, ##__VA_ARGS__)
#else
	#define Log(fmt, ...)
#endif

NSConditionLock *renderLock = nil;
WebView *renderWebView = nil;
int finishedRendering = 0;

@implementation NSAttributedString (NSAttributedStringHTMLAdditions)
+ (id) attributedStringWithURL:(NSURL *)url {
	extern NSConditionLock *renderLock;
	extern WebView *renderWebView;
	NSMutableAttributedString *result = nil;

	// renderLock is reused across calls
	if(!renderLock)
		renderLock = [[NSConditionLock alloc] initWithCondition:2];
	
	[renderLock lockWhenCondition:2]; // wait until any other call to this method finishes
	
	finishedRendering = 0;
	[NSThread detachNewThreadSelector:@selector(renderHTMLFragment:) toTarget:self withObject:[NSDictionary dictionaryWithObjectsAndKeys:url, @"url", nil]];
	[renderLock lockWhenCondition:1]; // block until the rendering is done
	
	if(finishedRendering == 1) {
		result = [[[(id <WebDocumentText>)[[[renderWebView mainFrame] frameView] documentView] attributedString] mutableCopy] autorelease];
		[renderLock unlockWithCondition:2]; // set the lock for the next run
		return [[[self alloc] initWithAttributedString:result] autorelease];
	}
	else {
		[renderLock unlockWithCondition:2]; // set the lock for the next run
		return nil;
	}
}

+ (void) renderHTMLFragment:(NSDictionary *) info {
	extern WebView *renderWebView;
	extern NSConditionLock *renderLock;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
	int i =0;

	[NSThread setThreadPriority:1.0];

	NSURL *url = [info objectForKey:@"url"];

	// renderWebView is reused across calls
	if(!renderWebView) renderWebView = [[WebView alloc] initWithFrame:NSMakeRect( 0., 0., 2000., 100. ) frameName:nil groupName:nil];
	[renderWebView setFrameLoadDelegate:self];
	[[renderWebView mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];

	while(!finishedRendering) {
		Log(@"ran loop");
		[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
	}
	
	[renderLock unlockWithCondition:1]; // rendering is complete
	
	Log(@"exiting loop");

	[renderWebView setFrameLoadDelegate:nil];
	[pool release];
}

+ (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame {
	Log(@"error from from webkit");
	finishedRendering = -1;
}

+ (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame {
	Log(@"error from from webkit");
	finishedRendering = -1;
}

+ (void) webView:(WebView *) sender didFinishLoadForFrame:(WebFrame *) frame {
	Log(@"webkit finished rendering");
	finishedRendering = 1;
}

@end