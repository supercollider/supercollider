
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
#include "sc_sema/create_graph.hpp"

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
        std::cerr
            << "ERROR: incorrect number of arguments to sc_sema_standalone, expected only supercollider source code.\n"
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

    std::filesystem::path dir_path { dir };

    std::vector<sc::ir::GraphAndIndex> ast_and_index;

    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator { dir_path })
        if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".sc") {
            std::ifstream file_stream { dir_entry.path() };
            if (!file_stream.is_open()) {
                // std::cerr << "Could not open file" << dir_entry.path();
                continue;
            }
            std::stringstream ss {};
            ss << file_stream.rdbuf();

            auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
                sc::lex::NormalisedSource { std::move(ss).str() }, //
                sc::lex::FileCodeLocation {}, //
                dir_entry.path(), //
                true //
            });

            auto [ast_graph, result] = sc::ast::parse(text_info);
            if (result != 0 || !ast_graph) {
                std::cerr << "Failed to parse " << dir_entry.path()
                          << " diagnostic count = " << ast_graph.diagnostics().size() << "\n";
                for (const auto& d : ast_graph.diagnostics()) {
                    std::cerr << d;
                }
                continue;
            }

            const auto roots = ast_graph.root();
            if (!roots) {
                // std::cerr << "No roots  " << dir_entry.path() << "\n";
                continue;
            }
            if (!std::get_if<sc::ast::ClassOrExtensionListIndex>(&*roots)) {
                // std::cerr << "Parser returned an scd file, not a class file.";
                continue;
            }

            const auto class_list_index = std::get<sc::ast::ClassOrExtensionListIndex>(*roots);
            ast_and_index.push_back({ std::move(ast_graph), class_list_index });
        }

    std::cout << ast_and_index.size() << "\n";

    sc::ir::IRCompiler comp {};

    const auto diags = comp.compile_class_library({}, std::move(ast_and_index));

    if (!diags.empty()) {
        for (const auto& d : diags)
            std::cerr << d;
    }

    comp.context()->class_declarations.for_all([&](sc::ir::Symbol name, const sc::ir::ClassDeclaration& d) {
        std::cout << comp.context()->symbol_table(name) << "\n";
    });

    return 0;
}
