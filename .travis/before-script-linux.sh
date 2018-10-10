#!/bin/bash

$TRAVIS_BUILD_DIR/.travis/lint.sh $TRAVIS_BUILD_DIR
if [[ -n "$1" && "$1" == "--qt=true" ]]; then
    source /opt/qt59/bin/qt59-env.sh
    if [[ -n "$2" && "$2" == "--webengine=false" ]]; then
        SC_USE_WEBENGINE=OFF
    elif [[ -n "$2" && "$2" == "--webengine=true" ]]; then
        SC_USE_WEBENGINE=ON
    fi

    cmake \
        -DSC_EL=OFF \
        -DSC_USE_WEBENGINE=$SC_USE_WEBENGINE \
        -DCMAKE_INSTALL_PREFIX:PATH=$TRAVIS_BUILD_DIR/BUILD/Install \
        -DCMAKE_BUILD_TYPE=Release \
        $TRAVIS_BUILD_DIR --debug-output
else
    cmake \
        -DSC_EL=OFF \
        -DSC_QT=OFF \
        -DSC_IDE=OFF \
        -DCMAKE_INSTALL_PREFIX:PATH=$TRAVIS_BUILD_DIR/BUILD/Install \
        -DCMAKE_BUILD_TYPE=Release \
        $TRAVIS_BUILD_DIR --debug-output
fi
