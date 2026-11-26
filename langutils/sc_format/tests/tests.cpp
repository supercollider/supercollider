// Copyright Jordan Henderson 2026
#include "sc_format/format.hpp"

#define BOOST_TEST_MODULE sc_format_tests
#include <boost/test/included/unit_test.hpp>

#include <fstream>
#include <sstream>

using namespace sc::format;

namespace {

// The lexer's Unicode whitespace, in UTF-8.
const std::string nbsp = "\xC2\xA0";
const std::string em_space = "\xE2\x80\x83";
const std::string ideographic_space = "\xE3\x80\x80";
const std::string line_separator = "\xE2\x80\xA8";
const std::string next_line = "\xC2\x85";

std::string why_not(const Formatted& result) {
    if (const auto* error = std::get_if<ParseError>(&result))
        return "doesn't parse, " + error->message;
    if (const auto* error = std::get_if<InternalError>(&result))
        return error->message;
    return {};
}

// Formats, requiring a result that keeps the tokens (without rewrites) and doesn't change when formatted again.
std::string formatted(const std::string& text, const Style& style, Kind kind = Kind::Scd,
                      std::optional<LineRange> lines = std::nullopt) {
    const auto result = format(text, kind, style, lines);
    const auto* formatted_text = std::get_if<Text>(&result);
    BOOST_TEST_REQUIRE(formatted_text != nullptr, why_not(result));
    if (!rewrites(style))
        BOOST_TEST(same_tokens(text, formatted_text->text));
    const auto again = format(formatted_text->text, kind, style, lines);
    const auto* again_text = std::get_if<Text>(&again);
    BOOST_TEST_REQUIRE(again_text != nullptr, why_not(again));
    BOOST_TEST(again_text->text == formatted_text->text);
    return formatted_text->text;
}

// The first parse error of a result, which must have text.
std::string parse_error_of(const Formatted& result) {
    const auto* formatted_text = std::get_if<Text>(&result);
    BOOST_TEST_REQUIRE(formatted_text != nullptr, why_not(result));
    BOOST_TEST_REQUIRE(formatted_text->parse_error.has_value());
    return formatted_text->parse_error.value_or(ParseError {}).message;
}

Style style_of(const StyleOrError& style_or_error) {
    const auto* error = std::get_if<std::string>(&style_or_error);
    BOOST_TEST_REQUIRE(error == nullptr, (error ? *error : std::string {}));
    return std::get<Style>(style_or_error);
}

std::string error_of(const StyleOrError& style_or_error) {
    const auto* error = std::get_if<std::string>(&style_or_error);
    BOOST_TEST_REQUIRE(error != nullptr);
    return *error;
}

Style with(std::string_view key, OptionValue value) {
    Style style;
    const auto problem = set_option(style, key, value);
    BOOST_TEST_REQUIRE(!problem.has_value(), problem.value_or(""));
    return style;
}

Style all_on() {
    return style_of(parse_style("(final_newline: \\one, trailing_whitespace: \\remove, max_blank_lines: 2, "
                                "unicode_spaces: \\ascii)"));
}

}

// Formatting is tested by the files in input/ and expected/, see test.py. These are the characters that can't be seen
// in a file: line endings other than `\n`, and Unicode spaces and newlines.

BOOST_AUTO_TEST_CASE(keep_changes_nothing) {
    const auto text = "a" + nbsp + "=" + em_space + "1;\nb = \"" + line_separator + "\";" + ideographic_space;
    BOOST_TEST(formatted(text, Style {}) == text);
}

