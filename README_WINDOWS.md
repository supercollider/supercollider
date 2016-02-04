Supercollider 3.7 for Windows
=============================

**IMPORTANT NOTE**:

- currently the Windows version of SC does not work. Here are the most serious
  problems:
  - communication between the IDE and sclang is seriously troubled. That causes
    the the interpreter (sclang) to crash very frequently (the "IPC bug")
  - currently only the VS 64bit build creates a startable SC. While The
    msys2 build succeeds, scide reports a segmentation fault on startup.
  - often the server does not close properly and stays in memory as "zombie".
    In that case restarting the server will fail (the process needs to be
    stopped from the task manager first)
  - command line mode does not work
  - likely plenty of smaller problems that couldn't be identified because the
    IPC bug makes any serious testing troublesome


The style of the 'Building SC...' chapters tries to accomodate unexperienced
developers. If you have done this before, the chapter 'Quick Steps'
might be right for you.

January 2016


Table of contents
-----------------

- Introduction
- *Installing SuperCollider*
  - System requirements
  - Using the SuperCollider IDE
  - Using SuperCollider in command line mode
  - Configuration- and support files
  - Extensions and Quarks
- *Building SuperCollider for Windows*
  - Overview
  - Quick Steps
  - Code management- and build-tools
  - Obtaining the SC source
  - Additional libraries
  - A temporary custom download for dependencies
  - Using `cmake`
      - Basic steps
      - Additional options
      - Specify library- and include-paths manually
  - Building with msys2/MinGW & QtCreator
    - Building portaudio with ASIO support (msys2)
  - Building with Visual Studio
    - Building portaudio with ASIO support (VS)
  - Creating a distributable installer binary
  - Diagnosing build problems
  - The SC 3.7 release build
- Links
- Outro


Introduction
============

This is the Windows version of James McCartney's SuperCollider synthesis
engine (`scsynth`) and programming language (`sclang`), distributed together
with a dedicated IDE (`scide`).

SuperCollider's central website is on Github:

    http://supercollider.github.io/

Binaries of release-versions and pre-release snapshots are available as
GitHub releases now:

    https://github.com/supercollider/supercollider/releases

Older releases and snapshots of the source code used for releases are available
at Sourceforge:

    http://sourceforge.net/projects/supercollider/files/?source=navbar

There is an online version of SuperCollider Help, the site SCDOC:

    http://doc.sccode.org/Help.html

SC has a life in social media too. Visit for example sccode.org, the Facebook
group SuperCollider, or find SuperCollider tweets, e.g. @sc140tweets. In
addition you are encouraged to subscribe to the two SC mailing lists 'sc-users'
and 'sc-devel'. You will get friendly and qualified replies there, as well as
interest in, and encouragement for your projects:

    http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml


Installing SuperCollider
========================

For SC itself just run the downloaded installer. If necessary select an
alternative install path when prompted during installation. In order to
uninstall, use the default procedure on Windows (Control panel -> Uninstall a
programm) or run Uninstall.exe from within the SC installation folder. Note
that files created in the 'user support directory' are not deleted (see below
for details and an explanation).

IMPORTANT: in order to use the Quarks extension system you *need* to install
Git. It is available from

    http://git-scm.com/downloads


System Requirements
-------------------

### Operating System

 - Windows 10
 - Windows 8
 - Windows 7
 - Windows Vista

### Sound card

Most sound cards running under Windows will work fine - the sound card should
be one that is supported by PortAudio (http://www.portaudio.com). For professional
use do some research and find a card that allows for low latency. If you have
issues with latency, consult http://www.portaudio.com/docs/latency.html
for advice on how to tune your operating system to achieve the best results.
The portaudio build in SC supports various Windows sound APIs, among which you
can chose. Usually you get best results and biggest control over latency by
using the ASIO driver. If your soundcard does not provide an ASIO driver you
should give *ASIO4ALL* a try (see the chapter 'Links' at the end of this document).
See SC help for how to select the audio interface to be used by SC.


Using the SuperCollider IDE
---------------------------

To start up the IDE use the Startmenu link or run the scide.exe program
found within the installation folder. On first startup expect several
firewall warnings to pop up. The SuperCollider components - the language
interpreter `sclang`, the sound server `scsynth` and the IDE `scide` -
communicate among each other over the network, so you will get a warning for
each of the three executables. If you are new to SuperCollider, inspect the
help system and look for the tutorials. You can find the documentation using
Cmd+D, or via the menu entry `Help` -> `Show Help Browser`.


Using SuperCollider in command line mode
----------------------------------------

1. Open a Windows command line window (the cmd.exe program).

2. Navigate to the SuperCollider installation folder.

3. Start up the `sclang.exe` program to enter the SuperCollider command line.

4. You can now type SuperCollider code and evaluate it by pressing
the Enter (Return) key. You can browse the previous commands using
the up and down arrow keys.

5. These are important keyboard shortcuts:

    `Ctrl+t`: equivalent to CmdPeriod.run
    `Ctrl+x`: to recompile the class library
    `Ctrl+d`: to quit the SuperCollider command line

The interface uses the GNU library Readline (Google can help ;) ).

If you want to use SC Help outside of the IDE, you must generate the help
files first by running `SCDoc.indexAllDocuments`. Help files will by default
be created in the 'user application support directory'.


Configuration- and support files
--------------------------------

