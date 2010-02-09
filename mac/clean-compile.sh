#!/bin/sh

# This script cleans all targets, then invokes compile.sh

# Replicate warning from compile.sh, because it's user-friendly for it to be the first output:
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

# Now let compile.sh do its job:
./compile.sh $*
