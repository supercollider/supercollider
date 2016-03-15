#!/bin/sh
# shell script to extract subset of boost
#
# run from the root of the supercollider repository
#
# required argument: root of the boost source tree

BOOST_ROOT=$1

bcp --boost=${BOOST_ROOT} \
	--scan common/*pp \
	--scan lang/*/*pp \
	--scan server/supernova/*/*pp \
	--scan editors/sc-ide/*/*pp \
	--scan editors/sc-ide/*/*/*pp \
	--scan external_libraries/boost*/*/*pp \
	--scan external_libraries/boost*/*/*/*pp \
	--scan external_libraries/boost*/*/*/*/*pp \
	--scan external_libraries/boost_sync/*/*/*/*pp \
	--scan external_libraries/boost_sync/*/*/*/*/*pp \
	--scan testsuite/supernova/*pp \
	external_libraries/boost

# nor do we need tests and the bjam build files
rm -rf external_libraries/boost/Jamroot
rm -rf external_libraries/boost/libs/*/build
rm -rf external_libraries/boost/libs/*/test
