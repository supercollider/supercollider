// Copyright Jordan Henderson 2026
// sc_format: formats SuperCollider code, see ../README.md.

#include "sc_format/format.hpp"
#include "sc_source/details/codepoint.hpp"
#include "sc_source/normalise_source.hpp"

#include <boost/program_options.hpp>

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <utility>
#include <variant>
#include <vector>
#ifdef _MSC_VER
#    include "SC_Codecvt.hpp"
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    include <cstdio>
#    include <fcntl.h>
#    include <io.h>
#    include <windows.h>
#endif

namespace fs = std::filesystem;
namespace po = boost::program_options;
using namespace sc::format;

namespace {

constexpr const char* usage_intro = R"(Usage: sc_format [options] [FILE|DIR]...

Formats SuperCollider code (.sc and .scd). Accepts a file, else reads from stdin.
Searches directories of format files.
Without one, every option is keep, except rewriting all newlines to '\n'.
The lines of what the parser skipped around a parse error are left as written.
)";

constexpr const char* usage_options_files = R"(
An options file is an event, e.g., (final_newline: \one, space: (after_comma: \one)). With inherit: true, its options
go on top of those of the nearest options file above it. An option named group.option is set in an event for the
group, or with --set space.after_comma=one. A group given one value, e.g., kr: \on, sets each of its options. Every
option is 'keep' by default.
)";

constexpr unsigned usage_line_length = 120;

// Writes a temporary file next to the file, then renames it over the file, so the file is never left part written.
bool write_file(const fs::path& path, const std::string& text) {
    auto temporary = path;
    temporary += ".sc-format-tmp";
    {
        std::ofstream file { temporary, std::ios::binary };
        file << text;
        file.close();
        if (!file) {
            std::error_code error;
            fs::remove(temporary, error);
            return false;
        }
    }
    std::error_code error;
    const auto status = fs::status(path, error);
    if (!error)
        fs::permissions(temporary, status.permissions(), error);
    fs::rename(temporary, path, error);
    if (error) {
        fs::remove(temporary, error);
        return false;
    }
    return true;
}

bool read_file(const fs::path& path, std::string& text) {
    std::ifstream file { path, std::ios::binary };
    if (!file)
        return false;
    std::stringstream contents;
    contents << file.rdbuf();
    text = contents.str();
    return true;
}

void add_source_files(const fs::path& path, std::vector<fs::path>& files) {
    std::error_code error;
    if (!fs::is_directory(path, error)) {
        files.push_back(path);
        return;
    }
    std::vector<fs::path> found;
    for (auto entry = fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied, error);
         entry != fs::recursive_directory_iterator(); entry.increment(error)) {
        if (error)
            break;
        const auto extension = entry->path().extension();
        if ((extension == ".sc" || extension == ".scd") && entry->is_regular_file(error))
            found.push_back(entry->path());
    }
    std::sort(found.begin(), found.end());
    files.insert(files.end(), found.begin(), found.end());
}

// Copies, turns all newlines in \n, then compares.
// Simply checks that 'nothing' has happened.
bool same_except_newlines(std::string_view original, std::string_view formatted) {
    const auto with_lf = [](std::string_view text) {
        const auto normalised = sc::src::NormalisedSource { std::string { text } }.as_string();
        std::string_view remaining { normalised };
        std::string with_newlines;
        with_newlines.reserve(remaining.size());
        while (!remaining.empty()) {
            const auto [code_point, size] = sc::src::utf8_sequence_to_codepoint(remaining.data(), remaining.size());
            const std::size_t length = code_point == sc::src::invalid_utf8_flag || size == 0 ? 1 : size;
            if (code_point != sc::src::invalid_utf8_flag && sc::src::is_newline(code_point))
                with_newlines += '\n';
            else
                with_newlines += remaining.substr(0, length);
            remaining.remove_prefix(length);
        }
        return with_newlines;
    };
    return with_lf(original) == with_lf(formatted);
}

// FIRST:LAST, counting from 1, as lines counting from 0.
std::optional<LineRange> line_range(std::string_view text) {
    const auto colon = text.find(':');
    if (colon == std::string_view::npos)
        return std::nullopt;
    const auto zero_based_line = [](std::string_view digits) -> std::optional<std::uint32_t> {
        std::uint32_t parsed { 0 };
        const auto [parsed_end, error] = std::from_chars(digits.data(), digits.data() + digits.size(), parsed);
        if (error != std::errc {} || parsed_end != digits.data() + digits.size() || parsed == 0)
            return std::nullopt;
        return parsed - 1;
    };
    const auto first_line = zero_based_line(text.substr(0, colon));
    const auto last_line = zero_based_line(text.substr(colon + 1));
    if (!first_line || !last_line || *last_line < *first_line)
        return std::nullopt;
    return LineRange { *first_line, *last_line };
}

