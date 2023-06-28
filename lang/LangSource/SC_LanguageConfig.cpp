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

#include <boost/algorithm/string.hpp> // split
#include <boost/filesystem/operations.hpp> // exists (, canonical?)
#include <boost/filesystem/fstream.hpp> // ofstream
#include <yaml-cpp/yaml.h> // YAML

SC_LanguageConfig::Path SC_LanguageConfig::gConfigFile;
bool SC_LanguageConfig::gPostInlineWarnings = false;

SC_LanguageConfig* gLanguageConfig;

static const char* INCLUDE_PATHS = "includePaths";
static const char* EXCLUDE_PATHS = "excludePaths";
static const char* POST_INLINE_WARNINGS = "postInlineWarnings";
static const char* CLASS_LIB_DIR_NAME = "SCClassLibrary";
const char* SCLANG_YAML_CONFIG_FILENAME = "sclang_conf.yaml";
static const char* EXCLUDE_DEFAULT_PATHS = "excludeDefaultPaths";

using DirName = SC_Filesystem::DirName;
namespace bfs = boost::filesystem;

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
        if (!func(it)) {
            return false;
        }
    }

    return true;
}

bool SC_LanguageConfig::pathIsIncluded(const Path& path) const { return findPath(mIncludedDirectories, path); }

bool SC_LanguageConfig::pathIsExcluded(const Path& path) const { return findPath(mExcludedDirectories, path); }

bool SC_LanguageConfig::addIncludedDirectory(const Path& path) { return addPath(mIncludedDirectories, path); }

bool SC_LanguageConfig::addExcludedDirectory(const Path& path) { return addPath(mExcludedDirectories, path); }

bool SC_LanguageConfig::removeIncludedDirectory(const Path& path) { return removePath(mIncludedDirectories, path); }

bool SC_LanguageConfig::removeExcludedDirectory(const Path& path) { return removePath(mExcludedDirectories, path); }

static void processPathList(const char* nodeName, YAML::Node& doc,
                            const std::function<void(const boost::filesystem::path&)>& func) {
    const YAML::Node& items = doc[nodeName];
    if (items && items.IsSequence()) {
        for (auto const& item : items) {
            const std::string& path = item.as<std::string>("");
            if (!path.empty()) {
                const boost::filesystem::path& native_path = SC_Codecvt::utf8_str_to_path(path);
                func(native_path);
            }
        }
    }
}

static void processBool(const char* nodeName, std::vector<YAML::Node>& docs,
                        const std::function<void(bool)>& successFunc, const std::function<void()>& failFunc) {
    for (auto doc : docs) {
        const YAML::Node& item = doc[nodeName];
        if (item) {
            try {
                successFunc(item.as<bool>());
                return;
            } catch (...) {
                postfl("WARNING: Cannot parse config file entry \"%s\": \n", nodeName);
            }
        }
    }
    failFunc();
}

bool SC_LanguageConfig::readLibraryConfigYAML(const DirDeque& paths, bool standalone) {
    freeLibraryConfig();
    gLanguageConfig = new SC_LanguageConfig();
    std::vector<YAML::Node> docs;

    try {
        bfs::ifstream fin;
        for (auto path : paths) {
            fin.open(path);
            YAML::Node doc = YAML::Load(fin);
            if (doc) {
                docs.push_back(doc);

                processPathList(EXCLUDE_PATHS, doc, [](const Path& p) {
                    if (!gLanguageConfig->pathIsIncluded(p)) {
                        gLanguageConfig->addExcludedDirectory(p);
                    }
                });
                processPathList(INCLUDE_PATHS, doc, [](const Path& p) {
                    if (!gLanguageConfig->pathIsExcluded(p)) {
                        gLanguageConfig->addIncludedDirectory(p);
                    }
                });
            }
            fin.close();
        }

        processBool(
            POST_INLINE_WARNINGS, docs, [](bool b) { gPostInlineWarnings = b; }, []() {});
        processBool(
            EXCLUDE_DEFAULT_PATHS, docs,
            [standalone](bool b) { gLanguageConfig->setExcludeDefaultPaths(standalone || b); },
            [standalone] { gLanguageConfig->setExcludeDefaultPaths(standalone); });

        return true;
    } catch (std::exception& e) {
        postfl("Exception while parsing YAML config file: %s\n", e.what());
        freeLibraryConfig();
        return false;
    }
}

bool SC_LanguageConfig::writeLibraryConfigYAML(const Path& fileName) {
    if (!bfs::exists(fileName.parent_path()))
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

    out << EndMap;

    bfs::ofstream fout(fileName);
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
    std::string envConf =
        getenv("SCLANG_CONF_PATH") == NULL ? std::string("") : std::string(getenv("SCLANG_CONF_PATH"));
    std::vector<std::string> splitConf;
    boost::split(splitConf, envConf, boost::is_any_of(":"));
    DirDeque paths;

    for (auto path : splitConf) {
        if (bfs::exists(Path(path))) {
            paths.push_back(Path(path));
        }
    }

    const Path userYamlConfigFile =
        SC_Filesystem::instance().getDirectory(DirName::UserConfig) / SCLANG_YAML_CONFIG_FILENAME;
    const Path globalYamlConfigFile = Path("/etc") / SCLANG_YAML_CONFIG_FILENAME;
    if (bfs::exists(gConfigFile)) {
        paths.push_front(gConfigFile);
    } else if (!standalone && bfs::exists(userYamlConfigFile)) {
        paths.push_back(userYamlConfigFile);
    } else if (!standalone && bfs::exists(globalYamlConfigFile)) {
        paths.push_back(globalYamlConfigFile);
    } else if (paths.empty()) {
        return SC_LanguageConfig::defaultLibraryConfig(standalone);
    }

    return readLibraryConfigYAML(paths, standalone);
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
