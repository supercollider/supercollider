#include "sc_diagnostic/sc_diagnostic.hpp"
#include "sc_parser/ast.hpp"
#include "sc_parser/details/nodes.hpp"
#include "sc_source/normalise_source.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_lexer/lexer.hpp"
#include "sc_source/text_location.hpp"
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#define BOOST_TEST_MODULE sc_parser_tests
#include <boost/test/included/unit_test.hpp>

namespace P = sc::ast;

const auto tester_base = [](const char* src, bool is_class_file, bool no_diags) {
    std::cout << "\n--------------------------------\n src: ";
    std::cout << src << "\n";

    auto text_info = std::shared_ptr<sc::src::TextInfo>(new sc::src::TextInfo {
        sc::src::NormalisedSource(src),
        sc::src::FileCodeLocation {},
        "test_file",
        is_class_file ? sc::src::TextKind::ClassFile : sc::src::TextKind::ScdFile,
    });

    auto [graph, accepted] = P::parse(text_info);
    BOOST_TEST(accepted);

    // graph.flat_walk(
    //     [&](const Graph::Edges& e, const Graph::Variant& payload, const sc::src::SourceCodeRange& loc, size_t i) {
    //         std::cout << i << " " << Graph::node_name(payload);
    //         std::cout << " ";
    //         std::cout << "parent: " << *e.parent << " first_child " << *e.first_child << " next " << *e.next_sibling
    //         << " prev " << *e.prev_sibling; std::cout << std::endl;
    //     });

    if (no_diags) {
        BOOST_TEST(graph->diagnostics().empty());
    }

    const auto root = graph->root_any();
    BOOST_TEST(root.has_value());

    //    std::cout << "ROOT == " << **root << '\n';

    for (const auto& d : graph->diagnostics())
        std::cout << d << std::endl;

    graph->debug_df_print(std::cout);
};

const auto tester = [](const char* src, bool no_diags) { tester_base(src, false, no_diags); };

const auto tester_class = [](const char* src, bool no_diags) { tester_base(src, true, no_diags); };


BOOST_AUTO_TEST_CASE(literals) {
    tester("\n\n1.2 +.(1 + 1) ('a' *.meow b; 1 + 1.3323 - true ++ \\meow or: $a)\n\n", true);
}


BOOST_AUTO_TEST_CASE(expr) {
    tester("meow(1, 10, a: 20, *args)", true);
    tester("meow(*1, 10, a: 20, *a, *b, woof: 75)", true);

    tester("if(a) {1} {2}", true);

    tester("if(*a, foo, a: 10) { 'meow' } { 'woof' } ", true);

    tester("(+)(1, 2)", true);
    tester("(+)(1, 2) { 'meow'} { 'woof'} ", true);
    tester("(+)(1, hello: 2) { 'meow'} { 'woof'} ", true);
    tester("(+) { 'meow'} { 'woof'} ", true);


    tester("1.meow(arg: 1)", true);
    tester("1.meow", true);
    tester("1.if { 'true' } { 'false' }", true);
    tester("{}.if { 'true' } { 'false' }", true);
    tester("1.meow()", true);
    tester("1.meow() {}", true);
    tester("1.meow(deep: true) {}", true);


    tester("meow.()", true);
    tester("meow.() {'cat' } ", true);


    tester("WOOF.new()", true);
    tester("WOOF()", true);
    tester("WOOF{a} {b}", true);
    tester("WOOF(meow: cat) {a} {b}", true);
    tester("WOOF(meow: cat, *  ~  ar) {a} {b}", true);


    tester("~meow", true);

    tester("FOO", true);
    tester("`FOO", true);
    tester("FOO.[1]", true);

    tester("`a = b = 1 * 2", true);


    tester("~meow = ~foo = 1 + 1", true);


    tester("~foo.meow = 10", true);


    tester("foo(1) = 10", true);
    tester("foo(1, 2, 3, meow: 10, *a) = 10", true);


    // This is different from the original!
    // It would not allow you to pass nil
    tester("a[] = 1", true);
    // Nor would it allow you to use kwargs
    tester("a[meow: 10] = 1", true);

    tester("a[1] = 1", true);

    tester("a.[1] = 1", true);


    tester("[1, 2, 3]", true);
    tester("#[1, 2, 3]", true);


    tester("(a: 1)", true);
    tester("(a : 1)", true);
    tester("()", true);
    tester("(1 + 1: 2)", true);
    tester("(1 + 1 : 2)", true);

    tester("(1+1)", true);


    tester("Foo[1: 2]", true);

    tester("(1+1)[foo: 2]", true);
    tester("(a: 1, 'b' : meow)[2]", true);
    tester("foo. + 10 - ~meow", false);

    tester("{}", true);

    tester("{ arg a; }", true);
    tester("{ arg a = 1; }", true);
    tester("{ arg a(1); }", true);
    tester("{ arg a(1; 2); }", true);
    tester("{ arg a=((1; 2);4); }", true);

    tester("{ arg a=1, b(2), c=(d); }", true);


    tester("{ |a=1| }", true);
    tester("{ |a(1)| }", true);
    tester("{ |a 1| }", true);

    tester("{ |a=1,| }", true);
    tester("{ |a(1),| }", true);
    tester("{ |a 1,| }", true);

    tester("{ |a=1, b= 2, c=(d)| }", true);

    tester("{ |a=1, b= 2, c=(d)| 1 + 1 }", true);
    tester("{ |a=1, b= 2, c=(d)| 1 + 1; a; }", true);


    tester("{ |a=1, b= 2, c=(d)| ^1 + 1; ^a }", true);
    tester("{  ^1 + 1; ^a }", true);
    tester("{ |a| ^if(a) {~meow} {~woof} }", true);


    tester("{ |a=(1:2)| }", true);
    tester("{ |a (1:2)| }", true);
    tester("{ |a ()| }", true);

    tester("{ |a({})| }", true);


    tester("{ var a; }", true);
    tester("{ var a = 10; }", true);
    tester("{ var a = 10, b(20); }", true);
    tester("{ |b| 1 + 1; var a }", true);
}


