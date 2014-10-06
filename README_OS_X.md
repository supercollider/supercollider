Supercollider 3.6 for OS X
==========================


Table of contents
-----------------

 * Introduction
 * NOTE: OS X Mavericks (10.9)
 * Obtaining the SC source
 * The SC 3.6.6 release
 * Quick steps (Read this if nothing else!)
 * Generic build instructions
 * Frequently used cmake settings
 * Using cmake with Xcode or QtCreator
 * Remarks on different SC versions
 * Qt vs. Cocoa, IDE vs. Editor "classic"
 * Standalones
 * On libsndfile
 * Special characters on mac
 * Problem getting Quarks?
 * Outro


Introduction
------------

This is the Mac OS X version of James McCartney's SuperCollider synthesis engine 
(scsynth) and programming language (sclang). http://supercollider.github.io/

The help files contain a lot of useful information and tutorials for getting
started. If a help-window is not visible when you open SC, try to access if from
the menu (Help/Show Help Browser). If you are not using an editor, open the file 
"Help.html" in the user application support folder as an entry point:

`/Users/<username>/Library/Application Support/SuperCollider/Help.html`

To get further information on SuperCollider usage or development, you should subscribe 
to the mailing lists:

http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml


NOTE: OS X Mavericks (10.9)
---------------------------

The 3.6.6-build of SC has not been adjusted to Mavericks yet. Please expect some rough
edges. Most importantly: "App Nap", a new energy saving feature for background applications,
causes the SC language interpreter to slow down dramatically after a while, if no sc-gui
window is in the foreground. It is therefore strongly recommended to switch off App Nap
for SuperCollider. The easiest way to do this, is to right-click SuperCollider.app and
select `Get Info` from the popup menu. Then choose `Prevent App Nap` in the list of start
options.


Obtaining the SC source
-----------------------

SC is on Github: https://github.com/supercollider/supercollider

Get a copy of the source code with:

`git clone --recursive https://github.com/supercollider/supercollider.git`

Snapshots of release-versions are available from Sourceforge:

http://sourceforge.net/projects/supercollider/files/Source/

The following assumes that you have installed git locally and use the clone from the 
Github repository.


The SC 3.6.6 release
--------------------

The release of SC v3.6.6 was built on OS X Snow Leopard (10.6.8) using the
following tools and dependencies:

  * Cmake 2.8.11
  * Xcode 4.2 Snow Leopard shipping with gcc 4.2.1 (llvmgcc42)
  * Qt libraries 4.8.5 as provided by MacPorts (universal build)
  * Readline 6.2 as provided by MacPorts (universal build)

It should be possible to build SC 3.6 using the tools as shown below, but this has not
been verified for a long time, so you will have to experiment and might have to go back
in release/commit history:

  * Mac OS X 10.4.9 or greater
  * Cmake 2.7 or greater
  * Xcode Tools 2.4.1 or greater
  * Qt libraries 4.7 or greater: http://qt-project.org/downloads

Since at least SC 3.6.5 it has not been possible any more to use the OSX supplied 
readline to build sclang with readline interface. Therefore GNU Readline >= 5.0 needs
to be installed on the build system.

SC 3.6.6 was built using the following cmake arguments:

`cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES='i386;x86_64' 
-DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.6.sdk/ 
-DCMAKE_INSTALL_PREFIX=./install -DREADLINE_INCLUDE_DIR=/opt/local/include 
-DREADLINE_LIBRARY=/opt/local/lib/libreadline.dylib -DSSE=ON -DSYSTEM_YAMLCPP=OFF ..`

This should create a universal build running on any intel based 32- and 64-bit Apple 
hardware using OSX 10.6 and later (see remarks on Mavericks above). 

In order to reproduce this build exactly you either have to use the source snapshot
distributed through SourceForge or checkout the tag `Version-3.6.6` from the SC Github repo.
Make sure the dependencies (Qt and Readline) are universal builds.


Quick steps
-----------

Assuming tools (cmake, Xcode) and dependencies (Qt, Readline) are in place and up to 
date (using Readline from MacPorts), assuming further that you just cloned the SC 
source into a folder `supercollider`, do this:

