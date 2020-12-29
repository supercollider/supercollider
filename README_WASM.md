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

Copy the .js and .wasm files from `server/scsynth/` to `../wasm/`.

__Or as a shortcut `../wasm/build.sh` which in turn is a shortcut for `../wasm/configure.sh` followed by `../wasm/make.sh`.__

Sometimes, when running `make.sh` for a while, there appears an error:

> System LLVM compiler cannot be used to build with Emscripten! Check

I don't know what the reason is. Removing and recreating the `build` directory and running the full build again "solves" this.

This builds `server/scsynth/scsynth.*` with a runtime library `*.js`, the actual binary `*.wasm` and a wrapping HTML `*.html`. Start a local webserver:

    $ cd ../wasm/
    $ python -m SimpleHTTPServer

Open `0.0.0.0:8000`.

## Modifications

There are `ifdef`s in nova-tt `thread_priority_mach.hpp` and `thread_priority_pthread.hpp` to bypass unsupported API.

There is a dummy no-op implementation of `SC_WebAudio.cpp` now.

## Status

```
SC FFT global init: cosTable initialised.
SC_WebAudio: constructor.
SC_AudioDriver::Setup(): thread
SC_AudioDriver::Setup(): DriverSetup
SC_WebAudio: DriverSetup.
SC_AudioDriver: sample rate = 0.000000, driver's block size = 0
SC_WebAudio: DriverStart.
```

Uncaught exception `30895544`, no stack trace.

We should probably set the sampling rate next.

## Temporary stuff

`-s DISABLE_EXCEPTION_CATCHING=0` -- this allegedly causes performance issues, so should eventually be removed again.

We were back to this:

```
scsynth.html:1246 Assertion failed: priv_end_block() == end_block, at: /home/hhrutz/Documents/devel/supercollider/external_libraries/boost/boost/interprocess/mem_algo/rbtree_best_fit.hpp,410,priv_add_segment
printErr	@	scsynth.html:1246
abort	@	scsynth.js:1475
___assert_fail	@	scsynth.js:2082
boost::interprocess::rbtree_best_fit<boost::interprocess::mutex_family, boost::interprocess::offset_ptr<void, long, unsigned long, 0ul>, 0ul>::priv_add_segment(void*, unsigned long)	@	scsynth.wasm:1
...
```

Without anyone with proper CMake knowledge, I'm giving up for now. The assertion is disabled in `rbtree_best_fit.hpp`, line 410 now.

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

There was a problem with pthreads and shared memory:

```
wasm-ld: error: --shared-memory is disallowed by SC_ComPort.cpp.o because it was not compiled with 'atomics' or 'bulk-memory' features.
```

See https://github.com/emscripten-core/emscripten/issues/8503

Although the linked issues say they are all fixed; perhaps this is a problem a library that was already compiled? Not sure what is going on.
We have now a workaround in `CMakeList.txt`: `-Wl,--shared-memory,--no-check-features` ; this is considered dangerous.

__Fixed:__ I think this was due to missing linker flags for `libscsynth`; it seems gone now, we no longer need the `--no-check-features`
