#!/bin/sh

brew update
brew tap homebrew/versions
brew outdated cmake || brew upgrade cmake
brew install libsndfile python || true
brew install qt5 || true
brew link qt5 --force
