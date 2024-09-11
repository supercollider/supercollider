#include <boost/test/unit_test.hpp>

#include "SC_LanguageConfig.hpp"

#ifdef CMAKE_CURRENT_SOURCE_DIR
BOOST_AUTO_TEST_CASE(merge_includepaths_test) {
    const SC_LanguageConfig::Path conf1 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_includepaths_test" / "sclang_conf_1.yaml";
    const SC_LanguageConfig::Path conf2 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_includepaths_test" / "sclang_conf_2.yaml";
    const SC_LanguageConfig::Path confr = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_includepaths_test" / "result.yaml";
    SCLANG_CONF_PATH = conf1.string() + ":" + conf2.string();

    SC_LanguageConfig::readLibraryConfig(false);

    SC_LanguageConfig::DirVector result;

    boost::filesystem::ifstream fin;
    fin.open(confr);
    YAML::Node doc = YAML::Load(fin);
    SC_LanguageConfig::processPathList(confr, INCLUDE_PATHS, doc, [&result](const SC_LanguageConfig::Path& p) {
        SC_LanguageConfig::addPath(result, p);
    });

    BOOST_REQUIRE(SC_LanguageConfig::compareUnorderedPathLists(gLanguageConfig->includedDirectories(), result));
}

BOOST_AUTO_TEST_CASE(merge_excludepaths_test) {
    const SC_LanguageConfig::Path conf1 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_excludepaths_test" / "sclang_conf_1.yaml";
    const SC_LanguageConfig::Path conf2 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_excludepaths_test" / "sclang_conf_2.yaml";
    const SC_LanguageConfig::Path confr = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_excludepaths_test" / "result.yaml";
    SCLANG_CONF_PATH = conf1.string() + ":" + conf2.string();

    SC_LanguageConfig::readLibraryConfig(false);

    SC_LanguageConfig::DirVector result;

    boost::filesystem::ifstream fin;
    fin.open(confr);
    YAML::Node doc = YAML::Load(fin);
    SC_LanguageConfig::processPathList(confr, EXCLUDE_PATHS, doc, [&result](const SC_LanguageConfig::Path& p) {
        SC_LanguageConfig::addPath(result, p);
    });

    BOOST_REQUIRE(SC_LanguageConfig::compareUnorderedPathLists(gLanguageConfig->excludedDirectories(), result));
}

BOOST_AUTO_TEST_CASE(merge_excludedefaultpaths_test) {
    const SC_LanguageConfig::Path conf1 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_excludedefaultpaths_test" / "sclang_conf_1.yaml";
    const SC_LanguageConfig::Path conf2 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_excludedefaultpaths_test" / "sclang_conf_2.yaml";
    const SC_LanguageConfig::Path confr = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR)
        / "sc_languageconfig_tests/merge_excludedefaultpaths_test" / "result.yaml";
    SCLANG_CONF_PATH = conf1.string() + ":" + conf2.string();

    SC_LanguageConfig::readLibraryConfig(false);

    bool result;

    boost::filesystem::ifstream fin;
    fin.open(confr);
    std::vector<YAML::Node> doc { YAML::Load(fin) };
    SC_LanguageConfig::processBool(
        EXCLUDE_DEFAULT_PATHS, doc, [&result](bool b) { result = b; }, []() {});

    BOOST_REQUIRE(gLanguageConfig->getExcludeDefaultPaths() == result);
}

BOOST_AUTO_TEST_CASE(merge_postinlinewarnings_test) {
    const SC_LanguageConfig::Path conf1 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_postinlinewarnings_test" / "sclang_conf_1.yaml";
    const SC_LanguageConfig::Path conf2 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_postinlinewarnings_test" / "sclang_conf_2.yaml";
    const SC_LanguageConfig::Path confr = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_postinlinewarnings_test" / "result.yaml";
    SCLANG_CONF_PATH = conf1.string() + ":" + conf2.string();

    SC_LanguageConfig::readLibraryConfig(false);

    bool result;

    boost::filesystem::ifstream fin;
    fin.open(confr);
    std::vector<YAML::Node> doc { YAML::Load(fin) };
    SC_LanguageConfig::processBool(
        POST_INLINE_WARNINGS, doc, [&result](bool b) { result = b; }, []() {});

    BOOST_REQUIRE(gLanguageConfig->getPostInlineWarnings() == result);
}

BOOST_AUTO_TEST_CASE(chain_of_configuration_preference_test) {
    const SC_LanguageConfig::Path conf1 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_includepaths_test" / "sclang_conf_1.yaml";
    const SC_LanguageConfig::Path conf2 = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_includepaths_test" / "sclang_conf_2.yaml";
    const SC_LanguageConfig::Path confr = SC_LanguageConfig::Path(CMAKE_CURRENT_SOURCE_DIR) / "sc_languageconfig_tests"
        / "merge_includepaths_test" / "sclang_conf_1.yaml";
    SCLANG_CONF_PATH = conf1.string() + ">" + conf2.string();

    SC_LanguageConfig::readLibraryConfig(false);

    SC_LanguageConfig::DirVector result;

    boost::filesystem::ifstream fin;
    fin.open(confr);
    YAML::Node doc = YAML::Load(fin);
    SC_LanguageConfig::processPathList(confr, INCLUDE_PATHS, doc, [&result](const SC_LanguageConfig::Path& p) {
        SC_LanguageConfig::addPath(result, p);
    });

    BOOST_REQUIRE(SC_LanguageConfig::compareUnorderedPathLists(gLanguageConfig->includedDirectories(), result));
}
#endif
