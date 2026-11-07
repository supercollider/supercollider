# sc_inspect

A command line tool that runs the compiler over a set of files and dumps what each stage produced.

It is used to compare output between revisions. Used in the regression tests.

## Use

Build the `sc_inspect` target, then give it class files, directories of them, and `.scd` files:

```
sc_inspect [options] [--prelude dir|file.sc]... [dir|file.sc|file.scd]...
```

Each class path is a module. 
A path after `--prelude` is compiled but its classes are not dumped.
Any `.scd` file is compiled as interpreted code after the class library.

| Option | Effect |
| --- | --- |
| `--frames`, `--lir`, `--bytecode`, `--compiled` | Add that backend stage to the dump, in that order. Each runs the stages before it. |
| `--no-dump` | Only compile, and print how many diagnostics and classes there are. For timing, since writing the dump takes longer than compiling. |
| `--time` | Print how long each stage took. |
| `--threads n` | How many threads to compile with. All of them by default. The output is the same for any number. |
| `--relative-to dir` | Write diagnostic paths relative to `dir`, so the output does not depend on where the repo is. |
| `--interpreted-function` | Parse each `.scd` file as the VM's interpreted code (one function), not as a list of regions. |
| `--intrinsic`, `--primitive` | Give a definition the VM would, such as `--intrinsic "Name : Super [slot] a b"` or `--primitive "_Name 2 args kwargs"`. |

## Design

`main.cpp` is the whole tool. It parses the arguments, then calls the same functions sclang does, `sc_compiler/inspect.hpp` for the backend stages, and prints each result with `sc_describe`.
