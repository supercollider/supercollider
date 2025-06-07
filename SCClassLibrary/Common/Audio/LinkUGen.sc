LinkUGen : MultiOutUGen {
	*ar { |setTempo, newTempo, setPlaying, playingValue, quantum=1.0, hardwareBlockSize=512|
	    var delay = hardwareBlockSize / SampleRate.ir;
		^this.multiNew('audio', setTempo, newTempo, setPlaying, playingValue, quantum, delay);
	}

    *kr { |setTempo, newTempo, setPlaying, playingValue, quantum=1.0, hardwareBlockSize=512|
        var delay = hardwareBlockSize / SampleRate.ir;
        ^this.multiNew('control', setTempo, newTempo, setPlaying, playingValue, quantum, delay);
    }

	init {arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(2, rate);
	}
}
