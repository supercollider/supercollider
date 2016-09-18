Welcome to Supercollider 3.8 for Windows!
=========================================

**IMPORTANT NOTE**:

This Readme has two blocks: "Installing SuperCollider", and "Building
SuperCollider for Windows". The first block provides basic information to get
going, and also tries to introduce to SC speak on the way. The second block
is divided into two blocks again, "Quick Steps" and "Walkthroughs". The Quick Steps
contain the essentials for a successful build, the "Walkthroughs" are more
tutorial-like and have unexperienced developers in mind. They are not required
reading to build the Windows version of SC. Furthermore the individual chapters
in "Walkthroughs" are not expected to be read consecutively, they might appear
quite repetitive if done so.

May 2016


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
  - Introduction
  - Quick Steps
    - Required software
    - Arrangement of toolchain and libraries
    - Configuring and executing the build
    - Portaudio
  - Walkthroughs
    - The trial-error approach: building sc3-plugins and aggregating
      build settings to pass to `cmake`
    - Avoiding the command line: from CMake-Gui to Visual Studio (environment
      and GUI-applications)
    - Using Qt Creator
    - More `cmake`: building supernova, qt-less, verbosity and more
    - Recalling environment- and build settings on the command line
    - On the horizon: MSYS2
  - Diagnosing build problems
    - Dirty build states
    - Wrong libraries found
- Description of SC 3.7 release build
- Known problems
- Outro
- Links

Introduction
============

This is the Windows version of James McCartney's SuperCollider synthesis engine
(`scsynth`) and programming language (`sclang`), distributed together with a
dedicated IDE (`scide`).

SuperCollider's central website is on Github:

    http://supercollider.github.io/

Binaries of release-versions and pre-release snapshots are available as
GitHub releases now:

    https://github.com/supercollider/supercollider/releases

This page also provides snapshots of the source version used for the release.
Note though, that this automated Github service misses out on the submodules.
For actual development work you should always use SuperCollider's Github repo:

    https://github.com/supercollider/supercollider

Older releases and snapshots of the source code used for them are available
from Sourceforge:

    http://sourceforge.net/projects/supercollider/files/?source=navbar

There is an online version of SuperCollider Help, the site SCDOC:

    http://doc.sccode.org/Help.html

SC has a life in social media too. Visit a lively community at sccode.org:

    http://sccode.org/

Listen to music composed with SuperCollider on SoundCloud:

    https://soundcloud.com/groups/supercollider

There are several SuperCollider Facebook groups, some SuperCollider tweets, e.g.
@sc140tweets. In addition you are strongly encouraged to subscribe to the two SC
mailing lists 'sc-users' and 'sc-dev'. You will get friendly and qualified
replies there, as well as interest, and encouragement for your projects:

    http://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx


Installing SuperCollider
========================

For SC itself just run the downloaded installer. If necessary select an
alternative install path when prompted during installation. In order to
uninstall, use the default procedure on Windows (Control panel -> Uninstall a
program) or run Uninstall.exe from within the SC installation folder. Note
that files created in the 'user support directory' are not deleted (see below
for details and an explanation).

IMPORTANT: in order to use the Quarks extension system you *need* to install
Git and add it to the PATH. The Git installer will prompt you to the addition.
For SC it is enough to add Git itself to the path, the additional unix tools
(curl, find etc.) are not required.

Git for Windows is available from

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
be one that is supported by [PortAudio][portaudio]. For professional
use do some research and find a card that allows for low latency. If you have
issues with latency, consult http://www.portaudio.com/docs/latency.html
for advice on how to tune your operating system to achieve the best results.
The portaudio build in SC supports various Windows audio APIs, among which you
can chose. Usually you get best results and biggest control over latency by
using the ASIO driver. If your soundcard does not provide an ASIO driver you
should give *[ASIO4ALL][asio4all]* a try. See SC help for how to select the audio
interface to be used by SC.


Using the SuperCollider IDE
---------------------------

To start up the IDE use the start menu link or run the scide.exe program found
within the installation folder. On first start-up expect several firewall
warnings to pop up. The SuperCollider components - the language interpreter
'sclang', the sound server 'scsynth' and the IDE 'scide' - communicate among
each other over the network, so you will get a warning for each of the three
executables. If you are new to SuperCollider, inspect the help system and look
for the tutorials. You can open the help system using Ctrl+D. This keyboard
shortcut is context sensitive - if your cursor is on a term from the
supercollider language ('sclang'), the help system will likely open with the page
explaining that term. Of course there is also the menu entry 'Help' -> 'Show
Help Browser'.


Using SuperCollider in command line mode
----------------------------------------

*Note*: this is currently broken (May 2016, SC3.8). If you are using the 32-bit
version, you can get output in emergency situations: either keep your finger on
the key for about halph a second or type each keystroke twice for it to
register. You can also send text files to sclang by passing the filename as
argument to `sclang`.

1. Open a Windows command line window (the cmd.exe program).

2. Navigate to the SuperCollider installation folder.

3. Start up the `sclang.exe` program to enter the SuperCollider command line.

4. You can now type SuperCollider code and evaluate it by pressing
the Enter (Return) key. You can browse the previous commands using
the up and down arrow keys.

5. Important keyboard shortcuts:

      `Ctrl+t`: stop sound, equivalent to CmdPeriod.run
      `Ctrl+x`: recompile the class library
      `Ctrl+d`: quit the SuperCollider command line

The interface uses the GNU library [Readline][Readline doc].

If you want to use SC Help outside of the IDE, you must generate the help
files first by running `SCDoc.renderAll`. Help files will by default
be created in the 'user application support directory' (see below). Usually
the html-files are aggregated with each help-file opened. With this command all
files are generated at once.


Configuration- and support files
--------------------------------

An important folder containing application related user data (for example
extensions, quarks, Help files, plugins, configuration files, synthdefs) is
located in the Windows 'local application data' folder. In SC-speak this folder
is called the userAppSupportDir. This location is a bit tricky to find when not
using SC-IDE because its grandparent `AppData` is hidden by default. You
can see it in the environment variable LOCALAPPDATA. Type this on a command
line:

    $> echo %LOCALAPPDATA%

You will likely see (the grandpa referred to earlier):

    C:\Users\<USERNAME>\AppData

The userAppSupportDir is in the subfolder `Local\SuperCollider`:

    C:\Users\<USERNAME>\AppData\Local\SuperCollider

This folder is user writable and *not* deleted by uninstall. Therefore your
settings, quarks, plugins etc persist through SC-updates. This is usually good,
but if you have binary "plugins" and update a minor version, e.g. 3.6 to 3.7,
the plugins will not work any more, and cause errors during SC start-up. There
are more files in the `userAppSupportDir` that can cause trouble after updates,
so you want to be aware of the possibility. In the IDE it is very easy to access
the folder. There is a menu entry -> `Open user support directory`). You can
also ask `sclang` where this folder is located. Type this in the SC-IDE:

    Platform.userAppSupportDir


Extensions and Quarks
---------------------

Extensions and Quarks are bundles of classes, and optionally plugins and
help files that extend the functionality of core SC. The old school way of
integrating them into SC is to move them to a folder called `Extensions`  in the
`userAppSupportDir` (soft linking is not supported any more).

