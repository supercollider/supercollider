#define BOOST_TEST_MODULE sc_backend_tests
#include "sc_compiler/inspect.hpp"
#include "sc_compiler/sc_compiler.hpp"
#include "sc_diagnostic/sc_diagnostic.hpp"

#include <boost/test/included/unit_test.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>

// Unit tests of the backend's decisions, read from its output rather than from golden files: which frames need a
// heap context, which functions are closed, which methods are shortcuts, where hoisted variables go, and the
// diagnostics the emitter reports. Each test compiles a small class file after the regression tests' prelude.

using namespace sc;
namespace sb = sc::backend;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////

std::shared_ptr<const src::TextInfo> class_text(std::string source, std::filesystem::path path) {
    return std::make_shared<const src::TextInfo>(src::NormalisedSource { std::move(source) }, src::FileCodeLocation {},
                                                 std::move(path), src::TextKind::ClassFile);
}

std::string read_prelude() {
    std::ifstream file { SC_BACKEND_TEST_PRELUDE };
    BOOST_REQUIRE_MESSAGE(file.is_open(), "the prelude " SC_BACKEND_TEST_PRELUDE);
    std::stringstream ss;
    ss << file.rdbuf();
    return std::move(ss).str();
}

// A class file compiled after the prelude, through every backend stage.
struct Compiled {
    std::unique_ptr<compiler::ClassLibrary> library;
    util::IndexedVector<compiler::ClassGraphIndex, compiler::CompiledGraph> graphs;

    explicit Compiled(std::string source) {
        util::IndexedVector<ir::SourceEntryIndex, compiler::ClassFileSource> sources;
        (void)sources.push_back({ class_text(read_prelude(), "prelude.sc"), ir::ModuleIndex { 0 } });
        (void)sources.push_back({ class_text(std::move(source), "test.sc"), ir::ModuleIndex { 1 } });
        library = compiler::compile_class_library(sources, 1, {});
        for (const auto& d : library->parse_diagnostics)
            BOOST_TEST_MESSAGE("parse diagnostic: " << d.to_string());
        for (const auto& d : library->diagnostics)
            BOOST_TEST_MESSAGE("diagnostic: " << d.to_string());
        graphs = compiler::compile_class_graphs(*library, 1);
    }

    std::string_view text(ir::Symbol s) const { return library->symbol_text(s); }

    const compiler::CompiledGraph* graph(std::string_view class_name) const {
        for (const auto& g : graphs.raw())
            if (const auto* c = std::get_if<compiler::CompiledClass>(&g.unit); c && text(c->name) == class_name)
                return &g;
        return nullptr;
    }

    // A method of a class, or of a meta class, e.g., "Meta_Foo".
    const compiler::CompiledFunction& method(std::string_view class_name, std::string_view selector) const {
        const auto* g = graph(class_name);
        BOOST_REQUIRE_MESSAGE(g, "class " << class_name << " was compiled");
        for (const auto& m : g->methods)
            if (text(m.selector) == selector)
                return g->functions[m.function];
        BOOST_FAIL("no method " << class_name << ":" << selector);
        throw;
    }

    // The functions nested in a method, in order.
    std::vector<const compiler::CompiledFunction*> nested(std::string_view class_name,
                                                          std::string_view selector) const {
        const auto* g = graph(class_name);
        BOOST_REQUIRE(g);
        sb::lir::OptionalFunctionIndex root;
        for (const auto& m : g->methods)
            if (text(m.selector) == selector)
                root = m.function;
        BOOST_REQUIRE(root);
        std::vector<const compiler::CompiledFunction*> out;
        for (const auto i : g->functions.indices()) {
            // A function belongs to the method if its chain of parents reaches it.
            auto p = g->functions[i].parent;
            while (p && p != root)
                p = g->functions[sb::lir::FunctionIndex { *p }].parent;
            if (p)
                out.push_back(&g->functions[i]);
        }
        return out;
    }

    const compiler::CompiledAccessor* accessor(std::string_view class_name, std::string_view selector) const {
        const auto* g = graph(class_name);
        BOOST_REQUIRE(g);
        for (const auto& a : g->accessors)
            if (text(a.selector) == selector)
                return &a;
        return nullptr;
    }

    std::vector<std::string> slot_names(const compiler::CompiledFunction& f) const {
        std::vector<std::string> out;
        for (const auto& s : f.slots.raw())
            out.emplace_back(text(s.name));
        return out;
    }

