#!/bin/sh

# This script cleans all targets, then builds the projects for the server, the plugins and the application (sclang) projects, one after another.

if [ $# == 0 ]; then
	echo "--------------------------------------------------------------------------------
Building SuperCollider as a UNIVERSAL BINARY (intel + ppc).
For faster build, or if you don't have UB versions of the libs (e.g. libsndfile),
you may specify your computer's architecture by adding ARCHS=i386 or ARCHS=ppc 
as a parameter to this script.
--------------------------------------------------------------------------------
"
fi

# clean
echo "Cleaning scsynth..."
xcodebuild -project Synth.xcodeproj -target "All" $* clean || exit

echo "Cleaning plugins..."
xcodebuild -project Plugins.xcodeproj -target "All" $* clean || exit

echo "Cleaning sclang..."
xcodebuild -project Language.xcodeproj -target "All" $* clean || exit

echo "Clean Done."

# scsynth
echo "Building scsynth..."
xcodebuild -project Synth.xcodeproj -target "AllExceptAU" -configuration "Deployment" $* build || exit

# plugins
echo "Building plugins..."
xcodebuild -project Plugins.xcodeproj -target "All" -configuration "Deployment" $* build || exit

#sclang
echo "Building sclang..."
xcodebuild -project Language.xcodeproj -target "All" -configuration "Deployment" $* build || exit

# scau
echo "Building SuperColliderAU..."
xcodebuild -project Synth.xcodeproj -target "SuperColliderAU" -configuration "Deployment" $* build || exit

echo "Done."
