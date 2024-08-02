// General testing strategy here is to test that the result of the optimisation is the same,
//    rather than *that* the optimisation has been applied.
// This is due to the complexities regarding:
//    * when optimisation are applied,
//    * and how they interact with each other.
// What is important is that the optimisation results in the same sound/output as the unoptimised version.

TestSynthDefOptimise : UnitTest {
	classvar printUGenGraphs = true;
	var server;

	*compare { |f, server, forceDontPrint=false|
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

			RecordBuf.perform(RecordBuf.methodSelectorForRate(defRate), outputs, bufnum, loop: 0);
			Line.perform(Line.methodSelectorForRate(defRate), dur: 0.01, doneAction: 2);
		};

		var withDef = SynthDef(\with, func).add;
		var withoutDef = SynthDef.newWithoutOptimisations(\without, func).add;

		var withBuf = Buffer.alloc(server, 0.01 * server.sampleRate);
		var withoutBuf = Buffer.alloc(server, 0.01 * server.sampleRate);

		var s = server.sync;

		var withSynth, withoutSynth;

		var bind = server.bind {
			withSynth = Synth(\with, [\bufnum, withBuf]);
			withoutSynth = Synth(\with, [\bufnum, withoutBuf]);
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
			withBuf.getToFloatArray(action: { |ar|
				withResult = ar;
				counter = counter - 1;
				cond.signalOne;
			});
		}, '/n_end', server.addr, nil, [withSynth.nodeID]).oneShot;

		OSCFunc({
			withoutBuf.getToFloatArray(action: { |ar|
				withoutResult = ar;
				counter = counter - 1;
				cond.signalOne;
			});
		}, '/n_end', server.addr, nil, [withoutSynth.nodeID]).oneShot;

		cond.wait { counter == 0 };

		^(withResult.debug("with") - withoutResult.debug("without")).abs.sum.debug("ERROR") < -240.dbamp
	}

	setUp {
		server = Server(this.class.name);
		server.options.memSize = 1024;
		server.options.blockSize = 64;
		this.bootServer(server);
		server.sync;
	}

	test_compare_arithmetic {
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5| a + b + c + d }, server),
			"Sum4 optimisation."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4| SinOsc.ar + a + b + c }, server),
			"Sum4 optimisation with SinOsc."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3| SinOsc.ar * a + b }, server),
			"MulAdd optimisation."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 10|
				SinOsc.ar * 2 + a * b + c * a + c + b + d * SinOsc.ar(22)
			}, server),
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
			}, server),
			"Complex BinaryOpUgen."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
				SinOsc.ar(123, 0.2, 2.0, 1.0)
			}, server),
			"MulAdd in UGen replace * 2 with adds."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
				Sum3(a, b, c) - a
			}, server),
			"Removing values from Sum3 - 1."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4|
				Sum3(a, b, c) - a + Sum3(a, b, c) - b + Sum3(a, b, c) - c
			}, server),
			"Removing values from Sum3 - 2."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				Sum4(a, b, c, d) - a
			}, server),
			"Removing values from Sum4 - 1."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				Sum4(a, b, c, d) - a + Sum4(a, b, c, d) - b + Sum4(a, b, c, d) - c + Sum4(a, b, c, d) - d
			}, server),
			"Removing values from Sum4 - 2."
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				a - b.neg
			}, server),
			"Replaceing negation with add"
		);
		this.assert(
			TestSynthDefOptimise.compare({ |a = 2, b = 3, c = 4, d = 5|
				// tw 0011 (f0)
				var s = { |o, i| SinOsc.ar([i, i + 0.0001] ** 2 * f.value(o, i - 1), f.value(o, i - 1) * 0.0001) * f.value(o, i - 1)};
				var f = { |o, i| if(i > 0, { s.value(o, i) }, o)};
				(f.value(60, 6) / 60).sum
			}, server, forceDontPrint: true),
			"A big graph - tw 0011 (f0)."
		);
	}
}

































