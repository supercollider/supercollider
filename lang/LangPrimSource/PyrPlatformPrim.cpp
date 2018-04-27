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

#include "SC_Filesystem.hpp" // getDirectory
#include "SC_Filesystem.hpp" // path_to_utf8_str
#include "SC_LanguageConfig.hpp" // getIdeName
#include "SC_PrimRegistry.hpp"
#include "PyrPrimitive.h"
#include "PyrKernel.h"
#ifdef _WIN32
# include "SC_Win32Utils.h"
# include "Shlobj.h"
#endif

#include <boost/filesystem/path.hpp> // path

LIBSCLANG_PRIMITIVE_GROUP( Platform );

namespace bfs = boost::filesystem;
using DirName = SC_Filesystem::DirName;

static inline int Platform_getDirectory(const struct VMGlobals *g, const DirName dirname)
{
	PyrSlot *a = g->sp;
	const bfs::path& p = SC_Filesystem::instance().getDirectory(dirname);
	PyrString* string = newPyrString(g->gc, SC_Codecvt::path_to_utf8_str(p).c_str(), 0, true);
	SetObject(a, string);
	return errNone;
}

SCLANG_DEFINE_PRIMITIVE( Platform_userHomeDir, 1 )
{
	return Platform_getDirectory(g, DirName::UserHome);
}

SCLANG_DEFINE_PRIMITIVE( Platform_systemAppSupportDir, 1 )
{
	return Platform_getDirectory(g, DirName::SystemAppSupport);
}

SCLANG_DEFINE_PRIMITIVE( Platform_userAppSupportDir, 1 )
{
	return Platform_getDirectory(g, DirName::UserAppSupport);
}

SCLANG_DEFINE_PRIMITIVE( Platform_systemExtensionDir, 1 )
{
	return Platform_getDirectory(g, DirName::SystemExtension);
}

SCLANG_DEFINE_PRIMITIVE( Platform_userExtensionDir, 1 )
{
	return Platform_getDirectory(g, DirName::UserExtension);
}

SCLANG_DEFINE_PRIMITIVE( Platform_userConfigDir, 1 )
{
	return Platform_getDirectory(g, DirName::UserConfig);
}

SCLANG_DEFINE_PRIMITIVE( Platform_resourceDir, 1 )
{
	return Platform_getDirectory(g, DirName::Resource);
}

#ifdef _WIN32
SCLANG_DEFINE_PRIMITIVE( WinPlatform_myDocumentsDir, 1 )
{
	return Platform_getDirectory(g, DirName::MyDocuments);
}
#endif

SCLANG_DEFINE_PRIMITIVE( Platform_ideName, 1 )
{
	PyrSlot *a = g->sp;
	PyrString* string = newPyrString(g->gc, SC_Filesystem::instance().getIdeName().c_str(), 0, true);
	SetObject(a, string);
	return errNone;
}
