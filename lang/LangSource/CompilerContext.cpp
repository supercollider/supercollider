#include "CompilerContext.hpp"
#include "PyrSymbol.h"
#include "SC_LanguageClient.h"
#include "codepoint.hpp"
#include "source_utils.hpp"
#include "SCBase.h"
#include "SC_Codecvt.hpp"
#include "SC_Filesystem.hpp"
#include "text_location.hpp"
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string_view>


[[nodiscard]] std::tuple<const char*, std::size_t> TextInfo::indexIntoSource(sc::lex::SourceCodeRange range) const {
    const std::string& str { normalisedSource.as_string() };
    return { str.c_str() + range.begin.absolute, range.size() };
}

[[nodiscard]] DiagnosticHighlight TextInfo::createDiagnosticHighlight(sc::lex::SourceCodeRange range,
                                                                      std::string description) const {
    const std::string& str { normalisedSource.as_string() };
    // clang-format off
    return {
        filePath ? filePath->name : nullptr,
		str.c_str(), 
		str.size(),
		range,
		offsetInFile.line_number,
		offsetInFile.column,
        std::move(description),
    };
    // clang-format on
}


CompilerContext* bison_cxt { nullptr };

std::string diagnosticToCompilerError(ErrorType type, const char* generalDescription,
                                      const DiagnosticHighlight* linesToPrint, size_t numLinesToPrint) {
    std::stringstream ss;

    ss << "\n────────────────────────────────────────────────────────────────────────────────\n";

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
    ss << "\n────────────────────────────────────────────────────────────────────────────────\n";
    return ss.str();
}


std::optional<std::string> absoluteSourcePath(std::string_view path) try {
    const std::filesystem::path given = SC_Codecvt::utf8_str_to_path(path);
    std::filesystem::path p = SC_Filesystem::instance().expandTilde(given);
    if (p == given && p.is_absolute())
        return std::nullopt;
    std::error_code ec;
    p = std::filesystem::absolute(p, ec);
    if (ec)
        return std::nullopt;
    return SC_Codecvt::path_to_utf8_str(p.lexically_normal());
} catch (const std::exception&) {
    // On Windows, utf8_str_to_path throws on invalid UTF-8.
    return std::nullopt;
}

void streamFileUri(std::ostream& ss, std::string_view path, std::optional<FileUriLocation> location, PathOs os) {
    static constexpr char hex[] = "0123456789ABCDEF";
    const bool windows = os == PathOs::Windows;
    const auto starts_with = [&](std::string_view prefix) { return path.substr(0, prefix.size()) == prefix; };
    const auto is_separator = [&](char c) { return c == '/' || (windows && c == '\\'); };

    bool is_unc = false;
    if (windows) {
        if (starts_with(R"(\\?\UNC\)")) {
            path.remove_prefix(8);
            is_unc = true;
        } else if (starts_with(R"(\\?\)")) {
            path.remove_prefix(4);
        } else if (path.size() >= 2 && is_separator(path[0]) && is_separator(path[1])) {
            path.remove_prefix(2);
            is_unc = true;
        }
    }

    ss << "file://";
    // A UNC server becomes the host; anything else is a path from the root, e.g. /C:/a.
    if (!is_unc && (path.empty() || !is_separator(path[0])))
        ss << '/';

    for (const char c : path) {
        const auto byte = static_cast<unsigned char>(c);
        const bool unreserved = (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z')
            || (byte >= '0' && byte <= '9') || byte == '-' || byte == '.' || byte == '_' || byte == '~' || byte == '/'
            || byte == ':';
        if (windows && byte == '\\')
            ss << '/';
        else if (unreserved)
            ss << c;
        else
            ss << '%' << hex[byte >> 4] << hex[byte & 0xF];
    }

    if (location)
        ss << '#' << location->line + 1 << ':' << location->column + 1;
}

std::ostream& streamSourceCodeWithHighlight(std::ostream& ss, const DiagnosticHighlight& highlight, bool printSource,
                                            const char* newlinePrefixPtr, size_t newlinePrefixSz) {
    const auto& [filepath, txt, txt_len, range, source_line, source_column, description] { highlight };

    const auto starting_line = source_line + range.begin.line_number;
    // The snippet's column offset only applies to its first line.
    const auto start_column = (range.begin.line_number == 0 ? source_column : 0) + range.begin.column;

    if (filepath) {
        streamFileUri(ss, filepath, FileUriLocation { starting_line, start_column });
        ss << '\n';
    } else
        ss << "unknown file:" << starting_line + 1 << ':' << start_column + 1 << '\n';

    if (!txt || txt_len == 0) {
        ss.write(newlinePrefixPtr, newlinePrefixSz);
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

        // If is outside of the highlight range
        if (line_number < range.begin.line_number || line_number > range.end.line_number) {
            ss.write(newlinePrefixPtr, newlinePrefixSz);
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
            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << std::setfill(' ') << std::setw(5) << real_line_number << " │ ";
            ss.write(ptr, sz);

            if (!ends_in_newline_char)
                ss << '\n';

            ss.write(newlinePrefixPtr, newlinePrefixSz);
            ss << std::setfill(' ') << std::setw(5) << ' ' << " ┆ ";
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
            ss.write(newlinePrefixPtr, newlinePrefixSz);
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
    thingsPosted += 1;
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
    thingsPosted += 1;
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