// As the compiler reads them, and between tokens the Unicode newlines too, but not inside a string.
BOOST_AUTO_TEST_CASE(line_endings_become_newlines) {
    BOOST_TEST(formatted("a = 1;\r\nb = \"x\r\ny\";\r\n\r\n", Style {}) == "a = 1;\nb = \"x\ny\";\n\n");
    BOOST_TEST(formatted("a = 1;\rb = 2;\r\n\vc = 3;\f", Style {}) == "a = 1;\nb = 2;\n\nc = 3;\n");
    BOOST_TEST(formatted("a;" + line_separator + next_line + "b; // c" + line_separator, Style {})
               == "a;\n\nb; // c\n");
    BOOST_TEST(formatted("Foo {\r\n}\r\n", Style {}, Kind::Class) == "Foo {\n}\n");
    // A quoted symbol is the same: a newline in one is escaped.
    const auto symbol = "x = 'a\\" + line_separator + "b';\n";
    BOOST_TEST(formatted(symbol, Style {}) == symbol);
    BOOST_TEST(formatted("y = 'c\\\r\nd';", Style {}) == "y = 'c\\\nd';");
    BOOST_TEST(formatted("a;\r\nb;\r\n\r\n", with("final_newline", "one")) == "a;\nb;\n");
    BOOST_TEST(formatted("a;  \r\nb;  \r\n", with("trailing_whitespace", "remove")) == "a;\nb;\n");
    BOOST_TEST(formatted("a;" + line_separator + line_separator + "\r\n\r\nb;", with("max_blank_lines", 0))
               == "a;\nb;");
}

BOOST_AUTO_TEST_CASE(unicode_spaces) {
    const auto ascii = with("unicode_spaces", "ascii");
    BOOST_TEST(formatted("a" + nbsp + "=" + em_space + "1;" + ideographic_space + "\n", ascii) == "a = 1; \n");
    // Never inside a string or comment, but the whitespace at the end of a `//` comment isn't the comment's.
    const auto inside = "x = \"" + nbsp + line_separator + "\"; /*" + nbsp + "*/ // " + nbsp + "b\n";
    BOOST_TEST(formatted(inside, ascii) == inside);
    BOOST_TEST(formatted("// c" + nbsp + "\n", ascii) == "// c \n");
    BOOST_TEST(formatted("a;" + nbsp + "\n" + ideographic_space, with("trailing_whitespace", "remove")) == "a;\n");
    BOOST_TEST(
        formatted("\n\n\n\na" + nbsp + "= 1;  // c  \n\n\n\n\nb = 2;" + line_separator + line_separator, all_on())
        == "\n\na = 1;  // c\n\n\nb = 2;\n");
}

// The part around a parse error is left as written, see input/parse_errors/, and the first error is kept.
BOOST_AUTO_TEST_CASE(parse_errors_are_kept) {
    BOOST_TEST(parse_error_of(format("x = (1;  \n", Kind::Scd, all_on())).rfind("line 1: ", 0) == 0);
    BOOST_TEST(formatted("x = (1;  \n", all_on()) == "x = (1;  \n");
    // The lines left as written, e.g., for an editor to indent them another way.
    const auto result = format("a = 1;  \n(\nx = [1;\n)\nb = 2;  \n", Kind::Scd, all_on());
    const auto* formatted_text = std::get_if<Text>(&result);
    BOOST_TEST_REQUIRE(formatted_text != nullptr, why_not(result));
    BOOST_TEST_REQUIRE(!formatted_text->unparsed_lines.empty());
    BOOST_TEST(formatted_text->unparsed_lines.front().first >= 1);
    BOOST_TEST(formatted_text->unparsed_lines.back().last <= 3);
    BOOST_TEST(formatted_text->text.rfind("a = 1;\n", 0) == 0);
    const auto clean = format("a = 1;\n", Kind::Scd, all_on());
    BOOST_TEST(std::get<Text>(clean).unparsed_lines.empty());
}