    std::vector<std::string> diagnostic_text(std::string_view class_name) const {
        std::vector<std::string> out;
        for (const auto& d : library->diagnostics)
            out.push_back(d.to_string());
        if (const auto* g = graph(class_name))
            for (const auto& d : g->diagnostics)
                out.push_back(d.to_string());
        return out;
    }
};

template <typename Shortcut> bool is(const compiler::CompiledFunction& f) {
    return std::holds_alternative<Shortcut>(f.emitted.shortcut);
}
bool no_shortcut(const compiler::CompiledFunction& f) {
    return std::holds_alternative<std::monostate>(f.emitted.shortcut);
}

bool contains(const std::vector<std::string>& v, std::string_view s) {
    return std::find(v.begin(), v.end(), s) != v.end();
}

} // namespace

////////////////////////////////////////////////////////////////////////////////
// Heap contexts and closed functions
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(heap_context)

BOOST_AUTO_TEST_CASE(method_without_closures_is_reference_counted) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |a| var b = a + 1; ^b } }" };
    BOOST_TEST(!c.method("Foo", "m").emitted.properties.needs_heap_context);
}

BOOST_AUTO_TEST_CASE(closure_over_a_local_needs_the_method_frame) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |a| ^{ a } } }" };
    BOOST_TEST(c.method("Foo", "m").emitted.properties.needs_heap_context);
    const auto inner = c.nested("Foo", "m");
    BOOST_REQUIRE_EQUAL(inner.size(), 1u);
    BOOST_TEST(inner[0]->emitted.properties.has_context);
    BOOST_TEST(!inner[0]->emitted.properties.is_closed);
}

BOOST_AUTO_TEST_CASE(function_using_nothing_outside_is_closed) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { ^{ |x| x + 1 } } }" };
    BOOST_TEST(!c.method("Foo", "m").emitted.properties.needs_heap_context);
    const auto inner = c.nested("Foo", "m");
    BOOST_REQUIRE_EQUAL(inner.size(), 1u);
    BOOST_TEST(inner[0]->emitted.properties.is_closed);
    BOOST_TEST(!inner[0]->emitted.properties.has_context);
}

BOOST_AUTO_TEST_CASE(closure_two_deep_keeps_every_frame_between) {
    // The middle function reads nothing, but the inner one reaches through it to the method.
    const Compiled c { "//# VERSION 3.16\nFoo { m { |a| ^{ { a } } } }" };
    BOOST_TEST(c.method("Foo", "m").emitted.properties.needs_heap_context);
    const auto inner = c.nested("Foo", "m");
    BOOST_REQUIRE_EQUAL(inner.size(), 2u);
    for (const auto* f : inner) {
        BOOST_TEST(f->emitted.properties.has_context);
        BOOST_TEST(!f->emitted.properties.is_closed);
    }
    BOOST_TEST(inner[0]->emitted.properties.needs_heap_context, "the middle frame is captured by the inner closure");
}

BOOST_AUTO_TEST_CASE(this_instance_variables_and_returns_are_not_closed) {
    const Compiled c { "//# VERSION 3.16\nFoo { var iv; t { ^{ this } } i { ^{ iv } } r { ^{ ^1 } } }" };
    for (const auto* name : { "t", "i", "r" }) {
        BOOST_TEST_CONTEXT(name) {
            BOOST_TEST(c.method("Foo", name).emitted.properties.needs_heap_context);
            const auto inner = c.nested("Foo", name);
            BOOST_REQUIRE_EQUAL(inner.size(), 1u);
            BOOST_TEST(!inner[0]->emitted.properties.is_closed);
        }
    }
}

BOOST_AUTO_TEST_CASE(explicitly_closed_function_has_no_context) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { ^#{ |x| x } } }" };
    BOOST_TEST(!c.method("Foo", "m").emitted.properties.needs_heap_context);
    const auto inner = c.nested("Foo", "m");
    BOOST_REQUIRE_EQUAL(inner.size(), 1u);
    BOOST_TEST(!inner[0]->emitted.properties.has_context);
}

