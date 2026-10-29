#define BOOST_TEST_MODULE sc_diagnostic_tests
#include "sc_diagnostic/highlight.hpp"
#include "sc_source/text_location.hpp"
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include <string>

using namespace sc::diag;
using namespace sc::src;

namespace {

// The range covering the first occurrence of `needle`.
SourceCodeRange find(const std::string& text, const std::string& needle, size_t from = 0) {
    const auto begin = text.find(needle, from);
    BOOST_REQUIRE(begin != std::string::npos);
    const auto locate = [&](size_t abs) {
        SourceCodeLocation l;
        l.absolute = static_cast<std::uint32_t>(abs);
        for (size_t i = 0; i < abs; ++i) {
            if (text[i] == '\n') {
                l.line_number += 1;
                l.column = 0;
            } else
                l.column += 1;
        }
        return l;
    };
    return { locate(begin), locate(begin + needle.size()) };
}

std::string render(const std::string& text, SourceCodeRange range, std::string_view msg) {
    std::stringstream ss;
    streamHighlight(ss, { nullptr, text, range, {}, msg });
    return ss.str();
}

using Marks = std::vector<std::pair<SourceCodeRange, std::string_view>>;

std::string render_all(const std::string& text, const Marks& marks, const std::filesystem::path* path = nullptr) {
    std::vector<Highlight> hs;
    for (const auto& [range, msg] : marks)
        hs.push_back({ path, text, range, {}, msg });
    std::stringstream ss;
    streamHighlights(ss, hs);
    return ss.str();
}

const std::string text = "a = 1;\nfoo {\n  bar;\n  baz;\n}\nb = 2;\n";
const std::string tabbed = "\tx = \"é∑\" + y;";
const std::string same = "numbers { ^1 2 }\n";
const std::string longer = [] {
    std::string out;
    for (int i = 0; i < 20; ++i)
        out += "line " + std::to_string(i) + "\n";
    return out;
}();

}


BOOST_AUTO_TEST_CASE(single_line) {
    BOOST_TEST(render(text, find(text, "bar"), "single") ==
               R"(unknown file:3:3
    1 │ a = 1;
    2 │ foo {
    3 │   bar;
      ┆   ^^^ single
    4 │   baz;
    5 │ }
    6 │ b = 2;
)");
}

BOOST_AUTO_TEST_CASE(multi_line) {
    BOOST_TEST(render(text, find(text, "foo {\n  bar;\n  baz;\n}"), "multi") ==
               R"(unknown file:2:1
    1 │   a = 1;
    2 │ ╭ foo {
    3 │ │   bar;
    4 │ │   baz;
    5 │ │ }
      ┆ ╰─^ multi
    6 │   b = 2;
)");
}

BOOST_AUTO_TEST_CASE(multi_line_ending_at_start_of_line) {
    BOOST_TEST(render(text, find(text, "foo {\n  bar;\n  baz;\n}\n"), "ends at line start") ==
               R"(unknown file:2:1
    1 │   a = 1;
    2 │ ╭ foo {
    3 │ │   bar;
    4 │ │   baz;
    5 │ │ }
      ┆ ╰─^ ends at line start
    6 │   b = 2;
)");
}

BOOST_AUTO_TEST_CASE(tabs_and_wide_characters) {
    BOOST_TEST(render(tabbed, find(tabbed, "+ y"), "unicode") ==
               R"(unknown file:1:14
    1 │ 	x = "é∑" + y;
      ┆ 	         ^^^ unicode
)");
}

BOOST_AUTO_TEST_CASE(two_on_one_line) {
    BOOST_TEST(render_all(same, { { find(same, "1"), "ends" }, { find(same, "2"), "starts" } }) ==
               R"(unknown file:1:14
    1 │ numbers { ^1 2 }
      ┆            ^ ^ starts
      ┆            └ ends
)");
}

BOOST_AUTO_TEST_CASE(three_on_one_line) {
    BOOST_TEST(render_all(same, { { find(same, "1"), "A" }, { find(same, "2"), "B" }, { find(same, "}"), "C" } }) ==
               R"(unknown file:1:16
    1 │ numbers { ^1 2 }
      ┆            ^ ^ ^ C
      ┆            │ └ B
      ┆            └ A
)");
}

BOOST_AUTO_TEST_CASE(nearby_lines_share_source) {
    BOOST_TEST(render_all(text, { { find(text, "a ="), "first" }, { find(text, "baz"), "second" } }) ==
               R"(unknown file:4:3
    1 │ a = 1;
      ┆ ^^^ first
    2 │ foo {
    3 │   bar;
    4 │   baz;
      ┆   ^^^ second
    5 │ }
    6 │ b = 2;
)");
}

