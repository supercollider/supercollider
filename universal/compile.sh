#!/bin/sh

# This script builds the server, the plugins and the application (sclang), one after another.
# It uses XCode 2.1 +, run it on Panther+ only.

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
