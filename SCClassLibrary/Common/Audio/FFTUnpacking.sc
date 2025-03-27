/**
"Unpack FFT" UGens (c) 2007 Dan Stowell.
Magical UGens for treating FFT data as demand-rate streams.
*/

// Actually this just wraps up a bundle of Unpack1FFT UGens
UnpackFFT : MultiOutUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*new { | chain, bufsize, frombin=0, tobin |
		var upperlimit = bufsize/2;
		tobin = if(tobin.isNil, upperlimit, {tobin.min(upperlimit)});
		^[Unpack1FFT(chain, bufsize, (frombin..tobin), 0),
			Unpack1FFT(chain, bufsize, (frombin..tobin), 1)].flop.flatten;
	}
}

Unpack1FFT : UGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\read }
	hasObservableEffect { ^false }
	canBeReplacedByIdenticalCall { ^true }

	*new { | chain, bufsize, binindex, whichmeasure=0 |
		^this.multiNew('demand', chain, bufsize, binindex, whichmeasure);
	}
}

// This does the demanding, to push the data back into an FFT buffer.
PackFFT : PV_ChainUGen {
	resourceManagers { ^[UGenBufferResourceManager] }
	bufferAccessType { ^\write }
	hasObservableEffect { ^true }
	canBeReplacedByIdenticalCall { ^true }

	*new { | chain, bufsize, magsphases, frombin=0, tobin, zeroothers=0 |
		tobin = tobin ?? {bufsize/2};
		^this.multiNewList(['control', chain, bufsize, frombin, tobin, zeroothers, magsphases.size] ++ magsphases.asArray)
	}

	fftSize {^this.inputs[1]}

}

// Conveniences to apply calculations to an FFT chain
PV_ChainUGen : UGen {

	// Give it a func to apply to whole set of vals: func(mags, phases)
	pvcalc { |numframes, func, frombin=0, tobin, zeroothers=0|
		var origmagsphases, magsphases, ret;
		origmagsphases = UnpackFFT(this, numframes, frombin, tobin).clump(2).flop;
		magsphases = func.value(origmagsphases[0], origmagsphases[1]);
		// Add phases back if they've been ignored
		magsphases = magsphases.size.switch(
			1, {magsphases ++ origmagsphases[1]},
			2, {magsphases},
			// any larger than 2 and we assume it's a list of magnitudes
			{[magsphases, origmagsphases[1]]}
		);
		magsphases = magsphases.flop.flatten;
		^PackFFT(this, numframes, magsphases, frombin, tobin, zeroothers);
	}
	// The same but for two chains together
	pvcalc2 { |chain2, numframes, func, frombin=0, tobin, zeroothers=0|
		var origmagsphases, origmagsphases2, magsphases, ret;
		origmagsphases  = UnpackFFT(this,   numframes, frombin, tobin).clump(2).flop;
		origmagsphases2 = UnpackFFT(chain2, numframes, frombin, tobin).clump(2).flop;
		magsphases = func.value(origmagsphases[0], origmagsphases[1], origmagsphases2[0], origmagsphases2[1]);
		// Add phases back if they've been ignored
		magsphases = magsphases.size.switch(
			1, {magsphases ++ origmagsphases[1]},
			2, {magsphases},
			// any larger than 2 and we assume it's a list of magnitudes
			{[magsphases, origmagsphases[1]]}
		);
		magsphases = magsphases.flop.flatten;
		^PackFFT(this, numframes, magsphases, frombin, tobin, zeroothers);
	}

	// Give it a func to apply to each bin in turn: func(mag, phase, index)
	pvcollect { |numframes, func, frombin=0, tobin, zeroothers=0|
		var magsphases, ret;
		magsphases = UnpackFFT(this, numframes, frombin, tobin).clump(2);
		magsphases = magsphases.collect({ |mp, index|
			ret = func.value(mp[0], mp[1], index + frombin, index).asArray;
			ret = if(ret.size==1, {ret ++ mp[1]}, ret); // Add phase if it's been ignored
		}).flatten;
		^PackFFT(this, numframes, magsphases, frombin, tobin, zeroothers);
	}

	// Return a BufFrames.
	// Travels up the chain until an FFT or FFTTrigger is reached, these classes must override this method, ending the recursion.
	// If this is called inside the optimisation path, you must call initEdges on the result.
	fftSize { ^inputs[0].fftSize }

	optimize {
		var desc = descendants.select { |d|
			// Get all descendants that write (not *just* read) to a buffer that aren't PV_Copy.
			// This will incorrectly select UGens that read and write to different buffers but don't write to this buffer,
			//   and insert an unnecessary PV_Copy, but this is rare and deemed worth the performance sacrifice.
			d.isKindOf(PV_Copy).not and: {
				d.resourceManagers !? { |m|
					m.includes(UGenBufferResourceManager) and: { d.bufferAccessType != \read }
				} ?? { true }
			}
		};

		desc = desc.removeIdenticalDuplicates;

		if (desc.size > 1) {
			var result = SynthDefOptimisationResult();
			var first = desc[0];
			desc.drop(-1).do { |d|
				var newBuffer = LocalBuf(this.fftSize.initEdges).initEdges;
				var newCopy = PV_Copy.newDuringOptimisation(\audio, this, newBuffer);

				var d_in = d.inputs.indexOfAll(this);
				d_in.do { |di| d.replaceInputAt(di, newCopy) };

				newCopy.createWeakConnectionTo(first); // ensures copy happens before the other buffer operation.

				result.addUGen(newBuffer, 1);
				result.addUGen(newCopy, 2);
			};
			^result
		};
		^nil;
	}
}
