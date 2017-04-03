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

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

static const char* INCLUDE_PATHS = "includePaths";
static const char* EXCLUDE_PATHS = "excludePaths";
static const char* POST_INLINE_WARNINGS = "postInlineWarnings";
static const char* CLASS_LIB_DIR_NAME = "SCClassLibrary";
static const char* SCLANG_YAML_CONFIG_FILENAME = "sclang_conf.yaml";

SC_LanguageConfig *gLanguageConfig = 0;

SC_LanguageConfig::SC_LanguageConfig(bool optStandalone)
{
	if( !optStandalone ) {
		const Path& classLibraryDir = SC_DirUtils::getResourceDirectory() / CLASS_LIB_DIR_NAME;
		if (!findPath(mDefaultClassLibraryDirectories, classLibraryDir))
			addPath(mDefaultClassLibraryDirectories, classLibraryDir);
	}

	if ( !( sc_IsStandAlone() || optStandalone ) ) {
		const Path& systemExtensionDir = sc_GetSystemExtensionDirectory();
		if (!findPath(mDefaultClassLibraryDirectories, systemExtensionDir))
			addPath(mDefaultClassLibraryDirectories, classLibraryDir);

		const Path& userExtensionDir = sc_GetUserExtensionDirectory();
		if (!findPath(mDefaultClassLibraryDirectories, userExtensionDir))
			addPath(mDefaultClassLibraryDirectories, classLibraryDir);
	}
}

inline const void SC_LanguageConfig::setPostInlineWarnings(bool b)
{
	gPostInlineWarnings = b;
}

inline void SC_LanguageConfig::postExcludedDirectories(void) const
{
	for (auto it : mExcludedDirectories) {
		post("\texcluding dir: '%s'\n", it.c_str());
	}
}

bool SC_LanguageConfig::forEachIncludedDirectory(bool (*func)(const Path&, int))
{
	for (const auto& it : mDefaultClassLibraryDirectories) {
		if (!pathIsExcluded(it)) {
			if (!func(it, 0))
				return false;
		}
	}

	for (const auto& it : mIncludedDirectories) {
		if (!pathIsExcluded(it)) {
			if (!func(it, 0))
				return false;
		}
	}

	return true;
}

inline bool SC_LanguageConfig::pathIsExcluded(const Path& path) const
{
	return findPath(mExcludedDirectories, path);
}

inline bool SC_LanguageConfig::addIncludedDirectory(const Path& path)
{
	return addPath(mIncludedDirectories, path);
}

inline bool SC_LanguageConfig::addExcludedDirectory(const Path& path)
{
	return addPath(mExcludedDirectories, path);
}

inline bool SC_LanguageConfig::removeIncludedDirectory(const Path& path)
{
	return removePath(mIncludedDirectories, path);
}

inline bool SC_LanguageConfig::removeExcludedDirectory(const Path& path)
{
	return removePath(mExcludedDirectories, path);
}

bool SC_LanguageConfig::readLibraryConfigYAML(const Path& fileName, bool standalone)
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig(standalone);

	std::string emptyString;

	using namespace YAML;
	try {
		boost::filesystem::ifstream fin(fileName);
		Node doc = Load( fin );
		if (doc) {
			const Node & includePaths = doc[ INCLUDE_PATHS ];
			if (includePaths && includePaths.IsSequence()) {
				for (auto const & pathNode : includePaths ) {
					std::string path = pathNode.as<std::string>( emptyString );
					if ( !path.empty() )
						gLanguageConfig->addIncludedDirectory( path.c_str() );
				}
			}

			const Node & excludePaths = doc[ EXCLUDE_PATHS ];
			if (excludePaths && excludePaths.IsSequence()) {
				for (auto const & pathNode : excludePaths ) {
					std::string path = pathNode.as<std::string>( emptyString );
					if ( !path.empty() )
						gLanguageConfig->addExcludedDirectory( path.c_str() );
				}
			}

			const Node & inlineWarnings = doc[ POST_INLINE_WARNINGS ];
			if (inlineWarnings) {
				try {
					gPostInlineWarnings = inlineWarnings.as<bool>();
				} catch(...) {
					postfl("WARNING: Cannot parse config file entry \"%s\"\n", POST_INLINE_WARNINGS);
				}
			}
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
	for (const auto& it : gLanguageConfig->mIncludedDirectories)
		out << it.c_str();
	out << EndSeq;

	out << Key << EXCLUDE_PATHS;
	out << Value << BeginSeq;
	for (const auto& it : gLanguageConfig->mExcludedDirectories)
		out << it.c_str();
	out << EndSeq;

	out << Key << POST_INLINE_WARNINGS;
	out << Value << gPostInlineWarnings;

	out << EndMap;

	boost::filesystem::ofstream fout(fileName);
	fout << out.c_str();
	return true;
}

bool SC_LanguageConfig::defaultLibraryConfig(bool standalone)
{
	freeLibraryConfig();
	gLanguageConfig = new SC_LanguageConfig(standalone);

	return true;
}

bool SC_LanguageConfig::readLibraryConfig(bool standalone)
{
	bool configured = false;

	if (boost::filesystem::exists(gConfigFile))
		configured = readLibraryConfigYAML(gConfigFile, standalone);

	if( !configured && !standalone ) {
		const Path userYamlConfigFile = SC_DirUtils::getUserConfigDirectory() / SCLANG_YAML_CONFIG_FILENAME;

		if (boost::filesystem::exists(userYamlConfigFile))
			configured = readLibraryConfigYAML(userYamlConfigFile, standalone);

		if (!configured) {
			const Path globalYamlConfigFile = Path("/etc") / SCLANG_YAML_CONFIG_FILENAME;

			if (boost::filesystem::exists(globalYamlConfigFile))
				configured = readLibraryConfigYAML(globalYamlConfigFile, standalone);
		}
	}

	if ( !configured )
		configured = SC_LanguageConfig::defaultLibraryConfig(standalone);

	return configured;
}

void SC_LanguageConfig::freeLibraryConfig()
{
	if (gLanguageConfig) {
		delete gLanguageConfig;
		gLanguageConfig = 0;
	}
}

inline bool SC_LanguageConfig::findPath(const DirVector& vec, const Path& path)
{
	const Path standardPath = boost::filesystem::canonical(path);

	for ( const auto& it : vec )
		if (standardPath == it)
			return true;

	return false;
}

inline bool SC_LanguageConfig::addPath(DirVector& vec, const Path& path)
{
	if (!findPath(vec, path)) {
		vec.push_back(path);
		return true;
	} else {
		return false;
	}
}

inline bool SC_LanguageConfig::removePath(DirVector& vec, const Path& path)
{
	SC_LanguageConfig::Path stdPath = sc_StandardizePath(path);
	SC_LanguageConfig::DirVector::iterator end = std::remove(vec.begin(), vec.end(), path);
	bool removed = end != vec.end();
	vec.erase(end, vec.end());
	return removed;
}
