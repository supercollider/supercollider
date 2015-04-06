Supercollider 3.7 for Windows
=============================

**IMPORTANT**: a full install of **Git** is required to use the SC extensions
system. Please read at least the section *Installation and Dependencies*

NOTE: This is a provisional version of the Windows Readme provided for the alpha-
and beta-stages of the upcoming 3.7-release. It is bound to change considerably.


Table of contents
-----------------

 * Introduction
 * System requirements
 * *Installation and Dependencies*
 * Using the SuperCollider IDE
 * Using SuperCollider in command line mode
 * Configuration- and support files
 * Extensions and Quarks
 * *Building SuperCollider for Windows*
 * Preliminaries
 * Obtaining the SC source
 * The SC 3.7 release
 * Using cmake with Visual Studio
    * External dependencies
    * Qt
    * Portaudio (PA)
    * Libsndfile
    * FFTW
    * Readline
    * Specifying library- and include-paths manually
    * Downloading a binary dependencies bundle (temporary)
 * Using cmake with msys/MinGW (outdated)
 * Using cmake with QtCreator (outdated)
 * Cmake arguments for SC (outdated)
 * Diagnosing build problems
 * Links
 * Outro


Introduction
------------

This is the Windows version of James McCartney's SuperCollider synthesis
engine (scsynth) and programming language (sclang). SuperCollider's central
website is on Github:

   http://supercollider.github.io/

The help files contain a lot of useful information and tutorials for getting
started. If you are using the SuperCollider ide, you can find the documentation
using Cmd+D, or via the menu entry `Help` -> `Show Help Browser`.

In a non-IDE context, you must generate the help files first, by running:
`SCDoc.indexAllDocuments`. Help files will be created in the use support
directory. On Windows 7&8 this is

`C:\Documents and Settings\<USERNAME>\Local Settings\Application Data\SuperCollider`

To get further information on SuperCollider usage or development, you are
strongly encouraged to subscribe to the mailing lists:

   http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml


System Requirements
-------------------


### Operating System

 - Windows 8
 - Windows 7
 - Windows Vista
 - Windows XP (?)


### Sound card

