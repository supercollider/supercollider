# WebAssembly

This file documents the current work on getting scsynth to build for [wasm](https://webassembly.org/).

## Overview

The SuperCollider server (scsynth) can be embedded on the browser's client-side using the WebAssembly architecture.
The build process is similar to the regular CMake build, but using the Emscripten SDK instead. The following limitations
are currently in place:

- libsndfile is disabled, thus streaming sound files and using UGens such as `DiskIn` does not work. This can be 
  solved in the future
- the deprecated `ScriptProcessorNode` Web Audio API is used, which means that the DSP loop runs in the main JavaScript
  thread, being prone to drop outs as the browser's user interface is used. In the future, one might look into
  using audio worklets -- but this complicates the build process. Emscripten already uses different workers for the
  phtreads, and it is unclear how pthreads and audio worklets would coexist.
- enabling pthreads means that a so-called `SharedArrayBuffer` is used for the static heap memory of the application.
  This feature is available on Chrome/Chromium, but in Firefox it is disabled for security reasons unless your site
  sets [COOP and COEP](https://web.dev/coop-coep/) for strong isolation. I didn't manage to do that on my own site
  purely through `.htaccess`, so the demo has to run in Chrome/Chromium for now.
- OSC return/reply messages are not yet implemented
- web audio context only does processing when there has been user-interaction. This is solved in the demo by requiring
  the user to press a 'boot' button first.
- I/O is currently hard-coded for 0 input channels and 2 output channels. This is easy to fix.

## Building

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
    $ ../wasm/make.sh

This takes care of copying the .js and .wasm files from `server/scsynth/` to `../wasm/`. The `../wasm/build.sh` script
runs `configure.sh` and `make.sh` after another.

Sometimes, when running `make.sh` for a while, there appears an error:

> System LLVM compiler cannot be used to build with Emscripten! Check

I don't know what the reason is. Removing and recreating the `build` directory and running the full build again "solves" this.

Technically, what is built is `server/scsynth/scsynth.*` with a runtime library `*.js`, the actual binary `*.wasm` and a wrapping HTML `*.html`.
The standard plugins (UGens) are statically linked and thus available.

## Running

To see the server running in the browser, start a local webserver:

    $ cd ../wasm/
    $ python -m SimpleHTTPServer

Then open `0.0.0.0:8000`.

The `index.html` includes a [JavaScript OSC library](https://github.com/colinbdclark/osc.js) and defines a few utility functions:

    sendOSC(name, args...)

    sendOSC_t(name, typeTags, args...)

Can be used to talk to the server after it has been booted. For example, you can print the node tree:

    sendOSC("/g_dumpTree", 0, 1)

or

    sendOSC_t("/g_dumpTree", "ii", 0, 1)

Some of the SuperCollider OSC API is immune to JS related confusion of float and int types, while some commands do not properly function without giving the correct type-tags (without type tages, the default is `"f"` float for any JS number).

    d_bubbles()

Sends a `/d_recv` command with the well-known `"AnalogBubbles"` example by James McCartney. You can then start a synth using

    s_bubbles()

which is equivalent to

    sendOSC_t("/s_new", "siii", "AnalogBubbles", 1000, 1, 0)

If you want to use other synth-defs, you'll have to compile them to their binary blobs (as `Uint8rray`) and send them like so

    sendOSC_t("/d_recv", "b", synthDefBlob)

To enable OSC dump:

    dumpOSC(1)

To set controls on a synth:

    sendOSC_t("/n_set", "isf", 1000, "freq2", 444.1)

To free the synth:

    sendOSC_t("/n_free", "i", 1000)


## Modifications

There are `ifdef`s in nova-tt `thread_priority_mach.hpp` and `thread_priority_pthread.hpp` to bypass unsupported API.
For this reason, a fork of nova-tt is used in git-modules. It may be that these changes are no longer necessary (TODO: check).

## Temporary stuff and performance

The current build is not optimised for speed or code size. This is a task for the future.

`-s DISABLE_EXCEPTION_CATCHING=0` -- this allegedly causes performance issues, so should eventually be removed again.

I was seeing an assertion error in boost:

```
scsynth.html:1246 Assertion failed: priv_end_block() == end_block, at: /home/hhrutz/Documents/devel/supercollider/external_libraries/boost/boost/interprocess/mem_algo/rbtree_best_fit.hpp,410,priv_add_segment
printErr	@	scsynth.html:1246
abort	@	scsynth.js:1475
___assert_fail	@	scsynth.js:2082
boost::interprocess::rbtree_best_fit<boost::interprocess::mutex_family, boost::interprocess::offset_ptr<void, long, unsigned long, 0ul>, 0ul>::priv_add_segment(void*, unsigned long)	@	scsynth.wasm:1
...
```

Without further investigation, the assertion is now disabled in `rbtree_best_fit.hpp`, line 410.
