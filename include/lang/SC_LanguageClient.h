/*  -*- c++ -*-
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

#ifndef SC_LANGUAGECLIENT_H_INCLUDED
#define SC_LANGUAGECLIENT_H_INCLUDED

#include "SC_StringBuffer.h"
#include "SC_Export.h"
#include "SC_Lock.h"
#include "SC_Win32Utils.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

// =====================================================================
// SC_LanguageClient - abstract sclang client.
// =====================================================================

struct PyrSymbol;
struct VMGlobals;

extern bool compiledOK;

extern pthread_mutex_t gLangMutex;
extern VMGlobals* gMainVMGlobals;


// lock language,
// if shouldBeRunning == false, return EINTR
// if language has been locked, return 0
template <typename FlagType>
static inline int lockLanguageOrQuit(FlagType const & shouldBeRunning)
{
#ifdef __linux__
	// we have pthread_mutex_timedlock

	int status = pthread_mutex_trylock(&gLangMutex);
	if (status == 0) {
		if (shouldBeRunning == false) {
			pthread_mutex_unlock(&gLangMutex);
			return EINTR;
		}
	} else if (status == EBUSY) {
		do {
			struct timespec now;
			clock_gettime(CLOCK_REALTIME, &now);
			now.tv_sec += 1;

			status = pthread_mutex_timedlock(&gLangMutex, &now);
			if (shouldBeRunning == false) {
				if (status == 0)
					pthread_mutex_unlock(&gLangMutex);
				return EINTR;
			}
		} while (status == ETIMEDOUT);
	}
	return status;
#else
	int status;
	do {
		status = pthread_mutex_trylock (&gLangMutex);
		if (shouldBeRunning == false) {
			if (status == 0)
				pthread_mutex_unlock(&gLangMutex);
			return EINTR;
		}
		if (status == EBUSY) {
			struct timespec sleepTime, remain;
			sleepTime.tv_sec = 0;
			sleepTime.tv_nsec = 100000;
			nanosleep(&sleepTime, &remain);
		}
	} while (status);
	return 0;
#endif
}


class SC_DLLEXPORT SC_LanguageClient
{
public:
	struct Options
	{
		Options()
			: mMemSpace(2*1024*1024),
			  mMemGrow(256*1024),
			  mPort(57120),
			  mRuntimeDir(0)
		{ }

		int				mMemSpace;				// memory space in bytes
		int				mMemGrow;				// memory growth in bytes
		int				mPort;					// network port number
		char*			mRuntimeDir;			// runtime directory
	};

public:
	// create singleton instance
	SC_LanguageClient(const char* name);
	virtual ~SC_LanguageClient();

	// singleton instance access locking
	static void lockInstance() { gInstanceMutex.Lock(); }
	static void unlockInstance() { gInstanceMutex.Unlock(); }

	// return the singleton instance
	static SC_LanguageClient* instance() { return gInstance; }
	static SC_LanguageClient* lockedInstance() { lockInstance(); return gInstance; }

	// initialize language runtime
	void initRuntime(const Options& opt=Options());
	void shutdownRuntime();

	// return application name
	const char* getName() const { return mName; }

	// library startup/shutdown
	bool isLibraryCompiled() { return compiledOK; }
	void compileLibrary();
	void shutdownLibrary();
	void recompileLibrary();

	// interpreter access
	void lock() { pthread_mutex_lock(&gLangMutex); }
	bool trylock() { return pthread_mutex_trylock(&gLangMutex) == 0; }
	void unlock() { pthread_mutex_unlock(&gLangMutex); }

	VMGlobals* getVMGlobals() { return gMainVMGlobals; }

	void setCmdLine(const char* buf, size_t size);
	void setCmdLine(const char* str);
	void setCmdLine(const SC_StringBuffer& strBuf);
	void setCmdLinef(const char* fmt, ...);
	void runLibrary(PyrSymbol* pyrSymbol);
	void runLibrary(const char* methodName);
	void interpretCmdLine() { runLibrary(s_interpretCmdLine); }
	void interpretPrintCmdLine() { runLibrary(s_interpretPrintCmdLine); }
	void executeFile(const char* fileName);
	void runMain() { runLibrary(s_run); }
	void stopMain() { runLibrary(s_stop); }

	// post file access
	FILE* getPostFile() { return mPostFile; }
	void setPostFile(FILE* file) { mPostFile = file; }

	// post buffer output (subclass responsibility)
	//     should be thread-save.
	virtual void postText(const char* str, size_t len) = 0;
	virtual void postFlush(const char* str, size_t len) = 0;
	virtual void postError(const char* str, size_t len) = 0;
	// flush post buffer contents to screen.
	//     only called from the main language thread.
	virtual void flush() = 0;

	// common symbols
	//     only valid after the library has been compiled.
	static PyrSymbol* s_interpretCmdLine;
	static PyrSymbol* s_interpretPrintCmdLine;
	static PyrSymbol* s_run;
	static PyrSymbol* s_stop;

	// command line argument handling utilities
	static void snprintMemArg(char* dst, size_t size, int arg);
	static bool parseMemArg(const char* arg, int* res);
	static bool parsePortArg(const char* arg, int* res);

	// AppClock driver
	//    to be called from client mainloop.
	void tick();
	// AppClock driver. WARNING: Must be called locked!
	// Returns whether there is anything scheduled,
	// and writes the scheduled absolute time, if any, into nextTime.
	bool tickLocked( double * nextTime );

protected:
	// language notifications, subclasses can override

	// called after language runtime has been initialized
	virtual void onInitRuntime();
	// called after the library has been compiled
	virtual void onLibraryStartup();
	// called before the library is shut down
	virtual void onLibraryShutdown();
	// called after the interpreter has been started
	virtual void onInterpStartup();

private:
	friend void closeAllGUIScreens();
	friend void initGUIPrimitives();
	friend void initGUI();

private:
	char*						mName;
	FILE*						mPostFile;
	SC_StringBuffer				mScratch;
	bool						mRunning;
	static SC_LanguageClient*	gInstance;
	static SC_Lock gInstanceMutex;
};

// =====================================================================
// library functions
// =====================================================================

extern void setPostFile(FILE* file);
extern "C" int vpost(const char *fmt, va_list vargs);
extern void post(const char *fmt, ...);
extern void postfl(const char *fmt, ...);
extern void postText(const char *text, long length);
extern void postChar(char c);
extern void error(const char *fmt, ...);
extern void flushPostBuf();

#endif // SC_LANGUAGECLIENT_H_INCLUDED
