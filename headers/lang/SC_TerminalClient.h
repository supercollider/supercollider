// emacs:	-*- c++ -*-
// file:	SC_TerminalClient.h
// author:	stefan kersten <steve@k-hornz.de>
// cvs:		$Id$

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

#ifndef SC_TERMINALCLIENT_H_INCLUDED
#define SC_TERMINALCLIENT_H_INCLUDED

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>

// SC_StringBuffer - Autogrowing string buffer.
// 
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
	void vprintf(const char* fmt, va_list vargs);
	void printf(const char* fmt, ...);

protected:
	void growBy(size_t request);

private:
	char*	mData;
	size_t	mCapacity;
	char*	mPtr;
};

// SC_TerminalClient - Command line sclang client.
//
class SC_TerminalClient
{
public:
	struct Options
	{
		Options()
			: mMemSpace(2*1024*1024),
			  mMemGrow(256*1024),
			  mPort(57120),
			  mCmdLine(0),
			  mCallRun(false),
			  mCallStop(false),
			  mArgc(0),
			  mArgv(0)
		{ }

		int				mMemSpace;		// memory space in bytes
		int				mMemGrow;		// memory growth in bytes
		int				mPort;			// network port number
		char*			mRuntimeDir;	// runtime directory
		SC_StringBuffer	mCmdLine;		// command line to execute
		bool			mCallRun;		// wether to call Main.run at startup
		bool			mCallStop;		// wether to call Main.stop at shutdown
		int				mArgc;			// number of filename arguments
		char**			mArgv;			// filename arguments
	};

public:
	SC_TerminalClient();
	~SC_TerminalClient();

	int run(Options& opt);
	int run(int argc, char** argv);
	void quit(int exitCode=0);

	void setCmdLine(const char* src, size_t size);
	void setCmdLine(const SC_StringBuffer& strBuf);
	void setCmdLine(const char* fmt, ...);
	void runLibrary(void* pyrSymbol);
	void executeFile(const char* fileName);

protected:
	void tick();
	void readCommands(FILE* inputFile);
	static void* tickThreadFunc(void* data);

private:
	bool			mShouldBeRunning;
	int				mExitCode;
	SC_StringBuffer	mScratch;
	pthread_t		mTickThread;
};

#endif // SC_TERMINALCLIENT_H_INCLUDED
