The SuperCollider Server Plugin API
===================================

This document is mostly aimed at plugin authors. SC developers who wish to make
changes or additions to the plugin API are advised to also read DEVELOPING.md.

# Overview

The SC plugin API is implemented as a collection of header files that contain
several structs, constants, enums, macros, helper functions and C++ helper classes.

Here are the most relevant files:

- `SC_PlugIn.h`: this is the only file that most plugins have to include.
  It transitively includes most other API headers.
  
- `SC_Unit.h`: defines the `Unit` struct, which is the base class for all Unit
  generators (UGens), together with some macros for convenient member access and
  thread-safe audio/control bus and Buffer access.
  
- `SC_PlugIn.hpp`: C++ wrapper class around the `Unit` struct. This allows to use
  regular C++ member functions instead of C macros.

- `SC_InterfaceTable.h`: contains the interface table with API function pointers
  together with several convenience macros. Also provides the `PluginLoad` macro
  for defining a plugin's entry point function.

- `SC_World.h`: defines the `World` struct, which holds the state of a particular
  Server instance. This is used implicitly by various macros. Plugin authors
  rarely need to access the `World` instance directly.

- `SC_SndBuf.h`: defines the `SndBuf` struct, which holds the properties and
  data of sound buffer (aka Buffer), together with some inline helper functions.

- `SC_Rate.h`: defines the `Rate` struct, which contains the properties of a
  UGen's rate (scalar rate, control rate, audio rate or demand rate), such as
  the sample rate or slope factor.

- `SC_Wire.h`: defines the `Wire` struct, which provides more information about
  the UGen's inputs. Typically, this is used for checking the input's rates,
  such as in the `INRATE` macro or the `SCUnit::inRate()` member function.

- `sc_msg_iter.h`: the `sc_msg_iter` struct holds the OSC message arguments
  for asynchronous (unit) commands and bufgen commands.

- `SC_RGen.h`: defines the `RGen` struct, a pseudo-random number generator,
  with many C++ member functions for generating pseudo-random numbers of
  different numeric types and distributions.

- `SC_Complex.h`: provides fast inline functions for converting FFT buffers
  between complex and polar coordinates.
  NOTE: this is *not* automatically included by `SC_PlugIn.h` or `SC_PlugIn.hpp`

- `SC_Graph.h`: defines the `Graph` struct. This is used indirectly by many
  macros and inline functions; plugins rarely need to access it directly.

- `SC_DemandUnit.h`: contains helper functions and macros for demand rate UGens.

- `SC_BufGen.h`: defines the prototype for bufgen functions.

- `SC_Command.h`: defines the prototypes for plugin command and unit command
  functions as well as asynchronous (unit) command stage functions.

- `SC_FifoMsg.h`: defines the `FifoMsg` which is used to exchange messages
  between RT thread and NRT thread. See the `SendMsgFromRT` and `SendMsgToRT`
  API functions.

- `SC_Node.h`: defines the `Node` struct, which is the "base class" of `Graph`.
  This is only included because `Graph` needs to see the definition of `Node`.


Utilities:

- `SC_InlineBinaryOp.h`: inline helper functions for binary math operations.

- `SC_InlineUnaryOp.h`: inline helper functions for unary math operations.

- `SC_Constants.h`: some useful mathematical constants.


For the full API documentation, consult the relevant header files.


# API functions

API functions are not directly exported from scsynth, instead they are implemented
as function pointers in the `InterfaceTable` struct (see `SC_InterfaceTable.h`).
This has the advantage that Server plugins only require the API headers, but they
do not have to link against a shared library.

Every function pointer in the interface table has a corresponding macro that assumes
that the interface table is stored in a (static) global variable named `ft`.
This allows to call API functions similar to free functions. For example,
`RTAlloc(world, size)` is the same as `ft->fRTAlloc(world, size)`.

For more information about the various API functions, see `SC_InterfaceTable.h`.

SC developers may add new API functions to the interface table to provide more
functionality to plugins. To prevent potential issues, the developer must mark these
as API extensions and they must explicitly enabled by the plugin author.
See the "Plugin versioning" section below for more information.

# Server plugins

A Server plugin (sometimes also called 'SC extensions') is a dynamic library (DLL on
Windows, dylib on macOS, shared object on Linux) with a special file extension (`.scx`).
The Server automatically searches for `.scx` files in platform-specific default directories.
Alternatively, the user may also provide their own search paths to scsynth or Supernova.

A Server plugin can do any of the following things:

- register one or more UGen definitions with the `DefineUnit`, `DefineDtorUnit`,
  `DefineSimpleCantAliasUnit` or `DefineDtorCantAliasUnit` macros.
  The C++ wrapper can also use the `registerUnit()` helper function template.
  
- register one or more plugin commands with `DefinePlugInCmd`

- register one or more unit commands with `DefineUnitCmd` or `DefineUnitCmdEx`

- register one or more bufgen commands with `DefineBufGen`

NOTE: you do *not* have to create a separate plugin binary for every UGen plugin;
instead you can register several UGen definitions in a single binary.

