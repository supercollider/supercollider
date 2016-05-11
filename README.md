Compiling SuperCollider scsynth on Bela
=======================================

See [README_main.md](README_main.md) for the "real" SuperCollider readme.

This file is Dan's notes about compiling SC on [Bela (BeagleRT)](http://beaglert.cc/) platform.

At time of writing, I'm compiling just the server `scsynth`, so I've deactivated other components in the cmake.
This branch `bela_hackery` contains that plus other modifications to get the SC source code master branch building.
The main addition in this branch is a **Xenomai/BeagleRT audio driver for scsynth**, to use Bela's ultra-low-latency audio thread *instead* of jack/portaudio.

> *NOTE:* If you have a prototype Bela board (from 2015) you'll need to [get a fresh SD card image](https://code.soundsoftware.ac.uk/projects/beaglert/wiki) since the Bela API changed a little.

All of the commands here are to be executed *on the Bela device itself*. Normally you would SSH to it from a computer connected by USB, in order to do the following stuff.

Preparation
===========

Plug in an ethernet cable (or connect to the Internet some other way). Then we need to (a) install/update packages and (b) set the system time:

    ifup eth0

    echo "deb http://http.debian.net/debian wheezy-backports main" >> /etc/apt/sources.list
    apt-get update
    apt-get install gcc-4.7 g++-4.7
    apt-get -t wheezy-backports install cmake    # need this updated version

    dpkg-reconfigure tzdata
    ntpdate pool.ntp.org
    date  # make sure this gives the right result

On my Bela's SD card I added an extra partition and mounted it at `/extrabela` - all my work will be in this partition.
You'll need at least maybe 250 MB spare (estimated).

    mkdir /extrabela
    mount /dev/mmcblk0p3 /extrabela

Actually I added this line to my /etc/fstab so the partition automounts:

    /dev/mmcblk0p3  /extrabela   ext4  noatime,errors=remount-ro  0  1


Get the source code
===================

My modified source code is in this git branch here, called `bela_hackery`. If your Bela is still connected to the network you can grab it directly:

    cd /extrabela
    git clone --recursive -b bela_hackery https://github.com/danstowell/supercollider.git
    cd supercollider

I believe that the Bela system image already includes most of SuperCollider's build dependencies. The updates to cmake/gcc described above are incurred because I'm using the latest `master` version of SC rather than 3.6.

Compiling and installing
========================

Before we compile, here's an optional step: installing ccache makes repeated builds faster, if you have spare disk space for it. It's especially helpful if you're going to be changing the cmake build scripts.

    apt-get install ccache
    echo "cache_dir = '/extrabela/ccache'" >> ~/.ccache/ccache.conf

Then here's how to build:

    mkdir /extrabela/build
    cd /extrabela/build
    # note that we must explicitly choose the compiler version 4.7 here too, whichever command we use
    # here's the command WITHOUT ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=gcc-4.7 -DCMAKE_CXX_COMPILER=g++-4.7 -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF -DSC_ED=OFF -DSC_VIM=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    # or here's the command WITH ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc-4.7 -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++-4.7 -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF -DSC_ED=OFF -DSC_VIM=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    make

The `make` step will take a little while, about 30 minutes for me.

Next we install. **CLASH WARNING:** Note that my version of Bela already comes with a standard compile of SuperCollider in `/usr/local/bin/scsynth`, plus plugins at `/usr/local/lib/SuperCollider/plugins`. To avoid confusion, you might want to destroy those before installing, else you might accidentally end up using the vanilla version.

    make install


# FROM testing image - https://github.com/BelaPlatform/bela-image/releases/tag/v0.1-testing-2016.04.19

This image already has a good version of gcc and g++ (4.9).

Preparation
===========

Plug in an ethernet cable (or connect to the Internet some other way). Then we need to (a) install/update packages and (b) set the system time:

    ntpdate -b -u pool.ntp.org
    date  # make sure this gives the right result

On my Bela's SD card I added an extra partition and mounted it at `/extrabela` - all my work will be in this partition.
You'll need at least maybe 250 MB spare (estimated).

    mkdir /extrabela
    mount /dev/mmcblk0p3 /extrabela

Actually I added this line to my /etc/fstab so the partition automounts:

    /dev/mmcblk0p3  /extrabela   ext4  noatime,errors=remount-ro  0  1



Get the source code
===================

My modified source code is in this git branch here, called `bela_hackery`. If your Bela is still connected to the network you can grab it directly:

    cd /extrabela
    git clone --recursive -b bela_hackery https://github.com/danstowell/supercollider.git
    cd supercollider

I believe that the Bela system image already includes most of SuperCollider's build dependencies. The updates to cmake/gcc described above are incurred because I'm using the latest `master` version of SC rather than 3.6.


Compiling and installing
========================

Get the newest cmake:

    apt-get update
    apt-get -t jessie install cmake    # need this updated version

