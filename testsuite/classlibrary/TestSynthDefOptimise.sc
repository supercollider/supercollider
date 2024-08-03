// General testing strategy here is to test that the result of the optimisation is the same,
//    rather than *that* the optimisation has been applied.
// This is due to the complexities regarding:
//    * when optimisation are applied,
//    * and how they interact with each other.
// What is important is that the optimisation results in the same sound/output as the unoptimised version.

TestSynthDefOptimise : UnitTest {
	classvar printUGenGraphs = true;
	var server;

	*compare { |f, server, threshold, forceDontPrint=false|
		var func = { |bufnum|
			var outputs = SynthDef.wrap(f);
			var numOutputs, defRate;
			if(outputs.isValidUGenInput.not) {
				outputs.dump;
				Error("Reading signal failed: % is not a valid UGen input.".format(outputs)).throw
			};
			outputs = UGen.replaceZeroesWithSilence(outputs.asArray);
			numOutputs = outputs.size.max(1);
			defRate = outputs.rate;
			if(defRate == \audio) {
				outputs = outputs.collect { |x| if(x.rate != \audio) { K2A.ar(x) } { x } }
			};

			RecordBuf.perform(RecordBuf.methodSelectorForRate(defRate), outputs, bufnum, loop: 0, doneAction: 2);
		};

		var withDef = SynthDef(\with, func).add;
		var withoutDef = SynthDef.newWithoutOptimisations(\without, func).add;

		var withBuf = Buffer.alloc(server, 0.01 * server.sampleRate);
		var withoutBuf = Buffer.alloc(server, 0.01 * server.sampleRate);

		var s = server.sync;

		var withSynth, withoutSynth;

		var bind = server.bind {
			withSynth = Synth(\with, [\bufnum, withBuf]);
			withoutSynth = Synth(\without, [\bufnum, withoutBuf]);
		};

		var counter = 2;
		var cond = CondVar();

		var withResult, withoutResult;
		if (printUGenGraphs and: { forceDontPrint.not }){
			"\nWith Optimisations' UGen Graph.".postln;
			withDef.dumpUGens;
			"\nWithout Optimisations' UGen Graph.".postln;
			withoutDef.dumpUGens;
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

		^(withResult - withoutResult).collect({|r|
			var er = r.abs > threshold.dbamp;
			if (er) { r.abs.debug("ERROR") };
			er
		}).any{|v| v }.not
	}

	*compareNOutputs { |n, f, server, threshold, forceDontPrint=false|
		var recordingFunc = { |bufnum, offest|
			var outputs = In.ar(offest, n);
			RecordBuf.ar(outputs, bufnum, loop: 0, doneAction: 2);
		};

		var cleanSlateDef = SynthDef(\cleanSlate, { ReplaceOut.ar(0, DC.ar(0).dup(n * 2)) }).add;
		var withDef = SynthDef(\with, f).add;
		var withoutDef = SynthDef.newWithoutOptimisations(\without, f).add;
		var recorder = SynthDef(\recorder, recordingFunc).add;

		var withBuf = Buffer.alloc(server, 0.01 * server.sampleRate, numChannels: n);
		var withoutBuf = Buffer.alloc(server, 0.01 * server.sampleRate, numChannels: n);

		var s = server.sync;

		var withSynth, withRecord, withoutSynth, withoutRecord, cleanSlate;

		var bind = server.bind {
			cleanSlate = Synth(\cleanSlate);
			withSynth = Synth.after(cleanSlate, \with, [\offset, 0]);
			withoutSynth = Synth.after(withSynth, \without, [\offset, n]);
			withRecord = Synth.after(withoutSynth, \recorder, [\bufnum, withBuf, \offset, 0]);
			withoutRecord = Synth.after(withRecord, \recorder, [\bufnum, withoutBuf, \offest, n]);
		};

		var counter = 2;
		var cond = CondVar();

		var withResult, withoutResult;

		if (printUGenGraphs and: { forceDontPrint.not }){
			"\nWith Optimisations' UGen Graph.".postln;
			withDef.dumpUGens;
			"\nWithout Optimisations' UGen Graph.".postln;
			withoutDef.dumpUGens;
		};

		OSCFunc({
			withBuf.loadToFloatArray(action: { |ar|
				withResult = ar;
				counter = counter - 1;
				cond.signalOne;
			});
		}, '/n_end', server.addr, nil, [withRecord.nodeID]).oneShot;

		OSCFunc({
			withoutBuf.loadToFloatArray(action: { |ar|
				withoutResult = ar;
				counter = counter - 1;
				cond.signalOne;
			});
		}, '/n_end', server.addr, nil, [withoutRecord.nodeID]).oneShot;

		cond.wait { counter == 0 };
		server.freeAll;

		^(withResult - withoutResult).collect({|r|
			var er = r.abs > threshold.dbamp;
			//if (er) { r.abs.debug("ERROR") };
			er
		}).any{|v| v }.not
	}

	setUp {
		server = Server(this.class.name);
		server.options.memSize = 1024;
		server.options.blockSize = 64;
		this.bootServer(server);
		server.sync;
	}
	tearDown {
		Buffer.freeAll;
		server.quit.remove;
	}

	test_compare_arithmetic {
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5| a + b + c + d }, server, threshold: -120),
			"Sum4 optimisation."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4| SinOsc.ar + a + b + c }, server, threshold: -120),
			"Sum4 optimisation with SinOsc."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3| SinOsc.ar * a + b }, server, threshold: -120),
			"MulAdd optimisation."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 10|
				SinOsc.ar * 2 + a * b + c * a + c + b + d * SinOsc.ar(22)
			}, server, threshold: -120),
			"Larger BinaryOpUGen optimisation test."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
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
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
				SinOsc.ar(123, 0.2, 2.0, 1.0)
			}, server, threshold: -120),
			"MulAdd in UGen replace * 2 with adds."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
				Sum3(a, b, c) - a
			}, server, threshold: -120),
			"Removing values from Sum3 - 1."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
				Sum3(a, b, c) - a + Sum3(a, b, c) - b + Sum3(a, b, c) - c
			}, server, threshold: -120),
			"Removing values from Sum3 - 2."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				Sum4(a, b, c, d) - a
			}, server, threshold: -120),
			"Removing values from Sum4 - 1."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				Sum4(a, b, c, d) - a + Sum4(a, b, c, d) - b + Sum4(a, b, c, d) - c + Sum4(a, b, c, d) - d
			}, server, threshold: -120),
			"Removing values from Sum4 - 2."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				a - b.neg
			}, server, threshold: -120),
			"Replaceing negation with add"
		);
		this.assert(
			TestSynthDefOptimise.compare({
				// tw 0011 (f0)
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

	}

	test_io {
		this.assert(
			TestSynthDefOptimise.compare({
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
			TestSynthDefOptimise.compare({
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

	test_compare_outputs {
		this.assert(
			TestSynthDefOptimise.compareNOutputs(10,
				{ |offset|
					var sig = SinOsc.ar;
					Out.ar(offset, sig!10);
				},
				server, threshold: -90
			),
			"Basic outputs."
		);
		this.assert(
			TestSynthDefOptimise.compareNOutputs(10,
				{ |offset|
					var sig = SinOsc.ar;
					var in;
					// Should remain
					Out.ar(offset, sig!10);
					Out.ar(offset, sig!10);
					Out.ar(offset, sig!10);

					sig = LFPar.ar(500!10);

					sig = (sig * 2 + SinOsc.ar) / sig;

					// Could be removed
					in = In.ar(offset, 10);
					in = In.ar(offset, 10);
					in = In.ar(offset, 10);
					in = In.ar(offset, 10);

					Out.ar(offset, sig * sig);

					// Could be removed
					ReplaceOut.ar(offset, in * sig);
					ReplaceOut.ar(offset, in * sig);
					ReplaceOut.ar(offset, in * sig);
					ReplaceOut.ar(offset, in * sig);
				},
				server, threshold: -90
			),
			"Interleaved ins and outs."
		);
	}
}

































