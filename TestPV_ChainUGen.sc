/*
TestPV_ChainUGen.run
UnitTest.gui
*/
TestPV_ChainUGen : UnitTest {

// This test manually creates an FFT frame equivalent to pure white noise, 
// on which certain ugens' action should be a no-op
test_whiteframe_noop {
	var fakeframe;
	
	// Every real is 1.0, every imag is 0.0. same as every mag is 1.0, every phase is 0.0.
	fakeframe = ([1,1] ++ {[1,0]}.dup(511)).flat;
	
	// All of these should be no-ops when applied to a faked frame with reals all 1 and imags all 0:
	this.pv_equivalencetests_common(fakeframe,
		Dictionary[
			"PV_MagAbove"   -> {|f| PV_MagAbove(f, 0.9)},
			"PV_MagBelow"   -> {|f| PV_MagBelow(f, 1.1)},
			"PV_MagClip"    -> {|f| PV_MagClip(f, 1)},
			"PV_MagSmear"   -> {|f| PV_MagSmear(f, 0)},
			"PV_BinShift"   -> {|f| PV_BinShift(f)},
			"PV_MagShift"   -> {|f| PV_MagShift(f)},
			"PV_MagSquared" -> {|f| PV_MagSquared(f)},
			"PV_PhaseShift90(PV_PhaseShift270)"   -> {|f| PV_PhaseShift90(PV_PhaseShift270(f))},
			"PV_Conj"       -> {|f| PV_Conj(f)},
			"PV_MagMul"     -> {|f| PV_MagMul(f, f)},
			"PV_PhaseShift" -> {|f| PV_PhaseShift(f, 0)},
			"PV_BrickWall"  -> {|f| PV_BrickWall(f, 0)},
			"PV_BinWipe"    -> {|f| PV_BinWipe(f, f, 0.5)},
			"PV_CopyPhase"  -> {|f| PV_CopyPhase(f, f)},
			"PV_Copy"       -> {|f| PV_Copy(f, f)},
			"PV_Max"        -> {|f| PV_Max(f, f)},
			"PV_Min"        -> {|f| PV_Min(f, f)},
			"PV_Mul"        -> {|f| PV_Mul(f, f)},
			"PV_Div"        -> {|f| PV_Div(f, f)},
			"PV_MagDiv"     -> {|f| PV_MagDiv(f, f)},
			"PV_RandComb"   -> {|f| PV_RandComb(f, 0)},
			"PV_RandWipe"   -> {|f| PV_RandWipe(f, f, 0.5)},
			"PV_MagFreeze"  -> {|f| PV_MagFreeze(f, Line.kr(-1, 1, 0.05))},
			"PV_BinScramble"-> {|f| PV_BinScramble(f, 0)},
		],
		fakeframe, "No-op unity-frame test on %");
}


// Various ugens have parameter settings which are supposed to zero the data
test_whiteframe_zeroing {
	var fakeframe;
	
	// Every real is 1.0, every imag is 0.0. same as every mag is 1.0, every phase is 0.0.
	// NOTE: Here we actually set the DC and nyqquist to zero, not one, 
	//  because many ugens simply don't bother to touch them.
	fakeframe = ([0,0] ++ {[1,0]}.dup(511)).flat;
	
	// All of these should be no-ops when applied to a faked frame with reals all 1 and imags all 0:
	this.pv_equivalencetests_common(fakeframe,
		Dictionary[
			"PV_MagAbove"   -> {|f| PV_MagAbove(f, inf)},
			"PV_MagBelow"   -> {|f| PV_MagBelow(f, -inf)},
			"PV_MagClip"    -> {|f| PV_MagClip(f, 0)},
// MEH, this one leaves the edge bins AS WELL as the dc+nyq: "PV_LocalMax"   -> {|f| PV_LocalMax(f, inf)},
			"PV_BinShift"   -> {|f| PV_BinShift(f, 0, 99999)},
			"PV_MagShift"   -> {|f| PV_MagShift(f, 0, 99999)},
			"PV_BrickWall"  -> {|f| PV_BrickWall(f, 1)},
			"PV_RandComb"   -> {|f| PV_RandComb(f, 1)},
		],
		0, "% can be used to produce silence");
}

// The main engine that is used to push a fake fft frame through a ugen:
pv_equivalencetests_common { |fakeframe, tests, equaltothis, message|
	var s = this.s, b;
	this.bootServer;
	tests.keysValuesDo{|name, func|
		b = Buffer.sendCollection(s, fakeframe);
		0.05.wait;
		s.sync;
		
		{var f; f = FFTTrigger(b); func.value(f); Line.ar(0, 0, 0.05, doneAction:2)}.play;
		s.sync;
		0.2.wait;
		b.loadToFloatArray(action: { |data|
			this.assertArrayFloatEquals(data, equaltothis, message.format(name), within: 0.001, report: true)
		});
		s.sync;
		rrand(0.12, 0.15).wait;
		
		b.free;
		0.05.wait;
		s.sync;
	};
	0.8.wait; // enough time for units to report back
}


/*
PV_LocalMax - hmmm would need a differently-designed frame for noop
PV_MagNoise - any unit test possible?
*/

}
