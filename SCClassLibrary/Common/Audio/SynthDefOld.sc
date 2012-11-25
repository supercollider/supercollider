SynthDefOld : SynthDef {

	*new { arg name, ugenGraphFunc, rates, prependArgs, variants, metadata;
		^super.new.name_(name.asSymbol).variants_(variants).metadata_(metadata).children_(Array.new(64))
			.build(ugenGraphFunc, rates, prependArgs)
	}

	writeDefFile { arg dir, overwrite(true);
		if((metadata.tryPerform(\at, \shouldNotSend) ? false).not) {
			super.writeDefFileOld(name, dir, overwrite);
		} {
			// actual error, not just warning as in .send and .load,
			// because you might try to write the file somewhere other than
			// the default location - could be fatal later, so crash now
			MethodError("This SynthDef (%) was reconstructed from a .scsyndef file. It does not contain all the required structure to write back to disk. File was not written."
				.format(name), this).throw
		}
	}

	writeDef { arg file;
		// This describes the file format for the synthdef files.
		var allControlNamesTemp, allControlNamesMap;

		file.putPascalString(name.asString);

		this.writeConstants(file);

		//controls have been added by the Control UGens
		file.putInt16(controls.size);
		controls.do { | item |
			file.putFloat(item);
		};

		allControlNamesTemp = allControlNames.reject { |cn| cn.rate == \noncontrol };
		file.putInt16(allControlNamesTemp.size);
		allControlNamesTemp.do { | item |
			if (item.name.notNil) {
				file.putPascalString(item.name.asString);
				file.putInt16(item.index);
			};
		};

		file.putInt16(children.size);
		children.do { | item |
			item.writeDefOld(file);
		};

		file.putInt16(variants.size);
		if (variants.size > 0) {
			allControlNamesMap = ();
			allControlNamesTemp.do { |cn|
				allControlNamesMap[cn.name] = cn;
			};
			variants.keysValuesDo {|varname, pairs|
				var varcontrols;

				varname = name ++ "." ++ varname;
				if (varname.size > 32) {
					Post << "variant '" << varname << "' name too long.\n";
					^nil
				};
				varcontrols = controls.copy;
				pairs.pairsDo { |cname, values|
					var cn, index;
					cn = allControlNamesMap[cname];
					if (cn.notNil) {
						values = values.asArray;
						if (values.size > cn.defaultValue.asArray.size) {
							postf("variant: '%' control: '%' size mismatch.\n",
								varname, cname);
							^nil
						}{
							index = cn.index;
							values.do {|val, i|
								varcontrols[index + i] = val;
							}
						}
					}{
						postf("variant: '%' control: '%' not found.\n",
								varname, cname);
						^nil
					}
				};
				file.putPascalString(varname);
				varcontrols.do { | item |
					file.putFloat(item);
				};
			};
		}
	}

	writeConstants { arg file;
		var array = FloatArray.newClear(constants.size);
		constants.keysValuesDo { arg value, index;
			array[index] = value;
		};

		file.putInt16(constants.size);
		array.do { | item |
			file.putFloat(item)
		};
	}

	asBytes {
		var stream = CollStream.on(Int8Array.new(256));
		this.asArray.writeDefOld(stream);
		^stream.collection;
	}

}

+ Collection {

	writeDefOld { | file |
		file.putString("SCgf");
		file.putInt32(1); // file version
		file.putInt16(this.size); // number of defs in file.

		this.do { | item | item.writeDef(file); }
	}

	writeInputSpecOld { | file, synthDef |
		this.do { | item | item.writeInputSpecOld(file, synthDef) };
	}
}

+ UGen {

	writeDefOld { arg file;
		//[\WR, this.class.name, rate, this.numInputs, this.numOutputs].postln;
		file.putPascalString(this.name);
		file.putInt8(this.rateNumber);
		file.putInt16(this.numInputs);
		file.putInt16(this.numOutputs);
		file.putInt16(this.specialIndex);
		// write wire spec indices.
		inputs.do({ arg input;
			input.writeInputSpecOld(file, synthDef);
		});
		this.writeOutputSpecs(file);
		//[this.class.name, file.length].postln;
	}

	writeInputSpecOld { arg file, synthDef;
		file.putInt16(synthIndex);
		file.putInt16(this.outputIndex);
	}
}

+ SimpleNumber {

	writeInputSpecOld { arg file, synth;
		var constIndex = synth.constants.at(this.asFloat);
		if (constIndex.isNil) {
			Error("SimpleNumber-writeInputSpec constant not found: " ++ this.asFloat).throw;		};
		//[\inpspc, this.class.name, constIndex, this].postln;
		file.putInt16(-1);
		file.putInt16(constIndex);
	}
}

+ Object {

	writeDefFileOld { arg name, dir, overwrite = (true);

		StartUp.defer { // make sure the synth defs are written to the right path
			var file;
			dir = dir ? SynthDef.synthDefDir;
			if (name.isNil) { Error("missing SynthDef file name").throw } {
				name = dir +/+ name ++ ".scsyndef";
				if(overwrite or: { pathMatch(name).isEmpty })
					{
					file = File(name, "w");
					protect {
						AbstractMDPlugin.clearMetadata(name);
						this.asArray.writeDefOld(file);
					}{
						file.close;
					}
				}
			}
		}

	}
}