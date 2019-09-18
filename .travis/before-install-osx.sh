#!/bin/sh

export HOMEBREW_NO_ANALYTICS=1

# according to https://docs.travis-ci.com/user/caching#ccache-cache
brew install ccache
export PATH="/usr/local/opt/ccache/libexec:$PATH"

brew install libsndfile || true
brew install portaudio || true
brew install dyfer/fftw-sc/fftw-sc || true
brew install qt5 || true
brew link qt5 --force

export FFTW3_DIR=`brew --prefix fftw-sc`

# To get less noise in xcode output
gem install xcpretty
