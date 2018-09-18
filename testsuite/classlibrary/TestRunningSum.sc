// tests both RunningSum and RunningSum2

TestRunningSum : UnitTest {

	test_runningSums {
		var advance=true, testfuncs, testOutput, numTestSamps, sumWin = 30;

		testfuncs = [
			{ RunningSum.ar(DC.ar(1), sumWin) },
			{ RunningSum2.ar(DC.ar(1), sumWin, sumWin) }
		];

		this.bootServer(Server.default);

		numTestSamps = Server.default.sampleRate / 2;

		// ramp up to sumWin and hold that value through numTestSamps
		testOutput = (1.0, 2 .. sumWin)++(numTestSamps - sumWin).asInt.collect{sumWin};

		[RunningSum, RunningSum2].do{ |ugen, i|

			this.wait{advance};
			advance = false;

			testfuncs[i].loadToFloatArray(
				0.5, // for some reason only 0.5 and 1 seem to work... no math, no vars
				Server.default,
				{ |data|
					this.assertArrayFloatEquals(
						data, testOutput, ugen.asString, within: 0.001, report:true
					);
					advance = true;
				}
			);
		};

		this.wait({advance}, "FAILED WAITING: RunningSum");
	}
}

/*
RunningSum.test
*/
