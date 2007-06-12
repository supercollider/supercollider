#!/bin/sh

# This script builds the server, the plugins and the application (sclang), one after another.

# scsynth
echo "Building scsynth..."
xcodebuild -project Synth.xcodeproj -target "All" -configuration "Deployment" build || exit

# plugins
echo "Building plugins..."
xcodebuild -project Plugins.xcodeproj -target "All" -configuration "Deployment" build || exit

#sclang
echo "Building sclang..."
xcodebuild -project Language.xcodeproj -target "All" -configuration "Deployment" build || exit

echo "Done."
