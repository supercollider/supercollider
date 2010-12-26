#if defined(__APPLE__) || defined(SC_IPHONE)

#include <sys/param.h>
#include <stdexcept>
#include <cstring>  // for strncpy

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFBundle.h>
//#include <CoreServices/CoreServices.h>

#include "SC_StandAloneInfo_Darwin.h"

bool SC_StandAloneInfo::sIsStandAlone;
bool SC_StandAloneInfo::haveCheckedBundleStatus;
char SC_StandAloneInfo::dirPath[PATH_MAX];

void SC_StandAloneInfo::SC_StandAloneInfoInit() {
	CFStringEncoding encoding = kCFStringEncodingASCII;

	if ( !haveCheckedBundleStatus )
	{
		haveCheckedBundleStatus = true;
		CFStringRef stringToFind = CFSTR("SCClassLibrary");
		CFURLRef enablerURL = CFBundleCopyResourceURL (
			CFBundleGetMainBundle(),
			stringToFind,
			NULL,
			NULL
		);
		if ( enablerURL )
		{
			CFStringRef string2ToFind = CFSTR(".app/");
			CFRange findResult = CFStringFind(CFURLGetString(enablerURL), string2ToFind, kCFCompareCaseInsensitive);
			if(findResult.length != 0)
			{
				// You'd think we could get an absolute path to the Resources directory. But
				// we can't, we can only get a relative path, or an absolute path to a
				// specific resource. Since we don't know the application name, we get the
				// latter, and then hack off the resource name.

				sIsStandAlone = true;
				CFStringRef rawPath = CFURLCopyFileSystemPath(enablerURL, kCFURLPOSIXPathStyle);

				CFRange discardRange = CFStringFind (
				   CFURLCopyFileSystemPath(enablerURL, kCFURLPOSIXPathStyle),
				   stringToFind,
				   0
				);

				CFRange validRange;
				validRange.location = 0;
				validRange.length = discardRange.location - 1;

				CFStringRef dirPathCFString = CFStringCreateWithSubstring (
					kCFAllocatorDefault,
					rawPath,
					validRange
				);

				CFStringGetCString (
					dirPathCFString,
					dirPath,
					PATH_MAX,
					encoding
				);
			}else
			{
				getcwd(dirPath, PATH_MAX);
			}
		}
		else
		{
			getcwd(dirPath, PATH_MAX);
		}
	}
}

bool SC_StandAloneInfo::IsStandAlone() {
	if ( !haveCheckedBundleStatus )
	{
		SC_StandAloneInfoInit();
	}
	return sIsStandAlone;
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
