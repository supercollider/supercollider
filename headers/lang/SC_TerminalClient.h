// emacs:		-*- c++ -*-
// file:		SC_TerminalClient.h
// copyright:	2003 stefan kersten <steve@k-hornz.de>
// cvs:			$Id$

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

#ifndef SC_TERMINALCLIENT_H_INCLUDED
#define SC_TERMINALCLIENT_H_INCLUDED

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

// =====================================================================
// SC_StringBuffer - Autogrowing string buffer.
// =====================================================================

class SC_StringBuffer
{
public:
	SC_StringBuffer(size_t initialSize=0);
	SC_StringBuffer(const SC_StringBuffer& other);
	~SC_StringBuffer();

	size_t getCapacity() const { return mCapacity; }
	size_t getSize() const { return mPtr - mData; }
	size_t getRemaining() const { return mCapacity - getSize(); }
	char* getData() const { return mData; }

	bool isEmpty() const { return getSize() == 0; }

	void finish() { append('\0'); }
	void reset() { mPtr = mData; }
	void append(const char* src, size_t len);
	void append(char c);
	void append(const char* str);
	void vappendf(const char* fmt, va_list vargs);
	void appendf(const char* fmt, ...);

protected:
	enum {
		kGrowAlign = 256,
		kGrowMask = kGrowAlign - 1
	};

	void growBy(size_t request);

private:
	size_t	mCapacity;
	char*	mPtr;
	char*	mData;
};

// =====================================================================
// SC_LanguageClient - abstract sclang client.
// =====================================================================

struct PyrSymbol;

class SC_LanguageClient
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

		int				mMemSpace;		// memory space in bytes
		int				mMemGrow;		// memory growth in bytes
		int				mPort;			// network port number
		const char*		mRuntimeDir;	// runtime directory
	};

public:
	// create the singleton instance.
	//     name is used for command line error reporting
	//     and should be the name of the executable
	SC_LanguageClient(const char* name);
	virtual ~SC_LanguageClient();

	// return the singleton instance.
	static SC_LanguageClient* instance() { return gInstance; }

	// initialize the language runtime with command line options.
	//     common options are stripped from the passed argv.
	//     subclass overrides should call this method.
	virtual void init(int& argc, char**& argv);
	void init(const Options& opt=Options());

	// return application name.
	const char* getName() const { return mName; }

	// print command line usage info.
	void printUsage();

	// library startup/shutdown
	void compileLibrary();
	void shutdownLibrary();
	void recompileLibrary();

	// interpreter access
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
	//     these routines should be thread-save.
	virtual void post(const char *fmt, va_list ap, bool error) = 0;
	virtual void post(char c) = 0;
	virtual void post(const char* str, size_t len) = 0;
	virtual void flush() = 0;

	// common symbols
	//     only valid after the library was compiled.
	static PyrSymbol* s_interpretCmdLine;
	static PyrSymbol* s_interpretPrintCmdLine;
	static PyrSymbol* s_run;
	static PyrSymbol* s_stop;

protected:
	// command line usage output.
	// NOTE: formatting
	//       - option/usage indent column:  4
	//       - option description column : 35
	// see SC_TerminalClient for an example.
	virtual void printUsage(FILE* file);		// print one-line usage info
	virtual void printOptions(FILE* file);		// print client options section

	// these exit the application by default.
	// maybe should just throw an exception.
	virtual void errorOptionInvalid(const char* opt);
	virtual void errorOptionArgExpected(const char* opt);
	virtual void errorOptionArgInvalid(const char* opt, const char* arg);

	// AppClock driver, to be called from client mainloop.
	void tick();

	// language notifications, subclasses can override.

	// called after the library has been compiled.
	virtual void onLibraryStartup();
	// called before the library is shut down.
	virtual void onLibraryShutdown();
	// called after the interpreter has been started.
	virtual void onInterpStartup();

private:
	friend void closeAllGUIScreens();
	friend void initGUIPrimitives();
	friend void initGUI();

	// command line handling utilities
	void snprintMemArg(char* dst, size_t size, int arg);
	int parseMemArg(const char* arg);
	int parsePortArg(const char* arg);
	void parseOptions(int& argc, char**& argv, Options& opt);

private:
	char*						mName;
	FILE*						mPostFile;
	SC_StringBuffer				mScratch;
	Options						mOptions;
	bool						mRunning;
	static SC_LanguageClient*	gInstance;
};

// =====================================================================
// SC_TerminalClient - command line sclang client.
// =====================================================================

class SC_TerminalClient : public SC_LanguageClient
{
public:
	SC_TerminalClient();

	int run(int argc, char** argv);

	void post(const char *fmt, va_list ap, bool error);
	void post(char c);
	void post(const char* str, size_t len);
	void flush();

protected:
	void printUsage(FILE* file);
	void printOptions(FILE* file);

	int readCommands(FILE* inputFile);
	static void* tickThreadFunc(void* data);

private:
	int				mArgc;
	char**			mArgv;
	bool			mShouldBeRunning;
	pthread_t		mTickThread;
};

#endif // SC_TERMINALCLIENT_H_INCLUDED
