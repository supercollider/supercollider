// Copyright Jordan Henderson 2026

// Compiles, without running, the code in help files and example scd files, so the docs keep compiling as the
// language changes. See README.md.

#include "SCDoc.h"

#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "sc_compiler/compiled.hpp"
#include "sc_compiler/inspect.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_source/text_info.hpp"
#include "sc_source/text_location.hpp"
#include "sc_util/indexed_vector.hpp"
#include "sc_util/language_version.hpp"
#include "sc_util/strong_index.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
using sc::ast::LanguageVersion;

////////////////////////////////////////////////////////////////////////////////
// SCDoc's hooks
////////////////////////////////////////////////////////////////////////////////

// SCDoc reports a parse error through error(), and notes through post().
void error(const char* fmt, ...) {
    std::fputs("ERROR: ", stdout);
    va_list args;
    va_start(args, fmt);
    std::vfprintf(stdout, fmt, args);
    va_end(args);
}

void post(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::vfprintf(stdout, fmt, args);
    va_end(args);
}

namespace {

////////////////////////////////////////////////////////////////////////////////
// Finding the code
////////////////////////////////////////////////////////////////////////////////

// What a snippet must do. A `code#fragment::` block, e.g., one using names it doesn't declare, is only parsed. A
// `code#error::` block shows an error, so it must fail to compile.
enum class Check : std::uint8_t { Compiles, Parses, Fails };

// Code to compile: a help file's code block, or a whole scd file.
struct Snippet {
    fs::path file;
    std::string text;
    std::uint32_t line; // Where the text starts in its file, from 1.
    // A code block's `code#<release>::`, or an scd file's line 1, else Legacy.
    LanguageVersion own;
    bool whole_file;
    bool class_code; // A `code#class::` block, class library code rather than code to evaluate.
    Check check;
};
using SnippetIndex = sc::util::StrongIndex<std::uint32_t, struct SnippetIndex__>;

// The class library sclang would compile in the IDE on this platform, see SC_Filesystem::shouldNotCompileDirectory.
// Every platform at once can't work: their directories extend the same classes.
bool skipped_library_directory(const fs::path& dir) {
#if defined(_WIN32)
    constexpr std::string_view host = "windows";
#elif defined(__APPLE__)
    constexpr std::string_view host = "osx";
#else
    constexpr std::string_view host = "linux";
#endif
    std::string name = dir.filename().string();
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return std::tolower(c); });
    const bool platform = name == "linux" || name == "osx" || name == "windows" || name == "iphone" || name == "wasm";
    const bool other_ide = name.size() > 6 && name.rfind("scide_", 0) == 0 && name != "scide_scqt";
    return name == "help" || name == "ignore" || (platform && name != host) || other_ide
        || (!name.empty() && name[0] == '.');
}

std::vector<fs::path> find_library_files(const fs::path& root) {
    std::vector<fs::path> found;
    std::error_code ec;
    fs::recursive_directory_iterator it { root, ec }, end;
    for (; !ec && it != end; it.increment(ec)) {
        if (it->is_directory(ec) && skipped_library_directory(it->path()))
            it.disable_recursion_pending();
        else if (it->is_regular_file(ec) && it->path().extension() == ".sc" && it->path().filename().string()[0] != '.')
            found.push_back(it->path());
    }
    std::sort(found.begin(), found.end());
    return found;
}

std::optional<std::string> read_file(const fs::path& path) {
    std::ifstream in { path, std::ios::binary };
    if (!in)
        return std::nullopt;
    std::ostringstream ss;
    ss << in.rdbuf();
    return std::move(ss).str();
}

// Each CODEBLOCK under node, in order. Inline code is mostly names and fragments, so it is skipped.
void collect_code_blocks(const DocNode* node, const fs::path& file, std::vector<Snippet>& out) {
    if (std::string_view { node->id } == "CODEBLOCK") {
        auto own = LanguageVersion::Legacy;
        bool class_code = false;
        auto check = Check::Compiles;
        for (int i = 0; i < node->n_childs; ++i) {
            const std::string_view id { node->children[i]->id };
            if (id == "VERSION")
                own = sc::ast::parse_release(node->children[i]->text).value_or(own);
            class_code = class_code || id == "CLASSCODE";
            if (id == "FRAGMENTCODE")
                check = Check::Parses;
            else if (id == "ERRORCODE")
                check = Check::Fails;
        }
        out.push_back({ file, node->text ? node->text : "", static_cast<std::uint32_t>(node->line), own, false,
                        class_code, check });
        return;
    }
    for (int i = 0; i < node->n_childs; ++i)
        collect_code_blocks(node->children[i], file, out);
}

