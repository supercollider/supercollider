#include "sc_parser/ast.hpp"
#include "sc_parser/nodes.hpp"
#include "sc_lexer/normalise_source.hpp"
#include "sc_parser/sc_parser.hpp"
#include "sc_lexer/text_location.hpp"
#include <memory>
#define BOOST_TEST_MODULE sc_parser_tests
#include <boost/test/included/unit_test.hpp>

namespace P = sc::ast;

const auto tester_base = [](const char* src, bool is_class_file, bool no_diags) {
    std::cout << "\n--------------------------------\n src: ";
    std::cout << src << "\n";

    auto text_info = std::shared_ptr<sc::lex::TextInfo>(new sc::lex::TextInfo {
        sc::lex::NormalisedSource(src),
        sc::lex::FileCodeLocation {},
        "test_file",
        is_class_file,
    });

    auto [graph, result] = P::parse(text_info);
    BOOST_TEST(result == 0);

    // graph.flat_walk(
    //     [&](const Graph::Edges& e, const Graph::Variant& payload, const sc::lex::SourceCodeRange& loc, size_t i) {
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
    auto text_info = std::make_shared<sc::lex::TextInfo>(sc::lex::NormalisedSource("a = 1;\n\xca\xca"
                                                                                   "b = 2;\nc = \xca"
                                                                                   "3;\n"),
                                                         sc::lex::FileCodeLocation {}, "test_file", false);
    auto [graph, result] = P::parse(text_info);
    BOOST_TEST_REQUIRE(graph->diagnostics().size() == 1);
    const auto& d = graph->diagnostics()[0];
    BOOST_TEST(std::string { d.name() } == "Invalid UTF-8.");
    BOOST_TEST(d.locations()[0].range.begin.line_number == 1);
    BOOST_TEST(d.locations()[0].msg.find("nor are 2 more") != std::string::npos);
}

namespace {
using V = P::LanguageVersion;

// The version of each class and extension, in order, and the names of the diagnostics.
void check_versions(const char* src, const std::vector<V>& versions, const std::vector<std::string>& diagnostics,
                    bool is_class_file = true) {
    auto text_info = std::make_shared<sc::lex::TextInfo>(sc::lex::NormalisedSource(src), sc::lex::FileCodeLocation {},
                                                         "test_file", is_class_file);
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
    check_versions("// Some licence.\n/* more */\n//# VERSION 3.16\nFoo {}", { V::V3_16 }, {});
    check_versions("//# VERSION 3.13\nFoo {\n    //# VERSION 3.16\n}\nBar {}", { V::V3_16, V::Legacy }, {});
}

BOOST_AUTO_TEST_CASE(versions_invalid) {
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
    check_versions("//# VERSION 3.16\n//# VERSION 3.13\nFoo {}", { V::Legacy }, { "Duplicate version" });
}

BOOST_AUTO_TEST_CASE(versions_misplaced) {
    check_versions("Foo {}\n//# VERSION 3.16\nBar {}", { V::Legacy, V::Legacy }, { "Misplaced version" });
    check_versions("Foo {}\n//# VERSION 3.16", { V::Legacy }, { "Misplaced version" });
    check_versions("Foo {\n    var a;\n    //# VERSION 3.16\n}", { V::Legacy }, { "Misplaced version" });
    check_versions("Foo {\n    bar {\n        //# VERSION 3.16\n        if (x) { ^1 }; ^2\n    }\n}", { V::Legacy },
                   { "Misplaced version" });
    check_versions("Foo : Object\n//# VERSION 3.16\n{}", { V::Legacy }, { "Misplaced version" });
}

BOOST_AUTO_TEST_CASE(versions_interpreted) {
    check_versions("//# VERSION 3.16\n1 + 1", {}, { "Version in interpreted code" }, false);
    check_versions("(\n//# VERSION 3.16\n1 + 1\n)", {}, { "Version in interpreted code" }, false);
}
