#if defined(__APPLE__) || defined(SC_IPHONE)

#include <sys/param.h>
#include <stdexcept>
#include <cstring>  // for strncpy
#include <mach-o/dyld.h> // for _NSGetExecutablePath
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFBundle.h>
//#include <CoreServices/CoreServices.h>

#include "SC_StandAloneInfo_Darwin.h"

bool SC_StandAloneInfo::haveCheckedBundleStatus;
char SC_StandAloneInfo::dirPath[PATH_MAX];

void SC_StandAloneInfo::SC_StandAloneInfoInit() {
	char relDir[PATH_MAX];
	CFStringEncoding encoding = kCFStringEncodingASCII;
	if(!haveCheckedBundleStatus) {
		haveCheckedBundleStatus = true;
		
		CFURLRef enablerURL = CFBundleCopyResourceURL (
			CFBundleGetMainBundle(),
			CFSTR("SCClassLibrary"),
			NULL,
			NULL
		);
		if( !enablerURL ) {
			enablerURL = CFBundleCopyResourceURL (
				  CFBundleGetMainBundle(),
				  CFSTR("sclang.app"),
				  NULL,
				  NULL
				  );
		}
		if ( enablerURL ) {
			// If sclang or SuperCollider binary is run within the .app bundle,
			// this is how we find the Resources path.
			CFStringRef rawUrlPath = CFURLCopyFileSystemPath(enablerURL, kCFURLPOSIXPathStyle);
			CFStringRef rawPath = CFStringCreateWithFormat(NULL, NULL, CFSTR("%@/.."), rawUrlPath);
			CFStringGetCString(rawPath, relDir, PATH_MAX, encoding);
			CFRelease( rawUrlPath );
			CFRelease( rawPath );
			CFRelease( enablerURL );
		} else {
			// when sclang is run from a symlink, the resource URL above will not be found,
			// so we need to find the path of the executable.
			uint32_t bufsize = PATH_MAX, *bufsizep = &bufsize;
			if(_NSGetExecutablePath(relDir, bufsizep)==0) {
				realpath(relDir, dirPath); // resolve symlink
				char *dir = dirname(dirPath);
				strcpy(dirPath, dir);
				return;
			} else {
				// in case it failed, fall back to current directory
				getcwd(dirPath, PATH_MAX);
			}
		}
		realpath(relDir, dirPath);
	}
}

bool SC_StandAloneInfo::IsStandAlone() {
#ifdef SC_STANDALONE
    return true;
#else
    return false;
#endif
}

void SC_StandAloneInfo::GetResourceDir(char* pathBuf, int length)
{
	if ( !haveCheckedBundleStatus )
	{
		SC_StandAloneInfoInit();
	}
	strncpy(pathBuf, dirPath, length);
}

#endif
