//
//  iSCLangController.m
//  isclang
//
//  Created by Axel Balley on 26/10/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//
#import <AudioToolbox/AudioToolbox.h>
#import <MediaPlayer/MediaPlayer.h>

/*
#import "HTTPServer.h"
#import "MyHTTPConnection.h"
*/

#import "iSCLangController.h"
#include <pthread.h>
#include "PyrObject.h"
#include "PyrKernel.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SC_DirUtils.h"
#include "SC_LanguageClient.h"
#include "SC_WorldOptions.h"


#define START_HTTP_SERVER

extern PyrSymbol* s_interpretCmdLine;
extern PyrSymbol* s_run;
extern PyrSymbol* s_tick;

PyrSymbol* s_stop;
PyrSymbol* s_interpretPrintCmdLine;


static iSCLangController* theController = 0;

void closeAllGUIScreens()
{
}


/*
void initGUI()
{
}

void initGUIPrimitives()
{
	s_interpretCmdLine = getsym("interpretCmdLine");
	s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");
	//s_run = getsym("run");
    s_tick = getsym("tick");
}


void initSCViewPrimitives()
{
}
*/

void initRendezvousPrimitives()
{
}

//////////////// queue

struct PostBuf {
	char *buf;
	long wrpos;
	long rdpos;
	pthread_mutex_t mutex;

	void Init();
	void Flush(UITextView *view);
};

static PostBuf mainPostBuf;

#define POSTBUFLEN 131072
#define POSTBUFMASK 131071

void PostBuf::Init()
{
	buf = (char *)malloc(POSTBUFLEN);
	//HoldMemory(buf, POSTBUFLEN);
	wrpos = 0;
	rdpos = 0;
        pthread_mutex_init(&mutex, NULL);
}

void PostBuf::Flush(UITextView *logView)
{
	long numtoread;
	long localwritepos = wrpos;

	if (localwritepos >= rdpos) {
		numtoread = localwritepos - rdpos;
	} else {
		numtoread = POSTBUFLEN - (rdpos - localwritepos);
	}
	if (numtoread > 0) {
		long endpos;
		endpos = rdpos + numtoread;
		if (endpos > POSTBUFLEN) {
			// wrap around end in two copies
			long firstpart, secondpart;

			firstpart = POSTBUFLEN - rdpos;
			endpos -= POSTBUFLEN;
			secondpart = endpos;

			NSString *s = [[logView text] stringByAppendingString:[NSString stringWithCString: buf + rdpos length: firstpart]];
			NSString *s2 = [s stringByAppendingString:[NSString stringWithCString: buf length: secondpart]];
			[logView setText:s2];

			rdpos = endpos;
		} else {
			NSString *s = [[logView text] stringByAppendingString:[NSString stringWithCString: buf + rdpos length: numtoread]];
			[logView setText:s];

			if (endpos == POSTBUFLEN) rdpos = 0;
			else rdpos = endpos;
		}
	int offset = [logView contentSize].height - [logView bounds].size.height;
	if (offset>=0) [logView setContentOffset:CGPointMake(0,offset) animated:NO];
	}
}

void initPostBuffer();
void initPostBuffer()
{
	mainPostBuf.Init();
}

void vposttext(const char *str, int length);
void vposttext(const char *str, int length)
{
	printf(str);

	pthread_mutex_lock(&mainPostBuf.mutex);

	for (int i=0; i<length && str[i]; ++i) {
		if (((mainPostBuf.wrpos+1) & POSTBUFMASK) == mainPostBuf.rdpos) {
			break;
			//mainPostBuf.Flush(); CANNOT DO THIS FROM OTHER THAN COCOA'S THREAD!
		}
		mainPostBuf.buf[mainPostBuf.wrpos] = str[i];
		mainPostBuf.wrpos = (mainPostBuf.wrpos+1) & POSTBUFMASK;
	}
	pthread_mutex_unlock(&mainPostBuf.mutex);
}

void postfl(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	char buf[512];
	int len = vsnprintf(buf, sizeof(buf), fmt, ap);

	vposttext(buf, len);
}

void post(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	char buf[512];
	int len = vsnprintf(buf, sizeof(buf), fmt, ap);

	vposttext(buf, len);
}

void error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	char buf[512];
	int len = vsnprintf(buf, sizeof(buf), fmt, ap);

	vposttext(buf, len);
}

void postText(const char *text, long length)
{
	char buf[512];
	strncpy(buf, text, length);
	buf[length] = 0;

	vposttext(buf, length);
}

void postChar(char c)
{
	char buf[2];
	buf[0] = c;
	buf[1] = 0;

	vposttext(buf, 1);
}

void flushPostBuf()
{
	if (theController) mainPostBuf.Flush([theController logView]);
}

