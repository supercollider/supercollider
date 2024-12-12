# scsynth wasm example

> __The JS interface is currently not considered stable and subject to change w/o prior notice!__

This example shows how to use scsynth.wasm on a website.
Use this as a starting block to integrate scsynth into your website or web project.

__Before starting, copy (or symlink) the compiled files `scsynth.js` and `scsynth.wasm` from the build directory `wasm/scsynth` to this folder.__

## Serving the files

The wasm build of _scsynth_ relies on [SharedArrayBuffers](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/SharedArrayBuffer) which are turned off by default on most browsers.
In order to activate this functionality it is necessary to set the HTTP headers

```http
Cross-Origin-Opener-Policy: same-origin
Cross-Origin-Embedder-Policy: require-corp
```

via a file server when serving the files to a browser.

A simply python script `static_dev_server.py` is provided with this example which serves the current directory with the appropriate headers.
To use this script to expose the files on <http://127.0.0.1:8000> run

```shell
python3 static_dev_server.py
```

> If get a 404 concerning `scsynth.js` in the browser be shure you have copied the compiled `scsynth.js` and `scsynth.wasm` to the example folder via e.g. `cp ../*.{js,wasm} .`.

> This script should __not__ be used to serve the files via the internet because the basic Python http server is not considered safe and can introduce security problems.
> Use an nginx or apache web server with the appropriate headers set if the files should be accessible via the internet.
>
> For nginx the config could look something like
>
> ```nginx
> server {
>   server_name my-address.com;
>
>   # Path to the root directory for serving static files
>   root /home/user/supercollider_wasm_files;
>
>   # URL prefix where the static files are located
>   location / {
>       # Set the default index file
>       index index.html;
>
>       # Set additional HTTP headers
>       add_header Cross-Origin-Embedder-Policy "require-corp";
>       add_header Cross-Origin-Opener-Policy "same-origin";
>
>       types {
>           application/wasm wasm;
>           text/html html;
>           application/javascript js;
>           text/css css;
>           default text/plain;
>       }
>   }
>   # use certbot to convert this to HTTPS which is necessary!!!
>   listen 80;
> }
> ```

## The scsynth module

Emscripten generates a factory function `createScsynth` in `scsynth.js` which will return an instance of scsynth.
It also includes the JavaScript function `buildScsynthArguments` (see `platform/wasm/pre.js` for its definition) which is a helper function to easily construct the CLI arguments.

```javascript
// needs to be called from within an async function
async function startScsynth() {
    const argumenents = buildScsynthArguments({
        numInputs: 1,
        numOutputs: 2,
    });
    scsynth = await createScsynth({
        arguments,
    });
};
```

See [the Emscripten documentation of `Module`](https://emscripten.org/docs/api_reference/module.html) for further information what methods and arguments are available for an Emscripten module.

## OSC communication

Scsynth interacts with the outside world through OSC messages which normally get send via UDP.
As it is not allowed to open an UDP socket within a browser context/sandbox scysnth.wasm instead exposes the sending and receiving OSC API through JavaScript.

The [`Uint8Array`](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array) byte array of an OSC messages must be passed as an argument to `scysnth.sendOscMessage`.
OSC replies from the server can be received by setting a callabck at `scsynth.receiveOscMessage` which accepts the byte array as a single argument.

To construct and parse OSC messages the class `scsynth.OscMessage` and the function `scsynth.parseOscMessage` can be used.
Take a look at `scysnth_demo.js` for their API.

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
scsynth.sendOscMessage(
    new scsynth.OscMessage().beginMessage("/d_recv").addBlob(myUint8Array).endMessage().getData()
);
```

To play this SynthDef as a Synth use

```javascript
scsynth.sendOscMessage(
    new scsynth.OscMessage().beginMessage("/s_new").addString("gcd").addInt(1000).addInt(1).addInt(0).endMessage().getData()
);
```

See the [_Server Command Reference_](https://docs.supercollider.online/Reference/Server-Command-Reference.html) for a comprehensive overview of available OSC commands on scsynth.
