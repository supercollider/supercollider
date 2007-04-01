#!/bin/sh

# This script cleans all targets, then builds the projects for the server, the plugins and the application (sclang) projects, one after another.
# It uses XCode 2.1 +, run it on Panther+ only.

# clean
echo "cleaning scsynth"
xcodebuild -project xSC3synth.xcodeproj -target "All" clean || exit

echo "cleaning plugins"
xcodebuild -project xSC3plugins.xcodeproj -target "All" clean || exit

echo "cleaning sclang"
xcodebuild -project xSC3lang.xcodeproj -target "All" clean || exit

echo "Clean Done."

# scsynth
echo "building scsynth"
xcodebuild -project xSC3synth.xcodeproj -target "All" -buildstyle "Deployment" build || exit

# sc plugins
echo "building plugins"
xcodebuild -project xSC3plugins.xcodeproj -target "All" -buildstyle "Deployment" build || exit

#sclang
echo "building sclang"
xcodebuild -project xSC3lang.xcodeproj -target "All" -buildstyle "Deployment" build || exit

echo "Done."
