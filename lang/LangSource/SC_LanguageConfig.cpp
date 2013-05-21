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

#include "SC_LanguageConfig.hpp"
#include "SCBase.h"
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

#include <boost/filesystem.hpp>
#include "yaml-cpp/yaml.h"

using namespace std;

SC_LanguageConfig *gLanguageConfig = 0;
string SC_LanguageConfig::gConfigFile;

static bool findPath( SC_LanguageConfig::DirVector & vec, const char * path, bool addIfMissing)
{
	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);

	for ( SC_LanguageConfig::DirVector::iterator it = vec.begin(); it != vec.end(); ++it) {
		typedef boost::filesystem::path Path;
		Path stdPath(standardPath), thisPath(it->c_str());
		stdPath = stdPath / ".";
		thisPath = thisPath / ".";

		if (boost::filesystem::absolute(stdPath) == boost::filesystem::absolute(thisPath))
			return true;
	}

	if (addIfMissing)
		vec.push_back(string(standardPath));

	return false;
}

SC_LanguageConfig::SC_LanguageConfig()
{
	char classLibraryDir[MAXPATHLEN];
	char systemExtensionDir[MAXPATHLEN];
	char userExtensionDir[MAXPATHLEN];

	sc_GetResourceDirectory(classLibraryDir, MAXPATHLEN-32);
	sc_AppendToPath(classLibraryDir, MAXPATHLEN, "SCClassLibrary");
	findPath(mDefaultClassLibraryDirectories, classLibraryDir, true);

	if (!sc_IsStandAlone()) {
		sc_GetSystemExtensionDirectory(systemExtensionDir, MAXPATHLEN);
		findPath(mDefaultClassLibraryDirectories, systemExtensionDir, true);

		sc_GetUserExtensionDirectory(userExtensionDir, MAXPATHLEN);
		findPath(mDefaultClassLibraryDirectories, userExtensionDir, true);
	}
}

void SC_LanguageConfig::postExcludedDirectories(void)
{
	DirVector &vec = mExcludedDirectories;
	DirVector::iterator it;
	for (it=vec.begin(); it!=vec.end(); ++it) {
		post("\texcluding dir: '%s'\n", it->c_str());
	}
}

bool SC_LanguageConfig::forEachIncludedDirectory(bool (*func)(const char *, int))
{
	for (DirVector::iterator it=mDefaultClassLibraryDirectories.begin(); it!=mDefaultClassLibraryDirectories.end(); ++it) {
		if (!pathIsExcluded(it->c_str())) {
			if (!func(it->c_str(), 0))
				return false;
		}
	}

	for (DirVector::iterator it=mIncludedDirectories.begin(); it!=mIncludedDirectories.end(); ++it) {
		if (!pathIsExcluded(it->c_str())) {
			if (!func(it->c_str(), 0))
				return false;
		}
	}

	return true;
}

bool SC_LanguageConfig::pathIsExcluded(const char *path)
{
	return findPath(mExcludedDirectories, path, false);
}

void SC_LanguageConfig::addIncludedDirectory(const char *path)
{
	if (path == 0) return;
	findPath(mIncludedDirectories, path, true);
}

void SC_LanguageConfig::addExcludedDirectory(const char *path)
{
	if (path == 0) return;
	findPath(mExcludedDirectories, path, true);
}

void SC_LanguageConfig::removeIncludedDirectory(const char *path)
{
	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);
	string str(standardPath);
	DirVector::iterator end = std::remove(mIncludedDirectories.begin(), mIncludedDirectories.end(), str);
	mIncludedDirectories.erase(end, mIncludedDirectories.end());
}

void SC_LanguageConfig::removeExcludedDirectory(const char *path)
{
	string str(path);
	DirVector::iterator end = std::remove(mExcludedDirectories.begin(), mExcludedDirectories.end(), str);
	mExcludedDirectories.erase(end, mExcludedDirectories.end());
}

extern bool gPostInlineWarnings;
bool SC_LanguageConfig::readLibraryConfigYAML(const char* fileName)
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig();

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
					gLanguageConfig->addIncludedDirectory(path.c_str());
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
					gLanguageConfig->addExcludedDirectory(path.c_str());
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
	} catch (std::exception & e)
	{
		postfl("Exception when parsing YAML config file: %s\n", e.what());
		freeLibraryConfig();
		return false;
	}
}

bool SC_LanguageConfig::writeLibraryConfigYAML(const char* fileName)
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
	for (DirVector::iterator it = gLanguageConfig->mIncludedDirectories.begin();
		 it != gLanguageConfig->mIncludedDirectories.end(); ++it)
		out << *it;
	out << EndSeq;

	out << Key << "excludePaths";
	out << Value << BeginSeq;
	for (DirVector::iterator it = gLanguageConfig->mExcludedDirectories.begin();
		 it != gLanguageConfig->mExcludedDirectories.end(); ++it)
		out << *it;
	out << EndSeq;

	out << Key << "postInlineWarnings";
	out << Value << gPostInlineWarnings;

	out << EndMap;
	ofstream fout(fileName);
	fout << out.c_str();
	return true;
}

bool SC_LanguageConfig::defaultLibraryConfig(void)
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig();

	return true;
}

static bool file_exists(const char * fileName)
{
	FILE * fp = fopen(fileName, "r");
	if (fp)
		fclose(fp);
	return fp != NULL;
}

static bool file_exists(std::string const & fileName)
{
	return file_exists(fileName.c_str());
}

bool SC_LanguageConfig::readLibraryConfig()
{
	bool configured = false;

	if (!gConfigFile.empty() && file_exists(gConfigFile)) {
		configured = readLibraryConfigYAML(gConfigFile.c_str());
		if (configured)
			return true;
	}

	char config_dir[PATH_MAX];
	sc_GetUserConfigDirectory(config_dir, PATH_MAX);

	std::string user_yaml_config_file = std::string(config_dir) + SC_PATH_DELIMITER + "sclang_conf.yaml";
	if (file_exists(user_yaml_config_file))
		configured = readLibraryConfigYAML(user_yaml_config_file.c_str());

	if (!configured) {
		char global_yaml_config_file[] = "/etc/sclang_conf.yaml";
		if (file_exists(global_yaml_config_file))
			configured = readLibraryConfigYAML(global_yaml_config_file);
	}

	if (configured)
		return true;

	SC_LanguageConfig::defaultLibraryConfig();
	return false;
}

void SC_LanguageConfig::freeLibraryConfig()
{
	if (gLanguageConfig) {
		delete gLanguageConfig;
		gLanguageConfig = 0;
	}
}
