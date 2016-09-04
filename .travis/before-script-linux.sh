#!/bin/bash

./lint.sh $TRAVIS_BUILD_DIR
if [[ -n "$1" && "$1" == "--qt=true" ]]; then
	source /opt/qt55/bin/qt55-env.sh
	cmake -DSC_EL=OFF -DCMAKE_INSTALL_PREFIX:PATH=$TRAVIS_BUILD_DIR/BUILD/Install -DCMAKE_BUILD_TYPE=Release $TRAVIS_BUILD_DIR --debug-output
else
	cmake -DSC_EL=OFF -DSC_QT=OFF -DSC_IDE=OFF -DCMAKE_INSTALL_PREFIX:PATH=$TRAVIS_BUILD_DIR/BUILD/Install -DCMAKE_BUILD_TYPE=Release $TRAVIS_BUILD_DIR --debug-output
fi
