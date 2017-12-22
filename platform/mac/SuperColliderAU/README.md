SuperColliderAU
===============

SuperColliderAU is an AudioUnit wrapper that allows using SuperCollider servers inside AudioUnits hosts on macOS. The embedded server may be controlled over OSC as usual. In addition, it may be packed with a synth definition and a configuration file that defines its parameters.

This version is an attempt to re-sync SuperColliderAU with current SuperCollider development, this document contains only some build instructions.

In order to build, you first need to build SuperCollider itself in the usual way. This is needed for generating SC\_Version.hpp and also for compiling plugins. At the moment plugins need to be manually copied to the AudioUnit bundle.

After compiling SuperCollider, you can build SuperColliderAU from this directory:

    mkdir build
    cd build
    cmake ..
    make

If everything goes well this should create a bundle named SuperColliderAU.component. To use, copy the scx files from the SuperCollider build. Assuming `RelWithDebInfo` was used as config and `build` as build folder:

    mkdir SuperColliderAU.component/Contents/Resources/plugins

    cp ../../../../build/server/plugins/RelWithDebInfo/*.scx SuperColliderAU.component/Contents/Resources/plugins

Then copy the bundle to ~/Library/Audio/Plug-Ins/Components/