void setPostFile(FILE* file)
{
}

int vpost(const char *fmt, va_list ap)
{
	char buf[512];
	int len = vsnprintf(buf, sizeof(buf), fmt, ap);

	vposttext(buf, len);
	return 0;
}


void setCmdLine(const char *buf)
{
	int size = strlen(buf);
	if (compiledOK) {
		pthread_mutex_lock(&gLangMutex);
		if (compiledOK) {
			VMGlobals *g = gMainVMGlobals;

			PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
			memcpy(strobj->s, buf, size);

			SetObject(&slotRawInterpreter(&g->process->interpreter)->cmdLine, strobj);
			g->gc->GCWrite(slotRawObject(&g->process->interpreter), strobj);
		}
		pthread_mutex_unlock(&gLangMutex);
	}
}

void AudioSessionAudioRouteChangeCbk(void *inClientData, AudioSessionPropertyID inID, UInt32 inDataSize, const void *inData)
{
	iSCLangController *c = (iSCLangController *) inClientData;
	if (inID==kAudioSessionProperty_AudioRouteChange && inData)
	{
		CFDictionaryRef dict = (CFDictionaryRef) inData;
		CFNumberRef reason = (CFNumberRef) CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_Reason));
		SInt32 r;
		CFNumberGetValue(reason, kCFNumberSInt32Type, &r);
		if (r==kAudioSessionRouteChangeReason_Override) return;
		/*
		CFStringRef route;
		UInt32 size = sizeof(route);
		AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &route);
		if (!CFStringCompare(route, CFSTR("Speaker"), 0)) [[c speakersButton] setStyle:UIBarButtonItemStyleDone];
		else [[c speakersButton] setStyle:UIBarButtonItemStyleBordered];
		CFRelease(route);
		*/
		c.routeOverride = kAudioSessionOverrideAudioRoute_None;
		[[c speakersButton] setStyle:UIBarButtonItemStyleBordered];
		/*
		UInt32 override = c.routeOverride;
		if (override==kAudioSessionOverrideAudioRoute_Speaker)
		{
			AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(override), &override);
		}
		*/
	}
}

@implementation iSCLangController

@synthesize routeOverride;

+ (iSCLangController *) sharedInstance
{
	return theController;
}

- (id) init
{
	if (theController)
	{
		[super dealloc];
		return 0;
	}

	if (self=[super init])
	{
		theController = self;
		deferredOperations = [NSMutableArray arrayWithCapacity: 8];
		[deferredOperations retain];
	}

	return self;
}

