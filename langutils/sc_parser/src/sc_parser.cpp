// Copyright Jordan Henderson 2026
#include "sc_parser/sc_parser.hpp"
#include "sc_lexer/codepoint_stream.hpp"


#include "parser_context.hpp"
#include <memory>

#include "sc_grammar_parser.hpp"
#include "sc_parser/ast.hpp"

namespace sc::ast {

[[nodiscard]] std::tuple<ASTGraph, bool> parse(std::shared_ptr<const lex::TextInfo> text_info,
                                                       ::sc::lex::CodePointStream cps) {
    parser::ParserContext cxt {
        text_info,
        cps,
        parser::Action {},
        text_info->is_class_file ? parser::ParserContext::Mode::ClassLibrary : parser::ParserContext::Mode::CommandInitial,
    };

    parser::parser p { cxt };

    const auto ret = p(); // if ret is false, some unknown error was encountered
    const bool result = ret && cxt.graph;
    return { std::move(cxt.graph), result };
}

std::tuple<ASTGraph, bool> parse(std::shared_ptr<const lex::TextInfo> text_info, sc::lex::SourceCodeRange r) {
    return parse(text_info, text_info->code_point_stream(r));
};

std::tuple<ASTGraph, bool> parse(std::shared_ptr<const lex::TextInfo> text_info, sc::lex::SourceCodeLocation r) {
    return parse(text_info, text_info->code_point_stream(r));
};

std::tuple<ASTGraph, bool> parse(std::shared_ptr<const sc::lex::TextInfo> text_info) {
    return parse(text_info, text_info->code_point_stream());
}
}
