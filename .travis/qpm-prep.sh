#!/bin/bash

cd $TRAVIS_BUILD_DIR/BUILD
sudo pip2 install git+https://github.com/scztt/qpm.git@qpm-unit

mkdir $HOME/Quarks && cd $HOME/Quarks
git clone --depth=1 https://github.com/supercollider-quarks/API
git clone --depth=1 https://github.com/supercollider-quarks/CommonTests
git clone --depth=1 https://github.com/supercollider-quarks/CommonTestsGUI
cd $TRAVIS_BUILD_DIR/BUILD

cp ../travis_test_run_proto.json ./travis_test_run.json
