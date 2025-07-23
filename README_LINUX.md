# Building SuperCollider on Linux

## Build requirements

These are strict requirements for scsynth and supernova:

- A C++ compiler with C++17 support such as [gcc](http://www.gnu.org/software/gcc) or [clang](https://clang.llvm.org)
- [cmake](http://www.cmake.org) >= 3.12: Cross-platform build system.
- [libsndfile](http://www.mega-nerd.com/libsndfile) >= 1.0: Soundfile I/O.
- [libjack](http://www.jackaudio.org/): Development headers for the JACK Audio Connection Kit.
- [fftw](http://www.fftw.org) >= 3.0: FFT library.

These packages are required by default for scsynth and supernova, but the components that require them can be disabled with flags:

- [libxt](http://www.X.org): X toolkit intrinsics, required for UGens such as `MouseX`.
  To build the servers without X, use the `NO_X11=ON` CMake flag.
- [libavahi-client](http://www.avahi.org): For zero-configuration networking.
  To build the servers without Avahi, use the `NO_AVAHI=ON` CMake flag.

### Recommended packages

For sclang and scide:

- [Qt](http://qt-project.org) >= 6.2 with QtWebEngine and QtWebSockets: Cross-platform GUI library, required for the IDE and for sclang's Qt GUI kit.
- [git](https://git-scm.com/): Required for sclang's Quarks system.
- [ALSA](http://www.alsa-project.org): Linux sound library, required for sclang MIDI support.
- [libudev](http://www.freedesktop.org/software/systemd/man/libudev.html): Device manager library, required for HID support.
- [Linux kernel](http://www.kernel.org) >= 2.6: Required for LID support.
- [libreadline](http://savannah.gnu.org/projects/readline) >= 5: Required for sclang's CLI interface.
- [ncurses](https://invisible-island.net/ncurses/): Required for sclang's CLI interface.

### Installing requirements on Debian

There are dedicated READMEs in this repository for building on particular embedded Linux platforms:

- Raspberry Pi: README_RASPBERRY_PI.md
- Bela: README_BELA.md

> Before installing the dependencies, it is recommended to update the package list on your system via
>
> ```shell
> sudo apt-get update
> ```
>
> It is also recommended to update the already installed packages on your system using the following command:
>
> ```shell
> sudo apt-get upgrade
> ```

On Debian-like systems (e.g: Ubuntu, Linux Mint, Kali Linux, Elementary OS, Knoppix, Corel Linux, Lindows, ...), the following commands can be executed step by step to install all necessary dependencies and build SuperCollider.

The following dependencies are recommended for compiling scsynth and supernova:

```shell
sudo apt-get install \
  build-essential \
  cmake \
  libjack-jackd2-dev \
  libsndfile1-dev \
  libfftw3-dev \
  libxt-dev \
  libavahi-client-dev \
  libudev-dev
```

If you need to use JACK1 replace `libjack-jackd2-dev` with `libjack-dev`.

The following command will install all recommended dependencies for sclang except Qt

```shell
sudo apt-get install \
  git \
  libasound2-dev \
  libicu-dev \
  libreadline6-dev \
  libudev-dev \
  pkg-config \
  libncurses5-dev \
  jackd2
```

#### Installing Qt

[Qt](https://www.qt.io/) 6.2 (or later) provides the GUI for SuperCollider like its IDE and allows to build custom user GUIs via *QtCollider* and is the default and recommended way to use and set up SuperCollider.
For systems without display capabilities, it is also possible to build SuperCollider without Qt (see configure build section).

Try this command to query the Qt6 version available to you:

```shell
apt-cache policy qt6-base-dev
```

If this displays version 6.2 or later, you can install Qt6 using `apt-get`.
Please note that this list of packages might not be complete, as it hasn't been revised for Qt6.

```shell
sudo apt-get install \
  qt6-base-dev \
  qt6-base-dev-tools \
  qt6-tools-dev \
  qt6-tools-dev-tools \
  libqt6websockets6-dev \
  libqt6webenginecore6 \
  qt6-webengine-dev \
  qt6-webengine-dev-tools \
  libqt6svgwidgets6
```

If you are on Ubuntu, check the sections below. If these instructions don't work, you will have to use the official Qt installer (see below).

##### Installing Qt on Ubuntu 22.04, 24.04

On 22.04 and 24.04 Qt6 is available in the system's package manager.
The following should install the correct packages:

```shell
sudo apt-get install \
  qt6-base-dev \
  qt6-base-dev-tools \
  qt6-tools-dev qt6-tools-dev-tools \
  qt6-declarative-dev \
  libqt6gui6 \
  libqt6printsupport6 \
  libqt6svgwidgets6 \
  libqt6websockets6-dev \
  libqt6webenginecore6 \
  libqt6webenginecore6-bin \
  qt6-webengine-dev \
  qt6-webengine-dev-tools \
  libqt6webchannel6-dev \
  libqt6opengl6-dev \
  libqt6svg6-dev \
  linguist-qt6 \
  qt6-l10n-tools \
  libglx-dev libgl1-mesa-dev \
  libvulkan-dev \
  libxkbcommon-dev \
  libxcb-xkb-dev
```

### Installing requirements on Fedora

The following commands should install all the recommended SuperCollider dependencies on Fedora, except for Qt:

```shell
sudo dnf groupinstall "Development Tools"
sudo dnf install \
  cmake \
  libsndfile-devel \
  wayland-devel \
  xorg-x11-server-Xwayland-devel \
  pipewire-devel \
  pipewire-jack-audio-connection-kit-devel \
  systemd-devel \
  fftw-devel \
  alsa-lib-devel \
  libatomic
sudo dnf install libXt-devel 
```

#### Installing Qt on Fedora

Please note, this list has not been tested after upgrading to Qt6.
Additional packages may be needed.

```shell
sudo dnf install \
  qt6-qtbase-devel \
  qt6-qtsvg-devel \
  qt6-qtwebengine-devel \
  qt6-linguist \
  qt6-qtwebsockets-devel\
  qt6-qttools-devel
```

### Installing Qt using the official installer

Worst case scenario, you can grab Qt off the [Qt official website](https://www.qt.io/).
It's best to get the latest version. Click "Download," select the open source license, and download the Qt installer. The Qt installer has a step that prompts for you to log in to a Qt Account, but you don't actually need to authenticate and you can safely click "Skip" at that step.

At the "Select Components" step, pop open Qt, select the latest version, and check the "Desktop" option.
If you are building the IDE, also select "QWebEngine."

Unfortunately, the Qt installer does not allow you to deselect the multi-gigabyte QtCreator download.

### Using Clang

SuperCollider can be compiled with Clang.
By default on Linux clang will use libstdc++ instead of libc++ (`-DSC_CLANG_USES_LIBSTDCPP=ON`).
You can set this option to off to use libc++ instead; this will however likely cause issues when linking with Qt6.

## Building

### Obtain the source code

SuperCollider is hosted on Github at <https://github.com/SuperCollider/SuperCollider>.
Obtaining the SuperCollider source code can be done either by downloading a release tarball, or by cloning the repository.

The SuperCollider release tarballes are available to download via <https://github.com/supercollider/supercollider/releases>.

Cloning the repository via `git` can be done with the following command:

```shell
git clone --recurse-submodules https://github.com/SuperCollider/SuperCollider.git
```

> This command will obtain the source code of the development version of SuperCollider.
> In order to checkout a specific release (or commit) use the tag name or the commit ID and pass it to the [`clone` command](https://www.atlassian.com/git/tutorials/setting-up-a-repository/git-clone) via
>
> ```shell
> git clone --branch 3.13.1 --recurse-submodules https://github.com/SuperCollider/SuperCollider.git
> ```
>
> Use [`git checkout`](https://www.atlassian.com/git/tutorials/using-branches/git-checkout) to switch to another version/branch of SuperCollider after cloning.

The `--recurse-submodules` option will clone the repository's submodules which are needed to build SuperCollider. The submodules can also be obtained by navigating to the root of your locally cloned SuperCollider repository and running the following command:

```shell
git submodule update --init --recursive
```

> **Important**: Please do not download the repository via GitHub's _Download ZIP_ button as this will not contain all necessary submodules!

### Make a build directory

First, `cd` into the root of the SuperCollider source directory (where this file resides).

Create a build directory and `cd` into it:

```shell
mkdir build && cd build
```

You can actually name this whatever you want, allowing you to have multiple independent build directories.
If your SuperCollider source is also a git repository, the `.gitignore` file is configured to ignore files of the form `build*`.

### Configure the build via CMake

Depending on what SuperCollider components you wish to build and install, you can set CMake flags which will _configure_ the build accordingly and create the necessary files to build SuperCollider accordingly.

To let cmake configure the build with default settings run the following command in the `build` directory

```shell
cmake ..
```

You can set CMake build flags via the command line using `cmake -DKEY=value ..` during the configure process where the `D` prefix is necessary!

> **Example:** It is possible to build an optional [Emacs](https://www.gnu.org/software/emacs/) plugin for SuperCollider.
> As of 3.14, this plugin is not built in by default and must be enabled manually.
> To tell CMake if this plugin should also be built as part of the build process, the flag `SC_EL` can be used - i.e. `-DSC_EL=YES`:
>
> ```shell
> cmake -DSC_EL=YES ..
> ```
>
> To display all available build flags and their values run `cmake -LH ..` in the `build` directory or use cmake gui-frontends like [`ccmake`](https://cmake.org/cmake/help/latest/manual/ccmake.1.html) or [`cmake-gui`](https://cmake.org/cmake/help/latest/manual/cmake-gui.1.html) to inspect and set the available flags.
>
> It is also possible to edit the `CMakeCache.txt` file or re-execute the `cmake` command with the desired flags.
> CMake flags are persistent and you only need to run these commands once each.

#### Nonstandard Qt locations

If you are installing sclang with GUI features and the IDE, and you installed Qt using the official Qt installer, you will need to tell SuperCollider where Qt is.
To do so it is necessary to specify the Qt path via the build variable `DCMAKE_PREFIX_PATH` during the configure process like so:

```shell
cmake -DCMAKE_PREFIX_PATH=/path/to/qt6 ..
```

The location of `/path/to/qt6` will depend on how you installed Qt.
If you downloaded Qt from the Qt website, the path is two directories down from the top-level unpacked Qt directory, in a folder called `gcc`: `Qt/6.2.0/gcc_64/` on 64 bit systems.
By default, the Qt installer places `Qt/` in your home directory.

If you want to build without Qt entirely, use

```shell
cmake -DSC_QT=OFF ..
```

#### Compiler optimizations

If you're building SC for production use and/or don't plan on using a debugger, make sure to build in release mode:

```shell
cmake -DCMAKE_BUILD_TYPE=Release ..
```

This sets the compiler to the best optimization settings.
Switch back to the defaults using `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`.

If you're compiling SC only for use on your own machine (that is, you aren't cross-compiling or packaging SC for distribution), it is recommended to turn on the `NATIVE` flag to enable CPU-specific optimizations:

```shell
cmake -DNATIVE=ON ..
```

#### Install location

By default, SuperCollider installs in `/usr/local`, a system-wide install.
Maybe you can't or don't want to use superuser privileges, or just want to install for a single user.
To do so, set `CMAKE_INSTALL_PREFIX` to the desired installation directory.
One good place to put it would be `$HOME/usr/local`:

```shell
cmake -DCMAKE_INSTALL_PREFIX=~/usr/local ..
```

Make sure `~/usr/local/bin` is in your `PATH` if you do this.
You can do that by adding a line such as `PATH=$PATH:$HOME/usr/local/bin` to `~/.profile`.
For users who are not accustomed to editing `~/.profile` via the terminal, the file can be opened using a GUI-based application. 
To do this, enable the display of hidden files in the system’s default file manager, such as Files (GNOME), Dolphin (KDE), or Thunar (XFCE).
After building SuperCollider on your machine, if you launch scide inside a terminal window, use the following command to apply modifications made to `~/.profile` without logging out or rebooting the system:
```shell
source ~/.profile
```
However, if you launch scide from the Files application, logging out of your user account and then logging back in will ensure that `~/.profile` is reloaded and its changes take effect.

#### Speeding up repeated builds

If you are developing SC or you're constantly pulling in the latest changes, rebuilding SC repeatedly can be a drag.
Installing `ccache` can speed up re-compilation.
Ccache will be used automatically if it's in the PATH. 
Use can disable using ccache by setting `-D USE_CCACHE=OFF`.


This assumes your ccache executables are installed into `/usr/lib/ccache` - you may need to change the path to reflect your installation.

#### Library suffix

In some situations, it is preferable to install libraries and plugins
not in the `lib` directory but in a suffixed one, e.g. `lib64`.
In such a case you can set the cmake variable `LIB_SUFFIX`.
For example if you wish to install into `lib64`:

```shell
cmake -DLIB_SUFFIX=64 ..
```

### Build

If CMake ran successfully without errors, you are ready to move on to building.
You can freely alternate between building and setting CMake flags.

After setting your CMake flags, just run

```shell
make
```

The `-j` option allows multiple jobs to be run simultaneously, which can improve compile times on machines with multiple cores.
The optimal `-j` setting varies between machines, but a good rule of thumb is the number of cores plus one.
For example, on a 4-core system, try `make -j5`.

And to install, run

```shell
sudo make install
```

You will need to use `make install` if you are doing a user-wide installation, which is not the default.

After installing for the first time, please run

```shell
sudo ldconfig
```

To uninstall:

```shell
sudo make uninstall
```

(or `make uninstall` if you did user-wide installation).

### Building a Debian package

The most up-to-date Debian packaging rules are maintained by the
Debian Multimedia team. Repository (with debian/ folder):

<https://salsa.debian.org/multimedia-team/supercollider>

## Running scsynth or supernova (standalone)

Run `scsynth --help`  or `supernova --help` to get an option summary.
Don't forget to start jackd before starting the server.
If you want to add directories to supercollider's search path or assign default jack ports, set up your environment as described below.

You can specify the number of jack input/output channels created with the options `-i` and `-o`, respectively.

The `-H` option can be used to specify a jack server to connect to and to set the jack client identifier.
The format is either `<SERVER-NAME>:<CLIENT-NAME>` or just `<CLIENT-NAME>` when connecting to the default server.

## Running sclang

Supercollider comes with its own powerful IDE.
Run it with:

```shell
scide
```

Alternatively you can use sclang in combination with your preferred text editor out of emacs/vim/gedit.
See the `README.md` files in `editors/*` for installation and usage.
Another alternative is to simply run the `sclang` executable which will provide a readline-based interface.

`sclang` executes the startup file `~/.config/SuperCollider/startup.scd` after class library initialization.
This file can contain statements to set up your supercollider environment, like setting default variables.
An example can be found in `linux/examples/sclang.sc`.

You _have_ to have a directory `~/.local/share/SuperCollider`. This is where a synthdefs directory is automatically created.
It is also the place to put Extensions to the class library, in a folder called Extensions.

The runtime directory is either the current working directory or the path specified with the `-d` option.

### Headless operation

Even though the standard distribution of SuperCollider is built with the Qt framework, `sclang` can still be run in terminal without the X server.
In order to do that, the `QT_QPA_PLATFORM` environment variable needs to be set to `offscreen`:

```shell
export QT_QPA_PLATFORM=offscreen
sclang
```

On platforms without any display server at all it may be necessary to build SuperCollider without `X11` and `Qt` respectively - refer to the build flags to disable those features while configuring the build.

## Environment variables

The jack audio driver interface is configured based on various
environment variables:

- `SC_JACK_DEFAULT_INPUTS` comma-separated list of jack ports that the server's inputs should connect by default

  ```shell
  export SC_JACK_DEFAULT_INPUTS="system:capture_1,system:capture_2"
  ```

  in order to connect the first ports of one jack client, it is possible to specify only the client name

  ```shell
  export SC_JACK_DEFAULT_INPUTS="system"
  ```

- `SC_JACK_DEFAULT_OUTPUTS` comma-separated list of jack ports that the server's outputs should be connected to by default.

  ```shell
  export SC_JACK_DEFAULT_OUTPUTS="system:playback_1,system:playback_2"
  ```

  In order to connect the first ports of one jack client, it is possible to specify only the client name

  ```shell
  export SC_JACK_DEFAULT_OUTPUTS="system"
  ```

- Two additional environment variables substitute directories for the default search path for plugins and synth definitions, respectively.
  Directory names are separated by ':' as in the Unix PATH variable: `SC_PLUGIN_PATH`, `SC_SYNTHDEF_PATH`

  ```shell
  export SC_SYNTHDEF_PATH="./synthdefs:/home/sk/SuperCollider/synthdefs"
  ```

## Contributors to this document

- stefan kersten (sk AT k-hornz DOT de)
- andi pieper
- maurizio umberto puxeddu
- rohan drape
- mario lang
- john yates
- nescivi (marije baalman)
- dan stowell
- tim blechmann
- prko