The more flexible and recommended way since SC 3.7 does not restrict you to a
single parent folder in a hidden system location. Any folder may be included
among the folders scanned by SC at start-up ("compiled by the interpreter
sclang"). New folders are integrated into SC by adding them to the list of
"include folders" listed in a file called `sclang_config.yaml` in the
`userAppSupportDir`. This file can be edited manually, but you might find it
easier to add or remove folders via the 'settings' dialog in the IDE, or:

The community driven *Quarks* system provides an automated way to add, remove
and also distribute extensions. It "clones" extensions from repositories. By
default it looks in a central repository called "Quarks", located on Github:

    https://github.com/supercollider-quarks

Under the hood it speaks `sclang` and uses Git, so there is even some version
control included. It also provides a management GUI that writes to the
`sclang_config.yaml` mentioned above.

*Note*: Each time `sclang_config.yaml` is modified (by editing, via a menu or by
using the Quarks system) the interpreter has to be restarted ("rebooted") so
that sclang can learn which files are available. This can be done via the menu
entry 'Language -> Restart Interpreter'. For an overview of the Quarks system
see 'Help -> Using Quarks'.

Enjoy SuperCollider!



Building SuperCollider for Windows
==================================

Introduction
------------

In order to provide the same build logic for all platforms, SC uses the cross
platform build system [CMake][cmake]. It can configure builds and generate
intermediary build files for many OS/toolchain combinations. The separation of
build system, and actual application development allows to define cross-platform
build definitions in a single framework, and still do actual application
development in your preferred native environment. On Windows this works nicely
with Visual Studio and Qt Creator. While CMake is command line oriented, it can
also be controlled from a GUI (`cmake-gui`) and is very well integrated into Qt
Creator.

As to Visual Studio, if you generate the solution file with CMake, you can build
from the IDE and CMake will automatically rerun if necessary. In order to
develop for SC proper, by any luck, you might not have to care much about CMake,
and can spend most of the time in your preferred IDE.

The Windows port of SC is in a stage of transition. The older build system used
the Windows port of GCC (MinGW), the newer uses the native Windows build tools
integrated into the IDE Visual Studio. This transition is not complete yet.
SuperNova, an alternative server with multi-core capabilities cannot be built
with VS yet. Also sclang's interactive command line functionality (readline
based) cannot be added in the Visual Studio build at the time of this writing.


Quick Steps
-----------

### Required software

- Source management
  - [Git][Git] for Windows (latest version)
- Build tools
  - [CMake][cmake], latest stable version (minimum 3.4.3)
  - One of:
    - [MinGW][Qt], version *4.82* 32-bit, the distribution shipped with Qt (QT\Tools\mingw482_32)
    - [Visual Studio 12 2013][VS]
- Libraries
  - Required
    - *[Qt][Qt]* versions 5.5.x Versions from 5.3 upwards should work but are
        little tested. Qt5.6 does not work with SC 3.7 because WebKit was dropped
        from the binary distribution.
        Use the official download from qt.io and the online-installer. It offers
        flexibility in choosing different packages in a single go (e.g. Qt,
        MinGW and Qt Creator), and helps managing maintenance. It provides a
        file structure that accommodates different Qt flavours (Qt built with
        different toolchains), versions, and modules within a single tree. We
        will be referring to that file structure throughout this text.

        *Important note*: the flavour of Qt (e.g. msvc2013_64 or mingw492_32) must
        match your toolchain as closely as possible!

        For a 64-bit VS build select msvc2013_64
        for a 32-bit MinGW build select mingw482_32

    - *[libsndfile][libsndfile]* >= 1.0.25
- Recommended
  - *[fftw][fftw]* >= 3.3.4
  - *[readline][readline]* = 5.0.1 (more recent versions, as found in MinGW distributions do
    *not* work at the time of this writing). Readline only works with the 32-bit
    MinGW build
  - *[ASIO-SDK][asiosdk]* = 2.3 (Asio support in Portaudio)
- Optional
  - *DirectX SDK* [v.9][dx9sdk] for VS build. (Direct Sound support in Portaudio)
  - *[NSIS][nsis]*, if you want to create a binary SC installer (add the binary `makensis` to PATH)
- Convenience
  - a Unix line-ending friendly text editor like *[Atom][atomeditor]* or *[Notepad++][notepad++]*.
    Atom is particularly interesting because there is a SuperCollider package available to extend it.
  - an extraction tool for unixy compression formats like tar and gz, e.g. *[7-zip][7-zip]*


Last but not least, the SC source:

    git clone --recursive https://github.com/supercollider/supercollider.git

The SuperCollider source comes with several submodules, or embedded external
libraries. Adding `--recursive` to the git command makes sure that they get
initialized and populated. When switching branches or otherwise traversing the
variations and history of the SC source, it may sometimes become necessary to
resynchronize SC and submodules. The command for that is:

    git submodule update

It may also become necessary to initialize a submodule, for example if a new
submodule is added to the repo, or if you switch to a branch that contains a
submodule that has not been initialized yet. The command is:

    git submodule init

Git will usually tell if a resynchronization/update is required.

*Note*: the Portaudio library is not available as binary download. A slightly
patched version of the pa-source is provided as submodule of the SC source and
built automatically during the SC build. In order to add ASIO and/or DSound
support, the respective SDKs (gratis downloads) have to be obtained and added to
the source tree. DSound support in the MinGW build uses the library provided by
MinGW, so no SDK is needed.


### Arrangement of toolchain, libraries and SDKs

- Toolchain/IDE
  - *[MinGW][Qt]*: embedded in the Qt folder tree, downloaded together with Qt. Likely
    location after install:

      C:\Qt\Tools\mingw482_32

    Compiler and other tools required for the build are found in the binaries
    subdirectory. It needs to be added to the environment PATH:

      C:\Qt\Tools\mingw482_32\bin


  - *[VS][VS]*: default install of VS2013, paths are handled automatically


- *[Qt][Qt]*
  - default install with web downloader
    - for VS-build: A likely location of the matching Qt-libraries is

          C:\Qt\5.5\msvs2013_64

    - for MinGW-build: A likely path of the matching Qt-libraries is

          C:\Qt\5.5\mingw492_32

- *[libsndfile][libsndfile]*
- *[fftw][fftw]*
- *[readline][readline]* (only works in MinGW build)

*Important note*: creating the folder structure described here will save you
from likely frustration when CMake cannot find dependencies:

- if you build a 32-bit SC, create a folder `x86` as a sibling of your
SC source. For a 64-bit SC call it `x64`
- extract/install/copy the library folders into that folder
- make sure the library's parent folders are called: `libsndfile`, `fftw` and
`readline`. A spelling mistake will let the build fail.

You should end up with this folder structure:

    sc-source
    x86
      libsndfile
          bin
          include
          lib
      readline
          bin
          include
          lib
      fftw


Unfortunately "fftw", as distributed by fftw.org, does not come with import
libraries required for a VS build (fftw3f-3.lib). If you can't get hold of
a prepared import library, you have to create it by using VS tools. This is not
difficult, but the official MS documentation is pretty cryptic:

    https://msdn.microsoft.com/en-us/library/0b9xe492.aspx

More practical explanations are easily found via Google. Don't forget that `lib`
needs the /MACHINE:X64 argument for the 64-bit build, and use the "Developer
Command Prompt for VS2013" to have the tools available in a pre-set environment
for this kind of work. The MinGW-build can use the .dll-files directly.

*Note*: if you compile fftw yourself please make sure that the install is
configured to copy all files to a single flat folder, otherwise cmake will not
find the required files automatically.

In order to get support for ASIO drivers, the [ASIO SDK][asiosdk] has to be downloaded
from Steinberg. The parent folder has to be placed next to the supercollider
source folder:

    sc-source
    x86
    asiosdk
        asio
        common
        ...

DSound support uses MinGW libraries in the MinGW build. For VS you have to
download the DirectX SDK [version 9][dx9sdk] and install it in the system.


### Configuring and executing the build

The quick-steps assume that you are comfortable with the command line. The main
advantage of using the command line is that you can change environment variables
without interfering with overall system environment settings. Any adjustments to
the build environment made below will not change settings in the overall system.
They will only be used by processes triggered from that command line (shell
process), and will be gone after the command line window is closed (ways to
avoid typing when recreating the environment are briefly described in a
walkthrough below).

Once all dependencies are arranged the way expected, we need to run `cmake`
in the build folder to configure the build and generate intermediary build files
for the toolchain and environment used in the build. In principle CMake must be
told:

    - which "generator" to use
    - the location of the compiler
    - the location of Qt (the other libraries will be found automatically
      if located as described above)
    - the location of the SC source.

*Note*: You should always use forward slashes for path statements when using
CMake commands. When creating/modifying environment variables, or interacting
otherwise directly with Windows command line functionality (`cd`, `rmdir`, ...),
you should use the proper Windows syntax with backslashes as directory
delimiters.


#### MinGW

- The generator has to be "MinGW Makefiles" (capitalization matters, the
  quotation marks are required because there is an empty space in the expression)
- If using MinGW, CMake expects to find the compiler location in the environment
  PATH
- Qt is best pointed to by assigning the location of the Qt-libs to the
  environment variable `CMAKE_PREFIX_PATH`
- The SC source is pointed to as last element of the `cmake` command. The path
  can be relative or absolute. The pointer to the source directory is the only
  element of the cmake command that has to be repeated each time cmake is run.
  The other arguments are stored in the file "CMakeCache.txt" in the build
  folder and used by CMake on subsequent runs.

