// Must be defined before the include, so the header provides main.
#define BOOST_TEST_MODULE sc_util_test
#include <boost/test/included/unit_test.hpp>

#include "sc_util/type_set_index.hpp"

namespace sc::util::typed_index {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static tests / Examples
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace tests {

enum struct Elements { A, B, C, D };

// size_t is the underlying index type.
// SomeCollectionType is just a tag, it could be anything. Only indexes with the same type here can convert to one
// another.
using Spec = Spec<std::size_t, Elements, struct SomeCollectionType>;

using Defs = QuicklyDefineTypesFromSpec<Spec>;

template <Elements... Set> using TypedIndex = Defs::TypedIndex<Set...>;
template <Elements... Set> using OptionalTypedIndex = Defs::OptionalTypedIndex<Set...>;

using A_Index = TypedIndex<Elements::A>;
using B_Index = TypedIndex<Elements::B>;
using C_Index = TypedIndex<Elements::C>;
using D_Index = TypedIndex<Elements::D>;

using AB_Index = join<A_Index, B_Index>;
using CD_Index = join<C_Index, D_Index>;

using AnyIndex = join<A_Index, B_Index, C_Index, D_Index>;


// Normal index stuff.
static_assert([]() -> bool {
    A_Index a1 { 0 };
    A_Index a2 { 1 };
    return *a1 != *a2;
}());

static_assert([]() -> bool {
    A_Index a1 { 0 };
    A_Index a2 { 0 };
    return *a1 == *a2;
}());

// Conversions
static_assert([]() -> bool {
    A_Index a { 0 };
    AB_Index b { a }; // a is a subset of ab, okay
    // B_Index bb{a}; // this is a compile time error
    return *b == 0;
}());

static_assert(std::is_constructible_v<AB_Index, A_Index>);
static_assert(!std::is_constructible_v<A_Index, CD_Index>);
static_assert(!std::is_convertible_v<A_Index, CD_Index>);
}

namespace details_both_types {

// Elements of the set.
enum struct Elements { A, B, C };

// Helper to define the types.
using Spec = Spec<std::size_t, Elements, struct Test>;

// Test/example code, templated on a typed index or an optionally typed index.
template <template <Elements...> typename IndexType> struct Tester {
    // Define index types that represent only a single element in the set.
    using A_Index = IndexType<Elements::A>;
    using B_Index = IndexType<Elements::B>;
    using C_Index = IndexType<Elements::C>;


    // Define index types that represent sets with many elements.
    // Verbose way.
    using AB_Index = IndexType<Elements::A, Elements::B>;
    // Using the 'join' helper
    using AC_Index = join<A_Index, C_Index>;

    using Any_Index = join<A_Index, B_Index, C_Index>;

    static_assert(convertible<A_Index, AB_Index>());

    static_assert(!convertible<AB_Index, A_Index>());

    static_assert(!convertible<A_Index, C_Index>());

    static_assert(convertible<A_Index, A_Index>());
    static_assert(convertible<AB_Index, AB_Index>());

    static_assert(convertible<A_Index, Any_Index>());
    static_assert(convertible<B_Index, Any_Index>());
    static_assert(convertible<C_Index, Any_Index>());
    static_assert(convertible<AB_Index, Any_Index>());
    static_assert(convertible<Any_Index, Any_Index>());

    static_assert(!convertible<Any_Index, A_Index>());

    static_assert([]() -> bool {
        A_Index a { std::size_t(0) };
        AB_Index ab { a };
        return *ab == 0;
    }());

    template <typename... Ts> struct o : Ts... { using Ts::operator()...; };
    template <class... Ts> o(Ts...) -> o<Ts...>;


    static_assert([]() -> bool {
        using V = std::variant<A_Index, B_Index>;
        AB_Index ab { 0 };
        V v = ab.template as_variant<Elements::A>();
        return std::visit(o {
                              [](A_Index) { return true; },
                              [](B_Index) { return false; },
                          },
                          v);
    }());

    // Runtime enum, picks the alternative matching the enum and keeps the index's value.
    static_assert([]() -> bool {
        AB_Index ab { 3 };
        const auto v = ab.as_variant(Elements::B);
        return std::visit(o {
                              [](A_Index) { return false; },
                              [](B_Index b) { return *b == 3; },
                          },
                          v);
    }());
    static_assert([]() -> bool {
        Any_Index any { 5 };
        return any.as_variant(Elements::C).index() == 2 && any.as_variant(Elements::A).index() == 0;
    }());
};

// This contains the definitions of the type erased index and optional index.
using G = QuicklyDefineTypesFromSpec<Spec>;

static constexpr auto test1 { Tester<G::TypedIndex> {} };
static constexpr auto test2 { Tester<G::OptionalTypedIndex> {} };

}

namespace details_graph_conversion_test {
enum struct Elements { X };
using SpecA = Spec<std::size_t, Elements, struct GraphTypeA>;
using SpecB = Spec<std::size_t, Elements, struct GraphTypeB>;

using DefA = QuicklyDefineTypesFromSpec<SpecA>;
using DefB = QuicklyDefineTypesFromSpec<SpecB>;

using AX_I = DefA::TypedIndex<Elements::X>;
using BX_I = DefB::TypedIndex<Elements::X>;

}

BOOST_AUTO_TEST_CASE(compiles) { BOOST_TEST(true); }
}