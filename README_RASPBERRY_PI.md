# Building SuperCollider on Raspberry Pi

SuperCollider can be built with the IDE and Qt GUI components or without them to run GUI-less (headless).
This README covers both options as well as options for building under a 64-bit or 32-bit OS.
Generally, all newer Raspberry Pi models can run a 64-bit OS, while the older models are 32-bit only.

These notes assume the official [Raspberry Pi OS](https://www.raspberrypi.com/software/operating-systems/)
but should also be useful if your Raspberry Pi is running, for example, [DietPi](https://dietpi.com).

**Note**: the project can only compile the QtWebEngine library under a 64-bit OS, which means that the help
browser in the IDE, and the HelpBrowser and WebView classes in sclang, are unavailable for a 32-bit OS.

## Build requirements

- Raspberry Pi 2, 3, 4 or 5 (Raspberry Pi 0 and 1 will also work, but note that compiling will take a _long_ time).
- microSD card with a 64-bit or 32-bit Raspberry Pi OS with desktop or Raspberry Pi OS Lite.
  Lite is the recommended OS for GUI-less builds.
- Internet connection for the Raspberry Pi.
- Screen, mouse and keyboard. Or SSH enabled to perform the build from another computer.
- Optional: USB sound card with headphones or speakers connected.

## Building

### Step 1: Hardware setup

1. Connect an ethernet cable from the network router to the RPi.
2. Insert the microSD card and USB sound card. Connect screen, mouse and keyboard.
3. Connect USB power from a power supply.

### Step 2: Update the system, install packages and dependencies

In a terminal, type (or copy-and-paste):

```shell
sudo apt-get update
sudo apt-get upgrade
sudo apt-get dist-upgrade
```

Install required libraries and dependencies:

```shell
sudo apt-get install \
  build-essential \
  cmake \
  libjack-jackd2-dev \
  libsndfile1-dev \
  libfftw3-dev \
  libxt-dev \
  libavahi-client-dev \
  libudev-dev \
  libasound2-dev \
  libreadline-dev \
  libxkbcommon-dev \
  git \
  jackd2 # Accept realtime permissions for jackd when asked
  ```

For GUI builds also install Qt - skip for GUI-less (Lite):

```shell
sudo apt-get install \
  qt6-base-dev \
  qt6-svg-dev \
  qt6-tools-dev \
  qt6-wayland \
  qt6-websockets-dev
```

For 64-bit OS GUI build - skip for GUI-less (Lite) and 32-bit OS:

```shell
sudo apt-get install qt6-webengine-dev
```

### Step 3: Obtain the source code

```shell
cd ~ # Or cd into the directory where you'd like to clone and build supercollider
git clone --branch main --recurse-submodules https://github.com/supercollider/supercollider.git
cd supercollider
mkdir build && cd build
```

### Step 4: Configure the build via CMake

**Note**: below are some configuration options. Pick the one that suits your OS and build type.

For a 64-bit GUI build:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DSUPERNOVA=OFF -DSC_EL=OFF -DSC_VIM=ON -DNATIVE=ON ..
```

For a 32-bit GUI build:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DSUPERNOVA=OFF -DSC_EL=OFF -DSC_VIM=ON -DNATIVE=ON \
  -DNOVA_SIMD:BOOL=OFF -DSC_USE_QTWEBENGINE:BOOL=OFF ..
```

For a 64-bit GUI-less build:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DSUPERNOVA=OFF -DSC_EL=OFF -DSC_VIM=ON -DNATIVE=ON \
  -DSC_IDE=OFF -DNO_X11=ON -DSC_QT=OFF ..
```

For a 32-bit GUI-less build:

```shell
cmake -DCMAKE_BUILD_TYPE=Release -DSUPERNOVA=OFF -DSC_EL=OFF -DSC_VIM=ON -DNATIVE=ON \
  -DSC_IDE=OFF -DNO_X11=ON -DSC_QT=OFF -DNOVA_SIMD:BOOL=OFF ..
```

### Step 5: Build and install

```shell
make # May add -j3 flag on newer RPis to speed up compilation
sudo make install
sudo ldconfig
```

### Step 6: Set up JACK

```shell
echo /usr/bin/jackd -P75 -p16 -dalsa -dhw:0 -p1024 -n3 > ~/.jackdrc
sudo usermod -aG audio $USER
sudo reboot
```

Change to `-dhw:1` or `-dhw:2` for USB sound cards. `aplay -l` will list available devices.

For GUI builds, another way to set up and start jack is to open a terminal and type `qjackctl`. Click 'setup' to
select a sound card and set periods to 3 (recommended). Then start jack before the SC IDE by clicking the play icon.

## Usage

To use SuperCollider (GUI), open SuperCollider IDE from the Applications Menu or open a terminal and execute `scide`.  
For GUI-less execute `sclang` from a terminal.

When you boot the server jack should start automatically with the settings in `~/.jackdrc`.

**Done!** See below for other usage notes and tips.

## sc3-plugins

To compile and install sc3-plugins, follow the instructions in the [sc3-plugins README](https://github.com/supercollider/sc3-plugins).

## Autostart

To automatically run SuperCollider code at system boot:

```shell
cat >~/autostart.sh <<EOF
#!/bin/bash
export PATH=/usr/local/bin:\$PATH
export QT_QPA_PLATFORM=offscreen
export JACK_NO_AUDIO_RESERVATION=1
sleep 10 # can be lower (5) for newer RPi models, or higher (30) for older
sclang ~/mycode.scd
EOF

chmod +x ~/autostart.sh

crontab -e # And add the following line to the end:
  @reboot ./autostart.sh

nano ~/mycode.scd # And add your code inside a waitForBoot. For example:
  s.waitForBoot{ {SinOsc.ar([400, 404])}.play }

sudo reboot # The sound should start after a few seconds
```

Login with SSH and run `killall jackd sclang scsynth` to stop the sound.

## Benchmarks

These are rough benchmark tests. The server should be booted and jackd running with settings:
`-P75 -p1024 -n3 -r44100`

Also for comparison, it is important to set CPU scaling to 'performance', by running:

```shell
sudo apt-get install cpufrequtils
sudo cpufreq-set -g performance
```

Start sclang or SC IDE and run:

```supercollider
s.boot
{1000000.do{2.5.sqrt}}.bench // ~0.56 for RPi3, ~0.84 for RPi2, ~1.7 for RPi0 headless, ~2.7 for RPi0 scide
a= {Mix(50.collect{RLPF.ar(SinOsc.ar)});DC.ar(0)}.play
s.avgCPU // run a few times. ~11% for RPi3, ~18% for RPi2, ~37% for RPi0
a.free
```

With the default CPU scaling (ondemand) these benchmarks perform worse, but 'ondemand' also saves battery life so
depending on your application, this might be the preferred mode.

To set 'performance' scaling mode permanently...
```shell
sudo crontab -e # And add the following line to the end:
  @reboot sleep 10 && cpufreq-set -g performance
```

## Notes and Troubleshooting

This applies to both GUI and GUI-less builds above:

- An easy way to install Raspberry Pi OS to a microSD card is to use the official
  [Raspberry Pi Imager](https://www.raspberrypi.com/software/).
- If the make step stops with an error you might have run out of RAM. Reboot and rerun the make command - possibly with a lower `-j` setting.
- The internal sound card volume is by default set low (40). Type `alsamixer` in the terminal and adjust the PCM volume to
  85 with the arrow keys. Press escape to exit.
- The audio quality of RPi's built-in sound is terrible. Dithering helps a bit so add `-zs` to the jackd command if
  you are using the built-in sound.
- For lower latency, set a lower blocksize for jackd. Try, for example `-p512` or `-p128`. Tune downwards until you
  get dropouts and xruns (also watch CPU %).
- If you get errors starting sclang from the terminal try `export QT_QPA_PLATFORM=offscreen && sclang`
- To avoid microSD card corruption one should always shut down the system properly and not just pull out the power. When
  running headless you can either SSH in and type `sudo halt -p`, use a GPIO pin with a button and Python script, or
  set up an OSC command from within SC that turns off the RPi. See
  [shutdown for raspberry pi](https://github.com/blacksound/VTM/wiki/Raspberry-Pi-Instructions#shutdown-for-raspberry-pi).
- To quit sclang after starting via the command line use `0.exit`
