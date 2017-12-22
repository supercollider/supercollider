SuperColliderAU
===============

SuperColliderAU is an AudioUnit wrapper that allows using SuperCollider servers inside AudioUnits hosts on macOS. The embedded server may be controlled over OSC as usual. In addition, it may be packed with a synth definition and a configuration file that defines its parameters.

Below are some quick instructions for building SuperColliderAU. The result is an AudioUnit plug-in that can be used for the first use case (interactive), and can also be modified for the second (standalone) using the AudioUnitBuilder quark.

In order to build SuperColliderAU, you first need to build SuperCollider itself in the usual way. This is needed for generating SC\_Version.hpp and also for compiling plugins. It is assumed that the SuperCollider build folder is named `build`.

After compiling SuperCollider, cd to this directory and run:

    mkdir build
    cd build
    cmake ..
    make

If everything goes well this should create a bundle named SuperColliderAU.component. To use, copy the bundle to ~/Library/Audio/Plug-Ins/Components/.