The following steps use a subfolder of the SC source as build folder (called
'build') and assume Qt (including the MinGW branch) installed in the default
location. They should be enough to successfully create the intermediary build
files required to start the build, if the smaller dependencies are in the place
expected:

    $> SET PATH=C:\Qt\5.5\mingw492_32\bin;C:\Qt\Tools\mingw482_32\bin;%PATH%
    $> SET CMAKE_PREFIX_PATH=C:\Qt\5.5\mingw492_32
    $> cd supercollider
    $> mkdir build
    $> cd build
    $> cmake -G "MinGW Makefiles" ..

Using a subfolder of the SC source as build folder has the advantage of being
able to use git to control the SC source from the build folder. Folders starting
with the name 'build' are ignored by git. It should be possible to build SC from
any folder though, provided the source is addressed properly and the
dependencies are arranged as expected.

The SC MinGW-build will be configured as `RelWithDebInfo` if the build
configuration isn't set in the configuration run. If you want a debug-build,
rerun cmake and *add* the assignment `-DCMAKE_BUILD_TYPE=Debug`:

      $> cmake -D CMAKE_BUILD_TYPE=Debug ..

The previous settings for the generator are kept (except the path to the SC
source).

If you want to add `supernova`, add `-D SUPERNOVA=ON`

      $> cmake -D SUPERNOVA=ON ..

Remains the actual `build`. We can also use `cmake`. The following command works
on all platforms and with all generators. It uses the build configuration
set in the previous step:

    $> cmake --build .

Once the build has completed you will have three target folders (four if
supernova is built), in which SC can be run for debugging or testing purposes.
As the Qt libraries are not copied to the target folders, it is *required*
to keep Qt5 in the environment PATH when running the targets. Only the servers
are exempted from that rule, as they don't require Qt. The target folders are:

    sc-ide: build-folder\editors\scide\<build configuration>
    sclang: build-folder\lang\<build configuration>
    scsynth: build-folder\server\scsynth\<build configuration>
    supernova: build-folder\server\supernova\<build configuration>

Other targets (like plugins and external libraries) do not provide executables
and thus cannot be run. If you use a development IDE (Qt Creator), it should
recognize the target executable automatically when you "Run" or "Debug" one of
the targets containing an executable. The target to be run is set in the
Projects/Run-pane accessible from the "Mode selector"-ribbon. You should also
set the working directory there to allow sclang find the server (for more
information on using QtC see the walkthrough below).

If you want to run SuperCollider independently of the build environment (as in:
run it from the file explorer, move it to another computer, pass it on to a
friend), you need to run the install step, or "build the target install". This
is done by appending `--target install` to the build command:

    $> cmake --build . --target install

The install step takes quite some time, as CMake's "bundle utilities" requires
a lot of time to find, and copy all required libraries to the installation
folder. Using MinGW this can take more than 10 minutes.

If you want to create a binary installer, run:

      $> cmake --build . --target installer

For this to work, the install target will have to be built already, and NSIS has
to be installed on your system, with its command line tool `makensis` added to
the PATH.

The SC installer executable will be written to the same parent folder as the
SuperCollider install, for example:

    ...\supercollider\build\x64\Release\SuperCollider-<version-build type>.exe

Note that for just passing SC to a friend it is not really necessary to create
an installer. You can copy the main SuperCollider folder created during install
to another machine. The binary installer just packages this folder and adds a
few installation details like registering the uninstaller in the system and
removing some registry settings during uninstall. The userAppSupportDirectory
will be created from scratch if SuperCollider doesn't find it during start-up.
Whether you copy the SC install folder, or use the full installer, you will
always loose the local customizations and additions stored in the
userAppSupportDirectory. The new Quarks system provides means to make porting
of extension/Quark groups easier.


### VS

*Note*: the preceding MinGW chapter contains a few details not repeated here.
You may want to read it if questions arise.

- add the Qt binaries location at the beginning of the environment PATH (while
  this is not strictly required, it helps to avoid that potential alternative
  Qt-installs on your system interfere during install).

      SET PATH=C:\Qt\5.5\msvc2013_64\bin;%PATH%

  The toolchain is detected automatically, using an environment variable
  set during the install of VS.

- create the environment variable `CMAKE_PREFIX_PATH` and have it point to Qt,
  e.g.:

      $> SET CMAKE_PREFIX_PATH=C:\Qt\5.5\msvc2013_64

- create a build-folder, enter it, and configure the build by running `cmake`.
  The *generator* (`-G`), and the path to the SC-source have to be passed in,
  e.g.:

      $> mkdir sc-build
      $> cd sc-build
      $> cmake -G "Visual Studio 12 2013 Win64" ..\supercollider

  *Note*: CMake chooses the VS toolchain by default on Windows if found, but it
  uses the version compiling for a 32-bit target. Therefore the important
  element in above generator choice is "Win64"

You should end up with a solution file `SuperCollider.sln` that can be opened in
VS to execute the actual build. Starting VS with the solution file from the
command line in which you ran CMake, makes sure that the environment settings
(PATH and Qt-pointer) remain the same:

    $> start SuperCollider.sln

You can also build from the command line. As the VS generator is a multi
configuration generator, the build configuration is determined in this step:

    $> cmake --build . --config Release

If you omit the build configuration, a debug version will be built.

In order to obtain a build that can be run independently of the build
environment, build the "target install":

    $> cmake --build . --config Release --target install

Note that this is required for deployment only. A development cycle would
typically use the version contained in the target folders to avoid the separate
step of *installing* SC. Provided Qt5 is added to the environment PATH, all
targets containing executables (scide, sclang and scsynth) can be run in their
respective target folders (hitting 'run' or 'debug' in the IDE should work).
Within VS you should define the working directory for the respective target in
the user-configuration settings of the target being run (e.g. SuperCollider),
otherwise the server will not be bootable from SC-IDE or sclang (see the
walkthrough on CMake-Gui and VS for more details on using the VS IDE).

- if you want to create a binary installer, build this custom target:

      $> cmake --build . --target installer


### Portaudio

Portaudio is provided as submodule of SC and is built automatically as part
of the SC-build. In order to add ASIO- and/or DSound support, the respective
SDKs have to be added to the build.

ASIO: download the SDK from [Steinberg][asiosdk], extract the zip and place
the folder in a sibling folder of the supercollider source (not the portaudio
folder in external_libraries). The folder name should not contain dots, start
with `as`, and be the immediate parent of the library folders (asio,
common, ...). You should end up with a folder tree like this:

    sc-source
    asiosdk
        asio
        common
        driver
        host

DSound/DirectX: The VS-build requires DirectX. It should be installed as usual.
[version 9 (June 2010)][dx9sdk].

You can study the file `CMakeLists.txt` in the portaudio folder
(external_libraries\portaudio_sc_org) to learn about the options that the build
provides. With default settings, all APIs that work out of the box are enabled
and only the library for static linking is built. In the VS-build all APIs are
enabled, in the MinGW build WASAPI had to be omitted, and some features of full
duplex mode in DSound had to be disabled. If you would like to tweak the PA-
build you can single it out from the SC build like so:

    cmake --build . --target portaudio


Walkthroughs
------------

### The trial-error approach: building sc3-plugins and passing build settings to `cmake`

sc3-plugins is a popular collection of binary extension to SC, providing many
additional UGens. They are hosted in the SuperCollider repository on Github:

    git clone --recursive https://github.com/supercollider/sc3-plugins.git

Like the core SuperCollider repo, this source collection contains sub-modules
that may need to be re-synchronized when traversing the source history. The
command is:

    submodule update

Initialization and population of the submodules was automatically done because
auf the flag `--recursive` at the initial clone. If at any point this is
required again (message "submodule missing"), the command is:

    submodule init

Git will usually tell, when a resynchronization is required. If cmake reports
missing files during the build, it might be related to un-initialized and/or
un-populated submodules.

This walkthrough expects that you have a working SC build environment in place.
sc3-plugins do not have additional dependencies, so you are set. Furthermore Qt
is not required, which makes things a bit easier. On the other hand the CMake
find-modules provided with sc3-plugins often fail, so that more manual
adjustment is likely to be required.

