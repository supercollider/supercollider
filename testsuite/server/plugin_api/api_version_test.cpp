// NOTE: these tests are written against server API version 4.1.
// SC_ServerPluginModule.cpp is modified accordingly by setting
// TEST_API_VERSION_MAJOR and TEST_API_VERSION_MINOR.
#include "SC_ServerPluginModule.hpp"
#include "SC_InterfaceTable.h"

#include <boost/test/unit_test.hpp>

namespace fs = std::filesystem;

// dummy InterfaceTable
static InterfaceTable interfaceTable {};

// for BOOST_TEST
std::ostream& operator<<(std::ostream& os, ServerPluginError e) { return os << static_cast<int>(e); }

BOOST_AUTO_TEST_CASE(file_does_not_exist_test) {
    // test non-existing file
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/does_not_exist.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::SystemError);
}

BOOST_AUTO_TEST_CASE(api_v3_test) {
    // test major API version mismatch with a v3 plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v3.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::MajorAPIVersionMismatch);
}

BOOST_AUTO_TEST_CASE(api_v3_supernova_test) {
    // test server type mismatch with a v3 supernova plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v3_supernova.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::WrongServerType);
}

BOOST_AUTO_TEST_CASE(api_v5_0_test) {
    // test major API version mismatch with a v5 plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v5_0.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::MajorAPIVersionMismatch);
}

BOOST_AUTO_TEST_CASE(api_v5_0_supernova_test) {
    // test server type mismatch with a v5 supernova plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v5_0_supernova.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::WrongServerType);
}

BOOST_AUTO_TEST_CASE(api_v4_5_test) {
    // a v4.x plugin with no min. version requirements should load successfully
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v4_5.scx");
        ServerPluginModule module(path);
        module.load(&interfaceTable);
        module.unload();
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::NoError);
}

BOOST_AUTO_TEST_CASE(api_v4_5_test_supernova) {
    // test server type mismatch with a v4_5 supernova plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v4_5_supernova.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::WrongServerType);
}

BOOST_AUTO_TEST_CASE(api_v4_5_min_v4_1_test) {
    // a v4.x plugin with a min. version of v4.1 should load successfully
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v4_5_min_v4_1.scx");
        ServerPluginModule module(path);
        module.load(&interfaceTable);
        module.unload();
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::NoError);
}

BOOST_AUTO_TEST_CASE(api_v4_5_min_v4_2_test) {
    // a v4.x plugin with a min. version of v4.2 should fail to load
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = fs::absolute("./plugins/test_plugin_v4_5_min_v4_2.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::MinRequiredAPIVersionNotMet);
}

BOOST_AUTO_TEST_CASE(api_v4_5_info_test) {
    auto path = fs::absolute("./plugins/test_plugin_v4_5.scx");
    ServerPluginModule module(path);

    BOOST_TEST(module.name() == "TestPlugin");

    BOOST_TEST(module.path() == path);

    // by default, the resource dir should be the same as the plugin binary dir
    BOOST_TEST(fs::equivalent(module.resourceDir(), path.parent_path()));
}

BOOST_AUTO_TEST_CASE(api_v4_5_rel_path_test) {
    auto path = fs::absolute("./plugins/test_plugin_v4_5_rel_path.scx");
    ServerPluginModule module(path);

    BOOST_TEST(module.resourceDir().lexically_normal() == (path.parent_path() / "data").lexically_normal());
}

BOOST_AUTO_TEST_CASE(api_v4_5_abs_path_test) {
    auto path = fs::absolute("./plugins/test_plugin_v4_5_abs_path.scx");
    ServerPluginModule module(path);

    BOOST_TEST(module.resourceDir().lexically_normal() == (path.parent_path() / "data").lexically_normal());
}
