#!/bin/sh


# Create the keychain with a password
security create-keychain -p travis ios-build.keychain

# Make the custom keychain default, so xcodebuild will use it for signing
security default-keychain -s ios-build.keychain

# Unlock the keychain
security unlock-keychain -p travis ios-build.keychain

# Add certificates to keychain and allow codesign to access them
security import ../certs/distribution.p12 -k ~/Library/Keychains/ios-build.keychain -P $KEY_PASSWORD -T /usr/bin/codesign

security set-key-partition-list -S apple-tool:,apple: -s -k travis ios-build.keychain

cmake -G"Xcode" \
    -DRULE_LAUNCH_COMPILE=ccache \
    -DCMAKE_PREFIX_PATH=`brew --prefix qt5` \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DSUPERNOVA=ON \
    $TRAVIS_BUILD_DIR --debug-output
