//@ts-check
// everything in this file will be prepended to scsynth.js

/**
 * A help function to build scsynth command line arguments.
 * For details see https://docs.supercollider.online/Classes/ServerOptions.html
 */
function buildScsynthArguments({
      numInputs=0,
      numOutputs=2,
      udpPort=57110,
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
      return [
        "-u", udpPort.toString(),
        "-c", numControlBusChannels.toString(),
        "-a", numAudioBusChannels.toString(),
        "-i", numInputs.toString(),
        "-o", numOutputs.toString(),
        "-z", blockSize.toString(),
        "-b", numSampleBuffers.toString(),
        "-n", maxNumNodes.toString(),
        "-d", maxNumSynthDefs.toString(),
        "-m", realTimeMemorySize.toString(),
        "-w", numWireBufs.toString(),
        "-r", numRandomSeeds.toString(),
      ]
}