For SC 3.6:

	$>	cd supercollider
	$>	git submodule init
	$>	git checkout -b 3.6 origin/3.6
	$>	git submodule update
	$>	mkdir build
	$>	cd build
	$>	cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DREADLINE_INCLUDE_DIR=/opt/local/include  
	        -DREADLINE_LIBRARY=/opt/local/lib/libreadline.dylib ..
	$>	make install

For SC 3.7:

	$>	cd supercollider
	$>	git submodule init
	$>	git checkout master
	$>	mkdir build
	$>	cd build
	$>	cmake -DREADLINE_INCLUDE_DIR=/opt/local/include	 
	        -DREADLINE_LIBRARY=/opt/local/lib/libreadline.dylib ..
	$>	make install

At the end, in each case you should find a folder "Install/SuperCollider" containing the
SC application bundle and a few more files.

For building with Xcode, configure the build adding -GXcode to the cmake command-line:

	$>	cmake -GXcode -DREADLINE_INCLUDE_DIR=/opt/local/include
			-DREADLINE_LIBRARY=/opt/local/lib/libreadline.dylib ..

This will generate a Xcode project file in the build folder. Open it in Xcode, select
the `install` target, and build.

*Note:* The `/opt/local` paths used in the commands above are pointing to the macports
version of Readline. You will need to adjust those paths if not using macports.


Generic build instructions
--------------------------

To build SuperCollider with Cmake, it is recommended to create a "build"
folder in the root folder of the SuperCollider source (i.e. inside the folder
created when you cloned SC from git).

A dedicated build folder is created to separate build files from the source. 
This will leave the source alone (git monitors all activities in the source 
tree) and makes it easy to delete all build files without touching the 
source if you require a guaranteed clean build.

So after cloning the SC source into the folder supercollider (default), do:

	$>	cd supercollider
	$>	mkdir build
	$>	cd build

Then run the following command to to configure the build (`cmake`) and to point
to the root folder of the SC source (`..`):

	$>	cmake ..

After the build configuration finished, and if no errors were returned, do:

	$>	make install

This will build the software and "install" it to a folder "SuperCollider" in 
`<build-directory>/Install`. You would usually move this folder into the folder 
`/Applications` manually.

If you would like to create a disk image for distribution, run the following command 
and find the .dmg file in your build folder:

	$>	make package

While this would ideally be sufficient to build SC on OSX, currently the following
cmake arguments are required:

If building SC 3.6 for Snow Leopard:

`-DCMAKE_OSX_DEPLOYMENT_TARGET=10.6`

Which results in this build command:

	$>	cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 ..

In all SC-versions, in order to enable *building* the readline interface, you will need 
to install it on your build system first (this is *not* required for running SC). It 
is advisable to use a package manager like Homebrew or MacPorts. Homebrew is a comparatively
recent, very flexible and modern package manager with broad community support. MacPorts is
the old (Apple backed) workhorse that might still be a bit more reliable, especially on
elder systems. Both are reported to work for the relatively slim requirements of building
SC on a OSX system (required: Qt, readline, optional: libsndfile, portaudio, possible: 
fftw3). If you run into problems with one of the packages it is more likely a problem with 
your local system than with one of the packages.

After you installed readline you need to tell the build system where to find the required 
headers and libraries. When using MacPorts, the paths are likely to be:

`-DREADLINE_INCLUDE_DIR=/opt/local/include` 
`-DREADLINE_LIBRARY=/opt/local/lib/libreadline.dylib`

For Homebrew the default locations are (Apr 2014):

`-DREADLINE_INCLUDE_DIR=/usr/local/Cellar/readline/6.3.3/include` 
`-DREADLINE_LIBRARY=/usr/local/Cellar/readline/6.3.3/lib/libreadline.dylib`

So a full cmake configure command, specifying a build target and including readline
(from MacPorts) would look like this:

	$>	cmake -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DREADLINE_INCLUDE_DIR=/opt/local/include -DREADLINE_LIBRARY=/opt/local/lib/libreadline.dylib ..

The Qt libraries also need to be installed in your system at build time, but usually 
cmake finds them automatically. It does so by looking for the file `qmake` in your system 
path and expects to find other Qt files relative to that location. In case you want to 
use a Qt install in a nonstandard location, you can specify the `qmake`-location like 
so:

