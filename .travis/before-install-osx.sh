#!/bin/sh

export HOMEBREW_NO_ANALYTICS=1

brew install libsndfile || brew install libsndfile || exit 1
brew install portaudio || exit 2
brew install ccache || exit 3
brew install qt5 || exit 4
brew link qt5 --force || exit 5
brew install fftw --verbose # temp allow

if $USE_SYSLIBS; then
    # boost is already installed
    brew install yaml-cpp || exit 7
fi

# according to https://docs.travis-ci.com/user/caching#ccache-cache
export PATH="/usr/local/opt/ccache/libexec:$PATH"

# To get less noise in xcode output
gem install xcpretty
