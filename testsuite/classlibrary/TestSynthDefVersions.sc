TestSynthDefVersions : UnitTest {
	compareSynthDescs { |a, b|
		(a.name + b.name);
		^(a.name == b.name) and: {
			a.inputs.every { |input, i|
				input.compareObject(b.inputs[i])
			};
		} and: {
			a.outputs.every { |output, i|
				output.compareObject(b.outputs[i])
			};
		} and: {
			a.controls.every { |control, i|
				control.compareObject(b.controls[i])
			};
		}
	}

	readAndWriteSynthDefs { |version|
		var defArray, stream, descArray;
		var libName = thisMethod.name.asSymbol;
		var lib = SynthDescLib(libName);
		var defClass = if (version > 1) { SynthDef } { SynthDefOld };
		var graphFunc = { |in, out, freq=440, amp=1.0| Out.ar(out, (SinOsc.ar(freq) * In.ar(in)) * amp) };
		var variants = (a: [ freq: 220 ], b: [ freq: 100 ], c: [ freq: 600 ]);

		// test with multiple SynthDefs per message. Also include variants.
		defArray = [
			defClass.new(("test_v"++version++"_1").asSymbol, graphFunc, variants: variants),
			defClass.new(("test_v"++version++"_2").asSymbol, graphFunc, variants: variants)
		];

		descArray = defArray.collect { |def| def.asSynthDesc(libName) };

		this.assertNoException({
			stream = CollStream.on(Int8Array.new(256));
			if (version > 1) {
				defArray.writeDef(stream, version);
			} {
				defArray.writeDefOld(stream);
			};
			stream.pos = 0; // rewind

			this.assertNoException({
				var set = lib.readStream(stream);
				var array = set.asArray.sort { |a, b| a.name < b.name };
				var isEqual = array.every { |desc, i|
					this.compareSynthDescs(desc, descArray[i])
				};

				this.assert(isEqual, "reconstructed SynthDesc (version %) should be equal to original SynthDesc".format(version));

				^array;
			}, "SynthDef version % can be read from stream".format(version));

		}, "SynthDef version % can be written to stream".format(version));

		^nil;
	}

	test_readAndWriteSynthDefs {
		var arrays = (1..3).collect { |version|
			this.readAndWriteSynthDefs(version)
		};
		var isEqual = arrays.flop.every { |descs|
			descs.do(_.name = "dummy"); // bash names
			this.compareSynthDescs(descs[0], descs[1]) and: {
				this.compareSynthDescs(descs[1], descs[2])
			}
		};
		this.assert(isEqual, "(reconstructed) SynthDescs of all versions should compare equal");
	}

	readCorruptedSynthDefs { |version|
		var defArray, data, stream, libName = thisMethod.name.asSymbol;
		var lib = SynthDescLib(libName);
		var defClass = if (version > 1) { SynthDef } { SynthDefOld };
		var graphFunc = { |in, out, freq=440, amp=1.0| Out.ar(out, (SinOsc.ar(freq) * In.ar(in)) * amp) };
		var variants = (a: [ freq: 220 ], b: [ freq: 100 ], c: [ freq: 600 ]);
		var didReadStream;
		// test with multiple SynthDefs per message. Also include variants.
		defArray = [
			defClass.new(("test_v"++version++"_1").asSymbol, graphFunc, variants: variants),
			defClass.new(("test_v"++version++"_2").asSymbol, graphFunc, variants: variants)
		];

		stream = CollStream.on(Int8Array(256));
		if (version > 1) {
			defArray.writeDef(stream, version);
		} {
			defArray.writeDefOld(stream);
		};
		data = stream.collection.copy;
		// remove the last 4 bytes
		data = data.extend(data.size - 4);
		// now try to read the corrupted SynthDef.
		// on every iteration, slice off half of the data.
		didReadStream = false;
		while { data.size > 1 } {
			stream = CollStream(data);
			try {
				SynthDescLib.global.readStream(stream);
				didReadStream = true;
			};
			data = data.extend(data.size div: 2);
		};
		this.assert(didReadStream.not, "passing corrupted SynthDef v% data to SynthDescLib-readStream must throw an Error".format(version));
		"done".postln;
	}

	test_readCorruptedSynthDefs {
		(1..3).do { |version|
			this.readCorruptedSynthDefs(version)
		};
	}
}
