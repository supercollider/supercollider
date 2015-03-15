Supercollider 3.7 for OS X
==========================


Table of contents
-----------------

 * Introduction
 * The SC 3.7 release
 * Obtaining the SC source
 * **Quick steps**
 * Generic build instructions
 * Frequently used cmake settings
 * Using cmake with Xcode or QtCreator
 * Remarks on different SC versions
 * Qt vs. Cocoa, IDE vs. Editor "classic"
 * Standalones
 * On libsndfile
 * Special characters on mac
 * Outro


Introduction
------------

This is the Mac OS X version of James McCartney's SuperCollider synthesis engine 
(scsynth) and programming language (sclang). http://supercollider.github.io/

The help files contain a lot of useful information and tutorials for getting
started. If you are using the SuperCollider ide, you can find the documentation using Cmd+D, or via the Help -> 	Show Help Browser menu.

In a non-IDE context, you must generate the help files first, by running:
`SCDoc.indexAllDocuments`
Help files will be created in the app support dir (on Mac, `~/Library/Application Support/SuperCollider/Help`)

To get further information on SuperCollider usage or development, you should subscribe 
to the mailing lists:

http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml

The SC 3.7 release
--------------------

The release of SC v3.7 was built on OS X Mavericks (10.9.5) using the following tools and dependencies:

  * Cmake 2.8.11
  * Xcode 6.1, using apple clang v6.0
  * Qt libraries 5.4.1 as provided by homebrew

SuperCollider 3.7 has been verified to build with Qt versions back to 5.2 (possibly earlier) - however, there is risk of SC-affecting bugs in prior releases of Qt.

SC 3.7 mac was built using travis-ci with following cmake arguments:

    cmake -DCMAKE_PREFIX_PATH=`brew --prefix qt5` -DSC_QT=1 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 .. --debug-output`

These are specified in the .travis.yml file - this file represents a known-good set of steps to build in a clean development environment.

To produce a 3.7 release build, sync to the `Version-3.7` tag in github.  Prerelease-ready builds will be produced from `Version-3.7-beta` series of tags. 

***Note**: the Version-3.7 tag will not exist until a final official build has been released.*

Obtaining the SC source
-----------------------

SC is on Github: https://github.com/supercollider/supercollider

Get a copy of the source code with:

`git clone --recursive https://github.com/supercollider/supercollider.git`

Snapshots of release-versions are available as GitHub releases:

https://github.com/supercollider/supercollider/releases

All build instructions assume that you have installed git locally and use the clone from the 
Github repository.

Quick steps
-----------

**Prerequisites:**

