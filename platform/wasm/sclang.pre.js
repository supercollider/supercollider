/*
`print` and `printErr` are emscripten internal methods,
see https://emscripten.org/docs/api_reference/console.h.html,
which can not be overwritten/reassigned during runtime.
To provide the ability to re-direct the output of sclang,
a `callback` version is provided, which defaults to console printing.
 */

Module['printCallback'] = function(text) {
    console.log(text);
};

Module['printErrCallback'] = function(text) {
    console.error(text);
};

// `print` and `printErr` are emscripten provided methods, but these can't
// be re-assigned during runtime. So instead we forward their output to
// our defined methods `printCallback` and `printErrCallback` which
// can be re-assigned during runtime.
Module['print'] = function(text) {
    Module['printCallback'](text);
};

Module['printErr'] = function(text) {
    Module['printErrCallback'](text);
};

Module['onRuntimeInitialized'] = function() {
    console.log("Spinning up sclang");
};

/**
 * Receive calls from JS.ideSend function.
 * This allows to send replies to the IDE via OSC messages, which don't have to
 * be passed to the server.
 *
 * @param message {Uint8Array} raw bytes of an OSC message
 */
Module['onIdeSend'] = function(message) {}

/**
 * Gets called from sclang when it sends out an OSC message.
 *
 * @param message {Uint8Array} Message is a copy owned by JS.
 * The buffer where this came from will be freed after the callback returns,
 * but the array itself can be kept or passed on without copying.
 */
Module['onOsc'] = function(message){
    console.log("New OSC message from sclang", message);
}

/** DOCS LAND **/

// internally used so the docs loading promise is a global singleton
let docsLoadPromise;

/**
 * Mount docs data async so while the interpreter compiles, the docs data can be loaded in the background.
 * `docsLoadPromise` acts as a promise, such that it can be awaited b/c the help files should be available
 * before sclang tries to access/index any!
 *
 * To make sure the docs have been mounted before accessing, use `await sclang.loadDocs();`
 */
Module['loadDocs'] = function() {
    docsLoadPromise ??= (async () => {
        const [meta, data] = await Promise.all([
            // locate file is vendored by emscripten
            fetch(locateFile('sclang-help.js.metadata')).then((r) => r.json()),
            fetch(locateFile('sclang-help.data')).then((r) => r.arrayBuffer()),
        ]);
        const bytes = new Uint8Array(data);
        for (const {filename, start, end} of meta.files) {
            // now write the loaded files into the filesystem
            Module['addFile'](filename, bytes.subarray(start, end));
        }
    })().catch((e) => {
        // allows for retries
        docsLoadPromise = undefined;
        throw e;
    });
    return docsLoadPromise;
}

// this acts as a flag so sclang can preload the docs using `const sclang = await ScLang({ preloadDocs: true });`
if(Module['preloadDocs'] ?? false) {
    Module['postRun'] = [].concat(Module['postRun'] ?? [], () => {
        Module['loadDocs']().catch((e) => {console.warn('sclang could not load docs', e)});
    })
}
