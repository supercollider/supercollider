Building SuperCollider on BeagleBone Black
==========================================

This is for compiling and running sclang and scsynth under **Debian**. It does not include the IDE nor the Qt GUI
components for sclang. For compiling with GUI support, you may be able to adapt the instructions from the
Raspberry Pi readme.

Build Requirements
------------------

* BeagleBone Black (or any of the variants green, blue, industrial, pocket, etc.)
* SD card with Debian Stretch or Buster. You could use [iot](http://beagleboard.org/latest-images),
   [lxqt](http://beagleboard.org/latest-images) or
   [console](https://elinux.org/Beagleboard:BeagleBoneBlack_Debian#Debian_Buster_Console_Snapshot). The console
   version is minimal and recommended.
* Router with ethernet internet connection for the BBB
* Laptop connected to same network as the BBB
* USB soundcard with headphones or speakers connected

Building
--------

### Step 1: Hardware setup

1. Connect an ethernet cable from the network router to the BBB
2. Insert the SD card and USB soundcard.
3. Connect USB power from a 5V@1A power supply

### Step 2: Update the system, install required libraries

ssh from your laptop by opening a terminal and typing:

    ssh debian@beaglebone # default password is temppwd
    sudo passwd debian # change password
    sudo /opt/scripts/tools/grow_partition.sh # expand file system. skip if you are running on the emmc
    sudo reboot # and log in again with ssh

    # log in again with ssh
    ssh debian@beaglebone
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get dist-upgrade
    sudo apt-get install libsamplerate0-dev libsndfile1-dev libasound2-dev libavahi-client-dev libreadline-dev \
        libncurses5-dev libfftw3-dev libudev-dev cmake git build-essential python-dev alsa-utils cpufrequtils

### Step 3: Compile, install and configure jackd (no d-bus)

    cd ~ # or cd into the directory where you'd like to build jack2
    git clone https://github.com/jackaudio/jack2 --depth 1
    cd jack2
    ./waf configure --alsa --libdir=/usr/lib/arm-linux-gnueabihf/
    ./waf build
    sudo ./waf install
    sudo ldconfig
    cd ..
    rm -rf jack2
    sudo sh -c "echo @audio - memlock 256000 >> /etc/security/limits.conf"
    sudo sh -c "echo @audio - rtprio 75 >> /etc/security/limits.conf"

    # `-dhw:1` is normally the USB soundcard. `sudo aplay -l` will list available devices.
    # Use `nano ~/.jackdrc` to edit jack settings.
    echo /usr/local/bin/jackd -P75 -p16 -dalsa -dhw:1 -r44100 -p1024 -n3 > ~/.jackdrc

    exit # and ssh in again to make the limits.conf settings work

### Step 4: Compile and install SuperCollider

    cd ~ # or cd into the directory where you'd like to clone and build supercollider
    git clone --recurse-submodules https://github.com/supercollider/supercollider.git
    cd supercollider
    mkdir build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DSUPERNOVA=OFF -DNATIVE=ON -DSC_IDE=OFF -DNO_X11=ON \
        -DSC_QT=OFF -DSC_EL=OFF -DSC_VIM=ON ..
    cmake --build . --config Release --target all
    sudo cmake --build . --config Release --target install
    sudo ldconfig


Usage
-----

To use SuperCollider, just open a terminal and execute `sclang`.

**Done!** See below for other usage notes and tips.

sc3-plugins
-----------

To compile and install sc3-plugins, follow the instructions in the [sc3-plugins README](https://github.com/supercollider/sc3-plugins).

Autostart
---------

To automatically run SuperCollider code at system boot:

    cat >~/autostart.sh <<EOF
    #!/bin/bash
    export PATH=/usr/local/bin:$PATH
    export DISPLAY=:0.0
    sleep 10
    sclang mycode.scd
    EOF

    chmod +x ~/autostart.sh
    crontab -e # and add the following line to the end:
        @reboot cd /home/pi && ./autostart.sh

    nano ~/mycode.scd # And add your code inside a waitForBoot. For example:
        s.waitForBoot{ {SinOsc.ar([400, 404])}.play }

    sudo reboot # the sound should start after a few seconds

Login with ssh and run `pkill jackd; pkill sclang; pkill scsynth` to stop the sound.

Benchmarks
----------

These are rough benchmark tests. The server should be booted and jackd running with settings: `-P75 -p1024 -n3 -r44100`

Also make sure to power the bbb through the barrel jack (else the CPU is capped at 300mhz - check with
`cpufreq-info`). Also for comparison it is important to set CPU scaling to 'performance' with:

    sudo cpufreq-set --governor performance

Start sclang and run:

    s.boot
    {1000000.do{2.5.sqrt}}.bench // ~1.9 on a bb-black, ~2.6 with lxqt on a bb-black
    a= {Mix(50.collect{RLPF.ar(SinOsc.ar)});DC.ar(0)}.play
    s.avgCPU // run a few times. ~47% on a bb-black
    a.free

The default CPU scaling mode 'ondemand' seem to work quite good on the BBB and will save battery without losing much
in performance.

Notes and Troubleshooting
-------------------------

* An easy way to burn the img.xy file (no need to unpack) to an SD card is to use [etcher](http://etcher.io).
* Type `alsamixer` in terminal and then F6. Adjust the mic and speaker volume with the arrow keys. Press escape to
  exit.
* If compilation stops or returns an error the compiler might just have run out of memory. Try to reboot and
  run the make command again.
* If you get `WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!` when trying to ssh in, type `ssh-keygen -R
  beaglebone` to reset.
* For lower latency, set a lower blocksize for jackd. Try, for example `-p512` or `-p128`. Tune downwards until you
  get dropouts and xruns (also watch cpu%).
* To avoid SD card corruption one should always shut down the system properly and not just pull out the 5V power. When
  running headless you can either ssh in and type `sudo halt -p`, use a GPIO pin with a button and Python script, or
  set up an OSC command from within SC that turns off the BBB. See
  [here](https://github.com/blacksound/VTM/wiki/Raspberry-Pi-Instructions#shutdown-for-raspberry-pi).
  version of these instructions.
* To quit sclang after starting via the commandline use `0.exit`.
