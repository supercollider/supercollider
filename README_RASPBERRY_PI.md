Building SuperCollider on Raspberry Pi
======================================

SuperCollider can be built with the IDE and Qt GUI components on Raspbian Desktop, or without them on Raspbian
Desktop or Raspbian Lite. However, the project cannot be compiled with the QtWebEngine library, which means that the
help browser in the IDE, and the HelpBrowser and WebView classes in sclang, are unavailable.

Build requirements
------------------

* Raspberry Pi 2, 3 or 4 (Raspberry Pi 0 and 1 will also work, but note that compiling will take a _long_ time)
* SD card with [Raspbian Desktop](https://www.raspberrypi.org/downloads/raspbian) Stretch or Buster, or Raspbian Lite for a GUI-less build
* Router with Ethernet internet connection for the RPi
* For a GUI build: screen, mouse and keyboard
* For a GUI-less build: an empty dummy file called `ssh` on the root level of the SD card, to enable ssh, and a laptop connected to the same network as the RPi
* Optional: USB soundcard with headphones or speakers connected

Building
--------

### Step 1: Hardware setup

1. Connect an ethernet cable from the network router to the RPi
2. Insert the SD card and USB soundcard. For a GUI build, connect screen, mouse and keyboard
3. Connect USB power from a power supply

### Step 2: Update the system, install required libraries

For a GUI-less build, first ssh into the RPi from your laptop by opening a terminal and typing:

    ssh pi@raspberrypi # default password is 'raspberry'

In a terminal, type (or copy-and-paste):

    sudo raspi-config # change password and optionally edit hostname and enable ssh and/or vnc
    sudo apt-get update
    sudo apt-get upgrade
    sudo apt-get dist-upgrade

Install required libraries:

    # For GUI builds:
    sudo apt-get install libjack-jackd2-dev libsndfile1-dev libasound2-dev libavahi-client-dev \
        libreadline-dev libfftw3-dev libxt-dev libudev-dev libncurses5-dev cmake git qttools5-dev qttools5-dev-tools \
        qtbase5-dev libqt5svg5-dev qjackctl
    # For GUI-less builds:
    sudo apt-get install libsamplerate0-dev libsndfile1-dev libasound2-dev libavahi-client-dev \
        libreadline-dev libfftw3-dev libudev-dev libncurses5-dev cmake git

### Step 3: GUI-less builds only: compile and install jackd (no d-bus)

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
    exit # and ssh in again to make the limits.conf settings work

### Step 4: Compile and install SuperCollider

    cd ~ # or cd into the directory where you'd like to clone and build supercollider
    git clone --recurse-submodules https://github.com/supercollider/supercollider.git
    cd supercollider
    mkdir build && cd build

    # For a GUI-less build:
    cmake -DCMAKE_BUILD_TYPE=Release -DSUPERNOVA=OFF -DSC_EL=OFF -DSC_VIM=ON -DNATIVE=ON \
        -DSC_IDE=OFF -DNO_X11=ON -DSC_QT=OFF ..

    # For a GUI build:
    cmake -DCMAKE_BUILD_TYPE=Release -DSUPERNOVA=OFF -DSC_EL=OFF -DSC_VIM=ON -DNATIVE=ON \
        -DSC_USE_QTWEBENGINE:BOOL=OFF ..

    cmake --build . --config Release --target all -- -j3 # use -j3 flag only on RPi3 or newer
    sudo cmake --build . --config Release --target install
    sudo ldconfig

### Step 5: Set up JACK

    # `-dhw:0` is the internal soundcard. Use `-dhw:1` for USB soundcards. `aplay -l` will list available devices.
    # Use `nano ~/.jackdrc` to edit jack settings.

    # For GUI builds:
    echo /usr/bin/jackd -P75 -p16 -dalsa -dhw:0 -r44100 -p1024 -n3 > ~/.jackdrc

    # For GUI-less builds:
    echo /usr/local/bin/jackd -P75 -p16 -dalsa -dhw:0 -r44100 -p1024 -n3 > ~/.jackdrc

For GUI builds, another way to set up and start jack is to open a terminal and type `qjackctl`. Click 'setup' to
select soundcard and set periods to 3 (recommended). Then start jack before the SC IDE by clicking the play icon.

Usage
-----

To use SuperCollider, just open a terminal and execute `scide` (GUI) or `sclang` (GUI-less).

When you boot the server jack should start automatically with the settings in `~/.jackdrc`.

**Done!** See below for other usage notes and tips.

Running a GUI build headless
----------------------------

If you want to ssh in and start SuperCollider headless, run:

    export DISPLAY=:0.0
    sclang

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
    sleep 10 # can be lower (5) for rpi3
    sclang ~/mycode.scd
    EOF

    chmod +x ~/autostart.sh
    crontab -e # and add the following line to the end:
        @reboot cd /home/pi && ./autostart.sh

    nano ~/mycode.scd # And add your code inside a waitForBoot. For example:
        s.waitForBoot{ {SinOsc.ar([400, 404])}.play }

    sudo reboot # the sound should start after a few seconds

Login with ssh and run `killall jackd sclang scsynth` to stop the sound.

Benchmarks
----------

These are rough benchmark tests. The server should be booted and jackd running with settings: `-P75 -p1024 -n3 -r44100`

Also for comparison it is important to set CPU scaling to 'performance', by running:

    echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

Start sclang or scide and run:

    s.boot
    {1000000.do{2.5.sqrt}}.bench // ~0.56 for rpi3 headless, ~0.7 for rpi3 scide, ~1.7 for rpi0 headless, ~3.8 for rpi0 scide
    a= {Mix(50.collect{RLPF.ar(SinOsc.ar)});DC.ar(0)}.play
    s.avgCPU // run a few times. ~12% for rpi3, ~18% for rpi2, ~79% for rpi1, ~50% for rpi0
    a.free

With the default cpu scaling (ondemand) these benchmarks perform much worse, but 'ondemand' also saves battery life so
depending on your application, this might be the preferred mode.

To set 'performance' scaling mode permanently see the "Gotcha..." section of [this StackExchange
post](https://raspberrypi.stackexchange.com/questions/9034/how-to-change-the-default-governor#9048).

Notes and Troubleshooting
-------------------------

This applies to both GUI and GUI-less builds above:

* An easy way to burn the zip file (no need to unpack) to an SD card is to use [etcher](http://etcher.io).
* The internal soundcard volume is by default set low (40). Type `alsamixer` in terminal and adjust the pcm volume to
  85 with the arrow keys. Press escape to exit.
* The audio quality of rpi's built-in sound is terrible. Dithering helps a bit so add `-zs` to the jackd command if
  you are using the built-in sound.
* If building with `-j 3` stops or returns an error the compiler might just have run out of memory. Try to reboot and
  run the make command again without `-j 3` or decrease the gpu memory in raspi-config under advanced (set it to 16).
* If you get `WARNING: REMOTE HOST IDENTIFICATION HAS CHANGED!` when trying to ssh in, type `ssh-keygen -R
  raspberrypi` to reset.
* For lower latency, set a lower blocksize for jackd. Try, for example `-p512` or `-p128`. Tune downwards until you
  get dropouts and xruns (also watch cpu%).
* To avoid SD card corruption one should always shut down the system properly and not just pull out the power. When
  running headless you can either ssh in and type `sudo halt -p`, use a GPIO pin with a button and Python script, or
  set up an OSC command from within SC that turns off the RPi. See
  [here](https://github.com/blacksound/VTM/wiki/Raspberry-Pi-Instructions#shutdown-for-raspberry-pi).
* For the older Raspbian Jessie system use a [previous](https://github.com/supercollider/supercollider.github.io/blob/1f578b5fa71e1acae0ce40d14bc0ef116062093d/development/building-raspberrypi.md)
  version of these instructions.
* To quit sclang after starting via the commandline use `0.exit`.