std::string why_not(const Formatted& result) {
    if (const auto* parse_error = std::get_if<ParseError>(&result))
        return "doesn't parse, " + parse_error->message;
    return std::get<InternalError>(result).message;
}

// --set's values: a number, or a name, which can be written as a symbol.
using Settings = std::vector<std::pair<std::string, std::string>>;

OptionValue option_value(std::string_view text) {
    std::int32_t number { 0 };
    const auto [parsed_end, error] = std::from_chars(text.data(), text.data() + text.size(), number);
    if (error == std::errc {} && parsed_end == text.data() + text.size())
        return number;
    if (!text.empty() && text.front() == '\\')
        text.remove_prefix(1);
    return text;
}

// The styles read so far, by options file, so each is evaluated once.
using ReadStyles = std::map<fs::path, StyleOrError>;

// The style for a file, from --config or its options file, then --set. Reports why there is none.
std::optional<Style> style_for(const fs::path& path, const std::optional<Style>& config, const Settings& settings,
                               ReadStyles& read_styles) {
    std::optional<Style> style = config;
    if (!style) {
        const auto file = find_style_file(path);
        if (file) {
            auto cached = read_styles.find(*file);
            if (cached == read_styles.end())
                cached = read_styles.emplace(*file, read_style(*file)).first;
            if (const auto* error = std::get_if<std::string>(&cached->second)) {
                std::cerr << "sc_format: " << *error << "\n";
                return std::nullopt;
            }
            style = std::get<Style>(cached->second);
        } else {
            style = Style {};
        }
    }
    for (const auto& [key, value] : settings)
        (void)set_option(*style, key, option_value(value)); // Checked when read.
    return style;
}

std::string read_stdin() {
    std::stringstream input;
    input << std::cin.rdbuf();
    return input.str();
}

enum class Mode { Print, InPlace, Check, Verify };

// Exit statuses, the worst of all the files'.
constexpr int status_changed_or_failed = 1;
constexpr int status_error = 2;

struct Options {
    Mode mode { Mode::Print };
    bool allow_unformatted { false };
    std::optional<Style> config {};
    Settings settings {};
    bool from_stdin { true };
    std::vector<fs::path> files {}; // stdin's is `-` in the current directory, where its options file is found from
    std::optional<LineRange> lines {};
    Kind stdin_kind { Kind::Scd };
};

// None after printing why a setting is wrong.
std::optional<Settings> parse_settings(const std::vector<std::string>& setting_texts) {
    Settings settings {};
    for (const auto& setting : setting_texts) {
        const auto equals = setting.find('=');
        Style ignored;
        const auto problem = equals == std::string::npos
            ? std::optional<std::string> { "expected KEY=VALUE" }
            : set_option(ignored, setting.substr(0, equals), option_value(setting.substr(equals + 1)));
        if (problem) {
            std::cerr << "sc_format: --set " << setting << ": " << *problem << "\n";
            return std::nullopt;
        }
        settings.emplace_back(setting.substr(0, equals), setting.substr(equals + 1));
    }
    return settings;
}

// The arguments as given, before they are checked.
struct CommandLine {
    bool in_place { false };
    bool check { false };
    bool verify { false };
    bool allow_unformatted { false };
    std::optional<std::string> config_file {};
    std::optional<std::string> lines_text {};
    std::optional<std::string> kind_text {};
    std::vector<std::string> setting_texts {};
    std::vector<std::string> file_texts {};
};

