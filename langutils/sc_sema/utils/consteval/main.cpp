#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/context.hpp"
#include "sc_sema/intrinsic_constraint.hpp"
#include "sc_sema/ir.hpp"
#include "sc_sema/symbol_table.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Evaluates a piece of interpreted code as a constant, printing the result as json. The code is compiled as 3.16, after
// this basic class library, the classes the compiler requires, as in the regression tests' prelude.
// Exits with failure, printing the diagnostics, if it isn't a constant.
namespace {
constexpr const char* prelude = R"(//# VERSION 3.16
AbstractObjectExperimental {}
Object : AbstractObjectExperimental {
    classvar dependantsDictionary, currentEnvironment, topEnvironment, uniqueMethods;
}
Class {}
String {}
True {}
False {}
Nil {}
Float {}
Integer {}
Char {}
Symbol {}
Ref {}
Array {}
Event {}
Interpreter {
    var <cmdLine, filePath, lineNumber, column, context;
    var <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;
    var <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;
    var <>u, <>v, <>w, <>x, <>y, <>z;
    var <>codeDump, <>preProcessor;

    functionCompileContext {}
}
)";

std::shared_ptr<const sc::ast::ASTGraph> parse(std::string source, const char* path, bool class_file,
                                               std::vector<sc::diag::Diagnostic>& diagnostics) {
    auto text_info = std::make_shared<sc::lex::TextInfo>(sc::lex::NormalisedSource { std::move(source) },
                                                         sc::lex::FileCodeLocation {}, path, class_file);
    auto [ast_graph, _] = sc::ast::parse(text_info);
    if (ast_graph)
        for (const auto& d : ast_graph->diagnostics())
            diagnostics.push_back(d);
    return ast_graph;
}
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
        std::cerr << "ERROR: incorrect number of arguments to sc_consteval, expected only supercollider source code.\n"
                  << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }

    const std::string source = utf8_encode(std::wstring(argv[1]));
#else
    const std::string source = argv[1];
#endif // _MSC_VER

    std::vector<sc::diag::Diagnostic> diagnostics;
    const auto print_diagnostics = [&]() {
        for (const auto& d : diagnostics)
            std::cerr << d;
    };

    std::vector<std::optional<sc::ir::SourceEntry>> entries;
    {
        const auto ast_graph = parse(prelude, "prelude.sc", true, diagnostics);
        const auto root = ast_graph ? ast_graph->root() : std::nullopt;
        assert(root && std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*root));
        entries.push_back(sc::ir::SourceEntry { ast_graph, std::get<sc::ast::ClassOrExtensionListIndex>(*root),
                                                sc::ir::ModuleIndex::from(0) });
    }

    // As in ir_dump, just what the compiler needs, not the class library's constraints.
    sc::ir::SymbolTable symbol_table;
    sc::ir::IntrinsicConstraints constraints {};
    const auto abstract_object = symbol_table.get_class_name("AbstractObjectExperimental");
    constraints.insert({ abstract_object, sc::ir::IntrinsicConstraint { abstract_object, {}, {}, {} } });
    {
        const auto object = symbol_table.get_class_name("Object");
        constraints.insert({ object, sc::ir::IntrinsicConstraint { object, { abstract_object }, {}, {} } });
        const auto meta_obj = symbol_table.get_meta_class_name(object);
        constraints.insert({ meta_obj,
                             sc::ir::IntrinsicConstraint {
                                 meta_obj, { symbol_table.get_meta_class_name(abstract_object) }, {}, {} } });
    }

    sc::ir::ModuleTable module_table {};
    sc::ir::Context ctx { symbol_table, sc::ir::symbol_table_and_common_classes(symbol_table, abstract_object),
                          module_table, entries };
    sc::ir::IRCompiler comp { ctx };

    // Compile class library
    for (auto& d : comp.compile_class_library(constraints, 1))
        diagnostics.push_back(std::move(d));


    // Compiler source
    const auto ast_graph = parse(source, "consteval.scd", false, diagnostics);
    const auto root = ast_graph ? ast_graph->root() : std::nullopt;
    if (!root || std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*root)) {
        print_diagnostics();
        std::cerr << "Expected interpreted code, not a class file.\n";
        return EXIT_FAILURE;
    }

    auto interpreted = comp.compile_interpreted(ast_graph, sc::ast::LanguageVersion::V3_16);
    for (auto& d : interpreted.diagnostics)
        diagnostics.push_back(std::move(d));
    print_diagnostics();
    if (!interpreted.ir)
        return EXIT_FAILURE;

    // A list of regions, or a file as a function.
    const auto& ir = *interpreted.ir;
    const auto [code] = ir.children(std::get<sc::ir::SCInterpreted_I>(*ir.root()));
    const sc::ir::NodeIndex code_i { *code };
    sc::ir::OptionalConstExprIndex result {};
    if (const auto regions = ir.as_a<sc::ir::SCRegionList_I>(code_i))
        result = ir.payload(*regions).constant_result;
    else if (const auto function = ir.as_a<sc::ir::SCFunction_I>(code_i))
        result = ir.payload(*function).constant_result;

    if (!result) {
        std::cerr << "Not a constant.\n";
        return EXIT_FAILURE;
    }
    ctx.constant_table(*result).to_json(std::cout, symbol_table, ctx.constant_table);
    std::cout << std::endl;
    return 0;
}
