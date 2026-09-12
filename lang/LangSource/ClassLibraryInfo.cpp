#include "ClassLibraryInfo.hpp"

ClassLibraryInfo gClassLibraryInfo {};

void ClassLibraryInfo::markCompilationInProgress() {
    assert(mCompilationState == CompilationState::NotInProgress);
    mCompilationState = CompilationState::InProgress;
}

void ClassLibraryInfo::markCompilationFailed() {
    assert(mCompilationState == CompilationState::InProgress);
    mCompilationState = CompilationState::CompilationFailed;
}

void ClassLibraryInfo::markCompilationOkay() {
    assert(mCompilationState == CompilationState::InProgress);
    mCompilationState = CompilationState::CompilationOkay;
}

void ClassLibraryInfo::markLibraryInitalised() {
    assert(mCompilationState == CompilationState::CompilationOkay);
    mCompilationState = CompilationState::ClassLibraryInitialised;
}

void ClassLibraryInfo::markShuttingDown() { mCompilationState = CompilationState::ShuttingDown; }

void ClassLibraryInfo::incrementMethodOverrideCount() { mMethodOverrideCount += 1; }

[[nodiscard]] std::size_t ClassLibraryInfo::methodOverrideCount() const { return mMethodOverrideCount; }

[[nodiscard]] bool ClassLibraryInfo::acceptsInput() const {
    return mCompilationState == CompilationState::ShuttingDown
        || mCompilationState == CompilationState::ClassLibraryInitialised;
}

void ClassLibraryInfo::reset() {
    mMethodOverrideCount = 0;
    mCompilationState = CompilationState::NotInProgress;
}
void ClassLibraryInfo::incrementNumUninlinedFunctions() { mNumUninlinedFunctions += 1; };
[[nodiscard]] std::size_t ClassLibraryInfo::numUninlinedFunctions() const { return mNumUninlinedFunctions; }
void ClassLibraryInfo::appendToOverwriteMsg(std::string str) { mOverwriteMsg += std::move(str); }
[[nodiscard]] const std::string& ClassLibraryInfo::overwriteMsg() const& { return mOverwriteMsg; }
