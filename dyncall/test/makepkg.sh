#!/bin/sh
# script to package some tests for quick deployment (useful for embedded stuff)

TOP=..
. $TOP/ConfigVars

TESTS="callf ellipsis malloc_wx plain plain_c++ suite suite2 suite3 suite_floats callback_plain callback_suite"
# addition test:
#
# callback_plain callback_suite callf
# ellipsis
# malloc_wx thunk
# nm
# plain suite suite2 suite3
# suite2_x86win32fast suite2_x86win32std suite_x86win32fast suite_x86win32std

TARGET="${CONFIG_OS}_${CONFIG_ARCH}_${CONFIG_TOOL}_${CONFIG_CONFIG}"
BUILD_DIR=build_out

mkdir -p pack_out/testpkg
for I in $TESTS ; do
cp $I/$BUILD_DIR/$TARGET/$I pack_out/testpkg
done
tar -cvzf test-pack.tar.gz -C pack_out testpkg