In order to find out which CMake variables need values, we can do a trial run.
For this to be meaningful we need to

  - add the toolchain binary path to the environment path before running `cmake`
  - specify the generator (element after flag `-G`)
  - point `cmake` to the source (last element in `cmake` command, cannot be
    omitted on subsequent runs). For MinGW do:

        $> SET PATH=C:\Qt\Tools\mingw482_32\bin;%PATH%
        $> mkdir sc3-plugins-build
        $> cd sc3-plugins-build
        $> cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release ..\sc3-plugins

    For VS Studio the toolchain need not be added to the path. You just need
    to specify the generator like so:

    $> cmake -G "Visual Studio 12 2013 Win64" ..\sc3-plugins

CMake will likely return two or three errors:

    FFTW3F_INCLUDE_DIR=...NOT_FOUND
    FFTW3F_LIBRARY=...NOT_FOUND
    SC_PATH=...NOT_FOUND

We know the SC path, and we also know the location of the 32-bit fftw library,
which was used in the SC build. The expected library is likely dynamic, so we
need to specify the .dll (no .dll.a available) file in the `bin` folder. Let's
do another trial run with the added information:

    $> cmake -DFFTW3F_INCLUDE_DIR=..\x86\fftw -DFFTW3F_LIBRARY=..\x86\fftw\libfftw3f-3.dll -DSC_PATH=..\supercollider ..\sc3-plugins

*Note*: there should not be any spaces after the `=` in variable assignments.
While cmake is lenient with spaces after the flags (`-D`, `-G` etc.), a
space after `=` is interpreted as a delimiter indicating the next element in
the command. The variable gets assigned "nothing" and the following path may
be wrongly interpreted as the path to the source. Another rule concerning spaces:
if a *path* contains a space, it must be enclosed in quotation marks.

We shouldn't get any more errors, time to build!

Note that CMake stores the variables you set manually in a file called
`CMakeCache.txt`, located in the build folder. It is not required to re-enter
values that have been stored there (except you want to overwrite them with
different values, of course). It is possible to change configuration values by
editing this file. If you get stuck and decide to configure the build from
scratch, you can delete that file and restart. This way you can keep the files
that have already been built, while making sure that all configuration values
you add are generated from anew. This can save a lot of compile time. At times
though, you might experience that it is necessary to remove everything in the
build folder.

There could also be variables that define build options without producing errors
if no value is set. They can be listed by adding the flag `-L` to the configure
run:

    $> cmake .. -L

This will show us amongst other things, that an option to build plugin versions
for the alternative audio server `supernova` is available (not availale in
VS builds). To add them to the build, just run:

    $> cmake -DSUPERNOVA=ON ..

The old settings will stay in place.

Ready to build:

    $> cmake --build . --target install

This works for both MinGW and VS. In the VS build you may want to specify
the build type:

    $> cmake --build . --target install --config Release


Unfortunately on Windows we will get an error at the end of this build, because
CMake wants to install the files to the Windows default install location, which
is the folder "Program Files (x86)". This path requires elevated access rights
and contains an empty space, the install fails. Further more, installing the
plugins to the application folder doesn't make much sense, we want to have them
around in a folder we can easily add to the folders scanned by `scsynth` and/or
`supernova`.

In order to correct this error, we need to rerun the configuration and set an
install folder in user space that does not contain a space. The variable for
this purpose is CMAKE_INSTALL_PREFIX:

    $> cmake -DCMAKE_INSTALL_PREFIX=./Install ..

    $> cmake --build . --target install

This will copy all files to the subfolder `Install` in the build folder. The
second build should proceed very quickly as CMake only builds changed components
from scratch.