// Only the selected lines change: rewrites and line breaks that would change another line aren't made.
BOOST_AUTO_TEST_CASE(line_ranges) {
    const auto style =
        style_of(parse_style("(indent: \\tab, trailing_whitespace: \\remove, space: (around_assignment: \\one), "
                             "parameters: (pipes: \\on), trailing_closures: \\all, kr: (braces: \\on))"));
    const std::string text = "f = {\narg a;\nx=1;\ny=2;\n};\n";
    const auto lines = [&](std::uint32_t first_line, std::uint32_t last_line) {
        return formatted(text, style, Kind::Scd, LineRange { first_line, last_line });
    };
    BOOST_TEST(lines(2, 2) == "f = {\narg a;\n\tx = 1;\ny=2;\n};\n");
    BOOST_TEST(lines(1, 2) == "f = {\n\t|a|\n\tx = 1;\ny=2;\n};\n");
    BOOST_TEST(lines(0, 99) == formatted(text, style));
    BOOST_TEST(lines(9, 9) == text);
    // The end of the last line selected, not of the line before the first.
    BOOST_TEST(formatted("a=1;  \nb=2;  \nc=3;  \n", style, Kind::Scd, LineRange { 1, 1 })
               == "a=1;  \nb = 2;\nc=3;  \n");
    // A rewrite or a join that would change a line outside isn't made.
    const std::string closure = "x.do({ |i|\ni })\n";
    BOOST_TEST(formatted(closure, style, Kind::Scd, LineRange { 0, 0 }) == closure);
    BOOST_TEST(formatted(closure, style) == "x.do { |i|\n\ti }\n");
    const std::string brace = "f =\n{\n1 }\n";
    BOOST_TEST(formatted(brace, style, Kind::Scd, LineRange { 1, 2 }) == "f =\n{\n\t1 }\n");
    BOOST_TEST(formatted(brace, style, Kind::Scd, LineRange { 0, 2 }) == "f = {\n\t1 }\n");
    // The space before a string or comment that runs into a line outside is between two pieces, not at a line's end.
    BOOST_TEST(formatted("f = {\nvar x = \"multi\nline\";\nx };\n", style, Kind::Scd, LineRange { 1, 1 })
               == "f = {\n\tvar x = \"multi\nline\";\nx };\n");
    BOOST_TEST(formatted("f = {\nx / /* a\nb */ 2 };\n", style, Kind::Scd, LineRange { 1, 1 })
               == "f = {\n\tx / /* a\nb */ 2 };\n");
}

// An empty `arg;` after a `//` comment goes without the comment's newline.
BOOST_AUTO_TEST_CASE(empty_parameters_after_comment) {
    BOOST_TEST(formatted("f = { // note\n\targ; 1 };\n", with("parameters.pipes", "on")) == "f = { // note\n\t1 };\n");
}

BOOST_AUTO_TEST_CASE(same_tokens_ignores_whitespace) {
    BOOST_TEST(same_tokens("a=1;\n", "a = 1 ;"));
    BOOST_TEST(same_tokens("a; // c  \n", "a; // c\n"));
    BOOST_TEST(same_tokens("a;\r\nb;", "a;\nb;"));
    BOOST_TEST(!same_tokens("a = 1;", "a = 2;"));
    BOOST_TEST(!same_tokens("a = 1;", "a = 1"));
    BOOST_TEST(!same_tokens("\"a \"", "\"a\""));
    BOOST_TEST(!same_tokens("/* a */", "/* a*/"));
    BOOST_TEST(!same_tokens("// a b", "// ab"));
}

// The rewrites change code, never comments: their text, their kind, or their order.
BOOST_AUTO_TEST_CASE(same_comments_ignores_code) {
    BOOST_TEST(same_comments("x.if({ 1; }) /* a */; // b\n", "if(x) { 1 } /* a */; // b  \n"));
    BOOST_TEST(same_comments("arg a /* a */, b;", "|a /* a */, b|"));
    BOOST_TEST(same_comments("a;\r\n// c\r\n", "a;\n// c\n"));
    BOOST_TEST(same_comments("a = 1;", "|a| 1"));
    BOOST_TEST(!same_comments("a; /* c */", "a;"));
    BOOST_TEST(!same_comments("a;", "a; // c"));
    BOOST_TEST(!same_comments("/* a */ /* b */", "/* b */ /* a */"));
    BOOST_TEST(!same_comments("/* a */ // b\n", "// b\n/* a */"));
    BOOST_TEST(!same_comments("/* a */", "/* a */ /* a */"));
    BOOST_TEST(!same_comments("/* a */", "// a"));
    BOOST_TEST(!same_comments("// a b", "// ab"));
    BOOST_TEST(!same_comments("/* a /* b */ */", "/* a */"));
    // A comment's text inside a string isn't a comment.
    BOOST_TEST(!same_comments("\"x\" /* c */", "\"x /* c */\""));
}

// A file's own empty `arg;` makes it a function, so it stays; a function's goes.
BOOST_AUTO_TEST_CASE(file_parameters_stay) {
    const auto pipes = with("parameters.pipes", "on");
    BOOST_TEST(formatted("arg; 1", pipes) == "arg; 1");
    BOOST_TEST(formatted("/* c */ arg;\n1", pipes) == "/* c */ arg;\n1");
    BOOST_TEST(formatted("arg x; x", pipes) == "|x| x");
    BOOST_TEST(formatted("f = { arg; 1 }", pipes) == "f = { 1 }");
}