BOOST_AUTO_TEST_CASE(inlined_bodies_make_no_functions) {
    const Compiled c {
        "//# VERSION 3.16\nFoo { m { |a| var r; if (a) { var h = 1; r = h }; while { false } { }; ^r } }"
    };
    BOOST_TEST(c.nested("Foo", "m").empty());
    BOOST_TEST(!c.method("Foo", "m").emitted.properties.needs_heap_context);
}

BOOST_AUTO_TEST_CASE(closure_in_an_inlined_body_needs_the_method_frame) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |a| if (a) { ^{ a } }; ^nil } }" };
    BOOST_TEST(c.method("Foo", "m").emitted.properties.needs_heap_context);
}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////
// Hoisting
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(hoisting)

BOOST_AUTO_TEST_CASE(inlined_body_variables_join_the_frame) {
    const Compiled c {
        "//# VERSION 3.16\nFoo { m { |x| var a = 1; if (x) { var b = 2, c; c = b }; while { false } { var d }; ^a } }"
    };
    const auto names = c.slot_names(c.method("Foo", "m"));
    BOOST_TEST(names == (std::vector<std::string> { "this", "x", "a", "b", "c", "d" }),
               boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(hoisted_variables_have_no_prototype_value) {
    // The body sets them each pass, so the prototype frame holds nil, unlike the method's own variable.
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| var a = 1; if (x) { var b = 2; a = b }; ^a } }" };
    const auto& f = c.method("Foo", "m");
    BOOST_REQUIRE_EQUAL(f.slots.size(), 4u);
    BOOST_TEST(static_cast<bool>(f.slots.raw()[2].prototype), "a's literal");
    BOOST_TEST(!f.slots.raw()[3].prototype, "b is set by the body");
}

BOOST_AUTO_TEST_CASE(captured_variable_is_not_hoisted) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| var a = 1; if (x) { var b = 2; ^{ b } }; ^a } }" };
    const auto names = c.slot_names(c.method("Foo", "m"));
    BOOST_TEST(!contains(names, "b"));
    const auto inner = c.nested("Foo", "m");
    BOOST_REQUIRE(!inner.empty());
    BOOST_TEST(contains(c.slot_names(*inner[0]), "b"), "the if body keeps a frame of its own");
}

// Hoisting stops at the frame limit, and the send stays a send (compiler-tests finding 2).
std::string many_variables(int n) {
    std::string s = "var ";
    for (int i = 0; i < n; ++i)
        s += (i ? ", w" : "w") + std::to_string(i);
    return s + "; ";
}

BOOST_AUTO_TEST_CASE(body_that_fits_exactly_is_hoisted) {
    // this, x and 250 variables, then 3 more: 255 slots.
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| " + many_variables(250)
                       + "if (x) { var a0, a1, a2; a0 = 1 }; ^nil } }" };
    BOOST_TEST(c.method("Foo", "m").slots.size() == 255u);
    BOOST_TEST(c.nested("Foo", "m").empty());
    BOOST_TEST(c.diagnostic_text("Foo").empty());
}

BOOST_AUTO_TEST_CASE(body_that_would_not_fit_is_a_function) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| " + many_variables(250)
                       + "if (x) { var a0, a1, a2, a3; a0 = 1 }; ^nil } }" };
    BOOST_TEST(c.method("Foo", "m").slots.size() == 252u);
    const auto inner = c.nested("Foo", "m");
    BOOST_REQUIRE_EQUAL(inner.size(), 1u);
    BOOST_TEST(c.slot_names(*inner[0]) == (std::vector<std::string> { "a0", "a1", "a2", "a3" }),
               boost::test_tools::per_element());
    BOOST_TEST(c.diagnostic_text("Foo").empty());
}

BOOST_AUTO_TEST_CASE(first_bodies_win) {
    // Greedy in source order: the first body fits, the second doesn't, the third (no variables) is inlined.
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| " + many_variables(250)
                       + "if (x) { var a0, a1; a0 = 1 }; if (x) { var b0, b1; b0 = 1 }; if (x) { x = 1 }; ^nil } }" };
    const auto names = c.slot_names(c.method("Foo", "m"));
    BOOST_TEST(contains(names, "a0"));
    BOOST_TEST(!contains(names, "b0"));
    BOOST_TEST(c.nested("Foo", "m").size() == 1u);
}

BOOST_AUTO_TEST_CASE(own_variables_past_the_limit_are_still_an_error) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { " + many_variables(260) + "^nil } }" };
    const auto text = c.diagnostic_text("Foo");
    BOOST_TEST(std::any_of(text.begin(), text.end(),
                           [](const std::string& d) { return d.find("Too many variables") != std::string::npos; }));
}

