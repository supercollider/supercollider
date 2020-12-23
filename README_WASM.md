# WebAssembly

This file documents the current work on getting scsynth to build for [wasm](https://webassembly.org/).

One needs to install [emscripten](https://emscripten.org), see [download instructions](https://emscripten.org/docs/getting_started/downloads.html).
What worked for me is the installation described there:

    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh

Then you prefix `cmake` by `emcmake`, e.g.:

    $ emcmake cmake -DSC_EL=no -DSUPERNOVA=no -DSC_HIDAPI=no -DNO_LIBSNDFILE=yes -DSC_QT=no -DNO_AVAHI=yes -DSC_ABLETON_LINK=no -DCMAKE_BUILD_TYPE="Debug" -Wno-dev -s USE_PTHREADS=1 -s WASM=1 -DPTHREADS_LIBRARY=ignore -DSSE=no -DSSE2=no -DNO_X11=yes --target scsynth ..

I.e.

    $ mkdir build
    $ cd build
    $ ../wasm/configure.sh
    $ make scsynth

(Or as a shortcut `../wasm/build.sh`).

This builds `server/scsynth/scsynth.*` with a runtime library `*.js`, the actual binary `*.wasm` and a wrapping HTML `*.html`. Start a local webserver:

    $ python -m SimpleHTTPServer

Open `0.0.0.0:8000` and click on `scsynth.html`. Currently, this fails at `SC_AudioDriver::Setup()`.

## Modifications

There are `ifdef`s in nova-tt `thread_priority_mach.hpp` and `thread_priority_pthread.hpp` to bypass unsupported API.

There is a dummy no-op implementation of `SC_WebAudio.cpp` now.

-----

## Deliberations on the Audio API

There are currently three options for Audio API on wasm: plain Web Audio API, OpenAL which has a back-end provided by emscripten, and SDL2, which apparently has a port that works with emscripten.
Currently the API string is `webaudio` -- since Jack and PortAudio already cover a variety of platforms, I see
no big benefit in going through an extra indirection with OpenAL or SDL2 -- these are basically options if you want to port an existing C application to wasm, but since we have to write the
driver from scratch, it seems natural to skip this indirection and go for Web Audio API direct support instead.

The emscripten docs already show how to use the `EM_ASM` macros to invoke JavaScript calls from C. This would probably be the glue needed to set up the audio worklet processor.

Similar to `SC_PortAudio.cpp` and `SC_Jack.cpp`, we added another file `SC_WebAudio.cpp` to `server/scsynth/` and registered it in `server/scsynth/CMakeList.txt`.
In this file, the sub-class of `SC_AudioDriver` will be implemented.

## Notes

There is a problem with pthreads and shared memory:

```
wasm-ld: error: --shared-memory is disallowed by SC_ComPort.cpp.o because it was not compiled with 'atomics' or 'bulk-memory' features.
```

See https://github.com/emscripten-core/emscripten/issues/8503

Although the linked issues say they are all fixed; perhaps this is a problem a library that was already compiled? Not sure what is going on.
We have now a workaround in `CMakeList.txt`: `-Wl,--shared-memory,--no-check-features` ; this is considered dangerous.

