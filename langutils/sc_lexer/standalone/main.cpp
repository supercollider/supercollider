#include "sc_lexer/codepoint_stream.hpp"
#include "sc_lexer/lexer.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <sstream>
#include <string_view>
#ifdef _MSC_VER
#    include <windows.h>
#endif // _MSC_VER

using namespace sc::lex;

template <typename Stream> void print(Stream& s, SourceCodeRange r) {
    s << "{ \"Start\": [" << r.begin.line_number << ", " << r.begin.column << "], \"End\": [" << r.end.line_number
      << ", " << r.end.column << "] }";
}

// As a json string, with quotes, control characters and backslashes escaped.
template <typename Stream> void print(Stream& s, std::string_view text) {
    s << '"';
    for (const char c : text) {
        if (c == '"' || c == '\\') {
            s << '\\' << c;
        } else if (static_cast<unsigned char>(c) < 0x20) {
            char escaped[7];
            std::snprintf(escaped, sizeof(escaped), "\\u%04x", static_cast<unsigned>(c));
            s << escaped;
        } else {
            s << c;
        }
    }
    s << '"';
}

#ifdef _MSC_VER
// utility function to convert default Windows stdin encoding to UTF-8
// https://stackoverflow.com/questions/215963/how-do-you-properly-use-widechartomultibyte
std::string utf8_encode(const std::wstring& wstr) {
    if (wstr.empty())
        return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), NULL, 0, NULL, NULL);
    std::string strTo(static_cast<size_t>(size_needed), 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), strTo.data(), size_needed, NULL, NULL);
    return strTo;
}

// need wmain for Windows default stdin encoding
int wmain(int argc, wchar_t* argv[]) {
#else
int main(int argc, char* argv[]) {
#endif // _MSC_VER
    if (argc != 2) {
        std::cerr
            << "ERROR: incorrect number of arguments to sc_lexer_standalone, expected only supercollider source code.\n"
            << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }

    const std::string source_str = utf8_encode(std::wstring(argv[1]));
    const auto* source = source_str.c_str();
#else
    const auto* const source = argv[1];
#endif // _MSC_VER

    const auto source_len = strlen(source);
    sc::lex::NormalisedSource src { source, source_len };

    CodePointStream stream { std::move(src), {} };
    actions::TypeAndLocationAction action {};

    std::stringstream ss {};

    // An array of the tokens, in order, without the end of the file.
    ss << "[";
    const char* separator = "\n";
    for (auto r = lexer(stream, action); r.type != TokenType::EndOfFile; r = lexer(stream, action)) {
        const auto [ptr, sz] = stream.source_code_range_to_text(r.range);

        ss << separator << "\t{ \"Type\": \"" << to_string(r.type) << "\", \"Location\": ";
        print(ss, r.range);
        ss << ", \"Text\": ";
        print(ss, std::string_view { ptr, sz });
        ss << " }";
        separator = ",\n";
    }
    ss << "\n]\n";

    const auto string = std::move(ss).str();
    std::cout << string;

    return 0;
}
