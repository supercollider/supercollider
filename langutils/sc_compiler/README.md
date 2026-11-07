# sc_compiler

Provides a clean api, Source text in, compiled classes and bytecode out.

Calls the other libraries.


## Layout

| File | Contents |
| --- | --- |
| `sc_compiler.hpp` | What sclang uses: compile the class library, compile its classes, compile interpreted code. |
| `vm_definitions.hpp` | What the VM defines itself, and the compiler must be told. |
| `inspect.hpp` | For tools and tests: each backend stage on its own, compiling from text, and printing the result. |
| `compiled.hpp` | The output: `CompiledGraph`, `CompiledClass`, `CompiledMethod`, and so on. Plain data. |
| `details/backend.hpp` | `Backend`, the state every graph's backend stages share. |
| `details/class_library_internals.hpp` | What `ClassLibrary` holds, for the tools. |

## Design

### The stages

```
files ─ sc_parser ─ sc_sema ─ member table ─ layout_frames ─ lower ─ emit ─ finish ─ CompiledGraph
        └──────────── compile_class_library ─────────────┘   └────── compile_class_graphs ──────┘
```

1. `compile_class_library` parses every file and compiles every class to IR, then builds the `Backend`. The result is a `ClassLibrary`, which is the state kept for the life of the VM.
2. `compile_class_graphs` runs the backend stages on each class graph and returns a `CompiledGraph` for each class that is not poisoned.
3. Interpreted code (an editor's text, an scd file) goes through `parse_interpreted`, `compile_interpreted_ir`, then `compile_interpreted`, against an existing library.

`inspect.hpp` exposes the backend stages as separate functions (`layout_frames`, `lower`, `emit`, `finish`). 
Each one takes the last one's result by move and keeps it, so a tool can stop after any stage and look at everything so far.
Diagnostics stay in the result of the stage that made them.

The class library is compiled ahead of time and does not change. 

Most users only need the small public API on `ClassLibrary`. 

Tools and tests can read `internals`.

### The VM tells the compiler what it needs

The compiler cannot know which classes have a layout the VM relies on, or which primitives exist. `VMDefinitions` carries both, as strings, because the library's symbols do not exist yet when it is given.

## Adding new behaviour

Decide first where the change belongs. 
Most changes are not made in this library.

- **A new language feature or check**: parser or `sc_sema`. Nothing changes here unless the output needs a new field.
- **A new optimisation or lowering**: `sc_backend`. Nothing changes here unless it adds a stage.
- **A new instruction**: `sc_bytecode`, see its README.

Changes that do belong here:

1. **New information from the VM**: add the field to `vm_definitions.hpp` and use it in `compile_class_library`, where `intrinsic_constraints` turns it into constraints for sema.
3. **A new backend stage**: add a result struct in `inspect.hpp` that holds the previous one, a function that moves it in, and call it in `compile_class_graphs` and `compile_interpreted`. Both paths must run it.
4. **New state shared by every graph**: add it to `Backend` in `details/backend.hpp`. It is built once, after the library compiles. Treat it as read only afterwards, since graphs are compiled in parallel.
5. **A new entry point**: declare it in `sc_compiler.hpp` if sclang needs it. Declare it in `inspect.hpp` if only tools do.

Keep `sc_compiler.hpp` small. Anything a tool needs that sclang does not goes in `inspect.hpp`.

