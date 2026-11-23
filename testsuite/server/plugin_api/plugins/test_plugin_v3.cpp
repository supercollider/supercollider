#include "SC_InterfaceTable.h"

C_LINKAGE SC_API_EXPORT int api_version(void) { return 3; }

C_LINKAGE SC_API_EXPORT int server_type(void) {
#ifdef SUPERNOVA
    return sc_server_supernova;
#else
    return sc_server_scsynth;
#endif
}

C_LINKAGE SC_API_EXPORT void load(InterfaceTable* inTable) {}
