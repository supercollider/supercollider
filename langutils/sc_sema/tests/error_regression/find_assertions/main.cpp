// Prints the assertions in a regression test's sc file, the line comments starting with `expect`, e.g.,
// `// expect: -> 1`, one per line as `line:column text`, counting from one. test.py checks them against ir_dump.
// The lexer finds the comments, so `//` in a string, symbol or char isn't one, and nor is anything in a block comment.
#include "sc_lexer/codepoint_stream.hpp"
#include "sc_lexer/lexer.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_lexer/tokens.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace {

// Keeps only line comments, and the end of the file so the loop below can stop.
struct CommentAction {
    struct Output {
        sc::lex::TokenType type;
        sc::lex::SourceCodeRange range;
    };
    template <sc::lex::TokenType type> std::optional<Output> process(sc::lex::SourceCodeRange range) {
        if constexpr (type == sc::lex::TokenType::Comment || type == sc::lex::TokenType::EndOfFile)
            return Output { type, range };
        else
            return std::nullopt;
    }
};

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: find_assertions file.sc\n";
        return EXIT_FAILURE;
    }
    std::ifstream file { argv[1] };
    if (!file.is_open()) {
        std::cerr << "could not open " << argv[1] << "\n";
        return EXIT_FAILURE;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    // Normalised as the compiler does, so the positions match ir_dump's.
    const sc::lex::NormalisedSource source { std::move(ss).str() };
    const auto& text = static_cast<const std::string&>(source);

    sc::lex::CodePointStream stream { source };
    CommentAction action;
    for (auto token = sc::lex::lexer(stream, action); token.type != sc::lex::TokenType::EndOfFile;
         token = sc::lex::lexer(stream, action)) {
        std::string_view comment { text.data() + token.range.begin.absolute, token.range.size() };
        comment.remove_prefix(2); // `//`
        comment.remove_prefix(std::min(comment.find_first_not_of(' '), comment.size()));
        if (comment.substr(0, 6) != "expect")
            continue;
        std::cout << token.range.begin.line_number + 1 << ':' << token.range.begin.column + 1 << ' ' << comment << '\n';
    }
    return EXIT_SUCCESS;
}
