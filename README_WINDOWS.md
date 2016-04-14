Welcome to Supercollider 3.7 for Windows!
=========================================

**IMPORTANT NOTE**:

Currently the Windows version of SC has several serious issues:

- communication between the IDE and sclang has serious issues. Especially
  during the first 20 or so seconds after start-up you can easily trigger the
  interpreter to crash. The exact cause is not well understood yet. You will
  sometimes notice high system activity shortly after the start-up of the
  IDE. You can see sclang spiking in the Windows task manager when that happens.
  In that period the interpreter can be easily crashed by keyboard input or mouse
  dragging (maybe the autocomplete popup enforces the crash, but that is not clear
  either). Don't panic, the interpreter can be restarted easily and the system
  will eventually stabilize and become quite useable then. If you had a server
  running when the interpreter crashed, the server will remain in memory as
  "zombie". That stray process will have to be killed before starting a new
  server. This can be done in the Windows task-manager (look out for
  `scsynth.exe`), or more  easily in the popup menu that appears if you click on
  the server status indicator at the right bottom of the IDE window ('Kill all
  servers').
- Command line mode (readline) does not work, i.e. you cannot work interactively
  from the keyboard in sclang. You can send it text files though by using the
  path to the textfile as argument to the command `sclang`.
- Midi doesn't work
- HID doesn't work and never worked
- using shell commands from SC only works in a very limited way (and always did).
  .unixCmd expects a unix shell, only for most essential requirements
  workarounds are in place on Windows.

More specific smaller Windows bugs are:

- on the command line the server cannot be closed with ctrl-c
- in general shutdown behavior of the server is not clean. When quit from
  sclang it crashes rather than closes properly (this seems not to have
  serious consequences though)
- version control of Quarks via git doesn't work (related to .unixCmd)

This Readme has two blocks: The first paragraphs provide basic use information,
and also try to introduce to SC speak a bit. Later chapters, starting with
'Building SC...' describe requirements and possible environments to build SC.
The initial chapters provide the essentials, subsequent "walkthroughs" try to
help unexperienced developers with common build troubles.

February 2016


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
    - Building sc3-plugins (passing build settings to `cmake`)
    - Avoiding the command line: from CMake-Gui to Visual Studio
    - Using Qt Creator
    - More cmake (building supernova and more)
  - Diagnosing build problems
  - Description of SC 3.7 release build
- Links
- Outro


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

Older releases and snapshots of the source code used for releases are available
at Sourceforge:

    http://sourceforge.net/projects/supercollider/files/?source=navbar

There is an online version of SuperCollider Help, the site SCDOC:

    http://doc.sccode.org/Help.html

SC has a life in social media too. Visit for example sccode.org, the Facebook
group SuperCollider, or find SuperCollider tweets, e.g. @sc140tweets. In
addition you are encouraged to subscribe to the two SC mailing lists 'sc-users'
and 'sc-devel'. You will get friendly and qualified replies there, as well as
interest, and encouragement for your projects:

    http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml


Installing SuperCollider
========================

For SC itself just run the downloaded installer. If necessary select an
alternative install path when prompted during installation. In order to
uninstall, use the default procedure on Windows (Control panel -> Uninstall a
program) or run Uninstall.exe from within the SC installation folder. Note
that files created in the 'user support directory' are not deleted (see below
for details and an explanation).

IMPORTANT: in order to use the Quarks extension system you *need* to install
Git. It is available from

    http://git-scm.com/downloads

Git for Windows also supplies a unix-shell running in Windows. This is likely
to help applications of "unixCmd" in the future.

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

To start up the IDE use the startmenu link or run the scide.exe program
found within the installation folder. On first start-up expect several
firewall warnings to pop up. The SuperCollider components - the language
interpreter `sclang`, the sound server `scsynth` and the IDE `scide` -
communicate among each other over the network, so you will get a warning for
each of the three executables. If you are new to SuperCollider, inspect the
help system and look for the tutorials. You can open the help system using
Ctrl+D. This keyboard shortcut is context sensitive - if your cursor is on
a sclang term, the help system will likely open with the page explaining that
term. Of course the is also the menu entry `Help` -> `Show Help Browser`.


Using SuperCollider in command line mode
----------------------------------------

*Note*: this is currently broken (Feb 2016, SC3.7)

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
located in the Windows 'local application data' folder. In SC-speak this folder
is called the userAppSupportDir. This location is a bit tricky to find outside
of SC (in the File Explorer its grandparent `AppData` is hidden by default). You
can see it in the environment variable LOCALAPPDATA. Type this on a command
line:

    $> echo %LOCALAPPDATA%

It will likely get this answer (the grandpa related to earlier):

    C:\Users\<USERNAME>\AppData

The userAppSupportDir is in the subfolder `Local\SuperCollider`:

    C:\Users\<USERNAME>\AppData\Local\SuperCollider

This folder is user writable and *not* deleted by uninstall. Therefore your
settings, quarks, plugins etc persist through SC-updates. This is usually great,
but if you have binary "plugins" and update a minor version, e.g. 3.6 to 3.7,
the plugins will not work any more, and cause errors during SC start-up. There
are more files in the `userAppSupportDir` that can cause trouble after updates,
so want to aware of the possibility. In the IDE it is very easy to access the
folder. There is a menu entry -> `Open user support directory`). You can also
ask `sclang` where this folder is located. Type this in the SC-IDE:

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
platform build system Cmake. It can configure builds and generate intermediary
build files for many OS/toolchain combinations. The separation of build system
and actual application development allows to define cross-platform build
definitions in a single framework, and still do actual application development
in your preferred native environment. On Windows this works nicely with Visual
Studio and Qt Creator. While Cmake is command line oriented, but it can also be
controlled from a GUI (`cmake-gui`) and is very well integrated into Qt Creator.
With an added plugin, Cmake can also be controlled from Visual Studio. In order
to develop for SC proper, by any luck, you might not have to care too much about
Cmake, and spend most of the time in your preferred IDE.

