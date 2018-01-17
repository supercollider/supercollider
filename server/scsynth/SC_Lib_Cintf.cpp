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



#include "SC_Lib_Cintf.h"
#include "SC_CoreAudio.h"
#include "SC_UnitDef.h"
#include "SC_BufGen.h"
#include "SC_World.h"
#include "SC_WorldOptions.h"
#include "SC_StringParser.h"
#include "SC_InterfaceTable.h"
#include "SC_Filesystem.hpp"
#include "ErrorMessage.hpp" // for apiVersionMismatch, apiVersionMissing
#include <stdexcept>
#include <iostream>

#ifndef _MSC_VER
#  include <dirent.h>
#endif // _MSC_VER

#ifdef _WIN32
#  include "SC_Win32Utils.h"
#else
#  include <dlfcn.h>
#  include <libgen.h>
# include <sys/param.h>
#endif // _WIN32

// Plugin directory in resource directory
#define SC_PLUGIN_DIR_NAME "plugins"

// Extension for binary plugins
#ifndef SC_PLUGIN_EXT
#  define SC_PLUGIN_EXT ".scx"
#endif

#include <boost/filesystem/path.hpp> // path
#include <boost/filesystem/operations.hpp> // is_directory

#ifdef __APPLE__
extern "C" {
#  include <mach-o/dyld.h>
#  include <mach-o/getsect.h>
}
char gTempVal;
#endif // __APPLE__

namespace bfs = boost::filesystem;

Malloc gMalloc;
HashTable<SC_LibCmd, Malloc> *gCmdLib;
HashTable<struct UnitDef, Malloc> *gUnitDefLib = 0;
HashTable<struct BufGen, Malloc> *gBufGenLib = 0;
HashTable<struct PlugInCmd, Malloc> *gPlugInCmds = 0;
extern struct InterfaceTable gInterfaceTable;
SC_LibCmd* gCmdArray[NUMBER_OF_COMMANDS];

void initMiscCommands();
static bool PlugIn_LoadDir(const bfs::path& dir, bool reportError);
std::vector<void*> open_handles;
#ifdef __APPLE__
void read_section(const struct mach_header *mhp, unsigned long slide, const char *segname, const char *sectname)
{
	u_int32_t size;
	char *sect = getsectdatafromheader(mhp, segname, sectname, &size);
	if(!sect) return;

	char *start = sect + slide;
	char *end = start + size;

	while(start != end) {
		gTempVal += *(char *)start;
		start++;
	}
}
#endif

extern void IO_Load(InterfaceTable *table);
extern void Osc_Load(InterfaceTable *table);
extern void Delay_Load(InterfaceTable *table);
extern void BinaryOp_Load(InterfaceTable *table);
extern void Filter_Load(InterfaceTable *table);
extern void Gendyn_Load(InterfaceTable *table);
extern void LF_Load(InterfaceTable *table);
extern void Noise_Load(InterfaceTable *table);
extern void MulAdd_Load(InterfaceTable *table);
extern void Grain_Load(InterfaceTable *table);
extern void Pan_Load(InterfaceTable *table);
extern void Reverb_Load(InterfaceTable *table);
extern void Trigger_Load(InterfaceTable *table);
extern void UnaryOp_Load(InterfaceTable *table);
extern void DiskIO_Load(InterfaceTable *table);
extern void Test_Load(InterfaceTable *table);
extern void PhysicalModeling_Load(InterfaceTable *table);
extern void Demand_Load(InterfaceTable *table);
extern void DynNoise_Load(InterfaceTable *table);
extern void FFT_UGens_Load(InterfaceTable *table);
extern void iPhone_Load(InterfaceTable *table);


