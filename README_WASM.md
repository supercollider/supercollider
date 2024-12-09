# SuperCollider on WebAssembly

This file documents the current status on getting the project to build for [WebAssembly](https://webassembly.org/) (wasm).

## Introduction

WebAssembly is a near-native format for running code on the client-side of the browser. That is, similar to JavaScript for
which a runtime environment is provided virtually by every mainstream browser, this relatively new standard competes in a
similar space, allowing for more performance oriented code to be executed and targetting lower-level languages such as
C or C++.

Compiling SuperCollider to wasm means that a real-time generative sound piece can run in a user's browser without
the need to install any software or to set up a streaming server. Other computer music systems like PD and CSound have also
explored this possibility. Essentially, one takes an existing code base and adapts it to use the
[Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API) where
normally a driver such as Port Audio or Jack would sit. Any interactive "control" of the application then goes through
JavaScript which can bridge to the wasm application.

As a side note, wasm can also run stand-alone in a native container outside of a browser, which may or may not be,
depending on the application, an interesting format to compile to, as it allows to deliver a binary in a cross-platform manner.
Since SuperCollider depends on maximising performance, this avenue is currently not explored, but it may be interesting
for processor architectures which are otherwise not supported by the native machine code builds.

## Overview

Current, the SuperCollider __server__ (scsynth) can be embedded using the WebAssembly architecture. In the future, this
may extend to __sclang__ as well (see 'clients' section).

The build process is similar to the regular CMake build, but using the [Emscripten SDK](https://emscripten.org) instead.
Emscripten allows to build C and C++ applications through a customised LLVM compiler (Clang), emitting wasm binaries.

The following limitations are currently in place:

- _libsndfile_ is disabled, thus accessing sound files and using UGens such as `DiskIn` does not work. This can be 
  solved in the future.
- the X11-based UGens, `MouseX`, `MouseY`, etc., are not available.
- the deprecated [ScriptProcessorNode](https://developer.mozilla.org/en-US/docs/Web/API/ScriptProcessorNode) Web Audio API
  is used, which means that the DSP loop runs in the main JavaScript thread, being prone to drop outs as the browser's
  user interface is used. In the future, one might look into using
  [AudioWorkletProcessor](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorkletProcessor) -- but this complicates
  the build process, as need to generate a dedicated wasm binary per thread, which then need to communicate with each
  other. Emscripten already uses different workers for the
  phtreads, and it is unclear how pthreads and audio worklets would coexist.
- enabling _pthreads_ (the standard UNIX threads used by regular scsynth) means that a so-called `SharedArrayBuffer` 
  is used for the static heap memory of the application. This feature is enabled on Chrome/Chromium, but in Firefox it
  is disabled for security reasons _unless_ your site sets [COOP and COEP](https://web.dev/coop-coep/) for strong isolation.
  That means making the wasm application run in [different browsers](https://caniuse.com/sharedarraybuffer) may pose
  additional efforts in terms of configuring the web server.
- the web [AudioContext](https://developer.mozilla.org/en-US/docs/Web/API/AudioContext) only does processing when there 
  has been _user interaction_, in order to avoid that malicious websites bombard the user with sound without prior
  permission. This is solved by requiring the user to press a 'boot' button first, for example.

## Known Issues

Some UGens seem broken, for example `NumOutputBuses` (and consequently the `SoundIn` pseudo-UGen). Using it causes a
runtime error:

```javascript
Uncaught RuntimeError: function signature mismatch
    at Graph_FirstCalc(Graph*) (http://127.0.0.1:8000/scsynth.wasm:wasm-function[2686]:0x70b98)
    at Group_Calc (http://127.0.0.1:8000/scsynth.wasm:wasm-function[2367]:0x59ef2)
    at World_Run(World*) (http://127.0.0.1:8000/scsynth.wasm:wasm-function[3855]:0xb821f)
    at SC_WebAudioDriver::WaRun() (http://127.0.0.1:8000/scsynth.wasm:wasm-function[11158]:0x43f0df)
    at emscripten::internal::MethodInvoker<void (SC_WebAudioDriver::*)(), void, SC_WebAudioDriver*>::invoke(void (SC_WebAudioDriver::* const&)(), SC_WebAudioDriver*) (http://127.0.0.1:8000/scsynth.wasm:wasm-function[11185]:0x440979)
    at SC_WebAudioDriver$WaRun [as WaRun] (eval at new_ (http://127.0.0.1:8000/scsynth.js:7673:27), <anonymous>:8:1)
    at ScriptProcessorNode.ad.proc.onaudioprocess (http://127.0.0.1:8000/scsynth.js:1699:1707)
```

which corresponds to a segfault on the desktop. The reason might be a 32-bit incompatibility.

## Building

> Building the wasm version of _scsynth_ is only necessary if you want to develop and improve the wasm build of scsynth.
> It is perfectly fine to use the compiled files available via GitHub as everything is statically linked and the wasm binary will run on any wasm runtime.

One needs to install [emscripten](https://emscripten.org), see
[download instructions](https://emscripten.org/docs/getting_started/downloads.html).
The installation described there should work:

```shell
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

Refer to the CI file `/.github/workflows/build_wasm.yml` for the currently tested version which is guaranteed to work.

To configure the build prefix `cmake` with `emcmake` and postfix any arguments, e.g. run the following command in a `build` directory.

```shell
emcmake cmake \
    -DSC_EL=no \
    -DSUPERNOVA=no \
    -DSC_HIDAPI=no \
    -DNO_LIBSNDFILE=yes \
    -DSC_QT=no \
    -DNO_AVAHI=yes \
    -DSC_ABLETON_LINK=no \
    -DCMAKE_BUILD_TYPE="Release" \
    -Wno-dev \
    -DSSE=no \
    -DSSE2=no \
    -DNO_X11=yes \
    ..
```

To build `scsynth` via Emscripten use

```shell
emmake cmake --build . --target scsynth
```

which will write all necessary files to `<you_build_directory>/wasm/scsynth`.

Technically, what is built is a runtime library `scsynth.js` and the actual binary `scsynth.wasm`.
The standard plugins (UGens) are statically linked and thus available.

## Example

See the `<your_build_directory>/wasm/scsynth/example` directory for an example how to use the wasm binary within a website.

## Clients

A [client](https://supercollider.github.io/community/systems-interfacing-with-sc) currently has to communicate to scsynth via
a JavaScript method, and while in the future an `sclang.wasm` is possible, clients for now are to be running in
JavaScript (or be written in a language that can compile to JavaScript). While one can write directly JS code that uses the
OSC interface, more viable candidates may be:

- [ScalaCollider](https://www.sciss.de/scalaCollider/) -- while it cross-compiles to JavaScript, the OSC interface will have be 
  to adapted
- [supercollider.js](https://crucialfelix.github.io/supercolliderjs/) -- this was designed to run in Node.js and communicate with 
  a regular scsynth process, but probably it will be simple to adapt it for usage with scsynth.wasm.
- probably clients written in Python, like [supriya](https://github.com/josiah-wolf-oberholtzer/supriya), and in
  [other languages](https://github.com/jashkenas/coffeescript/wiki/List-of-languages-that-compile-to-JS) could potentially
  run in the browser as well
- [Processing](https://processing.org/) may also be an interesting option to communicate with scsynth.wasm.

One passes OSC messages to scsynth.wasm via a JavaScript function 
The interface is available through `scsynth.oscEndpoint.send` for sending OSC messages to the server and `scsynth.oscReceive` for receiving messages from the server which can be set to a callback accepting the binary content of an OSC message as its sole parameter.


## Source Code

The main additions to the SuperCollider C++ source code are the classes `SC_WebAudioDriver` (file `server/scsynth/SC_WebAudio.cpp`)
and `SC_WebInPort` (in file `server/scsynth/SC_ComPort.cpp`), implementing the Web Audio API driver bridge and the OSC replacement
respectively. Throughout, conditional compilation is done by preprocessor `#ifdef` and `#ifndef` statements using the symbol
`__EMSCRIPTEN__`. In the file `server/scsynth/CMakeList.txt`, the symbol to look out for is `EMSCRIPTEN`.

Currently, dynamic heap growth is disabled, as it implies a couple of problems with sharing memory between WASM and JavaScript
(address pointers may change). That is, the total heap size is specified at build time. This is done in the `CMakeList.txt` with 
flag `-s INITIAL_MEMORY=<size>`. With default settings, scsynth runs at 32 MB heap memory. Currently 64 MB are specified to allow 
for non-trivial allocation of buffers. If scsynth runs out of heap memory at runtime, an `OOM` exception is thrown.

## Performance Considerations

The current build is not optimised for speed or code size. This is a task for the future.

`-s DISABLE_EXCEPTION_CATCHING=0` -- this allegedly causes performance issues, so should eventually be removed again.

Disabling the catching means we can have meaningful exception message ('what') print-outs in the browser console. This is
usually needed when trying to understand why something crashed.
