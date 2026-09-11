+ Server {
	bootServerApp { |onComplete|
		JS.runCode("bootServer(%)".format(this.prOptionsString));
		onComplete.value();
	}

	// creates a json from the current options
	prOptionsString {
		var o = Server.default.options;
		var optionDict = (
			numInputBusChannels: o.numInputBusChannels,
			numOutputBusChannels: o.numOutputBusChannels,
			realTimeMemorySize: o.memSize,
			bufLength: o.blockSize,
			maxWireBufs: o.numWireBufs,
			numBuffers: o.numBuffers,
			maxNodes: o.maxNodes,
			maxGraphDefs: o.maxSynthDefs,
			numAudioBusChannels: o.numAudioBusChannels,
			numControlBusChannels: o.numControlBusChannels,
			verbosity: o.verbosity,
		);
		var optionsArray = [];
		var optionStrings = optionDict.pairsDo({|key, value|
			optionsArray = optionsArray.add("\"%\":%".format(key, value));
		});
		^"{%}".format(optionsArray.join(","));
	}
}

+ Main {
	platformClass { ^WasmPlatform }

	applicationStart {
		ApplicationStart.run;
	}
}
