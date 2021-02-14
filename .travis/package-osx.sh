#!/bin/sh

mkdir -p $HOME/artifacts

cd $TRAVIS_BUILD_DIR/BUILD/Install
zip -q -r --symlinks $HOME/artifacts/$S3_ARTIFACT_NAME.zip SuperCollider
cp $HOME/artifacts/$S3_ARTIFACT_NAME.zip $HOME/$GITHUB_ARTIFACT_NAME.zip
