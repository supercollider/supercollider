#!/bin/sh

cd `dirname $0`/../../

# ---------------------------------------------------------- #
# DynCall build script using Makefile.generic on Minix 3.1.8 #
# ---------------------------------------------------------- #

# build libs and tests, but exclude dynload

./configure
make libdyncall libdyncallback tests-libdyncallback tests-libdyncall

