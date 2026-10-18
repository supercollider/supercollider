// Copyright Jordan Henderson 2026
#include "sc_parser/sc_parser.hpp"
#include "sc_lexer/codepoint_stream.hpp"


#include "parser_context.hpp"
#include <memory>

#include "sc_grammar_parser.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/ast.hpp"

namespace sc::ast {

[[nodiscard]] std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const lex::TextInfo> text_info,
                                                       ::sc::lex::CodePointStream cps) {
    parser::ParserContext ctx {
        text_info,
        cps,
        parser::Action {},
        text_info->is_class_file ? parser::ParserContext::Mode::ClassLibrary : parser::ParserContext::Mode::CommandInitial,
    };

    parser::parser p { ctx };

    const auto ret = p(); // if ret is false, some unknown error was encountered
    const bool result = ret && ctx.graph;
    return { std::move(ctx.graph), result };
}


std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const lex::TextInfo> text_info, sc::lex::SourceCodeRange r) {
    return parse(text_info, sc::lex::CodePointStream { text_info->source, text_info->source_start_in_file, r });
};

std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const lex::TextInfo> text_info, sc::lex::SourceCodeLocation r) {
    return parse(text_info, { text_info->source, text_info->source_start_in_file, r });
};

std::tuple<std::shared_ptr<ASTGraph>, bool> parse(std::shared_ptr<const sc::lex::TextInfo> text_info) {
    return parse(text_info, { text_info->source, text_info->source_start_in_file });
}
}
