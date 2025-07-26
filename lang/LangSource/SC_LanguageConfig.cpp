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
#include "SC_Filesystem.hpp" // getDirectory

#include "SCBase.h" // postfl

#include <algorithm> // std::find
#include <functional> // std::function

#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h> // YAML

SC_LanguageConfig::Path SC_LanguageConfig::gConfigFile;
bool SC_LanguageConfig::gPostInlineWarnings = false;

SC_LanguageConfig* gLanguageConfig;

static const std::string INCLUDE_PATHS = "includePaths";
static const std::string EXCLUDE_PATHS = "excludePaths";
static const std::string POST_INLINE_WARNINGS = "postInlineWarnings";
static const std::string CLASS_LIB_DIR_NAME = "SCClassLibrary";
const std::string SCLANG_YAML_CONFIG_FILENAME = "sclang_conf.yaml";
static std::string EXCLUDE_DEFAULT_PATHS = "excludeDefaultPaths";

using DirName = SC_Filesystem::DirName;
namespace fs = std::filesystem;

void SC_LanguageConfig::setExcludeDefaultPaths(bool value) {
    if (mExcludeDefaultPaths != value) {
        if (value) {
            mDefaultClassLibraryDirectories.clear();
        } else {
            const Path& classLibraryDir =
                SC_Filesystem::instance().getDirectory(DirName::Resource) / CLASS_LIB_DIR_NAME;
            addPath(mDefaultClassLibraryDirectories, classLibraryDir);

            const Path& systemExtensionDir = SC_Filesystem::instance().getDirectory(DirName::SystemExtension);
            addPath(mDefaultClassLibraryDirectories, systemExtensionDir);

            const Path& userExtensionDir = SC_Filesystem::instance().getDirectory(DirName::UserExtension);
            addPath(mDefaultClassLibraryDirectories, userExtensionDir);
        }
    }

    mExcludeDefaultPaths = value;
}


void SC_LanguageConfig::postExcludedDirectories(void) const {
    for (auto it : mExcludedDirectories) {
        post("\texcluding dir: '%s'\n", it.c_str());
    }
}