### The entry point

When the Server dynamically loads a plugin, it tries to find an exported function called
`load()`, which is the plugin's *entry point*. The Server calls this function with a pointer
to the interface table as the first argument. The plugin typically stores this pointer in
a (static) global variable called `ft`, as explained in the previous section.

Since SC 3.15, the `load()` function also receives a context object as a second argument
which contains the plugin binary directory as well as the plugin resource directory.
This can be useful if the plugin requires additional resourches, such as sound files,
textfiles or binary data, that are bundled together with the extension.

`PluginLoad` is a macro that defines and exports the entry point function together with
some other functions that are required for API versioning (more about that later).
The actual signature of the generated entry point function is
`void load(InterfaceTable* inTable, const ServerPluginContext* inContext)`.

```cpp
static InterfaceTable* ft;

PluginLoad(MyPlugin) {
    // assign interface table
    ft = inTable;

    // for example, read files in the resource directory
    readDataFiles(inContext->mResourceDir);

    // define UGens or commands...
}
```

Side note: the plugin's resource directory can be modified at compile time with the
SC_PLUGIN_RESOURCE_DIR define. See SC_InterfaceTable.h for further documentation.

# API versioning

A plugin API is typically not set in stone from day one, it rather evolves over time.
Let's say we want to add a new member to an existing struct, e.g. a new interface function.
If Server and plugin are built against different header versions, one side might attempt
to read or set a struct member that does not actually exist, leading to unexpected results
or even crashes.

To solve this issue, the SC plugin API has a *major* and *minor* API version:

- The *major* version is incremented whenever there is an unavoidable ABI break,
  such as changing the types of existing members. For example, this happened in SC 3.6
  when some members of `Unit` were changed from `uint16` to `uint32`.
  
  The Server only loads plugins of the same *major* API version. This means that major
  API version bumps should be avoided as much as possible because it requires all existing
  plugins to be recompiled!
  
- The *minor* version is incremented when the API is merely extended by adding new
  members to the end of an existing struct. Such new members are called *API extensions*
  and must be wrapped in `SC_API_EXTENSION` blocks.

  After incrementing the *minor* API version, the Server can still load existing plugins
  because it knows that these plugins would not access any of the new extensions.


There are two ways for plugins to access new API extensions:

### Compile time versioning

The plugin can set `SC_MIN_REQUIRED_API_VERSION` to the *minimum* required API version.
Only extension up to this API version are accessible to the plugin. Any attempts to use
later extensions will lead to a compilation error. Since API extensions are 'unlocked'
at compile time, we call this *compile time versioning*.

A plugin is only loaded if its `SC_MIN_REQUIRED_API_VERSION` is *less than or equal to*
the Server's API version, making sure that all required members are indeed present.

NOTE: make sure to set `SC_MIN_REQUIRED_API_VERSION` for all your headers and sources,
ideally via your build system.

### Runtime versioning

Setting `SC_MIN_REQUIRED_API_VERSION` to a recent version can limit backwards compability
with older Server versions. If an API extension is only needed for an optional feature,
of if the plugin can provide a meaningful fallback implementation, you can rely on
*runtime versioning* instead. This can ease the transition to newer Server versions and
help user adoption.

Runtime versioning is implemented via so-called 'versioned structs'. A plugin can use
the macro `SC_IMPLEMENTS_FIELD` to check *at runtime* whether a particular struct member
is actually implemented by the Server.

For safety reasons, this feature must be enabled explicitly by setting
`SC_ENABLE_ALL_API_EXTENSIONS` to 1.
Any field that is *newer* than `SC_MIN_REQUIRED_API_VERSION` must then be checked with
`SC_IMPLEMENTS_FIELD`!

### Code example

Server plugin API:
```cpp
// NOTE: this is a fictional example because at the time of writing
// (API v4.0) there are no API extensions yet.
// TODO: replace with a real example in API v4.1.
struct Unit {
    size_t mStructSize;
    World* mWorld;
    // etc.
#if SC_API_EXTENSION(4, 1)
    // added in API version 4.1
    int mFoo;
#endif
#if SC_API_EXTENSION(4, 2)
    // added in API version 4.2
    int mBar;
    int mBaz;
#endif
};
```

Plugin code:
```cpp
// Enable *required* extensions up to API v4.1.
// The plugin can only be loaded in a Server with API v4.1 or higher.
// In practice, you should set this via your build system.
#define SC_MIN_REQUIRED_API_VERSION SC_MAKE_API_VERSION(4, 1)

unit->mFoo = 1; // ok
unit->mBar = 2, // compile error, 4.2 extensions not enabled

// Enable *optional* extensions. Now you can also access fields from API version 4.2
// (or higher) while still being compatible with API version 4.1.
// Again, you should set this via your build system.
#define SC_ENABLE_ALL_API_EXTENSIONS 1

unit->mFoo = 1; // still ok
// Accessing fields from API version 4.2 (or higher) requires a runtime check!
if (SC_IMPLEMENTS_FIELD(unit, Unit, mBar))
    unit->mBar = 2; // ok
```
