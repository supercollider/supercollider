#!/bin/sh

# This script builds the server, the plugins and the application (sclang), one after another.
# It uses XCode, run it on Panther+ only.

# scsynth 
echo "building scsynth"
xcodebuild -project xSC3synth.pbproj -target "All" -buildstyle "Deployment" build || exit

# sc plugins
echo "building plugins"
xcodebuild -project xSC3plugins.pbproj -target "All Plugins" -buildstyle "Deployment" build || exit

#sclang
echo "building sclang"
xcodebuild -project xSC3lang.pbproj -target "All" -buildstyle "Deployment" build || exit

echo "Done."
