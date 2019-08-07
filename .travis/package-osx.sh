#!/bin/sh

mkdir -p $HOME/artifacts

cd $TRAVIS_BUILD_DIR/BUILD/Install/SuperCollider
echo "Removing Finder cruft..."
xattr -cr SuperCollider.app
echo "Attempting codesigning"
security set-key-partition-list -S apple-tool:,apple: -s -k $SC_KEYCHAIN_PASSWORD supercollider.keychain
codesign --deep --force --verify --verbose --sign "Developer ID Application: Joshua Parmenter" SuperCollider.app
cd ..
echo "Packaging..."
zip -q -r --symlinks $HOME/artifacts/SC-$TRAVIS_COMMIT.zip SuperCollider
echo "Copying build!"
cp $HOME/artifacts/SC-$TRAVIS_COMMIT.zip $HOME/SuperCollider-$VERSION_TO_BUILD-macOS.zip
echo "Done!"
