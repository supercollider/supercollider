#!/bin/bash

if $DO_LINT; then
    pushd . >/dev/null
    cd ..
    echo "Downloading and extracting Clang 8.0.0"
    RELEASE_NAME=clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-14.04
    curl -o clang.tar.xz https://releases.llvm.org/8.0.0/$RELEASE_NAME.tar.xz
    tar xf clang.tar.xz
    cd $RELEASE_NAME/bin
    export PATH=$(pwd):$PATH
    echo "Added clang-format in $(pwd) to PATH"
    popd >/dev/null

    echo "Running tools/clang-format.py lintall"
    tools/clang-format.py lintall || exit 1
    echo "Lint successful"
fi

npm install -g lintspaces-cli
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test
sudo add-apt-repository --yes ppa:beineri/opt-qt591-trusty
sudo apt-get update
sudo apt-get install --yes build-essential gcc-4.9 g++-4.9 cmake pkg-config libjack-jackd2-dev libsndfile1-dev libasound2-dev libavahi-client-dev libreadline6-dev libfftw3-dev libicu-dev libxt-dev libudev-dev
if [[ -n "$1" && "$1" == "--qt=true" ]]; then
	sudo apt-get install --yes libgl1-mesa-dev qt59base qt59location qt59declarative qt59tools qt59webengine qt59webchannel qt59xmlpatterns qt59svg qt59websockets
fi
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.9
sudo update-alternatives --auto gcc
