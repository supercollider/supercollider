
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

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
#include "sc_sema/symbol_table.hpp"

std::optional<std::pair<std::shared_ptr<const sc::ast::ASTGraph>, sc::ast::ClassOrExtensionListIndex>>
parse_file(std::filesystem::path dir_entry) {
    std::ifstream file_stream { dir_entry };
    if (!file_stream.is_open()) {
        std::cout << "Could not open file" << dir_entry << "\n";
        return { };
    }
    std::stringstream ss { };
    ss << file_stream.rdbuf();
    // std::cout << ss.rdbuf() << "\n";
    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource { std::move(ss).str() }, //
        sc::lex::FileCodeLocation { }, //
        dir_entry, //
        true //
    });

    auto [ast_graph, result] = sc::ast::parse(text_info);
    if (result != 0 || !ast_graph) {
        std::cout << "Failed to parse " << dir_entry << " diagnostic count = " << ast_graph->diagnostics().size()
                  << "\n";
        for (const auto& d : ast_graph->diagnostics()) {
            std::cout << d;
        }
    }

    const auto roots = ast_graph->root();
    if (!roots) {
        std::cout << "No roots  " << dir_entry << "\n";
        return { };
    }
    if (!std::get_if<sc::ast::ClassOrExtensionListIndex>(&*roots)) {
        std::cout << "Parser returned an scd file, not a class file.";
        return { };
    }

    const auto class_list_index = std::get<sc::ast::ClassOrExtensionListIndex>(*roots);
    return { { std::move(ast_graph), class_list_index } };
}


template <typename T> std::filesystem::path get_path(const T* c) {
#ifdef _MSC_VER
    const std::string source_str = utf8_encode(std::wstring(c));
    const auto* dir = source_str.c_str();
#else
    const char* const dir = c;
#endif // _MSC_VER

    return { dir };
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
    if (argc > 1) {
        std::cout
            << "ERROR: incorrect number of arguments to sc_sema_standalone, expected [dir|.sc file] [true or false]"
            << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }
#endif // _MSC_VER

    assert(argc > 0);
    const size_t num_mods = argc - 1;

    const auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<sc::ir::SourceEntry> entries {};

    sc::ir::ModuleTable module_table {};
    for (size_t i { 0 }; i < num_mods; ++i) {
        const auto module_index = sc::ir::ModuleIndex::from(i);

        std::filesystem::path dir_path { get_path(argv[i]) };

        std::error_code ec;
        if (std::filesystem::is_directory(dir_path, ec)) {
            for (auto const& dir_entry : std::filesystem::recursive_directory_iterator { dir_path })
                if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".sc") {
                    if (auto r = parse_file(dir_entry.path())) {
                        entries.push_back({ std::move(r->first), r->second, module_index });
                    }
                }
        } else if (std::filesystem::is_regular_file(dir_path)) {
            if (dir_path.extension() == ".sc") {
                if (auto r = parse_file(dir_path)) {
                    entries.push_back({ std::move(r->first), r->second, module_index });
                }
            }
        }
    }


    sc::ir::SymbolTable symbol_table;


    // These aren't the constraints of the class library.
    // They are just what is needed to get the compiler to work, and then to test the intrinsic system.
    sc::ir::IntrinsicConstraints constraints { };
    const auto abstract_object = symbol_table.get_class_name("AbstractObjectExperimental");
    constraints.insert({ abstract_object,
                         sc::ir::IntrinsicConstraint {
                             abstract_object, //
                             { }, //
                             { }, //
                             { } //
                         } });

    {
        const auto object = symbol_table.get_class_name("Object");
        constraints.insert({ object,
                             sc::ir::IntrinsicConstraint {
                                 object, //
                                 { abstract_object }, //
                                 { }, //
                                 { } //
                             } });
        const auto meta_obj = symbol_table.get_meta_class_name(object);
        constraints.insert({ meta_obj,
                             sc::ir::IntrinsicConstraint {
                                 meta_obj, { symbol_table.get_meta_class_name(abstract_object) }, {}, {} } });
    }
    { }


    auto common_c = sc::ir::symbol_table_and_common_classes(symbol_table, abstract_object);

    sc::ir::Context ctx { symbol_table, common_c, module_table, entries };

    sc::ir::IRCompiler comp { ctx };

    const auto diags = comp.compile_class_library(constraints);

    if (!diags.empty()) {
        for (const auto& d : diags)
            std::cout << d;
    }

    std::cout << "Successfully compiled " << ctx.class_declarations.num_not_poisoned() / 2 << " classes." << std::endl;

    const auto finish_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time);

    const auto seconds = static_cast<double>(time_taken.count()) / 1000.0;


    return 0;
}