- (void) awakeFromNib
{
	routeOverride = kAudioSessionOverrideAudioRoute_None;
	AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(routeOverride), &routeOverride);

	AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, AudioSessionAudioRouteChangeCbk, self);


	NSFileManager *manager = [NSFileManager defaultManager];
	CFBundleRef bundle = CFBundleGetMainBundle();
	CFURLRef url = CFBundleCopyBundleURL(bundle);
	NSString *s = (NSString *) CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	CFRelease(url);

	NSError *error;
	char supportpath[256];
	sc_GetUserAppSupportDirectory(supportpath, 256);
	NSString *support = [NSString stringWithCString:supportpath encoding:NSASCIIStringEncoding];

	if (![manager fileExistsAtPath:support]) [manager createDirectoryAtPath:support attributes:nil];

	NSString *dir;
	dir = [support stringByAppendingString:@"/SCClassLibrary"];
	if (![manager fileExistsAtPath:dir])
	{
		[manager createDirectoryAtPath:dir attributes:nil];

		NSString *from, *dest;
		from = [s stringByAppendingString:@"/Common"];
		dest = [dir stringByAppendingString:@"/Common"];
		[manager copyItemAtPath:from toPath:dest error:&error];
		from = [s stringByAppendingString:@"/DefaultLibrary"];
		dest = [dir stringByAppendingString:@"/DefaultLibrary"];
		[manager copyItemAtPath:from toPath:dest error:&error];
		from = [s stringByAppendingString:@"/Platform"];
		dest = [dir stringByAppendingString:@"/Platform"];
		[manager copyItemAtPath:from toPath:dest error:&error];
		from = [s stringByAppendingString:@"/backwards_compatibility"];
		dest = [dir stringByAppendingString:@"/backwards_compatibility"];
		[manager copyItemAtPath:from toPath:dest error:&error];
		from = [s stringByAppendingString:@"/JITLib"];
		dest = [dir stringByAppendingString:@"/JITLib"];
		[manager copyItemAtPath:from toPath:dest error:&error];
	}

	dir = [support stringByAppendingString:@"/synthdefs"];
	if (![manager fileExistsAtPath:dir])
	{
		NSString *from = [s stringByAppendingString:@"/synthdefs"];
		if ([manager fileExistsAtPath:from])
		{
			[manager copyItemAtPath:from toPath:dir error:&error];
		}
	}
	dir = [support stringByAppendingString:@"/sounds"];
	if (![manager fileExistsAtPath:dir])
	{
		NSString *from = [s stringByAppendingString:@"/sounds"];
		if ([manager fileExistsAtPath:from])
		{
			[manager copyItemAtPath:from toPath:dir error:&error];
		}
	}
	dir = [support stringByAppendingString:@"/Extensions"];
	if (![manager fileExistsAtPath:dir])
	{
		NSString *from = [s stringByAppendingString:@"/Extensions"];
		if ([manager fileExistsAtPath:from])
		{
			[manager copyItemAtPath:from toPath:dir error:&error];
		}
	}
	dir = [support stringByAppendingString:@"/plugins"];
	if (![manager fileExistsAtPath:dir])
	{
		NSString *from = [s stringByAppendingString:@"/plugins"];
		if ([manager fileExistsAtPath:from])
		{
			[manager copyItemAtPath:from toPath:dir error:&error];
		}
	}
	dir = [support stringByAppendingString:@"/patches"];
	if (![manager fileExistsAtPath:dir]) [manager createDirectoryAtPath:dir attributes:nil];
	NSString *patches_dir = [s stringByAppendingString:@"/patches"];
	NSArray *patches = [manager contentsOfDirectoryAtPath:patches_dir error:nil];
	for (NSString *patch in patches)
	{
		NSString *origin = [patches_dir stringByAppendingPathComponent:patch];
		NSString *destination = [dir stringByAppendingPathComponent:patch];
		if ([manager fileExistsAtPath:destination]) [manager removeItemAtPath:destination error:nil];
		[manager copyItemAtPath:origin toPath:destination error:&error];
	}
	dir = [support stringByAppendingString:@"/Recordings"];
	if (![manager fileExistsAtPath:dir])
	{
		[manager createDirectoryAtPath:dir attributes:nil];
	}
	dir = [support stringByAppendingString:@"/tmp"];
	if ([manager fileExistsAtPath:dir]) [manager removeItemAtPath:dir error:nil];
	[manager createDirectoryAtPath:dir attributes:nil];

	CFRelease(s);


	initPostBuffer();
	[logView setFont:[[logView font] fontWithSize:9.0f]];
	[logView setTextColor:[UIColor blueColor]];

	NSString *path;
	//path = [support stringByAppendingString:@"/patches"];
	[browserViewController setTarget:self withSelector:@selector(selectFile:)];
	[browserViewController setPath:support];

	[tabBarController setCustomizableViewControllers:nil];

	[liveViewController setTarget:self withSelector:@selector(interpret:)];
/*
#ifdef START_HTTP_SERVER
	HTTPServer *httpServer = [HTTPServer new];
	//[httpServer setType:@"_webdav._tcp."];
	[httpServer setPort:8080];
	[httpServer setConnectionClass:[MyHTTPConnection class]];
	[httpServer setDocumentRoot:[NSURL fileURLWithPath:support]];
	[httpServer start:&error];
#endif
*/
	//[self performSelectorInBackground:@selector(start:) withObject:nil];
	[self performSelector:@selector(start:) withObject:nil afterDelay:0.2f];
}

- (void) start:(id)arg
{
	//pyr_init_mem_pools(2*1024*1024, 256*1024);
	pyr_init_mem_pools(1*1024*1024, 256*1024);
	init_OSC(57120);
	schedInit();
	compileLibrary();

	appClockTimer = [NSTimer scheduledTimerWithTimeInterval:0.02f target:self selector:@selector(doClockTask:) userInfo:nil repeats:YES];
	deferredTaskTimer = [NSTimer scheduledTimerWithTimeInterval: 0.038 target: self selector:@selector(doPeriodicTask:) userInfo: nil repeats: YES];

	s_stop = getsym("stop");
	s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");
}

- (void) selectFile:(NSString *)string
{
	NSString *ext = [string pathExtension];
	if (![ext compare:@"aif"] || ![ext compare:@"aiff"] || ![ext compare:@"wav"]) [self selectRecording:string];
	else [self selectPatch:string];
}

- (void) selectPatch:(NSString *)string
{
	[liveViewController loadFile:string];
	[tabBarController performSelector:@selector(setSelectedViewController:) withObject:liveViewController afterDelay:0.5f];
}

- (void) interpret: (NSString *) string
{
	int length = [string length];
	char *cmd = (char *) malloc(length+1);
	[string getCString:cmd maxLength:length+1 encoding:NSASCIIStringEncoding];
	setCmdLine(cmd);

	if (pthread_mutex_trylock(&gLangMutex) == 0)
	{
		runLibrary(s_interpretPrintCmdLine);
		pthread_mutex_unlock(&gLangMutex);
	}
	free(cmd);
}