void deinitialize_library()
{
#ifdef _WIN32
	for(void * ptrhinstance : open_handles){
		HINSTANCE hinstance = (HINSTANCE)ptrhinstance;
		void *ptr = (void *)GetProcAddress( hinstance, "unload" );
		if(ptr){
			UnLoadPlugInFunc unloadFunc = (UnLoadPlugInFunc)ptr;
			(*unloadFunc)();
		}
	}
	//FreeLibrary dlclose(handle);
#else
	for(void * handle : open_handles){
		void *ptr = dlsym(handle, "unload");
		if(ptr){
			UnLoadPlugInFunc unloadFunc = (UnLoadPlugInFunc)ptr;
			(*unloadFunc)();
		}
	}
#endif
	open_handles.clear();
}
void initialize_library(const char *uGensPluginPath)
{
	gCmdLib     = new HashTable<SC_LibCmd, Malloc>(&gMalloc, 64, true);
	gUnitDefLib = new HashTable<UnitDef, Malloc>(&gMalloc, 512, true);
	gBufGenLib  = new HashTable<BufGen, Malloc>(&gMalloc, 512, true);
	gPlugInCmds = new HashTable<PlugInCmd, Malloc>(&gMalloc, 64, true);

	initMiscCommands();

#ifdef STATIC_PLUGINS
	IO_Load(&gInterfaceTable);
	Osc_Load(&gInterfaceTable);
	Delay_Load(&gInterfaceTable);
	BinaryOp_Load(&gInterfaceTable);
	Filter_Load(&gInterfaceTable);
	Gendyn_Load(&gInterfaceTable);
	LF_Load(&gInterfaceTable);
	Noise_Load(&gInterfaceTable);
	MulAdd_Load(&gInterfaceTable);
	Grain_Load(&gInterfaceTable);
	Pan_Load(&gInterfaceTable);
	Reverb_Load(&gInterfaceTable);
	Trigger_Load(&gInterfaceTable);
	UnaryOp_Load(&gInterfaceTable);
	DiskIO_Load(&gInterfaceTable);
	PhysicalModeling_Load(&gInterfaceTable);
	Test_Load(&gInterfaceTable);
	Demand_Load(&gInterfaceTable);
	DynNoise_Load(&gInterfaceTable);
#  if defined(SC_IPHONE) && !TARGET_IPHONE_SIMULATOR
	iPhone_Load(&gInterfaceTable);
#  endif
	FFT_UGens_Load(&gInterfaceTable);
	return;
#endif // STATIC_PLUGINS

	// If uGensPluginPath is supplied, it is exclusive.
	bool loadUGensExtDirs = true;
	if(uGensPluginPath){
		loadUGensExtDirs = false;
		SC_StringParser sp(uGensPluginPath, SC_STRPARSE_PATHDELIMITER);
		while (!sp.AtEnd()) {
			PlugIn_LoadDir(const_cast<char *>(sp.NextToken()), true);
		}
	}

	using DirName = SC_Filesystem::DirName;

	if(loadUGensExtDirs) {
#ifdef SC_PLUGIN_DIR
		// load globally installed plugins
		if (bfs::is_directory(SC_PLUGIN_DIR)) {
			PlugIn_LoadDir(SC_PLUGIN_DIR, true);
		}
#endif // SC_PLUGIN_DIR
		// load default plugin directory
		const bfs::path pluginDir = SC_Filesystem::instance().getDirectory(DirName::Resource) / SC_PLUGIN_DIR_NAME;

		if (bfs::is_directory(pluginDir)) {
			PlugIn_LoadDir(pluginDir, true);
		}
	}

	// get extension directories
	if (loadUGensExtDirs) {
		// load system extension plugins
		const bfs::path sysExtDir = SC_Filesystem::instance().getDirectory(DirName::SystemExtension);
		PlugIn_LoadDir(sysExtDir, false);

		// load user extension plugins
		const bfs::path userExtDir = SC_Filesystem::instance().getDirectory(DirName::UserExtension);
		PlugIn_LoadDir(userExtDir, false);

		// load user plugin directories
		SC_StringParser sp(getenv("SC_PLUGIN_PATH"), SC_STRPARSE_PATHDELIMITER);
		while (!sp.AtEnd()) {
			PlugIn_LoadDir(sp.NextToken(), true);
		}
	}
#ifdef __APPLE__
	/* on darwin plugins are lazily loaded (dlopen uses mmap internally), which can produce audible
		glitches when UGens have to be paged-in. to work around this we preload all the plugins by
		iterating through their memory space. */

#  ifndef __x86_64__
	/* seems to cause a stack corruption on llvm-gcc-4.2, sdk 10.5 on 10.6 */

	unsigned long images = _dyld_image_count();
	for(unsigned long i = 0; i < images; i++) {
		const mach_header	*hdr = _dyld_get_image_header(i);
		unsigned long slide = _dyld_get_image_vmaddr_slide(i);
		const char *name = _dyld_get_image_name(i);
		uint32_t	size;
		char *sect;

		if(!strcmp(name + (strlen(name) - 4), ".scx")) {
			read_section(hdr, slide, "__TEXT", "__text");
			read_section(hdr, slide, "__TEXT", "__const");
			read_section(hdr, slide, "__TEXT", "__cstring");
			read_section(hdr, slide, "__TEXT", "__picsymbol_stub");
			read_section(hdr, slide, "__TEXT", "__symbol_stub");
			read_section(hdr, slide, "__TEXT", "__const");
			read_section(hdr, slide, "__TEXT", "__literal4");
			read_section(hdr, slide, "__TEXT", "__literal8");

			read_section(hdr, slide, "__DATA", "__data");
			read_section(hdr, slide, "__DATA", "__la_symbol_ptr");
			read_section(hdr, slide, "__DATA", "__nl_symbol_ptr");
			read_section(hdr, slide, "__DATA", "__dyld");
			read_section(hdr, slide, "__DATA", "__const");
			read_section(hdr, slide, "__DATA", "__mod_init_func");
			read_section(hdr, slide, "__DATA", "__bss");
			read_section(hdr, slide, "__DATA", "__common");

			read_section(hdr, slide, "__IMPORT", "__jump_table");
			read_section(hdr, slide, "__IMPORT", "__pointers");
		}
	}
#  endif // __x86_64__

#endif // ifdef __APPLE__
}

