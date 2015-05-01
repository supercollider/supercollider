Supercollider 3 for linux
=========================

Intro
-----

SuperCollider is a synthesis engine (scsynth) and programming language
(sclang), originally Mac-based but now very widely used on Linux
(since Stefan Kersten ported the code in 2003). SuperCollider is free
software under the GPL - its main homepage is at

http://supercollider.github.com

To get further information on supercollider usage or development, you
might consider subscribing to the mailing lists

http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml


Build requirements
------------------

(most of these will be available in your linux distribution as packages )

 * gcc >= 4.6
   http://www.gnu.org/software/gcc

 * jack and libjack
   * http://jackit.sourceforge.net
   * jack audio connection kit

 * libsndfile >= 1.0
   * http://www.mega-nerd.com/libsndfile
   * _the_ soundfile i/o library

 * cmake >= 2.8.11
   * http://www.cmake.org
   * cross-platform build system

 * fftw >= 3.0
   * http://www.fftw.org
   * fast FFT transform library (for frequency-domain analysis, phase-vocoder effects)

 * libxt
   * http://www.X.org
   * X toolkit intrinsics

Build requirements (optional features)
--------------------------------------
(most of these will be available in your linux distribution as packages)

 * Qt >= 5.3 (+ qtwebkit)
   * http://qt-project.org
   * cross-platform graphical user interface library, for Qt IDE and sclang's Qt GUI kit
   * Qt >= 5.0 should work, but build-test is done against 5.3

 * alsa
   * http://www.alsa-project.org
   * advanced linux sound architecture drivers and library, for sclang's MIDI interface

 * libudev
   * http://www.freedesktop.org/software/systemd/libudev/
   * interaction with the device manager of linux (used for HID support)

 * libreadline >= 5
   * http://savannah.gnu.org/projects/readline
   * provides convenient CLI interface for sclang

 * libavahi-client
   * http://www.avahi.org
   * a more powerful zeroconf service discovery implementation

 * libcwiid
   * http://abstrakraft.org/cwiid
   * library for wiimote support

 * linux kernel >= 2.6
   * http://www.kernel.org
   * for sclang's linux input device (LID) interface

 * for `scel`: the **emacs** interface see [editors/scel/README.md](editors/scel/README.md)

 * for `sced`: the **gedit** interface see [editors/sced/README.md](editors/sced/README.md)

 * for `scvim`: the **vim** interface see [editors/scvim/README.md](editors/scvim/README.md)


Build requirements (debian users)
---------------------------------

On debian (unstable) you can install the following packages and be set
for building supercollider:

 - build-essential
 - libjack-dev or libjack-jackd2-dev
 - libsndfile1-dev
 - libasound2-dev
 - libavahi-client-dev
 - libicu-dev
 - libreadline6-dev
 - libfftw3-dev
 - libxt-dev
 - libudev-dev
 - libcwiid-dev (for wiimote support)
 - pkg-config
 - subversion (required by the Quarks class at run-time)
 - cmake (on some platforms, cmake >= 2.9 may require manual build)
 - qt5-default qt5-qmake qttools5-dev qttools5-dev-tools qtdeclarative5-dev libqt5webkit5-dev
 

Building
--------

 - to build supercollider with cmake, it is suggested to do out-of-tree
builds in a specific build directory:

   ```
   $> mkdir build
   $> cd build
   $> cmake -DCMAKE_PREFIX_PATH=/path/to/qt5 ..
   ```

 - to run the build process run:

   ```
   $> make
   ```

   The build process can be configured using the cmake program, cmake
   frontends like ccmake or cmake-gui, or by simply editing the
   `build/CMakeCache.txt` file.

   For example to enable a release build run the following in your build
   directory:

   ```
   $> cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

 - to install the whole program, run:

   ```
   $> make install
   ```

   For the above step you will probably need super-user privileges,
   e.g. using "sudo".

   Also, please run

   ```
   $> sudo ldconfig
   ```
   after installing for the first time.

 - to uninstall:

   ```
   $> make uninstall
   ```

### Qt GUI

By default the Qt GUI support will be built into sclang.
If you want to build without it configure cmake like this:

```
$> cmake -DSC_QT=OFF ..
```

### Speeding up repeated builds

If you build SuperCollider repeatedly, we recommend installing `ccache`
which can speed up re-compilation. Here is how to configure cmake to use it:

```
$> cmake -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++ -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc ..
```

This assumes your ccache executables are installed into `/usr/lib/ccache` - you may need to change the path to reflect your installation.

Building a Debian package
-------------------------

The most up-to-date debian packaging rules are maintained by the
Debian Multimedia team. Repository (with debian/ folder):

http://anonscm.debian.org/gitweb/?p=pkg-multimedia/supercollider.git;a=summary


Running scsynth (standalone)
----------------------------

Run `scsynth` without options to get an option summary. Don't forget to
start jackd before trying to use scsynth. If you want to add
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

 * SC_JACK_DEFAULT_INPUTS comma separated list of jack ports that scsynth's inputs should connect to by default

   ```
   $> export SC_JACK_DEFAULT_INPUTS="system:capture_1,system:capture_2"
   ```

   in order to connect the first ports of one jack client, it is possible to specify only the client name

   ```
   $> export SC_JACK_DEFAULT_INPUTS="system"
   ```

 * SC_JACK_DEFAULT_OUTPUTS comma separated list of jack ports that scsynth's outputs should be connected to by default.

   ```
   $> export SC_JACK_DEFAULT_OUTPUTS="system:playback_1,system:playback_2"
   ```

   In order to connect the first ports of one jack client, it is possible to specify only the client name

   ```
   $> export SC_JACK_DEFAULT_OUTPUTS="system"
   ```

Two additional environment variables substitute directories for the default
search path for plugins and synth definitions, respectively. Directory
names are separated by ':' as in the unix PATH variable:

 * SC_PLUGIN_PATH, SC_SYNTHDEF_PATH

   ```
   $> export SC_SYNTHDEF_PATH="./synthdefs:/home/sk/SuperCollider/synthdefs"
   ```


Reporting bugs
--------------

Please report bugs either to the sc-users or sc-dev mailing lists.


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


Outro
-----

Thanks to james mccartney, for making this great piece of audio software publically and freely available.
