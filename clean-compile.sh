#!/bin/sh

# This script cleans all targets, then invokes compile.sh

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
