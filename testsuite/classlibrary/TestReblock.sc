TestReblockBase : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
		server.options.blockSize = 64;
		server.options.sampleRate = 48000;
		this.bootServer(server);
	}

	tearDown {
		server.quitSync;
		server.remove;
	}

	// 'func' takes a bus and should produce an Array of two Synths (A and B).
	// A and B should write their output to the first resp. second channel of the bus.
	// A third Synth reads both channels and records the difference into a Buffer.
	// Finally, we read the content of the Buffer and check that the output of A and B match.
	performABTest { arg func, rate, message, verbose=false;
		var success, cond = Condition();
		var bus, buffer, testSig, compName, synths, comp;

		if (rate == \ar) {
			bus = Bus.audio(server, 4);
			buffer = Buffer.alloc(server, 1024, 4);
			SynthDef(\abTest, { |busA, busB, buffer|
				var a = In.ar(busA, 2);
				var b = In.ar(busB, 2);
				RecordBuf.ar(a ++ b, buffer, loop: 0.0, doneAction: Done.freeSelf);
			}).add;
		} {
			if (rate != \kr) { Error("bad rate argument").throw };
			bus = Bus.control(server, 4);
			buffer = Buffer.alloc(server, 16, 4);
			SynthDef(\abTest, { |busA, busB, buffer|
				var a = In.kr(busA, 2);
				var b = In.kr(busB, 2);
				RecordBuf.kr(a ++ b, buffer, loop: 0.0, doneAction: Done.freeSelf);
			}).add;
		};

		server.sync;

		server.bind {
			synths = func.value(bus.index, bus.index + 2);
			comp = Synth(\abTest, [ busA: bus.index, busB: bus.index + 2, buffer: buffer ], server, \addToTail);
			if (verbose) {
				server.queryAllNodes;
			}
		};

		cond.test = false;
		OSCFunc({
			cond.test_(true).signal
		}, '/n_end', nil, nil, [ comp.nodeID ]).oneShot;
		cond.wait;

		synths.do(_.free);

		cond.test = false;
		buffer.loadToFloatArray(action: { |data|
			success = true;
			data.clump(4).do { |array, i|
				var a0, a1, b0, b1;
				#a0, a1, b0, b1 = array;
				if (verbose and: { i < 128 }) {
					"#%\ta0: %, b0: %\n\ta1: %, b1: %".format(i, a0, b0, a1, b1).postln;
				};
				if (a0.equalWithPrecision(b0).not or: { a1.equalWithPrecision(b1).not }) { success = false };
			};
			cond.test_(true).signal;
		});
		cond.wait;

		this.assert(success, message);
	}
}