////////////////////////////////////////////////////////////////////////////////
// Compiling
////////////////////////////////////////////////////////////////////////////////

struct Failure {
    std::string first_error;
    bool looks_like_class_code = false; // Code to evaluate that parses as a class file with no errors.
    bool unsupported = false; // The new compiler can't compile the code yet, which isn't an error in it.
};

// The class library's files, read once, so each class code block is compiled with them, see compile_class().
using LibrarySources = sc::util::IndexedVector<sc::ir::SourceEntryIndex, sc::compiler::ClassFileSource>;

// Whether code to evaluate parses as class library code, e.g., a class or an extension, so the block needs `#class`.
bool looks_like_class_code(const Snippet& snippet) {
    sc::src::FileCodeLocation start {};
    start.line_number = snippet.line - 1;
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { snippet.text }, start, snippet.file,
                                                    sc::src::TextKind::ClassFile);
    const auto [ast, accepted] = sc::ast::parse(text);
    if (!accepted || !ast || !ast->root() || !std::holds_alternative<sc::ast::ClassOrExtensionListIndex>(*ast->root()))
        return false;
    for (const auto& d : ast->diagnostics())
        if (d.severity() == sc::diag::Severity::Error)
            return false;
    return true;
}

// The first error among diagnostics, if any.
std::optional<std::string> first_error(const std::vector<sc::diag::Diagnostic>& diagnostics,
                                       const fs::path& relative_to) {
    for (const auto& d : diagnostics)
        if (d.severity() == sc::diag::Severity::Error)
            return d.to_string(relative_to);
    return std::nullopt;
}

// A class code block is compiled as a module of its own after the class library, which is compiled again for each
// block, as blocks define the same classes, e.g., WritingClasses' MyClass. Only the block's diagnostics count. Its
// version is a line-1 comment put before it, so its first line is still the file's line `snippet.line`.
std::optional<Failure> compile_class(const LibrarySources& library, const Snippet& snippet, LanguageVersion version,
                                     const fs::path& relative_to) {
    sc::src::FileCodeLocation start {};
    start.line_number = snippet.line - 2;
    const auto source = std::string { "//# VERSION " } + sc::ast::to_string(version) + "\n" + snippet.text;
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { source }, start, snippet.file,
                                                    sc::src::TextKind::ClassFile);
    auto sources = library;
    (void)sources.push_back({ text, sc::ir::ModuleIndex { 1 } });
    const auto compiled = sc::compiler::compile_class_library(sources, 1, {});
    std::vector<sc::diag::Diagnostic> own;
    for (const auto* diagnostics : { &compiled->parse_diagnostics, &compiled->diagnostics })
        for (const auto& d : *diagnostics)
            if (!d.locations().empty() && d.locations().back().text_info == text)
                own.push_back(d);
    if (auto error = first_error(own, relative_to))
        return Failure { std::move(*error) };
    return std::nullopt;
}

// A code block is compiled like the help browser's evaluation, with no file. It has the help file's path and line,
// so diagnostics point into it.
std::optional<Failure> compile(sc::compiler::ClassLibrary& library, const Snippet& snippet,
                               const fs::path& relative_to) {
    // Line 1 of the block is the file's line `snippet.line`. Absolute stays 0, so a version comment at its start is
    // claimed, as it is when the help browser sends the block.
    sc::src::FileCodeLocation start {};
    start.line_number = snippet.line - 1;
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { snippet.text }, start, snippet.file,
                                                    sc::src::TextKind::ScdFile);

    std::optional<std::string> first_error;
    const auto check = [&](const std::vector<sc::diag::Diagnostic>& diagnostics) {
        for (const auto& d : diagnostics)
            if (!first_error && d.severity() == sc::diag::Severity::Error)
                first_error = d.to_string(relative_to);
    };
    auto parsed = sc::compiler::parse_interpreted(text);
    check(parsed.diagnostics);
    if (!parsed.ast)
        return Failure { first_error.value_or("could not be parsed\n") };
    // A block passes its version with the code. An scd file passes none, so its line 1 version is used, as sclang
    // runs a file.
    const auto given = snippet.whole_file ? std::nullopt : std::optional { snippet.own };
    // Only comments, whose value is nil, has no region, so no function.
    const auto root = parsed.ast->root();
    const bool empty = root && std::holds_alternative<sc::ast::RegionListIndex>(*root)
        && !parsed.ast->children(std::get<sc::ast::RegionListIndex>(*root)).iter();
    auto ir = sc::compiler::compile_interpreted_ir(library, std::move(parsed), given, LanguageVersion::Legacy);
    check(ir.diagnostics);
    if (first_error || !ir.graph)
        return Failure { first_error.value_or("could not be compiled\n") };
    const auto compiled = sc::compiler::compile_interpreted(library, ir);
    check(compiled.diagnostics);
    if (first_error)
        return Failure { *first_error };
    if (!empty && std::get<sc::compiler::CompiledInterpreted>(compiled.unit).functions.empty())
        return Failure { "the new compiler can't compile this code yet\n", false, true };
    return std::nullopt;
}

