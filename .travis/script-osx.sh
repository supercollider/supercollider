#!/bin/sh

cmake --build $TRAVIS_BUILD_DIR/BUILD --config Release --target install | tee $TRAVIS_BUILD_DIR/BUILD/build.log | xcpretty -c
