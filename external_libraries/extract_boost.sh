#!/bin/sh
# shell script to extract subset of boost, that is required for supernova
#
# run from the root of the supercollider repository

# root of the boost source tree
BOOST_ROOT=/home/tim/nightly/boost_1_46_0/

bcp --boost=${BOOST_ROOT} \
	--scan server/supernova/*/*pp \
	--scan external_libraries/boost_*/*/*pp \
	--scan external_libraries/boost_*/*/*/*pp \
	--scan external_libraries/boost_*/*/*/*/*pp \
	--scan testsuite/supernova/*pp \
	external_libraries/boost

# nor do we need tests and the bjam build files
rm -rf external_libraries/boost/Jamroot
rm -rf external_libraries/boost/libs/*/build
rm -rf external_libraries/boost/libs/*/test