`-DQT_QMAKE_EXECUTABLE=/yourQmakePath/qmake`

There are more settings in the build configuration you are likely to want to adjust. 
In order to see a useful list of your options, you can run: 

	$>	cmake -L ..

This configure the build using default settings or settings stored in the file 
build/CMakeCache.txt, print explanatory return statements and produce a list of 
variables the value of which you might want to change. In order to see all the command
line options `cmake` offers, type:

	$>	cmake --help

It is not necessary to pass in all required arguments each time you run cmake, as cmake 
caches previously set arguments in the file CMakeCache.txt. This is helpful, but
also something to keep in mind if unexpected things happen. If you feel uncomfortable 
with the command line, you might want to try cmake frontends like  `ccmake` or 
`cmake-gui`. You can also configure your build by manually editing build/CMakeCache.txt. 


Frequently used cmake settings
------------------------------

  * the location "install"-folder, i.e. the location of the SuperCollider folder including
    the application bundle. The following line moves it to the Applications folder 
    (which means you need to use `sudo`):

    `-DCMAKE_INSTALL_PREFIX=/Applications`

  * you can choose three build types (Release (optimised), Debug and RelWithDebInfo):

    `-DCMAKE_BUILD_TYPE=Debug`

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

  * or combine a 32- and 64-bit version into a bundle (i.e. build a universal binary). 
    This is only possible up until OSX 10.6 and requires the dependencies (Qtlibs & 
    readline) to be universal builds too:

    `-DCMAKE_OSX_ARCHITECTURES='i386;x86_64'`
 

  * by default the SCClassLibrary is installed into the application bundle. There is a 
    cmake option to create symlinks of the SCClassLibrary in the source instead. This way, 
    if you change your classes, the changes will show up in your SC git-repository. To turn 
    on this symlinking add:

    `-DSC_SYMLINK_CLASSLIB=ON`

  * If you run into trouble on a 10.8 system using an older version of the master branch, 
    you might want to try passing in the root folder of to 10.7 SDK as compiler flag `isysroot`. 
    This was reported to have helped complete building successfully in the past, but is not 
    required in recent builds:

    `-DCMAKE_CXX_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/"`
    `-DCMAKE_C_FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/"`


Using cmake with Xcode or QtCreator
-----------------------------------

If you want to use Xcode, you can use cmake to generate project files by adding the
cmake flag -G followed by the generator name:

`-GXcode`

You might also want to make the expected SDK-Version and location explicit:

`-DCMAKE_OSX_SYSROOT=`

Depending on your XCode version this will either be a subfolder of the `/Applications`- or 
the `/Developer`-folders. On Xcode 5 the path to SDK 10.8 is:

`/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/`

On Xcode 4.2 Snow Leopard the path to the SDK 10.6 is:

`/Developer/SDKs/MacOSX10.6.sdk/`

So to target 10.8 on a Mavericks system (10.9) using the SDK for 10.8, you would run:

`cmake -GXcode -DCMAKE_OSX_DEPLOYMENT_TARGET=10.8 -DCMAKE_OSX_SYSROOT=/Applications/
Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk/ ..`

Using cmake as generator for a Xcode project file is not done often within the sc 
developer-community, it is therefore not unlikely that you run into trouble when trying
to do so, especially if you try it outside the master branch and on a source older than 
August 2013. 

With current prominence of Qt in SC the likelihood to build successfully in a graphical 
IDE is far higher if you use Qt Creator. Qt Creator has very good `cmake` integration and 
can build `cmake` projects without requiring a `cmake` generated project file. 
Give it a try...


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

Qtlibs are used for the SC-IDE, and to provide a graphical toolkit for SC. While Qtlibs 
is the default toolkit for both SC 3.6 and 3.7, it is still possible to build SC 3.6 using 
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

Problem getting Quarks?
-----------------------

If you can't download Quarks via `Quarks.gui`, the reason might be that you need to 
install a subversion client. If that doesn't help, check where SC believes svn is 
located on your system by evaluating:

`QuarkSVNRepository.svnpath`

If it is pointing to the wrong file (or none at all), one cure would be to assign the 
correct value and add it to your sc startup file, for example:

`QuarksSVNRepository.svnpath = "/opt/local/bin/svn"`


Outro
-----

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available!
