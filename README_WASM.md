# WebAssembly

This file documents the current work on getting scsynth to build for [wasm](https://webassembly.org/).

One needs to install [emscripten](https://emscripten.org), see [download instructions](https://emscripten.org/docs/getting_started/downloads.html).
What worked for me is the installation described there:

    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh

Then you prefix `cmake` by `emcmake`, e.g.:

    $ emcmake cmake -DSC_WII=no -DSC_EL=no -DSUPERNOVA=no -DSC_HIDAPI=no -DNO_LIBSNDFILE=yes -DSC_QT=no -DNO_AVAHI=yes -DSC_ABLETON_LINK=no -DCMAKE_BUILD_TYPE="Debug" -Wno-dev -s USE_PTHREADS=1 -s WASM=1 -DPTHREADS_LIBRARY=ignore -DSSE=no -DSSE2=no -DNO_X11=yes --target scsynth ..

I.e.

    $ mkdir build
    $ cd build
    $ ../em-test.sh
    $ make

Currently make finishes with errors relating to the following missing symbols:

 - `oscTimeNow` ; declared in `SC_CoreAudio.h` ("Functions to be implemented by the driver backend")
 - `server_timeseed` ; dito.
 - `initializeScheduler` ; dito.
 - `SC_NewAudioDriver(struct World*)` ; dito.
 - `pthread_setschedparam` ; defined in `<pthread.h>`, used in  `external_libraries` > `nova-tt` > `nova-tt` > `thread_priority_mach.hpp`
 - `sched_get_priority_max` ; defined in `<sched.h>`, used in `external_libraries` > `nova-tt` > `nova-tt` > `thread_priority_pthread.hpp`
 - `sched_get_priority_min` ; dito.

