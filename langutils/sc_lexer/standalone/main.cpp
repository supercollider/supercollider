#include "codepoint_stream.hpp"
#include "normalise_source.hpp"
#include "text_location.hpp"
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <lexer.hpp>
#include <sstream>
#ifdef _MSC_VER
#    include <windows.h>
#endif // _MSC_VER

using namespace sc::lex;

template <typename Stream> void print(Stream& s, SourceCodeRange r) {
    s << "{ \"Start\": [" << r.begin.lineNumber << ", " << r.begin.column << "], \"End\": [" << r.end.lineNumber << ","
      << r.end.column << "]}";
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

    ss << "{\n";
    for (auto r = lexer(stream, action); r.type != TokenType::EndOfFile; r = lexer(stream, action)) {
        const auto [ptr, sz] = stream.source_code_range_to_text(r.range);

        ss << "\t{ \"Type\": "
           << "\"" << to_string(r.type) << "\", ";
        ss << "\"Location\": ";
        print(ss, r.range);
        ss << ", ";
        ss << "\"Text\": ";
        ss << "\"";
        ss.write(ptr, static_cast<std::streamsize>(sz));
        ss << "\"},\n";
    }
    ss << "}\n";

    const auto string = std::move(ss).str();
    std::cout << string;

    return 0;
}