typedef int (*InfoFunction)();

bool checkAPIVersion(void * f, const char * filename)
{
	using namespace std;
	using namespace scsynth;

	if (f) {
		InfoFunction fn = (InfoFunction)f;
		int pluginVersion = (*fn)();
		if (pluginVersion == sc_api_version)
			return true;
		else
			cout << ErrorMessage::apiVersionMismatch(filename, sc_api_version, pluginVersion) << endl;
	} else {
		cout << ErrorMessage::apiVersionNotFound(filename) << endl;
	}

	return false;
}

bool checkServerVersion(void * f, const char * filename)
{
	if (f) {
		InfoFunction fn = (InfoFunction)f;
		if ((*fn)() == 1)
			return false;
	}
	return true;
}

static bool PlugIn_Load(const bfs::path& filename)
{
#ifdef _WIN32
	HINSTANCE hinstance = LoadLibraryW( filename.wstring().c_str() );
	// here, we have to use a utf-8 version of the string for printing
	// because the native encoding on Windows is utf-16.
	const std::string filename_utf8_str = SC_Codecvt::path_to_utf8_str(filename);
	if (!hinstance) {
		char *s;
		DWORD lastErr = GetLastError();
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, lastErr , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&s, 0, NULL );
		scprintf("*** ERROR: LoadLibrary '%s' err '%s'\n", filename_utf8_str.c_str(), s);
		LocalFree( s );
		return false;
	}

	void *apiVersionPtr = (void *)GetProcAddress( hinstance, "api_version" );
	if (!checkAPIVersion(apiVersionPtr, filename_utf8_str.c_str())) {
		FreeLibrary(hinstance);
		return false;
	}

	void *serverCheckPtr = (void *)GetProcAddress( hinstance, "server_type" );
	if (!checkServerVersion(serverCheckPtr , filename_utf8_str.c_str())) {
		FreeLibrary(hinstance);
		return false;
	}

	void *ptr = (void *)GetProcAddress( hinstance, "load" );
	if (!ptr) {
		char *s;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError() , MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&s, 0, NULL );
		scprintf("*** ERROR: GetProcAddress err '%s'\n", s);
		LocalFree( s );

		FreeLibrary(hinstance);
		return false;
	}

	LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
	(*loadFunc)(&gInterfaceTable);

	// FIXME: at the moment we never call FreeLibrary() on a loaded plugin
	open_handles.push_back(hinstance);
	return true;

#else // (ifndef _WIN32)
	void* handle = dlopen(filename.c_str(), RTLD_NOW);

	if (!handle) {
		scprintf("*** ERROR: dlopen '%s' err '%s'\n", filename.c_str(), dlerror());
		dlclose(handle);
		return false;
	}

	void *apiVersionPtr = (void *)dlsym( handle, "api_version" );
	if (!checkAPIVersion(apiVersionPtr, filename.c_str())) {
		dlclose(handle);
		return false;
	}

	void *serverCheckPtr = (void *)dlsym( handle, "server_type" );
	if (!checkServerVersion(serverCheckPtr , filename.c_str())) {
		dlclose(handle);
		return false;
	}

	void *ptr = dlsym(handle, "load");
	if (!ptr) {
		scprintf("*** ERROR: dlsym load err '%s'\n", dlerror());
		dlclose(handle);
		return false;
	}

	LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
	(*loadFunc)(&gInterfaceTable);

	open_handles.push_back(handle);
	return true;

#endif // _WIN32
}

static bool PlugIn_LoadDir(const bfs::path& dir, bool reportError)
{
	boost::system::error_code ec;
	bfs::recursive_directory_iterator rditer(dir, bfs::symlink_option::recurse, ec);

	if (ec) {
		if (reportError) {
			scprintf(
				"*** ERROR: open directory failed '%s': %s\n",
				SC_Codecvt::path_to_utf8_str(dir).c_str(),
				ec.message().c_str()
			);
			fflush(stdout);
		}
		return false;
	}

	while (rditer != bfs::end(rditer)) {
		const bfs::path path = *rditer;

		if (bfs::is_directory(path)) {
			if (SC_Filesystem::instance().shouldNotCompileDirectory(path))
				rditer.no_push();
			else
				; // do nothing; recursion for free
		} else if (path.extension() == SC_PLUGIN_EXT) {
			// don't need to check result: PlugIn_Load does its own error handling and printing.
			// A `false` return value here just means that loading didn't occur, which is not
			// an error condition for us.
			PlugIn_Load(path);
		} else {
			// not a plugin, do nothing
		}

		rditer.increment(ec);
		if (ec) {
			scprintf(
				"*** ERROR: Could not iterate on directory '%s': %s\n",
				SC_Codecvt::path_to_utf8_str(path).c_str(),
				ec.message().c_str()
			);
			return false;
		}
	}

	return true;
}
