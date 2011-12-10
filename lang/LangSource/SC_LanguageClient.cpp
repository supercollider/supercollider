/*
	Abstract interpreter interface.
	Copyright (c) 2003 2004 stefan kersten.

	====================================================================

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

#include "SC_LanguageClient.h"
#include "SC_LibraryConfig.h"
#include <cstring>
#include <string>
#include <cerrno>

#ifdef SC_WIN32
# include <stdio.h>
# include <direct.h>
# define snprintf _snprintf
# ifndef PATH_MAX
#  define PATH_MAX _MAX_PATH
# endif
#else
# include <unistd.h>
#endif

#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "GC.h"
#include "VMGlobals.h"
#include "SC_DirUtils.h"
#include "SCBase.h"

void closeAllGUIScreens();
void initGUI();
void initGUIPrimitives();

extern PyrString* newPyrStringN(class PyrGC *gc, long length, long flags, long collect);

// =====================================================================
// SC_LanguageClient
// =====================================================================

SC_LanguageClient* SC_LanguageClient::gInstance = 0;
SC_Lock SC_LanguageClient::gInstanceMutex;

PyrSymbol* SC_LanguageClient::s_interpretCmdLine = 0;
PyrSymbol* SC_LanguageClient::s_interpretPrintCmdLine = 0;
PyrSymbol* SC_LanguageClient::s_run = 0;
PyrSymbol* SC_LanguageClient::s_stop = 0;
static PyrSymbol* s_tick = 0;

SC_LanguageClient::SC_LanguageClient(const char* name)
	: mName(0),
	  mPostFile(0),
	  mScratch(0),
	  mRunning(false)
{
	lockInstance();

	if (gInstance) {
		unlockInstance();
		fprintf(stderr, "SC_LanguageClient already running\n");
		abort();
	}

	mName = strdup(name);
	gInstance = this;

	unlockInstance();
}

SC_LanguageClient::~SC_LanguageClient()
{
	lockInstance();
	free(mName);
	gInstance = 0;
	unlockInstance();
}

void SC_LanguageClient::initRuntime(const Options& opt)
{
	// start virtual machine
	if (!mRunning) {
#ifdef __linux__
		char deprecatedSupportDirectory[PATH_MAX];
		sc_GetUserHomeDirectory(deprecatedSupportDirectory, PATH_MAX);
		sc_AppendToPath(deprecatedSupportDirectory, PATH_MAX, "share/SuperCollider");

		if (sc_DirectoryExists(deprecatedSupportDirectory)) {
			char supportDirectory[PATH_MAX];
			sc_GetUserAppSupportDirectory(supportDirectory, PATH_MAX);
			postfl("WARNING: Deprecated support directory detected: %s\n"
				"Extensions and other contents in this directory will not be available until you move them to the new support directory:\n"
				"%s\n"
				"Quarks will need to be reinstalled due to broken symbolic links.\n\n", deprecatedSupportDirectory, supportDirectory);
		}
#endif

		mRunning = true;
		if (opt.mRuntimeDir) {
			int err = chdir(opt.mRuntimeDir);
			if (err)
				error("Cannot change to runtime directory: %s", strerror(errno));
		}
		pyr_init_mem_pools(opt.mMemSpace, opt.mMemGrow);
		init_OSC(opt.mPort);
		schedInit();
		onInitRuntime();
	}
}

void SC_LanguageClient::shutdownRuntime()
{
	cleanup_OSC();
}

void SC_LanguageClient::compileLibrary()
{
	::compileLibrary();
}

extern void shutdownLibrary();
void SC_LanguageClient::shutdownLibrary()
{
	::shutdownLibrary();
	flush();
}

void SC_LanguageClient::recompileLibrary()
{
	compileLibrary();
}

void SC_LanguageClient::setCmdLine(const char* buf, size_t size)
{
    if (isLibraryCompiled()) {
		lock();
		if (isLibraryCompiled()) {
			VMGlobals *g = gMainVMGlobals;

			PyrString* strobj = newPyrStringN(g->gc, size, 0, true);
			memcpy(strobj->s, buf, size);

			SetObject(&slotRawInterpreter(&g->process->interpreter)->cmdLine, strobj);
			g->gc->GCWrite(slotRawObject(&g->process->interpreter), strobj);
		}
		unlock();
    }
}

void SC_LanguageClient::setCmdLine(const char* str)
{
	setCmdLine(str, strlen(str));
}

void SC_LanguageClient::setCmdLine(const SC_StringBuffer& strBuf)
{
	setCmdLine(strBuf.getData(), strBuf.getSize());
}

void SC_LanguageClient::setCmdLinef(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	mScratch.reset();
	mScratch.vappendf(fmt, ap);
	va_end(ap);
	setCmdLine(mScratch);
}

void SC_LanguageClient::runLibrary(PyrSymbol* symbol)
{
	lock();
    ::runLibrary(symbol);
	unlock();
}

void SC_LanguageClient::runLibrary(const char* methodName)
{
	lock();
    ::runLibrary(getsym(methodName));
	unlock();
}

void SC_LanguageClient::executeFile(const char* fileName)
{
	setCmdLinef("thisProcess.interpreter.executeFile(\"%s\")", fileName);
	runLibrary(s_interpretCmdLine);
}

void SC_LanguageClient::snprintMemArg(char* dst, size_t size, int arg)
{
	int rem = arg;
	int mod = 0;
	const char* modstr = "";

	while (((rem % 1024) == 0) && (mod < 4)) {
		rem /= 1024;
		mod++;
	}

	switch (mod) {
		case 0: modstr = ""; break;
		case 1: modstr = "k"; break;
		case 2: modstr = "m"; break;
		case 3: modstr = "g"; break;
		default: rem = arg; modstr = ""; break;
	}

	snprintf(dst, size, "%d%s", rem, modstr);
}

bool SC_LanguageClient::parseMemArg(const char* arg, int* res)
{
	long value, factor = 1;
	char* endPtr = 0;

	if (*arg == '\0') return false;

	value = strtol(arg, &endPtr, 0);

	char spec = *endPtr++;
	if (spec != '\0') {
		if (*endPtr != '\0')
			// trailing characters
			return false;

		switch (spec) {
			case 'k':
				factor = 1024;
				break;
			case 'm':
				factor = 1024 * 1024;
				break;
			default:
				// invalid mem spec
				return false;
		}
	}

	*res = value * factor;

	return true;
}

bool SC_LanguageClient::parsePortArg(const char* arg, int* res)
{
	long value;
	char* endPtr;

	if (*arg == '\0') return false;

	value = strtol(arg, &endPtr, 0);

	if ((*endPtr != '\0') || (value < 0) || (value > 65535))
		return false;

	*res = value;

	return true;
}

void SC_LanguageClient::tick()
{
    if (trylock()) {
		if (isLibraryCompiled()) {
			::runLibrary(s_tick);
		}
		unlock();
    }
    flush();
}

bool SC_LanguageClient::tickLocked( double * nextTime )
{
	if (isLibraryCompiled()) {
		::runLibrary(s_tick);
	}
	int err = slotDoubleVal( &gMainVMGlobals->result, nextTime );
	return ( err == 0 );
}

void SC_LanguageClient::onInitRuntime()
{
}

void SC_LanguageClient::onLibraryStartup()
{
}

void SC_LanguageClient::onLibraryShutdown()
{
}

void SC_LanguageClient::onInterpStartup()
{
}

// =====================================================================
// library functions
// =====================================================================

// this is defined in PySCLang
#ifndef PYSCLANG
void setPostFile(FILE* file)
{
	SC_LanguageClient::instance()->setPostFile(file);
}

int vpost(const char *fmt, va_list ap)
{
	char buf[512];
	int n = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (n > 0) {
		SC_LanguageClient *client = SC_LanguageClient::lockedInstance();
		if (client) client->postText(buf, sc_min(n, sizeof(buf) - 1));
		SC_LanguageClient::unlockInstance();
	}
	return 0;
}

void post(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
	vpost(fmt, ap);
}

void postfl(const char *fmt, ...)
{
	char buf[512];
    va_list ap;
    va_start(ap, fmt);
	int n = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (n > 0) {
		SC_LanguageClient *client = SC_LanguageClient::lockedInstance();
		if (client) client->postFlush(buf, sc_min(n, sizeof(buf) - 1));
		SC_LanguageClient::unlockInstance();
	}
}

void postText(const char *str, long len)
{
	SC_LanguageClient *client = SC_LanguageClient::lockedInstance();
	if (client) client->postFlush(str, len);
	SC_LanguageClient::unlockInstance();
}

void postChar(char c)
{
	SC_LanguageClient *client = SC_LanguageClient::lockedInstance();
	if (client) client->postFlush(&c, sizeof(char));
	SC_LanguageClient::unlockInstance();
}

void error(const char *fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buf, sizeof(buf), fmt, ap);
	if (n > 0) {
		SC_LanguageClient *client = SC_LanguageClient::lockedInstance();
		if (client) client->postError(buf, sc_min(n, sizeof(buf) - 1));
		SC_LanguageClient::unlockInstance();
	}
}

void flushPostBuf(void)
{
	SC_LanguageClient::instance()->flush();
}

void closeAllGUIScreens()
{
	SC_LanguageClient::instance()->onLibraryShutdown();
}

void initGUI()
{
	SC_LanguageClient::instance()->onInterpStartup();
}

void initGUIPrimitives()
{
	SC_LanguageClient::s_interpretCmdLine = getsym("interpretCmdLine");
	SC_LanguageClient::s_interpretPrintCmdLine = getsym("interpretPrintCmdLine");
	SC_LanguageClient::s_run = getsym("run");
	SC_LanguageClient::s_stop = getsym("stop");
    s_tick = getsym("tick");
	SC_LanguageClient::instance()->onLibraryStartup();
}

void initSCViewPrimitives();
void initSCViewPrimitives()
{
}

void initCocoaFilePrimitives();
void initCocoaFilePrimitives()
{
}

void initCocoaBridgePrimitives();
void initCocoaBridgePrimitives()
{
}

void initRendezvousPrimitives();
void initRendezvousPrimitives()
{
}

#if !defined(HAVE_SPEECH)
void initSpeechPrimitives();
void initSpeechPrimitives()
{
}
#endif // HAVE_SPEECH

long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2);
long scMIDIout(int port, int len, int statushi, int chan, int data1, int data2)
{
    return 0;
}
#endif
// EOF
