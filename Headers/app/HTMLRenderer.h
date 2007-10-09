#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface HTMLRenderer : NSObject {
	NSConditionLock *renderLock;
	WebView *renderWebView;
	int finishedRendering;
}

+ (NSAttributedString *) attributedStringWithURL:(NSURL *)url;
- (NSAttributedString *) attributedStringWithURL:(NSURL *)url;
@end
