TestResample : TestReblockBase {
	test_resampleWithConstantFactorWorks {
		var bus = Bus.control(server);
		var factors = [ 0, 1, 2, 4, 8 ];
		var results = Array.fill(factors.size, nil);
		var success, sr = server.options.sampleRate;
		sr.postln;

		factors.do { |factor, index|
			var cond = Condition(), synth;
			SynthDef(\testResampleConstant, { |out|
				Resample(factor);
				Out.kr(out, SampleRate.ir);
			}).add;
			server.sync;
			synth = Synth(\testResampleConstant, [ out: bus ]);
			server.sync; // let it compute at least one block
			bus.get { |f|
				results[index] = f;
				cond.test_(true).signal;
			};
			cond.wait;
			synth.free;
		};
		this.assert(results[0] == sr, "Resample with argument 0.0 matches Server sample rate");
		success = [ factors[1..], results[1..] ].flop.every { |pair|
			(pair[0] * sr) == pair[1]
		};
		this.assert(success, "Resample with constant argument matches SampleRate output");
	}

	test_resampleWithBadInputThrows {
		this.assertException({
			SynthDef(\test, { Resample(-1) });
		}, Error, "Resample with negative factor throws exception");

		this.assertException({
			SynthDef(\test, { Resample(3) });
		}, Error, "Resample with non-power-of-two factor throws exception");

		this.assertException({
			SynthDef(\test, { |resample| Resample(resample * 2) })
		}, Error, "Resample with Outproxy that is not an immediate control throws exception");

		this.assertException({
			SynthDef(\test, { Resample(DC.ar(2)) })
		}, Error, "Resample with UGen other than OutputProxy throws exception");
	}

	test_resampleWithControlFactorWorks {
		var bus = Bus.control(server);
		var factors = [ 0, 1, 2, 4, 8 ];
		var results = Array.fill(factors.size, nil);
		var success, sr = server.options.sampleRate;

		SynthDef(\testResampleControl, { |out, resample|
			Resample(resample);
			Out.kr(out, SampleRate.ir);
		}).add;
		server.sync;

		factors.do { |factor, index|
			var cond = Condition();
			var synth = Synth(\testResampleControl, [ out: bus, resample: factor ]);
			server.sync; // let it compute at least one block
			bus.get { |f|
				results[index] = f;
				cond.test_(true).signal;
			};
			cond.wait;
			synth.free;
		};
		this.assert(results[0] == sr, "Resample with control default argument matches Server sample rate");
		success = [ factors[1..], results[1..] ].flop.every { |pair| (pair[0] * sr) == pair[1] };
		this.assert(success, "Resample with control argument matches SampleRate output");
	}

	test_resampleWithBadControlFactorUsesDefault {
		var bus = Bus.control(server);
		var factors = [ -1, 0.9, 3, 7 ];
		var results = Array.fill(factors.size, nil);
		var success, sr = server.options.sampleRate;
		SynthDef(\testResampleControlFail, { |out, resample|
			Resample(resample);
			Out.kr(out, SampleRate.ir);
		}).add;
		server.sync;
		factors.do { |factor, index|
			var cond = Condition();
			var synth = Synth(\testResampleControlFail, [ out: bus, resample: factor ]);
			server.sync; // let it compute at least one block
			bus.get { |f|
				results[index] = f;
				cond.test_(true).signal;
			};
			cond.wait;
			synth.free;
		};
		success = results.every { |x| x == sr };
		this.assert(success, "Resample with bad control argument falls back to Server sample rate");
	}

	test_resampleIgnoresDuplicates {
		var bus = Bus.control(server);
		var factors = [ 1, 2, 4, 8 ];
		var cond = Condition(), value, synth;
		var sr = server.options.sampleRate;
		synth = {
			factors.do { |x| Resample(x) };
			Out.kr(bus, SampleRate.ir);
		}.play(server);
		server.sync; // let it compute at least one block
		bus.get { |f|
			value = f;
			cond.test_(true).signal;
		};
		cond.wait;
		synth.free;
		this.assert(value == (factors[0] * sr), "Duplicate Resample instances are ignored with a warning");
	}

	test_IOUGensWithResampling {
		var arTestSig, krTestSig, simpleAB;
		var trigBusA = Bus.control(server, 2);
		var trigBusB = Bus.control(server, 2);
		var arTestBus = Bus.audio(server, 2);
		var krTestBus = Bus.control(server, 2);
		var testFactor = 1;

		simpleAB = { |defName|
			{ |busA, busB|
				[
					Synth(defName, [ out: busA, resample: testFactor ], server, \addToTail),
					Synth(defName, [ out: busB ], server, \addToTail)
				]
			}
		};

		arTestSig = { Out.ar(arTestBus, SinOsc.ar([220, 440])) }.play(server);

		krTestSig = { Out.kr(krTestBus, SinOsc.kr([220, 440])) }.play(server);

		server.sync;

		//------------------ UGens ----------------------//

		// Out.ar/Out.kr

		SynthDef(\testOutAr, { |out, resample|
			var sig = [ Line.ar, SinOsc.ar ];
			Resample(resample);
			// make sure that summing works
			Out.ar(out, sig);
			Out.ar(out, sig);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testOutAr), \ar, "Out.ar works with resampling");

		SynthDef(\testOutKr, { |out, resample|
			var sig = [ Line.kr, SinOsc.kr ];
			Resample(resample);
			// make sure that summing works
			Out.kr(out, sig);
			Out.kr(out, sig);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testOutKr), \kr, "Out.kr works with resampling");

		// ReplaceOut.ar / ReplaceOut.kr

		SynthDef(\testReplaceOutAr, { |out, resample|
			Resample(resample);
			ReplaceOut.ar(out, [ Line.ar, SinOsc.ar ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testReplaceOutAr), \ar, "ReplaceOut.ar works with resampling");

		SynthDef(\testReplaceOutKr, { |out, resample|
			Resample(resample);
			ReplaceOut.kr(out, [ Line.kr, SinOsc.kr ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testReplaceOutKr), \kr, "ReplaceOut.kr works with resampling");

		// OffsetOut.ar

		SynthDef(\testOffsetOutAr, { |out, resample|
			var sig = [ Line.ar, SinOsc.ar ];
			Resample(resample);
			// make sure that summing works
			OffsetOut.ar(out, sig);
			OffsetOut.ar(out, sig);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testOffsetOutAr), \ar, "OffsetOut.ar works with resampling");

		// XOut.ar / XOut.kr

		SynthDef(\testXOutAr, { |out, resample|
			Resample(resample);
			XOut.ar(out, 0.5, [ Line.ar, SinOsc.ar ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testXOutAr), \ar, "XOut.ar works with resampling");

		SynthDef(\testXOutKr, { |out, resample|
			Resample(resample);
			XOut.kr(out, 0.5, [ Line.kr, SinOsc.kr ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testXOutKr), \kr, "XOut.kr works with resampling");

		// In.ar / In.kr

		SynthDef(\testInAr, { |out, resample|
			Resample(resample);
			ReplaceOut.ar(out, In.ar(arTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testInAr), \ar, "In.ar works with resampling");

		SynthDef(\testInKr, { |out, resample|
			Resample(resample);
			ReplaceOut.kr(out, In.kr(krTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testInKr), \kr, "In.kr works with resampling");

		// FeedbackIn.ar

		SynthDef(\testInFeedbackAr, { |out, resample|
			Resample(resample);
			ReplaceOut.ar(out, InFeedback.ar(arTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testInFeedbackAr), \ar, "InFeedback.ar works with resampling");

		// LagIn.kr

		SynthDef(\testLagInKr, { |out, resample|
			Resample(resample);
			ReplaceOut.kr(out, LagIn.kr(krTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLagInKr), \kr, "LagIn.kr works with resampling");

		// InTrig.kr

		SynthDef(\testInTrigKr, { |in, out, resample|
			Resample(resample);
			ReplaceOut.kr(out, InTrig.kr(in, 2));
		}).add;

		server.sync;

		this.performABTest({ |busA, busB|
			trigBusA.set(1, 1);
			trigBusB.set(1, 1);
			[
				Synth(\testInTrigKr, [ in: trigBusA, out: busA, resample: testFactor ], server, \addToTail),
				Synth(\testInTrigKr, [ in: trigBusB, out: busB ], server, \addToTail)
			]
		}, \kr, "InTrig.kr works with resampling");

		// Control

		SynthDef(\testControl, { |out, resample|
			Resample(resample);
			ReplaceOut.kr(out, \ctl.kr([ 0.5, 1.5 ]));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testControl), \kr, "Control with default value works with resampling");

		this.performABTest({ |busA, busB|
			var a = Synth(\testControl, [ out: busA, resample: testFactor ], server, \addToTail).map(\ctl, krTestBus);
			var b = Synth(\testControl, [ out: busB ], server, \addToTail).map(\ctl, krTestBus);
			[ a, b ]
		}, \kr, "Control mapped to control bus works with resampling");

		// AudioControl

		SynthDef(\testAudioControl, { |out, resample|
			Resample(resample);
			ReplaceOut.ar(out, \ctl.ar([ 0.5, 1.5 ]));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testAudioControl), \ar, "AudioControl with default value works with resampling");

		this.performABTest({ |busA, busB|
			var a = Synth(\testAudioControl, [ out: busA, resample: testFactor ], server, \addToTail).map(\ctl, arTestBus);
			var b = Synth(\testAudioControl, [ out: busB ], server, \addToTail).map(\ctl, arTestBus);
			[ a, b ]
		}, \ar, "AudioControl mapped to audio bus works with resampling");

		this.performABTest({ |busA, busB|
			var a = Synth(\testAudioControl, [ out: busA, resample: testFactor ], server, \addToTail).map(\ctl, krTestBus);
			var b = Synth(\testAudioControl, [ out: busB ], server, \addToTail).map(\ctl, krTestBus);
			[ a, b ]
		}, \ar, "AudioControl mapped to control bus works with resampling");

		// LagControl

		SynthDef(\testLagControl, { |out, resample|
			Resample(resample);
			ReplaceOut.kr(out, \ctl.kr([ 0.5, 1.5 ], 0.1, true));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLagControl), \kr, "LagControl with default value works with resampling");

		this.performABTest({ |busA, busB|
			var a = Synth(\testLagControl, [ out: busA, resample: testFactor ], server, \addToTail).map(\ctl, krTestBus);
			var b = Synth(\testLagControl, [ out: busB ], server, \addToTail).map(\ctl, krTestBus);
			[ a, b ]
		}, \kr, "LagControl mapped to control bus works with resampling");

		// TrigControl

		SynthDef(\testTrigControl, { |out, resample|
			Resample(resample);
			ReplaceOut.kr(out, \ctl.tr([1, 1]));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testTrigControl), \kr, "TrigControl with default value works with resampling");

		this.performABTest({ |busA, busB|
			var a, b;
			trigBusA.set(1, 1);
			trigBusB.set(1, 1);
			a = Synth(\testTrigControl, [ out: busA, resample: testFactor ], server, \addToTail).map(\ctl, trigBusA);
			b = Synth(\testTrigControl, [ out: busB ], server, \addToTail).map(\ctl, trigBusB);
			[ a, b ]
		}, \kr, "TrigControl mapped to control bus works with resampling");

		// LocalIn / LocalOut

		SynthDef(\testLocalBusAr, { |out, resample|
			var sig = SinOsc.ar([220, 440]);
			Resample(resample);
			// make sure that summing works
			LocalOut.ar(sig);
			LocalOut.ar(sig);
			Out.ar(out, LocalIn.ar(2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLocalBusAr), \ar, "LocalIn.ar and LocalOut.ar work with reblocking");

		SynthDef(\testLocalBusKr, { |out, resample|
			var sig = SinOsc.kr([220, 440]);
			Resample(resample);
			// make sure that summing works
			LocalOut.kr(sig);
			LocalOut.kr(sig);
			Out.kr(out, LocalIn.kr(2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLocalBusKr), \kr, "LocalIn.kr and LocalOut.kr work with reblocking");
	}
}
