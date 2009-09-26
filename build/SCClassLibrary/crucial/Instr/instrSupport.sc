
+ Function {
	iplay { arg ...args;
		Patch(this,args).play
	}
}

+ Instr {
	asInstr {}
}
+ InterfaceDef {
	asInterfaceDef {}
}
+ SequenceableCollection {
	asInstr {
		^Instr.at(this)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
}

+ Symbol {
	asInstr {
		^Instr.at(this)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
}

+ String {
	asInstr {
		^Instr.at(this)
	}
	asInterfaceDef {
		^InterfaceDef.at(this)
	}
	ar { arg ... args;
		^this.asInstr.ar(*args)
	}
}

+ Class { // eg. SinOsc the class becomes a UGenInstr
	// any class that responds to .ar or .kr
	asInstr {
		^UGenInstr(this)
	}
}


+ Function {
	asInstr {
		^Instr("f" ++ this.hash,this)
	}
	asInterfaceDef {
		^InterfaceDef("f" ++ this.hash,this)
	}
}

+ SynthDef {
	/*
	// doesnt hang onto its function
	*def { arg name, ugenGraphFunc, rates, prependArgs, variants;
		var new;
		new = this.new(name, ugenGraphFunc, rates, prependArgs, variants);
		Instr.put(new);
		^new
	}
	asInstr {
		^Instr(name, ugenGraphFunc)
	} */
	*at { arg name;
		var search;
		search = Instr.objectAt(name);
		if(search.isNil,{ ^nil });
		^search.asSynthDef
	}
}


/**  asSpec **/

+ Nil {
	asSpec {
		^ControlSpec.new;
	}
}

+ Spec {
	rate { ^\noncontrol }

	*findKeyForSpec { arg spec;
		var matching,exact;
		// find the exact spec object
		exact = Spec.specs.findKeyForValue(spec);
		if(exact.notNil,{ ^exact });

		matching = [];
		Spec.specs.keysValuesDo({ |k,v|
			if(v == spec,{
				matching = matching.add(k);
			})
		});
		// if there is precisely one, then it is most probably what you are looking for
		if(matching.size == 1,{ ^matching[0] });
		// many matched
		^nil
	}
}

+ ControlSpec 	{

	defaultControl { arg val;
		^KrNumberEditor.new(this.constrain(val ? this.default),this);
	}

	rate { ^\control }

	optimalRange { arg stdDev=0.3;
		var linMinVal,linMaxVal,linDefault,toSpec;
		toSpec = this.copy;
		linDefault = this.unmap(this.default);
		linMinVal = (linDefault - stdDev).max(0.0);
		linMaxVal = (linDefault +  stdDev).min(1.0);
		toSpec.minval = this.map(linMinVal);
		toSpec.maxval = this.map(linMaxVal);
		^toSpec
	}
}

/** spec **/


+ ListPattern {

	spec {
		var firstItemClass,firstItemSpec;
		if(list.first.respondsTo(\spec),{
			firstItemSpec = list.first.spec;
			if(list.every({ |item| item.tryPerform(\spec) == firstItemSpec }),{
				// an array of Pbinds etc.
				if(firstItemSpec.isKindOf(EventStreamSpec),{ ^firstItemSpec });
				// a stream of my items
				^StreamSpec(firstItemSpec)
			});
		});
		// something with spec unknowable
		firstItemClass = list.first.class;
		if(list.every({ |item| item.class == firstItemClass }),{
			// this is dodgy
			if(list.first.isNumber,{
				if(firstItemClass.isKindOf(Integer),{
					^StreamSpec(StaticIntegerSpec(list.minItem,list.maxItem))
				});
				^StreamSpec(StaticSpec(list.minItem,list.maxItem))
			});
		});
		("unable to determine spec of" + this.asCompileString).warn;
		^nil
	}
}

+ Pfsm {
	spec {
		^thisMethod.notYetImplemented
	}
}

+ Pbind {
	spec {
		^EventStreamSpec.new
	}
}
+ Pmono {
	spec {
		^EventStreamSpec.new
	}
}
/*
+ Pseries {
	spec {
		StreamSpec( integer, or float, unknown range )
	}
}
*/



/** addToDefName **/



/** building the Instr def name **/
//+ SynthDef {
//	longName { ^name }
//}

+ Editor {
	addToDefName { arg stream;
		^value.addToDefName(stream)
	}
}
+ KrNumberEditor {
	addToDefName { arg stream;
		^\kr
	}
}
+ IrNumberEditor {
	addToDefName { arg stream;
		^\ir
	}
}
+ SimpleNumber {
	addToDefName { arg stream;
		stream << this.asFileSafeString;
		^\obj
	}
}
+ BufferProxy {
	spec {
		var def,me;
		def = \bufferProxy.asSpec;
		me = BufferProxySpec(size,numChannels,sampleRate);
		if(me == def,{ ^def });
		^me
	}
}
+ Sample {
	addToDefName { arg stream;
		// beatsizek
		//var sum;
		//sum = numChannels - 1; // assumes no quad samples
		//if(beatsizek.notNil,{ sum = sum + 2 });
		//stream << sum;
		^2
	}
	spec {
		//	^SampleSpec.new
		^\sample.asSpec ?? {SampleSpec.new}
	}
}
+ AbstractPlayer {
	addToDefName {
		^0
	}
}

// the big one, espec. Env
+ Object {
	addToDefName { arg stream;
		stream << this.asCompileString;//.hash.asFileSafeString;
		^2
	}
	didSpawn {}
}

+ UGen {
	onTrig { |func,value=0.0|
		if(this.rate != \control,{
			Error("UGen:onTrig only permissable with a control rate signal").throw;
		});
		^InstrSynthDef.buildSynthDef.onTrig(this,func,value)
	}
}



