Notes on building DynCall with CMake
====================================


Build with CMake (in-source)
----------------------------

cd <dyncall-source-dir>

On Unix:
  cmake .
  make

On Windows:
  cmake . -G "NMake Makefiles"
  nmake /f Makefile

  NOTE: Assembly Support is broken for Visual Studio Generators in CMake
        So we support NMake for now only.

Install in a specific location
------------------------------
cmake -DCMAKE_INSTALL_PREFIX=<absolute-path>


Tested settings
---------------
- CMake Unix Makefile generators on Mac OS X and Linux.


Using from other CMake-based projects
-------------------------------------
Under buildsys/cmake/Modules you find some Find*() scripts
which you might find useful.


Make Universal Binary
---------------------
cmake -DCMAKE_OSX_ARCHITECTURES="i386;x86_64;ppc"


CMake Framework
---------------
The project name is "DynCall".
Each library in the source tree represents a target (not a sub-project!).
Support scripts for 'find_package' are at buildsys/cmake/Modules.

find_package( [DynLoad | DynCall | DynCallback] ) 

will set the variables:

	Dyn*_INCLUDE_DIRS
	Dyn*_LIBRARIES


Using dyncall libraries in other CMake projects
-----------------------------------------------
find_package(DynLoad REQUIRED)
add_includes(${DynLoad_INCLUDE_DIRS})
target_link_libraries( ... ${DynLoad_LIBRARIES})


Use as sub-project within CMake top-level project
-------------------------------------------------

add_subdirectory(path/to/dyncall/project/tree)
set(DynLoad_DIR ${DynCall_SOURCE_DIR}/dynload)
set(DynCall_DIR ${DynCall_SOURCE_DIR}/dyncall)
set(DynCallback_DIR ${DynCall_SOURCE_DIR}/dyncallback)

has the effect, that the 'find_package' works from within the project source 
tree.


Building for SPARC64 Architectures
----------------------------------

Supported Compilers: GCC, SunPro
Add -m64 to C, C++ and ASM flags, e.g.

$ cmake -DCMAKE_C_FLAGS=-m64 -DCMAKE_ASM_FLAGS=-m64 -DCMAKE_ASM_CXX_FLAGS=-m64

