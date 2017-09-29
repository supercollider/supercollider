SuperCollider 3.8 for macOS (OS X)
==================================

These are installation and build instructions for the macOS version of James McCartney's SuperCollider synthesis engine (scsynth) and programming language (sclang).

Pre-compiled releases are available for download at:

https://github.com/SuperCollider/SuperCollider/releases



Table of contents
-----------------

 * Prerequisites
 * Obtaining the source code
 * Build instructions
 * Diagnosing build problems
 * Frequently used cmake settings
 * Using cmake with Xcode or QtCreator
 * Using ccache with Xcode
 * Building without Qt or the IDE
 * sclang and scynth executables

Executables
-----------

`SuperCollider.app` is the IDE (integrated development environment) for writing and executing SuperCollider code.

Inside that application's folder (`SuperCollider.app/Contents/MacOS/`) are the two executables that make up supercollider itself:

`sclang` - the language interpreter including Qt gui
`scsynth` - the audio engine


Prerequisites:
-------------

- **Xcode** can be installed free from the Apple App Store or downloaded from: http://developer.apple.com.
  Xcode >= 7 is recommended. Xcode 6 will work, but requires macOS >= 10.9.4.
- If you do not have the **Xcode command line tools** installed already, install them with:
  `xcode-select --install`
- **homebrew** is recommended to install required libraries
  See http://brew.sh for installation instructions.
- **git, cmake, libsndfile, readline, and qt5.5**, installed via homebrew:
  `brew install git cmake readline qt55`

  *Note*: SuperCollider depends on Qt5WebKit, which was dropped from Qt in 5.6. So make sure you request Qt5.5 (`qt55`) and not the latest version (`qt5`).
  Use `brew info` to see the version of a package, and see `brew switch` or `brew pin` if you want to use Qt5.5 as your default version.

- If you want to build with the *supernova* server, you need **portaudio** package, which can also be installed via homebrew:
  `brew install portaudio`

Obtaining the source code
-------------------------

**Note** Please do not use non-ASCII characters (above code point 127) in your
SuperCollider program path (i.e. the names of the folders containing SuperCollider).
Doing so will break options to open class or method files.

SC is on Github: https://github.com/SuperCollider/SuperCollider

Get a copy of the source code:

    git clone --recursive https://github.com/SuperCollider/SuperCollider.git

`--recursive` specifies that it should also clone the git submodules.

Build instructions
------------------

    cd SuperCollider
    mkdir -p build
    cd build
    cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt55`  ..
    # or, if you want to build with supernova:
    cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt55` -DSUPERNOVA=ON ..
    cmake --build . --target install --config RelWithDebInfo

If successful this will build the application into `build/Install/SuperCollider/`

You can see the available build options with ```cmake -LH```.

To install, you may move this to /Applications or use it in place from the build directory.

More info on *supernova* can be found in the section **Frequently used cmake settings** below.

**Note**: You can also open the produced SuperCollider.xcodeproj in Xcode, and build the "Install" scheme in place of the last step. Do make sure you run the previous configuration steps.

#### Step by step explanation of the Build instructions:

##### Create a `build` folder if one doesn't already exist:

    mkdir -p build
    cd build

