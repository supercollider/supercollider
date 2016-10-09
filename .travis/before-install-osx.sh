#!/bin/sh

brew update
brew tap homebrew/versions
brew outdated cmake || brew upgrade cmake
brew install libsndfile python || true
brew install qt55 || true
brew link qt55 --force
