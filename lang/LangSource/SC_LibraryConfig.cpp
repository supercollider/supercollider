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

#include <fstream>
#include "yaml-cpp/yaml.h"

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

static bool findPath(SC_LibraryConfig::DirVector & vec, const char * path, bool addIfMissing)
{
	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);

	for (SC_LibraryConfig::DirVector::iterator it = vec.begin(); it != vec.end(); ++it)
		if (!strcmp(standardPath, it->c_str()))
			return true;

	if (addIfMissing)
		vec.push_back(string(standardPath));

	return false;
}

bool SC_LibraryConfig::pathIsExcluded(const char *path)
{
	return findPath(mExcludedDirectories, path, false);
}

void SC_LibraryConfig::addIncludedDirectory(const char *path)
{
	if (path == 0) return;
	findPath(mIncludedDirectories, path, true);
}

void SC_LibraryConfig::addExcludedDirectory(const char *path)
{
	if (path == 0) return;
	findPath(mExcludedDirectories, path, true);
}

void SC_LibraryConfig::removeIncludedDirectory(const char *path)
{
	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);
	string str(standardPath);
	DirVector::iterator end = std::remove(mIncludedDirectories.begin(), mIncludedDirectories.end(), str);
	mIncludedDirectories.erase(end, mIncludedDirectories.end());
}

void SC_LibraryConfig::removeExcludedDirectory(const char *path)
{
	string str(path);
	DirVector::iterator end = std::remove(mExcludedDirectories.begin(), mExcludedDirectories.end(), str);
	mExcludedDirectories.erase(end, mExcludedDirectories.end());
}

bool SC_LibraryConfig::readLibraryConfig(const char* fileName)
{
	freeLibraryConfig();
	gLibraryConfig = new SC_LibraryConfig();
	SC_LibraryConfigFile file(::post);
	if (!file.read(fileName, gLibraryConfig))
		return true;

	freeLibraryConfig();
	return false;
}

extern bool gPostInlineWarnings;
bool SC_LibraryConfig::readLibraryConfigYAML(const char* fileName)
{
	freeLibraryConfig();
	gLibraryConfig = new SC_LibraryConfig();

	using namespace YAML;
	try {
		std::ifstream fin(fileName);
		Parser parser(fin);

		Node doc;
		while(parser.GetNextDocument(doc)) {
			const Node * includePaths = doc.FindValue("includePaths");
			if (includePaths && includePaths->Type() == NodeType::Sequence) {
				for (Iterator it = includePaths->begin(); it != includePaths->end(); ++it) {
					Node const & pathNode = *it;
					if (pathNode.Type() != NodeType::Scalar)
						continue;
					string path;
					pathNode.GetScalar(path);
					gLibraryConfig->addIncludedDirectory(path.c_str());
				}
			}

			const Node * excludePaths = doc.FindValue("excludePaths");
			if (excludePaths && excludePaths->Type() == NodeType::Sequence) {
				for (Iterator it = excludePaths->begin(); it != excludePaths->end(); ++it) {
					Node const & pathNode = *it;
					if (pathNode.Type() != NodeType::Scalar)
						continue;
					string path;
					pathNode.GetScalar(path);
					gLibraryConfig->addExcludedDirectory(path.c_str());
				}
			}

			const Node * inlineWarnings = doc.FindValue("postInlineWarnings");
			if (inlineWarnings) {
				try {
					gPostInlineWarnings = inlineWarnings->to<bool>();
				} catch(...) {
					postfl("Warning: Cannot parse config file entry \"postInlineWarnings\"\n");
				}
			}
		}
		return true;
	} catch (...)
	{
		freeLibraryConfig();
		return false;
	}
}

bool SC_LibraryConfig::writeLibraryConfigYAML(const char* fileName)
{
	using namespace YAML;
	Emitter out;
	out.SetIndent(4);
	out.SetMapFormat(Block);
	out.SetSeqFormat(Block);
	out.SetBoolFormat(TrueFalseBool);

	out << BeginMap;

	out << Key << "includePaths";
	out << Value << BeginSeq;
	for (DirVector::iterator it = gLibraryConfig->mIncludedDirectories.begin();
		 it != gLibraryConfig->mIncludedDirectories.end(); ++it)
		out << *it;
	out << EndSeq;

	out << Key << "excludePaths";
	out << Value << BeginSeq;
	for (DirVector::iterator it = gLibraryConfig->mExcludedDirectories.begin();
		 it != gLibraryConfig->mExcludedDirectories.end(); ++it)
		out << *it;
	out << EndSeq;

	out << Key << "postInlineWarnings";
	out << Value << gPostInlineWarnings;

	out << EndMap;
	ofstream fout(fileName);
	fout << out.c_str();
	return true;
}

bool SC_LibraryConfig::defaultLibraryConfig(void)
{
	freeLibraryConfig();
	gLibraryConfig = new SC_LibraryConfig();

	char compileDir[MAXPATHLEN];
	char systemExtensionDir[MAXPATHLEN];
	char userExtensionDir[MAXPATHLEN];

	sc_GetResourceDirectory(compileDir, MAXPATHLEN-32);
	sc_AppendToPath(compileDir, "SCClassLibrary");
	gLibraryConfig->addIncludedDirectory(compileDir);

	if (!sc_IsStandAlone()) {
		sc_GetSystemExtensionDirectory(systemExtensionDir, MAXPATHLEN);
		gLibraryConfig->addIncludedDirectory(systemExtensionDir);

		sc_GetUserExtensionDirectory(userExtensionDir, MAXPATHLEN);
		gLibraryConfig->addIncludedDirectory(userExtensionDir);
	}
	return true;
}

static bool file_exists(const char * fileName)
{
	FILE * fp = fopen(fileName, "r");
	if (fp)
		fclose(fp);
	return fp != NULL;
}

bool SC_LibraryConfig::readDefaultLibraryConfig()
{
	char config_dir[PATH_MAX];
	sc_GetUserConfigDirectory(config_dir, PATH_MAX);
	std::string config_file = std::string(config_dir) + SC_PATH_DELIMITER + "sclang.cfg";
	std::string yaml_config_file = std::string(config_dir) + SC_PATH_DELIMITER + "sclang_conf.yaml";

	// skip deprecated config files
	const char* paths[2] = { ".sclang.cfg", "~/.sclang.cfg"};
	for (int i=0; i < 2; i++) {
		const char * ipath = paths[i];
		char opath[PATH_MAX];
		if (sc_StandardizePath(ipath, opath)) {
			if (file_exists(opath))
				postfl("skipping deprecated config file: %s\n"
					   "please use %s instead\n", opath, yaml_config_file.c_str());
		}
	}

	if (file_exists(yaml_config_file.c_str())) {
		if (file_exists(config_file.c_str()))
			postfl("skipping deprecated config file: %s\n", config_file.c_str());

		bool success = readLibraryConfigYAML(yaml_config_file.c_str());
		if (success)
			return true;
	}

	if (readLibraryConfig(config_file.c_str()))
		return true;

	if (file_exists("/etc/sclang_conf.yaml")) {
		if (file_exists("/etc/sclang.cfg"))
			postfl("skipping deprecated config file: /etc/sclang.cfg\n");
		readLibraryConfigYAML(yaml_config_file.c_str());
		return true;
	}

	if (readLibraryConfig("/etc/sclang.cfg"))
		return true;

	SC_LibraryConfig::defaultLibraryConfig();
	return false;
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
