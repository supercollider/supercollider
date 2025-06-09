TestUGen_RTAlloc : UnitTest {

	var server;

	*initClass {
		passVerbosity = UnitTest.brief;
	}

	setUp {
		server = Server(this.class.name);
		server.options.memSize = 2 ** 13; // scsynth default
		// - tests fail with memSize < 256 (GVerb fails)
		// - testing with memSize >= 2 ** 20 would require multiple allocations
		//   (allocation size itself can overflow size_t for some UGens)
		server.bootSync;
	}

	tearDown {
		server.quit;
		server.remove;
	}

	// Async helpers

	awaitSynthOutput { |synthFn, runtimeSamples=(server.options.blockSize), timeout=1|
		var cond = CondVar(), synthOutput = nil;
		var runtimeSeconds = runtimeSamples / server.sampleRate;
		// allow extra timeout for longer tests (e.g. testFFT with big memSize)
		if (runtimeSeconds > timeout) {
			timeout = runtimeSeconds + timeout
		};
		synthFn.loadToFloatArray(runtimeSeconds, server) { |out|
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
				"% allocPass test should produce non-zero output".format(name)
			);
		};
	}

	assertAllocFail { |name, func, runtimeSamples=(server.options.blockSize)|
		var out = this.awaitSynthOutput(func, runtimeSamples, 3);
		if (out.isNil) {
			this.assert(false, "% allocFail test should complete".format(name))
		} {
			this.assertEquals(out.sum, 0,
				"% allocFail test should produce zero output".format(name)
			);
		};
	}

	// mem calc helpers

	memSizeKb { ^server.options.memSize }
	memSizeFloats { ^this.memSizeKb * (1024 / 4) }
	memSizeSeconds { ^this.memSizeFloats / server.sampleRate }

	// Tests

	test_allocFail_clearUnit {
		var output = this.awaitSynthOutput {
			DelayN.ar(DC.ar(1), this.memSizeSeconds * 2)
		};
		this.assert(output.notNil, "test should complete", onFailure: \stop);
		this.assertEquals(output[0], 0.0,
			"a failed RTAlloc should clear unit outputs")
	}

	test_allocFail_continueProcessing_sameNode {
		var output = this.awaitSynthOutput {
			DelayN.ar(DC.ar(1), this.memSizeSeconds * 2); DC.ar(1)
		};
		this.assert(output.notNil, "test should complete", onFailure: \stop);
		this.assertEquals(output[0], 1.0,
			"a failed RTAlloc should not block other units in the same node")
	}

	test_allocFail_continueProcessing_otherNodes {
		var output = nil;
		var dummyOutBus = Bus.audio(server, 1);
		var memFill = { DelayN.ar(Silent.ar, this.memSizeSeconds * 2) }.play(server, dummyOutBus);
		server.sync;
		output = this.awaitSynthOutput { DC.ar(1) };
		memFill.free; dummyOutBus.free;
		this.assert(output.notNil, "test should complete", onFailure: \stop);
		this.assertEquals(output[0], 1.0,
			"a failed RTAlloc should not block other nodes in the processing chain")
	}

	test_allocFail_setDoneFlag {
		var output = this.awaitSynthOutput {
			K2A.ar(Done.kr(DelayN.ar(Silent.ar, this.memSizeSeconds * 2)))
		};
		this.assert(output.notNil, "test should complete", onFailure: \stop);
		this.assert(output[0] > 0,
			"a failed RTAlloc should set the Done flag")
	}

	// TEST ALL UGENS
	// for each UGen in /server/plugins which calls RTAlloc
	// - test successful alloc: should output non-zero values
	// - test failed alloc: should only output zeros
	// NOTE: All tests run for minimum a blockSize, because less than this
	// doesn't result in any time saving

	// FFT UGens needs separate testing (see below)
	// Some UGens were left out (see end of file)

	test_allUGens_allocPass {
		// allocate as little memory as possible for each UGen
		var allocSeconds = 1 / server.sampleRate; // 1 sample
		var blockSize = server.options.blockSize;

		var convBufFrames = blockSize * 8;
		var convBuf = Buffer.loadCollection(server, 1!convBufFrames);

		var testArgs = [
			["AllpassC", { AllpassC.ar(DC.ar(1), allocSeconds, 0) }],
			["AllpassL", { AllpassL.ar(DC.ar(1), allocSeconds, 0) }],
			["AllpassN", { AllpassN.ar(DC.ar(1), allocSeconds, 0) }],
			["CombC", { CombC.ar(DC.ar(1), allocSeconds, 0) }],
			["CombL", { CombL.ar(DC.ar(1), allocSeconds, 0) }],
			["CombN", { CombN.ar(DC.ar(1), allocSeconds, 0) }],
			["DelayC", { DelayC.ar(DC.ar(1), allocSeconds, 0) }],
			["DelayL", { DelayL.ar(DC.ar(1), allocSeconds, 0) }],
			["DelayN", { DelayN.ar(DC.ar(1), allocSeconds, 0) }],
			["Pluck", { Pluck.ar(DC.ar(1), 1, allocSeconds, 1e-4) }],
			["RunningSum", { RunningSum.ar(DC.ar(1), 1) }],
			["PitchShift", { PitchShift.ar(DC.ar(1), allocSeconds) }],
			["Pitch", { K2A.ar(Pitch.kr(SinOsc.ar)[0]).round }],
			["Limiter", { Limiter.ar(DC.ar(1), dur: allocSeconds) }],
			["Normalizer", { Normalizer.ar(DC.ar(1), dur: allocSeconds) }],

			["GrainBuf", { GrainBuf.ar(1, 1, 1, LocalBuf(1).set(1), maxGrains: 2)}],
			["GrainFM", { GrainFM.ar(1, 1, 1, maxGrains: 2) }],
			["GrainIn", { GrainIn.ar(1, 1, in:DC.ar(1), maxGrains:2) }],
			["GrainSin", { GrainSin.ar(1, 1, maxGrains: 2) }],

			["Gendy1", { Gendy1.ar(initCPs: 1) }],
			["Gendy2", { Gendy2.ar(initCPs: 1) }],
			["Gendy3", { Gendy3.ar(initCPs: 2) }],

			["IFFT", { IFFT(LocalBuf(blockSize).set(1)) }],
			["SpecPcile", { K2A.ar(SpecPcile.kr(LocalBuf(blockSize))) }],
			["GVerb", { GVerb.ar(DC.ar(1), 1, maxroomsize: 1) }],

			// -- ONLY PASS UGENS: couldn't reliably test allocFail on these:
			["IEnvGen", { IEnvGen.ar(Env([1,1]), 0) }],
			["PanAz", { PanAz.ar(1, DC.ar(1)) }],
			["Klank", { Klank.ar(`[100!2, nil, 1], DC.ar(1)) }],
			["Klang", { Klang.ar(`[100!2, nil, 1]) }],
			["Dshuf", { Demand.ar(DC.ar(1),0, Dshuf([1])) }],
			// FFT UGens: only tested for allocPass (hard to make these fail before LocalBuf or FFT fails)
			["Onsets", { K2A.ar(Onsets.kr(FFT(LocalBuf(blockSize*2), Impulse.ar(1)))) }],
			["Loudness", { K2A.ar(Loudness.kr(FFT(LocalBuf(blockSize*2), SinOsc.ar))) }],
			["BeatTrack", { K2A.ar(BeatTrack.kr(FFT(LocalBuf(blockSize*2)))) }],

			["Convolution", { Convolution.ar(DC.ar(1), DC.ar(1), blockSize) }],
			["Convolution2", { Convolution2.ar(DC.ar(1), convBuf, 0, blockSize) }],
				// Convolution2L onset output is not predictable, might sum to zero sporadically: give extra time
			["Convolution2L", { Convolution2L.ar(DC.ar(1), convBuf, 0, blockSize) }],
			["Convolution3", { Convolution3.ar(DC.ar(1), convBuf, 0, blockSize) }],
			["StereoConvolution2L", { StereoConvolution2L.ar(DC.ar(1), convBuf, convBuf, 0, blockSize) }],
			["PartConv", { PartConv.ar(DC.ar(1), convBufFrames / 2, convBuf) }, convBufFrames / 4],
		];

		server.sync; // for convBuf;

		// testArgs.collect { |args| { this.assertAllocPass(*args) } }.fork;
		testArgs.do { |args| this.assertAllocPass(*args) };

		convBuf.free;
	}

	test_allUGens_allocFail {

		var allocSeconds = this.memSizeSeconds;
		var allocSamples = nextPowerOfTwo(this.memSizeFloats);
		var blockSize = server.options.blockSize;
		var testArgs = [
			["AllpassC", { AllpassC.ar(DC.ar(1), allocSeconds) }],
			["AllpassL", { AllpassL.ar(DC.ar(1), allocSeconds) }],
			["AllpassN", { AllpassN.ar(DC.ar(1), allocSeconds) }],
			["CombC", { CombC.ar(DC.ar(1), allocSeconds, 0) }],
			["CombL", { CombL.ar(DC.ar(1), allocSeconds, 0) }],
			["CombN", { CombN.ar(DC.ar(1), allocSeconds, 0) }],
			["DelayC", { DelayC.ar(DC.ar(1), allocSeconds, 0) }],
			["DelayL", { DelayL.ar(DC.ar(1), allocSeconds, 0) }],
			["DelayN", { DelayN.ar(DC.ar(1), allocSeconds, 0) }],
			["Pluck", { Pluck.ar(DC.ar(1), 1, allocSeconds, 0.001) }],
			["RunningSum", { RunningSum.ar(DC.ar(1), allocSamples) }],
			["PitchShift", { PitchShift.ar(SinOsc.ar, allocSeconds) }],
			// 	// alloc big buffer via very small minFreq
			["Pitch", { K2A.ar(Pitch.kr(SinOsc.ar, minFreq: 1 / allocSeconds)[0]).round }],
			["Limiter", { Limiter.ar(DC.ar(1), dur: allocSeconds) }],
			["Normalizer", { Normalizer.ar(DC.ar(1), dur: allocSeconds) }],

			// grain UGens: allocFail with high maxGrains
			// allocated memory = maxGrains * size(GrainStruct)
			// magic numbers are approximated from the sizes of GrainStruct for each UGen (see GrainUGens.cpp)
			["GrainBuf", { GrainBuf.ar(1, 1, 1, LocalBuf(1).set(1), maxGrains: allocSamples/20)}],
			["GrainFM", { GrainFM.ar(1, 1, 1, maxGrains: allocSamples/20) }],
			["GrainIn", { GrainIn.ar(1, 1, in:DC.ar(1), maxGrains: allocSamples/15) }],
			["GrainSin", { GrainSin.ar(1, 1, maxGrains: allocSamples/15) }],

			["Gendy1", { Gendy1.ar(initCPs: allocSamples) }],
			["Gendy2", { Gendy2.ar(initCPs: allocSamples) }],
			["Gendy3", { Gendy3.ar(initCPs: allocSamples) }],

			// these UGens allocate same size as FFT buf
			// allocating memSizeFloats * 0.75 assures alloc fails in UGen and not in LocalBuf
			["IFFT", { IFFT(LocalBuf(this.memSizeFloats * 0.75)) }],
			["SpecPcile", { K2A.ar(SpecPcile.kr(LocalBuf(this.memSizeFloats * 0.75))) }],

			// GVerb needs more tests, as it can fail multiple ways
			// memSizeSeconds * 340 produces an allocation of memSizeFloats floats
			["GVerb", { GVerb.ar(DC.ar(1), 1, maxroomsize: this.memSizeSeconds * 340) }],

			// Convolution UGens don't need a valid 
			// note: when trying to run tests in parallel, these required a separate batch or would crash the server
			["Convolution", { Convolution.ar(DC.ar(1), DC.ar(1), allocSamples) }],
			["Convolution2", { Convolution2.ar(DC.ar(1), -1, 0, allocSamples) }],
			["Convolution2L", { Convolution2L.ar(DC.ar(1), -1, 0, allocSamples) }],
			["Convolution3", { Convolution3.ar(DC.ar(1), -1, 0, allocSamples) }],
			["StereoConvolution2L", { StereoConvolution2L.ar(DC.ar(1), -1, -1, 0, allocSamples) }],
			["PartConv", { PartConv.ar(DC.ar(1), allocSamples, -1) }, server.options.blockSize * 2],
		];

		// testArgs.collect { |args| { this.assertAllocFail(*args) } }.fork;
		testArgs.do { |args| this.assertAllocFail(*args) };
	}

	// special test for LocalBuf alloc
	test_LocalBuf {
		var out = this.awaitSynthOutput({ K2A.ar(LocalBuf(1)) });
		this.assert(out.notNil, "successful alloc test should complete", onFailure: \stop);
		this.assert(out[0] > -1, "should allocate a valid LocalBuf (bufnum: %)".format(out[0]));

		out = this.awaitSynthOutput({ K2A.ar(LocalBuf(this.memSizeFloats * 10)) });
		this.assert(out.notNil, "failed alloc test should complete", onFailure: \stop);
		this.assertEquals(out[0], -1, "should return -1 on alloc fail");
	}

	// FFT UGens: test RTAlloc success and fail
	// FFT normally outputs a stream of -1. When FFT has processed a frame, which typically takes more than a blockSize,
	// on that block it will output the bufnum instead of -1.
	// (It's a way for PV_UGens to know that a new spectral frame is ready and at the same time get the bufnum)
	// In these tests we are scanning the output of FFT for valid bufnums
	test_FFT {
		var blockSize = server.options.blockSize;
		var fftSize = blockSize;
		var out = this.awaitSynthOutput({ FFT(LocalBuf(fftSize), DC.ar(1), hop:1) });
		this.assert(out.notNil, 
			"successful alloc test should complete (fftSize: %)".format(fftSize),
			onFailure: \stop);
		this.assert(out.any(_ > 0), "should return a valid bufnum (bufnum: %)".format(out));

		// FFT should return always -1 when alloc fails
		// - alloc half of memSize, so that LocalBuf can succeed, but FFT can fail
		fftSize = nextPowerOfTwo(this.memSizeFloats * 0.5);
		// - use hop = blockSize / fftSize to make FFT output frames every blockSize
		out = this.awaitSynthOutput({ FFT(LocalBuf(fftSize), DC.ar(1), hop: blockSize / fftSize) });
		this.assert(out.notNil,
			"failed alloc test should complete (fftSize: %)".format(fftSize),
			onFailure: \stop);
		this.assert(out.every(_ == -1), "should only return -1 on alloc fail");
	}

	// PV_UGens
	// we need to call .drop(1) on UGen output when testing
	// because all these UGens just output their input for the first block, without calling _next
	// (thus returning the valid bufnum even for failed allocations)
	test_PVUGens {
		var blockSize = server.options.blockSize;
		var smallBuf = Buffer.alloc(server, blockSize);
		var bigBuf = Buffer.alloc(server, nextPowerOfTwo(this.memSizeFloats) * 2);
		var noneIsInvalid, out;

		var testAllocPass = { |ugen, testFunc|
			var out = this.awaitSynthOutput(testFunc, blockSize * 4);
			if (out.isNil) {
				this.assert(false, "% successful alloc test should complete".format(ugen));
			} {
				noneIsInvalid = out.drop(1).any(_ == (-1)).not;
				this.assert(noneIsInvalid, "% successful alloc should return a valid bufnum".format(ugen));
			};
		};

		var testAllocFail = { |ugen, testFunc|
			var allInvalid;
			out = this.awaitSynthOutput(testFunc, blockSize * 4);
			if (out.isNil) {
				this.assert(false, "% failed alloc test should complete".format(ugen));
			} {
				allInvalid = out.drop(1).every(_ == (-1));
				this.assert(allInvalid, "% should only return -1 on alloc fail".format(ugen));
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
