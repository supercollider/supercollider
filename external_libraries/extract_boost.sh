#!/bin/sh
# shell script to extract subset of boost, that is required for supernova

BOOST_ROOT=/home/tim/t/boost_1_45_0/

bcp --boost=${BOOST_ROOT} --scan ../server/supernova/*/*pp boost

# we don't use regex and date_time
rm -rf external_libraries/boost/*/regex
rm -rf external_libraries/boost/*/date_time

# nor do we need tests and the bjam build files
rm -rf external_libraries/boost/Jamroot
rm -rf external_libraries/boost/libs/*/build
rm -rf external_libraries/boost/libs/*/test