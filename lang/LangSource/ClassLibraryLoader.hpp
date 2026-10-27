#pragma once

#include "sc_source/text_location.hpp"

#include <cstddef>
#include <string_view>

struct PyrBlock;
struct PyrString;
struct PyrSymbol;
struct VMGlobals;

namespace sc::compiler {
struct ClassLibrary;
struct CompiledGraph;
}

// Also posts, will still need finalising. Keeps the library's compiler for interpreted code, until unloadClassLibrary.
[[nodiscard]] bool loadClassLibrary(std::size_t& thingsPosted);

// Asserts the runtime's finalisation agrees with what was loaded, call after buildBigMethodMatrix.
void checkFinalisedClassLibrary();

// Null unless loadClassLibrary succeeded.
[[nodiscard]] sc::compiler::ClassLibrary* loadedClassLibrary();
// Call once the class library's objects are freed.
void unloadClassLibrary();

// Interpreted code's function, from a compiled interpreted graph of one source, whose string and path the caller made.
// Its objects are the GC's, made without collecting, so nothing else may be allocated until the result is reachable.
[[nodiscard]] PyrBlock* loadInterpreted(VMGlobals* g, const sc::compiler::CompiledGraph& compiled, PyrString* text,
                                        PyrSymbol* path, PyrBlock* context);

// Interpreted code's function, compiled against the loaded class library in Interpreter:functionCompileContext, posting
// its diagnostics. Null after an error. Without a path it is interpreted_text's. As loadInterpreted, nothing else may
// be allocated until the result is reachable.
[[nodiscard]] PyrBlock* compileInterpreted(VMGlobals* g, std::string_view code, PyrSymbol* path,
                                           sc::src::FileCodeLocation startInFile);
