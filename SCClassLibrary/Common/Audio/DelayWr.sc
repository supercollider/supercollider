PingPong {
	//your buffer should be the same numChannels as your inputs
	*ar { arg  bufnum=0,  inputs, delayTime, feedback=0.7, rotate=1;

		var delaySamps = max(0, delayTime * SampleRate.ir - ControlDur.ir).round,
		phase, feedbackChannels, delayedSignals,
		frames = BufFrames.kr(bufnum);

		phase = Phasor.ar(0, 1, 0, frames);

		feedbackChannels = LocalIn.ar(inputs.size) * feedback;

 		delayedSignals = BufRd.ar(inputs.size, bufnum, (phase - delaySamps).wrap(0, frames), 0);
		LocalOut.ar(delayedSignals);

		BufWr.ar((inputs + feedbackChannels).rotate(rotate) <! delayedSignals.asArray.first, bufnum, phase, 1);
		^delayedSignals
	}
}

/* old sc3d5 version
PingPong {
	*ar { arg bank, index, inputs, delayTime, feedback=0.7, rotate=1, interpolationType=2;

		var indices, delayedSignals, outputs;

		delayedSignals = PlayBuf.ar(bank, index, -1, delayTime, interpolationType).postln;

		outputs = delayedSignals.rotate(rotate) * feedback + inputs;

		RecordBuf.ar(bank, index, outputs, run: -1);	// feedback to buffers

		^outputs
	}
}
*/


/**


/*
	these are still set up for sc3d5 style PlayBuf.
	won't work on scsynth yet
	-felix
*/


DelayProcess {
	*ar { arg bank, firstIndex, function, inputs, delayTime, interpolationType=2;
		var indices, delayedSignals, outputs;
		//[bank, firstIndex, function, inputs, delayTime, interpolationType].postln;
		if (delayTime.isArray, {
			// need separate buffers.

			inputs = inputs.asArray;
			indices = Array.series(inputs.size, firstIndex, 1);

			// tap the delay lines
			delayedSignals = PlayBuf.ar(bank, indices, -1, delayTime, interpolationType).postln;

			outputs = function.value(inputs, delayedSignals);

			// mix the delayed signal with the input

			RecordBuf.arN(bank, indices, outputs, run: -1);	// feedback to buffers
		},{
			// tap the delay lines
			delayedSignals = PlayBuf.ar(bank, firstIndex, -1, delayTime, interpolationType).postln;

			outputs = function.value(inputs, delayedSignals);

			// mix the delayed signal with the input

			RecordBuf.ar(bank, firstIndex, outputs, run: -1);	// feedback to buffers
		});

		^outputs	// output the mixed signal and force the DelayWr into the call graph
	}
}

**/
