TestUGen_RTAlloc : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		server.options.sampleRate = 48000;
		server.options.memSize = 1024;
		server.options.blockSize = 64;
		this.bootServer(server);
	}

	tearDown {
		server.quit.remove;
	}

	// Async helpers

	awaitSynthOutput { |synthFn, runtimeSamples=1, timeout=1|
		var cond = CondVar(), synthOutput = nil;
		synthFn.loadToFloatArray(runtimeSamples/server.sampleRate, server) { |out|
			synthOutput = out;
			cond.signalOne();
		};
		cond.waitFor(timeout) { synthOutput.notNil };
		^synthOutput;
	}

	assertAllocPass { |name, func, runtimeSamples=(server.options.blockSize)|
		var out = this.awaitSynthOutput(func, runtimeSamples);
		if (out.isNil) {
			this.assert(false, "% allocPass test should complete".format(name))
		} {
			this.assert(out.sum != 0,
				"% allocPass test should produce non-zero output".format(name),
				// out == 0 // report only if assert fails
			);
		};
	}

	assertAllocFail { |name, func, runtimeSamples=(server.options.blockSize)|
		var out = this.awaitSynthOutput(func, runtimeSamples);
		if (out.isNil) {
			this.assert(false, "% allocFail test should complete".format(name))
		} {
			this.assertEquals(out.sum, 0,
				"% allocFail test should produce zero output".format(name),
				// out != 0 // report only if assert fails
			);
		};
	}

	// Tests

	test_allocFail_clearUnit {
		var output = this.awaitSynthOutput {
			DelayN.ar(DC.ar(1), 100)
		};
		this.assertEquals(output[0], 0.0,
			"a failed RTAlloc should clear unit outputs")
	}

	test_allocFail_continueProcessing_sameNode {
		var output = this.awaitSynthOutput {
			DelayN.ar(DC.ar(1), 100); DC.ar(1)
		};
		this.assertEquals(output[0], 1.0,
			"a failed RTAlloc should not block other units in the same node")
	}

	test_allocFail_continueProcessing_otherNodes {
		var output = nil;
		var memFill = { DelayN.ar(Silent.ar, 100) }.play(server);
		server.sync;
		output = this.awaitSynthOutput { DC.ar(1) };
		memFill.free;
		this.assertEquals(output[0], 1.0,
			"a failed RTAlloc should not block other nodes in the processing chain")
	}

	test_allocFail_setDoneFlag {
		var output = this.awaitSynthOutput {
			K2A.ar(Done.kr(DelayN.ar(Silent.ar, 100)))
		};
		this.assert(output[0] > 0,
			"a failed RTAlloc should set the Done flag")
	}

	// TEST ALL UGENS
	// for each UGen in /server/plugins which calls RTAlloc
	// test successful alloc: should output non-zero values
	// test failed alloc: should only output zeros

	// FFT UGens needs separate testing (see below)
	// Some UGens were left out (see end of file)


	test_allUGens_allocPass {

		var memSizeKb = server.options.memSize;
		var memSizeFloats = memSizeKb * 1024 / 4;

		this.assertAllocPass("AllpassC", { AllpassC.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("AllpassL", { AllpassL.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("AllpassN", { AllpassN.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("CombC", { CombC.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("CombL", { CombL.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("CombN", { CombN.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("DelayC", { DelayC.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("DelayL", { DelayL.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("DelayN", { DelayN.ar(DC.ar(1), 1, 0) });
		this.assertAllocPass("Pluck", { Pluck.ar(DC.ar(1), 1, 1, 1e-4) });
		this.assertAllocPass("RunningSum", { RunningSum.ar(DC.ar(1), 1) });
		this.assertAllocPass("PitchShift", { PitchShift.ar(SinOsc.ar, 1e-6) });
		this.assertAllocPass("Pitch", { K2A.ar(Pitch.kr(SinOsc.ar)[0]).round });
		this.assertAllocPass("Limiter", { Limiter.ar(DC.ar(1), dur: 1e-6) });
		this.assertAllocPass("Normalizer", { Normalizer.ar(DC.ar(1), dur: 1e-6) });

		this.assertAllocPass("Convolution", { Convolution.ar(DC.ar(1), DC.ar(1), 64) });
		this.assertAllocPass("Convolution2", { Convolution2.ar(DC.ar(1), LocalBuf(256).set(1), 1, 64) });
		this.assertAllocPass("Convolution2L", { Convolution2L.ar(DC.ar(1), LocalBuf(256).set(1), 1, 64) });
		this.assertAllocPass("Convolution3", { Convolution3.ar(DC.ar(1), LocalBuf(256).set(1), 1, 64) });
		this.assertAllocPass("StereoConvolution2L", {
			var k = LocalBuf(64).set(1); StereoConvolution2L.ar(DC.ar(1), k, k, 1, 64)
		});
		this.assertAllocPass("GrainBuf", { GrainBuf.ar(1, 1, 1, LocalBuf(1).set(1), maxGrains: 2)});
		this.assertAllocPass("GrainFM", { GrainFM.ar(1, 1, 1, maxGrains: 2) });
		this.assertAllocPass("GrainIn", { GrainIn.ar(1, 1, in:DC.ar(1), maxGrains:2) });
		this.assertAllocPass("GrainSin", { GrainSin.ar(1, 1, maxGrains: 2) });

		this.assertAllocPass("Gendy1", { Gendy1.ar(initCPs: 1024) });
		this.assertAllocPass("Gendy2", { Gendy2.ar(initCPs: 1024) });
		this.assertAllocPass("Gendy3", { Gendy3.ar(initCPs: 1024) });

		this.assertAllocPass("IFFT", { IFFT(LocalBuf(256)) });

		this.assertAllocPass("SpecPcile", { K2A.ar(SpecPcile.kr(LocalBuf(256))) });

		this.assertAllocPass("GVerb", { GVerb.ar(DC.ar(1), 1, maxroomsize: 100) });

		// -- ONLY PASS UGENS: couldn't reliably test allocFail on these:
		this.assertAllocPass("IEnvGen", { IEnvGen.ar(Env([1,1]), 0) });
		this.assertAllocPass("PanAz", { PanAz.ar(12, DC.ar(1)) });
		this.assertAllocPass("Klank", { Klank.ar(`[100!2, nil, 1], DC.ar(1)) });
		this.assertAllocPass("Klang", { Klang.ar(`[100!2, nil, 1]) });
		this.assertAllocPass("Dshuf", { Demand.ar(DC.ar(1),0, Dshuf([1])) });
		// FFT UGens: only tested for allocPass (hard to make these fail before LocalBuf or FFT fails)
		this.assertAllocPass("Onsets", { K2A.ar(Onsets.kr(FFT(LocalBuf(128), Impulse.ar(1)))) });
		this.assertAllocPass("Loudness", { K2A.ar(Loudness.kr(FFT(LocalBuf(128), SinOsc.ar))) });
		this.assertAllocPass("BeatTrack", { K2A.ar(BeatTrack.kr(FFT(LocalBuf(128)))) });

		// special test for PartConv: requires buffer alloc
		{
			var b = Buffer.loadCollection(server, 1!(512));
			server.sync;
			this.assertAllocPass("PartConv", { PartConv.ar(DC.ar(1), 256, b) }, 128);
			b.free;
		}.value;
	}

	test_allUGens_allocFail {

		var memSizeKb = server.options.memSize;
		var memSizeFloats = memSizeKb * 1024 / 4;

		this.assertAllocFail("AllpassC", { AllpassC.ar(DC.ar(1), 100) });
		this.assertAllocFail("AllpassL", { AllpassL.ar(DC.ar(1), 100) });
		this.assertAllocFail("AllpassN", { AllpassN.ar(DC.ar(1), 100) });
		this.assertAllocFail("CombC", { CombC.ar(DC.ar(1), 100, 0) });
		this.assertAllocFail("CombL", { CombL.ar(DC.ar(1), 100, 0) });
		this.assertAllocFail("CombN", { CombN.ar(DC.ar(1), 100, 0) });
		this.assertAllocFail("DelayC", { DelayC.ar(DC.ar(1), 100, 0) });
		this.assertAllocFail("DelayL", { DelayL.ar(DC.ar(1), 100, 0) });
		this.assertAllocFail("DelayN", { DelayN.ar(DC.ar(1), 100, 0) });
		this.assertAllocFail("Pluck", { Pluck.ar(DC.ar(1), 1, 100, 0.001) });
		this.assertAllocFail("RunningSum", { RunningSum.ar(DC.ar(1), 2**20) });
		this.assertAllocFail("PitchShift", { PitchShift.ar(SinOsc.ar, 100) });
		// alloc big buffer via very small minFreq
		this.assertAllocFail("Pitch", { K2A.ar(Pitch.kr(SinOsc.ar, minFreq: 0.1)[0]).round });
		this.assertAllocFail("Limiter", { Limiter.ar(DC.ar(1), dur: 100) });
		this.assertAllocFail("Normalizer", { Normalizer.ar(DC.ar(1), dur: 100) });

		this.assertAllocFail("Convolution", { Convolution.ar(DC.ar(1), DC.ar(1), 2**20) });
		this.assertAllocFail("Convolution2", { Convolution2.ar(DC.ar(1), LocalBuf(256).set(1), 1, 2**20) });
		this.assertAllocFail("Convolution2L", { Convolution2L.ar(DC.ar(1), LocalBuf(256).set(1), 1, 2**20) });
		this.assertAllocFail("Convolution3", { Convolution3.ar(DC.ar(1), LocalBuf(256).set(1), 1, 2**20) });
		this.assertAllocFail("StereoConvolution2L", {
			var k = LocalBuf(64).set(1); StereoConvolution2L.ar(DC.ar(1), k, k, 1, 2**20)
		});
		// grain UGens: allocFail with high maxGrains
		this.assertAllocFail("GrainBuf", { GrainBuf.ar(1, 1, 1, LocalBuf(1).set(1), maxGrains: 1e5)});
		this.assertAllocFail("GrainFM", { GrainFM.ar(1, 1, 1, maxGrains: 1e5) });
		this.assertAllocFail("GrainIn", { GrainIn.ar(1, 1, in:DC.ar(1), maxGrains: 1e5) });
		this.assertAllocFail("GrainSin", { GrainSin.ar(1, 1, maxGrains: 1e5) });

		this.assertAllocFail("Gendy1", { Gendy1.ar(initCPs: 2**20) });
		this.assertAllocFail("Gendy2", { Gendy2.ar(initCPs: 2**20) });
		this.assertAllocFail("Gendy3", { Gendy3.ar(initCPs: 2**20) });

		// these UGens allocate same size as FFT buf
		// allocating memSizeFloats * 0.75 assures alloc fails in UGen and not in LocalBuf
		this.assertAllocFail("IFFT", { IFFT(LocalBuf(memSizeFloats * 0.75)) });
		this.assertAllocFail("SpecPcile", { K2A.ar(SpecPcile.kr(LocalBuf(memSizeFloats * 0.75))) });

		// GVerb needs more tests, as it can fail multiple ways
		this.assertAllocFail("GVerb", { GVerb.ar(DC.ar(1), 1, maxroomsize: 10000) });

		// PartConv doesn't need a valid buffer when it fails allocation
		this.assertAllocFail("PartConv", { PartConv.ar(DC.ar(1), 2**20, -1) });
	}

	// special test for LocalBuf alloc
	test_LocalBuf {
		var out = this.awaitSynthOutput({ K2A.ar(LocalBuf(512)) }, 1);
		if (out.isNil) {
			this.assert(false, "successful alloc test should complete");
		} {
			var bufnum = out[0];
			this.assert(bufnum > -1, "should allocate a valid LocalBuf (bufnum: %)".format(bufnum));
		};

		out = this.awaitSynthOutput({ K2A.ar(LocalBuf(2**20)) }, 1);
		if (out.isNil) {
			this.assert(false, "failed alloc test should complete");
		} {
			this.assertEquals(out[0], -1, "should return -1 on alloc fail");
		};
	}

	// FFT UGens: test RTAlloc success and fail
	test_FFT {
		var out = this.awaitSynthOutput({ FFT(LocalBuf(2**8), DC.ar(1), 0.25) }, 2**6);
		if (out.isNil) {
			this.assert(false, "successful alloc test should complete");
		} {
			this.assert(out.any(_ > 0), "should return a valid bufnum (bufnum: %)".format(out));
		};
		// FFT should return always -1 when alloc fails
		out = this.awaitSynthOutput({ FFT(LocalBuf(2**20), DC.ar(1), 0.25) }, 2**8);
		if (out.isNil) {
			this.assert(false, "failed alloc test should complete");
		} {
			this.assert(out.every(_ == -1), "should only return -1 on alloc fail");
		}
	}

	// PV_UGens
	// we need to call .drop(1) on UGen output when testing
	// because all these UGens just output their input for the first block, without calling _next
	// (thus returning the valid bufnum even for failed allocations)
	test_PVUGens {
		var smallBuf = Buffer.alloc(server, 2**9);
		var bigBuf = Buffer.alloc(server, 2**19);

		var testAllocPass = { |ugen, testFunc|
			var out = this.awaitSynthOutput(testFunc, 1024);
			if (out.isNil) {
				this.assert(false, "% successful alloc test should complete".format(ugen));
			} {
				var noneIsInvalid = out.drop(1).any(_ == (-1)).not;
				this.assert(noneIsInvalid,
					"% successful alloc should return a valid bufnum".format(ugen)
				);
			};
		};

		var testAllocFail = { |ugen, testFunc|
			var out = this.awaitSynthOutput(testFunc, 1024);
			if (out.isNil) {
				this.assert(false, "% failed alloc test should complete".format(ugen));
			} {
				var allInvalid = out.drop(1).every(_ == (-1));
				this.assert(allInvalid,
					"% should only return -1 on alloc fail".format(ugen)
				);
			};
		};

		server.sync;

		[PV_RandComb, PV_Diffuser, PV_MagFreeze, PV_BinScramble].do { |ugen|
			testAllocPass.value(ugen, { ugen.new(smallBuf) });
			testAllocFail.value(ugen, { ugen.new(bigBuf) });
		};

		testAllocPass.value(PV_RandWipe, {PV_RandWipe(smallBuf, smallBuf)});
		testAllocFail.value(PV_RandWipe, {PV_RandWipe(bigBuf, bigBuf)});
	}

}


/*
// These UGens are left out:

// BeatTrack2 requires a special test, doesn't work with .loadToFloatArray (defName too long)

// Onsets, Loudness, BeatTrack, and KeyTrack can't be easily tested for allocFail: it's hard not to make FFT fail first
// KeyTrack is also unreliable for allocPass

// MaxLocalBufs is delicate, can leak when calling .increment alone
// { m = MaxLocalBufs(); 128.do{ m.increment } }.play

// AudioControl, Demand, LocalIn (memory depends on numOutputs)
// LagControl, OffsetOut, Out (memory depends on numInputs)
// difficult to fail alloc before "exceeded number of interconnect buffers"

// Poll, Dpoll, SendReply
// memory depends on string size
// difficult to allocFail before `/s_new out of real time memory`
// {Duty.kr(0.5, 0, Dpoll(Dseq([1], inf), String.fill(400000, $a)))}.play
*/