- (UITextView *) logView
{
	return logView;
}

- (UIBarButtonItem *) speakersButton
{
	return speakersButton;
}

- (void)doPeriodicTask: (NSTimer*) timer
{
	[self performDeferredOperations];
	//[self doAnimatedViews];
	flushPostBuf();
}

- (void)doClockTask: (NSTimer*) timer
{
	if (pthread_mutex_trylock(&gLangMutex) == 0)
	{
		if (compiledOK) runLibrary(s_tick);
		pthread_mutex_unlock(&gLangMutex);
    }
    flushPostBuf();
}

- (void) triggerStop:(id)sender
{
	if (pthread_mutex_trylock(&gLangMutex) == 0)
	{
        runLibrary(s_stop);
        pthread_mutex_unlock(&gLangMutex);
	}
}


- (void) toggleSpeakers:(id)sender
{
	UIBarButtonItem *b = (UIBarButtonItem *) sender;
	if (routeOverride==kAudioSessionOverrideAudioRoute_None) routeOverride = kAudioSessionOverrideAudioRoute_Speaker;
	else routeOverride = kAudioSessionOverrideAudioRoute_None;
	AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(routeOverride), &routeOverride);

	[b setStyle:(routeOverride==kAudioSessionOverrideAudioRoute_None)?UIBarButtonItemStyleBordered:UIBarButtonItemStyleDone];
}

- (void) insertWindow:(SCNSWindow *)window
{
	NSArray *controllers = [tabBarController viewControllers];
	controllers = [controllers arrayByAddingObject:[window controller]];
	[tabBarController setViewControllers:controllers animated:YES];
}

- (void) makeWindowFront:(SCNSWindow *)window
{
	UINavigationController *more = [tabBarController moreNavigationController];
	/*
	if (more) [tabBarController setSelectedViewController:more];
	else [tabBarController setSelectedViewController:[window controller]];
	*/
	NSArray *controllers = [tabBarController viewControllers];
	if ([controllers count]>5) [tabBarController setSelectedViewController:more];
	else [tabBarController setSelectedViewController:[window controller]];
}

- (void) closeWindow:(SCNSWindow *)window
{
	NSArray *controllers = [tabBarController viewControllers];
	NSArray *new_controllers = [NSArray array];
	UIViewController *the_controller = [window controller];
	UIViewController *selection;
	for (UIViewController *v in controllers)
	{
		if (v!=the_controller)
		{
			new_controllers = [new_controllers arrayByAddingObject:v];
			selection = v;
		}
	}
	[tabBarController setViewControllers:new_controllers animated:YES];
	[tabBarController setSelectedViewController:selection];
	[window close];
}

- (void)defer: (NSInvocation*) action
{
    [deferredOperations addObject: action];
}


- (void)removeDeferredOperationsFor:(id) object
{
	NSMutableArray *newArray = [NSMutableArray arrayWithCapacity: 8];
	[newArray retain];
	for (unsigned int i=0; i<[deferredOperations count]; ++i) {
		NSInvocation* action = (NSInvocation*)[deferredOperations objectAtIndex: i];
		if ([action target] != object) {
			[newArray addObject: action];
		}
	}
	[deferredOperations release];
	deferredOperations = newArray;
}

- (void)performDeferredOperations
{
    while ([deferredOperations count]) {
		NSInvocation* action = (NSInvocation*)[deferredOperations objectAtIndex: 0];
		[action retain];
        [deferredOperations removeObjectAtIndex: 0];
		///NSLog(@"%d %@ %08X\n", [deferredOperations count], action, [action target]);
		[action invoke];
		[action release];
    }
}

- (void) selectRecording:(NSString *)string
{
	NSURL *url = [NSURL fileURLWithPath:string];
	recordingPlayer = [[MPMoviePlayerController alloc] initWithContentURL:url];
	[recordingPlayer setScalingMode:MPMovieScalingModeAspectFill];
	[recordingPlayer setMovieControlMode:MPMovieControlModeDefault];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(recordingPlaybackDidFinish:) name:MPMoviePlayerPlaybackDidFinishNotification object:recordingPlayer];
	[recordingPlayer play];
}

- (void) recordingPlaybackDidFinish:(NSNotification *)notification
{
	[[NSNotificationCenter defaultCenter] removeObserver:self name:MPMoviePlayerPlaybackDidFinishNotification object:recordingPlayer];
	if (recordingPlayer) [recordingPlayer release];
	recordingPlayer = 0;
}

- (void) dealloc
{
	[deferredOperations release];

	[super dealloc];
}

@end
