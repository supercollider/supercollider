Building SuperCollider on Linux
===============================

Build requirements
------------------

All of these are required for building scsynth and supernova.

- [gcc][gcc] >= 4.8
- [cmake][cmake] >= 3.5: Cross-platform build system.
- [libjack][libjack]: Development headers for the JACK Audio Connection Kit.
- [libsndfile][libsndfile] >= 1.0: Soundfile I/O.
- [fftw][fftw] >= 3.0: FFT library.
- [libxt][libxt]: X toolkit intrinsics.

[gcc]: http://www.gnu.org/software/gcc
[libjack]: http://www.jackaudio.org/
[cmake]: http://www.cmake.org
[libsndfile]: http://www.mega-nerd.com/libsndfile
[fftw]: http://www.fftw.org
[libxt]: http://www.X.org

Recommended packages
--------------------

For scsynth and supernova:

- [libavahi-client][libavahi-client]

[libavahi-client]: http://www.avahi.org

For sclang and scide:

- [Qt][Qt] >= 5.7: Cross-platform GUI library, required for the IDE and for sclang's Qt GUI kit. It's best to get the latest Qt 5.x version.
- QtWebEngine: required for the IDE.
- [git][git]: Required for sclang's Quarks system.
- [ALSA][ALSA]: Linux sound library, required for sclang MIDI support.
- [libudev][libudev]: Device manager library, required for HID support.
- [Linux kernel][Linux kernel] >= 2.6: Required for LID support.
- [libreadline][libreadline] >= 5: Required for sclang's CLI interface.

[Qt]: http://qt-project.org
[ALSA]: http://www.alsa-project.org
[libudev]: http://www.freedesktop.org/software/systemd/man/libudev.html
[libreadline]: http://savannah.gnu.org/projects/readline
[Linux kernel]: http://www.kernel.org
[git]: https://git-scm.com/

Installing requirements on Debian
---------------------------------

Building on embedded Linux platforms such as the Raspberry Pi or Beaglebone Black can be found at [a dedicated web page](http://supercollider.github.io/development/building). (Those instructions will be merged here soon.)

On Debian-like systems, the following command installs the minimal recommended dependencies for compiling scsynth and supernova:

    sudo apt-get install build-essential cmake libjack-dev libjack-jackd2-dev libsndfile1-dev libfftw3-dev libxt-dev libavahi-client-dev

The following command installs all the recommended dependencies for sclang except for Qt:

    sudo apt-get install git libasound2-dev libicu-dev libreadline6-dev libudev-dev pkg-config

Qt is trickier to install. As of June 2018, the version made available in the Debian repositories is too old for SuperCollider — 5.7 or later is required to build the IDE and sclang's Qt GUI system.

The [Qt official website](https://www.qt.io/) can be used to get Qt versions.

More conveniently, if you are on Ubuntu 14.04 (Trusty), 16.04 (Xenial), or 18.04 (Bionic), [Stephan Binner's Launchpad PPAs][Stephan Binner's Launchpad PPAs] allow for simple installation of new Qt versions.

On Xenial or Bionic:

    sudo apt-add-repository ppa:beineri/opt-qt511-`lsb_release -sc`
    sudo apt-get update
    sudo apt-get install qt511base qt511location qt511declarative qt511tools qt511webchannel qt511xmlpatterns qt511svg qt511webengine

(The `lsb_release -sc` command is there to substitute `xenial` or `bionic`.)

On Trusty, only Qt 5.10 and below are available:

    sudo apt-add-repository ppa:beineri/opt-qt510-trusty
    sudo apt-get update
    sudo apt-get install qt510base qt510location qt510declarative qt510tools qt510webchannel qt510xmlpatterns qt510svg qt510webengine

[Stephan Binner's Launchpad PPAs]: https://launchpad.net/~beineri

Building
--------

### Step 1: Make a build directory

First, `cd` into the root of the SuperCollider source directory (where this file resides).

Create a build directory and `cd` into it:

    mkdir build
    cd build

You can actually name this whatever you want, allowing you to have multiple independent build directories. If your SuperCollider source is also a git repository, the `.gitignore` file is configured to ignore files of the form `build*`.

### Step 2: Set CMake flags

Depending on what SuperCollider components you wish to install, you can set CMake flags. You can set CMake flags on the command line using `cmake -DKEY=value ..`. You can also use cmake frontends like ccmake or cmake-gui, or simply edit the `CMakeCache.txt` file. CMake flags are persistent and you only need to run these commands once each.

We will cover a few important settings. There are others, which you can view with `cmake -LH ..`. We will document more of them in this README file soon.

#### Qt

If you are installing sclang with Qt and IDE, it is required to tell SuperCollider where Qt is. To do so:

    cmake -DCMAKE_PREFIX_PATH=/path/to/qt5 ..

The location of `/path/to/qt5` will depend on how you installed Qt:

- If you used the Launchpad PPA's described in the above section, the path is `/opt/qt511` or `/opt/qt510` (depending on which version you installed).
- If you downloaded Qt from the Qt website, the path is two directories down from the top-level unpacked Qt directory: `Qt5.x.x/5.x/gcc/` (32-bit) or `Qt5.x.x/5.x/gcc_64/` (64-bit).
- If you used your Linux distribution's repositories, it will be `/usr/lib/i386-linux-gnu/` (32-bit) or `/usr/lib/x86_64-linux-gnu/` (64-bit).

If you want to build without Qt entirely, run

    cmake -DSC_QT=OFF ..

Note: running headless SC in an X-less environment requires
jackd without D-bus support. On Raspbian Jessie this requires
compiling jackd rather than using the packaged version.
Also, note that you will get errors on sclang startup from
classes requiring Qt. A workaround and more details are
described in:

    http://supercollider.github.io/development/building-raspberrypi

#### Release build

If you're building SC for production use, make sure to build in release mode:

    cmake -DCMAKE_BUILD_TYPE=Release ..

#### Install location

By default, SuperCollider installs in `/usr/local`, a system-wide install. Maybe you can't or don't want to use superuser privileges, or just want to install for a single user. To do so, set `CMAKE_PREFIX_PATH`:

    cmake -DCMAKE_PREFIX_PATH=~/usr/local ..

Make sure `~/usr/local/bin` is in your `PATH` if you do this.

#### Library suffix

In some situations, it is preferable to install libraries and plugins
not in the `lib` directory but in a suffixed one, e.g. `lib64`.
In such a case you can set the cmake variable `LIB_SUFFIX`.
For example if you wish to install into `lib64`:

    cmake -DLIB_SUFFIX=64 ..

### Step 3: Build

After setting your CMake flags, just run

    make

And to install, run

    make install

You will need to use `sudo make install` if you are doing a system-wide installation, which is the default.

After installing for the first time, please run

    sudo ldconfig

To uninstall:

    make uninstall

(or `sudo make uninstall`).

### Speeding up repeated builds

If you build SuperCollider repeatedly, we recommend installing `ccache`
which can speed up re-compilation. Here is how to configure cmake to use it:

    cmake -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++ -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc ..

This assumes your ccache executables are installed into `/usr/lib/ccache` - you may need to change the path to reflect your installation.


Building a Debian package
-------------------------

The most up-to-date Debian packaging rules are maintained by the
Debian Multimedia team. Repository (with debian/ folder):

http://anonscm.debian.org/gitweb/?p=pkg-multimedia/supercollider.git;a=summary


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
