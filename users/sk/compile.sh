#!/bin/sh

cd SC3synth.pbproj && pbxbuild && cd ..
cd SC3plugins.pbproj && pbxbuild && cd ..
cd SC3lang.pbproj && pbxbuild && cd ..

