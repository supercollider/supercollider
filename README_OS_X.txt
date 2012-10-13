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
 * Qt libraries 4.7 or greater: http://qt-project.org/downloads

To build SuperCollider with Cmake, it is recommended to create a "build"
folder (to keep the built files neatly all together) in the root of the 
SuperCollider source, then run "cmake" from within that folder. Like this:

	cd ~/SuperCollider3/
	mkdir build
	cd build
	cmake ..

Then to run the build process run:

	make install

This will build the software, then "install" it to a folder "SuperCollider" under
CMAKE_INSTALL_PREFIX, which defaults to "<build-directory>/Install",
with the finished products in. You might like to move the "SuperCollider"
folder into your /Applications folder or install it there directly by
passing "-DCMAKE_INSTALL_PREFIX=/Applications" to cmake.

The build process can be configured using the cmake program, cmake
frontends like "ccmake" or "cmake-gui", or by simply editing the
build/CMakeCache.txt file.

For example, by default cmake will create a "release" build, but if you want a
"debug" build (with optimisations turned off etc), configure cmake like this:

	cmake -DCMAKE_BUILD_TYPE=Debug ..

By default the SCClassLibrary is copied into place. There is a cmake option to create
symlinks of the SCClassLibrary instead so that changes to code in the class library are reflected in git.
To turn on the symlinking do:

	cmake -DSC_SYMLINK_CLASSLIB=ON ..

For a 32bit build on a 64bit machine (i.e. recent Mac systems), you need to 
configure cmake like this:

	cmake -DCMAKE_OSX_ARCHITECTURES='i386' ..

For the BIG universal binary (on 10.6), use:

	cmake -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' ..

cmake -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/ -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -GXcode
**** 10.8 warning ****

On 10.8 you need to:

compile against the 10.7 SDK
build with the Xcode generator (add -GXcode to your cmake flags).

Then, build the install target in Xcode.

I started working on the bluetooth issues today actually, but the changes aren't trivial.

For some reason, the 10.7 SDK choice doesn't seem to percolate into the makefile version created by cmake. You can either build with an Xcode file:

(an example from josh on 10.8 with Xcode 4.5: cmake -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/ -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -GXcode )

your build will be in the "Install" folder in the build directory.

or add the following flags for isysroot:

	-DCMAKE_CXX_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/" -DCMAKE_C_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/" 

Finally - a cmake command that builds on 10.8 currently:

cmake -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/ -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 -DCMAKE_CXX_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/"  -DCMAKE_C_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/" -DCMAKE_BUILD_TYPE="Release"  ..

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

	cmake -D standalone="MyFabApp" ..

This builds like normal but names the finished program "MyFabApp" and includes
any special resources you might have created in the folder
"platform/mac/MyFabApp Resources" (or, if that doesn't exist, it just uses
"platform/mac/Standalone Resources").

If you've built a standalone and want to go back to normal build mode, just
set that value as an empty string:

        cmake -D standalone="" ..


------------------------------------------------------------------------
On libsndfile
------------------------------------------------------------------------

Compiling a universal binary of libsndfile requires access to both a
i386 and PPC Mac. The reasons for this are described here:

	http://www.mega-nerd.com/libsndfile/FAQ.html#Q018

Because of this, libsndfile is included with the source as a precompiled
universal binary. This UB contains ppc, i386 and x86_64 archs.


------------------------------------------------------------------------
Outro
------------------------------------------------------------------------

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available.

------------------------------------------------------------------------
