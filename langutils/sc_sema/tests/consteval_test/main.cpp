#define BOOST_TEST_MODULE sc_sema_test
#include <boost/test/included/unit_test.hpp>

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/tables.hpp"

using Diagnostic = sc::diag::Diagnostic;

std::string consteval_as_json(std::string code) {
    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource { code }, //
        sc::lex::FileCodeLocation {}, //
        "/fake/path",
        false //
    });
    auto [ast_graph_t, result] = sc::ast::parse(text_info);
    auto& ast_graph = ast_graph_t;
    if (result != 0 || !ast_graph) {
        std::cerr << "Failed to parse. Diagnostic count = " << ast_graph.diagnostics().size() << "\n";
        for (const auto& d : ast_graph.diagnostics()) {
            std::cerr << d;
        }
        BOOST_TEST(false);
        return {};
    }
    const auto roots = ast_graph.root();
    if (!roots) {
        std::cerr << "No roots  \n";
        BOOST_TEST(false);
        return {};
    }

    const auto* region_list = std::get_if<sc::ast::RegionListIndex>(&*roots);
    if (!region_list) {
        std::cerr << "Parser did not return a region list";
        BOOST_TEST(false);
        return {};
    }

    if (ast_graph.num_children(*region_list) != 1) {
        std::cerr << "expected 1 expr";
        BOOST_TEST(false);
        return {};
    }

    auto first_expr = *ast_graph.children(*region_list).iter();

    using R = std::optional<std::variant<std::pair<sc::ir::ConstExpression, sc::ir::SymbolTable>, Diagnostic>>;
    auto res =
        std::visit(sc::util::overload { [](sc::ast::ErrorIndex) -> R { return {}; },
                                        [&](auto i) -> R { return { sc::ir::compile_consteval(ast_graph, i) }; } },
                   ast_graph.index_to_variant(first_expr));

    if (!res) {
        BOOST_TEST(false);
        return {};
    }

    if (auto diag = std::get_if<Diagnostic>(&res.value())) {
        std::cerr << *diag;
        BOOST_TEST(false);
        return {};
    }

    auto [const_e, symbol_table_t] = std::get<std::pair<sc::ir::ConstExpression, sc::ir::SymbolTable>>(std::move(*res));
    auto symbol_table = std::move(symbol_table_t);


    std::stringstream ss;
    const_e.to_json(ss, symbol_table);
    return std::move(ss).str();
}

BOOST_AUTO_TEST_CASE(consteval) {
    BOOST_TEST(consteval_as_json("1") == "{\"int\": 1}");
    BOOST_TEST(consteval_as_json("1.0") == "{\"float\": 1}");
    BOOST_TEST(consteval_as_json("'asdf'") == "{\"symbol\": \"asdf\"}");
    BOOST_TEST(consteval_as_json("'asdf'") == "{\"symbol\": \"asdf\"}");
    BOOST_TEST(consteval_as_json("[1,2.1, 3pi]")
               == "{\"array\": [{\"int\": 1}, {\"float\": 2.1}, {\"float\": 9.424778}]}");
    BOOST_TEST(consteval_as_json("[1,2.1, /* random stuff */ 3pi]")
               == "{\"array\": [{\"int\": 1}, {\"float\": 2.1}, {\"float\": 9.424778}]}");

    BOOST_TEST(consteval_as_json("[]") == "{\"array\": []}");
    BOOST_TEST(consteval_as_json("#[meow]") == "{\"array\": [{\"symbol\": \"meow\"}]}");

    BOOST_TEST(consteval_as_json("(a: 10)")
               == "{\"event\": [{\"key\": {\"symbol\": \"a\"}, \"value\": {\"int\": 10}}]}");
}