The Windows port of SC is in a stage of transition from an older build system
that emulated an unixy environment for build tools (msys/MinGW), to the native
Windows toolchain (Visual Studio and `msbuild`). This transition is not complete
yet. The sc3-plugins collection can currently only be built with the old build
system, as can SuperNova, an alternative server with multi-core capabilities.
Also sclang's interactive command line functionality (readline based) is not yet
in sight for SC built with Visual Studio.

While sc3-plubins cannot be built with Visual Studio yet, 64-bit MinGW built
plugins can be used with VS built SC, if you add the MinGW runtime libraries
to the SC application folder.


Quick Steps
-----------

### Required software

- Source management
  - Git for Windows (latest version)
- Build tools
  - Cmake, latest stable version
  - MinGW, version *4.82* 32-bit, the distribution shipped with Qt (QT\Tools\mingw482_32)
  - Visual Studio, VS 2013 (VS 2015 is likely to work, but untested)
- Libraries
  - Required
    - *Qt* >= 5.5.1
        Use the official download from qt.io and the online-installer. It offers
        flexibility in choosing different packages in a single go (e.g. Qt,
        MinGW and Qt Creator) and helps managing maintenance. It provides a
        file structure that accommodates different Qt flavors (Qt built with
        different toolchains), versions, and modules at the same time. We will
        be referring to that file structure throughout this text.

        *Important note*: the flavor of Qt (e.g. msvc2013_64 or mingw492_32) must
        match your toolchain as closely as possible!

        For a 64-bit VS build select msvc2013_64
        for a 32-bit MinGW build select mingw492_32
    - *libsndfile* >= 1.0.25
- Recommended
  - *fftw* >= 3.3.4
  - *readline* >= 5.0 (more recent versions can be found in MinGW distributions)
- Optional
  - *ASIO-SDK* = 2.3 (Asio support in Portaudio)
  - *DirectX SDK* v.7 for MinGW, v.9 for VS. (Direct Sound support in Portaudio)
  - *NSIS*, if you want to create an binary SC installer (add to path)
- Convenience
  - a unix line-ending friendly text editor like *Atom* or *Notepad++*
  - a extraction tool for unixy compression formats like tar and gz, e.g. *7-zip*


Last but not least: the SC source:

    git clone --recursive https://github.com/supercollider/supercollider.git

The SuperCollider source comes with several submodules, or embedded external
libraries. Adding `--recursive` to the git command makes sure, they get
initialized and populated. When switching branches or otherwise traveling the
variations and history of the SC source, it may sometimes become necessary to
resynchronize SC and submodules. The command to that is:

    git submodule update

If for any reasons submodules need to be re-initialized, the command is:

    git submodule init

Git will usually tell if a resynchronization/update is required.

*Note*: the Portaudio library is not available as binary download. A slightly
patched version of the pa-source is provided as submodule of the SC source.
Portaudio – with WMME support only – is built automatically as part of the
SC compile. If you need to support additional audio APIs, you need to build them
separately (described below).


### Arrangement of toolchain and libraries

- Toolchain/IDE
  - *MinGW*: embedded in the Qt folder tree, downloaded together with Qt. Likely
    location:

      C:\Qt\Tools\mingw482_32

  - *VS/msbuild*: default install of VS2013, paths are handled automatically


- *Qt*
  - default install with web downloader
    - for VS-build: A likely location of the matching libraries is

          C:\Qt\5.5\msvs2013_64

    - for MinGW-build: A likely path to the matching libraries is

          C:\Qt\5.5\mingw492_32

