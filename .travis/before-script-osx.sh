#!/bin/sh

cmake -G"Xcode" -DCMAKE_PREFIX_PATH=`brew --prefix qt55` -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 $TRAVIS_BUILD_DIR --debug-output