Before we compile, here's an optional step: installing ccache makes repeated builds faster, if you have spare disk space for it. It's especially helpful if you're going to be changing the cmake build scripts.

    apt-get install ccache
    echo "cache_dir = '/extrabela/ccache'" >> ~/.ccache/ccache.conf

Dependencies:

    apt-get install libudev-dev

Then here's how to build:

    mkdir /extrabela/build
    cd /extrabela/build
    # note that we explicitly choose the compiler version 4.9 here too, whichever command we use
    # here's the command WITHOUT ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=gcc-4.9 -DCMAKE_CXX_COMPILER=g++-4.9 -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF -DSC_ED=OFF -DSC_VIM=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    # or here's the command WITH ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc-4.9 -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++-4.9 -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF -DSC_ED=OFF -DSC_VIM=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    make

The `make` step will take a little while, about 30 minutes for me. It seems it is stuck for a long time at compiling the BinaryOpUGens, but it will get past that.

Next we install. 
**CLASH WARNING:** Note that my version of Bela already comes with a standard compile of SuperCollider in `/usr/local/bin/scsynth`, `/usr/local/bin/sclang`, `/usr/local/bin/sclang_pipeapp`, `/usr/local/bin/scvim`, plus plugins at `/usr/local/lib/SuperCollider/plugins` and class library in `/usr/local/share/SuperCollider`. To avoid confusion, you might want to destroy those before installing, else you might accidentally end up using the vanilla version.

    make install


Running it
==========

Just run the executable like this:

       scsynth -u 57110 -z 16

The `-u` flag tells it which UDP port to listen on, and with the `-z` flag we choose scsynth's internal blocksize. We need to do this because scsynth's default internal buffer size (64) is bigger than the hardware buffer size (16), so dividing hardware by internal returned 0 buffers per callback. To make it run, you need to add the command-line argument "-z 16" (or presumably make the hardware buffer size bigger).

So now you should have scsynth running on the device. You should be able to send OSC commands to it from SuperCollider running on your main computer:

    // These commands are to be run in SUPERCOLLIDER running on your MAIN computer. (I guess you could run them on the device too if you wanted.)
    Server.default = s = Server("belaServer", NetAddr("192.168.7.2", 57110));
    s.initTree;
    s.startAliveThread;
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
    
BELA I/O's
==========

I/O support for the BeLa is implemented.

The startup flag ```-J``` defines how many analog channels will be enabled.

So for all analog channels to be enabled run scsynth like this:

       scsynth -u 57110 -z 16 -J 8

To use the analog channels all as audio I/O 

       scsynth -u 57110 -z 16 -J 8 -i 10 -o 10

This will start scsynth with 10 inputs and outputs, inputs/outputs 2 - 9 are the analog pins

To use the analog channels all via the UGens only:

       scsynth -u 57110 -z 16 -J 8 -i 2 -o 2
       
This will start scsynth with 2 audio inputs and outputs, the analog I/O will only be accessible through UGens, but are all enabled.

If you want higher sample rates of the analog I/O, you can set the number of channels to 4; the number of available channels is then 4.

       scsynth -u 57110 -z 16 -J 4 -i 2 -o 2


The UGens ```AnalogInput```, ```AnalogOutput```, ```DigitalInput```, ```DigitalOutput```, ```DigitalIO``` give access to the pins.


Monitoring its performance
======================================================
Here's a tip from a Bela user on how to check CPU load and "mode switches" for the running program, to make sure it's running properly in realtime etc. (A "mode switch" is something to be avoided: it means the code is dropping out of the Xenomai real-time execution and into normal Linux mode, which can be caused by certain operations in the realtime thread.)

    watch -n 0.5 cat /proc/xenomai/stat

which produces output like:

<pre>
Every 0.5s: cat /proc/xenomai/stat                                                                                                                         Fri Apr  1 12:37:24 2016

CPU  PID    MSW        CSW        PF    STAT       %CPU  NAME
  0  0      0          159371     0     00500080   76.4  ROOT
  0  2282   1          1          0     00b00380    0.0  scsynth
  0  2286   1          2          0     00300380    0.0  mAudioSyncSignalTask
  0  2288   2          159368     1     00300184   21.0  beaglert-audio
  0  0      0          251165     0     00000000    1.9  IRQ67: [timer]
</pre>

the "MSW" column indicates mode switches; this number should NEVER increase in the beaglert-audio thread. It is fine if it increases on a task that runs occasionally, but keep in mind that each mode switch carries an additional overhead.

Optional: Bonus level: Even more plugins (sc3-plugins)
======================================================

SuperCollider comes with a built-in set of UGen plugins but there's an extra set in the community **sc3-plugins** project. So if you want, you can also install those:

    cd /extrabela
    git clone --recursive https://github.com/supercollider/sc3-plugins.git
    cd sc3-plugins
    mkdir build
    cd build
    cmake -DSC_PATH=/extrabela/supercollider -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc-4.7 -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++-4.7  ..
    make
    make install

These are basically just the instructions from the README of [the sc3-plugins project](https://github.com/supercollider/sc3-plugins/).
