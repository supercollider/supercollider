#!/bin/sh

# Keep a raw log of cmake's output
BUILD_LOG=$TRAVIS_BUILD_DIR/BUILD/raw_build.log
XCPRETTY='xcpretty --simple --no-utf --no-color'

# Make cmake failure an overall failure
set -o pipefail
cmake --build $TRAVIS_BUILD_DIR/BUILD --config Release --target install | tee $BUILD_LOG | $XCPRETTY
CMAKE_EXIT=$?
set +o pipefail

pushd $TRAVIS_BUILD_DIR/BUILD/Install
# Manually codesign. Without this, there will be present but invalid code signatures on SuperCollider.app and
# SuperCollider.app/Contents/MacOS/SuperCollider.
CODESIGN='/usr/bin/codesign --sign - --force --verbose=4'
# select executables, *.app's, dylibs, and frameworks
find SuperCollider.app \( \( -type f -a -perm -1 \) -o \( -name '*.app' \) -o \( -name '*.dylib' \) -o \
    \( -name '*.framework' \) \) -exec $CODESIGN {} \;
$CODESIGN SuperCollider.app/Contents/Frameworks/QtWebEngineCore.framework
$CODESIGN SuperCollider.app/Contents/MacOS/SuperCollider
$CODESIGN SuperCollider.app

# If any signing fails, then we will be unable to sign SuperCollider.app
/usr/bin/codesign --verify --verbose=4 SuperCollider.app || exit 1
popd

if [[ $CMAKE_EXIT != 0 ]]; then
    echo "Build failed. Log:"
    cat $BUILD_LOG
fi

exit $CMAKE_EXIT
