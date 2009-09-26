/*
	SuperColliderAU Copyright (c) 2006 Gerard Roma.

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

#include "Resources.h"
#include  <sys/syslimits.h>
#include <CoreFoundation/CFURL.h>
#include <CoreFoundation/CFURLAccess.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFPropertyList.h>
#include <string.h>

Resources::Resources(){
	SC_PLUGIN_PATH = "/plugins";
	SC_SYNTHDEF_PATH = "/synthdefs";
    PLUGIN_SPEC_FILE = "/pluginSpec.plist";
    SERVER_CONFIG_FILE = "/serverConfig.plist";
}

CFStringRef Resources::getResourcesPathFromDyldImage(){
	const mach_header* header;
	header = (mach_header*)&_mh_bundle_header;
	const char* image_name = 0;
	char buffer[PATH_MAX];

	int cnt = _dyld_image_count();
	for (int i = 1; i < cnt; i++)
	{
		if (_dyld_get_image_header((unsigned long)i) == header)
		{
			image_name = _dyld_get_image_name(i);
			break;
		}
	}

	CFURLRef executableURL = CFURLCreateFromFileSystemRepresentation (kCFAllocatorDefault, (const unsigned char*)image_name, strlen (image_name), false);
	CFURLRef bundleContentsMacOSURL = CFURLCreateCopyDeletingLastPathComponent (kCFAllocatorDefault, executableURL);
	CFRelease (executableURL);
    CFURLRef bundleContentsURL = CFURLCreateCopyDeletingLastPathComponent (kCFAllocatorDefault, bundleContentsMacOSURL);
    CFRelease (bundleContentsMacOSURL);
    CFURLRef bundleURL = CFURLCreateCopyDeletingLastPathComponent (kCFAllocatorDefault, bundleContentsURL);
    CFRelease (bundleContentsURL);
    CFBundleRef bundle = CFBundleCreate (kCFAllocatorDefault, bundleURL);
    CFURLRef bundleResourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
	CFURLGetFileSystemRepresentation(bundleResourcesURL, TRUE, (UInt8*)buffer,PATH_MAX);
	CFStringRef path = CFStringCreateWithCString(NULL,buffer, kCFStringEncodingUTF8);
	return path;
}


CFStringRef Resources::getResourcesPathFromBundleId() { //@@TODO
    char buffer[PATH_MAX];
    CFBundleRef bundle = CFBundleGetBundleWithIdentifier(CFSTR("SuperColldierAU") );
    if (bundle == NULL) return NULL;
    CFURLRef bundleURL = CFBundleCopyBundleURL(bundle);
    CFURLGetFileSystemRepresentation(bundleURL, TRUE, (UInt8*)buffer,PATH_MAX);
    CFStringRef bundlePath = CFStringCreateWithCString(NULL,buffer, kCFStringEncodingUTF8);
    CFURLRef bundleResourcesURL = CFBundleCopyResourcesDirectoryURL(bundle);
    CFStringRef resourcesRelativePath = CFURLGetString(bundleResourcesURL);
    CFMutableStringRef resourcesPath = CFStringCreateMutable(NULL,0);
    CFStringAppend(resourcesPath,bundlePath);
    CFStringAppend(resourcesPath,resourcesRelativePath);
    return resourcesPath;
}


CFStringRef Resources::getResourcePath(const char* resource){
    CFMutableStringRef path = CFStringCreateMutable(NULL,0);
    CFStringAppend(path,getResourcesPathFromDyldImage());
    CFStringAppend(path,  CFStringCreateWithCString(NULL,resource, kCFStringEncodingUTF8));
    return path;
}



CFPropertyListRef Resources::getPropertyList(const char* filename)
{
    CFDataRef xmlCFDataRef;
    CFStringRef error;
    char cerror[10240];
    CFPropertyListRef myCFPropertyListRef = NULL;
    Boolean readOK;
    char  filePathBuf[PATH_MAX];
    CFStringRef filePath = this->getResourcePath(filename);
    CFStringGetCString(filePath, filePathBuf, sizeof(filePathBuf), kCFStringEncodingUTF8);
    CFURLRef fileURL = CFURLCreateFromFileSystemRepresentation (kCFAllocatorDefault, (const unsigned char*)filePathBuf, strlen (filePathBuf), false);
    if (fileURL!=NULL)  {
        readOK = CFURLCreateDataAndPropertiesFromResource( kCFAllocatorDefault, fileURL, &xmlCFDataRef, NULL, NULL, NULL);
        if (readOK)
        {
            myCFPropertyListRef = CFPropertyListCreateFromXMLData(kCFAllocatorDefault, xmlCFDataRef, kCFPropertyListImmutable, &error);
            if (error != NULL){
                CFStringGetCString(error, cerror, sizeof(cerror), kCFStringEncodingUTF8);
                scprintf("getPropertyList error: %s\n", cerror);
            }
            CFRelease(xmlCFDataRef);
        }
        else{
            scprintf("Couldn't read Plist File %s\n", filePathBuf);
        }
    }
    return myCFPropertyListRef;
}