SynthDef2 : SynthDef {
	
	writeDef { arg file;
		// This describes the file format for the synthdef files.
		var allControlNamesTemp, allControlNamesMap;

		file.putPascalString(name);

		this.writeConstants(file);

		//controls have been added by the Control UGens
		file.putInt32(controls.size);
		"numControls: %\n".postf(controls.size);
		controls.do { | item |
			file.putFloat(item);
		};

		allControlNamesTemp = allControlNames.reject { |cn| cn.rate == \noncontrol };
		file.putInt32(allControlNamesTemp.size);
		"numParamsSpecs: %\n".postf(allControlNamesTemp.size);
		allControlNamesTemp.do { | item |
			if (item.name.notNil) {
				file.putPascalString(item.name.asString);
				file.putInt32(item.index);
			};
		};

		file.putInt32(children.size);
		"numUGenSpecs: %\n".postf(children.size);
		children.do { | item |
			item.writeDef2(file);
		};

		file.putInt16(variants.size);
		"numVariants: %\n".postf(variants.size);
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
		"/////// End lang side //////// ".postln; 
	}
	
	writeConstants { arg file;
		var array = FloatArray.newClear(constants.size);
		constants.keysValuesDo { arg value, index;
			array[index] = value;
		};

		file.putInt32(constants.size);
		"numConstants: %\n".postf(constants.size);
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
		//[\WR, this.class.name, rate, this.numInputs, this.numOutputs].postln;
		"//UGen Spec: %\n".postf(this.name);
		file.putPascalString(this.name);
		file.putInt8(this.rateNumber);
		"rate: %\n".postf(this.rateNumber);
		file.putInt32(this.numInputs);
		"numInputs: %\n".postf(this.numInputs);
		file.putInt32(this.numOutputs);
		"numOutputs: %\n".postf(this.numOutputs);
		file.putInt16(this.specialIndex);
		"specialInd: %\n".postf(this.specialIndex);
		// write wire spec indices.
		inputs.do({ arg input;
			input.writeInputSpec2(file, synthDef);
		});
		this.writeOutputSpecs(file);
		//[this.class.name, file.length].postln;
	}
	
	writeInputSpec2 { arg file, synthDef;
		file.putInt32(synthIndex);
		"fromUnitIndex: %\n".postf(synthIndex);
		file.putInt32(this.outputIndex);
		"fromOutputIndex: %\n".postf(this.outputIndex);
	}
	
}

+ SimpleNumber {
	
	writeInputSpec2 { arg file, synth;
		var constIndex = synth.constants.at(this.asFloat);
		if (constIndex.isNil) {
			Error("SimpleNumber-writeInputSpec constant not found: " ++ this.asFloat).throw;		};
		//[\inpspc, this.class.name, constIndex, this].postln;
		file.putInt32(-1);
		file.putInt32(constIndex);
	}
}