/*
TestCoreUGens.run
*/
TestCoreUGens : UnitTest {
test_ugen_generator_equivalences {
	this.bootServer;
	
	// These pairs should generate the same shapes, so subtracting should give zero.
	// Of course there's some rounding error due to floating-point accuracy.
	[
	 //////////////////////////////////////////
	 // Ramp generators:
	 {Line.ar(0,1,1) - LFSaw.ar(0.5)},
	 {Line.ar(0,1,1) - LinXFade2.ar(DC.ar(0), DC.ar(1), Line.ar(-1,1,1))},
	 // (Integrator goes a bit off ramp cos of roundoff error accumulations)
	 {Line.ar(0,1,1) - Integrator.ar(DC.ar(SampleDur.ir))},
	 
	 //////////////////////////////////////////
	 // Linear-to-exponential equivalences:
	 {XLine.ar(0.01, 10, 1) - Line.ar( 0, 1, 1).linexp(0,1, 0.01, 10)},
	 {XLine.ar(0.01, 10, 1) - Line.ar(-1, 1, 1).exprange(   0.01, 10)},
	 {Line.ar(0, 1, 1) - XLine.ar(0.01, 10, 1).explin(0.01, 10, 0, 1)},
	 
	 //////////////////////////////////////////
	 // Trigonometric:
	 {SinOsc.ar(1) - Line.ar(0,2pi,1).sin},
	 {SinOsc.ar(1, pi/2) - Line.ar(0,2pi,1).cos},
	 
	]
	.do{|func| func.loadToFloatArray(1, Server.default, { |data|
			this.assertArrayFloatEquals(data, 0, within: 0.001)
		});
	};
	
	
	1.6.wait; // enough time for 1-second generators to run and to report back.
}
} // end TestCoreUGens class
