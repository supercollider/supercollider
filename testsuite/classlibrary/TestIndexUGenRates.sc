TestIndexUGenRates : UnitTest {

	var server, numFrames, floats;

	setUp {

		server = Server.default;
		this.bootServer(server);
	}

	tearDown {

		Buffer.freeAll;
		server.quit;
	}

	addSynthDefs {

		SynthDef(\indexArPhaseAr, { |buf|
			var phase = Phasor.ar(0, 1, 0, numFrames);
			BufWr.ar(
				Index.ar(
					floats,
					phase
				),
				buf,
				phase,
				loop:0
			);
			Line.kr(0, 1, 0.01, doneAction: Done.freeSelf);
			Out.ar(0, Silent.ar);
		}).add;

		SynthDef(\indexKrPhaseAr, { |buf|
			var phase = Phasor.ar(0, 1, 0, numFrames);
			BufWr.ar(
				K2A.ar(
					Index.kr(
						floats,
						phase
					)
				),
				buf,
				phase,
				loop:0
			);
			Line.kr(0, 1, 0.01, doneAction: Done.freeSelf);
			Out.ar(0, Silent.ar);
		}).add;

		SynthDef(\indexArPhaseKr, { |buf|
			var phase = Phasor.kr(0, 1, 0, 1);
			BufWr.ar(
				Index.ar(
					floats,
					K2A.ar(phase)
				),
				buf,
				Phasor.ar(0, 1, 0, numFrames),
				loop:0
			);
			Line.kr(0, 1, 0.01, doneAction: Done.freeSelf);
			Out.ar(0, Silent.ar);
		}).add;

		SynthDef(\indexKrPhaseKr, { |buf|
			var phase = Phasor.kr(0, 1, 0, 1);
			BufWr.ar(
				K2A.ar(
					Index.kr(
						floats,
						phase
					)
				),
				buf,
				Phasor.ar(0, 1, 0, numFrames),
				loop:0
			);
			Line.kr(0, 1, 0.01, doneAction: Done.freeSelf);
			Out.ar(0, Silent.ar);
		}).add;

	}

	test_indexRates {

		var index, writeBuf, value, synth;

		numFrames = server.options.blockSize;
		index = (numFrames/2).asInteger;
		floats = Array.fill(numFrames, { |i| (numFrames-i)/numFrames });
		floats = Buffer.loadCollection(server, floats);
		writeBuf = Buffer.alloc(server, numFrames);

		this.addSynthDefs;
		server.sync;

		synth = Synth(\indexArPhaseAr, [\buf, writeBuf]);
		synth.waitForFree;
		writeBuf.get(index, { |i| value = i});
		server.sync;
		this.assertFloatEquals(value, 0.5, "Index.ar with audio rate input should output 0.5");
		writeBuf.zero;
		server.sync;

		synth = Synth(\indexKrPhaseAr, [\buf, writeBuf]);
		synth.waitForFree;
		writeBuf.get(index, { |i| value = i});
		server.sync;
		this.assertFloatEquals(value, 1.0, "Index.kr with audio rate input should output 1.0");
		writeBuf.zero;
		server.sync;

		synth = Synth(\indexArPhaseKr, [\buf, writeBuf]);
		synth.waitForFree;
		writeBuf.get(index, { |i| value = i});
		server.sync;
		this.assertFloatEquals(value, 1.0, "Index.ar with control rate input should output 1.0");
		writeBuf.zero;
		server.sync;

		synth = Synth(\indexKrPhaseKr, [\buf, writeBuf]);
		synth.waitForFree;
		writeBuf.get(index, { |i| value = i});
		server.sync;
		this.assertFloatEquals(value, 1.0, "Index.kr with control rate input should output 1.0");

	}

}
