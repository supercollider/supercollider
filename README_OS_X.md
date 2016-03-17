SuperCollider 3.7 for OS X
==========================

These are installation and build instructions for the Mac OS X version of James McCartney's SuperCollider synthesis engine (scsynth) and programming language (sclang).

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
 * Building without Qt or the IDE
 * sclang and scynth executables
 * On libsndfile

Executables
-----------

`SuperCollider.app` is the IDE (integrated development environment) for writing and executing SuperCollider code.

Inside that application's folder (`SuperCollider.app/Contents/MacOS/`) are the two executables that make up supercollider itself:

`sclang` - the language interpreter including Qt gui
`scsynth` - the audio engine


Prerequisites:
-------------

- **Xcode** can be installed free from the Apple App Store or downloaded from: http://developer.apple.com

  - Xcode 5 may work
  - Xcode 6 is known to work - it requires a Mac running OS X version 10.9.4 or later or 10.10
  - Later versions should definitely work

- **Xcode command line tools** must be installed - after installing Xcode, this can be done from the Xcode preferences or from the command line:
  `xcode-select --install`
- **homebrew** is recommended to install required libraries
  See http://brew.sh for installation instructions.
- **git, cmake, readline, and qt5**, installed via homebrew:
  `brew install git cmake readline qt5`

  As of this writing the latest stable Qt is 5.5.x. The 5.6 beta has also been reported to work fine.

Obtaining the source code
----------------------------

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
    cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt5`  ..
    cmake --build . --target install --config RelWithDebInfo

If successful this will build the application into `build/Install/SuperCollider/`

To install, you may move this to /Applications or use it in place from the build directory.

**Note**: You can also open the produced SuperCollider.xcodeproj in Xcode, and build the "Install" scheme in place of the last step. Do make sure you run the previous configuration steps.

#### Step by step explanation of the Build instructions:

##### Create a `build` folder if one doesn't already exist:

    mkdir -p build
    cd build

##### Prepare for building by making a configuration file:

    cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt5`  ..

This specifies to cmake that we will be using Xcode to build. It also specifies the location of qt so that the complier/linker can find it. Qt was installed with brew, so we ask brew to tell us where it installed it to.

If you are not using the Homebrew install then you should substitute the path to parent folder of the bin/include/lib folders in that Qt tree.

##### Build

    cmake --build . --target install --config RelWithDebInfo

Cmake will build the application, looking for the `CMakeCache.txt` configuration file in the specified directory (the current directory: `.` ).

The operation it will build is named 'install'.

The flag `--config RelWithDebInfo` will build an optimized binary but will still include some useful debug information.

By default Xcode builds the application in debug mode which runs much slower and has a larger application size. Its intended for use with the XCode debugger. For normal usage you will want an optimized release version.

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

**cmake, qt5, readline**: `brew info ____` will show you what you have installed - for example, `brew info qt5` should show you the Qt5 version information.

`brew upgrade ____` will update the dependency to a newer version.

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

  * Build a 32-bit version:

    `-DCMAKE_OSX_ARCHITECTURES='i386'`

    or combine a 32- and 64-bit version into a bundle (i.e. build a universal binary).
    This is only possible up until OSX 10.6 and requires the dependencies (Qtlibs &
    readline) to be universal builds too:

    `-DCMAKE_OSX_ARCHITECTURES='i386;x86_64'`

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


Building without Qt or the IDE
------------------------------

The Qt framework is used for the SC-IDE, and to provide a graphical toolkit for the sclang language interpreter for users to build their own GUIs.

The 3.7 release does not currently support building on OS X without also building the IDE.
It is also not currently possible to build sclang without Qt.

This should be fixed at some point (its a build tool configuration issue). Until then these build flags do not work on OS X:

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


On libsndfile
-------------

In the past compiling a universal binary of libsndfile used to require accessing both a
i386 and PPC Mac. The reasons for this are described here:

http://www.mega-nerd.com/libsndfile/FAQ.html#Q018

Because of this, libsndfile is included with the source as a precompiled
universal binary. This UB contains ppc, i386 and x86_64 archs. By default SC uses this
file, although the currently built universal does not contain Power PC versions any more
and intel universal binaries of libsndfile are readily available via package managers.

If you would like to build using the latest version of libsndfile, then specify its path to the cmake configuration:

`-DSNDFILE_INCLUDE_DIR=/opt/local/include`
`-DSNDFILE_LIBRARY=/opt/local/lib/libsndfile.dylib`

The actual paths you need to include will vary depending on how you installed libsndfile.