BOOST_AUTO_TEST_CASE(options_file) {
    const auto parsed =
        style_of(parse_style("// a comment\n(\n\tfinal_newline: \\one,\n\tmax_blank_lines: 1 + 2,\n)\n"));
    BOOST_TEST((parsed.final_newline == FinalNewline::One));
    BOOST_TEST((parsed.trailing_whitespace == TrailingWhitespace::Keep));
    BOOST_TEST((parsed.max_blank_lines == std::optional<std::uint8_t> { 3 }));
    BOOST_TEST((style_of(parse_style("()")) == Style {}));
    BOOST_TEST((style_of(parse_style("(max_blank_lines: 2, max_blank_lines: \\keep)")) == Style {}));
    // Nil, e.g., an empty file, is every option keep too.
    BOOST_TEST((style_of(parse_style("")) == Style {}));
    BOOST_TEST((style_of(parse_style("// only a comment\n")) == Style {}));
    BOOST_TEST((style_of(parse_style("nil")) == Style {}));

    const auto error = [](std::string text) { return error_of(parse_style(std::move(text), "a/.sc-format.scd")); };
    BOOST_TEST(error("(final_newline: \\one, final_newlines: \\one)")
               == "a/.sc-format.scd: unknown option `final_newlines`");
    BOOST_TEST(error("(final_newline: \\two)") == "a/.sc-format.scd: final_newline: `two` isn't keep or one");
    BOOST_TEST(error("(final_newline: 1)") == "a/.sc-format.scd: final_newline: `1` isn't keep or one");
    BOOST_TEST(error("(final_newline: \"one\")")
               == "a/.sc-format.scd: final_newline: the value isn't a symbol or an integer");
    BOOST_TEST(error("(max_blank_lines: 256)")
               == "a/.sc-format.scd: max_blank_lines: `256` isn't keep or a number up to 255");
    BOOST_TEST(error("(max_blank_lines: -1)")
               == "a/.sc-format.scd: max_blank_lines: `-1` isn't keep or a number up to 255");
    BOOST_TEST(error("(max_blank_lines: 2.5)")
               == "a/.sc-format.scd: max_blank_lines: the value isn't a symbol or an integer");
    const std::string example = ", e.g., `(final_newline: \\one, max_blank_lines: 2)`";
    BOOST_TEST(error("[1, 2]") == "a/.sc-format.scd: the options aren't an event" + example);
    BOOST_TEST(error("(max_blank_lines: 2.rand)") == "a/.sc-format.scd: the options aren't a constant" + example);
    // The compiler's errors, with their place in the file.
    const auto compile_error = error("(final_newline: one)");
    BOOST_TEST(compile_error.find("a/.sc-format.scd") != std::string::npos, compile_error);
    BOOST_TEST(compile_error.find("one") != std::string::npos, compile_error);

    BOOST_TEST(error("(inherit: 1)") == "a/.sc-format.scd: inherit: the value isn't true or false");
    BOOST_TEST(error("(indent: \\tabs)")
               == "a/.sc-format.scd: indent: `tabs` isn't keep, tab, or a number of spaces up to 255");
    BOOST_TEST((style_of(parse_style("(indent: 4)")).indent == Indent { Indent::Kind::Spaces, 4 }));
    BOOST_TEST((style_of(parse_style("(indent: \\tab)")).indent == Indent { Indent::Kind::Tab, 0 }));

    Style style;
    BOOST_TEST(set_option(style, "final_newline", 1).value_or("") == "final_newline: `1` isn't keep or one");
    BOOST_TEST(set_option(style, "nonsense", "tab").value_or("") == "unknown option `nonsense`");
}

