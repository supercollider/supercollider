Building SuperCollider on Linux
===============================


This document guides compiling and configuring SuperCollider on
Linux operating systems. It offers directions for customizing your
SuperCollider setup, making sure the installation meets your goals and technical
needs. It includes information on running `scsynth` and `supernova` in standalone 
mode and running `sclang` with or without a graphical user interface (GUI).


## Table of Contents

- [Build Requirements](#build-requirements)
  - [Essential Build Requirements](#essential-build-requirements)
  - [Configuration Tips](#configuration-tips)
  - [Additional Information](#additional-information)
- [Recommended Packages](#recommended-packages)
- [Installing Requirements on Debian](#installing-requirements-on-debian)
- [Installing Requirements on Fedora](#installing-requirements-on-fedora)
- [Installing Qt for SuperCollider](#installing-qt-for-supercollider)
  - [Installing Qt on recent Debian-like operating systems](#installing-qt-on-recent-debian-like-operating-systems)
  - [Installing Qt on Ubuntu Bionic, Focal, or Jammy](#installing-qt-on-ubuntu-bionic-focal-or-jammy)
  - [Installing Qt on Ubuntu Trusty or Xenial](#installing-qt-on-ubuntu-trusty-or-xenial)
  - [Installing Qt on Fedora](#installing-qt-on-fedora)
  - [Utilizing the Official Qt Installer](#utilizing-the-official-qt-installer)
  - [Using Clang as the Compiler](#using-clang-as-the-compiler)
- [Building](#building)
  - [Step 1: Obtain the source code](#step-1-obtain-the-source-code)
  - [Step 2: Make a build directory](#step-2-make-a-build-directory)
  - [Step 3: Set CMake flags](#step-3-set-cmake-flags)
  - [Step 4: Build](#step-4-build)
- [Troubleshooting Common Build Issues](#troubleshooting-common-build-issues)
- [Building a Debian package](#building-a-debian-package)
- [Running scsynth or supernova (standalone)](#running-scsynth-or-supernova-standalone)
- [Running sclang](#running-sclang)
  - [Headless operation](#headless-operation)
- [Environment](#environment)
- [Contributors to this document](#contributors-to-this-document)

Build Requirements
------------------

Specific prerequisites are necessary to compile SuperCollider on Linux systems.
 These requirements fall into two categories: essential dependencies for the
 core functionality of `scsynth` and `supernova` and optional components that
 provide additional features and capabilities.

### Essential Build Requirements ###

The table below outlines the mandatory dependencies and minimum versions
required to build `scsynth` and `supernova`, the core of SuperCollider's sound
synthesis engine. Also included are optional flags for turning off certain
features if they are not needed for your specific use case.

| Requirement              | Version    | Description                             | Optional Flags             |
|--------------------------|------------|-----------------------------------------|----------------------------|
| C++ Compiler (gcc/clang) | >= 6.3/3.9 | Required for C++17 support.             |                            |
| cmake                    | >= 3.12    | Cross-platform build system.            |                            |
| libsndfile               | >= 1.0     | Library for soundfile I/O.              |                            |
| libjack                  |            | Development headers for JACK.           |                            |
| fftw                     | >= 3.0     | FFT library for DSP functionalities.    |                            |
| libxt                    |            | X ToolKit (required for specific UGens) | `NO_X11=ON` (to disable)   |
| libavahi-client          |            | For zero-configuration networking.      | `NO_AVAHI=ON` (to disable) |
 
### Configuration Tips ###

- **Comprehensive Functionality**: To fully utilize `scsynth` and `supernova`,
  you must install all the listed dependencies. This ensures that you can use
  SuperCollider with all its features.
- **Customizing Features**: If you aim to customize your build for specific
  environments or to exclude certain functionalities, optional configuration
  flags offer flexibility:
  - `NO_X11=ON`: Disables the X11 graphics and the dependency
    [libxt](http://www.X.org). This feature is helpful for headless setups or
    where a GUI is not required. It is required by specific *Unit Generators*
    (`UGen`s), such as `MouseX`.
  - `NO_AVAHI=ON`: This option omits support for Avahi, offered by the
    dependency [libavahi-client](http://www.avahi.org). This dependency is
    primarily used for network service discovery.

Adjusting these flags allows you to streamline your SuperCollider installation,
ensuring it only includes the necessary components.


Additional Information
----------------------

Requirements for `scsynth` and `supernova` with their weblinks:

- C++ compiler with C++17 support: [gcc](http://www.gnu.org/software/gcc) >= 6.3, [clang](https://clang.llvm.org) >= 3.9.  
- Build system: [cmake](http://www.cmake.org) >= 3.12.
- Soundfile I/O: [libsndfile](http://www.mega-nerd.com/libsndfile) >= 1.0.  
- Audio routing: [libjack](http://www.jackaudio.org/).
- DSP functionalities: [fftw](http://www.fftw.org) >= 3.0.


Recommended Packages
--------------------

Enhancing `sclang` and `scide` beyond their core functionalities requires
additional packages. These packages add support for everything from graphical
user interfaces to MIDI integration. The table below details these packages,
highlighting their specific roles and, where applicable, the minimum version
requirements to ensure compatibility and functionality.

| Package      | Purpose                                           | Version | More Information                                                        |
|--------------|---------------------------------------------------|---------|-------------------------------------------------------------------------|
| Qt           | Powers the IDE and enhances the Qt GUI toolkit.   | >= 5.7  | [Qt](http://qt-project.org)                                             |
| git          | Enables the Quarks package management system.     |         | [git](https://git-scm.com/)                                             |
| ALSA         | Provides MIDI support for musical interfaces.     |         | [ALSA](http://www.alsa-project.org)                                     |
| libudev      | Supports Human Interface Devices (HID) for input. |         | [libudev](http://www.freedesktop.org/software/systemd/man/libudev.html) |
| Linux kernel | Ensures compatibility for low-level device input. | >= 2.6  | [Linux kernel](http://www.kernel.org)                                   |
| libreadline  | Improves command-line interface (CLI) usability.  | >= 5    | [libreadline](http://savannah.gnu.org/projects/readline)                |
| ncurses      | Enhances text interface interaction within CLI.   |         | [ncurses](https://invisible-island.net/ncurses/)                        |

**Qt Version Note:** We advise using the most recent release within the Qt 5.x series.**

Installing requirements on Debian
---------------------------------

Dedicated `README` files in this repository focus on building for particular
embedded Linux platforms:

- Raspberry Pi: `README_RASPBERRY_PI.md`
- BeagleBone Black: `README_BEAGLEBONE_BLACK.md`
- Bela: `README_BELA.md`

On Debian-like systems, the following command installs the minimal recommended
dependencies for compiling `scsynth` and `supernova`:

``` sh
sudo apt-get install build-essential cmake libjack-jackd2-dev \
  libsndfile1-dev libfftw3-dev libxt-dev libavahi-client-dev
```

If you need to use `jack1` replace `libjack-jackd2-dev` by `libjack-dev`.

The following command installs all the recommended dependencies for `sclang`
except for Qt:

``` sh
sudo apt-get install git libasound2-dev libicu-dev \
  libreadline6-dev libudev-dev pkg-config libncurses5-dev
```

Installing requirements on Fedora
---------------------------------

The following commands should install all the recommended SuperCollider
dependencies on Fedora, except for Qt:

```sh
sudo dnf groupinstall "Development Tools"

sudo dnf install cmake libsndfile-devel wayland-devel \
  xorg-x11-server-Xwayland-devel pipewire-devel \
  pipewire-jack-audio-connection-kit-devel systemd-devel \
  fftw-devel alsa-lib-devel libatomic

sudo dnf install emacs # if building with the sc-el backend (default)
```

Installing Qt for SuperCollider
-------------------------------

To run the SuperCollider IDE and Sclang’s Qt GUI system, you will need Qt
version 5.7 or newer. Upgrading Qt may present challenges, as some Linux
distributions may not provide the latest version in their standard repositories.


### Installing Qt on recent Debian-like operating systems ###


For Debian-based systems, including Ubuntu and its derivatives, the version of
Qt available through the official package repositories may only sometimes meet
the minimum required version (5.7 or newer) for SuperCollider. To determine the
version of Qt currently available to you, execute the following command in your
terminal:

```sh
apt-cache policy qt5-default
```

If this displays version 5.7 or later, installing Qt is easy:

```sh
sudo apt-get install qt5-default qt5-qmake qttools5-dev qttools5-dev-tools \
  qtwebengine5-dev libqt5svg5-dev libqt5websockets5-dev
```

If you are on Ubuntu, check the sections below. You must use the official Qt
installer if these instructions don't work.

### Installing Qt on Ubuntu Bionic, Focal, or Jammy ###

On Ubuntu 18.04 (Bionic), 20.04 (Focal), and 22.04 (Jammy) Qt5 is available in
the system's package manager. allow you to install new Qt versions simply.


``` sh
sudo apt-get install qtbase5-dev qt5-qmake \
    qttools5-dev qttools5-dev-tools qtdeclarative5-dev \
    libqt5svg5-dev libqt5websockets5-dev qtwebengine5-dev
```

### Installing Qt on Ubuntu Trusty or Xenial ###

If you are on Ubuntu 14.04 (Trusty) or 16.04 (Xenial), [Stephan Binner's
Launchpad PPAs](https://launchpad.net/~beineri) allow you to install new Qt versions easily.

On Xenial:

``` sh
sudo apt-add-repository ppa:beineri/opt-qt-5.11.0-xenial

sudo apt-get update

sudo apt-get install qt511base qt511location qt511tools \
    qt511webchannel qt511xmlpatterns qt511svg \
    qt511webengine qt511websockets
```

On Trusty, only Qt 5.10 and below are available:

``` sh
sudo apt-add-repository ppa:beineri/opt-qt-5.10.1-trusty

sudo apt-get update

sudo apt-get install qt510base qt510location qt510tools qt510webchannel \
  qt510xmlpatterns qt510svg qt510webengine qt510websockets
```

### Installing Qt on Fedora ###

``` sh
sudo dnf install qt5-qtbase-devel qt5-qtsvg-devel qt5-qtwebengine-devel \
  qt5-linguist qt5-qtwebsockets-devel
```

### Utilizing the Official Qt Installer ###

When the Qt version available through your distribution's package
manager does not meet SuperCollider's requirements, downloading Qt directly from
the [official Qt website](https://www.qt.io/) is an alternative.

During the download process, choose the "open-source" license option and proceed
with the installer. You may encounter a prompt requesting Qt Account login
during installation; this step can be bypassed by selecting "Skip," allowing you
to continue without authentication.

During the "Select Components" phase, expand the Qt section to pick the newest
version, ensuring that the "Desktop" components are included. For those
compiling the SuperCollider IDE, QWebEngine is also necessary. It's important to
note that the Qt installer includes QtCreator by default, a substantial download
that cannot be opted out of through the installer interface.


### Using Clang as the Compiler ###

**SuperCollider can be compiled with Clang.**

On Linux, clang will use `libstdc++` instead of `libc++`
(`-DSC_CLANG_USES_LIBSTDCPP=ON`). You can set this option to off to use `libc++`
instead; however, this may cause issues when linking Qt.
 

``` sh
cmake -DSC_CLANG_USES_LIBSTDCPP=ON
``` 

Building
--------

### Step 1: Obtain the source code ###

SuperCollider is presently hosted on Github:
<https://github.com/SuperCollider/SuperCollider>

To obtain the SuperCollider source code, download a release tarball or clone
the repository with `git`.

SuperCollider releases are available to download here:
<https://github.com/supercollider/supercollider/releases>

You can clone the repository with the following command:

``` sh
git clone --recurse-submodules https://github.com/SuperCollider/SuperCollider.git
```

The `--recurse-submodules` option will clone the repository's submodules needed
to build SuperCollider. Running the following command from the root of your
local repository also clones and syncs all the necessary submodules you will need:

``` sh
git submodule update --init --recursive
```

### Step 2: Make a build directory ###

First, `cd` into the root of the SuperCollider source directory (where this file
resides). Create a build directory and `cd` into it:

``` sh
mkdir build
cd build
```

You can name this whatever you want, allowing multiple independent build
directories. If your SuperCollider source is also a Git repository, the
`.gitignore` file will tell git to ignore files in the `build` folder.

### Step 3: Set CMake flags ###

Configuring your SuperCollider build with CMake allows you to customize the
installation based on your specific needs, including which components to include
or exclude.

Set CMake flags directly in the command line, use graphical interfaces like
`ccmake` or `cmake-gui`, or manually edit the `CMakeCache.txt` file for
fine-tuning. Remember, once set, CMake flags remain persistent across builds
unless changed.

#### Key CMake Options ####

CMake offers a variety of flags to customize your build. Running `cmake -LH ..`
in your build directory can explore a comprehensive list. We'll highlight
essential ones here and introduce them in more detail in the documentation.


#### Handling Nonstandard Qt Installations ####

Specifying the Qt installation path becomes necessary when integrating `sclang`
with GUI features or the SuperCollider IDE, especially if you've installed Qt
outside standard package managers.

``` sh
cmake -DCMAKE_PREFIX_PATH=/path/to/qt5 ..
```

The location of `/path/to/qt5` will depend on how you installed Qt:

- If you downloaded Qt from the Qt website, the path is two directories down
  from the top-level unpacked Qt directory, in a folder called `gcc`:
  `Qt/5.11.0/gcc_64/` (64-bit Linux) or `Qt/5.11.0/gcc/` (32-bit). By default,
  the Qt installer places `Qt/` in your home directory.
- If you used the Trusty/Xenial PPA's described above, the path is `/opt/qt511`
  or `/opt/qt510` (depending on which version you installed).

#### Building Without Qt ####

If your project doesn't require the SuperCollider IDE or Sclang's GUI elements,
you can turn off Qt support to streamline your build:


``` sh
cmake -DSC_QT=OFF ..
```

#### Compiler optimizations ####

If you're building SC for production use or don't plan on using a debugger,
make sure to build in release mode:

``` sh
cmake -DCMAKE_BUILD_TYPE=Release ..
```

This sets the compiler to the best optimization settings. Switch back to the
defaults using `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`.


Suppose you're compiling SC only for use on your machine (that is, you aren't
cross-compiling or packaging SC for distribution). In that case, you can turn on
the NATIVE flag to enable CPU-specific optimizations:


``` sh
cmake -DNATIVE=ON ..
```

#### Install location ####

By default, SuperCollider installs in `/usr/local`, a system-wide install.

If you can't or don't want to use superuser privileges or want to install for a
single user, to do so, set `CMAKE_INSTALL_PREFIX` to the desired installation
directory. One good place to put it would be `$HOME/usr/local`:

``` sh
cmake -DCMAKE_INSTALL_PREFIX=~/usr/local ..
```

If you do this, ensure `~/usr/local/bin` is in your PATH. Add a line such as
`PATH=$PATH:$HOME/usr/local/bin` to `~/.profile`.


#### Speeding up repeated builds ####


If you are developing SC or constantly pulling in the latest changes, rebuilding
SC can often be a drag. Installing `ccache` can speed up re-compilation. Here is
how to configure CMake to use it:

``` sh
cmake -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++ -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc ..
```

This assumes your system installs the `ccache` executable at `/usr/lib/ccache` -
you may need to change the path to reflect your system.

#### Library suffix ####

In some situations, it is preferable to install libraries and plugins not in the
`lib` directory but in a suffixed one, e.g., `lib64`. In such a case, you can set
the `cmake` variable `LIB_SUFFIX`. For example, if you wish to install into
`lib64`:

``` sh
cmake -DLIB_SUFFIX=64 ..
```

### Step 4: Build ###

If CMake runs successfully without errors, you are ready to move on to building.
You can freely alternate between building and setting CMake flags.

After setting your CMake flags, just use `make`:

``` sh
make
``` 

The `-j` option leverages multiple cores for faster build times if possible:

``` sh
make -j$(nproc)
```

To install system-wide, run

``` sh
sudo make install
```

Remember to run `sudo ldconfig` after the initial installation to update library links.

To uninstall (system-wide): `sudo make uninstall`.

Troubleshooting Common Build Issues
-----------------------------------

- **Missing Dependencies**: If you encounter errors related to missing packages
  or libraries, ensure all build requirements are according to the "Build
  Requirements" section. Some Linux distributions may have package names that
  are different from those listed, so you may need to search for the correct
  package names for your specific distribution. If you need clarification on
  what's missing, consult the error messages provided by the build process,
  which typically indicate what was not found.
  
 
- **Qt Version Issues**: If you're facing problems with Qt, especially on older
  Linux distributions, ensure you're using Qt 5.7 or later. See the
  [Installing Qt for SuperCollider](#installing-qt-for-supercollider)
  section for instructions on installing or updating Qt.


Building a Debian package
-------------------------

The Debian Multimedia team maintains the most up-to-date Debian packaging rules.
Find the repository (with a `debian/` folder) here:

<https://salsa.debian.org/multimedia-team/supercollider>

Running scsynth or supernova (standalone)
-----------------------------------------

Run `scsynth --help` or `supernova --help` to get an option summary. Remember to
start `jackd` before starting the server. If you want to add directories to
SuperCollider's search path or assign default jack ports, and set up your
environment as described below.

You can specify the number of jack input/output channels created with the
options `-i` and `-o`, respectively.

The `-H` option specifies a jack server to connect to and to set the jack client identifier. The format is either

  `<SERVER-NAME>:<CLIENT-NAME>`

Or just

  `<CLIENT-NAME>`

when connecting to the default server.

Running sclang
--------------

Supercollider comes with its own powerful IDE. Run it with:

```sh
scide
```

Alternatively, you can use sclang in combination with your preferred text
editor, such as Emacs, Vim, or Gedit. For installation and usage, see the
README.md files in editors/*. Another alternative is to run the sclang
executable, which provides a readline-based interface.


`sclang` executes the startup file `~/.config/SuperCollider/startup.scd` after
class library initialization. This file can contain statements to set up your
supercollider environment, like setting default variables. An example can be
found in `linux/examples/sclang.sc`.

You _have_ to have a directory `~/.local/share/SuperCollider`. This is where a
`SynthDef` directory is automatically created. It is also the place to put
Extensions to the class library in a folder called Extensions.

The runtime directory is the current working directory or the path
specified with the `-d` option.

### Headless operation ###


To utilize SuperCollider's command-line interface (`sclang`) without a graphical
interface, you can operate it in a headless mode even when the standard build
relies on the Qt framework. This is particularly useful for running
SuperCollider on servers or any environment lacking a graphical user interface
(GUI). To enable headless operation, you must set the `QT_QPA_PLATFORM`
environment variable `offscreen`. This instructs Qt to run without attempting
to connect to an X server, thereby allowing `sclang` to execute in environments
without a display server:

```sh
export QT_QPA_PLATFORM=offscreen
sclang
```

By configuring this environment variable, `sclang` bypasses the need for a graphical
output, making it adaptable for scripts, automated tasks, and server-based
applications where GUI is unnecessary or unavailable.

Environment
-----------

The jack audio driver interface is configured based on environment variables:

- SC_JACK_DEFAULT_INPUTS comma-separated list of jack ports to which the
  server's inputs should be connected by default:


```sh
export SC_JACK_DEFAULT_INPUTS="system:capture_1,system:capture_2"
```

To connect the first ports of one jack client, it is possible to
specify only the client name:

```sh
export SC_JACK_DEFAULT_INPUTS="system"
```

- SC_JACK_DEFAULT_OUTPUTS comma-separated list of jack ports to which the
  server's outputs should be connected by default:


``` sh
export SC_JACK_DEFAULT_OUTPUTS="system:playback_1,system:playback_2"
```

To connect the first ports of one jack client, it is possible to
specify only the client name:

```sh
export SC_JACK_DEFAULT_OUTPUTS="system"
```

Two additional environment variables substitute directories for the default
search path for plugins and synth definitions, respectively. Directory names are
separated by ':' as in the Unix PATH variable:

- SC_PLUGIN_PATH, SC_SYNTHDEF_PATH

```sh
export SC_SYNTHDEF_PATH="./synthdefs:/home/sk/SuperCollider/synthdefs"
```

Contributors to this document
-----------------------------

- stefan kersten <sk AT k-hornz DOT de>
- andi pieper
- maurizio umberto puxeddu
- rohan drape
- mario lang
- john yates
- nescivi (marije baalman)
- dan stowell
- tim blechmann
