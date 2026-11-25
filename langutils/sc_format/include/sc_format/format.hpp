// Copyright Jordan Henderson 2026
#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace sc::format {

////////////////////////////////////////////////////////////////////////////////
// API
////////////////////////////////////////////////////////////////////////////////

// Each option's first value is Keep, which leaves that part of the text as written.
enum struct FinalNewline : std::uint8_t { Keep, One };
enum struct TrailingWhitespace : std::uint8_t { Keep, Remove };
enum struct UnicodeSpaces : std::uint8_t { Keep, Ascii };
// With Keep, each line's indentation is as written, whatever the other indent options say.
struct Indent {
    enum struct Kind : std::uint8_t { Keep, Tab, Spaces };
    Kind kind { Kind::Keep };
    std::uint8_t spaces { 0 }; // a level's, with Kind::Spaces

    [[nodiscard]] bool operator==(const Indent& other) const { return kind == other.kind && spaces == other.spaces; }
    [[nodiscard]] bool operator!=(const Indent& other) const { return !(*this == other); }
};
// Keep indents a chain's line if it is indented in the text.
enum struct IndentMethodChains : std::uint8_t { Keep, On, Off };
// Keep indents a region's inside if it is indented in the text.
enum struct IndentRegions : std::uint8_t { Keep, On, Off };

enum struct RegionBrackets : std::uint8_t { Keep, OwnLines };
// The space between two tokens on the same line: one space, or none. Never next to a comment.
enum struct SpaceOne : std::uint8_t { Keep, One };
enum struct SpaceNone : std::uint8_t { Keep, None };
enum struct Space : std::uint8_t { Keep, One, None };
// Line structure. A switch is on or keep.
enum struct Switch : std::uint8_t { Keep, On };
// A list over several lines gets one element per line.
enum struct PerLine : std::uint8_t { Keep, One };
// `arg a;` to `|a|`, every list or only one on one line.
enum struct ParameterPipes : std::uint8_t { Keep, On, OneLine };
// The `;` after a body's last statement, never at the top level of an scd file.
enum struct FinalSemicolon : std::uint8_t { Keep, Remove };

enum struct RegionSemicolons : std::uint8_t { Keep, Insert };
// `f(a, { })` to `f(a) { }`, for control messages (`if`, `while`, `do`, `collect`...) or every message.
enum struct TrailingClosures : std::uint8_t { Keep, Control, All };
// A list ending in a comma goes over lines, one element per line or as many as each of its lines has.
enum struct TrailingCommaLists : std::uint8_t { Keep, Expand };

/// @brief Which options are on, read from an options file, see parse_style(). By default every option is Keep, so
/// formatting changes nothing but line endings, see format().
struct Style {
    FinalNewline final_newline { FinalNewline::Keep };
    TrailingWhitespace trailing_whitespace { TrailingWhitespace::Keep };
    std::optional<std::uint8_t> max_blank_lines {}; // Keep when empty.
    UnicodeSpaces unicode_spaces { UnicodeSpaces::Keep };
    Indent indent {};
    IndentMethodChains indent_method_chains { IndentMethodChains::Keep };
    IndentRegions indent_regions { IndentRegions::Keep };
    RegionBrackets region_brackets { RegionBrackets::Keep };
    SpaceOne space_around_binary_operators { SpaceOne::Keep };
    SpaceOne space_around_keyword_operators { SpaceOne::Keep };
    SpaceOne space_around_assignment { SpaceOne::Keep };
    SpaceOne space_after_comma { SpaceOne::Keep };
    SpaceNone space_before_comma { SpaceNone::Keep };
    SpaceNone space_before_semicolon { SpaceNone::Keep };
    Space space_inside_braces { Space::Keep };
    Space space_in_empty_braces { Space::Keep };
    Space space_before_parameters { Space::Keep };
    Space space_after_parameters { Space::Keep };
    SpaceNone space_inside_parens { SpaceNone::Keep };
    SpaceNone space_inside_brackets { SpaceNone::Keep };
    SpaceNone space_inside_pipes { SpaceNone::Keep };
    Space space_around_parameter_equals { Space::Keep };
    Space space_after_key { Space::Keep };
    SpaceNone space_around_dot { SpaceNone::Keep };
    Space space_before_control_paren { Space::Keep };
    Space space_before_closure { Space::Keep };
    Switch kr_braces { Switch::Keep };
    Switch kr_parens { Switch::Keep };
    Switch kr_brackets { Switch::Keep };
    Switch parameters_on_brace_line { Switch::Keep };
    PerLine multiline_arrays { PerLine::Keep };
    PerLine multiline_arguments { PerLine::Keep };
    PerLine multiline_control_pairs { PerLine::Keep }; // a `switch` or `case` over lines, each pair on its own
    ParameterPipes parameter_pipes { ParameterPipes::Keep };
    Switch parameter_commas { Switch::Keep };
    Switch parameter_default_equals { Switch::Keep };
    FinalSemicolon final_semicolon { FinalSemicolon::Keep };
    RegionSemicolons region_semicolons { RegionSemicolons::Keep };
    TrailingClosures trailing_closures { TrailingClosures::Keep };
    Switch control_prefix {
        Switch::Keep
    }; // `x.if { }` to `if(x) { }`, for `if`, `while`, `for`, `forBy`, `switch`, `case`, `try`
    TrailingCommaLists trailing_comma_lists { TrailingCommaLists::Keep };
    Switch align_columns { Switch::Keep }; // the rows of a list trailing_comma_lists expands

