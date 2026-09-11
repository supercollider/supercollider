#pragma once

#include "AdvancingAllocPool.h"
#include "PyrSymbol.h"
#include "SC_Version.hpp"
#include "SimpleStack.h"
#include "normalise_source.hpp"
#include "text_location.hpp"
#include "ByteCodeArray.h"
#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <tuple>
#include <vector>


// Declare that this exists.
int yyparse();

enum struct ErrorType { Error, Warning };

struct DiagnosticHighlight {
    const char* filepath;
    const char* txt;
    size_t txt_len;
    sc::lex::SourceCodeRange range_in_source;
    size_t source_loc_lin, source_loc_column;
    std::string description;
};

std::stringstream& streamSourceCodeWithHighlight(std::stringstream& ss, const DiagnosticHighlight& highlight,
                                                 bool printSource = true, const char* newlinePrefixPtr = "",
                                                 size_t newlinePrefixSz = 0);

std::string diagnosticToCompilerError(ErrorType type, const char* generalDescription,
                                      const DiagnosticHighlight* linesToPrint, size_t numLinesToPrint);


// Used to represent a bit fo text to compile, this can be a classlibrary file, a runtime file or a snippet thereof.
struct TextInfo {
    TextInfo(sc::lex::NormalisedSource normalisedSource, struct PyrString& normalisedSourceAsPyr,
             PyrSymbol& filePathDescription, sc::lex::FileCodeLocation offsetInFile = {}, bool isClassFile = false):
        normalisedSource(std::move(normalisedSource)),
        normalisedSourceAsPyr(normalisedSourceAsPyr),
        filePathDescription(filePathDescription),
        offsetInFile(offsetInFile),
        isClassFile(isClassFile) {}
    TextInfo(TextInfo&&) noexcept = default;
    TextInfo(const TextInfo&) = default;

    sc::lex::NormalisedSource normalisedSource;
    struct PyrString& normalisedSourceAsPyr;
    // Might not be a valid file path (e.g. when file is unsaved).
    PyrSymbol& filePathDescription;
    // Used to describe when a code snippet (CMD Mode) is in the middle of a file.
    // Zero init when in class library.
    sc::lex::FileCodeLocation offsetInFile {};
    bool isClassFile { false };
    [[nodiscard]] std::tuple<const char*, std::size_t> indexIntoSource(sc::lex::SourceCodeRange range) const;
    [[nodiscard]] DiagnosticHighlight createDiagnosticHighlight(sc::lex::SourceCodeRange range,
                                                                std::string description) const;
};

// Per file
struct CompilerContext {
    // from a location until the end of the text
    CompilerContext(std::shared_ptr<TextInfo> textInfo, sc::lex::FileCodeLocation fileLoc,
                    sc::lex::SourceCodeLocation loc = {}, struct VMGlobals* vm = nullptr);
    // a range within a text
    CompilerContext(std::shared_ptr<TextInfo> textInfo, sc::lex::FileCodeLocation fileLoc,
                    sc::lex::SourceCodeRange range, struct VMGlobals* vm = nullptr);

    // the whole text
    CompilerContext(std::shared_ptr<TextInfo> textInfo, struct VMGlobals* vm = nullptr);

    ~CompilerContext();
    // Pinned.
    CompilerContext(CompilerContext&& other) = delete;
    CompilerContext(const CompilerContext&) = delete;
    CompilerContext& operator=(CompilerContext&&) = delete;
    CompilerContext& operator=(const CompilerContext&) = delete;

    void postError(const std::string& str, std::optional<SemanticVersion> versionOfError = {});
    void postWarning(const std::string& str, std::optional<SemanticVersion> versionOfError = {});

    void postErrorInCurrentFile(sc::lex::SourceCodeRange range, const char* generalDescription, std::string description,
                                std::optional<SemanticVersion> versionOfError = {});

    void postWarningInCurrentFile(sc::lex::SourceCodeRange range, const char* generalDescription,
                                  std::string description, std::optional<SemanticVersion> versionOfError = {});

    void assignRoot(struct PyrRootNode& ptr);

    void logErrorInCurFile(sc::lex::SourceCodeRange loc, const char* msg,
                           std::optional<SemanticVersion> versionOfError = {});

    void pushToGenerator(struct PyrParseNode* node, intptr_t type);

    std::tuple<struct PyrParseNode*, intptr_t> popFromGenerator();

    // This has to be defined over in PyrParseNode.hpp
    template <typename T, typename... ARGS>
    [[nodiscard]] T* allocParseNode(sc::lex::SourceCodeRange location, ARGS&&... args);

    std::shared_ptr<TextInfo> textInfo;

    struct ParseErrorInCurFile {
        sc::lex::SourceCodeRange location;
        std::string msg;
        std::optional<SemanticVersion> versionOfError;
    };
    struct PyrRootNode* root { nullptr };
    std::vector<ParseErrorInCurFile> errorsInCurFile {};

    std::size_t errors {};
    std::size_t warnings {};

    LongStack generatorStack {};
    struct VMGlobals* vm_globals {};
    CompilingBytecodes bytecodes {};
    AdvancingAllocPool parseNodePool {};

    // Are all of these always valid? In not, perhaps there is a better way to represent this state?
    struct PyrClass* currentClass {};
    struct PyrClass* currentMetaClass {};
    struct PyrClass* compilingClass {};
    struct PyrMethod* compilingMethod {};
    struct PyrBlock* compilingBlock {};
    struct PyrBlock* compilingPartiallyAppliedFunction {};
    int functionHighestExternalRef { 1 };
    bool functionCantBeClosed { true };

    bool generateTailCallByteCodes { true }; // note, settings this to false, will always disable tail call
    bool isTailCodeBranch { false };
    bool tailIsMethodReturn { false };
};


int yyparse();

template <typename Success, typename Failure> decltype(auto) parse(CompilerContext& cxt, Success&& s, Failure&& f) {
    assert(cxt.root == nullptr);
    const auto err_code = yyparse();

    if (cxt.errorsInCurFile.empty() && err_code == 0) {
        return std::forward<Success>(s)(*cxt.root);
    } else {
        return std::forward<Failure>(f)(cxt.errorsInCurFile, err_code);
    }
}
