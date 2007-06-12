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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/



#include "SC_Lib_Cintf.h"
#include "SC_ComPort.h"
#include "SC_CoreAudio.h"
#include "SC_UnitDef.h"
#include "SC_BufGen.h"
#include "SC_World.h"
#include "SC_StringParser.h"
#include <stdexcept>
#include <dirent.h>

#ifdef SC_DARWIN
# include "dlfcn.h"
#else
# include <dlfcn.h>
#endif

#ifndef SC_PLUGIN_DIR
# define SC_PLUGIN_DIR "plugins"
#endif

#ifndef SC_PLUGIN_EXT
# define SC_PLUGIN_EXT ".scx"
#endif

#ifndef SC_PLUGIN_LOAD_SYM
# define SC_PLUGIN_LOAD_SYM "_load"
#endif

Malloc gMalloc;
HashTable<SC_LibCmd, Malloc> *gCmdLib;
HashTable<struct UnitDef, Malloc> *gUnitDefLib = 0;
HashTable<struct BufGen, Malloc> *gBufGenLib = 0;
HashTable<struct PlugInCmd, Malloc> *gPlugInCmds = 0;
extern struct InterfaceTable gInterfaceTable;
SC_LibCmd* gCmdArray[NUMBER_OF_COMMANDS];

void initMiscCommands();
bool PlugIn_LoadDir(char *dirname);

void initialize_library()
{	
	gCmdLib     = new HashTable<SC_LibCmd, Malloc>(&gMalloc, 64, true);
	gUnitDefLib = new HashTable<UnitDef, Malloc>(&gMalloc, 512, true);
	gBufGenLib  = new HashTable<BufGen, Malloc>(&gMalloc, 512, true);
	gPlugInCmds = new HashTable<PlugInCmd, Malloc>(&gMalloc, 64, true);

	initMiscCommands();

	// load default plugin directory
	PlugIn_LoadDir(SC_PLUGIN_DIR);

	// load user plugin directories
	SC_StringParser sp(getenv("SC_PLUGIN_PATH"), ':');
	while (!sp.AtEnd()) {
		PlugIn_LoadDir(const_cast<char *>(sp.NextToken()));
	}
}





bool PlugIn_Load(const char *filename);
bool PlugIn_Load(const char *filename)
{	
	void* handle = dlopen(filename, RTLD_NOW | RTLD_UNSHARED);
	
	if (!handle) {
		scprintf("*** ERROR: dlopen '%s' err '%s'\n", filename, dlerror());
		dlclose(handle);
		return false;
	}		
	
	void *ptr;
	
	ptr = dlsym(handle, SC_PLUGIN_LOAD_SYM);
	if (!ptr) {
		scprintf("*** ERROR: dlsym %s err '%s'\n", SC_PLUGIN_LOAD_SYM, dlerror());
		dlclose(handle);
		return false;
	}		
	
	LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
	(*loadFunc)(&gInterfaceTable);		

	return true;
}

bool PlugIn_LoadDir(char *dirname);
bool PlugIn_LoadDir(char *dirname)
{
	bool success = true;
	DIR *dir = opendir(dirname);	
	if (!dir) {
		scprintf("*** ERROR: open directory failed '%s'\n", dirname); fflush(stdout);
		return false;
	}
	
	for (;;) {
		struct dirent *de;
		
		de = readdir(dir);
		if (!de) break;
		
        if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) continue;
		char *entrypathname = (char*)malloc(strlen(dirname) + strlen((char*)de->d_name) + 2);
		strcpy(entrypathname, dirname);
		strcat(entrypathname, "/");
		strcat(entrypathname, (char*)de->d_name);

		if (de->d_type == DT_DIR) {
			success = PlugIn_LoadDir(entrypathname);
		} else {
			int dnamelen = strlen(de->d_name);
			int extlen = strlen(SC_PLUGIN_EXT);
			char *extptr = de->d_name+dnamelen-extlen;
			if (strncmp(extptr, SC_PLUGIN_EXT, extlen)==0) {
				success = PlugIn_Load(entrypathname);
			}
		}
		free(entrypathname);
		// sk: process rest of directory if load failed
		if (!success) continue /* break */;
	}
	closedir(dir);
	return success;
}