BOOST_AUTO_TEST_CASE(classes) {
    tester_class("Foo {}", false);
    tester_class("+Foo {}", false);
    tester_class("+Foo{} Bar{} +Car{}", false);

    tester_class("Foo : Bar {}", false);

    tester_class("Foo[obj] : Bar {}", false);
    tester_class("Foo[obj] {}", false);


    tester_class("Object { var a; }", false);
    tester_class("Object { var a = 1; }", false);
    tester_class("Object { var <a = 1; }", false);
    tester_class("Object { var >a = 1; }", false);
    tester_class("Object { var <>a = 1; }", false);

    tester_class("Object { \n"
                 "   var <a, <>b, >c;\n"
                 "   classvar <a, <>b, >c;\n"
                 "   const <d, <>e, >f;\n"
                 "   foo { |a,b,c| ^a + b + c }\n"
                 "   *bar { |a,b,c| ^a + b + c }\n"
                 "   + { |a,b,c| ^a + b + c }\n"
                 "   - { |a,b,c| ^a + b + c }\n"

                 "   * + { |a,b,c| ^a + b + c }\n"
                 "   * - { |a,b,c| ^a + b + c }\n"
                 "}\n"
                 "\n"
                 "+Foo {}",
                 false);

    tester_class( //
        R"%(
Foo {
    var a;
    method {
            if (a) { meow} // yes, you don't need a semi-colon here
            ^10
    }
}
)%",
        true);
}

BOOST_AUTO_TEST_CASE(regions) {
    tester(
        R"%(
(
    1 + 1
)

2
3
)%",
        false);

    tester(
        R"%(
(
    1 + 3 +    
)

2
3
)%",
        false);

    tester(
        R"%(
3 * 2.pow(2)

(
"hello".postln
)


( 1 + 3 +    )

2
3
)%",
        false);


    tester(
        R"%(
a.foo;

(1, a: 10)

1 + 1
)%",
        false);


    tester("(+)()", false);
    tester("foo()", false);
    tester("(1+1).[]", true);
    tester("(1+1)[]", true);

    tester(
        R"%(
(
    1 + 2;
    "hello"
    "meow"
        .postln
)

[1, 2, 3];
)%",
        false);


    tester(" 1 + 2 (\n\n 4 * 4;", false);
    tester(" 1 + 2 (\n\n 4 * 12 );", false);


    tester("1 + \n(var x = 10; x * 2)", false);
    tester("(var x = 10; x * 2)", false);

    tester("(|a| var x = 10; x * 2)", true);

    tester("foo(this: 2, arg2: 10)", true);


    tester("{ var #a, b, c = array; }", true);
    tester("{ var #a, b, c = array, d = 10; }", true);
    tester("#~foo, rest = array;", true);


    tester("{: x + 1, x <- [1, 2, 3, 4], x.odd, ::x.postln, :while x < 3}", true);


    tester("", true);
    tester_class("", true);
    tester("var a; 10", true);
    // tester("var a;", true);
    tester("|a| var a;", true);
    tester("|a| 1 + a", true);


    tester("1.[1,2,3 .. 30]", true);
    tester("1.[1 .. 30]", true);

    tester("1.[start: 1 .. 30]", true);
    tester("1.[*a .. 30]", true);

    tester("1[1, 2]", true);

    tester("1[1, 2 .. 10]", true);
    tester("1[.. 10]", true);
    tester("1[1 .. 10]", true);
    tester("1[1 .. ]", true);

    // tester("(:..10)", true);


    tester("_", true);

    tester("_ + 1", true);

    tester("a[1..] = 10", true);
    tester("a[1, 2..] = 10", true);
    tester("a[1, 2.. 20] = 10", true);
    tester("a[1.. 20] = 10", true);
    tester("a[.. 20] = 10", true);


    tester_class(R"%(	
Foo {
    *new { arg addr, cmdPath, action;
		var cmdName, path;
		this.deprecated(thisMethod, OSCFunc.class.findMethod(\new));
		#cmdName ...path = cmdPath;
		^super.new(addr, cmdName, action).path_(path);
    }
}
    )%",
                 true);


    tester("60.midicps", true);

    tester_class(R"%(
AmpComp  {
	ir { arg freq = 60.midicps, fsdf = 20.midicps;
	}
}
    )%",
                 true);

    tester_class(
        R"%(
Foo {
    bar { 1 + }
}
    )%",
        false);

    tester_class(
        R"%(
Foo {
    bar { 1 + }
    car { ^meow }
}
    )%",
        false);

    tester_class(
        R"%(
Foo {
    bar { 1 + }
    car { if(a) {^meow}; ^10 }
}
    )%",
        false);

    tester_class(
        R"%(
Foo {
    bar { 1 + }
    car { if(a) {^meow}
}
    )%",
        false);
}

// A file in another encoding, here Mac Roman non-breaking spaces, is reported once at its first invalid byte.
BOOST_AUTO_TEST_CASE(invalid_utf8_reported_once) {
    auto text_info =
        std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource("a = 1;\n\xca\xca"
                                                                      "b = 2;\nc = \xca"
                                                                      "3;\n"),
                                            sc::src::FileCodeLocation {}, "test_file", sc::src::TextKind::ScdFile);
    auto [graph, result] = P::parse(text_info);
    BOOST_TEST_REQUIRE(graph->diagnostics().size() == 1);
    const auto& d = graph->diagnostics()[0];
    BOOST_TEST(std::string { d.name() } == "Invalid UTF-8.");
    BOOST_TEST(d.locations()[0].range.begin.line_number == 1);
    BOOST_TEST(d.locations()[0].msg.find("nor are 2 more") != std::string::npos);
}

