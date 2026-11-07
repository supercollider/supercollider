# sc_check_docs

Compiles the code in the help files and the example scd files.
It is the ctest `langutils.docs.compile`.


## Use

Build the `sc_check_docs` target, then give it the class library and any help files, scd files, or directories of them:


```
sc_check_docs [--threads n] [--relative-to dir] [--exclude prefix]... --library SCClassLibrary [dir|file.schelp|file.scd]...
```

- Each `code::` block of a `.schelp` file is compiled as its version, `code#3.16::`, else Legacy. An `.ext.schelp`
  file is parsed as a partial page, as SCDoc.sc does. Inline code is skipped, as it is mostly names and fragments.
- A `code#class::` or `code#3.16#class::` block is class library code. 
- A `code#fragment::` block is only parsed. Use it for code that is meant to be read, not run, e.g., one that uses names it doesn't declare. Still has to be valid code as it is ran through sc_parser.
- A `code#error::` block shows an error, so it fails if it compiles with no errors.
- The parts of a code tag go in this order: the version, `class`, then `fragment` or `error`, e.g.,
  `code#3.16#class#error::`.

Code to evaluate is compiled as a list of regions, the way an `.scd` file is, with no file, as the help browser sends
it. Its diagnostics point into the help file.

The class library is the one sclang compiles in the IDE on this platform: other platforms' directories, `help` and
`ignore` are left out. All platforms at once can't compile, as their directories extend the same classes.

Each compile that reports an error is a failure, printed as `file:line: version:` and its first error. Warnings don't
fail. It exits non-zero on any failure, on a help file SCDoc can't parse, or on errors in the class library.

| Option | Effect |
| --- | --- |
| `--library dir` | The class library. Required. |
| `--exclude prefix` | Skip each file whose name starts with `prefix`. May be given more than once. |
| `--relative-to dir` | Write paths relative to `dir`. The current directory by default. |
| `--threads n` | How many threads to compile with. All of them by default. The output is the same for any number. |
