/*
TestCoreUGens.run
*/
TestCoreUGens : UnitTest {
	test_ugen_generator_equivalences {
		this.bootServer;
		
		// These pairs should generate the same shapes, so subtracting should give zero:
		[
		 {Line.ar(0,1,1) - LFSaw.ar(0.5)},
		 
		 // (Integrator goes a bit off ramp cos of roundoff error accumulations)
		 {Line.ar(0,1,1) - Integrator.ar(DC.ar(SampleDur.ir))},
		 
		]
		.do{|func| func.loadToFloatArray(1, Server.default, { |data|
				this.assertArrayFloatEquals(data, 0, within: 0.001)
			});
		};
		
		
		1.6.wait; // enough time for 1-second generators to run and to report back.
	}
}