BOOST_AUTO_TEST_CASE(legacy_does_not_hoist) {
    const Compiled c { "Foo { m { |x| var a = 1; if (x) { var b = 2; a = b }; ^a } }" };
    BOOST_TEST(!contains(c.slot_names(c.method("Foo", "m")), "b"));
    BOOST_TEST(c.nested("Foo", "m").size() == 1u, "the body with a variable is a function");
}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////
// Shortcuts
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(shortcuts)

BOOST_AUTO_TEST_CASE(each_kind) {
    const Compiled c { R"(//# VERSION 3.16
Foo {
    var iv, inner;
    classvar cv;
    self { ^this }
    empty { }
    literal { ^1.5 }
    argument { |a, b| ^b }
    instance { ^iv }
    assign { |v| iv = v }
    redirect { |a| ^this.other(a) }
    redirectSuper { |a| ^super.other(a) }
    forward { |a| ^inner.other(a) }
    other { |a| ^a }
    *classVariable { ^cv }
    *setClassVariable { |v| cv = v }
}
)" };
    BOOST_TEST(is<sb::shortcut::ReturnSelf>(c.method("Foo", "self")));
    BOOST_TEST(is<sb::shortcut::ReturnSelf>(c.method("Foo", "empty")));
    BOOST_TEST(is<sb::shortcut::ReturnLiteral>(c.method("Foo", "literal")));
    BOOST_REQUIRE(is<sb::shortcut::ReturnArgument>(c.method("Foo", "argument")));
    BOOST_TEST(*std::get<sb::shortcut::ReturnArgument>(c.method("Foo", "argument").emitted.shortcut).slot == 2u);
    BOOST_TEST(is<sb::shortcut::ReturnInstance>(c.method("Foo", "instance")));
    BOOST_TEST(is<sb::shortcut::AssignInstance>(c.method("Foo", "assign")));
    BOOST_REQUIRE(is<sb::shortcut::Redirect>(c.method("Foo", "redirect")));
    BOOST_TEST(!std::get<sb::shortcut::Redirect>(c.method("Foo", "redirect").emitted.shortcut).super);
    BOOST_REQUIRE(is<sb::shortcut::Redirect>(c.method("Foo", "redirectSuper")));
    BOOST_TEST(std::get<sb::shortcut::Redirect>(c.method("Foo", "redirectSuper").emitted.shortcut).super);
    BOOST_TEST(is<sb::shortcut::ForwardInstance>(c.method("Foo", "forward")));
    BOOST_TEST(is<sb::shortcut::ReturnClassVariable>(c.method("Meta_Foo", "classVariable")));
    BOOST_TEST(is<sb::shortcut::AssignClassVariable>(c.method("Meta_Foo", "setClassVariable")));
    for (const auto* name : { "self", "literal", "argument", "instance", "assign", "redirect", "forward" })
        BOOST_TEST(c.method("Foo", name).emitted.code.codes.empty(), name << " has no code");
}

BOOST_AUTO_TEST_CASE(not_shortcuts) {
    const Compiled c { R"(//# VERSION 3.16
Foo {
    var iv;
    className { ^Array }
    function { ^{ 1 } }
    assignLiteral { iv = 0 }
    assignExpression { |v| iv = v + 1 }
    variadic { |... a| ^this }
    keywordSend { |a| ^this.other(x: a) }
    reordered { |a, b| ^this.other(b, a) }
    other { |a, b| ^a }
    doesNotUnderstand { ^42 }
}
)" };
    for (const auto* name :
         { "className", "function", "assignLiteral", "assignExpression", "variadic", "keywordSend", "reordered" })
        BOOST_TEST(no_shortcut(c.method("Foo", name)), name);
    BOOST_TEST(no_shortcut(c.method("Foo", "doesNotUnderstand")), "doesNotUnderstand must keep its code");
}

BOOST_AUTO_TEST_CASE(assign_shortcut_is_316_only) {
    const Compiled c { "Foo { var iv; assign { |v| iv = v } }" };
    BOOST_TEST(no_shortcut(c.method("Foo", "assign")));
}

