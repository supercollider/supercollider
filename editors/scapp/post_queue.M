/*
	SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include <SCGeom.h>
#import <Cocoa/Cocoa.h>

char *gHomePath;
CFBundleRef gMainBundle;
NSTextView *gPostView;
SCColor postTextColor;

void SetupHomeDirectory();
void SetupHomeDirectory()
{
	char path[2048];
	gMainBundle = CFBundleGetMainBundle();
	CFURLRef mainURL = CFBundleCopyBundleURL(gMainBundle);
	CFStringRef mainPath = CFURLCopyFileSystemPath(mainURL, kCFURLPOSIXPathStyle);
	CFStringGetCString(mainPath, path, 2048, kCFStringEncodingUTF8);
	*strrchr(path, '/') = 0;
	int mainPathLen = strlen(path);
	
	/*
	path[mainPathLen] = '/';
	
	CFURLRef sharedURL = CFBundleCopySharedSupportURL(gMainBundle);
	CFStringRef sharedPath = CFURLCopyFileSystemPath(sharedURL, kCFURLPOSIXPathStyle);
	CFStringGetCString(sharedPath, path+mainPathLen+1, 2048-mainPathLen-1, kCFStringEncodingUTF8);
	*/
	
	gHomePath = (char*)malloc(mainPathLen + 1);
	strcpy(gHomePath, path);
	chdir(gHomePath);
}



struct PostBuf {
	char *buf;
	long wrpos;
	long rdpos;
	pthread_mutex_t mutex;
        
	void Init();
	void Flush();
};

static PostBuf mainPostBuf;

#define POSTBUFLEN 131072
#define POSTBUFMASK 131071

FILE *postfile=NULL;

void PostBuf::Init()
{
	buf = (char *)malloc(POSTBUFLEN);
	//HoldMemory(buf, POSTBUFLEN);
	postTextColor = SCMakeColor(0.0, 0.0, 0.0, 1.0); //default black
	wrpos = 0;
	rdpos = 0;
        pthread_mutex_init(&mutex, NULL);
}

void initPostBuffer();
void initPostBuffer()
{
	mainPostBuf.Init();
}

void setPostFile(FILE *file)
{
	postfile = file;
}

void vposttext(const char *str, int length);
void vposttext(const char *str, int length)
{
	pthread_mutex_lock(&mainPostBuf.mutex);

	if (postfile) {
		fwrite(str, length, 1, postfile);
		fflush(postfile);
	}
	
	for (int i=0; i<length && str[i]; ++i) {
		if (((mainPostBuf.wrpos+1) & POSTBUFMASK) == mainPostBuf.rdpos) {
			break;
			//mainPostBuf.Flush(); CANNOT DO THIS FROM OTHER THAN COCOA'S THREAD!
		}
		if (str[i] == '\n') mainPostBuf.buf[mainPostBuf.wrpos] = '\r';
		else mainPostBuf.buf[mainPostBuf.wrpos] = str[i];
		mainPostBuf.wrpos = (mainPostBuf.wrpos+1) & POSTBUFMASK;
	}
        pthread_mutex_unlock(&mainPostBuf.mutex);
}

extern "C" {
	int vpost(const char *fmt, va_list vargs);
}

int vpost(const char *fmt, va_list vargs)
{
	char str[512];
	
	int len = vsnprintf(str, 512, fmt, vargs);
	vposttext(str, len);
	return 0;
}

void postfl(const char *fmt, ...);
void postfl(const char *fmt, ...)
{
	va_list vargs;
	
	if (gPostView) {
		va_start(vargs, fmt); 
		vpost(fmt, vargs);
	}
}

void post(const char *fmt, ...);
void post(const char *fmt, ...)
{
	va_list vargs;
	
	if (gPostView) {
		va_start(vargs, fmt); 
		vpost(fmt, vargs);
	}
}

void error(const char *fmt, ...);
void error(const char *fmt, ...)
{
	va_list vargs;

	if (gPostView) {
		post("\xa5 ERROR: ");
		va_start(vargs, fmt); 
		vpost(fmt, vargs);
	}
}



void postText(const char *text, long length);
void postText(const char *text, long length)
{	
	if (gPostView) {
		vposttext(text, length);
	}
}

void postChar(char c);
void postChar(char c)
{	
	if (gPostView) {
		vposttext(&c, 1);
	}
}


void flushPostBuf();
void flushPostBuf()
{
	// can't from other threads..
	mainPostBuf.Flush();
}

void PostBuf::Flush()
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
					
			if (gPostView) {
				NSString* string;
				NSRange range = [gPostView selectedRange];
	
				if ([gPostView shouldChangeTextInRange: range replacementString: nil]) {
					//					string = [NSString stringWithCString: buf + rdpos length: firstpart];
					string = [NSString stringWithCString: buf + rdpos encoding:NSASCIIStringEncoding];
					string = [string substringToIndex:firstpart];
					[gPostView replaceCharactersInRange: range withString: string];
	
					//					string = [NSString stringWithCString: buf length: secondpart];
					string = [NSString stringWithCString: buf encoding:NSASCIIStringEncoding];
					string = [string substringToIndex: secondpart];
					range = [gPostView selectedRange];
					[gPostView replaceCharactersInRange: range withString: string];
					[gPostView didChangeText];
					
					range = [gPostView selectedRange];
					[gPostView scrollRangeToVisible: range];
				}
			}
			rdpos = endpos;
		} else {
			if (gPostView) {
				//				NSString* string = [NSString stringWithCString: buf + rdpos length: numtoread];
				NSString* string = [NSString stringWithCString:buf + rdpos encoding:NSASCIIStringEncoding];
				string = [string substringToIndex:numtoread];
				NSRange range = [gPostView selectedRange];
				
				if ([gPostView shouldChangeTextInRange: range replacementString: string]) {
					[gPostView replaceCharactersInRange: range withString: string];
					[gPostView setTextColor: [NSColor colorWithCalibratedRed: postTextColor.red 
																	   green: postTextColor.green 
																		 blue: postTextColor.blue 
																	   alpha: postTextColor.alpha] 
									   range: NSMakeRange(range.location, [gPostView selectedRange].location - range.location)];
					[gPostView didChangeText];
					
					range = [gPostView selectedRange];
					[gPostView scrollRangeToVisible: range];
				}
			}
			if (endpos == POSTBUFLEN) rdpos = 0;
			else rdpos = endpos;
		}		
	}
}

