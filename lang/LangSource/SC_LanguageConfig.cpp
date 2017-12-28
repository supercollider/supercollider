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
#include "SC_Filesystem.hpp"               // getDirectory

#include "SCBase.h"                        // postfl

#include <algorithm>                       // std::find
#include <functional>

#include <boost/filesystem/operations.hpp> // exists (, canonical?)
#include <boost/filesystem/fstream.hpp>    // ofstream

SC_LanguageConfig::Path SC_LanguageConfig::gConfigFile;
bool SC_LanguageConfig::gPostInlineWarnings = false;

SC_LanguageConfig* gLanguageConfig;

static const char* INCLUDE_PATHS = "includePaths";
static const char* EXCLUDE_PATHS = "excludePaths";
static const char* POST_INLINE_WARNINGS = "postInlineWarnings";
static const char* CLASS_LIB_DIR_NAME = "SCClassLibrary";
static const char* SCLANG_YAML_CONFIG_FILENAME = "sclang_conf.yaml";
static const char* EXCLUDE_DEFAULT_PATHS = "excludeDefaultPaths";
static const char* PROJECT = "project";

using DirName = SC_Filesystem::DirName;
namespace bfs = boost::filesystem;

//Adding default paths is done in this method instead of in the constructor
//because whether default paths are added is determined by the 'excludeDefaultPaths' key
//and whether this key is present can only be determined after reading the language config file
//which is done after the instance is created.
void SC_LanguageConfig::addDefaultPaths()
{
	if( !mExcludeDefaultPaths ) {
		const Path& classLibraryDir = SC_Filesystem::instance().getDirectory(DirName::Resource) / CLASS_LIB_DIR_NAME;
		addPath(mDefaultClassLibraryDirectories, classLibraryDir);

		const Path& systemExtensionDir = SC_Filesystem::instance().getDirectory(DirName::SystemExtension);
		addPath(mDefaultClassLibraryDirectories, systemExtensionDir);

		const Path& userExtensionDir = SC_Filesystem::instance().getDirectory(DirName::UserExtension);
		addPath(mDefaultClassLibraryDirectories, userExtensionDir);
	}
}

void SC_LanguageConfig::postExcludedDirectories(void) const
{
	for (auto it : mExcludedDirectories) {
		post("\texcluding dir: '%s'\n", it.c_str());
	}
}

bool SC_LanguageConfig::forEachIncludedDirectory(bool (*func)(const Path&)) const
{
	for (const auto& it : mDefaultClassLibraryDirectories) {
		if (!pathIsExcluded(it)) {
			if (!func(it))
				return false;
		}
	}

	for (const auto& it : mIncludedDirectories) {
		if (!pathIsExcluded(it)) {
			if (!func(it))
				return false;
		}
	}

	return true;
}

bool SC_LanguageConfig::pathIsExcluded(const Path& path) const
{
	return findPath(mExcludedDirectories, path);
}

bool SC_LanguageConfig::addIncludedDirectory(const Path& path)
{
	return addPath(mIncludedDirectories, path);
}

bool SC_LanguageConfig::addExcludedDirectory(const Path& path)
{
	return addPath(mExcludedDirectories, path);
}

bool SC_LanguageConfig::removeIncludedDirectory(const Path& path)
{
	return removePath(mIncludedDirectories, path);
}

bool SC_LanguageConfig::removeExcludedDirectory(const Path& path)
{
	return removePath(mExcludedDirectories, path);
}

void SC_LanguageConfig::setExcludeDefaultPaths(bool value)
{
	mExcludeDefaultPaths = value;
}

bool SC_LanguageConfig::getExcludeDefaultPaths() const
{
	return mExcludeDefaultPaths;
}

void SC_LanguageConfig::setProject(bool value)
{
	mProject = value;
}

bool SC_LanguageConfig::getProject() const
{
	return mProject;
}

void SC_LanguageConfig::processPathList(const char* nodeName, YAML::Node &doc, void (*func)(const Path&)) {
	const YAML::Node & items = doc[ nodeName ];
	if (items && items.IsSequence()) {
		std::string emptyString;
		for (auto const & item : items ) {
			const std::string& path = item.as<std::string>(emptyString);
			if ( !path.empty() ) {
				const Path& native_path = SC_Codecvt::utf8_str_to_path(path);
				func(native_path);
			}
		}
	}
}

void SC_LanguageConfig::processBool(const char* nodeName, YAML::Node &doc, void (*func)(bool)) {
	const YAML::Node & item = doc[ nodeName ];
	if (item) {
		try {
			func(item.as<bool>());
		} catch(...) {
			postfl("Warning: Cannot parse config file entry \"%s\"\n", nodeName);
		}
	}
}