namespace {
// Whether walking the tree from its root reaches no node twice, i.e., error recovery made no cycle.
bool is_a_tree(const P::ASTGraph& graph) {
    const auto root = graph.root_any();
    if (!root)
        return true;
    const auto before = [](P::Index a, P::Index b) { return *a < *b; }; // StrongIndex has no operator<
    std::set<P::Index, decltype(before)> seen { before };
    std::vector<P::Index> todo { *root };
    while (!todo.empty()) {
        const auto i = todo.back();
        todo.pop_back();
        if (!seen.insert(i).second)
            return false;
        for (auto c = graph.edges(i).first_child; c; c = graph.edges(P::Index { *c }).next_sibling)
            todo.push_back(P::Index { *c });
    }
    return true;
}

std::shared_ptr<P::ASTGraph> parse_class_file(const char* src) {
    auto text_info = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource(src), sc::src::FileCodeLocation {},
                                                         "test_file", sc::src::TextKind::ClassFile);
    return std::get<0>(P::parse(text_info));
}
}

// An error at the start of a body, after nodes with empty ranges (no parameters, no primitive), adopts only what it
// skipped, not those nodes, one of which then holds the error.
BOOST_AUTO_TEST_CASE(error_recovery_makes_a_tree) {
    const auto graph = parse_class_file("+ A {\n\tb { arg d;\n\t}\n\tc {\xca"
                                        "arg d, e=false;\n}\n\tf {\n\t\tg[h] = i;\n\t}\n}\n");
    BOOST_TEST_REQUIRE(graph != nullptr);
    BOOST_TEST(is_a_tree(*graph));
}

// Code between classes is an error item of the class list, and the classes around it are kept.
BOOST_AUTO_TEST_CASE(error_between_classes) {
    const auto graph = parse_class_file("A {\n\tb { ^1 }\n}\n(\nx = 1;\n)\n+ B {\n\tc { ^2 }\n}\nA.new;\n");
    BOOST_TEST_REQUIRE(graph != nullptr);
    BOOST_TEST(is_a_tree(*graph));
    const auto root = graph->root();
    BOOST_TEST_REQUIRE(root.has_value());
    const auto* list = std::get_if<P::ClassOrExtensionListIndex>(&*root);
    BOOST_TEST_REQUIRE(list != nullptr);
    std::vector<std::string> items;
    for (auto it = graph->children(*list).iter(); it; ++it)
        items.emplace_back(P::ASTGraph::node_name(graph->payload(*it)));
    BOOST_TEST((items == std::vector<std::string> { "Class", "Error", "ClassExtension", "Error" }),
               boost::test_tools::per_element());
}

namespace {
using V = P::LanguageVersion;

// The version of each class and extension, in order, and the names of the diagnostics.
void check_versions(const char* src, const std::vector<V>& versions, const std::vector<std::string>& diagnostics,
                    bool is_class_file = true) {
    auto text_info =
        std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource(src), sc::src::FileCodeLocation {}, "test_file",
                                            is_class_file ? sc::src::TextKind::ClassFile : sc::src::TextKind::ScdFile);
    auto [graph, result] = P::parse(text_info);

    std::vector<V> found_versions;
    if (const auto root = graph->root())
        if (const auto list = std::get_if<P::ClassOrExtensionListIndex>(&*root))
            for (auto it = graph->children(*list).iter(); it; ++it) {
                if (const auto c = graph->as_a<P::ClassIndex>(*it))
                    found_versions.push_back(graph->payload(*c).version);
                else if (const auto e = graph->as_a<P::ClassExtensionIndex>(*it))
                    found_versions.push_back(graph->payload(*e).version);
            }
    std::vector<std::string> found_diagnostics;
    for (const auto& d : graph->diagnostics())
        found_diagnostics.emplace_back(d.name());

    BOOST_TEST_INFO("src: " << src);
    BOOST_TEST((found_versions == versions));
    BOOST_TEST(found_diagnostics == diagnostics, boost::test_tools::per_element());
}
}

BOOST_AUTO_TEST_CASE(version_for_release) {
    BOOST_TEST((P::version_for_release(3, 13) == V::Legacy));
    BOOST_TEST((P::version_for_release(3, 16) == V::V3_16));
    BOOST_TEST(!P::version_for_release(3, 17).has_value());
    BOOST_TEST(!P::version_for_release(4, 0).has_value());
}

BOOST_AUTO_TEST_CASE(versions_placed) {
    check_versions("Foo {}", { V::Legacy }, {});
    check_versions("Foo {\n    //# VERSION 3.16\n    var a;\n}", { V::V3_16 }, {});
    check_versions("+ Foo {\n    //# VERSION 3.16\n    bar { ^1 }\n}", { V::V3_16 }, {});
    check_versions("Foo { //# VERSION 3.16\n}", { V::V3_16 }, {});
    // A release before the first that changed the rules.
    check_versions("Foo {\n    //# VERSION 3.13\n}", { V::Legacy }, {});

    // The file's is used by every class without its own.
    check_versions("//# VERSION 3.16\nFoo {}\n+ Bar {}\nCar {\n    //# VERSION 3.13\n}",
                   { V::V3_16, V::V3_16, V::Legacy }, {});
    check_versions("//# VERSION 3.13\nFoo {\n    //# VERSION 3.16\n}\nBar {}", { V::V3_16, V::Legacy }, {});
}

