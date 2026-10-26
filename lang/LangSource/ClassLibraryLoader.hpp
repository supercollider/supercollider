#pragma once

#include <cstddef>

// SCLANG_NEW_COMPILER=1 toggles new compiler
[[nodiscard]] bool newCompilerRequested();

// Also posts, will still need finalising.
[[nodiscard]] bool loadClassLibrary(std::size_t& thingsPosted);

// Asserts the runtime's finalisation agrees with what was loaded, call after buildBigMethodMatrix.
void checkFinalisedClassLibrary();
