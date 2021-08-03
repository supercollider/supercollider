Building SuperCollider on Linux
===============================

Build requirements
------------------

These are strict requirements for scsynth and supernova:

- A C++ compiler with C++17 support. SuperCollider guarantees support for [gcc][gcc] >= 6.3 and [clang][clang] >= 3.9.
- [cmake][cmake] >= 3.12: Cross-platform build system.
- [libsndfile][libsndfile] >= 1.0: Soundfile I/O.
- [libjack][libjack]: Development headers for the JACK Audio Connection Kit.
- [fftw][fftw] >= 3.0: FFT library.

These packages are required by default for scsynth and supernova, but the components that require them can be disabled with flags:

- [libxt][libxt]: X toolkit intrinsics, required for UGens such as `MouseX`. To build the servers without X, use the `NO_X11=ON` CMake flag.
- [libavahi-client][libavahi-client]: For zero-configuration networking. To build the servers without Avahi, use the `NO_AVAHI=ON` CMake flag.

[gcc]: http://www.gnu.org/software/gcc
[clang]: https://clang.llvm.org
[libjack]: http://www.jackaudio.org/
[cmake]: http://www.cmake.org
[libsndfile]: http://www.mega-nerd.com/libsndfile
[fftw]: http://www.fftw.org
[libavahi-client]: http://www.avahi.org
[libxt]: http://www.X.org

Recommended packages
--------------------

For sclang and scide:

- [Qt][Qt] >= 5.7 with QtWebEngine and QtWebSockets: Cross-platform GUI library, required for the IDE and for sclang's Qt GUI kit. It's best to get the latest Qt 5.x version.
- [git][git]: Required for sclang's Quarks system.
- [ALSA][ALSA]: Linux sound library, required for sclang MIDI support.
- [libudev][libudev]: Device manager library, required for HID support.
- [Linux kernel][Linux kernel] >= 2.6: Required for LID support.
- [libreadline][libreadline] >= 5: Required for sclang's CLI interface.
- [ncurses][ncurses]: Required for sclang's CLI interface.

[Qt]: http://qt-project.org
[ALSA]: http://www.alsa-project.org
[libudev]: http://www.freedesktop.org/software/systemd/man/libudev.html
[libreadline]: http://savannah.gnu.org/projects/readline
[ncurses]: https://invisible-island.net/ncurses/
[Linux kernel]: http://www.kernel.org
[git]: https://git-scm.com/

Installing requirements on Debian
---------------------------------

There are dedicated READMEs in this repository for building on particular embedded Linux platforms:

- Raspberry Pi: README_RASPBERRY_PI.md
- BeagleBone Black: README_BEAGLEBONE_BLACK.md
- Bela: README_BELA.md

On Debian-like systems, the following command installs the minimal recommended dependencies for compiling scsynth and supernova:

    sudo apt-get install build-essential cmake libjack-jackd2-dev libsndfile1-dev libfftw3-dev libxt-dev libavahi-client-dev

If you need to use JACK1 replace libjack-jackd2-dev by libjack-dev.

The following command installs all the recommended dependencies for sclang except for Qt:

    sudo apt-get install git libasound2-dev libicu-dev libreadline6-dev libudev-dev pkg-config libncurses5-dev

Installing Qt
-------------

**Qt 5.7 or later** is required to be able to run the SuperCollider IDE and sclang's Qt GUI system. This may be a little complicated since some versions of some Linux distributions are stuck with old Qt versions.

### Installing Qt on recent Debian-like operating systems

Depending on your Debian flavor and version, your distribution's PPA may be stuck in an old version of Qt. Try this command to query the Qt version available to you:

    apt-cache policy qt5-default

If this displays version 5.7 or later, installing Qt is easy:

    sudo apt-get install qt5-default qt5-qmake qttools5-dev qttools5-dev-tools qtdeclarative5-dev qtwebengine5-dev libqt5svg5-dev libqt5websockets5-dev

If you are on Ubuntu 14.04 (Trusty) or 16.04 (Xenial), check the next section. Otherwise, you will have to use the official Qt installer. Sorry.

### Installing Qt on Ubuntu Trusty or Xenial

If you are on Ubuntu 14.04 (Trusty) or 16.04 (Xenial), [Stephan Binner's Launchpad PPAs][Stephan Binner's Launchpad PPAs] allow for simple installation of new Qt versions.

On Xenial:

    sudo apt-add-repository ppa:beineri/opt-qt-5.11.0-xenial
    sudo apt-get update
    sudo apt-get install qt511base qt511location qt511declarative qt511tools qt511webchannel qt511xmlpatterns qt511svg qt511webengine qt511websockets

