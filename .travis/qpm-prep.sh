#!/bin/bash

cd $TRAVIS_BUILD_DIR/BUILD
sudo pip install git+https://github.com/scztt/qpm.git@qpm-unit
qpm quark checkout CommonTests CommonTestsGUI --location $HOME/Quarks
cp ../travis_test_run_proto.json ./travis_test_run.json
