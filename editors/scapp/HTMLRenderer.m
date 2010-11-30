#import "HTMLRenderer.h"

//#define DEBUG

#ifdef DEBUG
	#define Log(fmt, ...) NSLog(fmt, ##__VA_ARGS__)
#else
	#define Log(fmt, ...)
#endif

@implementation HTMLRenderer
+ (NSAttributedString *)attributedStringWithURL:(NSURL *)url {
	HTMLRenderer *htmlRenderer = [[HTMLRenderer alloc] init];
	NSAttributedString *attributedString = [htmlRenderer attributedStringWithURL:url];
	[htmlRenderer release];
	return attributedString;
}

- (id)init {
	if((self = [super init]) == nil)
		return nil;
		
	renderLock = [[NSConditionLock alloc] initWithCondition:0];
	finishedRendering = 0;
	renderWebView = [[WebView alloc] initWithFrame:NSMakeRect(0., 0., 2000., 100.) frameName:nil groupName:nil];
	[renderWebView setFrameLoadDelegate:self];

	return self;
}

- (NSAttributedString *)attributedStringWithURL:(NSURL *)url {
	NSMutableAttributedString *result = nil;
	
	finishedRendering = 0;
	[self renderHTMLFragment:url];
	
	if(finishedRendering == 1)
		return [(id <WebDocumentText>)[[[renderWebView mainFrame] frameView] documentView] attributedString];
	else
		return nil;
}

- (void)renderHTMLFragment:(NSURL *)url {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSRunLoop *runLoop = [NSRunLoop currentRunLoop];

	[NSThread setThreadPriority:1.0];
	[renderLock lock];
	
	[[renderWebView mainFrame] loadRequest:[NSURLRequest requestWithURL:url]];

	while(!finishedRendering)
		[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.05]];
	
	[renderLock unlockWithCondition:1];
	[pool release];
}

- (void)dealloc {
	[renderLock release];
	[renderWebView release];
	[super dealloc];
}

- (void)webView:(WebView *)sender didFailLoadWithError:(NSError *)error forFrame:(WebFrame *)frame {
	finishedRendering = -1;
}

- (void)webView:(WebView *)sender didFailProvisionalLoadWithError:(NSError *)error forFrame:(WebFrame *)frame {
	finishedRendering = -1;
}

- (void)webView:(WebView *) sender didFinishLoadForFrame:(WebFrame *) frame {
	finishedRendering = 1;
}

@end