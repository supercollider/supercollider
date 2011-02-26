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

#include "SC_LibraryConfig.h"
#include "SCBase.h"
#include "SC_StringBuffer.h"
#include "SC_DirUtils.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef SC_WIN32
# include "SC_Win32Utils.h"
#else
# include <sys/param.h>
# include <unistd.h>
# include <libgen.h>
#endif

using namespace std;

SC_LibraryConfig *gLibraryConfig = 0;

void SC_LibraryConfig::postExcludedDirectories(void)
{
	DirVector &vec = mExcludedDirectories;
	DirVector::iterator it;
	for (it=vec.begin(); it!=vec.end(); ++it) {
		post("\texcluding dir: '%s'\n", it->c_str());
	}
}

bool SC_LibraryConfig::forEachIncludedDirectory(bool (*func)(const char *, int))
{
	DirVector &vec = mIncludedDirectories;
	DirVector::iterator it;
	for (it=vec.begin(); it!=vec.end(); ++it) {
		if (!func(it->c_str(), 0)) return false;
	}
	return true;
}

SC_LibraryConfig::SC_LibraryConfig(void)
{}

SC_LibraryConfig::~SC_LibraryConfig()
{}

bool SC_LibraryConfig::pathIsExcluded(const char *path)
{
	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);

	DirVector &vec = mExcludedDirectories;
	DirVector::iterator it;
	for (it=vec.begin(); it!=vec.end(); ++it) {
		if (!strcmp(standardPath, it->c_str())) return true;
	}

	return false;
}

void SC_LibraryConfig::addIncludedDirectory(char *path)
{
	if (path == 0) return;

	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);

	mIncludedDirectories.push_back(string(standardPath));
}

void SC_LibraryConfig::addExcludedDirectory(char *path)
{
	if (path == 0) return;

	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);

	mExcludedDirectories.push_back(string(standardPath));
}

bool SC_LibraryConfig::readLibraryConfig(SC_LibraryConfigFile& file, const char* fileName)
{
	freeLibraryConfig();
	gLibraryConfig = new SC_LibraryConfig();
	return file.read(fileName, gLibraryConfig);
}

void SC_LibraryConfig::freeLibraryConfig()
{
	if (gLibraryConfig) {
		delete gLibraryConfig;
		gLibraryConfig = 0;
	}
}

// =====================================================================
// SC_LibraryConfigFile
// =====================================================================

SC_LibraryConfigFile::SC_LibraryConfigFile(ErrorFunc errorFunc)
	: mErrorFunc(errorFunc ? errorFunc : &defaultErrorFunc),
	  mFile(0)
{ }

bool SC_LibraryConfigFile::open(const char* filePath)
{
	close();
#ifdef SC_WIN32
	mFile = fopen(filePath, "rb");
#else
	mFile = fopen(filePath, "r");
#endif
	return mFile != 0;
}

void SC_LibraryConfigFile::close()
{
	if (mFile) {
		fclose(mFile);
		mFile = 0;
	}
}

bool SC_LibraryConfigFile::read(const char* fileName, SC_LibraryConfig* libConf)
{
	return read(0, fileName, libConf);
}

bool SC_LibraryConfigFile::read(int depth, const char* fileName, SC_LibraryConfig* libConf)
{
	if (!mFile) return false;

	bool error = false;
	size_t lineNumber = 1;
    SC_StringBuffer line;

    while (true) {
		int c = fgetc(mFile);
		bool eof = c == EOF;

		if (eof || (c == '\n')) {
			line.finish();
			// go on if line parse failed
            error |= parseLine(depth, fileName, lineNumber, line.getData(), libConf);
            line.reset();
			lineNumber++;
			if (eof) break;
        } else {
            line.append(c);
        }
    }

	return error;
}

bool SC_LibraryConfigFile::parseLine(int depth, const char* fileName, int lineNumber, const char* line, SC_LibraryConfig* libConf)
{
	char action = 0;
	SC_StringBuffer path;
	SC_StringBuffer envVarName;
	State state = kBegin;

	while (true) {
		// NOTE: in some parser states the character just read is
		// written back to be consumed by the following state in the
		// next iteration; this may be slightly inefficient, but makes
		// control flow more obvious.

		char c = *line++;

		if ((c == '\0') || ((c == '#') && (state != kEscape))) {
			break;
		}

		switch (state) {
			case kBegin:
				if (!isspace(c)) {
					line--;
					state = kAction;
				}
				break;
			case kAction:
				if ((c == '+') || (c == '-') || (c == ':')) {
					action = c;
					state = kPath;
				} else {
					(*mErrorFunc)("%s,%d: invalid action '%c'\n", fileName, lineNumber, c);
					return false;
				}
				break;
			case kPath:
				if (c == '\\') {
					state = kEscape;
				} else if (c == '$') {
					state = kEnvVar;
				} else if (isspace(c)) {
					state = kEnd;
				} else {
					path.append(c);
				}
				break;
			case kEscape:
				path.append(c);
				state = kPath;
				break;
			case kEnvVar:
				if (isalpha(c)) {
					line--;
					state = kEnvVarName;
					envVarName.reset();
				} else {
					(*mErrorFunc)("%s,%d: empty variable reference\n", fileName, lineNumber);
					return false;
				}
				break;
			case kEnvVarName:
				if (isalpha(c) || (c == '_')) {
					envVarName.append(c);
				} else {
					envVarName.finish();
					char* envVarValue = getenv(envVarName.getData());
					if (envVarValue) {
						line--;
						state = kPath;
						path.append(envVarValue);
					} else {
						(*mErrorFunc)("%s,%d: undefined variable '%s'\n", fileName, lineNumber, envVarName.getData());
						return false;
					}
				}
				break;
			case kEnd:
				if (!isspace(c)) {
					(*mErrorFunc)("%s,%d: trailing garbage\n", fileName, lineNumber);
					return false;
				}
				break;
			default:
				(*mErrorFunc)("%s,%d: [internal error] invalid parser state %d\n", fileName, lineNumber, state);
				return false;
		}
	}

	if (!action) return true;

	if (path.getSize() == 0) {
		(*mErrorFunc)("%s,%d: empty path\n", fileName, lineNumber);
		return false;
	}

	path.finish();
	char realPath[MAXPATHLEN];

	if (sc_StandardizePath(path.getData(), realPath) == 0) {
		(*mErrorFunc)("%s,%d: couldn't resolve path %s\n", fileName, lineNumber, path.getData());
		return false;
	}

	if (action == ':') {
		if (++depth > kMaxIncludeDepth) {
			(*mErrorFunc)("%s,%d: maximum include depth of %d exceeded\n", fileName, lineNumber, kMaxIncludeDepth);
			return false;
		}
		SC_LibraryConfigFile file(mErrorFunc);
		if (!file.open(realPath)) return true;
		const char* fileName = basename(realPath);
		bool success = file.read(depth, fileName, libConf);
		file.close();
		return success;
	}

	if (action == '+') {
		libConf->addIncludedDirectory(realPath);
	} else if (action == '-') {
		libConf->addExcludedDirectory(realPath);
	}

	return true;
}

void SC_LibraryConfigFile::defaultErrorFunc(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
}

// EOF
