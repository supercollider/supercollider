#!/bin/sh

brew update
brew tap homebrew/versions
brew outdated cmake || brew upgrade cmake

# according to https://docs.travis-ci.com/user/caching#ccache-cache
brew install ccache
export PATH="/usr/local/opt/ccache/libexec:$PATH"

brew install libsndfile || true
brew install qt5 || true
brew link qt5 --force

# To get less noise in xcode output
gem install xcpretty