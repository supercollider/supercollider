#!/bin/bash

if [[ $TRAVIS_OS_NAME == linux ]]; then
	export SCLANG=$TRAVIS_BUILD_DIR/BUILD/Install/bin/sclang
else
	export SCLANG=$TRAVIS_BUILD_DIR/BUILD/Install/SuperCollider/SuperCollider.app/Contents/MacOS/sclang
fi

if [[ $QT != true ]]; then $TRAVIS_BUILD_DIR/.travis/fix-non-Qt.sh; fi

$TRAVIS_BUILD_DIR/testsuite/sclang/launch_test.py $SCLANG \
	|| (echo "ERROR: Test launch failed." && exit 4)

if [[ $QT == true ]]
then
	$TRAVIS_BUILD_DIR/.travis/run-sc-class-library-test-suite.sh \
		|| (echo "ERROR: Class library unit tests failed." && exit 7)
fi
