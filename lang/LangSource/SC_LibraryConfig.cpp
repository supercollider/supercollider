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

// =====================================================================
// SC_LibraryConfig
// Copyright 2003 Maurizio Umberto Puxeddu
// =====================================================================

SC_LibraryConfig *gLibraryConfig = 0;

void SC_LibraryConfig::postExcludedDirectories(void)
{
	char **directories = m_excludedDirectories;
	if (directories != 0) {
		while (directories[0] != 0) {
			post("\texcluding dir: '%s'\n", directories[0]);
			directories += 1;
		}
	}
}

bool SC_LibraryConfig::forEachIncludedDirectory(bool (*func)(char *, int))
{
	char **directories = m_includedDirectories;
	if (directories != 0) {
		while (directories[0] != 0) {
			if (!func(directories[0], 0)) return false;
			directories += 1;
		}
	}
	return true;
}

SC_LibraryConfig::SC_LibraryConfig(void) :
	m_nIncludedDirectories(0),
	m_nExcludedDirectories(0)
{
	m_includedDirectories = (char **)malloc(sizeof(char *) * 1);
	m_excludedDirectories = (char **)malloc(sizeof(char *) * 1);

	m_includedDirectories[0] = 0;
	m_excludedDirectories[0] = 0;
}

SC_LibraryConfig::~SC_LibraryConfig()
{
	int i;

	for (i = 0; i <= m_nIncludedDirectories; ++i)
		free(m_includedDirectories[i]);
	for (i = 0; i <= m_nExcludedDirectories; ++i)
		free(m_excludedDirectories[i]);

	free(m_includedDirectories);
	free(m_excludedDirectories);
}

char **SC_LibraryConfig::includedDirectories(void)
{
	return m_includedDirectories;
}

char **SC_LibraryConfig::excludedDirectories(void)
{
	return m_excludedDirectories;
}

bool SC_LibraryConfig::pathIsExcluded(const char *path)
{
	int i;

	if (m_nExcludedDirectories != 0)
		for (i = 0; i < m_nExcludedDirectories; ++i)
			if (strcmp(path, m_excludedDirectories[i]) == 0) return true;

	return false;
}

void SC_LibraryConfig::addIncludedDirectory(char *path)
{
	char **includedDirectories = m_includedDirectories;

	if (path == 0) return;

	m_includedDirectories = (char **)realloc(m_includedDirectories, (m_nIncludedDirectories + 2) * sizeof(char *));
	if (m_includedDirectories == 0) {
		m_includedDirectories = includedDirectories;
	} else {
		m_includedDirectories[m_nIncludedDirectories] = path;
		m_includedDirectories[m_nIncludedDirectories + 1] = 0;
		m_nIncludedDirectories += 1;
	}
}

void SC_LibraryConfig::addExcludedDirectory(char *path)
{
	char **excludedDirectories = m_excludedDirectories;

	if (path == 0) return;

	m_excludedDirectories = (char **)realloc(m_excludedDirectories, (m_nExcludedDirectories + 2) * sizeof(char *));
	if (m_excludedDirectories == 0) {
		m_excludedDirectories = excludedDirectories;
	} else {
		m_excludedDirectories[m_nExcludedDirectories] = path;
		m_excludedDirectories[m_nExcludedDirectories + 1] = 0;
		m_nExcludedDirectories += 1;
	}
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

	char* str = strdup(realPath);
	if (str == 0) {
		(*mErrorFunc)("%s,%d: memory allocation failure\n", fileName, lineNumber);
		return false;
	}

	if (action == '+') {
		libConf->addIncludedDirectory(str);
	} else if (action == '-') {
		libConf->addExcludedDirectory(str);
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
