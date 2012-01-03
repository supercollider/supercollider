SynthDef2 : SynthDef {
	
	*new { arg name, ugenGraphFunc, rates, prependArgs, variants, metadata;
		^this.prNew(name).variants_(variants).metadata_(metadata).children_(Array.new(64))
			.build(ugenGraphFunc, rates, prependArgs)
	}
	
	writeDefFile { arg dir, overwrite;
		if((metadata.tryPerform(\at, \shouldNotSend) ? false).not) {
			super.writeDefFile2(name, dir, overwrite);
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

		file.putPascalString(name);

		this.writeConstants(file);

		//controls have been added by the Control UGens
		file.putInt32(controls.size);
		controls.do { | item |
			file.putFloat(item);
		};

		allControlNamesTemp = allControlNames.reject { |cn| cn.rate == \noncontrol };
		file.putInt32(allControlNamesTemp.size);
		allControlNamesTemp.do { | item |
			if (item.name.notNil) {
				file.putPascalString(item.name.asString);
				file.putInt32(item.index);
			};
		};

		file.putInt32(children.size);
		children.do { | item |
			item.writeDef2(file);
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
		};
	}
	
	writeConstants { arg file;
		var array = FloatArray.newClear(constants.size);
		constants.keysValuesDo { arg value, index;
			array[index] = value;
		};

		file.putInt32(constants.size);
		array.do { | item |
			file.putFloat(item)
		};
	}
	
	asBytes {
		var stream = CollStream.on(Int8Array.new(256));
		this.asArray.writeDef2(stream);
		^stream.collection;
	}
}

+ Collection {
	
	writeDef2 { | file |
		file.putString("SCgf");
		file.putInt32(2); // file version
		file.putInt16(this.size); // number of defs in file.

		this.do { | item | item.writeDef(file); }
	}
	
	writeInputSpec2 { | file, synthDef |
		this.do { | item | item.writeInputSpec2(file, synthDef) };
	}
	
}

+ UGen {
	
	writeDef2 { arg file;
		file.putPascalString(this.name);
		file.putInt8(this.rateNumber);
		file.putInt32(this.numInputs);
		file.putInt32(this.numOutputs);
		file.putInt16(this.specialIndex);
		// write wire spec indices.
		inputs.do({ arg input;
			input.writeInputSpec2(file, synthDef);
		});
		this.writeOutputSpecs(file);
	}
	
	writeInputSpec2 { arg file, synthDef;
		file.putInt32(synthIndex);
		file.putInt32(this.outputIndex);
	}
	
}

+ SimpleNumber {
	
	writeInputSpec2 { arg file, synth;
		var constIndex = synth.constants.at(this.asFloat);
		if (constIndex.isNil) {
			Error("SimpleNumber-writeInputSpec constant not found: " ++ this.asFloat).throw;		};
		file.putInt32(-1);
		file.putInt32(constIndex);
	}
}

+ Object {
	
	writeDefFile2 { arg name, dir, overwrite = (true);

		StartUp.defer { // make sure the synth defs are written to the right path
			var file;
			dir = dir ? SynthDef.synthDefDir;
			if (name.isNil) { error("missing SynthDef file name") } {
				name = dir +/+ name ++ ".scsyndef";
				if(overwrite or: { pathMatch(name).isEmpty })
					{
					file = File(name, "w");
					protect {
						AbstractMDPlugin.clearMetadata(name);
						this.asArray.writeDef2(file);
					}{
						file.close;
					}
				}
			}
		}

	}
}