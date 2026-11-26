#pragma once

#include "sc_source/text_location.hpp"
#include "sc_util/language_version.hpp"

#include <cstddef>
#include <optional>
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
// The version of interpreted text with no file and no version on its line 1, from --interpreted-version. Legacy
// unless set.
void setVersionWithoutFile(sc::ast::LanguageVersion version);
// Call once the class library's objects are freed.
void unloadClassLibrary();

// Interpreted code's function, from a compiled interpreted graph of one source, whose string and path the caller made.
// Its objects are the GC's, made without collecting, so nothing else may be allocated until the result is reachable.
[[nodiscard]] PyrBlock* loadInterpreted(VMGlobals* g, const sc::compiler::CompiledGraph& compiled, PyrString* text,
                                        PyrSymbol* path, PyrBlock* context);

// Interpreted code's function, compiled against the loaded class library in Interpreter:functionCompileContext, posting
// its diagnostics. Null after an error. Without a path it is interpreted_text's. Without a version it is the one on
// the text's line 1, else Legacy, or setVersionWithoutFile's when it has no path. As loadInterpreted, nothing else may
// be allocated until the result is reachable.
[[nodiscard]] PyrBlock* compileInterpreted(VMGlobals* g, std::string_view code, PyrSymbol* path,
                                           sc::src::FileCodeLocation startInFile,
                                           std::optional<sc::ast::LanguageVersion> version);
