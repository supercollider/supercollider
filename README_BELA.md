See [SuperCollider-on-Bela](https://github.com/BelaPlatform/Bela/wiki/SuperCollider-on-Bela) for general information on SuperCollider on Bela.

Compiling SuperCollider scsynth on Bela
=======================================

See [README.md](README.md) for the main SuperCollider readme.

This file is Dan's and Marije's notes about compiling SC on [Bela](http://bela.io) platform.

This branch `bela_hackery_v01` contains that plus other modifications to get the SC source code master branch building.
The main addition in this branch is a **Xenomai/Bela audio driver for scsynth**, to use Bela's ultra-low-latency audio thread *instead* of jack/portaudio, and **plugins to access the analog and digital channels of the Bela-cape**

> *NOTE:* This guide assumes you have the [Bela image v0.1.0](https://github.com/BelaPlatform/bela-image/releases/tag/v0.1.0) (or higher).

All of the commands here are to be executed *on the Bela device itself*. Normally you would SSH to it from a computer connected by USB, in order to do the following stuff.

Preparation
===========

Plug in an ethernet cable (or connect to the Internet some other way, see also [Bela Documentation](https://github.com/BelaPlatform/Bela/wiki/Getting-started-with-Bela#software-setup)). Then we need to (a) install/update packages and (b) set the system time:

a) See below at compiling and installing

b) Set the system time:

    dpkg-reconfigure tzdata
    ntpdate pool.ntp.org
    date  # make sure this gives the right result

Working partition
=================

On my Bela's SD card I added an extra partition and mounted it at `/extrabela` - all my work will be in this partition.
You'll need at least maybe 500 MB spare (estimated).

    mkdir /extrabela
    mount /dev/mmcblk0p3 /extrabela

Actually I added this line to my /etc/fstab so the partition automounts:

    /dev/mmcblk0p3  /extrabela   ext4  noatime,errors=remount-ro  0  1


Get the source code
===================

My modified source code is in this git branch here, called `bela_hackery`. If your Bela is still connected to the network you can grab it directly:

    cd /extrabela
    git clone --recursive -b bela_hackery_v01_master https://github.com/sensestage/supercollider.git
    cd supercollider

I believe that the Bela system image already includes most of SuperCollider's build dependencies. The updates to cmake/gcc described above are incurred because I'm using the latest `master` version of SC rather than 3.6.


Compiling and installing
========================

Update apt source list:
    apt-get update

We need gcc-4.8 / g++-4.8 as 4.9 causes a weird bug (https://github.com/supercollider/supercollider/issues/1450):

    apt-get install gcc-4.8 g++-4.8

Get the newest cmake:

    apt-get -t jessie install cmake    # need this updated version

Get dependent libraries:
    
    apt-get install libudev-dev


Before we compile, here's an optional step: installing ccache makes repeated builds faster, if you have spare disk space for it. It's especially helpful if you're going to be changing the cmake build scripts.

    apt-get install ccache
    mkdir /root/.ccache
    echo "cache_dir = '/extrabela/ccache'" >> ~/.ccache/ccache.conf

Then here's how to build:

    mkdir /extrabela/build
    cd /extrabela/build
    # note that we explicitly choose the compiler version 4.8 here too, whichever command we use
    # here's the command WITHOUT ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=gcc-4.8 -DCMAKE_CXX_COMPILER=g++-4.8 -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF -DSC_ED=OFF -DSC_VIM=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    # or here's the command WITH ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc-4.8 -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++-4.8 -DNOVA_SIMD=ON -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSC_EL=OFF -DSC_ED=OFF -DSC_VIM=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    make

The `make` step will take a little while, about 30 minutes for me. It seems it is stuck for a long time at compiling the BinaryOpUGens, but it will get past that.

Next we install.

**CLASH WARNING:** Note that my version of Bela already comes with a standard compile of SuperCollider in `/usr/local/bin/scsynth`, `/usr/local/bin/sclang`, `/usr/local/bin/sclang_pipeapp`, `/usr/local/bin/scvim`, plus plugins at `/usr/local/lib/SuperCollider/plugins` and class library in `/usr/local/share/SuperCollider`. To avoid confusion, you might want to destroy those before installing, else you might accidentally end up using the vanilla version.

With these commands you remove the old stuff:
    
    rm -r /usr/local/lib/SuperCollider/
    rm -r /usr/local/share/SuperCollider/
    rm /usr/local/bin/sclang
    rm /usr/local/bin/sclangpipe_app 
    rm /usr/local/bin/scsynth
    rm /usr/local/bin/scvim

Installation the new version:

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

![Channel explanation](bela_analog_audio_io.png)

The ```ServerOptions``` class has appropriate variables to set the command line arguments, so you can set them with (but also see the comment below):

    s.options.numAnalogInChannels = 8;
    s.options.numAnalogOutChannels = 8;
    s.options.numDigitalChannels = 16;


The UGens ```AnalogIn```, ```AnalogOut```, ```DigitalIn```, ```DigitalOut```, ```DigitalIO``` give access to the pins; they all have helpfiles with examples of usage.


Examples
======================================================

Example files are available in the folder ```examples/bela```, and will be installed to ```/usr/local/share/SuperCollider/examples/bela```.


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

    sclang examples/bela/bela_example_analogin_2.scd

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
  0  2288   2          159368     1     00300184   21.0  bela-audio
  0  0      0          251165     0     00000000    1.9  IRQ67: [timer]
</pre>

the "MSW" column indicates mode switches; this number should NEVER increase in the bela-audio thread. It is fine if it increases on a task that runs occasionally, but keep in mind that each mode switch carries an additional overhead.

Optional: Bonus level: Even more plugins (sc3-plugins)
======================================================

SuperCollider comes with a built-in set of UGen plugins but there's an extra set in the community **sc3-plugins** project. So if you want, you can also install those:

    cd /extrabela
    git clone --recursive https://github.com/supercollider/sc3-plugins.git
    cd sc3-plugins
    mkdir build
    cd build
    cmake -DSC_PATH=/extrabela/supercollider -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc-4.8 -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++-4.8 -DCMAKE_C_FLAGS="-march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -O2" -DCMAKE_CPP_FLAGS="-march=armv7-a -mtune=cortex-a8 -mfloat-abi=hard -mfpu=neon -O2" ..
    make
    make install

These are basically just the instructions from the README of [the sc3-plugins project](https://github.com/supercollider/sc3-plugins/).
