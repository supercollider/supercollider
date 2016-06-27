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
#ifdef _WIN32
# include "SC_Win32Utils.h"
#else
# include <sys/param.h>
# include <unistd.h>
# include <libgen.h>
#endif

#include <fstream>
#include <functional>

#include <boost/filesystem.hpp>
#include "yaml-cpp/yaml.h"

using namespace std;

SC_LanguageConfig *gLanguageConfig = 0;
string SC_LanguageConfig::gConfigFile;

SC_LanguageConfig::SC_LanguageConfig(const char *fileName)
{
	if(fileName != nullptr) {
		mConfigFileDirectory = boost::filesystem::absolute(boost::filesystem::path(fileName)).parent_path();
	}
}

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

void SC_LanguageConfig::setDefaultPaths()
{
	char resourceDir[MAXPATHLEN];
	char classLibraryDir[MAXPATHLEN];
	char systemExtensionDir[MAXPATHLEN];
	char userExtensionDir[MAXPATHLEN];

	sc_GetResourceDirectory(resourceDir, MAXPATHLEN-32);
	sc_GetSystemExtensionDirectory(systemExtensionDir, MAXPATHLEN);
	sc_GetUserExtensionDirectory(userExtensionDir, MAXPATHLEN);

	strcpy(classLibraryDir, resourceDir);

	mResourceDir = resourceDir;
	mSystemExtensionDir = systemExtensionDir;
	mUserExtensionDir = userExtensionDir;

	if( !mExcludeDefaultPaths ) {
		sc_AppendToPath(classLibraryDir, MAXPATHLEN, "SCClassLibrary");
		findPath(mDefaultClassLibraryDirectories, classLibraryDir, true);
	}

	if ( !( sc_IsStandAlone() || mExcludeDefaultPaths ) ) {
		findPath(mDefaultClassLibraryDirectories, systemExtensionDir, true);
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

void SC_LanguageConfig::doRelativeProject(std::function<void(const char *)> func, const char *path) {
	if( mProject ) {
		const char *confDir = mConfigFileDirectory.c_str();
		 if (strncmp(path, confDir, strlen(confDir) ) == 0) {
			boost::filesystem::path path2 = boost::filesystem::path(path);
			func(boost::filesystem::relative(path2, mConfigFileDirectory).c_str());
		} else func(path);
	} else func(path);
}

void SC_LanguageConfig::doRelativeProject(std::function<void(string)> func, string path) {
	if( mProject ) {
		string confDir = mConfigFileDirectory.string();
		if( path.size() >= confDir.size() && std::equal( confDir.begin(), confDir.end(), path.begin() ) ) {
			boost::filesystem::path path2 = boost::filesystem::path(path);
			func(boost::filesystem::relative(path2, mConfigFileDirectory).string());
		} else func(path);
	} else func(path);
}

void SC_LanguageConfig::addIncludedDirectory(const char *path)
{
	if (path == 0) return;
	doRelativeProject([this](const char *p){findPath(mIncludedDirectories, p, true);}, path);
}

void SC_LanguageConfig::addExcludedDirectory(const char *path)
{
	if (path == 0) return;
	doRelativeProject([this](const char *p){findPath(mExcludedDirectories, p, true);}, path);
}

void SC_LanguageConfig::removeIncludedDirectory(const char *path)
{
	char standardPath[PATH_MAX];
	sc_StandardizePath(path, standardPath);
	string str(standardPath);
	doRelativeProject([this](string p){
		DirVector::iterator end = std::remove(mIncludedDirectories.begin(), mIncludedDirectories.end(), p);
		mIncludedDirectories.erase(end, mIncludedDirectories.end());
	}, str);
}

void SC_LanguageConfig::removeExcludedDirectory(const char *path)
{
	string str(path);
	doRelativeProject([this](string p){
		DirVector::iterator end = std::remove(mExcludedDirectories.begin(), mExcludedDirectories.end(), p);
		mExcludedDirectories.erase(end, mExcludedDirectories.end());
	}, str);
}

const char* SC_LanguageConfig::getCurrentConfigPath()
{
	return gConfigFile.c_str();
}

bool SC_LanguageConfig::getExcludeDefaultPaths() const
{
	return mExcludeDefaultPaths;
}

void SC_LanguageConfig::setExcludeDefaultPaths(bool value)
{
	mExcludeDefaultPaths = value;
}

void SC_LanguageConfig::setProject(bool value)
{
	mProject = value;
}

bool SC_LanguageConfig::getProject() const
{
	return mProject;
}

extern bool gPostInlineWarnings;
bool SC_LanguageConfig::readLibraryConfigYAML(const char* fileName)
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig(fileName);

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

			const Node * excludeDefaultPathsNode = doc.FindValue("excludeDefaultPaths");
			if (excludeDefaultPathsNode) {
				try {
					gLanguageConfig->setExcludeDefaultPaths( excludeDefaultPathsNode->to<bool>() );
				} catch(...) {
					postfl("Warning: Cannot parse config file entry \"excludeDefaultPaths\"\n");
				}
			}

			const Node * isProject = doc.FindValue("project");
			if (isProject) {
				try {
					gLanguageConfig->setProject( isProject->to<bool>() );
				} catch(...) {
					postfl("Warning: Cannot parse config file entry \"project\"\n");
				}
			}
		}

		gLanguageConfig->setDefaultPaths();

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

	out << Key << "excludeDefaultPaths";
	out << Value << gLanguageConfig->mExcludeDefaultPaths;

	if(gLanguageConfig->mProject)
	{
	out << Key << "project";
	out << Value << true;
	}

	out << EndMap;
	ofstream fout(fileName);
	fout << out.c_str();
	return true;
}

bool SC_LanguageConfig::defaultLibraryConfig()
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig(nullptr);
	gLanguageConfig->setDefaultPaths();

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
	if (file_exists(user_yaml_config_file)) {
		gConfigFile = user_yaml_config_file;
		configured = readLibraryConfigYAML(user_yaml_config_file.c_str());
	}

	if (!configured) {
		char global_yaml_config_file[] = "/etc/sclang_conf.yaml";
		if (file_exists(global_yaml_config_file)) {
			gConfigFile = global_yaml_config_file;
			configured = readLibraryConfigYAML(global_yaml_config_file);
		}
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
