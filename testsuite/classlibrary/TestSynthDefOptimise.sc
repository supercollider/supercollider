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
		server.options.memSize = 8192 * 4;
		server.options.blockSize = 64;
		this.bootServer(server);
		server.sync;
	}

	tearDown {
		Buffer.freeAll;
		server.sync;
		server.quit.remove;
	}

	*compare_create_synth_def { |method, name, f|
		^SynthDef.perform(method, name, { |bufnum|
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
		}).dumpUGens
	}

	*compare_a_b { |withopts, withoutopts, server, threshold, forceDontPrint=false, duration=0.01|
		var a = TestSynthDefOptimise.compare_create_synth_def(\new, \a, withopts).add;
		var b = TestSynthDefOptimise.compare_create_synth_def(\new, \b, withoutopts).add;

		^TestSynthDefOptimise.compare_engine(server, threshold, a, b, forceDontPrint, duration)
	}

	*compare_new_old { |f, server, threshold, forceDontPrint=false, duration=0.01|
		var withDef = TestSynthDefOptimise.compare_create_synth_def(\new, \with, f).add;
		var withoutDef = TestSynthDefOptimise.compare_create_synth_def(\newWithoutOptimisations, \without, f).add;

		^TestSynthDefOptimise.compare_engine(server, threshold, withDef, withoutDef, forceDontPrint, duration)
	}

	*compare_engine { |server, threshold, defa, defb, forceDontPrint=false, duration=0.01|
		var withBuf = Buffer.alloc(server, duration * server.sampleRate);
		var withoutBuf = Buffer.alloc(server, duration * server.sampleRate);

		var s = server.sync;

		var withSynth, withoutSynth;

		var bind = server.bind {
			withSynth = Synth(defa.name, [\bufnum, withBuf]);
			withoutSynth = Synth(defb.name, [\bufnum, withoutBuf]);
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

		r = withResult - withoutResult;
		r = r.select(_ > threshold.dbamp);
		r.sort{ |l, r| l > r }; // bigest first;

		if (r.isEmpty) { ^true };

		r[0..10].debug("ERROR: lastest difference");
		^false;
	}

	test_pv_opts {
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
				duration: 0.1
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
				duration: 0.1
			),
			"Test PV auto PV_Copy Harder"
		);
	}

	test_compare_arithmetic {
		this.assert(
			TestSynthDefOptimise.compare_new_old({ LFPar.ar - SinOsc.ar().neg }, server, threshold: -120),
			"Negation: a - b.neg => a + b."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ LFPar.ar + SinOsc.ar().neg }, server, threshold: -120),
			"Negation - a + b.neg => a - b."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var a = SinOsc.ar().neg;
				a + a
			}, server, threshold: -120),
			"Negation - same this addition"
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var a = SinOsc.ar().neg;
				a - a
			}, server, threshold: -120),
			"Negation - same this subtraction"
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var a = SinOsc.ar();
				a.neg + a.neg
			}, server, threshold: -120),
			"Negation - different this addition"
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var a = SinOsc.ar();
				a.neg - a.neg
			}, server, threshold: -120),
			"Negation - different this subtraction"
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old(
				{ |a = 2, b = 3, c = 4, d = 5| a + b + c + d },
				server,
				threshold: -120
			),
			"Sum4 optimisation."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old(
				{ |a = 2, b = 3, c = 4| SinOsc.ar + a + b + c },
				server,
				threshold: -120
			),
			"Sum4 optimisation with SinOsc."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old(
				{ |a = 2, b = 3| SinOsc.ar * a + b },
				server,
				threshold: -120
			),
			"MulAdd optimisation."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4, d = 10|
				SinOsc.ar * 2 + a * b + c * a + c + b + d * SinOsc.ar(22)
			}, server, threshold: -120),
			"Larger BinaryOpUGen optimisation test."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4|
				var sig1 = SinOsc.ar; // dead code
				var sig2;
				sig1 = a + b;
				sig2 = sig1;
				sig1 = sig1 + a + b;
				sig1 * sig2 + sig1
			}, server, threshold: -120),
			"Complex BinaryOpUgen."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4|
				SinOsc.ar(123, 0.2, 2.0, 1.0)
			}, server, threshold: -120),
			"MulAdd in UGen replace * 2 with adds."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4|
				Sum3(a, b, c) - a
			}, server, threshold: -120),
			"Removing values from Sum3 - 1."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4|
				Sum3(a, b, c) - a + Sum3(a, b, c) - b + Sum3(a, b, c) - c
			}, server, threshold: -120),
			"Removing values from Sum3 - 2."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4, d = 5|
				Sum4(a, b, c, d) - a
			}, server, threshold: -120),
			"Removing values from Sum4 - 1."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4, d = 5|
				Sum4(a, b, c, d) - a + Sum4(a, b, c, d) - b + Sum4(a, b, c, d) - c + Sum4(a, b, c, d) - d
			}, server, threshold: -120),
			"Removing values from Sum4 - 2."
		);
		this.assert(
			TestSynthDefOptimise.compare_new_old({ |a = 2, b = 3, c = 4, d = 5|
				a - b.neg
			}, server, threshold: -120),
			"Replaceing negation with add"
		);
	}

	test_pv {
		var b = Buffer.read(server, Platform.resourceDir +/+ "sounds/a11wlk01.wav");
		server.sync;

		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var inA, chainA, inB, chainB, chain ;
				inA = PlayBuf.ar(1, b.bufnum, BufRateScale.kr(b.bufnum), loop: 0);
				inB =  PlayBuf.ar(1, b.bufnum, BufRateScale.kr(b.bufnum) * 0.5, loop: 0);
				chainA = FFT(LocalBuf(2048), inA);
				chainB = FFT(LocalBuf(2048), inB);
				chain = PV_Add(chainA, chainB);
				(0.1 * IFFT(chain).dup).sum
			}, server, threshold: -120),
			"PV_Add help example"
		);

		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var fftsize = 1024;
				var in, chain, in2, chain2, out;
				in = PlayBuf.ar(1, b, BufRateScale.kr(b), loop: 0);
				chain = FFT(LocalBuf(fftsize), in);

				// JMcC babbling brook
				in2 = ({
					RHPF.ar(OnePole.ar(BrownNoise.ar, 0.99), LPF.ar(BrownNoise.ar, 14)
						* 400 + 500, 0.03, 0.003) }!2)
				+ ({ RHPF.ar(OnePole.ar(BrownNoise.ar, 0.99), LPF.ar(BrownNoise.ar, 20)
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
			}, server, threshold: -96),
			"pvcalc2 help example"
		);
	}




	test_compare_real_world {
		var b;

		// https://github.com/thormagnusson/sctweets/tree/master
		this.assert(
			TestSynthDefOptimise.compare_new_old({
				Mix.fill(9,{
					var i = Impulse.ar(0.4)!2;
					CombC.ar(i, 1, Select.ar(Impulse.kr(50), (55 + Scale.aeolian.degrees).collect{ |x| DC.ar(1 / x.midicps) }), 3 )
				}).sum
			}, server, threshold: -96, forceDontPrint: true),
			"thormagnusson - Aeolian Strings 1"
		);

		this.assert(
			TestSynthDefOptimise.compare_new_old({
				Mix.fill(20, {
					var i = Impulse.ar(5)!2;
					CombC.ar(i, 1, Select.ar(Impulse.kr(0, 5, i), (77 + [0, 3, 7, 10, 12]).collect{ |x| DC.ar(1 / x.midicps)}), 0.3 )
				}).sum
			}, server, threshold: -96, forceDontPrint: true),
			"thormagnusson - Aeolian Strings 2"
		);

		this.assert(
			TestSynthDefOptimise.compare_new_old({
				({
					var a = SinOsc;
					var l = LFPar;
					a.ar(666 * a.ar(l.ar(l.ar(0.5)) * 9) * RLPF.ar(Saw.ar(9), l.ar(0.5).range(9,999), l.ar(2))).cubed
				}!2).sum
			}, server, threshold: -96, forceDontPrint: true),
			"thormagnusson - Arguing Osc"
		);


		b = Buffer.read(server, Platform.resourceDir +/+ "sounds/a11wlk01.wav");
		server.sync;
		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var t = Impulse.kr(5);
				var p = PlayBuf.ar(1, b, 1, t, Demand.kr(t, 0, Dseq(1e3*[103, 41, 162, 15, 141, 52, 124, 190], 4)))!2;
				p.sum
			}, server, threshold: -96, forceDontPrint: true),
			"Nathaniel Virgo - sc-140"
		);

		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var s = { |o, i|
					SinOsc.ar(
						[i, i + 0.0001] ** 2 * f.value(o, i - 1),
						f.value(o, i - 1) * 0.0001) * f.value(o, i - 1
					)
				};
				var f = { |o, i| if(i > 0, { s.value(o, i) }, o)};
				(f.value(60, 6) / 60).sum
			}, server, threshold: -96, forceDontPrint: true),
			"A big graph - tw 0011 (f0)."
		);

		this.assert(
			TestSynthDefOptimise.compare_new_old({
				var sig, chain;

				// The original sound source
				sig = [2076, 2457, 2337, 1949, 2477, 2229, 3859].collect {|f|
					SinOsc.ar(f, 0, 2 * Decay.ar(Impulse.ar(1), 0.1)).tanh
				}.sum;

				chain = sig;    // Start with the original signal

				[
					[0.156, 0.0863], [0.1133, 0.1258], [0.0785, 0.1674],
					[0.0104, 0.1523], [0.1664, 0.1706], [0.1784, 0.1117], [0.1432, 0.1894]
				].do {|p|
					chain = LeakDC.ar(AllpassL.ar(LPF.ar(chain * 0.9, 3000), 0.2, p, 3));
				};

				Limiter.ar(sig+chain).flat.sum;    // dry + wet
			}, server, threshold: -96, forceDontPrint: true),
			"Example from tour of UGens."
		);
	}

	test_io {
		this.assert(
			TestSynthDefOptimise.compare_new_old({
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

			}, server, threshold: -90),
			"Interleaved ins and outs - n = 5"
		);

		this.assert(
			TestSynthDefOptimise.compare_new_old({
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

			}, server, threshold: -70),
			"Interleaved ins and outs - n = 10"
		);
	}


}

