// A group's options are in an event, e.g., `space: (after_comma: \one)`, or are all set by one value.
BOOST_AUTO_TEST_CASE(options_groups) {
    const auto grouped =
        style_of(parse_style("(space: (after_comma: \\one, inside_parens: \\none), kr: (braces: \\on))"));
    BOOST_TEST((grouped.space_after_comma == SpaceOne::One));
    BOOST_TEST((grouped.space_inside_parens == SpaceNone::None));
    BOOST_TEST((grouped.space_before_comma == SpaceNone::Keep));
    BOOST_TEST((grouped.kr_braces == Switch::On));
    BOOST_TEST((grouped.kr_parens == Switch::Keep));
    BOOST_TEST((style_of(parse_style("(space: ())")) == Style {}));

    const auto error = [](std::string text) { return error_of(parse_style(std::move(text), "a/.sc-format.scd")); };
    BOOST_TEST(error("(space: (after_commas: \\one))") == "a/.sc-format.scd: unknown option `space.after_commas`");
    BOOST_TEST(error("(spaces: (after_comma: \\one))") == "a/.sc-format.scd: unknown option group `spaces`");
    BOOST_TEST(error("(final_newline: (one: \\one))") == "a/.sc-format.scd: unknown option group `final_newline`");
    BOOST_TEST(error("(space: (after_comma: (a: 1)))")
               == "a/.sc-format.scd: space.after_comma: the value isn't a symbol or an integer");
    BOOST_TEST(error("(space: (after_comma: \\two))")
               == "a/.sc-format.scd: space.after_comma: `two` isn't keep or one");

    // A value every option in the group takes sets them all; another changes nothing.
    const auto kr = style_of(parse_style("(kr: \\on)"));
    BOOST_TEST((kr.kr_braces == Switch::On && kr.kr_parens == Switch::On && kr.kr_brackets == Switch::On));
    Style style { grouped };
    BOOST_TEST(set_option(style, "space", "one").value_or("") == "space: `one` isn't a value of every space option");
    BOOST_TEST((style == grouped));
    BOOST_TEST(!set_option(style, "space", "keep").has_value());
    BOOST_TEST((style.space_after_comma == SpaceOne::Keep && style.space_inside_parens == SpaceNone::Keep));
    BOOST_TEST((style.kr_braces == Switch::On));
    BOOST_TEST(!set_option(style, "space.after_comma", "one").has_value());
    BOOST_TEST((style.space_after_comma == SpaceOne::One));
}

// Every option options_help() lists can be set, and then the style isn't every option keep.
BOOST_AUTO_TEST_CASE(every_option) {
    std::istringstream lines { options_help() };
    std::size_t count { 0 };
    for (std::string line; std::getline(lines, line); ++count) {
        std::istringstream words { line };
        std::string key, value;
        words >> key >> value;
        if (!value.empty() && value.back() == ',')
            value.pop_back();
        Style style;
        const auto problem = value == "a" ? set_option(style, key, 1) : set_option(style, key, value);
        BOOST_TEST(!problem.has_value(), line << ": " << problem.value_or(""));
        BOOST_TEST((style != Style {}), line);
        BOOST_TEST(!set_option(style, key, "keep").has_value(), line);
        BOOST_TEST((style == Style {}), line);
    }
    BOOST_TEST(count > 0);
}

// options_file_text() reads back as its style: with every option keep, and with each set to its last value.
BOOST_AUTO_TEST_CASE(options_file_text_reads_back) {
    BOOST_TEST((style_of(parse_style(options_file_text(Style {}))) == Style {}));

    Style every;
    std::istringstream lines { options_help() };
    for (std::string line; std::getline(lines, line);) {
        std::istringstream words { line };
        std::string key, value, last;
        words >> key;
        while (words >> value)
            if (value != "or")
                last = value;
        if (!last.empty() && last.back() == ',')
            last.pop_back();
        const auto problem = last == "255" ? set_option(every, key, 3) : set_option(every, key, last);
        BOOST_TEST(!problem.has_value(), line << ": " << problem.value_or(""));
    }
    const auto file = options_file_text(every);
    BOOST_TEST((style_of(parse_style(file)) == every), file);
    BOOST_TEST(file.find("//") == file.find("// sc_format options"));
}