- *libsndfile*, *fftw* and *readline*

  *Important note*: the following steps will save you from frustration when
  cmake can not find dependencies. It is strongly recommended to follow them
  slavishly:

  - if you build a 32-bit SC, create a folder `x86` as a sibling of your
    SC source. For a 64-bit SC call it `x64`
  - extract/install/copy the library folders into into that folder
  - make sure the library's parent folders are called: `libsndfile`, `fftw` and
    `readline`. A spelling mistake here can cause a lot of consternation and
    cost a lot of time.

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

If you have `portaudio` libraries, they can be added in the same logic
(but see below for details)


### Configuring and executing the build

#### MinGW

- add the location of the Qt- and toolchain *binaries* to the head of the search
  path. Qt *must* preceed MinGW:

      $> SET PATH=C:\Qt\5.5\mingw492_32\bin;C:\Qt\Tools\mingw482_32\bin;%PATH%

- store the location of the parent-folder of the Qt-libs of your flavor in the
  environment variable `CMAKE_PREFIX_PATH` (no `bin`!). Example:

      $> SET CMAKE_PREFIX_PATH=C:\Qt\5.5\mingw492_32

- create a build folder, enter it, and run `cmake` to configure the build.

  Cmake needs to to know

    - which "generator" to use
    - the location of the toolchain
    - the location of Qt
    - the location of the source.

  Cmake looks for the toolchain in the path. The Qt location is provided by the
  environment variable we just created.

  The generator has to be `MinGW Makefiles` (capitalization matters). The
  pointer to the source can be absolute or relative.

  Cmake can detect the location of the other (smaller) libraries because the
  find modules in SC look in custom locations. This is why we created that
  specific folder tree earlier.

  To create the build-folder as a subfolder of the SC source called `build` we
  can do this:

      $> cd supercollider
      $> mkdir build
      $> cd build
      $> cmake -G "MinGW Makefiles" ..

With some good fortune the configuration should complete successfully. Otherwise
the hunt starts!

The SC MinGW-build will be configured as `RelWithDebInfo` if we don't make a
configuration explicit. If you want a debug-build, rerun cmake and *add* the
assignment `-DCMAKE_BUILD_TYPE=Debug`:

      $> cmake -DCMAKE_BUILD_TYPE=Debug ..

The setting for the generator is kept, the path to the source has to be
entered each time Cmake is run.

If you want to add `supernova`, add `-DSUPERNOVA=ON`

      $> cmake -DSUPERNOVA=ON ..

Remains the actual `build`. We can also use `cmake`, the following command works
on all platforms with all generators:

      $> cmake --build . --target install

In the MinGW-build an additional custom target must be built to add the
missing libraries to the install folder:

      $> cmake --build . --target bundle

The time consuming `bundle` step can be omitted on subsequent builds, provided
no changes to libraries are to be expected.

If you want to create a binary installer, run:

      $> cmake --build . --target installer

For this to work you will have to have NSIS installed on your system. The NSIS
installer add's the command line tool `makensis` to the system, which is
required for the SC script to succeed.

The SC installer executable will be written to the same parent folder as the
SuperCollider install-build, for example:

    ...\supercollider\build\x64\Release\SuperCollider-<version-build type>.exe

Note that for just passing SC to a friend it is not really necessary to create
an installer. You can copy the main SuperCollider folder and copy it to another
machine. The userAppSupportDirectory will be created from scratch. But you will
loose your local customizations and additions. The new Quarks system provides
means to make porting of extension/Quark groups easier.


### VS (msbuild)

- while not strictly required it is safer to add the Qt binaries location
  at the head of the system path. The toolchain is detected automatically,
  using an environment variable provided by VS:

      SET PATH=C:\Qt\5.5\msvc2013_64\bin;%PATH%

- create the environment variable `CMAKE_PREFIX_PATH` to point to Qt, e.g.:

      $> SET CMAKE_PREFIX_PATH=C:\Qt\5.5\msvc2013_64

- create a build-folder, move to it and configure the build by running `cmake`.
  The *generator* and path to the sc source have to be passed in, e.g.:

      $> mkdir sc-build
      $> cd sc-build
      $> cmake -G "Visual Studio 12 2013 Win64" ..\supercollider

- you should end up with a solution file `supercollider.sln` that can be
  opened in VS. You can also to build from the command line. As the VS
  generator is a multi configuration generator, the build configuration is
  passed in in this step:

      $> cmake --build . --target install --config Release

- if you want to create a binary installer, build this custom target:

      $> cmake --build . --target installer

Done!


### Portaudio

