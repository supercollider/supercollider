#!/bin/sh

export HOMEBREW_NO_ANALYTICS=1
export HOMEBREW_NO_AUTO_UPDATE=1
export HOMEBREW_NO_INSTALL_CLEANUP=1

if ! $IS_LEGACY_BUILD; then
    #run update first so that possible update errors won't hold up package installation
    brew update --preinstall
fi

brew install libsndfile || exit 1
brew install portaudio || exit 2
brew install ccache || exit 3
brew install ninja || exit 3
if $IS_LEGACY_BUILD; then
    brew install supercollider/formulae/qt@5.9.3 --force || exit 4
else
    brew upgrade qt5 || exit 4
fi
brew install fftw # do not abort in this step - fftw dependency install may fail, but this is not fatal

if $USE_SYSLIBS; then
    # boost is already installed
    brew install yaml-cpp || exit 7
fi

# according to https://docs.travis-ci.com/user/caching#ccache-cache
export PATH="/usr/local/opt/ccache/libexec:$PATH"

# To get less noise in xcode output
gem install xcpretty
