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
/*

Primitives for platform dependent directories, constants etc.

*/

#include "SC_Filesystem.hpp"
#include "SC_LanguageConfig.hpp" // getIdeName
#include "PyrPrimitive.h"
#include "PyrKernel.h"
#ifdef _WIN32
# include "SC_Win32Utils.h"
# include "Shlobj.h"
#endif

using DirName = SC_Filesystem::DirName;

static inline int prPlatform_getDirectory(const struct VMGlobals *g, const DirName dirname)
{
	PyrSlot *a = g->sp;
	const SC_Filesystem::Path& p = SC_Filesystem::instance().getDirectory(dirname);
	PyrString* string = newPyrString(g->gc, p.c_str(), 0, true);
	SetObject(a, string);
	return errNone;
}

static int prPlatform_userHomeDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::UserHome);
}

static int prPlatform_systemAppSupportDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::SystemAppSupport);
}

static int prPlatform_userAppSupportDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::UserAppSupport);
}

static int prPlatform_systemExtensionDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::SystemExtension);
}

static int prPlatform_userExtensionDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::UserExtension);
}

static int prPlatform_userConfigDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::UserConfig);
}

static int prPlatform_resourceDir(struct VMGlobals *g, int numArgsPushed)
{
	return prPlatform_getDirectory(g, DirName::Resource);
}

// @TODO: handle this... :(
#ifdef _WIN32
static int prWinPlatform_myDocumentsDir(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	char path[PATH_MAX];
	win32_GetKnownFolderPath(CSIDL_PERSONAL, path, PATH_MAX);
	PyrString* string = newPyrString(g->gc, path, 0, true);
	SetObject(a, string);
	return errNone;
}
#endif

static int prPlatform_ideName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	PyrString* string = newPyrString(g->gc, SC_LanguageConfig::getIdeName().c_str(), 0, true);
	SetObject(a, string);
	return errNone;
}

void initPlatformPrimitives();
void initPlatformPrimitives()
{
	int base, index = 0;

	base = nextPrimitiveIndex();

	definePrimitive(base, index++, "_Platform_userHomeDir", prPlatform_userHomeDir, 1, 0);
	definePrimitive(base, index++, "_Platform_systemAppSupportDir", prPlatform_systemAppSupportDir, 1, 0);
	definePrimitive(base, index++, "_Platform_userAppSupportDir", prPlatform_userAppSupportDir, 1, 0);
	definePrimitive(base, index++, "_Platform_systemExtensionDir", prPlatform_systemExtensionDir, 1, 0);
	definePrimitive(base, index++, "_Platform_userExtensionDir", prPlatform_userExtensionDir, 1, 0);
	definePrimitive(base, index++, "_Platform_userConfigDir", prPlatform_userConfigDir, 1, 0);
	definePrimitive(base, index++, "_Platform_resourceDir", prPlatform_resourceDir, 1, 0);
	definePrimitive(base, index++, "_Platform_ideName", prPlatform_ideName, 1, 0);
#ifdef _WIN32
	definePrimitive(base, index++, "_WinPlatform_myDocumentsDir", prWinPlatform_myDocumentsDir, 1, 0);
#endif
}

// EOF
