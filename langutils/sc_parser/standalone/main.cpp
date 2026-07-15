#include <cstdlib>
#include <iostream>

#include "sc_parser.hpp"
#include "parser_error_handler.hpp"

namespace P = sc ::parser;
class PostErrorHandler final : public P::ErrorHandler {
public:
    void operator()(std::shared_ptr<const sc::parser::TextInfo>, sc::lex::TokenType, sc::lex::SourceCodeRange,
                    std::optional<sc::lex::SourceCodeRange>) override {
        std::cerr << "Got an error 1" << std::endl;
    }

    // Called from memory error, unlikely.
    void operator()(std::shared_ptr<const sc::parser::TextInfo>, sc::lex::SourceCodeRange,
                    const std::string&) override {
        std::cerr << "Got an error 2" << std::endl;
    }

    // Called for parsing error.
    // Note, the 'int' type here is a parser::symbol_kind_type. The parser has good conversion functions.
    void operator()(std::shared_ptr<const sc::parser::TextInfo> t, std::vector<const char*> expected,
                    sc::lex::SourceCodeRange got_location, const char* got_name) override {
        if (!expected.empty()) {
            std::cerr << "expected: ";
            for (const char* e : expected)
                std::cerr << e << ' ';
        }

        std::cerr << "got: " << got_name << " ";
        const auto [ptr, sz] = t->read(got_location);
        std::cerr.write(ptr, sz);
        std::cerr << "\n";

        std::cerr << "Got an error 3" << std::endl;
    }

    PostErrorHandler() {}
    PostErrorHandler(const PostErrorHandler&) = default;
    PostErrorHandler(PostErrorHandler&&) = delete;
    PostErrorHandler& operator=(const PostErrorHandler&) = default;
    PostErrorHandler& operator=(PostErrorHandler&&) = delete;
    virtual ~PostErrorHandler() = default;
};

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
            << "ERROR: incorrect number of arguments to sc_lexer_standalone, expected only supercollider source code.\n"
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
    auto text_info = std::shared_ptr<P::TextInfo>(new P::TextInfo {
        sc::lex::NormalisedSource(source),
        sc::lex::FileCodeLocation {},
        "test_file",
        false,
    });

    const auto [graph, result] = P::parse(text_info, std::make_shared<PostErrorHandler>());

    if (result == 0) {
        namespace N = P::nodes;
        graph.depth_first_traverse(
            [&](const sc::lex::SourceCodeRange& loc, const N::NodeVariant& payload, size_t depth) {
                for (size_t di { 0 }; di < depth; ++di)
                    std::cout << '\t';
                std::cout << N::NodeCollection::get_name(payload);
                const auto [ptr, sz] = text_info->read(loc);
                std::cout << " ";
                std::cout.write(ptr, sz);
                std::cout << std::endl;
            });
    }


    return result;
}