If you execute a build command, CMake compares the time stamps of source files
with the time stamp of the last build, to determine if anything has changed. So
if you don't need to verify that a reconfiguration was successful, you can just
execute the build command without running CMake manually. CMake will be triggered
automatically if required (according to what CMake believes is correct in
a certain situation - which isn't necessarily correct ;) ).

On the command line two things are important time savers: when entering paths,
tab completion works, which helps avoiding typos. Also the history function
(arrow up) is very useful. CMake will not always detect typos in path
assignments. If a wrong path is entered, this will only become apparent in the
build ("header not found" etc.). CMake-Gui makes setting variables a lot
more comfortable, but you have to make sure to set the system- or shell-
environment values mentioned above before running `cmake-gui`. Important to
understand here is that the environment variables have to be set in a place,
where CMake-Gui *inherits* them. If you set them at the command line (in a
"shell") you need to start CMake-Gui from that shell (by entering `cmake-gui`).
If you start CMake-Gui from the Windows Start Menu, the environment variables
must be set on system level. The same is obviously true for a development IDE.


### Avoiding the command line: from CMake-Gui to Visual Studio (environment and GUI-applications)

If you think you prefer to avoid the command line, there is the tempting option
of running CMake-Gui from the Windows Start Menu. While this is possible, there
is an important caveat: when run from the Windows Start Menu, CMake-Gui runs in
the environment defined by the overall system set during login. This means you
cannot set the path on the command line (in a *shell*), but have to make the
path adjustments on system level (using Control Panel/System/Advanced System
Settings/Environment Variables). Setting the PATH on system level brings along
some risks that need to be managed carefully, most importantly: adding Qt5 (or a
toolchain) at the head of the system PATH can cause other applications that may
depend on similar path settings, to malfunction. This can be easily corrected by
removing the offending entry when done with SC development (and avoiding the
conflicting application in the meantime), but this might not always be possible.
So we need to keep in mind: While working on a SC build with changed *system
environment* settings, other applications might get into trouble. Most common
cases are applications that depend on some implicit Qt install added to the
PATH, or MinGW applications that require MinGW runtime libraries that don't fit
the ones used in our build. It is not best practice to rely on the path to find
libraries, but it is quite common, especially among free applications stemming
from the unix world, where people are used to a homogenous system, in which all
applications use the same shared libraries. To avoid potential trouble, you
might want to make your environment adjustments on the command-line *and* start
`cmake-gui` form that same shell.

Once CMake-Gui is running it will need to know (just like on the command line):

1. the Qt location
  - While Qt can be added via the PATH as well, it is safer to define an
    environment variable "CMAKE_PREFIX_PATH", that contains the path
    to the parent folder of the "Qt-system" we are using. The Qt-architecture
    must fit the build system, in this case Microsoft Visual Studio 2013, 64-bit.
    In a standard install, the path is likely to be:

        C:\Qt\5.5\msvc2013_64

    You can modify the Windows environment variables by hitting Win-key-x,
    choosing `System` -> `Advanced System Settings` -> `Environment Variables`.
    Apply the `SET CMAKE_PREFIX_PATH=C:\Qt\5.5\msvc2013_64` logic to this
    dialog. Note that there is no `bin` in the CMAKE_PREFIX_PATH, this is *not*
    a binaries search path, but an anchor path for CMake to work out all
    relevant Qt locations.
    After the addition it might be necessary to re-login. You should experiment
    as to whether this is necessary on your system, with your Windows version.
    Applications behave different as to whether they instantly react to system
    environment changes or stick to the environment set up at login or system
    boot. So far it looks as if cmake-gui, Visual Studio and also the file-
    explorer are among the applications that sense environment changes
    instantly, but your experience may differ.

    Only start CMake-Gui *after* this adjustment has been made.

2. the path to the build folder and the SC source
  - they can be comfortably selected in the two text boxes at the top of
    the window.

3. the generator to use
  - The generator can be selected in a dialog that opens, when you hit the
    'Configure' button. Make sure to pick the 64-bit toolchain.

4. the path to the toolchain
  - In the case of Visual Studio CMake can reliably deduct the toolchain-path
    from an environment variable set by Visual Studio. This could only be an issue
    if you have several versions of Visual Studio installed on your system.

After the required assignments have been made, CMake-Gui will attempt to
configure the build. If this attempt returns errors, you will have provide
additional values. After the configuration CMake-Gui will present you with a
long list of cmake variables alongside with the values assigned to them as
stored in the cache. In the box at the bottom you can scroll through the output
CMake produced during configuration and analyse it. At this stage it is good to
scroll through the central pane and check for the paths related to the "smaller"
libraries (libsndfile, fftw (and readline, you want to work on that)) and the
Qt5 related paths. Do they fit the locations we expect? If not, you may try to
correct it in the window presented. On the other hand, errors in this stage are
more likely to be caused by conflicts with your system settings, typos in paths
or the like. In that case it is better to fix the error at its root, as such
mistakes are likely to cause build errors later on. Common causes are
conflicting Qt and MinGW installs, or alternative versions of the smaller
dependencies that were detected by CMake. In that case you will have to work
out, how to correct things while both keeping your overall system functional,
and allowing for a successful SC build.

If everything looks okay, you need to hit "generate" to create the files used to
build by VS. CMake-Gui introduces a distinction between 'configure' and
'generate'. If we use trial-error mode, this is quite useful, as generating
VS project files can take quite some time.

*Note*: not every "NOT-FOUND" entry necessarily indicates an error. At least in
theory only NOT-FOUNDs *creating a CMake error* should be problem.

*Note*: If you come back after changing your library locations, the safest
thing is to delete the build settings cache (`CMakeCach.txt` in the build
folder) and rerun the configuration from scratch. Otherwise old settings might
stick. You can delete the file yourself or use the more radical "delete cache"
in CMake-gui's file menu, which will delete the entire build folder. You may
also want to restart CMake-Gui, as it keeps manually entered settings for a
subsequent run regardless of whether the cache has been deleted or not. That can
be helpful at times, and confusing in other situations.

*Note*: resist the temptation to enable all options that seem promising. Some of
them don't work in the Windows build. For example the supernova server can't be
built with VS yet. Usually SuperCollider's default values are a considered
choice that should only be changed for a good reason (see the walkthrough for
"More CMake" for some options that do work).

CMake will generate a solution file in the build folder. Use it to start VS, and
you are ready to develop and build in this feature rich IDE. If you started
CMake-Gui from a command line/shell, should start Visual Studio from that shell
as well, to run Visual Studio in the same environment as CMake-Gui:

    $> start SuperCollider.sln

If you set the environment on Windows system level, you can start Visual Studio
in your preferred way. Once the "solution file" is open in Visual Studio, you
are ready to build. You can either chose "Build->Build solution" or right-click
the *target* "ALL_BUILD", or "Supercollider" and select "BUILD". Once the build is
completed you should be able to run any of the targets that contain executables:

    SuperCollider
    sclang
    scsynth

The targets can be selected most comfortably in the "Solution Explorer".
Right-click the wished target and select "Debug". The application should come up
after a while. If you find that VS has problems finding the target-executable,
you will have to adjust some configuration settings. Right-click the target and
chose 'Configure'. You will find an option to select the target executable path.
Note that these are VS user settings not passed on to CMake. While in theory it
would be possible to have CMake generate VS user settings, it is hard to keep
CMake generated VS user-settings in sync with changes made from within Visual
Studio. Therefore you should not expect CMake to know anything about the changes
you made within Visual Studio.

In order to boot the server from scide or sclang, it is necessary to set the
working directory for the respective target. This can be done in the "Debug"
pane of the target properties dialog.

*Important Note*: provided the build environment is set up properly (the main
setting is the requirement to have the Qt5/bin folder in the PATH of the current
environment), the various SC components can be run directly in the target
folders. If you want to run SC outside of the build environment, or copy the
build to a different computer, you need to build the target "install". This step
will create a autonomous install of SuperCollider, without any outside
dependencies except files guaranteed to be available on every Windows system.
Building the install target takes some time, as all SC files are copied to a
separate install folder, and a CMake utility, "bundle utilities", tries to
detect all libraries that the SC executables require to run. The bundle utility
copies those to the install folder, as that is the only way to guarantee that
they will be available wherever SC is run. Copying the libraries to the SC
install folder also guarantees that exactly these libraries are used by SC, and
not some other version or architecture that may be "findable" via the current
system/environment search PATH.


### Using Qt Creator

QtC is likely the IDE with the best CMake-, as well as Qt integration, therefore
it fits our requirements very well. The base steps, namely pointing to the
toolchain and Qt are handled as part of defining "kits", and controlled via
comfortable selection dialogs. You can open a CMake project directly, and run
`cmake` from within the IDE, quite similar to how you would work with CMake-Gui.
QtC automatically creates environment variables adjusted to the kit used (i.e.
Qt5 and the toolchain are found and added to the PATH as required), without
interfering with the overall system. You can "run" the SC targets containing
executables directly from within QtC. In order to start the server from the
SC-IDE or sclang, you will have to set the "working directory" to the target
folder in the Projects/Run pane.

QtC can handle a CMake project once it knows the location of the
`CMakeLists.txt` file in the source root folder. It does not require a dedicated
generator, but uses the "MinGW Makefiles" one automatically. It can configure
the build using CMake in a similar way as CMake-Gui does. You can also do trial
CMake runs, and add variable assignments in a text input box (preceded by `-D`).
While not presenting the build options in a neat list, it allows you to edit the
CMakeLists.txt files themselves in a meaningful context.

In a kit, among other things (debugger, source control system and more), you can
select one (among potentially several) Qts on your system, and chose a toolchain
to combine with. If your Qt is installed in a standard location, it is likely
that QtC already created a kit at start-up that fits the needs. While QtC can be
used to build with the MS toolchain, it is the more likely choice when building
with MinGW. So before opening the SC project, we may need to create the right kit:
Qt5.5.1-mingw492_32 combined with mingw482_32. This can be done in Tools ->
Options -> Kits and should be self-explanatory.

Next step is to open the project (File -> Open File or Project) by pointing to
the root CMake script (CMakeLists.txt) in the top parent folder of the SC
source. Then QtC will prompt you for a build folder and run `cmake`. This dialog
is well adapted to the trial-error approach of aggregating the required variable
assignments for the build (see "CMake-Gui to VS", or "Building sc3-plugins").
Once CMake returns no errors you should be ready to build.

For managing your build you will typically visit the "Project Pane". It offers
two sub-panes where you can make choices relating to build definition and install/
deployment. It is the place to add predefined alternative kits with a dedicated
build folder (for alternative builds), modify the environment in which the build
is executed, or add more build steps or targets (e.g. install, installer).
You can also chose individual targets to be built here. Each build step can be
conveniently clicked on or off. You may also assign additional build steps to
the second project pane "run", where install related steps are best placed. This
is also the place to select the target that is started when SC is "Run" or
"Debug"ged from the mode selector ribbon. Note though that the steps defined there
are executed each time you hit "Run" or "Debug", so it is not always helpful to
have these steps active during a development cycle.

In order to create a binary installer, you will want to add the (custom) target
`installer`. It creates a binary containing all files required to run SC autonomously
(NSIS required). But if you are not working on the installer itself, you will likely
only want to activate that step at the end of a development cycle.


### More `cmake`: building supernova, qt-less, verbosity and more

Learn more about the CMake options by running `cmake --help`. A good window into
the SC build configuration is the file CMakeCache.txt in the build folder. The
most comprehensive information is of course contained in the CMakeLists.txt
files scattered over the project source tree, starting from the source root
folder. A longer list of variables defined for the SC build is retrieved by
adding the flags -L or -LA ('list advanced') to the command `cmake ..`.

The file CMakeCache.txt can be manually edited and is the best place to verify
that variables have been assigned the values expected. Keep in mind that the
`clean` step does not delete or change this file. This simplifies configuration,
but may also cause trouble if wrong values are kept. To make sure CMake
generated values are updated correctly, remove the respective variables from
the file or delete the entire file. To make sure object files are recompiled,
"build" the `target clean`. To avoid any contamination by an old build, delete
the entire build folder. Of course each of these measures significantly extends
the time needed to complete a build.

If you get mysterious build failures (i.e. no CMake error message that helps),
sometimes increasing cmake output verbosity during the build gives you
information that helps working out what went wrong:

    cmake --build . -- VERBOSE=1

You can pass other make/mingw32-make/msbuild arguments after "--". To speed up
your build "-j4" with MinGW ("-m4" with VS) might work:

    cmake --build . -- -j4

Commonly used variables to modify the build configuration are:

* Location where SC gets "installed", which in this context means: where the
  SC files - including required external libraries - get copied to if you
  build the target "install". The default location is set to './Install'.
  If you maintain multiple builds you could either use different build trees
  or install to different folders. Change this for example like so:

      -D CMAKE_INSTALL_PREFIX=./x86/AllPullRequests

  This installs the files to a subfolder x86/AllPullRequests of the build-
  directory. Note that CMake uses forward slash as folder separator on all
  platforms. You may also specify an absolute path here. Building the install
  target is a precondition to creating a binary installer. Apart from copying
  all SC files to an install folder, it executes a command that pulls in all
  required libraries to run SC without depending on shared libraries other
  than the system libraries guaranteed to be available on every Windows system.
  The binary installer adds some comfort, enhances SC system integration and
  cleanup on uninstall, but it is not required to run SC autonomously.

* Enable compiler optimizations for your local system (could cause a faster
  binary, but could also cause build problems. Don't use this if building for
  computers with different CPUs than your own)

      -D NATIVE=ON                             # gcc/mingw only

* Build the *supernova* server:

      -D SUPERNOVA=ON

  *Note*: When you build with supernova, an alternative server executable and
  a supernova version of each plugin is built. If you also use the 'sc3-plugins'
  package, make sure to compile them with supernova support too. At the time
  of this writing supernova could only be built with the MinGW toolchain.

  Within SC you will be able to switch between scsynth and supernova by
  evaluating one of lines below and then booting the server as usual:

      Server.supernova
      Server.scsynth

  Check SC help for `ParGroup` to see how to make use of multi-core hardware.

  *Note*: If you experience problems getting the supernova plugins recognized
  (plugins related error at supernova boot), try placing them in a folder called
  "plugins" in the "userAppSupportDirectory\Extensions" folder. It may also be
  required to separate binary plugins and classes. The class files may reside
  anywhere in the Extensions folder (or in any "included" class folder), whereas
  the binary plugins should reside in a folder "plugins" (this mirrors the situation
  in the application directory)

  *Note*: While there is no readily available 64-bit Qt version for the MinGW
  build, supernova does not depend on Qt. Therefore a qt-less build could be
  used to build a 64-bit version of SuperNova, and combine it with the 64-bit
  VS build of the IDE and sclang (using MinGW runtimes for supernova). The
  64-bit build of supernova currently breaks, but might be more easy to fix
  than the VS build.

* It is possible to build SuperCollider without the IDE, and even without Qt
  (and implicitly without the IDE). This is not controlled via build targets,
  but via CMake variables:

      -DSC_IDE=OFF

  This will not build the IDE, but keep Qt in sclang. With the more radical
  setting:

      -DSC_QT=OFF

  SC will not link to Qt at all, and implicitly build without the IDE. This
  build variation is used in low resources environments and graphic-less use
  scenarios of SC. In the Qt-less build some Qt/GUI related classes are
  removed from the SCClassLibrary to avoid primitive not found errors at
  sclang start-up. The removed classes can be found in the folder
  SCClassLibrary/scide_scqt. If you need to run code with elements considered
  deprecated in SC3.7, you can move the deprecated/3.7 folder back into place.
  You should remove the unsupported Document-class related entries to avoid
  (non-fatal) errors at sclang start-up.

  You cannot use the Help browser in Qt-less SC. An option is, to render Help
  with sclang (`SCDoc.renderAll`) and run it in a browser. There is also the
  option to build, or pre-render the entire documentation during build time to a
  subfolder "RenderedHelp" in the build folder. This is enabled by setting
  "SC_DOC_RENDER" to on:

      -DSC_DOC_RENDER=ON

  You can also avoid *installing* the (unrendered) HelpSource:

      -DINSTALL_HELP=OFF

* Server-only: Currently there is no straightforward way to build server-only.
  Even if you mark only the server target (and the plugins), sclang is pulled in
  as well. So a bit of patience is required. You can turn off Qt (`-DSC_QT=OFF`)
  and skip Readline, to save on the dependencies. To make it easier to assemble a
  server bundle, the custom target "install_server_only" is provided. It can be
  used in place of the comprehensive "install". The custom target will copy
  scsynth's target folder and run fixup_bundle on scsynth to pull in required
  libraries.

  *Note VS*: As this is not a proper install, the MS runtime libraries
  (msvcp120.dll and msvcr120.dll) are not copied. If required, get
  them from a full SC install, or from the VS/VC/redist folder.

* SClang-no-Qt: the modifications to the SCClassLibrary are only done
  automatically in the build to the target folder. If you run the target
  "install", you get the unmodified class library. The changes are just:

    - move or delete Common/GUI and JITLib/GUI

More options can be explored by studying CMakeCache.txt in the build folder,
or file CMakeLists.txt in the root folder of the SC source.


### Recalling environment- and build settings on the command line

While using the command line for the build provides a safe, low system impact
way of doing your thing, it does require a lot of typing, especially if you need
to do this each time you re-enter a build shell (i.e. open a command line window ;) )

