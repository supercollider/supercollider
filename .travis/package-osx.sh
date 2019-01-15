#!/bin/sh

mkdir -p $HOME/artifacts

cd $TRAVIS_BUILD_DIR/BUILD/Install
zip -q -r --symlinks $HOME/artifacts/SC-$TRAVIS_COMMIT.zip SuperCollider
cp $HOME/artifacts/SC-$TRAVIS_COMMIT.zip $HOME/SuperCollider-$VERSION_TO_BUILD-macOS.zip
