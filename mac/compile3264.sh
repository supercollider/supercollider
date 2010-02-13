#!/bin/sh

# This script builds the server, the plugins and the application (sclang), one after another.

if [ $# == 0 ]; then
	echo "--------------------------------------------------------------------------------
Building SuperCollider as a UNIVERSAL BINARY (intel + x86_64 + ppc).
For faster build, or if you don't have UB versions of the libs (e.g. libsndfile),
you may specify your computer's architecture by adding ARCHS=i386, 
ARCHS=x86_64 or ARCHS=ppc as a parameter to this script.
--------------------------------------------------------------------------------
"
fi

# scsynth
echo "Building scsynth..."
xcodebuild -project Synth.xcodeproj -target "AllExceptAU" -configuration "Deployment32-64" $* build || exit

# plugins
echo "Building plugins..."
xcodebuild -project Plugins.xcodeproj -target "All" -configuration "Deployment32-64" $* build || exit

#sclang
echo "Building sclang..."
xcodebuild -project Language.xcodeproj -target "All" -configuration "Deployment" $* build || exit

# scau
echo "Building SuperColliderAU..."
xcodebuild -project Synth.xcodeproj -target "SuperColliderAU" -configuration "Deployment" $* build || exit

echo "Done."
