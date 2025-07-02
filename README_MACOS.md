Welcome to SuperCollider for macOS!
===================================

These are installation and build instructions for the macOS version of James McCartney's SuperCollider synthesis engine (scsynth) and programming language (sclang).

Pre-compiled releases are available for download at:

https://github.com/SuperCollider/SuperCollider/releases

You can also download a bleeding-edge build of the latest development branch
[here](http://supercollider.s3.amazonaws.com/builds/supercollider/supercollider/osx/develop-latest.html).

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

Inside that application are the executables that make up SuperCollider itself:

- `sclang`, the language interpreter including Qt GUI components
- `SuperCollider`, the IDE executable launched by the app
- `scsynth`, the original audio engine
- `supernova`, the newer audio engine which supports multithreading

Prerequisites:
-------------

- **Xcode** can be installed free from the Apple App Store or downloaded from: http://developer.apple.com.
- If you do not have the **Xcode command line tools** installed already, install them with:
  `xcode-select --install`
- **homebrew** is recommended to install required libraries
  See http://brew.sh for installation instructions.
- **git, cmake >= 3.12, libsndfile, readline, and qt6 >= 6.2**, installed via homebrew:
  `brew install git cmake libsndfile readline qt@6`

- If you want to build with the *supernova* server, you need **portaudio** and **fftw** packages, which can also be installed via homebrew:
  `brew install portaudio fftw`

Obtaining the source code
-------------------------

SuperCollider is hosted on Github: https://github.com/SuperCollider/SuperCollider

First, clone the repository with git:

    git clone --recursive https://github.com/SuperCollider/SuperCollider.git

`--recursive` specifies that it should also clone the git submodules.

Build instructions
------------------

    cd SuperCollider
    mkdir -p build
    cd build
    # the default way is to build SC with supernova: 
    cmake -G Xcode -DSUPERNOVA=ON ..
    # or, if you want to build without it:
    cmake -G Xcode  ..
    # then start the build
    cmake --build . --target install --config RelWithDebInfo

If successful this will build the application into `build/Install/SuperCollider/`

You can see the available build options with ```cmake -LH```.

To install, you may move this to /Applications or use it in place from the build directory.

**Qt 5.15 compatibility**: 
**Qt5 is outdated** and will soon be deprecated. It is strongly advised to build with Qt6. If your system also has Qt5 installed you may have to adjust your brew and shell config. In order to build with Qt5, Qt6 needs to be uninstalled or unlinked:

    brew unlink qt@6
    cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt@5' -DSUPERNOVA=ON ..
    cmake --build . --target install --config RelWithDebInfo

More info on *supernova* can be found in the section **Frequently used cmake settings** below.

**Note**: You can also open the produced SuperCollider.xcodeproj in Xcode, and build the "Install" scheme in place of the last step. Do make sure you run the previous configuration steps.

#### Step by step explanation of the Build instructions:

##### Create a `build` folder if one doesn't already exist:

    mkdir -p build
    cd build

##### Prepare for building by making a configuration file:

    cmake -G Xcode ..

(The `..` at the end is easy to miss. Don't forget it!)

This specifies to cmake that we will be using Xcode to build. 

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

**cmake, qt, libsndfile, readline**: `brew info ____` will show you what you have installed - for example, `brew info qt@6` should show you the Qt6 version information.

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

### GitHub Actions continuous integration

The code on github is tested anytime a contributor pushes new changes, so if a mistake was made in the cutting edge development version and something is broken, then you should be able to see this by visiting the GitHub Actions status page for the `develop` branch:

https://github.com/supercollider/supercollider/actions/workflows/actions.yml?query=branch%3Adevelop

If the latest build status is failing, then you can switch your local copy to a previous commit that is still working (until the developers get a chance to fix the problem):

- locate the most recent passing build in the GitHub Actions,
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

  * By default the build will only be compatible with the macOS (and
    subsequent versions) on which the compiler was run. To build with compatibility
    for previous versions of macOS, you can use e.g.:

    `-DCMAKE_OSX_DEPLOYMENT_TARGET=10.10`

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

Qt Creator has very good `cmake` integration and can build `cmake` projects without requiring a `cmake` generated project file. If you have Qt6 via homebrew installed, you can install Qt Creator by running:

    brew install qt-creator

Using ccache with Xcode
-----------------------

Although cmake does not support using `ccache` with Xcode out of the box, this project is set up to
support it. By default, ccache will be used if it's present on your system. This can speed up build times
significantly, even when the build directory has been cleared.

Using `ccache` can be disabled by setting cmake option `-D USE_CCACHE=OFF`.

Building without Qt or the IDE
------------------------------

The Qt framework is used for the SC-IDE, and to provide a graphical toolkit for the sclang language interpreter for users to build their own GUIs.

The 3.8 release does not currently support building on macOS without also building the IDE.
It is also not currently possible to build sclang without Qt.

This should be fixed at some point (its a build tool configuration issue). Until then these build flags do not work on macOS:

`-DSC_IDE=OFF`

`-DSC_QT=OFF`

They do however work on Linux and Windows.

Building with native JACK backend
------------------------------

If you want to use `scsynth` or `supernova` on macOS with JACK, but _without_ the JackRouter driver, you can build them with the native JACK backend. **In that case you will not be able to boot `scsynth`/`supernova` without booting JACK first.** JackRouter allows any macOS application to stream audio to/from JACK, but at the time of writing this (early 2019) it is outdated and the development has stalled.

First you need to install JACK, either through `homebrew`:

```
brew install jack qjackctl
```

or by installing the `JackOSX` package. Please note, JACK from `homebrew` is `jack1` and does _not_ include JackRouter. `JackOSX` is `jack2` and it does include JackRouter. `jack1` and `jack2` implement the same API, but you should have only one of them installed at a time.

In order to build with JACK, you need to add the `-DAUDIOAPI=jack` flag to cmake.

After running cmake configuration proceed with the build process as usual.

### Running SuperCollider with JACK

When `jack` is installed via `homebrew`, you need to add jack's path to the `$PATH` environment variable in `sclang`:

```supercollider
"PATH".setenv("echo $PATH".unixCmdGetStdOut ++ ":/usr/local/bin");
```
Please note, this is not needed when using the JackOSX package.

Optionally, you can have `scsynth`/`supernova` automatically connect to system inputs/outputs by setting appropriate environment variables (refer to the Linux section of the "Audio device selection" reference in SuperCollider help):

```supercollider
// connect all input channels with system
"SC_JACK_DEFAULT_INPUTS".setenv("system");
// connect all output channels with system
"SC_JACK_DEFAULT_OUTPUTS".setenv("system");
```

Now you can start JACK, either using `JackPilot` app (from JackOSX package), `qjackctl` (from `homebrew`) or from command line:


```
jackd -d coreaudio
# or
jackd -d coreaudio -r48000 -p512 # specifying sample rate and buffer size
```

Then start `scsynth`/`supernova` as usual.

### Caveats

JACK installed with `homebrew` can only use a single device for both input and output. In order to use it with the internal soundcard on macOS, one needs to create an aggregate device that includes both input and output. JACK from `JackOSX` does not have this limitation.

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
    export SC_PLUGIN_PATH="/full/path/to/SuperCollider.app/Contents/Resources/plugins/";
    exec ./scsynth $*

###### Why not just symlink them ?

- If you have Qt installed system-wide, sclang will load those as well as the Qt frameworks included in the application bundle. It will then fail with an error message like:

> You might be loading two sets of Qt binaries into the same process. Check that all plugins are compiled against the right Qt binaries. Export DYLD_PRINT_LIBRARIES=1 and check that only one set of binaries are being loaded.
This application failed to start because it could not find or load the Qt platform plugin "cocoa".

- scsynth will not find the included "plugins", unless given explicitly
  with the -U commandline flag or using the SC_PLUGIN_PATH environment variable as shown above.
