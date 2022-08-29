TestUGen_RTAlloc : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		server.options.memSize = 1024;
		server.options.blockSize = 64;
		this.bootServer(server);
	}

	tearDown {
		server.quit.remove;
	}

	test_allocFail_clearUnit {
		var cond = CondVar(), output = -1;
		{ DelayN.ar(DC.ar(1), 100); }.loadToFloatArray(1 / server.sampleRate, server, {|val|
			output = val[0];
			cond.signalOne();
		});

		cond.waitFor(1, { output != -1 });
		this.assertFloatEquals(output, 0.0, "a failed RTAlloc should clear unit outputs")
	}

	test_allocFail_continueProcessing_sameNode {
		var cond = CondVar(), output = -1;
		{ DelayN.ar(Silent.ar, 100); DC.ar(1)}.loadToFloatArray(1 / server.sampleRate, server, {|val|
			output = val[0];
			cond.signalOne();
		});

		cond.waitFor(1, { output != -1 });
		this.assertFloatEquals(output, 1.0, "a failed RTAlloc should not block other units in the same node")
	}

	test_allocFail_continueProcessing_otherNodes {
		var cond = CondVar(), output = -1;
		server.makeBundle(nil) {
			{ DelayN.ar(Silent.ar, 100) }.play(server);
			{ DC.ar(1) }.loadToFloatArray(1 / server.sampleRate, server, {|val|
				output = val[0];
				cond.signalOne();
			});
		};

		cond.waitFor(1, { output != -1 });
		this.assertFloatEquals(output, 1.0, "a failed RTAlloc should not block other nodes in the processing chain")
	}

	test_allocFail_setDoneFlag {
		var cond = CondVar(), output = -1;
		{
			K2A.ar(Done.kr(DelayN.ar(Silent.ar, 100)))
		}.loadToFloatArray(1 / server.sampleRate, server, {|val|
			output = val[0];
			cond.signalOne();
		});

		cond.waitFor(1, { output != -1 });
		this.assertFloatEquals(output, 1.0, "a failed RTAlloc should set the Done flag")
	}

	// TEST ALL UGENS
	// for each UGen in /server/plugins which calls RTAlloc
	// test successful alloc: should output non-zero values
	// test failed alloc: should only output zeros

	// FFT UGens needs separate testing (see below)
	// Some UGens were left out (see end of file)

	test_allUGens {

		var getDelayTest = { |ugen| { ugen.ar(DC.ar(1), 1, 0) } };

		var ugenTests = (
			AllpassC: getDelayTest.value(AllpassC),
			AllpassL: getDelayTest.value(AllpassL),
			AllpassN: getDelayTest.value(AllpassN),
			CombC: getDelayTest.value(CombC),
			CombL: getDelayTest.value(CombL),
			CombN: getDelayTest.value(CombN),
			DelayC: getDelayTest.value(DelayC),
			DelayL: getDelayTest.value(DelayL),
			DelayN: getDelayTest.value(DelayN),
			Pluck: { Pluck.ar(DC.ar(1), 1, 1, 0.001) },
			Pitch: { K2A.ar(Pitch.kr(SinOsc.ar, execFreq:1)[0]).round },

			RunningSum: { RunningSum.ar(DC.ar(1), 2**12) },

			Convolution: { Convolution.ar(DC.ar(1), DC.ar(1), 2**6) },
			Convolution2: { Convolution2.ar(DC.ar(1), LocalBuf(2**6), 1, 2**6) },
			Convolution2L: { Convolution2L.ar(DC.ar(1), LocalBuf(2**6), 1, 2**6) },
			Convolution3: { Convolution3.ar(DC.ar(1), LocalBuf(2**8), 1, 2**8) },
			StereoConvolution2L: {
				StereoConvolution2L.ar(DC.ar(1), LocalBuf(2**6), LocalBuf(2**6), 1, 2**6)
			},
			GrainBuf: {
				GrainBuf.ar(1, Impulse.kr(1000), 1, LocalBuf(1).set(1), maxGrains:1024)
			},
			GrainFM: { GrainFM.ar(1, Impulse.kr(1000), 1, maxGrains: 1024) },
			GrainIn: { GrainIn.ar(1, Impulse.kr(1000), in:DC.ar(1), maxGrains:1024) },
			GrainSin: { GrainSin.ar(1, Impulse.kr(1000), maxGrains: 1024) },

			Gendy1: { Gendy1.ar(initCPs: 1024) },
			Gendy2: { Gendy2.ar(initCPs: 1024) },
			Gendy3: { Gendy3.ar(initCPs: 1024) },

			IFFT: { IFFT(LocalBuf(2**9)) },
			SpecPcile: { K2A.ar(SpecPcile.kr(LocalBuf(2**10))) },

			// GVerb needs more tests, as it can fail multiple ways
			GVerb: { GVerb.ar(DC.ar(1), 10) }
		);

		var longerRuntimeUGens = (
			// these requires 0.02 seconds to run != 0
			Limiter: { Limiter.ar(DC.ar(1), dur: 0.01) },
			Normalizer: { Normalizer.ar(DC.ar(1), dur: 0.01) },
			KeyTrack: { KeyTrack.kr(FFT(LocalBuf(2**10), SinOsc.ar, 0.25), 0, 0) },
			BeatTrack2: { BeatTrack2.kr(FFT(LocalBuf(2**8)), 10) },
			PitchShift: { PitchShift.ar(SinOsc.ar, 0.01) }, // here 0.01s would be fine
			// Warning: not really testing allocFail here, because FFT fails first
			// however, we need the extra runtime for allocPass to work
			Onsets: { K2A.ar(Onsets.kr(FFT(LocalBuf(2**8), Impulse.ar(1)))) }
		);

		var onlyPassUGens = (
			// Couldn't reliably test allocFail on these:
			IEnvGen: { IEnvGen.ar(Env([1,1]), 0) },
			PanAz: { PanAz.ar(12, DC.ar(1)) },
			Klank: { Klank.ar(`[100!12, nil, 1], DC.ar(1)) },
			Klang: { Klang.ar(`[100!12, nil, 1]) },
			Dshuf: { Demand.ar(DC.ar(1),0, Dshuf(1!12)) },
			// hard to make these fail before LocalBuf or FFT fails
			Loudness: { K2A.ar(Loudness.kr(FFT(LocalBuf(2**8), SinOsc.ar))) },
			BeatTrack: { K2A.ar(BeatTrack.kr(FFT(LocalBuf(2**8)))) },
			MFCC: { K2A.ar(MFCC.kr(FFT(LocalBuf(2**8)), 60)) }
		);

		// test fuctions:

		// function to alloc LocalBuf to fill realtime memory:
		// maxFrames = memSizeBytes / frameBytes = 1024 * 1000 / 4
		var fillMemFunc = { LocalBuf(1024 * 1000 / 4) };

		var testAllocPass = { |name, func, runtime = 0.001|
			var cond = CondVar(), nonZeroOut = nil;
			func.loadToFloatArray(runtime, server) { |out|
				nonZeroOut = out.sum != 0;
				cond.signalOne();
			};
			cond.waitFor(1) { nonZeroOut.notNil };
			if (nonZeroOut.notNil) {
				this.assert(nonZeroOut,
					"% allocPass test should produce non-zero output".format(name)
				);
			} {
				this.assert(false, "% allocPass test should complete".format(name));
			}
		};

		var testAllocFail = { |name, func, runtime = 0.001|
			var cond = CondVar(), zeroOut = nil;
			func.loadToFloatArray(runtime, server) { |out|
				zeroOut = out.sum == 0;
				cond.signalOne();
			};
			cond.waitFor(1) { zeroOut.notNil };
			if (zeroOut.notNil) {
				this.assert(zeroOut,
					"% allocFail test should produce zero output".format(name.asString)
				);
			} {
				this.assert(false, "% allocFail test should complete".format(name.asString));
			}
		};

		// special test for PartConv: requires buffer alloc
		var partConvTest = {
			var b = Buffer.loadCollection(server, 1!(2**9));
			var testFunc = { PartConv.ar(DC.ar(1), 2**8, b) };
			var memFill;
			server.sync;
			testAllocPass.value("PartConv", testFunc, 0.1);
			memFill = fillMemFunc.play(server);
			testAllocFail.value("PartConv", testFunc, 0.1);
			memFill.free;
		};

		var memFill;

		// RUN TESTS

		ugenTests.keysValuesDo {|name, func|
			testAllocPass.value(name.asString, func, 0.001);
		};

		onlyPassUGens.keysValuesDo {|name, func|
			testAllocPass.value(name.asString, func, 0.001);
		};

		longerRuntimeUGens.keysValuesDo {|name, func|
			testAllocPass.value(name.asString, func, 0.02);
		};

		// now fill mem and test allocFail
		memFill = fillMemFunc.play(server);

		ugenTests.keysValuesDo {|name, func|
			testAllocFail.value(name.asString, func, 0.001);
		};

		longerRuntimeUGens.keysValuesDo {|name, func|
			testAllocFail.value(name.asString, func, 0.02);
		};

		memFill.free;
		// now test PartConv
		partConvTest.value;

	}

	// special test for LocalBuf alloc
	test_LocalBuf {
		var oneFrame = server.sampleRate.reciprocal;
		var completed = nil, cond = CondVar();
		{ K2A.ar(LocalBuf(512)) }.loadToFloatArray(oneFrame, server) { |out|
			this.assert(out[0] > -1, "should allocate a valid LocalBuf");
			completed = true;
			cond.signalOne;
		};
		cond.waitFor(1) { completed.notNil };
		if (completed.isNil) {
			this.assert(false, "successful alloc test should complete");
		};
		completed = nil;
		{ K2A.ar(LocalBuf(2**20)) }.loadToFloatArray(oneFrame, server) { |out|
			this.assertFloatEquals(out[0], -1, "should return -1 on alloc fail");
			completed = true;
			cond.signalOne;
		};
		cond.waitFor(1) { completed.notNil };
		if (completed.isNil) {
			this.assert(false, "failed alloc test should complete");
		};
	}

	// FFT UGens
	// Test FFT and PV UGens using RTAlloc

	// FFT should return always -1 when alloc fails
	test_FFT {
		var completed = nil, cond = CondVar();
		var runtime = 2**9 / server.sampleRate / 4;
		{ FFT(LocalBuf(2**9), DC.ar(1), 0.25) }.loadToFloatArray(runtime, server) { |out|
			this.assert(out.any(_ > 0), "should return a valid bufnum");
			completed = true;
			cond.signalOne;
		};
		cond.waitFor(runtime + 1) { completed.notNil };
		if (completed.isNil) {
			this.assert(false, "successful alloc test should complete");
		};
		completed = nil;
		runtime = 2**18 / server.sampleRate / 4;
		{ FFT(LocalBuf(2**18), DC.ar(1), 0.25) }.loadToFloatArray(runtime, server) { |out|
			this.assert(out.every(_ == (-1)), "should only return -1 on alloc fail");
			completed = true;
			cond.signalOne;
		};
		cond.waitFor(runtime + 1) { completed.notNil };
		if (completed.isNil) {
			this.assert(false, "failed alloc test should complete");
		};
	}

	// PV_UGens
	// we need to call .drop(1) on UGen output when testing
	// because all these UGens just output their input for the first block, without calling _next
	// (thus returning the valid bufnum even for failed allocations)
	test_PVUGens {
		var smallBuf = Buffer.alloc(server, 2**9);
		var bigBuf = Buffer.alloc(server, 2**19);

		var testAllocPass = { |ugen, testFunc|
			var completed = nil, cond = CondVar();
			testFunc.loadToFloatArray(0.1, server) { |out|
				var noneIsInvalid = out.drop(1).any(_ == (-1)).not;
				this.assert(noneIsInvalid,
					"% successful alloc should return a valid bufnum".format(ugen)
				);
				completed = true;
				cond.signalOne;
			};
			cond.waitFor(1) { completed.notNil };
			if (completed.isNil) {
				this.assert(false, "% successful alloc test should complete".format(ugen));
			};
		};

		var testAllocFail = { |ugen, testFunc|
			var completed = nil, cond = CondVar();
			testFunc.loadToFloatArray(0.1, server) { |out|
				var allInvalid = out.drop(1).every(_ == (-1));
				this.assert(allInvalid,
					"% should only return -1 on alloc fail".format(ugen)
				);
				completed = true;
				cond.signalOne;
			};
			cond.waitFor(1) { completed.notNil };
			if (completed.isNil) {
				this.assert(false, "% failed alloc test should complete".format(ugen));
			};
		};

		server.sync;

		[PV_RandComb, PV_Diffuser, PV_MagFreeze, PV_BinScramble].do { |ugen|
			testAllocPass.value(ugen, { ugen.value.new(smallBuf) });
			testAllocFail.value(ugen, { ugen.value.new(bigBuf) });
		};

		testAllocPass.value(PV_RandWipe, {PV_RandWipe(smallBuf, smallBuf)});
		testAllocFail.value(PV_RandWipe, {PV_RandWipe(bigBuf, bigBuf)});
	}

}


/*
// These UGens are left out:

// MaxLocalBufs is delicate, can leak when calling .increment alone
// { m = MaxLocalBufs(); 128.do{ m.increment } }.play

// AudioControl, Demand, LocalIn (memory depends on numOutputs)
// LagControl, OffsetOut, Out (memory depends on numInputs)
// difficult to fail alloc before "exceeded number of interconnect buffers"

// Poll, Dpoll, SendReply
// memory depends on string size
// difficult to allocFail before /s_new out of real time memory
// {Duty.kr(0.5, 0, Dpoll(Dseq([1], inf), String.fill(400000, $a)))}.play
*/