// The command line, or the exit status if there's nothing to format: after --help, or an unknown option.
std::variant<CommandLine, int> parse_command_line(const std::vector<std::string>& arguments) {
    CommandLine command_line;
    std::string config_file, lines_text, kind_text;

    po::options_description visible("Options", usage_line_length, usage_line_length / 2);
    // clang-format off
    visible.add_options()
        ("help,h", "print this help.")
        (",i", po::bool_switch(&command_line.in_place), "rewrite the files in place.")
        ("allow-unformatted", po::bool_switch(&command_line.allow_unformatted),
            "report the files that don't parse or that the formatter fails on, and leave them as written, without "
            "exit code 2, e.g., for a pre-commit hook.")
        ("check", po::bool_switch(&command_line.check), "list the files that would change, exit code 1 if any would.")
        ("verify", po::bool_switch(&command_line.verify),
            "check that formatting twice changes nothing. exit code 1 if a file fails (files that don't parse are "
            "counted, not failed).")
        ("lines", po::value(&lines_text)->value_name("FIRST:LAST"),
            "change only these lines of stdin, counting from 1.")
        ("config", po::value(&config_file)->value_name("FILE"), "set options file.")
        ("set", po::value(&command_line.setting_texts)->value_name("KEY=VALUE"),
            "set an option, overrides the file, e.g., --set final_newline=one.")
        ("kind", po::value(&kind_text)->value_name("sc|scd"), "whether stdin is a class or document file.");
    po::options_description hidden;
    hidden.add_options()
        ("file", po::value(&command_line.file_texts));
    // clang-format on
    po::options_description all;
    all.add(visible).add(hidden);
    po::positional_options_description positional;
    positional.add("file", -1);

    po::variables_map parsed;
    try {
        const std::vector<std::string> options(arguments.begin() + (arguments.empty() ? 0 : 1), arguments.end());
        po::store(po::command_line_parser(options).options(all).positional(positional).run(), parsed);
        po::notify(parsed);
    } catch (const po::error& error) {
        std::cerr << "sc_format: " << error.what() << "\nRun sc_format --help for the options.\n";
        return status_error;
    }

    if (parsed.count("help")) {
        std::cout << usage_intro << "\n" << visible << usage_options_files << options_help();
        return 0;
    }
    if (parsed.count("config"))
        command_line.config_file = config_file;
    if (parsed.count("lines"))
        command_line.lines_text = lines_text;
    if (parsed.count("kind"))
        command_line.kind_text = kind_text;
    return command_line;
}

// The options, or the exit status after a usage error.
std::variant<Options, int> parse_options(const CommandLine& command_line) {
    Options options;
    if (command_line.in_place + command_line.check + command_line.verify > 1) {
        std::cerr << "sc_format: -i, --check and --verify can't be combined\n";
        return status_error;
    }
    if (command_line.in_place)
        options.mode = Mode::InPlace;
    else if (command_line.check)
        options.mode = Mode::Check;
    else if (command_line.verify)
        options.mode = Mode::Verify;
    options.allow_unformatted = command_line.allow_unformatted;

    if (command_line.config_file) {
        const auto read_config = read_style(fs::u8path(*command_line.config_file));
        if (const auto* error = std::get_if<std::string>(&read_config)) {
            std::cerr << "sc_format: " << *error << "\n";
            return status_error;
        }
        options.config = std::get<Style>(read_config);
    }

    auto settings = parse_settings(command_line.setting_texts);
    if (!settings)
        return status_error;
    options.settings = std::move(*settings);

    for (const auto& file : command_line.file_texts)
        add_source_files(fs::u8path(file), options.files);
    options.from_stdin = command_line.file_texts.empty();

    if (!options.from_stdin && (command_line.lines_text || command_line.kind_text)) {
        std::cerr << "sc_format: --lines and --kind are for stdin\n";
        return status_error;
    }
    if (options.from_stdin && options.mode == Mode::InPlace) {
        std::cerr << "sc_format: -i needs files\n";
        return status_error;
    }

    if (command_line.lines_text) {
        options.lines = line_range(*command_line.lines_text);
        if (!options.lines) {
            std::cerr << "sc_format: --lines is FIRST:LAST, counting from 1\n";
            return status_error;
        }
    }

    if (command_line.kind_text) {
        if (*command_line.kind_text != "sc" && *command_line.kind_text != "scd") {
            std::cerr << "sc_format: --kind is sc or scd\n";
            return status_error;
        }
        options.stdin_kind = *command_line.kind_text == "sc" ? Kind::Class : Kind::Scd;
    }

    if (options.from_stdin) {
        std::error_code error;
        options.files.push_back(fs::current_path(error) / "-");
    }
    return options;
}

struct VerifyCounts {
    std::size_t changed { 0 };
    std::size_t unchanged { 0 };
    std::size_t partly_parsed { 0 };
    std::size_t unparsed { 0 };
    std::size_t failed { 0 };
};