TestReblock : TestReblockBase {
	test_reblockWithConstantBlockSizeWorks {
		var bus = Bus.control(server);
		var blockSizes = [ 0, 1, 2, 4, 8, 16, 32, 64 ];
		var results = Array.fill(blockSizes.size, nil);
		var success;
		blockSizes.do { |blockSize, index|
			var cond = Condition(), synth;
			SynthDef(\testReblockConstant, { |out|
				Reblock(blockSize);
				Out.kr(out, BlockSize.ir);
			}).add;
			server.sync;
			synth = Synth(\testReblockConstant, [ out: bus ]);
			server.sync; // let it compute at least one block
			bus.get { |f|
				results[index] = f;
				cond.test_(true).signal;
			};
			cond.wait;
			synth.free;
		};
		this.assert(results[0] == server.options.blockSize, "Reblock with argument 0 matches Server block size");
		success = [ blockSizes[1..], results[1..] ].flop.every { |pair| pair[0] == pair[1] };
		this.assert(success, "Reblock with constant argument matches BlockSize output");
	}

	test_reblockWithBadInputThrows {
		this.assertException({
			SynthDef(\test, { Reblock(-1) });
		}, Error, "Reblock with negative block size throws exception");

		this.assertException({
			SynthDef(\test, { Reblock(3) });
		}, Error, "Reblock with non-power-of-two block size throws exception");

		this.assertException({
			SynthDef(\test, { |blockSize| Reblock(blockSize + 1) })
		}, Error, "Reblock with Outproxy that is not an immediate control throws exception");

		this.assertException({
			SynthDef(\test, { Reblock(DC.ar(1)) })
		}, Error, "Reblock with UGen other than OutputProxy throws exception");
	}

	test_reblockWithControlBlockSizeWorks {
		var bus = Bus.control(server);
		var blockSizes = [ 0, 1, 2, 4, 8, 16, 32, 64 ];
		var results = Array.fill(blockSizes.size, nil);
		var success;

		SynthDef(\testReblockControl, { |out, blockSize|
			Reblock(blockSize);
			Out.kr(out, BlockSize.ir);
		}).add;
		server.sync;

		blockSizes.do { |blockSize, index|
			var cond = Condition();
			var synth = Synth(\testReblockControl, [ out: bus, blockSize: blockSize ]);
			server.sync; // let it compute at least one block
			bus.get { |f|
				results[index] = f;
				cond.test_(true).signal;
			};
			cond.wait;
			synth.free;
		};
		this.assert(results[0] == server.options.blockSize, "Reblock with control default argument matches Server block size");
		success = [ blockSizes[1..], results[1..] ].flop.every { |pair| pair[0] == pair[1] };
		this.assert(success, "Reblock with control argument matches BlockSize output");
	}

	test_reblockWithBadControlBlockSizeUsesDefault {
		var bus = Bus.control(server);
		var blockSizes = [ -1, 3, 7 ];
		var results = Array.fill(blockSizes.size, nil);
		var success;
		SynthDef(\testReblockControlFail, { |out, blockSize|
			Reblock(blockSize);
			Out.kr(out, BlockSize.ir);
		}).add;
		server.sync;
		blockSizes.do { |blockSize, index|
			var cond = Condition();
			var synth = Synth(\testReblockControlFail, [ out: bus, blockSize: blockSize ]);
			server.sync; // let it compute at least one block
			bus.get { |f|
				results[index] = f;
				cond.test_(true).signal;
			};
			cond.wait;
			synth.free;
		};
		success = results.every { |x| x == server.options.blockSize };
		this.assert(success, "Reblock with bad control argument falls back to Server block size");
	}

	test_reblockIgnoresDuplicates {
		var bus = Bus.control(server);
		var blockSizes = [ 1, 2, 4, 8 ];
		var cond = Condition(), value;
		var synth = {
			blockSizes.do { |x| Reblock(x) };
			Out.kr(bus, BlockSize.ir);
		}.play(server);
		server.sync; // let it compute at least one block
		bus.get { |f|
			value = f;
			cond.test_(true).signal;
		};
		cond.wait;
		synth.free;
		this.assert(value == blockSizes[0], "Duplicate Reblock instances are ignored with a warning");
	}

	test_IOUGensWithReblocking {
		var arTestSig, krTestSig, simpleAB;
		var trigBusA = Bus.control(server, 2);
		var trigBusB = Bus.control(server, 2);
		var arTestBus = Bus.audio(server, 2);
		var krTestBus = Bus.control(server, 2);
		var testBlockSize = 1;

		simpleAB = { |defName|
			{ |busA, busB|
				[
					Synth(defName, [ out: busA, blockSize: testBlockSize ], server, \addToTail),
					Synth(defName, [ out: busB ], server, \addToTail)
				]
			}
		};

		arTestSig = { Out.ar(arTestBus, SinOsc.ar([220, 440])) }.play(server);

		krTestSig = { Out.kr(krTestBus, SinOsc.kr([220, 440])) }.play(server);

		server.sync;

		//------------------ UGens ----------------------//

		// Out.ar/Out.kr

		SynthDef(\testOutAr, { |out, blockSize|
			var sig = [ Line.ar, SinOsc.ar ];
			Reblock(blockSize);
			// make sure that summing works
			Out.ar(out, sig);
			Out.ar(out, sig);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testOutAr), \ar, "Out.ar works with reblocking");

		SynthDef(\testOutKr, { |out, blockSize|
			var sig = [ Line.kr, SinOsc.kr ];
			Reblock(blockSize);
			// make sure that summing works
			Out.kr(out, sig);
			Out.kr(out, sig);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testOutKr), \kr, "Out.kr works with reblocking");

		// ReplaceOut.ar / ReplaceOut.kr

		SynthDef(\testReplaceOutAr, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.ar(out, [ Line.ar, SinOsc.ar ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testReplaceOutAr), \ar, "ReplaceOut.ar works with reblocking");

		SynthDef(\testReplaceOutKr, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, [ Line.kr, SinOsc.kr ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testReplaceOutKr), \kr, "ReplaceOut.kr works with reblocking");

		// OffsetOut.ar

		SynthDef(\testOffsetOutAr, { |out, blockSize|
			var sig = [ Line.ar, SinOsc.ar ];
			Reblock(blockSize);
			// make sure that summing works
			OffsetOut.ar(out, sig);
			OffsetOut.ar(out, sig);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testOffsetOutAr), \ar, "OffsetOut.ar works with reblocking");

		// XOut.ar / XOut.kr

		SynthDef(\testXOutAr, { |out, blockSize|
			Reblock(blockSize);
			XOut.ar(out, 0.5, [ Line.ar, SinOsc.ar ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testXOutAr), \ar, "XOut.ar works with reblocking");

		SynthDef(\testXOutKr, { |out, blockSize|
			Reblock(blockSize);
			XOut.kr(out, 0.5, [ Line.kr, SinOsc.kr ]);
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testXOutKr), \kr, "XOut.kr works with reblocking");

		// In.ar / In.kr

		SynthDef(\testInAr, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.ar(out, In.ar(arTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testInAr), \ar, "In.ar works with reblocking");

		SynthDef(\testInKr, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, In.kr(krTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testInKr), \kr, "In.kr works with reblocking");

		// FeedbackIn.ar

		SynthDef(\testInFeedbackAr, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.ar(out, InFeedback.ar(arTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testInFeedbackAr), \ar, "InFeedback.ar works with reblocking");

		// LagIn.kr

		SynthDef(\testLagInKr, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, LagIn.kr(krTestBus, 2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLagInKr), \kr, "LagIn.kr works with reblocking");

		// InTrig.kr

		SynthDef(\testInTrigKr, { |in, out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, InTrig.kr(in, 2));
		}).add;

		server.sync;

		this.performABTest({ |busA, busB|
			trigBusA.set(1, 1);
			trigBusB.set(1, 1);
			[
				Synth(\testInTrigKr, [ in: trigBusA, out: busA, blockSize: testBlockSize ], server, \addToTail),
				Synth(\testInTrigKr, [ in: trigBusB, out: busB ], server, \addToTail)
			]
		}, \kr, "InTrig.kr works with reblocking");

		// Control

		SynthDef(\testControl, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, \ctl.kr([ 0.5, 1.5 ]));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testControl), \kr, "Control with default value works with reblocking");

		this.performABTest({ |busA, busB|
			var a = Synth(\testControl, [ out: busA, blockSize: testBlockSize ], server, \addToTail).map(\ctl, krTestBus);
			var b = Synth(\testControl, [ out: busB ], server, \addToTail).map(\ctl, krTestBus);
			[ a, b ]
		}, \kr, "Control mapped to control bus works with reblocking");

		// AudioControl

		SynthDef(\testAudioControl, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.ar(out, \ctl.ar([ 0.5, 1.5 ]));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testAudioControl), \ar, "AudioControl with default value works with reblocking");

		this.performABTest({ |busA, busB|
			var a = Synth(\testAudioControl, [ out: busA, blockSize: testBlockSize ], server, \addToTail).map(\ctl, arTestBus);
			var b = Synth(\testAudioControl, [ out: busB ], server, \addToTail).map(\ctl, arTestBus);
			[ a, b ]
		}, \ar, "AudioControl mapped to audio bus works with reblocking");

		this.performABTest({ |busA, busB|
			var a = Synth(\testAudioControl, [ out: busA, blockSize: testBlockSize ], server, \addToTail).map(\ctl, krTestBus);
			var b = Synth(\testAudioControl, [ out: busB ], server, \addToTail).map(\ctl, krTestBus);
			[ a, b ]
		}, \ar, "AudioControl mapped to control bus works with reblocking");

		// LagControl

		SynthDef(\testLagControl, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, \ctl.kr([ 0.5, 1.5 ], 0.1, true));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLagControl), \kr, "LagControl with default value works with reblocking");

		this.performABTest({ |busA, busB|
			var a = Synth(\testLagControl, [ out: busA, blockSize: testBlockSize ], server, \addToTail).map(\ctl, krTestBus);
			var b = Synth(\testLagControl, [ out: busB ], server, \addToTail).map(\ctl, krTestBus);
			[ a, b ]
		}, \kr, "LagControl mapped to control bus works with reblocking");

		// TrigControl

		SynthDef(\testTrigControl, { |out, blockSize|
			Reblock(blockSize);
			ReplaceOut.kr(out, \ctl.tr([1, 1]));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testTrigControl), \kr, "TrigControl with default value works with reblocking");

		this.performABTest({ |busA, busB|
			var a, b;
			trigBusA.set(1, 1);
			trigBusB.set(1, 1);
			a = Synth(\testTrigControl, [ out: busA, blockSize: testBlockSize ], server, \addToTail).map(\ctl, trigBusA);
			b = Synth(\testTrigControl, [ out: busB ], server, \addToTail).map(\ctl, trigBusB);
			[ a, b ]
		}, \kr, "TrigControl mapped to control bus works with reblocking");

		// LocalIn / LocalOut

		SynthDef(\testLocalBusAr, { |out, blockSize|
			var sig = SinOsc.ar([220, 440]);
			Reblock(blockSize);
			// make sure that summing works
			LocalOut.ar(sig);
			LocalOut.ar(sig);
			Out.ar(out, LocalIn.ar(2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLocalBusAr), \ar, "LocalIn.ar and LocalOut.ar work with reblocking");

		SynthDef(\testLocalBusKr, { |out, blockSize|
			var sig = SinOsc.kr([220, 440]);
			Reblock(blockSize);
			// make sure that summing works
			LocalOut.kr(sig);
			LocalOut.kr(sig);
			Out.kr(out, LocalIn.kr(2));
		}).add;

		server.sync;

		this.performABTest(simpleAB.(\testLocalBusKr), \kr, "LocalIn.kr and LocalOut.kr work with reblocking");
	}
}
