#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_lexer/text_info.hpp"
#include "sc_lexer/text_location.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/sc_parser.hpp"

// Parses the source code, interpreted code unless --class is given, and prints the ast, one node per line indented by
// its depth, as its name and its text on one line. The diagnostics are printed to stderr. Fails if there are any
// errors.
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
    const bool class_file = argc == 3 && std::strcmp(argv[1], "--class") == 0;
    if (argc != 2 && !class_file) {
        std::cerr << "ERROR: incorrect number of arguments to sc_parser_standalone, expected [--class] and "
                     "supercollider source code.\n"
                  << std::endl;
        return EXIT_FAILURE;
    }

#ifdef _MSC_VER
    // need this in order to ensure console output encoding handles UTF-8 properly
    if (SetConsoleOutputCP(CP_UTF8) == 0) {
        std::cerr << "ERROR: can't set console output codepage to UTF-8." << std::endl;
    }

    const std::string source_str = utf8_encode(std::wstring(argv[argc - 1]));
    const auto* source = source_str.c_str();
#else
    const auto* const source = argv[argc - 1];
#endif // _MSC_VER
    const auto text_info = std::make_shared<sc::lex::TextInfo>(sc::lex::NormalisedSource { std::string { source } },
                                                               sc::lex::FileCodeLocation {}, "source", class_file);

    const auto [graph, _] = sc::ast::parse(text_info);
    bool has_error = false;
    for (const auto& d : graph->diagnostics()) {
        std::cerr << d;
        has_error = has_error || d.severity() == sc::diag::Severity::Error;
    }

    // The parser repairs errors, so there is still a tree.
    if (const auto root = graph->root_any()) {
        graph->depth_first_traverse(*root,
                                    [&](const sc::ast::ASTGraph::Edges&, const sc::ast::ASTGraph::Variant& payload,
                                        const sc::lex::SourceCodeRange& loc, sc::ast::Index, size_t depth) {
                                        for (size_t di { 0 }; di < depth; ++di)
                                            std::cout << '\t';
                                        std::string text { text_info->read(loc) };
                                        std::replace(text.begin(), text.end(), '\n', ' ');
                                        std::cout << sc::ast::ASTGraph::node_name(payload) << " " << text << "\n";
                                    });
    }

    return has_error ? EXIT_FAILURE : EXIT_SUCCESS;
}
