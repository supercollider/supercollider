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

// =====================================================================
// SC_TerminalClient - command line sclang client.
// =====================================================================

class SC_TerminalClient : public SC_LanguageClient
{
public:
	enum
	{
		kInterpretCmdLine = 0x1b,
		kInterpretPrintCmdLine = 0x0c
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

		char*			mLibraryConfigFile;
		bool			mDaemon;
		bool			mCallRun;
		bool			mCallStop;
		int				mArgc;
		char**			mArgv;
	};

	SC_TerminalClient(const char* name);

	const Options& options() const { return mOptions; }
	bool shouldBeRunning() const { return mShouldBeRunning; }

	int run(int argc, char** argv);
	void quit(int code);

	virtual void postText(const char* str, size_t len);
	virtual void postFlush(const char* str, size_t len);
	virtual void postError(const char* str, size_t len);
	virtual void flush();

protected:
	bool parseOptions(int& argc, char**& argv, Options& opt);
	void printUsage();

	// fd is assumed to be non-blocking
	bool readCmdLine(int fd, SC_StringBuffer& cmdLine);
	void interpretCmdLine(PyrSymbol* method, SC_StringBuffer& cmdLine);
	void interpretCmdLine(PyrSymbol* method, const char* cmdLine);

	// subclasses should override
	virtual void commandLoop();
	virtual void daemonLoop();

	static int prArgv(struct VMGlobals* g, int);
	static int prExit(struct VMGlobals* g, int);
	virtual void onLibraryStartup();

private:
	bool				mShouldBeRunning;
	int					mReturnCode;
	Options				mOptions;
};

#endif // SC_TERMINALCLIENT_H_INCLUDED
