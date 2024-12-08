# scsynth wasm example

This example shows how to use scsynth.wasm on a website.
It is not intended to provide an comprehensive library to interact with scsynth.wasm within JavaScript but tries to explain all necessary steps to get things running.

Before starting, copy (or symlink) the compiled files `scsynth.js` and `scsynth.wasm` from the build directory `wasm/scsynth` to this folder.

## Serving the files

The wasm build of _scsynth_ relies on [SharedArrayBuffers](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer) which are turned off by default on most browsers.
In order to activate this functionality it is necessary to set the HTTP headers

```http
Cross-Origin-Opener-Policy: same-origin
Cross-Origin-Embedder-Policy: require-corp
```

via a file server when serving the files to a browser.

A simply python script `static_dev_server.py` is provided in the build directory.
To start a local web server on <http://127.0.0.1:8000> with the necessary headers set run

```shell
python3 static_dev_server.py
```

> If get a 404 concerning `scsynth.js` be shure you have copied the compiled `scsynth.js` and `scsynth.wasm` to the example folder via e.g. `cp ../*.{js,wasm} .`.

> This script should __not__ be used to serve the files via the internet because the basic Python http server is not considered safe and can introduce security problems.
> Use an nginx or apache web server with the appropriate headers set if the files should be accessible via the internet.

## Module

Emscripten uses a JavaScript object named `Module` to contain all information on the wasm binary, see [the Emscripten documentation of `Module`](https://emscripten.org/docs/api_reference/module.html) for further information.

`scsynth.js` includes the JavaScript class `ScSynthArguments` (see `platform/wasm/pre.js` for its definition) to manage the command line arguments and provides some defaults.
An object of this class with its default parameters is attached to `Module.arguments` but can be overwritten before booting like this:

```javascript
let arguments = ScSynthArguments(
    // use microphone input via the browser
    numInputs=1,
);
// cli arguments need to be a list
Module['arguments'] = arguments.toArgList();
```

> `loadSynthDefs` is set to `false` (aka `0` within cli) because `scsynth.wasm` has (yet) no concept of a filesystem.

To start scsynth call

```javascript
Module.callMain(arguments.toArgList());
```

## OSC communication

Although `scsynth.wasm` uses the UDP argument to open an OSC communication for the client it is not a regular UDP connection because the browser is not allowed to open an UDP port.
Instead we pass the binary content of an OSC message to `scsynth.wasm` through a JavaScript function.
To build such a binary OSC message a [JavaScript OSC library](https://github.com/colinbdclark/osc.js) is included in `index.html`.

This binary OSC message is not transferred to scsynth.wasm via an UDP port but via a function call.
The function is located at `Module.oscDriver[57110].receive` and implemented in `server/scsynth/SC_ComPort.cpp`, where `57110` is the UDP port number specified through the arguments prior.

### Javascript number vs OSC types

Javascript uses `Number` to represent numerical data which does not distinguish between float and integer.
As it is necessary to distinguish between float and integer when communicating with scsynth (e.g. a group number must be an integer, but a frequency may be a float) it is necessary to provide the [OSC tags](https://opensoundcontrol.stanford.edu/spec-1_0.html) for each argument.

`scsynth_demo.js` provides two data classes for the construction of OSC messages, `OscMessage` for implicit tags (which will turn every number into a float) and `OscMessageTagged`.

To e.g. print the current node tree use

```javascript
oscClient.sendOscMessage(new OscMessageTagged("g_dumpTree", ["i", "i"], 0, 1));
```

## Add a SynthDef

In order to emit a sound it is necessary to register a SynthDef on the server.
`scysnth_demo.js` shows how to register the SynthDef `gcd` via OSC messages.

As currently there is no wasm build of sclang it is necessary to compile the SynthDef within sclang and copy the byte array from sclang to scsynth.wasm via JavaScript.

First obtain the byte code of a SynthDef via e.g.

```supercollider
SynthDef(\gcd, {
    var mx = LFNoise1.kr(0.1).range(-20, 20);
    var my = LFNoise1.kr(0.1).range(-20, 20);
    var sn = SinOsc.ar(SinOsc.kr(0.1) * 20 lcm: [mx, my] * 30 + 500) * 0.1;
    Out.ar(0, sn);
}).asBytes.cs;
```

Copying the result from the post window and replacing `Int8Array[ ... ]` by `new Uint8Array([ ... ])` and send the OSC message to the server.

```javascript
oscClient.addSynthDef(new Uint8Array([ ... ]));
```

To play this SynthDef as a Synth use

```javascript
oscClient.sendOscMessage(new OscMessageTagged(
    "/s_new", // address
    ["s", "i", "i", "i"], // tags
    "gcd", 1000, 1, 0, // message values
));
```

See the [_Server Command Reference_](https://docs.supercollider.online/Reference/Server-Command-Reference.html) for a comprehensive overview of available OSC commands on scsynth.
