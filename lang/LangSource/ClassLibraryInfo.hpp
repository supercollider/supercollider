#pragma once

#include "AllocPools.h"
#include "SC_AllocPool.h"
#include <cassert>
#include <cstddef>
#include <string>

// This class contains information about the class library that is mutated while compiling.
// This should be thread safe in the future.
struct ClassLibraryInfo {
public:
    template <typename T> [[nodiscard]] void* allocRuntimePool() const { return pyr_pool_runtime->Alloc(sizeof(T)); }
    template <typename T> [[nodiscard]] void* comptimePool() const { return pyr_pool_compile->Alloc(sizeof(T)); }
    void freeRuntimePool() const { pyr_pool_runtime->FreeAll(); }
    void freeInternalRuntimePool() const { pyr_pool_runtime->FreeAllInternal(); }
    void freeComptimePool() const { pyr_pool_compile->FreeAll(); }

    void markCompilationInProgress();
    void markCompilationFailed();
    void markCompilationOkay();
    void markLibraryInitalised();
    void markShuttingDown();

    void incrementMethodOverrideCount();
    [[nodiscard]] std::size_t methodOverrideCount() const;
    void incrementNumUninlinedFunctions();
    [[nodiscard]] std::size_t numUninlinedFunctions() const;

    void appendToOverwriteMsg(std::string str);
    [[nodiscard]] const std::string& overwriteMsg() const&;

    // Returns true if the library can be run, or if it is currently in the process of being shutdown.
    [[nodiscard]] bool acceptsInput() const;

    void reset();

    bool generateTailCalls { true };

private:
    enum struct CompilationState {
        NotInProgress,
        InProgress,
        CompilationFailed,
        CompilationOkay,
        ClassLibraryInitialised, // TODO: is it possible to know if this fails?
        ShuttingDown
    };
    std::size_t mMethodOverrideCount {};
    std::size_t mNumUninlinedFunctions {};
    std::string mOverwriteMsg;
    CompilationState mCompilationState { CompilationState::NotInProgress };
};


extern ClassLibraryInfo gClassLibraryInfo;
