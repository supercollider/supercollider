#!/bin/sh

mkdir -p $HOME/uploads

cd $TRAVIS_BUILD_DIR/package
./create_package.sh -v $VERSION_TO_BUILD
./create_package.sh -v $VERSION_TO_BUILD -l
mv -v SuperCollider-*.tar.bz2 $HOME/uploads/