Portaudio is provided as submodule of SC and will be built automatically as part
of the SC-build. Yet at the time of this writing the only sound-api supported by
this embedded build is WMME. If you need Asio support, or any of the other APIs
supported by Portaudio (DSound, WDM-KS, WASAPI), it needs to be built separately.
Especially when using the MinGW toolchain, the easiest way will be
to use the portaudio source provided as submodule of the SC source. SC currently
provides portaudio svn rev. 1955 with a few patches that simplify the
MinGW-build significantly (all APIs except WASAPI build out of the box using
Cmake). The portaudio source is located in:

    <supercollider source folder>\external-libraries\portaudio

You can study the file `CMakeLists.txt` in that folder to learn about the
options that the build provides. With default settings, all APIs that work out
of the box are enabled, and both static- (.a or .lib) and shared libraries
(.dll) are built.

In order to add Asio-support and DSound support, the respective SDKs have to
be added to the build.

ASIO: download the SDK from Steinberg, extract the zip and place the folder
in a sibling folder of *supercollider* (not portaudio). The folder name should
not contain dots, start with `as`, and be the immediate parent of the library
folders (asio, common, ...).

You should end up with a folder tree like this:

    sc-source
    asiosdk
        asio
        common
        driver
        host

DirectX: we need to distinguish between the MinGW- and the VS-build here. The
MinGW-build only works with version 7, whereas the VS-build only works with
version 9 (June 2010). Version 9 can still be downloaded from MS and installed
into the OS. Downloading version 7 needs some research, at the time of this
writing no official download was available. Provided you get a hand on the
files, don't install them, just copy the folders `include` and `lib`into a
folder. Its name needs to start with `dx`. Make that folder a sibling of the
SuperCollider source.

In the VS-build, a properly installed dx9-SDK will be found automatically. In
the MinGW-build, the dx7 files will be found if your folder arrangement is
like so:

    sc-source
    dx7sdk
        include
        lib

For a default build, proceed in the logic of the steps above. No reference to Qt
is required.


#### MinGW

- set the path to the MinGW toolchain (Qt is not used in the PA build):

      $> SET PATH=C:\Qt\Tools\mingw482_32\bin;%PATH%

- execute the following steps:

      $> mkdir pa-build
      $> cd pa-build
      $> cmake -G "MinGW Makefiles" ..\supercollider\external-libraries\portaudio

- build:

      cmake --build .

The resulting file into the folder `x86\portaudio` next to the SC source. The
folder `portaudio` must also contain a folder `include` with the header file
`portaudio.h`. Copy it from the include-folder in the portaudio source:

    x86
        portaudio
            libportaudio_x86.dll
            libportaudio_x86.dll.a
            libportaudio_static_x86.a
            include
                portaudio.h


If Cmake finds a portaudio library in the defined location at the beginning of
a SC build, it will not run the embedded portaudio build, but use the provided
library. The SC build preferrs the dll over the static library. So if you want
to link statically, zou have to remove the .dll and .dll.a from the folder.


#### VS

The steps are similar, but it is not necessary to provide a path to the
toolchain:

    $> mkdir pa-build
    $> cd pa-build
    $> cmake -G "Visual Studio 12 2013 Win64" ..\supercollider\external-libraries\portaudio
    $> cmake --build . --target portaudio_static

The SC VS-build requires the portaudio lib to have the same build configuration
as the SC build. Therefore a debug- and a release-version of the library must be
built. VS builds a debug version by default, so we have to repeat the command
above and add `--config Release`

    $> cmake --build . --config Release

The file- and folder-arrangement in x64\portaudio folder differs. We have to
provide a `Debug` and a `Release` folder. Also the bin/lib distinction is
flattened. The include folder is on the same level as Debug and Release:

    x86
        portaudio
            Debug
                portaudio_static_x64.lib
                portauido_x86.dll
                portauido_x86.exp
                portauido_x86.ilp
                portauido_x86.lib
                portauido_x86.pdb
            Release
                portaudio_static_x64.lib
                portauido_x86.dll
                portauido_x86.dll.manifest
                portauido_x86.exp
                portauido_x86.lib
            include
                portaudio.h

The linking method used in the SC build depends on the library provided. If both
static and dynamic libraries are provided, dynamic ones will be chosen. In order
to link to the static library, remove libraries for dynamic linking from the
folder.

In order to have the SC-build create and use a dynamic library with
the embedded build, pass -DPA_LIBRARY_TYPE="shared" to the configure/cmake
command.


Walkthroughs
------------

### Building sc3-plugins (passing build settings to `cmake`)

sc3-plugins is a popular collection of binary extension to SC providing many
additional UGens. They are hosted in the SuperCollider repository on Github:

    git clone --recursive https://github.com/supercollider/sc3-plugins.git

Like the core SuperCollider repo, this source collection contains sub-modules
that may need to be re-synchronized when traversing the source history. The
command is:

    submodule update

