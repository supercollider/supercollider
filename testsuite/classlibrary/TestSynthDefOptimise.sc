// General testing strategy here is to test that the result of the optimisation is the same,
//    rather than *that* the optimisation has been applied.
// This is due to the complexities regarding:
//    * when optimisation are applied,
//    * and how they interact with each other.
// What is important is that the optimisation results in the same sound/output as the unoptimised version.

TestSynthDefOptimise : UnitTest {
	classvar printUGenGraphs = false;
	var server;

	setUp {
		server = Server(this.class.name);
		server.options.memSize = 8192 * 16;
		server.options.blockSize = 64;
		server.options.numWireBufs = 256;
		server.bootSync;
	}

	tearDown {
		Buffer.freeAll;
		server.sync;
		server.quit;
		server.remove;
	}

	*compare_create_synth_def { |opts, name, f|
		^SynthDef.newWithOptimizations(opts, name, { |bufnum|
			var outputs = SynthDef.wrap(f);
			var numOutputs, defRate;
			if(outputs.isValidUGenInput.not) {
				outputs.dump;
				Error("Reading signal failed: % is not a valid UGen input.".format(outputs)).throw
			};
			outputs = UGen.replaceZeroesWithSilence(outputs.asArray);
			numOutputs = outputs.size.max(1);
			if(numOutputs != 1) {
				Error("TestSynthDefOptimise.compare only works on mono signals").throw
			};
			defRate = outputs.rate;
			if(defRate == \audio) {
				outputs = outputs.collect { |x| if(x.rate != \audio) { K2A.ar(x) } { x } }
			};

			RecordBuf.perform(
				RecordBuf.methodSelectorForRate(defRate),
				outputs, bufnum, loop: 0, doneAction: 2
			);
		})
	}

	*compare_a_b { |withopts, withoutopts, server, threshold, forceDontPrint=false, duration=0.01|
		var a = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.all, \a, withopts).add;
		var b = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.none, \b, withoutopts).add;
		^TestSynthDefOptimise.compare_engine(server, threshold, a, b, forceDontPrint, duration)
	}

	compare_optimization_levels { |f, server, threshold, forceDontPrint=false, duration=0.01, msg, extraArgs|
		var none = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.none, \none, f).add;
		var all = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.all, \all, f).add;
		var deduplicationAndSorting = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.deduplicationAndSorting, \deduplicationAndSorting, f).add;
		var sortingAndRewriteMany = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.sortingAndRewriteMany, \sortingAndRewriteMany, f).add;
		var onlySorting = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.onlySorting, \onlySorting, f).add;
		var allSingle = TestSynthDefOptimise.compare_create_synth_def(SynthDefOptimizationFlags.allSingle, \allSingle, f).add;

		var cond = CondVar();
		var count = 5;
		msg = msg ?? { "" };

		fork {
			this.assert(
				TestSynthDefOptimise.compare_engine(server, threshold, none, all, forceDontPrint, duration, extraArgs),
				msg + "--- null test against 'all'. Num UGens none: % all: %"
				.format(none.children.size, all.children.size);
			);
			count = count - 1;
			cond.signalOne;
		};
		fork {
			this.assert(
				TestSynthDefOptimise.compare_engine(server, threshold, none, deduplicationAndSorting, forceDontPrint, duration, extraArgs),
				msg + "--- null test against 'deduplicationAndSorting'. Num UGens none: % deduplicationAndSorting: %"
				.format(none.children.size, deduplicationAndSorting.children.size);
			);
			count = count - 1;
			cond.signalOne;
		};
		fork {
			this.assert(
				TestSynthDefOptimise.compare_engine(server, threshold, none, sortingAndRewriteMany, forceDontPrint, duration, extraArgs),
				msg + "--- null test against 'sortingAndRewriteMany'. Num UGens none: % sortingAndRewriteMany: %"
				.format(none.children.size, sortingAndRewriteMany.children.size);
			);
			count = count - 1;
			cond.signalOne;
		};
		fork {
			this.assert(
				TestSynthDefOptimise.compare_engine(server, threshold, none, onlySorting, forceDontPrint, duration, extraArgs),
				msg + "--- null test against 'onlySorting'. Num UGens none: % onlySorting: %"
				.format(none.children.size, onlySorting.children.size);
			);
			count = count - 1;
			cond.signalOne;
		};
		fork {
			this.assert(
				TestSynthDefOptimise.compare_engine(server, threshold, none, allSingle, forceDontPrint, duration, extraArgs),
				msg + "--- null test against 'allSingle'. Num UGens none:% allSingle%"
				.format(none.children.size, allSingle.children.size);
			);
			count = count - 1;
			cond.signalOne;
		};
		cond.wait { count == 0 };
	}

	*compare_engine { |server, threshold, defa, defb, forceDontPrint=false, duration=0.01, extraArgs=([])|
		var withBuf = Buffer.alloc(server, duration * server.sampleRate);
		var withoutBuf = Buffer.alloc(server, duration * server.sampleRate);

		var s = server.sync;

		var withSynth, withoutSynth;

		var bind = server.bind {
			withSynth = Synth(defa.name, [\bufnum, withBuf] ++ extraArgs);
			withoutSynth = Synth(defb.name, [\bufnum, withoutBuf] ++ extraArgs);
		};

		var counter = 2;
		var cond = CondVar();

		var r;

		var withResult, withoutResult;
		if (printUGenGraphs and: { forceDontPrint.not }){
			"\nA' UGen Graph.".postln;
			defa.dumpUGens;
			"\nB UGen Graph.".postln;
			defb.dumpUGens;
		};

		OSCFunc({
			withBuf.loadToFloatArray(action: { |ar|
				withResult = ar;
				counter = counter - 1;
				cond.signalOne;
			});
		}, '/n_end', server.addr, nil, [withSynth.nodeID]).oneShot;

		OSCFunc({
			withoutBuf.loadToFloatArray(action: { |ar|
				withoutResult = ar;
				counter = counter - 1;
				cond.signalOne;
			});
		}, '/n_end', server.addr, nil, [withoutSynth.nodeID]).oneShot;

		cond.wait { counter == 0 };
		if (withResult.every ( _ == 0 )) {
			"The compare engine should not be comparing all zeros, as this is the default output".warn;
			^false;
		};
		r = withResult - withoutResult;
		r = r.select({ |v| v.abs > threshold.dbamp });
		if (r.isEmpty) { ^true };

		withBuf.free;
		withoutBuf.free;

		r.sort{ |l, r| l > r }; // bigest first;
		r[0..10].debug("ERROR: lastest difference");
		^false;
	}


	test_compare_control {
		this.compare_optimization_levels(
			f: { \a.kr + \b.kr },
			server: server,
			threshold: -120,
			duration: 0.02,
			msg: "Test controls basic",
			extraArgs: [\a, 2, \b, 4]
		);
		this.compare_optimization_levels(
			f: {
				var s = SinOsc.ar;
				var ss = s * SinOsc.ar(354);
				\a.kr + \b.kr + A2K.kr(ss)
			},
			server: server,
			threshold: -120,
			duration: 0.01,
			msg: "Test controls with other ugens",
			extraArgs: [\a, 2, \b, 4]
		);
	}


	test_compare_arithmetic {
		this.compare_optimization_levels({ LFPar.ar - SinOsc.ar().neg }, server, threshold: -120,
			msg: "Negation: a - b.neg => a + b."
		);
		this.compare_optimization_levels({ LFPar.ar + SinOsc.ar().neg }, server, threshold: -120,
			msg: "Negation - a + b.neg => a - b."
		);
		this.compare_optimization_levels({
			var a = SinOsc.ar().neg;
			a + a
		}, server, threshold: -120,
		msg: "Negation - same this addition"
		);
		this.compare_optimization_levels({
			var a = SinOsc.ar().neg;
			a - a + 0.02
		}, server, threshold: -120,
		msg: "Negation - same this subtraction"
		);
		this.compare_optimization_levels({
			var a = SinOsc.ar();
			a.neg + a.neg
		}, server, threshold: -120,
		msg: "Negation - different this addition"
		);
		this.compare_optimization_levels({
			var a = SinOsc.ar();
			a.neg - a.neg + 0.02
		}, server, threshold: -120,
		msg: "Negation - different this subtraction"
		);
		this.compare_optimization_levels(
			{ |a = 2, b = 3, c = 4, d = 5| a + b + c + d },
			server,
			threshold: -120,
			msg: "Sum4 optimisation."
		);
		this.compare_optimization_levels(
			{ |a = 2, b = 3, c = 4| SinOsc.ar + a + b + c },
			server,
			threshold: -120,
			msg: "Sum4 optimisation with SinOsc."
		);
		this.compare_optimization_levels(
			{ |a = 2, b = 3| SinOsc.ar * a + b },
			server,
			threshold: -120,
			msg: "MulAdd optimisation."
		);
		// This one requires less of a threshold due to floating point.
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4, d = 10|
			SinOsc.ar * 2 + a * b + c * a + c + b + d * SinOsc.ar(22)
		}, server, threshold: -90,
		msg: "Larger BinaryOpUGen optimisation test."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4|
			var sig1 = SinOsc.ar; // dead code
			var sig2;
			sig1 = a + b;
			sig2 = sig1;
			sig1 = sig1 + a + b;
			sig1 * sig2 + sig1
		}, server, threshold: -120,
		msg: "Complex BinaryOpUgen."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4|
			SinOsc.ar(123, 0.2, 2.0, 1.0)
		}, server, threshold: -120,
		msg: "MulAdd in UGen replace * 2 with adds."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4|
			Sum3(a, b, c) - a
		}, server, threshold: -120,
		msg: "Removing values from Sum3 - 1."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4|
			(a + b + c) - a + (a + b + c) - b + (a + b + c) - c
		}, server, threshold: -120,
		msg: "Removing values from Sum3 - 2."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4, d = 5|
			(a + b + c + d) - a
		}, server, threshold: -120,
		msg: "Removing values from Sum4 - 1."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4, d = 5|
			Sum4(a, b, c, d) - a + Sum4(a, b, c, d) - b + Sum4(a, b, c, d) - c + Sum4(a, b, c, d) - d
		}, server, threshold: -120,
		msg: "Removing values from Sum4 - 2."
		);
		this.compare_optimization_levels({ |a = 2, b = 3, c = 4, d = 5|
			a - b.neg
		}, server, threshold: -120,
		msg: "Replaceing negation with add"
		);
	}

	test_demand {
		this.compare_optimization_levels({
			var d = Dseries(0, 1, inf);
			Demand.ar(Impulse.ar(10000), 0, d) + Demand.ar(Impulse.ar(10000), 0, d)
		}, server, threshold: -120,
		msg: "Demand 1"
		);
		this.compare_optimization_levels({
			var d = Dseries(0, 1, inf);
			var i = Impulse.ar(10000);
			Demand.ar(i, 0, d) + Demand.ar(i, 0, d)
		}, server, threshold: -120,
		msg: "Demand 2 "
		);
	}


	test_pv {
		var b = Buffer.read(server, Platform.resourceDir +/+ "sounds/a11wlk01.wav");
		server.sync;

		this.compare_optimization_levels(
			{
				var inA, chainA, inB, chainB, chain ;
				inA = PlayBuf.ar(1, b.bufnum, BufRateScale.kr(b.bufnum), loop: 0);
				inB =  PlayBuf.ar(1, b.bufnum, BufRateScale.kr(b.bufnum) * 0.5, loop: 0);
				chainA = FFT(LocalBuf(2048), inA);
				chainB = FFT(LocalBuf(2048), inB);
				chain = PV_Add(chainA, chainB);
				(0.1 * IFFT(chain).dup).sum
			}, server,
			threshold: -120,
			duration: 0.2,
			msg: "PV_Add help example"
		);

		this.compare_optimization_levels(
			{
				var fftsize = 1024;
				var in, chain, in2, chain2, out;
				in = PlayBuf.ar(1, b.bufnum, BufRateScale.kr(b), loop: 0);
				chain = FFT(LocalBuf(fftsize), in);

				// JMcC babbling brook
				in2 = ({
					RHPF.ar(OnePole.ar(Saw.ar(19999), 0.99), LPF.ar(Saw.ar(19993), 14)
						* 400 + 500, 0.03, 0.003) }!2)
				+ ({ RHPF.ar(OnePole.ar(Saw.ar(1991), 0.99), LPF.ar(Saw.ar(19991), 20)
					* 800 + 1000, 0.03, 0.005) }!2
				) * 4;
				chain2 = FFT(LocalBuf(fftsize), in2);

				chain = chain.pvcalc2(chain2, fftsize, { |mags, phases, mags2, phases2|
					[
						mags * mags2 / 10,
						phases2 + phases
					]
				}, frombin: 0, tobin: 125, zeroothers: 0);

				out = IFFT(chain);
				(0.5 * out.dup).sum
			}, server,
			threshold: -96,
			duration: 0.2,
			msg: "pvcalc2 help example"
		);
	}


	test_pv_opts {
		// Tests that the PV_ UGens insert PV_Copy in the correct places.
		var b = Buffer.read(server, Platform.resourceDir +/+ "sounds/a11wlk01.wav");
		server.sync;

		this.assert(
			TestSynthDefOptimise.compare_a_b(
				{
					var chain = FFT(LocalBuf(1024), Saw.ar(220));
					chain = PV_Add(PV_BinShift(chain, 2), PV_BinShift(chain, 1, 5));
					IFFT(chain)
				},
				{
					var chain = FFT(LocalBuf(1024), Saw.ar(220));
					var chain2 = PV_Copy(chain, LocalBuf(1024));
					chain = PV_Add(PV_BinShift(chain, 2), PV_BinShift(chain2, 1, 5));
					IFFT(chain)
				},
				server,
				threshold: -120,
				duration: 0.2
			),
			"Test PV auto PV_Copy simple"
		);

		this.assert(
			TestSynthDefOptimise.compare_a_b(
				{
					var chain = FFT(LocalBuf(512), Saw.ar(220));
					var chainB = PV_Add(PV_BinShift(chain, 2), PV_BinShift(chain, 1, 5));
					chainB = PV_Add(PV_BinShift(chainB, 2), PV_BinShift(chain, 1, -5));
					chainB = PV_Add(PV_BinShift(chainB, 0.5), PV_BinShift(chain, 1, 15));
					IFFT(chainB)
				},
				{
					var chainA1 = FFT(LocalBuf(512), Saw.ar(220));
					var chainA2 = PV_Copy(chainA1, LocalBuf(512));
					var chainA3 = PV_Copy(chainA1, LocalBuf(512));
					var chainA4 = PV_Copy(chainA1, LocalBuf(512));

					var chainB = PV_Add(PV_BinShift(chainA1, 2), PV_BinShift(chainA2, 1, 5));
					chainB = PV_Add(PV_BinShift(chainB, 2), PV_BinShift(chainA3, 1, -5));
					chainB = PV_Add(PV_BinShift(chainB, 0.5), PV_BinShift(chainA4, 1, 15));

					IFFT(chainB)
				},
				server,
				threshold: -120,
				duration: 0.2
			),
			"Test PV auto PV_Copy Harder"
		);
	}

	test_outputproxies {
		this.compare_optimization_levels({
			(\inputs.ar([2, 3, 4, 5, 6]) * 1).sum;
		}, server, threshold: -180,
		msg: "Replacing output proxies --- 1 "
		);
		this.compare_optimization_levels({
			var ins = \inputs.ar([2, 3, 4, 5, 6]);
			(ins * 2 + 0).sum
		}, server, threshold: -180,
		msg: "Replacing output proxies --- 2 "
		);
		this.compare_optimization_levels({
			var ins = \inputs.ar([2, 3, 4, 5, 6]);
			(ins * 0 + 1).sum
		}, server, threshold: -180,
		msg: "Replacing output proxies --- 3 "
		);
		this.compare_optimization_levels({
			Env.adsr(\atk.kr(0.0001), \dec.kr(0.001), \sus.kr(0.0014), \rel.kr(0.0005)).ar.asArray.flat.sum;
		}, server, threshold: -180,
		msg: "Replacing output proxies in Env."
		);
		this.compare_optimization_levels({
			var r = ReplaceOut.ar(0, SinOsc.ar(12000));
			var sig0 = In.ar(0);
			var sig1 = In.ar(0);
			Out.ar(0, sig0);
			Out.ar(0, sig1);
			In.ar(0);
		}, server, threshold: -120,
		msg: "Joining Inputs and preserving IO ordering."
		);
	}



	test_compare_real_world {
		var a, b;

		// https://github.com/thormagnusson/sctweets/tree/master
		this.compare_optimization_levels({
			Mix.fill(9,{
				var i = Impulse.ar(0.4)!2;
				CombC.ar(i, 1, Select.ar(Impulse.kr(50), (55 + Scale.aeolian.degrees).collect{ |x| DC.ar(1 / x.midicps) }), 3 )
			}).sum
		}, server, threshold: -96, forceDontPrint: true,
		msg: "thormagnusson - Aeolian Strings 1"
		);

		this.compare_optimization_levels({
			Mix.fill(20, {
				var i = Impulse.ar(5)!2;
				CombC.ar(i, 1, Select.ar(Impulse.kr(0, 5, i), (77 + [0, 3, 7, 10, 12]).collect{ |x| DC.ar(1 / x.midicps)}), 0.3 )
			}).sum
		}, server, threshold: -96, forceDontPrint: true,
		msg: "thormagnusson - Aeolian Strings 2"
		);

		this.compare_optimization_levels({
			({
				var a = SinOsc;
				var l = LFPar;
				a.ar(666 * a.ar(l.ar(l.ar(0.5)) * 9) * RLPF.ar(Saw.ar(9), l.ar(0.5).range(9,999), l.ar(2))).cubed
			}!2).sum
		}, server, threshold: -96, forceDontPrint: true,
		msg: "thormagnusson - Arguing Osc"
		);


		b = Buffer.read(server, Platform.resourceDir +/+ "sounds/a11wlk01.wav");
		server.sync;
		this.compare_optimization_levels({
			var t = Impulse.kr(5);
			var p = PlayBuf.ar(1, b, 1, t, Demand.kr(t, 0, Dseq(1e3*[103, 41, 162, 15, 141, 52, 124, 190], 4)))!2;
			p.sum
		}, server, threshold: -96, forceDontPrint: true,
		msg: "Nathaniel Virgo - sc-140"
		);

		this.compare_optimization_levels({
			var s = { |o, i|
				SinOsc.ar(
					[i, i + 0.0001] ** 2 * f.value(o, i - 1),
					f.value(o, i - 1) * 0.0001) * f.value(o, i - 1
				)
			};
			var f = { |o, i| if(i > 0, { s.value(o, i) }, o)};
			(f.value(60, 6) / 60).sum
		}, server, threshold: -96, forceDontPrint: true,
		msg: "A big graph - tw 0011 (f0)."
		);

		this.compare_optimization_levels(
			{
				var sig, chain;

				// The original sound source
				sig = [2076, 2457, 2337, 1949, 2477, 2229, 3859].collect {|f|
					SinOsc.ar(f, 0, 2 * Decay.ar(Impulse.ar(10000), 0.1)).tanh
				}.sum;

				chain = sig;    // Start with the original signal

				[
					[0.156, 0.0863], [0.1133, 0.1258], [0.0785, 0.1674],
					[0.0104, 0.1523], [0.1664, 0.1706], [0.1784, 0.1117], [0.1432, 0.1894]
				].do {|p|
					chain = LeakDC.ar(AllpassL.ar(LPF.ar(chain * 0.9, 3000), 0.2, p, 3));
				};

				Limiter.ar(sig+chain).flat.sum;    // dry + wet
			}, server,
			duration: 0.1,
			threshold: -96,
			forceDontPrint: true,
			msg: "Example from tour of UGens."
		);


		this.compare_optimization_levels({ |amp=0.2|
			var snd, noi;
			var coeff = {|freq| exp(-2pi * (freq * SampleDur.ir))};
			snd = SinOsc.ar([49, 98, 196]).sum * 2.0;
			snd = Clip.ar(snd, -0.9, 0.9);
			snd = OnePole.ar(snd, coeff.(50));
			snd = HPF.ar(snd, 20);
			noi = HPF.ar(OnePole.ar(Saw.ar, coeff.(100)), 500);
			noi = noi * EnvGen.ar(Env.linen(0.010, 0.005, 0.005));
			snd = snd * EnvGen.ar(Env.linen(0.010, 1.090, 0.200), doneAction: Done.freeSelf);
			snd = snd + noi;
			snd = snd * amp;
			snd.asArray.flat.sum
		}, server, threshold: -96, forceDontPrint: true,
		msg: "gosub — https://sccode.org/1-5i2"
		);

		this.compare_optimization_levels(
			{
				|pitch=440, freq=70, addFreq=17, attack=1, release = 12|
				var sig, sig1, saws, env, shapeEnv, local, local2;
				sig = Mix.new(
					Array.fill(8,
						{SinOsc.ar(freq + addFreq, 0.95, 0.03)})
				);

				env = EnvGen.kr(Env.perc(attack, release ), doneAction:2);
				sig1 = sig + (sig *
					Mix.new(
						Array.fill(8,
							{SinOsc.ar(0.02, 0.7, LFNoise1.kr(0.02, 0.08))}))
				);

				sig = sig * env;
				sig1 = sig1 * env;

				sig = PitchShift.ar(sig, 0.1, SinOsc.kr(pitch, 3.2, 0.9, 3));
				sig1 = PitchShift.ar(sig1, 0.1, SinOsc.kr(pitch, 0, 0.9, 3));

				saws = Mix.new(
					Array.fill(8,
						{LFSaw.ar(\sawFreq.ir(4000) + addFreq, 0.9, 0.02)})
				);
				shapeEnv = EnvGen.kr(Env([0.1, 0.02, 0.8, 0.0], [1, 5, 3 , 2]));

				saws = saws * shapeEnv;
				saws = saws * env;

				local = LocalIn.ar(2) + [sig+sig1, sig1+sig];
				local = DelayN.ar(local, 0.8, [0.3, 0.33]);
				local2 = LocalIn.ar(2) + [saws, saws];
				local2 = DelayN.ar(local2, 0.8, [0.02, 0.02]);
				local = local + local2;

				local = Compander.ar(
					local, local,
					0.2, slopeBelow: 1.3,
					slopeAbove: 0.1,
					clampTime:0.1,
					relaxTime:0.01);
				local = local.tanh;
				local = HPF.ar(local, 70);
				//local = BRF.ar(local, 260);
				LocalOut.ar(local * 0.8);
				local.asArray.flat.sum;
			},
			server,
			threshold: -96,
			duration: 0.1,
			forceDontPrint: true,
			msg: "iakamuri — https://sccode.org/1-5hW"
		);




		this.compare_optimization_levels(
			{
				var o = Mix.fill(20, {
					var i = Impulse.ar(5)!2;
					CombC.ar(i, 1, Select.ar(Impulse.kr(0, 5, i), (77 + [0, 3, 7, 10, 12]).collect{ |x| DC.ar(1 / x.midicps)}), 0.3 )
				}).sum;
				o
			}, server,
			threshold: -96,
			forceDontPrint: true,
			msg: "mix"
		);

		this.compare_optimization_levels(
			{

				arg decay=30, amp=2, gate=0, tone=56;
				var fenv, env, trienv, sig, sub, punch, pfenv;
				env = EnvGen.kr(Env.new([0.11, 1, 0], [0, decay], -225), doneAction:2);
				trienv = EnvGen.kr(Env.new([0.11, 0.6, 0], [0, decay], -230), doneAction:0);
				fenv = Env([tone*7, tone*1.35, tone], [0.05, 0.6], -14).kr;
				pfenv = Env([tone*7, tone*1.35, tone], [0.03, 0.6], -10).kr;
				sig = SinOsc.ar(fenv, pi/2) * env;
				sub = LFTri.ar(fenv, pi/2) * trienv * 0.05;
				punch = SinOsc.ar(pfenv, pi/2) * env * 2;
				punch = HPF.ar(punch, 350);
				sig = (sig + sub + punch) * 2.5;
				sig = Limiter.ar(sig, 0.5) * amp;
				sig = Pan2.ar(sig, 0);
				sig.flatten.sum + 0.001;
			},
			server,
			threshold: -120,
			forceDontPrint: true,
			msg: "Yoshinosuke Horiuchi's SC808"
		);

	}

	test_real_world_quarks {
		/*
		this.compare_optimization_levels(
			{
				Fb1({ |in, out| (in[0] * 0.05) + (out[1] * 0.95) }, SinOsc.ar, leakDC: false);
			},
			server,
			threshold: -70,
			duration: 0.1,
			forceDontPrint: true,
			msg: "Daniel Mayer --- Fb1 OnePole"
		);

		this.compare_optimization_levels(
			{
				var o = DXEnvFan.ar(
					Dseq((0..29), inf),
					size: 30,
					fadeTime: 0.005
				) * 0.25;
				o.asArray.flat.sum
			},
			server,
			threshold: -70,
			duration: 0.1,
			forceDontPrint: true,
			msg: "Daniel Mayer --- DXEnvFan 1"
		);


		Fb1_ODEdef(
			\izhikevich,
			{ |t, y, a=0.02, b=0.2, i=0|
			    [
				    { y[0] * (0.04 * y[0] + 5.0) + 140 - y[1] + i },
				    { a * (b * y[0] - y[1]) }
			    ]
		    },
			0.002,
			[-50.0, 1.0],
			1,
			1
		);


		// Single IZ unit
		this.compare_optimization_levels(
			{
				var env = EnvGate.new;
				var inleft = \inleft.ar(0.0, 0.0);
				var inright = \inright.ar(0.0, 0.0);
				var a = \a.kr(0.02, 0.05, spec: [0.0, 1.0, \lin, 0, 0.02]);
				var b = \b.kr(0.2, 0.05, spec: [0.0, 1.0, \lin, 0, 0.2]);
				var c = \c.kr(-65.0, 0.05, spec: [-70, -30.0, \lin, 0, -65]);
				var d = \d.kr(8.0, 0.05, spec: [0.0, 10.0, \lin, 0, 8.0]);
				var alpha = \alpha.kr(0.0, 0.05, spec: [-1.0, 1.0, \lin, 0, 0.0]);
				var beta = \beta.kr(0.0, 0.05, spec: [-1.0, 1.0, \lin, 0, 0.0]);
				var i0 = \i0.kr(3.0, 0.05, spec: [0.0, 10.0, \lin, 0, 3.0]);
				var tm = \tm.kr(1000, 0.1, spec: [1, 4000, \lin, 0, 1000]);
				var eq = Fb1_ODE.ar(\izhikevich,
					[a, b, (inleft * alpha) + (inright * beta) + i0],
					tm,
					0.002,
					[-50.0, 2.0],
					compose: \softclip
				);
				eq.flat.sum;
			},
			server,
			threshold: -96,
		    msg: "FB1 telephon"
		);
		*/
	}

	test_io {
		this.compare_optimization_levels({
			var n = 5;
			var sig = SinOsc.ar(19000!n);
			var in;

			// Cannot be removed
			ReplaceOut.ar(0, DC.ar(0)!n);

			// Cannot be removed
			Out.ar(0, sig!n);
			Out.ar(0, sig!n);
			Out.ar(0, sig!n);

			sig = SinOsc.ar(500!n);
			sig = (sig * 2 + SinOsc.ar) + sig;

			// Could be removed
			in = In.ar(0, n);
			in = In.ar(0, n);
			in = In.ar(0, n);
			in = In.ar(0, n);

			Out.ar(0, sig * sig);

			// Could be removed
			ReplaceOut.ar(0, in * sig);
			ReplaceOut.ar(0, in * sig);
			ReplaceOut.ar(0, in * sig);
			ReplaceOut.ar(0, in * sig);
			In.ar(0, n).sum;

		}, server, threshold: -90,
		msg: "Interleaved ins and outs - n = 5"
		);

		this.compare_optimization_levels({
			var n = 10;
			var sig = SinOsc.ar(19000!n);
			var in;

			// Cannot be removed
			ReplaceOut.ar(0, DC.ar(0)!n);

			// Cannot be removed
			Out.ar(0, sig!n);
			Out.ar(0, sig!n);
			Out.ar(0, sig!n);

			sig = SinOsc.ar(500!n);
			sig = (sig * 2 + SinOsc.ar) + sig;

			// Could be removed
			in = In.ar(0, n);
			in = In.ar(0, n);
			in = In.ar(0, n);
			in = In.ar(0, n);

			Out.ar(0, sig * sig);

			// Could be removed
			ReplaceOut.ar(0, in * sig);
			ReplaceOut.ar(0, in * sig);
			ReplaceOut.ar(0, in * sig);
			ReplaceOut.ar(0, in * sig);
			In.ar(0, n).sum;

		}, server, threshold: -70,
		msg: "Interleaved ins and outs - n = 10"
		);


	}
}
