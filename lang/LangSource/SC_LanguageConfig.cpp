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

#include <algorithm> // std::find, std::sort
#include <functional> // std::function

#include <boost/algorithm/string.hpp> // split, replace
#include <boost/dll/runtime_symbol_info.hpp> // program_location
#include <boost/system/error_code.hpp>

SC_LanguageConfig::Path SC_LanguageConfig::gConfigFile;
bool SC_LanguageConfig::gPostInlineWarnings = false;

SC_LanguageConfig* gLanguageConfig;

const char* INCLUDE_PATHS = "includePaths";
const char* EXCLUDE_PATHS = "excludePaths";
const char* POST_INLINE_WARNINGS = "postInlineWarnings";
static const char* CLASS_LIB_DIR_NAME = "SCClassLibrary";
const char* SCLANG_YAML_CONFIG_FILENAME = "sclang_conf.yaml";
const char* EXCLUDE_DEFAULT_PATHS = "excludeDefaultPaths";

#ifdef CMAKE_SCLANG_CONF_PATH
std::string SCLANG_CONF_PATH = CMAKE_SCLANG_CONF_PATH;
#else
std::string SCLANG_CONF_PATH;
#endif

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

void SC_LanguageConfig::processPathList(const Path& confPath, const char* nodeName, YAML::Node& doc,
                                        const std::function<void(const Path&)>& func) {
    boost::system::error_code ec;
    const YAML::Node& items = doc[nodeName];
    if (items && items.IsSequence()) {
        for (auto item : items) {
            std::string pathString = item.as<std::string>("");
            boost::replace_first(pathString, "USERHOME",
                                 SC_Filesystem::instance().getDirectory(DirName::UserHome).string());
            boost::replace_first(pathString, "PROGRAM_LOCATION", boost::dll::program_location().parent_path().string());
            const Path path = bfs::canonical(SC_Codecvt::utf8_str_to_path(pathString), confPath.parent_path(), ec);
            if (!pathString.empty() && ec.value() == boost::system::errc::success) {
                func(path);
            }
        }
    }
}

void SC_LanguageConfig::processBool(const char* nodeName, std::vector<YAML::Node>& docs,
                                    const std::function<void(bool)>& successFunc,
                                    const std::function<void()>& failFunc) {
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

                processPathList(path, EXCLUDE_PATHS, doc, [](const Path& p) {
                    if (!gLanguageConfig->pathIsIncluded(p)) {
                        gLanguageConfig->addExcludedDirectory(p);
                    }
                });
                processPathList(path, INCLUDE_PATHS, doc, [](const Path& p) {
                    if (!gLanguageConfig->pathIsExcluded(p)) {
                        gLanguageConfig->addIncludedDirectory(p);
                    }
                });
            }
            fin.close();
        }

        processBool(
            POST_INLINE_WARNINGS, docs, [](bool b) { gLanguageConfig->setPostInlineWarnings(b); }, []() {});
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
    std::string envPath =
        getenv("SCLANG_CONF_PATH") == NULL ? SCLANG_CONF_PATH : std::string(getenv("SCLANG_CONF_PATH"));
    boost::replace_all(envPath, "USERHOME", SC_Filesystem::instance().getDirectory(DirName::UserHome).string());
    std::vector<std::string> splitEnvPath;
    boost::split(splitEnvPath, envPath, boost::is_any_of(":"));
    boost::system::error_code ec;
    DirDeque paths;

    for (auto envPathString : splitEnvPath) {
        std::vector<std::string> splitEnvPathString;
        boost::split(splitEnvPathString, envPathString, boost::is_any_of(">"));
        for (auto confPathString : splitEnvPathString) {
            const Path confPath =
                bfs::canonical(Path(confPathString), boost::dll::program_location().parent_path(), ec);
            if (!confPathString.empty() && ec.value() == boost::system::errc::success) {
                paths.push_back(confPath);
                break;
            }
        }
    }

    const Path userYamlConfigFile =
        SC_Filesystem::instance().getDirectory(DirName::UserConfig) / SCLANG_YAML_CONFIG_FILENAME;
    const Path globalYamlConfigFile = Path("/etc") / SCLANG_YAML_CONFIG_FILENAME;
    const Path lConfigFile = bfs::canonical(gConfigFile, ec);
    if (!gConfigFile.empty() && ec.value() == boost::system::errc::success) {
        paths.push_front(lConfigFile);
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

bool SC_LanguageConfig::compareUnorderedPathLists(DirVector v1, DirVector v2) {
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    return v1 == v2;
}

bool SC_LanguageConfig::removePath(DirVector& vec, const Path& path) {
    const DirVector::iterator& end = std::remove(vec.begin(), vec.end(), path);
    const bool removed = end != vec.end();
    vec.erase(end, vec.end());
    return removed;
}
