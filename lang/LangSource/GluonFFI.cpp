#include <cstring>
#include <thread>
#include <chrono>
#include <filesystem>

#include "GluonFFI.hpp"
#include "VMGlobals.h"
#include "GC.h"
#include "PyrObject.h"
#include "PyrKernel.h"
#include "PyrSignal.h"
#include "PyrSched.h"
#include "PyrInterpreter.h"
#include "PyrSlot.h"
#include "PyrSymbolTable.h"


#ifdef _WIN32
#    include "SC_Win32Utils.h"
#    include "SC_Codecvt.hpp"
#else
#    include <dlfcn.h>
#endif

using namespace sc_gluon;


void* sc_gluon::details::LibraryV1::prepare_for_unload_return_handle() const {
    if (unloader)
        unloader(library_data);
    return library_handle;
}

struct LibraryEvaluatorVisitor {
    PyrSymbol* function_name;
    struct VMGlobals* g;
    PyrObject* maybe_callback;
    PyrSlot *return_slot, *first_argument_slot;
    int num_args_given;

    template <typename T> void operator()(const T& t) const {
        t.evaluate(function_name, g, maybe_callback, return_slot, first_argument_slot, num_args_given);
    }
};

struct LibraryUnloaderVisitor {
    template <typename T> void* operator()(const T& t) const { return t.prepare_for_unload_return_handle(); }
};

void GluonManager::evaluate_function(LibraryID library_id, PyrSymbol* function_name, VMGlobals* g,
                                     PyrObject* maybe_callback, PyrSlot* return_slot, PyrSlot* first_argument_slot,
                                     int num_args_given) const {
    const auto& lib = libraries.at(library_id);
    std::visit(
        LibraryEvaluatorVisitor { function_name, g, maybe_callback, return_slot, first_argument_slot, num_args_given },
        lib);
}

void sc_gluon::GluonManager::unregister_library_maybe_close(LibraryID library_id, bool close) noexcept(false) {
    const auto& lib = libraries.at(library_id);
    void* library_handle = std::visit(LibraryUnloaderVisitor {}, lib);

    if (library_handle) {
#ifndef _WIN32
        const auto r = dlclose(library_handle);
        const char* error = dlerror(); // discard error
        if (close)
            libraries.erase(library_id);
        if (r != 0)
            throw std::runtime_error { std::string { error } };
#else
        FreeLibrary((HMODULE)library_handle);
        if (close)
            libraries.erase(library_id);
#endif
    }

    if (close)
        libraries.erase(library_id);
}

LibraryID sc_gluon::GluonManager::get_inbuilt_library_id(PyrSymbol* name) const noexcept(false) {
    return inbuilt_test_to_library_id.at(name);
}

void sc_gluon::GluonManager::reset_or_prep_for_close() {
    for (const auto& [key, value] : libraries) {
        try {
            unregister_library_maybe_close(key, false);
        } catch (...) {}
    }
    libraries = std::unordered_map<LibraryID, LibraryVariant> {};
    inbuilt_test_to_library_id = std::unordered_map<PyrSymbol*, LibraryID> {};
    library_counter = std::numeric_limits<LibraryID>::min();
}
void sc_gluon::GluonManager::create_testing_library() { create_testing_library_v1(); }

LibraryID sc_gluon::GluonManager::register_library(const char* path, PyrSlot* first_parameter_slot,
                                                   int num_parameters) noexcept(false) {
#ifndef _WIN32
    void* library_handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (library_handle == nullptr) {
        throw std::runtime_error { "Could not load library" };
    }

    const auto close_library = [&]() { dlclose(library_handle); };

    void* version_f = dlsym(library_handle, "sc_gluon_version");
    const char* error = dlerror();
    if (error != nullptr || version_f == nullptr) {
        close_library();
        throw std::runtime_error { std::string { error } };
    }

#else
    const std::string path_str { path };
    const std::filesystem::path filename { path_str };

    HINSTANCE library_handle = LoadLibraryW(filename.wstring().c_str());
    if (!library_handle) {
        throw std::runtime_error { "Could not load library" };
    }

    const auto close_library = [&]() { FreeLibrary(library_handle); };

    void* version_f = (void*)GetProcAddress(library_handle, "sc_gluon_version");
    if (version_f == nullptr) {
        close_library();
        throw std::runtime_error { "Could not load sc_gluon_version from library" };
    }

#endif

    const sc_gluon_version_f version_func = (sc_gluon_version_f)version_f;
    const uint32_t version_v = version_func();

    if (version_v == 1) {
        return register_library_v1(library_handle, first_parameter_slot, num_parameters);
    } else {
        close_library();
        throw std::runtime_error { "Received unexpected version number for library." };
    }
}
