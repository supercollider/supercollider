#pragma once

#include "sc_sema/details/indexes.hpp"
#include "sc_util/details/typed_graph_node.hpp"

namespace sc::ir {

/// @brief `this`, `super`, and the pseudo variables, e.g., `thisProcess`.
/// These aren't variables: the language sets them, nothing declares them, and they can't be assigned.
struct SCSpecial : sc::util::typed_graph::TerminalNode<SCSpecial_I> {
    static constexpr auto type_name { "SCSpecial" };
    enum struct Kind {
        This,
        Super,
        Process,
        Thread,
        Method,
        FunctionDef,
        Function,
    } kind;
    SCSpecial(Kind k): kind(k) {}
};


// How the elements of a class's instances are stored.
// Not indexed means you can't index into it, in other words, it is a class.
enum struct MemoryLayout { NotIndexed, Slot, Double, Float, Int32, Int16, Int8, Char, Symbol };


}
