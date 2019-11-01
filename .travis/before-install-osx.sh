#!/bin/sh

export HOMEBREW_NO_ANALYTICS=1

brew install libsndfile || true
brew install portaudio || true
brew install ccache || true
brew install qt5 || true
brew link qt5 --force

# according to https://docs.travis-ci.com/user/caching#ccache-cache
export PATH="/usr/local/opt/ccache/libexec:$PATH"

# To get less noise in xcode output
gem install xcpretty
