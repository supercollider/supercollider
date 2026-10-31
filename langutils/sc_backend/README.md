# sc_backend

Turns the semantic IR from [sc_sema](../sc_sema/README.md) into bytecode for the VM in `lang/`.

It depends on `sc_sema`, `sc_bytecode`, `sc_diagnostic`, and `sc_util` (and `sc_parser` privately).
Everything is in namespace `sc::backend`.
The output is plain data.

Not normally called directly, [sc_compiler](../sc_compiler/README.md) runs the stages for you.

The only reason to call these directly is if you are benchmarking or debugging the output.

This is the stage where we implement low level optimisations.

## The stages

There are three, run one after another for each class or piece of interpreted code (the "root").

| Stage | Input | Output | What it decides |
|---|---|---|---|
| `layout_frames` | the IR | `Frames` | Which function calls create a frame, and the frame and slot of every variable. |
| `lower` | the IR and `Frames` | `lir::LoweredGraph` | The order the VM runs things in. Operands before the send that uses them. Locals, members, and nested functions are resolved. |
| `emit` | `lir::LoweredGraph` | `EmittedGraph` | The bytecodes, the selector and constant tables, jump lengths, method shortcuts, and function flags. |

Each stage can fail with diagnostics (a frame with more than 255 slots, a jump that is too long, and so on).
A function that fails is dropped, or lowered to throw `PoisonedMethodError` if the class library defines it.

## What is in it

| File | Purpose |
|---|---|
| `details/frames.hpp`, `src/frames.cpp` | `layout_frames` and `Frames`. Also `FrameCursor`, which every later walk uses to know which frame it is in. |
| `details/inlining.hpp`, `src/inlining.cpp` | Which sends become jumps (`if`, `while`, `and:`, `switch`, ...), the `Inlined` kinds, and `SpecialSymbols`. |
| `details/members.hpp`, `src/members.cpp` | `MemberTable`, the instance slot or class variable index of every member. |
| `lower.hpp`, `src/lower.cpp` | `lower`. Also `LoweringSymbols`, the selectors lowering sends that the source never wrote (`new`, `performList`, `copyToEnd`, ...). |
| `details/lir.hpp` | The lowered IR (`lir::Instruction` and friends). The contract between `lower` and `emit`. |
| `emit.hpp`, `src/emit.cpp` | `emit`, `EmittedFunction`, and the method shortcuts (`shortcut::`). |

## Frames

A frame is the set of slots the VM creates for one call: `this`, arguments, variadic arguments, variables.
A variable is found by a depth (frames up) and a slot.

An inlined function has no frame, its body runs in the frame around it.
So whether a send is inlined must be decided the same way in `layout_frames`, `lower`, and `emit`.
`FrameCursor` and `InliningContext` exist to keep that single sourced. If you change a rule in `inlining()`, all three stages follow.

Generator qualifiers (`do`, `for`, ...) get frames of their own because the old compiler made them functions.


## The lowered IR

`lir.hpp` is a small stack machine: push, store, `Dup`, `Drop`, `Send`, and returns.
An inlined send is one instruction that points at **bodies**, such as `If { then_body, else_body }`.
A body is a contiguous range of instructions in the same function, so the emitter can measure it to lay out a jump.
Nothing in `lir` says how an instruction is encoded, that is `emit`'s job.

Each instruction has a `SourceSpan`, which is how bytecode gets a line number, and how diagnostics point at code.

## Emitting

`emit.cpp` works in two passes over each body, because the old compiler's choice of opcode depends on what comes after:

1. **Plan** (`plan_body`, `decide`). For each send it picks the opcode form (special selector, special class, `SendSuper`, a tail call, and so on), and which operand pushes can be left out.
2. **Write**. The `Emitter` writes the bytes in order, and fills in the jump lengths from the body sizes.

Other things `emit` does:

- **Selector and constant tables.** Entries are shared and ordered as the old compiler did.
- **Shortcuts.** The VM implements a bunch of flags a method can set, allowing the interpreter to directly evaluate some code.
- **Hard coded loops.** `Integer:do` and the like are replaced by extended opcodes. The list is `SpecialSymbols::hard_coded_loops`.
- **Accessors.** The getters and setters of a class are shortcuts with no source.

## Matching the old compiler

Legacy (pre 3.16) output is meant to match the old compiler byte for byte.
Newer language versions are allowed to differ, such as hoisting variables of inlined functions.

## Where to look to change something

| I want to... | Look at |
|---|---|
| Inline another message, or change when one is inlined | `Inlined` and `inlining()` in `inlining.cpp`, then the `lower` and `emit` cases for the new kind. |
| Add or change an instruction | `lir.hpp`, then `lower.cpp` to make it and `emit.cpp` to encode it. Update `write_instruction`. |
| Change how a particular send is encoded | `decide` in `emit.cpp`. |
| Add a selector or class with its own opcode | `SpecialSymbols` in `inlining.cpp`. |
| Change where a variable lives | `layout_frames` in `frames.cpp`. |
| Change the instance or class variable numbering | `MemberTable` in `members.cpp`. The VM class loader must agree. |
| Add a method shortcut | "Method shortcuts" in `emit.cpp`, and `shortcut::` in `emit.hpp`. |

## Debugging

`sc_inspect` prints each stage:

- `--frames` the frames, their slots, and where each variable is.
- `--lir` the lowered instructions of every function.
- `--bytecode` the tables and bytes.

The same text comes from `write_instruction` and `write_emitted`.
`--no-dump` with one of those runs the backend without printing, which is how to time it.

The regression suite in [tests/error_regression](../tests/error_regression) covers the diagnostics. Add an input there for each new one.
