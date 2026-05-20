#include "codepoint_stream.hpp"
#include "text_location.hpp"
#include <cstdlib>
#include <cstring>
#include <ios>
#include <iostream>
#include <lexer.hpp>
#include <sstream>

using namespace sc::lex;

template <typename Stream> void print(Stream& s, SourceCodeRange r) {
    s << "{ \"Start\": [" << r.begin.lineNumber << ", " << r.begin.column << "], \"End\": [" << r.end.lineNumber << ","
      << r.end.column << "]}";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr
            << "ERROR: incorrect number of arguments to sc_lexer_standalone, expected only supercollider source code.\n"
            << std::endl;
        return EXIT_FAILURE;
    }

    const auto* const source = argv[1];
    const auto source_len = strlen(source);


    CodePointStream stream { source, source_len, {} };
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