// None if the text passes. format() checks itself that the tokens are kept.
std::optional<std::string> verify_problem(const std::string& text, const std::string& formatted, Kind kind,
                                          const Style& style, std::optional<LineRange> lines) {
    const auto again = format(formatted, kind, style, lines);
    const auto* again_text = std::get_if<Text>(&again);
    if (!again_text)
        return "the result doesn't format: " + why_not(again);
    if (again_text->text != formatted)
        return "formatting twice changes it";
    if (style == Style {} && !same_except_newlines(text, formatted))
        return "more than line endings changed with every option keep";
    return std::nullopt;
}

// Returns the file's exit status.
int format_file(const Options& options, const fs::path& path, ReadStyles& read_styles, VerifyCounts& counts) {
    const auto mode = options.mode;
    const auto name = options.from_stdin ? std::string { "<stdin>" } : path.u8string();
    std::string text;
    if (options.from_stdin) {
        text = read_stdin();
    } else if (!read_file(path, text)) {
        std::cerr << "sc_format: can't read " << name << "\n";
        return status_error;
    }
    const auto style = style_for(path, options.config, options.settings, read_styles);
    if (!style) {
        if (mode == Mode::Print)
            std::cout << text;
        return status_error;
    }
    const auto kind = options.from_stdin ? options.stdin_kind : kind_of(path);
    const auto result = format(text, kind, *style, options.lines);
    const auto* formatted_text = std::get_if<Text>(&result);
    if (!formatted_text) {
        if (mode == Mode::Verify && std::holds_alternative<ParseError>(result)) {
            ++counts.unparsed;
            return 0;
        }
        std::cerr << name << ": not formatted, " << why_not(result) << "\n";
        if (mode == Mode::Print)
            std::cout << text;
        if (mode == Mode::Verify) {
            ++counts.failed;
            return status_changed_or_failed;
        }
        return options.allow_unformatted ? 0 : status_error;
    }
    const auto& formatted = formatted_text->text;
    if (formatted_text->parse_error) {
        if (mode == Mode::Verify)
            ++counts.partly_parsed;
        else
            std::cerr << name << ": left the code around a parse error as written, "
                      << formatted_text->parse_error->message << "\n";
    }
    const bool differs = formatted != text;
    switch (mode) {
    case Mode::Print:
        std::cout << formatted;
        return 0;
    case Mode::InPlace:
        if (differs && !write_file(path, formatted)) {
            std::cerr << "sc_format: can't write " << name << "\n";
            return status_error;
        }
        return 0;
    case Mode::Check:
        if (!differs)
            return 0;
        std::cout << name << "\n";
        return status_changed_or_failed;
    case Mode::Verify:
        if (const auto problem = verify_problem(text, formatted, kind, *style, options.lines)) {
            std::cerr << name << ": " << *problem << "\n";
            ++counts.failed;
            return status_changed_or_failed;
        }
        ++(differs ? counts.changed : counts.unchanged);
        return 0;
    }
    return 0;
}

// The arguments are UTF-8.
int run(const std::vector<std::string>& arguments) {
    const auto command_line = parse_command_line(arguments);
    if (const auto* status = std::get_if<int>(&command_line))
        return *status;
    const auto parsed = parse_options(std::get<CommandLine>(command_line));
    if (const auto* status = std::get_if<int>(&parsed))
        return *status;
    const auto& options = std::get<Options>(parsed);
    ReadStyles read_styles;
    VerifyCounts counts;
    int status = 0;
    for (const auto& path : options.files)
        status = std::max(status, format_file(options, path, read_styles, counts));
    if (options.mode == Mode::Verify)
        std::cout << options.files.size() << " files: " << counts.changed << " formatted, " << counts.unchanged
                  << " unchanged, " << counts.unparsed << " not parsed, " << counts.partly_parsed
                  << " formatted around parse errors, " << counts.failed << " failed\n";
    return status;
}
}

#ifdef _MSC_VER
int wmain(int argc, wchar_t* argv[]) {
    const UINT code_page = GetConsoleOutputCP();
    SetConsoleOutputCP(CP_UTF8);
    (void)_setmode(_fileno(stdin), _O_BINARY);
    (void)_setmode(_fileno(stdout), _O_BINARY);
    std::vector<std::string> utf8_arguments;
    for (int index = 0; index < argc; ++index)
        utf8_arguments.push_back(SC_Codecvt::utf16_to_utf8_str(argv[index]));
    const int status = run(utf8_arguments);
    std::cout.flush();
    if (code_page != 0)
        SetConsoleOutputCP(code_page);
    return status;
}
#else
int main(int argc, char** argv) { return run({ argv, argv + argc }); }
#endif
