#import <WebKit/WebKit.h>
#import "NSAttributedStringAdditions.h"

// This code has its origins in the Colloquy project, which is licensed under the GPLv2. It has been heavily modified from the original. -rb

static NSConditionLock *renderLock = nil;
static WebView *renderWebView = nil;

@implementation NSAttributedString (NSAttributedStringHTMLAdditions)
+ (id) attributedStringWithURL:(NSURL *)url {
	extern NSConditionLock *renderLock;
	extern WebView *renderWebView;
	NSMutableAttributedString *result = nil;

	// renderLock is reused across calls
	if(!renderLock)
		renderLock = [[NSConditionLock alloc] initWithCondition:2];

	if([renderLock lockWhenCondition:2 beforeDate:[NSDate dateWithTimeIntervalSinceNow:2.]]) { // wait until any other call to this method finishes; timeout after 2 seconds waiting
		[renderLock unlockWithCondition:0];

		[NSThread detachNewThreadSelector:@selector(renderHTMLFragment:) toTarget:self withObject:[NSDictionary dictionaryWithObjectsAndKeys:url, @"url", nil]];

		if( [renderLock lockWhenCondition:1 beforeDate:[NSDate dateWithTimeIntervalSinceNow:3.]] ) { // wait until the rendering is done; timeout in 3 seconds
			result = [[[(id <WebDocumentText>)[[[renderWebView mainFrame] frameView] documentView] attributedString] mutableCopy] autorelease];
			[renderLock unlockWithCondition:2];
		}
	}

	return [[[self alloc] initWithAttributedString:result] autorelease];
}

+ (void) renderHTMLFragment:(NSDictionary *) info {
	extern WebView *renderWebView;
	extern NSConditionLock *renderLock;
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	[renderLock lockWhenCondition:0]; // start the rendering, make the parent thread block

	[NSThread setThreadPriority:1.0];

	NSURL *url = [info objectForKey:@"url"];

	// renderWebView is reused across calls
	if(!renderWebView) renderWebView = [[WebView alloc] initWithFrame:NSMakeRect( 0., 0., 2000., 100. ) frameName:nil groupName:nil];
	[renderWebView setFrameLoadDelegate:self];
	[[renderWebView mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];

	[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.25]];

	[renderLock lockWhenCondition:2];
	[renderLock unlockWithCondition:2];

	[pool release];
}

+ (void) webView:(WebView *) sender didFinishLoadForFrame:(WebFrame *) frame {
	extern NSConditionLock *renderLock;
	[renderLock unlockWithCondition:1]; // rendering is complete
	[sender setFrameLoadDelegate:nil];
}

@end