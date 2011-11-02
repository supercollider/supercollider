------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------

This is the Mac OS X version of James McCartney's SuperCollider
synthesis engine (scsynth) and programming language (sclang).
Supercollider's main homepage is at:

   http://supercollider.sourceforge.net/

The help files contain a lot of useful information and tutorials for getting
started - see the file "Help.html" for a starting point.

To get further information on SuperCollider usage or development, you
should subscribe to the mailing lists:

   http://swiki.hfbk-hamburg.de/MusicTechnology/880


SPECIAL CHARACTERS ON MAC:

Please do not use non-ASCII characters (above code point 127) in your SuperCollider 
program path (i.e. the names of the folders containing SuperCollider). 
Doing so will break options to open class or method files automatically.


------------------------------------------------------------------------
Compiling SuperCollider from the source code
------------------------------------------------------------------------

Requirements:
 * Mac OS X 10.4.9 or greater
 * Cmake 2.7 or greater
 * Xcode Tools 2.4.1 or greater
 * Qt 4: http://qt.nokia.com/downloads/qt-for-open-source-cpp-development-on-mac-os-x/

To build SuperCollider with Cmake, it is recommended to create a "build"
folder (to keep the built files neatly all together) in the root of the 
SuperCollider source, then run "cmake" from within that folder. Like this:

	cd ~/SuperCollider3/
	mkdir build
	cd build
	cmake ..

Then to run the build process run:

	make install

This will build the software, then "install" it to a folder "build/SuperCollider"
with the finished products in. You might like to move the "build/SuperCollider"
folder into your /Applications folder.

The build process can be configured using the cmake program, cmake
frontends like "ccmake" or "cmake-gui", or by simply editing the
build/CMakeCache.txt file.

For example, by default cmake will create a "release" build, but if you want a
"debug" build (with optimisations turned off etc), configure cmake like this:

	cmake -DCMAKE_BUILD_TYPE=Debug ..

By default cmake will copy SCClassLibrary to the Application directory. There is a cmake option to create
a symlink of the SCClassLibrary instead so that changes to code in the class library are reflected in git.
To turn on the symlinking do:

	cmake -DSC_SYMLINK_CLASSLIB=ON ..

For a 32bit build on a 64bit machine (i.e. recent Mac systems), you need to 
configure cmake like this:

	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS:STRING=-m32 -DCMAKE_C_FLAGS:STRING=-m32 ..

For the BIG universal binary (on 10.6), use:

	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES='ppc;i386;x86_64' ..


Qt GUI:
-------

By default the new Qt GUI support will be built into sclang and
SuperCollider application. If you want to build without it, configure
cmake like this:

	cmake -DSC_QT=OFF ..


Standalones:
------------

To create a "standalone" app using cmake, you need to use the "standalone" flag.
For example, to create a standalone whose name is MyFabApp:

	cmake -D standalone:string=MyFabApp ..

This builds like normal but names the finished program "MyFabApp" and puts all
resources into the app bundle, including any special resources you might have
created in the folder "platform/mac/MyFabApp Resources" (or, if that doesn't
exist, it just uses "platform/mac/Standalone Resources").

If you've built a standalone and want to go back to normal build mode, just
set that value as an empty string:

        cmake -D standalone:string= ..


------------------------------------------------------------------------
On libsndfile
------------------------------------------------------------------------

Compiling a universal binary of libsndfile requires access to both a
i386 and PPC Mac. The reasons for this are described here:

	http://www.mega-nerd.com/libsndfile/FAQ.html#Q018

Because of this, libsndfile is included with the source as a precompiled
universal binary. This UB contains ppc, i386 and x86_64 archs.


------------------------------------------------------------------------
Building - 10.6 and 64-bit scsynth and plugins
------------------------------------------------------------------------

You will need to do the following to build a 64-bit scsynth and plugins.
 * Open the XCode projects for Synth AND Plugins
 * Double-click each target, and change the
	 * Architectures setting to 'Standard (32-bit/64-bit Universal)'
	 * Base-SDK to 10.5
	 * (optional and recommended): Compiler to GCC 4.2 (if available)

This definitely works in Mac OS 10.6, probably 10.5, and these builds will NOT 
support 10.4.

XCode in 10.6 uses the GCC 4.2 compiler by default, and this compiler will NOT 
work with the 10.4 SDK (the default SDK). You will need to either set your build 
SDK to 10.5 or 10.6 inside each XCode project, or inspect (double-click) each 
target and set GCC 4.0 as your compiler.

UPDATE - There is now a Deployment32-64 build style that should build 
SuperCollider from source out of the box on 10.6. You can also use the compile.sh 
script and pass in ARCHS=32_64 as a flag:
    ./compile.sh ARCHS=32_64
This should build a 32/64 bit version of scsynth and the plugins and a 32-bit 
SuperCollider.app. This build style should use the 10.5 SDK by default.

The 10.6 SDK still has some problems. If you are on 10.6, please make sure you 
have installed the optional 10.5 SDK, installed with the Apple Developer Tools.


------------------------------------------------------------------------
Building SuperCollider THE OLD WAY (no cmake - just xcode required)
------------------------------------------------------------------------

Requirements:
 * Mac OS X 10.4.9 or greater
 * Xcode Tools 2.4.1 or greater.

The easy way:
	cd mac
	./clean-compile.sh
	cd ../../package
	./package.sh

Your build will be placed in a directory named "SuperCollider".

If you run into problems building the most likely cause is that you are
not running the correct version of the Xcode tools. You must be running
2.4.1 or above!

Building within Xcode:
	There are three projects that should be BUILT IN THIS ORDER:

	Synth
	 - the synthesis server application

	Plugins
	 - the UGens and plugins

	Language
	 - the OS X application with text editor, language interpreter,
	   event scheduling etc.

The package.sh script does several maintenance tasks (fixing creator
codes, exporting from svn, etc) and creates a ready-to-distribute zip of
SuperCollider.


------------------------------------------------------------------------
Outro
------------------------------------------------------------------------

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available.

------------------------------------------------------------------------
