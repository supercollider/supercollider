

PingPong {
	*ar { arg bank, index, inputs, delayTime, feedback=0.7, rotate=1, interpolationType=2;
	
		var indices, delayedSignals, outputs;
		
		delayedSignals = PlayBuf.ar(bank, index, -1, delayTime, interpolationType).postln;	

		outputs = delayedSignals.rotate(rotate) * feedback + inputs;
		
		RecordBuf.ar(bank, index, outputs, run: -1);	// feedback to buffers

		^outputs
	}
}


DelayProcess {
	*ar { arg bank, firstIndex, function, inputs, delayTime, interpolationType=2;
		var indices, delayedSignals, outputs;
		//[bank, firstIndex, function, inputs, delayTime, interpolationType].postln;
		if (delayTime.isKindOf(Array), {
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
