// @ts-check

/**
 * Data class for an OSC message with implicit types.
 * As JavaScript only has a number object for integers and floats
 * it may be necessary to state the types explicitly, see
 * `OscMessageTagged` for this.
 */
class OscMessage {
  /**
   * @param {String} address
   * @param  {...(number | String | Uint8Array)} values
   */
  constructor(address, ...values) {
    this.address = address;
    this.values = values;
  }
};

/**
 * Data class for an OSC message with explicit types via message tags.
 */
class OscMessageTagged {
  /**
   * 
   * @param {String} address 
   * @param {String[]} tags - one of i, f, s, or b. See section "OSC Type Tag String" in https://opensoundcontrol.stanford.edu/spec-1_0.html
   * @param {...(number | String | Uint8Array)} values 
   */
  constructor(address, tags, ...values) {
    console.assert(tags.length == values.length);
    this.address = address;
    this.values = values.map((value, i) => (
      {
        type: tags[i],
        value: value,
      }
    ));
  }
}

/**
 * A basic "OSC client" to communicate with a provided `scsynth.wasm` instance
 */
class OscClient {
  constructor(endPoint) {
    // @todo pass osc endpoint explicitly
    this.endPoint = endPoint;
    this.receiver = null;
    if(this.endPoint) {
      this.receiver = this.endPoint['receive'];
    } else {
      alert("Failed to setup OSC client because endPoint is missing!");
    }
  }

  /**
   * Transmits an OSC message to the wasm instance.
   * This simply converts a provided OSC message to its
   * binary representation through the `osc` library
   * and passes it to the wasm module.
   * @param {OscMessage | OscMessageTagged} oscMessage
   */
  sendOscMessage(oscMessage) {
    // @ts-ignore osc is global lib - convert osc message to binary
    let data = osc.writePacket({
      address: oscMessage.address,
      args: oscMessage.values
    }, {metadata: true});
    if(this.receiver) {
      this.receiver(57120, data);
    }
  };

  /**
   * "Resets" the server by freeing all groups.
   */
  cmdPeriod() {
    this.sendOscMessage(new OscMessageTagged("/g_freeAll", ["i"], 0))
  }

  /**
   * 
   * @param {number} code 
   */
  dumpOsc(code) {
    this.sendOscMessage(new OscMessageTagged("/dumpOSC", ["i"], code ? code : 1));
  }

  /**
   * Help method to directly send a SynthDef through
   * @param {Uint8Array} data - bit array of compiled synthdef
   */
  addSynthDef(data) {
    this.sendOscMessage(new OscMessageTagged("/d_recv", ["b"], data))
  }
}

// website dependent code

// init global variable which will hold the OSC client after booting
let oscClient = /** @type OscClient | null */ null;
// will store the scsynth module after booting
let scsynth = null;

/**
 * Gets called when clicked on the boot button.
 * - Parses the arguments of the website
 * - Starts scsynth.wasm with the provided arguments
 * - Inits the oscClient with the boot menu
 */
async function bootScsynth() {
  const numInputsElement = /** @type {HTMLInputElement | null} */ (document.getElementById("inputs"));
  const numOutputsElement = /** @type {HTMLInputElement | null} */ (document.getElementById("outputs"));

  // @ts-ignore - proivded by scsynth.js - see `platform/wasm/pre.js` for implementation
  let arguments = new ScsynthArguments({
    numInputs: Number(numInputsElement?.value ?? "0"),
    numOutputs: Number(numOutputsElement?.value ?? "2"),
  });

  // @ts-ignore - provided by scsynth.js
  scsynth = await createScsynth({
    arguments: arguments.toArgList(),
    print: function (text) {printToWebsiteConsole(text)},
    printErr: function (text) {printToWebsiteConsole(`ERROR: ${text}`)},
    oscReceiver: function (data) {
      //@ts-ignore osc is globally available
      let oscMessage = osc.readMessage(data);
      printToWebsiteOscConsole(oscMessage);
    }
  });

  oscClient = new OscClient(
    scsynth.oscDriver[arguments.udpPort],
  );

  disableBootButton();
}

function disableBootButton() {
  let bootButton = /** @type{HTMLInputElement | null} */ (document.getElementById("boot"));
  if(bootButton) {
    bootButton.disabled = true;
  }
}

function playGcd() {
  // a nice SynthDef called "gcd" by @Sciss in its compiled byte array format
  // obtained by calling `.asBytes.cs` on the SynthDef.
  // The name "gcd" is included within the byte array.
  const gcdDef = Uint8Array.from(
    [83, 67, 103, 102, 0, 0, 0, 2, 0, 1, 3, 103, 99, 100, 0, 0, 0, 5, 61, -52, -52, -51, 65, -96, 0, 0, 0, 0, 0, 0, 65, -16, 0, 0, 67, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 6, 83, 105, 110, 79, 115, 99, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 12, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 3, 79, 117, 116, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0]
  ); 
  oscClient?.addSynthDef(gcdDef);
  // delay playing of synth because scsynth takes time to make
  // the synthdef available
  setTimeout(() => {oscClient?.sendOscMessage(new OscMessageTagged(
      "/s_new",
      // synthdef name, synth id, action(1=add to tail of target), target
      ["s", "i", "i", "i"],
      "gcd", 1000, 1, 0,
    ))
  }, 500);
}

/**
 * Parses the input into an OscMessageTagged which
 * gets passed to scsynth.wasm through the osc client
 */
function sendOsc() {
  let oscElement = /** @type{HTMLInputElement | null} */ (document.getElementById("oscMessage"));
  if(!oscElement) {return}
  let rawOscString = "";
  rawOscString = oscElement.value.trim();
  let oscElements = rawOscString.split(" ");

  let address = oscElements[0];
  let tags = oscElements[1].split("").map(char => char);
  // @ts-ignore - we want to perform a type conversion here
  let values = oscElements.slice(2).map(arg => isNaN(arg) ? arg : Number(arg));

  let message = new OscMessageTagged(
    address,
    tags,
    ...values,
  );
  oscClient?.sendOscMessage(message);

  oscElement.value = "";
}

function sendByteArray() {
  let byteElement = /** @type{HTMLInputElement | null} */ (document.getElementById("byteArray"));
  if(!byteElement) {return}
  const jsArray = JSON.parse(byteElement.value);
  const byteArray = new Uint8Array(jsArray);

  oscClient?.addSynthDef(byteArray);
  byteElement.value = "";
}

function printToWebsiteConsole(text) {
  const consoleOutput = /** @type {HTMLTextAreaElement | null} */ (document.getElementById("console"));
  if (consoleOutput !== null) {
    consoleOutput.value += text + "\n";
    // follow scrolling to bottom
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
  }
};

function printToWebsiteOscConsole(oscMessage) {
  const oscConsoleOutput = /** @type {HTMLTextAreaElement | null} */ (document.getElementById("console-osc"));
  if (oscConsoleOutput !== null) {
    oscConsoleOutput.value += `${oscMessage.address}: ${oscMessage.args}\n`;
    // follow scrolling to bottom
    oscConsoleOutput.scrollTop = oscConsoleOutput.scrollHeight;
  }
}

window.onerror = function (event) {
  alert("Exception thrown, see JavaScript console");
};
