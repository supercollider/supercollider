/**
"Unpack FFT" UGens (c) 2007 Dan Stowell.
Magical UGens for treating FFT data as demand-rate streams.
*/

// Actually this just wraps up a bundle of Unpack1FFT UGens
UnpackFFT : MultiOutUGen {
	*new { | chain, bufsize, frombin=0, tobin |
		var upperlimit = bufsize/2;
		tobin = if(tobin.isNil, upperlimit, {tobin.min(upperlimit)});
		^[Unpack1FFT(chain, bufsize, (frombin..tobin), 0),
		  Unpack1FFT(chain, bufsize, (frombin..tobin), 1)].flop.flatten;
	}
}

Unpack1FFT : UGen {
	*new { | chain, bufsize, binindex, whichmeasure=0 |
		//("bufsize:"+bufsize).postln;
		^this.multiNew('demand', chain, bufsize, binindex, whichmeasure);
	}
}

// This does the demanding, to push the data back into an FFT buffer.
PackFFT : PV_ChainUGen {

	*new { | chain, bufsize, magsphases, frombin=0, tobin, zeroothers=0 |
		tobin = tobin ?? {bufsize/2};
		^this.multiNewList(['control', chain, bufsize, frombin, tobin, zeroothers, magsphases.size] ++ magsphases.asArray)
	}

}

// Conveniences to apply calculations to an FFT chain
PV_ChainUGen : WidthFirstUGen {

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

}
