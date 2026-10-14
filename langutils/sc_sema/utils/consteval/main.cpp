
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/const_expr.hpp"
#include "sc_sema/tables.hpp"
#include <iostream>


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
        std::cerr << "ERROR: incorrect number of arguments to sc_consteval, expected only supercollider source code.\n"
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

    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource { std::string { source } }, //
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
        return 1;
        ;
    }

    const auto roots = ast_graph.root();
    if (!roots) {
        std::cerr << "No roots  \n";
        return 1;
    }

    const auto* region_list = std::get_if<sc::ast::RegionListIndex>(&*roots);
    if (!region_list) {
        std::cerr << "Parser did not return a region list";
        return 1;
    }


    if (ast_graph.num_children(*region_list) != 1) {
        std::cerr << "expected 1 expr";
        return 1;
    }

    auto first_expr = *ast_graph.children(*region_list).iter();

    using Diagnostic = sc::diag::Diagnostic;

    using R = std::optional<std::variant<std::pair<sc::ir::ConstExpression, sc::ir::SymbolTable>, Diagnostic>>;
    auto res =
        std::visit(sc::util::overload { [](sc::ast::ErrorIndex) -> R { return {}; },
                                        [&](auto i) -> R { return { sc::ir::compile_consteval(ast_graph, i) }; } },
                   ast_graph.index_to_variant(first_expr));

    if (!res) {
        return 1;
    }

    if (auto diag = std::get_if<Diagnostic>(&res.value())) {
        std::cout << *diag;
        return 1;
    }

    auto [const_e, symbol_table_t] = std::get<std::pair<sc::ir::ConstExpression, sc::ir::SymbolTable>>(std::move(*res));
    auto symbol_table = std::move(symbol_table_t);


    const_e.to_json(std::cout, symbol_table);
    std::cout << std::endl;


    return 0;
}
