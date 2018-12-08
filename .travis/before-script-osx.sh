#!/bin/sh

cmake -G"Xcode" \
    -DRULE_LAUNCH_COMPILE=ccache \
    -DCMAKE_PREFIX_PATH=`brew --prefix qt5` \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DSUPERNOVA=ON \
    $TRAVIS_BUILD_DIR --debug-output