bool SC_LanguageConfig::readLibraryConfigYAML(const Path& fileName)
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig();

	using namespace YAML;
	try {
		bfs::ifstream fin(fileName);
		Node doc = Load( fin );
		if (doc) {
			processBool(POST_INLINE_WARNINGS, doc, [](bool b){
				gPostInlineWarnings = b;
			});
			processBool(EXCLUDE_DEFAULT_PATHS, doc, [](bool b){
				gLanguageConfig->setExcludeDefaultPaths(b);
			});
			processBool(PROJECT, doc, [](bool b){
				gLanguageConfig->setProject(b);
			});
			processPathList(INCLUDE_PATHS, doc, [](const Path &p){
				gLanguageConfig->addIncludedDirectory(p);
			});
			processPathList(EXCLUDE_PATHS, doc, [](const Path &p){
				gLanguageConfig->addExcludedDirectory(p);
			});
		}
		if (!gLanguageConfig->mExcludeDefaultPaths) {
			gLanguageConfig->addDefaultPaths();
		}

		return true;
	} catch (std::exception & e) {
		postfl("Exception while parsing YAML config file: %s\n", e.what());
		freeLibraryConfig();
		return false;
	}
}

bool SC_LanguageConfig::writeLibraryConfigYAML(const Path& fileName)
{
	using namespace YAML;
	Emitter out;
	out.SetIndent(4);
	out.SetMapFormat(Block);
	out.SetSeqFormat(Block);
	out.SetBoolFormat(TrueFalseBool);

	out << BeginMap;

	out << Key << INCLUDE_PATHS;
	out << Value << BeginSeq;
	for (const bfs::path& it : gLanguageConfig->mIncludedDirectories)
		out << SC_Codecvt::path_to_utf8_str(it);
	out << EndSeq;

	out << Key << EXCLUDE_PATHS;
	out << Value << BeginSeq;
	for (const bfs::path& it : gLanguageConfig->mExcludedDirectories)
		out << SC_Codecvt::path_to_utf8_str(it);
	out << EndSeq;

	out << Key << POST_INLINE_WARNINGS;
	out << Value << gPostInlineWarnings;

	out << Key << EXCLUDE_DEFAULT_PATHS;
	out << Value << gLanguageConfig->mExcludeDefaultPaths;

	if(gLanguageConfig->mProject)
	{
		out << Key << PROJECT;
		out << Value << true;
	}

	out << EndMap;

	bfs::ofstream fout(fileName);
	fout << out.c_str();
	return true;
}

bool SC_LanguageConfig::defaultLibraryConfig()
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig();
	if (!gLanguageConfig->mExcludeDefaultPaths) {
		gLanguageConfig->addDefaultPaths();
	}
	return true;
}

bool SC_LanguageConfig::readLibraryConfig()
{
	bool configured = false;

	if (bfs::exists(gConfigFile))
		configured = readLibraryConfigYAML(gConfigFile);

	if( !configured ) {
		const Path userYamlConfigFile = SC_Filesystem::instance().getDirectory(DirName::UserConfig) / SCLANG_YAML_CONFIG_FILENAME;

		if (bfs::exists(userYamlConfigFile))
			configured = readLibraryConfigYAML(userYamlConfigFile);

		if (!configured) {
			const Path globalYamlConfigFile = Path("/etc") / SCLANG_YAML_CONFIG_FILENAME;

			if (bfs::exists(globalYamlConfigFile))
				configured = readLibraryConfigYAML(globalYamlConfigFile);
		}
	}

	if ( !configured )
		configured = SC_LanguageConfig::defaultLibraryConfig();

	return configured;
}

void SC_LanguageConfig::freeLibraryConfig()
{
	if (gLanguageConfig) {
		delete gLanguageConfig;
		gLanguageConfig = nullptr;
	}
}

inline const bool SC_LanguageConfig::findPath(const DirVector& vec, const Path& path)
{
	return std::find(vec.begin(), vec.end(), path) != vec.end();
}

inline const bool SC_LanguageConfig::addPath(DirVector& vec, const Path& path)
{
	if (!findPath(vec, path)) {
		vec.push_back(path);
		return true;
	} else {
		return false;
	}
}

inline const bool SC_LanguageConfig::removePath(DirVector& vec, const Path& path)
{
	const DirVector::iterator& end = std::remove(vec.begin(), vec.end(), path);
	const bool removed = end != vec.end();
	vec.erase(end, vec.end());
	return removed;
}