##### Prepare for building by making a configuration file:

    cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt55`  ..

(The `..` at the end is easy to miss. Don't forget it!)

This specifies to cmake that we will be using Xcode to build. It also specifies the location of qt so that the complier/linker can find it.
Depending on your configuration, you might need to use `qt5` instead of `qt55`; use `brew info` to confirm you are referring to the correct version.

If you are not using the Homebrew install then you should substitute the path to the parent folder of the bin/include/lib folders in that
Qt tree.

##### Build

    cmake --build . --target install --config RelWithDebInfo

Cmake will build the application looking up configuration information in the file `CMakeCache.txt` in the specified directory
(the current directory: `.` ). By specifying '--target install' you build all targets and trigger the creation of a portable
bundle containing all files contained in the SC distribution. The default install location is `./Install`.

The flag `--config RelWithDebInfo` will build an optimized binary but will still include some useful debug information.

By default Xcode builds the application in debug mode which runs much slower and has a larger application size. It is intended for use with
the XCode debugger. For normal usage you will want an optimized release version.

The four possible build configs are:

- Debug
- RelWithDebInfo
- Release
- MinSizeRel



Diagnosing Build Problems
-------------------------

The most common build problems are related to incorrect versions of the core dependencies, or dirty states in your build folder.

### Checking component versions:

**Xcode**: `xcodebuild -version`, or the "About" dialog of the Xcode application. Any build from the 6.x series or greater should generally work.

**cmake, qt5(.5.x), libsndfile, readline**: `brew info ____` will show you what you have installed - for example, `brew info qt5` should show you the Qt5 version
information. A build using v5.6 and above will fail at the time of this writing because Qt5WebKit is missing in its binary distribution.

`brew upgrade ____` will update the dependency to a newer version (avoid this for Qt5 or handle different Qt5 versions with `brew switch`).

Other common homebrew problems can be fixed using `brew doctor`.


### Dirty build states

While it's generally safe to re-use your build folder, changing branches, build tools, cmake settings, or the versions of your dependencies can sometimes put you in a state where you can no longer build. The solution is to clean your build folder - the common ways to do this, in order of severity:

1. `rm CMakeCache.txt` (delete your cmake settings for that build)
2. `xcodebuild clean --target install` or `make clean` (clean your intermediate build files)
3. `rm -r ./Install` (delete the output of your build)
4. `cd ..; rm -r ./build` (delete your entire build folder)

Removing the CMakeCache.txt should fix most build problems. After running each one of these, you must re-run the two cmake commands shown in the build instructions above (configure and rebuild).

If you wish to build multiple git branches you should usually create a new build folder for each branch you're building. In practice, though, you can usually switch between similar branches and rebuild by simply deleting your CMakeCache.txt.

### Travis continuous integration

The code on github is tested anytime a contributor pushes new changes, so if a mistake was made in the cutting edge development version and something is broken, then you should be able to see this by visiting the Travis status page:

https://travis-ci.org/supercollider/supercollider

If the latest build status is failing, then you can switch your local copy to a previous commit that is still working (until the developers get a chance to fix the problem):

- locate the most recent green build on the travis,
- find it's git commit id (e.g. `595b956`), and
- check out that change in git: `git checkout 595b956`
- build

### If all else fails

Post to the user list stating what git hash you have checked out and all xcode version and library information and most importantly the error messages.

Simply posting "the latest checkout is broken" won't help. We need the exact compile error message.


Frequently used cmake settings
------------------------------

There are more settings in the build configuration you may want to adjust. In order to
see a useful list of your options, you can run:

    cmake -L ..

This configures the build using default settings or settings stored in the file
build/CMakeCache.txt, prints explanatory return statements and produces a list of
variables the value of which you might want to change.

In order to see all the command line options `cmake` offers, type:

    cmake --help

It is not necessary to pass in all required arguments each time you run cmake,
as cmake caches previously set arguments in the file CMakeCache.txt. This is
helpful, but also something to keep in mind if unexpected things happen.

If you feel uncomfortable with the command line, you might want to try cmake
frontends like  `ccmake` or `cmake-gui`. You can also configure your build by
manually editing build/CMakeCache.txt.


Common arguments to control the build configuration are:

  * Control the location where SC gets installed. The following line moves it to the Applications folder
    (which means you need to use `sudo`):

    `-DCMAKE_INSTALL_PREFIX=/Applications`

  * Enable compiler optimizations for your local system

    `-DNATIVE=ON`

  * Build the *supernova* server:

    `-DSUPERNOVA=ON`

    Using supernova requires the `portaudio` audio backend, so you need to install it
    (Homebrew and MacPorts both provide packages).

    *Note*: When you build with supernova, an alternative server executable and a supernova
    version of each plugin is built. If you also use the sc3-plugins package, make sure to
    compile them with supernova support too.

    Within SC you will be able to switch between scsynth and supernova by evaluating one of:

    `Server.supernova`
    `Server.scsynth`

    Check sc help for `ParGroup` to see how to make use of multi-core hardware.

  * Build a 32-bit version (sc 3.6 only):

    `-DCMAKE_OSX_ARCHITECTURES='i386'`

    or combine a 32- and 64-bit version into a bundle (i.e. build a universal binary).
    This is only possible up until macOS 10.6 and requires the dependencies (Qtlibs &
    readline) to be universal builds too:

    `-DCMAKE_OSX_ARCHITECTURES='i386;x86_64'`

  * Homebrew installations of libsndfile should be detected automatically. To link to a
    version of libsndfile that is not installed in /usr/local/include|lib, you can use:

    `-DSNDFILE_INCLUDE_DIR='/path/to/libsndfile/include'`
    `-DSNDFILE_LIBRARY='/path/to/libsndfile/lib/libreadline.dylib'`

  * Normally, homebrew installations of readline are detected automatically, and building with
    readline is only required if you plan to use SuperCollider from the terminal. To link to a
    non-standard version of readline, you can use:

    `-DREADLINE_INCLUDE_DIR='/path/to/readline/include'`
    `-DREADLINE_LIBRARY='/path/to/readline/lib/libreadline.dylib'`


Using cmake with Xcode or QtCreator
-----------------------------------

Xcode projects are generated by appending: `-G Xcode`. The build instructions above use the Xcode toolchain, which is well-tested and generally recommended if you're planning to debug or hack on SC.

You may also want to make the expected SDK-Version and location explicit, using:

`-DCMAKE_OSX_SYSROOT=`

This is often useful to point to an older SDK even with a newer Xcode installed. These are generally located in the

`/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs`

of an Xcode.app package.

You can build without using XCode using `make`, by omitting the `-G Xcode` - in this case, your build command
is `make` rather than `xcodebuild`

Qt Creator has very good `cmake` integration and can build `cmake` projects without requiring a `cmake` generated project file. If you have Qt5 via homebrew installed, you can install Qt Creator by running:

    brew linkapps qt5

Using ccache with Xcode
-----------------------

Although cmake does not support using `ccache` with Xcode out of the box, this project is set up to
allow it with the option `-DRULE_LAUNCH_COMPILE=ccache`. This can speed up build times
significantly, even when the build directory has been cleared.

Building without Qt or the IDE
------------------------------

The Qt framework is used for the SC-IDE, and to provide a graphical toolkit for the sclang language interpreter for users to build their own GUIs.

The 3.8 release does not currently support building on macOS without also building the IDE.
It is also not currently possible to build sclang without Qt.

This should be fixed at some point (its a build tool configuration issue). Until then these build flags do not work on macOS:

`-DSC_IDE=OFF`

`-DSC_QT=OFF`

They do however work on Linux and Windows.


sclang and scynth executables
-----------------------------

The executables `sclang`, `scsynth` and (if available) `supernova` are inside the application bundle:

`SuperCollider.app/Contents/MacOS/sclang`
`SuperCollider.app/Contents/Resources/scsynth`
`SuperCollider.app/Contents/Resources/supernova`

The SuperCollider class library and help files are in:

`SuperCollider.app/Contents/Resources`

In previous versions of SuperCollider these resources lived in the top folder next to SuperCollider.app. To make a standard self-contained app bundle with correct library linking, these have now been moved into the app bundle.

If you need to access them from the Finder, ctrl-click SuperCollider.app and choose "Show package contents" from the context menu.

To access them in the Terminal:

    cd /path/to/SuperCollider.app/Contents/Resources

or

    cd /path/to/SuperCollider.app/Contents/MacOS

##### Adding scsynth and sclang to your path

To have `sclang` and `scsynth` available system-wide, you can create shell scripts and put them somewhere that is in your PATH (eg. `/usr/local/bin` or `~/bin`)

For `sclang`:

    #!/bin/sh
    cd /full/path/to/SuperCollider.app/Contents/MacOS
    exec ./sclang $*

And for `scsynth`:

    #!/bin/sh
    cd /full/path/to/SuperCollider.app/Contents/Resources
    export SC_PLUGIN_PATH="/full/path/to/SuperCollider.app/Resources/plugins/";
    exec ./scsynth $*

###### Why not just symlink them ?

- If you have Qt installed system-wide, sclang will load those as well as the Qt frameworks included in the application bundle. It will then fail with an error message like:

> You might be loading two sets of Qt binaries into the same process. Check that all plugins are compiled against the right Qt binaries. Export DYLD_PRINT_LIBRARIES=1 and check that only one set of binaries are being loaded.
This application failed to start because it could not find or load the Qt platform plugin "cocoa".

- scsynth will not find the included "plugins", unless given explicitly
  with the -U commandline flag or using the SC_PLUGIN_PATH environment variable as shown above.
