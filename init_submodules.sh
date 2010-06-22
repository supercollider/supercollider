#!/bin/sh
git submodule init
git submodule update

./libs/nova-simd/init_submodules.sh
