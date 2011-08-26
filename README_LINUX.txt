-*-text-*---------------------------------------------------------------
supercollider 3 for linux
------------------------------------------------------------------------

------------------------------------------------------------------------
intro
------------------------------------------------------------------------

SuperCollider is a synthesis engine (scsynth) and programming language
(sclang), originally Mac-based but now very widely used on Linux
(since Stefan Kersten ported the code in 2003). SuperCollider is free
software under the GPL - its main homepage is at

   http://supercollider.sourceforge.net

to get further information on supercollider usage or development, you
might consider subscribing to the mailing lists

  http://www.beast.bham.ac.uk/research/sc_mailing_lists.shtml

------------------------------------------------------------------------
build requirements
(most of these will be available in your linux distribution as packages )
------------------------------------------------------------------------

 * gcc >= 3.0
   http://www.gnu.org/software/gcc/
   gcc versions < 3.0 are missing some required c++ features

 * jack and libjack
   http://jackit.sourceforge.net/
   jack audio connection kit

 * libsndfile >= 1.0
   http://www.mega-nerd.com/libsndfile/
   _the_ soundfile i/o library

 * pkg-config >= 0.14.0
   http://www.freedesktop.org/software/pkgconfig/
   facilitates checking for installed packages when compiling from
   source

 * cmake >= 2.8
   http://www.cmake.org/
   cross-platform build system

 * fftw >= 3.0
   http://www.fftw.org/
   fast FFT transform library (for frequency-domain analysis,
                                        phase-vocoder effects)

 * libxt
   http://www.X.org/
   X toolkit intrinsics

 * libicu
   http://www.icu-project.org/
   unicode libraries

