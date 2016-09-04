#!/bin/bash

./lint.sh $TRAVIS_BUILD_DIR
cmake -DSC_EL=no -DCMAKE_INSTALL_PREFIX:PATH=$TRAVIS_BUILD_DIR/build/Install -DCMAKE_BUILD_TYPE=Release $TRAVIS_BUILD_DIR --debug-output
