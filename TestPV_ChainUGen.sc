/*
TestPV_ChainUGen.run
UnitTest.gui
*/
TestPV_ChainUGen : UnitTest {


// This test manually creates an FFT frame equivalent to pure white noise, 
// on which certain ugens' action should be a no-op
test_whiteframe_noop {
	var s=this.s, b, fakeframe;
	this.bootServer;
	
	// Every real is 1.0, every imag is 0.0. same as every mag is 1.0, every phase is 0.0.
	fakeframe = ([1,1] ++ {[1,0]}.dup(511)).flat;
	
	// All of these should be no-ops when applied to a faked frame with reals all 1 and imags all 0:
	Dictionary[
		"PV_MagAbove"   -> {|f| PV_MagAbove(f, 0.9)},
		"PV_MagBelow"   -> {|f| PV_MagBelow(f, 1.1)},
		"PV_MagClip"    -> {|f| PV_MagClip(f, 1)},
		"PV_LocalMax"   -> {|f| PV_MagAbove(f, 0.9)},
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
	].keysValuesDo{|name, func|
	
		// Every real is 1.0, every imag is 0.0. same as every mag is 1.0, every phase is 0.0.
		b = Buffer.sendCollection(s, fakeframe);
		0.05.wait;
		s.sync;
		
		{var f; f = FFTTrigger(b); func.value(f); Line.ar(0, 0, 0.05, doneAction:2)}.play;
		s.sync;
		0.2.wait;
		b.loadToFloatArray(action: { |data|
			this.assertArrayFloatEquals(data, fakeframe, "No-op unity-frame test on %".format(name), within: 0.001, report: true)
		});
		s.sync;
		rrand(0.12, 0.15).wait;
		
		b.free;
		0.05.wait;
		s.sync;
		
		
		
	};
	
	1.6.wait; // enough time for units to report back
}


/*
PV_LocalMax - hmmm would need a differently-designed frame for noop
PV_MagNoise - any unit test possible?
*/

}
