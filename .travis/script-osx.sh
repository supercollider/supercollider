#!/bin/sh

# Keep a raw log of cmake's output
BUILD_LOG=$TRAVIS_BUILD_DIR/BUILD/raw_build.log
XCPRETTY='xcpretty --simple --no-utf --no-color'

# Make cmake failure an overall failure
set -o pipefail
cmake --build $TRAVIS_BUILD_DIR/BUILD --config Release --target install | tee $BUILD_LOG | $XCPRETTY
CMAKE_EXIT=$?
set +o pipefail

if [[ $CMAKE_EXIT != 0 ]]; then
    echo "Build failed. Log:"
    cat $BUILD_LOG
fi

exit $CMAKE_EXIT
