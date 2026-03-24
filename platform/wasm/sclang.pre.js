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
 * @param message {Uint8Array} This will be freed, so it is necessary to make a copy
 * if it should be passed around.
 */
Module['onOsc'] = function(message){
    console.log("New OSC message from sclang", message);
}
