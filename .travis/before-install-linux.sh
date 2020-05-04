#!/bin/bash

if $DO_LINT; then
    pushd . >/dev/null
    cd ..
    if ! [[ -d "downloads" ]]
    then
       mkdir downloads
    fi
    cd downloads
    RELEASE_NAME=clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-14.04
    if ! [[ -d "$RELEASE_NAME" ]]
    then
       echo "Downloading and extracting Clang 8.0.0"
       curl -o clang.tar.xz https://releases.llvm.org/8.0.0/$RELEASE_NAME.tar.xz
       tar xf clang.tar.xz
    else
       echo "Using Clang 8.0.0 from cache"
    fi
    cd $RELEASE_NAME/bin
    export PATH=$(pwd):$PATH
    echo "Added clang-format in $(pwd) to PATH"
    popd >/dev/null

    echo "Running tools/clang-format.py lintall"
    tools/clang-format.py lintall || exit 1
    echo "Lint successful"
fi

# Install required clang toolchain
if [[ -n "$CLANG_VERSION" ]]; then
    wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    echo "deb https://apt.llvm.org/$TRAVIS_DIST/ llvm-toolchain-$TRAVIS_DIST-$CLANG_VERSION main" | sudo tee -a /etc/apt/sources.list
    sudo apt-get update
    sudo apt-get install --yes clang-$CLANG_VERSION
    # Clang 4.0-6.0 didn't package libstdc++ as far as I can tell; so we use libc++ instead
    if [[ ! "$CLANG_VERSION" =~ "^[456].0$" ]]; then
        sudo apt-get install --yes libc++-$CLANG_VERSION-dev libc++abi-$CLANG_VERSION-dev
    fi
fi

npm install -g lintspaces-cli
sudo add-apt-repository --yes ppa:beineri/opt-qt-5.14.1-$TRAVIS_DIST
sudo apt-get update
sudo apt-get install --yes build-essential cmake pkg-config libjack-jackd2-dev libsndfile1-dev libasound2-dev libavahi-client-dev libreadline6-dev libfftw3-dev libicu-dev libxt-dev libudev-dev
if [[ -n "$1" && "$1" == "--qt=true" ]]; then
	sudo apt-get install --yes libgl1-mesa-dev qt514base qt514location qt514declarative qt514tools qt514webengine qt514webchannel qt514xmlpatterns qt514svg qt514websockets
fi

if $USE_SYSLIBS; then
    sudo apt-get install --yes libboost-thread-dev libboost-system-dev libboost-filesystem-dev libboost-regex-dev \
        libboost-test-dev libboost-program-options-dev libyaml-cpp-dev
fi
