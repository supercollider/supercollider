# sc_parser

Turns text into an abstract syntax tree (AST). 
It depends on [sc_lexer](../sc_lexer/README.md),
[sc_source](../sc_source/README.md), `sc_util`, and `sc_diagnostic`. 

Everything is in namespace `sc::ast`.

The parser recovers from syntax errors, so it nearly always returns a tree, if it doesn't please report this.

## What is in it

| File | Purpose |
|---|---|
| `sc_parser.hpp` | `parse`, the only entry point. |
| `ast.hpp` | `ASTGraph`, the tree, and the list of every node type. |
| `details/nodes.hpp` | The node types, such as `IntNode`, `BinaryMessage`, and `Class`. Each says which children it accepts. |
| `details/indexes_typed.hpp` | The typed indexes, such as `IntLitIndex`, and `maybe<T>`. |
| `src/sc_grammar.y` | The Bison grammar which is used to generate `sc_grammar_parser.*`. The generated files are committed. CMake runs Bison 3.8 or newer only when the grammar changes (tracked by `sc_grammar.y.sha256`). |

## Parsing

The text is a `TextInfo` from `sc_source`. Its `TextKind` chooses what the parser expects.

| `TextKind` | Parses | Root |
|---|---|---|
| `ClassFile` | A file of classes and extensions. | `ClassOrExtensionListIndex` |
| `ScdFile` | Interpreted code, split into regions. | `RegionListIndex` |
| `InterpretedFunction` | Code written as the body of a function. | `FileAsFunctionIndex` |

```cpp
auto text = std::make_shared<sc::src::TextInfo>(
    sc::src::NormalisedSource { code }, sc::src::FileCodeLocation {}, "file.sc", sc::src::TextKind::ScdFile);

auto [graph, accepted] = sc::ast::parse(text);

for (const auto& d : graph->diagnostics())
    std::cerr << d;
```

There are three overloads. Besides the whole text, you can parse from a location to the end, or only a range.
The graph keeps the text alive.

`accepted` is false if Bison gave up. A tree with errors can still be accepted, so check `diagnostics()` for errors.
`explicit operator bool` on the graph is false if a fatal diagnostic was added.

## The tree

`ASTGraph` is a typed graph. Every node has a source range and a type, and its children are other nodes.

A lot of template programming is used to ensure that a node's children are typed. 
This means if we alter this structure, all other visitors in the codebase that don't handle this case will be compile time errors.
Additionally, the construction of the nodes is type safe, so the grammar file is checked.
The downside is that these errors are hard to read, so make alterations one at a time, and that compile time can be slow.

This is deemed worth it because having an invalid graph can be fatal.

- `root()` returns a `std::variant` of the three root indexes, and `root_any()` returns an untyped one.
- `location(i)`, `string_view(i)`, and `string(i)` give where a node is and its text.
- `visit(...)` and `children(i)` are the type safe way to look inside a node. 
- `index_to_variant(i)` turns an index of many possible node types into a variant of indexes.
- `is_bracketed(i)` says if the expression was written in brackets, this is needed for default arguments.
- `has_curry()` says if there is a `_` anywhere, this is done for performance reasons.


The best way to traverse the whole graph is with `depth_first_traverse`.
```cpp
graph->depth_first_traverse(*graph->root_any(),
    [&](const ASTGraph::Edges&, const ASTGraph::Variant& node, 
        const sc::src::SourceCodeRange& loc, Index, size_t depth) {

        std::cout 
            << std::string(depth, '\t') 
            << ASTGraph::node_name(node) 
            << " " 
            << text->read(loc) 
            << "\n";
    }
);
```

`depth_first_traverse` has optional callbacks before, after, and on leaving the children (useful for serialising the data to json).
`flat_walk` visits every node in storage order, which is faster when the tree shape does not matter.

## Version comments

A comment such as `//# VERSION 3.16` or `//# VERSION legacy` at the top of a file sets the language version. It is read
by `tokenise.cpp`. A bad or misplaced one is reported as a diagnostic.

## Command line

```
sc_parser_standalone "1 + 2"
sc_parser_standalone --class "Foo { bar { ^1 } }"
```

It prints one node per line, indented by depth, and prints the diagnostics to stderr. The exit code is failure if there
is any error.

## Tests

`tests/tests.cpp` is a Boost test that parses snippets and checks that they are accepted with the expected diagnostics.
