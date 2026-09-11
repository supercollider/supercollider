#include "CompilerContext.hpp"
#include "PyrSymbol.h"
#include "SC_LanguageClient.h"
#include "codepoint.hpp"
#include "source_utils.hpp"
#include "SCBase.h"
#include "text_location.hpp"
#include <cstring>
#include <iomanip>
#include <sstream>


[[nodiscard]] std::tuple<const char*, std::size_t> TextInfo::indexIntoSource(sc::lex::SourceCodeRange range) const {
    const std::string& str { normalisedSource.as_string() };
    return { str.c_str() + range.begin.absolute, range.size() };
}

[[nodiscard]] DiagnosticHighlight TextInfo::createDiagnosticHighlight(sc::lex::SourceCodeRange range,
                                                                      std::string description) const {
    const std::string& str { normalisedSource.as_string() };
    return { filePathDescription.name, str.c_str(),           str.size(), range, offsetInFile.line_number,
             offsetInFile.column,      std::move(description) };
}


CompilerContext* bison_cxt { nullptr };

std::string diagnosticToCompilerError(ErrorType type, const char* generalDescription,
                                      const DiagnosticHighlight* linesToPrint, size_t numLinesToPrint) {
    std::stringstream ss;

    ss << "\n──────────────────────────────────────────────────────────────────────────────────\n";

    if (type == ErrorType::Error)
        ss << "Error: ";
    else if (type == ErrorType::Warning)
        ss << "Warning: ";

    if (generalDescription)
        ss << generalDescription;

    ss << '\n';

    for (size_t i { 0 }; i < numLinesToPrint; ++i) {
        streamSourceCodeWithHighlight(ss, linesToPrint[i]);

        if (i < numLinesToPrint - 1) {
            ss << "\n";
        }
    }
    ss << "\n──────────────────────────────────────────────────────────────────────────────────\n";
    return ss.str();
}


std::stringstream& streamSourceCodeWithHighlight(std::stringstream& ss, const DiagnosticHighlight& highlight,
                                                 bool printSource, const char* newlinePrefixPtr,
                                                 size_t newlinePrefixSz) {
    const auto& [filepath, txt, txt_len, range, source_line, source_column, description] { highlight };

    const auto starting_line = source_line + range.begin.line_number;
    const auto start_column = source_column + range.end.column;

    if (filepath)
        ss << "file://" << filepath << ':' << starting_line + 1 << ':' << start_column + 1 << '\n';
    else
        ss << "unknown file:" << starting_line + 1 << ':' << start_column + 1 << '\n';

    if (!txt || txt_len == 0) {
        ss << description;
        return ss; // nothing left to do
    }
    if (!printSource)
        return ss;

    const auto selection_start = txt + range.begin.absolute;
    const auto selection_end = txt + range.end.absolute;

    auto maybe_iter = sc::lex::utils::LineIter::make(txt, txt + txt_len, range.begin);
    if (!maybe_iter.has_value())
        return ss;

    auto iter = std::move(*maybe_iter);

    if (range.line_count() <= 4) {
        iter.backwards();
        iter.backwards();
    }

    const auto single_line_error = range.line_count() == 1;

    for (auto line = iter.forwards(); line; line = iter.forwards()) {
        const auto [ptr, sz, line_number, ends_in_newline_char] = *line;
        const auto real_line_number = line_number + 1 + source_line;

        ss.write(newlinePrefixPtr, newlinePrefixSz);

        // If is outside of the highligh range
        if (line_number < range.begin.line_number || line_number > range.end.line_number) {
            ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
            ss.write(ptr, sz);
            if (!ends_in_newline_char)
                ss << '\n';
            if (line_number > range.end.line_number + 2)
                break;
            else
                continue;
        }
        // Otherwise...

        if (single_line_error) {
            ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << "      ┆ ";
            auto cp_iter = sc::lex::utils::CodePointIterator::make(ptr, ptr + sz, ptr).value();

            for (auto cp = cp_iter.forwards(); cp; cp = cp_iter.forwards()) {
                if (cp_iter.current_location() <= selection_start) {
                    ss << sc::lex::codepoint_as_whitespace(*cp);
                } else if (cp_iter.current_location() <= selection_end) {
                    const auto w { sc::lex::codepoint_width(*cp) };
                    for (size_t i { 0 }; i < w; ++i)
                        ss << '^';
                } else
                    break;
            }

            ss << " " << description << '\n';
        } else {
            ss << std::setfill(' ') << std::setw(5) << real_line_number << "*│ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            if (line_number == range.end.line_number) {
                ss.write(newlinePrefixPtr, newlinePrefixSz);
                ss << "     >│ ^ " << description << '\n';
            }
        }
    }
    return ss;
}

void CompilerContext::postError(const std::string& str, std::optional<SemanticVersion> versionOfError) {
    ::postText(str.c_str(), str.size());
    if (versionOfError) {
        if (SC_Version >= *versionOfError) {
            ++errors;
        } else {
            const auto str = versionOfError->asString();
            ::post("WARNING: From version %s onwards the preceding error will be a compilation failure, please fix "
                   "the code before updating.\n\n",
                   str.c_str());
        }
    } else {
        ++errors;
    }
}


void CompilerContext::postWarning(const std::string& str, std::optional<SemanticVersion> versionOfError) {
    ::postText(str.c_str(), str.size());
    if (versionOfError) {
        if (SC_Version >= *versionOfError) {
            ++warnings;
        } else {
            const auto str = versionOfError->asString();
            ::post("WARNING: From version %s onwards the preceding error will be a compilation failure, please fix "
                   "the code before updating.\n\n",
                   str.c_str());
        }
    } else {
        ++warnings;
    }
}


void CompilerContext::logErrorInCurFile(sc::lex::SourceCodeRange loc, const char* msg,
                                        std::optional<SemanticVersion> versionOfError) {
    errorsInCurFile.push_back({ loc, std::string { msg }, versionOfError });
}

void CompilerContext::assignRoot(struct PyrRootNode& ptr) {
    assert(root == nullptr);
    root = &ptr;
}

void CompilerContext::pushToGenerator(struct PyrParseNode* node, intptr_t type) {
    pushls(&generatorStack, (intptr_t)node);
    pushls(&generatorStack, type);
}

std::tuple<struct PyrParseNode*, intptr_t> CompilerContext::popFromGenerator() {
    auto t = popls(&generatorStack);
    auto n = (struct PyrParseNode*)popls(&generatorStack);
    return { n, t };
}
void CompilerContext::postErrorInCurrentFile(sc::lex::SourceCodeRange range, const char* generalDescription,
                                             std::string description, std::optional<SemanticVersion> versionOfError) {
    const auto h = textInfo->createDiagnosticHighlight(range, std::move(description));
    const auto str = diagnosticToCompilerError(ErrorType::Error, generalDescription, &h, 1);
    postError(str, versionOfError);
}

void CompilerContext::postWarningInCurrentFile(sc::lex::SourceCodeRange range, const char* generalDescription,
                                               std::string description, std::optional<SemanticVersion> versionOfError) {
    const auto h = textInfo->createDiagnosticHighlight(range, std::move(description));
    const auto str = diagnosticToCompilerError(ErrorType::Error, generalDescription, &h, 1);
    postWarning(str, versionOfError);
}
