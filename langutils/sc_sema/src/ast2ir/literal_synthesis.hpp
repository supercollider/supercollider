#pragma once

#include "sc_parser/nodes.hpp"
#include <charconv>
#include <string_view>
namespace sc::ir {

[[nodiscard]] inline int synthesize_literal(const ast::IntNode& node, std::string_view str) {
    if (node.kind == ast::IntNode::Kind::Normal) {
        int r;
        const auto [ptr, er] = std::from_chars(str.begin(), str.end(), r);
        // we know this should work because the parser has done the checking for us.
        assert(er == std::errc { });
        assert(ptr == str.end());
        return r;
    }
    return 0;
}

[[nodiscard]] inline double synthesize_literal(const ast::FloatNode& node, std::string_view str) {
    (void)str;
    if (node.kind == ast::FloatNode::Kind::Inf) {
        return std::numeric_limits<double>::infinity();
    }
    return 0;
}

}
