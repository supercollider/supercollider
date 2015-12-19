COMPILING SUPERCOLLIDER SCSYNTH ON BELA
=======================================

See [README_main.md](README_main.md) for the "real" SuperCollider readme.

This file is Dan's notes about compiling SC on Bela (BeagleRT) platform.

At time of writing, I'm compiling just the server `scsynth`, so I've deactivated other components in the cmake.
This branch `bela_hackery` contains that plus other modifications to get the SC source code master branch building.
Eventually the idea would be to have a Xenomai/BeagleRT audio driver for scsynth.


PREPARATION
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


GET THE SOURCE CODE
===================

My modified source code is in this git branch here, called `bela_hackery`. If your Bela is still connected to the network you can grab it directly:

    cd /extrabela
    git clone -b bela_hackery https://github.com/danstowell/supercollider.git
    cd supercollider
    git submodule init && git submodule update

I believe that the Bela system image already includes most of SuperCollider's build dependencies. The updates to cmake/gcc described above are incurred because I'm using the latest `master` version of SC rather than 3.6.

COMPILING
=========

Before we compile, here's an optional step: installing ccache makes repeated builds faster, if you have spare disk space for it. It's especially helpful if you're going to be changing the cmake build scripts.

    apt-get install ccache
    echo "cache_dir = '/extrabela/ccache'" >> ~/.ccache/ccache.conf

Then here's how to build:

    mkdir /extrabela/build
    cd /extrabela/build
    # note that we must explicitly choose the compiler version 4.7 here too -- here's the version WITHOUT ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=gcc-4.7 -DCMAKE_CXX_COMPILER=g++-4.7 -DNOVA_SIMD=OFF -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    # or here's the version WITH ccache
    cmake /extrabela/supercollider -DCMAKE_C_COMPILER=/usr/lib/ccache/gcc-4.7 -DCMAKE_CXX_COMPILER=/usr/lib/ccache/g++-4.7 -DNOVA_SIMD=OFF -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF -DAUDIOAPI=bela
    make

The `make` step will take a little while, about 30 minutes for me.


INSTALL - CLASH WARNING
=======================

Note that my version of Bela already comes with a standard compile of SuperCollider in `/usr/local/bin/scsynth`, plus plugins at `/usr/local/lib/SuperCollider/plugins`. To avoid confusion, you might want to destroy those before installing, else you might accidentally end up using the vanilla version.

