#!/bin/sh

cd `dirname $0`/../../

# ------------------------------------------------------------- #
# DynCall build script using Makefile.generic for PSP toolchain #
# ------------------------------------------------------------- #

# build libs and tests, but exclude dynload

./configure --target=PSP
#no callback for mips, yet, but build 'empty' libdyncallback to make linker happy
make libdyncall libdyncallback tests-libdyncall # tests-libdyncallback