BOOST_AUTO_TEST_CASE(versions_invalid) {
    check_versions("//# VERSION legacy\nFoo {}\nBar {}", { V::Legacy, V::Legacy }, {});
    check_versions("Foo {\n    //# VERSION legacy \t\n}", { V::Legacy }, {});
    check_versions("//# VERSION 3.16\nFoo {\n    //# VERSION legacy\n}\nBar {}", { V::Legacy, V::V3_16 }, {});
    check_versions("Foo {\n    //# VERSION legacyx\n}", { V::Legacy }, { "Malformed version" });
    check_versions("Foo {\n    //# VERSION legacy 3.16\n}", { V::Legacy }, { "Malformed version" });
    check_versions("Foo {\n    //# VERSION 3\n}", { V::Legacy }, { "Malformed version" });
    check_versions("Foo {\n    //# VERSION 3.16x\n}", { V::Legacy }, { "Malformed version" });
    check_versions("Foo {\n    //# VERSION abc\n}", { V::Legacy }, { "Malformed version" });
    check_versions("Foo {\n    //# VERSION 3.16 // why\n}", { V::Legacy }, { "Malformed version" });
    check_versions("Foo {\n    //# VERSION 3.16 \t\n}", { V::V3_16 }, {});
    check_versions("Foo {\n    //# VERSION 4.0\n}", { V::Legacy }, { "Unknown version" });
    // A broken file version doesn't fall back to anything else.
    check_versions("//# VERSION 99.0\nFoo {}", { V::Legacy }, { "Unknown version" });
    // Malformed, but still in the class's place, so it doesn't use the file's.
    check_versions("//# VERSION 3.16\nFoo {\n    //# VERSION 3\n}", { V::Legacy }, { "Malformed version" });
}

BOOST_AUTO_TEST_CASE(versions_duplicate) {
    check_versions("Foo {\n    //# VERSION 3.13\n    //# VERSION 3.16\n}", { V::V3_16 }, { "Duplicate version" });
}

BOOST_AUTO_TEST_CASE(versions_misplaced) {
    check_versions("Foo {}\n//# VERSION 3.16\nBar {}", { V::Legacy, V::Legacy }, { "Misplaced version" });
    check_versions("Foo {}\n//# VERSION 3.16", { V::Legacy }, { "Misplaced version" });
    check_versions("Foo {\n    var a;\n    //# VERSION 3.16\n}", { V::Legacy }, { "Misplaced version" });
    check_versions("Foo {\n    bar {\n        //# VERSION 3.16\n        if (x) { ^1 }; ^2\n    }\n}", { V::Legacy },
                   { "Misplaced version" });
    check_versions("Foo : Object\n//# VERSION 3.16\n{}", { V::Legacy }, { "Misplaced version" });
    // Only the first line sets the file's version, not the first thing in it.
    check_versions("// Some licence.\n/* more */\n//# VERSION 3.16\nFoo {}", { V::Legacy }, { "Misplaced version" });
    check_versions("//# VERSION 3.16\n//# VERSION 3.13\nFoo {}", { V::V3_16 }, { "Misplaced version" });
    check_versions(" //# VERSION 3.16\nFoo {}", { V::Legacy }, { "Misplaced version" });
    check_versions("\n//# VERSION 3.16\nFoo {}", { V::Legacy }, { "Misplaced version" });
}

namespace {
// The version from the comment on the file's first line, if there is one, and the names of the diagnostics.
void check_file_version(const char* src, std::optional<std::optional<V>> version,
                        const std::vector<std::string>& diagnostics,
                        sc::src::TextKind kind = sc::src::TextKind::ScdFile, sc::src::FileCodeLocation start = {}) {
    auto text_info = std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource(src), start, "test_file", kind);
    auto [graph, result] = P::parse(text_info);
    const auto& found = graph->file_version();
    std::vector<std::string> found_diagnostics;
    for (const auto& d : graph->diagnostics())
        found_diagnostics.emplace_back(d.name());

    BOOST_TEST_INFO("src: " << src);
    BOOST_TEST(found.has_value() == version.has_value());
    if (found && version)
        BOOST_TEST((found->version == *version));
    BOOST_TEST(found_diagnostics == diagnostics, boost::test_tools::per_element());
}
}

BOOST_AUTO_TEST_CASE(versions_interpreted) {
    using sc::src::TextKind;
    check_file_version("//# VERSION 3.16\n1 + 1", V::V3_16, {});
    check_file_version("//# VERSION legacy\n1 + 1", V::Legacy, {});
    check_file_version("//# VERSION 3.16", V::V3_16, {});
    check_file_version("//# VERSION\t3.16 \t\n1", V::V3_16, {});
    check_file_version("//# VERSION 3.16\r\n1", V::V3_16, {});
    check_file_version("//# VERSION 3.16\n1", V::V3_16, {}, TextKind::InterpretedFunction);
    check_file_version("//# VERSION 3.16\nFoo {}", V::V3_16, {}, TextKind::ClassFile);
    check_file_version("1 + 1", std::nullopt, {});

    // Anywhere but the first line is misplaced, and later ones too.
    check_file_version("(\n//# VERSION 3.16\n1 + 1\n)", std::nullopt, { "Misplaced version" });
    check_file_version("1; //# VERSION 3.16\n2", std::nullopt, { "Misplaced version" });
    check_file_version("\n//# VERSION 3.16\n1", std::nullopt, { "Misplaced version" });
    check_file_version(" //# VERSION 3.16\n1", std::nullopt, { "Misplaced version" });
    check_file_version("//# VERSION 3.16\n//# VERSION legacy\n1", V::V3_16, { "Misplaced version" });

    // Text that starts part way into its file, e.g., a region an editor evaluates, doesn't hold the first line.
    check_file_version("//# VERSION 3.16\n1", std::nullopt, { "Misplaced version" }, TextKind::ScdFile,
                       sc::src::FileCodeLocation { 20, 2, 0 });

    // A broken comment is still the file's, so it isn't misplaced.
    check_file_version("//# VERSION latest\n1", std::optional<V> {}, { "Latest in a version comment" });
    check_file_version("//# VERSION latest 3.16\n1", std::optional<V> {}, { "Malformed version" });
    check_file_version("//# VERSION 3\n1", std::optional<V> {}, { "Malformed version" });
    check_file_version("//# VERSION 99.0\n1", std::optional<V> {}, { "Unknown version" });
}

