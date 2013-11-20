Supercollider 3.6 for OS X
==========================

This is the Mac OS X version of James McCartney's SuperCollider
synthesis engine (scsynth) and programming language (sclang).
Supercollider's main homepage is at:

http://supercollider.github.com

The help files contain a lot of useful information and tutorials for getting
started - see the file "Help.html" for a starting point.

To get further information on SuperCollider usage or development, you
should subscribe to the mailing lists:

http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml


NOTE: OS X Mavericks (10.9)
---------------------------

This version of SC has not been adjusted to Mavericks yet. Please expect some rough
edges. Most importantly: "App Nap", a new energy saving feature for background applications,
causes the SC language interpreter to slow down dramatically after a while, if no sc-gui
window is in the foreground. It is therefore strongly recommended to switch off App Nap
for SuperCollider. The easiest way to do this, is to right-click SuperCollider.app and
select `Get Info` from the popup-Menu. Then choose `Prevent App Nap` in the list of start
options.


Special characters on mac
-------------------------

Please do not use non-ASCII characters (above code point 127) in your
SuperCollider program path (i.e. the names of the folders containing SuperCollider).
Doing so will break options to open class or method files automatically.


Compiling SuperCollider from the source code
--------------------------------------------

Requirements:
 * Mac OS X 10.6 or greater
 * Cmake 2.8.11 or greater
 * clang-3.3 or apple clang
 * Qt libraries 4.8.5 or greater: http://qt-project.org/downloads

To build SuperCollider with Cmake, it is recommended to create a "build"
folder (to keep the built files neatly all together) in the root of the
SuperCollider source, then run "cmake" from within that folder. Like this (to
build using make):

```
$> cd ~/SuperCollider3
$> mkdir build
$> cd build
$> cmake ..
```

Then to run the build process run:

```
$> make install
```

This will build the software, then "install" it to a folder "SuperCollider"
under CMAKE_INSTALL_PREFIX, which defaults to "<build-directory>/Install",
with the finished products in. You might like to move the "SuperCollider"
folder into your /Applications folder or install it there directly by
passing `-DCMAKE_INSTALL_PREFIX=/Applications` to cmake.

Alternatively, to build using Xcode:

```
$> cmake .. -GXcode
```

will generate an Xcode project within the build directory. Within Xcode, select
the `install` target and build.

The build process can be configured using the cmake program, cmake
frontends like "ccmake" or "cmake-gui", or by simply editing the
`build/CMakeCache.txt` file.

For example, by default cmake will create a "release" build, but if you want a
"debug" build (with optimisations turned off etc), configure cmake like this:

```
$> cmake -DCMAKE_BUILD_TYPE=Debug ..
```

By default the SCClassLibrary is copied into place. There is a cmake option to create
symlinks of the SCClassLibrary instead so that changes to code in the class library are reflected in git.
To turn on the symlinking do:

```
$> cmake -DSC_SYMLINK_CLASSLIB=ON ..
```

For a 32bit build on a 64bit machine (i.e. recent Mac systems), you need to
configure cmake like this:

```
$> cmake -DCMAKE_OSX_ARCHITECTURES='i386' ..
```

For the BIG universal binary (on 10.6), use:

```
$> cmake -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' ..
```


Qt GUI
------

By default the new Qt GUI support will be built into sclang and
SuperCollider application. If you want to build without it, configure
cmake like this:

```
$> cmake -DSC_QT=OFF ..
```


Standalones
-----------

To create a "standalone" app using cmake, you need to use the "standalone" flag.
For example, to create a standalone whose name is MyFabApp:

```
$> cmake -D standalone="MyFabApp" ..
```

This builds like normal but names the finished program "MyFabApp" and includes
any special resources you might have created in the folder
`platform/mac/MyFabApp Resources` (or, if that doesn't exist, it just uses
`platform/mac/Standalone Resources`).

If you've built a standalone and want to go back to normal build mode, just
set that value as an empty string:

```
$> cmake -D standalone="" ..
```


On libsndfile
-------------

Compiling a universal binary of libsndfile requires access to both a
i386 and PPC Mac. The reasons for this are described here:

http://www.mega-nerd.com/libsndfile/FAQ.html#Q018

Because of this, libsndfile is included with the source as a precompiled
universal binary. This UB contains ppc, i386 and x86_64 archs.


Outro
-----

Thanks to James McCartney, for making this great piece of audio software publicly and freely available.