------------------------------------------------------------------------
build requirements (optional features)
(most of these will be available in your linux distribution as packages )
------------------------------------------------------------------------

 * qt >= 4.5 (+ qtwebkit)
   http://qt.nokia.com/
   cross-platform graphical user interface library, for sclang's GUI
   classes

 * alsa
   http://www.alsa-project.org/
   advanced linux sound architecture drivers and library, for sclang's
   MIDI interface

 * libreadline >= 5
   http://savannah.gnu.org/projects/readline
   provides convenient CLI interface for sclang

 * libhowl (obsolete)
   http://www.porchdogsoft.com/products/howl/
   zeroconf service dicovery implementation

 * libavahi-client
   http://www.avahi.org/
   a more powerful zeroconf service discovery implementation

 * linux kernel >= 2.6
   http://www.kernel.org/
   for sclang's linux input device (LID) interface

 * for scel: the Emacs interface see the README in the directory
   `editors/scel'

 * for sced: the gedit interface see the README in the directory
   `editors/sced'

 * for scvim: the vim interface see the README in the directory
   `editors/scvim'


------------------------------------------------------------------------
build requirements (debian users)
------------------------------------------------------------------------

on debian (unstable) you can install the following packages and be set
for building supercollider:

   build-essential
   libqt4-dev
   libqtwebkit-dev
   libjack-dev
   libsndfile1-dev
   libasound2-dev
   libavahi-client-dev
   libicu-dev
   libreadline6-dev
   libfftw3-dev
   libxt-dev
   pkg-config
   cmake
   cmake-modules

------------------------------------------------------------------------
building
------------------------------------------------------------------------

to build supercollider with cmake, it is suggested to do out-of-tree
builds in a specific build directory:

mkdir build
cd build
cmake ..

to run the build process run:
make

the build process can be configured using the cmake program, cmake
frontends like ccmake or cmake-gui, or by simply editing the
build/CMakeCache.txt file.

for example to enable a release build run the following in your build
directory:
cmake -DCMAKE_BUILD_TYPE=Release ..

to install the whole program, run:
make install

to uninstall:
make uninstall

Qt GUI:
-------

By default the experimental Qt GUI support will be built into sclang.
If you want to build without it configure cmake like this:

        cmake -DSC_QT=OFF ..

------------------------------------------------------------------------
Building a Debian package
------------------------------------------------------------------------

In our SVN we have debian packaging rules. These were designed on Ubuntu
but we believe they should work on most common debian systems - please
let us know if you have any issues with this way of building:

  cd SuperCollider3    # (or wherever your SC source is)
  svn co https://supercollider.svn.sourceforge.net/svnroot/supercollider/packages/ubuntu/ debian
  debuild

Eventually, some .deb packages will be put in the parent folder, and can
then be installed just like any other Debian package. For example:

  dpkg -i ../supercollider*.deb

(Note: the packages supercollider-scel*.deb, supercollider-scvim*.deb,
supercollider-sced*.deb, represent the different editor integrations:
emacs, vim, gedit. You might not want to install all of those, but
choose your preferred editor.)


------------------------------------------------------------------------
running scsynth (standalone)
------------------------------------------------------------------------

run scsynth without options to get an option summary. don't forget to
start jackd before trying to use scsynth. if you want to add
directories to supercollider's search path or assign default jack
ports, set up your environment as described below.

you can specify the number of jack input/output channels created with
the options -i and -o, respectively.

the -H option can be used to specify a jack server to connect to and
to set the jack client identifier. the format is either

    <SERVER-NAME>:<CLIENT-NAME>

or just

    <CLIENT-NAME>

when connecting to the default server.

------------------------------------------------------------------------
running sclang
------------------------------------------------------------------------

it is recommended to use sclang in combination with your preferred text
editor out of emacs/vim/gedit. see the README files in `linux/*' for
installation and usage. as an alternative you can simply run the
`sclang' executable which will provide a readline-based interface.

sclang executes the startup file `~/.config/SuperCollider/startup.scd' after class library
initialization. this file can contain statements to set up your
supercollider environment, like setting default variables. an example can
be found in `linux/examples/sclang.sc'.

you _have_ to have a directory `~/.local/share/SuperCollider/'. This is where
automatically a synthdefs directory is created. It is also the place
to put Extensions to the class library, in a folder called Extensions.

the runtime directory is either the current working directory or the
path specified with the `-d' option.

for advanced setups, sclang's compilation search path can be
customized with a library configuration file. an example is provided
in `linux/examples/sclang.cfg'; install it as `/etc/sclang.cfg' or
`~/.config/SuperCollider/sclang.cfg'. This config file is only needed when you want to
_exclude_ directories from the class library. Otherwise (so in most cases)
it is not needed.

------------------------------------------------------------------------
environment
------------------------------------------------------------------------

the jack audio driver interface is configured based on various
environment variables:

 * SC_JACK_DEFAULT_INPUTS comma separated list of jack ports that
   scsynth's inputs should connect to by default

   $ export SC_JACK_DEFAULT_INPUTS="system:capture_1,system:capture_2"

   in order to connect the first ports of one jack client, it is possible
   to specify only the client name

   $ export SC_JACK_DEFAULT_INPUTS="system"

 * SC_JACK_DEFAULT_OUTPUTS comma separated list of jack ports that
   scsynth's outputs should be connected to by default.

   $ export SC_JACK_DEFAULT_OUTPUTS="system:playback_1,system:playback_2"

   in order to connect the first ports of one jack client, it is possible
   to specify only the client name

   $ export SC_JACK_DEFAULT_OUTPUTS="system"

two additional environment variables substitute directories for the default
search path for plugins and synth definitions, respectively. directory
names are separated by ':' as in the unix PATH variable:

 * SC_PLUGIN_PATH, SC_SYNTHDEF_PATH

   $ export SC_SYNTHDEF_PATH="./synthdefs:/home/sk/SuperCollider/synthdefs"

------------------------------------------------------------------------
documentation
------------------------------------------------------------------------

apart from the sites listed above, some more documentation links ...

a wiki for supercollider, set up by julian rohrhuber, is at

    http://swiki.hfbk-hamburg.de/MusicTechnology/6

linux specific information can be found at

    http://swiki.hfbk-hamburg.de/MusicTechnology/478

------------------------------------------------------------------------
reporting bugs
------------------------------------------------------------------------

please report bugs either to the sc-users or sc-dev mailing lists.

------------------------------------------------------------------------
contributors to this document
------------------------------------------------------------------------

stefan kersten <sk AT k-hornz DOT de>
andi pieper
maurizio umberto puxeddu
rohan drape
mario lang
john yates
nescivi (marije baalman)
dan stowell
tim blechmann

------------------------------------------------------------------------
outro
------------------------------------------------------------------------

thanks to james mccartney, for making this great piece of audio
software publically and freely available.

------------------------------------------------------------------------
eof
------------------------------------------------------------------------