// Each block's closed flag, in source order, `#{ }` is closed, `{ }` isn't.
BOOST_AUTO_TEST_CASE(closed_blocks) {
    auto text_info =
        std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource("#{ |a| { a } }; { #{ } }; f(#{ 1 }) { 2 }"),
                                            sc::src::FileCodeLocation {}, "test_file", sc::src::TextKind::ScdFile);
    auto [graph, result] = P::parse(text_info);
    BOOST_TEST(result);
    BOOST_TEST(graph->diagnostics().empty());

    std::vector<std::pair<std::size_t, bool>> blocks;
    graph->flat_walk([&](const auto&, const auto& payload, const sc::src::SourceCodeRange& loc, size_t) {
        if (const auto block = std::get_if<P::BlockNode>(&payload))
            blocks.emplace_back(loc.begin.absolute, block->closed);
    });
    std::sort(blocks.begin(), blocks.end());
    std::vector<bool> closed;
    for (const auto& [_, c] : blocks)
        closed.push_back(c);
    BOOST_TEST(closed == (std::vector<bool> { true, false, false, true, true, false }),
               boost::test_tools::per_element());
}

// The VM's interpreted code is one function, as the old grammar's cmdlinecode: a body, or a body with variables in
// brackets, whose variables are the function's own, and arguments, e.g., an scd file loaded with arguments. Without
// regions, a missing semicolon is an error.
BOOST_AUTO_TEST_CASE(interpreted_function) {
    const auto parse_function = [](const char* src) {
        auto text_info =
            std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource(src), sc::src::FileCodeLocation {},
                                                "test_file", sc::src::TextKind::InterpretedFunction);
        return std::get<0>(P::parse(text_info));
    };
    for (const char* src : { "", ";", "1", "a = 1; b = 2; a + b;", "var a = 1; a", "(var a = 1; a)", "( var a, b; )",
                             "(a; b)", "^1", "a = 1; ^a", "(\nvar a = 1;\na\n) // comment", "|a| a", "|a=1, b=2| a + b",
                             "arg a; a", "|a|", "(|a| var b = a; b)", "(|a|)" }) {
        BOOST_TEST_CONTEXT(src) {
            const auto graph = parse_function(src);
            BOOST_TEST_REQUIRE(graph);
            BOOST_TEST(graph->diagnostics().empty());
            const auto root = graph->root();
            BOOST_TEST_REQUIRE(root.has_value());
            BOOST_TEST(std::holds_alternative<P::FileAsFunctionIndex>(*root));
            bool immediately_evaluated = false;
            graph->flat_walk([&](const auto&, const auto& payload, const sc::src::SourceCodeRange&, size_t) {
                immediately_evaluated =
                    immediately_evaluated || std::holds_alternative<P::ImmediatelyEvaluatedBlock>(payload);
            });
            BOOST_TEST(!immediately_evaluated);
        }
    }
    for (const char* src : { "(var a = 1; a); 2", "(var a = 1; a) (var b = 2; b)", "a = 1\nb = 2" }) {
        BOOST_TEST_CONTEXT(src) {
            const auto graph = parse_function(src);
            BOOST_TEST(!(graph && graph->diagnostics().empty()));
        }
    }
}


////////////////////////////////////////////////////////////////////////////////
// Kept tokens and comments
////////////////////////////////////////////////////////////////////////////////

namespace {
std::shared_ptr<sc::src::TextInfo> text_of(std::string src, sc::src::TextKind kind) {
    return std::make_shared<sc::src::TextInfo>(sc::src::NormalisedSource(src), sc::src::FileCodeLocation {},
                                               "test_file", kind);
}

// Every token the lexer finds.
struct KeepEverything {
    struct Output {
        sc::lex::TokenType token;
        sc::src::SourceCodeRange range;
    };
    template <sc::lex::TokenType T> [[nodiscard]] std::optional<Output> process(sc::src::SourceCodeRange r) {
        return Output { T, r };
    }
};

// The kept tokens, comments, and unmatched brackets are exactly the lexer's tokens but whitespace, range by range,
// and the lexer's tokens cover the whole text. Returns what is wrong, or nothing.
std::optional<std::string> check_source_tokens(const sc::src::TextInfo& text_info, const P::ASTGraph& graph) {
    const auto& kept = graph.source_tokens();
    if (!kept)
        return "no tokens were kept";
    const auto at = [](const sc::src::SourceCodeRange& r) {
        return "line " + std::to_string(r.begin.line_number + 1) + ":" + std::to_string(r.begin.codepoint_in_line + 1);
    };
    const auto before = [](const sc::src::SourceCodeRange& a, const sc::src::SourceCodeRange& b) {
        return a.begin.absolute < b.begin.absolute;
    };

    if (kept->tokens.empty() || kept->tokens.back().token != sc::lex::TokenType::EndOfFile)
        return "the tokens don't end with EndOfFile";
    std::vector<sc::src::SourceCodeRange> tokens;
    for (const auto& t : kept->tokens.raw())
        if (!t.inserted() && t.token != sc::lex::TokenType::EndOfFile)
            tokens.push_back(t.range);
    std::vector<sc::src::SourceCodeRange> comments;
    for (const auto& c : kept->comments.raw()) {
        if (!sc::lex::is_comment(c.kind))
            return "a comment of a kind that isn't a comment at " + at(c.range);
        comments.push_back(c.range);
    }
    const auto& unmatched = kept->unmatched_brackets;
    if (!std::is_sorted(tokens.begin(), tokens.end(), before)
        || !std::is_sorted(comments.begin(), comments.end(), before)
        || !std::is_sorted(unmatched.begin(), unmatched.end(), before))
        return "a list is out of order";
    std::vector<sc::src::SourceCodeRange> tokens_and_comments;
    std::merge(tokens.begin(), tokens.end(), comments.begin(), comments.end(), std::back_inserter(tokens_and_comments),
               before);
    std::vector<sc::src::SourceCodeRange> all;
    std::merge(tokens_and_comments.begin(), tokens_and_comments.end(), unmatched.begin(), unmatched.end(),
               std::back_inserter(all), before);

    std::vector<sc::src::SourceCodeRange> lexed;
    sc::lex::CodePointStream stream { text_info.source, text_info.source_start_in_file };
    KeepEverything action;
    std::uint32_t end { 0 };
    for (auto t = sc::lex::lexer(stream, action);; t = sc::lex::lexer(stream, action)) {
        if (t.range.begin.absolute != end)
            return "the lexer skipped text before " + at(t.range);
        end = t.range.end.absolute;
        if (t.token == sc::lex::TokenType::EndOfFile)
            break;
        if (!sc::lex::is_whitespace(t.token))
            lexed.push_back(t.range);
    }
    if (end != text_info.source.as_string().size())
        return "the lexer stopped before the end of the text";

    const auto same = [](const sc::src::SourceCodeRange& a, const sc::src::SourceCodeRange& b) {
        return a.begin.absolute == b.begin.absolute && a.end.absolute == b.end.absolute;
    };
    const auto [k, l] = std::mismatch(all.begin(), all.end(), lexed.begin(), lexed.end(), same);
    if (k != all.end() && l != lexed.end())
        return "kept " + at(*k) + " where the lexer has " + at(*l);
    if (k != all.end() || l != lexed.end())
        return std::to_string(all.size()) + " kept, the lexer has " + std::to_string(lexed.size());
    return std::nullopt;
}

// Parses every `.sc` and `.scd` file under a directory, keeping the tokens, and calls f with its path, text, and graph.
// Returns how many files there were.
template <typename F> std::size_t parse_each_file(const std::filesystem::path& dir, F f) {
    std::size_t files { 0 };
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        const auto ext = entry.path().extension();
        if (!entry.is_regular_file() || (ext != ".sc" && ext != ".scd"))
            continue;
        std::ifstream file { entry.path(), std::ios::binary };
        std::stringstream contents;
        contents << file.rdbuf();
        const auto text_info =
            text_of(contents.str(), ext == ".sc" ? sc::src::TextKind::ClassFile : sc::src::TextKind::ScdFile);
        const auto [graph, accepted] = P::parse(text_info, P::KeepTokens::Yes);
        ++files;
        f(entry.path(), *text_info, *graph);
    }
    return files;
}

