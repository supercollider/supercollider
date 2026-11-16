#include "SC_InterfaceTable.h"

#undef SC_API_VERSION_MAJOR
#define SC_API_VERSION_MAJOR TEST_API_VERSION_MAJOR

#undef SC_API_VERSION_MINOR
#define SC_API_VERSION_MINOR TEST_API_VERSION_MINOR

#include <string>

static InterfaceTable* ft;
static std::string pluginPath;
static float* globalData;

PluginLoad(TestPlugin) {
    ft = inTable;
    pluginPath = inPluginPath;
    globalData = new float[100] {};
}

PluginUnload(TestPlugin) { delete[] globalData; }