The solution is: store the commands required to make the environment settings in
a batch-file (.bat or .cmd) and never type those settings again. Simple Windows
batch-files are basically series of commands in their own line each. You can run
a batch-file automatically upon opening a command line window, by passing the
path to the batch file to the command that opens the command window (after the
flag /k). This can further be automated by storing the start-cmd in a link
opening a command line window. Just copy the link called "Command prompt" and
access the properties for that link. The "Target" input-box can take the command
that opens the command line window *and* runs your batch file, e.g.:

    %COMSPEC% /A /Q /K %USERPROFILE%\qtenvQt5.5mingw492_32.bat

If the batch file contains these lines:

    echo off
    echo Setting up environment for Qt usage...
    echo Qt 5.5 mingw492_32 paired with MinGW 4.8.2
    set PATH=C:\Qt\5.5\mingw492_32\bin;C:\Qt\Tools\mingw482_32\bin;%PATH%
    set CMAKE_PREFIX_PATH=C:\Qt\5.5\mingw492_32

You are set to start the environment for the MinGW build system with a
single click. But there is more, e.g. DOSKEY, the Windows alternative to
Unix' `alias`, e.g.:

    DOSKEY cmDb=cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Debug %USERPROFILE%\Projects\sc\supercollider
    DOSKEY cmRl=cmake -G "MinGW Makefiles" -D CMAKE_BUILD_TYPE=Release %USERPROFILE%\Projects\sc\supercollider
    DOSKEY mk=cmake --build .
    DOSKEY mki=cmake --build . --target install
    DOSKEY cds=cd %USERPROFILE%\Projects\sc\supercollider
    DOSKEY cdbdb=cd %USERPROFILE%\Projects\sc\supercollider\build_MW_Db
    DOSKEY cdbrl=cd %USERPROFILE%\Projects\sc\supercollider\build_MW_Rl
    DOSKEY cdd=cd %USERPROFILE%\Projects\sc\deployment\mingw492_32

These lines create a VS environment:

    echo off
    echo Setting up environment for Qt usage...
    set PATH=C:\Qt\5.5\msvc2013_64\bin;%PATH%
    set CMAKE_PREFIX_PATH=C:\Qt\5.5\msvc2013_64
    echo Running vcvarsall.bat to complete environment setup!
    call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
    echo Done!
    cd /D %USERPROFILE%\Projects\sc\supercollider
    DOSKEY cm=cmake -G "Visual Studio 12 2013 Win64" %USERPROFILE%\Projects\sc\supercollider
    DOSKEY mkDb=cmake --build . --config Debug
    DOSKEY mkRl=cmake --build . --config Release
    DOSKEY mkDbI=cmake --build . --config Debug --target Install
    DOSKEY mkRlI=cmake --build . --config Release --target Install
    DOSKEY cds=cd %USERPROFILE%\Projects\sc\supercollider
    DOSKEY cdb=cd %USERPROFILE%\Projects\sc\supercollider\build_VS
    DOSKEY cdd=cd %USERPROFILE%\Projects\sc\deployment\msvc2013_64

Another option are batch-files that construct CMake arguments and execute the
build. If for some reason you need to enter many of those and/or can't keep the
build folder with the cache file, this can come in handy. Here is a example that
allows to edit all paths to dependencies. They go into a single environment
variable (`CMAKE_PREFIX_PATH`) that CMake will read. The lines here contain
random assignments to demonstrate some possibilities. Note that the batch files
above use Windows path syntax, while the one below uses forward slash for
CMake:

    set CMAKE_GENERATOR="Visual Studio 12 2013 Win64"
    set QT_HOME=C:/Qt/5.5
    set QT_FLAVOUR=msvc2013_64
    set QT_PREFIX_PATH=%QT_HOME%/%QT_FLAVOUR%
    set SNDFILE_PREFIX_PATH="C:/Program Files/nerd/libsndfile"
    set FFTW_PREFIX_PATH=%USERPROFILE%/fftw
    set READLINE_PREFIX_PATH=D:/scdeps/x64/readline
    set CMAKE_PREFIX_PATH=%QT_PREFIX_PATH%;%SNDFILE_PREFIX_PATH%;%FFTW_PREFIX_PATH%;%READLINE_PREFIX_PATH%
    cmake --build .
    start Supercollider.sln