// Checks the kept tokens of every `.sc` and `.scd` file under a directory, returning how many there were and what was
// wrong.
std::tuple<std::size_t, std::vector<std::string>> check_source_tokens_in(const std::filesystem::path& dir) {
    std::vector<std::string> failures;
    const auto files = parse_each_file(
        dir, [&](const std::filesystem::path& path, const sc::src::TextInfo& text_info, const P::ASTGraph& graph) {
            if (const auto wrong = check_source_tokens(text_info, graph))
                failures.push_back(path.string() + ": " + *wrong);
        });
    return { files, failures };
}

// A node range rule that doesn't hold, see broken_range_rules. `rule` names the rule and the node, with its parent
// or sibling.
struct BrokenRangeRule {
    std::string rule;
    std::string where;
};

// The node range rules, other than which tokens belong to which node:
// - a non-empty range starts at the start of a token and ends at the end of one;
// - a node lies inside its parent;
// - a node doesn't overlap the sibling before it. Siblings needn't be in source order, e.g., a BinaryMessage's
//   selector is its first child, so only neighbours are compared;
// - something not written, a Missing node or a list of parts with nothing in it, has an empty range.
// The root, which spans the whole text, and Error nodes and their children, whose ranges come from error recovery,
// aren't checked.
std::vector<BrokenRangeRule> broken_range_rules(const sc::src::TextInfo& text_info, const P::ASTGraph& graph) {
    const auto& kept = graph.source_tokens();
    if (!kept)
        return { { "no tokens were kept", "" } };
    std::vector<std::uint32_t> begins;
    std::vector<std::uint32_t> ends;
    for (const auto& t : kept->tokens.raw())
        if (!t.inserted() && t.token != sc::lex::TokenType::EndOfFile) {
            begins.push_back(t.range.begin.absolute);
            ends.push_back(t.range.end.absolute);
        }
    const auto is_empty = [](const sc::src::SourceCodeRange& r) { return r.begin.absolute == r.end.absolute; };
    const auto overlap = [&](const sc::src::SourceCodeRange& a, const sc::src::SourceCodeRange& b) {
        if (is_empty(a) && is_empty(b))
            return false;
        if (is_empty(a) || is_empty(b)) {
            const auto& point = is_empty(a) ? a : b;
            const auto& other = is_empty(a) ? b : a;
            return other.begin.absolute < point.begin.absolute && point.begin.absolute < other.end.absolute;
        }
        return a.begin.absolute < b.end.absolute && b.begin.absolute < a.end.absolute;
    };
    const auto where = [&](const sc::src::SourceCodeRange& r) {
        std::string text { text_info.read(r) };
        std::replace(text.begin(), text.end(), '\n', ' ');
        if (text.size() > 60)
            text = text.substr(0, 60) + "...";
        return std::to_string(r.begin.line_number + 1) + ":" + std::to_string(r.begin.codepoint_in_line + 1) + " '"
            + text + "'";
    };
    const auto name_of = [&](P::Index i) { return std::string { P::ASTGraph::node_name(graph.payload(i)) }; };
    const std::set<std::string> lists_of_parts { "ArgumentList",
                                                 "BigMemberList",
                                                 "BlockContentsList",
                                                 "BlockList",
                                                 "DeclareArgumentList",
                                                 "DeclareClassVarList",
                                                 "DeclareVariableList",
                                                 "ExprSeq",
                                                 "GeneratorQualList",
                                                 "MethodList",
                                                 "NameList" };

    std::vector<BrokenRangeRule> broken;
    graph.flat_walk([&](const P::ASTGraph::Edges& e, const P::ASTGraph::Variant& v, const sc::src::SourceCodeRange& r,
                        size_t index) {
        if (!e.parent)
            return;
        const P::Index self { static_cast<std::uint32_t>(index) };
        // A node isn't its own ancestor, which error recovery adopting a node still being built would make.
        std::size_t depth { 0 };
        for (auto a = e.parent; a && depth < 100000; a = graph.edges(P::Index { *a }).parent, ++depth)
            if (P::Index { *a } == self) {
                broken.push_back({ "its own ancestor: " + std::string { P::ASTGraph::node_name(v) }, where(r) });
                break;
            }
        const std::string name = P::ASTGraph::node_name(v);
        const P::Index parent { *e.parent };
        const std::string parent_name = name_of(parent);
        if (name == "Error" || parent_name == "Error")
            return;
        if (!is_empty(r)
            && (!std::binary_search(begins.begin(), begins.end(), r.begin.absolute)
                || !std::binary_search(ends.begin(), ends.end(), r.end.absolute)))
            broken.push_back({ "not on token boundaries: " + name, where(r) });
        const auto& p = graph.location(parent);
        if (r.begin.absolute < p.begin.absolute || p.end.absolute < r.end.absolute)
            broken.push_back({ "outside its parent: " + name + " in " + parent_name, where(r) });
        if (e.prev_sibling) {
            const P::Index sibling { *e.prev_sibling };
            const auto sibling_name = name_of(sibling);
            if (sibling_name != "Error" && overlap(r, graph.location(sibling)))
                broken.push_back(
                    { "overlaps a sibling: " + std::min(name, sibling_name) + " and " + std::max(name, sibling_name),
                      where(r) });
        }
        const bool unwritten = name == "Missing" || (!e.first_child && lists_of_parts.count(name) > 0);
        if (unwritten && !is_empty(r))
            broken.push_back({ "not written but not empty: " + name, where(r) });
    });
    return broken;
}

