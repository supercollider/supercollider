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

# First let's detect the special 32/64bit mode
switchableConfig="Deployment"
buildArg=$*
for opt in $*
do
    if [ $opt == "ARCHS=32_64" ]; then
		switchableConfig="Deployment32-64"
		buildArg=""
        echo "Building 32/64 bit sclang, scsynth and plugins"
    fi
done

# scsynth
echo "Building scsynth..."
xcodebuild -project Synth.xcodeproj -target "AllExceptAU" -configuration $switchableConfig $buildArg build || exit

# plugins
echo "Building plugins..."
xcodebuild -project Plugins.xcodeproj -target "All" -configuration $switchableConfig $buildArg build || exit

#sclang
echo "Building sclang..."
xcodebuild -project Language.xcodeproj -target "All" -configuration $switchableConfig $buildArg build || exit

# scau
echo "Building SuperColliderAU..."
xcodebuild -project Synth.xcodeproj -target "SuperColliderAU" -configuration $switchableConfig $buildArg build || exit

echo "Done."
