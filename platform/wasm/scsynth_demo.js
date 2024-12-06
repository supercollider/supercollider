// @ts-check

const consoleOutput = document.getElementById("console");

const gcdDef = Uint8Array.from(
  [83, 67, 103, 102, 0, 0, 0, 2, 0, 1, 3, 103, 99, 100, 0, 0, 0, 5, 61, -52, -52, -51, 65, -96, 0, 0, 0, 0, 0, 0, 65, -16, 0, 0, 67, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 8, 76, 70, 78, 111, 105, 115, 101, 49, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 6, 83, 105, 110, 79, 115, 99, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 1, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 8, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 17, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 6, 77, 117, 108, 65, 100, 100, 1, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 3, -1, -1, -1, -1, 0, 0, 0, 4, 1, 6, 83, 105, 110, 79, 115, 99, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 2, 0, 0, 0, 12, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 2, 3, 79, 117, 116, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 2, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0]
);

/** @type ScWasmOscClient | null */
let oscClient = null;

class OscMessage {
  /**
   * 
   * @param {String} address
   * @param  {...(number | String | Uint8Array)} values 
   */
  constructor(address, ...values) {
    this.address = address;
    this.values = values;
  }
};

class OscMessageTagged {
  /**
   * 
   * @param {String} address 
   * @param {String[]} tags 
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

class ScWasmOscClient {
  constructor() {
    this.endPoint = Module.oscDriver[57110];
    this.receiver = null;
    if(this.endPoint) {
      this.receiver = this.endPoint['receive'];
    }
  }

  /**
   * 
   * @param {OscMessage | OscMessageTagged} oscMessage - osc message to be send
   */
  sendOscMessage(oscMessage) {
    // @ts-ignore osc is global lib
    let data = osc.writePacket({
      address: oscMessage.address,
      args: oscMessage.values
    }, {metadata: true});
    if (this.receiver) {
      this.receiver(57120, data);
    } else {
      console.log(`Could not send message b/c receiver not available: ${oscMessage}`);
    }
  };

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
   * 
   * @param {Uint8Array} data - bit array of compiled synthdef
   */
  addSynthDef(data) {
    this.sendOscMessage(new OscMessageTagged("/d_recv", ["b"], data))
  }
}


function bootScsynth() {
  // @ts-ignore
  const numInputs = document.getElementById("inputs")?.value ?? '0';
  // @ts-ignore
  const numOutputs = document.getElementById("outputs")?.value ?? '2';

  // @todo change args into dict instead of arg array
  /** @type String[] */
  let args = Module['arguments'];
  args[args.indexOf("-i")+1] = numInputs;
  args[args.indexOf("-o")+1] = numOutputs;
  Module.callMain(args);
  oscClient = new ScWasmOscClient();
  let bootButton = document.getElementById("boot");
  if(bootButton) {
    // @ts-ignore
    bootButton.disabled = true;
  }
}

function playGcd() {
  oscClient?.addSynthDef(gcdDef);
  setTimeout(() => {oscClient?.sendOscMessage(new OscMessageTagged(
      "/s_new",
      ["s", "i", "i", "i"],
      "gcd", 1000, 1, 0,
    ))
  }, 500);
}

function sendOsc() {
  let oscElement = document.getElementById("oscMessage");
  if(!oscElement) {return}
  let rawOscString = "";
  // @ts-ignore
  rawOscString = oscElement.value.trim();
  let oscElements = rawOscString.split(" ");

  let address = oscElements[0];
  let tags = oscElements[1].split("").map(char => char);
  // @ts-ignore
  let values = oscElements.slice(2).map(arg => isNaN(arg) ? arg : Number(arg));

  let message = new OscMessageTagged(
    address,
    tags,
    ...values,
  );
  oscClient?.sendOscMessage(message);

  // @ts-ignore
  oscElement.value = "";
}

function sendByteArray() {
  let byteElement = document.getElementById("byteArray");
  if(!byteElement) {return}
  // @ts-ignore
  const jsArray = JSON.parse(byteElement.value);
  const byteArray = new Uint8Array(jsArray);

  oscClient?.addSynthDef(byteArray);
  // @ts-ignore
  byteElement.value = "";
}


/**
 * The js wrapper for scsynth to handle i/o
 */

// @todo we overload some functions of the scsynth module here?!
var Module = {
  preRun: [],
  postRun: [],
  
  print: (function () {
    // @ts-ignore outputWindow is an input
    if (consoleOutput) consoleOutput.value = ""; // clear browser cache
    return function (text) {
      if (arguments.length > 1)
        text = Array.prototype.slice.call(arguments).join(" ");
      // These replacements are necessary if you render to raw HTML
      //text = text.replace(/&/g, "&amp;");
      //text = text.replace(/</g, "&lt;");
      //text = text.replace(/>/g, "&gt;");
      //text = text.replace('\n', '<br>', 'g');
      console.log(text);
      if (consoleOutput !== null) {
        // @ts-ignore
        consoleOutput.value += text + "\n";
        consoleOutput.scrollTop = consoleOutput.scrollHeight; // focus on bottom
      }
    };
  })(),

  printErr: function (text) {
    if (arguments.length > 1)
      text = Array.prototype.slice.call(arguments).join(" ");
    console.error(text);
  },

  setStatus: function(text) {
    console.log(text);
  }
};

window.onerror = function (event) {
  alert("Exception thrown, see JavaScript console");
};
