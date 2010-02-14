#!/bin/sh

# This script builds the server, the plugins and the application (sclang), one after another.

if [ $# == 0 ]; then
	echo "--------------------------------------------------------------------------------
Building SuperCollider as a 32 bit UNIVERSAL BINARY (intel + ppc).
For faster build, or if you don't have UB versions of the libs 
(e.g. libsndfile),you may specify your computer's architecture 
by adding ARCHS=i386 or ARCHS=ppc as a parameter to this script.
Or, if you are using Mac OS 10.6, you may also specify ARCHS=32_64
to build a 32/64-bit Universal Binary version of the server and 
plugins along with a 32-bit version of the application
--------------------------------------------------------------------------------
"
fi

# scsynth
echo "Building scsynth..."
if [ $# != 0  ]; then
    if [ $* == "ARCHS=32_64" ]; then
        echo "Building 32/64 bit scsynth"
        xcodebuild -project Synth.xcodeproj -target "AllExceptAU" -configuration "Deployment32-64" build || exit
    else
    xcodebuild -project Synth.xcodeproj -target "AllExceptAU" -configuration "Deployment" $* build || exit
    fi
else 
    xcodebuild -project Synth.xcodeproj -target "AllExceptAU" -configuration "Deployment" build || exit    
fi

# plugins
echo "Building plugins..."
if [ $# != 0  ]; then
    if [ $* == "ARCHS=32_64" ]; then
        echo "Building 32/64 bit plugins"
        xcodebuild -project Plugins.xcodeproj -target "All" -configuration "Deployment32-64" build || exit
    else
        xcodebuild -project Plugins.xcodeproj -target "All" -configuration "Deployment" $* build || exit
    fi
else
    xcodebuild -project Plugins.xcodeproj -target "All" -configuration "Deployment" build || exit
fi

#sclang
echo "Building sclang..."
if [ $# != 0  ]; then
    if [ $* == "ARCHS=32_64" ]; then
        xcodebuild -project Language.xcodeproj -target "All" -configuration "Deployment" build || exit
    else
    xcodebuild -project Language.xcodeproj -target "All" -configuration "Deployment" $* build || exit
    fi
else
    xcodebuild -project Language.xcodeproj -target "All" -configuration "Deployment" build || exit
fi

# scau
echo "Building SuperColliderAU..."
if [ $# != 0  ]; then
    if [ $* == "ARCHS=32_64" ]; then
        xcodebuild -project Synth.xcodeproj -target "SuperColliderAU" -configuration "Deployment32-64" build || exit
    else
        xcodebuild -project Synth.xcodeproj -target "SuperColliderAU" -configuration "Deployment" $* build || exit
    fi
else
    xcodebuild -project Synth.xcodeproj -target "SuperColliderAU" -configuration "Deployment" build || exit
fi

echo "Done."
