#!/bin/sh

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