// Checks the node ranges of every `.sc` and `.scd` file under a directory, reporting each broken rule once, with how
// often it broke.
void check_node_ranges_in(const std::filesystem::path& dir) {
    std::map<std::string, std::tuple<std::size_t, std::string>> broken;
    const auto files = parse_each_file(
        dir, [&](const std::filesystem::path& path, const sc::src::TextInfo& text_info, const P::ASTGraph& graph) {
            for (const auto& b : broken_range_rules(text_info, graph)) {
                auto& [count, example] = broken[b.rule];
                if (count++ == 0)
                    example = path.string() + ":" + b.where;
            }
        });
    BOOST_TEST(files > 0);
    for (const auto& [rule, found] : broken)
        BOOST_ERROR(rule + " (" + std::to_string(std::get<0>(found)) + " times), e.g., " + std::get<1>(found));
}
}

BOOST_AUTO_TEST_CASE(tokens_not_kept_unless_asked) {
    const auto [graph, accepted] = P::parse(text_of("a = 1; // b", sc::src::TextKind::ScdFile));
    BOOST_TEST(!graph->source_tokens().has_value());
}

BOOST_AUTO_TEST_CASE(kept_tokens_and_comments) {
    const char* src = "//# VERSION 3.16\n"
                      "// a comment\n"
                      "(\n"
                      "\tvar s = \"not // a comment\"; /* a /* nested */ block */\n"
                      "\ts ++ \\sym; // trailing\n"
                      ")\n";
    const auto text_info = text_of(src, sc::src::TextKind::ScdFile);
    const auto [graph, accepted] = P::parse(text_info, P::KeepTokens::Yes);
    BOOST_TEST(accepted);
    BOOST_TEST(graph->diagnostics().empty());
    BOOST_TEST_REQUIRE(graph->source_tokens().has_value());
    if (const auto wrong = check_source_tokens(*text_info, *graph))
        BOOST_ERROR(*wrong);

    using sc::lex::TokenType;
    const auto described = [](TokenType kind, std::string_view text) {
        return std::string { sc::lex::to_string(kind) } + " " + std::string { text };
    };
    const std::vector<std::string> expected {
        described(TokenType::VersionComment, "//# VERSION 3.16"),
        described(TokenType::Comment, "// a comment"),
        described(TokenType::MultilineComment, "/* a /* nested */ block */"),
        described(TokenType::Comment, "// trailing"),
    };
    std::vector<std::string> comments;
    for (const auto& c : graph->source_tokens()->comments.raw())
        comments.push_back(described(c.kind, text_info->read(c.range)));
    BOOST_TEST(comments == expected, boost::test_tools::per_element());

    // The tokens are the parser's, matching brackets included.
    const auto& tokens = graph->source_tokens()->tokens;
    BOOST_TEST_REQUIRE(tokens.size() == 12); // ( var s = "..." ; s ++ \sym ; ) EndOfFile
    BOOST_TEST((tokens[P::TokenIndex { 0 }].token == TokenType::OpenParen));
    BOOST_TEST((tokens[P::TokenIndex { 0 }].matching() == P::TokenIndex { 10 }));
    BOOST_TEST((tokens[P::TokenIndex { 11 }].token == TokenType::EndOfFile));
}