std::optional<Failure> parse_fragment(const Snippet& snippet, LanguageVersion version, const fs::path& relative_to) {
    sc::src::FileCodeLocation start {};
    if (snippet.class_code) {
        start.line_number = snippet.line - 2;
        const auto source = std::string { "//# VERSION " } + sc::ast::to_string(version) + "\n" + snippet.text;
        auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { source }, start, snippet.file,
                                                        sc::src::TextKind::ClassFile);
        const auto [ast, accepted] = sc::ast::parse(text);
        if (!ast)
            return Failure { "could not be parsed\n" };
        if (auto error = first_error(ast->diagnostics(), relative_to))
            return Failure { std::move(*error) };
        if (!accepted)
            return Failure { "could not be parsed\n" };
        return std::nullopt;
    }
    start.line_number = snippet.line - 1;
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { snippet.text }, start, snippet.file,
                                                    sc::src::TextKind::ScdFile);
    const auto parsed = sc::compiler::parse_interpreted(text);
    if (auto error = first_error(parsed.diagnostics, relative_to))
        return Failure { std::move(*error) };
    if (!parsed.ast)
        return Failure { "could not be parsed\n" };
    return std::nullopt;
}

// An scd file's line 1 version, else Legacy, as sclang runs a file.
LanguageVersion file_version(const fs::path& file, const std::string& source) {
    auto text = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { source }, sc::src::FileCodeLocation {},
                                                    file, sc::src::TextKind::ScdFile);
    const auto parsed = sc::compiler::parse_interpreted(text);
    if (parsed.ast && parsed.ast->file_version() && parsed.ast->file_version()->version)
        return *parsed.ast->file_version()->version;
    return LanguageVersion::Legacy;
}

std::string display(const fs::path& file, const fs::path& relative_to) {
    return file.lexically_relative(relative_to).generic_string();
}

int usage() {
    std::cout << "usage: sc_check_docs [--threads n] [--relative-to dir] [--exclude prefix]... --library dir "
                 "[dir|file.schelp|file.scd]...\n";
    return EXIT_FAILURE;
}

}

////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////

