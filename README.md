See [README_main.md](README_main.md) for the "real" SuperCollider readme.


This file is Dan's notes about compiling SC on Bela (BeagleRT) platform.


At time of writing, I'm compiling just the server `scsynth`, so I've deactivated other components in the cmake.



PREPARATION
===========

Plug in an ethernet cable, so you can (a) install/update packages and (b) set the system time. Then:

    ifup eth0
    echo "deb http://http.debian.net/debian wheezy-backports main" >> /etc/apt/sources.list
    apt-get update
    apt-get install gcc-4.7 g++-4.7
    apt-get -t wheezy-backports install cmake    # need this updated version

    dpkg-reconfigure tzdata
    ntpdate pool.ntp.org
    date  # make sure this gives the right result

On my Bela's SD card I added an extra partition and mounted it at `/extrabela` - all my work will be in this partition:

    mkdir /extrabela
    mount /dev/mmcblk0p3 /extrabela

Actually I added this line to my /etc/fstab:

    /dev/mmcblk0p3  /extrabela   ext4  noatime,errors=remount-ro  0  1


COMPILING
=========

I put the supercollider source code at `/extrabela/supercollider-master-stripped`. Then here's how to build:

    mkdir -p /extrabela/build
    cd /extrabela/build
    # note that we're choosing the compiler version here too
    cmake /extrabela/supercollider-master-stripped -DCMAKE_C_COMPILER=gcc-4.7 -DCMAKE_CXX_COMPILER=g++-4.7 -DNOVA_SIMD=OFF -DSSE=OFF -DSSE2=OFF -DINSTALL_HELP=OFF -DSC_QT=OFF -DSC_IDE=OFF -DSUPERNOVA=OFF -DNO_AVAHI=ON -DNATIVE=ON -DENABLE_TESTSUITE=OFF
    make

