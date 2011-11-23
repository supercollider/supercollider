+ ServerOptions {
	// get options from command line passed by SCSpawn.exe
	setFromCmdString { arg str;
		var segments;
		segments = str.split($ ).reject(_ == "");
		Server.program = segments.first;
		segments.drop(1).clump(2).do { |pair|
			var opt, val; #opt, val = pair;
			if (opt[0] != $-) { "" + opt + "is not a valid server option!".postln };
			opt = opt.drop(1).asSymbol;
			if (opt == \t) { protocol = \tcp; };
			if (opt == \u) { protocol = \udp; };
			if (opt == \a) { numPrivateAudioBusChannels = val.asInteger };
			if (opt == \c) { numControlBusChannels = val.asInteger };
			if (opt == \i) { numInputBusChannels = val.asInteger };
			if (opt == \o) { numOutputBusChannels = val.asInteger };
			if (opt == \b) { numBuffers = val.asInteger  };
			if (opt == \n) { maxNodes = val.asInteger };
			if (opt == \d) { maxSynthDefs = val.asInteger };
			if (opt == \z) { blockSize = val.asInteger };
			if (opt == \Z) { hardwareBufferSize = val.asInteger };
			if (opt == \m) { memSize = val.asInteger };
			if (opt == \r) { numRGens = val.asInteger };
			if (opt == \w) { numWireBufs = val.asInteger };
			if (opt == \S) { sampleRate = val.asInteger };
			if (opt == \D) { loadDefs = (val != "0") };
		// inputStreamsEnabled, outputStreamsEnabled, device, port number not yet handled.
		}
	}
}