- **Xcode 6.1** can be installed free from the Apple App Store, or downloaded from http://developer.apple.com
- **Xcode command line tools** must be installed - after installing Xcode, this can be done from the Xcode preferences, or from the command line using: `xcode-select --install`
- **[homebrew](http://brew.sh)** is recommended to easily install required libraries:
 `ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"`
- **git, cmake, readline, and qt5**, installed via homebrew: `brew install git cmake readline qt5`:
 
Once those dependencies are satisfied, the following steps will build Supercollider: 

	$>	git clone --recursive https://github.com/supercollider/supercollider.git
	$>	cd supercollider
	$>	mkdir build; cd build
	$>	cmake -G Xcode -DCMAKE_PREFIX_PATH=`brew --prefix qt5`  ..
  $>  cmake --build . --target install --config Debug

A successful build will result in a SuperCollider.app and components at `build/Install/SuperCollider`

**Note**: By default, Xcode builds will be Debug - the four possible build configs are Debug, RelWithDebInfo, Release, and MinSizeRel.

**Note**: You can also open the produced SuperCollider.xcodeproj in Xcode, and build the "Install" scheme in place of the last step.

**Note**: For a manually installed version of Qt5, use a full path to that Qt install in place of the `` `brew --prefix qt5` ``


Diagnosing Build Problems
------------------------------

The most common build problems are related to incorrect versions of the core dependencies, or dirty states in your build folder.

### Checking component versions:

**Xcode**: `xcodebuild -version`, or the "About" dialog of the Xcode application. Any build from the 5.x and 6.x series should generally work. 

**cmake, qt5, readline**: `brew info ____` will show you what you have installed - for example, `brew info qt5` should show you the Qt5 version information.

`brew upgrade _____` will update the dependency to a newer version. Be cautious!  Homebrew is not set up to allow easy installation of older versions, so only upgrade when you need to. Other homebrew problems can be fixed using `brew doctor`. 

When asking for build help, always check and mention the installed versions any components you used to build SC!

### Dirty build states

While it's generally safe to re-use your build folder, changing branches, build tools, cmake settings, or the versions of your dependencies can sometimes put you in a state where you can no longer build. The solution is to clean your build folder - the common ways to do this, in order of severity:

1. `rm CMakeCache.txt` (delete your cmake settings for that build)
2. `xcodebuild clean -scheme install` or `make clean` (clean your intermediate build files)
2. `rm -r ./Install` (delete the output of your build)
3. `cd ..; rm -r ./build` (delete your entire build folder)

Generally, clearing the CMakeCache.txt should be enough to fix many build problems. After each one of these, you must re-run the cmake command and rebuild. It's recommended that you create a new build folder for each branch you're building. In practice, though, you can usually switch between similar branches and rebuild by simply deleting your CMakeCache.txt.

### Someone else's fault...
If you're *sure* you're doing everything right and you're still failing, check the travis-ci status page:
[https://travis-ci.org/supercollider/supercollider]()

Travis does incremental mac and linux builds for every git commit, and can show you if the place you are currently synced (`git log -n 1`) is actually just plain *broken*. If the build status looks bad, it's easy to work around:

- locate the most recent green build on the travis, 
- find it's git commit id (e.g. `595b956`), and 
- check out that change in git: `git checkout 595b956`
- build



Frequently used cmake settings
------------------------------

There are more settings in the build configuration you may want to adjust. 
In order to see a useful list of your options, you can run: 

	$>	cmake -L ..

This configure the build using default settings or settings stored in the file 
build/CMakeCache.txt, print explanatory return statements and produce a list of 
variables the value of which you might want to change. In order to see all the command line options `cmake` offers, type:

	$>	cmake --help

It is not necessary to pass in all required arguments each time you run cmake, as cmake caches previously set arguments in the file CMakeCache.txt. This is helpful, but also something to keep in mind if unexpected things happen. If you feel uncomfortable with the command line, you might want to try cmake frontends like  `ccmake` or `cmake-gui`. You can also configure your build by manually editing build/CMakeCache.txt. 

  * by default the SCClassLibrary is installed into the application bundle. There is a 
    cmake option to create symlinks of the SCClassLibrary in the source instead. This way, 
    if you change your classes, the changes will show up in your SC git-repository. To turn 
    on this symlinking add:

    `-DSC_SYMLINK_CLASSLIB=ON`

  * you can choose three build types (Release (optimised), Debug and RelWithDebInfo):

    `-DCMAKE_BUILD_TYPE=Debug`

  * the location "install"-folder, i.e. the location of the SuperCollider folder including
    the application bundle. The following line moves it to the Applications folder 
    (which means you need to use `sudo`):

    `-DCMAKE_INSTALL_PREFIX=/Applications`

  * make sure to get the best optimizations for your local system
 
    `-DNATIVE=ON`

  * build the *supernova* server:
    
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
    
  * build a 32-bit version:

    `-DCMAKE_OSX_ARCHITECTURES='i386'`

    or combine a 32- and 64-bit version into a bundle (i.e. build a universal binary). 
    This is only possible up until OSX 10.6 and requires the dependencies (Qtlibs & 
    readline) to be universal builds too:

    `-DCMAKE_OSX_ARCHITECTURES='i386;x86_64'`

 * Normally, homebrew installations of readline are detected automatically, and building with readline is only required if you plan to use SuperCollider from the terminal. To link to a non-standard version of readline, you can use:
    `-DREADLINE_INCLUDE_DIR='/path/to/readline/include'`
    `-DREADLINE_LIBRARY='/path/to/readline/lib/libreadline.dylib'`
 
  * If you run into trouble on a 10.8 system using an older version of the master branch, 
    you might want to try passing in the root folder of to 10.7 SDK as compiler flag `isysroot`. 
    This was reported to have helped complete building successfully in the past, but is not 
    required in recent builds:

    `-DCMAKE_CXX_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/"`
    `-DCMAKE_C_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/"`


Using cmake with Xcode or QtCreator
-----------------------------------

Xcode projects are generated by appending: `-G Xcode`. The build instructions above use the Xcode toolchain, which is well-tested and generally recommended if you're planning to debug or hack on SC.

You may also want to make the expected SDK-Version and location explicit, using:
`-DCMAKE_OSX_SYSROOT=`
This is often useful to point to an older SDK even with a newer Xcode installed. These are generally located in the `/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs` of an Xcode.app package.

You can build without an IDE using `make`, by omitting the `-G Xcode` - in this case, your build command is `make` rather than `xcodebuild`

Qt Creator has very good `cmake` integration and can build `cmake` projects without requiring a `cmake` generated project file. If you have Qt5 via homebrew installed, you can install Qt Creator by running `brew linkapps qt5`


Remarks on different SC versions
--------------------------------

At the time of this writing (Nov 2013) only the 3.6 branch of SC is considered release
quality, while most developers use the 3.7 master-branch in daily practice. Another, 
considerable part of the OSX community is still sticking to SC 3.4, because HID support 
is broken in later versions, and because this was the last "pure" OSX version designed
for "literate programing" and incorporating a lot achievements of a generation of SC
developers. HID-support and some other features of "classical SC" are hoped to return 
with the release of version 3.7. 

The SC 3.6-branch introduced an all new Qt based IDE, which among many other achievements
featured first time ever, close to identical support of the three platforms OSX, Windows 
and Linux, both in featureset and appearance/user-interface. At the same time is the last 
branch to support:

  * compiling in and for Snow Leopard and 32 as well as 64 bit.
  * using Cocoa as graphical toolkit
  * using SwingOSC as graphical toolkit
  * building the "classical" SC editor with rich-text support and a 100% OSX 
    look and feel

Starting from SC 3.5 also a new, highly structured and dynamic help-system was introduced.
Structured help text is written using a superset of the Markdown-language and converted 
to html for display. The help system also provides a efficient way to generate sc class 
documentation templates.

Among the OSX-versions, Snow Leopard (SL, 10.6) sticks out in that it is the last to 
support 32-bit hardware and ship with Rosetta, a virtual machine for Power PC software. 
Therefore the 3.6-branch of the SC source will remain SL compatible and support both 
64&32 bit hardware, although it will become more difficult to support later OSX-versions 
at the same time. It is therefore generally expected that from 2014 onwards SC 3.7 will
become the release version of SC, and the only actively maintained branch. On the downside
expect it to become more difficult to run SC 3.6 on OSX versions after Mavericks and
find working 3.6 build systems. Apple makes accessing development ressources for Snow 
Leopard increasingly difficult.

On the 3.7-branch SL support was given up around August 2013 alongside with raising 
compiler requirements to current clang. These decisions were taken to enhance code 
maintainability as part of an major effort to streamline the SC codebase.
On the upside expect the reintroduction of HID support, a new backend for the document 
class, and support for SuperNova, an alternative synthesis engine with multicore-support 
written by Tim Blechmann, also on OSX.

In general it is strongly recommended to always use the most recent build tools available 
for your OSX-environment and update regularly. Fortunately the OSX-tools update 
automatically on recent systems and Readline is uncritical as to version requirements. 
But keep looking out for updates to Cmake (currently v2.8.12) and *minor version* updates 
to Qt (currently 4.8.5). SC cannot currently be compiled using any version of Qt5.

Git allows you to easily go back in history and build from every stage of the SC commit 
history. Usually though it is only advisable to build release version snapshots (marked by 
git tags), or the endpoints of actively maintained branches. Other states are likely
to be provisional, or incompatible with current toolchains. Installing multiple versions of
build tools is not always trivial, and on top of that Apple strongly enforces a "the latest 
only" policy in its maintenance culture. Building for 10.5 and 10.4 is going to be a major 
effort. Note though that all Apple hardware with intel processors should be supported by 
SL - you might be better off updating your OS. If you really need to build for older OSX 
versions and also Power PC hardware, expect some it-archeological work. If you run into 
trouble, chances are that oldhand SC developers on the mailing lists can help. SC bugs, 
btw, are officially managed on the Github issue tracker, but often a preliminary enquiry 
on the mailing lists is useful.


Qt vs. Cocoa, IDE vs. Editor "classic"
--------------------------------------

The Qt framework is used for the SC-IDE, and to provide a graphical toolkit for SC. 

While Qt is the default toolkit for both SC 3.6 and 3.7, it is still possible to build SC 3.6 using 
Cocoa and the old SC editor. This is controlled by a cmake switch for the IDE, which 
implicitly toggles building the old editor:

`-DSC_IDE=OFF`    -> old mac editor is built (by default using Qt)

There is also a switch to turn the use of Qt as GUI toolkit on or off. Switching Qt off
in turn switches Cocoa on. Turning Qt off implicitly prevents building the IDE, which in 
turn triggers building the old editor, now using Cocoa:

`-DSC_QT=OFF`     -> no Qt toolkit -> mac editor is built -> Cocoa is used/provided

While Qt and Cocoa are mutually exclusive, SwingOSC can still be used as an alternative
and switchable toolkit to either of them in 3.6.


Standalones
------------

**NOTE**: *Standalone support has fallen behind in the 3.7 release. These instructions may not fully work on recent versions of master. They should still apply to older versions of SuperCollider.*

To create a "standalone" app using cmake, you need to use the "standalone" flag. For 
example, to create a standalone whose name is MyFabApp:

`cmake -D standalone="MyFabApp" ..`

If you want to include special resources, put them in a folder called 

`platform/mac/MyFabApp Resources`

or 

`platform/mac/Standalone Resources`

within the SC source tree.

This builds like normal but names the finished program "MyFabApp". When you run 
"MyFabApp" for the first time, a user application support directory called "MyFabApp"
too will be created in the default location (`/Users/<username>/Library/Application 
Support/MyFabApp`). This folder will reside alongside a folder called supercollider 
created by a default SC install you might have on the same machine. The default install
will not scan the "MyFabApp" folder.

If you've built a standalone and want to go back to normal build mode, just
set that value as an empty string:

`cmake -D standalone="" ..`


On libsndfile
-------------

In the past compiling a universal binary of libsndfile used to require accessing both a
i386 and PPC Mac. The reasons for this are described here:

http://www.mega-nerd.com/libsndfile/FAQ.html#Q018

Because of this, libsndfile is included with the source as a precompiled
universal binary. This UB contains ppc, i386 and x86_64 archs. By default SC uses this 
file, although the currently built universal does not contain Power PC versions any more 
and intel universal binaries of libsndfile are readily available via package managers.

If you would like to build using the latest version of libsndfile, you need to intall it
and point cmake to it very much the same way it was demonstrated above for readline. 
The cmake variables, alongside with the likely path if you install libsndfile using 
MacPorts, are:

`-DSNDFILE_INCLUDE_DIR=/opt/local/include`
`-DSNDFILE_LIBRARY=/opt/local/lib/libsndfile.dylib`


Special characters on mac
-------------------------

Please do not use non-ASCII characters (above code point 127) in your
SuperCollider program path (i.e. the names of the folders containing SuperCollider).
Doing so will break options to open class or method files.


Outro
-----

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available!
