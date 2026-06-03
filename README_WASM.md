# SuperCollider on WebAssembly

[WebAssembly](https://webassembly.org/) (wasm) is used by browsers (and beyond) to provide native-like performance across a variety of platforms and processor architectures using the same binary.
Since almost all browsers ship with a WebAssembly runtime, it is possible to run SuperCollider in a browser without any kind of installation.

There is currently only a wasm build for scsynth, such that all the sound functionality of SuperCollider is available in the browser.

## scsynth

Any interaction with scsynth needs to happen through OSC, so it is necessary to have a client which creates the appropriate OSC messages, which are passed as binary [`Uint8Array`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array) to the wasm version of scsynth.
Therefore, instead of recreating a network interface in JavaScript, OSC messages are passed to scsynth via JavaScript functions.

In sclang, one can use `.asRawOSC` on an array to obtain the bytes of an OSC message.

Alternatively, use a JS client for scsynth or wrap the OSC communication in a WebSocket through a script.

A basic JS binding for building and parsing OSC messages is exposed by the function `parseOscMessage` and the object `OscMessage()`.

### Example

> The example should be available via <https://supercollider.dennis-scheiba.com/>

An example on how to use the wasm version of scsynth in a website is given in `platform/wasm/index.html` and `platform/wasm/init.js`, which require the wasm build artifact `scsynth.js` and `scsynth.wasm` and should be served using the provided `dev_server.py`.
Check the CI of SuperCollider to obtain the wasm build artifact.

### Limitations

* scsynth requires `pthreads`. In order to allow for communication between those threads, it is necessary to use [`SharedArrayBuffers`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer) which are, due to [security concerns](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer#security_requirements), only available in the browser if the webserver sets the HTTP headers
  * `Cross-Origin-Embedder-Policy`: `require-corp`
  * `Cross-Origin-Opener-Policy`: `same-origin`
* An user interaction (i.e. a click) is necessary to enable audio output of a website, see https://developer.mozilla.org/en-US/docs/Web/Media/Guides/Autoplay
* Loading buffers is not supported yet since it requires filesystem access, see https://emscripten.org/docs/porting/files/file_systems_overview.html#file-system-overview for more information.
  This will likely be implemented in a future version.
* Any kind of Mouse/X11 UGens (i.e. `MouseX`, `MouseY`) are not implemented, though there is a emscripten binding to access the mouse data, see https://emscripten.org/docs/api_reference/html5.h.html#mouse.
  This will likely be implemented in a future version.

#### HTTP Headers

As mentioned, it is necessary to set HTTP headers in the webserver in order to make the necessary `SharedArrayBuffers` available.
The provided `dev_server.py` allows to spin up a local development server with the necessary headers.
This script should **not** be used to expose the files to the internet, instead use a webserver like *nginx* with a configuration along the following lines

```nginx
server {
    server_name supercollider.dennis-scheiba.com;

    # Path to the root directory for serving static files
    root /home/scheiba/supercollider;

    # URL prefix where the static files are located
    location / {
        # Set the default index file
        index index.html;

        # Set additional HTTP headers
        add_header Cross-Origin-Embedder-Policy "require-corp";
        add_header Cross-Origin-Opener-Policy "same-origin";

        types {
          application/wasm wasm;
          text/html html;
          application/javascript js;
          text/css css;
          default text/plain;
        }
    }

    listen 80;
}
```

Use e.g. [certbot](https://certbot.eff.org/) to obtain a certificate for the domain, since a `https` context is also necessary for `SharedArrayBuffer` and `AudioContext` access.

### Source code

The basic idea of the wasm implementation is to remove the network stack since it requires socket access which is not available in a browser environment.
Instead of "faking" a network stack, `SC_ComPort.cpp` is removed from compilation and the `ReplyAddress` struct also removes its `boost::asio::ip::address mAddress` member.
Any OSC I/O is instead provided by JavaScript functions which write the messages into the scsynth `World` directly.

Beyond that, the main additions to the scsynth source code are

* `SC_WebAudioDriver`, located in `server/scsynth/SC_WebAudio.cpp`, which implements a [`WebAudioWorklet`](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet) as a "fake" audio driver which receives a callback from the real time environment of the browser to generate the next n samples.
  The emscripten documentation provides a good introduction on WebAudioWorklets, see https://emscripten.org/docs/api_reference/wasm_audio_worklets.html.
  It also contains all missing fake implementations necessary due to the removal of `SC_ComPort.cpp` and contains the JS<->CPP glue code using emscripten.
* `platform/wasm/SC_WebOsc.cpp` which implements a basic OSC builder and parser for JavaScript using the already included [oscpack](https://github.com/RossBencina/oscpack)
* `platform/wasm/scsynth.pre.js` which provides some glue code and scaffolding such that a nice user facing JS API is available.

Additionally, all plugins are statically linked.

In `server/scsynth/CMakeList.txt`, the symbol to look out for is `EMSCRIPTEN` and within the source code any modifications are guarded by the `__EMSCRIPTEN__` symbol.

## Building

> Building the wasm version is only necessary for development purposes.
> It is advised to use the wasm build from the CI artifacts for plain usage of SuperCollider in the web.

Install [emscripten](https://emscripten.org) by following its [download instructions](https://emscripten.org/docs/getting_started/downloads.html).

Assuming the `emsdk` repository has been cloned, the setup looks in general something like

```shell
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

> Refer to the CI file `/.github/workflows/build_wasm.yml` for the currently supported emscripten version.

This should make the `emcc` compiler available in this shell, which can be verified by running `emcc -v`.

Create a clean(!) build directory, e.g. `build-emscripten` and execute the following command in it to configure the build

```shell
emcmake cmake \
  -DSC_EL=no \
  -DSUPERNOVA=no \
  -DSC_HIDAPI=no \
  -DNO_LIBSNDFILE=yes \
  -DSTATIC_PLUGINS=yes \
  -DSC_QT=no \
  -DNO_AVAHI=yes \
  -DSC_ABLETON_LINK=no \
  -DCMAKE_BUILD_TYPE="Release" \
  -Wno-dev \
  -DNO_X11=yes \
  -DAUDIOAPI=webaudio \
  ..
```

Now build `scsynth` for wasm using emscripten by running 

```shell
emmake cmake --build . --target scsynth
```

The build artifacts with examples will be written to `<your_build_directry>/scsynth-wasm-dist`.

### CLion setup

The wasm build has been developed using [CLion](https://www.jetbrains.com/clion/), which is free to use for Open Source projects like SuperCollider and allows to easily switch between a "normal" and "emscripten" development pipeline.

The general idea is to create a new `EMSDK` [toolchain](https://www.jetbrains.com/help/clion/how-to-create-toolchain-in-clion.html) within the CLion settings, using e.g. `/Users/scheiba/github/emsdk/upstream/emscripten/em++` for its C++ compiler, bundled CMake and bundled LLDB as debugger.

After the toolchain has been added, a new [CMake Profile](https://www.jetbrains.com/help/clion/cmake-profile.html), named e.g. `Emscripten`, can be added which uses the `EMSDK` toolchain.
This needs to use its own `build` directory in order to avoid clashes with the local build!

Additionally, the following environment variables needs to be specified which should look something like

```
EMSDK=/Users/scheiba/github/emsdk;EMSDK_NODE=/Users/scheiba/github/emsdk/node/22.16.0_64bit/bin/node;EMSDK_PYTHON=/Users/scheiba/github/emsdk/python/3.13.3_64bit/bin/python3;SSL_CERT_FILE=/Users/scheiba/github/emsdk/python/3.13.3_64bit/lib/python3.13/site-packages/certifi/cacert.pem
```

and the following CMake options should be set

```
-DCMAKE_TOOLCHAIN_FILE=/Users/scheiba/github/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DSC_EL=no  -DSUPERNOVA=no  -DSC_HIDAPI=no  -DNO_LIBSNDFILE=yes  -DSC_QT=no  -DNO_AVAHI=yes  -DSC_ABLETON_LINK=no  -DCMAKE_BUILD_TYPE="Release" -Wno-dev -DSSE=no -DSSE2=no -DNO_X11=yes -DAUDIOAPI=webaudio
```

## Credits

The first implementation of scsynth in wasm has been written by Hanns Holger Rutz (Sciss).

The current implementation, which is enhanced by using AudioWorklets, does not retain any of the implementation from Hanns Holger Rutz except for a few lines of CMake and the demo SynthDef `gcd`.

## License

scsynth and sclang are licensed via GPL-3.0.
The wasm binding of scsynth is licensed via AGPL-3.0.
