// @ts-check

// will store the scsynth module after booting
let scsynth = null;

/**
 * Gets called when clicked on the boot button.
 * - Parses the arguments of the website
 * - Starts scsynth.wasm with the provided arguments
 */
async function bootScsynth() {
  if (scsynth !== null) {
    console.log("Already booted");
    return;
  };
  const numInputsElement = /** @type {HTMLInputElement | null} */ (document.getElementById("num-inputs"));
  const numOutputsElement = /** @type {HTMLInputElement | null} */ (document.getElementById("num-outputs"));

  // @ts-ignore - proivded by scsynth.js - see `platform/wasm/pre.js` for implementation
  const arguments = buildScsynthArguments({
    numInputs: Number(numInputsElement?.value ?? "0"),
    numOutputs: Number(numOutputsElement?.value ?? "2"),
  });

  // @ts-ignore - provided by scsynth.js
  scsynth = await createScsynth({
    arguments,
    print: function (text) {printToWebsiteConsole(text)},
    printErr: function (text) {printToWebsiteConsole(`ERROR: ${text}`)},
    oscReceiver: function (data) {
      const oscMessage = scsynth.parseOscMessage(data);
      printToWebsiteOscConsole(`${oscMessage.address}: ${oscMessage.arguments}`);
    }
  });
}

function playGcd() {
  // a nice SynthDef called "gcd" by @Sciss in its compiled byte array format
  // obtained by calling `.asBytes.cs` on the SynthDef.
  // The name "gcd" is included within the byte array.
  const gcdDef = Uint8Array.from(
    [83, 67, 103, 102, 0, 0, 0, 2, 0, 1, 3, 103, 99, 100, 0, 0, 0, 5, 61, -52, -52, -51, 65, -96, 0, 0, 0, 0, 0, 0, 65, -16, 0, 0, 67, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 6, 83, 105, 110, 79, 115, 99, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 12, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 3, 79, 117, 116, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0]
  );

  scsynth.sendOscMessage(
    new scsynth.OscMessage().beginMessage("/d_recv").addBlob(gcdDef).endMessage().getData()
  );

  // delay playing of synth because scsynth takes time to make
  // the synthdef available
  setTimeout(() => {
     scsynth.sendOscMessage(
      new scsynth.OscMessage().beginMessage("/s_new").addString("gcd").addInt(1000).addInt(1).addInt(0).endMessage().getData()
     );
  }, 500);
}

function cmdPeriod() {
  scsynth.sendOscMessage(
    new scsynth.OscMessage().beginMessage("/g_freeAll").addInt(0).endMessage().getData(),
  );
}

/**
 * Parses a custom defined string input into an
 * OscMessage which then gets passed to scsynth.wasm
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

  let message = new scsynth.OscMessage().beginMessage(address);

  tags.map((tag, i) => {
    switch (tag.toLowerCase()) {
      case "i":
        message.addInt(values[i]);
        break;
      case "f":
        message.addFloat(values[i]);
        break;
      case "s":
        message.addString(values[i]);
        break;
      // case "b" is not supported yet :/
      default:
        alert(`Unsupported OSC type ${tag}`);
        break;
    };
  });

  message.endMessage();
  scsynth.sendOscMessage(message.getData());

  oscElement.value = "";
}

/**
 * Parses the input as Uint8 array and registers it as a synth
 */
function sendByteArray() {
  let byteElement = /** @type{HTMLInputElement | null} */ (document.getElementById("byteArray"));
  if(!byteElement) {return}
  const jsArray = JSON.parse(byteElement.value);
  const byteArray = new Uint8Array(jsArray);

  scsynth.sendOscMessage(
    new scsynth.OscMessage().beginMessage("/d_recv").addBlob(byteArray).endMessage().getData()
  )

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

function printToWebsiteOscConsole(text) {
  const oscConsoleOutput = /** @type {HTMLTextAreaElement | null} */ (document.getElementById("console-osc"));
  if (oscConsoleOutput !== null) {
    oscConsoleOutput.value += text;
    // follow scrolling to bottom
    oscConsoleOutput.scrollTop = oscConsoleOutput.scrollHeight;
  }
}

window.onerror = function (event) {
  alert("Exception thrown, see JavaScript console");
};
