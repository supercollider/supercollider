//@ts-check

// everything in this file will be prepended to scsynth.js

/**
 * A wrapper class for scsynth command line arguments.
 * For details see https://docs.supercollider.online/Classes/ServerOptions.html
 */
class ScsynthArguments {
  constructor({
      numInputs=0,
      numOutputs=2,
      udpPort=57110,
      loadSynthDefs=false,
      numControlBusChannels=16384,
      numAudioBusChannels=1024,
      blockSize=64,
      numSampleBuffers=1024,
      maxNumNodes=1024,
      maxNumSynthDefs=1024,
      realTimeMemorySize=8192,
      numWireBufs=64,
      numRandomSeeds=64,
    }) {
    this.numInputs = numInputs;
    this.numOutputs = numOutputs;
    this.udpPort = udpPort;
    this.loadSynthDefs = loadSynthDefs;
    this.numControlBusChannels = numControlBusChannels;
    this.numAudioBusChannels = numAudioBusChannels;
    this.blockSize = blockSize;
    this.numSampleBuffers = numSampleBuffers;
    this.maxNumNodes = maxNumNodes;
    this.maxNumSynthDefs = maxNumSynthDefs;
    this.realTimeMemorySize = realTimeMemorySize;
    this.numWireBufs = numWireBufs;
    this.numRandomSeeds = numRandomSeeds;
  }

  /**
   * Converts the parameters into a list of parameters for the binary
   * @returns {String[]}
   */
  toArgList() {
    return [
      "-u", this.udpPort.toString(),
      "-c", this.numControlBusChannels.toString(),
      "-a", this.numAudioBusChannels.toString(),
      "-i", this.numInputs.toString(),
      "-o", this.numOutputs.toString(),
      "-z", this.blockSize.toString(),
      "-b", this.numSampleBuffers.toString(),
      "-n", this.maxNumNodes.toString(),
      "-d", this.maxNumSynthDefs.toString(),
      "-m", this.realTimeMemorySize.toString(),
      "-w", this.numWireBufs.toString(),
      "-r", this.numRandomSeeds.toString(),
      "-D", this.loadSynthDefs ? "1" : "0",
    ]
  }
}
