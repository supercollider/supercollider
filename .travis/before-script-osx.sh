#!/bin/sh


# Create the keychain with a password
security create-keychain -p $SC_KEYCHAIN_PASSWORD supercollider.keychain

# Make the custom keychain default, so xcodebuild will use it for signing
security default-keychain -s supercollider.keychain

# Unlock the keychain
security unlock-keychain -p $SC_KEYCHAIN_PASSWORD supercollider.keychain

# Add certificates to keychain and allow codesign to access them
security import ../certs/distribution.p12 -k ~/Library/Keychains/supercollider.keychain -P $KEY_PASSWORD -T /usr/bin/codesign -T /usr/bin/security

security set-key-partition-list -S apple-tool:,apple: -s -k $SC_KEYCHAIN_PASSWORD supercollider.keychain

cmake -G"Xcode" \
    -DRULE_LAUNCH_COMPILE=ccache \
    -DCMAKE_PREFIX_PATH=`brew --prefix qt5` \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DSUPERNOVA=ON \
    $TRAVIS_BUILD_DIR --debug-output