An important folder containing application related user data (for example
extensions, quarks, Help files, plugins, configuration files, synthdefs) is
located in the Windows 'local application data' folder. This location is a
bit tricky to find (in the file explorer it's grandparent is hidden by default).
You can see it in the environment variable LOCALAPPDATA. Type this on a command
line:

    $> echo %LOCALAPPDATA%

It will likely be in this location:

    C:\Users\<USERNAME>\AppData\Local\SuperCollider

This folder is user writable and *not* deleted by uninstall. Therefore your
settings, quarks, plugins etc persist through SC-updates (beware of potential
incompatibilities with new sc-versions). The folder can easily be accessed
via the IDE (menu -> `Open user support directory`). You can also ask `sclang`
where this folder is located. Type this in the SC-IDE:

    Platform.userAppSupportDir


Extensions and Quarks
---------------------

Extensions are bundles of classes, and optionally plugins and helpfiles that
extend the functionality of core SC. They typically have a root folder that may
be moved to the `Extensions` subfolder of the `user support directory`. The more
flexible and recommended way since SC version 3.7 does not restrict you to a
single parent folder in a hidden system location. Any folder may be included
among the folders scanned by SC at startup ("compiled by the interpreter
(sclang)"). This is done by adding them to the list of `include folders` in a
file called `sclang_config.yaml` in the user support directory. This file can be
edited manually, or through the settings dialog of the IDE, or programmatically
(see help->`LanguageConfig`). The new Quarks system also adds and removes
entries here. After each change in this file the interpreter has to be restarted
("rebooted"). A comfortable and community driven way of adding and distributing
extensions for SC is the so called *Quarks* system. Quarks may be added
programmatically or by using a GUI. See Help->Using Quarks for an overview.



Building SuperCollider for Windows
==================================


Overview
--------

The build system of SC uses the cross platform, open source build system
`cmake`. It can configure builds and generate intermediary build files for many
OS-toolchain combinations.

Since SC version 3.7, the preferred way to build SC is using `cmake` in
conjunction with `msbuild`. In this case the IDE of choice (and the application
to download) would be `Visual Studio 12 2013` (VS). You can also build SC with
the toolchain *MinGW* in conjunction with the unixy build environment *msys2*.
In this later environment building SC is quite similar to building SC on Unix
platforms. The recommended IDE in this environment is Qt Creator, as it works
very well with `cmake`, 'MinGW', and of course 'Qt'. This is the traditional way
of building SC on Windows, and still has several advantages over the "native"
build. Building optional extensions like 'sc3-plugins' or `supernova`, and
enabling `sclang` command-line mode (depending on `readline`) currently only
works with the MinGW toolchain. The msys2/MinGW-build system atm only compiles a
32-bit version out of the box, VS can do both 64- and 32-bit versions.


Quick steps
-----------

This README generally assumes a developer with little experience in building
SC, especially on Windows. For experienced developers the following two chapters
might be enough, they just list the essential steps. After that the whole process
is described again with more detail and background information.


### Quick steps: Msys2 and MinGW (Win32)

#### Install msys2 and MinGW

* Download the msys2 installer from http://msys2.github.io and bootstrap the
  base system. Make sure you update to the latest version before adding any
  new packages.

* Install these packages with `pacman -S`

      base-devel
      git
      mingw-w64-{i686,x86_64}-cmake
      mingw-w64-{i686,x86_64}-toolchain
      mingw-w64-{i686,x86_64}-pkg-config
      mingw-w64-{i686,x86_64}-qt5
      mingw-w64-{i686,x86_64}-portaudio
      mingw-w64-{i686,x86_64}-libsndfile
      mingw-w64-{i686,x86_64}-fftw

* If you need portaudio with ASIO support you have two options: download a
  private build, or build within msys2
    - private download (temporarily):

          https://drive.google.com/open?id=0B7igZrWv7UxdUmxHdThYLUE2QXc

      from folder BuildDependencies. You will also find the SDKs mentioned below.


#### Optionally build portauido

* copy dx7sdk and ASIO2.3SDK to /usr/local/share
      $> mkdir /repos; cd /repos
* git clone https://github.com/Alexpux/MINGW-packages.git
* cd MINGW-packages/mingw-w64-portaudio

    $> updpkgsums

* make these changes to PKGBUILD (optionally on a separate branch)

      -    --with-dxdir=${MINGW_PREFIX}/${MINGW_CHOST} \
      -    --with-winapi=wmme,directx,wdmks
      +    --with-dxdir=/usr/local/share/dx7sdk \
      +    --with-winapi=wmme,directx,wdmks,asio \
      +    --with-asiodir=/usr/local/share/ASIOSDK2.3

* build
      $> makepkg-mingw -cCifL

*note*:the cC's in the arguments make sure a radical cleanup occures between
the two builds. This does not include the ASIO sdk though.

*note*: in the middle of the build, the script will ask if it
should continue with the second build/architecture. This is
the opportunity to overwrite the ASIOSDK2.3 with a fresh
version.

#### Build SC

* Clone the SC source into your msys2 home folder.

      git clone --recursive https://github.com/supercollider/supercollider.git

* Build as usual with "MSYS Makefiles" as generator (No Qt-prefix required):

      $> cd supercollider; mkdir build; cd build
      $> cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
      $> cmake --build . --target install
      $> cmake --build . --target bundle   # copies dll's into install folder (be patient)

* To create the distributable installer do:

      $> cmake --build . --target installer  # creates the binary installer, requires NSIS in your path

You should find the SC install in the subfolder `Install` of the
build folder.


### Quick steps: Visual Studio 12 2013

* Install

  - git, svn, cmake, NSIS and make sure they are in the search path
  - Visual Studio 2013, DirectX, Windows SDK
  - Qt5 (online installer, at minimum the msvc2013_64_opengl libraries)
  - For now download prebuilt binaries of additional dependencies from:

        https://drive.google.com/open?id=0B7igZrWv7UxdUmxHdThYLUE2QXc

    and extract the contained folders into a sibbling folder of the SC source.
    Or:

      - Download libsndfile, fftw3 and optionally readline
      - create a folder x64 for 64-bit, and/or x86 for 32-bit dependencies as sibling
        of the supercollider source
      - copy/extract/install the dependencies into these folders
      - using the 'lib' tool accessible from VS Developer prompt, create import
        libraries for: libsndfile-1.dll, libfftw3f-3.dll and optionally
        libreadline6.dll

    * Compile portaudio using cmake

      - use the latest svn checkout (rev 1952 or bigger) and move it to a sibling
        folder of the supercollider source (not in x64!).
      - add the ASIO SDK and make sure it is found during configuration
        (the ASIOSDK2.3 folder should reside in a sibling of the portaudio folder)
      - create a Release and a Debug build. The SC build will sellect the correct
        version (only Release and Debug are supported at this time)

* Build with "Visual Studio 12 2013 Win64" as generator file. You will need to specify
  (and identify) the correct path to Qt (parent of the `bin:lib:include` that fit's your
  environment) and pass it to cmake (-DCMAKE_PREFIX_PATH (sic)). The build type
  (configuration) is specified in the build step (`cmake --build .`), not during
  the first cmake run that creates the build files. The build configuration
  defaults to Debug. Currently only 'Debug' and 'Release' are supported. So
  building on the command line would look like this:

      $> cd supercollider; mkdir build; cd build
      $> cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_PREFIX_PATH=C:\Qt\5.4\msvc2013_64_opengl ..
      $> cmake --build . --target install --config Release -- /m:4

                              ****

Code management and build tools
-------------------------------

In order to build SC you need recent versions of a few tools to download and
manage your code, to edit the source, analyze problems, build the executables
and create a binary installer. Don't immediately download them, you will want
to use the package manager if you build with msys2.

* For retrieving and managing the sources:

  - Git (>v1.9.4): the version control system used for SC. Make sure Git is not
    set to convert Unix (`\n`) to Windows (`\r\n`) line endings. If necessary add
    `autocrlf = input` in the core section of your git configuration file.

  - SVN (>v1.8): the version control system used by the SC dependency `portaudio`

* For configuring the build and generating intermediate build files:

  - cmake (>v2.8.11): used for SC (and Portaudio when building with Visual Studio)

* For a comprehensive environment for editing, debugging and controlling the
  build, get one of:

  1. Visual Studio 2013 (v12), which includes `msbuild`. Optionally the Windows
     SDK for your Windows version. If you choose this, you will have to download
     dependencies (see below) from the original providers.

     If your keyboard reflexes are 'dir, del and \', if you want the native toolchain,
     the VS debugger, or want to try instrumented builds (optimising SC for your specific application);
     and if you don't (yet) need sc3-plugins, supernova or command-line sclang, you want
     this.

     You'll still be at the commandline alot, do yourself a solid and get `ConEmu`.

  2. Msys2 & MinGW (and QtCreator): unixy build environment and packages
     repository. The required packages (build-tools, toolchain and dependencies)
     are obtained with `pacman`, a clone of the ArchLinux package manager. This
     means you can downloaded everything from the command line, and it will get
     stored in default system locations automatically.

     If your keyboard reflexes are 'ls, rm and /', if /usr does not look like a
     misspelled word, if you prefer typing 'packam -S git' to downloading
     git from a website, if a 32-bit version is enough, and if you need any of
     sc3-plugins, supernova and sclang command line mode, you want this.

* To create a distributable binary installer:

  - NSIS (v2.46 or 3.0b1): a free tool to create binary installers

You will need a text editor that supports unix line endings and a file
decompression tool for formats other than .zip. Common free tools are:

  - Atom
  - Sublime Text
  - Visual Studio Code
  - Notepad++
  - 7-zip

Obtaining the SC source
-----------------------

SC is on Github:

    https://github.com/supercollider/supercollider

Get a copy of the source code with:

    git clone --recursive https://github.com/supercollider/supercollider.git

The `recursive` argument will download all submodules as well. If you forget it,
you will have to initialize the submodules manually:

    git submodules init

As different checkouts (revisions/versions) of SC might require different
checkouts of submodules, you need to run `git submodules update` if
resynchronization is necessary. Git/SC will warn you if this is required.

There are snapshots of release versions of the source on Sourceforge:

    http://sourceforge.net/projects/supercollider/files/Source/

But you do not want to miss out on being able to manage your code and access the
entire source history with git. Release versions of SC can be checked out as `tag`s.

By default `git clone` will create a folder called `supercollider` in the folder
where you run the command. Think a moment about where to store the source. If
you use msys2, you will want to install that first and then work in the msys2
home folder (see below the dedicated chapter). With VS you might prefer a folder
within your Windows user folder. It is also common to create a isolated project
folder in the root of your system drive. This makes paths short and allows you to
make sure that pathname requirements of elder tools are met (e.g no spaces,
short). If for some reason you need both build systems, you can use the same
source folder for the VS build too. As long as you don't open a msys2 shell,
the msys tree behaves like any other one on the system. In an msys2 shell paths
to msys- (and optionally mingw-) build tools are added at the beginning of the
search path. This will cause problems for a vs/msbuild-build.

Make sure your build folder is outside of the SC source. Traditionally a
subfolder of 'supercollider' is used, but this is not mandatory. If the name of
the build folder starts with 'build', git is set to ignore it.


Additional libraries (core dependencies)
----------------------------------------

The SC source comes with various dependencies bundled as submodules (see the
subfolder `external_libraries`). Additionally you need to obtain the library
packages listed below. They are not easy to compile, so - with the exception
of portaudio - we recommend using the binary distributions.

You will need to obtain these libraries:

* Qt5 (>5.5.1 use the latest available). Qt5 provides libraries for both the
  SC-IDE and sclang. For the VS build, use the official distribution, for the
  msys2 build get them with msys2's package manager `pacman`.

* Portaudio (VS: svn rev1952 or as provided by msys2). The latest svn-revision
  compiles easily using cmake and VS. To get ASIO driver support you need to
  add the ASIO SDK to the tree of sources. Msys2 provides a portaudio package,
  but unfortunately without ASIO support. You will have to recompile the package
  if you need it.

    * ASIO SDK (v2.3) - optional but strongly recommended

    * DirectX (VS: latest version installed in Windows, msys2: version 7) - optional

* Libsndfile (v1.0.25) - not required but providing the functionality to read and
  write many sound file formats.

* FFTW (v3.3.4) - not required but superior to the bundled Green FFT

* Readline (>5) - not required for build. Provides the interface in command line
  mode (command line mode is currently broken in the Windows builds)

Weblinks to the recommended distributors are provided at the end of this document.
Additional detail is given in the description of the individual build systems/steps.

*WARNING*: As said above, if you build with msys2, you must must use Qt from
msys2 as well. Do not mix the officially distributed Qt version with
msys2/MinGW-Qt (at least this has not been tested, and is warned against by many
gurus). SC and Qt should be compiled with the same toolchain. You *can*
have two or more versions of Qt on your system, but you must make sure that
cmake sees the intended version for your *current* build environment. If this
version is not contained in the default locations of what cmake sees as your
current base system, the safest way is to add the path of the Qt-root (parent
of bin:lib:include in the targeted Qt tree) to the cmake variable
`CMAKE_PREFIX_PATH`. This value can be passed to cmake with a command line
argument (preceded by `-D`) or as environment variable
(`set CMAKE_PREFIX_PATH=X:\Qt...`). You would usually do that in a shell/command
line. IDEs additionally allows to control the environment from the project
settings.

If you insist on using official Qt with MinGW, you need to install the MinGW
compiled Qt-version, which also comes with the MinGW toolchain (contained in
the folder 'Tools'). Note that currently there only is a 32-bit version
available. You will then have to decide how to build `portaudio`,
which is not distributed as binary by the portaudio team. The current svn
version (rev 1952) builds nicely with cmake, but only when you use the VS
compiler. In order to compile with MinGW the supported way, you would have to
run a configure script, which requires a unixy environment like msys2.

So if you want to go for the simple (i.e. "downloadable") and tested
alternatives, there are basically two alternatives:

  1. go with cmake & msys2 only (using all their packages)

  2. use cmake & VS for protaudio and supercollider, plus the binary downloads
     from the original providers

When dealing with build dependencies, the VS build is still a bit more involved.

An important advantage of the msys2 build is that libraries are automatically
moved to default locations where cmake can find them. So the situation is quite
similar to Linux and OSX, where you just "install" a dependency and then take
for granted that any build finds and uses it. On vanilla Windows (without the
msys "implant") the situation is different, as there are no, or only weakly
defined/enforced default locations. Therefore, when building with VS, you will
either have to let cmake know where to find the libraries (which can be a
quite involved process), or store the files in the locations described below (see
'Building with VS'). In the later case the customized find-modules provided
in the SC source can find them automatically (compare the `cmake_modules` folder
in the SC source).

*NOTE for the VS build:* For the time period leading to the 3.7 release we
provide a download that contains all required files in the suggested tree. You
just need to extract one zip file into the parent folder of the SC source and
should be set (32- and 64-bit versions provided, import libraries already
generated, folder names an nesting as expected). The folder structure proposed
is explicit as to the target architecture used (x86/x64). The msys2 and VS build
can coexist. Get the download from this folder:

    https://drive.google.com/open?id=0B7igZrWv7UxdUmxHdThYLUE2QXc

*NOTE for the msys2 build:* As DirectX 7 is not easy to find and you only need
the `include` and `lib` folders, another zip file containing a stripped down
version is provided in the download folder linked above too. The use is described
in the chapter dedicated to the msys2 build. Note also that you do not need to
provide DirectX to build portaudio with ASIO and MME support only.


Using `cmake`
-------------

### Basic steps

No matter whether you build on the command line only or use an IDE, whether
you choose the VS- or msys2-environment, the build process will always start
from running `cmake` (you can run cmake from Qt Creator, and there is also
a Visual Studio extension that allows it).

Cmake configures the build and generates intermediary build files by evaluating
arguments you provide on the command line together with a tree of files named
`CMakeLists.txt` starting from the top level source folder. We assume below that
SC source and the the dependencies are in place. For msys2 this is the case if you
installed it properly and downloaded all required packages as described in the
chapter dedicated to msys2. For the VS build this is the case if you place the
dependencies as described in the VS chapter, or if you downloaded the provided
zip file and extracted the contained folders (x64, x86, portaudio, ASIOSDK2.3)
bo be sibling folders of the SC source.

If you build in the SC-source subdirectory `build`, the initial steps are:

1. Open the shell fitting your build environment:

    - for VS: "Developer command prompt for VS2013"
    - for msys2: "MinGW-w64 Win32 shell"

2. Enter the SC source directory, create a subfolder `build` and enter that
   folder.

        $> cd supercollider; mkdir build; cd build

3. Run `cmake` with these arguments:

    - msys2/MinGW:

          $> cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Debug ..

          -G  defines the generator activated by cmake
          -D  can provide values for cmake variables. In the msys2 build
              CMAKE_BUILD_TYPE is used to define the build configuration (Debug
              or Release). The VS build ignores this setting.
          ..  points to the SC source folder and tells cmake where to find the
              first CMakeLists.txt-file to evaluate.

          Note: When identifying the compiler location for MSYS and MinGW
                generators, cmake depends on the search path. It is
                automatically set when you start the mingw32 or mingw64 shell.
                This means that you switch between 32- and 64-bit builds of SC
                when you switch the shell, which is a convenient way to switch
                the search path.

          Note: While not supported by msys2 officially, you can actually run
                the SC build - which does not depend on unix build tools by
                itself - in a Windows command prompt. All you need to do is
                prepend the mingw??-path to the Windows path in your shell:

                    $> set PATH=C:\msys\mingw32\bin;%PATH%

                Cmake (running from the mingw-tree) is clever enough to find
                all other libraries relative to this path. This means that
                strictly speaking - if `ls`, `/` and `rm` are too hard to
                stand for longer than the inescapable you are only bound to the
                msys2 shell for installing and maintaining the packages and
                for building portaudio. The actual SC build can be done in a
                "pure" Windows environment.

    - Visual Studio:

          $> cmake -G "Visual Studio 12 2013 Win64" -DCMAKE_PREFIX_PATH=C:\Qt\5.4\msvc2013_64_opengl ..

          Note: no build type is specified. This is because the VS generator
                supports VS's ability to compile with different build
                configurations from within the IDE. This means that the build
                type or configuration needs to be specified in the next step
                that triggers the actual build.

          Note: outside of the msys environment there is no way for cmake to know
                where Qt is stored. You therefore have to provide the path with
                DCMAKE_PREFIX_PATH or create an environment variable of the same
                name:

                    $> set CMAKE_PREFIX_PATH=C:\Qt\5.4\msvc2013_64_opengl

                Study the Qt tree of your distribution and identify the parent
                directory of the folders bin:include:lib in the branch that fit's
                your target architecture and toolchain.

          Note: you determine whether to build 32- or 64 bit SC by choosing a
                different generator. For 32-bit omit the `Win64` at the end of
                above generator name. The VS generator does not depend on the
                MS compiler being stored in the path. Changing the path is
                therefore not the way to change the target architecture.

4. Provided cmake did not return an error, and that you want to continue on the
   command line rather than switching to an IDE, you can now start the build
   process:

   - msys/MinGW:

         $> cmake --build . --target install

         - The dot after `--build` indicates the location of the file that contains
           cached build configuration information: CMakeCache.txt.

         - You can speed the build up significantly on multi core systems by
           adding the flag `-j` with the number of provided cores after `--`:

             $> cmake --build . --target install -- -j4

         This will build and install the SC files, but it will not add the
         required dynamic libraries provided by the dependencies. If you remain
         in the build environment, they can be found via search path, but for
         a complete build you will have to add another step:

             $> cmake --build . --target bundle -- -j4

         Be patient, this step takes very long with the mingw tools.

         By default the build is installed to the subfolder `Install`.

   - Visual Studio

         $> cmake --build . --config Release --target install -- /m

         Note: the build type (build configuration) `--config Release` is
         provided here. It cannot be set in the preceding cmake configuration
         step. Supported configurations are 'Release' and 'Debug'. Cmake's VS
         generator is a so called 'multi configuration generator' that generates
         separate intermediary build files for all configurations supported. This
         means you do not have to rerun cmake to switch configurations, you can
         switch from within the IDE and build different configurations a lot
         faster as the buildfiles don't have to be regenerated.

         Note: although the IDE offers four alternative configuration options,
         only 'Release' and 'Debug' will build cleanly. This is because different
         versions of the portaudio-dll are linked to for different configurations.
         In a basic configuration cmake only provides two different configuration
         types for linked libraries: 'debug' and 'optimized'. They are mapped
         to 'Debug' and 'Release' in the overall build.

         Note: the attached `/m` makes msbuild use all your cores :)

         Note: The --target bundle step is not required in the VS build

Voilà, you should have a working install of SC on your system. If you want to
create a distributable binary installer, read the chapter towards the end of this
text. Provided NSIS is installed correctly on your system, you just have to build
the target installer:

        $> cmake --build . --target installer

*Note:* When regenerating your build files (running `cmake...`), it is not
necessary to retype all arguments, provided the cmake configuration cache
CMakeCache.txt is still available and can be used. Just add the variable you
want to change, and point to the SC-root, for example:

        $> cmake -DSUPERNOVA=ON ..

All other settings (build type, library location etc) are retained. Note though
that some changes could cause an unclean state of the build files. If you observe
unusual build problems, you might want to delete the cache file, or even delete
the entire build folder and build from scratch. Experience has shown that a mere

      $> cmake --build . --target clean

is sometimes not enough. See 'Dirty build states' towards the end of this
document for a discussion of the topic. If you regenerate from scratch you will
have to provide all arguments again.


### Additional options

Learn more about the cmake system by running `cmake --help`. A good window into
the SC build configuration is the cache CMakeCache.txt in the build folder, and
the files CMakeLists.txt scattered over the project. A longer list of variables
defined for the build is retrieved by adding the flags -L or -AL to the command
`cmake ..`.

The file CMakeCache.txt can be manually edited and is the best place to verify
that all variables have been assigned the values expected. Keep in mind that
the `clean` step does not delete or change this file. This simplifies
configuration, but may also cause trouble if wrong values are kept. For a
guaranteed clean build delete this file, or the entire build folder.

Common variables to modify the build configuration are:

 * Location where SC get's "installed". The default location is set to
   './Install'. If you maintain multiple builds you could either use different
   build trees or install to different folders. Change this for example
   like so:

       CMAKE_INSTALL_PREFIX=./x86/AllPullRequests

   This installs the files to a subfolder x86/AllPullRequests of the build-
   directory. Note that cmake uses forward slash as folder separator on all
   platforms. You may also specify an absolute path here. This "install"
   is different from creating a binary installer described towards the end
   of this text. Note though that you *can* zip the install folder and run
   it on a different system. There is no installer magic required to get SC
   running on other computers.

 * Enable compiler optimizations for your local system (could cause a faster
   binary, but could also cause problems. Don't use this if building for
   other computers than your own.)

       NATIVE=ON                             # gcc/mingw only

 * Build the *supernova* server:

       SUPERNOVA=ON

   *Note*: When you build with supernova, an alternative server executable and
   a supernova version of each plugin is built. If you also use the 'sc3-plugins'
   package, make sure to compile them with supernova support too (does currently
   not work with the VS build).

   Within SC you will be able to switch between scsynth and supernova by
   evaluating one of lines below and then booting the server as usual:

       Server.supernova
       Server.scsynth

   Check SC help for `ParGroup` to see how to make use of multi-core hardware.


### Specifying library- and include-paths manually

If for some reason you cannot use the suggested locations for dependencies, you
will have to tell cmake where to find them. The easiest, but sometimes unfeasible
way is to provide them as command line argument to `cmake`:

    $> cmake -DSNDFILE_LIBRARY_PATH=F:/scbuild/libsndfile ..

If you need to do that for all library- and include-path required, you will get
a loong cmake command. One option is to store these values in a batch-file that
you can edit at leisure. This has the added advantage that you can delete the
cmake cache file to get rid of dirty configuration data and recreate
the cache with a single short command.

Here is a example that allows to edit all paths to dependencies. They go into
a single environment variable (`CMAKE_PREFIX_PATH`) that cmake will read. The
lines here contain random data to demonstrate some possibilities. The case
of portaudio is a bit more complicated, because we need to distinguish two
library versions (Debug and Release). See the module `FindPortaudio.cmake`
in the cmake_modules folder for more details. Note this option (or complication)
out of the box is only available in the VS build:

    set CMAKE_GENERATOR="Visual Studio 12 2013 Win64"
    set QT_HOME=C:/Qt/5.3/5.3.2
    set QT_COMPILATION=msvc2013_64_opengl
    set QT_PREFIX_PATH=%QT_HOME%/%QT_COMPILATION%
    set SNDFILE_PREFIX_PATH="C:/Program files/nerd/libsndfile"
    set FFTW_PREFIX_PATH=%USERPROFILE%/fftw
    set READLINE_PREFIX_PATH=D:/scdeps/x64/readline
    set PORTAUDIO_PREFIX_PATH=../../x64/portaudio
    set CMAKE_PREFIX_PATH=%QT_PREFIX_PATH%;%SNDFILE_PREFIX_PATH%;%FFTW_PREFIX_PATH%;%READLINE_PREFIX_PATH%;%PORTAUDIO_PREFIX_PATH%
    set PORTAUDIO_LIBRARIES=debug;%PORTAUDIO_PREFIX_PATH%/Debug;optimized;%PORTAUDIO_PREFIX_PATH%/Release

    cmake -G %CMAKE_GENERATOR% -DPORTAUIDO_LIBRARIES=%PORTAUDIO_LIBRARIES% ..

    REM -DCMAKE_BUILD_TYPE=Debug          #use only in msys2 build
    REM -DSUPERNOVA=ON
    REM start Supercollider.sln


Another way of specifying cmake command line arguments is creating a "toolchain"
file. This is the cmake suggested method. Please look up the cmake documentation
if you require an advanced configuration and are interested in this approach.


Building with msys2/MinGW
-------------------------

### Introduction

Because of SC's Apple/Unix background, and the requirement to build with unix
build tools, past SCWin releases have been built within a mini unix emulation
running on top of Windows, called `msys`. Msys provides an environment to use
unix build tools and access the toolchain MinGW, which is basically a Windows
port of gcc. For the time being the msys/MinGW build environment is the only
one to successfully build 32-bit SC 3.7 with readline support, sc3-plugins and
supernova. Msys has recently been revitalized and extended by a package manager
that emulates the ArchLinux package manager `pacman`. This version is called
msys2 and is used exclusively in this description. It provides all SC
dependencies in an integrated file tree following unix standards.

The first step of assembling this system is downloading the installer executive.
All following steps are done from within the provided bash-shell with `pacman`.

Msys2 provides full environments for both 32- and 64-bit builds. Managing this
difference within one system requires naming conventions that can be confusing
in the beginning. On top of this we need to distinguish whether a package
belongs to msys2 (unix build tools that require a custom environment) or MinGW,
the actual toolchain for *native* Windows applications that can run outside of
msys. All *libraries* used for compiling SC have to be in the MinGW tree, which
usually starts at `/c/msys/mingw32` (or 64). Tools (cmake, git a.o.) could
belong to either environment.


### Bootstrapping the system

Run the installer downloaded from:

    http://msys2.github.io

This will start a shell from which you should first update the package
information, and then the core system:

    $> pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime

Make sure to close the shell after this and reopen it to refresh settings. The
shell can be opened from the Start menu or by running `msys2_shell.bat` from the
top level msys2 folder.

Now update the remaining packages:

    $> pacman -Su

This will install a basic environment (not yet including the toolchain) and
update it to the latest version.


### Installing the packages

Let's first install Git:

    $> pacman -S git

This pulls in a lot of stuff, including openssh, perl and vim and ends by
suggesting to install python2 and subversion. Let's do it, and add the editor
'nano' to the mix:

    $> pacman -S python2 svn nano

You could add these three lines to your ~/.bashrc to simplify keeping your
packages up-to-date a bit:

    alias cu="pacman -Sy; pacman --needed -Sy bash pacman pacman-mirrors msys2-runtime;"
    alias pu="pacman -Sy; pacman -Su"
    alias edit="start notepad++"

This will create three commands, `cu`, `pu`and `edit`. The first two to simplify
the system/package update and the third to edit texts in notepad++. Updating
an individual package would be done by:

    pacman -U nano

The flag for removing individual packages is `-R`

The important tool we are still missing is `cmake`. Try:

    $> pacman -S cmake

The return value likely is `error: target not found: cmake`. This doesn't
necessarily mean that `cmake` isn't provided by msys2. In fact it is, only
it is provided within the MinGW branch, and all packages there carry a prefix
that identifies them as 32- or 64-bit versions. Lets take this as a little
lesson. Let's ask pacman about cmake:

    $> pacman -Ss cmake

At the time of this writing this returned six packages, three for each mingw32
and mingw64. If you are new to this, take some time to get used to the
prefixes:

    mingw32/mingw-w64-i686-cmake 3.2.1-2
        A cross-platform open-source make system (mingw-w64).
    mingw32/mingw-w64-i686-cmake-git r25180.3bbdb23-1
        A cross-platform open-source make system (mingw-w64).
    mingw32/mingw-w64-i686-extra-cmake-modules 1.5.0-1
        Extra CMake modules (mingw-w64)
    mingw64/mingw-w64-x86_64-cmake 3.2.1-2
        A cross-platform open-source make system (mingw-w64).
    mingw64/mingw-w64-x86_64-cmake-git r25180.3bbdb23-1
        A cross-platform open-source make system (mingw-w64).
    mingw64/mingw-w64-x86_64-extra-cmake-modules 1.5.0-1
        Extra CMake modules (mingw-w64)

The first prefix just indicates to which toolchain branch the package belongs.
After the slash you get the so called mingw package prefix. There are various
types of triplets around, allowing to identify provider, host- and target system.
These elements become particularly important when cross-compiling. The middle
element identifies the operating system of the host system (in this case 64-bit
Windows). For our purpose it is enough to pay attention to the third element,
which identifies the target system: do we build *for* a 32- or 64-bit systems?
If you are happy to go for 32-bit only, it is enough to pick the package with
the `mingw-w64-i686` triplet. For a comprehensive build system that can build for
both architectures, you would have to install both versions of
each package. Let's do the later for cmake:

    packman -S mingw-w64-i686-cmake mingw-w64-x86_64-cmake

Again quite a few dependencies are pulled in. After the install is finished,
let's run cmake:

    cmake --version

Did you get a meaningful answer? If yes, get very alert: you apparently have
cmake installed under Windows. It is not entirely impossible to run Windows
cmake in msys2, but it is not clean. Run:

    which cmake

You will likely get a Windows path (converted to Unix syntax: /c/Progra...) as
return value.

So how to make sure that the msys/mingw cmake is used? Or, why in the first
place, didn't the mingw-cmake start we just installed? ->  because the currently
active search path is wrong.

Why is it wrong, and how should it be corrected? -> by opening the right shell
for the job.

If only maintaining msys2/MinGW, we can work in the shell called `Msys2 shell`,
but if we need to access the packages stored in the mingw branches, we need to
pick a shell that includes them in the path. So for running cmake, and for
building SC you will have to use the `mingw-w64 Win32 shell` (or 64). Now try
that and repeat `cmake --version`. It should answer something sensible. But how
do you know it's msys/MinGW's cmake, and not the one installed under Windows?
Try again `which cmake`, and it should return a MinGW-path now. You can also try:

    where cmake

This time you should get both path's returned. Only the first one will be used,
and that is what we want.

After the basic system setup let's progress to toolchain and libraries required
to build SC. The build stuff first, using package groups:

    pacman -S base-devel

This brings in a lot of packages, among others `make`, autotools, a debugger,
pkg-config and more. Notice that these are all msys packages. For fun let's get
some more:

    pacman -S p7zip ruby perl wget

Noticed it, it told you that perl and wget were already installed and gives you
a chance to back out of the install.

Now the toolchain, we need it for the system we are targeting, so either i686
(32-bit), x86_64 (64-bit), or both:

    pacman -S mingw-w64-i686-toolchain mingw-w64-x86_64-toolchain

Now we are getting the hardcore stuff, including some MinGW versions of packages
that were already included in msys development packages. Various compilers, libs
and a MinGW version of the gdb-debugger. Note that libreadline is already
included here.

Now Qt5, we only need the packages with dynamic link libraries. We will not
consider the option of building Qt5 ourselves. The fastest way to get everything
is to install Qt-creator, which will pull in the Qt5 libraries as dependencies:

    pacman -S mingw-w64-i686-qt-creator mingw-w64-x86_64-qt-creator

For some reason pkg-config seems to be missing from the meta-packages,
so we need to install them:

pacman -S mingw-w64-i686-pkg-config mingw-w64-x86_64-pkg-config

At this point only the actual audio stuff remains to be installed:

    pacman -S mingw-w64-i686-portaudio mingw-w64-i686-libsndfile mingw-w64-i686-fftw mingw-w64-x86_64-portaudio mingw-w64-x86_64-libsndfile mingw-w64-x86_64-fftw

Once everything is in place you may proceed with the steps described in the
`cmake` paragraph above. If you already know you want ASIO support, read the
following chapter too. But you may as well try if what you've done until now
actually works, and come back later.


### Building portaudio with ASIO support (msys2)

As the msys2 portaudio package does not offer ASIO support, we have to build
that package ourselves within the msys2 environment. Msys2 provide's very well
for this requirement: All msys2/MinGW packages are hosted on Github. To get the
sources for both mingw and msys2 packages, clone these two repos (for our
purposes the mingw repo is enough). This is commonly done in a folder called
`repos` in the root of msys2:

    git clone https://github.com/alexpux/MINGW-packages.git

The portaudio package is found in `/repos/MINGW-packages/mingw-w64-portaudio`.
Note the missing `i686` or `x86_64`. This will be added to the name of the
built packages.

We will have to download the ASIO SDK provided by Steinberg (free, see Links
at the end). Put the ASIOSDK2.3 folder in some easily accessible (and writeable)
folder. You may also add the DirectX SDK v7 to support Direct Sound (newer
versions don't work out of the box with the portaudio version provided by
msys2). It is enough to provide the folders `include` and `lib`(30 MB)
in a parent like `dx7sdk`. Put it next to ASIOSDK2.3.

The build script configures the build the way it is are built for msys2/MinGW.
Modifications can easily be made by editing the file `PKGBUILD` in the root of
the package source. First step is to bring in the latest version of source and
patches:

    $> updpkgsums

Let's go ahead and specify that we want to enable ASIO, and where the SDKs are
located. You may want to do this on a new git-branch. Open `PKGBUILD` in an
editor and:

  * Go to the `build ()` function and append `, asio` to the `--with-winapi`
    line (42).

  * Adjust the path to the DirectX SDK in you environment (e.g.
    `/usr/local/share/dx7sdk`).

  * Add the path to the ASIO SDK in a new line (doesn't matter where, but don't
    forget the backslash):

    `--with-asiodir=/usr/local/share/ASIOSDK2.3`

That are all additions necessary to the configuration. Now we have to run the build
and provide a few arguments to get what we want:

  * build for both 32- and 64-bit (default)
  * make sure the source is wiped before the second build to prevent any
    interference with the previous build files (`clean` is not enough).
  * we want to install and keep the build package in case we need to reinstall
    later

The command to run a mingw-build is `makepkg-mingw`. We want the flags -cCifL
(check the options by running `makepkg-mingw --help` and use your own ;)

*WARNING*: if you build both 32- and 64-bit versions you will run into a problem:
some intermediary build files are written to the ASIO SDK and not cleared out
before a new build! The safest option is to remove the old folder and replace
it by a fresh copy. The right moment to do this is when the build script prompts
you: "Continue with build?" before the second compile (32-bit). If you only want
to build for one architecture, precede the build command with an environment
variable like so:

    MINGW_INSTALLS=mingw32 makepkg-mingw -cCifL

This setting will be gone when you reopen the shell.


### Building with QtCreator (QtC)

QtCreator (the IDE bundled with Qtlib) has excellent cmake and MinGW integration.
It can evaluate the CMakeLists.txt files provided in the SC source, and directly
uses the MinGW generator files created by cmake. You can control cmake from
within the IDE. During this process you will be prompted to provide cmake
arguments. If you run QtC as provided by msys2, no arguments are required.
For optional arguments refer to the chapter on using cmake above.

At the time of this writing QtC does not automatically add the targets `install`
and `bundle` to the build chain. You will have to do this manually. You may
add and subsequently disable the target `bundle` as takes a long time to complete
without adding anything new when your environment doesn't change. You may do
the same with the target `installer`, which creates the binary distributable
installer.

If you need to use a different version of QtC than the one provided by msys2,
you will have to pass the location of the dependencies to cmake. This can be
done by passing in the root of your dependencies tree with the variable
`CMAKE_PREFIX_PATH` (refer to the chapter 'Using `cmake`' above for some
considerations. In some cases you might need a more fine grained approach.
If you use the entire MinGW32-tree, all dependencies can be found in a single
go. Be aware though that in that case you also need to set the toolchain used
by QtC to the one provided by msys2/MinGW.

If you want to use the original Qt distribution you need to use the MinGW-
toolchain provided by Qt too. It is found automatically by Qt-provided QtC,
but the default might be set to the VS compilers if you have VS installed on
your system. For considerations why this approach might be less advisable, read
the chapter 'Additional libraries' above. The main problem is that it is not
easy to obtain a mingw-based portaudio build without installing the msys2
environment. If you do that, you might as well use the comfort provided by using
the msys2 system for all your requirements.


Building with Visual Studio (VS)
--------------------------------

Most of the preperatory steps needed to work with VS IDE have been described in
the `cmake` chapter. The cmake VS generator creates a VS solution file that can
be opend in VS to work with the project. There are a few additional details to
be taken into consideration when trying to build SC with VS.

- Three the dependencies are not provided as VS built binaries by the
  original providers (libsndfile, fftw, readline) - you will have to create
  so called "import libraries" for them.

- Portaudio is not provided as binary at all, so you will have to build it,
  optionally adding ASIO and DSound support. Building with ASIO support requires
  to download the ASIO SDK from Steinberg, and DSound support requires the
  DirectX SDK from Microsoft.

- GNU Readline is not provided as VS built binary, and no recent version is
  available as separate download. We therefore need to exctract it form one
  of the MinGW distributions, where it is often bundled.


*Note:* When working on any of this (see below) always use the MS provided
developer shell "Developer Command Prompt for VS2013" to have the necessary
environment created automatically. That way the correct versions of tools
like `lib` and `dumpbin` are directly available on the command line.

*Note:* The following assumes that you have installed git, svn, cmake and NSIS,
and that they are in your search path. You might also need 7z and an editor like
Notepad++ to edit text files with unix line endings. Make sure your git doesn't
add carriage return to line endings in source files. If necessary change to the
sc source directory and run:

    $> git config core.autocrlf input

You should have already downloaded the latest versions of:

  - *Qt5*: installed with the online installer: choose msvc2013_64, with or without
    opengl. If you'd like to get a 32-bit build of SC working in this environment,
    you will need 32-bit Qt as well. The Qt online installer allows to maintain
    several Qt-versions/flavors within one folder tree.

  - *MS DirectX SDK* (optional): Install with default settings

  - *libsndfile*: you may install it in the location suggested by the installer,
    but for better control or independence of your base system read the
    recommendation below.

  - *fftw3f*: see below for the recommended install/extraction location

  - *ASIO SDK*: extract to a sibling folder of the SC source

*NOTE:* In the time towards the 3.7 release we provide a package providing
the binary dependencies (exept Qt and DirectX). You can save yourself the
work described in the following chapters, by downloading it, and placing the
contained folders (x86/x64) next to the SC source. Get it from here:

    https://drive.google.com/open?id=0B7igZrWv7UxdUmxHdThYLUE2QXc


### Creating a folder tree

If you move the dependency folders to specific locations, the customized cmake
find-modules of the SC project can find them automatically when configuring the
SC build. This simplifies building SC significantly. The suggested arrangement
expects the SC-source and all dependencies in a single parent folder. With the
exception of portaudio, the binary dependencies should go into folders indicating
the target architecture: x86 for a 32-bit compile, and x64 for a 64-bit compile.
Within these folders the individual dependencies are laid out as provided by
the original project (flat in the case of fftw, split up in bin:lib:include for
most others, hybrid in case of portaudio). Portaudio is a special case as we
provide a custom build and the headers from the sources in the tree of binaries.
You can also download the portaudio source and build in the default locations.
Cmake will find them (see below).

As a result you should get this folder tree:

    someParent
        supercollider (source)
            buildXYZ
        ASIOSDK2.3
        x64
            libsndfile
            fftw
            readline
            portaudio
                include
                Debug
                Release
        x86
            libsndfile
            fftw
            readline
            portaudio
                include
                Debug
                Release


### Building portaudio with ASIO support (VS)

There is currently no binary distribution of Portaudio available. You need to
download the source and build it yourself. Fortunately the PA source offers
cmake build files working with VS, so we can use build steps very similar to
the ones used for SC.

Move to the folder containing the SC source (the parent folder) and download
PA with svn:

    $> svn co https://subversion.assembla.com/svn/portaudio/portaudio/trunk portaudio

This will create a folder `portaudio` containing the source. In order
to support some of the audio driver APIs available in Windows portaudio needs
one or two additional SDKs. The ASIO SDK provided by Steinberg and the latest
DirectX SDK provided by Microsoft, if you want DirectSound support. Go get them
following the links provided at the end of this document. DirectX will be
installed in the default location and then found automatically. Extract the
ASIO SDK to a sibling folder of the portaudio source.

Once this is in place change into the `portaudio` folder, enter the (existing)
folder build (ignore the scons and msvc content), and run cmake specifying our
generator and the build type required. Then start the build:

    $> cd portaudio\build
    $> cmake -G "Visual Studio 12 2013 Win64" ..
    $> cmake --build . --config Debug

Repeat the build step for the configuration `Release`:

    $> cmake --build . --target clean
    $> cmake --build . --config Release

You should now have two subfolders `Debug` and `Release` that contain the various
libraries provided by the build (specifying --target install is not required).
These two build configurations are currently supported by the SCWin build when
using VS.

If you want 32-bit versions as well, repeat above steps with the generator name
"Visual Studio 12 2013" (no `Win64`). You should clean the build and delete the
build cache before doing this. Also delete the subfolder `.libs` in the ASIOSDK2.3
tree as shown below. Unfortunately the build writes to that folder as well.

If you are currently in the subfolder `build` of the portaudio source, use these
steps:

    $> rmdir /s ..\..\ASIOSDK2.3\common\.libs
    $> cmake --build . --target clean
    $> del .\CMakeCache.txt
    $> cmake -G "Visual Studio 12 2013" ..  # this configures a 32-bit build
    $> cmake --build . --config Debug       # after this repeat with Release config


You could also build versions for the configurations `RelWithDebInfo` and
`MinSizeRel`, but they are currently not supported by the SC build. By default
cmake only supports two build configurations in a multi configuration generator.


### Obtaining and using readline

The Readline library provides an interface for keyboard control of command
line sclang. As of the writing of this guide (April 2015), Readline support
in SCWin VS-build does not work. Sclang crashes when started from the command
line (the default is in Readline mode, sclang can be started in Daemon mode
with the argument `-D`)

As there is no official binary distribution of readline (apart from an outdated
version 5.0.1 which is only available for 32-bit), we need to extract readline
from one of the toolchain distributions that make it available. Readline is
included in both msys2 and the MinGW toolchain provided by Qt.

Get one of them and copy these files from the folders bin, lib and include (not
all is required for SC, but this creates a full self-contained readline package)
into a folder `readline`, using the same subdirectories as the distribution:

    bin/libreadline6.dll
    bin/libhistory6.dll
    bin/libtermcap-0.dll   # dependency of libreadline

    include/readline       # this is a folder containing several headers

    lib/libreadline.a
    lib/libreadline.dll.a
    lib/libhistory.a
    lib/libhistory.dll.a

Do this for 64- and/or 32-bit as required. Move the resulting tree into the
x86 or x64 folders as required and move on to the next step.

NOTE: readline might depend on other libraries at runtime. Atm 32-bit readline
from msys2/MinGW requires `libgcc_s_dw2-1.dll` and `libwinpthread-1.dll` on top of
termcap. Add them to the bin folder. No import-libraries are required.


### Creating import libraries

To get the MinGW build dynamic link libraries of libsndfile, fftw and readline
work in a VS build, you will have to create import libraries, that tell VS what
to expect. This process is a bit tedious and implies editing a textfile
created with `dumpbin` manually. It has to be applied to three library files:

    libsndfile-1.dll
    libfftw3f-3.dll        # mind the "f" (single precision)
    libreadline6.dll

It's the easiest to do this in the 'lib' folder of each of the packages. That is
where the resulting 'lib' file should reside to be found automatically by cmake.

We'll do the walkthrough for libsndfile-1.dll. It's exactly the same thing for
the other libraries. Open the 'Developer Command prompt for VS2013' and head
into x64/libsndfile/lib. Run:

    $> dumpbin /EXPORTS ../bin/libsndfile-1.dll > ./libsndfile-1.DEF

Edit the resulting textfile and remove everything except the list of
functions under the heading "name", and add the heading EXPORTS.

A shortened version of the dump looks like this (this is the readline dump):

    Microsoft (R) COFF/PE Dumper Version 12.00.31101.0
    ...
        ordinal hint RVA      name

              1    0 0002E83C BC
              2    1 0002E874 PC
              ...
            613  264 000153C0 xmalloc
            614  265 000153F0 xrealloc

      Summary

            1000 .CRT
            ...
            1000 .tls

After trimming it should look like this:

    EXPORTS
    BC
    PC
    ...
    xmalloc
    xrealloc


With this edited DEF-file run the following command, and you are set:

    $> lib /machine:x64 /def:libsndfile-1.DEF

*Note:* Use /machine:x86 for 32-bit builds and don't forget the heading EXPORTS in
the .def file.

Repeat this for the remaining two and the last hurdle on the way to
a shiny VS SC-build is taken.


#### Configuring the build with cmake

Please refer to the chapter 'building with cmake' above. Everything required
should be in place by now.


#### Starting the VS IDE

After `cmake --build . --config Debug --target install` finished successfully
you should find the file `SuperCollider.sln` in your build folder. Either open
it from the VS IDE or start the IDE with the solution file loaded:

    $> start SuperCollider.sln

After the IDE processed all project files you should be ready to work. Note that
at the time of this writing cmake<->VS integration is not very good. While
you can always rebuild the initial configuration from within the IDE, changing
the build configuration between Release and Debug schould be safe too. If changing
the architecture (64- vs 32-bit) might consider a different build folder. In any
ways you will have to regenerate build files with cmake. More research and work
is required here.

You will have to configure several user settings to work comfortably. The only
target that can directly run after setting the 'command' in properties/debugging
is 'install'. Executives are first built to the output folder within the target
folders (target/Debug, target/Release). The targets that could in theory run in
the targets folders are sclang, scsynth and scide. Atm they can only be run
from the install folder because all the main binaries require dll's not available
in the target folders.

To simplify working with cmake from within the VS IDE you might like this plugin:

    https://visualstudiogallery.msdn.microsoft.com/6d1586a9-1c98-4ac7-b54f-7615d5f9fbc7

When trying to debug from the IDE, this plugin is helpful, as it allows
to debug child processes easily:

    https://visualstudiogallery.msdn.microsoft.com/a1141bff-463f-465f-9b6d-d29b7b503d7a

*Note*: building the target `install` (accessible in the "Solution Explorer")
copyies all dynamic link libraries to the install folder as well. This differs
from the msys2/MinGW build, where building a separate target ('bundle') is
required for that.


Creating a distributable installer binary
-----------------------------------------

In order to create a binary installer for your build, you just need to build the
target `installer`. This expects the target `install` - and when building with msys2
also target `bundle` having completed successfully before:

    cmake --build . --target installer

Of course these steps can also be done in the IDEs.

In order for this to work you will have to have NSIS installed on your
system. The NSIS installer add's the command line tool `makensis` to the
system, which is required for the SC script to succeed.

The SC installer executable will be written to the same parent folder as the
SuperCollider install-build, for example:

    ...\supercollider\build\x64\Release\SuperCollider-<version-build type>.exe

Note that for just passing SC to a friend it is not really necessary to create
an installer. You can just copy the main SuperCollider folder and copy it
to another machine. The userAppSupportDirectory will be created from scratch.
But you will use your local customizations and additions. The new Quarks system
provides means to make easy porting of extension/Quarks groups easier.


Diagnosing build problems
-------------------------

The most common build problems are related to incorrect versions of the core
dependencies, or dirty states in your build folder. An additional source for
problems could be that dependencies and core sc are built with different
compiler versions or toolchains.


### Checking component versions:

- **cmake**: `cmake --version` (should be bigger than 2.8.11)

- Double-check the downloaded versions for qt5, readline, libsndfiles, fftw
  and portaudio.

- If you build with msys2, make sure you use the Qt package provided by msys2.
  If you build with VS, make sure you use the corect Qt version from the Qt
  site. You want to see `msvc2013` in the parent folder of the core Qtlib
  tree.

When asking for build help, always check and mention the installed versions
any components you used to build SC!


### Dirty build states

While it's generally safe to re-use your build folder, changing branches,
build tools, cmake settings, or the versions of your dependencies can
sometimes put you in a state where you can no longer build. The solution
is to clean your build folder - the common ways to do this, in order of
severity:

1. `del CMakeCache.txt` (delete your cmake settings for that build)
2. `cmake --build . --config Debug* --target clean` (clean your intermediate
   build files, *use your previous build type in place of Debug*)
2. Delete the output of your build. Use the file Explorer or run
   `rmdir /s ./x86/Debug`
   The install is found in `supercollider\build\x64\Debug` or a folder
   corresponding to your build type.
3. Delete everything in the build folder. Use the file Explorer or run
   `rmdir /s .`

Generally, clearing the CMakeCache.txt should be enough to fix many build
problems. After each one of these, you must re-run `cmake` to configure the
build and generate build files. It's recommended that you create a new build
folder for each branch you're building. In practice, though, you can usually
switch between similar branches and rebuild by simply deleting your
CMakeCache.txt.


### Someone else's fault...
If you're *sure* you're doing everything right and you're still failing,
check the travis-ci status page:
[https://travis-ci.org/supercollider/supercollider]()


The SC 3.7 release build
------------------------

**Note**: the following is a sketch of what is going to come, and will be
adjusted to the actual way the SCWin Release 3.7 build will be done.

SCWin64 3.7 was built with Visual Studio 12/2013 update 4

- Qt5.4.1 (msvc2013_64_opengl, 64-bit)
- Portaudio 1.9 (svn rev. 1952)
- ASIO SDK 2.3
- MS DirectX v9 (June 10)
- linsndfile 1.25
- FFTW 3.3.4
- Readline 6.2

and various bundled software packages (boost 1.5.7, portmidi, hidapi and
several more (see `external libraries` in the source tree))

The tools used were Git (1.9.5) and cmake (3.2.1), SlikSVN (1.8.10), and
NSIS (3.0b1) to create the binary installer.

SCWin32 3.7 was built with msys2 and MinGW 4.9.2 as distributed by msys2. The
additional packages were mostly used as provided. The exception was
portaudio, which was recompiled within the msys2 environment with ASIO support.

- Qt5.4.1 (msvc2013_64_opengl, 64-bit)
- Portaudio 1.9
- ASIO SDK 2.3
- MS DirectX v7
- linsndfile 1.25
- FFTW 3.3.4
- Readline 6.2


Links
=====

- [Central SC-site](https://supercollider.github.io)
- [SC source repository on Github](https://github.com/supercollider/supercollider)
  The issue tracker and Github pull requests are developing into an important forum
  for the discussion of the SC application development.
- [SC online help](http://doc.sccode.org/Help.html)
- [SC mailing lists](http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml)

- Requirements/Useful additions to SC
    - [ASIO4ALL](http://www.asio4all.com/)
    - [Git](http://git-scm.com/download/win)

- Build tools
    - [cmake](http://www.cmake.org/download/)
    - [Git](http://git-scm.com/download/win)
    - [svn client (suggested: sliksvn)](https://sliksvn.com/download/)
    - [NSIS](http://nsis.sourceforge.net/Download)

- Documented build environments
    - [Visual Studio 2013, community edition](https://www.visualstudio.com/en-us/products/free-developer-offers-vs.aspx)
        - [Windows 8.1 SDK](https://msdn.microsoft.com/en-us/windows/desktop/bg162891.aspx) - contains cdb debugger used by Qt-Creator (Google should know the link for other Windows versions).
    - [msys2](https://msys2.github.io/)

- Build dependencies (other than the bundled ones)
    - [Qt official](http://download.qt.io/archive/qt/5.3/5.3.2/)
    - [portaudio](http://www.portaudio.com/download.html)
        - [ASIO SDK](http://www.steinberg.net/en/company/developers.html)
        - [DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
    - [libsndfile](http://www.mega-nerd.com/libsndfile/)  - not obligatory but important
    - [fftw](http://www.fftw.org/install/windows.html) - not needed but more powerful than the bundled Green FFT
    - [readline](http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)  - not needed for build
    - [portmidi](http://portmedia.sourceforge.net/portmidi/) - *note*: bundled with SC source since SC 3.7

- Utilities
    - [notepad++](http://notepad-plus-plus.org) - free text editor that handles unix linefeed
    - [7-zip](http://www.7-zip.org) - decompression tool that handles formats from unix world
    - [Dependency Walker](http://www.dependencywalker.com/) - follow up missing library errors
    - [ssh server & client] (https://www.bitvise.com/download-area) - gratis for non commercial use


Outro
=====

Thanks to James McCartney, for making this great piece of audio
software publically and freely available.
