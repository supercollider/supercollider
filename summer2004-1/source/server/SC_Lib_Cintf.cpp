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
#include "SC_InterfaceTable.h"
#include <stdexcept>
#ifndef _MSC_VER
#include <dirent.h>
#endif //_MSC_VER

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
#ifdef SC_WIN32

    HINSTANCE hinstance = LoadLibrary( filename );
    if (!hinstance) {
        char *s;
        DWORD lastErr = GetLastError();
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                0, lastErr , 0, (char*)&s, 1, 0 );
        scprintf("*** ERROR: LoadLibrary '%s' err '%s'\n", filename, s);
        LocalFree( s );     
		return false;
	}

    void *ptr = GetProcAddress( hinstance, SC_PLUGIN_LOAD_SYM );
    if (!ptr) {
        char *s;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                0, GetLastError(), 0, (char*)&s, 1, 0 );
        scprintf("*** ERROR: GetProcAddress %s err '%s'\n", SC_PLUGIN_LOAD_SYM, s);
        LocalFree( s );

		FreeLibrary(hinstance);
		return false;
	}	

    LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
	(*loadFunc)(&gInterfaceTable);		

    // FIXME: at the moment we never call FreeLibrary() on a loaded plugin
    
	return true;

#else

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
    
#endif
}

#ifdef SC_LINUX
# include <sys/stat.h>
# include <sys/types.h>
#endif // SC_LINUX

bool PlugIn_LoadDir(char *dirname);
#ifndef SC_WIN32
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

		bool isDirectory = false;

#ifdef SC_DARWIN
		isDirectory = (de->d_type == DT_DIR);
#endif // SC_DARWIN
#ifdef SC_LINUX
		{
			struct stat stat_buf;
			isDirectory =
				(stat(entrypathname, &stat_buf) == 0)
				&& S_ISDIR(stat_buf.st_mode);
		}
#endif // SC_LINUX
#ifdef SC_WIN32
		// no d_type in POSIX dirent, so no directory recursion
		isDirectory = false;
#endif SC_WIN32

		if (isDirectory) {
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
#else //#ifndef SC_WIN32
bool PlugIn_LoadDir(char *dirname)
{
  bool success = true;
  std::string folderPath = std::string(dirname);
  folderPath += "\\";
  std::string allInFolder = folderPath; 
  allInFolder += std::string("*.*");
  WIN32_FIND_DATA findData;
  HANDLE hFind = ::FindFirstFile(allInFolder.c_str( ), &findData);
  for(;;) {
    if (hFind == INVALID_HANDLE_VALUE) {
  		scprintf("*** ERROR: ::FindFirstFile() failed '%s' GetLastError() = %d\n", dirname, ::GetLastError()); fflush(stdout);
		  return false;
    } 
    else {
      if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        size_t pathLen = strlen(findData.cFileName);
        if ( (pathLen >= 1) && (strncmp(findData.cFileName,".",1) == 0) ) {
          // "." -> don't do anything
        }
        else if ( (pathLen >= 2) && (strncmp(findData.cFileName,"..",1) == 0) ) {
          // ".." -> don't do anything
        }
        else {
          std::string childFolderPath = folderPath;
          childFolderPath += "\\";
          childFolderPath += findData.cFileName;
          success = PlugIn_LoadDir(const_cast<char*>(childFolderPath.c_str()));
        }
      } 
      else {
        size_t extlen = strlen(SC_PLUGIN_EXT);
        size_t filePathLen = strlen(findData.cFileName);
        if (filePathLen >= extlen) {
          if (strnicmp(findData.cFileName + filePathLen - extlen, SC_PLUGIN_EXT, extlen) == 0) {
            std::string plugInPath = folderPath;
            plugInPath += findData.cFileName;
            success = PlugIn_Load(plugInPath.c_str());
          }
        }
      }
      BOOL nextFileFound = ::FindNextFile(hFind, &findData);
      if (!nextFileFound) {
        if (GetLastError() == ERROR_NO_MORE_FILES)
          return true; 
        else
          return false;
      }
    }
  }
  ::FindClose(hFind);
  return success;
}
#endif //#ifndef SC_WIN32
