// NOTE: these tests are written against server API version 4.1.
// SC_ServerPluginModule.cpp is modified accordingly by setting
// TEST_API_VERSION_MAJOR and TEST_API_VERSION_MINOR.
#include "SC_ServerPluginModule.hpp"
#include "SC_InterfaceTable.h"

#include <boost/test/unit_test.hpp>

// dummy InterfaceTable
static InterfaceTable interfaceTable {};

// for BOOST_TEST
std::ostream& operator<<(std::ostream& os, ServerPluginError e) { return os << static_cast<int>(e); }

BOOST_AUTO_TEST_CASE(api_v3_test) {
    // test major API version mismatch with a v3 plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = std::filesystem::absolute("./plugins/test_plugin_v3.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::MajorAPIVersionMismatch);
}

BOOST_AUTO_TEST_CASE(api_v5_0_test) {
    // test major API version mismatch with a v5 plugin
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = std::filesystem::absolute("./plugins/test_plugin_v5_0.scx");
        ServerPluginModule module(path);
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::MajorAPIVersionMismatch);
}

BOOST_AUTO_TEST_CASE(api_v4_5_test) {
    // a v4.x plugin with no min. version requirements should load successfully
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = std::filesystem::absolute("./plugins/test_plugin_v4_5.scx");
        ServerPluginModule module(path);
        module.load(&interfaceTable, path.u8string().c_str());
        module.unload();
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::NoError);
}

BOOST_AUTO_TEST_CASE(api_v4_5_min_v4_1_test) {
    // a v4.x plugin with a min. version of v4.1 should load successfully
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = std::filesystem::absolute("./plugins/test_plugin_v4_5_min_v4_1.scx");
        ServerPluginModule module(path);
        module.load(&interfaceTable, path.u8string().c_str());
        module.unload();
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::NoError);
}

BOOST_AUTO_TEST_CASE(api_v4_5_min_v4_2_test) {
    // a v4.x plugin with a min. version of v4.2 should fail to load
    ServerPluginError err = ServerPluginError::NoError;

    try {
        auto path = std::filesystem::absolute("./plugins/test_plugin_v4_5_min_v4_2.scx");
        ServerPluginModule module(path);
        module.load(&interfaceTable, path.u8string().c_str());
        module.unload();
    } catch (const ServerPluginException& e) { err = e.errorCode(); }

    BOOST_TEST(err == ServerPluginError::MinRequiredAPIVersionNotMet);
}
