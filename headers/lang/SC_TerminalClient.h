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
			  mCallStop(false)
		{ }

		char*			mLibraryConfigFile;
		bool			mDaemon;
		bool			mCallRun;
		bool			mCallStop;
	};

	SC_TerminalClient();

	int run(int argc, char** argv);
	void quit(int code);

	virtual void post(const char *fmt, va_list ap, bool error);
	virtual void post(char c);
	virtual void post(const char* str, size_t len);
	virtual void flush();

protected:
	void parseOptions(int& argc, char**& argv, Options& opt);
	void printUsage();

	void interpretCmdLine(PyrSymbol* method, SC_StringBuffer& cmdLine);
	void commandLoop();
	void daemonLoop();

	static int prExit(struct VMGlobals* g, int);
	virtual void onLibraryStartup();

private:
	bool				mShouldBeRunning;
	int					mReturnCode;
};

#endif // SC_TERMINALCLIENT_H_INCLUDED
