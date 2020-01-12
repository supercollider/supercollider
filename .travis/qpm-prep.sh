#!/bin/bash

cd $TRAVIS_BUILD_DIR/BUILD
sudo pip2 install git+https://github.com/supercollider/qpm.git

mkdir $HOME/Quarks && cd $HOME/Quarks
git clone --depth=1 https://github.com/supercollider-quarks/API
cd $TRAVIS_BUILD_DIR/BUILD

cp ../travis_test_run_proto.json ./travis_test_run.json