// Every code block of each schelp file is compiled as its declared version, else Legacy. Every scd file is compiled
// whole, as its line 1 version, else Legacy. A compile that reports an error is a
// failure, printed as `file:line: version:` and its first error. Warnings don't fail. Exits non-zero on any failure,
// including a help file SCDoc can't parse.
int main(int argc, char* argv[]) {
    std::optional<fs::path> library_root;
    fs::path relative_to = fs::current_path();
    std::size_t num_threads = std::max(1u, std::thread::hardware_concurrency());
    std::vector<fs::path> inputs;
    std::vector<std::string> excluded;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg { argv[i] };
        if (arg == "--library" && i + 1 < argc) {
            library_root = fs::absolute(argv[++i]);
        } else if (arg == "--relative-to" && i + 1 < argc) {
            relative_to = fs::absolute(argv[++i]);
        } else if (arg == "--exclude" && i + 1 < argc) {
            excluded.emplace_back(argv[++i]);
        } else if (arg == "--threads" && i + 1 < argc) {
            const std::string_view count { argv[++i] };
            const auto [end, ec] = std::from_chars(count.data(), count.data() + count.size(), num_threads);
            if (ec != std::errc {} || end != count.data() + count.size() || num_threads == 0)
                return usage();
        } else if (!arg.empty() && arg[0] == '-') {
            return usage();
        } else {
            inputs.push_back(fs::absolute(argv[i]));
        }
    }
    if (!library_root || inputs.empty())
        return usage();
    const auto start_time = std::chrono::steady_clock::now();

    // The schelp and scd files, sorted so the output is the same everywhere.
    std::vector<fs::path> help_files;
    std::vector<fs::path> scd_files;
    const auto add_file = [&](const fs::path& p) {
        const auto name = p.filename().string();
        for (const auto& prefix : excluded)
            if (name.rfind(prefix, 0) == 0)
                return;
        if (p.extension() == ".schelp")
            help_files.push_back(p);
        else if (p.extension() == ".scd")
            scd_files.push_back(p);
    };
    for (const auto& input : inputs) {
        std::error_code ec;
        if (fs::is_directory(input, ec)) {
            fs::recursive_directory_iterator it { input, ec }, end;
            for (; !ec && it != end; it.increment(ec))
                if (it->is_regular_file(ec))
                    add_file(it->path());
        } else if (fs::is_regular_file(input, ec)) {
            add_file(input);
        } else {
            std::cout << "ERROR: no such file or directory " << input << "\n";
            return EXIT_FAILURE;
        }
    }
    std::sort(help_files.begin(), help_files.end());
    std::sort(scd_files.begin(), scd_files.end());

    // SCDoc's parser has global state, so the help files are parsed one at a time.
    std::size_t failures = 0;
    sc::util::IndexedVector<SnippetIndex, Snippet> snippets;
    for (const auto& file : help_files) {
        // An `.ext.schelp` adds methods to another file's page, so it is only a body, as SCDoc.sc parses it.
        const bool addition = file.filename().string().size() > 11
            && file.filename().string().compare(file.filename().string().size() - 11, 11, ".ext.schelp") == 0;
        DocNode* root = scdoc_parse_file(file.string(), addition ? SCDOC_PARSE_PARTIAL : SCDOC_PARSE_FULL);
        if (!root) {
            std::cout << display(file, relative_to) << ": SCDoc can't parse it\n";
            ++failures;
            continue;
        }
        collect_code_blocks(root, file, snippets.raw());
        doc_node_free_tree(root);
    }
    const auto num_blocks = snippets.size();
    for (const auto& file : scd_files) {
        auto text = read_file(file);
        if (!text) {
            std::cout << display(file, relative_to) << ": can't be read\n";
            ++failures;
            continue;
        }
        const auto own = file_version(file, *text);
        (void)snippets.push_back({ file, std::move(*text), 1, own, true, false, Check::Compiles });
    }

    LibrarySources library_sources;
    for (const auto& file : find_library_files(*library_root)) {
        auto text = read_file(file);
        if (!text) {
            std::cout << "ERROR: can't read " << display(file, relative_to) << "\n";
            return EXIT_FAILURE;
        }
        (void)library_sources.push_back(
            { std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource { std::move(*text) },
                                                  sc::src::FileCodeLocation {}, file, sc::src::TextKind::ClassFile),
              sc::ir::ModuleIndex { 0 } });
    }
    const auto library = sc::compiler::compile_class_library(library_sources, num_threads, {});
    std::cout << library->parse_log;
    std::size_t library_errors = 0;
    for (const auto* diagnostics : { &library->parse_diagnostics, &library->diagnostics })
        for (const auto& d : *diagnostics)
            if (d.severity() == sc::diag::Severity::Error) {
                std::cout << d.to_string(relative_to) << "\n";
                ++library_errors;
            }
    if (library_errors) {
        std::cout << library_errors << " errors in the class library, so the docs weren't checked.\n";
        return EXIT_FAILURE;
    }

    sc::util::IndexedVector<SnippetIndex, std::optional<Failure>> results;
    results.resize(snippets.size());
    {
        boost::asio::thread_pool pool { num_threads };
        for (const auto j : snippets.indices())
            boost::asio::post(pool, [&, j] {
                try {
                    const auto& s = snippets[j];
                    if (s.check == Check::Parses) {
                        results[j] = parse_fragment(s, s.own, relative_to);
                    } else if (s.class_code) {
                        results[j] = compile_class(library_sources, s, s.own, relative_to);
                    } else {
                        results[j] = compile(*library, s, relative_to);
                        if (results[j] && s.check == Check::Compiles)
                            results[j]->looks_like_class_code = looks_like_class_code(s);
                    }
                    if (s.check == Check::Fails) {
                        if (!results[j])
                            results[j] = Failure { "compiles with no errors, but is marked code#error::\n" };
                        else if (!results[j]->unsupported)
                            results[j].reset();
                    }
                } catch (const std::exception& e) { results[j] = Failure { std::string { e.what() } + "\n" }; }
            });
        pool.join();
    }

    for (const auto j : snippets.indices()) {
        if (!results[j])
            continue;
        const auto& s = snippets[j];
        std::cout << display(s.file, relative_to) << ":" << s.line << ": " << sc::ast::to_string(s.own) << ": "
                  << (results[j]->looks_like_class_code ? "parses as class code, so mark it code#class::" : "")
                  << results[j]->first_error << "\n";
        ++failures;
    }
    const std::chrono::duration<double> seconds = std::chrono::steady_clock::now() - start_time;
    std::cout << help_files.size() << " help files, " << num_blocks << " code blocks, " << scd_files.size()
              << " scd files, " << failures << " failures, " << seconds.count() << " s.\n";
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
