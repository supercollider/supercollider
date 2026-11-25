// Copyright Jordan Henderson 2026
#pragma once

#include "text.hpp"

#include "sc_format/format.hpp"

#include <memory>
#include <string>
#include <vector>

namespace sc::ast {
struct ASTGraph;
}

namespace sc::format {

// The text, its AST, and the lines of it left as written, which a rewrite changes together.
struct CodeToFormat {
    std::string text;
    std::shared_ptr<ast::ASTGraph> graph;
    std::vector<TextRange> verbatim_lines;
};

// Makes the rewrites the style turns on, in rounds. A round edits the text, parses it again, and checks the AST
// changed only as its rewrites may change it. Throws InternalError if it didn't.
void apply_rewrites(CodeToFormat& code, Kind kind, const Style& style, bool has_parse_error);
}
