import ScSynth from './scsynth.js';

const GCD_SYNTHDEF = [83, 67, 103, 102, 0, 0, 0, 2, 0, 1, 3, 103, 99, 100, 0, 0, 0, 5, 61, -52, -52, -51, 65, -96, 0, 0, 0, 0, 0, 0, 65, -16, 0, 0, 67, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 6, 83, 105, 110, 79, 115, 99, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 12, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 3, 79, 117, 116, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0]

// create a new instance of scsynth - attach it to the window object of the browser
window.scsynth = await ScSynth();
// allows to make scsynth a singleton
let scsynthStarted = false;

// overwrite message callbacks
scsynth.onPrint = (line) => {
    const consoleOutput = document.getElementById("console");
    consoleOutput.value += `${line}\n`;
    // follow scrolling to bottom
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
};

scsynth.onOscReply = (message) => {
    const oscConsoleOutput = document.getElementById("console-osc");
    let oscObject = scsynth.parseOscMessage(message);
    oscConsoleOutput.value += `${oscObject.address}: ${oscObject.arguments.join(" ")}\n`;
    // follow scrolling to bottom
    oscConsoleOutput.scrollTop = oscConsoleOutput.scrollHeight;
}

// custom boot function to parse i/o configuration and connect mic if necessary
// since this is a module, we have to export the function to the window object
window.start = () => {
    if(!scsynthStarted) {
        console.log("Starting scsynth...");
        let numInputs = Number(document.getElementById("num-inputs").value);
        let numOutputs = Number(document.getElementById("num-outputs").value);
        scsynth.boot({
            numInputBusChannels: numInputs,
            numOutputBusChannels: numOutputs,
        });
        scsynthStarted = true;
        if(numInputs > 0) {
            setTimeout(function() {
                // wait 1 sec before connecting
                // @todo signal isReady via a callback
                connectMic();
            }, 1000);

        }
    } else {
        console.log("scsynth already started");
    }
}


// helper method to connect microphone to scsynth audio worklet
async function connectMic() {
    console.log("Connecting Mic...");
    const stream = await navigator.mediaDevices.getUserMedia({
        audio: true,
        video: false,
    });

    const audioContext = scsynth.getAudioContext();
    const worklet = scsynth.getWorkletNode();

    const micSource = audioContext.createMediaStreamSource(stream);
    micSource.connect(worklet);
}

document.getElementById("oscMessage").addEventListener("keydown", (e) => {
    if(e.key === 'enter') {
        sendOscMessage(true);
    }
});

document.getElementById("synthDefName").addEventListener("keydown", (e) => {
    if(e.key === 'enter') {
        playSynthDef();
    }
});

window.sendOscMessage = (makeBlank) => {
    let oscString = document.getElementById("oscMessage").value;
    let oscArgs = oscString.split(" ");
    let oscMessage = new scsynth.OscMessage();
    let message = oscMessage.beginMessage(oscArgs[0]);

    function addOscArg(rawOscString, oscMessage) {
        let isNumber = !isNaN(rawOscString);
        if(isNumber) {
            let num = Number(rawOscString);
            if(Number.isInteger(num)) {
                oscMessage.addInt(num);
            } else {
                oscMessage.addFloat(num);
            }
        } else {
            oscMessage.addString(rawOscString);
        }
    }

    // skip 1 which is the address
    oscArgs.slice(1).forEach(rawOscArg => {
        addOscArg(rawOscArg, message);
    });

    let data = message.endMessage().getData();
    // manually free osc message factory - see https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html#memory-management
    oscMessage.delete();
    console.log("Send out ", data);
    scsynth.sendOsc(data);

    if(makeBlank) {
        document.getElementById("oscMessage").value = "";
    }
}

window.sendSynthDef = () => {
    let rawString = document.getElementById("synthDefBytes").value;
    let byteArray = Uint8Array.from(rawString.split(',').map(x => Number(x.replace("[", "").replace("]", ""))));

    let oscMessage = new scsynth.OscMessage();
    let data = oscMessage.beginMessage("/d_recv").addBlob(byteArray).endMessage().getData();
    // manually free osc message factory
    oscMessage.delete();
    console.log("Send out ", data);

    scsynth.sendOsc(data);
}

window.playSynthDef = () => {
    let synthDefName = document.getElementById("synthDefName").value;
    let oscMessage = new scsynth.OscMessage();
    let oscData = oscMessage.beginMessage("/s_new").addString(synthDefName).addInt(-1).addInt(1).addInt(0).endMessage().getData();
    // manually free osc message factory
    oscMessage.delete();
    console.log("Send out ", oscData);

    scsynth.sendOsc(oscData);
}

window.playGcd = () => {
    // gcd is written by name by hanns holger rutz, the original creator of scsynth.wasm
    const gcdDef = Uint8Array.from(GCD_SYNTHDEF);
    let oscMessage = new scsynth.OscMessage();
    const data = oscMessage.beginMessage("/d_recv").addBlob(gcdDef).endMessage().getData();
    // manually free osc message factory
    oscMessage.delete();
    scsynth.sendOsc(data);

    // delay play by 500ms b/c synthdef registration is async
    // This should ideally listen for a `/sync` message from the server instead of waiting a random amount of time.
    // To keep the example concise, this is left as an exercise to the reader.
    setTimeout(() => {
        let oscMessage = new scsynth.OscMessage();
        const playSynthData = oscMessage.beginMessage("/s_new").addString("gcd").addInt(-1).addInt(1).addInt(0).endMessage().getData();
        // manually free osc message factory
        oscMessage.delete();
        scsynth.sendOsc(playSynthData);
    }, 500);
}

window.cmdPeriod = () => {
    const oscMessage = new scsynth.OscMessage();
    const freeAllData = oscMessage.beginMessage("/g_freeAll").addInt(0).endMessage().getData();
    // manually free osc message factory
    oscMessage.delete();
    scsynth.sendOsc(freeAllData);
}