On Trusty, only Qt 5.10 and below are available:

    sudo apt-add-repository ppa:beineri/opt-qt-5.10.1-trusty
    sudo apt-get update
    sudo apt-get install qt510base qt510location qt510declarative qt510tools qt510webchannel qt510xmlpatterns qt510svg qt510webengine qt510websockets

[Stephan Binner's Launchpad PPAs]: https://launchpad.net/~beineri

### Installing Qt using the official installer

Worst case scenario, you can grab Qt off the [Qt official website](https://www.qt.io/). It's best to get the latest version. Click "Download," select the open source license, and download the Qt installer. The Qt installer has a step that prompts for you to log in to a Qt Account, but you don't actually need to authenticate and you can safely click "Skip" at that step.

At the "Select Components" step, pop open Qt, select the latest version, and check the "Desktop" option. If you are building the IDE, also select "QWebEngine."

Unfortunately, the Qt installer does not allow you to deselect the multi-gigabyte QtCreator download.

Using clang
-----------

SuperCollider can be compiled with clang, with the following limitations:
- for clang 4, pass `-DSC_ABLETON_LINK=OFF` when configuring the project
- by default clang will use libc++; you can pass `-DSC_CLANG_USES_LIBSTDCPP=ON` to use libstdc++ instead

Building
--------

### Step 1: Obtain the source code

SuperCollider is hosted on Github: https://github.com/SuperCollider/SuperCollider

Obtaining the SuperCollider source code can be done either by downloading a release tarball, or by cloning the repository.

SuperCollider releases are available to download here: https://github.com/supercollider/supercollider/releases

Cloning the repository can be done with the following command:

    git clone --recurse-submodules https://github.com/SuperCollider/SuperCollider.git

The `--recurse-submodules` option will clone the repository's submodules which are needed to build SuperCollider. The submodules can also be obtained by navigating to the root of your locally cloned SuperCollider repository and running the following command:

    git submodule update --init --recursive

### Step 2: Make a build directory

First, `cd` into the root of the SuperCollider source directory (where this file resides).

Create a build directory and `cd` into it:

    mkdir build
    cd build

You can actually name this whatever you want, allowing you to have multiple independent build directories. If your SuperCollider source is also a git repository, the `.gitignore` file is configured to ignore files of the form `build*`.

### Step 3: Set CMake flags

Depending on what SuperCollider components you wish to install, you can set CMake flags. You can set CMake flags on the command line using `cmake -DKEY=value ..`. You can also use cmake frontends like ccmake or cmake-gui, or simply edit the `CMakeCache.txt` file. CMake flags are persistent and you only need to run these commands once each.

We will cover a few important settings. There are others, which you can view with `cmake -LH ..`. We will document more of them in this README file soon.

#### Nonstandard Qt locations

If you are installing sclang with GUI features and the IDE, and you installed Qt using the official Qt installer or the Trusty/Xenial PPAs, you will need to tell SuperCollider where Qt is. To do so:

    cmake -DCMAKE_PREFIX_PATH=/path/to/qt5 ..

The location of `/path/to/qt5` will depend on how you installed Qt:

- If you downloaded Qt from the Qt website, the path is two directories down from the top-level unpacked Qt directory, in a folder called `gcc`: `Qt/5.11.0/gcc_64/` (64-bit Linux) or `Qt/5.11.0/gcc/` (32-bit). By default, the Qt installer places `Qt/` in your home directory.
- If you used the Trusty/Xenial PPA's described above, the path is `/opt/qt511` or `/opt/qt510` (depending on which version you installed).

If you want to build without Qt entirely, run

    cmake -DSC_QT=OFF ..

#### Compiler optimizations

If you're building SC for production use and/or don't plan on using a debugger, make sure to build in release mode:

    cmake -DCMAKE_BUILD_TYPE=Release ..

This sets the compiler to the best optimization settings. Switch back to the defaults using `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`.

If you're compiling SC only for use on your own machine (that is, you aren't cross-compiling or packaging SC for distribution), it is recommended to turn on the `NATIVE` flag to enable CPU-specific optimizations:

    cmake -DNATIVE=ON ..

#### Install location

By default, SuperCollider installs in `/usr/local`, a system-wide install. Maybe you can't or don't want to use superuser privileges, or just want to install for a single user. To do so, set `CMAKE_INSTALL_PREFIX` to the desired installation directory. One good place to put it would be `$HOME/usr/local`:

    cmake -DCMAKE_INSTALL_PREFIX=~/usr/local ..

Make sure `~/usr/local/bin` is in your `PATH` if you do this. You can do that by adding a line such as `PATH=$PATH:$HOME/usr/local/bin` to `~/.profile`.

#### Speeding up repeated builds

If you are developing SC or you're constantly pulling in the latest changes, rebuilding SC repeatedly can be a drag. Installing `ccache` can speed up re-compilation. Here is how to configure cmake to use it:

    cmake -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++ -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc ..

This assumes your ccache executables are installed into `/usr/lib/ccache` - you may need to change the path to reflect your installation.

#### Library suffix

In some situations, it is preferable to install libraries and plugins
not in the `lib` directory but in a suffixed one, e.g. `lib64`.
In such a case you can set the cmake variable `LIB_SUFFIX`.
For example if you wish to install into `lib64`:

    cmake -DLIB_SUFFIX=64 ..

### Step 4: Build

If CMake ran successfully without errors, you are ready to move on to building. You can freely alternate between building and setting CMake flags.

After setting your CMake flags, just run

    make

The `-j` option allows multiple jobs to be run simultaneously, which can improve compile times on machines with multiple cores. The optimal `-j` setting varies between machines, but a good rule of thumb is the number of cores plus one. For example, on a 4-core system, try `make -j5`.

And to install, run

    make install

You will need to use `sudo make install` if you are doing a system-wide installation, which is the default.

After installing for the first time, please run

    sudo ldconfig

To uninstall:

    make uninstall

(or `sudo make uninstall`).

Building a Debian package
-------------------------

The most up-to-date Debian packaging rules are maintained by the
Debian Multimedia team. Repository (with debian/ folder):

https://salsa.debian.org/multimedia-team/supercollider


Running scsynth or supernova (standalone)
----------------------------

Run `scsynth --help`  or `supernova --help` to get an option summary. Don't forget to
start jackd before starting the server. If you want to add
directories to supercollider's search path or assign default jack
ports, set up your environment as described below.

You can specify the number of jack input/output channels created with
the options `-i` and `-o`, respectively.

the `-H` option can be used to specify a jack server to connect to and
to set the jack client identifier. The format is either

`<SERVER-NAME>:<CLIENT-NAME>`

or just

`<CLIENT-NAME>`

when connecting to the default server.


Running sclang
--------------

Supercollider comes with its own powerful IDE. Run it with:

```
$> scide
```

Alternatively you can use sclang in combination with your preferred text
editor out of emacs/vim/gedit. See the `README.md` files in `editors/*` for
installation and usage. Another alternative is to simply run the
`sclang` executable which will provide a readline-based interface.

`sclang` executes the startup file `~/.config/SuperCollider/startup.scd` after class library
initialization. This file can contain statements to set up your
supercollider environment, like setting default variables. An example can
be found in `linux/examples/sclang.sc`.

You _have_ to have a directory `~/.local/share/SuperCollider`. This is where
a synthdefs directory is automatically created. It is also the place
to put Extensions to the class library, in a folder called Extensions.

The runtime directory is either the current working directory or the
path specified with the `-d` option.

#### Headless operation

Even though the standard distribution of SuperCollider is built with the Qt framework, `sclang` can still be run in terminal without the X server. In order to do that, the `QT_QPA_PLATFORM` environment variable needs to be set to `offscreen`:
```shell
$> export QT_QPA_PLATFORM=offscreen
$> sclang
```

Environment
-----------

The jack audio driver interface is configured based on various
environment variables:

 * SC_JACK_DEFAULT_INPUTS comma-separated list of jack ports that the server's inputs should connect by default

   ```
   $> export SC_JACK_DEFAULT_INPUTS="system:capture_1,system:capture_2"
   ```

   in order to connect the first ports of one jack client, it is possible to specify only the client name

   ```
   $> export SC_JACK_DEFAULT_INPUTS="system"
   ```

 * SC_JACK_DEFAULT_OUTPUTS comma-separated list of jack ports that the server's outputs should be connected to by default.

   ```
   $> export SC_JACK_DEFAULT_OUTPUTS="system:playback_1,system:playback_2"
   ```

   In order to connect the first ports of one jack client, it is possible to specify only the client name

   ```
   $> export SC_JACK_DEFAULT_OUTPUTS="system"
   ```

Two additional environment variables substitute directories for the default
search path for plugins and synth definitions, respectively. Directory
names are separated by ':' as in the Unix PATH variable:

 * SC_PLUGIN_PATH, SC_SYNTHDEF_PATH

   ```
   $> export SC_SYNTHDEF_PATH="./synthdefs:/home/sk/SuperCollider/synthdefs"
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
