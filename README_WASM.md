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

-----

## Deliberations on the Audio API

There are currently three options for Audio API on wasm: plain Web Audio API, OpenAL which has a back-end provided by emscripten, and SDL2, which apparently has a port that works with emscripten.
Currently the API string is `openal`, but it would have to be changed to `webaudio` or `sdl2` if we decide on one of these. Since Jack and PortAudio already cover a variety of platforms, I see
no big benefit in going through an extra indirection with OpenAL or SDL2 -- these are basically options if you want to port an existing C application to wasm, but since we have to write the
driver from scratch, it seems natural to skip this indirection and go for Web Audio API direct support instead.

The emscripten docs already show how to use the `EM_ASM` macros to invoke JavaScript calls from C. This would probably be the glue needed to set up the audio worklet processor.

Similar to `SC_PortAudio.cpp` and `SC_Jack.cpp`, we would add another file `SC_WebAudio.cpp` to `server/scsynth/` and register it in `server/scsynth/CMakeList.txt`.
In this file, the sub-class of `SC_AudioDriver` will be implemented.