BOOST_AUTO_TEST_CASE(options_file_found_above) {
    namespace fs = std::filesystem;
    const auto root = fs::temp_directory_path() / "sc_format_tests_options_file";
    fs::remove_all(root);
    fs::create_directories(root / "a" / "b");
    const auto write = [](const fs::path& path, const std::string& text) { std::ofstream { path } << text; };
    const auto file = root / "a" / "b" / "x.scd";

    BOOST_TEST(!find_style_file(file).has_value());
    write(root / ".sc-format.scd", "(trailing_whitespace: \\remove)\n");
    BOOST_TEST((find_style_file(file) == root / ".sc-format.scd"));
    const auto found = style_of(read_style(root / ".sc-format.scd"));
    BOOST_TEST((found.trailing_whitespace == TrailingWhitespace::Remove));
    // The nearest file wins.
    write(root / "a" / ".sc-format.scd", "(nonsense: 1)\n");
    BOOST_TEST((find_style_file(file) == root / "a" / ".sc-format.scd"));
    BOOST_TEST(error_of(read_style(root / "a" / ".sc-format.scd"))
               == (root / "a" / ".sc-format.scd").u8string() + ": unknown option `nonsense`");
    BOOST_TEST(error_of(read_style(root / "missing.sc-format.scd"))
               == (root / "missing.sc-format.scd").u8string() + ": can't be read");
    fs::remove_all(root);
}

// With `inherit: true`, a file's options go on top of the nearest file above it, which can inherit too.
BOOST_AUTO_TEST_CASE(options_file_inherits) {
    namespace fs = std::filesystem;
    const auto root = fs::temp_directory_path() / "sc_format_tests_inherit";
    fs::remove_all(root);
    fs::create_directories(root / "a" / "b" / "c");
    const auto write = [](const fs::path& path, const std::string& text) { std::ofstream { path } << text; };
    const auto top_options = root / ".sc-format.scd";
    const auto a_options = root / "a" / ".sc-format.scd";
    const auto c_options = root / "a" / "b" / "c" / ".sc-format.scd";

    write(top_options, "(trailing_whitespace: \\remove, max_blank_lines: 1)");
    write(a_options, "(max_blank_lines: 3, inherit: true)");
    write(c_options, "(inherit: true, final_newline: \\one)");
    const auto from_a = style_of(read_style(a_options));
    BOOST_TEST((from_a.trailing_whitespace == TrailingWhitespace::Remove));
    BOOST_TEST((from_a.max_blank_lines == std::optional<std::uint8_t> { 3 }));
    // c's nearest above is a, as b has none, which inherits from the top.
    const auto from_c = style_of(read_style(c_options));
    BOOST_TEST((from_c.final_newline == FinalNewline::One));
    BOOST_TEST((from_c.trailing_whitespace == TrailingWhitespace::Remove));
    BOOST_TEST((from_c.max_blank_lines == std::optional<std::uint8_t> { 3 }));

    // Without inherit, or with nothing above, only the file's own options.
    write(a_options, "(max_blank_lines: 3, inherit: false)");
    BOOST_TEST((style_of(read_style(a_options)).trailing_whitespace == TrailingWhitespace::Keep));
    write(top_options, "(inherit: true, max_blank_lines: 1)");
    BOOST_TEST((style_of(read_style(top_options)).max_blank_lines == std::optional<std::uint8_t> { 1 }));

    // A group goes on top of the one above it option by option.
    write(top_options, "(space: (after_comma: \\one, inside_parens: \\none), kr: \\on)");
    write(a_options, "(inherit: true, space: (after_comma: \\keep), kr: (parens: \\keep))");
    const auto merged = style_of(read_style(a_options));
    BOOST_TEST((merged.space_after_comma == SpaceOne::Keep));
    BOOST_TEST((merged.space_inside_parens == SpaceNone::None));
    BOOST_TEST((merged.kr_braces == Switch::On && merged.kr_parens == Switch::Keep));

    // An error above is the file's error.
    write(a_options, "(inherit: true)");
    write(top_options, "(nonsense: 1)");
    BOOST_TEST(error_of(read_style(c_options)) == top_options.u8string() + ": unknown option `nonsense`");

    // A relative path finds the file above it, not itself.
    write(top_options, "(final_newline: \\one)");
    write(a_options, "(inherit: true, trailing_whitespace: \\remove)");
    const auto previous_directory = fs::current_path();
    fs::current_path(root / "a");
    const auto relative = style_of(read_style(".sc-format.scd"));
    const auto dotted = style_of(read_style("./.sc-format.scd"));
    fs::current_path(previous_directory);
    BOOST_TEST(
        (relative.final_newline == FinalNewline::One && relative.trailing_whitespace == TrailingWhitespace::Remove));
    BOOST_TEST((dotted == relative));
    fs::remove_all(root);
}
