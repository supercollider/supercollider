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
        BOOST_TEST(graph.diagnostics().empty());
    }

    const auto root = graph.root_any();
    BOOST_TEST(root.has_value());

    //    std::cout << "ROOT == " << **root << '\n';

    // for (const auto& d : graph.diagnostics())
    //     std::cout << d << std::endl;

    graph.debug_df_print(std::cout);
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


    tester("1 + \n(var x = 10; x * 2)", true);
    tester("(var x = 10; x * 2)", true);

    tester("(|a| var x = 10; x * 2)", true);

    tester("foo(this: 2, arg2: 10)", true);


    tester("{ var #a, b, c = array; }", true);
    tester("{ var #a, b, c = array, d = 10; }", true);
    tester("#~foo, rest = array;", true);


    tester("{: x + 1, x <- [1, 2, 3, 4], x.odd, ::x.postln, :while x < 3}", true);
}
