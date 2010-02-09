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


#ifndef _Resources_
#define _Resources_
#include "SC_World.h"
#include <CoreFoundation/CFString.h>

#include <mach-o/dyld.h>
#include <mach-o/ldsyms.h>

class Resources  {

	public:
			Resources();
			const char* SC_PLUGIN_PATH;
			const char* SC_SYNTHDEF_PATH;
			const char* PLUGIN_SPEC_FILE;
            const char* SERVER_CONFIG_FILE;
			CFStringRef getControlSpecPath();
			CFStringRef getBundleResourcesPath();
            CFStringRef getResourcePath(const char* resource);
            CFPropertyListRef getPropertyList(const char* filename);

	private:
			CFStringRef getResourcesPathFromDyldImage();
			CFStringRef getResourcesPathFromBundleId();
};

#endif