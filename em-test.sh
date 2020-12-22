#!/bin/bash
emcmake cmake -DSC_WII=no -DSC_EL=no -DSUPERNOVA=no -DSC_HIDAPI=no -DNO_LIBSNDFILE=yes -DSC_QT=no -DNO_AVAHI=yes -DSC_ABLETON_LINK=no -DCMAKE_BUILD_TYPE="Debug" -Wno-dev -s USE_PTHREADS=1 -s WASM=1 -DPTHREADS_LIBRARY=ignore -DSSE=no -DSSE2=no -DNO_X11=yes --target scsynth ..