BOOST_AUTO_TEST_CASE(accessors) {
    const Compiled c { "//# VERSION 3.16\nFoo { var <>x, <y, >z; }" };
    const auto* getter = c.accessor("Foo", "x");
    const auto* setter = c.accessor("Foo", "x_");
    BOOST_REQUIRE(getter && setter);
    BOOST_TEST(std::holds_alternative<sb::shortcut::ReturnInstance>(getter->shortcut));
    BOOST_TEST(std::holds_alternative<sb::shortcut::AssignInstance>(setter->shortcut));
    BOOST_TEST(c.accessor("Foo", "y") != nullptr);
    BOOST_TEST(c.accessor("Foo", "y_") == nullptr);
    BOOST_TEST(c.accessor("Foo", "z") == nullptr);
    BOOST_TEST(c.accessor("Foo", "z_") != nullptr);
}

BOOST_AUTO_TEST_CASE(function_compile_context_keeps_its_code) {
    const Compiled c { "Foo {}" };
    BOOST_TEST(no_shortcut(c.method("Interpreter", "functionCompileContext")));
}

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////
// Switch tables and diagnostics
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(emitter)

BOOST_AUTO_TEST_CASE(inlined_switch_has_a_table) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| ^switch (x) { 1 } { \\a } { 2 } { \\b } { 3 } { \\c } } }" };
    const auto& f = c.method("Foo", "m");
    BOOST_REQUIRE_EQUAL(f.emitted.switch_tables.size(), 1u);
    BOOST_TEST(f.emitted.switch_tables.raw()[0].offsets.size() == 3u);
}

BOOST_AUTO_TEST_CASE(duplicate_switch_keys_keep_the_first) {
    // compiler-tests finding 6: the old compiler took the last.
    const Compiled c { "//# VERSION 3.16\nFoo { m { |x| ^switch (x) { 1 } { \\a } { 1 } { \\b } } }" };
    const auto& f = c.method("Foo", "m");
    BOOST_REQUIRE_EQUAL(f.emitted.switch_tables.size(), 1u);
    BOOST_TEST(f.emitted.switch_tables.raw()[0].offsets.size() == 1u);
}

BOOST_AUTO_TEST_CASE(too_many_selectors_is_a_diagnostic) {
    std::string body;
    for (int i = 0; i < 300; ++i)
        body += "this.s" + std::to_string(i) + "; ";
    const Compiled c { "//# VERSION 3.16\nFoo { m { " + body + "} }" };
    BOOST_TEST(!c.diagnostic_text("Foo").empty(), "300 selectors in one function");
}

// The bytecode counts the frames out to a local in a byte (compiler-tests finding 1).
std::string nested_use(int depth, const std::string& use) {
    std::string s = "//# VERSION 3.16\nFoo { m { var a = 5; ^";
    for (int i = 0; i < depth; ++i)
        s += "{ ";
    s += use;
    for (int i = 0; i < depth; ++i)
        s += " }";
    return s + " } }";
}

bool reports_out_of_reach(const Compiled& c) {
    const auto text = c.diagnostic_text("Foo");
    return std::any_of(text.begin(), text.end(),
                       [](const std::string& d) { return d.find("Variable out of reach") != std::string::npos; });
}

BOOST_AUTO_TEST_CASE(local_255_functions_out_is_reachable) {
    BOOST_TEST(!reports_out_of_reach(Compiled { nested_use(255, "a") }));
    BOOST_TEST(!reports_out_of_reach(Compiled { nested_use(255, "a = 1") }));
}

BOOST_AUTO_TEST_CASE(local_256_functions_out_is_a_diagnostic) {
    BOOST_TEST(reports_out_of_reach(Compiled { nested_use(256, "a") }), "a read");
    BOOST_TEST(reports_out_of_reach(Compiled { nested_use(256, "a = 1") }), "a store");
}

BOOST_AUTO_TEST_CASE(deep_nesting_without_far_uses_is_fine) {
    BOOST_TEST(!reports_out_of_reach(Compiled { nested_use(300, "var b = 1; b") }));
}

BOOST_AUTO_TEST_CASE(nothing_reported_for_ordinary_code) {
    const Compiled c { "//# VERSION 3.16\nFoo { m { |a| var b = a ? 1; ^[b, { b }] } }" };
    BOOST_TEST(c.diagnostic_text("Foo").empty());
}

BOOST_AUTO_TEST_SUITE_END()
