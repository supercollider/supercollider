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

#include "SC_DirUtils.h"
#include "PyrPrimitive.h"
#include "PyrKernel.h"

#define PATH_CONSTANT_PRIM_BODY(func) \
	PyrSlot *a = g->sp; \
	char* path = (char*)malloc(PATH_MAX); \
	func(path, PATH_MAX); \
	PyrString* string = newPyrString(g->gc, path, 0, false); \
	SetObject(a, string); \
	free(path); \
	return errNone

int prPlatform_systemAppSupportDir(struct VMGlobals *g, int numArgsPushed);
int prPlatform_systemAppSupportDir(struct VMGlobals *g, int numArgsPushed)
{
	PATH_CONSTANT_PRIM_BODY(sc_GetSystemAppSupportDirectory);
}

int prPlatform_userAppSupportDir(struct VMGlobals *g, int numArgsPushed);
int prPlatform_userAppSupportDir(struct VMGlobals *g, int numArgsPushed)
{
	PATH_CONSTANT_PRIM_BODY(sc_GetUserAppSupportDirectory);
}

int prPlatform_systemExtensionDir(struct VMGlobals *g, int numArgsPushed);
int prPlatform_systemExtensionDir(struct VMGlobals *g, int numArgsPushed)
{
	PATH_CONSTANT_PRIM_BODY(sc_GetSystemExtensionDirectory);
}

int prPlatform_userExtensionDir(struct VMGlobals *g, int numArgsPushed);
int prPlatform_userExtensionDir(struct VMGlobals *g, int numArgsPushed)
{
	PATH_CONSTANT_PRIM_BODY(sc_GetUserExtensionDirectory);
}

int prPlatform_ideName(struct VMGlobals *g, int numArgsPushed);
int prPlatform_ideName(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	PyrString* string = newPyrString(g->gc, gIdeName, 0, false);
	SetObject(a, string);
	return errNone;
}

void initPlatformPrimitives();
void initPlatformPrimitives()
{
	int base, index = 0;

	base = nextPrimitiveIndex();

	definePrimitive(base, index++, "_Platform_systemAppSupportDir", prPlatform_systemAppSupportDir, 1, 0);
	definePrimitive(base, index++, "_Platform_userAppSupportDir", prPlatform_userAppSupportDir, 1, 0);
	definePrimitive(base, index++, "_Platform_systemExtensionDir", prPlatform_systemExtensionDir, 1, 0);
	definePrimitive(base, index++, "_Platform_userExtensionDir", prPlatform_userExtensionDir, 1, 0);
	definePrimitive(base, index++, "_Platform_ideName", prPlatform_ideName, 1, 0);
}

// EOF