bool SC_LanguageConfig::forEachIncludedDirectory(bool (*func)(const Path&)) const {
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

bool SC_LanguageConfig::pathIsExcluded(const Path& path) const { return findPath(mExcludedDirectories, path); }

bool SC_LanguageConfig::addIncludedDirectory(const Path& path) { return addPath(mIncludedDirectories, path); }

bool SC_LanguageConfig::addExcludedDirectory(const Path& path) { return addPath(mExcludedDirectories, path); }

bool SC_LanguageConfig::removeIncludedDirectory(const Path& path) { return removePath(mIncludedDirectories, path); }

bool SC_LanguageConfig::removeExcludedDirectory(const Path& path) { return removePath(mExcludedDirectories, path); }

static void processPathList(const std::string& nodeName, YAML::Node& doc,
                            const std::function<void(const std::filesystem::path&)>& func) {
    const YAML::Node& items = doc[nodeName];
    if (items && items.IsSequence()) {
        for (auto const& item : items) {
            const std::string& path = item.as<std::string>("");
            if (!path.empty()) {
                const std::filesystem::path& native_path = SC_Codecvt::utf8_str_to_path(path);
                func(native_path);
            }
        }
    }
}

static void processBool(const std::string& nodeName, YAML::Node& doc, const std::function<void(bool)>& successFunc,
                        const std::function<void()>& failFunc) {
    const YAML::Node& item = doc[nodeName];
    if (item) {
        try {
            successFunc(item.as<bool>());
        } catch (...) {
            postfl("WARNING: Cannot parse config file entry \"%s\"\n", nodeName.c_str());
            failFunc();
        }
    } else {
        failFunc(); // call failFunc if item is not found
    }
}

bool SC_LanguageConfig::readLibraryConfigYAML(const Path& fileName, bool standalone) {
    freeLibraryConfig();
    gLanguageConfig = new SC_LanguageConfig();

    using namespace YAML;
    try {
        std::ifstream fin(fileName);
        Node doc = Load(fin);
        if (doc) {
            processBool(
                POST_INLINE_WARNINGS, doc, [](bool b) { gPostInlineWarnings = b; }, []() {});
            processBool(
                EXCLUDE_DEFAULT_PATHS, doc,
                [standalone](bool b) { gLanguageConfig->setExcludeDefaultPaths(standalone || b); },
                [standalone] { gLanguageConfig->setExcludeDefaultPaths(standalone); });

            processPathList(INCLUDE_PATHS, doc, [](const Path& p) { gLanguageConfig->addIncludedDirectory(p); });
            processPathList(EXCLUDE_PATHS, doc, [](const Path& p) { gLanguageConfig->addExcludedDirectory(p); });
        }
        return true;
    } catch (std::exception& e) {
        postfl("Exception while parsing YAML config file: %s\n", e.what());
        freeLibraryConfig();
        return false;
    }
}

bool SC_LanguageConfig::writeLibraryConfigYAML(const Path& fileName) {
    if (!fs::exists(fileName.parent_path()))
        return false;

    using namespace YAML;
    Emitter out;
    out.SetIndent(4);
    out.SetMapFormat(Block);
    out.SetSeqFormat(Block);
    out.SetBoolFormat(TrueFalseBool);

    out << BeginMap;

    out << Key << INCLUDE_PATHS;
    out << Value << BeginSeq;
    for (const fs::path& it : gLanguageConfig->mIncludedDirectories)
        out << SC_Codecvt::path_to_utf8_str(it);
    out << EndSeq;

    out << Key << EXCLUDE_PATHS;
    out << Value << BeginSeq;
    for (const fs::path& it : gLanguageConfig->mExcludedDirectories)
        out << SC_Codecvt::path_to_utf8_str(it);
    out << EndSeq;

    out << Key << POST_INLINE_WARNINGS;
    out << Value << gPostInlineWarnings;

    out << Key << EXCLUDE_DEFAULT_PATHS;
    out << Value << gLanguageConfig->mExcludeDefaultPaths;

    out << EndMap;

    std::ofstream fout(fileName);
    fout << out.c_str();
    return fout.good();
}

bool SC_LanguageConfig::defaultLibraryConfig(bool standalone) {
    freeLibraryConfig();
    gLanguageConfig = new SC_LanguageConfig();
    gLanguageConfig->setExcludeDefaultPaths(standalone);
    return true;
}

bool SC_LanguageConfig::readLibraryConfig(bool standalone) {
    bool configured = false;

    if (fs::exists(gConfigFile))
        configured = readLibraryConfigYAML(gConfigFile, standalone);

    if (!configured && !standalone) {
        const Path userYamlConfigFile =
            SC_Filesystem::instance().getDirectory(DirName::UserConfig) / SCLANG_YAML_CONFIG_FILENAME;

        if (fs::exists(userYamlConfigFile))
            configured = readLibraryConfigYAML(userYamlConfigFile, standalone);

        if (!configured) {
            const Path globalYamlConfigFile = Path("/etc") / SCLANG_YAML_CONFIG_FILENAME;

            if (fs::exists(globalYamlConfigFile))
                configured = readLibraryConfigYAML(globalYamlConfigFile, standalone);
        }
    }

    if (!configured)
        configured = SC_LanguageConfig::defaultLibraryConfig(standalone);

    return configured;
}

void SC_LanguageConfig::freeLibraryConfig() {
    if (gLanguageConfig) {
        delete gLanguageConfig;
        gLanguageConfig = nullptr;
    }
}

bool SC_LanguageConfig::findPath(const DirVector& vec, const Path& path) {
    return std::find(vec.begin(), vec.end(), path) != vec.end();
}

bool SC_LanguageConfig::addPath(DirVector& vec, const Path& path) {
    if (!findPath(vec, path)) {
        vec.push_back(path);
        return true;
    } else {
        return false;
    }
}

bool SC_LanguageConfig::removePath(DirVector& vec, const Path& path) {
    const DirVector::iterator& end = std::remove(vec.begin(), vec.end(), path);
    const bool removed = end != vec.end();
    vec.erase(end, vec.end());
    return removed;
}
