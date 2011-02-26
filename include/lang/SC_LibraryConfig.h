// emacs:		-*- c++ -*-
// file:		SC_LibraryConfig.h
// cvs:			$Id$

/*
 *  Copyright 2003 Maurizio Umberto Puxeddu
 *  Copyright 2011 Jakob Leben
 *
 *  This file is part of SuperCollider.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 *
 */

#ifndef SC_LIBRARYCONFIG_H_INCLUDED
#define SC_LIBRARYCONFIG_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>
#include <vector>
#include <string>

// =====================================================================
// SC_LibraryConfigFile
//    simple library configuration file parser
// =====================================================================

class SC_LibraryConfig;

class SC_LibraryConfigFile
{
public:
	typedef void (*ErrorFunc)(const char* fmt, ...);

public:
    SC_LibraryConfigFile(ErrorFunc errorFunc=0);

    bool open(const char* filePath);
    bool read(const char* fileName, SC_LibraryConfig* libConf);
    void close();

protected:
	enum State
	{
		kBegin,
		kAction,
		kPath,
		kEscape,
		kEnvVar,
		kEnvVarName,
		kEnd
	};

	enum
	{
		kMaxIncludeDepth = 10
	};

    bool read(int depth, const char* fileName, SC_LibraryConfig* libConf);
	bool parseLine(int depth, const char* fileName, int lineNumber, const char* line, SC_LibraryConfig* libConf);
	static void defaultErrorFunc(const char* fmt, ...);

private:
	ErrorFunc			mErrorFunc;
	FILE*				mFile;
};

// =====================================================================
// SC_LibraryConfig
//    library configuration management
// Copyright 2003 Maurizio Umberto Puxeddu
// =====================================================================

class SC_LibraryConfig
{
public:
	typedef std::vector<std::string> DirVector;

	SC_LibraryConfig(void);
	virtual ~SC_LibraryConfig();

	const DirVector& includedDirectories() { return mIncludedDirectories; }
	const DirVector& excludedDirectories() { return mExcludedDirectories; }

	void postExcludedDirectories(void);
	bool forEachIncludedDirectory(bool (*func)(const char *, int));

	bool pathIsExcluded(const char *path);

	void addIncludedDirectory(char *name);
	void addExcludedDirectory(char *name);

	// convenience functions to access the global library config
	static bool readLibraryConfig(SC_LibraryConfigFile& file, const char* fileName);
	static void freeLibraryConfig();

private:
	DirVector mIncludedDirectories;
	DirVector mExcludedDirectories;
};

extern SC_LibraryConfig* gLibraryConfig;

#endif // SC_LIBRARYCONFIG_H_INCLUDED
