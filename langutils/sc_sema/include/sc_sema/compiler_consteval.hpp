#pragma once

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_sema/const_expr.hpp"

namespace sc::ast {
class ASTGraph;
}

namespace sc::ir {

// Makes some basic assumptions about the class library.
// Should not be used when compiling true class library code where we want to allow users to provide extention methods
//   that override common methods.

struct Context;
std::variant<ConstExpression, sc::diag::Diagnostic> compile_consteval(ast::ASTGraph&, ast::AnyExprIndex, Context&);

}