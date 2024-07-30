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
