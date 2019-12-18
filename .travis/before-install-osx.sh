#!/bin/sh

export HOMEBREW_NO_ANALYTICS=1

travis_retry brew install libsndfile || exit 1
travis_retry brew install portaudio || exit 2
travis_retry brew install ccache || exit 3
travis_retry brew install qt5 || exit 4
travis_retry brew link qt5 --force || exit 5

# according to https://docs.travis-ci.com/user/caching#ccache-cache
export PATH="/usr/local/opt/ccache/libexec:$PATH"

# To get less noise in xcode output
gem install xcpretty
