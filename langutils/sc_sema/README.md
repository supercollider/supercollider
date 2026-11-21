# sc_sema

Semantic analysis. Turns the parser's `ASTGraph` into an `IRGraph`, one per class, or one per piece of interpreted code.

Everything is in namespace `sc::ir`. The IR is what [sc_backend](../sc_backend/README.md) lowers, and it is the last stage that keeps types and structure.

This process produces several class library wide stuctures stored in a context struct.
There is information about the classes, but also a symbol and constants table.

## Layout

| File | Contents |
| --- | --- |
| `compiler.hpp` | `IRCompiler`, the entry point: compile the class library, then interpreted code. |
| `context.hpp` | `Context`, what a compilation shares (symbols, constants, declared classes), made with `make_context`. |
| `ir.hpp` | `IRGraph`, and its tables of local and member variables. |
| `symbol_table.hpp` | Where symbols are interned. |
| `module.hpp` | `ModuleTable`. A module is just a path. |
| `details/ir_nodes.hpp` | Every kind of IR node, each with its documentation. |
| `details/const_expr.hpp` | Where the constants are stored. |
| `details/node_collection.hpp`, `details/indexes.hpp` | The list of node kinds, their enum, and the typed indexes. |
| `details/versions.hpp` | The rules that differ between language versions. |
| `details/intrinsic_constraint.hpp` | What the VM requires of certain classes. |
| `src/ast2ir/` | The translation from the AST. One file per area, `expr/` has one per kind of expression. |
| `src/known_methods.*` | Methods the compiler assumes it knows, checked for extensions and overrides. |
| `utils/describe`, `utils/consteval` | `describe` writes IR nodes as text, shared by `sc_inspect` and `sc_explorer`. `consteval` is a small tool. |

## Design

### The stages

1. `make_context` creates the shared state. Class declarations are collected from the parsed files first, so every class can see every other.
2. `IRCompiler::compile_class_library` compiles every class, in parallel where it can. There is one `IRGraph` per class.
3. `IRCompiler::compile_interpreted` compiles interpreted code against the finished library. It is compiled as if written inside `Interpreter:functionCompileContext`. Several can be compiled at once, since each only reads the library.

### The IR is a typed graph

*These use the same tempalted graph structure that the AST uses.*

Each node kind is a struct in `ir_nodes.hpp`, and its type says what children it accepts:

```cpp
struct SCRange : priv::Node<SCRange_I(SCAnyExpr_I first, maybe<SCAnyExpr_I> second, maybe<SCAnyExpr_I> last)>
```

The graph holds its own text, so the AST can be dropped once it is built.


The IR is more uniform than the AST. 
A block and a function are both an `SCFunction`, and the receiver and arguments of a message are the same shape.
Features that could be lowered to messages (`SCRange`, `SCPackAssign`, generators) are kept as nodes, so the backend can choose how to lower them.

This graph is constructed by visisting the ast graph, see the ast2ir directory.

Several passes are taken when doing this, some (like the curray args) are slower than others.
Be careful when introducing new passes.

### Errors

Diagnostics are collected, not thrown. 

A method or class with a fatal error is *poisoned*. 
A poisoned class has no graph, and `depth_first_do` skips it. 
In the class library a poisoned method still exists, so the backend can make it throw an error when called.

### Language versions

Code written for older releases keeps the old compiler's behaviour (so long as it wasn't a bug). 
`versions.hpp` names each rule by what it decides (`can_fold_constants`, `is_frame_scoped`, ...). 
Don't check the version number.

### Known methods

Constant folding and some rewrites assume what a library method does, such as `copySeries` on an `Array`. 
Extensions can replace them after the classes are declared, so `KnownMethods` checks each assumption once. 

## Adding new behaviour

**A new kind of node**

1. Add the struct to `ir_nodes.hpp`, with a `///` comment and its child types.
2. Add it to `SCNodeType` and the index aliases in `indexes.hpp`.
3. Add it to `NodeCollectionHelper` in `node_collection.hpp`, in the same order as the enum.
4. Build it in `src/ast2ir/`.
5. Print it in `utils/describe`. Without this the tools will not show it.
6. Handle it in [sc_backend](../sc_backend/README.md), in `layout_frames` and `lower`.


**Behaviour that differs by version**: add a function to `versions.hpp` that says what it decides, and use it.

**Something the VM requires of a class**: add an `IntrinsicConstraint`. It is checked by `check_intrinsic_constraint`. The VM supplies these through [sc_compiler](../sc_compiler/README.md).

**A new method the compiler assumes**: add it to `KnownMethods`.

Every new diagnostic or IR change needs a regression test, an input in `tests/error_regression/input/`.
