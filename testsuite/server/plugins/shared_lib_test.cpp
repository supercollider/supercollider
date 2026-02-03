#include "SC_PlugIn.h"

static InterfaceTable* ft;

// this function is defined in the accompanying shared library
#ifdef _WIN32
__declspec(dllimport)
#endif
    void printGreeting(const char* who);

PluginLoad(ML_UGens) {
    ft = inTable;

#ifdef SUPERNOVA
    printGreeting("Supernova");
#else
    printGreeting("scsynth");
#endif
}
