# sc_util

Small header-only helpers used by every other library. It has no dependencies and no source files. Everything is in
namespace `sc::util`.

The main idea is that a number which indexes something should have its own type. 
This make putting the wrong index type some where a compiler error, and drastically improves the readability of functions.


Additionally, type_set_index define a way of working with sets of possible meanings and converting between them safely. Similar to inheritance, in that you can cast to a base class, but using set operations at compile time.

## What is in it

| Header | Purpose |
|---|---|
| `strong_index.hpp` | `StrongIndex<Underlying, Tag>`, an integer wrapped in a type. Also `advance`, `retreat`, and `IndexRange`. |
| `indexed_vector.hpp` | `IndexedVector<Index, T>`, a vector that only accepts `Index`. |
| `type_set_index.hpp` | `TypedIndex`, an index that carries a set of node kinds, and `join` to combine sets. |
| `typed_graph*.hpp` | The graph that both `ASTGraph` and `IRGraph` are built on. |
| `narrow.hpp` | `narrow<To>(value)`, a cast that asserts the value no overflow. |
| `overload.hpp` | `overload`, for building a visitor from several lambdas, standard c++. |
| `unreachable.hpp` | `unreachable()`, which is `std::unreachable` from C++23. |

## StrongIndex and IndexedVector

```cpp
using SelectorIndex = util::StrongIndex<std::uint8_t, struct SelectorIndex__>;
```

The tag can be an incomplete type. Two indexes with different tags never convert to each other. `*i` or `i.value()` gives
the integer. `StrongIndex::from(t)` casts from another integer type.

`IndexedVector<SelectorIndex, Selector>` is indexed only with a `SelectorIndex`. `push_back` returns the new index.
`indices()` gives every index in order, for a range-for. `raw()` gives the `std::vector` for algorithms, and is meant for
reading, as inserting through it skips the type check.

The rules for using indexes are in the [conventions](../README.md#conventions) section of the main README.

## TypedIndex

The main purpose of this is to create a graph where the children of a node have a range of node types they accept.

A `TypedIndex` is an index plus a compile-time set of node kinds, taken from an enum. 
An index for one kind converts to an index for a set that includes it. 
The reverse is a compile error. 

Let $E$ be the enum, $S$ be one or more values in the enum such that $S \subseteq E$, and $I_S$ is the index type.

$$I_A \to I_B \text{ converts implicitly} \iff A \subseteq B$$


In the example below, $\{X\} \subseteq \{X, Y\}$, so `XIndex` converts to `XYIndex`. The reverse does not hold, as
$\{X, Y\} \not\subseteq \{X\}$.

To go from a set to a single kind, check the node at
run time with `is_a`, `as_a`, or `visit` on the graph.

```cpp
enum struct E { X, Y, Z };
using Defs = QuicklyDefineTypesFromSpec<Spec<std::uint32_t, E, struct MyGraphTag>>;
using XIndex  = Defs::TypedIndex<E::X>;
using XYIndex = join<XIndex, Defs::TypedIndex<E::Y>>;

XYIndex xy { XIndex { 1 } };   // fine, X is in {X, Y}
XIndex  x  { xy };             // compile error
```

The tag in `Spec` stops indexes from different graphs converting to each other. There is a long comment at the top of
`type_set_index.hpp` and compile-time tests in `test/main.cpp`. 
If you change this file, add a compile-time test. 

The templates are hard to read but the compile time test are clear and document the behaviour.

This complexity cost is worth it because it makes all the node construction validated at compile time. While we could do something similar in an object oriented style, the class hierachy would be unmangable.

## The typed graph

This is essentially a linked list of indices, it uses data oriented principles with multiple vectors.

`typed_graph::GraphImplementation` has as parallel vectors, one entry per node: the payload, the edges
(parent, first child, and siblings), and the last child. 
There are no pointers, it is trival to serialise.
It is designed to be inherited from privately. 
`ASTGraph` in `sc_parser/ast.hpp` and `IRGraph` in `sc_sema/ir.hpp` are the two users.

Each node type derives from one of three bases, and names the index it produces:

As many of this requirements are statically asserted as possible, this makes it impossible to use incorrectly, and somewhat improves errors messages. 
This code is not designed to be edited, but to make it impossible to construct an incorrect graph, and to ensure  a case that isn't handled in a later pass is made a compile time error, as this can introduce silent errors that are only reachable at runtime, requiring extensive testing.




| Base | Children |
|---|---|
| `TerminalNode<Index>` | None. |
| `Node<Index(ChildIndex, ...)>` | A fixed number, each with its own index type. |
| `ListNode<Index, ChildIndex>` | Zero +, all of the same type. |

```cpp
struct ExprSeq : ListNode<ExprSeqIndex, AnyExprIndex> { ... };
struct PiNode : Node<PiLitIndex(maybe<NumberIndex> multiplier)> { ... };
```

A node can hold extra data as ordinary members. Nodes must be trivially movable and destructible, which is a
`static_assert` in the graph, so they cannot own memory.

All node types are listed in a `TypedGraphHelper<...>`, which builds the `std::variant` of payloads. A node that is
not in the list does not compile. Every node needs its own enum entry.

Main operations, all checked at compile time against the node's declared children:
- `create(Node{...}, children...)` returns the typed index.
- `children(i)` returns a tuple for a fixed node, or a range for a list.
- `visit(f, i)` calls `f` with the typed index of the real node type.
- `is_a<T>(i)`, `as_a<T>(i)`, and `assume_a<T>(i)` check or assume the kind.
- `append`, `prepend`, and `merge` change lists. `merge` kills the second list. `cast` changes a node's kind in place.
- `flat_walk` and `traverse_only_children` walk the graph.

Dead nodes are reused by the next `create`. `orphans()` finds nodes with no parent, which is useful after errors.
`debug_find_loop()` finds cycles when debugging.

Compile time matters here. Looking up a payload by type makes `std::variant` search every alternative, so the graph uses
positions and folds. Keep that in mind when adding code to these headers.

This has been optimised as much as possible for compilation performance.

## How to add

**A new strong index.** Declare it next to the thing it indexes, with a unique tag, and use `IndexedVector` for the
container.

**A new node in a graph.** Each graph that uses the typed graph defines the same four things, so a new node needs:
1. An entry in the graph's enum of node kinds.
2. An index alias for that kind.
3. A struct derived from `TerminalNode`, `Node`, or `ListNode`, naming that index and its children.
4. The struct added to the graph's `TypedGraphHelper` list. A node left out of the list does not compile.

The README of the library that owns the graph says where each of these lives.

**A new helper.** Only add it here if two or more libraries need it. Keep it header-only.

## Tests

`test/main.cpp`, built as `sc_util_test`, is mostly `static_assert`s on `TypedIndex`. Run it with:

```
ctest --test-dir build -R langutils.sc_util_test_run
```
