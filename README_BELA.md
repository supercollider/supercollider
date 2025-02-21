Building SuperCollider for Bela
======================================

This README covers _building_ SuperCollider for Bela.
By doing so, you will be able to use the Bela audio driver for scsynth, to use Bela's ultra-low-latency audio thread *instead* of jack/portaudio, as well as plugins to access the analog and digital channels of the Bela-cape.
See [here](https://learn.bela.io/using-bela/languages/supercollider/) for general information about _using_ SuperCollider on Bela.

Build requirements
------------------

SuperCollider on Bela will build on Bela image 0.3.0 and above and it uses Bela's API 1.9 or above.
While it is possible to build the code on a Bela system ("native"), you are probably better off cross-compiling it from a more powerful machine.

### Prerequisites [cross-compiling only]

On a Linux machine:

- you need access to the files that are on Bela's filesystem. The easiest way to do this is to copy the whole filesystem from the board you want to build for, or mount that filesystem remotely with `sshfs`.
- download a cross compiler that matches the one on the board version you are targeting. For Bela Images v0.3.x, use [gcc-6.3.1](http://releases.linaro.org/components/toolchain/binaries/6.3-2017.05/arm-linux-gnueabihf/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf.tar.xz).
- create a file with the cross-toolchain details, e.g.: `~/Toolchain-arm-linux-gnueabihf.cmake` which contains something like this (edit the `SYSROOT` to point to the root of the Bela filesystem and `GCC_BASE` to point to the compiler you are going to use).

```
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_SYSTEM_PROCESSOR arm )
SET(CMAKE_LIBRARY_ARCHITECTURE arm-linux-gnueabihf)

# where is the target environment
SET(SYSROOT "$ENV{HOME}/bela-image-builder-stretch/rootfs/")

# RPATH - a list of directories which is linked into the executable,
# supported on most UNIX systems. It is ignored if RUNPATH is present.
set(FLAGS "${FLAGS} -Wl,-rpath-link,${SYSROOT}/lib/arm-linux-gnueabihf")
set(FLAGS "${FLAGS} -Wl,-rpath-link,${SYSROOT}/usr/lib/arm-linux-gnueabihf")
set(FLAGS "${FLAGS} -Wl,-rpath-link,${SYSROOT}/usr/local/lib")
set(RPATH_FLAGS ${FLAGS})

# specify the cross compiler
set(GCC_BASE $ENV{HOME}/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-)
SET(CMAKE_C_COMPILER ${GCC_BASE}gcc)
SET(CMAKE_CXX_COMPILER ${GCC_BASE}g++)
SET(CMAKE_ASM_COMPILER ${GCC_BASE}as)
SET(CMAKE_RANLIB ${CROSS_COMPILER}ranlib)
UNSET(CMAKE_C_FLAGS CACHE)
UNSET(CMAKE_CXX_FLAGS CACHE)

#link_libraries("-no-pie")
set(COMMON_FLAGS "-no-pie -fno-pie")
set(CMAKE_EXE_LINKER_FLAGS "-no-pie")
set(CMAKE_SHARED_LINKER_FLAGS "-no-pie")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} ${RPATH_FLAGS}" CACHE STRING "c++ flags" FORCE)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS} ${RPATH_FLAGS}" CACHE STRING "c flags" FORCE)

set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS}" CACHE INTERNAL "c link flags" FORCE)
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS}" CACHE INTERNAL "c++ link flags" FORCE)

SET(CMAKE_SYSROOT ${SYSROOT})
SET(CMAKE_FIND_ROOT_PATH ${SYSROOT})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER) # for xeno-config an bela-config we have to manually override this there
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
```

- create a couple of symlinks to make things easier:
```
ln -s ~/bela-image-builder-stretch/rootfs/usr/xenomai /usr/xenomai
ln -s ~/bela-image-builder-stretch/rootfs/lib/arm-linux-gnueabihf/ /lib/arm-linux-gnueabihf
```

### Prerequisites [native building only]

Building SuperCollider on the board using the on-boar compiler is slow, so we recommend you use `distcc` to make all your builds faster by off-loading the actual compilation to your host computer. You need to:
* install an arm-linux-gnueabihf capabale `gcc-6.3` cross-compiler on your host.
* then follow instructions here to setup a working distcc environment https://forum.bela.io/d/724-distcc-distributed-compilation-with-bela
* on the host, launch `distccd` with something like `distccd --verbose --no-detach --daemon --allow 192.168.7.2 --log-level error --log-file ~/distccd.log` (and then `tail ~/distccd.log` for errors)
* then on the board run the following before the `cmake` commands below:

```
export DISTCC_HOSTS="192.168.7.1"
export CC="distcc-clang" # or other as appropriate, see forum post above
export CXX="distcc-clang++" # or other as appropriate, see forum post above
```

NOTE: make sure you don't pass `-march=native` to the compiler when using `distcc`, or it will compile natively. Therefore, make sure you do NOT pass `-DNATIVE=ON` to `cmake`, as per below

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

**Note**: in order to run the commands above on Bela itself, you need to connect the device to the internet.
Alternatively, if you are going to build natively, but the board is not connected to the internet, you can run them on an internet-connected machine and `scp` the whole folder to Bela.

### Step 2: Make a build directory

First, `cd` into the root of the SuperCollider source directory (where this file resides).

Create a build directory and `cd` into it:

    mkdir build
    cd build

You can actually name this whatever you want, allowing you to have multiple independent build directories. If your SuperCollider source is also a git repository, the `.gitignore` file is configured to ignore files of the form `build*`.

### Step 3: Set CMake flags

Here we define the configuration of SuperCollider and we select which components we want to build.
SuperCollider runs headless on Bela (so no Sc IDE), there is no support for SuperNova and no need for QT, so we only need to build `sclang`, `scsynth` and the UGens.

#### Native 

Here's the command with `distcc` (it will infer the compilers from the `export ...` lines above (if you did set up `distcc`), or use the default compilers otherwise.

Note that if you set `CC=clang` and `CXX=clang++` on Bela image v0.3.x (which has `clang-3.9`), you should add:
```
-DSC_ALBETON_LINK=OFF -DSC_CLANG_USES_LIBSTDCPP=ON
```
as explained in `README_LINUX.md`.

```
cmake .. -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DNO_X11=ON -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF \
	-DSC_VIM=OFF -DSC_HIDAPI=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela \
		-DSC_ABLETON_LINK=ON -DCMAKE_INSTALL_PREFIX=/usr/
```

#### Cross-compiling

```
cmake .. -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DNO_X11=ON -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF \
	-DSC_VIM=OFF -DSC_HIDAPI=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela \
		-DSC_ABLETON_LINK=ON -DCMAKE_INSTALL_PREFIX=/usr/ -DCMAKE_TOOLCHAIN_FILE=~/Toolchain-arm-linux-gnueabihf.cmake
```

### Step 4: Build

If CMake ran successfully without errors, you are ready to move on to building. You can freely alternate between building and setting CMake flags.

After setting your CMake flags, just run

    make

If you are doing a native build with `distcc` you can try `make -j3` to improve the build speed.

And to install, run, for a native build:

    make install

for a cross-build, where you have Bela's live filesystem mounted locally via `sshfs` or other magic:

	DESTDIR=~/bbb/ make install

Building a Debian package for Bela
-------------------------

- create a debian package to be installed on the board:
```
cpack -G DEB -D CPACK_PACKAGE_CONTACT="Your Name <your.name@domain.com>" -D CPACK_DEBIAN_PACKAGE_ARCHITECTURE="armhf" -D CPACK_CMAKE_GENERATOR=Ninja -D CPACK_STRIP_FILES=true
```
(the `CPACK_CMAKE_GENERATOR=Ninja` is a trick to prevent the `preinstall` target from rebuilding the whole thing slower (see [here](https://stackoverflow.com/a/57530945/2958741)).

Running it
-------------------------

Just run the executable like this:

       scsynth -u 57110 -z 16 -B 0.0.0.0

The `-u` flag tells it which UDP port to listen on, with the `-z` flag we choose scsynth's internal blocksize and `-B 0.0.0.0` we allow `scsynth` to receive commands from any device on the network (which by default includes only Bela and your computer).

So now you should have scsynth running on the device. You should be able to send OSC commands to it from SuperCollider running on your main computer:

    // These commands are to be run in SUPERCOLLIDER running on your MAIN computer. (I guess you could run them on the device too if you wanted.)
    Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));
    SynthDef("funsound", { Out.ar(0, 0.5 * Pan2.ar(SinOsc.ar(LFNoise1.kr(2).exprange(100, 1000)), LFNoise1.kr(2))) }).add;
    x = Synth("funsound");
    SynthDef("bish", { Out.ar(0, PinkNoise.ar * EnvGen.ar(Env.perc, Impulse.kr(2))) }).add;
    y = Synth("bish");

    // then when you want to stop the sounds:
    x.free;
    y.free;

    // You could use this to test mic input - be careful of feedback!
    SynthDef("mic", { Out.ar(0, SoundIn.ar([0,1])) }).add;
    z = Synth("mic");
    z.free;

Bela I/O's
==========

I/O support for the Bela is implemented.

The startup flag ```-J``` defines how many analog input channels will be enabled, the startup flag ```-K``` how many analog output channels will be enabled, the startup flag ```-G``` how many digital channels will be enabled; by default all are set to 0.

So for all analog and digital channels to be enabled run scsynth like this:

       scsynth -u 57110 -z 16 -J 8 -K 8 -G 16

To use the analog channels all as audio I/O

       scsynth -u 57110 -z 16 -J 8 -K 8 -G 16 -i 10 -o 10

This will start scsynth with 10 inputs and outputs, inputs/outputs 2 - 9 are the analog pins

To use the analog channels all via the UGens only:

       scsynth -u 57110 -z 16 -J 8 -K 8 -G 16 -i 2 -o 2

This will start scsynth with 2 audio inputs and outputs, the analog I/O will only be accessible through UGens, but are all enabled.

If you want higher sample rates of the analog I/O, you can set the number of channels to 4; the number of available channels is then 4.

       scsynth -u 57110 -z 16 -J 4 -K 4 -G 16 -i 2 -o 2

The amount of analog inputs and outputs actually used will be rounded to a multiple of 4, so the actual options are 0, 4 or 8 analog channels. This is because in SuperCollider we cannot sample the analog channels faster than audio rate (right now).

The ```ServerOptions``` class has appropriate variables to set the command line arguments, so you can set them with (but also see the comment below):

    s.options.numAnalogInChannels = 8;
    s.options.numAnalogOutChannels = 8;
    s.options.numDigitalChannels = 16;


The UGens ```AnalogIn```, ```AnalogOut```, ```DigitalIn```, ```DigitalOut```, ```DigitalIO``` give access to the pins; they all have helpfiles with examples of usage.


Examples
======================================================

Example files are available in the folder ```examples/Bela```, and will be installed to ```/usr/share/SuperCollider/examples/Bela```.
More examples are available on [Bela](https://github.com/BelaPlatform/Bela/tree/master/examples/SuperCollider) and can be accessed through the Bela IDE.

Running scsynth *and* sclang
======================================================

You can start the server as normal from the language. To set the settings for the analog I/O you should set them to some reasonable values. The defaults are to not pass the flags to scsynth.

    s = Server.default;

    s.options.numAnalogInChannels = 8;
    s.options.numAnalogOutChannels = 8;
    s.options.numDigitalChannels = 16;

    s.options.blockSize = 16;
    s.options.numInputBusChannels = 2;
    s.options.numOutputBusChannels = 2;

    s.waitForBoot({
        "THE SERVER IS BOOTED! Start of my actually interesting code".postln;
    });

Alternatively, you can start scsynth manually, and then connect to it from a separate instance of sclang.

So make one connection and start scsynth:

    scsynth -u 57110 -z 16 -J 8 -K 8 -G 16 -i 2 -o 2

And another to start sclang:

    sclang examples/Bela/bela_example_analogin_2.scd

Options Overview
----------------------------

Here is a breakdown of the options for running *scsynth* and how to set them up with either *scsynth* or *sclang*

<table>
<tr>
<th>param</th>
<th>scsynth</th>
<th>sclang</th>
</tr>
<tr>
<td>audio computation block size</td>
<td>-z #</td>
<td> s.options.blockSize = 16;</td>
</tr>
<tr>
<td>number analog input channels enabled [0, 4, 8]</td>
<td>-J #</td>
<td>s.options.numAnalogInChannels = 0;</td>
</tr>
<tr>
<td>number analog output channels enabled [0, 4, 8]</td>
<td>-K #</td>
<td>s.options.numAnalogOutChannels = 0;</td>
</tr>
<tr>
<td>number digital channels enabled</td>
<td>-G #</td>
<td>s.options.numDigitalChannels = 16;</td>
</tr>
<tr>
<td>number of input buffer channels</td>
<td>-i #</td>
<td>s.options.numInputBusChannels = 2;</td>
</tr>
<tr>
<td>number of output buffer channels</td>
<td>-o #</td>
<td>s.options.numOutputBusChannels = 2;</td>
</tr>
<tr>
<td>max number of Bela oscilloscope channels</td>
<td>-E #</td>
<td>s.options.belaMaxScopeChannels = 4</td>
</tr>
</table>

Monitoring its performance
======================================================
Here's a tip on how to check CPU load and "mode switches" for the running program, to make sure it's running properly in realtime etc. (A "mode switch" is something to be avoided: it means the code is dropping out of the Xenomai real-time execution and into normal Linux mode, which can be caused by certain operations in the realtime thread.)

    watch -n 0.5 cat /proc/xenomai/sched/stat

which produces output like:

<pre>
Every 0.5s: cat /proc/xenomai/sched/stat                                                                                                               bela: Fri Jan 11 00:39:12 2019

CPU  PID    MSW        CSW        XSC        PF    STAT       %CPU  NAME
  0  0      0          835043     0          0     00018000   79.0  [ROOT]
  0  14390  5          5          14         1     000480c0    0.0  scsynth
  0  14405  1          69782      104649     0     00048046   14.3  bela-audio
  0  0      0          744555     0          0     00000000    0.8  [IRQ16: [timer]]
  0  0      0          34899      0          0     00000000    1.0  [IRQ181: rtdm_pruss_irq_irq]
</pre>

the "MSW" column indicates mode switches; this number should NEVER increase in the bela-audio thread. It is fine if it increases on a task that runs occasionally, but keep in mind that each mode switch carries an additional overhead.

Additional notes
======================================================

Bela's SuperCollider affects the interface with the UGens, therefore UGens built for "generic" SuperCollider may not run fine with SuperCollider built for Bela.
When using UGens on Bela, always make sure you build them against Sc with `-DAUDIOAPI=bela` enabled.

Contributors to this document
-----------------------------

- nescivi (marije baalman)
- dan stowell
- giulio moro
