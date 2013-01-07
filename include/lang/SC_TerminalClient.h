/*  -*- c++ -*-
	Commandline interpreter interface.
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

#ifndef SC_TERMINALCLIENT_H_INCLUDED
#define SC_TERMINALCLIENT_H_INCLUDED

#include "SC_LanguageClient.h"
#include "SC_StringBuffer.h"
#include <pthread.h>

// =====================================================================
// SC_TerminalClient - command line sclang client.
// =====================================================================

// TODO: move locks out of the header, possibly using pimpl
class SC_DLLEXPORT SC_TerminalClient : public SC_LanguageClient
{
public:
	enum
	{
		kInterpretCmdLine = 0x1b,
		kInterpretPrintCmdLine = 0x0c,
		kRecompileLibrary = 0x18 // ctrl+x
	};

	enum Signal
	{
		sig_input = 0x01, // there is new input
		sig_sched = 0x02, // something has been scheduled
		sig_recompile = 0x04, // class lib recompilation requested
		sig_stop = 0x08 // call Main:-stop
	};

	struct Options : public SC_LanguageClient::Options
	{
		Options()
			: mLibraryConfigFile(0),
			  mDaemon(false),
			  mCallRun(false),
			  mCallStop(false),
			  mArgc(0), mArgv(0)
		{ }

		const char*		mLibraryConfigFile;
		bool			mDaemon;
		bool			mCallRun;
		bool			mCallStop;
		int				mArgc;
		char**			mArgv;
	};

	SC_TerminalClient(const char* name);
	virtual ~SC_TerminalClient();

	const Options& options() const { return mOptions; }
	bool shouldBeRunning() const { return mShouldBeRunning; }

	int run(int argc, char** argv);
	void quit(int code);

	virtual void postText(const char* str, size_t len);
	virtual void postFlush(const char* str, size_t len);
	virtual void postError(const char* str, size_t len);
	virtual void flush();

	// Requests an action to be taken on the main thread.
	// NOTE: It may be called from any thread, and with interpreter locked.
	virtual void sendSignal( Signal code );

protected:
	bool parseOptions(int& argc, char**& argv, Options& opt);
	void printUsage();

	void interpretCmdLine(const char* cmdLine, bool silent);
	void interpretCmdLine(const char *buf, size_t size, bool silent);

	void lockInput() { pthread_mutex_lock(&mInputMutex); }
	void unlockInput() { pthread_mutex_unlock(&mInputMutex); }

	// --------------------------------------------------------------

	// NOTE: Subclasses should call from main thread
	// after receiving sig_input, and with input locked:
	void interpretInput();

	// --------------------------------------------------------------

	// Language requested the application to quit
	// NOTE: It may be called from any thread, and with interpreter locked.
	virtual void onQuit( int exitCode );

	// See super class
	virtual void onLibraryStartup();

	// --------------------------------------------------------------

	// NOTE: Subclasses should override:
	virtual void commandLoop();
	virtual void daemonLoop();

	// --------------------------------------------------------------

	static int prArgv(struct VMGlobals* g, int);
	static int prExit(struct VMGlobals* g, int);
	static int prScheduleChanged( struct VMGlobals *, int);
	static int prRecompile(struct VMGlobals *, int);

private:
	// NOTE: called from input thread:
#ifdef HAVE_READLINE
	static void readlineInit();
	static void *readlineFunc(void *);
	static int readlineRecompile(int, int);
	static void readlineCmdLine(char *cmdLine);
#endif
	static void *pipeFunc( void * );
	void pushCmdLine( SC_StringBuffer &buf, const char *newData, size_t size );

	void initInput();
	void startInput();
	void endInput();
	void cleanupInput();

	// helpers
	void lockSignal() { pthread_mutex_lock(&mSignalMutex); }
	void unlockSignal() { pthread_mutex_unlock(&mSignalMutex); }

	bool				mShouldBeRunning;
	int					mReturnCode;
	Options				mOptions;

	// signals to main thread
	pthread_mutex_t mSignalMutex;
	pthread_cond_t mCond;
	int mSignals;

	// command input
	bool mUseReadline;
	bool mInputShouldBeRunning;
	SC_StringBuffer mInputBuf;
#ifndef _WIN32
	int mInputCtlPipe[2];
#else
	HANDLE mQuitInputEvent;
#endif
	pthread_t mInputThread;
	pthread_mutex_t mInputMutex;
	pthread_cond_t mInputCond;
};

#endif // SC_TERMINALCLIENT_H_INCLUDED
