# CMake Generation Scripts

## `generate_server_plugin_cmake.py`

Requires Python >= 3.2

The purpose of this program is to automatically generate a CMakeLists.txt file for a set of
SuperCollider server plugins.

The generated file has dependencies; you should install both `SuperColliderServerPlugin.cmake` and
`SuperColliderCompilerConfig.cmake` to the `project_root/cmake_modules` directory.

### Usage

See --help for all options; many are not covered here.

There are two standard project layouts supported by this script: *modular* and *flat*. In modular
mode, there is one plugin unit - C++, sc, and optionally schelp file - per directory. In flat mode,
things are "flopped" - there is one directory that contains all the C++ files, one that contains all
the sc files, and one for the schelp files. All three directories may be the same.

A modular project looks like this:

  root
  |-- README, COPYING, etc.
  |-+ src
    |-+ Plugin1
    | |-- Plugin1.hpp
    | |-- Plugin1.cpp
    | |-- Plugin1.schelp
    | .-- Plugin1.sc
    .-+ Plugin2
      |-- Plugin2.hpp
      |-- Plugin2.cpp
      |-- Plugin2.schelp
      .-- Plugin2.sc

And you would run this program from `root` as follows:

    python3 generate_server_plugin_cmake.py . --project-name MyPlugins --plugins src/Plugin1 src/Plugin2

or more simply:

    python3 generate_server_plugin_cmake.py . -P MyPlugins -p src/Plugin1 src/Plugin2

This will generate a CMakeLists.txt file that can build two plugins, named Plugin1 and Plugin2.
These names are based on the names of the directories.

A flat project looks like this:

  root
  |-- README, COPYING, etc.
  |-+ Source
  | |-- Plugin1.cpp
  | |-- Plugin1.hpp
  | |-- Plugin2.cpp
  | .-- Plugin2.hpp
  |-+ Classes
  | |-- Plugin1.sc
  | .-- Plugin2.sc
  .-+ Help
    |-- Plugin1.schelp
    .-- Plugin2.schelp

And you would use this script like this:

    python3 generate_server_plugin_cmake.py . --project-name MyPlugins --flat-dir Source \
        --sc-files-dir Classes --schelp-files-dir Help

or more simply

    python3 generate_server_plugin_cmake.py . -P MyPlugins -f Source -C Classes -H Help

There are many other, more self-explanatory flags you can use; check the output of `--help` for more
info.

Although this program is meant for plugins that use the preferred C++ plugin API, it can also be
used on projects that are C-only; .c, .cpp, .cxx and the header counterparts are all recognized as
valid C++ source file extensions.

The `--install-cmake` and `--install-cmake-only` options assume that the CMake modules to copy are
in the same directory as the script.

### Generated CMake

The generated CMake file has the following options and features:

- `SC_PATH` - path to SC source relatively to main CMakeLists.txt. Defaults to `../supercollider`.
- `NATIVE` - optimize for native architecture - off by default
- `STRICT` - use very strict compiler settings while compiling - off by default
- `SCSYNTH` - build plugins for scsynth - on by default
- `SUPERNOVA` - build plugins for supernova - on by default

Even if `SCSYNTH` and `SUPERNOVA` are both off, sc and schelp files will still be included in the
`install` target.

At the end of the script, a CMake variable `all_sc_server_plugins` contains the names of all server
plugin targets, to make it easy to iterate over them in user-provided code.