Initialization was automatically done because auf the flag `--recursive` at the
initial clone. If at any point this is required again (message "submodule
missing"), the command is:

    submodule init

Git will usually tell when a resynchronization is required.

This walkthrough expects that you have a working SC build environment in place.
sc3-plugins do not have additional dependencies, so you are set. Furthermore Qt
is not required, which makes things a bit easier. On the other hand the cmake
find-modules provided with sc3-plugins often fail, so that more manual
adjustment is likely to be necessary.

In order to see for which Cmake variables we need to provide values, we can do a
trial run. For this to be meaningful we need to

  - add the toolchain binary path to the system path
  - specify the generator
  - and point `cmake` to the source

        $> SET PATH=C:\Qt\Tools\mingw482_32\bin;%PATH%
        $> mkdir sc3-plugins-build
        $> cd sc3-plugins-build
        $> cmake -G "MinGW Makefiles" ..\sc3-plugins

Cmake will likely return two or three errors:

    FFTW3F_INCLUDE_DIR=...NOT_FOUND
    FFTW3F_LIBRARY=...NOT_FOUND
    SC_PATH=...NOT_FOUND

We know the SC path, and we also know the location of the 32-bit fftw library,
which was used in the SC build. The expected library is likely dynamic, so we
need to specify the .dll file in the `bin` folder. Let's do another trial run
with the added information:

    $> cmake -DFFTW3F_INCLUDE_DIR=..\x86\fftw -DFFTW3F_LIBRARY=..\x86\fftw\libfftw3f-3.dll -DSC_PATH= ..\supercollider ..\sc3-plugins

This looks like enough. Note that Cmake stores the variables you set in a file
called `CMakeCache.txt` in the build folder. It is not required to re-enter
values that have been set already. If you get stuck and decide to restart from a
empty build configuration, you can delete that file and reconfigure. This way
you can keep many of the files already built while making sure that all
configuration values you add are generated from anew. This can save a lot of
compile time. At times though you might experience that it is necessary to
remove everything in the build folder and start from scratch.

There could also be variables that define build options without producing errors
if no value is provided. They can be listed by adding the flag `-L` (or `-LA` for
a longer list) to the  configure run:

    $> cmake .. -L

This will show us amongst other things, that an option to build plugin versions
for the alternative audio server `supernova` is available. To add them to the
build, just run:

    $> cmake -DSUPERNOVA=ON ..

The old settings will stay in place.

Ready to build:

    $> cmake --build . --target install

Unfortunately on Windows we will get an error at the end of this build, because
Cmake wants to install the files to the Windows default install location, which
is in the folder "Program Files (x86)". As this path contains an empty space,
the install fails. Further more, installing the plugins to the application folder
doesn't make much sense, we want to have them around in a folder we can easily
add to the folders scanned by the server.

In order to correct that error, we need to rerun the configuration and add a
install folder without a space as value for the variable CMAKE_INSTALL_PREFIX:

    cmake -DCMAKE_PREFIX_PATH=./Install ..

    $> cmake --build . --target install

This will copy all files to the subfolder `Install` in the build folder. The
second build should proceed very quickly as cmake only builds changed components
from scratch.


#### 64-bit sc3-plugins

It is not clear yet whether MinGW compiled plugins can be used reliably with a
VS studio built 64-bit SC. It seems to work and no negative feedback has arrived,
but the opinion of experts vary between "no problem" and "unlikely to work
well".

As to the build: switching to a 64-bit build is done by pointing to a toolchain
that builds 64-bit applications. The path should be added to the head of the
system search path to minimize the risk that wrong files are found in other
folders (the folders in the search path are searched from head to tail until
a match is made, so the first hit wins):

    $> SET PATH=C:\MinGW64\bin;%PATH%

We need to point to the FFTW3F library again, but this time from the x64
folder:

    $> cmake -G "MinGW Makefiles" -DFFTW3F_INCLUDE_DIR=..\x64\fftw\include -DFFTW3F_LIBRARY=..\x64\fftw\libfftw3f-3.dll -DSC_PATH= ..\supercollider ..\sc3-plugins

Binaries built with MinGW require MinGW provided runtime libraries. These will
have to be added to the SC install directory where `scsynth` is located. You can
identify the libraries one by one by starting scsynth, and providing the files
reported as missing. You will find them in the MinGW binaries folder. A more
systematic approach would be using a tool like `depends`, which is gratis. Most
likely the libraries are:

    libstdc++-6.dll
    libwinpthread-1.dll
    libgcc_s_seh-1.dll

The third library's name varies in accordance with the compiler used.

To sum up: When working in this interactive trial-error mode with Cmake
variables that have already been assigned values are stored in the cache file
(CMakeCache.txt). There is no need to enter them repeatedly. Passing in
arguments with `-D` will set new variables with their values, or overwrite
existing ones in the cache. You can gradually aggregate the required values
error by error. On the command line two things are important time savers: when
entering paths, tab completion works, which helps avoiding typos. Also the
history function (arrow up) is very useful. Cmake will not always detect typos
in path assignments. If a wrong path is entered, this will only become apparent
in the build ("header not found" etc.).


### Avoiding the command line: from CMake-Gui to Visual Studio

If you feel like preferring to avoid the command line, the next most geeky
option is Cmake-gui, which can be started from the Windows Start Menu.

As on the command line, cmake will need to know:

  - the generator to use
  - the path to the toolchain
  - the Qt location
  - the path to the source

Cmake can read the path to the MS toolchain from an environment variable. So we
do not need to add anything to the search path. But we still have to supply the
pointer to Qt. Therefore we need to add the variable CMAKE_PREFIX_PATH to the
environment and assign the path to Qt *before* we start Cmake-gui. The path we
need to add is the parent folder of the bin/lib/include folders of the Qt flavor
used (msvc2013_64). Accessing the environment variables in Windows can be done
by hitting Win-key-x, choosing `System` -> `Advanced System Settings` ->
`Environment Variables`. Apply the `SET CMAKE_PREFIX_PATH=C:\Qt\5.5\msvc2013_64`
to this dialog. After the addition it might be necessary to re-login. Note that
there is no `bin` in the CMAKE_PREFIX_PATH, Cmake will work out all relevant Qt
locations relative to this anchor path.

#### Ready for CMake (`cmake-gui`).

In the first two input boxes enter the path to the source and your build folder.
The generator can be chosen in a dialog after you hit `configure` for the first
time. As we chose "Visual Studio 12 2013 Win64" Cmake can work out the location
of the toolchain by itself. With some good fortune Cmake will work for a while
and – without returning an error – present you with a long list of handy data at
the center. At the bottom you can scroll through the output Cmake produced
during configuration and analyze it. At this stage it is good to scroll through
the central pane and check for the paths related to our "smaller" libraries:
libsndfile, fftw and readline. Do the paths presented fit the locations on your
system? If everything looks okay, you can hit "generate" to create the files
used to build by VS. If anything looks wrong, you may try to correct it in the
window presented. On the other hand, errors in this stage are more likely to be
caused by conflicts with your system settings, typos in paths or the like. In
that case it is better to fix things at the cause, because such mistakes are
likely to cause build errors later on. Common cause are conflicting Qt and MinGW
installs, or alternative versions of the smaller dependencies that were detected
by Cmake. In that case you will have to work out, how to correct things while
both keeping your overall system functional, and allowing for a successful SC
build.

*Note*: not every "NOT-FOUND" entry necessarily indicates an error. In the case of
Portaudio for example, the absence of provided libraries triggers the build of
portaudio as part of the SC build. So as a first approximation only settings
should be changed or corrected that are understood to be wrong or missing.

*Note*: If you come back after changing something in your system, the safest thing
is to delete the build settings cache (`CMakeCach.txt` in the build folder) and
rerun the configuration from scratch. Otherwise old setting might stick. You can
delete the file yourself or use the more radical "delete cache" in CMake-gui's
file menu.

The nicest feature of CMake-gui is that it presents a complete list of available
build options and variables in an editable list. This allows identifying
variables in a far more comfortable way than by querying with `cmake -L`. Also
entering paths is both more comfortable and safer: you get path-selection
dialogs which minimize the the likelihood of typos and wrong syntax. Options
can be set on or off with a single click.

*Note*: don't click randomly and enable all options that seem promising. Some of
them don't work in the Windows build. For example the supernova server can't be
built with VS yet. Usually SuperCollider's default values are a considered
choice that should only be changed for a good reason, knowing the implications.
Once the configuration looks okay, you are ready to "generate" the final build
files. This isn't the build though. Cmake-gui just generates intermediate build
files, the build will either have to be done on the command line, or in an IDE.
In our case Cmake will generate a solution file in the build folder. Use it to
start VS, and you are ready to develop and build in this feature rich IDE. You
can either double click the solution file or type:

    $> start SuperCollider.sln

After the IDE processed all project files you should be ready to work. Note that
at the time of this writing cmake<->VS integration has its restrictions. You can
always rebuild the initial configuration from within the IDE, changing the build
configuration between Release and Debug should be safe too. But if e.g. you
want to change the architecture (64- vs 32-bit), consider using two separate
build folders and regenerate build files from scratch.

To simplify working with cmake from within the VS IDE you might like this plugin:

https://visualstudiogallery.msdn.microsoft.com/6d1586a9-1c98-4ac7-b54f-7615d5f9fbc7

You will have to configure several user settings to work comfortably. The only
target that can directly run after setting the 'command' in properties/debugging
is 'install'. Executables are first built to the output folder within the target
folders (target/Debug, target/Release). The targets that could in theory run in
their targets folders are sclang, scsynth and scide. At the moment they can only
be run from the install folder because the dll's required by the main binaries
are not available in the target folders. Also can the SC-IDE not easily call
sclang in a different folder, and sclang can't easily call scsynth. So to debug
in the target- rather than the install folders, some additional work on SC's
build system is required.

When trying to debug from the IDE, this plugin is helpful, as it allows
to debug child processes more easily:

https://visualstudiogallery.msdn.microsoft.com/a1141bff-463f-465f-9b6d-d29b7b503d7a

*Note*: building the target `install` (accessible in the "Solution Explorer")
copies all dynamic link libraries to the install folder as well. This differs
from the MinGW build, where building a separate target ('bundle') is
required to get all third party dll's.


### Using Qt Creator

QtC is likely the IDE with the best Cmake-, as well as Qt integration, therefore
it fits our requirements very well. The base steps, namely pointing to the the
toolchain and Qt are handled as part of defining "kits" and controlled via
comfortable selection dialogs. You can open a cmake project *from scratch* and
define a encapsulated, build specific environment that leaves your overall system
alone. QtC can open a Cmake project once it knows the location of the
`CMakeLists.txt` file in the source root folder. It can configure the build
using Cmake in a similar way as the CMake-gui does. You can also do trial cmake
runs, and add variable assignments in text input box. While not presenting
the build options in a neat list, it allows you to edit the CMakeLists.txt files
themselves in a meaningful context.

In a kit, among other things (debugger, source control system and more), you can
select one (among potentially several) Qts on your system, and chose a toolchain
to combine with. If your Qt is installed in a standard location, it is possible
(though unlikely) that QtC already created a kit at start-up that fits our needs.
While QtC can be used to build with the MS toolchain, it is the more likely
choice when building with MinGW.

Before running cmake, we need to create the kit we want to use - it implicitly
tells cmake where to look for Qt and the toolchain: Qt5.5.1-mingw492_32 combined
with mingw482_32 as toolchain. This can be done in Tools -> Options -> Kits and
should be self-explanatory.

Next step is to open the project (File -> Open File or Project) by pointing
to the root cmake script in the top parent folder of the SC source. Then QtC
will prompt you to chose a build folder and run `cmake`. This dialog is well
adapted to the trial-error approach of aggregating the required variables for
the build.

QtC does not automatically add the targets `install` and `bundle` to the build
chain. You will have to do this manually in the "Projects" pane. That is also
the place add kits or modify the environment in which you build is run. You can
add, and after the first build disable the target `bundle` as it takes a long
time to complete without adding anything new if your environment hasn't changed.
You may do the same with the target `installer`, which creates the binary
distributable installer.


### More cmake (building supernova and more)

Learn more about the cmake options by running `cmake --help`. A good window into
the SC build configuration is the file CMakeCache.txt in the build folder, and
the files CMakeLists.txt scattered over the project. A longer list of variables
defined for the SC build is retrieved by adding the flags -L or -AL to the
command `cmake ..`.

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
  package, make sure to compile them with supernova support too. At the time
  of this writing supernova could not be built with VS.

  Within SC you will be able to switch between scsynth and supernova by
  evaluating one of lines below and then booting the server as usual:

      Server.supernova
      Server.scsynth

  Check SC help for `ParGroup` to see how to make use of multi-core hardware.

* It is still possible to build SuperCollider without Qt (and implicitly without
  the IDE). You will get some start-up errors in the language, but they can be
  avoided quite easily with some changes in the in the start-up files. Excluding
  Qt is done with the option "SC_QT":

    -DSC_QT=OFF

More options can be explored by going over CMakeCache.txt in the build folder,
or by studying the file CMakeLists.txt in the SC source root folder.


#### Specifying library- and include-paths manually

If for some reason you cannot use the suggested locations for dependencies, you
will have to tell cmake where to find them. The easiest, but sometimes unfeasible
way is to provide them as command line argument to `cmake`:

    $> cmake -DSNDFILE_LIBRARY_DIR=F:/scbuild/libsndfile ..

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
in the cmake_modules folder for more details:

    set CMAKE_GENERATOR="Visual Studio 12 2013 Win64"
    set QT_HOME=C:/Qt/5.5
    set QT_COMPILATION=msvc2013_64
    set QT_PREFIX_PATH=%QT_HOME%/%QT_COMPILATION%
    set SNDFILE_PREFIX_PATH="C:/Program Files/nerd/libsndfile"
    set FFTW_PREFIX_PATH=%USERPROFILE%/fftw
    set READLINE_PREFIX_PATH=D:/scdeps/x64/readline
    set CMAKE_PREFIX_PATH=%QT_PREFIX_PATH%;%SNDFILE_PREFIX_PATH%;%FFTW_PREFIX_PATH%;%READLINE_PREFIX_PATH%;%PORTAUDIO_PREFIX_PATH%

    REM -DCMAKE_BUILD_TYPE=Debug          #use only in msys2 build
    REM -DSUPERNOVA=ON
    REM start Supercollider.sln

Another way of specifying cmake command line arguments is creating a "toolchain"
file. This is the cmake suggested method. Please look up the Cmake documentation
if you require an advanced configuration and are interested in this approach.


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

- If you build with MinGW, it is the safest to use the Qt www.qt.io. Your flavor
  has to be mingw492_32, and the version should be 5.5.1 or bigger.
  If you build with VS the flavor has to be `msvc2013`.

When asking for build help, please mention the installed versions of the
components you used to build SC!


### Dirty build states

While you are developing SC code, it's generally safe to re-use your build
folder, the build will complete much faster. If you change branches, build
tools, cmake settings, or the versions of your dependencies, things are less
safe, the build configuration might have gone out of sync with the files used in
the build, without Cmake being aware of it. The way out of this is to clean your
build folder - the common ways to do this, in order of severity:

1. `del CMakeCache.txt` (delete your cmake settings for that build)
2. `cmake --build . --config Debug* --target clean` (clean your intermediate
   build files, *use your previous build type in place of Debug*)
3. Delete everything in the build folder. Use the file Explorer or run
   `rmdir . /s /q`

Generally, clearing the CMakeCache.txt should be enough to fix many build
problems. After each one of these, you must re-run `cmake` to configure the
build and generate build files. It's recommended that you create a new build
folder for each branch you're building. In practice, though, you can usually
switch between similar branches and rebuild by simply deleting your
CMakeCache.txt.

Of course there could also be bugs in the SC source. A good source for the
current build status of SC is the travis-ci status page:
[https://travis-ci.org/supercollider/supercollider](). Other good places to find
help are the SuperCollider mailing lists and the issue tracker at Github.


Description of the SC 3.7 release build
---------------------------------------

All dependencies - except portaudio - were downloaded from the original
providers (see links at the end). For both VS and QT the free community editions
were used.

SCWin64 3.7 was built with Visual Studio 12/2013

- Qt5.5.1 (flavor msvc2013_64)
- libsndfile 1.0.26
- FFTW 3.3.4

SCWin32 3.7 was built using Qt Creator, combining MinGW 4.8.2 and Qt 5.5.1
mingw492_32 into a kit. The MinGW distribution provided by Qt was used.

- linsndfile 1.0.26
- FFTW 3.3.4
- Readline 6.2 (as provided by the MinGW distribution shipped with Qt)

In both cases portaudio was compiled using `cmake` with the same toolchain. The
source version used was the one bundled with the SC source (external libraries).

- Portaudio 1.9 svn rev. 1955 with patches for cmake/MinGW build
- ASIO SDK 2.3

The patches can be inspected by looking up the commit history of portaudio
in the SC sources.

The VS build used MS DirectX v9 (June 10), the MinGW build the directX SDK v.7

The tools used were Git (2.7.0.windows.2), cmake (3.4.3), and NSIS (3.0b1) to
create the binary installer.


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
    - [NSIS](http://nsis.sourceforge.net/Download)

- Documented build environments
    - [Visual Studio 2013, community edition](https://www.visualstudio.com/en-us/products/free-developer-offers-vs.aspx)
        - [Windows 8.1 SDK](https://msdn.microsoft.com/en-us/windows/desktop/bg162891.aspx) - contains cdb debugger used by Qt-Creator (Google should know the link for other Windows versions).
    - [msys2](https://msys2.github.io/)

- Build dependencies
    - [Qt official](http://www.qt.io/)
    - [portaudio](http://www.portaudio.com/download.html) - *note*: bundled with SC source since SC 3.7
        - [ASIO SDK](http://www.steinberg.net/en/company/developers.html)
        - [DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
    - [libsndfile](http://www.mega-nerd.com/libsndfile/)
    - [fftw](http://www.fftw.org/install/windows.html) - not required but more powerful than the bundled Green FFT
    - [readline](http://cnswww.cns.cwru.edu/php/chet/readline/rltop.html)  - not needed for build
    - [portmidi](http://portmedia.sourceforge.net/portmidi/) - *note*: bundled with SC source since SC 3.7

- Utilities
    - [notepad++](http://notepad-plus-plus.org) - free text editor that handles unix linefeed
    - [7-zip](http://www.7-zip.org) - decompression tool that handles formats from unix world
    - [Dependency Walker](http://www.dependencywalker.com/) - follow up missing library errors


Outro
=====

Thanks to James McCartney, for making this great piece of audio
software publicly and freely available.