BOOST_AUTO_TEST_CASE(distant_lines_are_separate) {
    BOOST_TEST(render_all(longer, { { find(longer, "line 2"), "near" }, { find(longer, "line 17"), "far" } }) ==
               R"(unknown file:3:1
    1 │ line 0
    2 │ line 1
    3 │ line 2
      ┆ ^^^^^^ near
    4 │ line 3
    5 │ line 4
    6 │ line 5

unknown file:18:1
   16 │ line 15
   17 │ line 16
   18 │ line 17
      ┆ ^^^^^^^ far
   19 │ line 18
   20 │ line 19
)");
}

BOOST_AUTO_TEST_CASE(long_span_is_elided) {
    BOOST_TEST(render(longer, find(longer, "line 4\nline 5\nline 6\nline 7\nline 8\nline 9\nline 10"), "long") ==
               R"(unknown file:5:1
    3 │   line 2
    4 │   line 3
    5 │ ╭ line 4
    6 │ │ line 5
  ... │ │ 
   10 │ │ line 9
   11 │ │ line 10
      ┆ ╰───────^ long
   12 │   line 11
   13 │   line 12
   14 │   line 13
)");
}

BOOST_AUTO_TEST_CASE(nested_multi_line) {
    BOOST_TEST(render_all(longer,
                          { { find(longer, "line 3\nline 4\nline 5\nline 6\nline 7"), "outer" },
                            { find(longer, "line 4\nline 5"), "inner" } })
               ==
               R"(unknown file:5:1
    2 │     line 1
    3 │     line 2
    4 │ ╭   line 3
    5 │ │ ╭ line 4
    6 │ │ │ line 5
      ┆ │ ╰──────^ inner
    7 │ │   line 6
    8 │ │   line 7
      ┆ ╰────────^ outer
    9 │     line 8
   10 │     line 9
   11 │     line 10
)");
}

BOOST_AUTO_TEST_CASE(header_uses_the_final_highlight_and_text_offset) {
    const std::filesystem::path path { "/dir/file.sc" };
    std::stringstream ss;
    streamHighlights(ss,
                     { { &path, same, find(same, "1"), { 0, 10, 4 }, "first" },
                       { &path, same, find(same, "2"), { 0, 10, 4 }, "last" } });
    BOOST_TEST(ss.str().substr(0, ss.str().find('\n')) == "file:///dir/file.sc:11:" + std::to_string(4 + 13 + 1));
}

BOOST_AUTO_TEST_CASE(relative_header) {
    const std::filesystem::path path { "/dir/sub/file.sc" }, root { "/dir" };
    std::stringstream ss;
    streamHighlight(ss, { &path, same, find(same, "1"), {}, "m" }, false, {}, &root);
    BOOST_TEST(ss.str() == "sub/file.sc:1:12\n");
}

BOOST_AUTO_TEST_CASE(different_texts_do_not_share) {
    const std::string other = same;
    std::stringstream ss;
    streamHighlights(ss,
                     { { nullptr, same, find(same, "1"), {}, "a" }, { nullptr, other, find(other, "2"), {}, "b" } });
    BOOST_TEST(ss.str() == R"(unknown file:1:12
    1 │ numbers { ^1 2 }
      ┆            ^ a

unknown file:1:14
    1 │ numbers { ^1 2 }
      ┆              ^ b
)");
}

BOOST_AUTO_TEST_CASE(no_text_prints_the_description) {
    std::stringstream ss;
    streamHighlight(ss, { nullptr, {}, {}, {}, "nothing to show" }, true, "> ");
    BOOST_TEST(ss.str() == "unknown file:1:1\n> nothing to show");
}

BOOST_AUTO_TEST_CASE(line_prefix_begins_every_line_after_the_first) {
    std::stringstream ss;
    streamHighlight(ss, { nullptr, same, find(same, "1"), {}, "m" }, true, "| ");
    BOOST_TEST(ss.str() == R"(unknown file:1:12
|     1 │ numbers { ^1 2 }
|       ┆            ^ m
)");
}

BOOST_AUTO_TEST_CASE(highlights_are_listed_by_position_but_the_last_given_is_the_location) {
    BOOST_TEST(render_all(text, { { find(text, "baz"), "given first" }, { find(text, "a ="), "given last" } })
               == R"(unknown file:1:1
    1 │ a = 1;
      ┆ ^^^ given last
    2 │ foo {
    3 │   bar;
    4 │   baz;
      ┆   ^^^ given first
    5 │ }
    6 │ b = 2;
)");
}

BOOST_AUTO_TEST_CASE(distant_highlights_are_listed_by_position) {
    const auto out = render_all(longer, { { find(longer, "line 17"), "late" }, { find(longer, "line 2"), "early" } });
    BOOST_TEST(out.find("early") < out.find("late"));
    BOOST_TEST(out.substr(0, out.find('\n')) == "unknown file:3:1");
}