Another way of storing CMake command line arguments is creating a "toolchain"
file. This is the CMake suggested method. Please look up the CMake documentation
if you require an advanced configuration, and are interested in this approach.


### On the horizon: MSYS2

There is a new star on the horizon of building unixy software in Windows, a
relatively recent msys/cygwin-offspring called [msys2][msys2]. Git has already
recognized it's advantages and uses it as base of Git for Windows. Apart from
being a rapidly progressing distribution using the rolling update model, and
providing many more packages than just build tools (among them everything
required for SC, including the tools), msys2 features a package manager just
as comfortable as in big Linux distributions, a clone of Arch Linux' `pacman`.
Starting from a binary installer, it is pretty simple to assemble a complete
build environment for SC, including Qt5, *both* in 32- and 64-bit versions. Once
the environment has been assembled, SC can be built with the same simplicity as
on Linux, all that needs to be added is the selection of the generator "MSYS
Makefiles":

    $> cmake -G"MSYS Makefiles" ..

It is also possible (though not officially supported) to use msys2 as toolchain
and library repository only, and build from a normal Windows shell, or Qt
Creator outside of msys2. This minimizes the necessity to deal with unix syntax
and commands. All that needs to be done, is set the environment PATH to point to
the right toolchain, and tell CMake about msys2's sysroot via CMAKE_PREFIX_PATH.
In this case the generator has to be "MinGW Makefiles". For example:

    $> SET PATH=C:\msys2\mingw64\bin;%PATH%
    $> cmake -G "MinGW Makefiles" -D CMAKE_PREFIX_PATH=C:\msys2\mingw64 ..

Ready to build!

Unfortunately the resulting SC binaries don't work for now (segfault in the
IDE). This is likely due to a bug in 32-bit MSYS2. Once this changes, and once
the SC codebase is updated to support a MinGW 64-bit build, big times lay ahead
for the SCWin build. 64-bit IDE, sclang, scsynth, supernova, base- &
sc3-plugins are in close reach, and shouldn't be too difficult to maintain
alongside mainstream SC development on the unixy platforms.


Diagnosing build problems
-------------------------

These are likely the most common causes of build problems:

- after run- or debug-sessions sometimes scsynth or sclang "zombies" stay
  in memory. That will create file-access errors during build. End the process
  the in the task manager
- too old or new compiler/toolchain: tested are MinGW 4.9.2 32-bit and
  VS 12/2013 64-bit
- incorrect versions of the dependencies (readline 5.0.1, libsndfile 1.0.25/26,
  fftw 3.3.4, Qt 5.5.1)
- too old cmake version (3.4.2 or bigger are required)
- dynamic/runtime-library mismatch. This can happen if dependencies and
  core SC require different versions of the same runtime library. Reach out
  for libraries that do not depend on MinGW runtimes or make sure all components
  in the build can use the same runtimes.
- SC- and Qt-architecture mismatch. This does not only concern target system
  architecture (32- or 64-bit) but also the toolchain used to compile SC and Qt
  (sometimes called flavour in this text). They have to match as closely as
  possible (anything but an exact match is likely to cause trouble). SC built
  with VS requires the msvc2013_64 flavour, the MinGW 4.8.2 build requires
  the mingw492_32 flavour
- dirty states in your build folder (usually resulting from changes in the build
  configuration). See below for various "fixes"
- cmake finds and uses different libraries on your system than the ones intended
  for the build. See below for ways to identify these problems.

When asking for build help, please mention the installed versions of the
components you used to build SC!


### Dirty build states

While you are developing SC code, it's generally safe to re-use your build
folder, the build will complete *much* faster. If you change branches, build
tools, cmake settings, or the versions of your dependencies, things are less
safe, the build configuration might have gone out of sync with the files used in
the build, without CMake being aware of it. The way out of this is to clean your
build folder. These are common ways to accomplish this, in order of severity:

1. delete individual entries from CMakeCache.txt to make sure they are rewritten
2. `del CMakeCache.txt` (delete all stored configuration settings for that build)
3. `cmake --build . --config Debug* --target clean` remove the compiled object
   files from the build (forces recompile)
4. Delete everything in the build folder. Use the file Explorer or run
   `rmdir . /s /q`

Generally, clearing the CMakeCache.txt and rerunning `cmake` should be enough to
fix many build problems. Switching git branches that only contain changes to
code in SC proper should usually not require this kind of repair.


### Wrong libraries found

While some precautions have been taken to prevent wrong finds, it is hard to
completely exclude the possibility (or comes at the price of lesser
flexibility). Wrong or unplanned finds carry big risks: the libraries could be
of an unsupported version, wrong architecture, or missing the right header
files. If you overlook a wrong find, and the build succeeds, you will likely
get an error message at SC start-up. This is actually a good thing, because it
allows to identify the actual problem. Far worse are generic error messages or
mysterious crashes somewhere along the way because of a misfit.

The two most likely types of wrong finds occur either at the very beginning of
the build, when CMake find-modules try to identify the libraries required for
the build on your system, or at the very end (during "install"), when a CMake
utility (bundle utilities) tries to find the libraries required to allow your
build to work outside of the build environment (*standalone* in the meaning of
*build environment independent* or *autonomous*).

There are two relatively easy ways to identify wrong finds at the beginning:

- use `cmake-gui` and look up the paths to all libraries in the list of build
  options presented in the central pane (if you are in doubt about the
  significance of envirnment settings when using CMake-Gui, please refer to
  the walkthrough 'Avoiding the command line: From CMake-Gui to Visual Studio'.
  The aspects relating to environment settings are relevant for other use cases
  as well)
- study the file CMakeCache.txt in the root build folder. The quickest way to
  find the relevant entries is by searching for the library names (readline,
  sndfile, fftw and Qt5). Look for paths pointing to headers (include_dir),
  (import-) libraries, dll's and dll-directories.

You can change build settings both in CMake-Gui and by editing CMakeCache.txt.
CMakeCache.txt - the cmake configuration "cache" - also contains hints as to the
function of the variables. You can directly edit this file, and new values will
be used on reconfiguration of the build. This is equivalent to passing in
variable assignments with the flag `-D` on the command line. These values will
be stored in CMakeCache.txt and replace values assigned by CMake's search
routines. It must be said though, that you are likely to be better of, if you
identify the cause of the wrong find and fix it, rather than manually edit a
configuration generated automatically.

*Note*: if you reconfigure the build by running `cmake` after system changes
(like deleting unwanted libraries or changing the path order), CMake will
usually *not* overwrite values that have already been stored in the cache file.
The cache serves to store values expected to be readable globally during the
build, *and* values that have been passed into the configuration via the flag
`-D`. During the build cache values are prioritized over values any cmake
routine might produce. This priority order helps to protect manual edits of the
build configuration, which can be done with `-D`, in CMake-Gui or by editing
CMakeCache.txt. So keep in mind that implementing a different logic to create a
value for a cmake variable (or e.g. moving a library to a place where it will be
found by cmake) will not necessarily become effective in the next configure run.
If the cache already contains a value assigned to that variable it will continue
to use that value. A safe fix for this kind of problems is to delete the entire
cache file, but it is usually sufficient to delete the variable(s) that should be
reassigned values.

