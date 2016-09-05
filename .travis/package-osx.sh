#!/bin/sh

mkdir -p $HOME/artifacts

cd $TRAVIS_BUILD_DIR/BUILD/Install
zip -q -r $HOME/artifacts/SC-$TRAVIS_COMMIT.zip SuperCollider
