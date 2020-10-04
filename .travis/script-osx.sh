#!/bin/sh

# Keep a raw log of cmake's output
BUILD_LOG=$TRAVIS_BUILD_DIR/BUILD/raw_build.log
XCPRETTY='xcpretty --simple --no-utf --no-color'

# Make cmake failure an overall failure
set -o pipefail
cmake --build $TRAVIS_BUILD_DIR/BUILD --config Release --target install -- -v
CMAKE_EXIT=$?
set +o pipefail

exit $CMAKE_EXIT
