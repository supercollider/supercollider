// sets up ring buffers
Module['onRuntimeInitialized'] = function() {
    // using pthreads and memory growths doesn't guarantee that the heap stays
    // in the same location - see https://github.com/WebAssembly/design/issues/1271
    // in order to account for this, we update the buffer location each time we allocate.
    // This has some runtime cost, but probably this is neglectable in the
    // grand scheme.
    // This variable acts as a cache.
    const scHeap = Module.HEAPU8.buffer;
    this.scHeap = scHeap;

    // poll the post ring buffer every 20ms
    // @todo create a thread when `World_New` gets called which consumes the log via a semaphore, then uses
    // `MAIN_THREAD_ASYNC_EM_ASM` to push messages to the JS thread as alternative implementation.
    // See supernova implementation which uses this approach.
    setInterval(function() {
        const line = Module['nextLine']();
        if(line) {
            Module['onPrint'](line);
        }
    }, 20);
};

/**
 * Boots scsynth server
 *
 * @param options.numInputBusChannels {Number} The number of audio input bus channels.
 * @param options.numOutputBusChannels {Number} The number of audio output bus channels.
 * @param options.realTimeMemorySize {Number} The number of kilobytes of real time memory allocated to the server.
 * This memory is used to allocate synths and any memory that unit generators themselves allocate
 * (for instance in the case of delay ugens which do not use buffers, such as CombN),
 * and is separate from the memory used for buffers.
 * Setting this too low is a common cause of 'exception in real time: alloc failed' errors.
 * The default is 8192.
 * @param options.bufLength {Number} The number of samples in one control period. The default is 64.
 * @param options.maxWireBufs {Number} The maximum number of buffers that are allocated to interconnect unit generators.
 * (Not to be confused with the global sample buffers represented by Buffer.)
 * This sets the limit of complexity of SynthDefs that can be loaded at runtime.
 * This value will be automatically increased if a more complex def is loaded at startup,
 * but it cannot be increased thereafter without rebooting.
 * @param options.numBuffers {Number} The number of global sample buffers available.
 * @param options.maxNodes {Number} The maximum number of nodes.
 * @param options.maxGraphDefs {Number} The maximum number of synthdefs.
 * @param options.numAudioBusChannels {Number} The number of audio rate busses, which includes input and output busses.
 * @param options.numControlBusChannels {Number} The number of internal control rate busses.
 * @param options.verbosity {Number} Controls the verbosity of server messages.
 * A value of 0 is normal behavior.
 * -1 suppresses informational messages.
 * -2 suppresses informational and many error messages, as well as messages from Poll.
 */
Module['boot'] = ({
    numInputBusChannels = 0,
    numOutputBusChannels = 2,
    realTimeMemorySize = 8192,
    bufLength = 64,
    maxWireBufs = 64,
    numBuffers = 1024,
    maxNodes = 1024,
    maxGraphDefs = 1024,
    numAudioBusChannels = 1024,
    numControlBusChannels = 16384,
    verbosity = 0,
} = {}) => {
    let scsynthOptions = new Module["Options"];
    scsynthOptions.numInputBusChannels = numInputBusChannels;
    scsynthOptions.numOutputBusChannels = numOutputBusChannels;
    scsynthOptions.realTimeMemorySize = realTimeMemorySize;
    scsynthOptions.bufLength = bufLength;
    scsynthOptions.maxWireBufs = maxWireBufs;
    scsynthOptions.numBuffers = numBuffers;
    scsynthOptions.maxNodes = maxNodes;
    scsynthOptions.maxGraphDefs = maxGraphDefs;
    scsynthOptions.numAudioBusChannels = numAudioBusChannels;
    scsynthOptions.numControlBusChannels = numControlBusChannels;
    scsynthOptions.verbosity = verbosity;
    Module['bootWithOptions'](scsynthOptions);
}

/**
 * send osc data to the server
 *
 * @param oscBytes {Uint8Array} OSC message as raw bytes, using Uint8Array
 */
Module['sendOsc'];

/**
 * Gets called in case a new line was sent to stdout from scsynth.
 * Defaults to console.log, but feel free to overwrite it.
 *
 * @param line {String} Line without `\n`
 */
Module['onPrint'] = (line) => {
    console.log(`stdout: ${line}`);
}

/**
 * Gets called when the server sends an OSC reply to the client.
 * The bytes will be freed, so if e.g. the data should be passed to
 * sclang, this should be copied.
 *
 * @param message {Uint8Array<ArrayBuffer>}
 */
Module['onOscReply'] = (message) => {
    console.log("osc reply: ", message);
}

/**
 * Gets the audio context used by scsynth wasm
 * @returns {AudioContext}
 */
Module['getAudioContext'] = () => {
    return EmAudio[Module['_getAudioContext']()];
}

/**
 * Gets the audio worklet node which runs scsynth wasm
 * @returns {AudioWorkletNode}
 */
Module['getWorkletNode'] = () => {
    return EmAudio[Module['_getScsynthWorkletNode']()]
}
