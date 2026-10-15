
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
#include "sc_parser/indexes_typed.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_sema/compiler.hpp"
#include "sc_sema/tables.hpp"


std::optional<sc::ir::GraphAndIndex> parse_file(std::filesystem::path dir_entry) {
    std::ifstream file_stream { dir_entry };
    if (!file_stream.is_open()) {
        std::cout << "Could not open file" << dir_entry;
        return { };
    }
    std::stringstream ss { };
    ss << file_stream.rdbuf();
    std::cout << ss.rdbuf();
    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource { std::move(ss).str() }, //
        sc::lex::FileCodeLocation { }, //
        dir_entry, //
        true //
    });

    auto [ast_graph, result] = sc::ast::parse(text_info);
    if (result != 0 || !ast_graph) {
        std::cout << "Failed to parse " << dir_entry << " diagnostic count = " << ast_graph.diagnostics().size()
                  << "\n";
        for (const auto& d : ast_graph.diagnostics()) {
            std::cout << d;
        }
    }

    const auto roots = ast_graph.root();
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
    if (argc != 3) {
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

    const std::string source_str = utf8_encode(std::wstring(argv[1]));
    const auto* source = source_str.c_str();
#else
    const auto* const dir = argv[1];
#endif // _MSC_VER

    const auto start_time = std::chrono::high_resolution_clock::now();

    std::filesystem::path dir_path { dir };

    std::vector<sc::ir::GraphAndIndex> ast_and_index;

    std::error_code ec;
    if (std::filesystem::is_directory(dir_path, ec)) {
        for (auto const& dir_entry : std::filesystem::recursive_directory_iterator { dir_path })
            if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".sc") {
                if (auto r = parse_file(dir_entry.path())) {
                    ast_and_index.push_back(std::move(*r));
                }
            }
    } else if (std::filesystem::is_regular_file(dir_path)) {
        if (dir_path.extension() == ".sc") {
            if (auto r = parse_file(dir_path)) {
                ast_and_index.push_back(std::move(*r));
            }
        } else {
            std::cout << "expected a directory of a .sc file, received : " << dir_path << "\n";
        }
    }


    sc::ir::SymbolTable symbol_table;


    sc::ir::IntrinsicConstraints constraints{};

    const auto* const constraint_text = argv[2];
    if (std::string_view { constraint_text } == "true") {
        const auto abstract_object = symbol_table.get_class_name("AbstractObjectExperimental");
        constraints.insert({ abstract_object,
                             sc::ir::IntrinsicConstraint {
                                 abstract_object, //
                                 { }, //
                                 { }, //
                                 { } //
                             } });
        const auto object = symbol_table.get_class_name("Object");
        constraints.insert({ object,
                             sc::ir::IntrinsicConstraint {
                                 object, //
                                 { abstract_object }, //
                                 { }, //
                                 { } //
                             } });
    }


    sc::ir::IRCompiler comp { symbol_table };
    const auto diags = comp.compile_class_library(
        std::move(constraints), symbol_table.get_class_name("AbstractObjectExperimental"), std::move(ast_and_index));

    if (!diags.empty()) {
        for (const auto& d : diags)
            std::cout << d;
    }

    std::cout << "Successfully compiled " << comp.context()->class_declarations.size() / 2 << " classes." << std::endl;

    const auto finish_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(finish_time - start_time);

    const auto seconds = static_cast<double>(time_taken.count()) / 1000.0;

    return 0;
}