If you get wrong finds because of a conflicting system configuration (Qt4,
different toolchains, individual libraries in high priority system locations
(e.g. Windows\system32 or Windows\sysWow64) or in early PATH positions.
Sometimes adding the parent folder of the intended library to the front of the
environment path can help. If nothing works, temporarily renaming the offending
libraries can take them out of the game. In principle the build system tries to
anticipate these problems and prevent them. So if you bump into a new case,
please report this on one of the SC mailing lists, or add an issue in SC's
issue-tracker on Github.

The second candidate for wrong finds is the fixup_bundle utility at the very end
of the install process. Remember, for developing SC the "install" step (building
the target "install") is not required, SC can be run in the respective target
folders - using and depending on the build environment settings. The development
IDEs should be able to find the correct executable if you hit "run target". But
if you want to start SC from the file explorer, or use a copy on another
computer, you will need to "install" and then run from/copy the generated
installation folder.

On Windows the bundle utility just reads out the dynamic libraries required to
run an executable, tries to find them in the system, and copies them to the
folder in which the executable resides. This is the safest way to guarantee that
SC can be used independently of the build environment, because Windows
executables always look for libraries in their own working directory *first*.
Fixup_bundle just identifies libraries by their names (version and architecture
are not verified). When it tries to find the libraries in the system, the
environment PATH is taken into consideration with relatively high priority.
Therefore it is theoretically possible that fixup_bundle finds different
libraries from the ones used at build time. As a result your build will work,
but the install won't. Even worse: the error messages produced in this case
are far less specific than the ones produced if a library is simply missing.

Fortunately it is not too difficult to identify wrong finds of fixup_bundle,
because it produces output that tells exactly, which libraries were copied to
the executable folder. Further more it is the last install step, you just need
to scroll up the cmake build output. So if you get obscure Windows system
messages at SC start up, try to scroll up the install output, and go over the
last segment, starting with `fixup_qt5_executable` and/or `fixup_bundle`. If you
lost the output, just repeat `cmake --build . --target install`. Going over that
list you might be able to spot cases where required libraries were copied from
unintended places. A quick workaround to get SC running in such cases is to
replace the wrong library by the correct one. A better workaround is to adjust
your build environment, so that you don't have to repeat the replacement after
each install. Adding the overlooked library's parent folder to the head of the
environment PATH should usually do the job. Again, the build system was
constructed to anticipate such errors and prevent them. All directories
containing libraries are passed to fixup_bundle and should be treated with
higher priority than the environment path. So if you bump into a case, please
report it to the SC community. Or even better: create a pull request on Github
that enhances the build system.

Of course there could also be bugs in the SC source. A source for the
current build status of SC is the travis-ci status page:

    https://travis-ci.org/supercollider/supercollider

Unfortunately though there is no Continuous Integration system in place for
Windows yet. Therefore you are strongly encouraged to report Windows build
issues in one of the mailing lists, or the SC issue tracker on Github.
Reporting Windows build issues is currently the only way to detect errors
for SCWin resulting from progress in mainstream SC development.


Description of the SC 3.8 release build
---------------------------------------

All dependencies not contained in the SC source (external_libraries) were
downloaded from the original providers (see links at the end). For both VS and
QT the free community editions were used.

SCWin64 3.8 was built with Visual Studio 12/2013

- Qt5.5.1 (flavour msvc2013_64)
- libsndfile 1.0.26
- FFTW 3.3.4

SCWin32 3.8 was built using Qt Creator, combining MinGW 4.8.2 and Qt 5.5.1
mingw492_32 into a kit. The MinGW distribution provided by Qt was used.

- linsndfile 1.0.26
- FFTW 3.3.4
- Readline 5.0.1 (as provided by gnuwin32)

For both builds all other external libraries (including portaudio) were compiled
as part of the SC build, using the sources embedded in the SC source tree (in
the folder external libraries) and the settings defined there.

The portaudio build was extended for ASIO support by adding the ASIO-SDK:

- ASIO SDK 2.3

For DSound support different DirectX versions had to be used in the VS and MinGW
build. For VS:

- DirectX v9 (June 2010)

The MinGW build uses the libraries coming with MinGW.

The tools used were Git for Windows v2.8.2.windows.1, cmake v3.52, and NSIS v3.0b1
to create the binary installer.


Known problems
==============

- depending on the release used, communication between the IDE and sclang is
  seriously troubled. Especially during the first 20 or so seconds after
  start-up you can easily trigger the interpreter to crash. The exact cause is
  not well understood yet. You will sometimes notice high system activity
  shortly after the start-up of the IDE. You can see sclang spiking in the
  Windows task manager when that happens. In that period the interpreter can be
  easily crashed by keyboard input or mouse dragging (maybe the autocomplete
  popup enforces the crash, but that is not clear either). Don't panic, the
  interpreter can be restarted easily and the system will eventually stabilize
  and become quite useable then. If you had a server running when the
  interpreter crashed, the server will remain in memory as "zombie". The stray
  process will have to be killed before starting a new server. This can be done
  in the Windows task-manager (look out for `scsynth.exe`), or more  easily in
  the popup menu that appears if you click on the server status indicator at the
  right bottom of the IDE window ('Kill all servers', requires a running
  interpreter). There is an alternative release of SCWin that contains a
  elaborate fix for this problem (look for IPCfix). The code for this fix,
  written by Lucas Cornelisse, is contained in a topic branch in the SC repo
  (topic/IPC_QTcp)

- READLINE/Command line-mode does not work properly.
  You have to wait a short while with the key pressed to get it to register - if
  you wait for too long, the key will repeat. It's kind of possible to get used
  to it for a short while. Second problem: sclang will crash if you use ctrl-d.
  Another problem shared with the IDE: You need to stop the server process
  manually to avoid scsynth-zombies. Readline is initialized in
  lang/LangSource/SC_TerminalClient.cpp (readlineInit()). This problem can only
  be studied in the MinGW build for now, as the VS build (even 32-bit) cannot
  link to the readline lib yet (and you can only use v. 5.0.1 which isn't
  available as 64-bit binary).

- HID doesn't work (and never did on Windows)

- using shell commands from SC only works in a quite limited way (and always did).
  .unixCmd expects a unix shell, only for essential requirements workarounds
  are in place on Windows.

A build issue that does not seem to create a problem:

- during the MinGW build, when statically linking portaudio, we currently get
  several dozen non-fatal errors (this is not due to the portaudio version,
  it has been observed with several pa-builds. Similar errors are reported in
  unrelated contexts and have to do with boost). These errors seem to have no
  consequence:

      CMakeFiles\SuperCollider.dir\objects.a(find_replace_tool.cpp.obj):-1: error: duplicate section `.data$_ZGVZN5ScIDE4Main8instanceEvE9singleton[__ZGVZN5ScIDE4Main8instanceEvE9singleton]' has different size
      ...


Outro
=====

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available.


[7-zip]: http://www.7-zip.org (unixy decompression tool)
[asio4all]: http://www.asio4all.com/ (ASIO4ALL, generic ASIO driver)
[asiosdk]: http://www.steinberg.net/en/company/developers.html (ASIO SDK v2.3)
[atomeditor]: https://atom.io/ (free unixy text editor with SuperCollider package)
[cmake]: http://www.cmake.org/download/
[conemu]: https://conemu.github.io/ (free console emulator)
[dependency walker]: http://www.dependencywalker.com/ (inspect missing library errors)
[dx9sdk]: http://www.microsoft.com/en-us/download/details.aspx?id=6812 (MS DirectX 9 SDK (June 2010) for VS build)
[fftw]: http://www.fftw.org/install/windows.html
[Git]: http://git-scm.com/download/win (Git for Windows)
[libsndfile]: http://www.mega-nerd.com/libsndfile/
[msys2]: https://msys2.github.io/
[notepad++]: http://notepad-plus-plus.org (free unixy text editor)
[NSIS]: http://nsis.sourceforge.net/Download (create installer)
[portaudio]: http://www.portaudio.com/
[portmidi]: http://portmedia.sourceforge.net/portmidi/
[Qt]: http://www.qt.io/download-open-source/#section-2 (Qt official distribution, choose online installer)
[readline]:http://gnuwin32.sourceforge.net/packages/readline.htm
[readline doc]: https://cnswww.cns.cwru.edu/php/chet/readline/rltop.html
[SC]: https://supercollider.github.io (Main SC-site)
[SC mailing lists]: http://www.birmingham.ac.uk/facilities/ea-studios/research/supercollider/mailinglist.aspx
[SC repo]: https://github.com/supercollider/supercollider (SC source repository on Github with issue tracker)
[SC help]: http://doc.sccode.org/Help.html (SC online help)
[VS]: https://www.visualstudio.com/downloads/download-visual-studio-vs (Visual Studio 2013, community edition)
[Windows SDK]: https://msdn.microsoft.com/en-us/windows/desktop/bg162891.aspx (Windows 8.1 SDK including debugger used by Qt Creator)