Most common sound cards will work fine - the sound card should be one
that is supported by PortAudio (http://www.portaudio.com). If you have
issues with latency, consult http://www.portaudio.com/docs/latency.html
for advice on how to tune your operating system to achieve the best results
(in short: use ASIO drivers wherever possible).


Installation and Dependencies
-----------------------------

For SC itself just run the installer, and choose (or confirm) the path where all
the application files will be unpacked. In order to uninstall use the Windows
recommended way (Control panel -> Uninstall a programm) or run Uninstall.exe
from within the SC installation folder. Note that files created in the user
support directory are not deleted (see below for details and an explanation).

IMPORTANT: in order to use the Quarks extension system you *need* to install
Git. It is available from

    http://git-scm.com/downloads

After installation, you can control SuperCollider from the provided IDE or
in command line mode:


Using the SuperCollider IDE
---------------------------

To start up the IDE use the Startmenu link or run the scide.exe program
found within the installation folder. On first startup expect several
firewall warnings to pop up. The SuperCollider components communicate
through the network, so you will get a warning for each of scide, sclang,
scsynth and - if installed - supernova.


Using SuperCollider in command line mode
----------------------------------------

1. Open a Windows command line window (the cmd.exe program).

2. Navigate to the SuperCollider installation folder.

3. Start up the sclang.exe program, to enter the SuperCollider
command line.

4. You can now type SuperCollider code and evaluate it by pressing
the Enter (Return) key. You can browse the previous commands using
the up and down arrow keys.

5. These are important keyboard shortcuts:

    `Ctrl+t`: equivalent to CmdPeriod.run  
    `Ctrl+x`: to recompile the class library  
    `Ctrl+d`: to quit the SuperCollider command line  

The interface uses the GNU library Readline. Google readline for more
information.


Configuration- and support files
--------------------------------

The location of the folder containing application related user data (f.e.
Quarks, Help, plugins, config-files, synthdefs) follows general Windows
policies:

- Windows Vista/7/8 `C:\Users\<USERNAME>\AppData\Local\SuperCollider`
- Windows XP: `C:\Documents and Settings\<USERNAME>\Local Settings\Application Data\SuperCollider`

This folder is user writable and *not* deleted by uninstall. Therefore your
settings, quarks, plugins etc persist through updates (beware of potential
incompatibilities with new sc-versions). The folder can easily be accessed
via the IDE (menu -> `Open user support directory`). Its location can be
queried by executing:

    Platform.userAppSupportDir

In order to access it through the FileExplorer you might have to make hidden
system folders visible.


Extensions and Quarks
---------------------

Extensions are bundles of classes, and optionally plugins and helpfiles that
extend the functionality of SC. They typically have a root folder that may
be moved to the `Extensions` subfolder of the user support directory, or - the
more flexible and recommended way since SC version 3.7: extensions - wherever
they are stored - may be added to the folders scanned by SC at startup
by adding them to the list of `include folders` in a file called
`sclang_config.yaml` in the user support directory. This file can be edited
manually, or through the settings dialog of the IDE, or programmatically (see
help->`LanguageConfig`). After each change of this file the interpreter has to
be restarted. A comfortable and community driven way of adding extensions to SC
is the so called *Quarks* system. Quarks may be added programmatically or by
using a GUI. See help->`Using Quarks` for an overview.


Building SuperCollider for Windows
==================================

Preliminaries
-------------

The build system of SC uses the cross platform, open source build system `cmake`.
Cmake supports native toolchains on many operating systems. Since SC version
3.7 the recommended way of building SC is using cmake in conjunction with
`msbuild` and `Visual Studio 12 2013`. Mainly for historical reasons the
previous way of building SC, using the unix emulation `msys` in conjunction
with the Windows port of gcc `MinGW`, is described below as well.

In order to build SC you need recent versions of a few tools (see download links
at the end of this document):

* Git (>v1.9.4): the currently most popular version control system used for SC
* SVN (>v1.8): the version control system used by the SC dependency `portaudio`
* cmake (>v2.8.11): build system used for SC and Portaudio
* Visual Studio 2013 (v12): MS development environment with free community edition
* NSIS (v2.46 or 3.0b1): a free tool to create binary installers

The SC source comes with various dependencies bundled as submodules. The build
dependencies you need to download are (see links at the end):

* Qt5 (>5.3.2)
* Portaudio (rev1952)
    * ASIO SDK (v2.3) - optional but strongly recommended
    * DirectX (v9, "june 10") - optional
* Libsndfile (v1.0.25)
* FFTW (v3.3.4)
* Readline

If your soundcard does not provide ASIO drivers, you might want to try a very
popular generic driver:

* ASIO4ALL

You may also prefer a lightweight text editor that supports unix line endings
and a file decompression tool for formats other than .zip:

* Notepad++
* 7-zip


Obtaining the SC source
-----------------------

[SC is on Github](https://github.com/supercollider/supercollider)

Get a copy of the source code with:

    git clone --recursive https://github.com/supercollider/supercollider.git

Binaries of release-versions and pre-release snapshots are available as
GitHub releases:

    https://github.com/supercollider/supercollider/releases


The SC 3.7 release
------------------

*** the following is the provisional description, and will be
adjusted to however the SCWin Release 3.7 build will be done.

SC 3.7 was built with

- Qt5.4.1 (msvc2013_64_opengl, ie. 64-bit)
- Portaudio 1.9 (rev. 1952)
- ASIO SDK 2.3
- MS DirectX v9 (June 10)
- linsndfile 1.25
- FFTW 3.3.4
- Readline 6.2

and various bundled software packages (boost 1.5.7, portmidi, hidapi and
several more (see `external libraries` in the source tree))

Other than Visual Studio, the tools used were Git (1.9.5) and cmake (3.2.1),
SlikSVN (1.8.10) and NSIS (3.0b1) to create the binary installer. These four
tools should be installed under Windows (not msys) and be added to the system
path (a reboot might be required). When working on the Windows command line,
using combo `cmake` and `msbuild` you should always use the Visual Studio
provided shell called `Developer Command Prompt for VS2013`. This will create
the environment (especially path settings) required to easily access the correct
build tools.


Using `cmake` with Visual Studio (VS)
-----------------------------------

VS uses a central `solution` file and several project files to organize the
build. Cmake has the capability to generate these files. You tell cmake what
kind of project files to generate by adding the flag -G and a generator name
after the command `cmake`. As cmake creates generators by evaluating a tree
structure of files called `CMakeLists.txt`, you also need to point cmake to
the top-level file of this tree, which is always located in the root of you
source tree. SC is traditionally built in a folder called `build` below the
root folder of the source. Pointing from `build` to the root source folder thus
just requires the shortcut `..`. Provided you have a command prompt in the
root folder of the SC source tree, these steps will start configuring the
build

```
$>  mkdir build
$>  cd build
$>  cmake -G "Visual Studio 12 2013 Win64" ..

```

The ending `Win64` in the generator name tells cmake to configure the build for
a 64-bit version of SC. Without the ending - so just "Visual Studio 12 2013" - a
32-bit version would be built (yet untested). Provided the other dependencies are
located in the right locations (see below), and you want to use the default
build type called `RelWithDebInfo` ('release with debug info'), you only need
one more setting to have build configuration and creation of project files
complete successfully: the location of Qt5. Qt, as distributed via web installer,
has a particular tree structure that you need to specify down to the forth level
for cmake to find all the required Qt-files. For the official SC build this was:

    C:/Qt/5.4/msvc2013_64_opengl

You need to assign this path to the keyword `CMAKE_PREFIX_PATH` and add it to
the cmake command using the flag -D ('definition'):

    $>  cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_64_opengl ..

If you want to build a debug- or release-version, you need to assign
the respective value to the keyword `CMAKE_BUILD_TYPE`. So in order to
configure and prepare a debug-build, you would type:

    $>  cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:/Qt/5.4/msvc2013_64_opengl ..

Provided the other dependencies are stored in places that can be found by cmake,
the build configuration should succeed with this command line. After the
configuration you need to start the actual build. This can conveniently be done
on the command line, again using cmake (which in turn will start `msbuild`),
with a few arguments:

    $>  cmake --build . --config Debug --target install

If you prefer to start the build (and install) from the VS-IDE, you as well
run VS and open the `solution` file `SuperCollider.sln` that cmake created
in the build folder. Or run VS from the command line by `start`-ing the
solution file:

    $>  start SuperCollider.sln

Then select the target `install` in the `Solution Explorer` and build it
(right-click target INSTALL->build, or select INSTALL, then menu->BUILD->Build
INSTALL). This will basically trigger three steps with a single command:

* build all required targets for SC to work (sclang, scsynth & plugins, scide)
* install them in a folder that reflects build type (Debug) and target
  architecture (64-bit): `build\x64\Debug\SuperCollider`
* copy the required dependency libraries into the install folder (fixup bundle)

**IMPORTANT NOTE:** in the current state of cmake<->VS integration, build type
as defined by cmake and by VS are not coordinated automatically. The IDE chooses
`Debug` by default, whereas cmake configures `RelWithDebInfo`, if you do not
specify a `CMAKE_BUILD_TYPE` definition. This can cause incongruities between
the build files created by cmake and the build strategy chosen by VS. Therefore
make sure to specify the same build type in cmake configuration and for a VS (or
`msbuild`) build. If you want to change the build type, make sure to `clean` the
build first

    cmake --build . --config Debug --target clean

and then rerun the cmake configuration, specifying the new build type you intend
to use with `cmake --build` or from within the VS-IDE. Possible build types are
`Release`, `MinSizeRel`, `RelWithDebInfo` and `Debug`. Pay particular attention
to the `found path` returned by cmake for Portaudio. The path should contain
the build type, as there is a separate Portaudio build for each build type.


## External dependencies

####Qt

You should download the latest version (currently 5.4.1) using the web installer
provided by the official Qt distribution. The web installer provides you a
choice of various flavours, aimed at different plattforms and created with
different toolchains. When using VS you have the choice between 64- and 32-bit
versions, as well as between an opengl and default version. Sofar both opengl and
default versions have been used in 64-bit versions. A MinGW-compiled 32-bit
version has been used in a msys environment. The most common path defining Qt-
version and flavor is this:

    C:\Qt\5.4\msvc2013_64

You must add the version you choose as cmake definition for `CMAKE_PREFIX_PATH`


####Portaudio (PA)

There is currently no binary distribution of Portaudio available. You need to
download the source and build it yourself. Fortunately PA supports cmake, so
we can use build steps very similar to the ones described above for SC.

Move to the folder containing the SC source (the parent folder) and download
PA with svn:

    $> svn co https://subversion.assembla.com/svn/portaudio/portaudio/trunk portaudio

This will create a folder `portaudio` containing the source. In order
to support some of the audio driver APIs available in Windows portaudio needs
one or two additional SDKs. The ASIO SDK provided by Steinberg, and if you want
DirectSound support, the latest DirectX SDK provided by Microsoft. Go get them
following the links provided at the end of this document. DirectX will be
installed in the default location and then found automatically. For the ASIO SDK
to be sound, it's root folder should be parallel to the Portaudio folder.

Once this is in place change into the `portaudio` folder, enter the (existing)
folder build, and run cmake specifying our generator and the build type required.
Then start the build:

```
$>  cd portaudio\build`
$>  cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_BUILD_TYPE=Debug ..
$>  cmake --build . --config Debug
```

No target required. This will create a folder `Debug` within the folder `build`
that contains all libraries required. Provided the portaudio source root folder
is contained in the same parent folder as the SC source tree, the PA libraries
should be found automatically during the SC build configuration.


####Libsndfile

Libsndfile (>= v1.0.25) is provided with a binary installer for Windows (see
download link at end of this document). You may install in the default
location (`C:\Program Files\Mega-Nerd\libsndfile` for the 64-bit version) or
(maybe better) in a folder `libsndfile` next to the SC source tree (shared parent
folder, ommit the Mega-Nerd level). Using the 'Developer Command Prompt' (!) change
to the folder `libsndfile\bin` and use the lib-tool to create a .lib-file from
the existing .dll:

```
$>  cd libsndfile\bin
$>  lib /machine:x64 /def:libsndfile-1.dll
```

This will create two new files in the bin folder: libsndfile-1.def and
libsndfile-1.lib. The later will be used in the SC compile.


####FFTW

FFTW (>= v3.3.4) provides a "zip-ball" of Windows binaries. Please extract it
into a folder parallel to the SC source tree and rename the folder to `fftw`.
The folder is flat, containing both headers and libraries. Repeat the 'lib'-steps
described above for libsndfile, in order to create a .lib of the file
`libfftw3f-3.dll` (*not* `libfftw3-3.dll`).


####Readline

As of the writing of this guide (April 2015), Readline support in SCWin 64-bit
MSVC does not work. The Readline library provides an interface for keyboard
control of command line sclang. Anticipating Readline to work, the SC build
configuration can automatically find a readline binary tree that resides
in parallel to the SC source tree (same parent folder). So far experiments have
been made with the MinGW compiled 64-bit version of Readline provided in the
`opt` folder of the MinGW tree.


###Specifying library- and include-paths manually

If for some reason you cannot adhere to the folder structure described above,
you may also specify the paths manually as cmake definitions. This will create
a loong cmake command, therefore you might prefer to save it as a batch file.
Here is an example:

```
set CMAKE_BUILD_TYPE=Debug
set SC_SOURCE_DIR=C:/scbuild/supercollider
set QT_HOME=C:/Qt/5.4
set CMAKE_LIBRARY_ARCHITECTURE=x64
set CMAKE_PREFIX_PATH=%QT_HOME%/msvc2013_64_opengl
set SNDFILE_PREFIX_PATH=%SC_SOURCE_DIR%/../libsndfile
set FFTW_PREFIX_PATH=%SC_SOURCE_DIR%/../fftw
set READLINE_PREFIX_PATH=%SC_SOURCE_DIR%/../readline
set PORTAUDIO_PREFIX_PATH=%SC_SOURCE_DIR%/../portaudio

cmake -G "Visual Studio 12 2013 Win64" ^
-DCMAKE_LIBRARY_ARCHITECTURE=%CMAKE_LIBRARY_ARCHITECHTURE% ^
-DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE% ^
-DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% ^
-DSNDFILE_INCLUDE_DIR=%SNDFILE_PREFIX_PATH%/include ^
-DSNDFILE_LIBRARY=%SNDFILE_PREFIX_PATH%/bin/libsndfile-1.lib ^
-DFFTW3F_INCLUDE_DIR=%FFTW_PREFIX_PATH% ^
-DFFTW3F_LIBRARY=%FFTW_PREFIX_PATH%/libfftw3f-3.lib ^
-DPORTAUDIO_INCLUDE_DIRS=%PORTAUDIO_PREFIX_PATH%/include ^
-DPORTAUDIO_LIBRARIES=%PORTAUDIO_PREFIX_PATH%/build/%CMAKE_BUILD_TYPE%/portaudio_x64.lib ^
-DCMAKE_LIBRARY_PATH=%CMAKE_PREFIX_PATH%/bin;%SNDFILE_PREFIX_PATH%/bin;%FFTW_PREFIX_PATH%;%PORTAUDIO_PREFIX_PATH%/build/%CMAKE_BUILD_TYPE% ^
..

REM -DREADLINE_INCLUDE_DIR=C:/scbuild/bin64/libreadline.mingw492_64/include ^
REM -DREADLINE_LIBRARY=C:/scbuild/bin64/libreadline.mingw492_64/bin/libreadline6.lib ^
REM start Supercollider.sln
REM ;C:/scbuild/bin64/libreadline.mingw492_64/bin

```


###Downloading a binary dependencies bundle (temporary)

For convenience during the development and testing period towards the SC 3.7
release we provide a zip-ball of the dependencies (except Qt and DirectX).
It contains prebuilt Portaudio (Debug, Release, RelWithDebInfo, MinSizeRel) the
ASIO-SDK, .lib-libraries where necessary, and an experimental Readline, grabbed
from MinGW 4.92_64. The folders are named and nested as described above, and
should be copied/extracted to the same parent folder as the SC source tree.
There are only 64-bit versions.

[Find the zip-ball here (29.4 MB)](https://drive.google.com/file/d/0B7igZrWv7UxdemRzSkVhc0pMRFE/view?usp=sharing)

*IMPORTANT NOTE:* You should only place the Readline folder in parallel to the
SC source tree, if you want to experiment with it. Currently the SC build will
break if Readline is added.


Using cmake with msys/MinGW (outdated)
--------------------------------------

Because of SC's Unix-history past SCWin releases have been built within a
mini unix emulation running within Windows, called `msys`. Msys is used to
control MinGW, which is more or less a Windows version of gcc. For the time
being the msys/MinGW environment is the only to bild 32-bit SC 3.7 with readline
support.
Msys provides a bash shell, the file structure of a unix system, a
syntax to integrate files from the Windows file system (`/c/Users/...`),
it allows to merge an msys path with the Windows system path and of course
various build tools. Depending on whether you prefer to work in a Unix- or
Dos shell, different approaches to to using either of them might be
chosen. This description assumes you prefer a Unix shell, even in cases
where the same operation could be executed in a Dos shell (cmake, the tool
used to configure the build, is actually cross platform).

The msys-distribution used was from *mingwbuilds* (r13), MinGW 4.8.2 was
used as bundled with Qt5.3.2.

The cmake arguments used were:

```
  cmake -G"MSYS Makefiles" -DCMAKE_TOOLCHAIN_FILE=../sc-toolchain-hybrid.cmake -DSUPERNOVA=ON -DNATIVE=OFF ../../sources/supercollider
```

The toolchain file referred to above supplies other, more static arguments,
and the paths to the libraries and headers of the dependencies:

```
  SET(CMAKE_CXX_COMPILER_ENV_VAR C:/Qt/Qt5.3.2/Tools/mingw482_32)
  SET(CMAKE_PREFIX_PATH=C:/Qt/Qt5.3.2/5.3/mingw482_32)
  SET(CMAKE_INSTALL_PREFIX=./install)
  SET(CMAKE_BUILD_TYPE=RelWithDebInfo) # could also be Debug or Release
  SET(NO_AVAHI=ON)
  SET(SYSTEM_YAMLCPP=OFF)
  SET(SYSTEM_BOOST=OFF)

  SET(CMAKE_LIBRARY_PATH
	  C:/msys/home/sc/prebuilt32/libsndfile.nerd/bin
	  C:/msys/home/sc/prebuilt32/readline/bin
	  C:/msys/home/sc/prebuilt32/fftw.org
	  C:/msys/home/sc/prebuilt32/portaudio/lib
  )

  SET(CMAKE_INCLUDE_PATH
	  C:/msys/home/sc/prebuilt32/libsndfile.nerd/include
  	C:/msys/home/sc/prebuilt32/readline/include
	  C:/msys/home/sc/prebuilt32/fftw.org
	  C:/msys/home/sc/prebuilt32/portaudio/include
  )

# set paths of libraries not found correctly with above syntax explicitly (workaround)

  SET(READLINE_LIBRARY C:/msys/home/sc/prebuilt32/readline/bin/libreadline6.dll)
  SET(FFTW3F_LIBRARY C:/msys/home/sc/prebuilt32/fftw3.org/libfftw3f-3.dll)
  SET(FFTW3F_INCLUDE_DIR C:/msys/home/sc/prebuilt32/fftw3.org)
```

If the build configuration finished successfully (read the cmake output
carefully to verify that all dependencies were found correctly and no other
errors are reported), the following steps will build SuperCollider:

```
  $>	make
  $>  make install
  $>  make bundle
```

The resulting build will reside in the folder specified by the
CMAKE_INSTALL_PREFIX-argument (by default folder `install` in the
build-folder). It can be used to run SuperCollider, especially for
test- and debug work. You can ommit the `make bundle` step in sub-
sequent builds, if you are sure nothing in the dependencies has
changed. In order to create an installer binary, add the following
command:

  `$>  make installer`

This only works if you have NSIS installed and in the system path.


Using cmake with QtCreator (outdated)
-------------------------------------

QtCreator (the IDE bundled with Qtlib) has excellent cmake integration.
It can read the CMakeLists.txt files provided in the SC source. It is
not necessary to use a generator (defined via the -G flag) to create
a project file as with other IDEs like Visual Studio or Xcode. In
order to create a QtC project file you will be prompted to provide
cmake arguments. It is advisable to make a reference to a toolchain
file as demonstrated above, to keep the list of arguments to be typed
short.

If possible deselect the option to add
the bash-shell provided by git to the system path. This conflicts with using
MinGW the way it is accessed by QtC.


Cmake arguments for SC (outdated)
---------------------------------

There are more settings in the build configuration you may want to adjust. In
order to see a useful list of your options, you can run:

	$>	cmake -L ..

or

  $>	cmake -AL ..

This configures the build using default settings or settings stored in the
file build/CMakeCache.txt, prints explanatory return statements and produces
a list of variables the value of which you might want to change.

In order to see all the command line options `cmake` offers, type:

	$>	cmake --help

It is not necessary to pass in all required arguments each time you run cmake,
as cmake caches previously set arguments in the file CMakeCache.txt. This is
helpful, but also something to keep in mind if unexpected things happen.

If you feel uncomfortable with the command line, you might want to try cmake
frontends like  `ccmake` or `cmake-gui`. You can also configure your build by
manually editing build/CMakeCache.txt.


Common arguments to control the build configuration are:

  * Location where SC get's "installed". The following line:

    `-DCMAKE_INSTALL_PREFIX=./build`

    moves the installed files in a subfolder `build` in the build-directory.

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


Diagnosing build problems
-------------------------

The most common build problems are related to incorrect versions of the core dependencies, or dirty states in your build folder. An additional
source for problems could be that dependencies and core sc are built
with different compiler versions or toolchains.

### Checking component versions:

- **cmake**: `cmake --version` (should be bigger than 2.8.11)

- Double-check the downloaded versions for qt5, readline, libsndfiles, fftw and portaudio.

- Make sure you downloaded a MinGW based Qt-distribution.

When asking for build help, always check and mention the installed versions any components you used to build SC!

### Dirty build states

While it's generally safe to re-use your build folder, changing branches, build tools, cmake settings, or the versions of your dependencies can sometimes put you in a state where you can no longer build. The solution is to clean your build folder - the common ways to do this, in order of severity:

1. `rm CMakeCache.txt` (delete your cmake settings for that build)
2. `make clean` (clean your intermediate build files)
2. `rm -r ./Install` (delete the output of your build)
3. `rm -rf *` (delete everything in the build folder)

Generally, clearing the CMakeCache.txt should be enough to fix many build problems. After each one of these, you must re-run the cmake command and rebuild. It's recommended that you create a new build folder for each branch you're building. In practice, though, you can usually switch between similar branches and rebuild by simply deleting your CMakeCache.txt.

### Someone else's fault...
If you're *sure* you're doing everything right and you're still failing, check the travis-ci status page:
[https://travis-ci.org/supercollider/supercollider]()


Links
-----

- [SC repo on Github](https://github.com/supercollider/supercollider)
- [Git](http://git-scm.com/download/win)
- [svn client (sliksvn)](https://sliksvn.com/download/)
- [cmake](http://www.cmake.org/download/)
- [NSIS](http://nsis.sourceforge.net/Download)
- [Visual Studio 2013, community edition](https://www.visualstudio.com/en-us/products/free-developer-offers-vs.aspx)
- [notepad++](http://notepad-plus-plus.org)
- [7-zip](http://www.7-zip.org)
- [Qt official](http://download.qt.io/archive/qt/5.3/5.3.2/)
- [QT mingwbuilds](http://sourceforge.net/projects/mingwbuilds/files/external-binary-packages/Qt-Builds/)
- [msys mingwbuilds](http://sourceforge.net/projects/mingwbuilds/files/external-binary-packages/)
- [portaudio](http://www.portaudio.com/download.html)
- [portmidi](http://portmedia.sourceforge.net/portmidi/) - note: bundled since SC uses Qt5
- [ASIO SDK](http://www.steinberg.net/en/company/developers.html)
- [DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
- [libsndfile](http://www.mega-nerd.com/libsndfile/)
- [fftw](http://www.fftw.org/install/windows.html)
- [readline](http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)


Outro
-----

Thanks to James McCartney, for making this great piece of audio
software publically and freely available.