    [[nodiscard]] bool operator==(const Style& other) const;
    [[nodiscard]] bool operator!=(const Style& other) const { return !(*this == other); }
};

/// @brief What the text is, which decides how it parses.
enum struct Kind : std::uint8_t { Scd, Class };

/// @brief deduce kind from file extention.
[[nodiscard]] Kind kind_of(const std::filesystem::path& path);

/// @brief The code doesn't parse, so it isn't formatted. The message is the parser's first error.
struct ParseError {
    std::string message;
};

/// @brief A bug in the formatter, e.g., the result wouldn't have the code's tokens. Nothing is formatted.
struct InternalError {
    std::string message;
};

/// @brief Lines of the text, counting from zero, `last` included.
struct LineRange {
    std::uint32_t first;
    std::uint32_t last;
};

/// @brief Formatted code. Where the code has parse errors, the lines of what the parser skipped are left as written,
/// and the first error is kept.
struct Text {
    std::string text;
    std::optional<ParseError> parse_error;
    std::vector<LineRange> unparsed_lines; // of the code given, left as written around parse errors, in order
};

/// @brief The result of format(): the formatted text, or why there is none.
using Formatted = std::variant<Text, ParseError, InternalError>;

/// @brief Formats code with the options in @p style.
/// @param lines Only these lines change, e.g., an editor's selection. A rewrite or a line break that would change
/// another line isn't made.
[[nodiscard]] Formatted format(std::string_view text, Kind kind, const Style& style,
                               std::optional<LineRange> lines = std::nullopt);

/// @brief Whether the style turns on an option that changes tokens, e.g., `arg a;` to `|a|`.
[[nodiscard]] bool rewrites(const Style& style);

/// @brief The style with every option that changes tokens set to Keep, e.g., for an editor's Tab.
[[nodiscard]] Style without_rewrites(Style style);

/// @brief Whether two texts lex to the same tokens, ignoring whitespace, including whitespace at the end of a `//`
/// comment.
[[nodiscard]] bool same_tokens(std::string_view original, std::string_view formatted);

/// @brief Whether two texts have the same comments in the same order, as same_tokens() checks them. The rewrites
/// change tokens, so they are checked with this.
[[nodiscard]] bool same_comments(std::string_view original, std::string_view rewritten);

/// @brief A style, or why it couldn't be read.
using StyleOrError = std::variant<Style, std::string>;

/// @brief An option's value: a name, written as a symbol in an options file (`\one`), or a number.
using OptionValue = std::variant<std::string_view, std::int32_t>;

/// @brief Sets an option. Returns what is wrong if the option or its value is unknown.
[[nodiscard]] std::optional<std::string> set_option(Style& style, std::string_view option, OptionValue value);

/// @brief Every option and the values it takes besides keep, a line each, e.g., for a command's help.
[[nodiscard]] std::string options_help();

/// @brief An options file giving @p style: each option that isn't keep is set, the others are written commented out
/// with a value they can take, so a file can be made to be edited. parse_style() reads it as @p style.
[[nodiscard]] std::string options_file_text(const Style& style);

/// @brief The name of an options file, see find_style_file().
inline constexpr std::string_view style_file_name = ".sc-format.scd";

/// @brief A style from an options file's text: sclang code that evaluates to an event of options and their values,
/// as a `.quark` file does, e.g., `(final_newline: \one, max_blank_lines: 2)`. Options not given are Keep, and nil
/// (an empty file) is every option Keep. With `inherit: true`, the options go on top of those of the nearest options
/// file above @p path's directory, as clang-format's `InheritParentConfig` does.
/// @param path Where the text is from, for the errors.
[[nodiscard]] StyleOrError parse_style(std::string text, std::optional<std::filesystem::path> path = std::nullopt);

/// @brief Reads an options file, see parse_style().
[[nodiscard]] StyleOrError read_style(const std::filesystem::path& style_file);

/// @brief The options file for a file: the first `.sc-format.scd` in its directory or above it.
[[nodiscard]] std::optional<std::filesystem::path> find_style_file(const std::filesystem::path& file);

}