BOOST_AUTO_TEST_CASE(kept_tokens_with_errors) {
    // An inserted closing bracket, an unmatched one, a mismatched one, an unclosed string, an unclosed comment, a
    // non-breaking space, and a class file.
    for (const auto& [src, kind] : std::vector<std::tuple<const char*, sc::src::TextKind>> {
             { "f = { 1 + 2", sc::src::TextKind::ScdFile },
             { "f = 1 + 2);\ng = 3;", sc::src::TextKind::ScdFile },
             { "f = [1, 2);", sc::src::TextKind::ScdFile },
             { "f = {\xc2\xa0|a| a };", sc::src::TextKind::ScdFile },
             { "a = \"unclosed\n", sc::src::TextKind::ScdFile },
             { "a = 1; /* unclosed\n", sc::src::TextKind::ScdFile },
             { "A { // c\n\tb { ^1 /* c */ }\n", sc::src::TextKind::ClassFile },
         }) {
        BOOST_TEST_CONTEXT(src) {
            const auto text_info = text_of(src, kind);
            const auto [graph, accepted] = P::parse(text_info, P::KeepTokens::Yes);
            if (const auto wrong = check_source_tokens(*text_info, *graph))
                BOOST_ERROR(*wrong);
        }
    }
    const auto [inserted, a] = P::parse(text_of("f = { 1 + 2", sc::src::TextKind::ScdFile), P::KeepTokens::Yes);
    const auto& tokens = inserted->source_tokens()->tokens.raw();
    BOOST_TEST(std::any_of(tokens.begin(), tokens.end(), [](const P::Token& t) { return t.inserted(); }));
    const auto [unmatched, b] = P::parse(text_of("f = 1 + 2);", sc::src::TextKind::ScdFile), P::KeepTokens::Yes);
    BOOST_TEST(unmatched->source_tokens()->unmatched_brackets.size() == 1u);
}

BOOST_AUTO_TEST_CASE(kept_tokens_class_library) {
    const auto [files, failures] = check_source_tokens_in(SC_CLASS_LIBRARY_DIR);
    BOOST_TEST(files > 0);
    for (const auto& f : failures)
        BOOST_ERROR(f);
}

// Set SC_CORPUS to a directory of `.sc` and `.scd` files to check them too.
// The corpus is not on github.
BOOST_AUTO_TEST_CASE(kept_tokens_corpus) {
    const char* corpus = std::getenv("SC_CORPUS");
    if (!corpus)
        return;
    const auto [files, failures] = check_source_tokens_in(corpus);
    BOOST_TEST_MESSAGE(std::to_string(files) + " corpus files, " + std::to_string(failures.size()) + " failures");
    for (const auto& f : failures)
        BOOST_ERROR(f);
}


BOOST_AUTO_TEST_CASE(node_ranges_class_library) { check_node_ranges_in(SC_CLASS_LIBRARY_DIR); }

// Set SC_CORPUS to a directory of `.sc` and `.scd` files to check them too.
BOOST_AUTO_TEST_CASE(node_ranges_corpus) {
    if (const char* corpus = std::getenv("SC_CORPUS"))
        check_node_ranges_in(corpus);
}

// Which tokens belong to a node:
// - a list of parts runs from its first part to its last, and the brackets, keyword, or terminator around it belong
//   to its parent: `( )`, `| |`, `arg ;`, `{ }`, and `var`;
// - an array or dictionary includes its brackets, the array in `Set[1, 2]` too;
// - brackets around an expression belong to its parent, see ASTGraph::is_bracketed;
// - something not written, a Missing node or an empty list, has an empty range where it would be.
BOOST_AUTO_TEST_CASE(node_range_delimiters) {
    // The text of every node with this name, in the order they were made, so inner nodes first.
    const auto ranges_of = [](const char* src, sc::src::TextKind kind, std::string_view name) {
        const auto text_info = text_of(src, kind);
        const auto [graph, accepted] = P::parse(text_info);
        std::vector<std::string> texts;
        graph->flat_walk([&](const auto&, const auto& v, const sc::src::SourceCodeRange& r, size_t) {
            if (P::ASTGraph::node_name(v) == name)
                texts.emplace_back(text_info->read(r));
        });
        return texts;
    };
    const auto scd = sc::src::TextKind::ScdFile;
    using Texts = std::vector<std::string>;

    // A list of parts leaves out the brackets, keyword, or terminator around it.
    BOOST_TEST(ranges_of("f = { |a, b| a }", scd, "DeclareArgumentList") == Texts { "a, b" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { arg a, b; a }", scd, "DeclareArgumentList") == Texts { "a, b" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { |a ... b| a }", scd, "DeclareArgumentList") == Texts { "a ... b" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { arg ... b; b }", scd, "DeclareArgumentList") == Texts { "... b" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { | | 1 }", scd, "DeclareArgumentList") == Texts { "" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { arg; 1 }", scd, "DeclareArgumentList") == Texts { "" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f(1, 2)", scd, "ArgumentList") == Texts { "1, 2" }, boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { var a = 1, b; a }", scd, "DeclareVariableList") == Texts { "a = 1, b" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { 1; 2 }", scd, "BlockContentsList") == Texts { "1; 2" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("A { var <a, b; classvar c; }", sc::src::TextKind::ClassFile, "DeclareClassVarList")
                   == (Texts { "<a, b", "c" }),
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("A { var <a, b; classvar c; }", sc::src::TextKind::ClassFile, "BigMemberList")
                   == Texts { "var <a, b; classvar c" },
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("x = \"a\" \"b\"", scd, "StringLineList") == Texts { "\"a\" \"b\"" },
               boost::test_tools::per_element());

    // A literal includes its brackets.
    BOOST_TEST(ranges_of("x = [1, 2]; y = Set[3]", scd, "ArrayList") == (Texts { "[1, 2]", "[3]" }),
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("x = (a: 1)", scd, "DictionaryNode") == Texts { "(a: 1)" }, boost::test_tools::per_element());

    // Brackets around an expression belong to its parent.
    BOOST_TEST(ranges_of("x = (1 + 2) * 3", scd, "BinaryMessage") == (Texts { "1 + 2", "(1 + 2) * 3" }),
               boost::test_tools::per_element());

    // Something not written is empty.
    BOOST_TEST(ranges_of("x = Pfunc { 1 }; y.f { 2 }; z.f()", scd, "ArgumentList") == (Texts { "", "", "" }),
               boost::test_tools::per_element());
    BOOST_TEST(ranges_of("x = pi; #a, b = c", scd, "Missing") == (Texts { "", "" }), boost::test_tools::per_element());
    BOOST_TEST(ranges_of("f = { }", scd, "BlockContentsList") == Texts { "" }, boost::test_tools::per_element());
}
