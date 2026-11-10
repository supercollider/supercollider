#include "SC_ServerPluginModule.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(shared_lib_test) {
    // check that we can load a plugin that links against a shared lib in the same folder.
    // NOTE: the plugin lives in a different folder than our test executable!
    auto filePath = std::filesystem::absolute("./plugins/shared_lib_test_plugin.scx");
    ServerPluginModule module(filePath);
}
