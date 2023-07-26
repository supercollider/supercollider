/*
 *  Copyright 2003 Maurizio Umberto Puxeddu <umbpux@tin.it>
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

#pragma once

#include <deque>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

class SC_LanguageConfig;
extern SC_LanguageConfig* gLanguageConfig;

extern const char* INCLUDE_PATHS;
extern const char* EXCLUDE_PATHS;
extern const char* POST_INLINE_WARNINGS;
extern const char* SCLANG_YAML_CONFIG_FILENAME;
extern const char* EXCLUDE_DEFAULT_PATHS;
extern std::string SCLANG_CONF_PATH;

/**
 * \brief Language configuration settings.
 *
 * \c sclang uses a global instance of this class to manage knowledge of
 * compilation paths.
 *
 * \c SC_LanguageConfig also provides services for the global language
 * configuration file and the option for warning on function inlining.
 */
class SC_LanguageConfig {
public:
    typedef boost::filesystem::path Path;
    typedef std::vector<Path> DirVector;
    typedef std::deque<Path> DirDeque;

    const DirVector& includedDirectories() const { return mIncludedDirectories; }
    const DirVector& excludedDirectories() const { return mExcludedDirectories; }
    const DirVector& defaultClassLibraryDirectories() const { return mDefaultClassLibraryDirectories; }

    void postExcludedDirectories(void) const;

    bool pathIsIncluded(const Path&) const; // true if the path is in mIncludedDirectories
    bool pathIsExcluded(const Path&) const; // true if the path is in mExcludedDirectories

    bool addIncludedDirectory(const Path&); // false if the path was already in the vector
    bool addExcludedDirectory(const Path&);

    bool removeIncludedDirectory(const Path&); // false if there was nothing to remove
    bool removeExcludedDirectory(const Path&);

    bool forEachIncludedDirectory(bool (*)(const Path&)) const;

    bool getExcludeDefaultPaths() const { return mExcludeDefaultPaths; }
    void setExcludeDefaultPaths(bool value);

    static bool readLibraryConfigYAML(const DirDeque&, bool standalone);
    static bool writeLibraryConfigYAML(const Path&);
    static void freeLibraryConfig();
    static bool defaultLibraryConfig(bool standalone);
    static bool readLibraryConfig(bool standalone);

    static bool getPostInlineWarnings() { return gPostInlineWarnings; }
    static void setPostInlineWarnings(bool b) { gPostInlineWarnings = b; }
    static const Path& getConfigPath() { return gConfigFile; }
    static void setConfigPath(const Path& p) { gConfigFile = p; }

    static bool addPath(DirVector&, const Path&);
    static bool compareUnorderedPathLists(DirVector, DirVector);
    static void processPathList(const char* nodeName, YAML::Node& doc, const std::function<void(const Path&)>& func);
    static void processBool(const char* nodeName, std::vector<YAML::Node>& docs,
                            const std::function<void(bool)>& successFunc, const std::function<void()>& failFunc);

private:
    static bool findPath(const DirVector&, const Path&);
    static bool removePath(DirVector&, const Path&);

    DirVector mIncludedDirectories;
    DirVector mExcludedDirectories;
    DirVector mDefaultClassLibraryDirectories;
    bool mExcludeDefaultPaths = true;
    static Path gConfigFile;
    static bool gPostInlineWarnings;
};
